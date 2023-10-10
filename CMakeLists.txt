cmake_minimum_required(VERSION 3.27)
project(vulkan_test)

set(CMAKE_CXX_STANDARD 23)

set(SPDLOG_FMT_EXTERNAL OFF)
set(SPDLOG_USE_STD_FORMAT ON)
set(STUFF_RUN_TESTS ON)

find_package(Vulkan REQUIRED)

add_subdirectory(thirdparty/sdl)
add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/stuff)
add_subdirectory(thirdparty/vulkan_memory_allocator)

find_program(SPV_COMPILER glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)
message("spir-v compiler: ${SPV_COMPILER}")
set(SHADER_FILES
        ${CMAKE_SOURCE_DIR}/shaders/triangle.frag
        ${CMAKE_SOURCE_DIR}/shaders/triangle.vert
)

foreach (SHADER_FILE ${SHADER_FILES})
    get_filename_component(SHADER_FILE_NAME ${SHADER_FILE} NAME)
    set(OUTPUT_FILE "${CMAKE_SOURCE_DIR}/run/${SHADER_FILE_NAME}.spv")
    add_custom_command(
            OUTPUT ${OUTPUT_FILE}
            COMMAND ${SPV_COMPILER} -V ${SHADER_FILE} -o ${OUTPUT_FILE}
            DEPENDS ${SHADER_FILE}
    )
    list(APPEND SPV_FILES ${OUTPUT_FILE})
endforeach ()

add_custom_target(${PROJECT_NAME}_shaders DEPENDS ${SPV_FILES})

add_executable(vulkan_test
        main.cpp
        src/vxl/vk/things/debug_messenger.cpp
        src/vxl/vk/things/device.cpp
        src/vxl/vk/things/frame.cpp
        src/vxl/vk/things/instance.cpp
        src/vxl/vk/things/pipeline.cpp
        src/vxl/vk/things/surface.cpp
        src/vxl/vk/things/swapchain.cpp
        src/vxl/vk/functions.cpp
        src/vxl/vk/loggers.cpp
        src/vxl/vk/stuff.cpp
        src/vxl/dyna_loader.cpp
)

add_dependencies(${PROJECT_NAME}
        ${PROJECT_NAME}_shaders
)

target_include_directories(vulkan_test PUBLIC
        inc)

target_link_libraries(${PROJECT_NAME}
        stf::blas stf::core stf::intro stf::ply stf::scope
        spdlog::spdlog
        Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator SDL3::SDL3
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
        #VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_NO_EXCEPTIONS)

target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra)

sanitize(vulkan_test)
