#include <vxl/dyna_loader.hpp>
#include <vxl/vk/functions.hpp>
#include <vxl/vk/loggers.hpp>
#include <vxl/vk/utils.hpp>

#include <stuff/core.hpp>
#include <stuff/expected.hpp>
#include <stuff/intro/introspectors/function.hpp>
#include <stuff/scope.hpp>

#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <chrono>
#include <cmath>
#include <expected>
#include <iostream>
#include <ranges>

/*
struct vulkan_stuff {
    static auto make() -> std::expected<vulkan_stuff, error> {


        TRYX(ret.init_vk_debug_messenger());
        TRYX(ret.init_sdl_window());
        TRYX(ret.init_vk_surface());
        TRYX(ret.init_vk_devices_and_queues());
        TRYX(ret.init_vk_command_pool());
        TRYX(ret.init_vk_command_buffer());
        TRYX(ret.reinit_vk_swapchain(ret.m_window_size));

        // return std::unexpected{"WIP"};
        return ret;
    }

    vulkan_stuff() = default;

    ~vulkan_stuff() {
        std::ignore = destroy_vk_swapchain();
    }

    auto frame(vk::Fence& render_fence, vk::Semaphore& present_semaphore, vk::Semaphore& render_semaphore, usize frame_number) -> std::expected<bool, error> {
        static constexpr auto a_second = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();

        bool running = true;
        static constexpr auto color_period = 120.f;

        for (SDL_Event event; SDL_PollEvent(&event) != 0;) {
            switch (event.type) {
                case SDL_EVENT_QUIT: running = false; break;
                case SDL_EVENT_WINDOW_RESIZED:
                    TRYX(error::from_vk(m_vk_device.waitIdle(m_vk_dispatch), "while waiting for device idle"));

                    // TRYX(destroy_vk_swapchain());
                    m_window_size.width = static_cast<u32>(event.window.data1);
                    m_window_size.height = static_cast<u32>(event.window.data2);
                    TRYX(reinit_vk_swapchain(m_window_size));
                    break;
                default: break;
            }
        }

        auto swapchain_image_index = 0u;
        switch (auto&& res = m_vk_device.acquireNextImageKHR(m_vk_swapchain, a_second, present_semaphore, nullptr, m_vk_dispatch); res.result) {
            case vk::Result::eSuccess: [[fallthrough]];
            case vk::Result::eSuboptimalKHR: swapchain_image_index = res.value; break;
            default: return std::unexpected{error::make_vk(res.result, "could not acquire the next image in the swapchain")};
        }

        m_vk_command_buffer.reset({}, m_vk_dispatch);

        const auto command_begin_info = vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        std::ignore = m_vk_command_buffer.begin(command_begin_info);

        const auto clear_color = std::abs(std::sin(static_cast<float>(frame_number) / color_period));
        const auto clear_value = vk::ClearValue((std::array<float, 4>){0.f, 0.f, clear_color, 1.f});

        const auto render_pass_info = vk::RenderPassBeginInfo(
          m_vk_render_pass,                                                                     //
          m_vk_framebuffers[swapchain_image_index],                                             //
          vk::Rect2D(vk::Offset2D(), vk::Extent2D(m_window_size.width, m_window_size.height)),  //
          1, &clear_value                                                                       //
        );

        m_vk_command_buffer.beginRenderPass(render_pass_info, vk::SubpassContents::eInline, m_vk_dispatch);
        m_vk_command_buffer.endRenderPass(m_vk_dispatch);

        TRYX(error::from_vk(m_vk_command_buffer.end(m_vk_dispatch), "failed to end the command buffer"));

        const auto wait_dst_stage_flags = vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        const auto submit_info = vk::SubmitInfo(
          1, &present_semaphore,    //
          &wait_dst_stage_flags,    //
          1, &m_vk_command_buffer,  //
          1, &render_semaphore      //
        );

        TRYX(error::from_vk(m_vk_queue.submit(1, &submit_info, render_fence, m_vk_dispatch), "failed to submit commands to the queue"));

        const auto present_info = vk::PresentInfoKHR(
          1, &render_semaphore,                        //
          1, &m_vk_swapchain, &swapchain_image_index,  //
          nullptr                                      //
        );

        switch (auto&& res = m_vk_queue.presentKHR(present_info, m_vk_dispatch); res) {
            case vk::Result::eSuccess: [[fallthrough]];
            case vk::Result::eSuboptimalKHR: break;
            default: return std::unexpected{error::make_vk(std::move(res), "failed to present")};
        }

        return running;
    }

    auto run() -> std::expected<void, error> {
        static constexpr auto a_second = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();

        auto fence_create_info = vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled);
        auto render_fence = TRYX(error::from_vk(m_vk_device.createFence(fence_create_info, nullptr, m_vk_dispatch), "could not create a fence (for renders)"));
        UNNAMED = stf::scope_exit{[this, &render_fence] { m_vk_device.destroy(render_fence, nullptr, m_vk_dispatch); }};

        auto present_semaphore = TRYX(error::from_vk(m_vk_device.createSemaphore(vk::SemaphoreCreateInfo{}, nullptr, m_vk_dispatch), "could not create a semaphore (for present)"));
        UNNAMED = stf::scope_exit{[this, &present_semaphore] { m_vk_device.destroy(present_semaphore, nullptr, m_vk_dispatch); }};

        auto render_semaphore = TRYX(error::from_vk(m_vk_device.createSemaphore(vk::SemaphoreCreateInfo{}, nullptr, m_vk_dispatch), "could not create a semaphore (for render)"));
        UNNAMED = stf::scope_exit{[this, &render_semaphore] { m_vk_device.destroy(render_semaphore, nullptr, m_vk_dispatch); }};

        for (auto frame_number = 0uz;; frame_number++) {
            std::ignore = m_vk_device.waitForFences(1, &render_fence, VK_TRUE, a_second, m_vk_dispatch);
            std::ignore = m_vk_device.resetFences(1, &render_fence, m_vk_dispatch);

            auto res = frame(render_fence, present_semaphore, render_semaphore, frame_number);

            if (res) {
                if (*res) {
                    continue;
                }

                break;
            }

            auto const& error = res.error();

            spdlog::warn("received an error: {}", error);

            switch (error.m_vk_result) {
                case vk::Result::eSuboptimalKHR: [[fallthrough]];
                case vk::Result::eErrorOutOfDateKHR: spdlog::warn("asd"); break;
                default: spdlog::error("the error was not a soft error, propagating"); return std::unexpected{error};
            }
        }

        std::ignore = m_vk_device.waitForFences(1, &render_fence, VK_TRUE, a_second, m_vk_dispatch);
        std::ignore = m_vk_device.resetFences(1, &render_fence, m_vk_dispatch);

        return {};
    }

private:

    VkRenderPass m_vk_render_pass;

    VkSwapchainKHR m_vk_swapchain;
    VkFormat m_vk_swapchain_format;
    std::vector<VkImage> m_vk_swapchain_images;
    std::vector<VkImageView> m_vk_swapchain_image_views;
    std::vector<VkFramebuffer> m_vk_framebuffers;

    auto init_vk_command_pool() -> std::expected<void, error> {
        const auto command_pool_create_info = vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, m_vk_queue_family_index);
        m_vk_command_pool = TRYX(error::from_vk(m_vk_device.createCommandPool(command_pool_create_info, nullptr, m_vk_dispatch), "could not create a vk::CommandPool"));

        return {};
    }

    auto init_vk_command_buffer() -> std::expected<void, error> {
        const auto command_buffer_allocate_info = vk::CommandBufferAllocateInfo(m_vk_command_pool, vk::CommandBufferLevel::ePrimary, 1);
        const auto command_buffers =
          TRYX(error::from_vk(m_vk_device.allocateCommandBuffers(command_buffer_allocate_info, m_vk_dispatch), "could not create a vector of vk::CommandBuffer"));

        if (command_buffers.empty()) {
            return std::unexpected{error::make("zero command buffers were created despite success")};
        }

        m_vk_command_buffer = command_buffers[0];

        return {};
    }

    auto destroy_vk_swapchain() -> std::expected<void, error> {
        if (!(bool)m_vk_swapchain) {
            return {};
        }

        for (auto const& framebuffer : m_vk_framebuffers) {
            if (!framebuffer) {
                continue;
            }

            m_vk_device.destroy(framebuffer, nullptr, m_vk_dispatch);
        }

        m_vk_device.destroy(m_vk_render_pass, nullptr, m_vk_dispatch);

        std::ranges::for_each(m_vk_swapchain_image_views, [this](auto const& view) { m_vk_device.destroy(view); });

        m_vk_device.destroy(m_vk_swapchain, nullptr, m_vk_dispatch);

        return {};
    }

    auto reinit_vk_swapchain(vk::Extent2D extent) -> std::expected<void, error> {
        const auto surface_capabilities =
          TRYX(error::from_vk(m_vk_physical_device.getSurfaceCapabilitiesKHR(m_vk_surface), "could not fetch surface capabilities from the physical device"));

        const auto surface_formats = TRYX(error::from_vk(m_vk_physical_device.getSurfaceFormatsKHR(m_vk_surface), "could not fetch surface formats from the physical device"));

        for (auto const& [no, format] : surface_formats | std::views::enumerate) {
            spdlog::debug("Surface #{}:", no);
            spdlog::debug("\tColor space: {}", vk::to_string(format.colorSpace));
            spdlog::debug("\tFormat     : {}", vk::to_string(format.format));
        }

        const auto surface_format = surface_formats[0].format == vk::Format::eUndefined ? vk::Format::eR8G8B8A8Unorm : surface_formats[0].format;
        m_vk_swapchain_format = surface_format;

        const auto swapchain_present_mode = vk::PresentModeKHR::eFifo;
        const auto pre_transform = (surface_capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) != vk::SurfaceTransformFlagBitsKHR{}
                                   ? vk::SurfaceTransformFlagBitsKHR::eIdentity
                                   : surface_capabilities.currentTransform;

        const auto composite_alpha = (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) != vk::CompositeAlphaFlagBitsKHR{}
                                     ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
                                   : (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) != vk::CompositeAlphaFlagBitsKHR{}
                                     ? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
                                   : (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit) != vk::CompositeAlphaFlagBitsKHR{}
                                     ? vk::CompositeAlphaFlagBitsKHR::eInherit
                                     : vk::CompositeAlphaFlagBitsKHR::eOpaque;

        const auto swapchain_create_info = vk::SwapchainCreateInfoKHR(
          vk::SwapchainCreateFlagsKHR(),                                                                                              //
          m_vk_surface,                                                                                                               //
          std::clamp(3u, surface_capabilities.minImageCount, surface_capabilities.maxImageCount ?: std::numeric_limits<u32>::max()),  //
          surface_format,                                                                                                             //
          vk::ColorSpaceKHR::eSrgbNonlinear,                                                                                          //
          extent,                                                                                                                     //
          1,                                                                                                                          //
          vk::ImageUsageFlagBits::eColorAttachment,                                                                                   //
          vk::SharingMode::eExclusive,                                                                                                //
          {},                                                                                                                         //
          pre_transform,                                                                                                              //
          composite_alpha,                                                                                                            //
          swapchain_present_mode,                                                                                                     //
          VK_TRUE,                                                                                                                    //
          m_vk_swapchain,                                                                                                             //
          nullptr                                                                                                                     //
        );

        const auto swapchain_temp = TRYX(error::from_vk(m_vk_device.createSwapchainKHR(swapchain_create_info), "could not create a swapchain"));
        TRYX(destroy_vk_swapchain());
        m_vk_swapchain = swapchain_temp;

        m_vk_swapchain_images = TRYX(error::from_vk(m_vk_device.getSwapchainImagesKHR(m_vk_swapchain), "could not get swapchain images"));

        m_vk_swapchain_images = TRYX(error::from_vk(m_vk_device.getSwapchainImagesKHR(m_vk_swapchain), "could not get swapchain images"));

        m_vk_swapchain_image_views = std::vector<vk::ImageView>{};
        m_vk_swapchain_image_views.reserve(m_vk_swapchain_images.size());

        vk::ImageViewCreateInfo imageViewCreateInfo({}, {}, vk::ImageViewType::e2D, surface_format, {}, {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
        for (auto const& [no, image] : m_vk_swapchain_images | std::views::enumerate) {
            imageViewCreateInfo.image = image;
            auto&& image_view = TRYX(error::from_vk(m_vk_device.createImageView(imageViewCreateInfo), "could not create a swapchain image view"));

            m_vk_swapchain_image_views.emplace_back(std::move(image_view));
        }

        auto color_attachment = vk::AttachmentDescription(
          {},                                //
          m_vk_swapchain_format,             //
          vk::SampleCountFlagBits::e1,       //
          vk::AttachmentLoadOp::eClear,      //
          vk::AttachmentStoreOp::eStore,     //
          vk::AttachmentLoadOp::eDontCare,   //
          vk::AttachmentStoreOp::eDontCare,  //
          vk::ImageLayout::eUndefined,       //
          vk::ImageLayout::ePresentSrcKHR    //
        );

        auto color_attachment_ref = vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);

        auto subpass = vk::SubpassDescription(
          {},                                //
          vk::PipelineBindPoint::eGraphics,  //
          0, nullptr,                        //
          1, &color_attachment_ref           //
        );

        auto render_pass_create_info = vk::RenderPassCreateInfo(
          {},                    //
          1, &color_attachment,  //
          1, &subpass            //
        );

        m_vk_render_pass = TRYX(error::from_vk(m_vk_device.createRenderPass(render_pass_create_info, nullptr, m_vk_dispatch), "could not create a render pass"));

        m_vk_framebuffers = std::vector<vk::Framebuffer>{m_vk_swapchain_image_views.size()};
        for (auto const& [no, view] : m_vk_swapchain_image_views | std::views::enumerate) {
            auto framebuffer_create_info = vk::FramebufferCreateInfo(
              {},                                         //
              m_vk_render_pass,                           //
              1, &view,                                   //
              m_window_size.width, m_window_size.height,  //
              1                                           //
            );

            auto&& res = m_vk_device.createFramebuffer(framebuffer_create_info, nullptr, m_vk_dispatch);
            m_vk_framebuffers[no] = TRYX(error::from_vk(std::move(res), fmt::format("could not create a framebuffer (index {})", no)));
        }

        return {};
    }
};
*/

