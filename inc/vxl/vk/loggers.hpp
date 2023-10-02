#pragma once

#include <vxl/vk/formatters.hpp>

#include <stuff/core.hpp>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <vulkan/vulkan_core.h>

#include <ranges>

namespace vxl {

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

auto vulkan_debug_messenger(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data, void*)
  -> VkBool32;

void log(VkLayerProperties const& layer, usize tabulation = 0, spdlog::level::level_enum severity = spdlog::level::debug);

void log(VkExtensionProperties const& extension, usize tabulation = 0, spdlog::level::level_enum severity = spdlog::level::debug);

void log(VkPhysicalDeviceProperties const& props, usize tabulation = 0, spdlog::level::level_enum severity = spdlog::level::debug);

void log(VkQueueFamilyProperties const& props, usize tabulation = 0, spdlog::level::level_enum severity = spdlog::level::debug);

}  // namespace vxl
