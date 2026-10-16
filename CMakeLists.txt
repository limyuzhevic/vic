cmake_minimum_required(VERSION 3.16)
project(nlm_unified_cli VERSION 1.0.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Find nlohmann/json library
find_package(nlohmann_json REQUIRED)

# Find yaml-cpp library
find_package(yaml-cpp REQUIRED)

# Add all component libraries
add_subdirectory(src/core)
add_subdirectory(src/brain)
add_subdirectory(src/memory)
add_subdirectory(src/prediction)
add_subdirectory(src/cognition)
add_subdirectory(src/neuromodulation)
add_subdirectory(src/development)
add_subdirectory(src/agent)
add_subdirectory(src/world)
add_subdirectory(src/experiments)

# Add Python bindings module
add_subdirectory(src/python)

# Add unified CLI executable
add_subdirectory(src)

# Add tests for unified CLI
enable_testing()
add_subdirectory(tests)
