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

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

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

template<typename T>
constexpr auto vk_res_to_expected(vk::ResultValue<T>&& val) -> std::expected<T, vk::Result> {
    auto&& [result, value] = std::move(val);

    if (result == vk::Result::eSuccess) {
        return std::move(value);
    }

    return std::unexpected<vk::Result>(result);
}

#define TRYX_VK(_expr) TRYX(vk_res_to_expected(std::move((_expr))))

auto main() -> int {
    spdlog::set_level(spdlog::level::debug);

    auto loader = *vk_dynamic_loader::make();
    vk::defaultDispatchLoaderDynamic.init(loader.vkGetInstanceProcAddr);

    const auto vk_info = ({
        auto&& res = vulkan_information::make();

        if (!res) {
            spdlog::error("failed to gather vulkan information, error code: {}", (int)res.error());
            return 1;
        }

        std::move(*res);
    });

    for (auto layers_range = vk_info.layers(); auto const& [layer_no, layer] : layers_range | std::views::enumerate) {
        spdlog::debug("Layer #{}", layer_no);
        vxl::log(layer, 1);

        for (auto extensions_range = *vk_info.extensions(layer.layerName); auto const& [extension_no, extension] : extensions_range | std::views::enumerate) {
            spdlog::debug("\tExtension #{}", extension_no);
            vxl::log(extension, 2);
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

    const auto application_info = vk::ApplicationInfo("vulkan test", VK_MAKE_VERSION(0, 0, 1), "voxels", VK_MAKE_VERSION(0, 0, 1), VK_API_VERSION_1_1);

    const auto instance_create_info = vk::InstanceCreateInfo({}, &application_info, layers, extensions, nullptr);
    auto instance = ({
        auto&& [result, value] = vk::createInstance(instance_create_info);

        if (result != vk::Result::eSuccess) {
            spdlog::error("could not create a vk::Instance, error code: {}", vk::to_string(result));
            return 1;
        }

        value;
    });

    UNNAMED = stf::scope_exit([&instance] { instance.destroy(); });

    vk::defaultDispatchLoaderDynamic.init(instance);

    const auto debug_utils_messenger_create_info = vk::DebugUtilsMessengerCreateInfoEXT(
      {},                                                                                                                                                       //
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,  //
      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,  //
      vxl::vulkan_debug_messenger                                                                                                                               //
    );

    auto debug_utils_messenger = ({
        auto&& [result, value] = instance.createDebugUtilsMessengerEXT(debug_utils_messenger_create_info);

        if (result != vk::Result::eSuccess) {
            spdlog::error("could not create a vk::DebugUtilsMessengerEXT, error code: {}", vk::to_string(result));
            return 1;
        }

        value;
    });
    UNNAMED = stf::scope_exit([&instance, &debug_utils_messenger] { instance.destroy(debug_utils_messenger); });

    const auto physical_devices = ({
        auto&& [result, value] = instance.enumeratePhysicalDevices();

        if (result != vk::Result::eSuccess) {
            spdlog::error("could not enumerate physical devices, error code: {}", vk::to_string(result));
            return 1;
        }

        value;
    });

    for (auto const& [no, device] : physical_devices | std::views::enumerate) {
        const auto properties = device.getProperties();
        spdlog::debug("Device #{}:", no);
        vxl::log(properties, 1);

        const auto queue_family_properties = device.getQueueFamilyProperties();
        for (auto const& [no, family] : queue_family_properties | std::views::enumerate) {
            spdlog::debug("\tQueue family #{}", no);
            vxl::log(family, 2);
        }
    }

    const auto chosen_device = physical_devices[0];

    spdlog::info("a physical device was chosen:");
    vxl::log(chosen_device.getProperties(), 1, spdlog::level::info);

    const auto chosen_family = chosen_device.getQueueFamilyProperties()[0];
    const auto family_index = 0;

    spdlog::info("a queue family was chosen at index {}:", family_index);
    vxl::log(chosen_family, 1, spdlog::level::info);

    auto queue_priority = 0.f;
    const auto queue_create_info = vk::DeviceQueueCreateInfo({}, static_cast<u32>(family_index), 1, &queue_priority);
    const auto device_create_info = vk::DeviceCreateInfo({}, queue_create_info);
    const auto device = ({
        auto&& [result, value] = chosen_device.createDevice(device_create_info);

        if (result != vk::Result::eSuccess) {
            spdlog::error("could not create a vk::Device, error code: {}", vk::to_string(result));
            return 1;
        }

        value;
    });
    UNNAMED = stf::scope_exit([&device] { device.destroy(); });

    spdlog::debug("device was created");

    const auto command_pool_create_info = vk::CommandPoolCreateInfo({}, family_index);
    const auto command_pool = ({
      auto&& [result, value] = device.createCommandPool(command_pool_create_info);

      if (result != vk::Result::eSuccess) {
          spdlog::error("could not create a vk::CommandPool, error code: {}", vk::to_string(result));
          return 1;
      }

      value;
    });
    UNNAMED = stf::scope_exit([&device, &command_pool] { device.destroyCommandPool(command_pool); });

    const auto command_buffer_allocate_info = vk::CommandBufferAllocateInfo(command_pool, vk::CommandBufferLevel::ePrimary, 1);
    const auto command_buffers = ({
      auto&& [result, value] = device.allocateCommandBuffers(command_buffer_allocate_info);

      if (result != vk::Result::eSuccess) {
          spdlog::error("could not create a vector of vk::CommandBuffer, error code: {}", vk::to_string(result));
          return 1;
      }

      value;
    });
    UNNAMED = stf::scope_exit([&device, &command_pool, &command_buffers] { device.freeCommandBuffers(command_pool, command_buffers); });

    auto const& command_buffer = command_buffers[0];

    constexpr auto extent = vk::Extent2D{600, 400};

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        spdlog::error("failed to initialize SDL: {}", SDL_GetError());
        return 1;
    }
    UNNAMED = stf::scope_exit{[] { SDL_Quit(); }};

    auto* const window = SDL_CreateWindow(
      "vulkan test",                                                    //
      static_cast<int>(extent.width), static_cast<int>(extent.height),  //
      static_cast<SDL_WindowFlags>(SDL_WINDOW_VULKAN)
    );
    UNNAMED = stf::scope_exit{[window] { SDL_DestroyWindow(window); }};

    auto vk_surface = ({
        auto* surface_tmp = VkSurfaceKHR{};
        SDL_Vulkan_CreateSurface(window, instance, &surface_tmp);
        if (surface_tmp == nullptr) {
            spdlog::error("failed to create a vulkan surface through SDL, error: {}", SDL_GetError());
            return 1;
        }

        auto&& vk_surface_tmp = vk::SurfaceKHR(surface_tmp);
        vk_surface_tmp;
    });
    UNNAMED = stf::scope_exit{[&instance, &vk_surface] { instance.destroy(vk_surface); }};

    auto frame_number = 0u;
    for (auto running = true; running;) {
        for (SDL_Event event; SDL_PollEvent(&event) != 0;) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
    }
}
