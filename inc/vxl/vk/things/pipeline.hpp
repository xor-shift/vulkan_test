#pragma once

#include <vxl/vk/functions.hpp>
#include <vxl/vk/things/device.hpp>

namespace vxl::vk {

struct pipeline_settings {
    std::vector<VkPipelineShaderStageCreateInfo> m_shader_stages;
    VkPipelineVertexInputStateCreateInfo m_vertex_input_info;
    VkPipelineInputAssemblyStateCreateInfo m_input_assembly;
    VkViewport m_viewport;
    VkRect2D m_scissor;
    VkPipelineRasterizationStateCreateInfo m_rasterizer;
    VkPipelineColorBlendAttachmentState m_color_blend_attachment;
    VkPipelineMultisampleStateCreateInfo m_multisampling;
    VkPipelineLayout m_pipeline_layout;
};

struct pipeline_things {
    pipeline_things(std::shared_ptr<vulkan_functions> vk_fns, std::shared_ptr<device_things> device_things)
        : m_vk_fns(std::move(vk_fns))
        , m_device_things(std::move(device_things)) {}

    pipeline_things(pipeline_things const&) = delete;
    pipeline_things(pipeline_things&&) = delete;

    ~pipeline_things();

    auto init(pipeline_settings const& settings, VkDevice device, VkRenderPass render_pass) -> std::expected<void, error>;

    constexpr auto pipeline() const -> VkPipeline { return m_pipeline; }
    constexpr operator VkPipeline() const { return m_pipeline; }

private:
    std::shared_ptr<vulkan_functions> m_vk_fns;
    std::shared_ptr<device_things> m_device_things;

    VkPipelineLayout m_pipeline_layout_ref;
    VkPipeline m_pipeline = nullptr;
};

}