struct foo {
    //=== VK_VERSION_1_0 ===
    PFN_vkCreateInstance                               vkCreateInstance                               = 0;
    PFN_vkDestroyInstance                              vkDestroyInstance                              = 0;
    PFN_vkEnumeratePhysicalDevices                     vkEnumeratePhysicalDevices                     = 0;
    PFN_vkGetPhysicalDeviceFeatures                    vkGetPhysicalDeviceFeatures                    = 0;
    PFN_vkGetPhysicalDeviceFormatProperties            vkGetPhysicalDeviceFormatProperties            = 0;
    PFN_vkGetPhysicalDeviceImageFormatProperties       vkGetPhysicalDeviceImageFormatProperties       = 0;
    PFN_vkGetPhysicalDeviceProperties                  vkGetPhysicalDeviceProperties                  = 0;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties       vkGetPhysicalDeviceQueueFamilyProperties       = 0;
    PFN_vkGetPhysicalDeviceMemoryProperties            vkGetPhysicalDeviceMemoryProperties            = 0;
    PFN_vkGetInstanceProcAddr                          vkGetInstanceProcAddr                          = 0;
    PFN_vkGetDeviceProcAddr                            vkGetDeviceProcAddr                            = 0;
    PFN_vkCreateDevice                                 vkCreateDevice                                 = 0;
    PFN_vkDestroyDevice                                vkDestroyDevice                                = 0;
    PFN_vkEnumerateInstanceExtensionProperties         vkEnumerateInstanceExtensionProperties         = 0;
    PFN_vkEnumerateDeviceExtensionProperties           vkEnumerateDeviceExtensionProperties           = 0;
    PFN_vkEnumerateInstanceLayerProperties             vkEnumerateInstanceLayerProperties             = 0;
    PFN_vkEnumerateDeviceLayerProperties               vkEnumerateDeviceLayerProperties               = 0;
    PFN_vkGetDeviceQueue                               vkGetDeviceQueue                               = 0;
    PFN_vkQueueSubmit                                  vkQueueSubmit                                  = 0;
    PFN_vkQueueWaitIdle                                vkQueueWaitIdle                                = 0;
    PFN_vkDeviceWaitIdle                               vkDeviceWaitIdle                               = 0;
    PFN_vkAllocateMemory                               vkAllocateMemory                               = 0;
    PFN_vkFreeMemory                                   vkFreeMemory                                   = 0;
    PFN_vkMapMemory                                    vkMapMemory                                    = 0;
    PFN_vkUnmapMemory                                  vkUnmapMemory                                  = 0;
    PFN_vkFlushMappedMemoryRanges                      vkFlushMappedMemoryRanges                      = 0;
    PFN_vkInvalidateMappedMemoryRanges                 vkInvalidateMappedMemoryRanges                 = 0;
    PFN_vkGetDeviceMemoryCommitment                    vkGetDeviceMemoryCommitment                    = 0;
    PFN_vkBindBufferMemory                             vkBindBufferMemory                             = 0;
    PFN_vkBindImageMemory                              vkBindImageMemory                              = 0;
    PFN_vkGetBufferMemoryRequirements                  vkGetBufferMemoryRequirements                  = 0;
    PFN_vkGetImageMemoryRequirements                   vkGetImageMemoryRequirements                   = 0;
    PFN_vkGetImageSparseMemoryRequirements             vkGetImageSparseMemoryRequirements             = 0;
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties vkGetPhysicalDeviceSparseImageFormatProperties = 0;
    PFN_vkQueueBindSparse                              vkQueueBindSparse                              = 0;
    PFN_vkCreateFence                                  vkCreateFence                                  = 0;
    PFN_vkDestroyFence                                 vkDestroyFence                                 = 0;
    PFN_vkResetFences                                  vkResetFences                                  = 0;
    PFN_vkGetFenceStatus                               vkGetFenceStatus                               = 0;
    PFN_vkWaitForFences                                vkWaitForFences                                = 0;
    PFN_vkCreateSemaphore                              vkCreateSemaphore                              = 0;
    PFN_vkDestroySemaphore                             vkDestroySemaphore                             = 0;
    PFN_vkCreateEvent                                  vkCreateEvent                                  = 0;
    PFN_vkDestroyEvent                                 vkDestroyEvent                                 = 0;
    PFN_vkGetEventStatus                               vkGetEventStatus                               = 0;
    PFN_vkSetEvent                                     vkSetEvent                                     = 0;
    PFN_vkResetEvent                                   vkResetEvent                                   = 0;
    PFN_vkCreateQueryPool                              vkCreateQueryPool                              = 0;
    PFN_vkDestroyQueryPool                             vkDestroyQueryPool                             = 0;
    PFN_vkGetQueryPoolResults                          vkGetQueryPoolResults                          = 0;
    PFN_vkCreateBuffer                                 vkCreateBuffer                                 = 0;
    PFN_vkDestroyBuffer                                vkDestroyBuffer                                = 0;
    PFN_vkCreateBufferView                             vkCreateBufferView                             = 0;
    PFN_vkDestroyBufferView                            vkDestroyBufferView                            = 0;
    PFN_vkCreateImage                                  vkCreateImage                                  = 0;
    PFN_vkDestroyImage                                 vkDestroyImage                                 = 0;
    PFN_vkGetImageSubresourceLayout                    vkGetImageSubresourceLayout                    = 0;
    PFN_vkCreateImageView                              vkCreateImageView                              = 0;
    PFN_vkDestroyImageView                             vkDestroyImageView                             = 0;
    PFN_vkCreateShaderModule                           vkCreateShaderModule                           = 0;
    PFN_vkDestroyShaderModule                          vkDestroyShaderModule                          = 0;
    PFN_vkCreatePipelineCache                          vkCreatePipelineCache                          = 0;
    PFN_vkDestroyPipelineCache                         vkDestroyPipelineCache                         = 0;
    PFN_vkGetPipelineCacheData                         vkGetPipelineCacheData                         = 0;
    PFN_vkMergePipelineCaches                          vkMergePipelineCaches                          = 0;
    PFN_vkCreateGraphicsPipelines                      vkCreateGraphicsPipelines                      = 0;
    PFN_vkCreateComputePipelines                       vkCreateComputePipelines                       = 0;
    PFN_vkDestroyPipeline                              vkDestroyPipeline                              = 0;
    PFN_vkCreatePipelineLayout                         vkCreatePipelineLayout                         = 0;
    PFN_vkDestroyPipelineLayout                        vkDestroyPipelineLayout                        = 0;
    PFN_vkCreateSampler                                vkCreateSampler                                = 0;
    PFN_vkDestroySampler                               vkDestroySampler                               = 0;
    PFN_vkCreateDescriptorSetLayout                    vkCreateDescriptorSetLayout                    = 0;
    PFN_vkDestroyDescriptorSetLayout                   vkDestroyDescriptorSetLayout                   = 0;
    PFN_vkCreateDescriptorPool                         vkCreateDescriptorPool                         = 0;
    PFN_vkDestroyDescriptorPool                        vkDestroyDescriptorPool                        = 0;
    PFN_vkResetDescriptorPool                          vkResetDescriptorPool                          = 0;
    PFN_vkAllocateDescriptorSets                       vkAllocateDescriptorSets                       = 0;
    PFN_vkFreeDescriptorSets                           vkFreeDescriptorSets                           = 0;
    PFN_vkUpdateDescriptorSets                         vkUpdateDescriptorSets                         = 0;
    PFN_vkCreateFramebuffer                            vkCreateFramebuffer                            = 0;
    PFN_vkDestroyFramebuffer                           vkDestroyFramebuffer                           = 0;
    PFN_vkCreateRenderPass                             vkCreateRenderPass                             = 0;
    PFN_vkDestroyRenderPass                            vkDestroyRenderPass                            = 0;
    PFN_vkGetRenderAreaGranularity                     vkGetRenderAreaGranularity                     = 0;
    PFN_vkCreateCommandPool                            vkCreateCommandPool                            = 0;
    PFN_vkDestroyCommandPool                           vkDestroyCommandPool                           = 0;
    PFN_vkResetCommandPool                             vkResetCommandPool                             = 0;
    PFN_vkAllocateCommandBuffers                       vkAllocateCommandBuffers                       = 0;
    PFN_vkFreeCommandBuffers                           vkFreeCommandBuffers                           = 0;
    PFN_vkBeginCommandBuffer                           vkBeginCommandBuffer                           = 0;
    PFN_vkEndCommandBuffer                             vkEndCommandBuffer                             = 0;
    PFN_vkResetCommandBuffer                           vkResetCommandBuffer                           = 0;
    PFN_vkCmdBindPipeline                              vkCmdBindPipeline                              = 0;
    PFN_vkCmdSetViewport                               vkCmdSetViewport                               = 0;
    PFN_vkCmdSetScissor                                vkCmdSetScissor                                = 0;
    PFN_vkCmdSetLineWidth                              vkCmdSetLineWidth                              = 0;
    PFN_vkCmdSetDepthBias                              vkCmdSetDepthBias                              = 0;
    PFN_vkCmdSetBlendConstants                         vkCmdSetBlendConstants                         = 0;
    PFN_vkCmdSetDepthBounds                            vkCmdSetDepthBounds                            = 0;
    PFN_vkCmdSetStencilCompareMask                     vkCmdSetStencilCompareMask                     = 0;
    PFN_vkCmdSetStencilWriteMask                       vkCmdSetStencilWriteMask                       = 0;
    PFN_vkCmdSetStencilReference                       vkCmdSetStencilReference                       = 0;
    PFN_vkCmdBindDescriptorSets                        vkCmdBindDescriptorSets                        = 0;
    PFN_vkCmdBindIndexBuffer                           vkCmdBindIndexBuffer                           = 0;
    PFN_vkCmdBindVertexBuffers                         vkCmdBindVertexBuffers                         = 0;
    PFN_vkCmdDraw                                      vkCmdDraw                                      = 0;
    PFN_vkCmdDrawIndexed                               vkCmdDrawIndexed                               = 0;
    PFN_vkCmdDrawIndirect                              vkCmdDrawIndirect                              = 0;
    PFN_vkCmdDrawIndexedIndirect                       vkCmdDrawIndexedIndirect                       = 0;
    PFN_vkCmdDispatch                                  vkCmdDispatch                                  = 0;
    PFN_vkCmdDispatchIndirect                          vkCmdDispatchIndirect                          = 0;
    PFN_vkCmdCopyBuffer                                vkCmdCopyBuffer                                = 0;
    PFN_vkCmdCopyImage                                 vkCmdCopyImage                                 = 0;
    PFN_vkCmdBlitImage                                 vkCmdBlitImage                                 = 0;
    PFN_vkCmdCopyBufferToImage                         vkCmdCopyBufferToImage                         = 0;
    PFN_vkCmdCopyImageToBuffer                         vkCmdCopyImageToBuffer                         = 0;
    PFN_vkCmdUpdateBuffer                              vkCmdUpdateBuffer                              = 0;
    PFN_vkCmdFillBuffer                                vkCmdFillBuffer                                = 0;
    PFN_vkCmdClearColorImage                           vkCmdClearColorImage                           = 0;
    PFN_vkCmdClearDepthStencilImage                    vkCmdClearDepthStencilImage                    = 0;
    PFN_vkCmdClearAttachments                          vkCmdClearAttachments                          = 0;
    PFN_vkCmdResolveImage                              vkCmdResolveImage                              = 0;
    PFN_vkCmdSetEvent                                  vkCmdSetEvent                                  = 0;
    PFN_vkCmdResetEvent                                vkCmdResetEvent                                = 0;
    PFN_vkCmdWaitEvents                                vkCmdWaitEvents                                = 0;
    PFN_vkCmdPipelineBarrier                           vkCmdPipelineBarrier                           = 0;
    PFN_vkCmdBeginQuery                                vkCmdBeginQuery                                = 0;
    PFN_vkCmdEndQuery                                  vkCmdEndQuery                                  = 0;
    PFN_vkCmdResetQueryPool                            vkCmdResetQueryPool                            = 0;
    PFN_vkCmdWriteTimestamp                            vkCmdWriteTimestamp                            = 0;
    PFN_vkCmdCopyQueryPoolResults                      vkCmdCopyQueryPoolResults                      = 0;
    PFN_vkCmdPushConstants                             vkCmdPushConstants                             = 0;
    PFN_vkCmdBeginRenderPass                           vkCmdBeginRenderPass                           = 0;
    PFN_vkCmdNextSubpass                               vkCmdNextSubpass                               = 0;
    PFN_vkCmdEndRenderPass                             vkCmdEndRenderPass                             = 0;
    PFN_vkCmdExecuteCommands                           vkCmdExecuteCommands                           = 0;

    //=== VK_VERSION_1_1 ===
    PFN_vkEnumerateInstanceVersion                      vkEnumerateInstanceVersion                      = 0;
    PFN_vkBindBufferMemory2                             vkBindBufferMemory2                             = 0;
    PFN_vkBindImageMemory2                              vkBindImageMemory2                              = 0;
    PFN_vkGetDeviceGroupPeerMemoryFeatures              vkGetDeviceGroupPeerMemoryFeatures              = 0;
    PFN_vkCmdSetDeviceMask                              vkCmdSetDeviceMask                              = 0;
    PFN_vkCmdDispatchBase                               vkCmdDispatchBase                               = 0;
    PFN_vkEnumeratePhysicalDeviceGroups                 vkEnumeratePhysicalDeviceGroups                 = 0;
    PFN_vkGetImageMemoryRequirements2                   vkGetImageMemoryRequirements2                   = 0;
    PFN_vkGetBufferMemoryRequirements2                  vkGetBufferMemoryRequirements2                  = 0;
    PFN_vkGetImageSparseMemoryRequirements2             vkGetImageSparseMemoryRequirements2             = 0;
    PFN_vkGetPhysicalDeviceFeatures2                    vkGetPhysicalDeviceFeatures2                    = 0;
    PFN_vkGetPhysicalDeviceProperties2                  vkGetPhysicalDeviceProperties2                  = 0;
    PFN_vkGetPhysicalDeviceFormatProperties2            vkGetPhysicalDeviceFormatProperties2            = 0;
    PFN_vkGetPhysicalDeviceImageFormatProperties2       vkGetPhysicalDeviceImageFormatProperties2       = 0;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties2       vkGetPhysicalDeviceQueueFamilyProperties2       = 0;
    PFN_vkGetPhysicalDeviceMemoryProperties2            vkGetPhysicalDeviceMemoryProperties2            = 0;
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 vkGetPhysicalDeviceSparseImageFormatProperties2 = 0;
    PFN_vkTrimCommandPool                               vkTrimCommandPool                               = 0;
    PFN_vkGetDeviceQueue2                               vkGetDeviceQueue2                               = 0;
    PFN_vkCreateSamplerYcbcrConversion                  vkCreateSamplerYcbcrConversion                  = 0;
    PFN_vkDestroySamplerYcbcrConversion                 vkDestroySamplerYcbcrConversion                 = 0;
    PFN_vkCreateDescriptorUpdateTemplate                vkCreateDescriptorUpdateTemplate                = 0;
    PFN_vkDestroyDescriptorUpdateTemplate               vkDestroyDescriptorUpdateTemplate               = 0;
    PFN_vkUpdateDescriptorSetWithTemplate               vkUpdateDescriptorSetWithTemplate               = 0;
    PFN_vkGetPhysicalDeviceExternalBufferProperties     vkGetPhysicalDeviceExternalBufferProperties     = 0;
    PFN_vkGetPhysicalDeviceExternalFenceProperties      vkGetPhysicalDeviceExternalFenceProperties      = 0;
    PFN_vkGetPhysicalDeviceExternalSemaphoreProperties  vkGetPhysicalDeviceExternalSemaphoreProperties  = 0;
    PFN_vkGetDescriptorSetLayoutSupport                 vkGetDescriptorSetLayoutSupport                 = 0;

