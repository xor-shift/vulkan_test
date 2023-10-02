#include <vxl/vk/things/debug_messenger.hpp>

namespace vxl::vk {

std::atomic_size_t debug_messenger_things::m_instance_counter = 0;

debug_messenger_things::debug_messenger_things(std::shared_ptr<instance_things> vk_instance, std::shared_ptr<vulkan_functions> vk_fns)
    : m_messenger_instance(m_instance_counter++)
    , m_vk_instance(vk_instance)
    , m_vk_fns(vk_fns) {}

debug_messenger_things::~debug_messenger_things() {
    if (m_debug_messenger != nullptr) {
        m_vk_fns->destroy_debug_utils_messenger_ext(*m_vk_instance, m_debug_messenger);
        m_debug_messenger = nullptr;
    }
}

auto debug_messenger_things::init() -> std::expected<void, error> {
    const auto debug_utils_messenger_create_info = VkDebugUtilsMessengerCreateInfoEXT{
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .pNext = nullptr,
      .flags = 0,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,
      .pfnUserCallback = &vulkan_debug_messenger,
      .pUserData = reinterpret_cast<void*>(this),
    };

    m_debug_messenger =
      TRYX(error::from_vk(m_vk_fns->create_debug_utils_messenger_ext(*m_vk_instance, &debug_utils_messenger_create_info), "could not create a VkDebugUtilsMessengerEXT"));

    return {};
}

auto debug_messenger_things::vulkan_debug_messenger(
  VkDebugUtilsMessageSeverityFlagBitsEXT severity,
  VkDebugUtilsMessageTypeFlagsEXT type,
  const VkDebugUtilsMessengerCallbackDataEXT* data,
  void* user_data
) -> VkBool32 {
    auto level = spdlog::level::n_levels;

    static constexpr std::pair<int, spdlog::level::level_enum> level_lookup[]{
      {VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, spdlog::level::err},
      {VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, spdlog::level::warn},
      {VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, spdlog::level::info},
      {VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT, spdlog::level::debug},
    };

    if (auto it = std::ranges::find_if(level_lookup, [severity](auto p) { return (p.first & severity) != 0; }); it != std::ranges::end(level_lookup)) {
        level = it->second;
    }

    static constexpr std::pair<VkDebugUtilsMessageTypeFlagsEXT, char> message_type_lookup[]{
      {VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT, 'G'},
      {VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, 'V'},
      {VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT, 'P'},
      {VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT, 'D'},
    };

    const auto message_type_arr = make_flags_short(std::span(message_type_lookup), type);
    const auto message_type = std::string_view(message_type_arr);

    reinterpret_cast<debug_messenger_things*>(user_data)->log(level, message_type, data);

    return (VkBool32) false;
}

void debug_messenger_things::log(spdlog::level::level_enum severity, std::string_view message_type, const VkDebugUtilsMessengerCallbackDataEXT* data) {
    if (severity == spdlog::level::n_levels) {
        spdlog::warn("vulkan debug messenger callback called with an invalid level: {:#08X}", static_cast<u32>(severity));
        severity = spdlog::level::info;
    }

    spdlog::log(severity, fmt::runtime("[{}]: {}"), message_type, data->pMessage);
}

}  // namespace vxl::vk
