#pragma once

#include <vxl/dyna_loader.hpp>

#include <stuff/intro/introspectors/function.hpp>

#include <vulkan/vulkan.h>

namespace vxl::vk {

struct vulkan_functions {
    vulkan_functions() = default;

    void init(dynamic_loader const& dyna_loader) { init(dyna_loader.get_dl_symbol<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr")); }

    void init(PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr);

    void init(VkInstance instance);

    void init(VkDevice device);

#define TYPE_3_PRELUDE(_name, _n)                                                \
    using introspector_t = stf::intro::function_introspector<decltype(_name)>;   \
    using return_type = std::remove_pointer_t<introspector_t::nth_argument<_n>>; \
    auto ret = return_type {}

    template<auto... SuccessValues>
    static constexpr auto success(auto value) -> bool {
        return ((value == SuccessValues) || ...);
    }

#define VXL_VK_GENERATED_DONT_INCLUDE_IDE_HELPERS

#include <vxl/vk/detail/vk_functions/generated/wrappers.ipp>

#include <vxl/vk/detail/vk_functions/generated/members.ipp>

private:
    template<typename T, T vulkan_functions::*member_ptr>
    void fetch_function(const char* name, VkInstance instance = nullptr) {
        this->*member_ptr = reinterpret_cast<T>(vkGetInstanceProcAddr(instance, name));
    }
};

}  // namespace vxl::vk
