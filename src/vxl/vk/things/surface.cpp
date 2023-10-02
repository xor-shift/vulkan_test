#pragma once

#include <vxl/vk/things/surface.hpp>

#include <SDL.h>

namespace vxl::vk {

surface_things::surface_things(std::shared_ptr<instance_things> vk_instance, std::shared_ptr<vulkan_functions> vk_fns)
    : m_vk_fns(vk_fns)
    , m_vk_instance(vk_instance) {}

surface_things::~surface_things() {
    if (m_vk_surface != nullptr) {
        m_vk_fns->destroy_surface_khr(*m_vk_instance, m_vk_surface);
        m_vk_surface = nullptr;
    }

    if (m_sdl_window != nullptr) {
        SDL_DestroyWindow(m_sdl_window);
        m_sdl_window = nullptr;
    }
}

auto surface_things::init(VkExtent2D window_size) -> std::expected<void, error> {
    m_sdl_window = SDL_CreateWindow(
      "vulkan test",                                                              //
      static_cast<int>(window_size.width), static_cast<int>(window_size.height),  //
      static_cast<SDL_WindowFlags>(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE)
    );

    if (m_sdl_window == nullptr) {
        spdlog::error("failed to create an SDL window, error: {}", SDL_GetError());
        return std::unexpected{error::make("failed to create an SDL window")};
    }

    const auto res = SDL_Vulkan_CreateSurface(m_sdl_window, *m_vk_instance, &m_vk_surface);

    if (res == SDL_FALSE || m_vk_surface == nullptr) {
        m_vk_surface = nullptr;
        spdlog::error("failed to create a vulkan surface through SDL, error: {}", SDL_GetError());
        return std::unexpected{error::make("failed to create a vulkan surface through SDL")};
    }

    return {};
}

}
