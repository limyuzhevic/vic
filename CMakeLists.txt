# CMake configuration for NLM Neural Learning Machine
# Modern CMake with better organization and build options

cmake_minimum_required(VERSION 3.16)
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

# Source directory - use absolute path for consistency
get_filename_component(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" ABSOLUTE)

# Configuration options
option(NLM_BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(NLM_BUILD_TESTS "Build tests" ON)
option(NLM_BUILD_DOCS "Build documentation" OFF)
option(NLM_ENABLE_PERFORMANCE_OPTS "Enable performance optimizations" ON)
option(NLM_ENABLE_VALIDATION "Enable runtime validation" ON)

# Set version for pkg-config (if available)
set(NLM_SOVERSION 0)

# Core library - use GLOB to organize by module structure
file(GLOB NLM_CORE_SOURCES
    "${NLM_SOURCE_DIR}/core/*.cpp"
    "${NLM_SOURCE_DIR}/brain/*.cpp"
    "${NLM_SOURCE_DIR}/dynamics/*.cpp"
    "${NLM_SOURCE_DIR}/plasticity/*.cpp"
    "${NLM_SOURCE_DIR}/development/*.cpp"
    "${NLM_SOURCE_DIR}/neuromodulation/*.cpp"
    "${NLM_SOURCE_DIR}/memory/*.cpp"
    "${NLM_SOURCE_DIR}/cognition/*.cpp"
    "${NLM_SOURCE_DIR}/sensory/*.cpp"
    "${NLM_SOURCE_DIR}/prediction/*.cpp"
    "${NLM_SOURCE_DIR}/motor/*.cpp"
    "${NLM_SOURCE_DIR}/environment/*.cpp"
    "${NLM_SOURCE_DIR}/experiments/*.cpp"
    "${NLM_SOURCE_DIR}/visualization/*.cpp"
)

# Filter out test and benchmark files
list(FILTER NLM_CORE_SOURCES EXCLUDE REGEX ".*test.*\.cpp$|.*benchmark\.cpp$")

add_library(nlm_core STATIC ${NLM_CORE_SOURCES})

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Agent system
add_library(nlm_agent STATIC
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system
add_library(nlm_world STATIC
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executables
add_executable(nlm ${NLM_SOURCE_DIR}/main.cpp)
target_link_libraries(nlm PUBLIC nlm_core)

add_executable(nlm_phase3_demo ${NLM_SOURCE_DIR}/experiments/Phase3Demo.cpp)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

add_executable(nlm_phase4_demo ${NLM_SOURCE_DIR}/experiments/Phase4Demo.cpp)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Python bindings (optional)
if(NLM_BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Testing (optional)
if(NLM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Performance optimizations
if(NLM_ENABLE_PERFORMANCE_OPTS)
    target_compile_definitions(nlm_core PUBLIC NLM_ENABLE_PERF_OPT)
    target_compile_definitions(nlm_agent PUBLIC NLM_ENABLE_PERF_OPT)
    target_compile_definitions(nlm_world PUBLIC NLM_ENABLE_PERF_OPT)
endif()

# Validation (optional)
if(NLM_ENABLE_VALIDATION)
    target_compile_definitions(nlm_core PUBLIC NLM_ENABLE_VALIDATION)
    target_compile_definitions(nlm_agent PUBLIC NLM_ENABLE_VALIDATION)
endif()

# Installation
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    EXPORT NLMTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Install headers
install(DIRECTORY ${NLM_SOURCE_DIR}/ DESTINATION include/
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)

# Install Python module (if built)
if(NLM_BUILD_PYTHON_BINDINGS)
    install(TARGETS pynlm
        LIBRARY DESTINATION pynlm
        ARCHIVE DESTINATION pynlm
        RUNTIME DESTINATION pynlm
    )
endif()

# Generate and install pkg-config file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/nlm.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/nlm.pc"
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/nlm.pc
    DESTINATION lib/pkgconfig
)

# Create CMake package configuration
install(EXPORT NLMTargets
    FILE NLMConfig.cmake
    NAMESPACE nlm::
    DESTINATION lib/cmake/nlm
)

# Summary
message(STATUS "NLM Build Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Python bindings: ${NLM_BUILD_PYTHON_BINDINGS}")
message(STATUS "  Tests: ${NLM_BUILD_TESTS}")
message(STATUS "  Performance optimizations: ${NLM_ENABLE_PERFORMANCE_OPTS}")
message(STATUS "  Runtime validation: ${NLM_ENABLE_VALIDATION}")
message(STATUS "  Core source files: ${NLM_CORE_SOURCES}")