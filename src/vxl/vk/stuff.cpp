#include <vxl/vk/stuff.hpp>

#include <stuff/blas.hpp>
#include <stuff/ply.hpp>
#include <stuff/scope.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>

namespace vxl::vk {

template<usize Bindings, usize Attributes>
struct vertex_input_description {
    std::array<VkVertexInputBindingDescription, Bindings> bindings;
    std::array<VkVertexInputAttributeDescription, Attributes> attributes;

    VkPipelineVertexInputStateCreateFlags flags = 0;
};

struct vert {
    stf::blas::vector<float, 4> m_position;
    stf::blas::vector<float, 4> m_normal;
    stf::blas::vector<float, 4> m_color;
    stf::blas::vector<float, 2> m_texture_coordinate;

    // TODO: use stf::intro to generate descriptions automatically
    static constexpr auto description() -> vertex_input_description<1, 4> {
        auto ret = vertex_input_description<1, 4>{};

        ret.bindings[0] = VkVertexInputBindingDescription{
          .binding = 0,
          .stride = sizeof(vert),
          .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        };

        ret.attributes[0] = VkVertexInputAttributeDescription{
          .location = 0,
          .binding = 0,
          .format = VK_FORMAT_R32G32B32A32_SFLOAT,
          .offset = offsetof(vert, m_position),
        };

        ret.attributes[1] = VkVertexInputAttributeDescription{
          .location = 1,
          .binding = 0,
          .format = VK_FORMAT_R32G32B32A32_SFLOAT,
          .offset = offsetof(vert, m_normal),
        };

        ret.attributes[2] = VkVertexInputAttributeDescription{
          .location = 2,
          .binding = 0,
          .format = VK_FORMAT_R32G32B32A32_SFLOAT,
          .offset = offsetof(vert, m_color),
        };

        ret.attributes[3] = VkVertexInputAttributeDescription{
          .location = 3,
          .binding = 0,
          .format = VK_FORMAT_R32G32_SFLOAT,
          .offset = offsetof(vert, m_texture_coordinate),
        };

        return ret;
    }
};

struct push_constants {
    stf::blas::matrix<float, 4, 4> m_transform;
};

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

    TRYX(ret.m_dyna_loader.init(get_vulkan_library_names()).transform_error([](auto err) { return error::make(std::string{err}); }));
    ret.m_vk_fns = std::make_shared<vulkan_functions>();

    ret.m_vk_fns->init(ret.m_dyna_loader.get_dl_symbol<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"));

    ret.m_instance = std::make_shared<instance_things>(ret.m_app_info, ret.m_vk_fns);
    TRYX(ret.m_instance->init());

    ret.m_debug_messenger = std::make_shared<debug_messenger_things>(ret.m_instance, ret.m_vk_fns);
    TRYX(ret.m_debug_messenger->init());

    ret.m_surface = std::make_shared<surface_things>(ret.m_instance, ret.m_vk_fns);
    TRYX(ret.m_surface->init(ret.m_window_size));

    ret.m_device = std::make_shared<device_things>(ret.m_instance, ret.m_vk_fns);
    TRYX(ret.m_device->init(
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
          .m_present_to = ret.m_surface->surface(),
          .m_features = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT,
        }},
        .m_min_vk_version = VK_VERSION_1_2,
      }
    ));

    ret.m_vertex_shader = TRYX(ret.read_shader("triangle.vert.spv"));
    ret.m_fragment_shader = TRYX(ret.read_shader("triangle.frag.spv"));

    auto push_constant_range = VkPushConstantRange{
      .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
      .offset = 0,
      .size = sizeof(push_constants),
    };

    const auto pipeline_layout_create_info = VkPipelineLayoutCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .setLayoutCount = 0,
      .pSetLayouts = nullptr,
      .pushConstantRangeCount = 1,
      .pPushConstantRanges = &push_constant_range,
    };

    ret.m_pipeline_layout = TRYX(error::from_vk(ret.m_vk_fns->create_pipeline_layout(*ret.m_device, &pipeline_layout_create_info)));

    TRYX(ret.reinit_vk_swapchain(ret.m_window_size));

    return ret;
}

