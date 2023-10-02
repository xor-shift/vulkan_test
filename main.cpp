#include <vxl/dyna_loader.hpp>
#include <vxl/vk/stuff.hpp>

#include <SDL.h>

#include <cmath>

auto main() -> int {
    spdlog::set_level(spdlog::level::debug);

    spdlog::debug("{}", vxl::vk::error::make("this is a test error"));

    if (auto init_res = SDL_Init(SDL_INIT_VIDEO); init_res != 0) {
        spdlog::error("SDL initialisation failed, error: {}", SDL_GetError());
        return 1;
    }

    auto vulkan_stuff = ({
        auto&& res = vxl::vk::vulkan_stuff::make();

        if (!res) {
            spdlog::error("failed to initialize graphics: {}", res.error());
            return 1;
        }

        std::move(*res);
    });

    auto&& res = vulkan_stuff.run();

    if (!res) {
        spdlog::error("program failed with error: {}", res.error());
        return 1;
    }
}
