#ifndef VXL_VK_GENERATED_DONT_INCLUDE_IDE_HELPERS
#    include <stuff/core.hpp>
#    include <stuff/intro/introspectors/function.hpp>
#    include <vulkan/vulkan_core.h>
#    include <expected>

#define TYPE_3_PRELUDE(_name, _n)                                                \
    using introspector_t = stf::intro::function_introspector<decltype(_name)>;   \
    using return_type = std::remove_pointer_t<introspector_t::nth_argument<_n>>; \
    auto ret = return_type {}

template<auto... SuccessValues>
static constexpr auto success(auto value) -> bool {
    return ((value == SuccessValues) || ...);
}

#endif

// vulkan 1.0
auto create_instance() const noexcept { return [this](const VkInstanceCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) noexcept { this->create_instance(create_info, allocator); }; }
auto create_instance(const VkInstanceCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) const noexcept -> std::expected<std::remove_pointer_t<VkInstance*>, VkResult> {
    TYPE_3_PRELUDE(vkCreateInstance, 2);
    const auto res = vkCreateInstance(create_info, allocator, &ret);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return ret;
}

auto destroy_instance() const noexcept { return [this](VkInstance instance, const VkAllocationCallbacks* allocator = nullptr) noexcept { return destroy_instance(instance, allocator); }; }
void destroy_instance(VkInstance instance, const VkAllocationCallbacks* allocator = nullptr) const noexcept { return vkDestroyInstance(instance, allocator); }

auto enumerate_physical_devices() const noexcept { return [this](VkInstance instance, u32* out_physical_device_count, VkPhysicalDevice* out_physical_devices) noexcept { return this->enumerate_physical_devices(instance, out_physical_device_count, out_physical_devices); }; }
auto enumerate_physical_devices(VkInstance instance, u32* out_physical_device_count, VkPhysicalDevice* out_physical_devices) const noexcept -> std::expected<VkResult, VkResult> {
    const auto res = vkEnumeratePhysicalDevices(instance, out_physical_device_count, out_physical_devices);
    if (!success<VK_SUCCESS, VK_INCOMPLETE>(res)) {
        return std::unexpected{res};
    }
    return res;
}

auto get_physical_device_properties() const noexcept { return [this](VkPhysicalDevice physical_device) noexcept { this->get_physical_device_properties(physical_device); }; }
auto get_physical_device_properties(VkPhysicalDevice physical_device) const noexcept -> std::remove_pointer_t<VkPhysicalDeviceProperties*> {
    TYPE_3_PRELUDE(vkGetPhysicalDeviceProperties, 1);
    vkGetPhysicalDeviceProperties(physical_device, &ret);
    return ret;
}

auto get_physical_device_queue_family_properties() const noexcept { return [this](VkPhysicalDevice physical_device, u32* out_property_count, VkQueueFamilyProperties* out_properties) noexcept { return get_physical_device_queue_family_properties(physical_device, out_property_count, out_properties); }; }
void get_physical_device_queue_family_properties(VkPhysicalDevice physical_device, u32* out_property_count, VkQueueFamilyProperties* out_properties) const noexcept { return vkGetPhysicalDeviceQueueFamilyProperties(physical_device, out_property_count, out_properties); }

auto create_device() const noexcept { return [this](VkPhysicalDevice physical_device, const VkDeviceCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) noexcept { this->create_device(physical_device, create_info, allocator); }; }
auto create_device(VkPhysicalDevice physical_device, const VkDeviceCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) const noexcept -> std::expected<std::remove_pointer_t<VkDevice*>, VkResult> {
    TYPE_3_PRELUDE(vkCreateDevice, 3);
    const auto res = vkCreateDevice(physical_device, create_info, allocator, &ret);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return ret;
}

auto destroy_device() const noexcept { return [this](VkDevice device, const VkAllocationCallbacks* allocator = nullptr) noexcept { return destroy_device(device, allocator); }; }
void destroy_device(VkDevice device, const VkAllocationCallbacks* allocator = nullptr) const noexcept { return vkDestroyDevice(device, allocator); }

auto enumerate_instance_extension_properties() const noexcept { return [this](const char* layer_name, u32* out_property_count, VkExtensionProperties* out_properties) noexcept { return this->enumerate_instance_extension_properties(layer_name, out_property_count, out_properties); }; }
auto enumerate_instance_extension_properties(const char* layer_name, u32* out_property_count, VkExtensionProperties* out_properties) const noexcept -> std::expected<VkResult, VkResult> {
    const auto res = vkEnumerateInstanceExtensionProperties(layer_name, out_property_count, out_properties);
    if (!success<VK_SUCCESS, VK_INCOMPLETE>(res)) {
        return std::unexpected{res};
    }
    return res;
}

