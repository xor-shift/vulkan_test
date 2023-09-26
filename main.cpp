#include <log.hpp>
#include <vulkan_functions.hpp>

#include <stuff/core.hpp>
#include <stuff/expected.hpp>
#include <stuff/scope.hpp>

#include <SDL.h>
#include <SDL_vulkan.h>
#include <VkBootstrap.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_to_string.hpp>

#include <chrono>
#include <cmath>
#include <expected>
#include <iostream>
#include <ranges>

#define UNNAMED3(_name) const auto _unnamed_##_name
#define UNNAMED2(_name) UNNAMED3(_name)
#define UNNAMED UNNAMED2(__COUNTER__)

struct error {
    std::string m_description;
    std::source_location m_location;
};

template<typename T>
constexpr auto vk_res_to_expected(vk::ResultValue<T>&& val) -> std::expected<T, vk::Result> {
    auto&& [result, value] = std::move(val);

    if (result == vk::Result::eSuccess) {
        return std::move(value);
    }

    return std::unexpected<vk::Result>(result);
}

#define TRYX_VK(_expr) TRYX(vk_res_to_expected(std::move((_expr))))

struct vulkan_stuff {
    static auto make() -> std::expected<vulkan_stuff, std::string_view> {
        auto&& ret = vulkan_stuff{};

        ret.m_dyna_loader = TRYX(dynamic_loader::make(get_vulkan_library_names()));
        ret.m_vk_dispatch.init(ret.m_dyna_loader.get_dl_symbol<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"));

        TRYX(ret.fill_in_vk_layers_and_extensions());

        for (auto layers_range = ret.layers(); auto const& [layer_no, layer] : layers_range | std::views::enumerate) {
            spdlog::debug("Layer #{}", layer_no);
            vxl::log(layer, 1);

            for (auto extensions_range = *ret.extensions(layer.layerName); auto const& [extension_no, extension] : extensions_range | std::views::enumerate) {
                spdlog::debug("\tExtension #{}", extension_no);
                vxl::log(extension, 2);
            }
        }

        for (auto extensions_range = ret.extensions(); auto const& [extension_no, extension] : extensions_range | std::views::enumerate) {
            spdlog::debug("Extension #{}", extension_no);
            vxl::log(extension, 2);
        }

        TRYX(ret.init_vk_instance());
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

    vulkan_stuff(vulkan_stuff&& other)
        : m_app_info(other.m_app_info)
        , m_vk_instance_layers(std::move(other.m_vk_instance_layers))
        , m_vk_instance_extensions(std::move(other.m_vk_instance_extensions))
        , m_dyna_loader(std::move(other.m_dyna_loader))
        , m_vk_dispatch(other.m_vk_dispatch)
        , m_vk_instance(other.m_vk_instance)
        , m_vk_debug_messenger(other.m_vk_debug_messenger)
        , m_window_size(other.m_window_size)
        , m_sdl_window(other.m_sdl_window)
        , m_vk_surface(other.m_vk_surface)
        , m_vk_physical_device(other.m_vk_physical_device)
        , m_vk_queue_family_index(other.m_vk_queue_family_index)
        , m_vk_queue_family(other.m_vk_queue_family)
        , m_vk_queue(other.m_vk_queue)
        , m_vk_device(other.m_vk_device)
        , m_vk_command_pool(other.m_vk_command_pool)
        , m_vk_command_buffer(other.m_vk_command_buffer)
        , m_vk_swapchain(other.m_vk_swapchain)
        , m_vk_swapchain_format(other.m_vk_swapchain_format)
        , m_vk_swapchain_images(std::move(other.m_vk_swapchain_images))
        , m_vk_swapchain_image_views(std::move(other.m_vk_swapchain_image_views)) {
        other.m_vk_instance = nullptr;
        other.m_vk_debug_messenger = nullptr;

        other.m_sdl_window = nullptr;
        other.m_vk_surface = nullptr;

        other.m_vk_physical_device = nullptr;

        other.m_vk_queue_family_index = 0;
        other.m_vk_queue_family = vk::QueueFamilyProperties{};
        other.m_vk_queue = nullptr;

        other.m_vk_device = nullptr;
        other.m_vk_command_pool = nullptr;
        other.m_vk_command_buffer = nullptr;

        other.m_vk_swapchain = nullptr;
    }

    ~vulkan_stuff() {
        if ((bool)m_vk_swapchain) {
            std::ranges::for_each(m_vk_swapchain_image_views, [this](auto const& view) { m_vk_device.destroy(view); });

            m_vk_device.destroy(m_vk_swapchain, nullptr, m_vk_dispatch);
        }

        if ((bool)m_vk_command_buffer) {
            m_vk_device.freeCommandBuffers(m_vk_command_pool, 1, &m_vk_command_buffer, m_vk_dispatch);
        }

        if ((bool)m_vk_command_pool) {
            m_vk_device.destroyCommandPool(m_vk_command_pool, nullptr, m_vk_dispatch);
        }

        if ((bool)m_vk_device) {
            m_vk_device.destroy(nullptr, m_vk_dispatch);
        }

        if (m_sdl_window != nullptr) {
            SDL_DestroyWindow(m_sdl_window);
        }

        if ((bool)m_vk_surface) {
            m_vk_instance.destroy(m_vk_surface, nullptr, m_vk_dispatch);
        }

        if ((bool)m_vk_debug_messenger) {
            m_vk_instance.destroy(m_vk_debug_messenger, nullptr, m_vk_dispatch);
        }

        if ((bool)m_vk_instance) {
            m_vk_instance.destroy(nullptr, m_vk_dispatch);
        }
    }

    void run() {
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

        auto render_pass = ({
            auto&& [result, value] = m_vk_device.createRenderPass(render_pass_create_info, nullptr, m_vk_dispatch);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not create a render pass, error code: {}", vk::to_string(result));
                return;
            }

            value;
        });
        UNNAMED = stf::scope_exit{[this, &render_pass] { m_vk_device.destroy(render_pass, nullptr, m_vk_dispatch); }};

        auto framebuffers = std::vector<vk::Framebuffer>{m_vk_swapchain_image_views.size()};
        UNNAMED = stf::scope_exit{[this, &framebuffers] {
            for (auto const& framebuffer : framebuffers) {
                if (!framebuffer) {
                    continue;
                }

                m_vk_device.destroy(framebuffer, nullptr, m_vk_dispatch);
            }
        }};

        for (auto const& [no, view] : m_vk_swapchain_image_views | std::views::enumerate) {
            auto framebuffer_create_info = vk::FramebufferCreateInfo(
              {},                                         //
              render_pass,                                //
              1, &view,                                   //
              m_window_size.width, m_window_size.height,  //
              1                                           //
            );

            framebuffers[no] = ({
                auto&& [result, value] = m_vk_device.createFramebuffer(framebuffer_create_info, nullptr, m_vk_dispatch);

                if (result != vk::Result::eSuccess) {
                    spdlog::error("could not create a framebuffer (index {}), error code: {}", no, vk::to_string(result));
                    return;
                }

                value;
            });
        }

        auto fence_create_info = vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled);
        auto render_fence = ({
            auto&& [result, value] = m_vk_device.createFence(fence_create_info, nullptr, m_vk_dispatch);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not create a fence (for renders), error code: {}", vk::to_string(result));
                return;
            }

            value;
        });
        UNNAMED = stf::scope_exit{[this, &render_fence] { m_vk_device.destroy(render_fence, nullptr, m_vk_dispatch); }};

