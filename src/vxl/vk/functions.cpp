#include <vxl/vk/functions.hpp>

namespace vxl::vk {

#pragma push_macro("PTR_NAME_PAIR")
#pragma push_macro("PTR_PTR_PAIR")

#define PTR_NAME_PAIR(_name) \
    { reinterpret_cast<void (*vulkan_functions::*)()>(&vulkan_functions::_name), #_name }

void vulkan_functions::init(PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr) {
    vkGetInstanceProcAddr = vk_get_instance_proc_addr;

    static const std::pair<void (*vulkan_functions::*)(), const char*> functions_and_names[]{
      // vulkan 1.0
      {reinterpret_cast<void (*vulkan_functions::*)()>(&vulkan_functions::vkCreateInstance), "vkCreateInstance"},
      {reinterpret_cast<void (*vulkan_functions::*)()>(&vulkan_functions::vkEnumerateInstanceExtensionProperties), "vkEnumerateInstanceExtensionProperties"},
      {reinterpret_cast<void (*vulkan_functions::*)()>(&vulkan_functions::vkEnumerateInstanceLayerProperties), "vkEnumerateInstanceLayerProperties"},

      // vulkan 1.1
      {reinterpret_cast<void (*vulkan_functions::*)()>(&vulkan_functions::vkEnumerateInstanceVersion), "vkEnumerateInstanceVersion"},
    };

    for (auto [ptr, name] : functions_and_names) {
        this->*ptr = vkGetInstanceProcAddr(nullptr, name);
    }
}

void vulkan_functions::init(VkInstance instance) {
    // the statement-expression is to be able to collapse this block in my IDE
    static const auto functions_and_names = ({
        static const std::pair<void (*vulkan_functions::*)(), const char*> arr[]{
          // vulkan version 1.0
          PTR_NAME_PAIR(vkDestroyInstance),
          PTR_NAME_PAIR(vkEnumeratePhysicalDevices),
          PTR_NAME_PAIR(vkGetPhysicalDeviceFeatures),
          PTR_NAME_PAIR(vkGetPhysicalDeviceFormatProperties),
          PTR_NAME_PAIR(vkGetPhysicalDeviceImageFormatProperties),
          PTR_NAME_PAIR(vkGetPhysicalDeviceProperties),
          PTR_NAME_PAIR(vkGetPhysicalDeviceQueueFamilyProperties),
          PTR_NAME_PAIR(vkGetPhysicalDeviceMemoryProperties),
          PTR_NAME_PAIR(vkGetDeviceProcAddr),
          PTR_NAME_PAIR(vkCreateDevice),
          PTR_NAME_PAIR(vkDestroyDevice),
          PTR_NAME_PAIR(vkEnumerateDeviceExtensionProperties),
          PTR_NAME_PAIR(vkEnumerateDeviceLayerProperties),
          PTR_NAME_PAIR(vkGetDeviceQueue),
          PTR_NAME_PAIR(vkQueueSubmit),
          PTR_NAME_PAIR(vkQueueWaitIdle),
          PTR_NAME_PAIR(vkDeviceWaitIdle),
          PTR_NAME_PAIR(vkAllocateMemory),
          PTR_NAME_PAIR(vkMapMemory),
          PTR_NAME_PAIR(vkUnmapMemory),
          PTR_NAME_PAIR(vkFlushMappedMemoryRanges),
          PTR_NAME_PAIR(vkInvalidateMappedMemoryRanges),
          PTR_NAME_PAIR(vkGetDeviceMemoryCommitment),
          PTR_NAME_PAIR(vkBindBufferMemory),
          PTR_NAME_PAIR(vkBindImageMemory),
          PTR_NAME_PAIR(vkGetBufferMemoryRequirements),
          PTR_NAME_PAIR(vkGetImageMemoryRequirements),
          PTR_NAME_PAIR(vkCreateFence),
          PTR_NAME_PAIR(vkDestroyFence),
          PTR_NAME_PAIR(vkResetFences),
          PTR_NAME_PAIR(vkGetFenceStatus),
          PTR_NAME_PAIR(vkWaitForFences),
          PTR_NAME_PAIR(vkCreateSemaphore),
          PTR_NAME_PAIR(vkDestroySemaphore),
          PTR_NAME_PAIR(vkCreateEvent),
          PTR_NAME_PAIR(vkDestroyEvent),
          PTR_NAME_PAIR(vkGetEventStatus),
          PTR_NAME_PAIR(vkSetEvent),
          PTR_NAME_PAIR(vkResetEvent),
          PTR_NAME_PAIR(vkCreateQueryPool),
          PTR_NAME_PAIR(vkGetQueryPoolResults),
          PTR_NAME_PAIR(vkCreateBuffer),
          PTR_NAME_PAIR(vkDestroyBuffer),
          PTR_NAME_PAIR(vkCreateBufferView),
          PTR_NAME_PAIR(vkDestroyBufferView),
          PTR_NAME_PAIR(vkCreateImage),
          PTR_NAME_PAIR(vkDestroyImage),
          PTR_NAME_PAIR(vkGetImageSubresourceLayout),
          PTR_NAME_PAIR(vkCreateImageView),
          PTR_NAME_PAIR(vkDestroyImageView),
          PTR_NAME_PAIR(vkCreatePipelineCache),
          PTR_NAME_PAIR(vkDestroyPipelineCache),
          PTR_NAME_PAIR(vkCreateGraphicsPipelines),
          PTR_NAME_PAIR(vkCreateComputePipelines),
          PTR_NAME_PAIR(vkDestroyPipeline),
          PTR_NAME_PAIR(vkCreatePipelineLayout),
          PTR_NAME_PAIR(vkDestroyPipelineLayout),
          PTR_NAME_PAIR(vkCreateSampler),
          PTR_NAME_PAIR(vkDestroySampler),
          PTR_NAME_PAIR(vkCreateDescriptorSetLayout),
          PTR_NAME_PAIR(vkDestroyDescriptorSetLayout),
          PTR_NAME_PAIR(vkCreateDescriptorPool),
          PTR_NAME_PAIR(vkResetDescriptorPool),
          PTR_NAME_PAIR(vkAllocateDescriptorSets),
          PTR_NAME_PAIR(vkFreeDescriptorSets),
          PTR_NAME_PAIR(vkUpdateDescriptorSets),
          PTR_NAME_PAIR(vkCreateFramebuffer),
          PTR_NAME_PAIR(vkDestroyFramebuffer),
          PTR_NAME_PAIR(vkCreateRenderPass),
          PTR_NAME_PAIR(vkDestroyRenderPass),
          PTR_NAME_PAIR(vkGetRenderAreaGranularity),
          PTR_NAME_PAIR(vkCreateCommandPool),
          PTR_NAME_PAIR(vkResetCommandPool),
          PTR_NAME_PAIR(vkAllocateCommandBuffers),
          PTR_NAME_PAIR(vkFreeCommandBuffers),
          PTR_NAME_PAIR(vkBeginCommandBuffer),
          PTR_NAME_PAIR(vkEndCommandBuffer),
          PTR_NAME_PAIR(vkResetCommandBuffer),
          PTR_NAME_PAIR(vkCmdBindPipeline),
          PTR_NAME_PAIR(vkCmdSetViewport),
          PTR_NAME_PAIR(vkCmdSetScissor),
          PTR_NAME_PAIR(vkCmdSetLineWidth),
          PTR_NAME_PAIR(vkCmdSetDepthBias),
          PTR_NAME_PAIR(vkCmdSetBlendConstants),
          PTR_NAME_PAIR(vkCmdSetDepthBounds),
          PTR_NAME_PAIR(vkCmdSetStencilCompareMask),
          PTR_NAME_PAIR(vkCmdSetStencilWriteMask),
          PTR_NAME_PAIR(vkCmdSetStencilReference),
          PTR_NAME_PAIR(vkCmdBindDescriptorSets),
          PTR_NAME_PAIR(vkCmdBindIndexBuffer),
          PTR_NAME_PAIR(vkCmdBindVertexBuffers),
          PTR_NAME_PAIR(vkCmdDraw),
          PTR_NAME_PAIR(vkCmdDrawIndexed),
          PTR_NAME_PAIR(vkCmdDrawIndirect),
          PTR_NAME_PAIR(vkCmdDrawIndexedIndirect),
          PTR_NAME_PAIR(vkCmdDispatch),
          PTR_NAME_PAIR(vkCmdDispatchIndirect),
          PTR_NAME_PAIR(vkCmdCopyBuffer),
          PTR_NAME_PAIR(vkCmdCopyImage),
          PTR_NAME_PAIR(vkCmdBlitImage),
          PTR_NAME_PAIR(vkCmdCopyBufferToImage),
          PTR_NAME_PAIR(vkCmdCopyImageToBuffer),
          PTR_NAME_PAIR(vkCmdUpdateBuffer),
          PTR_NAME_PAIR(vkCmdFillBuffer),
          PTR_NAME_PAIR(vkCmdClearColorImage),
          PTR_NAME_PAIR(vkCmdClearDepthStencilImage),
          PTR_NAME_PAIR(vkCmdClearAttachments),
          PTR_NAME_PAIR(vkCmdResolveImage),
          PTR_NAME_PAIR(vkCmdSetEvent),
          PTR_NAME_PAIR(vkCmdResetEvent),
          PTR_NAME_PAIR(vkCmdWaitEvents),
          PTR_NAME_PAIR(vkCmdPipelineBarrier),
          PTR_NAME_PAIR(vkCmdBeginQuery),
          PTR_NAME_PAIR(vkCmdEndQuery),
          PTR_NAME_PAIR(vkCmdResetQueryPool),
          PTR_NAME_PAIR(vkCmdWriteTimestamp),
          PTR_NAME_PAIR(vkCmdCopyQueryPoolResults),
          PTR_NAME_PAIR(vkCmdPushConstants),
          PTR_NAME_PAIR(vkCmdBeginRenderPass),
          PTR_NAME_PAIR(vkCmdNextSubpass),
          PTR_NAME_PAIR(vkCmdEndRenderPass),
          PTR_NAME_PAIR(vkCmdExecuteCommands),

          // vulkan version 1.1
          PTR_NAME_PAIR(vkBindBufferMemory2),
          PTR_NAME_PAIR(vkBindImageMemory2),
          PTR_NAME_PAIR(vkGetDeviceGroupPeerMemoryFeatures),
          PTR_NAME_PAIR(vkCmdSetDeviceMask),
          PTR_NAME_PAIR(vkCmdDispatchBase),
          PTR_NAME_PAIR(vkEnumeratePhysicalDeviceGroups),
          PTR_NAME_PAIR(vkGetImageMemoryRequirements2),
          PTR_NAME_PAIR(vkGetBufferMemoryRequirements2),
          PTR_NAME_PAIR(vkGetPhysicalDeviceFeatures2),
          PTR_NAME_PAIR(vkGetPhysicalDeviceProperties2),
          PTR_NAME_PAIR(vkGetPhysicalDeviceFormatProperties2),
          PTR_NAME_PAIR(vkGetPhysicalDeviceImageFormatProperties2),
          PTR_NAME_PAIR(vkGetPhysicalDeviceQueueFamilyProperties2),
          PTR_NAME_PAIR(vkGetPhysicalDeviceMemoryProperties2),
          PTR_NAME_PAIR(vkGetDeviceQueue2),
          PTR_NAME_PAIR(vkCreateSamplerYcbcrConversion),
          PTR_NAME_PAIR(vkDestroySamplerYcbcrConversion),
          PTR_NAME_PAIR(vkGetPhysicalDeviceExternalBufferProperties),
          PTR_NAME_PAIR(vkGetPhysicalDeviceExternalFenceProperties),
          PTR_NAME_PAIR(vkGetPhysicalDeviceExternalSemaphoreProperties),
          PTR_NAME_PAIR(vkGetDescriptorSetLayoutSupport),

          // vulkan version 1.2
          PTR_NAME_PAIR(vkCmdDrawIndirectCount),
          PTR_NAME_PAIR(vkCmdDrawIndexedIndirectCount),
          PTR_NAME_PAIR(vkCreateRenderPass2),
          PTR_NAME_PAIR(vkCmdBeginRenderPass2),
          PTR_NAME_PAIR(vkCmdNextSubpass2),
          PTR_NAME_PAIR(vkCmdEndRenderPass2),
          PTR_NAME_PAIR(vkResetQueryPool),
          PTR_NAME_PAIR(vkGetSemaphoreCounterValue),
          PTR_NAME_PAIR(vkWaitSemaphores),
          PTR_NAME_PAIR(vkSignalSemaphore),
          PTR_NAME_PAIR(vkGetBufferDeviceAddress),
          PTR_NAME_PAIR(vkGetBufferOpaqueCaptureAddress),
          PTR_NAME_PAIR(vkGetDeviceMemoryOpaqueCaptureAddress),

          // vulkan version 1.3
          PTR_NAME_PAIR(vkGetPhysicalDeviceToolProperties),
          PTR_NAME_PAIR(vkCreatePrivateDataSlot),
          PTR_NAME_PAIR(vkDestroyPrivateDataSlot),
          PTR_NAME_PAIR(vkSetPrivateData),
          PTR_NAME_PAIR(vkGetPrivateData),
          PTR_NAME_PAIR(vkCmdSetEvent2),
          PTR_NAME_PAIR(vkCmdResetEvent2),
          PTR_NAME_PAIR(vkCmdWaitEvents2),
          PTR_NAME_PAIR(vkCmdPipelineBarrier2),
          PTR_NAME_PAIR(vkCmdWriteTimestamp2),
          PTR_NAME_PAIR(vkQueueSubmit2),
          PTR_NAME_PAIR(vkCmdCopyBuffer2),
          PTR_NAME_PAIR(vkCmdCopyImage2),
          PTR_NAME_PAIR(vkCmdCopyBufferToImage2),
          PTR_NAME_PAIR(vkCmdCopyImageToBuffer2),
          PTR_NAME_PAIR(vkCmdBlitImage2),
          PTR_NAME_PAIR(vkCmdResolveImage2),
          PTR_NAME_PAIR(vkCmdBeginRendering),
          PTR_NAME_PAIR(vkCmdEndRendering),
          PTR_NAME_PAIR(vkCmdSetCullMode),
          PTR_NAME_PAIR(vkCmdSetFrontFace),
          PTR_NAME_PAIR(vkCmdSetPrimitiveTopology),
          PTR_NAME_PAIR(vkCmdSetViewportWithCount),
          PTR_NAME_PAIR(vkCmdSetScissorWithCount),
          PTR_NAME_PAIR(vkCmdBindVertexBuffers2),
          PTR_NAME_PAIR(vkCmdSetDepthTestEnable),
          PTR_NAME_PAIR(vkCmdSetDepthWriteEnable),
          PTR_NAME_PAIR(vkCmdSetDepthCompareOp),
          PTR_NAME_PAIR(vkCmdSetDepthBoundsTestEnable),
          PTR_NAME_PAIR(vkCmdSetStencilTestEnable),
          PTR_NAME_PAIR(vkCmdSetStencilOp),
          PTR_NAME_PAIR(vkCmdSetRasterizerDiscardEnable),
          PTR_NAME_PAIR(vkCmdSetDepthBiasEnable),
          PTR_NAME_PAIR(vkCmdSetPrimitiveRestartEnable),
          PTR_NAME_PAIR(vkGetDeviceBufferMemoryRequirements),
          PTR_NAME_PAIR(vkGetDeviceImageMemoryRequirements),
          PTR_NAME_PAIR(vkGetDeviceImageSparseMemoryRequirements),

          // extension: VK_KHR_surface
          PTR_NAME_PAIR(vkDestroySurfaceKHR),
          PTR_NAME_PAIR(vkGetPhysicalDeviceSurfaceSupportKHR),
          PTR_NAME_PAIR(vkGetPhysicalDeviceSurfaceCapabilitiesKHR),
          PTR_NAME_PAIR(vkGetPhysicalDeviceSurfaceFormatsKHR),
          PTR_NAME_PAIR(vkGetPhysicalDeviceSurfacePresentModesKHR),

          // extension: VK_KHR_swapchain
          PTR_NAME_PAIR(vkCreateSwapchainKHR),
          PTR_NAME_PAIR(vkGetSwapchainImagesKHR),
          PTR_NAME_PAIR(vkAcquireNextImageKHR),
          PTR_NAME_PAIR(vkQueuePresentKHR),
          PTR_NAME_PAIR(vkGetDeviceGroupPresentCapabilitiesKHR),
          PTR_NAME_PAIR(vkGetDeviceGroupSurfacePresentModesKHR),
          PTR_NAME_PAIR(vkGetPhysicalDevicePresentRectanglesKHR),
          PTR_NAME_PAIR(vkAcquireNextImage2KHR),

          // extension: VK_KHR_display
          PTR_NAME_PAIR(vkGetPhysicalDeviceDisplayPropertiesKHR),
          PTR_NAME_PAIR(vkGetPhysicalDeviceDisplayPlanePropertiesKHR),
          PTR_NAME_PAIR(vkGetDisplayPlaneSupportedDisplaysKHR),
          PTR_NAME_PAIR(vkGetDisplayModePropertiesKHR),
          PTR_NAME_PAIR(vkCreateDisplayModeKHR),
          PTR_NAME_PAIR(vkGetDisplayPlaneCapabilitiesKHR),
          PTR_NAME_PAIR(vkCreateDisplayPlaneSurfaceKHR),

          // extension: VK_KHR_display_swapchain
          PTR_NAME_PAIR(vkCreateSharedSwapchainsKHR),

          // extension: VK_KHR_external_memory_fd
          PTR_NAME_PAIR(vkGetMemoryFdKHR),
          PTR_NAME_PAIR(vkGetMemoryFdPropertiesKHR),

          // extension: VK_KHR_external_semaphore_fd
          PTR_NAME_PAIR(vkImportSemaphoreFdKHR),
          PTR_NAME_PAIR(vkGetSemaphoreFdKHR),

          // extension: VK_EXT_direct_mode_display
          PTR_NAME_PAIR(vkReleaseDisplayEXT),

          // extension: VK_EXT_display_surface_counter
          PTR_NAME_PAIR(vkGetPhysicalDeviceSurfaceCapabilities2EXT),

          // extension: VK_EXT_display_control
          PTR_NAME_PAIR(vkDisplayPowerControlEXT),
          PTR_NAME_PAIR(vkRegisterDeviceEventEXT),
          PTR_NAME_PAIR(vkRegisterDisplayEventEXT),
          PTR_NAME_PAIR(vkGetSwapchainCounterEXT),

          // extension: VK_EXT_discard_rectangles
          PTR_NAME_PAIR(vkCmdSetDiscardRectangleEXT),
          PTR_NAME_PAIR(vkCmdSetDiscardRectangleEnableEXT),
          PTR_NAME_PAIR(vkCmdSetDiscardRectangleModeEXT),

          // extension: VK_EXT_hdr_metadata
          PTR_NAME_PAIR(vkSetHdrMetadataEXT),

          // extension: VK_KHR_shared_presentable_image
          PTR_NAME_PAIR(vkGetSwapchainStatusKHR),

          // extension: VK_KHR_external_fence_fd
          PTR_NAME_PAIR(vkImportFenceFdKHR),
          PTR_NAME_PAIR(vkGetFenceFdKHR),

          // extension: VK_KHR_performance_query
          PTR_NAME_PAIR(vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR),
          PTR_NAME_PAIR(vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR),
          PTR_NAME_PAIR(vkAcquireProfilingLockKHR),
          PTR_NAME_PAIR(vkReleaseProfilingLockKHR),

          // extension: VK_KHR_get_surface_capabilities2
          PTR_NAME_PAIR(vkGetPhysicalDeviceSurfaceCapabilities2KHR),
          PTR_NAME_PAIR(vkGetPhysicalDeviceSurfaceFormats2KHR),

          // extension: VK_KHR_get_display_properties2
          PTR_NAME_PAIR(vkGetPhysicalDeviceDisplayProperties2KHR),
          PTR_NAME_PAIR(vkGetPhysicalDeviceDisplayPlaneProperties2KHR),
          PTR_NAME_PAIR(vkGetDisplayModeProperties2KHR),
          PTR_NAME_PAIR(vkGetDisplayPlaneCapabilities2KHR),

          // extension: VK_EXT_debug_utils
          PTR_NAME_PAIR(vkSetDebugUtilsObjectNameEXT),
          PTR_NAME_PAIR(vkSetDebugUtilsObjectTagEXT),
          PTR_NAME_PAIR(vkQueueBeginDebugUtilsLabelEXT),
          PTR_NAME_PAIR(vkQueueEndDebugUtilsLabelEXT),
          PTR_NAME_PAIR(vkQueueInsertDebugUtilsLabelEXT),
          PTR_NAME_PAIR(vkCmdBeginDebugUtilsLabelEXT),
          PTR_NAME_PAIR(vkCmdEndDebugUtilsLabelEXT),
          PTR_NAME_PAIR(vkCmdInsertDebugUtilsLabelEXT),
          PTR_NAME_PAIR(vkCreateDebugUtilsMessengerEXT),
          PTR_NAME_PAIR(vkDestroyDebugUtilsMessengerEXT),
          PTR_NAME_PAIR(vkSubmitDebugUtilsMessageEXT),

          // extension: VK_EXT_sample_locations
          PTR_NAME_PAIR(vkCmdSetSampleLocationsEXT),
          PTR_NAME_PAIR(vkGetPhysicalDeviceMultisamplePropertiesEXT),

          // extension: VK_EXT_image_drm_format_modifier
          PTR_NAME_PAIR(vkGetImageDrmFormatModifierPropertiesEXT),

          // extension: VK_EXT_external_memory_host
          PTR_NAME_PAIR(vkGetMemoryHostPointerPropertiesEXT),

          // extension: VK_EXT_calibrated_timestamps
          PTR_NAME_PAIR(vkGetPhysicalDeviceCalibrateableTimeDomainsEXT),
          PTR_NAME_PAIR(vkGetCalibratedTimestampsEXT),

          // extension: VK_KHR_fragment_shading_rate
          PTR_NAME_PAIR(vkGetPhysicalDeviceFragmentShadingRatesKHR),
          PTR_NAME_PAIR(vkCmdSetFragmentShadingRateKHR),

          // extension: VK_EXT_headless_surface
          PTR_NAME_PAIR(vkCreateHeadlessSurfaceEXT),

          // extension: VK_EXT_line_rasterization
          PTR_NAME_PAIR(vkCmdSetLineStippleEXT),

          // extension: VK_EXT_extended_dynamic_state
          PTR_NAME_PAIR(vkCmdSetCullModeEXT),
          PTR_NAME_PAIR(vkCmdSetFrontFaceEXT),
          PTR_NAME_PAIR(vkCmdSetPrimitiveTopologyEXT),
          PTR_NAME_PAIR(vkCmdSetViewportWithCountEXT),
          PTR_NAME_PAIR(vkCmdSetScissorWithCountEXT),
          PTR_NAME_PAIR(vkCmdBindVertexBuffers2EXT),
          PTR_NAME_PAIR(vkCmdSetDepthTestEnableEXT),
          PTR_NAME_PAIR(vkCmdSetDepthWriteEnableEXT),
          PTR_NAME_PAIR(vkCmdSetDepthCompareOpEXT),
          PTR_NAME_PAIR(vkCmdSetDepthBoundsTestEnableEXT),
          PTR_NAME_PAIR(vkCmdSetStencilTestEnableEXT),
          PTR_NAME_PAIR(vkCmdSetStencilOpEXT),

          // extension: VK_KHR_synchronization2
          PTR_NAME_PAIR(vkCmdSetEvent2KHR),
          PTR_NAME_PAIR(vkCmdResetEvent2KHR),
          PTR_NAME_PAIR(vkCmdWaitEvents2KHR),
          PTR_NAME_PAIR(vkCmdPipelineBarrier2KHR),
          PTR_NAME_PAIR(vkCmdWriteTimestamp2KHR),
          PTR_NAME_PAIR(vkQueueSubmit2KHR),
          PTR_NAME_PAIR(vkCmdWriteBufferMarker2AMD),
          PTR_NAME_PAIR(vkGetQueueCheckpointData2NV),

          // extension: VK_KHR_copy_commands2
          PTR_NAME_PAIR(vkCmdCopyBuffer2KHR),
          PTR_NAME_PAIR(vkCmdCopyImage2KHR),
          PTR_NAME_PAIR(vkCmdCopyBufferToImage2KHR),
          PTR_NAME_PAIR(vkCmdCopyImageToBuffer2KHR),
          PTR_NAME_PAIR(vkCmdBlitImage2KHR),
          PTR_NAME_PAIR(vkCmdResolveImage2KHR),

          // extension: VK_EXT_vertex_input_dynamic_state
          PTR_NAME_PAIR(vkCmdSetVertexInputEXT),
        };

        auto ret = std::span(arr);
        ret;
    });

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
    static const auto functions_and_names = ({
        static const std::pair<void (*vulkan_functions::*)(), const char*> arr[]{
          // vulkan version 1.0
          PTR_NAME_PAIR(vkGetDeviceProcAddr),
          PTR_NAME_PAIR(vkDestroyDevice),
          PTR_NAME_PAIR(vkGetDeviceQueue),
          PTR_NAME_PAIR(vkQueueSubmit),
          PTR_NAME_PAIR(vkQueueWaitIdle),
          PTR_NAME_PAIR(vkDeviceWaitIdle),
          PTR_NAME_PAIR(vkAllocateMemory),
          PTR_NAME_PAIR(vkMapMemory),
          PTR_NAME_PAIR(vkUnmapMemory),
          PTR_NAME_PAIR(vkFlushMappedMemoryRanges),
          PTR_NAME_PAIR(vkInvalidateMappedMemoryRanges),
          PTR_NAME_PAIR(vkGetDeviceMemoryCommitment),
          PTR_NAME_PAIR(vkBindBufferMemory),
          PTR_NAME_PAIR(vkBindImageMemory),
          PTR_NAME_PAIR(vkGetBufferMemoryRequirements),
          PTR_NAME_PAIR(vkGetImageMemoryRequirements),
          PTR_NAME_PAIR(vkCreateFence),
          PTR_NAME_PAIR(vkDestroyFence),
          PTR_NAME_PAIR(vkResetFences),
          PTR_NAME_PAIR(vkGetFenceStatus),
          PTR_NAME_PAIR(vkWaitForFences),
          PTR_NAME_PAIR(vkCreateSemaphore),
          PTR_NAME_PAIR(vkDestroySemaphore),
          PTR_NAME_PAIR(vkCreateEvent),
          PTR_NAME_PAIR(vkDestroyEvent),
          PTR_NAME_PAIR(vkGetEventStatus),
          PTR_NAME_PAIR(vkSetEvent),
          PTR_NAME_PAIR(vkResetEvent),
          PTR_NAME_PAIR(vkCreateQueryPool),
          PTR_NAME_PAIR(vkGetQueryPoolResults),
          PTR_NAME_PAIR(vkCreateBuffer),
          PTR_NAME_PAIR(vkDestroyBuffer),
          PTR_NAME_PAIR(vkCreateBufferView),
          PTR_NAME_PAIR(vkDestroyBufferView),
          PTR_NAME_PAIR(vkCreateImage),
          PTR_NAME_PAIR(vkDestroyImage),
          PTR_NAME_PAIR(vkGetImageSubresourceLayout),
          PTR_NAME_PAIR(vkCreateImageView),
          PTR_NAME_PAIR(vkDestroyImageView),
          PTR_NAME_PAIR(vkCreatePipelineCache),
          PTR_NAME_PAIR(vkDestroyPipelineCache),
          PTR_NAME_PAIR(vkCreateGraphicsPipelines),
          PTR_NAME_PAIR(vkCreateComputePipelines),
          PTR_NAME_PAIR(vkDestroyPipeline),
          PTR_NAME_PAIR(vkCreatePipelineLayout),
          PTR_NAME_PAIR(vkDestroyPipelineLayout),
          PTR_NAME_PAIR(vkCreateSampler),
          PTR_NAME_PAIR(vkDestroySampler),
          PTR_NAME_PAIR(vkCreateDescriptorSetLayout),
          PTR_NAME_PAIR(vkDestroyDescriptorSetLayout),
          PTR_NAME_PAIR(vkCreateDescriptorPool),
          PTR_NAME_PAIR(vkResetDescriptorPool),
          PTR_NAME_PAIR(vkAllocateDescriptorSets),
          PTR_NAME_PAIR(vkFreeDescriptorSets),
          PTR_NAME_PAIR(vkUpdateDescriptorSets),
          PTR_NAME_PAIR(vkCreateFramebuffer),
          PTR_NAME_PAIR(vkDestroyFramebuffer),
          PTR_NAME_PAIR(vkCreateRenderPass),
          PTR_NAME_PAIR(vkDestroyRenderPass),
          PTR_NAME_PAIR(vkGetRenderAreaGranularity),
          PTR_NAME_PAIR(vkCreateCommandPool),
          PTR_NAME_PAIR(vkResetCommandPool),
          PTR_NAME_PAIR(vkAllocateCommandBuffers),
          PTR_NAME_PAIR(vkFreeCommandBuffers),
          PTR_NAME_PAIR(vkBeginCommandBuffer),
          PTR_NAME_PAIR(vkEndCommandBuffer),
          PTR_NAME_PAIR(vkResetCommandBuffer),
          PTR_NAME_PAIR(vkCmdBindPipeline),
          PTR_NAME_PAIR(vkCmdSetViewport),
          PTR_NAME_PAIR(vkCmdSetScissor),
          PTR_NAME_PAIR(vkCmdSetLineWidth),
          PTR_NAME_PAIR(vkCmdSetDepthBias),
          PTR_NAME_PAIR(vkCmdSetBlendConstants),
          PTR_NAME_PAIR(vkCmdSetDepthBounds),
          PTR_NAME_PAIR(vkCmdSetStencilCompareMask),
          PTR_NAME_PAIR(vkCmdSetStencilWriteMask),
          PTR_NAME_PAIR(vkCmdSetStencilReference),
          PTR_NAME_PAIR(vkCmdBindDescriptorSets),
          PTR_NAME_PAIR(vkCmdBindIndexBuffer),
          PTR_NAME_PAIR(vkCmdBindVertexBuffers),
          PTR_NAME_PAIR(vkCmdDraw),
          PTR_NAME_PAIR(vkCmdDrawIndexed),
          PTR_NAME_PAIR(vkCmdDrawIndirect),
          PTR_NAME_PAIR(vkCmdDrawIndexedIndirect),
          PTR_NAME_PAIR(vkCmdDispatch),
          PTR_NAME_PAIR(vkCmdDispatchIndirect),
          PTR_NAME_PAIR(vkCmdCopyBuffer),
          PTR_NAME_PAIR(vkCmdCopyImage),
          PTR_NAME_PAIR(vkCmdBlitImage),
          PTR_NAME_PAIR(vkCmdCopyBufferToImage),
          PTR_NAME_PAIR(vkCmdCopyImageToBuffer),
          PTR_NAME_PAIR(vkCmdUpdateBuffer),
          PTR_NAME_PAIR(vkCmdFillBuffer),
          PTR_NAME_PAIR(vkCmdClearColorImage),
          PTR_NAME_PAIR(vkCmdClearDepthStencilImage),
          PTR_NAME_PAIR(vkCmdClearAttachments),
          PTR_NAME_PAIR(vkCmdResolveImage),
          PTR_NAME_PAIR(vkCmdSetEvent),
          PTR_NAME_PAIR(vkCmdResetEvent),
          PTR_NAME_PAIR(vkCmdWaitEvents),
          PTR_NAME_PAIR(vkCmdPipelineBarrier),
          PTR_NAME_PAIR(vkCmdBeginQuery),
          PTR_NAME_PAIR(vkCmdEndQuery),
          PTR_NAME_PAIR(vkCmdResetQueryPool),
          PTR_NAME_PAIR(vkCmdWriteTimestamp),
          PTR_NAME_PAIR(vkCmdCopyQueryPoolResults),
          PTR_NAME_PAIR(vkCmdPushConstants),
          PTR_NAME_PAIR(vkCmdBeginRenderPass),
          PTR_NAME_PAIR(vkCmdNextSubpass),
          PTR_NAME_PAIR(vkCmdEndRenderPass),
          PTR_NAME_PAIR(vkCmdExecuteCommands),

          // vulkan version 1.1
          PTR_NAME_PAIR(vkBindBufferMemory2),
          PTR_NAME_PAIR(vkBindImageMemory2),
          PTR_NAME_PAIR(vkGetDeviceGroupPeerMemoryFeatures),
          PTR_NAME_PAIR(vkCmdSetDeviceMask),
          PTR_NAME_PAIR(vkCmdDispatchBase),
          PTR_NAME_PAIR(vkGetImageMemoryRequirements2),
          PTR_NAME_PAIR(vkGetBufferMemoryRequirements2),
          PTR_NAME_PAIR(vkGetDeviceQueue2),
          PTR_NAME_PAIR(vkCreateSamplerYcbcrConversion),
          PTR_NAME_PAIR(vkDestroySamplerYcbcrConversion),
          PTR_NAME_PAIR(vkGetDescriptorSetLayoutSupport),

          // vulkan version 1.2
          PTR_NAME_PAIR(vkCmdDrawIndirectCount),
          PTR_NAME_PAIR(vkCmdDrawIndexedIndirectCount),
          PTR_NAME_PAIR(vkCreateRenderPass2),
          PTR_NAME_PAIR(vkCmdBeginRenderPass2),
          PTR_NAME_PAIR(vkCmdNextSubpass2),
          PTR_NAME_PAIR(vkCmdEndRenderPass2),
          PTR_NAME_PAIR(vkResetQueryPool),
          PTR_NAME_PAIR(vkGetSemaphoreCounterValue),
          PTR_NAME_PAIR(vkWaitSemaphores),
          PTR_NAME_PAIR(vkSignalSemaphore),
          PTR_NAME_PAIR(vkGetBufferDeviceAddress),
          PTR_NAME_PAIR(vkGetBufferOpaqueCaptureAddress),
          PTR_NAME_PAIR(vkGetDeviceMemoryOpaqueCaptureAddress),

          // vulkan version 1.3
          PTR_NAME_PAIR(vkCreatePrivateDataSlot),
          PTR_NAME_PAIR(vkDestroyPrivateDataSlot),
          PTR_NAME_PAIR(vkSetPrivateData),
          PTR_NAME_PAIR(vkGetPrivateData),
          PTR_NAME_PAIR(vkCmdSetEvent2),
          PTR_NAME_PAIR(vkCmdResetEvent2),
          PTR_NAME_PAIR(vkCmdWaitEvents2),
          PTR_NAME_PAIR(vkCmdPipelineBarrier2),
          PTR_NAME_PAIR(vkCmdWriteTimestamp2),
          PTR_NAME_PAIR(vkQueueSubmit2),
          PTR_NAME_PAIR(vkCmdCopyBuffer2),
          PTR_NAME_PAIR(vkCmdCopyImage2),
          PTR_NAME_PAIR(vkCmdCopyBufferToImage2),
          PTR_NAME_PAIR(vkCmdCopyImageToBuffer2),
          PTR_NAME_PAIR(vkCmdBlitImage2),
          PTR_NAME_PAIR(vkCmdResolveImage2),
          PTR_NAME_PAIR(vkCmdBeginRendering),
          PTR_NAME_PAIR(vkCmdEndRendering),
          PTR_NAME_PAIR(vkCmdSetCullMode),
          PTR_NAME_PAIR(vkCmdSetFrontFace),
          PTR_NAME_PAIR(vkCmdSetPrimitiveTopology),
          PTR_NAME_PAIR(vkCmdSetViewportWithCount),
          PTR_NAME_PAIR(vkCmdSetScissorWithCount),
          PTR_NAME_PAIR(vkCmdBindVertexBuffers2),
          PTR_NAME_PAIR(vkCmdSetDepthTestEnable),
          PTR_NAME_PAIR(vkCmdSetDepthWriteEnable),
          PTR_NAME_PAIR(vkCmdSetDepthCompareOp),
          PTR_NAME_PAIR(vkCmdSetDepthBoundsTestEnable),
          PTR_NAME_PAIR(vkCmdSetStencilTestEnable),
          PTR_NAME_PAIR(vkCmdSetStencilOp),
          PTR_NAME_PAIR(vkCmdSetRasterizerDiscardEnable),
          PTR_NAME_PAIR(vkCmdSetDepthBiasEnable),
          PTR_NAME_PAIR(vkCmdSetPrimitiveRestartEnable),
          PTR_NAME_PAIR(vkGetDeviceBufferMemoryRequirements),
          PTR_NAME_PAIR(vkGetDeviceImageMemoryRequirements),
          PTR_NAME_PAIR(vkGetDeviceImageSparseMemoryRequirements),

          // extension: VK_KHR_swapchain
          PTR_NAME_PAIR(vkCreateSwapchainKHR),
          PTR_NAME_PAIR(vkGetSwapchainImagesKHR),
          PTR_NAME_PAIR(vkAcquireNextImageKHR),
          PTR_NAME_PAIR(vkQueuePresentKHR),
          PTR_NAME_PAIR(vkGetDeviceGroupPresentCapabilitiesKHR),
          PTR_NAME_PAIR(vkGetDeviceGroupSurfacePresentModesKHR),
          PTR_NAME_PAIR(vkAcquireNextImage2KHR),

          // extension: VK_KHR_display_swapchain
          PTR_NAME_PAIR(vkCreateSharedSwapchainsKHR),

          // extension: VK_KHR_external_memory_fd
          PTR_NAME_PAIR(vkGetMemoryFdKHR),
          PTR_NAME_PAIR(vkGetMemoryFdPropertiesKHR),

          // extension: VK_KHR_external_semaphore_fd
          PTR_NAME_PAIR(vkImportSemaphoreFdKHR),
          PTR_NAME_PAIR(vkGetSemaphoreFdKHR),

          // extension: VK_EXT_display_control
          PTR_NAME_PAIR(vkDisplayPowerControlEXT),
          PTR_NAME_PAIR(vkRegisterDeviceEventEXT),
          PTR_NAME_PAIR(vkRegisterDisplayEventEXT),
          PTR_NAME_PAIR(vkGetSwapchainCounterEXT),

          // extension: VK_EXT_discard_rectangles
          PTR_NAME_PAIR(vkCmdSetDiscardRectangleEXT),
          PTR_NAME_PAIR(vkCmdSetDiscardRectangleEnableEXT),
          PTR_NAME_PAIR(vkCmdSetDiscardRectangleModeEXT),

          // extension: VK_EXT_hdr_metadata
          PTR_NAME_PAIR(vkSetHdrMetadataEXT),

          // extension: VK_KHR_shared_presentable_image
          PTR_NAME_PAIR(vkGetSwapchainStatusKHR),

          // extension: VK_KHR_external_fence_fd
          PTR_NAME_PAIR(vkImportFenceFdKHR),
          PTR_NAME_PAIR(vkGetFenceFdKHR),

          // extension: VK_KHR_performance_query
          PTR_NAME_PAIR(vkAcquireProfilingLockKHR),
          PTR_NAME_PAIR(vkReleaseProfilingLockKHR),

          // extension: VK_EXT_debug_utils
          PTR_NAME_PAIR(vkSetDebugUtilsObjectNameEXT),
          PTR_NAME_PAIR(vkSetDebugUtilsObjectTagEXT),
          PTR_NAME_PAIR(vkQueueBeginDebugUtilsLabelEXT),
          PTR_NAME_PAIR(vkQueueEndDebugUtilsLabelEXT),
          PTR_NAME_PAIR(vkQueueInsertDebugUtilsLabelEXT),
          PTR_NAME_PAIR(vkCmdBeginDebugUtilsLabelEXT),
          PTR_NAME_PAIR(vkCmdEndDebugUtilsLabelEXT),
          PTR_NAME_PAIR(vkCmdInsertDebugUtilsLabelEXT),

          // extension: VK_EXT_sample_locations
          PTR_NAME_PAIR(vkCmdSetSampleLocationsEXT),

          // extension: VK_EXT_image_drm_format_modifier
          PTR_NAME_PAIR(vkGetImageDrmFormatModifierPropertiesEXT),

          // extension: VK_EXT_external_memory_host
          PTR_NAME_PAIR(vkGetMemoryHostPointerPropertiesEXT),

          // extension: VK_EXT_calibrated_timestamps
          PTR_NAME_PAIR(vkGetCalibratedTimestampsEXT),

          // extension: VK_KHR_fragment_shading_rate
          PTR_NAME_PAIR(vkCmdSetFragmentShadingRateKHR),

          // extension: VK_EXT_line_rasterization
          PTR_NAME_PAIR(vkCmdSetLineStippleEXT),

          // extension: VK_EXT_extended_dynamic_state
          PTR_NAME_PAIR(vkCmdSetCullModeEXT),
          PTR_NAME_PAIR(vkCmdSetFrontFaceEXT),
          PTR_NAME_PAIR(vkCmdSetPrimitiveTopologyEXT),
          PTR_NAME_PAIR(vkCmdSetViewportWithCountEXT),
          PTR_NAME_PAIR(vkCmdSetScissorWithCountEXT),
          PTR_NAME_PAIR(vkCmdBindVertexBuffers2EXT),
          PTR_NAME_PAIR(vkCmdSetDepthTestEnableEXT),
          PTR_NAME_PAIR(vkCmdSetDepthWriteEnableEXT),
          PTR_NAME_PAIR(vkCmdSetDepthCompareOpEXT),
          PTR_NAME_PAIR(vkCmdSetDepthBoundsTestEnableEXT),
          PTR_NAME_PAIR(vkCmdSetStencilTestEnableEXT),
          PTR_NAME_PAIR(vkCmdSetStencilOpEXT),

          // extension: VK_KHR_synchronization2
          PTR_NAME_PAIR(vkCmdSetEvent2KHR),
          PTR_NAME_PAIR(vkCmdResetEvent2KHR),
          PTR_NAME_PAIR(vkCmdWaitEvents2KHR),
          PTR_NAME_PAIR(vkCmdPipelineBarrier2KHR),
          PTR_NAME_PAIR(vkCmdWriteTimestamp2KHR),
          PTR_NAME_PAIR(vkQueueSubmit2KHR),
          PTR_NAME_PAIR(vkCmdWriteBufferMarker2AMD),
          PTR_NAME_PAIR(vkGetQueueCheckpointData2NV),

          // extension: VK_KHR_copy_commands2
          PTR_NAME_PAIR(vkCmdCopyBuffer2KHR),
          PTR_NAME_PAIR(vkCmdCopyImage2KHR),
          PTR_NAME_PAIR(vkCmdCopyBufferToImage2KHR),
          PTR_NAME_PAIR(vkCmdCopyImageToBuffer2KHR),
          PTR_NAME_PAIR(vkCmdBlitImage2KHR),
          PTR_NAME_PAIR(vkCmdResolveImage2KHR),

          // extension: VK_EXT_vertex_input_dynamic_state
          PTR_NAME_PAIR(vkCmdSetVertexInputEXT),

          // extension: VK_EXT_extended_dynamic_state2
          PTR_NAME_PAIR(vkCmdSetPatchControlPointsEXT),
          PTR_NAME_PAIR(vkCmdSetRasterizerDiscardEnableEXT),
          PTR_NAME_PAIR(vkCmdSetDepthBiasEnableEXT),
          PTR_NAME_PAIR(vkCmdSetLogicOpEXT),
          PTR_NAME_PAIR(vkCmdSetPrimitiveRestartEnableEXT),

          // extension: VK_EXT_color_write_enable
          PTR_NAME_PAIR(vkCmdSetColorWriteEnableEXT),
        };

        auto ret = std::span(arr);
        ret;
    });

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

#pragma pop_macro("PTR_NAME_PAIR")

}
