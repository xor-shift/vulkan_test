#pragma once

#include <stuff/blas.hpp>
#include <stuff/intro.hpp>

#include <vulkan/vulkan_core.h>

namespace vxl::vk {

template<usize Bindings, usize Attributes>
struct vertex_input_description {
    std::array<VkVertexInputBindingDescription, Bindings> bindings;
    std::array<VkVertexInputAttributeDescription, Attributes> attributes;

    VkPipelineVertexInputStateCreateFlags flags = 0;
};

namespace detail {

struct example_vertex_format {
    stf::blas::vector<float, 4> m_position;
    stf::blas::vector<float, 4> m_normal;
    stf::blas::vector<float, 4> m_color;
    stf::blas::vector<float, 2> m_texture_coordinate;
};

template<typename T, usize N>
struct n_dimensional_vk_format_helper;

#pragma push_macro("ND_VK_FORMAT_HELPER")
#define ND_VK_FORMAT_HELPER(_type, _dims, _vk_format) \
    template<>                                        \
    struct n_dimensional_vk_format_helper<_type, _dims> : std::integral_constant<VkFormat, _vk_format> {}

ND_VK_FORMAT_HELPER(u32, 1, VK_FORMAT_R32_UINT);
ND_VK_FORMAT_HELPER(i32, 1, VK_FORMAT_R32_SINT);
ND_VK_FORMAT_HELPER(float, 1, VK_FORMAT_R32_SFLOAT);
ND_VK_FORMAT_HELPER(u32, 2, VK_FORMAT_R32G32_UINT);
ND_VK_FORMAT_HELPER(i32, 2, VK_FORMAT_R32G32_SINT);
ND_VK_FORMAT_HELPER(float, 2, VK_FORMAT_R32G32_SFLOAT);
ND_VK_FORMAT_HELPER(u32, 3, VK_FORMAT_R32G32B32_UINT);
ND_VK_FORMAT_HELPER(i32, 3, VK_FORMAT_R32G32B32_SINT);
ND_VK_FORMAT_HELPER(float, 3, VK_FORMAT_R32G32B32_SFLOAT);
ND_VK_FORMAT_HELPER(u32, 4, VK_FORMAT_R32G32B32A32_UINT);
ND_VK_FORMAT_HELPER(i32, 4, VK_FORMAT_R32G32B32A32_SINT);
ND_VK_FORMAT_HELPER(float, 4, VK_FORMAT_R32G32B32A32_SFLOAT);
ND_VK_FORMAT_HELPER(u64, 1, VK_FORMAT_R64_UINT);
ND_VK_FORMAT_HELPER(i64, 1, VK_FORMAT_R64_SINT);
ND_VK_FORMAT_HELPER(double, 1, VK_FORMAT_R64_SFLOAT);
ND_VK_FORMAT_HELPER(u64, 2, VK_FORMAT_R64G64_UINT);
ND_VK_FORMAT_HELPER(i64, 2, VK_FORMAT_R64G64_SINT);
ND_VK_FORMAT_HELPER(double, 2, VK_FORMAT_R64G64_SFLOAT);
ND_VK_FORMAT_HELPER(u64, 3, VK_FORMAT_R64G64B64_UINT);
ND_VK_FORMAT_HELPER(i64, 3, VK_FORMAT_R64G64B64_SINT);
ND_VK_FORMAT_HELPER(double, 3, VK_FORMAT_R64G64B64_SFLOAT);
ND_VK_FORMAT_HELPER(u64, 4, VK_FORMAT_R64G64B64A64_UINT);
ND_VK_FORMAT_HELPER(i64, 4, VK_FORMAT_R64G64B64A64_SINT);
ND_VK_FORMAT_HELPER(double, 4, VK_FORMAT_R64G64B64A64_SFLOAT);

#pragma pop_macro("ND_VK_FORMAT_HELPER")

template<typename T>
struct vk_format_helper;

template<typename T>
    requires (std::integral<T> || std::floating_point<T>)
struct vk_format_helper<T> : n_dimensional_vk_format_helper<T, 1> {};

template<typename T, typename... Ts>
    requires((std::is_same_v<T, Ts>) && ...)
struct vk_format_helper<std::tuple<T, Ts...>> : n_dimensional_vk_format_helper<T, 1 + sizeof...(Ts)> {};

template<typename T, usize N>
struct vk_format_helper<std::array<T, N>> : n_dimensional_vk_format_helper<T, N> {};

template<typename T, usize N>
struct vk_format_helper<stf::blas::vector<T, N>> : n_dimensional_vk_format_helper<T, N> {};

template<typename T, usize N>
struct nth_vertex_input_attribute_description {
    static constexpr auto m_format = vk_format_helper<typename stf::intro::introspector_t<T>::template nth_type<N>>::value;

    constexpr operator VkVertexInputAttributeDescription() const noexcept {
        return VkVertexInputAttributeDescription{
          .location = static_cast<u32>(N),
          .binding = 0,
          .format = m_format,
          .offset = stf::intro::introspector_t<T>::template nth_offset<N>,
        };
    }
};

template<typename T, typename Seqs>
struct vertex_input_attribute_descriptions_helper;

template<typename T, usize... Ns>
struct vertex_input_attribute_descriptions_helper<T, std::index_sequence<Ns...>> {
    static constexpr std::array<VkVertexInputAttributeDescription, sizeof...(Ns)> value{
      static_cast<VkVertexInputAttributeDescription>(detail::nth_vertex_input_attribute_description<T, Ns>{})...
    };
};

template<typename T>
static constexpr auto vertex_input_attribute_descriptions_for =
  vertex_input_attribute_descriptions_helper<T, std::make_index_sequence<stf::intro::introspector_t<T>::size()>>::value;

}  // namespace detail

template<typename T, VkVertexInputRate Rate = VK_VERTEX_INPUT_RATE_VERTEX>
constexpr auto automatic_vertex_description() {
    auto attribs = detail::vertex_input_attribute_descriptions_for<T>;
    auto ret = vertex_input_description<1, attribs.size()>{
      .attributes = attribs,
    };

    ret.bindings[0] = VkVertexInputBindingDescription{
      .binding = 0,
      .stride = sizeof(T),
      .inputRate = Rate,
    };

    return ret;
}

}  // namespace vxl::vk
