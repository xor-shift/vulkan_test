#pragma once

#include <vxl/vk/things/instance.hpp>

namespace vxl::vk {

struct queue_constraints {
    enum class pass_kind {
        does_not_pass,
        barely_passes,
        overshoots,
    };

    VkSurfaceKHR m_present_to = nullptr;
    VkQueueFlags m_features = 0;

    constexpr auto check_flags(VkQueueFamilyProperties props) const -> pass_kind {
        if ((props.queueFlags & m_features) != m_features) {
            return pass_kind::does_not_pass;
        }

        if ((props.queueFlags ^ m_features) == 0) {
            return pass_kind::barely_passes;
        }

        return pass_kind::overshoots;
    }

    auto check(VkQueueFamilyProperties props, u32 family_no, VkPhysicalDevice device, vulkan_functions const& vk_fns) const -> pass_kind {
        const auto present_pass = m_present_to == nullptr || vk_fns.get_physical_device_surface_support_khr(device, family_no, m_present_to);
        return present_pass ? check_flags(props) : pass_kind::does_not_pass;
    }
};

struct device_check_result {
    std::vector<const char*> m_layers_to_enable;
    std::vector<const char*> m_extensions_to_enable;
    std::vector<u32> m_queue_families_for_queues;
};

struct device_constraints {
    std::span<const std::pair<const char*, usize>> m_desired_layers;
    std::span<const std::pair<const char*, usize>> m_desired_extensions;
    std::span<const queue_constraints> m_desired_queues;

    u32 m_min_vk_version = 0;

    auto check_layers_extensions(VkPhysicalDevice device, vulkan_functions const& vk_fns) const -> std::expected<std::optional<device_check_result>, error>;

    auto check(VkPhysicalDevice device, vulkan_functions const& vk_fns) const -> std::expected<std::optional<device_check_result>, error>;
};

struct device_things {
    struct queue_information {
        static constexpr auto sentinel = std::numeric_limits<u32>::max();

        u32 family_no = sentinel;
        u32 queue_no = sentinel;
        VkQueue queue_handle = nullptr;
        VkCommandBuffer command_buffer = nullptr;

        constexpr operator VkQueue() const { return queue_handle; }
        constexpr operator VkCommandBuffer() const { return command_buffer; }
    };

    device_things(std::shared_ptr<instance_things> vk_instance, std::shared_ptr<vulkan_functions> vk_fns);

    ~device_things();

    device_things(device_things const&) = delete;
    device_things(device_things&&) = delete;

    auto init(std::span<const VkPhysicalDeviceType> device_preference, device_constraints const& device_constraints) -> std::expected<void, error>;

    constexpr operator VkPhysicalDevice() const { return m_physical_device; }
    constexpr operator VkDevice() const { return m_device; }

    constexpr auto physical_device() const -> VkPhysicalDevice { return m_physical_device; }
    constexpr auto device() const -> VkDevice { return m_device; }

    constexpr auto queue(usize index) const -> queue_information const& { return m_queues[index]; }

private:
    std::shared_ptr<vulkan_functions> m_vk_fns;
    std::shared_ptr<instance_things> m_vk_instance;

    VkPhysicalDevice m_physical_device = nullptr;
    VkDevice m_device = nullptr;
    std::vector<queue_information> m_queues{};

    std::vector<std::pair<u32, VkCommandPool>> m_command_pools;

    auto real_init(VkPhysicalDevice physical_device, device_check_result const& check_result) -> std::expected<void, error>;
};

}  // namespace vxl::vk
