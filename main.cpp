#include <vxl/dyna_loader.hpp>
#include <vxl/vk/functions.hpp>
#include <vxl/vk/loggers.hpp>
#include <vxl/vk/utils.hpp>

#include <stuff/core.hpp>
#include <stuff/expected.hpp>
#include <stuff/intro/introspectors/function.hpp>
#include <stuff/scope.hpp>

#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <chrono>
#include <cmath>
#include <expected>
#include <iostream>
#include <ranges>

/*
struct vulkan_stuff {
    static auto make() -> std::expected<vulkan_stuff, error> {


        TRYX(ret.init_vk_debug_messenger());
        TRYX(ret.init_sdl_window());
        TRYX(ret.init_vk_surface());
        TRYX(ret.init_vk_devices_and_queues());
        TRYX(ret.init_vk_command_pool());
        TRYX(ret.init_vk_command_buffer());
        TRYX(ret.reinit_vk_swapchain(ret.m_window_size));

        // return std::unexpected{"WIP"};
        return ret;
    }

    vulkan_stuff() = default;

    ~vulkan_stuff() {
        std::ignore = destroy_vk_swapchain();
    }

    auto frame(vk::Fence& render_fence, vk::Semaphore& present_semaphore, vk::Semaphore& render_semaphore, usize frame_number) -> std::expected<bool, error> {
        static constexpr auto a_second = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();

        bool running = true;
        static constexpr auto color_period = 120.f;

        for (SDL_Event event; SDL_PollEvent(&event) != 0;) {
            switch (event.type) {
                case SDL_EVENT_QUIT: running = false; break;
                case SDL_EVENT_WINDOW_RESIZED:
                    TRYX(error::from_vk(m_vk_device.waitIdle(m_vk_dispatch), "while waiting for device idle"));

                    // TRYX(destroy_vk_swapchain());
                    m_window_size.width = static_cast<u32>(event.window.data1);
                    m_window_size.height = static_cast<u32>(event.window.data2);
                    TRYX(reinit_vk_swapchain(m_window_size));
                    break;
                default: break;
            }
        }

        auto swapchain_image_index = 0u;
        switch (auto&& res = m_vk_device.acquireNextImageKHR(m_vk_swapchain, a_second, present_semaphore, nullptr, m_vk_dispatch); res.result) {
            case vk::Result::eSuccess: [[fallthrough]];
            case vk::Result::eSuboptimalKHR: swapchain_image_index = res.value; break;
            default: return std::unexpected{error::make_vk(res.result, "could not acquire the next image in the swapchain")};
        }

        m_vk_command_buffer.reset({}, m_vk_dispatch);

        const auto command_begin_info = vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        std::ignore = m_vk_command_buffer.begin(command_begin_info);

        const auto clear_color = std::abs(std::sin(static_cast<float>(frame_number) / color_period));
        const auto clear_value = vk::ClearValue((std::array<float, 4>){0.f, 0.f, clear_color, 1.f});

        const auto render_pass_info = vk::RenderPassBeginInfo(
          m_vk_render_pass,                                                                     //
          m_vk_framebuffers[swapchain_image_index],                                             //
          vk::Rect2D(vk::Offset2D(), vk::Extent2D(m_window_size.width, m_window_size.height)),  //
          1, &clear_value                                                                       //
        );

        m_vk_command_buffer.beginRenderPass(render_pass_info, vk::SubpassContents::eInline, m_vk_dispatch);
        m_vk_command_buffer.endRenderPass(m_vk_dispatch);

        TRYX(error::from_vk(m_vk_command_buffer.end(m_vk_dispatch), "failed to end the command buffer"));

        const auto wait_dst_stage_flags = vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        const auto submit_info = vk::SubmitInfo(
          1, &present_semaphore,    //
          &wait_dst_stage_flags,    //
          1, &m_vk_command_buffer,  //
          1, &render_semaphore      //
        );

        TRYX(error::from_vk(m_vk_queue.submit(1, &submit_info, render_fence, m_vk_dispatch), "failed to submit commands to the queue"));

        const auto present_info = vk::PresentInfoKHR(
          1, &render_semaphore,                        //
          1, &m_vk_swapchain, &swapchain_image_index,  //
          nullptr                                      //
        );

        switch (auto&& res = m_vk_queue.presentKHR(present_info, m_vk_dispatch); res) {
            case vk::Result::eSuccess: [[fallthrough]];
            case vk::Result::eSuboptimalKHR: break;
            default: return std::unexpected{error::make_vk(std::move(res), "failed to present")};
        }

        return running;
    }

    auto run() -> std::expected<void, error> {
        static constexpr auto a_second = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();

        auto fence_create_info = vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled);
        auto render_fence = TRYX(error::from_vk(m_vk_device.createFence(fence_create_info, nullptr, m_vk_dispatch), "could not create a fence (for renders)"));
        UNNAMED = stf::scope_exit{[this, &render_fence] { m_vk_device.destroy(render_fence, nullptr, m_vk_dispatch); }};

        auto present_semaphore = TRYX(error::from_vk(m_vk_device.createSemaphore(vk::SemaphoreCreateInfo{}, nullptr, m_vk_dispatch), "could not create a semaphore (for present)"));
        UNNAMED = stf::scope_exit{[this, &present_semaphore] { m_vk_device.destroy(present_semaphore, nullptr, m_vk_dispatch); }};

        auto render_semaphore = TRYX(error::from_vk(m_vk_device.createSemaphore(vk::SemaphoreCreateInfo{}, nullptr, m_vk_dispatch), "could not create a semaphore (for render)"));
        UNNAMED = stf::scope_exit{[this, &render_semaphore] { m_vk_device.destroy(render_semaphore, nullptr, m_vk_dispatch); }};

        for (auto frame_number = 0uz;; frame_number++) {
            std::ignore = m_vk_device.waitForFences(1, &render_fence, VK_TRUE, a_second, m_vk_dispatch);
            std::ignore = m_vk_device.resetFences(1, &render_fence, m_vk_dispatch);

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
                case vk::Result::eSuboptimalKHR: [[fallthrough]];
                case vk::Result::eErrorOutOfDateKHR: spdlog::warn("asd"); break;
                default: spdlog::error("the error was not a soft error, propagating"); return std::unexpected{error};
            }
        }

        std::ignore = m_vk_device.waitForFences(1, &render_fence, VK_TRUE, a_second, m_vk_dispatch);
        std::ignore = m_vk_device.resetFences(1, &render_fence, m_vk_dispatch);

        return {};
    }

private:

    VkRenderPass m_vk_render_pass;

    VkSwapchainKHR m_vk_swapchain;
    VkFormat m_vk_swapchain_format;
    std::vector<VkImage> m_vk_swapchain_images;
    std::vector<VkImageView> m_vk_swapchain_image_views;
    std::vector<VkFramebuffer> m_vk_framebuffers;

    auto init_vk_command_pool() -> std::expected<void, error> {
        const auto command_pool_create_info = vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, m_vk_queue_family_index);
        m_vk_command_pool = TRYX(error::from_vk(m_vk_device.createCommandPool(command_pool_create_info, nullptr, m_vk_dispatch), "could not create a vk::CommandPool"));

        return {};
    }

    auto init_vk_command_buffer() -> std::expected<void, error> {
        const auto command_buffer_allocate_info = vk::CommandBufferAllocateInfo(m_vk_command_pool, vk::CommandBufferLevel::ePrimary, 1);
        const auto command_buffers =
          TRYX(error::from_vk(m_vk_device.allocateCommandBuffers(command_buffer_allocate_info, m_vk_dispatch), "could not create a vector of vk::CommandBuffer"));

        if (command_buffers.empty()) {
            return std::unexpected{error::make("zero command buffers were created despite success")};
        }

        m_vk_command_buffer = command_buffers[0];

        return {};
    }

    auto destroy_vk_swapchain() -> std::expected<void, error> {
        if (!(bool)m_vk_swapchain) {
            return {};
        }

        for (auto const& framebuffer : m_vk_framebuffers) {
            if (!framebuffer) {
                continue;
            }

            m_vk_device.destroy(framebuffer, nullptr, m_vk_dispatch);
        }

        m_vk_device.destroy(m_vk_render_pass, nullptr, m_vk_dispatch);

        std::ranges::for_each(m_vk_swapchain_image_views, [this](auto const& view) { m_vk_device.destroy(view); });

        m_vk_device.destroy(m_vk_swapchain, nullptr, m_vk_dispatch);

        return {};
    }

    auto reinit_vk_swapchain(vk::Extent2D extent) -> std::expected<void, error> {
        const auto surface_capabilities =
          TRYX(error::from_vk(m_vk_physical_device.getSurfaceCapabilitiesKHR(m_vk_surface), "could not fetch surface capabilities from the physical device"));

        const auto surface_formats = TRYX(error::from_vk(m_vk_physical_device.getSurfaceFormatsKHR(m_vk_surface), "could not fetch surface formats from the physical device"));

        for (auto const& [no, format] : surface_formats | std::views::enumerate) {
            spdlog::debug("Surface #{}:", no);
            spdlog::debug("\tColor space: {}", vk::to_string(format.colorSpace));
            spdlog::debug("\tFormat     : {}", vk::to_string(format.format));
        }

        const auto surface_format = surface_formats[0].format == vk::Format::eUndefined ? vk::Format::eR8G8B8A8Unorm : surface_formats[0].format;
        m_vk_swapchain_format = surface_format;

        const auto swapchain_present_mode = vk::PresentModeKHR::eFifo;
        const auto pre_transform = (surface_capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) != vk::SurfaceTransformFlagBitsKHR{}
                                   ? vk::SurfaceTransformFlagBitsKHR::eIdentity
                                   : surface_capabilities.currentTransform;

        const auto composite_alpha = (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) != vk::CompositeAlphaFlagBitsKHR{}
                                     ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
                                   : (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) != vk::CompositeAlphaFlagBitsKHR{}
                                     ? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
                                   : (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit) != vk::CompositeAlphaFlagBitsKHR{}
                                     ? vk::CompositeAlphaFlagBitsKHR::eInherit
                                     : vk::CompositeAlphaFlagBitsKHR::eOpaque;

        const auto swapchain_create_info = vk::SwapchainCreateInfoKHR(
          vk::SwapchainCreateFlagsKHR(),                                                                                              //
          m_vk_surface,                                                                                                               //
          std::clamp(3u, surface_capabilities.minImageCount, surface_capabilities.maxImageCount ?: std::numeric_limits<u32>::max()),  //
          surface_format,                                                                                                             //
          vk::ColorSpaceKHR::eSrgbNonlinear,                                                                                          //
          extent,                                                                                                                     //
          1,                                                                                                                          //
          vk::ImageUsageFlagBits::eColorAttachment,                                                                                   //
          vk::SharingMode::eExclusive,                                                                                                //
          {},                                                                                                                         //
          pre_transform,                                                                                                              //
          composite_alpha,                                                                                                            //
          swapchain_present_mode,                                                                                                     //
          VK_TRUE,                                                                                                                    //
          m_vk_swapchain,                                                                                                             //
          nullptr                                                                                                                     //
        );

        const auto swapchain_temp = TRYX(error::from_vk(m_vk_device.createSwapchainKHR(swapchain_create_info), "could not create a swapchain"));
        TRYX(destroy_vk_swapchain());
        m_vk_swapchain = swapchain_temp;

        m_vk_swapchain_images = TRYX(error::from_vk(m_vk_device.getSwapchainImagesKHR(m_vk_swapchain), "could not get swapchain images"));

        m_vk_swapchain_images = TRYX(error::from_vk(m_vk_device.getSwapchainImagesKHR(m_vk_swapchain), "could not get swapchain images"));

        m_vk_swapchain_image_views = std::vector<vk::ImageView>{};
        m_vk_swapchain_image_views.reserve(m_vk_swapchain_images.size());

        vk::ImageViewCreateInfo imageViewCreateInfo({}, {}, vk::ImageViewType::e2D, surface_format, {}, {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
        for (auto const& [no, image] : m_vk_swapchain_images | std::views::enumerate) {
            imageViewCreateInfo.image = image;
            auto&& image_view = TRYX(error::from_vk(m_vk_device.createImageView(imageViewCreateInfo), "could not create a swapchain image view"));

            m_vk_swapchain_image_views.emplace_back(std::move(image_view));
        }

        auto color_attachment = vk::AttachmentDescription(
          {},                                //
          m_vk_swapchain_format,             //
          vk::SampleCountFlagBits::e1,       //
          vk::AttachmentLoadOp::eClear,      //
          vk::AttachmentStoreOp::eStore,     //
          vk::AttachmentLoadOp::eDontCare,   //
          vk::AttachmentStoreOp::eDontCare,  //
          vk::ImageLayout::eUndefined,       //
          vk::ImageLayout::ePresentSrcKHR    //
        );

        auto color_attachment_ref = vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);

        auto subpass = vk::SubpassDescription(
          {},                                //
          vk::PipelineBindPoint::eGraphics,  //
          0, nullptr,                        //
          1, &color_attachment_ref           //
        );

        auto render_pass_create_info = vk::RenderPassCreateInfo(
          {},                    //
          1, &color_attachment,  //
          1, &subpass            //
        );

        m_vk_render_pass = TRYX(error::from_vk(m_vk_device.createRenderPass(render_pass_create_info, nullptr, m_vk_dispatch), "could not create a render pass"));

        m_vk_framebuffers = std::vector<vk::Framebuffer>{m_vk_swapchain_image_views.size()};
        for (auto const& [no, view] : m_vk_swapchain_image_views | std::views::enumerate) {
            auto framebuffer_create_info = vk::FramebufferCreateInfo(
              {},                                         //
              m_vk_render_pass,                           //
              1, &view,                                   //
              m_window_size.width, m_window_size.height,  //
              1                                           //
            );

            auto&& res = m_vk_device.createFramebuffer(framebuffer_create_info, nullptr, m_vk_dispatch);
            m_vk_framebuffers[no] = TRYX(error::from_vk(std::move(res), fmt::format("could not create a framebuffer (index {})", no)));
        }

        return {};
    }
};
*/

