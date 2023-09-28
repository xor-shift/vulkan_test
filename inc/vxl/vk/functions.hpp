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

#include <vxl/vk/detail/vk_functions/generated/members.ipp>

private:
    template<typename T, T vulkan_functions::*member_ptr>
    void fetch_function(const char* name, VkInstance instance = nullptr) {
        this->*member_ptr = reinterpret_cast<T>(vkGetInstanceProcAddr(instance, name));
    }
};

}  // namespace vxl::vk
