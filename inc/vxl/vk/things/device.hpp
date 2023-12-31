#pragma once

#include <vxl/vk/things/instance.hpp>

#include <stuff/scope/unique_resource.hpp>

#include <vk_mem_alloc.h>

#include <cstring>

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

struct device_allocation {
    device_allocation() = default;
    device_allocation(device_allocation const&) = delete;

    auto init(VmaAllocator allocator, usize size, VkBufferUsageFlags usage, std::span<const u32> queue_families = {}) -> std::expected<void, error> {
        m_allocator = allocator;
        m_allocated_size = size;

        auto buffer_create_info = VkBufferCreateInfo{
          .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .size = static_cast<VkDeviceSize>(size),
          .usage = usage,
          .sharingMode = queue_families.empty() ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
          .queueFamilyIndexCount = static_cast<u32>(queue_families.size()),
          .pQueueFamilyIndices = queue_families.empty() ? (const u32*)nullptr : queue_families.data(),
        };

        auto allocation_info = VmaAllocationCreateInfo{
          .flags = 0,
          .usage = VMA_MEMORY_USAGE_CPU_TO_GPU,
          .requiredFlags = 0,
          .preferredFlags = 0,
          .memoryTypeBits = 0,
          .pool = nullptr,
          .pUserData = nullptr,
          .priority = 0.f,
        };

        vmaCreateBuffer(m_allocator, &buffer_create_info, &allocation_info, &m_buffer, &m_allocation, nullptr);

        return {};
    }

    template<typename T>
    auto write(T* data, usize count = 1) -> std::expected<void, error> {
        if (sizeof(T) * count > m_allocated_size) {
            return std::unexpected{error::make("trying to write more bytes than the buffer can hold")};
        }

        auto* gpu_data = (void*)nullptr;
        vmaMapMemory(m_allocator, m_allocation, &gpu_data);

        std::memcpy(gpu_data, static_cast<void*>(data), count * sizeof(T));

        vmaUnmapMemory(m_allocator, m_allocation);

        return {};
    }

    device_allocation(device_allocation&& other) noexcept
        : m_allocated_size(other.m_allocated_size)
        , m_allocator(other.m_allocator)
        , m_allocation(other.m_allocation)
        , m_buffer(other.m_buffer) {
        other.m_allocated_size = 0;
        other.m_allocator = nullptr;
        other.m_allocation = nullptr;
        other.m_buffer = nullptr;
    }

    ~device_allocation() {
        if (m_buffer != nullptr) {
            vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
        }
    }

    // private:
    usize m_allocated_size = 0uz;

    VmaAllocator m_allocator = nullptr;
    VmaAllocation m_allocation = nullptr;
    VkBuffer m_buffer = nullptr;
};

/*
struct device_image {
    device_image(std::shared_ptr<vulkan_functions> vk_fns)
      :m_vk_fns(std::move(vk_fns)) {}

    device_image(device_image const&) = delete;

    device_image(device_image&& other) {}

    ~device_image() {
        if (m_image_view != nullptr) {
            m_image_view = nullptr;
        }
    }

    auto init(
      VkFormat image_format,
      VkExtent3D extent,
      VkImageUsageFlags image_usage,
      VkFormat image_view_format,
      VkImageAspectFlags image_view_aspect_flags,
      VmaMemoryUsage memory_usage,
      std::span<const u32> queue_families = {}) -> std::expected<void, error> {
        auto image_create_info = VkImageCreateInfo{
          .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .imageType = VK_IMAGE_TYPE_2D,
          .format = image_format,
          .extent = extent,
          .mipLevels = 1,
          .arrayLayers = 1,
          .samples = VK_SAMPLE_COUNT_1_BIT,
          .tiling = VK_IMAGE_TILING_OPTIMAL,
          .usage = image_usage,
          .sharingMode = queue_families.empty() ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
          .queueFamilyIndexCount = static_cast<u32>(queue_families.size()),
          .pQueueFamilyIndices = queue_families.empty() ? (const u32*)nullptr : queue_families.data(),
          .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        auto allocation_info = VmaAllocationCreateInfo{
          .flags = 0,
          .usage = memory_usage,
          .requiredFlags = memory_usage == VMA_MEMORY_USAGE_GPU_ONLY ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : (VkMemoryPropertyFlagBits)0,
          .preferredFlags = 0,
          .memoryTypeBits = 0,
          .pool = nullptr,
          .pUserData = nullptr,
          .priority = 0.f,
        };

        auto image = (VkImage) nullptr;
        auto allocation = (VmaAllocation) nullptr;
        vmaCreateImage(m_gpu_allocator, &image_create_info, &allocation_info, &image, &allocation, nullptr);

        auto image_view_create_info = VkImageViewCreateInfo{
          .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .image = nullptr,
          .viewType = VK_IMAGE_VIEW_TYPE_2D,
          .format = image_view_format,
          .components = {},
          .subresourceRange =
            VkImageSubresourceRange{
              .aspectMask = image_view_aspect_flags,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 1,
              .layerCount = 1,
            },
        };

        auto image_view = TRYX(error::from_vk(m_vk_fns->create_image_view(m_device, &image_view_create_info)));

      return {};
    }

    VkImage m_image;
    VkImageView m_image_view;

private:
    std::shared_ptr<vulkan_functions> m_vk_fns {};

    VkDevice m_device = nullptr;
    VmaAllocator m_allocator = nullptr;
    VmaAllocation m_allocation = nullptr;
};
*/

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

    auto allocate(usize size, VkBufferUsageFlags usage, std::span<const u32> queue_families = {}) -> std::expected<device_allocation, error> {
        auto ret = device_allocation{};
        TRYX(ret.init(m_gpu_allocator, size, usage, queue_families));
        return ret;
    }

private:
    std::shared_ptr<vulkan_functions> m_vk_fns;
    std::shared_ptr<instance_things> m_vk_instance;

    VkPhysicalDevice m_physical_device = nullptr;
    VkDevice m_device = nullptr;

    VmaAllocator m_gpu_allocator;

    std::vector<queue_information> m_queues{};

    std::vector<std::pair<u32, VkCommandPool>> m_command_pools;

    auto real_init(VkPhysicalDevice physical_device, device_check_result const& check_result) -> std::expected<void, error>;
};

}  // namespace vxl::vk
