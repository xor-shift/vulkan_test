#pragma once

#include <vxl/vk/things/swapchain.hpp>

#include <stuff/scope.hpp>

namespace vxl::vk {

swapchain_things::~swapchain_things() {
    for (auto& image_view : m_image_views) {
        UNNAMED = stf::scope_success([&image_view] { image_view = nullptr; });

        m_vk_fns->destroy_image_view(*m_device_things, image_view);
    }

    m_vk_fns->destroy_swapchain_khr(*m_device_things, m_swapchain);
    m_swapchain = nullptr;
}

auto swapchain_things::init(surface_things const& surface, swapchain_settings settings, VkSwapchainKHR old_swapchain) -> std::expected<void, error> {
    TRYX(init_swapchain(surface, settings, old_swapchain));
    TRYX(init_images());
    TRYX(init_image_views());

    return {};
}

auto swapchain_things::init_swapchain(surface_things const& surface, swapchain_settings settings, VkSwapchainKHR old_swapchain) -> std::expected<void, error> {
    const auto surface_format = TRYX(choose_surface_format(surface, settings));
    const auto present_mode = TRYX(choose_present_mode(surface, settings));

    const auto surface_capabilities =
      TRYX(error::from_vk(m_vk_fns->get_physical_device_surface_capabilities_khr(*m_device_things, surface), "could not fetch surface capabilities from the physical device"));

    spdlog::debug("surface capabilities:");
    spdlog::debug("\tmin. image count : {}", surface_capabilities.minImageCount);
    spdlog::debug("\tmax. image count : {}", surface_capabilities.maxImageCount);
    spdlog::debug("\tcurrent extent   : {}", surface_capabilities.currentExtent);
    spdlog::debug("\tmin. image extent: {}", surface_capabilities.minImageExtent);
    spdlog::debug("\tmax. image extent: {}", surface_capabilities.maxImageExtent);

    const auto composite_alpha = choose_composite_alpha(surface_capabilities, settings);
    const auto pre_transform = choose_pre_transform(surface_capabilities, settings);

    const auto swapchain_create_info = VkSwapchainCreateInfoKHR{
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .pNext = nullptr,
      .flags = 0,
      .surface = surface,
      .minImageCount = std::max(surface_capabilities.minImageCount, settings.m_least_buffering),
      .imageFormat = surface_format.format,
      .imageColorSpace = surface_format.colorSpace,
      .imageExtent = settings.m_extent,
      .imageArrayLayers = 1,
      .imageUsage = settings.m_image_usage,
      .imageSharingMode = settings.m_queue_family_indices.empty() ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
      .queueFamilyIndexCount = static_cast<u32>(settings.m_queue_family_indices.size()),
      .pQueueFamilyIndices = settings.m_queue_family_indices.empty() ? nullptr : settings.m_queue_family_indices.data(),
      .preTransform = pre_transform,
      .compositeAlpha = composite_alpha,
      .presentMode = present_mode,
      .clipped = VK_TRUE,
      .oldSwapchain = old_swapchain,
    };

    m_swapchain = TRYX(error::from_vk(m_vk_fns->create_swapchain_khr(*m_device_things, &swapchain_create_info), "could not create a swapchain"));
    m_format = surface_format;

    return {};
}

auto swapchain_things::choose_surface_format(surface_things const& surface, swapchain_settings const& settings) -> std::expected<VkSurfaceFormatKHR, error> {
    auto&& surface_formats = TRYX(error::from_vk(
      get_vector<VkSurfaceFormatKHR>(std::bind_front(m_vk_fns->get_physical_device_surface_formats_khr(), m_device_things->physical_device(), surface.surface())),
      "could not fetch surface formats from the physical device"
    ));

    for (auto const& [no, format] : surface_formats | std::views::enumerate) {
        spdlog::debug("surface #{}:", no);
        spdlog::debug("\tcolor space: {}", format.colorSpace);
        spdlog::debug("\tformat     : {}", format.format);
    }

    const auto sentinel_surface_format = VkSurfaceFormatKHR{VK_FORMAT_MAX_ENUM, VK_COLOR_SPACE_MAX_ENUM_KHR};
    auto surface_format = sentinel_surface_format;

    for (auto const& [available_format, available_colorspace] : surface_formats) {
        for (auto const& [preferred_format, preferred_colorspace] : settings.m_surface_format_preference) {
            const auto format_matches = preferred_format == VK_FORMAT_MAX_ENUM || preferred_format == available_format;
            const auto colorspace_matches = preferred_colorspace == VK_COLOR_SPACE_MAX_ENUM_KHR || preferred_colorspace == available_colorspace;

            if (format_matches && colorspace_matches) {
                surface_format = {available_format, available_colorspace};
                break;
            }
        }

        if (surface_format != sentinel_surface_format) {
            break;
        }
    }

    if (surface_format == sentinel_surface_format) {
        surface_format = surface_formats[0];
    }

    return surface_format;
}

auto swapchain_things::choose_present_mode(surface_things const& surface, swapchain_settings const& settings) -> std::expected<VkPresentModeKHR, error> {
    auto&& present_modes = TRYX(error::from_vk(
      get_vector<VkPresentModeKHR>(std::bind_front(m_vk_fns->get_physical_device_surface_present_modes_khr(), m_device_things->physical_device(), surface.surface())),
      "could not fetch surface formats from the physical device"
    ));

    auto present_mode = *detail::pick_with_preference_and_constraint(std::span(present_modes), settings.m_present_mode_preferences);

    return present_mode;
}

auto swapchain_things::choose_composite_alpha(VkSurfaceCapabilitiesKHR const& surface_capabilities, swapchain_settings const& settings) -> VkCompositeAlphaFlagBitsKHR {
    auto composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    for (auto preference : settings.m_composite_alpha_preference) {
        if ((surface_capabilities.supportedCompositeAlpha & preference) != VkFlags{}) {
            composite_alpha = preference;
            break;
        }
    }

    return composite_alpha;
}

auto swapchain_things::choose_pre_transform(VkSurfaceCapabilitiesKHR const& surface_capabilities, swapchain_settings const& settings) -> VkSurfaceTransformFlagBitsKHR {
    auto pre_transform = surface_capabilities.currentTransform;
    for (auto preference : settings.m_pre_transform_preferences) {
        if ((surface_capabilities.supportedTransforms & preference) != preference) {
            continue;
        }

        pre_transform = preference;
        break;
    }

    return pre_transform;
}

auto swapchain_things::init_images() -> std::expected<void, error> {
    m_images = TRYX(error::from_vk(                                                                                        //
      get_vector<VkImage>(std::bind_front(m_vk_fns->get_swapchain_images_khr(), m_device_things->device(), m_swapchain)),  //
      "could not get swapchain images"
    ));

    return {};
}

auto swapchain_things::init_image_views() -> std::expected<void, error> {
    m_image_views.reserve(m_images.size());

    for (auto const& [no, image] : m_images | std::views::enumerate) {
        const auto image_view_create_info = VkImageViewCreateInfo{
          .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .image = image,
          .viewType = VK_IMAGE_VIEW_TYPE_2D,
          .format = m_format.format,
          .components = {},
          .subresourceRange =
            VkImageSubresourceRange{
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1,
            },
        };

        auto&& image_view = TRYX(error::from_vk(m_vk_fns->create_image_view(*m_device_things, &image_view_create_info), "could not create a swapchain image view"));

        m_image_views.emplace_back(std::move(image_view));
    }

    return {};
}

}  // namespace vxl::vk
