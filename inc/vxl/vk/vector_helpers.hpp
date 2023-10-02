#pragma once

#include <vxl/vk/utils.hpp>

#include <stuff/intro/introspectors/function.hpp>

#include <vulkan/vulkan.h>

#include <expected>
#include <memory_resource>

namespace vxl::vk {

template<typename Fn, typename T = std::remove_pointer_t<typename stf::intro::function_introspector<Fn>::template nth_argument<1>>>
    requires (!std::is_same_v<typename stf::intro::function_introspector<Fn>::return_type, void>)
auto get_vector(Fn fn, std::pmr::memory_resource* memory_resource = std::pmr::get_default_resource()) -> std::expected<std::pmr::vector<T>, VkResult> {
    auto result = VK_SUCCESS;
    auto ret = std::pmr::vector<T>{memory_resource};

    for (;;) {
        ret.clear();

        auto property_count = 0u;
        auto&& asd = fn(&property_count, nullptr);
        result = TRYX(asd);

        if (result == VK_SUCCESS && property_count != 0) {
            ret.resize(property_count);
            result = TRYX(fn(&property_count, ret.data()));
        }

        if (result != VK_INCOMPLETE) {
            break;
        }
    }

    if (result != VK_SUCCESS) {
        return std::unexpected{result};
    }

    return ret;
}

template<typename Fn, typename T = std::remove_pointer_t<typename stf::intro::function_introspector<Fn>::template nth_argument<1>>>
    requires(std::is_same_v<typename stf::intro::function_introspector<Fn>::return_type, void>)
auto get_vector(Fn fn, std::pmr::memory_resource* memory_resource = std::pmr::get_default_resource()) -> std::pmr::vector<T> {
    auto property_count = 0u;
    fn(&property_count, nullptr);

    auto ret = std::pmr::vector<T>{property_count, {}, memory_resource};
    fn(&property_count, ret.data());

    if (property_count < ret.size()) {
        return get_vector(fn, memory_resource);
    }

    return ret;
}

}  // namespace vxl::vk
