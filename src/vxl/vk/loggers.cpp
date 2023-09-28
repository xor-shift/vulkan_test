#include <vxl/vk/loggers.hpp>

#include <ranges>

#include <vulkan/vk_enum_string_helper.h>

namespace {

template<typename Flags, typename FlagBit, usize N>
constexpr auto make_flags_short(std::span<const std::pair<FlagBit, char>, N> lookup, Flags flags) -> std::array<char, N> {
    auto arr = std::array<char, N>{};
    std::ranges::copy(lookup | std::views::values, arr.begin());

    for (auto const& [index, bit] : lookup | std::views::keys | std::views::enumerate) {
        if ((flags & bit) != Flags {}) {
            continue;
        }

        arr[index] = '-';
    }

    return arr;
}

template<typename Flags, typename FlagBit, usize N, typename It>
constexpr void make_flags_short(std::span<const std::pair<FlagBit, char>, N> lookup, Flags flags, It out) {
    auto arr = make_flags_short(lookup, flags);
    std::ranges::copy(arr, out);
}

template<typename Flags, typename FlagBit, usize N, typename It>
constexpr void make_flags_long(std::span<const std::pair<FlagBit, std::string_view>, N> lookup, Flags flags, It out, std::string_view if_empty = "[no bits set]") {
    auto separator = " | "sv;

    auto first = true;
    for (auto const& [bit, name] : lookup) {
        if ((flags & bit) == Flags {}) {
            continue;
        }

        if (!first) {
            std::ranges::copy(separator, out);
        }
        first = false;

        std::ranges::copy(name, out);
    }

    if (first) {
        std::ranges::copy(if_empty, out);
    }
}

template<typename Flags, typename FlagBit, usize N>
auto make_flags_long(std::span<const std::pair<FlagBit, std::string_view>, N> lookup, Flags flags, std::string_view if_empty = "[no bits set]") -> std::string {
    auto ret = std::string {};
    make_flags_long(lookup, flags, back_inserter(ret), if_empty);
    return ret;
}


}

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

    spdlog::log(level, fmt::runtime("[{}]: {}"), message_type, data->pMessage);

    return (VkBool32) false;
}

void log(VkLayerProperties const& layer, usize tabulation, spdlog::level::level_enum severity) {
    spdlog::log(severity, fmt::runtime("{:\t>{}}Name        : {}"), "", tabulation, std::string_view(layer.layerName));
    spdlog::log(
      severity, fmt::runtime("{:\t>{}}Spec version: {}.{}.{}"), "", tabulation, VK_VERSION_MAJOR(layer.specVersion), VK_VERSION_MINOR(layer.specVersion),
      VK_VERSION_PATCH(layer.specVersion)
    );
    spdlog::log(
      severity, fmt::runtime("{:\t>{}}Impl version: {}.{}.{}"), "", tabulation, VK_VERSION_MAJOR(layer.implementationVersion), VK_VERSION_MINOR(layer.implementationVersion),
      VK_VERSION_PATCH(layer.implementationVersion)
    );
    spdlog::log(severity, fmt::runtime("{:\t>{}}Description : {}"), "", tabulation, std::string_view(layer.description));
}

void log(VkExtensionProperties const& extension, usize tabulation, spdlog::level::level_enum severity) {
    spdlog::log(severity, fmt::runtime("{:\t>{}}Name        : {}\", layer.layerName"), "", tabulation, std::string_view(extension.extensionName));
    spdlog::log(
      severity, fmt::runtime("{:\t>{}}Spec version: {}.{}.{}"), "", tabulation, VK_VERSION_MAJOR(extension.specVersion), VK_VERSION_MINOR(extension.specVersion),
      VK_VERSION_PATCH(extension.specVersion)
    );
}

void log(VkPhysicalDeviceProperties const& props, usize tabulation, spdlog::level::level_enum severity) {
    spdlog::log(
      severity, fmt::runtime("{:\t>{}}API Version   : {}.{}.{}"), "", tabulation, VK_VERSION_MAJOR(props.apiVersion), VK_VERSION_MINOR(props.apiVersion),
      VK_VERSION_PATCH(props.apiVersion)
    );
    spdlog::log(
      severity, fmt::runtime("{:\t>{}}Driver Version: {}.{}.{}"), "", tabulation, VK_VERSION_MAJOR(props.driverVersion), VK_VERSION_MINOR(props.driverVersion),
      VK_VERSION_PATCH(props.driverVersion)
    );
    spdlog::log(severity, fmt::runtime("{:\t>{}}Vendor ID     : {:08X}"), "", tabulation, props.vendorID);
    spdlog::log(severity, fmt::runtime("{:\t>{}}Device ID     : {:08X}"), "", tabulation, props.deviceID);
    spdlog::log(severity, fmt::runtime("{:\t>{}}Device Type   : {}"), "", tabulation, props.deviceType);
    spdlog::log(severity, fmt::runtime("{:\t>{}}Device Name   : {}"), "", tabulation, std::string_view(props.deviceName));
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

    spdlog::log(severity, fmt::runtime("{:\t>{}}Queue flags     : {}"), "", tabulation, make_flags_long(std::span(queue_flags_lookup), props.queueFlags));
    spdlog::log(severity, fmt::runtime("{:\t>{}}Queue count     : {}"), "", tabulation, props.queueCount);
    spdlog::log(severity, fmt::runtime("{:\t>{}}TSV bits        : {:08X}"), "", tabulation, props.timestampValidBits);
    spdlog::log(severity, fmt::runtime("{:\t>{}}Min. granularity: {}"), "", tabulation, props.minImageTransferGranularity);
}

}  // namespace vxl