    //=== VK_VERSION_1_2 ===
    PFN_vkCmdDrawIndirectCount                vkCmdDrawIndirectCount                = 0;
    PFN_vkCmdDrawIndexedIndirectCount         vkCmdDrawIndexedIndirectCount         = 0;
    PFN_vkCreateRenderPass2                   vkCreateRenderPass2                   = 0;
    PFN_vkCmdBeginRenderPass2                 vkCmdBeginRenderPass2                 = 0;
    PFN_vkCmdNextSubpass2                     vkCmdNextSubpass2                     = 0;
    PFN_vkCmdEndRenderPass2                   vkCmdEndRenderPass2                   = 0;
    PFN_vkResetQueryPool                      vkResetQueryPool                      = 0;
    PFN_vkGetSemaphoreCounterValue            vkGetSemaphoreCounterValue            = 0;
    PFN_vkWaitSemaphores                      vkWaitSemaphores                      = 0;
    PFN_vkSignalSemaphore                     vkSignalSemaphore                     = 0;
    PFN_vkGetBufferDeviceAddress              vkGetBufferDeviceAddress              = 0;
    PFN_vkGetBufferOpaqueCaptureAddress       vkGetBufferOpaqueCaptureAddress       = 0;
    PFN_vkGetDeviceMemoryOpaqueCaptureAddress vkGetDeviceMemoryOpaqueCaptureAddress = 0;

    //=== VK_VERSION_1_3 ===
    PFN_vkGetPhysicalDeviceToolProperties        vkGetPhysicalDeviceToolProperties        = 0;
    PFN_vkCreatePrivateDataSlot                  vkCreatePrivateDataSlot                  = 0;
    PFN_vkDestroyPrivateDataSlot                 vkDestroyPrivateDataSlot                 = 0;
    PFN_vkSetPrivateData                         vkSetPrivateData                         = 0;
    PFN_vkGetPrivateData                         vkGetPrivateData                         = 0;
    PFN_vkCmdSetEvent2                           vkCmdSetEvent2                           = 0;
    PFN_vkCmdResetEvent2                         vkCmdResetEvent2                         = 0;
    PFN_vkCmdWaitEvents2                         vkCmdWaitEvents2                         = 0;
    PFN_vkCmdPipelineBarrier2                    vkCmdPipelineBarrier2                    = 0;
    PFN_vkCmdWriteTimestamp2                     vkCmdWriteTimestamp2                     = 0;
    PFN_vkQueueSubmit2                           vkQueueSubmit2                           = 0;
    PFN_vkCmdCopyBuffer2                         vkCmdCopyBuffer2                         = 0;
    PFN_vkCmdCopyImage2                          vkCmdCopyImage2                          = 0;
    PFN_vkCmdCopyBufferToImage2                  vkCmdCopyBufferToImage2                  = 0;
    PFN_vkCmdCopyImageToBuffer2                  vkCmdCopyImageToBuffer2                  = 0;
    PFN_vkCmdBlitImage2                          vkCmdBlitImage2                          = 0;
    PFN_vkCmdResolveImage2                       vkCmdResolveImage2                       = 0;
    PFN_vkCmdBeginRendering                      vkCmdBeginRendering                      = 0;
    PFN_vkCmdEndRendering                        vkCmdEndRendering                        = 0;
    PFN_vkCmdSetCullMode                         vkCmdSetCullMode                         = 0;
    PFN_vkCmdSetFrontFace                        vkCmdSetFrontFace                        = 0;
    PFN_vkCmdSetPrimitiveTopology                vkCmdSetPrimitiveTopology                = 0;
    PFN_vkCmdSetViewportWithCount                vkCmdSetViewportWithCount                = 0;
    PFN_vkCmdSetScissorWithCount                 vkCmdSetScissorWithCount                 = 0;
    PFN_vkCmdBindVertexBuffers2                  vkCmdBindVertexBuffers2                  = 0;
    PFN_vkCmdSetDepthTestEnable                  vkCmdSetDepthTestEnable                  = 0;
    PFN_vkCmdSetDepthWriteEnable                 vkCmdSetDepthWriteEnable                 = 0;
    PFN_vkCmdSetDepthCompareOp                   vkCmdSetDepthCompareOp                   = 0;
    PFN_vkCmdSetDepthBoundsTestEnable            vkCmdSetDepthBoundsTestEnable            = 0;
    PFN_vkCmdSetStencilTestEnable                vkCmdSetStencilTestEnable                = 0;
    PFN_vkCmdSetStencilOp                        vkCmdSetStencilOp                        = 0;
    PFN_vkCmdSetRasterizerDiscardEnable          vkCmdSetRasterizerDiscardEnable          = 0;
    PFN_vkCmdSetDepthBiasEnable                  vkCmdSetDepthBiasEnable                  = 0;
    PFN_vkCmdSetPrimitiveRestartEnable           vkCmdSetPrimitiveRestartEnable           = 0;
    PFN_vkGetDeviceBufferMemoryRequirements      vkGetDeviceBufferMemoryRequirements      = 0;
    PFN_vkGetDeviceImageMemoryRequirements       vkGetDeviceImageMemoryRequirements       = 0;
    PFN_vkGetDeviceImageSparseMemoryRequirements vkGetDeviceImageSparseMemoryRequirements = 0;

    //=== VK_KHR_surface ===
    PFN_vkDestroySurfaceKHR                       vkDestroySurfaceKHR                       = 0;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR      vkGetPhysicalDeviceSurfaceSupportKHR      = 0;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR = 0;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR      vkGetPhysicalDeviceSurfaceFormatsKHR      = 0;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR = 0;

    //=== VK_KHR_swapchain ===
    PFN_vkCreateSwapchainKHR                    vkCreateSwapchainKHR                    = 0;
    PFN_vkDestroySwapchainKHR                   vkDestroySwapchainKHR                   = 0;
    PFN_vkGetSwapchainImagesKHR                 vkGetSwapchainImagesKHR                 = 0;
    PFN_vkAcquireNextImageKHR                   vkAcquireNextImageKHR                   = 0;
    PFN_vkQueuePresentKHR                       vkQueuePresentKHR                       = 0;
    PFN_vkGetDeviceGroupPresentCapabilitiesKHR  vkGetDeviceGroupPresentCapabilitiesKHR  = 0;
    PFN_vkGetDeviceGroupSurfacePresentModesKHR  vkGetDeviceGroupSurfacePresentModesKHR  = 0;
    PFN_vkGetPhysicalDevicePresentRectanglesKHR vkGetPhysicalDevicePresentRectanglesKHR = 0;
    PFN_vkAcquireNextImage2KHR                  vkAcquireNextImage2KHR                  = 0;

    //=== VK_KHR_display ===
    PFN_vkGetPhysicalDeviceDisplayPropertiesKHR      vkGetPhysicalDeviceDisplayPropertiesKHR      = 0;
    PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR vkGetPhysicalDeviceDisplayPlanePropertiesKHR = 0;
    PFN_vkGetDisplayPlaneSupportedDisplaysKHR        vkGetDisplayPlaneSupportedDisplaysKHR        = 0;
    PFN_vkGetDisplayModePropertiesKHR                vkGetDisplayModePropertiesKHR                = 0;
    PFN_vkCreateDisplayModeKHR                       vkCreateDisplayModeKHR                       = 0;
    PFN_vkGetDisplayPlaneCapabilitiesKHR             vkGetDisplayPlaneCapabilitiesKHR             = 0;
    PFN_vkCreateDisplayPlaneSurfaceKHR               vkCreateDisplayPlaneSurfaceKHR               = 0;

    //=== VK_KHR_display_swapchain ===
    PFN_vkCreateSharedSwapchainsKHR vkCreateSharedSwapchainsKHR = 0;

#if defined( VK_USE_PLATFORM_XLIB_KHR )
    //=== VK_KHR_xlib_surface ===
    PFN_vkCreateXlibSurfaceKHR                        vkCreateXlibSurfaceKHR                        = 0;
    PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR vkGetPhysicalDeviceXlibPresentationSupportKHR = 0;
#else
    PFN_dummy vkCreateXlibSurfaceKHR_placeholder                                  = 0;
    PFN_dummy vkGetPhysicalDeviceXlibPresentationSupportKHR_placeholder           = 0;
#endif /*VK_USE_PLATFORM_XLIB_KHR*/

#if defined( VK_USE_PLATFORM_XCB_KHR )
    //=== VK_KHR_xcb_surface ===
    PFN_vkCreateXcbSurfaceKHR                        vkCreateXcbSurfaceKHR                        = 0;
    PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR vkGetPhysicalDeviceXcbPresentationSupportKHR = 0;
#else
    PFN_dummy vkCreateXcbSurfaceKHR_placeholder                                   = 0;
    PFN_dummy vkGetPhysicalDeviceXcbPresentationSupportKHR_placeholder            = 0;
#endif /*VK_USE_PLATFORM_XCB_KHR*/

#if defined( VK_USE_PLATFORM_WAYLAND_KHR )
    //=== VK_KHR_wayland_surface ===
    PFN_vkCreateWaylandSurfaceKHR                        vkCreateWaylandSurfaceKHR                        = 0;
    PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR vkGetPhysicalDeviceWaylandPresentationSupportKHR = 0;
#else
    PFN_dummy vkCreateWaylandSurfaceKHR_placeholder                               = 0;
    PFN_dummy vkGetPhysicalDeviceWaylandPresentationSupportKHR_placeholder        = 0;
#endif /*VK_USE_PLATFORM_WAYLAND_KHR*/

#if defined( VK_USE_PLATFORM_ANDROID_KHR )
    //=== VK_KHR_android_surface ===
    PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR = 0;
#else
    PFN_dummy vkCreateAndroidSurfaceKHR_placeholder                               = 0;
#endif /*VK_USE_PLATFORM_ANDROID_KHR*/

#if defined( VK_USE_PLATFORM_WIN32_KHR )
    //=== VK_KHR_win32_surface ===
    PFN_vkCreateWin32SurfaceKHR                        vkCreateWin32SurfaceKHR                        = 0;
    PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vkGetPhysicalDeviceWin32PresentationSupportKHR = 0;
#else
    PFN_dummy vkCreateWin32SurfaceKHR_placeholder                                 = 0;
    PFN_dummy vkGetPhysicalDeviceWin32PresentationSupportKHR_placeholder          = 0;
#endif /*VK_USE_PLATFORM_WIN32_KHR*/

    //=== VK_EXT_debug_report ===
    PFN_vkCreateDebugReportCallbackEXT  vkCreateDebugReportCallbackEXT  = 0;
    PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = 0;
    PFN_vkDebugReportMessageEXT         vkDebugReportMessageEXT         = 0;

    //=== VK_EXT_debug_marker ===
    PFN_vkDebugMarkerSetObjectTagEXT  vkDebugMarkerSetObjectTagEXT  = 0;
    PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectNameEXT = 0;
    PFN_vkCmdDebugMarkerBeginEXT      vkCmdDebugMarkerBeginEXT      = 0;
    PFN_vkCmdDebugMarkerEndEXT        vkCmdDebugMarkerEndEXT        = 0;
    PFN_vkCmdDebugMarkerInsertEXT     vkCmdDebugMarkerInsertEXT     = 0;

    //=== VK_KHR_video_queue ===
    PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR     vkGetPhysicalDeviceVideoCapabilitiesKHR     = 0;
    PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR vkGetPhysicalDeviceVideoFormatPropertiesKHR = 0;
    PFN_vkCreateVideoSessionKHR                     vkCreateVideoSessionKHR                     = 0;
    PFN_vkDestroyVideoSessionKHR                    vkDestroyVideoSessionKHR                    = 0;
    PFN_vkGetVideoSessionMemoryRequirementsKHR      vkGetVideoSessionMemoryRequirementsKHR      = 0;
    PFN_vkBindVideoSessionMemoryKHR                 vkBindVideoSessionMemoryKHR                 = 0;
    PFN_vkCreateVideoSessionParametersKHR           vkCreateVideoSessionParametersKHR           = 0;
    PFN_vkUpdateVideoSessionParametersKHR           vkUpdateVideoSessionParametersKHR           = 0;
    PFN_vkDestroyVideoSessionParametersKHR          vkDestroyVideoSessionParametersKHR          = 0;
    PFN_vkCmdBeginVideoCodingKHR                    vkCmdBeginVideoCodingKHR                    = 0;
    PFN_vkCmdEndVideoCodingKHR                      vkCmdEndVideoCodingKHR                      = 0;
    PFN_vkCmdControlVideoCodingKHR                  vkCmdControlVideoCodingKHR                  = 0;

    //=== VK_KHR_video_decode_queue ===
    PFN_vkCmdDecodeVideoKHR vkCmdDecodeVideoKHR = 0;

    //=== VK_EXT_transform_feedback ===
    PFN_vkCmdBindTransformFeedbackBuffersEXT vkCmdBindTransformFeedbackBuffersEXT = 0;
    PFN_vkCmdBeginTransformFeedbackEXT       vkCmdBeginTransformFeedbackEXT       = 0;
    PFN_vkCmdEndTransformFeedbackEXT         vkCmdEndTransformFeedbackEXT         = 0;
    PFN_vkCmdBeginQueryIndexedEXT            vkCmdBeginQueryIndexedEXT            = 0;
    PFN_vkCmdEndQueryIndexedEXT              vkCmdEndQueryIndexedEXT              = 0;
    PFN_vkCmdDrawIndirectByteCountEXT        vkCmdDrawIndirectByteCountEXT        = 0;

    //=== VK_NVX_binary_import ===
    PFN_vkCreateCuModuleNVX    vkCreateCuModuleNVX    = 0;
    PFN_vkCreateCuFunctionNVX  vkCreateCuFunctionNVX  = 0;
    PFN_vkDestroyCuModuleNVX   vkDestroyCuModuleNVX   = 0;
    PFN_vkDestroyCuFunctionNVX vkDestroyCuFunctionNVX = 0;
    PFN_vkCmdCuLaunchKernelNVX vkCmdCuLaunchKernelNVX = 0;

    //=== VK_NVX_image_view_handle ===
    PFN_vkGetImageViewHandleNVX  vkGetImageViewHandleNVX  = 0;
    PFN_vkGetImageViewAddressNVX vkGetImageViewAddressNVX = 0;

    //=== VK_AMD_draw_indirect_count ===
    PFN_vkCmdDrawIndirectCountAMD        vkCmdDrawIndirectCountAMD        = 0;
    PFN_vkCmdDrawIndexedIndirectCountAMD vkCmdDrawIndexedIndirectCountAMD = 0;

    //=== VK_AMD_shader_info ===
    PFN_vkGetShaderInfoAMD vkGetShaderInfoAMD = 0;

    //=== VK_KHR_dynamic_rendering ===
    PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR = 0;
    PFN_vkCmdEndRenderingKHR   vkCmdEndRenderingKHR   = 0;

#if defined( VK_USE_PLATFORM_GGP )
    //=== VK_GGP_stream_descriptor_surface ===
    PFN_vkCreateStreamDescriptorSurfaceGGP vkCreateStreamDescriptorSurfaceGGP = 0;
#else
    PFN_dummy vkCreateStreamDescriptorSurfaceGGP_placeholder                      = 0;
#endif /*VK_USE_PLATFORM_GGP*/

    //=== VK_NV_external_memory_capabilities ===
    PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV vkGetPhysicalDeviceExternalImageFormatPropertiesNV = 0;

#if defined( VK_USE_PLATFORM_WIN32_KHR )
    //=== VK_NV_external_memory_win32 ===
    PFN_vkGetMemoryWin32HandleNV vkGetMemoryWin32HandleNV = 0;
#else
    PFN_dummy vkGetMemoryWin32HandleNV_placeholder                                = 0;
#endif /*VK_USE_PLATFORM_WIN32_KHR*/

