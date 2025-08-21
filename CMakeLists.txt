cmake_minimum_required(VERSION 3.10)
project(tiny_universe VERSION 0.0
        DESCRIPTION "2D hardware-accelerated universe"
        LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)


set(SOURCE
    src/Application.cpp)
add_executable(tiny_universe ${SOURCE})
target_compile_options(tiny_universe PRIVATE -Wall -Wextra -Werror)
target_compile_definitions(tiny_universe PRIVATE -DCMAKE_PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
                                                 -DCMAKE_PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR})
