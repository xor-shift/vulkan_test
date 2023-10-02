#pragma once

#include <vxl/vk/things/instance.hpp>

namespace vxl::vk {

static auto get_desired_instance_layers() -> std::span<const std::pair<const char*, usize>> {
    return (const std::pair<const char*, usize>[]){
      {"VK_LAYER_KHRONOS_validation", 1},
    };
}

static auto get_desired_instance_extensions() -> std::span<const std::pair<const char*, usize>> {
    return (std::pair<const char*, usize> const[]){
      {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, 0},              //
      {VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, 1},  //
      {"VK_KHR_surface", 2},                               //
      {"VK_KHR_xlib_surface", 3},                          //
      {"VK_KHR_xcb_surface", 3},                           //
      {"VK_KHR_wayland_surface", 3},                       //
    };
}

instance_things::instance_things(VkApplicationInfo app_info, std::shared_ptr<vulkan_functions> vk_fns)
    : m_app_info(app_info)
    , m_vk_fns(std::move(vk_fns)) {}

instance_things::~instance_things() {
    if (m_instance != nullptr) {
        m_vk_fns->destroy_instance(m_instance);
        m_instance = nullptr;
    }
}

auto instance_things::init() -> std::expected<void, error> {
    m_instance_extensions = TRYX(layer_extension_store<>::make(m_vk_fns->enumerate_instance_layer_properties(), m_vk_fns->enumerate_instance_extension_properties()));

    for (auto layers_range = m_instance_extensions.layers(); auto const& [layer_no, layer] : layers_range | std::views::enumerate) {
        spdlog::debug("Layer #{}", layer_no);
        vxl::log(layer, 1);

        for (auto extensions_range = *m_instance_extensions.extensions(layer.layerName); auto const& [extension_no, extension] : extensions_range | std::views::enumerate) {
            spdlog::debug("\tExtension #{}", extension_no);
            vxl::log(extension, 2);
        }
    }

    for (auto extensions_range = m_instance_extensions.extensions(); auto const& [extension_no, extension] : extensions_range | std::views::enumerate) {
        spdlog::debug("Extension #{}", extension_no);
        vxl::log(extension, 2);
    }

    auto desired_layers = get_desired_instance_layers();
    auto available_layers = desired_layers | std::views::filter([this](auto const& ext) { return (bool)m_instance_extensions.layer(ext.first); });
    auto layers_to_enable = TRYX(m_instance_extensions.pick_stuff(desired_layers, available_layers, "layer").transform_error([](auto) {
        return error::make("could not enable all required instance layers");
    }));

    auto desired_exts = get_desired_instance_extensions();
    auto available_exts = desired_exts | std::views::filter([this](auto const& ext) { return (bool)m_instance_extensions.extension(ext.first); });
    auto exts_to_enable = TRYX(m_instance_extensions.pick_stuff(desired_exts, available_exts, "extension").transform_error([](auto) {
        return error::make("could not enable all required instance extensions");
    }));

    const auto instance_create_info = VkInstanceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = {},
      .pApplicationInfo = &m_app_info,
      .enabledLayerCount = static_cast<u32>(layers_to_enable.size()),
      .ppEnabledLayerNames = layers_to_enable.data(),
      .enabledExtensionCount = static_cast<u32>(exts_to_enable.size()),
      .ppEnabledExtensionNames = exts_to_enable.data(),
    };
    m_instance = TRYX(error::from_vk(m_vk_fns->create_instance(&instance_create_info), "could not create a vk::Instance"));

    m_vk_fns->init(m_instance);

    return {};
}

}  // namespace vxl::vk
