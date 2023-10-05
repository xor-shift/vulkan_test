#include <vxl/vk/stuff.hpp>

#include <chrono>

namespace vxl::vk {

static auto get_desired_device_layers() -> std::span<const std::pair<const char*, usize>> {
    // return (std::pair<const char*, usize> const[]){};
    return {};
}

static auto get_desired_device_extensions() -> std::span<const std::pair<const char*, usize>> {
    return (std::pair<const char*, usize> const[]){
      {"VK_KHR_swapchain", 0},  //
    };
}

static constexpr auto get_vulkan_library_names() -> std::span<const char* const> {
#ifdef __linux__
    return (const char* const[]){
      "libvulkan.so.1",  //
      "libvulkan.so"     //
    };
#elifdef __APPLE__
    return (const char* const[]){
      "libvulkan.1.dylib",
      "libvulkan.dylib",
    };
#elifdef _WIN32
    return (const char* const[]){
      "vulkan-1.dll",
    };
#else
    return {};
#endif
}

auto vulkan_stuff::make() -> std::expected<vulkan_stuff, error> {
    auto&& ret = vulkan_stuff{};

    ret.m_window_size = {640, 360};

    ret.m_dyna_loader = TRYX(dynamic_loader::make(get_vulkan_library_names()).transform_error([](auto err) { return error::make(std::string{err}); }));
    ret.m_vk_fns = std::make_shared<vulkan_functions>();

    ret.m_vk_fns->init(ret.m_dyna_loader.get_dl_symbol<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"));

    ret.m_vk_instance = std::make_shared<instance_things>(ret.m_app_info, ret.m_vk_fns);
    TRYX(ret.m_vk_instance->init());

    ret.m_vk_debug_messenger = std::make_shared<debug_messenger_things>(ret.m_vk_instance, ret.m_vk_fns);
    TRYX(ret.m_vk_debug_messenger->init());

    ret.m_surface_things = std::make_shared<surface_things>(ret.m_vk_instance, ret.m_vk_fns);
    TRYX(ret.m_surface_things->init(ret.m_window_size));

    ret.m_device_things = std::make_shared<device_things>(ret.m_vk_instance, ret.m_vk_fns);
    TRYX(ret.m_device_things->init(
      (const VkPhysicalDeviceType[]){
        VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
        VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU,
        VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
        VK_PHYSICAL_DEVICE_TYPE_CPU,
        VK_PHYSICAL_DEVICE_TYPE_OTHER,
      },
      device_constraints{
        .m_desired_layers = get_desired_device_layers(),
        .m_desired_extensions = get_desired_device_extensions(),
        .m_desired_queues = (const queue_constraints[]){queue_constraints{
          .m_present_to = ret.m_surface_things->surface(),
          .m_features = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT,
        }},
        .m_min_vk_version = VK_VERSION_1_2,
      }
    ));

    // ret.m_vertex_shader = TRYX(ret.read_shader("triangle.vert.spv"));
    // ret.m_fragment_shader = TRYX(ret.read_shader("triangle.frag.spv"));

    TRYX(ret.reinit_vk_swapchain(ret.m_window_size));

    return ret;
}

vulkan_stuff::vulkan_stuff(vulkan_stuff&& other) noexcept
    : m_dyna_loader(std::move(other.m_dyna_loader))
    , m_vk_fns(std::move(other.m_vk_fns))
    , m_vk_instance(std::move(other.m_vk_instance))
    , m_vk_debug_messenger(std::move(other.m_vk_debug_messenger))
    , m_window_size(other.m_window_size)
    , m_surface_things(std::move(other.m_surface_things))
    , m_device_things(std::move(other.m_device_things))
    , m_swapchain_things(std::move(other.m_swapchain_things))
    , m_vk_framebuffers(std::move(other.m_vk_framebuffers)) {
#pragma push_macro("MOVE")
#define MOVE(_name)            \
    this->_name = other._name; \
    other._name = {}

    MOVE(m_vk_render_pass);

#pragma pop_macro("MOVE")
}

auto vulkan_stuff::frame(VkFence render_fence, VkSemaphore present_semaphore, VkSemaphore render_semaphore, usize frame_number) -> std::expected<bool, error> {
    static constexpr auto a_second = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();

    bool running = true;
    static constexpr auto color_period = 120.f;

    for (SDL_Event event; SDL_PollEvent(&event) != 0;) {
        switch (event.type) {
            case SDL_EVENT_QUIT: running = false; break;
            case SDL_EVENT_WINDOW_RESIZED:
                TRYX(error::from_vk(m_vk_fns->device_wait_idle(*m_device_things), "while waiting for device idle"));

                // TRYX(destroy_vk_swapchain());
                m_window_size.width = static_cast<u32>(event.window.data1);
                m_window_size.height = static_cast<u32>(event.window.data2);
                TRYX(reinit_vk_swapchain(m_window_size));
                break;
            default: break;
        }
    }

    auto swapchain_image_index = 0u;
    switch (auto&& res = TRYX(error::from_vk(
              m_vk_fns->acquire_next_image_khr(*m_device_things, *m_swapchain_things, a_second, present_semaphore, nullptr), "could not acquire the next image in the swapchain"
            ));
            res.second) {
        case VK_SUCCESS: [[fallthrough]];
        case VK_SUBOPTIMAL_KHR: swapchain_image_index = res.first; break;
        default: return std::unexpected{error::make_vk(res.second, "could not acquire the next image in the swapchain")};
    }

    m_vk_fns->reset_command_buffer(m_device_things->queue(0));

    const auto command_begin_info = VkCommandBufferBeginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      .pInheritanceInfo = nullptr,
    };
    std::ignore = m_vk_fns->begin_command_buffer(m_device_things->queue(0), &command_begin_info);

    const auto clear_color = std::abs(std::sin(static_cast<float>(frame_number) / color_period));
    const auto clear_value = VkClearValue{VkClearColorValue{
      .float32{0.f, 0.f, clear_color, 1.f},
    }};

    const auto render_pass_info = VkRenderPassBeginInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .pNext = nullptr,
      .renderPass = m_vk_render_pass,
      .framebuffer = m_vk_framebuffers[swapchain_image_index],
      .renderArea = VkRect2D{{0, 0}, m_window_size},
      .clearValueCount = 1,
      .pClearValues = &clear_value,
    };

