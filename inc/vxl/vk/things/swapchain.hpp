#pragma once

#include <vxl/vk/error.hpp>
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

    static constexpr VkPresentModeKHR m_default_present_mode_preferences[] {
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

    ~swapchain_things() {
        for (auto& image_view : m_image_views) {
            UNNAMED = stf::scope_success([&image_view] { image_view = nullptr; });

            m_vk_fns->destroy_image_view(*m_device_things, image_view);
        }

        m_vk_fns->destroy_swapchain_khr(*m_device_things, m_swapchain);
        m_swapchain = nullptr;
    }

    swapchain_things(swapchain_things const&) = delete;
    swapchain_things(swapchain_things&&) = delete;

    constexpr auto swapchain() const -> VkSwapchainKHR { return m_swapchain; }
    constexpr operator VkSwapchainKHR() { return m_swapchain; }

    constexpr auto format() const -> VkSurfaceFormatKHR { return m_format; }
    constexpr auto images() const -> std::span<const VkImage> { return std::span(m_images); }
    constexpr auto image_views() const -> std::span<const VkImageView> { return std::span(m_image_views); }

    auto init(surface_things const& surface, swapchain_settings settings, VkSwapchainKHR old_swapchain = nullptr) -> std::expected<void, error> {
        TRYX(init_swapchain(surface, settings, old_swapchain));
        TRYX(init_images());
        TRYX(init_image_views());

        return {};
    }

private:
    std::shared_ptr<vulkan_functions> m_vk_fns;
    std::shared_ptr<device_things> m_device_things;

    VkSwapchainKHR m_swapchain;
    VkSurfaceFormatKHR m_format;
    std::pmr::vector<VkImage> m_images;
    std::pmr::vector<VkImageView> m_image_views;

    auto init_swapchain(surface_things const& surface, swapchain_settings settings, VkSwapchainKHR old_swapchain = nullptr) -> std::expected<void, error> {
        auto&& surface_formats = TRYX(error::from_vk(
          get_vector([this, &surface](u32* out_count, VkSurfaceFormatKHR* out_formats) {
              return m_vk_fns->get_physical_device_surface_formats_khr(*m_device_things, surface, out_count, out_formats);
          }),
          "could not fetch surface formats from the physical device"
        ));

        auto&& present_modes = TRYX(error::from_vk(
          get_vector([this, &surface](u32* out_count, VkPresentModeKHR* out_formats) {
              return m_vk_fns->get_physical_device_surface_present_modes_khr(*m_device_things, surface, out_count, out_formats);
          }),
          "could not fetch surface formats from the physical device"
        ));

        for (auto const& [no, format] : surface_formats | std::views::enumerate) {
            spdlog::debug("Surface #{}:", no);
            spdlog::debug("\tColor space: {}", format.colorSpace);
            spdlog::debug("\tFormat     : {}", format.format);
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

        const auto present_mode = *detail::pick_with_preference_and_constraint(std::span(present_modes), settings.m_present_mode_preferences);

        const auto surface_capabilities =
          TRYX(error::from_vk(m_vk_fns->get_physical_device_surface_capabilities_khr(*m_device_things, surface), "could not fetch surface capabilities from the physical device"));

        auto composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        for (auto preference : settings.m_composite_alpha_preference) {
            if ((surface_capabilities.supportedCompositeAlpha & preference) != VkFlags{}) {
                composite_alpha = preference;
                break;
            }
        }

        auto pre_transform = surface_capabilities.currentTransform;
        for (auto preference : settings.m_pre_transform_preferences) {
            if ((surface_capabilities.supportedTransforms & preference) != preference) {
                continue;
            }

            pre_transform = preference;
            break;
        }

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

    auto init_images() -> std::expected<void, error> {
        m_images = TRYX(error::from_vk(
          get_vector([this](u32* out_count, VkImage* out_images) { return m_vk_fns->get_swapchain_images_khr(*m_device_things, m_swapchain, out_count, out_images); }),
          "could not get swapchain images"
        ));

        return {};
    }

    auto init_image_views() -> std::expected<void, error> {
        m_image_views.reserve(m_images.size());

        for (auto const& [no, image] : m_images | std::views::enumerate) {
            const auto image_view_create_info = VkImageViewCreateInfo{
              .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
              .pNext = nullptr,
              .flags = 0,
              .image = image,
              .viewType = VK_IMAGE_VIEW_TYPE_1D,
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
};

}  // namespace vxl::vk