namespace vxl::vk {

template<typename LayerType = VkLayerProperties, typename ExtensionType = VkExtensionProperties>
struct layer_extension_store {
    using layer_type = std::pair<LayerType, std::pmr::vector<ExtensionType>>;

    static auto make(auto&& layer_enumerator, auto&& extension_enumerator) -> std::expected<layer_extension_store, error> {
        auto ret = layer_extension_store{};

        auto layers = TRYX(error::from_vk(get_vector(layer_enumerator), "failed to enumerate instance layers"));

        ret.m_vk_instance_layers.reserve(layers.size());
        for (auto const& layer_props : layers) {
            const auto enumerator = [&extension_enumerator, &layer_props](u32* out_size, ExtensionType* out_extensions) {
                return extension_enumerator(layer_props.layerName, out_size, out_extensions);
            };

            auto&& layer_extensions = TRYX(error::from_vk(get_vector(enumerator), "failed to enumerate instance layer properties"));

            ret.m_vk_instance_layers.emplace_back(layer_props, std::move(layer_extensions));
        }

        auto const& layerless_extension_enumerator = [&extension_enumerator](u32* out_size, ExtensionType* out_extensions) {
            return extension_enumerator(nullptr, out_size, out_extensions);
        };

        ret.m_vk_instance_extensions = TRYX(error::from_vk(get_vector(layerless_extension_enumerator), "could not enumerate instance layer properties"));

        return ret;
    }