    m_vk_fns->cmd_begin_render_pass(m_device_things->queue(0), &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
    m_vk_fns->cmd_end_render_pass(m_device_things->queue(0));

    TRYX(error::from_vk(m_vk_fns->end_command_buffer(m_device_things->queue(0)), "failed to end the command buffer"));

    const auto wait_dst_stage_flags = (VkPipelineStageFlags)VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    const auto submit_info = VkSubmitInfo{
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &present_semaphore,
      .pWaitDstStageMask = &wait_dst_stage_flags,
      .commandBufferCount = 1,
      .pCommandBuffers = &(m_device_things->queue(0).command_buffer),
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &render_semaphore,
    };

    TRYX(error::from_vk(m_vk_fns->queue_submit(m_device_things->queue(0), 1, &submit_info, render_fence), "failed to submit commands to the queue"));

    const auto swapchain = m_swapchain_things->swapchain();
    const auto present_info = VkPresentInfoKHR{
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &render_semaphore,
      .swapchainCount = 1,
      .pSwapchains = &swapchain,
      .pImageIndices = &swapchain_image_index,
      .pResults = nullptr,
    };

    switch (auto&& res = TRYX(error::from_vk(m_vk_fns->queue_present_khr(m_device_things->queue(0), &present_info), "failed to present")); res) {
        case VK_SUCCESS: [[fallthrough]];
        case VK_SUBOPTIMAL_KHR: [[fallthrough]];
        case VK_ERROR_OUT_OF_DATE_KHR: break;
        default: return std::unexpected{error::make_vk(std::move(res), "failed to present")};
    }

    return running;
}

auto vulkan_stuff::run() -> std::expected<void, error> {
    static constexpr auto a_second = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();

    const auto fence_create_info = VkFenceCreateInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, VK_FENCE_CREATE_SIGNALED_BIT};
    auto* const render_fence = TRYX(error::from_vk(m_vk_fns->create_fence(*m_device_things, &fence_create_info), "could not create a fence (for renders)"));
    UNNAMED = stf::scope_exit{[this, &render_fence] { m_vk_fns->destroy_fence(*m_device_things, render_fence); }};