        auto present_semaphore = ({
            auto&& [result, value] = m_vk_device.createSemaphore(vk::SemaphoreCreateInfo{}, nullptr, m_vk_dispatch);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not create a semaphore (for present), error code: {}", vk::to_string(result));
                return;
            }

            value;
        });
        UNNAMED = stf::scope_exit{[this, &present_semaphore] { m_vk_device.destroy(present_semaphore, nullptr, m_vk_dispatch); }};

        auto render_semaphore = ({
            auto&& [result, value] = m_vk_device.createSemaphore(vk::SemaphoreCreateInfo{}, nullptr, m_vk_dispatch);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not create a semaphore (for render), error code: {}", vk::to_string(result));
                return;
            }

            value;
        });
        UNNAMED = stf::scope_exit{[this, &render_semaphore] { m_vk_device.destroy(render_semaphore, nullptr, m_vk_dispatch); }};

        static constexpr auto a_second = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
        static constexpr auto color_period = 120.f;

        auto frame_number = 0u;
        for (auto running = true; running;) {
            for (SDL_Event event; SDL_PollEvent(&event) != 0;) {
                if (event.type == SDL_EVENT_QUIT) {
                    running = false;
                }
            }
            std::ignore = m_vk_device.waitForFences(1, &render_fence, VK_TRUE, a_second, m_vk_dispatch);
            std::ignore = m_vk_device.resetFences(1, &render_fence, m_vk_dispatch);

            auto swapchain_image_index = ({
                auto&& [result, value] = m_vk_device.acquireNextImageKHR(m_vk_swapchain, a_second, present_semaphore, nullptr, m_vk_dispatch);

                if (result != vk::Result::eSuccess) {
                    spdlog::error("could not acquire the next image in the swapchain, error code: {}", vk::to_string(result));
                    return;
                }

                value;
            });

            m_vk_command_buffer.reset({}, m_vk_dispatch);

            const auto command_begin_info = vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
            std::ignore = m_vk_command_buffer.begin(command_begin_info);

            const auto flash = std::abs(std::sin(static_cast<float>(frame_number) / color_period));
            const auto clear_value = vk::ClearValue((std::array<float, 4>){0.f, 0.f, flash, 1.f});

            const auto render_pass_info = vk::RenderPassBeginInfo(
              render_pass,                                                                          //
              framebuffers[swapchain_image_index],                                                  //
              vk::Rect2D(vk::Offset2D(), vk::Extent2D(m_window_size.width, m_window_size.height)),  //
              1, &clear_value                                                                       //
            );

            m_vk_command_buffer.beginRenderPass(render_pass_info, vk::SubpassContents::eInline, m_vk_dispatch);
            m_vk_command_buffer.endRenderPass(m_vk_dispatch);
            m_vk_command_buffer.end(m_vk_dispatch);

            const auto wait_dst_stage_flags = vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
            const auto submit_info = vk::SubmitInfo(
              1, &present_semaphore,    //
              &wait_dst_stage_flags,    //
              1, &m_vk_command_buffer,  //
              1, &render_semaphore      //
            );

            m_vk_queue.submit(1, &submit_info, render_fence, m_vk_dispatch);

            const auto present_info = vk::PresentInfoKHR(
              1, &render_semaphore,                        //
              1, &m_vk_swapchain, &swapchain_image_index,  //
              nullptr                                      //
            );

            m_vk_queue.presentKHR(present_info, m_vk_dispatch);

            frame_number++;
        }

        std::ignore = m_vk_device.waitForFences(1, &render_fence, VK_TRUE, a_second, m_vk_dispatch);
        std::ignore = m_vk_device.resetFences(1, &render_fence, m_vk_dispatch);
    }

