#include <vulkan_functions.hpp>

#include <stuff/core.hpp>
#include <stuff/expected.hpp>
#include <stuff/scope.hpp>

#include <SDL.h>
#include <SDL_vulkan.h>
#include <VkBootstrap.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>

#include <chrono>
#include <cmath>
#include <expected>
#include <iostream>
#include <ranges>

#define UNNAMED3(_name) const auto _unnamed_##_name
#define UNNAMED2(_name) UNNAMED3(_name)
#define UNNAMED UNNAMED2(__COUNTER__)

auto old_main() -> int {
    constexpr auto extent = VkExtent2D{600, 400};

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Failed to initialise SDL: " << SDL_GetError() << '\n';
        throw std::runtime_error("failed to initialize SDL");
    }

    UNNAMED = stf::scope_exit{[] { SDL_Quit(); }};

    auto* const window = SDL_CreateWindow(
      "vulkan test",                                                    //
      static_cast<int>(extent.width), static_cast<int>(extent.height),  //
      static_cast<SDL_WindowFlags>(SDL_WINDOW_VULKAN)
    );
    UNNAMED = stf::scope_exit{[window] { SDL_DestroyWindow(window); }};

    vkb::InstanceBuilder builder;
    auto inst_ret = builder
                      .set_app_name("vulkan test")      //
                      .request_validation_layers(true)  //
                      .require_api_version(1, 1, 0)     //
                      .use_default_debug_messenger()    //
                      .build();

    auto vkb_instance = inst_ret.value();

    auto* instance = vkb_instance.instance;
    auto* messenger = vkb_instance.debug_messenger;

    UNNAMED = stf::scope_exit{[instance, messenger] {
        vkb::destroy_debug_utils_messenger(instance, messenger);
        vkDestroyInstance(instance, nullptr);
    }};

    auto* surface = VkSurfaceKHR{};
    SDL_Vulkan_CreateSurface(window, instance, &surface);

    UNNAMED = stf::scope_exit{[instance, surface] { vkDestroySurfaceKHR(instance, surface, nullptr); }};

    auto selector = vkb::PhysicalDeviceSelector{vkb_instance};
    auto physical_device = selector
                             .set_minimum_version(1, 1)  //
                             .set_surface(surface)       //
                             .select()                   //
                             .value();

    auto device_builder = vkb::DeviceBuilder{physical_device};
    auto vkb_device = device_builder.build().value();

    auto* device = vkb_device.device;
    auto* chosen_gpu = physical_device.physical_device;

    UNNAMED = stf::scope_exit{[device] { vkDestroyDevice(device, nullptr); }};

    auto* graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
    auto graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

    vkb::SwapchainBuilder swapchainBuilder{chosen_gpu, device, surface};

    vkb::Swapchain vkbSwapchain = swapchainBuilder
                                    .use_default_format_selection()
                                    // use vsync present mode
                                    .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                                    .set_desired_extent(extent.width, extent.height)
                                    .build()
                                    .value();

    auto* swap_chain = vkbSwapchain.swapchain;
    auto swap_chain_images = vkbSwapchain.get_images().value();
    auto swap_chain_image_views = vkbSwapchain.get_image_views().value();

    UNNAMED = stf::scope_exit{[device, swap_chain] { vkDestroySwapchainKHR(device, swap_chain, nullptr); }};
    UNNAMED = stf::scope_exit{[device, &imgs = swap_chain_image_views] { std::ranges::for_each(imgs, [device](auto view) { vkDestroyImageView(device, view, nullptr); }); }};

    const auto swap_chain_image_format = vkbSwapchain.image_format;

    auto command_pool_info = VkCommandPoolCreateInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = graphics_queue_family,
    };

    auto* command_pool = VkCommandPool{};
    vkCreateCommandPool(device, &command_pool_info, nullptr, &command_pool);
    UNNAMED = stf::scope_exit{[device, command_pool] { vkDestroyCommandPool(device, command_pool, nullptr); }};

    auto command_alloc_info = VkCommandBufferAllocateInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = command_pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
    };

    auto* main_command_buffer = VkCommandBuffer{};
    vkAllocateCommandBuffers(device, &command_alloc_info, &main_command_buffer);

    auto color_attachment = VkAttachmentDescription{
      .format = swap_chain_image_format,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    auto color_attachment_ref = VkAttachmentReference{
      .attachment = 0,
      .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    auto subpass = VkSubpassDescription{
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .colorAttachmentCount = 1,
      .pColorAttachments = &color_attachment_ref,
    };

    auto render_pass_create_info = VkRenderPassCreateInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .attachmentCount = 1,
      .pAttachments = &color_attachment,
      .subpassCount = 1,
      .pSubpasses = &subpass,
    };

    auto* render_pass = VkRenderPass{};
    vkCreateRenderPass(device, &render_pass_create_info, nullptr, &render_pass);
    UNNAMED = stf::scope_exit{[device, render_pass] { vkDestroyRenderPass(device, render_pass, nullptr); }};

    VkFramebufferCreateInfo fb_info = {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .pNext = nullptr,
      .renderPass = render_pass,
      .attachmentCount = 1,
      .width = extent.width,
      .height = extent.height,
      .layers = 1,
    };

    const auto swapchain_imagecount = swap_chain_images.size();
    auto framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);
    UNNAMED = stf::scope_exit{[device, &framebuffers] { std::ranges::for_each(framebuffers, [device](auto buffer) { vkDestroyFramebuffer(device, buffer, nullptr); }); }};

    for (int i = 0; i < swapchain_imagecount; i++) {
        fb_info.pAttachments = &swap_chain_image_views[i];
        vkCreateFramebuffer(device, &fb_info, nullptr, &framebuffers[i]);
    }

    auto fenceCreateInfo = VkFenceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    auto* render_fence = VkFence{};
    vkCreateFence(device, &fenceCreateInfo, nullptr, &render_fence);
    UNNAMED = stf::scope_exit{[device, render_fence] { vkDestroyFence(device, render_fence, nullptr); }};

    auto semaphoreCreateInfo = VkSemaphoreCreateInfo{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
    };

    auto* present_semaphore = VkSemaphore{};
    vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &present_semaphore);
    UNNAMED = stf::scope_exit{[device, present_semaphore] { vkDestroySemaphore(device, present_semaphore, nullptr); }};

    auto* render_semaphore = VkSemaphore{};
    vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &render_semaphore);
    UNNAMED = stf::scope_exit{[device, render_semaphore] { vkDestroySemaphore(device, render_semaphore, nullptr); }};

    static constexpr auto a_second = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
    static constexpr auto color_period = 120.f;

    auto frame_number = 0u;
    for (auto running = true; running;) {
        for (SDL_Event event; SDL_PollEvent(&event) != 0;) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        vkWaitForFences(device, 1, &render_fence, (VkBool32) true, a_second);
        vkResetFences(device, 1, &render_fence);

        auto swapchain_image_index = (u32)0;
        vkAcquireNextImageKHR(device, swap_chain, a_second, present_semaphore, nullptr, &swapchain_image_index);

        vkResetCommandBuffer(main_command_buffer, 0);

        auto command_begin_info = VkCommandBufferBeginInfo{
          .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
          .pNext = nullptr,
          .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
          .pInheritanceInfo = nullptr,
        };

        vkBeginCommandBuffer(main_command_buffer, &command_begin_info);

        auto clear_value = VkClearValue{};
        float flash = std::abs(std::sin(static_cast<float>(frame_number) / color_period));
        clear_value.color = {{0.f, 0.f, flash, 1.f}};

        auto render_pass_info = VkRenderPassBeginInfo{
          .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
          .pNext = nullptr,
          .renderPass = render_pass,
          .framebuffer = framebuffers[swapchain_image_index],
          .renderArea{
            .offset{},
            .extent{
              .width = extent.width,
              .height = extent.height,
            },
          },
          .clearValueCount = 1,
          .pClearValues = &clear_value,
        };

        vkCmdBeginRenderPass(main_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdEndRenderPass(main_command_buffer);
        vkEndCommandBuffer(main_command_buffer);

        const auto waitStage = (VkPipelineStageFlags)VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        auto submit = VkSubmitInfo{
          .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
          .pNext = nullptr,
          .waitSemaphoreCount = 1,
          .pWaitSemaphores = &present_semaphore,
          .pWaitDstStageMask = &waitStage,
          .commandBufferCount = 1,
          .pCommandBuffers = &main_command_buffer,
          .signalSemaphoreCount = 1,
          .pSignalSemaphores = &render_semaphore,
        };

        vkQueueSubmit(graphics_queue, 1, &submit, render_fence);

        auto present_info = VkPresentInfoKHR{
          .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
          .pNext = nullptr,
          .waitSemaphoreCount = 1,
          .pWaitSemaphores = &render_semaphore,
          .swapchainCount = 1,
          .pSwapchains = &swap_chain,
          .pImageIndices = &swapchain_image_index,
        };
        vkQueuePresentKHR(graphics_queue, &present_info);

        frame_number++;
    }

    vkWaitForFences(device, 1, &render_fence, (VkBool32) true, a_second);
    vkResetFences(device, 1, &render_fence);

    return 0;
}

