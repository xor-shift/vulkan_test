#pragma once

#include <fmt/core.h>

#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

template<>
struct fmt::formatter<VkExtent3D> {
    constexpr auto parse(auto& parse_context) { return parse_context.begin(); }

    constexpr auto format(VkExtent3D const& extent, auto& format_context) {
        return fmt::format_to(format_context.out(), "(w: {}, h: {}, d: {})", extent.width, extent.height, extent.depth);
    }
};

namespace vxl::detail {

template<typename T, const char* (*ToStr)(T)>
struct vk_enum_formatter {
    constexpr auto parse(auto& parse_context) { return parse_context.begin(); }

    constexpr auto format(T v, auto& format_context) {
        auto str = std::string_view(ToStr(v));
        return std::copy(str.begin(), str.end(), format_context.out());
    }
};

}

#pragma push_macro("ENUM_FORMATTER_FACTORY")
#define ENUM_FORMATTER_FACTORY(_type) template<>struct fmt::formatter<_type> : vxl::detail::vk_enum_formatter<_type, string_##_type> {}

ENUM_FORMATTER_FACTORY(VkColorSpaceKHR);
ENUM_FORMATTER_FACTORY(VkFormat);
ENUM_FORMATTER_FACTORY(VkPhysicalDeviceType);
ENUM_FORMATTER_FACTORY(VkResult);

#pragma pop_macro("ENUM_FORMATTER_FACTORY")
