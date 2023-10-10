#pragma once

#include <vxl/vk/formatters.hpp>
#include <vxl/vk/utils.hpp>

#include <expected>
#include <format>
#include <memory_resource>
#include <source_location>

namespace vxl {

struct error {
    std::string m_description;
    std::source_location m_location;
    VkResult m_vk_result = VK_SUCCESS;

    static auto make(std::string description, std::source_location location = std::source_location::current()) {
        return error{
          .m_description{std::move(description)},
          .m_location = location,
        };
    }

    static auto make_vk(VkResult result, std::string_view additional = "no additional information", std::source_location location = std::source_location::current()) {
        return error{
          .m_description = std::format("vulkan call returned {}, additional information: {}", result, additional),
          .m_location = location,
          .m_vk_result = result,
        };
    }

    // vk return types:
    // std::expected<T, VkResult>                      : no soft-errors
    // std::expected<std::pair<T, VkResult>, VkResult> : soft errors
    // std::expected<void, VkResult>                   : no soft-errors
    // std::expected<VkResult, VkResult>               : soft errors

    static auto
    from_vk(std::expected<void, VkResult> vk_result, std::string_view additional = "no additional information", std::source_location location = std::source_location::current())
      -> std::expected<void, error> {
        return vk_result.transform_error([&additional, &location](VkResult result) { return error::make_vk(result, additional, location); });
    }

    template<typename T>
        requires(!std::is_void_v<T>)
    static auto
    from_vk(std::expected<T, VkResult> vk_result, std::string_view additional = "no additional information", std::source_location location = std::source_location::current())
      -> std::expected<T, error> {
        return vk_result.transform_error([&additional, &location](VkResult result) { return error::make_vk(result, additional, location); });
    }
};

}  // namespace vxl

template<typename CharT>
struct std::formatter<std::source_location, CharT> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }

    auto format(std::source_location const& loc, auto& ctx) const {
        return std::format_to(ctx.out(), "function \"{}\" at {}:{}:{}", loc.function_name(), loc.file_name(), loc.line(), loc.column());
    }
};

template<typename CharT>
struct std::formatter<vxl::error, CharT> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }

    auto format(vxl::error const& err, auto& ctx) const { return std::format_to(ctx.out(), "error at {}, description: {}", err.m_location, err.m_description); }
};
