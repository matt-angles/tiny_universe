cmake_minimum_required(VERSION 3.11)
project(tiny_universe VERSION 0.0
        DESCRIPTION "2D hardware-accelerated universe"
        LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(Vulkan REQUIRED)
find_package(glfw3 3.4 REQUIRED)
find_package(glm REQUIRED)
find_package(PNG REQUIRED)
find_package(spdlog REQUIRED)

set(SOURCE
    src/Application.cpp
    src/AssetManager.cpp
    src/renderer/Renderer.cpp
    src/renderer/Mesh.cpp
    src/renderer/vk/Instance.cpp
    src/renderer/vk/Device.cpp
    src/renderer/vk/Command.cpp
    src/renderer/vk/Swapchain.cpp
    src/renderer/vk/PipelineFactory.cpp
    src/renderer/vk/Pipeline.cpp
    src/renderer/vk/Buffer.cpp
    src/logging.cpp)
add_executable(tiny_universe ${SOURCE})

# TODO: -Wextra and -Werror are not compatible with MSVC
target_compile_options(tiny_universe PRIVATE -Wall -Wextra -Werror)
target_compile_definitions(tiny_universe PRIVATE -DCMAKE_PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
                                                 -DCMAKE_PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR})
target_include_directories(tiny_universe PRIVATE src/)
target_link_libraries(tiny_universe PRIVATE vulkan glfw glm::glm PNG::PNG spdlog::spdlog)

set(SHADERS
    shader/default.vert.glsl
    shader/default.frag.glsl)
set(SHADERC "")

foreach(SHADER_INPUT ${SHADERS})
    get_filename_component(SHADER_BASENAME ${SHADER_INPUT} NAME_WLE)
    set(SHADER_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/shaderc/${SHADER_BASENAME}.spv")

    add_custom_command(OUTPUT ${SHADER_OUTPUT}
                       COMMAND glslangValidator -V ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_INPUT} -o ${SHADER_OUTPUT}
                       DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_INPUT}
                       COMMENT "Compiling GLSL shader \"${SHADER_BASENAME}\""
                       VERBATIM
    )

    list(APPEND SHADERC ${SHADER_OUTPUT})
endforeach()
add_custom_target(tiny_universe_shaders ALL DEPENDS ${SHADERC})