    const auto semaphore_create_info = VkSemaphoreCreateInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, nullptr, 0};

    auto* const present_semaphore = TRYX(error::from_vk(m_vk_fns->create_semaphore(*m_device_things, &semaphore_create_info), "could not create a semaphore (for present)"));
    UNNAMED = stf::scope_exit{[this, &present_semaphore] { m_vk_fns->destroy_semaphore(*m_device_things, present_semaphore); }};

    auto* const render_semaphore = TRYX(error::from_vk(m_vk_fns->create_semaphore(*m_device_things, &semaphore_create_info), "could not create a semaphore (for render)"));
    UNNAMED = stf::scope_exit{[this, &render_semaphore] { m_vk_fns->destroy_semaphore(*m_device_things, render_semaphore); }};

    for (auto frame_number = 0uz;; frame_number++) {
        std::ignore = m_vk_fns->wait_for_fences(*m_device_things, 1, &render_fence, VK_TRUE, a_second);
        std::ignore = m_vk_fns->reset_fences(*m_device_things, 1, &render_fence);

        auto res = frame(render_fence, present_semaphore, render_semaphore, frame_number);

        if (res) {
            if (*res) {
                continue;
            }

            break;
        }

        auto const& error = res.error();

        spdlog::warn("received an error: {}", error);

        switch (error.m_vk_result) {
            case VK_SUBOPTIMAL_KHR: [[fallthrough]];
            case VK_ERROR_OUT_OF_DATE_KHR: spdlog::warn("asd"); break;
            default: spdlog::error("the error was not a soft error, propagating"); return std::unexpected{error};
        }
    }

    std::ignore = m_vk_fns->wait_for_fences(*m_device_things, 1, &render_fence, VK_TRUE, a_second);
    std::ignore = m_vk_fns->reset_fences(*m_device_things, 1, &render_fence);

    return {};
}

auto vulkan_stuff::read_shader(const char* file_name) -> std::expected<VkShaderModule, error> {
    thread_local static char strerror_buffer[64];

    auto* file = std::fopen(file_name, "rb");
    if (file == nullptr) {
        return std::unexpected{error::make(fmt::format("failed to open {} for reading. errno: {}", file_name, ::strerror_r(errno, strerror_buffer, sizeof(strerror_buffer))))};
    }

    if (std::fseek(file, 0, SEEK_END) != 0) {
        return std::unexpected{error::make(fmt::format("failed to seek to the end of {}. errno: {}", file_name, ::strerror_r(errno, strerror_buffer, sizeof(strerror_buffer))))};
    }

    auto file_size = 0uz;
    if (auto res = std::ftell(file); res >= 0) {
        file_size = static_cast<usize>(res);
    } else {
        return std::unexpected{error::make(fmt::format("failed to determine the size of {}. errno: {}", file_name, ::strerror_r(errno, strerror_buffer, sizeof(strerror_buffer))))};
    }

    if ((file_size % sizeof(u32)) != 0 || file_size == 0) {
        return std::unexpected{error::make(fmt::format("file {} has an invalid size {}", file_name, file_size))};
    }

    if (std::fseek(file, 0, SEEK_SET) != 0) {
        return std::unexpected{error::make(fmt::format("failed to rewind {}. errno: {}", file_name, ::strerror_r(errno, strerror_buffer, sizeof(strerror_buffer))))};
    }

    auto file_contents = std::pmr::vector<u32>(file_size);

    if (std::fread(file_contents.data(), sizeof(u32), file_size / 4, file) != file_size / 4) {
        return std::unexpected{error::make(fmt::format("failed to read all bytes from {}. errno: {}", file_name, ::strerror_r(errno, strerror_buffer, sizeof(strerror_buffer))))};
    }

    const auto shader_create_info = VkShaderModuleCreateInfo{
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .codeSize = file_size,
      .pCode = file_contents.data(),
    };

    auto* const shader = TRYX(error::from_vk(m_vk_fns->create_shader_module(*m_device_things, &shader_create_info), fmt::format("while reading shader from {}", file_name)));

    return shader;
}