    constexpr auto layer(std::string_view layer_name) const -> std::optional<LayerType> {
        return layer_internal(layer_name).transform([](auto const& layer) { return layer.get().first; });
    }

    constexpr auto extension(std::string_view extension_name) const -> std::optional<ExtensionType> {
        auto iter = std::ranges::find_if(m_vk_instance_extensions, [extension_name](auto const& extension) { return extension_name == std::string_view(extension.extensionName); });

        if (iter == std::ranges::end(m_vk_instance_extensions)) {
            return std::nullopt;
        }

        return *iter;
    }

    constexpr auto extension(std::string_view extension_name, std::string_view layer_name) const -> std::optional<ExtensionType> {
        return layer_internal(layer_name).and_then([extension_name](auto const& layer) -> std::optional<ExtensionType> {
            auto iter = std::ranges::find_if(layer.get().second, [extension_name](auto const& layer) { return extension_name == std::string_view(layer.extensionName); });

            if (iter == std::ranges::end(layer.get().second)) {
                return std::nullopt;
            }

            return *iter;
        });
    }

    constexpr auto layers() const -> std::ranges::keys_view<std::pmr::vector<layer_type> const&> { return m_vk_instance_layers | std::views::keys; }

    constexpr auto extensions() const -> std::ranges::ref_view<const std::pmr::vector<ExtensionType>> { return m_vk_instance_extensions | std::views::all; }

