#pragma once

#define UNNAMED3(_name) const auto _unnamed_##_name
#define UNNAMED2(_name) UNNAMED3(_name)
#define UNNAMED UNNAMED2(__COUNTER__)

#include <vulkan/vulkan_core.h>

constexpr auto operator==(VkSurfaceFormatKHR const& lhs, VkSurfaceFormatKHR const& rhs) -> bool {
    return lhs.format == rhs.format && lhs.colorSpace == rhs.colorSpace;
}