template<typename T, typename Fn, typename... Args>
auto read_vulkan_vector(Fn&& fn, Args&&... args) -> std::expected<std::vector<T>, VkResult> {
    auto count = (u32)0;
    const auto size_read_res = fn(std::forward<Args>(args)..., &count, nullptr);

    if (size_read_res != VK_SUCCESS) {
        return std::unexpected{size_read_res};
    }

    auto vec = std::vector<T>(count);
    const auto vec_read_res = fn(std::forward<Args>(args)..., &count, vec.data());

    if (vec_read_res == VK_INCOMPLETE) {
        return read_vulkan_vector<T>(std::forward<Fn>(fn), std::forward<Args>(args)...);
    }

    if (vec_read_res != VK_SUCCESS) {
        return std::unexpected{vec_read_res};
    }

    return vec;
}

struct vulkan_information {
    static auto make() -> std::expected<vulkan_information, VkResult> {
        auto&& available_layers = TRYX(read_vulkan_vector<VkLayerProperties>(&vkEnumerateInstanceLayerProperties));

        auto layer_extensions = std::vector<std::vector<VkExtensionProperties>>{};
        layer_extensions.reserve(available_layers.size());

        for (auto const& layer : available_layers) {
            auto&& avail_extensions_for_layer = TRYX(read_vulkan_vector<VkExtensionProperties>(&vkEnumerateInstanceExtensionProperties, layer.layerName));
            layer_extensions.emplace_back(std::move(avail_extensions_for_layer));
        }

        auto ret = vulkan_information{};
        ret.m_layers.reserve(available_layers.size());

        for (auto&& [layer_no, layer] : std::move(available_layers) | std::views::enumerate) {
            auto&& elem = layer_type{std::move(layer), std::move(layer_extensions[layer_no])};
            ret.m_layers.emplace_back(std::move(elem));
        }

        return ret;
    }

