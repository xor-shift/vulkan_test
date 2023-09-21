#pragma once

#include <stuff/core.hpp>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.hpp>

template<> struct fmt::formatter<VkExtent3D> {
    constexpr auto parse(auto& parse_context) {
        return parse_context.begin();
    }

    constexpr auto format(VkExtent3D const& extent, auto& format_context) {
        return fmt::format_to(format_context.out(), "(w: {}, h: {}, d: {})", extent.width, extent.height, extent.depth);
    }
};

template<> struct fmt::formatter<vk::Extent3D> {
    constexpr auto parse(auto& parse_context) {
        return parse_context.begin();
    }

    constexpr auto format(vk::Extent3D const& extent, auto& format_context) {
        return fmt::format_to(format_context.out(), "(w: {}, h: {}, d: {})", extent.width, extent.height, extent.depth);
    }
};

namespace vxl {

auto vulkan_debug_messenger(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data, void*)
  -> VkBool32;

void log(vk::LayerProperties const& layer, usize tabulation = 0, spdlog::level::level_enum severity = spdlog::level::debug);

void log(vk::ExtensionProperties const& extension, usize tabulation = 0, spdlog::level::level_enum severity = spdlog::level::debug);

void log(vk::PhysicalDeviceProperties const& props, usize tabulation = 0, spdlog::level::level_enum severity = spdlog::level::debug);

void log(vk::QueueFamilyProperties const& props, usize tabulation = 0, spdlog::level::level_enum severity = spdlog::level::debug);

}  // namespace vxl
