#include <stuff/scope.hpp>

#include <SDL.h>
#include <SDL_vulkan.h>
#include <VkBootstrap.h>
#include <vulkan/vulkan.hpp>

#include <iostream>

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Failed to initialise SDL: " << SDL_GetError() << '\n';
        return 1;
    }

    const auto _1 = stf::scope_exit{[] { SDL_Quit(); }};

    auto* const window = SDL_CreateWindow(
      "vulkan test",  //
      600, 400,       //
      static_cast<SDL_WindowFlags>(SDL_WINDOW_VULKAN)
    );

    const auto _2 = stf::scope_exit{[window] { SDL_DestroyWindow(window); }};

    vkb::InstanceBuilder builder;
    auto inst_ret = builder
                      .set_app_name("Example Vulkan Application")  //
                      .request_validation_layers(true)             //
                      .require_api_version(1, 1, 0)                //
                      .use_default_debug_messenger()               //
                      .build();

    for (auto running = true; running;) {
        for (SDL_Event e; SDL_PollEvent(&e) != 0;) {
            if (e.type == SDL_EVENT_QUIT)
                running = false;
        }

        // draw();
    }

    return 0;
}
