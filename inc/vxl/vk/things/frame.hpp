#pragma once

#include <vxl/vk/functions.hpp>
#include <vxl/vk/things/device.hpp>
#include <vxl/vk/things/swapchain.hpp>

namespace vxl::vk {

struct frame_things {
    frame_things(
      vulkan_functions const& vk_fns,
      device_things const& device,
      swapchain_things const& swapchain,
      VkFence render_fence,
      VkSemaphore present_semaphore,
      VkSemaphore render_semaphore,
      VkExtent2D window_size,
      VkRenderPass render_pass,
      std::span<const VkFramebuffer> framebuffers
    )
        : m_vk_fns(vk_fns)
        , m_device(device)
        , m_swapchain(swapchain)
        , m_render_fence(render_fence)
        , m_present_semaphore(present_semaphore)
        , m_render_semaphore(render_semaphore)
        , m_window_size(window_size)
        , m_render_pass(render_pass)
        , m_framebuffers(framebuffers) {}

    auto begin(VkClearValue clear_value = VkClearValue{.color{.float32{0.f, 0.f, 0.f, 1.f}}}) -> std::expected<void, error>;

    auto end() -> std::expected<void, error>;

private:
    static constexpr auto a_second = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();

    vulkan_functions const& m_vk_fns;

    device_things const& m_device;
    swapchain_things const& m_swapchain;
    VkFence m_render_fence;
    VkSemaphore m_present_semaphore;
    VkSemaphore m_render_semaphore;

    VkExtent2D m_window_size;
    VkRenderPass m_render_pass;
    std::span<const VkFramebuffer> m_framebuffers;

    // filled in in begin()

    u32 m_swapchain_index = -1;
};

}
