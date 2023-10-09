#pragma once

#include <vxl/dyna_loader.hpp>

#include <stuff/intro/introspectors/function.hpp>

#include <vulkan/vulkan.h>

#include <ranges>

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

    // experimental wrappers

    auto create_graphics_pipelines(
      VkDevice device,
      VkPipelineCache pipeline_cache,
      std::span<const VkGraphicsPipelineCreateInfo> create_infos,
      const VkAllocationCallbacks* allocator = nullptr
    ) const noexcept -> std::expected<std::vector<VkPipeline>, VkResult> {
        auto ret = std::vector<VkPipeline>(create_infos.size(), nullptr);
        const auto res = vkCreateGraphicsPipelines(device, pipeline_cache, create_infos.size(), create_infos.data(), allocator, ret.data());

        if (!success<VK_SUCCESS, VK_PIPELINE_COMPILE_REQUIRED_EXT>(res)) {
            return std::unexpected{res};
        }

        return ret;
    }

    auto create_graphics_pipelines() const noexcept {
        /*
        // clang-format off
        using base_type = std::expected<std::vector<VkPipeline>, VkResult> (vulkan_functions::*)(VkDevice, VkPipelineCache, std::span<const VkGraphicsPipelineCreateInfo>, const
        VkAllocationCallbacks*) const noexcept;
        // clang-format on

        auto asd = static_cast<base_type>(&vulkan_functions::create_graphics_pipelines);

        return std::bind_front(asd, this);
        */
        return [this](
                 VkDevice device,                                             //
                 VkPipelineCache pipeline_cache,                              //
                 std::span<const VkGraphicsPipelineCreateInfo> create_infos,  //
                 const VkAllocationCallbacks* allocator = nullptr             //
               ) noexcept {
            return this->create_graphics_pipelines(device, pipeline_cache, create_infos, allocator);  //
        };
    }

    template<usize N>
        requires(N != std::dynamic_extent)
    void cmd_bind_vertex_buffers(VkCommandBuffer command_buffer, u32 first_binding, std::span<const std::pair<const VkBuffer*, const VkDeviceSize*>, N> buffers_and_offsets)
      const noexcept {
        auto buffers = std::array<const VkBuffer*, N>{};
        auto offsets = std::array<const VkDeviceSize*, N>{};

        std::ranges::copy(buffers_and_offsets | std::views::keys, buffers.begin());
        std::ranges::copy(buffers_and_offsets | std::views::values, offsets.begin());

        vkCmdBindVertexBuffers(command_buffer, first_binding, buffers_and_offsets.size(), buffers.data(), offsets.data());
    }

    auto cmd_bind_vertex_buffers() const noexcept {
        return [this]<usize N>
            requires(N != std::dynamic_extent)
        (                                                                                          //
          VkCommandBuffer command_buffer,                                                          //
          u32 first_binding,                                                                       //
          std::span<const std::pair<const VkBuffer*, const VkDeviceSize*>, N> buffers_and_offsets  //
        ) noexcept {
            return this->cmd_bind_vertex_buffers(command_buffer, first_binding, buffers_and_offsets);  //
        };
    }

    void cmd_bind_vertex_buffer(VkCommandBuffer command_buffer, u32 first_binding, VkBuffer buffer, VkDeviceSize offset) const noexcept {
        vkCmdBindVertexBuffers(command_buffer, first_binding, 1, &buffer, &offset);
    }

    auto cmd_bind_vertex_buffer() const noexcept {
        return [this](
                 VkCommandBuffer command_buffer,  //
                 u32 first_binding,               //
                 VkBuffer buffer,                 //
                 VkDeviceSize offset              //
               ) noexcept {
            return this->cmd_bind_vertex_buffer(command_buffer, first_binding, buffer, offset);  //
        };
    }

private:
    template<typename T, T vulkan_functions::*member_ptr>
    void fetch_function(const char* name, VkInstance instance = nullptr) {
        this->*member_ptr = reinterpret_cast<T>(vkGetInstanceProcAddr(instance, name));
    }

public:
#define VXL_VK_GENERATED_DONT_INCLUDE_IDE_HELPERS

#include <vxl/vk/detail/vk_functions/generated/wrappers.ipp>

#include <vxl/vk/detail/vk_functions/generated/members.ipp>
};

}  // namespace vxl::vk
