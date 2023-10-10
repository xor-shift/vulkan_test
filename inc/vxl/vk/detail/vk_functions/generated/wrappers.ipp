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

// vulkan version 1.0
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

auto queue_submit() const noexcept { return [this](VkQueue queue, u32 submit_count, const VkSubmitInfo* submits, VkFence fence) noexcept { return this->queue_submit(queue, submit_count, submits, fence); }; }
auto queue_submit(VkQueue queue, u32 submit_count, const VkSubmitInfo* submits, VkFence fence) const noexcept -> std::expected<void, VkResult> {
    const auto res = vkQueueSubmit(queue, submit_count, submits, fence);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return {};
}

auto device_wait_idle() const noexcept { return [this](VkDevice device) noexcept { return this->device_wait_idle(device); }; }
auto device_wait_idle(VkDevice device) const noexcept -> std::expected<void, VkResult> {
    const auto res = vkDeviceWaitIdle(device);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return {};
}

auto create_fence() const noexcept { return [this](VkDevice device, const VkFenceCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) noexcept { this->create_fence(device, create_info, allocator); }; }
auto create_fence(VkDevice device, const VkFenceCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) const noexcept -> std::expected<std::remove_pointer_t<VkFence*>, VkResult> {
    TYPE_3_PRELUDE(vkCreateFence, 3);
    const auto res = vkCreateFence(device, create_info, allocator, &ret);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return ret;
}

auto destroy_fence() const noexcept { return [this](VkDevice device, VkFence fence, const VkAllocationCallbacks* allocator = nullptr) noexcept { return destroy_fence(device, fence, allocator); }; }
void destroy_fence(VkDevice device, VkFence fence, const VkAllocationCallbacks* allocator = nullptr) const noexcept { return vkDestroyFence(device, fence, allocator); }

auto reset_fences() const noexcept { return [this](VkDevice device, u32 fence_count, const VkFence* fences) noexcept { return this->reset_fences(device, fence_count, fences); }; }
auto reset_fences(VkDevice device, u32 fence_count, const VkFence* fences) const noexcept -> std::expected<void, VkResult> {
    const auto res = vkResetFences(device, fence_count, fences);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return {};
}

auto wait_for_fences() const noexcept { return [this](VkDevice device, u32 fence_count, const VkFence* fences, VkBool32 wait_all, u64 timeout_ns) noexcept { return this->wait_for_fences(device, fence_count, fences, wait_all, timeout_ns); }; }
auto wait_for_fences(VkDevice device, u32 fence_count, const VkFence* fences, VkBool32 wait_all, u64 timeout_ns) const noexcept -> std::expected<VkResult, VkResult> {
    const auto res = vkWaitForFences(device, fence_count, fences, wait_all, timeout_ns);
    if (!success<VK_SUCCESS, VK_TIMEOUT>(res)) {
        return std::unexpected{res};
    }
    return res;
}

auto create_semaphore() const noexcept { return [this](VkDevice device, const VkSemaphoreCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) noexcept { this->create_semaphore(device, create_info, allocator); }; }
auto create_semaphore(VkDevice device, const VkSemaphoreCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) const noexcept -> std::expected<std::remove_pointer_t<VkSemaphore*>, VkResult> {
    TYPE_3_PRELUDE(vkCreateSemaphore, 3);
    const auto res = vkCreateSemaphore(device, create_info, allocator, &ret);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return ret;
}

auto destroy_semaphore() const noexcept { return [this](VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks* allocator = nullptr) noexcept { return destroy_semaphore(device, semaphore, allocator); }; }
void destroy_semaphore(VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks* allocator = nullptr) const noexcept { return vkDestroySemaphore(device, semaphore, allocator); }

auto create_image_view() const noexcept { return [this](VkDevice device, const VkImageViewCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) noexcept { this->create_image_view(device, create_info, allocator); }; }
auto create_image_view(VkDevice device, const VkImageViewCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) const noexcept -> std::expected<std::remove_pointer_t<VkImageView*>, VkResult> {
    TYPE_3_PRELUDE(vkCreateImageView, 3);
    const auto res = vkCreateImageView(device, create_info, allocator, &ret);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return ret;
}

auto destroy_image_view() const noexcept { return [this](VkDevice device, VkImageView image_view, const VkAllocationCallbacks* allocator = nullptr) noexcept { return destroy_image_view(device, image_view, allocator); }; }
void destroy_image_view(VkDevice device, VkImageView image_view, const VkAllocationCallbacks* allocator = nullptr) const noexcept { return vkDestroyImageView(device, image_view, allocator); }

