cmake_minimum_required(VERSION 3.11)
project(tiny_universe VERSION 0.0
        DESCRIPTION "2D hardware-accelerated universe"
        LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
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
    src/renderer/vk/Instance.cpp
    src/renderer/vk/Device.cpp
    src/logging.cpp)
add_executable(tiny_universe ${SOURCE})
target_compile_options(tiny_universe PRIVATE -Wall -Wextra -Werror -Wno-error)
target_compile_definitions(tiny_universe PRIVATE -DCMAKE_PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
                                                 -DCMAKE_PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR})
target_include_directories(tiny_universe PRIVATE src/)
target_link_libraries(tiny_universe PRIVATE vulkan glfw glm::glm PNG::PNG spdlog::spdlog)
