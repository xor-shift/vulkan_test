#pragma once

#include <vxl/vk/things/instance.hpp>

namespace vxl::vk {

struct debug_messenger_things {
    debug_messenger_things(std::shared_ptr<instance_things> vk_instance, std::shared_ptr<vulkan_functions> vk_fns);

    ~debug_messenger_things();

    debug_messenger_things(debug_messenger_things const&) = delete;
    debug_messenger_things(debug_messenger_things&&) = delete;

    auto init() -> std::expected<void, error>;

private:
    static std::atomic_size_t m_instance_counter;
    usize m_messenger_instance;
    std::shared_ptr<instance_things> m_vk_instance;
    std::shared_ptr<vulkan_functions> m_vk_fns;

    VkDebugUtilsMessengerEXT m_debug_messenger = nullptr;

    static auto
    vulkan_debug_messenger(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data, void* user_data)
      -> VkBool32;

    void log(spdlog::level::level_enum severity, std::string_view message_type, const VkDebugUtilsMessengerCallbackDataEXT* data);
};

}  // namespace vxl::vk