    constexpr auto extensions(std::string_view layer_name) const -> std::optional<std::ranges::ref_view<const std::pmr::vector<ExtensionType>>> {
        return layer_internal(layer_name)  //
          .transform([](auto const& layer) { return layer.get().second | std::views::all; });
        //.value_or([] { return std::ranges::empty_view<ExtensionType>{}; });
    }

    static constexpr auto pick_stuff(auto&& desired, auto&& available, std::string_view name) -> std::expected<std::vector<const char*>, error> {
        static constexpr auto grouper = std::views::chunk_by([](auto const& lhs, auto const& rhs) { return lhs.second == rhs.second; });

        auto desired_groups = desired | grouper;
        const auto desired_group_count = std::ranges::distance(desired_groups);

        auto available_groups = available | grouper;
        const auto available_group_count = std::ranges::distance(available_groups);

        if (available_group_count != desired_group_count) {
            spdlog::error("not all desired {}s could be loaded, available_group_count ({}) != desired_group_count ({})", name, available_group_count, desired_group_count);

            spdlog::error("desired extensions (grouped):");
            for (auto const& [no, group] : desired_groups | std::views::enumerate) {
                spdlog::error("\tgroup #{}:", no);
                for (auto const& [ext_no, ext] : group | std::views::enumerate) {
                    spdlog::error("\t\t{} #{}: {}", name, ext_no, ext.first);
                }
            }

            spdlog::error("available {}s (grouped):", name);
            for (auto const& [no, group] : available_groups | std::views::enumerate) {
                spdlog::error("\tgroup #{}:", no);
                for (auto const& [ext_no, ext] : group | std::views::enumerate) {
                    spdlog::error("\t\t{} #{}: {}", name, ext_no, ext.first);
                }
            }

            return std::unexpected{error{}};
        }

        auto extension_names_range = available_groups | std::views::transform([](auto group) { return group | std::views::take(1); }) | std::views::join | std::views::keys;
        auto ret = std::vector<const char*>(std::ranges::distance(extension_names_range));
        std::ranges::copy(extension_names_range, ret.begin());

        spdlog::debug("enabling the following {}s:", name);
        for (auto const& [no, elem_name] : ret | std::views::enumerate) {
            spdlog::debug("\t{} #{}: {}", name, no, elem_name);
        }

        return ret;
    }

private:
    std::pmr::vector<layer_type> m_vk_instance_layers{};
    std::pmr::vector<ExtensionType> m_vk_instance_extensions{};

