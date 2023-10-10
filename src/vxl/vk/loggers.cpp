#include <vxl/vk/loggers.hpp>

#include <ranges>

#include <vulkan/vk_enum_string_helper.h>

namespace vxl {

auto vulkan_debug_messenger(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data, void*)
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

    static constexpr std::pair<VkDebugUtilsMessageTypeFlagsEXT, char> message_type_lookup[]{
      {VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT, 'G'},
      {VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, 'V'},
      {VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT, 'P'},
      {VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT, 'D'},
    };

    const auto message_type_arr = make_flags_short(std::span(message_type_lookup), type);
    const auto message_type = std::string_view(message_type_arr);

    spdlog::log(level, "[{}]: {}", message_type, data->pMessage);

    return (VkBool32) false;
}

void log(VkLayerProperties const& layer, usize tabulation, spdlog::level::level_enum severity) {
    spdlog::log(severity, "{:\t>{}}Name        : {}", "", tabulation, std::string_view(layer.layerName));
    spdlog::log(
      severity, "{:\t>{}}spec version: {}.{}.{}", "", tabulation, VK_VERSION_MAJOR(layer.specVersion), VK_VERSION_MINOR(layer.specVersion), VK_VERSION_PATCH(layer.specVersion)
    );
    spdlog::log(
      severity, "{:\t>{}}impl version: {}.{}.{}", "", tabulation, VK_VERSION_MAJOR(layer.implementationVersion), VK_VERSION_MINOR(layer.implementationVersion),
      VK_VERSION_PATCH(layer.implementationVersion)
    );
    spdlog::log(severity, "{:\t>{}}description : {}", "", tabulation, std::string_view(layer.description));
}

void log(VkExtensionProperties const& extension, usize tabulation, spdlog::level::level_enum severity) {
    spdlog::log(severity, "{:\t>{}}name        : {}\"", "", tabulation, std::string_view(extension.extensionName));
    spdlog::log(
      severity, "{:\t>{}}spec version: {}.{}.{}", "", tabulation, VK_VERSION_MAJOR(extension.specVersion), VK_VERSION_MINOR(extension.specVersion),
      VK_VERSION_PATCH(extension.specVersion)
    );
}

void log(VkPhysicalDeviceProperties const& props, usize tabulation, spdlog::level::level_enum severity) {
    spdlog::log(
      severity, "{:\t>{}}API version   : {}.{}.{}", "", tabulation, VK_VERSION_MAJOR(props.apiVersion), VK_VERSION_MINOR(props.apiVersion), VK_VERSION_PATCH(props.apiVersion)
    );
    spdlog::log(
      severity, "{:\t>{}}driver vrsion: {}.{}.{}", "", tabulation, VK_VERSION_MAJOR(props.driverVersion), VK_VERSION_MINOR(props.driverVersion),
      VK_VERSION_PATCH(props.driverVersion)
    );
    spdlog::log(severity, "{:\t>{}}vendor ID     : {:08X}", "", tabulation, props.vendorID);
    spdlog::log(severity, "{:\t>{}}device ID     : {:08X}", "", tabulation, props.deviceID);
    spdlog::log(severity, "{:\t>{}}device type   : {}", "", tabulation, props.deviceType);
    spdlog::log(severity, "{:\t>{}}device name   : {}", "", tabulation, std::string_view(props.deviceName));
}

void log(VkQueueFamilyProperties const& props, usize tabulation, spdlog::level::level_enum severity) {
    static constexpr std::pair<VkQueueFlags, std::string_view> queue_flags_lookup[]{
      {VK_QUEUE_GRAPHICS_BIT, "graphics"},
      {VK_QUEUE_COMPUTE_BIT, "compute"},
      {VK_QUEUE_TRANSFER_BIT, "transfer"},
      {VK_QUEUE_SPARSE_BINDING_BIT, "sparse binding"},
      {VK_QUEUE_PROTECTED_BIT, "protected"},
      {VK_QUEUE_VIDEO_DECODE_BIT_KHR, "video decode (KHR)"},
      {VK_QUEUE_OPTICAL_FLOW_BIT_NV, "optical flow (NV)"},
    };

    spdlog::log(severity, "{:\t>{}}queue flags     : {}", "", tabulation, make_flags_long(std::span(queue_flags_lookup), props.queueFlags));
    spdlog::log(severity, "{:\t>{}}qeue count     : {}", "", tabulation, props.queueCount);
    spdlog::log(severity, "{:\t>{}}TSV bits        : {:08X}", "", tabulation, props.timestampValidBits);
    spdlog::log(severity, "{:\t>{}}min. granularity: {}", "", tabulation, props.minImageTransferGranularity);
}

}  // namespace vxl