auto enumerate_device_extension_properties() const noexcept { return [this](VkPhysicalDevice device, const char* layer_name, u32* out_property_count, VkExtensionProperties* out_properties) noexcept { return this->enumerate_device_extension_properties(device, layer_name, out_property_count, out_properties); }; }
auto enumerate_device_extension_properties(VkPhysicalDevice device, const char* layer_name, u32* out_property_count, VkExtensionProperties* out_properties) const noexcept -> std::expected<VkResult, VkResult> {
    const auto res = vkEnumerateDeviceExtensionProperties(device, layer_name, out_property_count, out_properties);
    if (!success<VK_SUCCESS, VK_INCOMPLETE>(res)) {
        return std::unexpected{res};
    }
    return res;
}

auto enumerate_instance_layer_properties() const noexcept { return [this](u32* out_property_count, VkLayerProperties* out_properties) noexcept { return this->enumerate_instance_layer_properties(out_property_count, out_properties); }; }
auto enumerate_instance_layer_properties(u32* out_property_count, VkLayerProperties* out_properties) const noexcept -> std::expected<VkResult, VkResult> {
    const auto res = vkEnumerateInstanceLayerProperties(out_property_count, out_properties);
    if (!success<VK_SUCCESS, VK_INCOMPLETE>(res)) {
        return std::unexpected{res};
    }
    return res;
}

auto enumerate_device_layer_properties() const noexcept { return [this](VkPhysicalDevice device, u32* out_property_count, VkLayerProperties* out_properties) noexcept { return this->enumerate_device_layer_properties(device, out_property_count, out_properties); }; }
auto enumerate_device_layer_properties(VkPhysicalDevice device, u32* out_property_count, VkLayerProperties* out_properties) const noexcept -> std::expected<VkResult, VkResult> {
    const auto res = vkEnumerateDeviceLayerProperties(device, out_property_count, out_properties);
    if (!success<VK_SUCCESS, VK_INCOMPLETE>(res)) {
        return std::unexpected{res};
    }
    return res;
}

auto get_device_queue() const noexcept { return [this](VkDevice device, u32 queue_family_index, u32 queue_index) noexcept { this->get_device_queue(device, queue_family_index, queue_index); }; }
auto get_device_queue(VkDevice device, u32 queue_family_index, u32 queue_index) const noexcept -> std::remove_pointer_t<VkQueue*> {
    TYPE_3_PRELUDE(vkGetDeviceQueue, 3);
    vkGetDeviceQueue(device, queue_family_index, queue_index, &ret);
    return ret;
}


// vulkan 1.1

// vulkan 1.2

// vulkan 1.3

// extension: VK_KHR_surface
auto destroy_surface_khr() const noexcept { return [this](VkInstance instance, VkSurfaceKHR surface, const VkAllocationCallbacks* allocator = nullptr) noexcept { return destroy_surface_khr(instance, surface, allocator); }; }
void destroy_surface_khr(VkInstance instance, VkSurfaceKHR surface, const VkAllocationCallbacks* allocator = nullptr) const noexcept { return vkDestroySurfaceKHR(instance, surface, allocator); }

auto get_physical_device_surface_support_khr() const noexcept { return [this](VkPhysicalDevice physical_device, u32 queue_family_index, VkSurfaceKHR surface) noexcept { this->get_physical_device_surface_support_khr(physical_device, queue_family_index, surface); }; }
auto get_physical_device_surface_support_khr(VkPhysicalDevice physical_device, u32 queue_family_index, VkSurfaceKHR surface) const noexcept -> std::expected<std::remove_pointer_t<VkBool32*>, VkResult> {
    TYPE_3_PRELUDE(vkGetPhysicalDeviceSurfaceSupportKHR, 3);
    const auto res = vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, queue_family_index, surface, &ret);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return ret;
}


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
auto create_debug_utils_messenger_ext() const noexcept { return [this](VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator = nullptr) noexcept { this->create_debug_utils_messenger_ext(instance, create_info, allocator); }; }
auto create_debug_utils_messenger_ext(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator = nullptr) const noexcept -> std::expected<std::remove_pointer_t<VkDebugUtilsMessengerEXT*>, VkResult> {
    TYPE_3_PRELUDE(vkCreateDebugUtilsMessengerEXT, 3);
    const auto res = vkCreateDebugUtilsMessengerEXT(instance, create_info, allocator, &ret);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return ret;
}

auto destroy_debug_utils_messenger_ext() const noexcept { return [this](VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* allocator = nullptr) noexcept { return destroy_debug_utils_messenger_ext(instance, messenger, allocator); }; }
void destroy_debug_utils_messenger_ext(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* allocator = nullptr) const noexcept { return vkDestroyDebugUtilsMessengerEXT(instance, messenger, allocator); }


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
