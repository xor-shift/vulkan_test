#pragma once

#include <vxl/vk/formatters.hpp>

#include <stuff/core.hpp>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <vulkan/vulkan_core.h>

namespace vxl {

auto vulkan_debug_messenger(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data, void*)
  -> VkBool32;

void log(VkLayerProperties const& layer, usize tabulation = 0, spdlog::level::level_enum severity = spdlog::level::debug);

void log(VkExtensionProperties const& extension, usize tabulation = 0, spdlog::level::level_enum severity = spdlog::level::debug);

void log(VkPhysicalDeviceProperties const& props, usize tabulation = 0, spdlog::level::level_enum severity = spdlog::level::debug);

void log(VkQueueFamilyProperties const& props, usize tabulation = 0, spdlog::level::level_enum severity = spdlog::level::debug);

}  // namespace vxl