auto create_shader_module() const noexcept { return [this](VkDevice device, const VkShaderModuleCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) noexcept { this->create_shader_module(device, create_info, allocator); }; }
auto create_shader_module(VkDevice device, const VkShaderModuleCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) const noexcept -> std::expected<std::remove_pointer_t<VkShaderModule*>, VkResult> {
    TYPE_3_PRELUDE(vkCreateShaderModule, 3);
    const auto res = vkCreateShaderModule(device, create_info, allocator, &ret);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return ret;
}

auto destroy_shader_module() const noexcept { return [this](VkDevice device, VkShaderModule shader, const VkAllocationCallbacks* allocator = nullptr) noexcept { return destroy_shader_module(device, shader, allocator); }; }
void destroy_shader_module(VkDevice device, VkShaderModule shader, const VkAllocationCallbacks* allocator = nullptr) const noexcept { return vkDestroyShaderModule(device, shader, allocator); }

auto destroy_pipeline() const noexcept { return [this](VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks* allocator = nullptr) noexcept { return destroy_pipeline(device, pipeline, allocator); }; }
void destroy_pipeline(VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks* allocator = nullptr) const noexcept { return vkDestroyPipeline(device, pipeline, allocator); }

auto create_pipeline_layout() const noexcept { return [this](VkDevice device, const VkPipelineLayoutCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) noexcept { this->create_pipeline_layout(device, create_info, allocator); }; }
auto create_pipeline_layout(VkDevice device, const VkPipelineLayoutCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) const noexcept -> std::expected<std::remove_pointer_t<VkPipelineLayout*>, VkResult> {
    TYPE_3_PRELUDE(vkCreatePipelineLayout, 3);
    const auto res = vkCreatePipelineLayout(device, create_info, allocator, &ret);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return ret;
}

auto destroy_pipeline_layout() const noexcept { return [this](VkDevice device, VkPipelineLayout pipeline, const VkAllocationCallbacks* allocator = nullptr) noexcept { return destroy_pipeline_layout(device, pipeline, allocator); }; }
void destroy_pipeline_layout(VkDevice device, VkPipelineLayout pipeline, const VkAllocationCallbacks* allocator = nullptr) const noexcept { return vkDestroyPipelineLayout(device, pipeline, allocator); }

auto create_framebuffer() const noexcept { return [this](VkDevice device, const VkFramebufferCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) noexcept { this->create_framebuffer(device, create_info, allocator); }; }
auto create_framebuffer(VkDevice device, const VkFramebufferCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) const noexcept -> std::expected<std::remove_pointer_t<VkFramebuffer*>, VkResult> {
    TYPE_3_PRELUDE(vkCreateFramebuffer, 3);
    const auto res = vkCreateFramebuffer(device, create_info, allocator, &ret);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return ret;
}

auto destroy_framebuffer() const noexcept { return [this](VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* allocator = nullptr) noexcept { return destroy_framebuffer(device, framebuffer, allocator); }; }
void destroy_framebuffer(VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* allocator = nullptr) const noexcept { return vkDestroyFramebuffer(device, framebuffer, allocator); }

auto create_render_pass() const noexcept { return [this](VkDevice device, const VkRenderPassCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) noexcept { this->create_render_pass(device, create_info, allocator); }; }
auto create_render_pass(VkDevice device, const VkRenderPassCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) const noexcept -> std::expected<std::remove_pointer_t<VkRenderPass*>, VkResult> {
    TYPE_3_PRELUDE(vkCreateRenderPass, 3);
    const auto res = vkCreateRenderPass(device, create_info, allocator, &ret);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return ret;
}

auto destroy_render_pass() const noexcept { return [this](VkDevice device, VkRenderPass render_pass, const VkAllocationCallbacks* allocator = nullptr) noexcept { return destroy_render_pass(device, render_pass, allocator); }; }
void destroy_render_pass(VkDevice device, VkRenderPass render_pass, const VkAllocationCallbacks* allocator = nullptr) const noexcept { return vkDestroyRenderPass(device, render_pass, allocator); }