vulkan_stuff::vulkan_stuff(vulkan_stuff&& other) noexcept
    : m_dyna_loader(std::move(other.m_dyna_loader))
    , m_vk_fns(std::move(other.m_vk_fns))
    , m_instance(std::move(other.m_instance))
    , m_debug_messenger(std::move(other.m_debug_messenger))
    , m_window_size(other.m_window_size)
    , m_surface(std::move(other.m_surface))
    , m_device(std::move(other.m_device))
    , m_swapchain(std::move(other.m_swapchain))
    , m_vk_framebuffers(std::move(other.m_vk_framebuffers))
    , m_pipeline(std::move(other.m_pipeline)) {
#pragma push_macro("MOVE")
#define MOVE(_name)            \
    this->_name = other._name; \
    other._name = {}

    MOVE(m_vk_render_pass);
    MOVE(m_fragment_shader);
    MOVE(m_vertex_shader);
    MOVE(m_pipeline_layout);

#pragma pop_macro("MOVE")
}

vulkan_stuff::~vulkan_stuff() {
    std::ignore = destroy_render_pass();

    if (m_pipeline_layout != nullptr) {
        m_vk_fns->destroy_pipeline_layout(*m_device, m_pipeline_layout);
        m_pipeline_layout = nullptr;
    }

    if (m_fragment_shader != nullptr) {
        m_vk_fns->destroy_shader_module(*m_device, m_fragment_shader);
    }

    if (m_vertex_shader != nullptr) {
        m_vk_fns->destroy_shader_module(*m_device, m_vertex_shader);
    }
}

static auto read_ply(std::filesystem::path filename) -> std::expected<std::vector<vert>, stf::ply::error> {
    auto ifs = std::ifstream(filename);
    auto context = stf::ply::context(std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>{});

    auto header = TRYX(context.read_header());

    auto vertices = std::vector<std::tuple<float, float, float>>{};
    auto indices = std::vector<std::array<u16, 3>>{};

    for (usize i = 0; i < header.m_elements[0].m_size; i++) {
        auto&& elem = TRYX(context.read_element<std::tuple<float, float, float>>(0));
        vertices.emplace_back(std::move(elem));
    }

    for (usize i = 0; i < header.m_elements[1].m_size; i++) {
        auto elem = TRYX(context.read_element<std::array<u16, 3>>(0));
        indices.emplace_back(elem);
    }

    auto mesh = std::vector<vert>{};
    mesh.reserve(indices.size() * 3);

    for (auto [idx_0, idx_1, idx_2] : indices) {
        static constexpr auto tup_to_vec = []<typename T>(std::tuple<T, T, T> tup) -> stf::blas::vector<T, 3> {
            return stf::blas::vector<T, 3>{std::get<0>(tup), std::get<1>(tup), std::get<2>(tup)};
        };

        static constexpr auto triangle_norm = [](auto vert_0, auto vert_1, auto vert_2) {
            auto edge_0 = vert_1 - vert_0;
            auto edge_1 = vert_2 - vert_0;

            auto res = normalize(cross(edge_0, edge_1));

            return res;
        };

        auto vert_0 = tup_to_vec(vertices[idx_0]);
        auto vert_1 = tup_to_vec(vertices[idx_1]);
        auto vert_2 = tup_to_vec(vertices[idx_2]);

        auto norm = triangle_norm(vert_0, vert_1, vert_2);

        mesh.emplace_back(vert{.m_position = stf::blas::vector<float, 4>(vert_0, 1.f), .m_normal = stf::blas::vector<float, 4>(norm, 0.f)});
        mesh.emplace_back(vert{.m_position = stf::blas::vector<float, 4>(vert_1, 1.f), .m_normal = stf::blas::vector<float, 4>(norm, 0.f)});
        mesh.emplace_back(vert{.m_position = stf::blas::vector<float, 4>(vert_2, 1.f), .m_normal = stf::blas::vector<float, 4>(norm, 0.f)});
    }

    return mesh;
}

