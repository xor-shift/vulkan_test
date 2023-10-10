#pragma once

#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

#include <format>

template<typename CharT>
struct std::formatter<VkExtent2D, CharT> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }

    constexpr auto format(VkExtent2D const& extent, auto& format_context) const { return std::format_to(format_context.out(), "{}x{}", extent.width, extent.height); }
};

template<typename CharT>
struct std::formatter<VkExtent3D, CharT> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }

    constexpr auto format(VkExtent3D const& extent, auto& format_context) const { return std::format_to(format_context.out(), "{}x{}x{}", extent.width, extent.height, extent.depth); }
};

namespace vxl::detail {

template<typename T, const char* (*ToStr)(T)>
struct vk_enum_formatter : std::formatter<std::basic_string_view<char>, char> {
    template<typename FormatContext>
    constexpr auto format(T v, FormatContext&& format_context) const {
        const auto* const str = ToStr(v);
        return std::formatter<std::basic_string_view<char>, char>::format(str, std::forward<FormatContext>(format_context));
    }
};

}  // namespace vxl::detail

#pragma push_macro("ENUM_FORMATTER_FACTORY")
#define ENUM_FORMATTER_FACTORY(_type) \
    template<>          \
    struct std::formatter<_type, char> : vxl::detail::vk_enum_formatter<_type, string_##_type> {}

ENUM_FORMATTER_FACTORY(VkColorSpaceKHR);
ENUM_FORMATTER_FACTORY(VkFormat);
ENUM_FORMATTER_FACTORY(VkPhysicalDeviceType);
ENUM_FORMATTER_FACTORY(VkResult);

#pragma pop_macro("ENUM_FORMATTER_FACTORY")