auto create_command_pool() const noexcept { return [this](VkDevice device, const VkCommandPoolCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) noexcept { this->create_command_pool(device, create_info, allocator); }; }
auto create_command_pool(VkDevice device, const VkCommandPoolCreateInfo* create_info, const VkAllocationCallbacks* allocator = nullptr) const noexcept -> std::expected<std::remove_pointer_t<VkCommandPool*>, VkResult> {
    TYPE_3_PRELUDE(vkCreateCommandPool, 3);
    const auto res = vkCreateCommandPool(device, create_info, allocator, &ret);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return ret;
}

auto destroy_command_pool() const noexcept { return [this](VkDevice device, VkCommandPool command_pool, const VkAllocationCallbacks* allocator = nullptr) noexcept { return destroy_command_pool(device, command_pool, allocator); }; }
void destroy_command_pool(VkDevice device, VkCommandPool command_pool, const VkAllocationCallbacks* allocator = nullptr) const noexcept { return vkDestroyCommandPool(device, command_pool, allocator); }

auto allocate_command_buffers() const noexcept { return [this](VkDevice device, const VkCommandBufferAllocateInfo* allocate_info, VkCommandBuffer* out_command_buffers) noexcept { return this->allocate_command_buffers(device, allocate_info, out_command_buffers); }; }
auto allocate_command_buffers(VkDevice device, const VkCommandBufferAllocateInfo* allocate_info, VkCommandBuffer* out_command_buffers) const noexcept -> std::expected<void, VkResult> {
    const auto res = vkAllocateCommandBuffers(device, allocate_info, out_command_buffers);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return {};
}

auto free_command_buffers() const noexcept { return [this](VkDevice device, VkCommandPool command_pool, u32 command_buffer_count, const VkCommandBuffer* command_buffers) noexcept { return free_command_buffers(device, command_pool, command_buffer_count, command_buffers); }; }
void free_command_buffers(VkDevice device, VkCommandPool command_pool, u32 command_buffer_count, const VkCommandBuffer* command_buffers) const noexcept { return vkFreeCommandBuffers(device, command_pool, command_buffer_count, command_buffers); }

auto begin_command_buffer() const noexcept { return [this](VkCommandBuffer buffer, const VkCommandBufferBeginInfo* begin_info) noexcept { return this->begin_command_buffer(buffer, begin_info); }; }
auto begin_command_buffer(VkCommandBuffer buffer, const VkCommandBufferBeginInfo* begin_info) const noexcept -> std::expected<void, VkResult> {
    const auto res = vkBeginCommandBuffer(buffer, begin_info);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return {};
}

auto end_command_buffer() const noexcept { return [this](VkCommandBuffer buffer) noexcept { return this->end_command_buffer(buffer); }; }
auto end_command_buffer(VkCommandBuffer buffer) const noexcept -> std::expected<void, VkResult> {
    const auto res = vkEndCommandBuffer(buffer);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return {};
}

auto reset_command_buffer() const noexcept { return [this](VkCommandBuffer buffer, VkCommandBufferResetFlags flags = 0) noexcept { return this->reset_command_buffer(buffer, flags); }; }
auto reset_command_buffer(VkCommandBuffer buffer, VkCommandBufferResetFlags flags = 0) const noexcept -> std::expected<void, VkResult> {
    const auto res = vkResetCommandBuffer(buffer, flags);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return {};
}

auto cmd_bind_pipeline() const noexcept { return [this](VkCommandBuffer buffer, VkPipelineBindPoint bind_point, VkPipeline pipeline) noexcept { return cmd_bind_pipeline(buffer, bind_point, pipeline); }; }
void cmd_bind_pipeline(VkCommandBuffer buffer, VkPipelineBindPoint bind_point, VkPipeline pipeline) const noexcept { return vkCmdBindPipeline(buffer, bind_point, pipeline); }

auto cmd_draw() const noexcept { return [this](VkCommandBuffer command_buffer, u32 vertex_count, u32 instance_count, u32 first_vertex, u32 first_instance) noexcept { return cmd_draw(command_buffer, vertex_count, instance_count, first_vertex, first_instance); }; }
void cmd_draw(VkCommandBuffer command_buffer, u32 vertex_count, u32 instance_count, u32 first_vertex, u32 first_instance) const noexcept { return vkCmdDraw(command_buffer, vertex_count, instance_count, first_vertex, first_instance); }