auto vulkan_stuff::run() -> std::expected<void, error> {
    static constexpr auto a_second = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();

    const auto fence_create_info = VkFenceCreateInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, VK_FENCE_CREATE_SIGNALED_BIT};
    auto* const render_fence = TRYX(error::from_vk(m_vk_fns->create_fence(*m_device, &fence_create_info), "could not create a fence (for renders)"));
    UNNAMED = stf::scope_exit{[this, &render_fence] { m_vk_fns->destroy_fence(*m_device, render_fence); }};

    const auto semaphore_create_info = VkSemaphoreCreateInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, nullptr, 0};

    auto* const present_semaphore = TRYX(error::from_vk(m_vk_fns->create_semaphore(*m_device, &semaphore_create_info), "could not create a semaphore (for present)"));
    UNNAMED = stf::scope_exit{[this, &present_semaphore] { m_vk_fns->destroy_semaphore(*m_device, present_semaphore); }};

    auto* const render_semaphore = TRYX(error::from_vk(m_vk_fns->create_semaphore(*m_device, &semaphore_create_info), "could not create a semaphore (for render)"));
    UNNAMED = stf::scope_exit{[this, &render_semaphore] { m_vk_fns->destroy_semaphore(*m_device, render_semaphore); }};

    using mat4x4 = stf::blas::matrix<float, 4, 4>;

    auto vertices = *read_ply("./bun_zipper.ply");

    /*auto vertices = std::array<vert, 3>{
      vert{
        .m_position{.5f, -.5f, .5f, 1.f},
        .m_normal{0.f, 0.f, 0.f, 0.f},
        .m_color{1.f, 0.f, 0.f, 1.f},
      },
      vert{
        .m_position{-.5f, -.5f, .5f, 1.f},
        .m_normal{0.f, 0.f, 0.f, 0.f},
        .m_color{0.0f, 1.f, 0.f, 1.f},
      },
      vert{
        .m_position{0.f, .5f, .5f, 1.f},
        .m_normal{0.f, 0.f, 0.f, 0.f},
        .m_color{0.f, 0.f, 1.f, 1.f},
      },
    };*/

    auto center = stf::blas::vector<float, 3>{};
    for (auto& vert : vertices) {
        auto mat = mat4x4::scale(5.f, 5.f, 5.f);
        vert.m_position = mat * vert.m_position;
        center = center + swizzle<"xyz">(vert.m_position);
    }

    center = center / vertices.size();
    for (auto& vert : vertices) {
        auto sphere_point = normalize(swizzle<"xyz">(vert.m_position) - center);
        auto u = 0.5f + std::atan2(sphere_point[2], sphere_point[0]) / (2 * std::numbers::pi_v<float>);
        auto v = 0.5f + std::asin(sphere_point[1]) / std::numbers::pi_v<float>;
        vert.m_texture_coordinate = {u, v};
    }

    auto allocation = TRYX(m_device->allocate(sizeof(vert) * vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT));

    auto constants_to_push = push_constants{
      .m_transform = mat4x4::identity(),
    };

    struct {
        stf::blas::vector<float, 3> m_offset;
        stf::blas::vector<float, 3> m_rotation;
    } transform_info{};

    auto running = true;
    auto auto_rotate = false;
    for (auto frame_number = 0uz; running; frame_number++) {
        std::ignore = m_vk_fns->wait_for_fences(*m_device, 1, &render_fence, VK_TRUE, a_second);
        std::ignore = m_vk_fns->reset_fences(*m_device, 1, &render_fence);

        const auto rot_param = (static_cast<float>(frame_number % 120) / 60.f - 1.f) * std::numbers::pi_v<float>;

        auto rotation = mat4x4::rotate(rot_param, 0.f, 0.f);

        /*for (auto& vert : vertices) {
            vert.m_position = rotation * vert.m_position;
        }*/

        *allocation.write(vertices.data(), vertices.size());

        for (SDL_Event event; SDL_PollEvent(&event) != 0;) {
            switch (event.type) {
                case SDL_EVENT_QUIT: running = false; break;
                case SDL_EVENT_WINDOW_RESIZED:
                    TRYX(error::from_vk(m_vk_fns->device_wait_idle(*m_device), "while waiting for device idle"));

                    // TRYX(destroy_vk_swapchain());
                    m_window_size.width = static_cast<u32>(event.window.data1);
                    m_window_size.height = static_cast<u32>(event.window.data2);
                    TRYX(reinit_vk_swapchain(m_window_size));
                    break;
                case SDL_EVENT_KEY_DOWN:
                    event.key.keysym.sym;
                    if (event.key.keysym.sym == SDLK_w) {
                        transform_info.m_offset[2] -= 0.05f;
                    }
                    if (event.key.keysym.sym == SDLK_a) {
                        transform_info.m_offset[0] -= 0.05f;
                    }
                    if (event.key.keysym.sym == SDLK_s) {
                        transform_info.m_offset[2] += 0.05f;
                    }
                    if (event.key.keysym.sym == SDLK_d) {
                        transform_info.m_offset[0] += 0.05f;
                    }
                    if (event.key.keysym.sym == SDLK_r) {
                        transform_info.m_offset[1] += 0.05f;
                    }
                    if (event.key.keysym.sym == SDLK_f) {
                        transform_info.m_offset[1] -= 0.05f;
                    }
                    if (event.key.keysym.sym == SDLK_e) {
                        transform_info.m_rotation[1] += 0.05f;
                    }
                    if (event.key.keysym.sym == SDLK_q) {
                        transform_info.m_rotation[1] -= 0.05f;
                    }

                    if (event.key.keysym.sym == SDLK_SPACE) {
                        auto_rotate = !auto_rotate;
                    }
                    break;

                default: break;
            }
        }

        if (auto_rotate) {
            transform_info.m_rotation[1] += 0.05f;
        }

        constants_to_push.m_transform =  //
          mat4x4::translate(transform_info.m_offset[0], transform_info.m_offset[1], transform_info.m_offset[2]) *
          mat4x4::rotate(transform_info.m_rotation[0], transform_info.m_rotation[1], transform_info.m_rotation[2]);

        constants_to_push.m_transform = transpose(constants_to_push.m_transform);

        auto frame = frame_things(*m_vk_fns, *m_device, *m_swapchain, render_fence, present_semaphore, render_semaphore, m_window_size, m_vk_render_pass, m_vk_framebuffers);

        TRYX(frame.begin());

        m_vk_fns->cmd_bind_vertex_buffer(m_device->queue(0), 0, allocation.m_buffer, 0);

        m_vk_fns->cmd_bind_pipeline(m_device->queue(0), VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
        m_vk_fns->cmd_push_constants(m_device->queue(0), m_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(constants_to_push), &constants_to_push);
        m_vk_fns->cmd_draw(m_device->queue(0), vertices.size(), 1, 0, 0);

        TRYX(frame.end());
    }

    std::ignore = m_vk_fns->wait_for_fences(*m_device, 1, &render_fence, VK_TRUE, a_second);
    std::ignore = m_vk_fns->reset_fences(*m_device, 1, &render_fence);

    return {};
}