    //=== VK_KHR_get_physical_device_properties2 ===
    PFN_vkGetPhysicalDeviceFeatures2KHR                    vkGetPhysicalDeviceFeatures2KHR                    = 0;
    PFN_vkGetPhysicalDeviceProperties2KHR                  vkGetPhysicalDeviceProperties2KHR                  = 0;
    PFN_vkGetPhysicalDeviceFormatProperties2KHR            vkGetPhysicalDeviceFormatProperties2KHR            = 0;
    PFN_vkGetPhysicalDeviceImageFormatProperties2KHR       vkGetPhysicalDeviceImageFormatProperties2KHR       = 0;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR       vkGetPhysicalDeviceQueueFamilyProperties2KHR       = 0;
    PFN_vkGetPhysicalDeviceMemoryProperties2KHR            vkGetPhysicalDeviceMemoryProperties2KHR            = 0;
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR vkGetPhysicalDeviceSparseImageFormatProperties2KHR = 0;

    //=== VK_KHR_device_group ===
    PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR vkGetDeviceGroupPeerMemoryFeaturesKHR = 0;
    PFN_vkCmdSetDeviceMaskKHR                 vkCmdSetDeviceMaskKHR                 = 0;
    PFN_vkCmdDispatchBaseKHR                  vkCmdDispatchBaseKHR                  = 0;

#if defined( VK_USE_PLATFORM_VI_NN )
    //=== VK_NN_vi_surface ===
    PFN_vkCreateViSurfaceNN vkCreateViSurfaceNN = 0;
#else
    PFN_dummy vkCreateViSurfaceNN_placeholder                                     = 0;
#endif /*VK_USE_PLATFORM_VI_NN*/

    //=== VK_KHR_maintenance1 ===
    PFN_vkTrimCommandPoolKHR vkTrimCommandPoolKHR = 0;

    //=== VK_KHR_device_group_creation ===
    PFN_vkEnumeratePhysicalDeviceGroupsKHR vkEnumeratePhysicalDeviceGroupsKHR = 0;

    //=== VK_KHR_external_memory_capabilities ===
    PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR vkGetPhysicalDeviceExternalBufferPropertiesKHR = 0;

#if defined( VK_USE_PLATFORM_WIN32_KHR )
    //=== VK_KHR_external_memory_win32 ===
    PFN_vkGetMemoryWin32HandleKHR           vkGetMemoryWin32HandleKHR           = 0;
    PFN_vkGetMemoryWin32HandlePropertiesKHR vkGetMemoryWin32HandlePropertiesKHR = 0;
#else
    PFN_dummy vkGetMemoryWin32HandleKHR_placeholder                               = 0;
    PFN_dummy vkGetMemoryWin32HandlePropertiesKHR_placeholder                     = 0;
#endif /*VK_USE_PLATFORM_WIN32_KHR*/

    //=== VK_KHR_external_memory_fd ===
    PFN_vkGetMemoryFdKHR           vkGetMemoryFdKHR           = 0;
    PFN_vkGetMemoryFdPropertiesKHR vkGetMemoryFdPropertiesKHR = 0;

    //=== VK_KHR_external_semaphore_capabilities ===
    PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR vkGetPhysicalDeviceExternalSemaphorePropertiesKHR = 0;

#if defined( VK_USE_PLATFORM_WIN32_KHR )
    //=== VK_KHR_external_semaphore_win32 ===
    PFN_vkImportSemaphoreWin32HandleKHR vkImportSemaphoreWin32HandleKHR = 0;
    PFN_vkGetSemaphoreWin32HandleKHR    vkGetSemaphoreWin32HandleKHR    = 0;
#else
    PFN_dummy vkImportSemaphoreWin32HandleKHR_placeholder                         = 0;
    PFN_dummy vkGetSemaphoreWin32HandleKHR_placeholder                            = 0;
#endif /*VK_USE_PLATFORM_WIN32_KHR*/

    //=== VK_KHR_external_semaphore_fd ===
    PFN_vkImportSemaphoreFdKHR vkImportSemaphoreFdKHR = 0;
    PFN_vkGetSemaphoreFdKHR    vkGetSemaphoreFdKHR    = 0;

    //=== VK_KHR_push_descriptor ===
    PFN_vkCmdPushDescriptorSetKHR             vkCmdPushDescriptorSetKHR             = 0;
    PFN_vkCmdPushDescriptorSetWithTemplateKHR vkCmdPushDescriptorSetWithTemplateKHR = 0;

    //=== VK_EXT_conditional_rendering ===
    PFN_vkCmdBeginConditionalRenderingEXT vkCmdBeginConditionalRenderingEXT = 0;
    PFN_vkCmdEndConditionalRenderingEXT   vkCmdEndConditionalRenderingEXT   = 0;

    //=== VK_KHR_descriptor_update_template ===
    PFN_vkCreateDescriptorUpdateTemplateKHR  vkCreateDescriptorUpdateTemplateKHR  = 0;
    PFN_vkDestroyDescriptorUpdateTemplateKHR vkDestroyDescriptorUpdateTemplateKHR = 0;
    PFN_vkUpdateDescriptorSetWithTemplateKHR vkUpdateDescriptorSetWithTemplateKHR = 0;

    //=== VK_NV_clip_space_w_scaling ===
    PFN_vkCmdSetViewportWScalingNV vkCmdSetViewportWScalingNV = 0;

    //=== VK_EXT_direct_mode_display ===
    PFN_vkReleaseDisplayEXT vkReleaseDisplayEXT = 0;

#if defined( VK_USE_PLATFORM_XLIB_XRANDR_EXT )
    //=== VK_EXT_acquire_xlib_display ===
    PFN_vkAcquireXlibDisplayEXT    vkAcquireXlibDisplayEXT    = 0;
    PFN_vkGetRandROutputDisplayEXT vkGetRandROutputDisplayEXT = 0;
#else
    PFN_dummy vkAcquireXlibDisplayEXT_placeholder                                 = 0;
    PFN_dummy vkGetRandROutputDisplayEXT_placeholder                              = 0;
#endif /*VK_USE_PLATFORM_XLIB_XRANDR_EXT*/

    //=== VK_EXT_display_surface_counter ===
    PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT vkGetPhysicalDeviceSurfaceCapabilities2EXT = 0;

    //=== VK_EXT_display_control ===
    PFN_vkDisplayPowerControlEXT  vkDisplayPowerControlEXT  = 0;
    PFN_vkRegisterDeviceEventEXT  vkRegisterDeviceEventEXT  = 0;
    PFN_vkRegisterDisplayEventEXT vkRegisterDisplayEventEXT = 0;
    PFN_vkGetSwapchainCounterEXT  vkGetSwapchainCounterEXT  = 0;

    //=== VK_GOOGLE_display_timing ===
    PFN_vkGetRefreshCycleDurationGOOGLE   vkGetRefreshCycleDurationGOOGLE   = 0;
    PFN_vkGetPastPresentationTimingGOOGLE vkGetPastPresentationTimingGOOGLE = 0;

    //=== VK_EXT_discard_rectangles ===
    PFN_vkCmdSetDiscardRectangleEXT       vkCmdSetDiscardRectangleEXT       = 0;
    PFN_vkCmdSetDiscardRectangleEnableEXT vkCmdSetDiscardRectangleEnableEXT = 0;
    PFN_vkCmdSetDiscardRectangleModeEXT   vkCmdSetDiscardRectangleModeEXT   = 0;

    //=== VK_EXT_hdr_metadata ===
    PFN_vkSetHdrMetadataEXT vkSetHdrMetadataEXT = 0;

    //=== VK_KHR_create_renderpass2 ===
    PFN_vkCreateRenderPass2KHR   vkCreateRenderPass2KHR   = 0;
    PFN_vkCmdBeginRenderPass2KHR vkCmdBeginRenderPass2KHR = 0;
    PFN_vkCmdNextSubpass2KHR     vkCmdNextSubpass2KHR     = 0;
    PFN_vkCmdEndRenderPass2KHR   vkCmdEndRenderPass2KHR   = 0;

    //=== VK_KHR_shared_presentable_image ===
    PFN_vkGetSwapchainStatusKHR vkGetSwapchainStatusKHR = 0;

    //=== VK_KHR_external_fence_capabilities ===
    PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR vkGetPhysicalDeviceExternalFencePropertiesKHR = 0;

#if defined( VK_USE_PLATFORM_WIN32_KHR )
    //=== VK_KHR_external_fence_win32 ===
    PFN_vkImportFenceWin32HandleKHR vkImportFenceWin32HandleKHR = 0;
    PFN_vkGetFenceWin32HandleKHR    vkGetFenceWin32HandleKHR    = 0;
#else
    PFN_dummy vkImportFenceWin32HandleKHR_placeholder                             = 0;
    PFN_dummy vkGetFenceWin32HandleKHR_placeholder                                = 0;
#endif /*VK_USE_PLATFORM_WIN32_KHR*/

    //=== VK_KHR_external_fence_fd ===
    PFN_vkImportFenceFdKHR vkImportFenceFdKHR = 0;
    PFN_vkGetFenceFdKHR    vkGetFenceFdKHR    = 0;

    //=== VK_KHR_performance_query ===
    PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR = 0;
    PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR         vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR         = 0;
    PFN_vkAcquireProfilingLockKHR                                       vkAcquireProfilingLockKHR                                       = 0;
    PFN_vkReleaseProfilingLockKHR                                       vkReleaseProfilingLockKHR                                       = 0;

    //=== VK_KHR_get_surface_capabilities2 ===
    PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR vkGetPhysicalDeviceSurfaceCapabilities2KHR = 0;
    PFN_vkGetPhysicalDeviceSurfaceFormats2KHR      vkGetPhysicalDeviceSurfaceFormats2KHR      = 0;

    //=== VK_KHR_get_display_properties2 ===
    PFN_vkGetPhysicalDeviceDisplayProperties2KHR      vkGetPhysicalDeviceDisplayProperties2KHR      = 0;
    PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR vkGetPhysicalDeviceDisplayPlaneProperties2KHR = 0;
    PFN_vkGetDisplayModeProperties2KHR                vkGetDisplayModeProperties2KHR                = 0;
    PFN_vkGetDisplayPlaneCapabilities2KHR             vkGetDisplayPlaneCapabilities2KHR             = 0;

#if defined( VK_USE_PLATFORM_IOS_MVK )
    //=== VK_MVK_ios_surface ===
    PFN_vkCreateIOSSurfaceMVK vkCreateIOSSurfaceMVK = 0;
#else
    PFN_dummy vkCreateIOSSurfaceMVK_placeholder                                   = 0;
#endif /*VK_USE_PLATFORM_IOS_MVK*/

#if defined( VK_USE_PLATFORM_MACOS_MVK )
    //=== VK_MVK_macos_surface ===
    PFN_vkCreateMacOSSurfaceMVK vkCreateMacOSSurfaceMVK = 0;
#else
    PFN_dummy vkCreateMacOSSurfaceMVK_placeholder                                 = 0;
#endif /*VK_USE_PLATFORM_MACOS_MVK*/

    //=== VK_EXT_debug_utils ===
    PFN_vkSetDebugUtilsObjectNameEXT    vkSetDebugUtilsObjectNameEXT    = 0;
    PFN_vkSetDebugUtilsObjectTagEXT     vkSetDebugUtilsObjectTagEXT     = 0;
    PFN_vkQueueBeginDebugUtilsLabelEXT  vkQueueBeginDebugUtilsLabelEXT  = 0;
    PFN_vkQueueEndDebugUtilsLabelEXT    vkQueueEndDebugUtilsLabelEXT    = 0;
    PFN_vkQueueInsertDebugUtilsLabelEXT vkQueueInsertDebugUtilsLabelEXT = 0;
    PFN_vkCmdBeginDebugUtilsLabelEXT    vkCmdBeginDebugUtilsLabelEXT    = 0;
    PFN_vkCmdEndDebugUtilsLabelEXT      vkCmdEndDebugUtilsLabelEXT      = 0;
    PFN_vkCmdInsertDebugUtilsLabelEXT   vkCmdInsertDebugUtilsLabelEXT   = 0;
    PFN_vkCreateDebugUtilsMessengerEXT  vkCreateDebugUtilsMessengerEXT  = 0;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = 0;
    PFN_vkSubmitDebugUtilsMessageEXT    vkSubmitDebugUtilsMessageEXT    = 0;

#if defined( VK_USE_PLATFORM_ANDROID_KHR )
    //=== VK_ANDROID_external_memory_android_hardware_buffer ===
    PFN_vkGetAndroidHardwareBufferPropertiesANDROID vkGetAndroidHardwareBufferPropertiesANDROID = 0;
    PFN_vkGetMemoryAndroidHardwareBufferANDROID     vkGetMemoryAndroidHardwareBufferANDROID     = 0;
#else
    PFN_dummy vkGetAndroidHardwareBufferPropertiesANDROID_placeholder             = 0;
    PFN_dummy vkGetMemoryAndroidHardwareBufferANDROID_placeholder                 = 0;
#endif /*VK_USE_PLATFORM_ANDROID_KHR*/

#if defined( VK_ENABLE_BETA_EXTENSIONS )
    //=== VK_AMDX_shader_enqueue ===
    PFN_vkCreateExecutionGraphPipelinesAMDX        vkCreateExecutionGraphPipelinesAMDX        = 0;
    PFN_vkGetExecutionGraphPipelineScratchSizeAMDX vkGetExecutionGraphPipelineScratchSizeAMDX = 0;
    PFN_vkGetExecutionGraphPipelineNodeIndexAMDX   vkGetExecutionGraphPipelineNodeIndexAMDX   = 0;
    PFN_vkCmdInitializeGraphScratchMemoryAMDX      vkCmdInitializeGraphScratchMemoryAMDX      = 0;
    PFN_vkCmdDispatchGraphAMDX                     vkCmdDispatchGraphAMDX                     = 0;
    PFN_vkCmdDispatchGraphIndirectAMDX             vkCmdDispatchGraphIndirectAMDX             = 0;
    PFN_vkCmdDispatchGraphIndirectCountAMDX        vkCmdDispatchGraphIndirectCountAMDX        = 0;
#else
    PFN_dummy vkCreateExecutionGraphPipelinesAMDX_placeholder                     = 0;
    PFN_dummy vkGetExecutionGraphPipelineScratchSizeAMDX_placeholder              = 0;
    PFN_dummy vkGetExecutionGraphPipelineNodeIndexAMDX_placeholder                = 0;
    PFN_dummy vkCmdInitializeGraphScratchMemoryAMDX_placeholder                   = 0;
    PFN_dummy vkCmdDispatchGraphAMDX_placeholder                                  = 0;
    PFN_dummy vkCmdDispatchGraphIndirectAMDX_placeholder                          = 0;
    PFN_dummy vkCmdDispatchGraphIndirectCountAMDX_placeholder                     = 0;
#endif /*VK_ENABLE_BETA_EXTENSIONS*/

    //=== VK_EXT_sample_locations ===
    PFN_vkCmdSetSampleLocationsEXT                  vkCmdSetSampleLocationsEXT                  = 0;
    PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT vkGetPhysicalDeviceMultisamplePropertiesEXT = 0;

    //=== VK_KHR_get_memory_requirements2 ===
    PFN_vkGetImageMemoryRequirements2KHR       vkGetImageMemoryRequirements2KHR       = 0;
    PFN_vkGetBufferMemoryRequirements2KHR      vkGetBufferMemoryRequirements2KHR      = 0;
    PFN_vkGetImageSparseMemoryRequirements2KHR vkGetImageSparseMemoryRequirements2KHR = 0;

