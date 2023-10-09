#pragma once

#include <vxl/vk/error.hpp>
#include <vxl/vk/formatters.hpp>
#include <vxl/vk/things/device.hpp>
#include <vxl/vk/things/surface.hpp>
#include <vxl/vk/things/utils.hpp>
#include <vxl/vk/vector_helpers.hpp>

#include <span>

namespace vxl::vk {

struct swapchain_settings {
    static constexpr std::pair<VkFormat, VkColorSpaceKHR> m_default_surface_format_preference[]{
      {VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR},
      {VK_FORMAT_R8G8B8A8_SRGB, VK_COLORSPACE_SRGB_NONLINEAR_KHR},
      {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_MAX_ENUM_KHR},
      {VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_MAX_ENUM_KHR},
    };

    // if none of these match, the first available format will be used
    // use the ENUM_MAX for a wildcard
    // failure: won't fail if a surface format could not be chosen
    std::span<const std::pair<VkFormat, VkColorSpaceKHR>> m_surface_format_preference = std::span(m_default_surface_format_preference);

    static constexpr VkPresentModeKHR m_default_present_mode_preferences[]{
      VK_PRESENT_MODE_FIFO_KHR,
    };

    // failure: will fail if none of the present modes are supported by the surface
    std::span<const VkPresentModeKHR> m_present_mode_preferences = std::span(m_default_present_mode_preferences);

    static constexpr VkCompositeAlphaFlagBitsKHR m_default_composite_alpha_preference[]{
      VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };

    // default: VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
    // failure: will not fail. if no alpha compositing setting could be chosen, the default will be used
    std::span<const VkCompositeAlphaFlagBitsKHR> m_composite_alpha_preference = std::span(m_default_composite_alpha_preference);

    static constexpr VkSurfaceTransformFlagBitsKHR m_default_pre_transform_preferences[]{
      VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
    };

    // default: whatever the current transform is
    // failure: will not fail.
    std::span<const VkSurfaceTransformFlagBitsKHR> m_pre_transform_preferences = std::span(m_default_pre_transform_preferences);

    // leave empty if a single queue is used for graphics and presentation
    std::span<const u32> m_queue_family_indices = {};

    u32 m_least_buffering = 2;
    VkExtent2D m_extent;
    VkImageUsageFlags m_image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
};

struct swapchain_things {
    swapchain_things(std::shared_ptr<vulkan_functions> vk_fns, std::shared_ptr<device_things> device_things)
        : m_vk_fns(std::move(vk_fns))
        , m_device_things(std::move(device_things)) {}

    ~swapchain_things();

    swapchain_things(swapchain_things const&) = delete;
    swapchain_things(swapchain_things&&) = delete;

    constexpr auto swapchain() const -> VkSwapchainKHR { return m_swapchain; }
    constexpr operator VkSwapchainKHR() const { return m_swapchain; }

    constexpr auto format() const -> VkSurfaceFormatKHR { return m_format; }
    constexpr auto images() const -> std::span<const VkImage> { return std::span(m_images); }
    constexpr auto image_views() const -> std::span<const VkImageView> { return std::span(m_image_views); }

    auto init(surface_things const& surface, swapchain_settings settings, VkSwapchainKHR old_swapchain = nullptr) -> std::expected<void, error>;

private:
    std::shared_ptr<vulkan_functions> m_vk_fns;
    std::shared_ptr<device_things> m_device_things;

    VkSwapchainKHR m_swapchain;
    VkSurfaceFormatKHR m_format;
    std::pmr::vector<VkImage> m_images;
    std::pmr::vector<VkImageView> m_image_views;

    auto init_swapchain(surface_things const& surface, swapchain_settings settings, VkSwapchainKHR old_swapchain = nullptr) -> std::expected<void, error>;

    auto choose_surface_format(surface_things const& surface, swapchain_settings const& settings) -> std::expected<VkSurfaceFormatKHR, error>;

    auto choose_present_mode(surface_things const& surface, swapchain_settings const& settings) -> std::expected<VkPresentModeKHR, error>;

    auto choose_composite_alpha(VkSurfaceCapabilitiesKHR const& surface_capabilities, swapchain_settings const& settings) -> VkCompositeAlphaFlagBitsKHR;

    auto choose_pre_transform(VkSurfaceCapabilitiesKHR const& surface_capabilities, swapchain_settings const& settings) -> VkSurfaceTransformFlagBitsKHR;

    auto init_images() -> std::expected<void, error>;

    auto init_image_views() -> std::expected<void, error>;
};

}  // namespace vxl::vk
