static const std::pair<void (*vulkan_functions::*)(), const char*> functions_and_names[] {
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
  PTR_NAME_PAIR(vkFreeMemory),
  PTR_NAME_PAIR(vkMapMemory),
  PTR_NAME_PAIR(vkUnmapMemory),
  PTR_NAME_PAIR(vkFlushMappedMemoryRanges),
  PTR_NAME_PAIR(vkInvalidateMappedMemoryRanges),
  PTR_NAME_PAIR(vkGetDeviceMemoryCommitment),
  PTR_NAME_PAIR(vkBindBufferMemory),
  PTR_NAME_PAIR(vkBindImageMemory),
  PTR_NAME_PAIR(vkGetBufferMemoryRequirements),
  PTR_NAME_PAIR(vkGetImageMemoryRequirements),
  PTR_NAME_PAIR(vkGetImageSparseMemoryRequirements),
  PTR_NAME_PAIR(vkGetPhysicalDeviceSparseImageFormatProperties),
  PTR_NAME_PAIR(vkQueueBindSparse),
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
  PTR_NAME_PAIR(vkDestroyQueryPool),
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
  PTR_NAME_PAIR(vkCreateShaderModule),
  PTR_NAME_PAIR(vkDestroyShaderModule),
  PTR_NAME_PAIR(vkCreatePipelineCache),
  PTR_NAME_PAIR(vkDestroyPipelineCache),
  PTR_NAME_PAIR(vkGetPipelineCacheData),
  PTR_NAME_PAIR(vkMergePipelineCaches),
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
  PTR_NAME_PAIR(vkDestroyDescriptorPool),
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
  PTR_NAME_PAIR(vkDestroyCommandPool),
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
  PTR_NAME_PAIR(vkGetImageSparseMemoryRequirements2),
  PTR_NAME_PAIR(vkGetPhysicalDeviceFeatures2),
  PTR_NAME_PAIR(vkGetPhysicalDeviceProperties2),
  PTR_NAME_PAIR(vkGetPhysicalDeviceFormatProperties2),
  PTR_NAME_PAIR(vkGetPhysicalDeviceImageFormatProperties2),
  PTR_NAME_PAIR(vkGetPhysicalDeviceQueueFamilyProperties2),
  PTR_NAME_PAIR(vkGetPhysicalDeviceMemoryProperties2),
  PTR_NAME_PAIR(vkGetPhysicalDeviceSparseImageFormatProperties2),
  PTR_NAME_PAIR(vkTrimCommandPool),
  PTR_NAME_PAIR(vkGetDeviceQueue2),
  PTR_NAME_PAIR(vkCreateSamplerYcbcrConversion),
  PTR_NAME_PAIR(vkDestroySamplerYcbcrConversion),
  PTR_NAME_PAIR(vkCreateDescriptorUpdateTemplate),
  PTR_NAME_PAIR(vkDestroyDescriptorUpdateTemplate),
  PTR_NAME_PAIR(vkUpdateDescriptorSetWithTemplate),
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
  PTR_NAME_PAIR(vkDestroySwapchainKHR),
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

  // extension: VK_KHR_xlib_surface
#ifdef VK_USE_PLATFORM_XLIB_KHR
  PTR_NAME_PAIR(vkCreateXlibSurfaceKHR),
  PTR_NAME_PAIR(vkGetPhysicalDeviceXlibPresentationSupportKHR),
#endif // VK_USE_PLATFORM_XLIB_KHR

  // extension: VK_KHR_xcb_surface
#ifdef VK_USE_PLATFORM_XCB_KHR
  PTR_NAME_PAIR(vkCreateXcbSurfaceKHR),
  PTR_NAME_PAIR(vkGetPhysicalDeviceXcbPresentationSupportKHR),
#endif // VK_USE_PLATFORM_XCB_KHR

  // extension: VK_KHR_wayland_surface
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
  PTR_NAME_PAIR(vkCreateWaylandSurfaceKHR),
  PTR_NAME_PAIR(vkGetPhysicalDeviceWaylandPresentationSupportKHR),
#endif // VK_USE_PLATFORM_WAYLAND_KHR

  // extension: VK_KHR_android_surface
#ifdef VK_USE_PLATFORM_ANDROID_KHR
  PTR_NAME_PAIR(vkCreateAndroidSurfaceKHR),
#endif // VK_USE_PLATFORM_ANDROID_KHR

  // extension: VK_KHR_win32_surface
#ifdef VK_USE_PLATFORM_WIN32_KHR
  PTR_NAME_PAIR(vkCreateWin32SurfaceKHR),
  PTR_NAME_PAIR(vkGetPhysicalDeviceWin32PresentationSupportKHR),
#endif // VK_USE_PLATFORM_WIN32_KHR

  // extension: VK_EXT_debug_report
  PTR_NAME_PAIR(vkCreateDebugReportCallbackEXT),
  PTR_NAME_PAIR(vkDestroyDebugReportCallbackEXT),
  PTR_NAME_PAIR(vkDebugReportMessageEXT),

  // extension: VK_EXT_debug_marker
  PTR_NAME_PAIR(vkDebugMarkerSetObjectTagEXT),
  PTR_NAME_PAIR(vkDebugMarkerSetObjectNameEXT),
  PTR_NAME_PAIR(vkCmdDebugMarkerBeginEXT),
  PTR_NAME_PAIR(vkCmdDebugMarkerEndEXT),
  PTR_NAME_PAIR(vkCmdDebugMarkerInsertEXT),

  // extension: VK_KHR_video_queue
  PTR_NAME_PAIR(vkGetPhysicalDeviceVideoCapabilitiesKHR),
  PTR_NAME_PAIR(vkGetPhysicalDeviceVideoFormatPropertiesKHR),
  PTR_NAME_PAIR(vkCreateVideoSessionKHR),
  PTR_NAME_PAIR(vkDestroyVideoSessionKHR),
  PTR_NAME_PAIR(vkGetVideoSessionMemoryRequirementsKHR),
  PTR_NAME_PAIR(vkBindVideoSessionMemoryKHR),
  PTR_NAME_PAIR(vkCreateVideoSessionParametersKHR),
  PTR_NAME_PAIR(vkUpdateVideoSessionParametersKHR),
  PTR_NAME_PAIR(vkDestroyVideoSessionParametersKHR),
  PTR_NAME_PAIR(vkCmdBeginVideoCodingKHR),
  PTR_NAME_PAIR(vkCmdEndVideoCodingKHR),
  PTR_NAME_PAIR(vkCmdControlVideoCodingKHR),

  // extension: VK_KHR_video_decode_queue
  PTR_NAME_PAIR(vkCmdDecodeVideoKHR),

  // extension: VK_EXT_transform_feedback
  PTR_NAME_PAIR(vkCmdBindTransformFeedbackBuffersEXT),
  PTR_NAME_PAIR(vkCmdBeginTransformFeedbackEXT),
  PTR_NAME_PAIR(vkCmdEndTransformFeedbackEXT),
  PTR_NAME_PAIR(vkCmdBeginQueryIndexedEXT),
  PTR_NAME_PAIR(vkCmdEndQueryIndexedEXT),
  PTR_NAME_PAIR(vkCmdDrawIndirectByteCountEXT),

  // extension: VK_NVX_binary_import
  PTR_NAME_PAIR(vkCreateCuModuleNVX),
  PTR_NAME_PAIR(vkCreateCuFunctionNVX),
  PTR_NAME_PAIR(vkDestroyCuModuleNVX),
  PTR_NAME_PAIR(vkDestroyCuFunctionNVX),
  PTR_NAME_PAIR(vkCmdCuLaunchKernelNVX),

  // extension: VK_NVX_image_view_handle
  PTR_NAME_PAIR(vkGetImageViewHandleNVX),
  PTR_NAME_PAIR(vkGetImageViewAddressNVX),

  // extension: VK_AMD_draw_indirect_count
  PTR_NAME_PAIR(vkCmdDrawIndirectCountAMD),
  PTR_NAME_PAIR(vkCmdDrawIndexedIndirectCountAMD),

  // extension: VK_AMD_shader_info
  PTR_NAME_PAIR(vkGetShaderInfoAMD),

  // extension: VK_KHR_dynamic_rendering
  PTR_NAME_PAIR(vkCmdBeginRenderingKHR),
  PTR_NAME_PAIR(vkCmdEndRenderingKHR),

  // extension: VK_GGP_stream_descriptor_surface
#ifdef VK_USE_PLATFORM_GGP
  PTR_NAME_PAIR(vkCreateStreamDescriptorSurfaceGGP),
#endif // VK_USE_PLATFORM_GGP

  // extension: VK_NV_external_memory_capabilities
  PTR_NAME_PAIR(vkGetPhysicalDeviceExternalImageFormatPropertiesNV),

  // extension: VK_NV_external_memory_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
  PTR_NAME_PAIR(vkGetMemoryWin32HandleNV),
#endif // VK_USE_PLATFORM_WIN32_KHR

  // extension: VK_KHR_get_physical_device_properties2
  PTR_NAME_PAIR(vkGetPhysicalDeviceFeatures2KHR),
  PTR_NAME_PAIR(vkGetPhysicalDeviceProperties2KHR),
  PTR_NAME_PAIR(vkGetPhysicalDeviceFormatProperties2KHR),
  PTR_NAME_PAIR(vkGetPhysicalDeviceImageFormatProperties2KHR),
  PTR_NAME_PAIR(vkGetPhysicalDeviceQueueFamilyProperties2KHR),
  PTR_NAME_PAIR(vkGetPhysicalDeviceMemoryProperties2KHR),
  PTR_NAME_PAIR(vkGetPhysicalDeviceSparseImageFormatProperties2KHR),

  // extension: VK_KHR_device_group
  PTR_NAME_PAIR(vkGetDeviceGroupPeerMemoryFeaturesKHR),
  PTR_NAME_PAIR(vkCmdSetDeviceMaskKHR),
  PTR_NAME_PAIR(vkCmdDispatchBaseKHR),

  // extension: VK_NN_vi_surface
#ifdef VK_USE_PLATFORM_VI_NN
  PTR_NAME_PAIR(vkCreateViSurfaceNN),
#endif // VK_USE_PLATFORM_VI_NN

  // extension: VK_KHR_maintenance1
  PTR_NAME_PAIR(vkTrimCommandPoolKHR),

  // extension: VK_KHR_device_group_creation
  PTR_NAME_PAIR(vkEnumeratePhysicalDeviceGroupsKHR),

  // extension: VK_KHR_external_memory_capabilities
  PTR_NAME_PAIR(vkGetPhysicalDeviceExternalBufferPropertiesKHR),

  // extension: VK_KHR_external_memory_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
  PTR_NAME_PAIR(vkGetMemoryWin32HandleKHR),
  PTR_NAME_PAIR(vkGetMemoryWin32HandlePropertiesKHR),
#endif // VK_USE_PLATFORM_WIN32_KHR

  // extension: VK_KHR_external_memory_fd
  PTR_NAME_PAIR(vkGetMemoryFdKHR),
  PTR_NAME_PAIR(vkGetMemoryFdPropertiesKHR),

  // extension: VK_KHR_external_semaphore_capabilities
  PTR_NAME_PAIR(vkGetPhysicalDeviceExternalSemaphorePropertiesKHR),

  // extension: VK_KHR_external_semaphore_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
  PTR_NAME_PAIR(vkImportSemaphoreWin32HandleKHR),
  PTR_NAME_PAIR(vkGetSemaphoreWin32HandleKHR),
#endif // VK_USE_PLATFORM_WIN32_KHR

  // extension: VK_KHR_external_semaphore_fd
  PTR_NAME_PAIR(vkImportSemaphoreFdKHR),
  PTR_NAME_PAIR(vkGetSemaphoreFdKHR),

  // extension: VK_KHR_push_descriptor
  PTR_NAME_PAIR(vkCmdPushDescriptorSetKHR),
  PTR_NAME_PAIR(vkCmdPushDescriptorSetWithTemplateKHR),

  // extension: VK_EXT_conditional_rendering
  PTR_NAME_PAIR(vkCmdBeginConditionalRenderingEXT),
  PTR_NAME_PAIR(vkCmdEndConditionalRenderingEXT),

  // extension: VK_KHR_descriptor_update_template
  PTR_NAME_PAIR(vkCreateDescriptorUpdateTemplateKHR),
  PTR_NAME_PAIR(vkDestroyDescriptorUpdateTemplateKHR),
  PTR_NAME_PAIR(vkUpdateDescriptorSetWithTemplateKHR),

  // extension: VK_NV_clip_space_w_scaling
  PTR_NAME_PAIR(vkCmdSetViewportWScalingNV),

  // extension: VK_EXT_direct_mode_display
  PTR_NAME_PAIR(vkReleaseDisplayEXT),

  // extension: VK_EXT_acquire_xlib_display
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
  PTR_NAME_PAIR(vkAcquireXlibDisplayEXT),
  PTR_NAME_PAIR(vkGetRandROutputDisplayEXT),
#endif // VK_USE_PLATFORM_XLIB_XRANDR_EXT

  // extension: VK_EXT_display_surface_counter
  PTR_NAME_PAIR(vkGetPhysicalDeviceSurfaceCapabilities2EXT),

  // extension: VK_EXT_display_control
  PTR_NAME_PAIR(vkDisplayPowerControlEXT),
  PTR_NAME_PAIR(vkRegisterDeviceEventEXT),
  PTR_NAME_PAIR(vkRegisterDisplayEventEXT),
  PTR_NAME_PAIR(vkGetSwapchainCounterEXT),

  // extension: VK_GOOGLE_display_timing
  PTR_NAME_PAIR(vkGetRefreshCycleDurationGOOGLE),
  PTR_NAME_PAIR(vkGetPastPresentationTimingGOOGLE),

  // extension: VK_EXT_discard_rectangles
  PTR_NAME_PAIR(vkCmdSetDiscardRectangleEXT),
  PTR_NAME_PAIR(vkCmdSetDiscardRectangleEnableEXT),
  PTR_NAME_PAIR(vkCmdSetDiscardRectangleModeEXT),

  // extension: VK_EXT_hdr_metadata
  PTR_NAME_PAIR(vkSetHdrMetadataEXT),

  // extension: VK_KHR_create_renderpass2
  PTR_NAME_PAIR(vkCreateRenderPass2KHR),
  PTR_NAME_PAIR(vkCmdBeginRenderPass2KHR),
  PTR_NAME_PAIR(vkCmdNextSubpass2KHR),
  PTR_NAME_PAIR(vkCmdEndRenderPass2KHR),

  // extension: VK_KHR_shared_presentable_image
  PTR_NAME_PAIR(vkGetSwapchainStatusKHR),

  // extension: VK_KHR_external_fence_capabilities
  PTR_NAME_PAIR(vkGetPhysicalDeviceExternalFencePropertiesKHR),

  // extension: VK_KHR_external_fence_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
  PTR_NAME_PAIR(vkImportFenceWin32HandleKHR),
  PTR_NAME_PAIR(vkGetFenceWin32HandleKHR),
#endif // VK_USE_PLATFORM_WIN32_KHR

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

  // extension: VK_MVK_ios_surface
#ifdef VK_USE_PLATFORM_IOS_MVK
  PTR_NAME_PAIR(vkCreateIOSSurfaceMVK),
#endif // VK_USE_PLATFORM_IOS_MVK

  // extension: VK_MVK_macos_surface
#ifdef VK_USE_PLATFORM_MACOS_MVK
  PTR_NAME_PAIR(vkCreateMacOSSurfaceMVK),
#endif // VK_USE_PLATFORM_MACOS_MVK

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

  // extension: VK_ANDROID_external_memory_android_hardware_buffer
#ifdef VK_USE_PLATFORM_ANDROID_KHR
  PTR_NAME_PAIR(vkGetAndroidHardwareBufferPropertiesANDROID),
  PTR_NAME_PAIR(vkGetMemoryAndroidHardwareBufferANDROID),
#endif // VK_USE_PLATFORM_ANDROID_KHR

  // extension: VK_AMDX_shader_enqueue
#ifdef VK_ENABLE_BETA_EXTENSIONS
  PTR_NAME_PAIR(vkCreateExecutionGraphPipelinesAMDX),
  PTR_NAME_PAIR(vkGetExecutionGraphPipelineScratchSizeAMDX),
  PTR_NAME_PAIR(vkGetExecutionGraphPipelineNodeIndexAMDX),
  PTR_NAME_PAIR(vkCmdInitializeGraphScratchMemoryAMDX),
  PTR_NAME_PAIR(vkCmdDispatchGraphAMDX),
  PTR_NAME_PAIR(vkCmdDispatchGraphIndirectAMDX),
  PTR_NAME_PAIR(vkCmdDispatchGraphIndirectCountAMDX),
#endif // VK_ENABLE_BETA_EXTENSIONS

  // extension: VK_EXT_sample_locations
  PTR_NAME_PAIR(vkCmdSetSampleLocationsEXT),
  PTR_NAME_PAIR(vkGetPhysicalDeviceMultisamplePropertiesEXT),

  // extension: VK_KHR_get_memory_requirements2
  PTR_NAME_PAIR(vkGetImageMemoryRequirements2KHR),
  PTR_NAME_PAIR(vkGetBufferMemoryRequirements2KHR),
  PTR_NAME_PAIR(vkGetImageSparseMemoryRequirements2KHR),

  // extension: VK_KHR_acceleration_structure
  PTR_NAME_PAIR(vkCreateAccelerationStructureKHR),
  PTR_NAME_PAIR(vkDestroyAccelerationStructureKHR),
  PTR_NAME_PAIR(vkCmdBuildAccelerationStructuresKHR),
  PTR_NAME_PAIR(vkCmdBuildAccelerationStructuresIndirectKHR),
  PTR_NAME_PAIR(vkBuildAccelerationStructuresKHR),
  PTR_NAME_PAIR(vkCopyAccelerationStructureKHR),
  PTR_NAME_PAIR(vkCopyAccelerationStructureToMemoryKHR),
  PTR_NAME_PAIR(vkCopyMemoryToAccelerationStructureKHR),
  PTR_NAME_PAIR(vkWriteAccelerationStructuresPropertiesKHR),
  PTR_NAME_PAIR(vkCmdCopyAccelerationStructureKHR),
  PTR_NAME_PAIR(vkCmdCopyAccelerationStructureToMemoryKHR),
  PTR_NAME_PAIR(vkCmdCopyMemoryToAccelerationStructureKHR),
  PTR_NAME_PAIR(vkGetAccelerationStructureDeviceAddressKHR),
  PTR_NAME_PAIR(vkCmdWriteAccelerationStructuresPropertiesKHR),
  PTR_NAME_PAIR(vkGetDeviceAccelerationStructureCompatibilityKHR),
  PTR_NAME_PAIR(vkGetAccelerationStructureBuildSizesKHR),

  // extension: VK_KHR_ray_tracing_pipeline
  PTR_NAME_PAIR(vkCmdTraceRaysKHR),
  PTR_NAME_PAIR(vkCreateRayTracingPipelinesKHR),
  PTR_NAME_PAIR(vkGetRayTracingShaderGroupHandlesKHR),
  PTR_NAME_PAIR(vkGetRayTracingCaptureReplayShaderGroupHandlesKHR),
  PTR_NAME_PAIR(vkCmdTraceRaysIndirectKHR),
  PTR_NAME_PAIR(vkGetRayTracingShaderGroupStackSizeKHR),
  PTR_NAME_PAIR(vkCmdSetRayTracingPipelineStackSizeKHR),

  // extension: VK_KHR_sampler_ycbcr_conversion
  PTR_NAME_PAIR(vkCreateSamplerYcbcrConversionKHR),
  PTR_NAME_PAIR(vkDestroySamplerYcbcrConversionKHR),

  // extension: VK_KHR_bind_memory2
  PTR_NAME_PAIR(vkBindBufferMemory2KHR),
  PTR_NAME_PAIR(vkBindImageMemory2KHR),

  // extension: VK_EXT_image_drm_format_modifier
  PTR_NAME_PAIR(vkGetImageDrmFormatModifierPropertiesEXT),

  // extension: VK_EXT_validation_cache
  PTR_NAME_PAIR(vkCreateValidationCacheEXT),
  PTR_NAME_PAIR(vkDestroyValidationCacheEXT),
  PTR_NAME_PAIR(vkMergeValidationCachesEXT),
  PTR_NAME_PAIR(vkGetValidationCacheDataEXT),

  // extension: VK_NV_shading_rate_image
  PTR_NAME_PAIR(vkCmdBindShadingRateImageNV),
  PTR_NAME_PAIR(vkCmdSetViewportShadingRatePaletteNV),
  PTR_NAME_PAIR(vkCmdSetCoarseSampleOrderNV),

  // extension: VK_NV_ray_tracing
  PTR_NAME_PAIR(vkCreateAccelerationStructureNV),
  PTR_NAME_PAIR(vkDestroyAccelerationStructureNV),
  PTR_NAME_PAIR(vkGetAccelerationStructureMemoryRequirementsNV),
  PTR_NAME_PAIR(vkBindAccelerationStructureMemoryNV),
  PTR_NAME_PAIR(vkCmdBuildAccelerationStructureNV),
  PTR_NAME_PAIR(vkCmdCopyAccelerationStructureNV),
  PTR_NAME_PAIR(vkCmdTraceRaysNV),
  PTR_NAME_PAIR(vkCreateRayTracingPipelinesNV),
  PTR_NAME_PAIR(vkGetRayTracingShaderGroupHandlesNV),
  PTR_NAME_PAIR(vkGetAccelerationStructureHandleNV),
  PTR_NAME_PAIR(vkCmdWriteAccelerationStructuresPropertiesNV),
  PTR_NAME_PAIR(vkCompileDeferredNV),

  // extension: VK_KHR_maintenance3
  PTR_NAME_PAIR(vkGetDescriptorSetLayoutSupportKHR),

  // extension: VK_KHR_draw_indirect_count
  PTR_NAME_PAIR(vkCmdDrawIndirectCountKHR),
  PTR_NAME_PAIR(vkCmdDrawIndexedIndirectCountKHR),

  // extension: VK_EXT_external_memory_host
  PTR_NAME_PAIR(vkGetMemoryHostPointerPropertiesEXT),

  // extension: VK_AMD_buffer_marker
  PTR_NAME_PAIR(vkCmdWriteBufferMarkerAMD),

  // extension: VK_EXT_calibrated_timestamps
  PTR_NAME_PAIR(vkGetPhysicalDeviceCalibrateableTimeDomainsEXT),
  PTR_NAME_PAIR(vkGetCalibratedTimestampsEXT),

  // extension: VK_NV_mesh_shader
  PTR_NAME_PAIR(vkCmdDrawMeshTasksNV),
  PTR_NAME_PAIR(vkCmdDrawMeshTasksIndirectNV),
  PTR_NAME_PAIR(vkCmdDrawMeshTasksIndirectCountNV),

  // extension: VK_NV_scissor_exclusive
  PTR_NAME_PAIR(vkCmdSetExclusiveScissorEnableNV),
  PTR_NAME_PAIR(vkCmdSetExclusiveScissorNV),

  // extension: VK_NV_device_diagnostic_checkpoints
  PTR_NAME_PAIR(vkCmdSetCheckpointNV),
  PTR_NAME_PAIR(vkGetQueueCheckpointDataNV),

  // extension: VK_KHR_timeline_semaphore
  PTR_NAME_PAIR(vkGetSemaphoreCounterValueKHR),
  PTR_NAME_PAIR(vkWaitSemaphoresKHR),
  PTR_NAME_PAIR(vkSignalSemaphoreKHR),

  // extension: VK_INTEL_performance_query
  PTR_NAME_PAIR(vkInitializePerformanceApiINTEL),
  PTR_NAME_PAIR(vkUninitializePerformanceApiINTEL),
  PTR_NAME_PAIR(vkCmdSetPerformanceMarkerINTEL),
  PTR_NAME_PAIR(vkCmdSetPerformanceStreamMarkerINTEL),
  PTR_NAME_PAIR(vkCmdSetPerformanceOverrideINTEL),
  PTR_NAME_PAIR(vkAcquirePerformanceConfigurationINTEL),
  PTR_NAME_PAIR(vkReleasePerformanceConfigurationINTEL),
  PTR_NAME_PAIR(vkQueueSetPerformanceConfigurationINTEL),
  PTR_NAME_PAIR(vkGetPerformanceParameterINTEL),

  // extension: VK_AMD_display_native_hdr
  PTR_NAME_PAIR(vkSetLocalDimmingAMD),

  // extension: VK_FUCHSIA_imagepipe_surface
#ifdef VK_USE_PLATFORM_FUCHSIA
  PTR_NAME_PAIR(vkCreateImagePipeSurfaceFUCHSIA),
#endif // VK_USE_PLATFORM_FUCHSIA

  // extension: VK_EXT_metal_surface
#ifdef VK_USE_PLATFORM_METAL_EXT
  PTR_NAME_PAIR(vkCreateMetalSurfaceEXT),
#endif // VK_USE_PLATFORM_METAL_EXT

  // extension: VK_KHR_fragment_shading_rate
  PTR_NAME_PAIR(vkGetPhysicalDeviceFragmentShadingRatesKHR),
  PTR_NAME_PAIR(vkCmdSetFragmentShadingRateKHR),

  // extension: VK_EXT_buffer_device_address
  PTR_NAME_PAIR(vkGetBufferDeviceAddressEXT),

  // extension: VK_EXT_tooling_info
  PTR_NAME_PAIR(vkGetPhysicalDeviceToolPropertiesEXT),

  // extension: VK_KHR_present_wait
  PTR_NAME_PAIR(vkWaitForPresentKHR),

  // extension: VK_NV_cooperative_matrix
  PTR_NAME_PAIR(vkGetPhysicalDeviceCooperativeMatrixPropertiesNV),

  // extension: VK_NV_coverage_reduction_mode
  PTR_NAME_PAIR(vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV),

  // extension: VK_EXT_full_screen_exclusive
#ifdef VK_USE_PLATFORM_WIN32_KHR
  PTR_NAME_PAIR(vkGetPhysicalDeviceSurfacePresentModes2EXT),
  PTR_NAME_PAIR(vkAcquireFullScreenExclusiveModeEXT),
  PTR_NAME_PAIR(vkReleaseFullScreenExclusiveModeEXT),
  PTR_NAME_PAIR(vkGetDeviceGroupSurfacePresentModes2EXT),
#endif // VK_USE_PLATFORM_WIN32_KHR

  // extension: VK_EXT_headless_surface
  PTR_NAME_PAIR(vkCreateHeadlessSurfaceEXT),

  // extension: VK_KHR_buffer_device_address
  PTR_NAME_PAIR(vkGetBufferDeviceAddressKHR),
  PTR_NAME_PAIR(vkGetBufferOpaqueCaptureAddressKHR),
  PTR_NAME_PAIR(vkGetDeviceMemoryOpaqueCaptureAddressKHR),

  // extension: VK_EXT_line_rasterization
  PTR_NAME_PAIR(vkCmdSetLineStippleEXT),

  // extension: VK_EXT_host_query_reset
  PTR_NAME_PAIR(vkResetQueryPoolEXT),

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

  // extension: VK_KHR_deferred_host_operations
  PTR_NAME_PAIR(vkCreateDeferredOperationKHR),
  PTR_NAME_PAIR(vkDestroyDeferredOperationKHR),
  PTR_NAME_PAIR(vkGetDeferredOperationMaxConcurrencyKHR),
  PTR_NAME_PAIR(vkGetDeferredOperationResultKHR),
  PTR_NAME_PAIR(vkDeferredOperationJoinKHR),

  // extension: VK_KHR_pipeline_executable_properties
  PTR_NAME_PAIR(vkGetPipelineExecutablePropertiesKHR),
  PTR_NAME_PAIR(vkGetPipelineExecutableStatisticsKHR),
  PTR_NAME_PAIR(vkGetPipelineExecutableInternalRepresentationsKHR),

  // extension: VK_EXT_host_image_copy
#ifdef VXL_VK_DO_NOT_DEFINE
  PTR_NAME_PAIR(vkCopyMemoryToImageEXT),
  PTR_NAME_PAIR(vkCopyImageToMemoryEXT),
  PTR_NAME_PAIR(vkCopyImageToImageEXT),
  PTR_NAME_PAIR(vkTransitionImageLayoutEXT),
  PTR_NAME_PAIR(vkGetImageSubresourceLayout2EXT),
#endif // VXL_VK_DO_NOT_DEFINE

  // extension: VK_KHR_map_memory2
#ifdef VXL_VK_DO_NOT_DEFINE
  PTR_NAME_PAIR(vkMapMemory2KHR),
  PTR_NAME_PAIR(vkUnmapMemory2KHR),
#endif // VXL_VK_DO_NOT_DEFINE

  // extension: VK_EXT_swapchain_maintenance1
  PTR_NAME_PAIR(vkReleaseSwapchainImagesEXT),

  // extension: VK_NV_device_generated_commands
  PTR_NAME_PAIR(vkGetGeneratedCommandsMemoryRequirementsNV),
  PTR_NAME_PAIR(vkCmdPreprocessGeneratedCommandsNV),
  PTR_NAME_PAIR(vkCmdExecuteGeneratedCommandsNV),
  PTR_NAME_PAIR(vkCmdBindPipelineShaderGroupNV),
  PTR_NAME_PAIR(vkCreateIndirectCommandsLayoutNV),
  PTR_NAME_PAIR(vkDestroyIndirectCommandsLayoutNV),

  // extension: VK_EXT_depth_bias_control
#ifdef VXL_VK_DO_NOT_DEFINE
  PTR_NAME_PAIR(vkCmdSetDepthBias2EXT),
#endif // VXL_VK_DO_NOT_DEFINE

  // extension: VK_EXT_acquire_drm_display
  PTR_NAME_PAIR(vkAcquireDrmDisplayEXT),
  PTR_NAME_PAIR(vkGetDrmDisplayEXT),

  // extension: VK_EXT_private_data
  PTR_NAME_PAIR(vkCreatePrivateDataSlotEXT),
  PTR_NAME_PAIR(vkDestroyPrivateDataSlotEXT),
  PTR_NAME_PAIR(vkSetPrivateDataEXT),
  PTR_NAME_PAIR(vkGetPrivateDataEXT),

  // extension: VK_KHR_video_encode_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
  PTR_NAME_PAIR(vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR),
  PTR_NAME_PAIR(vkGetEncodedVideoSessionParametersKHR),
  PTR_NAME_PAIR(vkCmdEncodeVideoKHR),
#endif // VK_ENABLE_BETA_EXTENSIONS

  // extension: VK_EXT_metal_objects
#ifdef VK_USE_PLATFORM_METAL_EXT
  PTR_NAME_PAIR(vkExportMetalObjectsEXT),
#endif // VK_USE_PLATFORM_METAL_EXT

  // extension: VK_KHR_synchronization2
  PTR_NAME_PAIR(vkCmdSetEvent2KHR),
  PTR_NAME_PAIR(vkCmdResetEvent2KHR),
  PTR_NAME_PAIR(vkCmdWaitEvents2KHR),
  PTR_NAME_PAIR(vkCmdPipelineBarrier2KHR),
  PTR_NAME_PAIR(vkCmdWriteTimestamp2KHR),
  PTR_NAME_PAIR(vkQueueSubmit2KHR),
  PTR_NAME_PAIR(vkCmdWriteBufferMarker2AMD),
  PTR_NAME_PAIR(vkGetQueueCheckpointData2NV),

  // extension: VK_EXT_descriptor_buffer
  PTR_NAME_PAIR(vkGetDescriptorSetLayoutSizeEXT),
  PTR_NAME_PAIR(vkGetDescriptorSetLayoutBindingOffsetEXT),
  PTR_NAME_PAIR(vkGetDescriptorEXT),
  PTR_NAME_PAIR(vkCmdBindDescriptorBuffersEXT),
  PTR_NAME_PAIR(vkCmdSetDescriptorBufferOffsetsEXT),
  PTR_NAME_PAIR(vkCmdBindDescriptorBufferEmbeddedSamplersEXT),
  PTR_NAME_PAIR(vkGetBufferOpaqueCaptureDescriptorDataEXT),
  PTR_NAME_PAIR(vkGetImageOpaqueCaptureDescriptorDataEXT),
  PTR_NAME_PAIR(vkGetImageViewOpaqueCaptureDescriptorDataEXT),
  PTR_NAME_PAIR(vkGetSamplerOpaqueCaptureDescriptorDataEXT),
  PTR_NAME_PAIR(vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT),

  // extension: VK_NV_fragment_shading_rate_enums
  PTR_NAME_PAIR(vkCmdSetFragmentShadingRateEnumNV),

  // extension: VK_EXT_mesh_shader
  PTR_NAME_PAIR(vkCmdDrawMeshTasksEXT),
  PTR_NAME_PAIR(vkCmdDrawMeshTasksIndirectEXT),
  PTR_NAME_PAIR(vkCmdDrawMeshTasksIndirectCountEXT),

  // extension: VK_KHR_copy_commands2
  PTR_NAME_PAIR(vkCmdCopyBuffer2KHR),
  PTR_NAME_PAIR(vkCmdCopyImage2KHR),
  PTR_NAME_PAIR(vkCmdCopyBufferToImage2KHR),
  PTR_NAME_PAIR(vkCmdCopyImageToBuffer2KHR),
  PTR_NAME_PAIR(vkCmdBlitImage2KHR),
  PTR_NAME_PAIR(vkCmdResolveImage2KHR),

  // extension: VK_EXT_device_fault
  PTR_NAME_PAIR(vkGetDeviceFaultInfoEXT),

  // extension: VK_NV_acquire_winrt_display
#ifdef VK_USE_PLATFORM_WIN32_KHR
  PTR_NAME_PAIR(vkAcquireWinrtDisplayNV),
  PTR_NAME_PAIR(vkGetWinrtDisplayNV),
#endif // VK_USE_PLATFORM_WIN32_KHR

  // extension: VK_EXT_directfb_surface
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
  PTR_NAME_PAIR(vkCreateDirectFBSurfaceEXT),
  PTR_NAME_PAIR(vkGetPhysicalDeviceDirectFBPresentationSupportEXT),
#endif // VK_USE_PLATFORM_DIRECTFB_EXT

  // extension: VK_EXT_vertex_input_dynamic_state
  PTR_NAME_PAIR(vkCmdSetVertexInputEXT),

  // extension: VK_FUCHSIA_external_memory
#ifdef VK_USE_PLATFORM_FUCHSIA
  PTR_NAME_PAIR(vkGetMemoryZirconHandleFUCHSIA),
  PTR_NAME_PAIR(vkGetMemoryZirconHandlePropertiesFUCHSIA),
#endif // VK_USE_PLATFORM_FUCHSIA

  // extension: VK_FUCHSIA_external_semaphore
#ifdef VK_USE_PLATFORM_FUCHSIA
  PTR_NAME_PAIR(vkImportSemaphoreZirconHandleFUCHSIA),
  PTR_NAME_PAIR(vkGetSemaphoreZirconHandleFUCHSIA),
#endif // VK_USE_PLATFORM_FUCHSIA

  // extension: VK_FUCHSIA_buffer_collection
#ifdef VK_USE_PLATFORM_FUCHSIA
  PTR_NAME_PAIR(vkCreateBufferCollectionFUCHSIA),
  PTR_NAME_PAIR(vkSetBufferCollectionImageConstraintsFUCHSIA),
  PTR_NAME_PAIR(vkSetBufferCollectionBufferConstraintsFUCHSIA),
  PTR_NAME_PAIR(vkDestroyBufferCollectionFUCHSIA),
  PTR_NAME_PAIR(vkGetBufferCollectionPropertiesFUCHSIA),
#endif // VK_USE_PLATFORM_FUCHSIA

  // extension: VK_HUAWEI_subpass_shading
  PTR_NAME_PAIR(vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI),
  PTR_NAME_PAIR(vkCmdSubpassShadingHUAWEI),

  // extension: VK_HUAWEI_invocation_mask
  PTR_NAME_PAIR(vkCmdBindInvocationMaskHUAWEI),

  // extension: VK_NV_external_memory_rdma
  PTR_NAME_PAIR(vkGetMemoryRemoteAddressNV),

  // extension: VK_EXT_pipeline_properties
  PTR_NAME_PAIR(vkGetPipelinePropertiesEXT),

  // extension: VK_EXT_extended_dynamic_state2
  PTR_NAME_PAIR(vkCmdSetPatchControlPointsEXT),
  PTR_NAME_PAIR(vkCmdSetRasterizerDiscardEnableEXT),
  PTR_NAME_PAIR(vkCmdSetDepthBiasEnableEXT),
  PTR_NAME_PAIR(vkCmdSetLogicOpEXT),
  PTR_NAME_PAIR(vkCmdSetPrimitiveRestartEnableEXT),

  // extension: VK_QNX_screen_surface
#ifdef VK_USE_PLATFORM_SCREEN_QNX
  PTR_NAME_PAIR(vkCreateScreenSurfaceQNX),
  PTR_NAME_PAIR(vkGetPhysicalDeviceScreenPresentationSupportQNX),
#endif // VK_USE_PLATFORM_SCREEN_QNX

  // extension: VK_EXT_color_write_enable
  PTR_NAME_PAIR(vkCmdSetColorWriteEnableEXT),

  // extension: VK_KHR_ray_tracing_maintenance1
  PTR_NAME_PAIR(vkCmdTraceRaysIndirect2KHR),

  // extension: VK_EXT_multi_draw
  PTR_NAME_PAIR(vkCmdDrawMultiEXT),
  PTR_NAME_PAIR(vkCmdDrawMultiIndexedEXT),

  // extension: VK_EXT_opacity_micromap
  PTR_NAME_PAIR(vkCreateMicromapEXT),
  PTR_NAME_PAIR(vkDestroyMicromapEXT),
  PTR_NAME_PAIR(vkCmdBuildMicromapsEXT),
  PTR_NAME_PAIR(vkBuildMicromapsEXT),
  PTR_NAME_PAIR(vkCopyMicromapEXT),
  PTR_NAME_PAIR(vkCopyMicromapToMemoryEXT),
  PTR_NAME_PAIR(vkCopyMemoryToMicromapEXT),
  PTR_NAME_PAIR(vkWriteMicromapsPropertiesEXT),
  PTR_NAME_PAIR(vkCmdCopyMicromapEXT),
  PTR_NAME_PAIR(vkCmdCopyMicromapToMemoryEXT),
  PTR_NAME_PAIR(vkCmdCopyMemoryToMicromapEXT),
  PTR_NAME_PAIR(vkCmdWriteMicromapsPropertiesEXT),
  PTR_NAME_PAIR(vkGetDeviceMicromapCompatibilityEXT),
  PTR_NAME_PAIR(vkGetMicromapBuildSizesEXT),

  // extension: VK_HUAWEI_cluster_culling_shader
  PTR_NAME_PAIR(vkCmdDrawClusterHUAWEI),
  PTR_NAME_PAIR(vkCmdDrawClusterIndirectHUAWEI),

  // extension: VK_EXT_pageable_device_local_memory
  PTR_NAME_PAIR(vkSetDeviceMemoryPriorityEXT),

  // extension: VK_KHR_maintenance4
  PTR_NAME_PAIR(vkGetDeviceBufferMemoryRequirementsKHR),
  PTR_NAME_PAIR(vkGetDeviceImageMemoryRequirementsKHR),
  PTR_NAME_PAIR(vkGetDeviceImageSparseMemoryRequirementsKHR),

  // extension: VK_VALVE_descriptor_set_host_mapping
  PTR_NAME_PAIR(vkGetDescriptorSetLayoutHostMappingInfoVALVE),
  PTR_NAME_PAIR(vkGetDescriptorSetHostMappingVALVE),

  // extension: VK_NV_copy_memory_indirect
  PTR_NAME_PAIR(vkCmdCopyMemoryIndirectNV),
  PTR_NAME_PAIR(vkCmdCopyMemoryToImageIndirectNV),

  // extension: VK_NV_memory_decompression
  PTR_NAME_PAIR(vkCmdDecompressMemoryNV),
  PTR_NAME_PAIR(vkCmdDecompressMemoryIndirectCountNV),

  // extension: VK_NV_device_generated_commands_compute
#ifdef VXL_VK_DO_NOT_DEFINE
  PTR_NAME_PAIR(vkGetPipelineIndirectMemoryRequirementsNV),
  PTR_NAME_PAIR(vkCmdUpdatePipelineIndirectBufferNV),
  PTR_NAME_PAIR(vkGetPipelineIndirectDeviceAddressNV),
#endif // VXL_VK_DO_NOT_DEFINE

  // extension: VK_EXT_extended_dynamic_state3
  PTR_NAME_PAIR(vkCmdSetTessellationDomainOriginEXT),
  PTR_NAME_PAIR(vkCmdSetDepthClampEnableEXT),
  PTR_NAME_PAIR(vkCmdSetPolygonModeEXT),
  PTR_NAME_PAIR(vkCmdSetRasterizationSamplesEXT),
  PTR_NAME_PAIR(vkCmdSetSampleMaskEXT),
  PTR_NAME_PAIR(vkCmdSetAlphaToCoverageEnableEXT),
  PTR_NAME_PAIR(vkCmdSetAlphaToOneEnableEXT),
  PTR_NAME_PAIR(vkCmdSetLogicOpEnableEXT),
  PTR_NAME_PAIR(vkCmdSetColorBlendEnableEXT),
  PTR_NAME_PAIR(vkCmdSetColorBlendEquationEXT),
  PTR_NAME_PAIR(vkCmdSetColorWriteMaskEXT),
  PTR_NAME_PAIR(vkCmdSetRasterizationStreamEXT),
  PTR_NAME_PAIR(vkCmdSetConservativeRasterizationModeEXT),
  PTR_NAME_PAIR(vkCmdSetExtraPrimitiveOverestimationSizeEXT),
  PTR_NAME_PAIR(vkCmdSetDepthClipEnableEXT),
  PTR_NAME_PAIR(vkCmdSetSampleLocationsEnableEXT),
  PTR_NAME_PAIR(vkCmdSetColorBlendAdvancedEXT),
  PTR_NAME_PAIR(vkCmdSetProvokingVertexModeEXT),
  PTR_NAME_PAIR(vkCmdSetLineRasterizationModeEXT),
  PTR_NAME_PAIR(vkCmdSetLineStippleEnableEXT),
  PTR_NAME_PAIR(vkCmdSetDepthClipNegativeOneToOneEXT),
  PTR_NAME_PAIR(vkCmdSetViewportWScalingEnableNV),
  PTR_NAME_PAIR(vkCmdSetViewportSwizzleNV),
  PTR_NAME_PAIR(vkCmdSetCoverageToColorEnableNV),
  PTR_NAME_PAIR(vkCmdSetCoverageToColorLocationNV),
  PTR_NAME_PAIR(vkCmdSetCoverageModulationModeNV),
  PTR_NAME_PAIR(vkCmdSetCoverageModulationTableEnableNV),
  PTR_NAME_PAIR(vkCmdSetCoverageModulationTableNV),
  PTR_NAME_PAIR(vkCmdSetShadingRateImageEnableNV),
  PTR_NAME_PAIR(vkCmdSetRepresentativeFragmentTestEnableNV),
  PTR_NAME_PAIR(vkCmdSetCoverageReductionModeNV),

  // extension: VK_EXT_shader_module_identifier
  PTR_NAME_PAIR(vkGetShaderModuleIdentifierEXT),
  PTR_NAME_PAIR(vkGetShaderModuleCreateInfoIdentifierEXT),

  // extension: VK_NV_optical_flow
  PTR_NAME_PAIR(vkGetPhysicalDeviceOpticalFlowImageFormatsNV),
  PTR_NAME_PAIR(vkCreateOpticalFlowSessionNV),
  PTR_NAME_PAIR(vkDestroyOpticalFlowSessionNV),
  PTR_NAME_PAIR(vkBindOpticalFlowSessionImageNV),
  PTR_NAME_PAIR(vkCmdOpticalFlowExecuteNV),

  // extension: VK_KHR_maintenance5
#ifdef VXL_VK_DO_NOT_DEFINE
  PTR_NAME_PAIR(vkCmdBindIndexBuffer2KHR),
  PTR_NAME_PAIR(vkGetRenderingAreaGranularityKHR),
  PTR_NAME_PAIR(vkGetDeviceImageSubresourceLayoutKHR),
  PTR_NAME_PAIR(vkGetImageSubresourceLayout2KHR),
#endif // VXL_VK_DO_NOT_DEFINE

  // extension: VK_EXT_shader_object
#ifdef VXL_VK_DO_NOT_DEFINE
  PTR_NAME_PAIR(vkCreateShadersEXT),
  PTR_NAME_PAIR(vkDestroyShaderEXT),
  PTR_NAME_PAIR(vkGetShaderBinaryDataEXT),
  PTR_NAME_PAIR(vkCmdBindShadersEXT),
#endif // VXL_VK_DO_NOT_DEFINE

  // extension: VK_QCOM_tile_properties
  PTR_NAME_PAIR(vkGetFramebufferTilePropertiesQCOM),
  PTR_NAME_PAIR(vkGetDynamicRenderingTilePropertiesQCOM),

  // extension: VK_KHR_cooperative_matrix
#ifdef VXL_VK_DO_NOT_DEFINE
  PTR_NAME_PAIR(vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR),
#endif // VXL_VK_DO_NOT_DEFINE

  // extension: VK_EXT_attachment_feedback_loop_dynamic_state
#ifdef VXL_VK_DO_NOT_DEFINE
  PTR_NAME_PAIR(vkCmdSetAttachmentFeedbackLoopEnableEXT),
#endif // VXL_VK_DO_NOT_DEFINE

  // extension: VK_QNX_external_memory_screen_buffer
#ifdef VK_USE_PLATFORM_SCREEN_QNX
  PTR_NAME_PAIR(vkGetScreenBufferPropertiesQNX),
#endif // VK_USE_PLATFORM_SCREEN_QNX
};