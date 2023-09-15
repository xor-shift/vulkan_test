#include <stuff/core.hpp>
#include <stuff/scope.hpp>

#include <SDL.h>
#include <SDL_vulkan.h>
#include <VkBootstrap.h>
#include <vulkan/vulkan.hpp>

#include <chrono>
#include <cmath>
#include <iostream>

#define UNNAMED3(_name) const auto _unnamed_##_name
#define UNNAMED2(_name) UNNAMED3(_name)
#define UNNAMED UNNAMED2(__COUNTER__)

struct thing {
    thing(u32 width, u32 height)
        : m_extent(width, height) {
        init_sdl();
    }

private:
    vk::Extent2D m_extent;

    SDL_Window* m_window = nullptr;
    vk::SurfaceKHR m_surface{(VkSurfaceKHR)VK_NULL_HANDLE};

    void init_sdl() {}

    void init_vk_instance() {}

    void init_vk_surface() {}

    void init_vk_device() {}
};

auto main() -> int {
    constexpr auto extent = vk::Extent2D{600, 400};

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Failed to initialise SDL: " << SDL_GetError() << '\n';
        throw std::runtime_error("failed to initialize SDL");
    }

    UNNAMED = stf::scope_exit{[] { SDL_Quit(); }};

    auto* const window = SDL_CreateWindow(
      "vulkan test",                                                        //
      static_cast<int>(extent.width), static_cast<int>(extent.height),  //
      static_cast<SDL_WindowFlags>(SDL_WINDOW_VULKAN)
    );
    UNNAMED = stf::scope_exit{[window] { SDL_DestroyWindow(window); }};

    vkb::InstanceBuilder builder;
    auto inst_ret = builder
                      .set_app_name("vulkan test")      //
                      .request_validation_layers(true)  //
                      .require_api_version(1, 1, 0)     //
                      .use_default_debug_messenger()    //
                      .build();

    auto vkb_instance = inst_ret.value();

    auto* instance = vkb_instance.instance;
    auto* messenger = vkb_instance.debug_messenger;

    UNNAMED = stf::scope_exit{[instance, messenger] {
        vkb::destroy_debug_utils_messenger(instance, messenger);
        vkDestroyInstance(instance, nullptr);
    }};

    auto* surface = VkSurfaceKHR{};
    SDL_Vulkan_CreateSurface(window, instance, &surface);

    UNNAMED = stf::scope_exit{[instance, surface] { vkDestroySurfaceKHR(instance, surface, nullptr); }};

    auto selector = vkb::PhysicalDeviceSelector{vkb_instance};
    auto physical_device = selector
                             .set_minimum_version(1, 1)  //
                             .set_surface(surface)       //
                             .select()                   //
                             .value();

    auto device_builder = vkb::DeviceBuilder{physical_device};
    auto vkb_device = device_builder.build().value();

    auto* device = vkb_device.device;
    auto* chosen_gpu = physical_device.physical_device;

    UNNAMED = stf::scope_exit{[device] { vkDestroyDevice(device, nullptr); }};

    auto* graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
    auto graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

    vkb::SwapchainBuilder swapchainBuilder{chosen_gpu, device, surface};

    vkb::Swapchain vkbSwapchain = swapchainBuilder
                                    .use_default_format_selection()
                                    // use vsync present mode
                                    .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                                    .set_desired_extent(extent.width, extent.height)
                                    .build()
                                    .value();

    auto* swap_chain = vkbSwapchain.swapchain;
    auto swap_chain_images = vkbSwapchain.get_images().value();
    auto swap_chain_image_views = vkbSwapchain.get_image_views().value();

    UNNAMED = stf::scope_exit{[device, swap_chain] { vkDestroySwapchainKHR(device, swap_chain, nullptr); }};
    UNNAMED = stf::scope_exit{[device, &imgs = swap_chain_image_views] { std::ranges::for_each(imgs, [device](auto view) { vkDestroyImageView(device, view, nullptr); }); }};

    const auto swap_chain_image_format = vkbSwapchain.image_format;

    auto command_pool_info = VkCommandPoolCreateInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = graphics_queue_family,
    };

    auto* command_pool = VkCommandPool{};
    vkCreateCommandPool(device, &command_pool_info, nullptr, &command_pool);
    UNNAMED = stf::scope_exit{[device, command_pool] { vkDestroyCommandPool(device, command_pool, nullptr); }};

    auto command_alloc_info = VkCommandBufferAllocateInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = command_pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
    };

    auto* main_command_buffer = VkCommandBuffer{};
    vkAllocateCommandBuffers(device, &command_alloc_info, &main_command_buffer);

    auto color_attachment = VkAttachmentDescription{
      .format = swap_chain_image_format,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    auto color_attachment_ref = VkAttachmentReference{
      .attachment = 0,
      .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    auto subpass = VkSubpassDescription{
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .colorAttachmentCount = 1,
      .pColorAttachments = &color_attachment_ref,
    };

    auto render_pass_create_info = VkRenderPassCreateInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .attachmentCount = 1,
      .pAttachments = &color_attachment,
      .subpassCount = 1,
      .pSubpasses = &subpass,
    };

    auto* render_pass = VkRenderPass{};
    vkCreateRenderPass(device, &render_pass_create_info, nullptr, &render_pass);
    UNNAMED = stf::scope_exit{[device, render_pass] { vkDestroyRenderPass(device, render_pass, nullptr); }};

    VkFramebufferCreateInfo fb_info = {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .pNext = nullptr,
      .renderPass = render_pass,
      .attachmentCount = 1,
      .width = extent.width,
      .height = extent.height,
      .layers = 1,
    };

    const auto swapchain_imagecount = swap_chain_images.size();
    auto framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);
    UNNAMED = stf::scope_exit{[device, &framebuffers] { std::ranges::for_each(framebuffers, [device](auto buffer) { vkDestroyFramebuffer(device, buffer, nullptr); }); }};

    for (int i = 0; i < swapchain_imagecount; i++) {
        fb_info.pAttachments = &swap_chain_image_views[i];
        vkCreateFramebuffer(device, &fb_info, nullptr, &framebuffers[i]);
    }

    auto fenceCreateInfo = VkFenceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    auto* render_fence = VkFence{};
    vkCreateFence(device, &fenceCreateInfo, nullptr, &render_fence);
    UNNAMED = stf::scope_exit{[device, render_fence] { vkDestroyFence(device, render_fence, nullptr); }};

    auto semaphoreCreateInfo = VkSemaphoreCreateInfo{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
    };

    auto* present_semaphore = VkSemaphore{};
    vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &present_semaphore);
    UNNAMED = stf::scope_exit{[device, present_semaphore] { vkDestroySemaphore(device, present_semaphore, nullptr); }};

    auto* render_semaphore = VkSemaphore{};
    vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &render_semaphore);
    UNNAMED = stf::scope_exit{[device, render_semaphore] { vkDestroySemaphore(device, render_semaphore, nullptr); }};

    static constexpr auto a_second = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
    static constexpr auto color_period = 120.f;

    auto frame_number = 0u;
    for (auto running = true; running;) {
        for (SDL_Event event; SDL_PollEvent(&event) != 0;) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        vkWaitForFences(device, 1, &render_fence, (VkBool32) true, a_second);
        vkResetFences(device, 1, &render_fence);

        auto swapchain_image_index = (u32)0;
        vkAcquireNextImageKHR(device, swap_chain, a_second, present_semaphore, nullptr, &swapchain_image_index);

        vkResetCommandBuffer(main_command_buffer, 0);

        auto command_begin_info = VkCommandBufferBeginInfo{
          .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
          .pNext = nullptr,
          .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
          .pInheritanceInfo = nullptr,
        };

        vkBeginCommandBuffer(main_command_buffer, &command_begin_info);

        auto clear_value = VkClearValue{};
        float flash = std::abs(std::sin(static_cast<float>(frame_number) / color_period));
        clear_value.color = {{0.f, 0.f, flash, 1.f}};

        auto render_pass_info = VkRenderPassBeginInfo{
          .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
          .pNext = nullptr,
          .renderPass = render_pass,
          .framebuffer = framebuffers[swapchain_image_index],
          .renderArea{
            .offset{},
            .extent{
              .width = extent.width,
              .height = extent.height,
            },
          },
          .clearValueCount = 1,
          .pClearValues = &clear_value,
        };

        vkCmdBeginRenderPass(main_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdEndRenderPass(main_command_buffer);
        vkEndCommandBuffer(main_command_buffer);

        const auto waitStage = (VkPipelineStageFlags)VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        auto submit = VkSubmitInfo{
          .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
          .pNext = nullptr,
          .waitSemaphoreCount = 1,
          .pWaitSemaphores = &present_semaphore,
          .pWaitDstStageMask = &waitStage,
          .commandBufferCount = 1,
          .pCommandBuffers = &main_command_buffer,
          .signalSemaphoreCount = 1,
          .pSignalSemaphores = &render_semaphore,
        };

        vkQueueSubmit(graphics_queue, 1, &submit, render_fence);

        auto present_info = VkPresentInfoKHR{
          .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
          .pNext = nullptr,
          .waitSemaphoreCount = 1,
          .pWaitSemaphores = &render_semaphore,
          .swapchainCount = 1,
          .pSwapchains = &swap_chain,
          .pImageIndices = &swapchain_image_index,
        };
        vkQueuePresentKHR(graphics_queue, &present_info);

        frame_number++;
    }

    vkWaitForFences(device, 1, &render_fence, (VkBool32) true, a_second);
    vkResetFences(device, 1, &render_fence);

    return 0;
}