private:
    vk::ApplicationInfo m_app_info{"vulkan test", VK_MAKE_VERSION(0, 0, 1), "voxels", VK_MAKE_VERSION(0, 0, 1), VK_API_VERSION_1_1};

    using layer_type = std::pair<vk::LayerProperties, std::vector<vk::ExtensionProperties>>;
    std::vector<layer_type> m_vk_instance_layers{};
    std::vector<vk::ExtensionProperties> m_vk_instance_extensions{};

    dynamic_loader m_dyna_loader;
    vk::DispatchLoaderDynamic m_vk_dispatch;

    vk::Instance m_vk_instance;
    vk::DebugUtilsMessengerEXT m_vk_debug_messenger;

    vk::Extent2D m_window_size{640u, 360u};
    SDL_Window* m_sdl_window = nullptr;
    vk::SurfaceKHR m_vk_surface;

    vk::PhysicalDevice m_vk_physical_device;

    u32 m_vk_queue_family_index;
    vk::QueueFamilyProperties m_vk_queue_family;
    vk::Queue m_vk_queue;

    // vk::QueueFamilyProperties m_vk_queue_family_graphics;
    // u32 m_vk_queue_family_index_graphics;
    // vk::QueueFamilyProperties m_vk_queue_family_present;
    // u32 m_vk_queue_family_index_present;

    vk::Device m_vk_device;
    vk::CommandPool m_vk_command_pool;
    vk::CommandBuffer m_vk_command_buffer;

    vk::SwapchainKHR m_vk_swapchain;
    vk::Format m_vk_swapchain_format;
    std::vector<vk::Image> m_vk_swapchain_images;
    std::vector<vk::ImageView> m_vk_swapchain_image_views;

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

    static constexpr auto pick_stuff(auto&& desired, auto&& available, std::string_view name) -> std::expected<std::vector<const char*>, std::string_view> {
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

            return std::unexpected{"THIS ERROR MUST HAVE BEEN REPLACED"};
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

    auto fill_in_vk_layers_and_extensions() -> std::expected<void, std::string_view> {
        auto layers = ({
            auto&& [result, value] = vk::enumerateInstanceLayerProperties(m_vk_dispatch);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not enumerate instance layer properties, error code: {}", vk::to_string(result));
                return std::unexpected{"could not enumerate instance layer properties"};
            }

            value;
        });

        m_vk_instance_layers.reserve(layers.size());
        for (auto const& layer_props : layers) {
            auto layer_extensions = ({
                // this is such a bad use of std::string
                auto&& [result, value] = vk::enumerateInstanceExtensionProperties(std::string(layer_props.layerName.data()), m_vk_dispatch);

                if (result != vk::Result::eSuccess) {
                    spdlog::error(
                      "could not enumerate instance extensions for layer \"{}\", error code: {}", std::string_view(layer_props.layerName.data()), vk::to_string(result)
                    );
                    return std::unexpected{"could not enumerate instance extensions for a layer"};
                }

                value;
            });

            m_vk_instance_layers.emplace_back(layer_props, layer_extensions);
        }

        m_vk_instance_extensions = ({
            auto&& [result, value] = vk::enumerateInstanceExtensionProperties(nullptr, m_vk_dispatch);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not enumerate instance layer properties, error code: {}", vk::to_string(result));
                return std::unexpected{"could not enumerate instance layer properties"};
            }

            value;
        });

        return {};
    }

    constexpr auto layer_internal(std::string_view layer_name) const -> std::optional<std::reference_wrapper<const layer_type>> {
        auto it = std::ranges::find_if(m_vk_instance_layers, [layer_name](auto const& layer) { return layer_name == std::string_view(layer.first.layerName); });

        if (it == std::ranges::end(m_vk_instance_layers)) {
            return std::nullopt;
        }

        return *it;
    }

    constexpr auto layer(std::string_view layer_name) const -> std::optional<VkLayerProperties> {
        return layer_internal(layer_name).transform([](auto const& layer) { return layer.get().first; });
    }

    constexpr auto extension(std::string_view extension_name) const -> std::optional<VkExtensionProperties> {
        auto iter = std::ranges::find_if(m_vk_instance_extensions, [extension_name](auto const& extension) { return extension_name == std::string_view(extension.extensionName); });

        if (iter == std::ranges::end(m_vk_instance_extensions)) {
            return std::nullopt;
        }

        return *iter;
    }

    constexpr auto extension(std::string_view extension_name, std::string_view layer_name) const -> std::optional<VkExtensionProperties> {
        return layer_internal(layer_name).and_then([extension_name](auto const& layer) -> std::optional<VkExtensionProperties> {
            auto iter = std::ranges::find_if(layer.get().second, [extension_name](auto const& layer) { return extension_name == std::string_view(layer.extensionName); });

            if (iter == std::ranges::end(layer.get().second)) {
                return std::nullopt;
            }

            return *iter;
        });
    }

    constexpr auto layers() const -> std::ranges::keys_view<std::vector<layer_type> const&> { return m_vk_instance_layers | std::views::keys; }

    constexpr auto extensions() const -> std::ranges::ref_view<const std::vector<vk::ExtensionProperties>> { return m_vk_instance_extensions | std::views::all; }

    constexpr auto extensions(std::string_view layer_name) const -> std::optional<std::ranges::ref_view<const std::vector<vk::ExtensionProperties>>> {
        return layer_internal(layer_name)  //
          .transform([](auto const& layer) { return layer.get().second | std::views::all; });
        //.value_or([] { return std::ranges::empty_view<VkExtensionProperties>{}; });
    }

    auto init_vk_instance() -> std::expected<void, std::string_view> {
        auto desired_layers = get_desired_instance_layers();
        auto available_layers = desired_layers | std::views::filter([this](auto const& ext) { return (bool)layer(ext.first); });
        auto layers_to_enable = TRYX(pick_stuff(desired_layers, available_layers, "layer").transform_error([](auto) { return "could not enable all required instance layers"; }));

        auto desired_exts = get_desired_instance_extensions();
        auto available_exts = desired_exts | std::views::filter([this](auto const& ext) { return (bool)extension(ext.first); });
        auto exts_to_enable = TRYX(pick_stuff(desired_exts, available_exts, "extension").transform_error([](auto) { return "could not enable all required instance extensions"; }));

        const auto instance_create_info = vk::InstanceCreateInfo({}, &m_app_info, layers_to_enable, exts_to_enable, nullptr);
        m_vk_instance = ({
            auto&& [result, value] = vk::createInstance(instance_create_info, nullptr, m_vk_dispatch);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not create a vk::Instance, error code: {}", vk::to_string(result));
                return std::unexpected{"could not create a vk::Instance"};
            }

            value;
        });

        m_vk_dispatch.init(m_vk_instance);

        return {};
    }

    auto init_vk_debug_messenger() -> std::expected<void, std::string_view> {
        const auto debug_utils_messenger_create_info = vk::DebugUtilsMessengerCreateInfoEXT(
          {},                                                                                                                                                       //
          vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,  //
          vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,  //
          vxl::vulkan_debug_messenger                                                                                                                               //
        );

        m_vk_debug_messenger = ({
            auto&& [result, value] = m_vk_instance.createDebugUtilsMessengerEXT(debug_utils_messenger_create_info, nullptr, m_vk_dispatch);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not create a vk::DebugUtilsMessengerEXT, error code: {}", vk::to_string(result));
                return std::unexpected{"could not create a vk::DebugUtilsMessengerEXT"};
            }

            value;
        });

        return {};
    }

    auto init_sdl_window() -> std::expected<void, std::string_view> {
        m_sdl_window = SDL_CreateWindow(
          "vulkan test",                                                                  //
          static_cast<int>(m_window_size.width), static_cast<int>(m_window_size.height),  //
          static_cast<SDL_WindowFlags>(SDL_WINDOW_VULKAN)
        );

        if (m_sdl_window == nullptr) {
            spdlog::error("failed to create an SDL window, error: {}", SDL_GetError());
            return std::unexpected{"failed to create an SDL window"};
        }

        return {};
    }

    auto init_vk_surface() -> std::expected<void, std::string_view> {
        m_vk_surface = ({
            auto* surface_tmp = VkSurfaceKHR{};
            SDL_Vulkan_CreateSurface(m_sdl_window, m_vk_instance, &surface_tmp);

            if (surface_tmp == nullptr) {
                spdlog::error("failed to create a vulkan surface through SDL, error: {}", SDL_GetError());
                return std::unexpected{"failed to create a vulkan surface through SDL"};
            }

            auto&& vk_surface_tmp = vk::SurfaceKHR(surface_tmp);
            vk_surface_tmp;
        });

        return {};
    }

    auto try_init_device(vk::PhysicalDevice const& physical_device, usize queue_family_index) const -> std::expected<vk::Device, std::string_view> {
        auto device_layers = ({
            auto&& [result, value] = physical_device.enumerateDeviceLayerProperties(m_vk_dispatch);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not enumerate physical device layers, error code: {}", vk::to_string(result));
                return std::unexpected{"could not enumerate physical device layers"};
            }

            value;
        });

        auto device_extensions = ({
            auto&& [result, value] = physical_device.enumerateDeviceExtensionProperties(nullptr, m_vk_dispatch);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not enumerate physical device extensions, error code: {}", vk::to_string(result));
                return std::unexpected{"could not enumerate physical device extensions"};
            }

            value;
        });

        auto filterer = [](auto& range_to_check, auto member) {
            return std::views::filter([&range_to_check, member](auto const& to_check) {
                return std::ranges::find_if(range_to_check, [&to_check, member](auto const& candidate) {
                           return std::strcmp(to_check.first, (candidate.*member).data()) == 0;  //
                       }) != std::ranges::end(range_to_check);
            });
        };

        auto desired_layers = get_desired_device_layers();
        auto available_layers = desired_layers | filterer(device_layers, &vk::LayerProperties::layerName);
        auto layers_to_enable =
          TRYX(pick_stuff(desired_layers, available_layers, "device layer").transform_error([](auto) { return "could not enable all required device layers"; }));

        auto desired_extensions = get_desired_device_extensions();
        auto available_extensions = desired_extensions | filterer(device_extensions, &vk::ExtensionProperties::extensionName);
        auto extensions_to_enable =
          TRYX(pick_stuff(desired_extensions, available_extensions, "device extension").transform_error([](auto) { return "could not enable all required device extensions"; }));

        const float queue_priorities[] = {0.f};
        const auto queue_create_info = vk::DeviceQueueCreateInfo({}, static_cast<u32>(queue_family_index), queue_priorities);
        const auto device_create_info = vk::DeviceCreateInfo({}, queue_create_info, layers_to_enable, extensions_to_enable);
        auto&& device = ({
            auto&& [result, value] = physical_device.createDevice(device_create_info, nullptr, m_vk_dispatch);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not create a vk::Device, error code: {}", vk::to_string(result));
                return std::unexpected{"could not create a vk::Device"};
            }

            value;
        });

        return device;
    }

    auto init_vk_devices_and_queues() -> std::expected<void, std::string_view> {
        const auto physical_devices = ({
            auto&& [result, value] = m_vk_instance.enumeratePhysicalDevices(m_vk_dispatch);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not enumerate physical devices, error code: {}", vk::to_string(result));
                return std::unexpected{"could not enumerate physical devices"};
            }

            value;
        });

        for (auto const& [device_no, device] : physical_devices | std::views::enumerate) {
            const auto properties = device.getProperties(m_vk_dispatch);
            spdlog::debug("Device #{}:", device_no);
            vxl::log(properties, 1);

            const auto queue_family_properties = device.getQueueFamilyProperties(m_vk_dispatch);
            for (auto const& [family_no, family] : queue_family_properties | std::views::enumerate) {
                spdlog::debug("\tQueue family #{}", family_no);
                vxl::log(family, 2);
                spdlog::debug("\t\tCan present?    : {}", device.getSurfaceSupportKHR(family_no, m_vk_surface).value == VK_TRUE ? "Yes" : "No");
            }
        }

        m_vk_physical_device = physical_devices[0];

        spdlog::info("a physical device was chosen:");
        vxl::log(m_vk_physical_device.getProperties(), 1, spdlog::level::info);

        m_vk_queue_family_index = 0;
        m_vk_queue_family = m_vk_physical_device.getQueueFamilyProperties(m_vk_dispatch)[m_vk_queue_family_index];

        m_vk_device = TRYX(try_init_device(m_vk_physical_device, m_vk_queue_family_index));

        m_vk_queue = m_vk_device.getQueue(m_vk_queue_family_index, 0);

        return {};
    }

    auto init_vk_command_pool() -> std::expected<void, std::string_view> {
        const auto command_pool_create_info = vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, m_vk_queue_family_index);
        m_vk_command_pool = ({
            auto&& [result, value] = m_vk_device.createCommandPool(command_pool_create_info, nullptr, m_vk_dispatch);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not create a vk::CommandPool, error code: {}", vk::to_string(result));
                return std::unexpected{"could not create a vk::CommandPool"};
            }

            value;
        });

        return {};
    }

    auto init_vk_command_buffer() -> std::expected<void, std::string_view> {
        const auto command_buffer_allocate_info = vk::CommandBufferAllocateInfo(m_vk_command_pool, vk::CommandBufferLevel::ePrimary, 1);
        const auto command_buffers = ({
            auto&& [result, value] = m_vk_device.allocateCommandBuffers(command_buffer_allocate_info, m_vk_dispatch);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not create a vector of vk::CommandBuffer, error code: {}", vk::to_string(result));
                return std::unexpected{"could not create a vector of vk::CommandBuffer"};
            }

            value;
        });

        if (command_buffers.empty()) {
            spdlog::error("zero command buffers were created despite success?");
            return std::unexpected{"zero command buffers were created despite success"};
        }

        m_vk_command_buffer = command_buffers[0];

        return {};
    }

    auto reinit_vk_swapchain(vk::Extent2D extent) -> std::expected<void, std::string_view> {
        const auto surface_capabilities = ({
            auto&& [result, value] = m_vk_physical_device.getSurfaceCapabilitiesKHR(m_vk_surface);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not fetch surface capabilities from the physical device, error code: {}", vk::to_string(result));
                return std::unexpected{"could not fetch surface capabilities from the physical device"};
            }

            value;
        });

        const auto surface_formats = ({
            auto&& [result, value] = m_vk_physical_device.getSurfaceFormatsKHR(m_vk_surface);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not fetch surface formats from the physical device, error code: {}", vk::to_string(result));
                return std::unexpected{"could not fetch surface formats from the physical device"};
            }

            value;
        });

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
          nullptr
        );

        m_vk_swapchain = ({
            auto&& [result, value] = m_vk_device.createSwapchainKHR(swapchain_create_info);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not create a swapchain, error code: {}", vk::to_string(result));
                return std::unexpected{"could not create a swapchain"};
            }

            value;
        });

        m_vk_swapchain_images = ({
            auto&& [result, value] = m_vk_device.getSwapchainImagesKHR(m_vk_swapchain);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not get swapchain images, error code: {}", vk::to_string(result));
                return std::unexpected{"could not get swapchain images"};
            }

            value;
        });

        m_vk_swapchain_images = ({
            auto&& [result, value] = m_vk_device.getSwapchainImagesKHR(m_vk_swapchain);

            if (result != vk::Result::eSuccess) {
                spdlog::error("could not get swapchain images, error code: {}", vk::to_string(result));
                return std::unexpected{"could not get swapchain images"};
            }

            value;
        });

        m_vk_swapchain_image_views = std::vector<vk::ImageView>{};
        m_vk_swapchain_image_views.reserve(m_vk_swapchain_images.size());

        vk::ImageViewCreateInfo imageViewCreateInfo({}, {}, vk::ImageViewType::e2D, surface_format, {}, {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
        for (auto const& [no, image] : m_vk_swapchain_images | std::views::enumerate) {
            imageViewCreateInfo.image = image;
            auto&& image_view = ({
                auto&& [result, value] = m_vk_device.createImageView(imageViewCreateInfo);

                if (result != vk::Result::eSuccess) {
                    spdlog::error("could not create a swapchain image view, index {}, error code: {}", no, vk::to_string(result));
                    return std::unexpected{"could not create a swapchain image view"};
                }

                value;
            });

            m_vk_swapchain_image_views.emplace_back(std::move(image_view));
        }

        return {};
    }
};

auto main() -> int {
    spdlog::set_level(spdlog::level::debug);

    if (auto init_res = SDL_Init(SDL_INIT_VIDEO); init_res != 0) {
        spdlog::error("SDL initialisation failed, error: {}", SDL_GetError());
        return 1;
    }

    auto vulkan_stuff = ({
        auto&& res = vulkan_stuff::make();

        if (!res) {
            spdlog::error("failed to initialize graphics: {}", res.error());
            return 1;
        }

        std::move(*res);
    });

    vulkan_stuff.run();
}