auto cmd_draw_indexed() const noexcept { return [this](VkCommandBuffer command_buffer, u32 index_count, u32 instance_count, u32 first_index, i32 vertex_offset, u32 first_instance) noexcept { return cmd_draw_indexed(command_buffer, index_count, instance_count, first_index, vertex_offset, first_instance); }; }
void cmd_draw_indexed(VkCommandBuffer command_buffer, u32 index_count, u32 instance_count, u32 first_index, i32 vertex_offset, u32 first_instance) const noexcept { return vkCmdDrawIndexed(command_buffer, index_count, instance_count, first_index, vertex_offset, first_instance); }

auto cmd_draw_indirect() const noexcept { return [this](VkCommandBuffer command_buffer, VkBuffer buffer, VkDeviceSize offset, u32 draw_count, u32 stride) noexcept { return cmd_draw_indirect(command_buffer, buffer, offset, draw_count, stride); }; }
void cmd_draw_indirect(VkCommandBuffer command_buffer, VkBuffer buffer, VkDeviceSize offset, u32 draw_count, u32 stride) const noexcept { return vkCmdDrawIndirect(command_buffer, buffer, offset, draw_count, stride); }

auto cmd_draw_indexed_indirect() const noexcept { return [this](VkCommandBuffer command_buffer, VkBuffer buffer, VkDeviceSize offset, u32 draw_count, u32 stride) noexcept { return cmd_draw_indexed_indirect(command_buffer, buffer, offset, draw_count, stride); }; }
void cmd_draw_indexed_indirect(VkCommandBuffer command_buffer, VkBuffer buffer, VkDeviceSize offset, u32 draw_count, u32 stride) const noexcept { return vkCmdDrawIndexedIndirect(command_buffer, buffer, offset, draw_count, stride); }

auto cmd_push_constants() const noexcept { return [this](VkCommandBuffer command_buffer, VkPipelineLayout pipeline_layout, VkShaderStageFlags shader_stage, u32 offset, u32 size, void* push_values) noexcept { return cmd_push_constants(command_buffer, pipeline_layout, shader_stage, offset, size, push_values); }; }
void cmd_push_constants(VkCommandBuffer command_buffer, VkPipelineLayout pipeline_layout, VkShaderStageFlags shader_stage, u32 offset, u32 size, void* push_values) const noexcept { return vkCmdPushConstants(command_buffer, pipeline_layout, shader_stage, offset, size, push_values); }

auto cmd_begin_render_pass() const noexcept { return [this](VkCommandBuffer command_buffer, const VkRenderPassBeginInfo* begin_info, VkSubpassContents contents) noexcept { return cmd_begin_render_pass(command_buffer, begin_info, contents); }; }
void cmd_begin_render_pass(VkCommandBuffer command_buffer, const VkRenderPassBeginInfo* begin_info, VkSubpassContents contents) const noexcept { return vkCmdBeginRenderPass(command_buffer, begin_info, contents); }

auto cmd_end_render_pass() const noexcept { return [this](VkCommandBuffer command_buffer) noexcept { return cmd_end_render_pass(command_buffer); }; }
void cmd_end_render_pass(VkCommandBuffer command_buffer) const noexcept { return vkCmdEndRenderPass(command_buffer); }


// vulkan version 1.1

// vulkan version 1.2

// vulkan version 1.3

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

auto get_physical_device_surface_capabilities_khr() const noexcept { return [this](VkPhysicalDevice physical_device, VkSurfaceKHR surface) noexcept { this->get_physical_device_surface_capabilities_khr(physical_device, surface); }; }
auto get_physical_device_surface_capabilities_khr(VkPhysicalDevice physical_device, VkSurfaceKHR surface) const noexcept -> std::expected<std::remove_pointer_t<VkSurfaceCapabilitiesKHR*>, VkResult> {
    TYPE_3_PRELUDE(vkGetPhysicalDeviceSurfaceCapabilitiesKHR, 2);
    const auto res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &ret);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return ret;
}

auto get_physical_device_surface_formats_khr() const noexcept { return [this](VkPhysicalDevice device, VkSurfaceKHR surface, u32* out_format_count, VkSurfaceFormatKHR* out_formats) noexcept { return this->get_physical_device_surface_formats_khr(device, surface, out_format_count, out_formats); }; }
auto get_physical_device_surface_formats_khr(VkPhysicalDevice device, VkSurfaceKHR surface, u32* out_format_count, VkSurfaceFormatKHR* out_formats) const noexcept -> std::expected<VkResult, VkResult> {
    const auto res = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, out_format_count, out_formats);
    if (!success<VK_SUCCESS, VK_INCOMPLETE>(res)) {
        return std::unexpected{res};
    }
    return res;
}

