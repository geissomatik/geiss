# Portable engine tests. The Windows screensaver and plugin stay on the root CMake build.
cmake_minimum_required(VERSION 3.20)
project(geiss_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(geiss_engine STATIC
    src/audio.cpp
    src/beat.cpp
    src/effects.cpp
    src/map.cpp
    src/engine.cpp
)
target_include_directories(geiss_engine PUBLIC include)
target_compile_options(geiss_engine PRIVATE -Wall -Wextra -Wpedantic)

add_executable(geiss_engine_tests tests/test_main.cpp)
target_link_libraries(geiss_engine_tests PRIVATE geiss_engine)
target_compile_options(geiss_engine_tests PRIVATE -Wall -Wextra -Wpedantic)
