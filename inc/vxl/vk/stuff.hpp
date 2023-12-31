#pragma once

#include <vxl/dyna_loader.hpp>
#include <vxl/vk/things/debug_messenger.hpp>
#include <vxl/vk/things/device.hpp>
#include <vxl/vk/things/frame.hpp>
#include <vxl/vk/things/instance.hpp>
#include <vxl/vk/things/pipeline.hpp>
#include <vxl/vk/things/surface.hpp>
#include <vxl/vk/things/swapchain.hpp>

#include <SDL_events.h>

namespace vxl::vk {

struct vulkan_stuff {
    static auto make() -> std::expected<vulkan_stuff, error>;

    vulkan_stuff() = default;

    ~vulkan_stuff();

    vulkan_stuff(vulkan_stuff&& other) noexcept;

    auto frame(VkBuffer vert_buffer, VkFence render_fence, VkSemaphore present_semaphore, VkSemaphore render_semaphore, usize frame_number) -> std::expected<bool, error>;

    auto run() -> std::expected<void, error>;

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
    std::shared_ptr<vulkan_functions> m_vk_fns;

    layer_extension_store<> m_device_extensions;

    std::shared_ptr<instance_things> m_instance = nullptr;
    std::shared_ptr<debug_messenger_things> m_debug_messenger = nullptr;

    VkExtent2D m_window_size;
    std::shared_ptr<surface_things> m_surface = nullptr;

    std::shared_ptr<device_things> m_device = nullptr;

    std::shared_ptr<swapchain_things> m_swapchain = nullptr;

    VkRenderPass m_vk_render_pass;
    std::pmr::vector<VkFramebuffer> m_vk_framebuffers;

    VkShaderModule m_vertex_shader;
    VkShaderModule m_fragment_shader;

    VkPipelineLayout m_pipeline_layout;
    std::shared_ptr<pipeline_things> m_pipeline;

    auto try_init_device(VkPhysicalDevice const& physical_device, usize queue_family_index) const -> std::expected<VkDevice, error>;

    auto init_vk_devices_and_queues() -> std::expected<void, error>;

    auto read_shader(const char* file_name) -> std::expected<VkShaderModule, error>;

    auto init_vk_command_pool() -> std::expected<void, error>;

    auto init_vk_command_buffer() -> std::expected<void, error>;

    auto destroy_render_pass() -> std::expected<void, error>;

    auto reinit_vk_swapchain(VkExtent2D extent) -> std::expected<void, error>;
};

}  // namespace vxl::vk
