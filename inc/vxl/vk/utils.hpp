#pragma once

#include <vxl/vk/formatters.hpp>

#include <stuff/intro/introspectors/function.hpp>

#include <fmt/format.h>
#include <vulkan/vulkan.h>

#include <expected>
#include <memory_resource>
#include <source_location>

#define UNNAMED3(_name) const auto _unnamed_##_name
#define UNNAMED2(_name) UNNAMED3(_name)
#define UNNAMED UNNAMED2(__COUNTER__)

namespace vxl::vk {

struct error {
    std::string m_description;
    std::source_location m_location;
    VkResult m_vk_result = VK_SUCCESS;

    static auto make(std::string_view description, std::source_location location = std::source_location::current()) {
        return error{
          .m_description{description.begin(), description.end()},
          .m_location = location,
        };
    }

    static auto make_vk(VkResult result, std::string_view additional = "no additional information", std::source_location location = std::source_location::current()) {
        return error{
          .m_description = fmt::format("vulkan call returned {}, additional information: {}", result, additional),
          .m_location = location,
          .m_vk_result = result,
        };
    }

    // vk return types:
    // std::expected<T, VkResult>                      : no soft-errors
    // std::expected<std::pair<T, VkResult>, VkResult> : soft errors
    // std::expected<void, VkResult>                   : no soft-errors
    // std::expected<VkResult, VkResult>               : soft errors

    /*template<typename T>
        requires(!std::is_void_v<T>)
    static auto from_vk(vk::ResultValue<T>&& vk_result, std::string_view additional = "no additional information", std::source_location location = std::source_location::current())
      -> std::expected<T, error> {
        auto&& [result, value] = std::move(vk_result);

        if (result != vk::Result::eSuccess) {
            return std::unexpected{error::make_vk(result, additional, location)};
        }

        return value;
    }

    static auto from_vk(vk::Result&& result, std::string_view additional = "no additional information", std::source_location location = std::source_location::current())
      -> std::expected<void, error> {
        if (result != vk::Result::eSuccess) {
            return std::unexpected{error::make_vk(result, additional, location)};
        }

        return {};
    }*/
};

}  // namespace vxl::vk

template<>
struct fmt::formatter<std::source_location> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }

    auto format(std::source_location const& loc, auto& ctx) {
        return fmt::format_to(ctx.out(), "function \"{}\" at {}:{}:{}", loc.function_name(), loc.file_name(), loc.line(), loc.column());
    }
};

template<>
struct fmt::formatter<vxl::vk::error> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }

    auto format(vxl::vk::error const& err, auto& ctx) { return fmt::format_to(ctx.out(), "error at {}, description: {}", err.m_location, err.m_description); }
};

namespace vxl::vk {

template<typename Fn, typename T = std::remove_pointer_t<typename stf::intro::function_introspector<Fn>::template nth_argument<0>>>
auto get_vector(Fn fn, std::pmr::memory_resource* memory_resource = std::pmr::get_default_resource()) -> std::expected<std::pmr::vector<T>, VkResult> {
    auto result = VK_SUCCESS;
    auto ret = std::pmr::vector<T>{memory_resource};

    for (;;) {
        ret.clear();

        auto property_count = 0uz;
        std::tie(property_count, result) = TRYX(fn(nullptr));

        if (result == VK_SUCCESS && property_count != 0) {
            ret.resize(property_count);
            std::tie(std::ignore, result) = TRYX(fn(ret.data()));
        }

        if (result != VK_INCOMPLETE) {
            break;
        }
    }

    if (result != VK_SUCCESS) {
        return std::unexpected{result};
    }

    return ret;
}

}  // namespace vxl::vk