    constexpr auto layer(std::string_view layer_name) const -> std::optional<VkLayerProperties> {
        return layer_internal(layer_name).transform([](auto const& layer) { return layer.get().first; });
    }

    constexpr auto extension(std::string_view extension_name) const -> std::optional<VkExtensionProperties> {
        auto extension_properties = m_layers | std::views::values | std::views::join;
        auto it = std::ranges::find_if(extension_properties, [extension_name](auto const& extension) { return extension_name == std::string_view(extension.extensionName); });

        if (it == std::ranges::end(extension_properties)) {
            return std::nullopt;
        }

        return *it;
    }

    constexpr auto extension(std::string_view extension_name, std::string_view layer_name) const -> std::optional<VkExtensionProperties> {
        return layer_internal(layer_name).and_then([extension_name](auto const& layer) -> std::optional<VkExtensionProperties> {
            auto it = std::ranges::find_if(layer.get().second, [extension_name](auto const& layer) { return extension_name == std::string_view(layer.extensionName); });

            if (it == std::ranges::end(layer.get().second)) {
                return std::nullopt;
            }

            return *it;
        });
    }

    constexpr auto layers() const { return m_layers | std::views::keys; }

    constexpr auto extensions() const { return m_layers | std::views::values | std::views::join; }

    constexpr auto extensions(std::string_view layer_name) const {
        return layer_internal(layer_name)  //
          .transform([](auto const& layer) { return layer.get().second | std::views::all; });
        //.value_or([] { return std::ranges::empty_view<VkExtensionProperties>{}; });
    }

private:
    using layer_type = std::pair<VkLayerProperties, std::vector<VkExtensionProperties>>;
    std::vector<layer_type> m_layers{};

    constexpr auto layer_internal(std::string_view layer_name) const -> std::optional<std::reference_wrapper<const layer_type>> {
        auto it = std::ranges::find_if(m_layers, [layer_name](auto const& layer) { return layer_name == std::string_view(layer.first.layerName); });

        if (it == std::ranges::end(m_layers)) {
            return std::nullopt;
        }

        return *it;
    }
};

static auto
vulkan_debug_messenger(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data, void* user_data)
  -> VkBool32 {
    auto level = spdlog::level::n_levels;

    static constexpr std::pair<int, spdlog::level::level_enum> level_lookup[]{
      {VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, spdlog::level::err},
      {VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, spdlog::level::warn},
      {VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, spdlog::level::info},
      {VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT, spdlog::level::debug},
    };

    if (auto it = std::ranges::find_if(level_lookup, [severity](auto p) { return (p.first & severity) != 0; }); it != std::ranges::end(level_lookup)) {
        level = it->second;
    }

    if (level == spdlog::level::n_levels) {
        spdlog::warn("vulkan debug messenger callback called with an invalid level: {:#08X}", static_cast<u32>(severity));
        level = spdlog::level::info;
    }

    char message_type_arr[]{'G', 'V', 'P', 'D', '\0'};

    static constexpr std::pair<VkFlags, usize> type_lookup[]{
      {VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT, 0uz},
      {VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, 1uz},
      {VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT, 2uz},
      {VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT, 3uz},
    };

    for (auto [flag, index] : type_lookup) {
        if ((type & flag) == 0) {
            message_type_arr[index] = '-';
        }
    }

    const auto message_type = std::string_view(message_type_arr, std::size(message_type_arr));

    spdlog::log(level, fmt::runtime("[{}]: {}"), message_type, data->pMessage);

    return (VkBool32) false;
}