auto get_physical_device_surface_present_modes_khr() const noexcept { return [this](VkPhysicalDevice device, VkSurfaceKHR surface, u32* out_mode_count, VkPresentModeKHR* out_modes) noexcept { return this->get_physical_device_surface_present_modes_khr(device, surface, out_mode_count, out_modes); }; }
auto get_physical_device_surface_present_modes_khr(VkPhysicalDevice device, VkSurfaceKHR surface, u32* out_mode_count, VkPresentModeKHR* out_modes) const noexcept -> std::expected<VkResult, VkResult> {
    const auto res = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, out_mode_count, out_modes);
    if (!success<VK_SUCCESS, VK_INCOMPLETE>(res)) {
        return std::unexpected{res};
    }
    return res;
}


// extension: VK_KHR_swapchain
auto create_swapchain_khr() const noexcept { return [this](VkDevice device, const VkSwapchainCreateInfoKHR* create_info, const VkAllocationCallbacks* allocator = nullptr) noexcept { this->create_swapchain_khr(device, create_info, allocator); }; }
auto create_swapchain_khr(VkDevice device, const VkSwapchainCreateInfoKHR* create_info, const VkAllocationCallbacks* allocator = nullptr) const noexcept -> std::expected<std::remove_pointer_t<VkSwapchainKHR*>, VkResult> {
    TYPE_3_PRELUDE(vkCreateSwapchainKHR, 3);
    const auto res = vkCreateSwapchainKHR(device, create_info, allocator, &ret);
    if (!success<VK_SUCCESS>(res)) {
        return std::unexpected{res};
    }
    return ret;
}

auto destroy_swapchain_khr() const noexcept { return [this](VkDevice device, VkSwapchainKHR swapchain, const VkAllocationCallbacks* allocator = nullptr) noexcept { return destroy_swapchain_khr(device, swapchain, allocator); }; }
void destroy_swapchain_khr(VkDevice device, VkSwapchainKHR swapchain, const VkAllocationCallbacks* allocator = nullptr) const noexcept { return vkDestroySwapchainKHR(device, swapchain, allocator); }

auto get_swapchain_images_khr() const noexcept { return [this](VkDevice device, VkSwapchainKHR swapchain, u32* out_image_count, VkImage* out_images) noexcept { return this->get_swapchain_images_khr(device, swapchain, out_image_count, out_images); }; }
auto get_swapchain_images_khr(VkDevice device, VkSwapchainKHR swapchain, u32* out_image_count, VkImage* out_images) const noexcept -> std::expected<VkResult, VkResult> {
    const auto res = vkGetSwapchainImagesKHR(device, swapchain, out_image_count, out_images);
    if (!success<VK_SUCCESS, VK_INCOMPLETE>(res)) {
        return std::unexpected{res};
    }
    return res;
}

auto acquire_next_image_khr() const noexcept { return [this](VkDevice device, VkSwapchainKHR swapchain, u64 timeout_ns, VkSemaphore semaphore, VkFence fence) noexcept { this->acquire_next_image_khr(device, swapchain, timeout_ns, semaphore, fence); }; }
auto acquire_next_image_khr(VkDevice device, VkSwapchainKHR swapchain, u64 timeout_ns, VkSemaphore semaphore, VkFence fence) const noexcept -> std::expected<std::pair<std::remove_pointer_t<u32*>, VkResult>, VkResult> {
    TYPE_3_PRELUDE(vkAcquireNextImageKHR, 5);
    const auto res = vkAcquireNextImageKHR(device, swapchain, timeout_ns, semaphore, fence, &ret);
    if (!success<VK_SUCCESS, VK_TIMEOUT, VK_NOT_READY, VK_SUBOPTIMAL_KHR>(res)) {
        return std::unexpected{res};
    }
    return std::pair<std::remove_pointer_t<u32*>, VkResult>{ret, res};
}

auto queue_present_khr() const noexcept { return [this](VkQueue queue, const VkPresentInfoKHR* present_info) noexcept { return this->queue_present_khr(queue, present_info); }; }
auto queue_present_khr(VkQueue queue, const VkPresentInfoKHR* present_info) const noexcept -> std::expected<VkResult, VkResult> {
    const auto res = vkQueuePresentKHR(queue, present_info);
    if (!success<VK_SUCCESS, VK_SUBOPTIMAL_KHR>(res)) {
        return std::unexpected{res};
    }
    return res;
}


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
