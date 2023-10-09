#include <vxl/vk/things/frame.hpp>

namespace vxl::vk {

auto frame_things::begin(VkClearValue clear_value) -> std::expected<void, error> {
    switch (auto&& res = TRYX(error::from_vk(
              m_vk_fns.acquire_next_image_khr(m_device, m_swapchain, a_second, m_present_semaphore, nullptr), "could not acquire the next image in the swapchain"
            ));
            res.second) {
        case VK_SUCCESS: [[fallthrough]];
        case VK_SUBOPTIMAL_KHR: m_swapchain_index = res.first; break;
        default: return std::unexpected{error::make_vk(res.second, "could not acquire the next image in the swapchain")};
    }

    auto const& command_queue = m_device.queue(0);

    TRYX(error::from_vk(m_vk_fns.reset_command_buffer(command_queue), "failed to reset a command buffer"));

    const auto command_begin_info = VkCommandBufferBeginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      .pInheritanceInfo = nullptr,
    };
    TRYX(error::from_vk(m_vk_fns.begin_command_buffer(command_queue, &command_begin_info), "failed to begin a command buffer"));

    const auto render_pass_info = VkRenderPassBeginInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .pNext = nullptr,
      .renderPass = m_render_pass,
      .framebuffer = m_framebuffers[m_swapchain_index],
      .renderArea = VkRect2D{{0, 0}, m_window_size},
      .clearValueCount = 1,
      .pClearValues = &clear_value,
    };

    m_vk_fns.cmd_begin_render_pass(m_device.queue(0), &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    return {};
}

auto frame_things::end() -> std::expected<void, error> {
    auto const& command_queue = m_device.queue(0);

    m_vk_fns.cmd_end_render_pass(command_queue);

    TRYX(error::from_vk(m_vk_fns.end_command_buffer(command_queue), "failed to end the command buffer"));

    const auto wait_dst_stage_flags = (VkPipelineStageFlags)VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    const auto submit_info = VkSubmitInfo{
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &m_present_semaphore,
      .pWaitDstStageMask = &wait_dst_stage_flags,
      .commandBufferCount = 1,
      .pCommandBuffers = &(command_queue.command_buffer),
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &m_render_semaphore,
    };

    TRYX(error::from_vk(m_vk_fns.queue_submit(command_queue, 1, &submit_info, m_render_fence), "failed to submit commands to the queue"));

    auto* const swapchain = m_swapchain.swapchain();
    const auto present_info = VkPresentInfoKHR{
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &m_render_semaphore,
      .swapchainCount = 1,
      .pSwapchains = &swapchain,
      .pImageIndices = &m_swapchain_index,
      .pResults = nullptr,
    };

    switch (auto&& res = TRYX(error::from_vk(m_vk_fns.queue_present_khr(command_queue, &present_info), "failed to present")); res) {
        case VK_SUCCESS: [[fallthrough]];
        case VK_SUBOPTIMAL_KHR: [[fallthrough]];
        case VK_ERROR_OUT_OF_DATE_KHR: break;
        default: return std::unexpected{error::make_vk(std::move(res), "failed to present")};
    }

    return {};
}

}
