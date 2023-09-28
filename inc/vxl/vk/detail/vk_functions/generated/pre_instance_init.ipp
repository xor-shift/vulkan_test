static const std::pair<void (*vulkan_functions::*)(), const char*> functions_and_names[] {
  // vulkan 1.0
  PTR_NAME_PAIR(vkCreateInstance),
  PTR_NAME_PAIR(vkEnumerateInstanceExtensionProperties),
  PTR_NAME_PAIR(vkEnumerateInstanceLayerProperties),

  // vulkan 1.1
  PTR_NAME_PAIR(vkEnumerateInstanceVersion),

  // vulkan 1.2

  // vulkan 1.3

  // extension: VK_KHR_surface

  // extension: VK_KHR_swapchain

  // extension: VK_KHR_display

  // extension: VK_KHR_display_swapchain

  // extension: VK_KHR_external_memory_fd

  // extension: VK_KHR_external_semaphore_fd

  // extension: VK_EXT_direct_mode_display

  // extension: VK_EXT_display_surface_counter

  // extension: VK_EXT_display_control

  // extension: VK_EXT_discard_rectangles

  // extension: VK_EXT_hdr_metadata

  // extension: VK_KHR_shared_presentable_image

  // extension: VK_KHR_external_fence_fd

  // extension: VK_KHR_performance_query

  // extension: VK_KHR_get_surface_capabilities2

  // extension: VK_KHR_get_display_properties2

  // extension: VK_EXT_debug_utils

  // extension: VK_EXT_sample_locations

  // extension: VK_EXT_image_drm_format_modifier

  // extension: VK_EXT_external_memory_host

  // extension: VK_EXT_calibrated_timestamps

  // extension: VK_KHR_fragment_shading_rate

  // extension: VK_EXT_headless_surface

  // extension: VK_EXT_line_rasterization

  // extension: VK_EXT_extended_dynamic_state

  // extension: VK_KHR_synchronization2

  // extension: VK_KHR_copy_commands2

  // extension: VK_EXT_vertex_input_dynamic_state

  // extension: VK_EXT_extended_dynamic_state2

  // extension: VK_EXT_color_write_enable
};