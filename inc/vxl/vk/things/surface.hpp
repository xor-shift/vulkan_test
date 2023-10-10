#pragma once

#include <vxl/vk/things/instance.hpp>

#include <SDL_vulkan.h>

namespace vxl::vk {

struct surface_things {
    surface_things(std::shared_ptr<instance_things> vk_instance, std::shared_ptr<vulkan_functions> vk_fns);

    ~surface_things();

    surface_things(surface_things const&) = delete;
    surface_things(surface_things&&) = delete;

    operator SDL_Window*() const { return m_sdl_window; }
    operator VkSurfaceKHR() const { return m_vk_surface; }

    auto window() const -> SDL_Window* { return m_sdl_window; }
    auto surface() const -> VkSurfaceKHR { return m_vk_surface; }

    auto init(SDL_Window* sdl_window) -> std::expected<void, error>;
    auto init(VkExtent2D window_size) -> std::expected<void, error>;

private:
    std::shared_ptr<vulkan_functions> m_vk_fns;
    std::shared_ptr<instance_things> m_vk_instance;

    SDL_Window* m_sdl_window = nullptr;
    VkSurfaceKHR m_vk_surface = nullptr;
};

}
