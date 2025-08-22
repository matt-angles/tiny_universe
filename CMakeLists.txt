cmake_minimum_required(VERSION 3.11)
project(tiny_universe VERSION 0.0
        DESCRIPTION "2D hardware-accelerated universe"
        LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(spdlog REQUIRED)

set(SOURCE
    src/Application.cpp
    src/logging.cpp)
add_executable(tiny_universe ${SOURCE})
target_compile_options(tiny_universe PRIVATE -Wall -Wextra -Werror -Wno-error)
target_compile_definitions(tiny_universe PRIVATE -DCMAKE_PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
                                                 -DCMAKE_PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR})
target_link_libraries(tiny_universe PRIVATE spdlog::spdlog)
