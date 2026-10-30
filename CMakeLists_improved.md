cmake_minimum_required(VERSION 3.16)

# Complete build system with enhanced configuration options

project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
 set(CMAKE_CXX_STANDARD 20)
 set(CMAKE_CXX_STANDARD_REQUIRED ON)
 set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Configuration options
set(BUILD_TESTS "Build test executables" ON CACHE BOOL "Build test executables")
set(BUILD_DOCS "Build documentation" OFF CACHE BOOL "Build documentation")
set(BUILD_PYTHON_BINDINGS "Build Python bindings" ON CACHE BOOL "Build Python bindings")
set(BUILD_EXAMPLES "Build example scripts" ON CACHE BOOL "Build example scripts")
set(BUILD_PERFORMANCE_TESTS "Build performance tests" OFF CACHE BOOL "Build performance tests")

# Core library
add_library(nlm_core STATIC
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Dependencies for core library
find_package(Threads REQUIRED)
target_link_libraries(nlm_core PUBLIC Threads::Threads)

# Modular component libraries (forward declarations to avoid circular dependencies)

# Brain module
add_subdirectory(src/brain)

# Dynamics module
add_subdirectory(src/dynamics)

# Plasticity module
add_subdirectory(src/plasticity)

# Development module
add_subdirectory(src/development)

# Neuromodulation module
add_subdirectory(src/neuromodulation)

# Memory module
add_subdirectory(src/memory)

# Cognition module
add_subdirectory(src/cognition)

# Sensory module
add_subdirectory(src/sensory)

# Prediction module
add_subdirectory(src/prediction)

# Motor module
add_subdirectory(src/motor)

# Environment module
add_subdirectory(src/environment)

# Experiments module
add_subdirectory(src/experiments)

# Agent module
add_subdirectory(src/agent)

# World module
add_subdirectory(src/world)

# Visualization module
add_subdirectory(src/visualization)

# Performance module
add_subdirectory(src/performance)

# Main executable
add_executable(nlm
    ${NLM_SOURCE_DIR}/main.cpp
)

# Link main executable with all core components
target_link_libraries(nlm PUBLIC
    nlm_core
    nlm_brain
    nlm_dynamics
    nlm_plasticity
    nlm_development
    nlm_neuromodulation
    nlm_memory
    nlm_cognition
    nlm_sensory
    nlm_prediction
    nlm_motor
    nlm_environment
    nlm_experiments
    nlm_agent
    nlm_world
    nlm_visualization
    nlm_performance
)

# Set target properties
target_compile_features(nlm PRIVATE cxx_std_20)

# Install targets for development
install(TARGETS nlm nlm_core
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install headers
install(DIRECTORY ${NLM_SOURCE_DIR}/core/
    DESTINATION include/core
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY ${NLM_SOURCE_DIR}/brain/
    DESTINATION include/brain
    FILES_MATCHING PATTERN "*.hpp"
)

# Python bindings (optional)
if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Tests (optional)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples (optional)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Documentation (future)
if(BUILD_DOCS)
    add_subdirectory(docs)
endif()

# Summary
message(STATUS "NLM Build Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  Build Python bindings: ${BUILD_PYTHON_BINDINGS}")
message(STATUS "  Build examples: ${BUILD_EXAMPLES}")
message(STATUS "  Build documentation: ${BUILD_DOCS}")
message(STATUS "  C++ standard: C++20")
message(STATUS "  Source directory: ${NLM_SOURCE_DIR}")

# Print available targets
message(STATUS "\nAvailable targets:")
message(STATUS "  nlm                    - Main executable")
message(STATUS "  nlm_core               - Core library")
message(STATUS "  nlm_brain              - Brain module")
message(STATUS "  nlm_dynamics           - Dynamics module")
message(STATUS "  nlm_plasticity         - Plasticity module")
message(STATUS "  nlm_development        - Development module")
message(STATUS "  nlm_neuromodulation    - Neuromodulation module")
message(STATUS "  nlm_memory             - Memory module")
message(STATUS "  nlm_cognition          - Cognition module")
message(STATUS "  nlm_sensory            - Sensory module")
message(STATUS "  nlm_prediction         - Prediction module")
message(STATUS "  nlm_motor              - Motor module")
message(STATUS "  nlm_environment        - Environment module")
message(STATUS "  nlm_experiments        - Experiments module")
message(STATUS "  nlm_agent              - Agent module")
message(STATUS "  nlm_world              - World module")
message(STATUS "  nlm_visualization      - Visualization module")
message(STATUS "  nlm_performance        - Performance module")

if(BUILD_PYTHON_BINDINGS)
    message(STATUS "  pynlm                  - Python bindings")
endif()

if(BUILD_TESTS)
    message(STATUS "  nlm_test               - Test suite")
endif()
