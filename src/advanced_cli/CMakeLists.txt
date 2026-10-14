# Advanced CLI CMake Configuration
# This file builds the advanced CLI components

cmake_minimum_required(VERSION 3.16)

project(NLM_Advanced_CLI LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Advanced CLI executable
add_executable(nlm_advanced_cli
    src/advanced_cli/main.cpp
    src/advanced_cli/AdvancedCLI.cpp
)

target_link_libraries(nlm_advanced_cli PUBLIC nlm_core)

# Install target
install(TARGETS nlm_advanced_cli
    RUNTIME DESTINATION bin
)