    constexpr auto layer_internal(std::string_view layer_name) const -> std::optional<std::reference_wrapper<const layer_type>> {
        auto it = std::ranges::find_if(m_vk_instance_layers, [layer_name](auto const& layer) { return layer_name == std::string_view(layer.first.layerName); });

        if (it == std::ranges::end(m_vk_instance_layers)) {
            return std::nullopt;
        }

        return *it;
    }
};

struct vulkan_stuff {
    static auto make() -> std::expected<vulkan_stuff, error> {
        auto&& ret = vulkan_stuff{};

        ret.m_dyna_loader = TRYX(dynamic_loader::make(get_vulkan_library_names()).transform_error([](auto err) { return error::make(err); }));
        ret.m_vk_functions.init(ret.m_dyna_loader.get_dl_symbol<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"));

        TRYX(ret.init_vk_instance());
        TRYX(ret.init_vk_debug_messenger());

        return ret;
    }

    vulkan_stuff() = default;

    ~vulkan_stuff() {
        /*if (m_vk_command_buffer != nullptr) {
            m_vk_device.freeCommandBuffers(m_vk_command_pool, 1, &m_vk_command_buffer, m_vk_dispatch);
        }

        if (m_vk_command_pool != nullptr) {
            m_vk_device.destroyCommandPool(m_vk_command_pool, nullptr, m_vk_dispatch);
        }*/

        if (m_vk_device != nullptr) {
            m_vk_functions.destroy_device(m_vk_device);
        }

        if (m_sdl_window != nullptr) {
            SDL_DestroyWindow(m_sdl_window);
        }

        if (m_vk_surface != nullptr) {
            m_vk_functions.destroy_surface_khr(m_vk_instance, m_vk_surface);
            m_vk_surface = nullptr;
        }

        if (m_vk_debug_messenger != nullptr) {
            m_vk_functions.destroy_debug_utils_messenger_ext(m_vk_instance, m_vk_debug_messenger);
            m_vk_debug_messenger = nullptr;
        }

        if (m_vk_instance != nullptr) {
            m_vk_functions.destroy_instance(m_vk_instance);
            m_vk_instance = nullptr;
        }
    }

    vulkan_stuff(vulkan_stuff&& other) noexcept
        : m_dyna_loader(std::move(other.m_dyna_loader))
        , m_vk_functions(std::move(other.m_vk_functions))
        , m_instance_extensions(std::move(other.m_instance_extensions))
        , m_device_extensions(std::move(other.m_device_extensions)) {
        using std::swap;

        swap(m_vk_instance, other.m_vk_instance);
        swap(m_vk_debug_messenger, other.m_vk_debug_messenger);

        swap(m_window_size, other.m_window_size);
        swap(m_sdl_window, other.m_sdl_window);
        swap(m_vk_surface, other.m_vk_surface);
    }

    auto run() -> std::expected<void, error> { return {}; }

private:
    VkApplicationInfo m_app_info{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = "vulkan test",
      .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
      .pEngineName = "voxels",
      .engineVersion = VK_MAKE_VERSION(0, 0, 1),
      .apiVersion = VK_API_VERSION_1_1,
    };

    dynamic_loader m_dyna_loader;
    vulkan_functions m_vk_functions;

    layer_extension_store<> m_instance_extensions;
    layer_extension_store<> m_device_extensions;

    VkInstance m_vk_instance = nullptr;
    VkDebugUtilsMessengerEXT m_vk_debug_messenger = nullptr;

    VkExtent2D m_window_size{640u, 360u};
    SDL_Window* m_sdl_window = nullptr;
    VkSurfaceKHR m_vk_surface = nullptr;

    VkPhysicalDevice m_vk_physical_device = nullptr;

    u32 m_vk_queue_family_index = 0;
    VkQueueFamilyProperties m_vk_queue_family = {};
    VkQueue m_vk_queue = nullptr;

    // VkQueueFamilyProperties m_vk_queue_family_graphics;
    // u32 m_vk_queue_family_index_graphics;
    // VkQueueFamilyProperties m_vk_queue_family_present;
    // u32 m_vk_queue_family_index_present;

    VkDevice m_vk_device = nullptr;
    VkCommandPool m_vk_command_pool = nullptr;
    VkCommandBuffer m_vk_command_buffer = nullptr;

    static auto get_desired_instance_layers() -> std::span<const std::pair<const char*, usize>> {
        return (const std::pair<const char*, usize>[]){
          {"VK_LAYER_KHRONOS_validation", 1},
        };
    }

    static auto get_desired_instance_extensions() -> std::span<const std::pair<const char*, usize>> {
        return (std::pair<const char*, usize> const[]){
          {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, 0},              //
          {VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, 1},  //
          {"VK_KHR_surface", 2},                               //
          {"VK_KHR_xlib_surface", 3},                          //
          {"VK_KHR_xcb_surface", 3},                           //
          {"VK_KHR_wayland_surface", 3},                       //
        };
    }

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

    auto init_vk_instance() -> std::expected<void, error> {
        m_instance_extensions = TRYX(layer_extension_store<>::make(m_vk_functions.enumerate_instance_layer_properties(), m_vk_functions.enumerate_instance_extension_properties()));

        for (auto layers_range = m_instance_extensions.layers(); auto const& [layer_no, layer] : layers_range | std::views::enumerate) {
            spdlog::debug("Layer #{}", layer_no);
            vxl::log(layer, 1);

            for (auto extensions_range = *m_instance_extensions.extensions(layer.layerName); auto const& [extension_no, extension] : extensions_range | std::views::enumerate) {
                spdlog::debug("\tExtension #{}", extension_no);
                vxl::log(extension, 2);
            }
        }

        for (auto extensions_range = m_instance_extensions.extensions(); auto const& [extension_no, extension] : extensions_range | std::views::enumerate) {
            spdlog::debug("Extension #{}", extension_no);
            vxl::log(extension, 2);
        }

        auto desired_layers = get_desired_instance_layers();
        auto available_layers = desired_layers | std::views::filter([this](auto const& ext) { return (bool)m_instance_extensions.layer(ext.first); });
        auto layers_to_enable = TRYX(m_instance_extensions.pick_stuff(desired_layers, available_layers, "layer").transform_error([](auto) {
            return error::make("could not enable all required instance layers");
        }));

        auto desired_exts = get_desired_instance_extensions();
        auto available_exts = desired_exts | std::views::filter([this](auto const& ext) { return (bool)m_instance_extensions.extension(ext.first); });
        auto exts_to_enable = TRYX(m_instance_extensions.pick_stuff(desired_exts, available_exts, "extension").transform_error([](auto) {
            return error::make("could not enable all required instance extensions");
        }));

        const auto instance_create_info = VkInstanceCreateInfo{
          .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
          .pNext = nullptr,
          .flags = {},
          .pApplicationInfo = &m_app_info,
          .enabledLayerCount = static_cast<u32>(layers_to_enable.size()),
          .ppEnabledLayerNames = layers_to_enable.data(),
          .enabledExtensionCount = static_cast<u32>(exts_to_enable.size()),
          .ppEnabledExtensionNames = exts_to_enable.data(),
        };
        m_vk_instance = TRYX(error::from_vk(m_vk_functions.create_instance(&instance_create_info), "could not create a vk::Instance"));

        m_vk_functions.init(m_vk_instance);

        return {};
    }

    auto init_vk_debug_messenger() -> std::expected<void, error> {
        const auto debug_utils_messenger_create_info = VkDebugUtilsMessengerCreateInfoEXT{
          .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
          .pNext = nullptr,
          .flags = 0,
          .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
          .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,
          .pfnUserCallback = &vulkan_debug_messenger,
          .pUserData = nullptr,
        };

        m_vk_debug_messenger =
          TRYX(error::from_vk(m_vk_functions.create_debug_utils_messenger_ext(m_vk_instance, &debug_utils_messenger_create_info), "could not create a VkDebugUtilsMessengerEXT"));

        return {};
    }

    auto init_sdl_window() -> std::expected<void, error> {
        m_sdl_window = SDL_CreateWindow(
          "vulkan test",                                                                  //
          static_cast<int>(m_window_size.width), static_cast<int>(m_window_size.height),  //
          static_cast<SDL_WindowFlags>(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE)
        );

        if (m_sdl_window == nullptr) {
            spdlog::error("failed to create an SDL window, error: {}", SDL_GetError());
            return std::unexpected{error::make("failed to create an SDL window")};
        }

        return {};
    }

    auto init_vk_surface() -> std::expected<void, error> {
        const auto res = SDL_Vulkan_CreateSurface(m_sdl_window, m_vk_instance, &m_vk_surface);

        if (res == SDL_FALSE || m_vk_surface == nullptr) {
            m_vk_surface = nullptr;
            spdlog::error("failed to create a vulkan surface through SDL, error: {}", SDL_GetError());
            return std::unexpected{error::make("failed to create a vulkan surface through SDL")};
        }

        return {};
    }

    auto try_init_device(VkPhysicalDevice const& physical_device, usize queue_family_index) const -> std::expected<VkDevice, error> {
        const auto device_extensions = TRYX(layer_extension_store<>::make(
          [this, &physical_device](u32* out_count, VkLayerProperties* out_layers) {
              return m_vk_functions.enumerate_device_layer_properties(physical_device, out_count, out_layers);
          },
          [this, &physical_device](const char* name, u32* out_count, VkExtensionProperties* out_extensions) {
              return m_vk_functions.enumerate_device_extension_properties(physical_device, name, out_count, out_extensions);
          }
        ));

        for (auto layers_range = device_extensions.layers(); auto const& [layer_no, layer] : layers_range | std::views::enumerate) {
            spdlog::debug("Device Layer #{}", layer_no);
            vxl::log(layer, 1);

            for (auto extensions_range = *device_extensions.extensions(layer.layerName); auto const& [extension_no, extension] : extensions_range | std::views::enumerate) {
                spdlog::debug("\tExtension #{}", extension_no);
                vxl::log(extension, 2);
            }
        }

        for (auto extensions_range = device_extensions.extensions(); auto const& [extension_no, extension] : extensions_range | std::views::enumerate) {
            spdlog::debug("Device Extension #{}", extension_no);
            vxl::log(extension, 2);
        }

        auto desired_layers = get_desired_device_layers();
        auto available_layers = desired_layers | std::views::filter([&device_extensions](auto const& ext) { return (bool)device_extensions.layer(ext.first); });
        auto layers_to_enable = TRYX(m_instance_extensions.pick_stuff(desired_layers, available_layers, "layer").transform_error([](auto) {
            return error::make("could not enable all required device layers");
        }));

        auto desired_exts = get_desired_device_extensions();
        auto available_exts = desired_exts | std::views::filter([&device_extensions](auto const& ext) { return (bool)device_extensions.extension(ext.first); });
        auto exts_to_enable = TRYX(m_instance_extensions.pick_stuff(desired_exts, available_exts, "extension").transform_error([](auto) {
            return error::make("could not enable all required device extensions");
        }));

        const float queue_priority = 0.f;
        const auto queue_create_info = VkDeviceQueueCreateInfo{
          .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .queueFamilyIndex = static_cast<u32>(queue_family_index),
          .queueCount = 1,
          .pQueuePriorities = &queue_priority,
        };

        const auto device_create_info = VkDeviceCreateInfo{
          .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .queueCreateInfoCount = 1,
          .pQueueCreateInfos = &queue_create_info,
          .enabledLayerCount = static_cast<u32>(layers_to_enable.size()),
          .ppEnabledLayerNames = layers_to_enable.data(),
          .enabledExtensionCount = static_cast<u32>(exts_to_enable.size()),
          .ppEnabledExtensionNames = exts_to_enable.data(),
          .pEnabledFeatures = nullptr,
        };

        auto&& device = TRYX(error::from_vk(m_vk_functions.create_device(physical_device, &device_create_info), "could not create a vk::Device"));

        return device;
    }

    auto init_vk_devices_and_queues() -> std::expected<void, error> {
        const auto physical_devices = TRYX(error::from_vk(
          get_vector([this](u32* out_count, VkPhysicalDevice* out_devices) { return m_vk_functions.enumerate_physical_devices(m_vk_instance, out_count, out_devices); }),
          "could not enumerate physical devices"
        ));

        for (auto const& [device_no, device] : physical_devices | std::views::enumerate) {
            const auto properties = m_vk_functions.get_physical_device_properties(device);
            spdlog::debug("Device #{}:", device_no);
            vxl::log(properties, 1);

            const auto queue_family_properties = get_vector([this, &device](u32* out_count, VkQueueFamilyProperties* out_properties) {
                m_vk_functions.get_physical_device_queue_family_properties(device, out_count, out_properties);
            });

            for (auto const& [family_no, family] : queue_family_properties | std::views::enumerate) {
                spdlog::debug("\tQueue family #{}", family_no);
                vxl::log(family, 2);
                auto can_present = TRYX(error::from_vk(m_vk_functions.get_physical_device_surface_support_khr(device, family_no, m_vk_surface)));
                spdlog::debug("\t\tCan present?    : {}", can_present == VK_TRUE ? "Yes" : "No");
            }
        }

        m_vk_physical_device = physical_devices[0];

        spdlog::info("a physical device was chosen:");
        vxl::log(m_vk_functions.get_physical_device_properties(m_vk_physical_device), 1, spdlog::level::info);

        m_vk_queue_family_index = 0;
        m_vk_queue_family = get_vector([this](u32* out_count, VkQueueFamilyProperties* out_properties) {
            m_vk_functions.get_physical_device_queue_family_properties(m_vk_physical_device, out_count, out_properties);
        })[m_vk_queue_family_index];

        m_vk_device = TRYX(try_init_device(m_vk_physical_device, m_vk_queue_family_index));

        m_vk_queue = m_vk_functions.get_device_queue(m_vk_device, m_vk_queue_family_index, 0);

        return {};
    }
};

}  // namespace vxl::vk

auto main() -> int {
    spdlog::set_level(spdlog::level::debug);

    spdlog::debug("{}", vxl::vk::error::make("this is a test error"));

    if (auto init_res = SDL_Init(SDL_INIT_VIDEO); init_res != 0) {
        spdlog::error("SDL initialisation failed, error: {}", SDL_GetError());
        return 1;
    }

    auto vulkan_stuff = ({
        auto&& res = vxl::vk::vulkan_stuff::make();

        if (!res) {
            spdlog::error("failed to initialize graphics: {}", res.error());
            return 1;
        }

        std::move(*res);
    });

    auto&& res = vulkan_stuff.run();

    if (!res) {
        spdlog::error("program failed with error: {}", res.error());
        return 1;
    }
}