    //=== VK_KHR_acceleration_structure ===
    PFN_vkCreateAccelerationStructureKHR                 vkCreateAccelerationStructureKHR                 = 0;
    PFN_vkDestroyAccelerationStructureKHR                vkDestroyAccelerationStructureKHR                = 0;
    PFN_vkCmdBuildAccelerationStructuresKHR              vkCmdBuildAccelerationStructuresKHR              = 0;
    PFN_vkCmdBuildAccelerationStructuresIndirectKHR      vkCmdBuildAccelerationStructuresIndirectKHR      = 0;
    PFN_vkBuildAccelerationStructuresKHR                 vkBuildAccelerationStructuresKHR                 = 0;
    PFN_vkCopyAccelerationStructureKHR                   vkCopyAccelerationStructureKHR                   = 0;
    PFN_vkCopyAccelerationStructureToMemoryKHR           vkCopyAccelerationStructureToMemoryKHR           = 0;
    PFN_vkCopyMemoryToAccelerationStructureKHR           vkCopyMemoryToAccelerationStructureKHR           = 0;
    PFN_vkWriteAccelerationStructuresPropertiesKHR       vkWriteAccelerationStructuresPropertiesKHR       = 0;
    PFN_vkCmdCopyAccelerationStructureKHR                vkCmdCopyAccelerationStructureKHR                = 0;
    PFN_vkCmdCopyAccelerationStructureToMemoryKHR        vkCmdCopyAccelerationStructureToMemoryKHR        = 0;
    PFN_vkCmdCopyMemoryToAccelerationStructureKHR        vkCmdCopyMemoryToAccelerationStructureKHR        = 0;
    PFN_vkGetAccelerationStructureDeviceAddressKHR       vkGetAccelerationStructureDeviceAddressKHR       = 0;
    PFN_vkCmdWriteAccelerationStructuresPropertiesKHR    vkCmdWriteAccelerationStructuresPropertiesKHR    = 0;
    PFN_vkGetDeviceAccelerationStructureCompatibilityKHR vkGetDeviceAccelerationStructureCompatibilityKHR = 0;
    PFN_vkGetAccelerationStructureBuildSizesKHR          vkGetAccelerationStructureBuildSizesKHR          = 0;

    //=== VK_KHR_ray_tracing_pipeline ===
    PFN_vkCmdTraceRaysKHR                                 vkCmdTraceRaysKHR                                 = 0;
    PFN_vkCreateRayTracingPipelinesKHR                    vkCreateRayTracingPipelinesKHR                    = 0;
    PFN_vkGetRayTracingShaderGroupHandlesKHR              vkGetRayTracingShaderGroupHandlesKHR              = 0;
    PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = 0;
    PFN_vkCmdTraceRaysIndirectKHR                         vkCmdTraceRaysIndirectKHR                         = 0;
    PFN_vkGetRayTracingShaderGroupStackSizeKHR            vkGetRayTracingShaderGroupStackSizeKHR            = 0;
    PFN_vkCmdSetRayTracingPipelineStackSizeKHR            vkCmdSetRayTracingPipelineStackSizeKHR            = 0;

    //=== VK_KHR_sampler_ycbcr_conversion ===
    PFN_vkCreateSamplerYcbcrConversionKHR  vkCreateSamplerYcbcrConversionKHR  = 0;
    PFN_vkDestroySamplerYcbcrConversionKHR vkDestroySamplerYcbcrConversionKHR = 0;

    //=== VK_KHR_bind_memory2 ===
    PFN_vkBindBufferMemory2KHR vkBindBufferMemory2KHR = 0;
    PFN_vkBindImageMemory2KHR  vkBindImageMemory2KHR  = 0;

    //=== VK_EXT_image_drm_format_modifier ===
    PFN_vkGetImageDrmFormatModifierPropertiesEXT vkGetImageDrmFormatModifierPropertiesEXT = 0;

    //=== VK_EXT_validation_cache ===
    PFN_vkCreateValidationCacheEXT  vkCreateValidationCacheEXT  = 0;
    PFN_vkDestroyValidationCacheEXT vkDestroyValidationCacheEXT = 0;
    PFN_vkMergeValidationCachesEXT  vkMergeValidationCachesEXT  = 0;
    PFN_vkGetValidationCacheDataEXT vkGetValidationCacheDataEXT = 0;

    //=== VK_NV_shading_rate_image ===
    PFN_vkCmdBindShadingRateImageNV          vkCmdBindShadingRateImageNV          = 0;
    PFN_vkCmdSetViewportShadingRatePaletteNV vkCmdSetViewportShadingRatePaletteNV = 0;
    PFN_vkCmdSetCoarseSampleOrderNV          vkCmdSetCoarseSampleOrderNV          = 0;

    //=== VK_NV_ray_tracing ===
    PFN_vkCreateAccelerationStructureNV                vkCreateAccelerationStructureNV                = 0;
    PFN_vkDestroyAccelerationStructureNV               vkDestroyAccelerationStructureNV               = 0;
    PFN_vkGetAccelerationStructureMemoryRequirementsNV vkGetAccelerationStructureMemoryRequirementsNV = 0;
    PFN_vkBindAccelerationStructureMemoryNV            vkBindAccelerationStructureMemoryNV            = 0;
    PFN_vkCmdBuildAccelerationStructureNV              vkCmdBuildAccelerationStructureNV              = 0;
    PFN_vkCmdCopyAccelerationStructureNV               vkCmdCopyAccelerationStructureNV               = 0;
    PFN_vkCmdTraceRaysNV                               vkCmdTraceRaysNV                               = 0;
    PFN_vkCreateRayTracingPipelinesNV                  vkCreateRayTracingPipelinesNV                  = 0;
    PFN_vkGetRayTracingShaderGroupHandlesNV            vkGetRayTracingShaderGroupHandlesNV            = 0;
    PFN_vkGetAccelerationStructureHandleNV             vkGetAccelerationStructureHandleNV             = 0;
    PFN_vkCmdWriteAccelerationStructuresPropertiesNV   vkCmdWriteAccelerationStructuresPropertiesNV   = 0;
    PFN_vkCompileDeferredNV                            vkCompileDeferredNV                            = 0;

    //=== VK_KHR_maintenance3 ===
    PFN_vkGetDescriptorSetLayoutSupportKHR vkGetDescriptorSetLayoutSupportKHR = 0;

    //=== VK_KHR_draw_indirect_count ===
    PFN_vkCmdDrawIndirectCountKHR        vkCmdDrawIndirectCountKHR        = 0;
    PFN_vkCmdDrawIndexedIndirectCountKHR vkCmdDrawIndexedIndirectCountKHR = 0;

    //=== VK_EXT_external_memory_host ===
    PFN_vkGetMemoryHostPointerPropertiesEXT vkGetMemoryHostPointerPropertiesEXT = 0;

    //=== VK_AMD_buffer_marker ===
    PFN_vkCmdWriteBufferMarkerAMD vkCmdWriteBufferMarkerAMD = 0;

    //=== VK_EXT_calibrated_timestamps ===
    PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT vkGetPhysicalDeviceCalibrateableTimeDomainsEXT = 0;
    PFN_vkGetCalibratedTimestampsEXT                   vkGetCalibratedTimestampsEXT                   = 0;

    //=== VK_NV_mesh_shader ===
    PFN_vkCmdDrawMeshTasksNV              vkCmdDrawMeshTasksNV              = 0;
    PFN_vkCmdDrawMeshTasksIndirectNV      vkCmdDrawMeshTasksIndirectNV      = 0;
    PFN_vkCmdDrawMeshTasksIndirectCountNV vkCmdDrawMeshTasksIndirectCountNV = 0;

    //=== VK_NV_scissor_exclusive ===
    PFN_vkCmdSetExclusiveScissorEnableNV vkCmdSetExclusiveScissorEnableNV = 0;
    PFN_vkCmdSetExclusiveScissorNV       vkCmdSetExclusiveScissorNV       = 0;

    //=== VK_NV_device_diagnostic_checkpoints ===
    PFN_vkCmdSetCheckpointNV       vkCmdSetCheckpointNV       = 0;
    PFN_vkGetQueueCheckpointDataNV vkGetQueueCheckpointDataNV = 0;

    //=== VK_KHR_timeline_semaphore ===
    PFN_vkGetSemaphoreCounterValueKHR vkGetSemaphoreCounterValueKHR = 0;
    PFN_vkWaitSemaphoresKHR           vkWaitSemaphoresKHR           = 0;
    PFN_vkSignalSemaphoreKHR          vkSignalSemaphoreKHR          = 0;

    //=== VK_INTEL_performance_query ===
    PFN_vkInitializePerformanceApiINTEL         vkInitializePerformanceApiINTEL         = 0;
    PFN_vkUninitializePerformanceApiINTEL       vkUninitializePerformanceApiINTEL       = 0;
    PFN_vkCmdSetPerformanceMarkerINTEL          vkCmdSetPerformanceMarkerINTEL          = 0;
    PFN_vkCmdSetPerformanceStreamMarkerINTEL    vkCmdSetPerformanceStreamMarkerINTEL    = 0;
    PFN_vkCmdSetPerformanceOverrideINTEL        vkCmdSetPerformanceOverrideINTEL        = 0;
    PFN_vkAcquirePerformanceConfigurationINTEL  vkAcquirePerformanceConfigurationINTEL  = 0;
    PFN_vkReleasePerformanceConfigurationINTEL  vkReleasePerformanceConfigurationINTEL  = 0;
    PFN_vkQueueSetPerformanceConfigurationINTEL vkQueueSetPerformanceConfigurationINTEL = 0;
    PFN_vkGetPerformanceParameterINTEL          vkGetPerformanceParameterINTEL          = 0;

    //=== VK_AMD_display_native_hdr ===
    PFN_vkSetLocalDimmingAMD vkSetLocalDimmingAMD = 0;

#if defined( VK_USE_PLATFORM_FUCHSIA )
    //=== VK_FUCHSIA_imagepipe_surface ===
    PFN_vkCreateImagePipeSurfaceFUCHSIA vkCreateImagePipeSurfaceFUCHSIA = 0;
#else
    PFN_dummy vkCreateImagePipeSurfaceFUCHSIA_placeholder                         = 0;
#endif /*VK_USE_PLATFORM_FUCHSIA*/

#if defined( VK_USE_PLATFORM_METAL_EXT )
    //=== VK_EXT_metal_surface ===
    PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT = 0;
#else
    PFN_dummy vkCreateMetalSurfaceEXT_placeholder                                 = 0;
#endif /*VK_USE_PLATFORM_METAL_EXT*/

    //=== VK_KHR_fragment_shading_rate ===
    PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR vkGetPhysicalDeviceFragmentShadingRatesKHR = 0;
    PFN_vkCmdSetFragmentShadingRateKHR             vkCmdSetFragmentShadingRateKHR             = 0;

    //=== VK_EXT_buffer_device_address ===
    PFN_vkGetBufferDeviceAddressEXT vkGetBufferDeviceAddressEXT = 0;

    //=== VK_EXT_tooling_info ===
    PFN_vkGetPhysicalDeviceToolPropertiesEXT vkGetPhysicalDeviceToolPropertiesEXT = 0;

    //=== VK_KHR_present_wait ===
    PFN_vkWaitForPresentKHR vkWaitForPresentKHR = 0;

    //=== VK_NV_cooperative_matrix ===
    PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV vkGetPhysicalDeviceCooperativeMatrixPropertiesNV = 0;

    //=== VK_NV_coverage_reduction_mode ===
    PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV = 0;

#if defined( VK_USE_PLATFORM_WIN32_KHR )
    //=== VK_EXT_full_screen_exclusive ===
    PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT vkGetPhysicalDeviceSurfacePresentModes2EXT = 0;
    PFN_vkAcquireFullScreenExclusiveModeEXT        vkAcquireFullScreenExclusiveModeEXT        = 0;
    PFN_vkReleaseFullScreenExclusiveModeEXT        vkReleaseFullScreenExclusiveModeEXT        = 0;
    PFN_vkGetDeviceGroupSurfacePresentModes2EXT    vkGetDeviceGroupSurfacePresentModes2EXT    = 0;
#else
    PFN_dummy vkGetPhysicalDeviceSurfacePresentModes2EXT_placeholder              = 0;
    PFN_dummy vkAcquireFullScreenExclusiveModeEXT_placeholder                     = 0;
    PFN_dummy vkReleaseFullScreenExclusiveModeEXT_placeholder                     = 0;
    PFN_dummy vkGetDeviceGroupSurfacePresentModes2EXT_placeholder                 = 0;
#endif /*VK_USE_PLATFORM_WIN32_KHR*/

    //=== VK_EXT_headless_surface ===
    PFN_vkCreateHeadlessSurfaceEXT vkCreateHeadlessSurfaceEXT = 0;

    //=== VK_KHR_buffer_device_address ===
    PFN_vkGetBufferDeviceAddressKHR              vkGetBufferDeviceAddressKHR              = 0;
    PFN_vkGetBufferOpaqueCaptureAddressKHR       vkGetBufferOpaqueCaptureAddressKHR       = 0;
    PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR vkGetDeviceMemoryOpaqueCaptureAddressKHR = 0;

    //=== VK_EXT_line_rasterization ===
    PFN_vkCmdSetLineStippleEXT vkCmdSetLineStippleEXT = 0;

    //=== VK_EXT_host_query_reset ===
    PFN_vkResetQueryPoolEXT vkResetQueryPoolEXT = 0;

    //=== VK_EXT_extended_dynamic_state ===
    PFN_vkCmdSetCullModeEXT              vkCmdSetCullModeEXT              = 0;
    PFN_vkCmdSetFrontFaceEXT             vkCmdSetFrontFaceEXT             = 0;
    PFN_vkCmdSetPrimitiveTopologyEXT     vkCmdSetPrimitiveTopologyEXT     = 0;
    PFN_vkCmdSetViewportWithCountEXT     vkCmdSetViewportWithCountEXT     = 0;
    PFN_vkCmdSetScissorWithCountEXT      vkCmdSetScissorWithCountEXT      = 0;
    PFN_vkCmdBindVertexBuffers2EXT       vkCmdBindVertexBuffers2EXT       = 0;
    PFN_vkCmdSetDepthTestEnableEXT       vkCmdSetDepthTestEnableEXT       = 0;
    PFN_vkCmdSetDepthWriteEnableEXT      vkCmdSetDepthWriteEnableEXT      = 0;
    PFN_vkCmdSetDepthCompareOpEXT        vkCmdSetDepthCompareOpEXT        = 0;
    PFN_vkCmdSetDepthBoundsTestEnableEXT vkCmdSetDepthBoundsTestEnableEXT = 0;
    PFN_vkCmdSetStencilTestEnableEXT     vkCmdSetStencilTestEnableEXT     = 0;
    PFN_vkCmdSetStencilOpEXT             vkCmdSetStencilOpEXT             = 0;

    //=== VK_KHR_deferred_host_operations ===
    PFN_vkCreateDeferredOperationKHR            vkCreateDeferredOperationKHR            = 0;
    PFN_vkDestroyDeferredOperationKHR           vkDestroyDeferredOperationKHR           = 0;
    PFN_vkGetDeferredOperationMaxConcurrencyKHR vkGetDeferredOperationMaxConcurrencyKHR = 0;
    PFN_vkGetDeferredOperationResultKHR         vkGetDeferredOperationResultKHR         = 0;
    PFN_vkDeferredOperationJoinKHR              vkDeferredOperationJoinKHR              = 0;