auto vulkan_stuff::destroy_render_pass() -> std::expected<void, error> {
    for (auto framebuffer : m_vk_framebuffers) {
        if (framebuffer == nullptr) {
            continue;
        }

        m_vk_fns->destroy_framebuffer(*m_device_things, framebuffer);
    }

    if (m_vk_render_pass != nullptr) {
        m_vk_fns->destroy_render_pass(*m_device_things, m_vk_render_pass);
    }

    return {};
}

auto vulkan_stuff::reinit_vk_swapchain(VkExtent2D extent) -> std::expected<void, error> {
    auto old_swapchain = std::move(m_swapchain_things);

    if (old_swapchain) {
        TRYX(destroy_render_pass());
    }

    m_swapchain_things = std::make_shared<swapchain_things>(m_vk_fns, m_device_things);
    TRYX(m_swapchain_things->init(
      *m_surface_things,
      swapchain_settings{
        .m_extent = extent,
      },
      old_swapchain ? *old_swapchain : (VkSwapchainKHR) nullptr
    ));

    auto color_attachment = VkAttachmentDescription{
      .flags = {},                                         //
      .format = m_swapchain_things->format().format,       //
      .samples = VK_SAMPLE_COUNT_1_BIT,                    //
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,               //
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,             //
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,    //
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,  //
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,          //
      .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,      //
    };

    auto color_attachment_ref = VkAttachmentReference{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

    auto subpass = VkSubpassDescription{
      .flags = 0,
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .inputAttachmentCount = 0,
      .pInputAttachments = nullptr,
      .colorAttachmentCount = 1,
      .pColorAttachments = &color_attachment_ref,
      .pResolveAttachments = 0,
      .pDepthStencilAttachment = nullptr,
      .preserveAttachmentCount = 0,
      .pPreserveAttachments = nullptr,
    };

    auto render_pass_create_info = VkRenderPassCreateInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,  //
      .pNext = nullptr,                                    //
      .flags = 0,                                          //
      .attachmentCount = 1,                                //
      .pAttachments = &color_attachment,                   //
      .subpassCount = 1,                                   //
      .pSubpasses = &subpass,                              //
      .dependencyCount = 0,                                //
      .pDependencies = nullptr,                            //
    };

    m_vk_render_pass = TRYX(error::from_vk(m_vk_fns->create_render_pass(*m_device_things, &render_pass_create_info), "could not create a render pass"));

    m_vk_framebuffers = std::pmr::vector<VkFramebuffer>{m_swapchain_things->image_views().size()};
    for (auto const& [no, view] : m_swapchain_things->image_views() | std::views::enumerate) {
        auto framebuffer_create_info = VkFramebufferCreateInfo{
          .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .renderPass = m_vk_render_pass,
          .attachmentCount = 1,
          .pAttachments = &view,
          .width = m_window_size.width,
          .height = m_window_size.height,
          .layers = 1,
        };

        m_vk_framebuffers[no] =
          TRYX(error::from_vk(m_vk_fns->create_framebuffer(*m_device_things, &framebuffer_create_info), fmt::format("could not create a framebuffer (index {})", no)));
    }

    return {};
}

}  // namespace vxl::vk