auto main() -> int {
    spdlog::set_level(spdlog::level::debug);

    auto functions = ({
        auto&& res = vulkan_functions::make();

        if (!res) {
            spdlog::error("failed to load vulkan library, error: {}", res.error());
            return 1;
        }

        std::move(*res);
    });

    if (auto res = functions.load_instanceless_functions(); !res) {
        spdlog::error("failed to load instanceless vulkan functions, error: {}", res.error());
        return 1;
    }

    const auto vk_info = ({
        auto&& res = vulkan_information::make();

        if (!res) {
            spdlog::error("failed to gather vulkan information, error code: {}", (int)res.error());
            return 1;
        }

        std::move(*res);
    });

    const auto application_info = VkApplicationInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = "vulkan test",
      .applicationVersion = VK_MAKE_VERSION(0, 0, 0),
      .pEngineName = "voxels",
      .engineVersion = VK_MAKE_VERSION(0, 0, 0),
      .apiVersion = VK_API_VERSION_1_0,
    };

    for (auto layers_range = vk_info.layers(); auto const& [layer_no, layer] : layers_range | std::views::enumerate) {
        spdlog::debug("Layer #{}", layer_no);
        spdlog::debug("\tName        : {}", layer.layerName);
        spdlog::debug("\tSpec version: {}.{}.{}", VK_VERSION_MAJOR(layer.specVersion), VK_VERSION_MINOR(layer.specVersion), VK_VERSION_PATCH(layer.specVersion));
        spdlog::debug(
          "\tImpl version: {}.{}.{}",  //
          VK_VERSION_MAJOR(layer.implementationVersion), VK_VERSION_MINOR(layer.implementationVersion), VK_VERSION_PATCH(layer.implementationVersion)
        );
        spdlog::debug("\tDescription : {}", layer.description);

        for (auto extensions_range = *vk_info.extensions(layer.layerName); auto const& [extension_no, extension] : extensions_range | std::views::enumerate) {
            spdlog::debug("\tExtension #{}", extension_no);
            spdlog::debug("\t\tName        : {}", extension.extensionName);
            spdlog::debug("\t\tSpec version: {}.{}.{}", VK_VERSION_MAJOR(extension.specVersion), VK_VERSION_MINOR(extension.specVersion), VK_VERSION_PATCH(extension.specVersion));
        }
    }

    static constexpr const char* desired_layers[] = {
      "VK_LAYER_KHRONOS_validation",
    };
    auto layers_range = desired_layers | std::views::filter([&vk_info](auto* layer_name) { return (bool)vk_info.layer(layer_name); });
    auto layers = std::vector<const char*>{std::ranges::begin(layers_range), std::ranges::end(layers_range)};

    static constexpr const char* desired_extensions[] = {
      VK_EXT_DEBUG_UTILS_EXTENSION_NAME,              //
      VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,  //
      "VK_KHR_xcb_surface",                           //
      "VK_KHR_xlib_surface",                          //
      "VK_KHR_wayland_surface",                       //
    };

    auto extensions_range = desired_extensions | std::views::filter([&vk_info](auto* ext_name) { return (bool)vk_info.extension(ext_name); });
    auto extensions = std::vector<const char*>{std::ranges::begin(extensions_range), std::ranges::end(extensions_range)};

    const auto debug_utils_messenger_create_info = VkDebugUtilsMessengerCreateInfoEXT{
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .pNext = nullptr,
      .flags = 0,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = vulkan_debug_messenger,
    };

    const auto instance_create_info = VkInstanceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .pApplicationInfo = &application_info,
      .enabledLayerCount = static_cast<u32>(layers.size()),
      .ppEnabledLayerNames = layers.data(),
      .enabledExtensionCount = static_cast<u32>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data(),
    };

     auto* instance = VkInstance{};
     const auto create_instance_res = functions.vkCreateInstance(&instance_create_info, nullptr, &instance);

     if (auto res = functions.load_instance_functions(instance); !res) {
        spdlog::error("failed to load instanced vulkan functions, error: {}", res.error());
        return 1;
     }

     auto* debug_messenger = VkDebugUtilsMessengerEXT{};
     functions.vkCreateDebugUtilsMessengerEXT(instance, &debug_utils_messenger_create_info, nullptr, &debug_messenger);

     functions.vkDestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
     functions.vkDestroyInstance(instance, nullptr);
}
