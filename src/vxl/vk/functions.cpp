#include <vxl/vk/functions.hpp>

#include <cstring>

namespace vxl::vk {

#pragma push_macro("PTR_NAME_PAIR")
#pragma push_macro("PTR_PTR_PAIR")

#define PTR_NAME_PAIR(_name) \
    { reinterpret_cast<void (*vulkan_functions::*)()>(&vulkan_functions::_name), #_name }

#define PTR_PTR_PAIR(_name_0, _name_1) \
    { reinterpret_cast<void (*vulkan_functions::*)()>(&vulkan_functions::_name_0), reinterpret_cast<void (*vulkan_functions::*)()>(&vulkan_functions::_name_1), }

void vulkan_functions::init(PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr) {
    vkGetInstanceProcAddr = vk_get_instance_proc_addr;

#include <vxl/vk/detail/vk_functions/generated/pre_instance_init.ipp>

    for (auto [ptr, name] : functions_and_names) {
        this->*ptr = vkGetInstanceProcAddr(nullptr, name);
    }

#include <vxl/vk/detail/vk_functions/generated/pre_instance_init_replace.ipp>

    for (auto [ptr_dst, ptr_src] : replacement_pairs) {
        if (ptr_dst != nullptr) {
            continue;
        }

        std::memcpy((void*)&(this->*ptr_dst), (void*)&(this->*ptr_src), sizeof(void(*)()));
    }
}

void vulkan_functions::init(VkInstance instance) {
#include <vxl/vk/detail/vk_functions/generated/instance_init.ipp>

    for (auto [ptr, name] : functions_and_names) {
        this->*ptr = vkGetInstanceProcAddr(instance, name);
    }

#include <vxl/vk/detail/vk_functions/generated/instance_init_replace.ipp>

    for (auto [ptr_dst, ptr_src] : replacement_pairs) {
        if (ptr_dst != nullptr) {
            continue;
        }

        std::memcpy((void*)&(this->*ptr_dst), (void*)&(this->*ptr_src), sizeof(void(*)()));
    }
}

void vulkan_functions::init(VkDevice device) {
#include <vxl/vk/detail/vk_functions/generated/device_init.ipp>

    for (auto [ptr, name] : functions_and_names) {
        this->*ptr = vkGetDeviceProcAddr(device, name);
    }

#include <vxl/vk/detail/vk_functions/generated/device_init_replace.ipp>

    for (auto [ptr_dst, ptr_src] : replacement_pairs) {
        if (ptr_dst != nullptr) {
            continue;
        }

        std::memcpy((void*)&(this->*ptr_dst), (void*)&(this->*ptr_src), sizeof(void(*)()));
    }
}

#pragma pop_macro("PTR_PTR_PAIR")
#pragma pop_macro("PTR_NAME_PAIR")

}  // namespace vxl::vk
