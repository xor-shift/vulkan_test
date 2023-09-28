#include <vxl/vk/functions.hpp>

namespace vxl::vk {

#pragma push_macro("PTR_NAME_PAIR")
#pragma push_macro("PTR_PTR_PAIR")

#define PTR_NAME_PAIR(_name) \
    { reinterpret_cast<void (*vulkan_functions::*)()>(&vulkan_functions::_name), #_name }

void vulkan_functions::init(PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr) {
    vkGetInstanceProcAddr = vk_get_instance_proc_addr;

#include <vxl/vk/detail/vk_functions/generated/pre_instance_init.ipp>

    for (auto [ptr, name] : functions_and_names) {
        this->*ptr = vkGetInstanceProcAddr(nullptr, name);
    }
}

void vulkan_functions::init(VkInstance instance) {
#include <vxl/vk/detail/vk_functions/generated/instance_init.ipp>

    for (auto [ptr, name] : functions_and_names) {
        this->*ptr = vkGetInstanceProcAddr(instance, name);
    }

    // extension: VK_EXT_extended_dynamic_state
    vkCmdSetCullMode = vkCmdSetCullMode ?: vkCmdSetCullModeEXT;
    vkCmdSetFrontFace = vkCmdSetFrontFace ?: vkCmdSetFrontFaceEXT;
    vkCmdSetPrimitiveTopology = vkCmdSetPrimitiveTopology ?: vkCmdSetPrimitiveTopologyEXT;
    vkCmdSetViewportWithCount = vkCmdSetViewportWithCount ?: vkCmdSetViewportWithCountEXT;
    vkCmdSetScissorWithCount = vkCmdSetScissorWithCount ?: vkCmdSetScissorWithCountEXT;
    vkCmdBindVertexBuffers2 = vkCmdBindVertexBuffers2 ?: vkCmdBindVertexBuffers2EXT;
    vkCmdSetDepthTestEnable = vkCmdSetDepthTestEnable ?: vkCmdSetDepthTestEnableEXT;
    vkCmdSetDepthWriteEnable = vkCmdSetDepthWriteEnable ?: vkCmdSetDepthWriteEnableEXT;
    vkCmdSetDepthCompareOp = vkCmdSetDepthCompareOp ?: vkCmdSetDepthCompareOpEXT;
    vkCmdSetDepthBoundsTestEnable = vkCmdSetDepthBoundsTestEnable ?: vkCmdSetDepthBoundsTestEnableEXT;
    vkCmdSetStencilTestEnable = vkCmdSetStencilTestEnable ?: vkCmdSetStencilTestEnableEXT;
    vkCmdSetStencilOp = vkCmdSetStencilOp ?: vkCmdSetStencilOpEXT;

    // extension: VK_KHR_synchronization2
    vkCmdSetEvent2 = vkCmdSetEvent2 ?: vkCmdSetEvent2KHR;
    vkCmdResetEvent2 = vkCmdResetEvent2 ?: vkCmdResetEvent2KHR;
    vkCmdWaitEvents2 = vkCmdWaitEvents2 ?: vkCmdWaitEvents2KHR;
    vkCmdPipelineBarrier2 = vkCmdPipelineBarrier2 ?: vkCmdPipelineBarrier2KHR;
    vkCmdWriteTimestamp2 = vkCmdWriteTimestamp2 ?: vkCmdWriteTimestamp2KHR;
    vkQueueSubmit2 = vkQueueSubmit2 ?: vkQueueSubmit2KHR;

    // extension: VK_KHR_copy_commands2
    vkCmdCopyBuffer2 = vkCmdCopyBuffer2 ?: vkCmdCopyBuffer2KHR;
    vkCmdCopyImage2 = vkCmdCopyImage2 ?: vkCmdCopyImage2KHR;
    vkCmdCopyBufferToImage2 = vkCmdCopyBufferToImage2 ?: vkCmdCopyBufferToImage2KHR;
    vkCmdCopyImageToBuffer2 = vkCmdCopyImageToBuffer2 ?: vkCmdCopyImageToBuffer2KHR;
    vkCmdBlitImage2 = vkCmdBlitImage2 ?: vkCmdBlitImage2KHR;
    vkCmdResolveImage2 = vkCmdResolveImage2 ?: vkCmdResolveImage2KHR;
}

void vulkan_functions::init(VkDevice device) {
#include <vxl/vk/detail/vk_functions/generated/device_init.ipp>

    for (auto [ptr, name] : functions_and_names) {
        this->*ptr = vkGetDeviceProcAddr(device, name);
    }

    // extension: VK_EXT_extended_dynamic_state
    vkCmdSetCullMode = vkCmdSetCullMode ?: vkCmdSetCullModeEXT;
    vkCmdSetFrontFace = vkCmdSetFrontFace ?: vkCmdSetFrontFaceEXT;
    vkCmdSetPrimitiveTopology = vkCmdSetPrimitiveTopology ?: vkCmdSetPrimitiveTopologyEXT;
    vkCmdSetViewportWithCount = vkCmdSetViewportWithCount ?: vkCmdSetViewportWithCountEXT;
    vkCmdSetScissorWithCount = vkCmdSetScissorWithCount ?: vkCmdSetScissorWithCountEXT;
    vkCmdBindVertexBuffers2 = vkCmdBindVertexBuffers2 ?: vkCmdBindVertexBuffers2EXT;
    vkCmdSetDepthTestEnable = vkCmdSetDepthTestEnable ?: vkCmdSetDepthTestEnableEXT;
    vkCmdSetDepthWriteEnable = vkCmdSetDepthWriteEnable ?: vkCmdSetDepthWriteEnableEXT;
    vkCmdSetDepthCompareOp = vkCmdSetDepthCompareOp ?: vkCmdSetDepthCompareOpEXT;
    vkCmdSetDepthBoundsTestEnable = vkCmdSetDepthBoundsTestEnable ?: vkCmdSetDepthBoundsTestEnableEXT;
    vkCmdSetStencilTestEnable = vkCmdSetStencilTestEnable ?: vkCmdSetStencilTestEnableEXT;
    vkCmdSetStencilOp = vkCmdSetStencilOp ?: vkCmdSetStencilOpEXT;

    // extension: VK_KHR_synchronization2
    vkCmdSetEvent2 = vkCmdSetEvent2 ?: vkCmdSetEvent2KHR;
    vkCmdResetEvent2 = vkCmdResetEvent2 ?: vkCmdResetEvent2KHR;
    vkCmdWaitEvents2 = vkCmdWaitEvents2 ?: vkCmdWaitEvents2KHR;
    vkCmdPipelineBarrier2 = vkCmdPipelineBarrier2 ?: vkCmdPipelineBarrier2KHR;
    vkCmdWriteTimestamp2 = vkCmdWriteTimestamp2 ?: vkCmdWriteTimestamp2KHR;
    vkQueueSubmit2 = vkQueueSubmit2 ?: vkQueueSubmit2KHR;

    // extension: VK_KHR_copy_commands2
    vkCmdCopyBuffer2 = vkCmdCopyBuffer2 ?: vkCmdCopyBuffer2KHR;
    vkCmdCopyImage2 = vkCmdCopyImage2 ?: vkCmdCopyImage2KHR;
    vkCmdCopyBufferToImage2 = vkCmdCopyBufferToImage2 ?: vkCmdCopyBufferToImage2KHR;
    vkCmdCopyImageToBuffer2 = vkCmdCopyImageToBuffer2 ?: vkCmdCopyImageToBuffer2KHR;
    vkCmdBlitImage2 = vkCmdBlitImage2 ?: vkCmdBlitImage2KHR;
    vkCmdResolveImage2 = vkCmdResolveImage2 ?: vkCmdResolveImage2KHR;

    // extension: VK_EXT_extended_dynamic_state2
    vkCmdSetRasterizerDiscardEnable = vkCmdSetRasterizerDiscardEnable ?: vkCmdSetRasterizerDiscardEnableEXT;
    vkCmdSetDepthBiasEnable = vkCmdSetDepthBiasEnable ?: vkCmdSetDepthBiasEnableEXT;
    vkCmdSetPrimitiveRestartEnable = vkCmdSetPrimitiveRestartEnable ?: vkCmdSetPrimitiveRestartEnableEXT;
}

#pragma pop_macro("PTR_PTR_PAIR")
#pragma pop_macro("PTR_NAME_PAIR")

}  // namespace vxl::vk
