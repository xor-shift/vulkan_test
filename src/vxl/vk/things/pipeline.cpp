#include <vxl/vk/things/pipeline.hpp>

namespace vxl::vk {

pipeline_things::~pipeline_things() {
    if (m_pipeline != nullptr) {
        m_vk_fns->destroy_pipeline(*m_device_things, m_pipeline);
        m_pipeline = nullptr;
    }
}

auto pipeline_things::init(pipeline_settings const& settings, VkDevice device, VkRenderPass render_pass) -> std::expected<void, error> {
    auto viewport_state = VkPipelineViewportStateCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .viewportCount = 1,
      .pViewports = &settings.m_viewport,
      .scissorCount = 1,
      .pScissors = &settings.m_scissor,
    };

    auto color_blending = VkPipelineColorBlendStateCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .logicOpEnable = VK_FALSE,
      .logicOp = VK_LOGIC_OP_COPY,
      .attachmentCount = 1,
      .pAttachments = &settings.m_color_blend_attachment,
    };

    auto pipeline_create_info = VkGraphicsPipelineCreateInfo{
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stageCount = static_cast<u32>(settings.m_shader_stages.size()),
      .pStages = settings.m_shader_stages.data(),
      .pVertexInputState = &settings.m_vertex_input_info,
      .pInputAssemblyState = &settings.m_input_assembly,
      .pTessellationState = nullptr,
      .pViewportState = &viewport_state,
      .pRasterizationState = &settings.m_rasterizer,
      .pMultisampleState = &settings.m_multisampling,
      .pDepthStencilState = nullptr,
      .pColorBlendState = &color_blending,
      .pDynamicState = nullptr,
      .layout = settings.m_pipeline_layout,
      .renderPass = render_pass,
      .subpass = 0,
      .basePipelineHandle = nullptr,
      .basePipelineIndex = 0,
    };

    auto pipelines = TRYX(error::from_vk(m_vk_fns->create_graphics_pipelines(device, nullptr, std::span(&pipeline_create_info, 1))));
    m_pipeline = pipelines[0];

    return {};
}

}