auto vulkan_stuff::read_shader(const char* file_name) -> std::expected<VkShaderModule, error> {
    thread_local static char strerror_buffer[64];

    auto* file = std::fopen(file_name, "rb");
    if (file == nullptr) {
        return std::unexpected{error::make(std::format("failed to open {} for reading. errno: {}", file_name, ::strerror_r(errno, strerror_buffer, sizeof(strerror_buffer))))};
    }

    if (std::fseek(file, 0, SEEK_END) != 0) {
        return std::unexpected{error::make(std::format("failed to seek to the end of {}. errno: {}", file_name, ::strerror_r(errno, strerror_buffer, sizeof(strerror_buffer))))};
    }

    auto file_size = 0uz;
    if (auto res = std::ftell(file); res >= 0) {
        file_size = static_cast<usize>(res);
    } else {
        return std::unexpected{error::make(std::format("failed to determine the size of {}. errno: {}", file_name, ::strerror_r(errno, strerror_buffer, sizeof(strerror_buffer))))};
    }

    if ((file_size % sizeof(u32)) != 0 || file_size == 0) {
        return std::unexpected{error::make(std::format("file {} has an invalid size {}", file_name, file_size))};
    }

    if (std::fseek(file, 0, SEEK_SET) != 0) {
        return std::unexpected{error::make(std::format("failed to rewind {}. errno: {}", file_name, ::strerror_r(errno, strerror_buffer, sizeof(strerror_buffer))))};
    }

    auto file_contents = std::pmr::vector<u32>(file_size);

    if (std::fread(file_contents.data(), sizeof(u32), file_size / 4, file) != file_size / 4) {
        return std::unexpected{error::make(std::format("failed to read all bytes from {}. errno: {}", file_name, ::strerror_r(errno, strerror_buffer, sizeof(strerror_buffer))))};
    }

    const auto shader_create_info = VkShaderModuleCreateInfo{
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .codeSize = file_size,
      .pCode = file_contents.data(),
    };

    auto* const shader = TRYX(error::from_vk(m_vk_fns->create_shader_module(*m_device, &shader_create_info), std::format("while reading shader from {}", file_name)));

    return shader;
}

auto vulkan_stuff::destroy_render_pass() -> std::expected<void, error> {
    for (auto framebuffer : m_vk_framebuffers) {
        if (framebuffer == nullptr) {
            continue;
        }

        m_vk_fns->destroy_framebuffer(*m_device, framebuffer);
    }

    if (m_vk_render_pass != nullptr) {
        m_vk_fns->destroy_render_pass(*m_device, m_vk_render_pass);
    }

    return {};
}

