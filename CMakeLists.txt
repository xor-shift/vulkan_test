cmake_minimum_required(VERSION 3.27)
project(vulkan_test)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(thirdparty/sdl)
add_subdirectory(thirdparty/stuff)
add_subdirectory(thirdparty/vk-bootstrap)
find_package(Vulkan REQUIRED)

add_executable(vulkan_test main.cpp)

target_link_libraries(${PROJECT_NAME}
        stf::core
        stf::scope
        Vulkan::Vulkan SDL3::SDL3
        vk-bootstrap::vk-bootstrap
)
