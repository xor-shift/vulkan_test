#pragma once

#include <vxl/dyna_loader.hpp>

#include <stuff/intro/introspectors/function.hpp>

#include <vulkan/vulkan_core.h>

namespace vxl::vk {

struct vulkan_functions {
    vulkan_functions() = default;

    void init(dynamic_loader const& dyna_loader) { init(dyna_loader.get_dl_symbol<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr")); }

    void init(PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr);

    void init(VkInstance instance);

    void init(VkDevice device);

    template<auto... SuccessValues>
    static constexpr auto success(auto value) -> bool {
        return (((value & SuccessValues) != 0) && ...);
    }

    /// function types:
    /// 0: no wrapper generation
    /// 1: void(Args...)
    /// 2: functions returning a value through an out parameter at some index
    /// 3:

#define TYPE_2_PRELUDE(_name, _n) \
    using introspector_t = stf::intro::function_introspector<decltype(_name)>; \
    using return_type = std::remove_pointer_t<introspector_t::nth_argument<_n>>; \
    auto ret = return_type{}

    auto create_instance() {
        return [this](const VkInstanceCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) { return create_instance(create_info, allocator); };
    }
    auto create_instance(const VkInstanceCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) -> std::expected<VkInstance, VkResult> {
        TYPE_2_PRELUDE(vkCreateInstance, 2);
        const auto res = vkCreateInstance(create_info, allocator, &ret);
        return success<VK_SUCCESS>(res) ? std::expected<return_type, VkResult>(ret) : std::unexpected{res};
    }

    auto destroy_instance() { return [this](VkInstance instance, const VkAllocationCallbacks* allocator = nullptr) { return destroy_instance(instance, allocator); }; }
    void destroy_instance(VkInstance instance, const VkAllocationCallbacks* allocator = nullptr) { return vkDestroyInstance(instance, allocator); }

    auto enumerate_instance_layer_properties() { return [this](VkLayerProperties* out_layer_properties) { return enumerate_instance_layer_properties(out_layer_properties); }; }
    auto enumerate_instance_layer_properties(VkLayerProperties* out_layer_properties) -> std::expected<std::pair<u32, VkResult>, VkResult> {
        TYPE_2_PRELUDE(vkEnumerateInstanceLayerProperties, 0);
        const auto res = vkEnumerateInstanceLayerProperties(&ret, out_layer_properties);
        return success<VK_SUCCESS, VK_INCOMPLETE>(res) ? std::expected<std::pair<return_type, VkResult>, VkResult>({ret, res}) : std::unexpected{res};
    }

    // vulkan 1.0 functions

    PFN_vkCreateInstance vkCreateInstance = nullptr;
    PFN_vkDestroyInstance vkDestroyInstance = nullptr;
    PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices = nullptr;
    PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures = nullptr;
    PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties = nullptr;
    PFN_vkGetPhysicalDeviceImageFormatProperties vkGetPhysicalDeviceImageFormatProperties = nullptr;
    PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties = nullptr;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
    PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties = nullptr;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = nullptr;
    PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr = nullptr;
    PFN_vkCreateDevice vkCreateDevice = nullptr;
    PFN_vkDestroyDevice vkDestroyDevice = nullptr;
    PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties = nullptr;
    PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties = nullptr;
    PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties = nullptr;
    PFN_vkEnumerateDeviceLayerProperties vkEnumerateDeviceLayerProperties = nullptr;
    PFN_vkGetDeviceQueue vkGetDeviceQueue = nullptr;
    PFN_vkQueueSubmit vkQueueSubmit = nullptr;
    PFN_vkQueueWaitIdle vkQueueWaitIdle = nullptr;
    PFN_vkDeviceWaitIdle vkDeviceWaitIdle = nullptr;
    PFN_vkAllocateMemory vkAllocateMemory = nullptr;
    PFN_vkMapMemory vkMapMemory = nullptr;
    PFN_vkUnmapMemory vkUnmapMemory = nullptr;
    PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges = nullptr;
    PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges = nullptr;
    PFN_vkGetDeviceMemoryCommitment vkGetDeviceMemoryCommitment = nullptr;
    PFN_vkBindBufferMemory vkBindBufferMemory = nullptr;
    PFN_vkBindImageMemory vkBindImageMemory = nullptr;
    PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements = nullptr;
    PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements = nullptr;
    PFN_vkCreateFence vkCreateFence = nullptr;
    PFN_vkDestroyFence vkDestroyFence = nullptr;
    PFN_vkResetFences vkResetFences = nullptr;
    PFN_vkGetFenceStatus vkGetFenceStatus = nullptr;
    PFN_vkWaitForFences vkWaitForFences = nullptr;
    PFN_vkCreateSemaphore vkCreateSemaphore = nullptr;
    PFN_vkDestroySemaphore vkDestroySemaphore = nullptr;
    PFN_vkCreateEvent vkCreateEvent = nullptr;
    PFN_vkDestroyEvent vkDestroyEvent = nullptr;
    PFN_vkGetEventStatus vkGetEventStatus = nullptr;
    PFN_vkSetEvent vkSetEvent = nullptr;
    PFN_vkResetEvent vkResetEvent = nullptr;
    PFN_vkCreateQueryPool vkCreateQueryPool = nullptr;
    PFN_vkGetQueryPoolResults vkGetQueryPoolResults = nullptr;
    PFN_vkCreateBuffer vkCreateBuffer = nullptr;
    PFN_vkDestroyBuffer vkDestroyBuffer = nullptr;
    PFN_vkCreateBufferView vkCreateBufferView = nullptr;
    PFN_vkDestroyBufferView vkDestroyBufferView = nullptr;
    PFN_vkCreateImage vkCreateImage = nullptr;
    PFN_vkDestroyImage vkDestroyImage = nullptr;
    PFN_vkGetImageSubresourceLayout vkGetImageSubresourceLayout = nullptr;
    PFN_vkCreateImageView vkCreateImageView = nullptr;
    PFN_vkDestroyImageView vkDestroyImageView = nullptr;
    PFN_vkCreatePipelineCache vkCreatePipelineCache = nullptr;
    PFN_vkDestroyPipelineCache vkDestroyPipelineCache = nullptr;
    PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines = nullptr;
    PFN_vkCreateComputePipelines vkCreateComputePipelines = nullptr;
    PFN_vkDestroyPipeline vkDestroyPipeline = nullptr;
    PFN_vkCreatePipelineLayout vkCreatePipelineLayout = nullptr;
    PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout = nullptr;
    PFN_vkCreateSampler vkCreateSampler = nullptr;
    PFN_vkDestroySampler vkDestroySampler = nullptr;
    PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout = nullptr;
    PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout = nullptr;
    PFN_vkCreateDescriptorPool vkCreateDescriptorPool = nullptr;
    PFN_vkResetDescriptorPool vkResetDescriptorPool = nullptr;
    PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets = nullptr;
    PFN_vkFreeDescriptorSets vkFreeDescriptorSets = nullptr;
    PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets = nullptr;
    PFN_vkCreateFramebuffer vkCreateFramebuffer = nullptr;
    PFN_vkDestroyFramebuffer vkDestroyFramebuffer = nullptr;
    PFN_vkCreateRenderPass vkCreateRenderPass = nullptr;
    PFN_vkDestroyRenderPass vkDestroyRenderPass = nullptr;
    PFN_vkGetRenderAreaGranularity vkGetRenderAreaGranularity = nullptr;
    PFN_vkCreateCommandPool vkCreateCommandPool = nullptr;
    PFN_vkResetCommandPool vkResetCommandPool = nullptr;
    PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers = nullptr;
    PFN_vkFreeCommandBuffers vkFreeCommandBuffers = nullptr;
    PFN_vkBeginCommandBuffer vkBeginCommandBuffer = nullptr;
    PFN_vkEndCommandBuffer vkEndCommandBuffer = nullptr;
    PFN_vkResetCommandBuffer vkResetCommandBuffer = nullptr;
    PFN_vkCmdBindPipeline vkCmdBindPipeline = nullptr;
    PFN_vkCmdSetViewport vkCmdSetViewport = nullptr;
    PFN_vkCmdSetScissor vkCmdSetScissor = nullptr;
    PFN_vkCmdSetLineWidth vkCmdSetLineWidth = nullptr;
    PFN_vkCmdSetDepthBias vkCmdSetDepthBias = nullptr;
    PFN_vkCmdSetBlendConstants vkCmdSetBlendConstants = nullptr;
    PFN_vkCmdSetDepthBounds vkCmdSetDepthBounds = nullptr;
    PFN_vkCmdSetStencilCompareMask vkCmdSetStencilCompareMask = nullptr;
    PFN_vkCmdSetStencilWriteMask vkCmdSetStencilWriteMask = nullptr;
    PFN_vkCmdSetStencilReference vkCmdSetStencilReference = nullptr;
    PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets = nullptr;
    PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer = nullptr;
    PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers = nullptr;
    PFN_vkCmdDraw vkCmdDraw = nullptr;
    PFN_vkCmdDrawIndexed vkCmdDrawIndexed = nullptr;
    PFN_vkCmdDrawIndirect vkCmdDrawIndirect = nullptr;
    PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect = nullptr;
    PFN_vkCmdDispatch vkCmdDispatch = nullptr;
    PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect = nullptr;
    PFN_vkCmdCopyBuffer vkCmdCopyBuffer = nullptr;
    PFN_vkCmdCopyImage vkCmdCopyImage = nullptr;
    PFN_vkCmdBlitImage vkCmdBlitImage = nullptr;
    PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage = nullptr;
    PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer = nullptr;
    PFN_vkCmdUpdateBuffer vkCmdUpdateBuffer = nullptr;
    PFN_vkCmdFillBuffer vkCmdFillBuffer = nullptr;
    PFN_vkCmdClearColorImage vkCmdClearColorImage = nullptr;
    PFN_vkCmdClearDepthStencilImage vkCmdClearDepthStencilImage = nullptr;
    PFN_vkCmdClearAttachments vkCmdClearAttachments = nullptr;
    PFN_vkCmdResolveImage vkCmdResolveImage = nullptr;
    PFN_vkCmdSetEvent vkCmdSetEvent = nullptr;
    PFN_vkCmdResetEvent vkCmdResetEvent = nullptr;
    PFN_vkCmdWaitEvents vkCmdWaitEvents = nullptr;
    PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier = nullptr;
    PFN_vkCmdBeginQuery vkCmdBeginQuery = nullptr;
    PFN_vkCmdEndQuery vkCmdEndQuery = nullptr;
    PFN_vkCmdResetQueryPool vkCmdResetQueryPool = nullptr;
    PFN_vkCmdWriteTimestamp vkCmdWriteTimestamp = nullptr;
    PFN_vkCmdCopyQueryPoolResults vkCmdCopyQueryPoolResults = nullptr;
    PFN_vkCmdPushConstants vkCmdPushConstants = nullptr;
    PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass = nullptr;
    PFN_vkCmdNextSubpass vkCmdNextSubpass = nullptr;
    PFN_vkCmdEndRenderPass vkCmdEndRenderPass = nullptr;
    PFN_vkCmdExecuteCommands vkCmdExecuteCommands = nullptr;

    // vulkan 1.1 functions

    PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion = nullptr;
    PFN_vkBindBufferMemory2 vkBindBufferMemory2 = nullptr;
    PFN_vkBindImageMemory2 vkBindImageMemory2 = nullptr;
    PFN_vkGetDeviceGroupPeerMemoryFeatures vkGetDeviceGroupPeerMemoryFeatures = nullptr;
    PFN_vkCmdSetDeviceMask vkCmdSetDeviceMask = nullptr;
    PFN_vkCmdDispatchBase vkCmdDispatchBase = nullptr;
    PFN_vkEnumeratePhysicalDeviceGroups vkEnumeratePhysicalDeviceGroups = nullptr;
    PFN_vkGetImageMemoryRequirements2 vkGetImageMemoryRequirements2 = nullptr;
    PFN_vkGetBufferMemoryRequirements2 vkGetBufferMemoryRequirements2 = nullptr;
    PFN_vkGetPhysicalDeviceFeatures2 vkGetPhysicalDeviceFeatures2 = nullptr;
    PFN_vkGetPhysicalDeviceProperties2 vkGetPhysicalDeviceProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceFormatProperties2 vkGetPhysicalDeviceFormatProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceImageFormatProperties2 vkGetPhysicalDeviceImageFormatProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties2 vkGetPhysicalDeviceQueueFamilyProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceMemoryProperties2 vkGetPhysicalDeviceMemoryProperties2 = nullptr;
    PFN_vkGetDeviceQueue2 vkGetDeviceQueue2 = nullptr;
    PFN_vkCreateSamplerYcbcrConversion vkCreateSamplerYcbcrConversion = nullptr;
    PFN_vkDestroySamplerYcbcrConversion vkDestroySamplerYcbcrConversion = nullptr;
    PFN_vkGetPhysicalDeviceExternalBufferProperties vkGetPhysicalDeviceExternalBufferProperties = nullptr;
    PFN_vkGetPhysicalDeviceExternalFenceProperties vkGetPhysicalDeviceExternalFenceProperties = nullptr;
    PFN_vkGetPhysicalDeviceExternalSemaphoreProperties vkGetPhysicalDeviceExternalSemaphoreProperties = nullptr;
    PFN_vkGetDescriptorSetLayoutSupport vkGetDescriptorSetLayoutSupport = nullptr;

    // vulkan 1.2 functions

    PFN_vkCmdDrawIndirectCount vkCmdDrawIndirectCount = nullptr;
    PFN_vkCmdDrawIndexedIndirectCount vkCmdDrawIndexedIndirectCount = nullptr;
    PFN_vkCreateRenderPass2 vkCreateRenderPass2 = nullptr;
    PFN_vkCmdBeginRenderPass2 vkCmdBeginRenderPass2 = nullptr;
    PFN_vkCmdNextSubpass2 vkCmdNextSubpass2 = nullptr;
    PFN_vkCmdEndRenderPass2 vkCmdEndRenderPass2 = nullptr;
    PFN_vkResetQueryPool vkResetQueryPool = nullptr;
    PFN_vkGetSemaphoreCounterValue vkGetSemaphoreCounterValue = nullptr;
    PFN_vkWaitSemaphores vkWaitSemaphores = nullptr;
    PFN_vkSignalSemaphore vkSignalSemaphore = nullptr;
    PFN_vkGetBufferDeviceAddress vkGetBufferDeviceAddress = nullptr;
    PFN_vkGetBufferOpaqueCaptureAddress vkGetBufferOpaqueCaptureAddress = nullptr;
    PFN_vkGetDeviceMemoryOpaqueCaptureAddress vkGetDeviceMemoryOpaqueCaptureAddress = nullptr;

    // vulkan 1.3 functions

    PFN_vkGetPhysicalDeviceToolProperties vkGetPhysicalDeviceToolProperties = nullptr;
    PFN_vkCreatePrivateDataSlot vkCreatePrivateDataSlot = nullptr;
    PFN_vkDestroyPrivateDataSlot vkDestroyPrivateDataSlot = nullptr;
    PFN_vkSetPrivateData vkSetPrivateData = nullptr;
    PFN_vkGetPrivateData vkGetPrivateData = nullptr;
    PFN_vkCmdSetEvent2 vkCmdSetEvent2 = nullptr;
    PFN_vkCmdResetEvent2 vkCmdResetEvent2 = nullptr;
    PFN_vkCmdWaitEvents2 vkCmdWaitEvents2 = nullptr;
    PFN_vkCmdPipelineBarrier2 vkCmdPipelineBarrier2 = nullptr;
    PFN_vkCmdWriteTimestamp2 vkCmdWriteTimestamp2 = nullptr;
    PFN_vkQueueSubmit2 vkQueueSubmit2 = nullptr;
    PFN_vkCmdCopyBuffer2 vkCmdCopyBuffer2 = nullptr;
    PFN_vkCmdCopyImage2 vkCmdCopyImage2 = nullptr;
    PFN_vkCmdCopyBufferToImage2 vkCmdCopyBufferToImage2 = nullptr;
    PFN_vkCmdCopyImageToBuffer2 vkCmdCopyImageToBuffer2 = nullptr;
    PFN_vkCmdBlitImage2 vkCmdBlitImage2 = nullptr;
    PFN_vkCmdResolveImage2 vkCmdResolveImage2 = nullptr;
    PFN_vkCmdBeginRendering vkCmdBeginRendering = nullptr;
    PFN_vkCmdEndRendering vkCmdEndRendering = nullptr;
    PFN_vkCmdSetCullMode vkCmdSetCullMode = nullptr;
    PFN_vkCmdSetFrontFace vkCmdSetFrontFace = nullptr;
    PFN_vkCmdSetPrimitiveTopology vkCmdSetPrimitiveTopology = nullptr;
    PFN_vkCmdSetViewportWithCount vkCmdSetViewportWithCount = nullptr;
    PFN_vkCmdSetScissorWithCount vkCmdSetScissorWithCount = nullptr;
    PFN_vkCmdBindVertexBuffers2 vkCmdBindVertexBuffers2 = nullptr;
    PFN_vkCmdSetDepthTestEnable vkCmdSetDepthTestEnable = nullptr;
    PFN_vkCmdSetDepthWriteEnable vkCmdSetDepthWriteEnable = nullptr;
    PFN_vkCmdSetDepthCompareOp vkCmdSetDepthCompareOp = nullptr;
    PFN_vkCmdSetDepthBoundsTestEnable vkCmdSetDepthBoundsTestEnable = nullptr;
    PFN_vkCmdSetStencilTestEnable vkCmdSetStencilTestEnable = nullptr;
    PFN_vkCmdSetStencilOp vkCmdSetStencilOp = nullptr;
    PFN_vkCmdSetRasterizerDiscardEnable vkCmdSetRasterizerDiscardEnable = nullptr;
    PFN_vkCmdSetDepthBiasEnable vkCmdSetDepthBiasEnable = nullptr;
    PFN_vkCmdSetPrimitiveRestartEnable vkCmdSetPrimitiveRestartEnable = nullptr;
    PFN_vkGetDeviceBufferMemoryRequirements vkGetDeviceBufferMemoryRequirements = nullptr;
    PFN_vkGetDeviceImageMemoryRequirements vkGetDeviceImageMemoryRequirements = nullptr;
    PFN_vkGetDeviceImageSparseMemoryRequirements vkGetDeviceImageSparseMemoryRequirements = nullptr;

    // extension: VK_KHR_surface

    PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;

    // extension: VK_KHR_swapchain

    PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR = nullptr;
    PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR = nullptr;
    PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR = nullptr;
    PFN_vkQueuePresentKHR vkQueuePresentKHR = nullptr;
    PFN_vkGetDeviceGroupPresentCapabilitiesKHR vkGetDeviceGroupPresentCapabilitiesKHR = nullptr;
    PFN_vkGetDeviceGroupSurfacePresentModesKHR vkGetDeviceGroupSurfacePresentModesKHR = nullptr;
    PFN_vkGetPhysicalDevicePresentRectanglesKHR vkGetPhysicalDevicePresentRectanglesKHR = nullptr;
    PFN_vkAcquireNextImage2KHR vkAcquireNextImage2KHR = nullptr;

    // extension: VK_KHR_display

    PFN_vkGetPhysicalDeviceDisplayPropertiesKHR vkGetPhysicalDeviceDisplayPropertiesKHR = nullptr;
    PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR vkGetPhysicalDeviceDisplayPlanePropertiesKHR = nullptr;
    PFN_vkGetDisplayPlaneSupportedDisplaysKHR vkGetDisplayPlaneSupportedDisplaysKHR = nullptr;
    PFN_vkGetDisplayModePropertiesKHR vkGetDisplayModePropertiesKHR = nullptr;
    PFN_vkCreateDisplayModeKHR vkCreateDisplayModeKHR = nullptr;
    PFN_vkGetDisplayPlaneCapabilitiesKHR vkGetDisplayPlaneCapabilitiesKHR = nullptr;
    PFN_vkCreateDisplayPlaneSurfaceKHR vkCreateDisplayPlaneSurfaceKHR = nullptr;

    // extension: VK_KHR_display_swapchain

    PFN_vkCreateSharedSwapchainsKHR vkCreateSharedSwapchainsKHR = nullptr;

    // extension: VK_KHR_external_memory_fd

    PFN_vkGetMemoryFdKHR vkGetMemoryFdKHR = nullptr;
    PFN_vkGetMemoryFdPropertiesKHR vkGetMemoryFdPropertiesKHR = nullptr;

    // extension: VK_KHR_external_semaphore_fd

    PFN_vkImportSemaphoreFdKHR vkImportSemaphoreFdKHR = nullptr;
    PFN_vkGetSemaphoreFdKHR vkGetSemaphoreFdKHR = nullptr;

    // extension: VK_EXT_direct_mode_display

    PFN_vkReleaseDisplayEXT vkReleaseDisplayEXT = nullptr;

    // extension: VK_EXT_display_surface_counter

    PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT vkGetPhysicalDeviceSurfaceCapabilities2EXT = nullptr;

    // extension: VK_EXT_display_control

    PFN_vkDisplayPowerControlEXT vkDisplayPowerControlEXT = nullptr;
    PFN_vkRegisterDeviceEventEXT vkRegisterDeviceEventEXT = nullptr;
    PFN_vkRegisterDisplayEventEXT vkRegisterDisplayEventEXT = nullptr;
    PFN_vkGetSwapchainCounterEXT vkGetSwapchainCounterEXT = nullptr;

    // extension: VK_EXT_discard_rectangles

    PFN_vkCmdSetDiscardRectangleEXT vkCmdSetDiscardRectangleEXT = nullptr;
    PFN_vkCmdSetDiscardRectangleEnableEXT vkCmdSetDiscardRectangleEnableEXT = nullptr;
    PFN_vkCmdSetDiscardRectangleModeEXT vkCmdSetDiscardRectangleModeEXT = nullptr;

    // extension: VK_EXT_hdr_metadata

    PFN_vkSetHdrMetadataEXT vkSetHdrMetadataEXT = nullptr;

    // extension: VK_KHR_shared_presentable_image

    PFN_vkGetSwapchainStatusKHR vkGetSwapchainStatusKHR = nullptr;

    // extension: VK_KHR_external_fence_fd

    PFN_vkImportFenceFdKHR vkImportFenceFdKHR = nullptr;
    PFN_vkGetFenceFdKHR vkGetFenceFdKHR = nullptr;

    // extension: VK_KHR_performance_query

    PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR = nullptr;
    PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR = nullptr;
    PFN_vkAcquireProfilingLockKHR vkAcquireProfilingLockKHR = nullptr;
    PFN_vkReleaseProfilingLockKHR vkReleaseProfilingLockKHR = nullptr;

    // extension: VK_KHR_get_surface_capabilities2

    PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR vkGetPhysicalDeviceSurfaceCapabilities2KHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceFormats2KHR vkGetPhysicalDeviceSurfaceFormats2KHR = nullptr;

    // extension: VK_KHR_get_display_properties2

    PFN_vkGetPhysicalDeviceDisplayProperties2KHR vkGetPhysicalDeviceDisplayProperties2KHR = nullptr;
    PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR vkGetPhysicalDeviceDisplayPlaneProperties2KHR = nullptr;
    PFN_vkGetDisplayModeProperties2KHR vkGetDisplayModeProperties2KHR = nullptr;
    PFN_vkGetDisplayPlaneCapabilities2KHR vkGetDisplayPlaneCapabilities2KHR = nullptr;

    // extension: VK_EXT_debug_utils

    PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT = nullptr;
    PFN_vkSetDebugUtilsObjectTagEXT vkSetDebugUtilsObjectTagEXT = nullptr;
    PFN_vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabelEXT = nullptr;
    PFN_vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT = nullptr;
    PFN_vkQueueInsertDebugUtilsLabelEXT vkQueueInsertDebugUtilsLabelEXT = nullptr;
    PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT = nullptr;
    PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT = nullptr;
    PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT = nullptr;
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;
    PFN_vkSubmitDebugUtilsMessageEXT vkSubmitDebugUtilsMessageEXT = nullptr;

    // extension: VK_EXT_sample_locations

    PFN_vkCmdSetSampleLocationsEXT vkCmdSetSampleLocationsEXT = nullptr;
    PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT vkGetPhysicalDeviceMultisamplePropertiesEXT = nullptr;

    // extension: VK_EXT_image_drm_format_modifier

    PFN_vkGetImageDrmFormatModifierPropertiesEXT vkGetImageDrmFormatModifierPropertiesEXT = nullptr;

    // extension: VK_EXT_external_memory_host

    PFN_vkGetMemoryHostPointerPropertiesEXT vkGetMemoryHostPointerPropertiesEXT = nullptr;

    // extension: VK_EXT_calibrated_timestamps

    PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT vkGetPhysicalDeviceCalibrateableTimeDomainsEXT = nullptr;
    PFN_vkGetCalibratedTimestampsEXT vkGetCalibratedTimestampsEXT = nullptr;

    // extension: VK_KHR_fragment_shading_rate

    PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR vkGetPhysicalDeviceFragmentShadingRatesKHR = nullptr;
    PFN_vkCmdSetFragmentShadingRateKHR vkCmdSetFragmentShadingRateKHR = nullptr;

    // extension: VK_EXT_headless_surface

    PFN_vkCreateHeadlessSurfaceEXT vkCreateHeadlessSurfaceEXT = nullptr;

    // extension: VK_EXT_line_rasterization

    PFN_vkCmdSetLineStippleEXT vkCmdSetLineStippleEXT = nullptr;

    // extension: VK_EXT_extended_dynamic_state

    PFN_vkCmdSetCullModeEXT vkCmdSetCullModeEXT = nullptr;
    PFN_vkCmdSetFrontFaceEXT vkCmdSetFrontFaceEXT = nullptr;
    PFN_vkCmdSetPrimitiveTopologyEXT vkCmdSetPrimitiveTopologyEXT = nullptr;
    PFN_vkCmdSetViewportWithCountEXT vkCmdSetViewportWithCountEXT = nullptr;
    PFN_vkCmdSetScissorWithCountEXT vkCmdSetScissorWithCountEXT = nullptr;
    PFN_vkCmdBindVertexBuffers2EXT vkCmdBindVertexBuffers2EXT = nullptr;
    PFN_vkCmdSetDepthTestEnableEXT vkCmdSetDepthTestEnableEXT = nullptr;
    PFN_vkCmdSetDepthWriteEnableEXT vkCmdSetDepthWriteEnableEXT = nullptr;
    PFN_vkCmdSetDepthCompareOpEXT vkCmdSetDepthCompareOpEXT = nullptr;
    PFN_vkCmdSetDepthBoundsTestEnableEXT vkCmdSetDepthBoundsTestEnableEXT = nullptr;
    PFN_vkCmdSetStencilTestEnableEXT vkCmdSetStencilTestEnableEXT = nullptr;
    PFN_vkCmdSetStencilOpEXT vkCmdSetStencilOpEXT = nullptr;

    // extension: VK_KHR_synchronization2

    PFN_vkCmdSetEvent2KHR vkCmdSetEvent2KHR = nullptr;
    PFN_vkCmdResetEvent2KHR vkCmdResetEvent2KHR = nullptr;
    PFN_vkCmdWaitEvents2KHR vkCmdWaitEvents2KHR = nullptr;
    PFN_vkCmdPipelineBarrier2KHR vkCmdPipelineBarrier2KHR = nullptr;
    PFN_vkCmdWriteTimestamp2KHR vkCmdWriteTimestamp2KHR = nullptr;
    PFN_vkQueueSubmit2KHR vkQueueSubmit2KHR = nullptr;
    PFN_vkCmdWriteBufferMarker2AMD vkCmdWriteBufferMarker2AMD = nullptr;
    PFN_vkGetQueueCheckpointData2NV vkGetQueueCheckpointData2NV = nullptr;

    // extension: VK_KHR_copy_commands2

    PFN_vkCmdCopyBuffer2KHR vkCmdCopyBuffer2KHR = nullptr;
    PFN_vkCmdCopyImage2KHR vkCmdCopyImage2KHR = nullptr;
    PFN_vkCmdCopyBufferToImage2KHR vkCmdCopyBufferToImage2KHR = nullptr;
    PFN_vkCmdCopyImageToBuffer2KHR vkCmdCopyImageToBuffer2KHR = nullptr;
    PFN_vkCmdBlitImage2KHR vkCmdBlitImage2KHR = nullptr;
    PFN_vkCmdResolveImage2KHR vkCmdResolveImage2KHR = nullptr;

    // extension: VK_EXT_vertex_input_dynamic_state

    PFN_vkCmdSetVertexInputEXT vkCmdSetVertexInputEXT = nullptr;

    // extension: VK_EXT_extended_dynamic_state2
    PFN_vkCmdSetPatchControlPointsEXT vkCmdSetPatchControlPointsEXT = nullptr;
    PFN_vkCmdSetRasterizerDiscardEnableEXT vkCmdSetRasterizerDiscardEnableEXT = nullptr;
    PFN_vkCmdSetDepthBiasEnableEXT vkCmdSetDepthBiasEnableEXT = nullptr;
    PFN_vkCmdSetLogicOpEXT vkCmdSetLogicOpEXT = nullptr;
    PFN_vkCmdSetPrimitiveRestartEnableEXT vkCmdSetPrimitiveRestartEnableEXT = nullptr;

    // extension: VK_EXT_color_write_enable
    PFN_vkCmdSetColorWriteEnableEXT vkCmdSetColorWriteEnableEXT = nullptr;

private:
    template<typename T, T vulkan_functions::*member_ptr>
    void fetch_function(const char* name, VkInstance instance = nullptr) {
        this->*member_ptr = reinterpret_cast<T>(vkGetInstanceProcAddr(instance, name));
    }
};

}  // namespace vxl::vk