    //=== VK_KHR_pipeline_executable_properties ===
    PFN_vkGetPipelineExecutablePropertiesKHR              vkGetPipelineExecutablePropertiesKHR              = 0;
    PFN_vkGetPipelineExecutableStatisticsKHR              vkGetPipelineExecutableStatisticsKHR              = 0;
    PFN_vkGetPipelineExecutableInternalRepresentationsKHR vkGetPipelineExecutableInternalRepresentationsKHR = 0;

    //=== VK_EXT_host_image_copy ===
    PFN_vkCopyMemoryToImageEXT          vkCopyMemoryToImageEXT          = 0;
    PFN_vkCopyImageToMemoryEXT          vkCopyImageToMemoryEXT          = 0;
    PFN_vkCopyImageToImageEXT           vkCopyImageToImageEXT           = 0;
    PFN_vkTransitionImageLayoutEXT      vkTransitionImageLayoutEXT      = 0;
    PFN_vkGetImageSubresourceLayout2EXT vkGetImageSubresourceLayout2EXT = 0;

    //=== VK_KHR_map_memory2 ===
    PFN_vkMapMemory2KHR   vkMapMemory2KHR   = 0;
    PFN_vkUnmapMemory2KHR vkUnmapMemory2KHR = 0;

    //=== VK_EXT_swapchain_maintenance1 ===
    PFN_vkReleaseSwapchainImagesEXT vkReleaseSwapchainImagesEXT = 0;

    //=== VK_NV_device_generated_commands ===
    PFN_vkGetGeneratedCommandsMemoryRequirementsNV vkGetGeneratedCommandsMemoryRequirementsNV = 0;
    PFN_vkCmdPreprocessGeneratedCommandsNV         vkCmdPreprocessGeneratedCommandsNV         = 0;
    PFN_vkCmdExecuteGeneratedCommandsNV            vkCmdExecuteGeneratedCommandsNV            = 0;
    PFN_vkCmdBindPipelineShaderGroupNV             vkCmdBindPipelineShaderGroupNV             = 0;
    PFN_vkCreateIndirectCommandsLayoutNV           vkCreateIndirectCommandsLayoutNV           = 0;
    PFN_vkDestroyIndirectCommandsLayoutNV          vkDestroyIndirectCommandsLayoutNV          = 0;

    //=== VK_EXT_depth_bias_control ===
    PFN_vkCmdSetDepthBias2EXT vkCmdSetDepthBias2EXT = 0;

    //=== VK_EXT_acquire_drm_display ===
    PFN_vkAcquireDrmDisplayEXT vkAcquireDrmDisplayEXT = 0;
    PFN_vkGetDrmDisplayEXT     vkGetDrmDisplayEXT     = 0;

    //=== VK_EXT_private_data ===
    PFN_vkCreatePrivateDataSlotEXT  vkCreatePrivateDataSlotEXT  = 0;
    PFN_vkDestroyPrivateDataSlotEXT vkDestroyPrivateDataSlotEXT = 0;
    PFN_vkSetPrivateDataEXT         vkSetPrivateDataEXT         = 0;
    PFN_vkGetPrivateDataEXT         vkGetPrivateDataEXT         = 0;

#if defined( VK_ENABLE_BETA_EXTENSIONS )
    //=== VK_KHR_video_encode_queue ===
    PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR = 0;
    PFN_vkGetEncodedVideoSessionParametersKHR                   vkGetEncodedVideoSessionParametersKHR                   = 0;
    PFN_vkCmdEncodeVideoKHR                                     vkCmdEncodeVideoKHR                                     = 0;
#else
    PFN_dummy vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR_placeholder = 0;
    PFN_dummy vkGetEncodedVideoSessionParametersKHR_placeholder                   = 0;
    PFN_dummy vkCmdEncodeVideoKHR_placeholder                                     = 0;
#endif /*VK_ENABLE_BETA_EXTENSIONS*/

#if defined( VK_USE_PLATFORM_METAL_EXT )
    //=== VK_EXT_metal_objects ===
    PFN_vkExportMetalObjectsEXT vkExportMetalObjectsEXT = 0;
#else
    PFN_dummy vkExportMetalObjectsEXT_placeholder                                 = 0;
#endif /*VK_USE_PLATFORM_METAL_EXT*/

    //=== VK_KHR_synchronization2 ===
    PFN_vkCmdSetEvent2KHR           vkCmdSetEvent2KHR           = 0;
    PFN_vkCmdResetEvent2KHR         vkCmdResetEvent2KHR         = 0;
    PFN_vkCmdWaitEvents2KHR         vkCmdWaitEvents2KHR         = 0;
    PFN_vkCmdPipelineBarrier2KHR    vkCmdPipelineBarrier2KHR    = 0;
    PFN_vkCmdWriteTimestamp2KHR     vkCmdWriteTimestamp2KHR     = 0;
    PFN_vkQueueSubmit2KHR           vkQueueSubmit2KHR           = 0;
    PFN_vkCmdWriteBufferMarker2AMD  vkCmdWriteBufferMarker2AMD  = 0;
    PFN_vkGetQueueCheckpointData2NV vkGetQueueCheckpointData2NV = 0;

    //=== VK_EXT_descriptor_buffer ===
    PFN_vkGetDescriptorSetLayoutSizeEXT                          vkGetDescriptorSetLayoutSizeEXT                          = 0;
    PFN_vkGetDescriptorSetLayoutBindingOffsetEXT                 vkGetDescriptorSetLayoutBindingOffsetEXT                 = 0;
    PFN_vkGetDescriptorEXT                                       vkGetDescriptorEXT                                       = 0;
    PFN_vkCmdBindDescriptorBuffersEXT                            vkCmdBindDescriptorBuffersEXT                            = 0;
    PFN_vkCmdSetDescriptorBufferOffsetsEXT                       vkCmdSetDescriptorBufferOffsetsEXT                       = 0;
    PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT             vkCmdBindDescriptorBufferEmbeddedSamplersEXT             = 0;
    PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT                vkGetBufferOpaqueCaptureDescriptorDataEXT                = 0;
    PFN_vkGetImageOpaqueCaptureDescriptorDataEXT                 vkGetImageOpaqueCaptureDescriptorDataEXT                 = 0;
    PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT             vkGetImageViewOpaqueCaptureDescriptorDataEXT             = 0;
    PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT               vkGetSamplerOpaqueCaptureDescriptorDataEXT               = 0;
    PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT = 0;

    //=== VK_NV_fragment_shading_rate_enums ===
    PFN_vkCmdSetFragmentShadingRateEnumNV vkCmdSetFragmentShadingRateEnumNV = 0;

    //=== VK_EXT_mesh_shader ===
    PFN_vkCmdDrawMeshTasksEXT              vkCmdDrawMeshTasksEXT              = 0;
    PFN_vkCmdDrawMeshTasksIndirectEXT      vkCmdDrawMeshTasksIndirectEXT      = 0;
    PFN_vkCmdDrawMeshTasksIndirectCountEXT vkCmdDrawMeshTasksIndirectCountEXT = 0;

    //=== VK_KHR_copy_commands2 ===
    PFN_vkCmdCopyBuffer2KHR        vkCmdCopyBuffer2KHR        = 0;
    PFN_vkCmdCopyImage2KHR         vkCmdCopyImage2KHR         = 0;
    PFN_vkCmdCopyBufferToImage2KHR vkCmdCopyBufferToImage2KHR = 0;
    PFN_vkCmdCopyImageToBuffer2KHR vkCmdCopyImageToBuffer2KHR = 0;
    PFN_vkCmdBlitImage2KHR         vkCmdBlitImage2KHR         = 0;
    PFN_vkCmdResolveImage2KHR      vkCmdResolveImage2KHR      = 0;

    //=== VK_EXT_device_fault ===
    PFN_vkGetDeviceFaultInfoEXT vkGetDeviceFaultInfoEXT = 0;

#if defined( VK_USE_PLATFORM_WIN32_KHR )
    //=== VK_NV_acquire_winrt_display ===
    PFN_vkAcquireWinrtDisplayNV vkAcquireWinrtDisplayNV = 0;
    PFN_vkGetWinrtDisplayNV     vkGetWinrtDisplayNV     = 0;
#else
    PFN_dummy vkAcquireWinrtDisplayNV_placeholder                                 = 0;
    PFN_dummy vkGetWinrtDisplayNV_placeholder                                     = 0;
#endif /*VK_USE_PLATFORM_WIN32_KHR*/

#if defined( VK_USE_PLATFORM_DIRECTFB_EXT )
    //=== VK_EXT_directfb_surface ===
    PFN_vkCreateDirectFBSurfaceEXT                        vkCreateDirectFBSurfaceEXT                        = 0;
    PFN_vkGetPhysicalDeviceDirectFBPresentationSupportEXT vkGetPhysicalDeviceDirectFBPresentationSupportEXT = 0;
#else
    PFN_dummy vkCreateDirectFBSurfaceEXT_placeholder                              = 0;
    PFN_dummy vkGetPhysicalDeviceDirectFBPresentationSupportEXT_placeholder       = 0;
#endif /*VK_USE_PLATFORM_DIRECTFB_EXT*/

    //=== VK_EXT_vertex_input_dynamic_state ===
    PFN_vkCmdSetVertexInputEXT vkCmdSetVertexInputEXT = 0;

#if defined( VK_USE_PLATFORM_FUCHSIA )
    //=== VK_FUCHSIA_external_memory ===
    PFN_vkGetMemoryZirconHandleFUCHSIA           vkGetMemoryZirconHandleFUCHSIA           = 0;
    PFN_vkGetMemoryZirconHandlePropertiesFUCHSIA vkGetMemoryZirconHandlePropertiesFUCHSIA = 0;
#else
    PFN_dummy vkGetMemoryZirconHandleFUCHSIA_placeholder                          = 0;
    PFN_dummy vkGetMemoryZirconHandlePropertiesFUCHSIA_placeholder                = 0;
#endif /*VK_USE_PLATFORM_FUCHSIA*/

#if defined( VK_USE_PLATFORM_FUCHSIA )
    //=== VK_FUCHSIA_external_semaphore ===
    PFN_vkImportSemaphoreZirconHandleFUCHSIA vkImportSemaphoreZirconHandleFUCHSIA = 0;
    PFN_vkGetSemaphoreZirconHandleFUCHSIA    vkGetSemaphoreZirconHandleFUCHSIA    = 0;
#else
    PFN_dummy vkImportSemaphoreZirconHandleFUCHSIA_placeholder                    = 0;
    PFN_dummy vkGetSemaphoreZirconHandleFUCHSIA_placeholder                       = 0;
#endif /*VK_USE_PLATFORM_FUCHSIA*/

#if defined( VK_USE_PLATFORM_FUCHSIA )
    //=== VK_FUCHSIA_buffer_collection ===
    PFN_vkCreateBufferCollectionFUCHSIA               vkCreateBufferCollectionFUCHSIA               = 0;
    PFN_vkSetBufferCollectionImageConstraintsFUCHSIA  vkSetBufferCollectionImageConstraintsFUCHSIA  = 0;
    PFN_vkSetBufferCollectionBufferConstraintsFUCHSIA vkSetBufferCollectionBufferConstraintsFUCHSIA = 0;
    PFN_vkDestroyBufferCollectionFUCHSIA              vkDestroyBufferCollectionFUCHSIA              = 0;
    PFN_vkGetBufferCollectionPropertiesFUCHSIA        vkGetBufferCollectionPropertiesFUCHSIA        = 0;
#else
    PFN_dummy vkCreateBufferCollectionFUCHSIA_placeholder                         = 0;
    PFN_dummy vkSetBufferCollectionImageConstraintsFUCHSIA_placeholder            = 0;
    PFN_dummy vkSetBufferCollectionBufferConstraintsFUCHSIA_placeholder           = 0;
    PFN_dummy vkDestroyBufferCollectionFUCHSIA_placeholder                        = 0;
    PFN_dummy vkGetBufferCollectionPropertiesFUCHSIA_placeholder                  = 0;
#endif /*VK_USE_PLATFORM_FUCHSIA*/

    //=== VK_HUAWEI_subpass_shading ===
    PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI = 0;
    PFN_vkCmdSubpassShadingHUAWEI                       vkCmdSubpassShadingHUAWEI                       = 0;

    //=== VK_HUAWEI_invocation_mask ===
    PFN_vkCmdBindInvocationMaskHUAWEI vkCmdBindInvocationMaskHUAWEI = 0;

    //=== VK_NV_external_memory_rdma ===
    PFN_vkGetMemoryRemoteAddressNV vkGetMemoryRemoteAddressNV = 0;

    //=== VK_EXT_pipeline_properties ===
    PFN_vkGetPipelinePropertiesEXT vkGetPipelinePropertiesEXT = 0;

    //=== VK_EXT_extended_dynamic_state2 ===
    PFN_vkCmdSetPatchControlPointsEXT      vkCmdSetPatchControlPointsEXT      = 0;
    PFN_vkCmdSetRasterizerDiscardEnableEXT vkCmdSetRasterizerDiscardEnableEXT = 0;
    PFN_vkCmdSetDepthBiasEnableEXT         vkCmdSetDepthBiasEnableEXT         = 0;
    PFN_vkCmdSetLogicOpEXT                 vkCmdSetLogicOpEXT                 = 0;
    PFN_vkCmdSetPrimitiveRestartEnableEXT  vkCmdSetPrimitiveRestartEnableEXT  = 0;

#if defined( VK_USE_PLATFORM_SCREEN_QNX )
    //=== VK_QNX_screen_surface ===
    PFN_vkCreateScreenSurfaceQNX                        vkCreateScreenSurfaceQNX                        = 0;
    PFN_vkGetPhysicalDeviceScreenPresentationSupportQNX vkGetPhysicalDeviceScreenPresentationSupportQNX = 0;
#else
    PFN_dummy vkCreateScreenSurfaceQNX_placeholder                                = 0;
    PFN_dummy vkGetPhysicalDeviceScreenPresentationSupportQNX_placeholder         = 0;
#endif /*VK_USE_PLATFORM_SCREEN_QNX*/

    //=== VK_EXT_color_write_enable ===
    PFN_vkCmdSetColorWriteEnableEXT vkCmdSetColorWriteEnableEXT = 0;

    //=== VK_KHR_ray_tracing_maintenance1 ===
    PFN_vkCmdTraceRaysIndirect2KHR vkCmdTraceRaysIndirect2KHR = 0;

    //=== VK_EXT_multi_draw ===
    PFN_vkCmdDrawMultiEXT        vkCmdDrawMultiEXT        = 0;
    PFN_vkCmdDrawMultiIndexedEXT vkCmdDrawMultiIndexedEXT = 0;

