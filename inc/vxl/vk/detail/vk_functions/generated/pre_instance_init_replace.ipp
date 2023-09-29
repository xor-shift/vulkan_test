static const std::pair<void (*vulkan_functions::*)(), void (*vulkan_functions::*)()> replacement_pairs[] {
  // vulkan version 1.0

  // vulkan version 1.1

  // vulkan version 1.2

  // vulkan version 1.3

  // extension: VK_KHR_surface

  // extension: VK_KHR_swapchain

  // extension: VK_KHR_display

  // extension: VK_KHR_display_swapchain

  // extension: VK_KHR_xlib_surface
#ifdef VK_USE_PLATFORM_XLIB_KHR
#endif // VK_USE_PLATFORM_XLIB_KHR

  // extension: VK_KHR_xcb_surface
#ifdef VK_USE_PLATFORM_XCB_KHR
#endif // VK_USE_PLATFORM_XCB_KHR

  // extension: VK_KHR_wayland_surface
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
#endif // VK_USE_PLATFORM_WAYLAND_KHR

  // extension: VK_KHR_android_surface
#ifdef VK_USE_PLATFORM_ANDROID_KHR
#endif // VK_USE_PLATFORM_ANDROID_KHR

  // extension: VK_KHR_win32_surface
#ifdef VK_USE_PLATFORM_WIN32_KHR
#endif // VK_USE_PLATFORM_WIN32_KHR

  // extension: VK_EXT_debug_report

  // extension: VK_EXT_debug_marker

  // extension: VK_KHR_video_queue

  // extension: VK_KHR_video_decode_queue

  // extension: VK_EXT_transform_feedback

  // extension: VK_NVX_binary_import

  // extension: VK_NVX_image_view_handle

  // extension: VK_AMD_draw_indirect_count

  // extension: VK_AMD_shader_info

  // extension: VK_KHR_dynamic_rendering

  // extension: VK_GGP_stream_descriptor_surface
#ifdef VK_USE_PLATFORM_GGP
#endif // VK_USE_PLATFORM_GGP

  // extension: VK_NV_external_memory_capabilities

  // extension: VK_NV_external_memory_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
#endif // VK_USE_PLATFORM_WIN32_KHR

  // extension: VK_KHR_get_physical_device_properties2

  // extension: VK_KHR_device_group

  // extension: VK_NN_vi_surface
#ifdef VK_USE_PLATFORM_VI_NN
#endif // VK_USE_PLATFORM_VI_NN

  // extension: VK_KHR_maintenance1

  // extension: VK_KHR_device_group_creation

  // extension: VK_KHR_external_memory_capabilities

  // extension: VK_KHR_external_memory_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
#endif // VK_USE_PLATFORM_WIN32_KHR

  // extension: VK_KHR_external_memory_fd

  // extension: VK_KHR_external_semaphore_capabilities

  // extension: VK_KHR_external_semaphore_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
#endif // VK_USE_PLATFORM_WIN32_KHR

  // extension: VK_KHR_external_semaphore_fd

  // extension: VK_KHR_push_descriptor

  // extension: VK_EXT_conditional_rendering

  // extension: VK_KHR_descriptor_update_template

  // extension: VK_NV_clip_space_w_scaling

  // extension: VK_EXT_direct_mode_display

  // extension: VK_EXT_acquire_xlib_display
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
#endif // VK_USE_PLATFORM_XLIB_XRANDR_EXT

  // extension: VK_EXT_display_surface_counter

  // extension: VK_EXT_display_control

  // extension: VK_GOOGLE_display_timing

  // extension: VK_EXT_discard_rectangles

  // extension: VK_EXT_hdr_metadata

  // extension: VK_KHR_create_renderpass2

  // extension: VK_KHR_shared_presentable_image

  // extension: VK_KHR_external_fence_capabilities

  // extension: VK_KHR_external_fence_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
#endif // VK_USE_PLATFORM_WIN32_KHR

  // extension: VK_KHR_external_fence_fd

  // extension: VK_KHR_performance_query

  // extension: VK_KHR_get_surface_capabilities2

  // extension: VK_KHR_get_display_properties2

  // extension: VK_MVK_ios_surface
#ifdef VK_USE_PLATFORM_IOS_MVK
#endif // VK_USE_PLATFORM_IOS_MVK

  // extension: VK_MVK_macos_surface
#ifdef VK_USE_PLATFORM_MACOS_MVK
#endif // VK_USE_PLATFORM_MACOS_MVK

  // extension: VK_EXT_debug_utils

  // extension: VK_ANDROID_external_memory_android_hardware_buffer
#ifdef VK_USE_PLATFORM_ANDROID_KHR
#endif // VK_USE_PLATFORM_ANDROID_KHR

  // extension: VK_AMDX_shader_enqueue
#ifdef VK_ENABLE_BETA_EXTENSIONS
#endif // VK_ENABLE_BETA_EXTENSIONS

  // extension: VK_EXT_sample_locations

  // extension: VK_KHR_get_memory_requirements2

  // extension: VK_KHR_acceleration_structure

  // extension: VK_KHR_ray_tracing_pipeline

  // extension: VK_KHR_sampler_ycbcr_conversion

  // extension: VK_KHR_bind_memory2

  // extension: VK_EXT_image_drm_format_modifier

  // extension: VK_EXT_validation_cache

  // extension: VK_NV_shading_rate_image

  // extension: VK_NV_ray_tracing

  // extension: VK_KHR_maintenance3

  // extension: VK_KHR_draw_indirect_count

  // extension: VK_EXT_external_memory_host

  // extension: VK_AMD_buffer_marker

  // extension: VK_EXT_calibrated_timestamps

  // extension: VK_NV_mesh_shader

  // extension: VK_NV_scissor_exclusive

  // extension: VK_NV_device_diagnostic_checkpoints

  // extension: VK_KHR_timeline_semaphore

  // extension: VK_INTEL_performance_query

  // extension: VK_AMD_display_native_hdr

  // extension: VK_FUCHSIA_imagepipe_surface
#ifdef VK_USE_PLATFORM_FUCHSIA
#endif // VK_USE_PLATFORM_FUCHSIA

  // extension: VK_EXT_metal_surface
#ifdef VK_USE_PLATFORM_METAL_EXT
#endif // VK_USE_PLATFORM_METAL_EXT

  // extension: VK_KHR_fragment_shading_rate

  // extension: VK_EXT_buffer_device_address

  // extension: VK_EXT_tooling_info

  // extension: VK_KHR_present_wait

  // extension: VK_NV_cooperative_matrix

  // extension: VK_NV_coverage_reduction_mode

  // extension: VK_EXT_full_screen_exclusive
#ifdef VK_USE_PLATFORM_WIN32_KHR
#endif // VK_USE_PLATFORM_WIN32_KHR

  // extension: VK_EXT_headless_surface

  // extension: VK_KHR_buffer_device_address

  // extension: VK_EXT_line_rasterization

  // extension: VK_EXT_host_query_reset

  // extension: VK_EXT_extended_dynamic_state

  // extension: VK_KHR_deferred_host_operations

  // extension: VK_KHR_pipeline_executable_properties

  // extension: VK_EXT_host_image_copy
#ifdef VXL_VK_DO_NOT_DEFINE
#endif // VXL_VK_DO_NOT_DEFINE

  // extension: VK_KHR_map_memory2
#ifdef VXL_VK_DO_NOT_DEFINE
#endif // VXL_VK_DO_NOT_DEFINE

  // extension: VK_EXT_swapchain_maintenance1

  // extension: VK_NV_device_generated_commands

  // extension: VK_EXT_depth_bias_control
#ifdef VXL_VK_DO_NOT_DEFINE
#endif // VXL_VK_DO_NOT_DEFINE

  // extension: VK_EXT_acquire_drm_display

  // extension: VK_EXT_private_data

  // extension: VK_KHR_video_encode_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
#endif // VK_ENABLE_BETA_EXTENSIONS

  // extension: VK_EXT_metal_objects
#ifdef VK_USE_PLATFORM_METAL_EXT
#endif // VK_USE_PLATFORM_METAL_EXT

  // extension: VK_KHR_synchronization2

  // extension: VK_EXT_descriptor_buffer

  // extension: VK_NV_fragment_shading_rate_enums

  // extension: VK_EXT_mesh_shader

  // extension: VK_KHR_copy_commands2

  // extension: VK_EXT_device_fault

  // extension: VK_NV_acquire_winrt_display
#ifdef VK_USE_PLATFORM_WIN32_KHR
#endif // VK_USE_PLATFORM_WIN32_KHR

  // extension: VK_EXT_directfb_surface
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
#endif // VK_USE_PLATFORM_DIRECTFB_EXT

  // extension: VK_EXT_vertex_input_dynamic_state

  // extension: VK_FUCHSIA_external_memory
#ifdef VK_USE_PLATFORM_FUCHSIA
#endif // VK_USE_PLATFORM_FUCHSIA

  // extension: VK_FUCHSIA_external_semaphore
#ifdef VK_USE_PLATFORM_FUCHSIA
#endif // VK_USE_PLATFORM_FUCHSIA

  // extension: VK_FUCHSIA_buffer_collection
#ifdef VK_USE_PLATFORM_FUCHSIA
#endif // VK_USE_PLATFORM_FUCHSIA

  // extension: VK_HUAWEI_subpass_shading

  // extension: VK_HUAWEI_invocation_mask

  // extension: VK_NV_external_memory_rdma

  // extension: VK_EXT_pipeline_properties

  // extension: VK_EXT_extended_dynamic_state2

  // extension: VK_QNX_screen_surface
#ifdef VK_USE_PLATFORM_SCREEN_QNX
#endif // VK_USE_PLATFORM_SCREEN_QNX

  // extension: VK_EXT_color_write_enable

  // extension: VK_KHR_ray_tracing_maintenance1

  // extension: VK_EXT_multi_draw

  // extension: VK_EXT_opacity_micromap

  // extension: VK_HUAWEI_cluster_culling_shader

  // extension: VK_EXT_pageable_device_local_memory

  // extension: VK_KHR_maintenance4

  // extension: VK_VALVE_descriptor_set_host_mapping

  // extension: VK_NV_copy_memory_indirect

  // extension: VK_NV_memory_decompression

  // extension: VK_NV_device_generated_commands_compute
#ifdef VXL_VK_DO_NOT_DEFINE
#endif // VXL_VK_DO_NOT_DEFINE

  // extension: VK_EXT_extended_dynamic_state3

  // extension: VK_EXT_shader_module_identifier

  // extension: VK_NV_optical_flow

  // extension: VK_KHR_maintenance5
#ifdef VXL_VK_DO_NOT_DEFINE
#endif // VXL_VK_DO_NOT_DEFINE

  // extension: VK_EXT_shader_object
#ifdef VXL_VK_DO_NOT_DEFINE
#endif // VXL_VK_DO_NOT_DEFINE

  // extension: VK_QCOM_tile_properties

  // extension: VK_KHR_cooperative_matrix
#ifdef VXL_VK_DO_NOT_DEFINE
#endif // VXL_VK_DO_NOT_DEFINE

  // extension: VK_EXT_attachment_feedback_loop_dynamic_state
#ifdef VXL_VK_DO_NOT_DEFINE
#endif // VXL_VK_DO_NOT_DEFINE

  // extension: VK_QNX_external_memory_screen_buffer
#ifdef VK_USE_PLATFORM_SCREEN_QNX
#endif // VK_USE_PLATFORM_SCREEN_QNX
};