auto vulkan_stuff::reinit_vk_swapchain(VkExtent2D extent) -> std::expected<void, error> {
    auto old_swapchain = std::move(m_swapchain);

    if (old_swapchain) {
        TRYX(destroy_render_pass());
    }

    m_swapchain = std::make_shared<swapchain_things>(m_vk_fns, m_device);
    TRYX(m_swapchain->init(
      *m_surface,
      swapchain_settings{
        .m_extent = extent,
      },
      old_swapchain ? *old_swapchain : (VkSwapchainKHR) nullptr
    ));

    auto color_attachment = VkAttachmentDescription{
      .flags = {},                                         //
      .format = m_swapchain->format().format,              //
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

    m_vk_render_pass = TRYX(error::from_vk(m_vk_fns->create_render_pass(*m_device, &render_pass_create_info), "could not create a render pass"));

    m_vk_framebuffers = std::pmr::vector<VkFramebuffer>{m_swapchain->image_views().size()};
    for (auto const& [no, view] : m_swapchain->image_views() | std::views::enumerate) {
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
          TRYX(error::from_vk(m_vk_fns->create_framebuffer(*m_device, &framebuffer_create_info), std::format("could not create a framebuffer (index {})", no)));
    }

    const auto vertex_desc = vert::description();

    auto stages = std::array<VkPipelineShaderStageCreateInfo, 2>{
      VkPipelineShaderStageCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = m_vertex_shader,
        .pName = "main",
        .pSpecializationInfo = nullptr,
      },
      VkPipelineShaderStageCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = m_fragment_shader,
        .pName = "main",
        .pSpecializationInfo = nullptr,
      },
    };

    auto p_settings = pipeline_settings{
      .m_shader_stages = stages,
      .m_vertex_input_info =
        VkPipelineVertexInputStateCreateInfo{
          .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .vertexBindingDescriptionCount = static_cast<u32>(vertex_desc.bindings.size()),
          .pVertexBindingDescriptions = vertex_desc.bindings.data(),
          .vertexAttributeDescriptionCount = static_cast<u32>(vertex_desc.attributes.size()),
          .pVertexAttributeDescriptions = vertex_desc.attributes.data(),
        },
      .m_input_assembly =
        VkPipelineInputAssemblyStateCreateInfo{
          .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
          .primitiveRestartEnable = VK_FALSE,
        },
      .m_viewport =
        VkViewport{
          .x = 0.f,
          .y = 0.f,
          .width = static_cast<float>(extent.width),
          .height = static_cast<float>(extent.height),
          .minDepth = 0.f,
          .maxDepth = 1.f,
        },
      .m_scissor =
        VkRect2D{
          .offset{0, 0},
          .extent = extent,
        },
      .m_rasterizer =
        VkPipelineRasterizationStateCreateInfo{
          .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .depthClampEnable = VK_FALSE,
          .rasterizerDiscardEnable = VK_FALSE,
          .polygonMode = VK_POLYGON_MODE_FILL,
          .cullMode = VK_CULL_MODE_BACK_BIT,
          .frontFace = VK_FRONT_FACE_CLOCKWISE,
          .depthBiasEnable = VK_FALSE,
          .depthBiasConstantFactor = 0.f,
          .depthBiasClamp = 0.f,
          .depthBiasSlopeFactor = 0.f,
          .lineWidth = 1.f,
        },
      .m_color_blend_attachment =
        VkPipelineColorBlendAttachmentState{
          .blendEnable = VK_FALSE,
          .srcColorBlendFactor = {},
          .dstColorBlendFactor = {},
          .colorBlendOp = {},
          .srcAlphaBlendFactor = {},
          .dstAlphaBlendFactor = {},
          .alphaBlendOp = {},
          .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        },
      .m_multisampling =
        VkPipelineMultisampleStateCreateInfo{
          .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
          .sampleShadingEnable = VK_FALSE,
          .minSampleShading = 1.f,
          .pSampleMask = nullptr,
          .alphaToCoverageEnable = VK_FALSE,
          .alphaToOneEnable = VK_FALSE,
        },
      .m_pipeline_layout = m_pipeline_layout,
    };

    m_pipeline = std::make_shared<pipeline_things>(m_vk_fns, m_device);
    TRYX(m_pipeline->init(p_settings, *m_device, m_vk_render_pass));

    return {};
}

}  // namespace vxl::vk