    //=== VK_EXT_opacity_micromap ===
    PFN_vkCreateMicromapEXT                 vkCreateMicromapEXT                 = 0;
    PFN_vkDestroyMicromapEXT                vkDestroyMicromapEXT                = 0;
    PFN_vkCmdBuildMicromapsEXT              vkCmdBuildMicromapsEXT              = 0;
    PFN_vkBuildMicromapsEXT                 vkBuildMicromapsEXT                 = 0;
    PFN_vkCopyMicromapEXT                   vkCopyMicromapEXT                   = 0;
    PFN_vkCopyMicromapToMemoryEXT           vkCopyMicromapToMemoryEXT           = 0;
    PFN_vkCopyMemoryToMicromapEXT           vkCopyMemoryToMicromapEXT           = 0;
    PFN_vkWriteMicromapsPropertiesEXT       vkWriteMicromapsPropertiesEXT       = 0;
    PFN_vkCmdCopyMicromapEXT                vkCmdCopyMicromapEXT                = 0;
    PFN_vkCmdCopyMicromapToMemoryEXT        vkCmdCopyMicromapToMemoryEXT        = 0;
    PFN_vkCmdCopyMemoryToMicromapEXT        vkCmdCopyMemoryToMicromapEXT        = 0;
    PFN_vkCmdWriteMicromapsPropertiesEXT    vkCmdWriteMicromapsPropertiesEXT    = 0;
    PFN_vkGetDeviceMicromapCompatibilityEXT vkGetDeviceMicromapCompatibilityEXT = 0;
    PFN_vkGetMicromapBuildSizesEXT          vkGetMicromapBuildSizesEXT          = 0;

    //=== VK_HUAWEI_cluster_culling_shader ===
    PFN_vkCmdDrawClusterHUAWEI         vkCmdDrawClusterHUAWEI         = 0;
    PFN_vkCmdDrawClusterIndirectHUAWEI vkCmdDrawClusterIndirectHUAWEI = 0;

    //=== VK_EXT_pageable_device_local_memory ===
    PFN_vkSetDeviceMemoryPriorityEXT vkSetDeviceMemoryPriorityEXT = 0;

    //=== VK_KHR_maintenance4 ===
    PFN_vkGetDeviceBufferMemoryRequirementsKHR      vkGetDeviceBufferMemoryRequirementsKHR      = 0;
    PFN_vkGetDeviceImageMemoryRequirementsKHR       vkGetDeviceImageMemoryRequirementsKHR       = 0;
    PFN_vkGetDeviceImageSparseMemoryRequirementsKHR vkGetDeviceImageSparseMemoryRequirementsKHR = 0;

    //=== VK_VALVE_descriptor_set_host_mapping ===
    PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE vkGetDescriptorSetLayoutHostMappingInfoVALVE = 0;
    PFN_vkGetDescriptorSetHostMappingVALVE           vkGetDescriptorSetHostMappingVALVE           = 0;

    //=== VK_NV_copy_memory_indirect ===
    PFN_vkCmdCopyMemoryIndirectNV        vkCmdCopyMemoryIndirectNV        = 0;
    PFN_vkCmdCopyMemoryToImageIndirectNV vkCmdCopyMemoryToImageIndirectNV = 0;

    //=== VK_NV_memory_decompression ===
    PFN_vkCmdDecompressMemoryNV              vkCmdDecompressMemoryNV              = 0;
    PFN_vkCmdDecompressMemoryIndirectCountNV vkCmdDecompressMemoryIndirectCountNV = 0;

    //=== VK_NV_device_generated_commands_compute ===
    PFN_vkGetPipelineIndirectMemoryRequirementsNV vkGetPipelineIndirectMemoryRequirementsNV = 0;
    PFN_vkCmdUpdatePipelineIndirectBufferNV       vkCmdUpdatePipelineIndirectBufferNV       = 0;
    PFN_vkGetPipelineIndirectDeviceAddressNV      vkGetPipelineIndirectDeviceAddressNV      = 0;

    //=== VK_EXT_extended_dynamic_state3 ===
    PFN_vkCmdSetTessellationDomainOriginEXT         vkCmdSetTessellationDomainOriginEXT         = 0;
    PFN_vkCmdSetDepthClampEnableEXT                 vkCmdSetDepthClampEnableEXT                 = 0;
    PFN_vkCmdSetPolygonModeEXT                      vkCmdSetPolygonModeEXT                      = 0;
    PFN_vkCmdSetRasterizationSamplesEXT             vkCmdSetRasterizationSamplesEXT             = 0;
    PFN_vkCmdSetSampleMaskEXT                       vkCmdSetSampleMaskEXT                       = 0;
    PFN_vkCmdSetAlphaToCoverageEnableEXT            vkCmdSetAlphaToCoverageEnableEXT            = 0;
    PFN_vkCmdSetAlphaToOneEnableEXT                 vkCmdSetAlphaToOneEnableEXT                 = 0;
    PFN_vkCmdSetLogicOpEnableEXT                    vkCmdSetLogicOpEnableEXT                    = 0;
    PFN_vkCmdSetColorBlendEnableEXT                 vkCmdSetColorBlendEnableEXT                 = 0;
    PFN_vkCmdSetColorBlendEquationEXT               vkCmdSetColorBlendEquationEXT               = 0;
    PFN_vkCmdSetColorWriteMaskEXT                   vkCmdSetColorWriteMaskEXT                   = 0;
    PFN_vkCmdSetRasterizationStreamEXT              vkCmdSetRasterizationStreamEXT              = 0;
    PFN_vkCmdSetConservativeRasterizationModeEXT    vkCmdSetConservativeRasterizationModeEXT    = 0;
    PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT vkCmdSetExtraPrimitiveOverestimationSizeEXT = 0;
    PFN_vkCmdSetDepthClipEnableEXT                  vkCmdSetDepthClipEnableEXT                  = 0;
    PFN_vkCmdSetSampleLocationsEnableEXT            vkCmdSetSampleLocationsEnableEXT            = 0;
    PFN_vkCmdSetColorBlendAdvancedEXT               vkCmdSetColorBlendAdvancedEXT               = 0;
    PFN_vkCmdSetProvokingVertexModeEXT              vkCmdSetProvokingVertexModeEXT              = 0;
    PFN_vkCmdSetLineRasterizationModeEXT            vkCmdSetLineRasterizationModeEXT            = 0;
    PFN_vkCmdSetLineStippleEnableEXT                vkCmdSetLineStippleEnableEXT                = 0;
    PFN_vkCmdSetDepthClipNegativeOneToOneEXT        vkCmdSetDepthClipNegativeOneToOneEXT        = 0;
    PFN_vkCmdSetViewportWScalingEnableNV            vkCmdSetViewportWScalingEnableNV            = 0;
    PFN_vkCmdSetViewportSwizzleNV                   vkCmdSetViewportSwizzleNV                   = 0;
    PFN_vkCmdSetCoverageToColorEnableNV             vkCmdSetCoverageToColorEnableNV             = 0;
    PFN_vkCmdSetCoverageToColorLocationNV           vkCmdSetCoverageToColorLocationNV           = 0;
    PFN_vkCmdSetCoverageModulationModeNV            vkCmdSetCoverageModulationModeNV            = 0;
    PFN_vkCmdSetCoverageModulationTableEnableNV     vkCmdSetCoverageModulationTableEnableNV     = 0;
    PFN_vkCmdSetCoverageModulationTableNV           vkCmdSetCoverageModulationTableNV           = 0;
    PFN_vkCmdSetShadingRateImageEnableNV            vkCmdSetShadingRateImageEnableNV            = 0;
    PFN_vkCmdSetRepresentativeFragmentTestEnableNV  vkCmdSetRepresentativeFragmentTestEnableNV  = 0;
    PFN_vkCmdSetCoverageReductionModeNV             vkCmdSetCoverageReductionModeNV             = 0;

    //=== VK_EXT_shader_module_identifier ===
    PFN_vkGetShaderModuleIdentifierEXT           vkGetShaderModuleIdentifierEXT           = 0;
    PFN_vkGetShaderModuleCreateInfoIdentifierEXT vkGetShaderModuleCreateInfoIdentifierEXT = 0;

    //=== VK_NV_optical_flow ===
    PFN_vkGetPhysicalDeviceOpticalFlowImageFormatsNV vkGetPhysicalDeviceOpticalFlowImageFormatsNV = 0;
    PFN_vkCreateOpticalFlowSessionNV                 vkCreateOpticalFlowSessionNV                 = 0;
    PFN_vkDestroyOpticalFlowSessionNV                vkDestroyOpticalFlowSessionNV                = 0;
    PFN_vkBindOpticalFlowSessionImageNV              vkBindOpticalFlowSessionImageNV              = 0;
    PFN_vkCmdOpticalFlowExecuteNV                    vkCmdOpticalFlowExecuteNV                    = 0;

    //=== VK_KHR_maintenance5 ===
    PFN_vkCmdBindIndexBuffer2KHR             vkCmdBindIndexBuffer2KHR             = 0;
    PFN_vkGetRenderingAreaGranularityKHR     vkGetRenderingAreaGranularityKHR     = 0;
    PFN_vkGetDeviceImageSubresourceLayoutKHR vkGetDeviceImageSubresourceLayoutKHR = 0;
    PFN_vkGetImageSubresourceLayout2KHR      vkGetImageSubresourceLayout2KHR      = 0;

    //=== VK_EXT_shader_object ===
    PFN_vkCreateShadersEXT       vkCreateShadersEXT       = 0;
    PFN_vkDestroyShaderEXT       vkDestroyShaderEXT       = 0;
    PFN_vkGetShaderBinaryDataEXT vkGetShaderBinaryDataEXT = 0;
    PFN_vkCmdBindShadersEXT      vkCmdBindShadersEXT      = 0;

    //=== VK_QCOM_tile_properties ===
    PFN_vkGetFramebufferTilePropertiesQCOM      vkGetFramebufferTilePropertiesQCOM      = 0;
    PFN_vkGetDynamicRenderingTilePropertiesQCOM vkGetDynamicRenderingTilePropertiesQCOM = 0;

    //=== VK_KHR_cooperative_matrix ===
    PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR = 0;

    //=== VK_EXT_attachment_feedback_loop_dynamic_state ===
    PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT vkCmdSetAttachmentFeedbackLoopEnableEXT = 0;

#if defined( VK_USE_PLATFORM_SCREEN_QNX )
    //=== VK_QNX_external_memory_screen_buffer ===
    PFN_vkGetScreenBufferPropertiesQNX vkGetScreenBufferPropertiesQNX = 0;
#else
    PFN_dummy vkGetScreenBufferPropertiesQNX_placeholder                          = 0;
#endif /*VK_USE_PLATFORM_SCREEN_QNX*/

};

namespace vxl::vk {

template<typename LayerType = VkLayerProperties, typename ExtensionType = VkExtensionProperties>
struct layer_extension_store {
    using layer_type = std::pair<LayerType, std::pmr::vector<ExtensionType>>;

    static auto make(auto&& layer_enumerator, auto&& extension_enumerator) -> std::expected<layer_extension_store, error> {
        auto ret = layer_extension_store{};

        auto layers = TRYX(error::from_vk(get_vector(layer_enumerator), "failed to enumerate instance layers"));

        ret.m_vk_instance_layers.reserve(layers.size());
        for (auto const& layer_props : layers) {
            const auto enumerator = [&extension_enumerator, &layer_props](u32* out_size, ExtensionType* out_extensions) {
                return extension_enumerator(layer_props.layerName, out_size, out_extensions);
            };

            auto&& layer_extensions = TRYX(error::from_vk(get_vector(enumerator), "failed to enumerate instance layer properties"));

            ret.m_vk_instance_layers.emplace_back(layer_props, std::move(layer_extensions));
        }

        auto const& layerless_extension_enumerator = [&extension_enumerator](u32* out_size, ExtensionType* out_extensions) {
            return extension_enumerator(nullptr, out_size, out_extensions);
        };

        ret.m_vk_instance_extensions = TRYX(error::from_vk(get_vector(layerless_extension_enumerator), "could not enumerate instance layer properties"));

        return ret;
    }

    constexpr auto layer(std::string_view layer_name) const -> std::optional<LayerType> {
        return layer_internal(layer_name).transform([](auto const& layer) { return layer.get().first; });
    }

    constexpr auto extension(std::string_view extension_name) const -> std::optional<ExtensionType> {
        auto iter = std::ranges::find_if(m_vk_instance_extensions, [extension_name](auto const& extension) { return extension_name == std::string_view(extension.extensionName); });

        if (iter == std::ranges::end(m_vk_instance_extensions)) {
            return std::nullopt;
        }

        return *iter;
    }

    constexpr auto extension(std::string_view extension_name, std::string_view layer_name) const -> std::optional<ExtensionType> {
        return layer_internal(layer_name).and_then([extension_name](auto const& layer) -> std::optional<ExtensionType> {
            auto iter = std::ranges::find_if(layer.get().second, [extension_name](auto const& layer) { return extension_name == std::string_view(layer.extensionName); });

            if (iter == std::ranges::end(layer.get().second)) {
                return std::nullopt;
            }

            return *iter;
        });
    }

    constexpr auto layers() const -> std::ranges::keys_view<std::pmr::vector<layer_type> const&> { return m_vk_instance_layers | std::views::keys; }

    constexpr auto extensions() const -> std::ranges::ref_view<const std::pmr::vector<ExtensionType>> { return m_vk_instance_extensions | std::views::all; }

    constexpr auto extensions(std::string_view layer_name) const -> std::optional<std::ranges::ref_view<const std::pmr::vector<ExtensionType>>> {
        return layer_internal(layer_name)  //
          .transform([](auto const& layer) { return layer.get().second | std::views::all; });
        //.value_or([] { return std::ranges::empty_view<ExtensionType>{}; });
    }

    static constexpr auto pick_stuff(auto&& desired, auto&& available, std::string_view name) -> std::expected<std::vector<const char*>, error> {
        static constexpr auto grouper = std::views::chunk_by([](auto const& lhs, auto const& rhs) { return lhs.second == rhs.second; });

        auto desired_groups = desired | grouper;
        const auto desired_group_count = std::ranges::distance(desired_groups);

        auto available_groups = available | grouper;
        const auto available_group_count = std::ranges::distance(available_groups);

        if (available_group_count != desired_group_count) {
            spdlog::error("not all desired {}s could be loaded, available_group_count ({}) != desired_group_count ({})", name, available_group_count, desired_group_count);

            spdlog::error("desired extensions (grouped):");
            for (auto const& [no, group] : desired_groups | std::views::enumerate) {
                spdlog::error("\tgroup #{}:", no);
                for (auto const& [ext_no, ext] : group | std::views::enumerate) {
                    spdlog::error("\t\t{} #{}: {}", name, ext_no, ext.first);
                }
            }

            spdlog::error("available {}s (grouped):", name);
            for (auto const& [no, group] : available_groups | std::views::enumerate) {
                spdlog::error("\tgroup #{}:", no);
                for (auto const& [ext_no, ext] : group | std::views::enumerate) {
                    spdlog::error("\t\t{} #{}: {}", name, ext_no, ext.first);
                }
            }

            return std::unexpected{error{}};
        }

        auto extension_names_range = available_groups | std::views::transform([](auto group) { return group | std::views::take(1); }) | std::views::join | std::views::keys;
        auto ret = std::vector<const char*>(std::ranges::distance(extension_names_range));
        std::ranges::copy(extension_names_range, ret.begin());

        spdlog::debug("enabling the following {}s:", name);
        for (auto const& [no, elem_name] : ret | std::views::enumerate) {
            spdlog::debug("\t{} #{}: {}", name, no, elem_name);
        }

        return ret;
    }

private:
    std::pmr::vector<layer_type> m_vk_instance_layers{};
    std::pmr::vector<ExtensionType> m_vk_instance_extensions{};

