# CMake build configuration for NLM Neural Learning Machine
# This file sets up the modular build system with organized target dependencies

cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# Configuration options
option(NLM_ENABLE_PYTHON_BINDINGS "Enable Python bindings" ON)
option(NLM_ENABLE_TESTS "Enable testing" ON)
option(NLM_ENABLE_DOCS "Enable documentation generation" OFF)
option(NLM_OPTIMIZE_FOR_PERFORMANCE "Optimize for performance" ON)
option(NLM_BUILD_EXAMPLES "Build example executables" ON)

# C++ standard and compiler settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /WX)
    add_definitions(-DNLM_PLATFORM_WINDOWS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_definitions(-DNLM_PLATFORM_UNIX)
endif()

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Performance optimizations
if(NLM_OPTIMIZE_FOR_PERFORMANCE)
    if(MSVC)
        add_compile_options(/arch:AVX2 /fp:fast)
    else()
        add_compile_options(-O3 -march=native -flto)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(NLM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Logging configuration
message(STATUS "NLM Build Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Python bindings: ${NLM_ENABLE_PYTHON_BINDINGS}")
message(STATUS "  Tests enabled: ${NLM_ENABLE_TESTS}")
message(STATUS "  Performance optimization: ${NLM_OPTIMIZE_FOR_PERFORMANCE}")

# Include modular build functions
include(cmake/BuildModules.cmake)

# Core library targets
build_core_libraries()

# Agent and world system targets
build_agent_system()
build_world_system()

# Experimental phase targets
build_phase3_library()
build_phase4_library()

# Main executables
if(NLM_BUILD_EXAMPLES)
    build_main_executable()
    build_phase3_demo()
    build_phase4_demo()
endif()

# Python bindings
if(NLM_ENABLE_PYTHON_BINDINGS)
    build_python_bindings()
endif()

# Testing
if(NLM_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install_targets()

message(STATUS "NLM build configuration complete!")
