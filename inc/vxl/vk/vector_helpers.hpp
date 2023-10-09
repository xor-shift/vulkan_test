#pragma once

#include <vxl/vk/utils.hpp>

#include <stuff/intro/introspectors/function.hpp>

#include <vulkan/vulkan.h>

#include <expected>
#include <memory_resource>

namespace vxl::vk {

template<typename Fn, typename T = std::remove_pointer_t<typename stf::intro::function_introspector<Fn>::template nth_argument<1>>>
    requires(std::is_invocable_v<Fn, u32*, T*>) && (!std::is_same_v<std::invoke_result_t<Fn, u32*, T*>, void>)
auto get_vector(Fn&& fn, std::pmr::memory_resource* memory_resource = std::pmr::get_default_resource()) -> std::expected<std::pmr::vector<T>, VkResult> {
    auto result = VK_SUCCESS;
    auto ret = std::pmr::vector<T>{memory_resource};

    for (;;) {
        ret.clear();

        auto property_count = 0u;
        auto&& asd = std::invoke(std::forward<Fn>(fn), &property_count, nullptr);
        result = TRYX(asd);

        if (result == VK_SUCCESS && property_count != 0) {
            ret.resize(property_count);
            result = TRYX(std::invoke(std::forward<Fn>(fn), &property_count, ret.data()));
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

template<typename T, typename Fn>
    requires(!std::is_same_v<std::invoke_result_t<Fn, u32*, T*>, void>) && (std::is_invocable_v<Fn, u32*, T*>)
auto get_vector(Fn&& fn, std::pmr::memory_resource* memory_resource = std::pmr::get_default_resource()) -> std::expected<std::pmr::vector<T>, VkResult> {
    return get_vector<Fn, T>(std::forward<Fn>(fn), memory_resource);
}

template<typename Fn, typename T = std::remove_pointer_t<typename stf::intro::function_introspector<Fn>::template nth_argument<1>>>
    requires(std::is_same_v<std::invoke_result_t<Fn, u32*, T*>, void>)
auto get_vector(Fn&& fn, std::pmr::memory_resource* memory_resource = std::pmr::get_default_resource()) -> std::pmr::vector<T> {
    auto property_count = 0u;
    std::invoke(std::forward<Fn>(fn), &property_count, nullptr);

    auto ret = std::pmr::vector<T>{property_count, {}, memory_resource};
    std::invoke(std::forward<Fn>(fn), &property_count, ret.data());

    if (property_count < ret.size()) {
        return get_vector<Fn, T>(std::forward<Fn>(fn), memory_resource);
    }

    return ret;
}

template<typename T, typename Fn>
    requires(std::is_same_v<std::invoke_result_t<Fn, u32*, T*>, void>) && (std::is_invocable_v<Fn, u32*, T*>)
auto get_vector(Fn&& fn, std::pmr::memory_resource* memory_resource = std::pmr::get_default_resource()) -> std::pmr::vector<T> {
    return get_vector<Fn, T>(std::forward<Fn>(fn), memory_resource);
}

}  // namespace vxl::vk