    constexpr auto layer_internal(std::string_view layer_name) const -> std::optional<std::reference_wrapper<const layer_type>> {
        auto it = std::ranges::find_if(m_vk_instance_layers, [layer_name](auto const& layer) { return layer_name == std::string_view(layer.first.layerName); });

        if (it == std::ranges::end(m_vk_instance_layers)) {
            return std::nullopt;
        }

        return *it;
    }
};

struct vulkan_stuff {
    static auto make() -> std::expected<vulkan_stuff, error> {
        auto&& ret = vulkan_stuff{};

        ret.m_dyna_loader = TRYX(dynamic_loader::make(get_vulkan_library_names()).transform_error([](auto err) { return error::make(err); }));
        ret.m_vk_functions.init(ret.m_dyna_loader.get_dl_symbol<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"));

        TRYX(ret.init_vk_instance());
        TRYX(ret.init_vk_debug_messenger());

        return ret;
    }

    vulkan_stuff() = default;

    ~vulkan_stuff() {
        if (m_vk_command_buffer != nullptr) {
            m_vk_device.freeCommandBuffers(m_vk_command_pool, 1, &m_vk_command_buffer, m_vk_dispatch);
        }

        if (m_vk_command_pool != nullptr) {
            m_vk_device.destroyCommandPool(m_vk_command_pool, nullptr, m_vk_dispatch);
        }

        if (m_vk_device != nullptr) {
            m_vk_functions.destroy_device(m_vk_device);
        }

        if (m_sdl_window != nullptr) {
            SDL_DestroyWindow(m_sdl_window);
        }

        if (m_vk_surface != nullptr) {
            m_vk_functions.destroy_surface_khr(m_vk_instance, m_vk_surface);
            m_vk_surface = nullptr;
        }

        if (m_vk_debug_messenger != nullptr) {
            m_vk_functions.destroy_debug_utils_messenger_ext(m_vk_instance, m_vk_debug_messenger);
            m_vk_debug_messenger = nullptr;
        }

        if (m_vk_instance != nullptr) {
            m_vk_functions.destroy_instance(m_vk_instance);
            m_vk_instance = nullptr;
        }
    }

    vulkan_stuff(vulkan_stuff&& other) noexcept
        : m_dyna_loader(std::move(other.m_dyna_loader))
        , m_vk_functions(std::move(other.m_vk_functions))
        , m_instance_extensions(std::move(other.m_instance_extensions))
        , m_device_extensions(std::move(other.m_device_extensions)) {
        using std::swap;

        swap(m_vk_instance, other.m_vk_instance);
        swap(m_vk_debug_messenger, other.m_vk_debug_messenger);

        swap(m_window_size, other.m_window_size);
        swap(m_sdl_window, other.m_sdl_window);
        swap(m_vk_surface, other.m_vk_surface);
    }

    auto run() -> std::expected<void, error> { return {}; }

private:
    VkApplicationInfo m_app_info{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = "vulkan test",
      .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
      .pEngineName = "voxels",
      .engineVersion = VK_MAKE_VERSION(0, 0, 1),
      .apiVersion = VK_API_VERSION_1_1,
    };

    dynamic_loader m_dyna_loader;
    vulkan_functions m_vk_functions;

    layer_extension_store<> m_instance_extensions;
    layer_extension_store<> m_device_extensions;

    VkInstance m_vk_instance = nullptr;
    VkDebugUtilsMessengerEXT m_vk_debug_messenger = nullptr;

    VkExtent2D m_window_size{640u, 360u};
    SDL_Window* m_sdl_window = nullptr;
    VkSurfaceKHR m_vk_surface = nullptr;

    VkPhysicalDevice m_vk_physical_device = nullptr;

    u32 m_vk_queue_family_index = 0;
    VkQueueFamilyProperties m_vk_queue_family = {};
    VkQueue m_vk_queue = nullptr;

    // VkQueueFamilyProperties m_vk_queue_family_graphics;
    // u32 m_vk_queue_family_index_graphics;
    // VkQueueFamilyProperties m_vk_queue_family_present;
    // u32 m_vk_queue_family_index_present;

    VkDevice m_vk_device = nullptr;
    VkCommandPool m_vk_command_pool = nullptr;
    VkCommandBuffer m_vk_command_buffer = nullptr;

    static auto get_desired_instance_layers() -> std::span<const std::pair<const char*, usize>> {
        return (const std::pair<const char*, usize>[]){
          {"VK_LAYER_KHRONOS_validation", 1},
        };
    }

    static auto get_desired_instance_extensions() -> std::span<const std::pair<const char*, usize>> {
        return (std::pair<const char*, usize> const[]){
          {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, 0},              //
          {VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, 1},  //
          {"VK_KHR_surface", 2},                               //
          {"VK_KHR_xlib_surface", 3},                          //
          {"VK_KHR_xcb_surface", 3},                           //
          {"VK_KHR_wayland_surface", 3},                       //
        };
    }

    static auto get_desired_device_layers() -> std::span<const std::pair<const char*, usize>> {
        // return (std::pair<const char*, usize> const[]){};
        return {};
    }

    static auto get_desired_device_extensions() -> std::span<const std::pair<const char*, usize>> {
        return (std::pair<const char*, usize> const[]){
          {"VK_KHR_swapchain", 0},  //
        };
    }

    static constexpr auto get_vulkan_library_names() -> std::span<const char* const> {
#ifdef __linux__
        return (const char* const[]){
          "libvulkan.so.1",  //
          "libvulkan.so"     //
        };
#elifdef __APPLE__
        return (const char* const[]){
          "libvulkan.1.dylib",
          "libvulkan.dylib",
        };
#elifdef _WIN32
        return (const char* const[]){
          "vulkan-1.dll",
        };
#else
        return {};
#endif
    }

    auto init_vk_instance() -> std::expected<void, error> {
        m_instance_extensions = TRYX(layer_extension_store<>::make(m_vk_functions.enumerate_instance_layer_properties(), m_vk_functions.enumerate_instance_extension_properties()));

        for (auto layers_range = m_instance_extensions.layers(); auto const& [layer_no, layer] : layers_range | std::views::enumerate) {
            spdlog::debug("Layer #{}", layer_no);
            vxl::log(layer, 1);

            for (auto extensions_range = *m_instance_extensions.extensions(layer.layerName); auto const& [extension_no, extension] : extensions_range | std::views::enumerate) {
                spdlog::debug("\tExtension #{}", extension_no);
                vxl::log(extension, 2);
            }
        }

        for (auto extensions_range = m_instance_extensions.extensions(); auto const& [extension_no, extension] : extensions_range | std::views::enumerate) {
            spdlog::debug("Extension #{}", extension_no);
            vxl::log(extension, 2);
        }

        auto desired_layers = get_desired_instance_layers();
        auto available_layers = desired_layers | std::views::filter([this](auto const& ext) { return (bool)m_instance_extensions.layer(ext.first); });
        auto layers_to_enable = TRYX(m_instance_extensions.pick_stuff(desired_layers, available_layers, "layer").transform_error([](auto) {
            return error::make("could not enable all required instance layers");
        }));

        auto desired_exts = get_desired_instance_extensions();
        auto available_exts = desired_exts | std::views::filter([this](auto const& ext) { return (bool)m_instance_extensions.extension(ext.first); });
        auto exts_to_enable = TRYX(m_instance_extensions.pick_stuff(desired_exts, available_exts, "extension").transform_error([](auto) {
            return error::make("could not enable all required instance extensions");
        }));

        const auto instance_create_info = VkInstanceCreateInfo{
          .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
          .pNext = nullptr,
          .flags = {},
          .pApplicationInfo = &m_app_info,
          .enabledLayerCount = static_cast<u32>(layers_to_enable.size()),
          .ppEnabledLayerNames = layers_to_enable.data(),
          .enabledExtensionCount = static_cast<u32>(exts_to_enable.size()),
          .ppEnabledExtensionNames = exts_to_enable.data(),
        };
        m_vk_instance = TRYX(error::from_vk(m_vk_functions.create_instance(&instance_create_info), "could not create a vk::Instance"));

        m_vk_functions.init(m_vk_instance);

        return {};
    }

    auto init_vk_debug_messenger() -> std::expected<void, error> {
        const auto debug_utils_messenger_create_info = VkDebugUtilsMessengerCreateInfoEXT{
          .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
          .pNext = nullptr,
          .flags = 0,
          .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
          .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,
          .pfnUserCallback = &vulkan_debug_messenger,
          .pUserData = nullptr,
        };

        m_vk_debug_messenger =
          TRYX(error::from_vk(m_vk_functions.create_debug_utils_messenger_ext(m_vk_instance, &debug_utils_messenger_create_info), "could not create a VkDebugUtilsMessengerEXT"));

        return {};
    }

    auto init_sdl_window() -> std::expected<void, error> {
        m_sdl_window = SDL_CreateWindow(
          "vulkan test",                                                                  //
          static_cast<int>(m_window_size.width), static_cast<int>(m_window_size.height),  //
          static_cast<SDL_WindowFlags>(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE)
        );

        if (m_sdl_window == nullptr) {
            spdlog::error("failed to create an SDL window, error: {}", SDL_GetError());
            return std::unexpected{error::make("failed to create an SDL window")};
        }

        return {};
    }

    auto init_vk_surface() -> std::expected<void, error> {
        const auto res = SDL_Vulkan_CreateSurface(m_sdl_window, m_vk_instance, &m_vk_surface);

        if (res == SDL_FALSE || m_vk_surface == nullptr) {
            m_vk_surface = nullptr;
            spdlog::error("failed to create a vulkan surface through SDL, error: {}", SDL_GetError());
            return std::unexpected{error::make("failed to create a vulkan surface through SDL")};
        }

        return {};
    }

    auto try_init_device(VkPhysicalDevice const& physical_device, usize queue_family_index) const -> std::expected<VkDevice, error> {
        const auto device_extensions = TRYX(layer_extension_store<>::make(
          [this, &physical_device](u32* out_count, VkLayerProperties* out_layers) {
              return m_vk_functions.enumerate_device_layer_properties(physical_device, out_count, out_layers);
          },
          [this, &physical_device](const char* name, u32* out_count, VkExtensionProperties* out_extensions) {
              return m_vk_functions.enumerate_device_extension_properties(physical_device, name, out_count, out_extensions);
          }
        ));

        for (auto layers_range = device_extensions.layers(); auto const& [layer_no, layer] : layers_range | std::views::enumerate) {
            spdlog::debug("Device Layer #{}", layer_no);
            vxl::log(layer, 1);

            for (auto extensions_range = *device_extensions.extensions(layer.layerName); auto const& [extension_no, extension] : extensions_range | std::views::enumerate) {
                spdlog::debug("\tExtension #{}", extension_no);
                vxl::log(extension, 2);
            }
        }

        for (auto extensions_range = device_extensions.extensions(); auto const& [extension_no, extension] : extensions_range | std::views::enumerate) {
            spdlog::debug("Device Extension #{}", extension_no);
            vxl::log(extension, 2);
        }

        auto desired_layers = get_desired_device_layers();
        auto available_layers = desired_layers | std::views::filter([&device_extensions](auto const& ext) { return (bool)device_extensions.layer(ext.first); });
        auto layers_to_enable = TRYX(m_instance_extensions.pick_stuff(desired_layers, available_layers, "layer").transform_error([](auto) {
            return error::make("could not enable all required device layers");
        }));

        auto desired_exts = get_desired_device_extensions();
        auto available_exts = desired_exts | std::views::filter([&device_extensions](auto const& ext) { return (bool)device_extensions.extension(ext.first); });
        auto exts_to_enable = TRYX(m_instance_extensions.pick_stuff(desired_exts, available_exts, "extension").transform_error([](auto) {
            return error::make("could not enable all required device extensions");
        }));

        const float queue_priority = 0.f;
        const auto queue_create_info = VkDeviceQueueCreateInfo{
          .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .queueFamilyIndex = static_cast<u32>(queue_family_index),
          .queueCount = 1,
          .pQueuePriorities = &queue_priority,
        };

        const auto device_create_info = VkDeviceCreateInfo{
          .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .queueCreateInfoCount = 1,
          .pQueueCreateInfos = &queue_create_info,
          .enabledLayerCount = static_cast<u32>(layers_to_enable.size()),
          .ppEnabledLayerNames = layers_to_enable.data(),
          .enabledExtensionCount = static_cast<u32>(exts_to_enable.size()),
          .ppEnabledExtensionNames = exts_to_enable.data(),
          .pEnabledFeatures = nullptr,
        };

        auto&& device = TRYX(error::from_vk(m_vk_functions.create_device(physical_device, &device_create_info), "could not create a vk::Device"));

        return device;
    }

    auto init_vk_devices_and_queues() -> std::expected<void, error> {
        const auto physical_devices = TRYX(error::from_vk(
          get_vector([this](u32* out_count, VkPhysicalDevice* out_devices) { return m_vk_functions.enumerate_physical_devices(m_vk_instance, out_count, out_devices); }),
          "could not enumerate physical devices"
        ));

        for (auto const& [device_no, device] : physical_devices | std::views::enumerate) {
            const auto properties = m_vk_functions.get_physical_device_properties(device);
            spdlog::debug("Device #{}:", device_no);
            vxl::log(properties, 1);

            const auto queue_family_properties = get_vector([this, &device](u32* out_count, VkQueueFamilyProperties* out_properties) {
                m_vk_functions.get_physical_device_queue_family_properties(device, out_count, out_properties);
            });

            for (auto const& [family_no, family] : queue_family_properties | std::views::enumerate) {
                spdlog::debug("\tQueue family #{}", family_no);
                vxl::log(family, 2);
                auto can_present = TRYX(error::from_vk(m_vk_functions.get_physical_device_surface_support_khr(device, family_no, m_vk_surface)));
                spdlog::debug("\t\tCan present?    : {}", can_present == VK_TRUE ? "Yes" : "No");
            }
        }

        m_vk_physical_device = physical_devices[0];

        spdlog::info("a physical device was chosen:");
        vxl::log(m_vk_functions.get_physical_device_properties(m_vk_physical_device), 1, spdlog::level::info);

        m_vk_queue_family_index = 0;
        m_vk_queue_family = get_vector([this](u32* out_count, VkQueueFamilyProperties* out_properties) {
            m_vk_functions.get_physical_device_queue_family_properties(m_vk_physical_device, out_count, out_properties);
        })[m_vk_queue_family_index];

        m_vk_device = TRYX(try_init_device(m_vk_physical_device, m_vk_queue_family_index));

        m_vk_queue = m_vk_functions.get_device_queue(m_vk_device, m_vk_queue_family_index, 0);

        return {};
    }
};

}  // namespace vxl::vk

auto main() -> int {
    spdlog::set_level(spdlog::level::debug);

    spdlog::debug("{}", vxl::vk::error::make("this is a test error"));

    if (auto init_res = SDL_Init(SDL_INIT_VIDEO); init_res != 0) {
        spdlog::error("SDL initialisation failed, error: {}", SDL_GetError());
        return 1;
    }

    auto vulkan_stuff = ({
        auto&& res = vxl::vk::vulkan_stuff::make();

        if (!res) {
            spdlog::error("failed to initialize graphics: {}", res.error());
            return 1;
        }

        std::move(*res);
    });

    auto&& res = vulkan_stuff.run();

    if (!res) {
        spdlog::error("program failed with error: {}", res.error());
        return 1;
    }
}
