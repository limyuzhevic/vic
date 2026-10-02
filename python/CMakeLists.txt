cmake_minimum_required(VERSION 3.16)

project(pynlm_bindings LANGUAGES CXX)

# Modern C++ configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Position independent code for shared libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Enhanced warning configuration with errors
if(MSVC)
    add_compile_options(/WX /W4 /permissive-)
else()
    add_compile_options(-Werror -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion)
endif()

# Platform-specific configurations
if(NLM_PLATFORM_WINDOWS)
    add_definitions(-DNLM_PLATFORM_WINDOWS)
elseif(NLM_PLATFORM_APPLE)
    add_definitions(-DNLM_PLATFORM_APPLE)
elseif(NLM_PLATFORM_UNIX)
    add_definitions(-DNLM_PLATFORM_UNIX)
endif()

# Dependency management
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Source directory configuration
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# ==============================================================================
# MODERN LIBRARY DEFINITIONS WITH TARGET_SOURCES
# ==============================================================================

# Core libraries with modern CMake target_sources()
add_library(nlm_core_fundamental STATIC)
target_sources(nlm_core_fundamental
    PRIVATE
        ${NLM_SOURCE_DIR}/core/Types/Types.cpp
        ${NLM_SOURCE_DIR}/core/Config/Config.cpp
        ${NLM_SOURCE_DIR}/core/Random/Random.cpp
        ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
        ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
)

target_include_directories(nlm_core_fundamental
    PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/core>
        $<INSTALL_INTERFACE:include/core>
)

# Brain library
add_library(nlm_brain STATIC)
target_sources(nlm_brain
    PRIVATE
        ${NLM_SOURCE_DIR}/brain/Neuron.cpp
        ${NLM_SOURCE_DIR}/brain/Synapse.cpp
        ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
        ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
        ${NLM_SOURCE_DIR}/brain/Brain.cpp
)

target_include_directories(nlm_brain PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/brain>
    $<INSTALL_INTERFACE:include/brain>
)

# Aggregated core library (maintained for backward compatibility)
add_library(nlm_core STATIC)
target_link_libraries(nlm_core PUBLIC nlm_core_fundamental nlm_brain)

# ==============================================================================
# PYTHON BINDINGS MODULE
# ==============================================================================

# Modern pybind11 module with proper target dependencies
pybind11_add_module(pynlm bindings.cpp)

target_link_libraries(pynlm PRIVATE nlm_core)

target_include_directories(pynlm PRIVATE
    ${NLM_SOURCE_DIR}
)

# Compiler-specific options
target_compile_options(pynlm PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/permissive->
)

# ==============================================================================
# PACKAGE CONFIGURATION
# ==============================================================================

# Configure Python package
include(CMakePackageConfigHelpers)

# Generate Python-specific configuration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pynlm-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/pynlm-config.cmake
)

# Install Python module
install(TARGETS pynlm
    LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES}
    ARCHIVE DESTINATION ${PYTHON_SITE_PACKAGES}
    RUNTIME DESTINATION ${PYTHON_SITE_PACKAGES}
)

# Install configuration file
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/pynlm-config.cmake
    DESTINATION ${PYTHON_SITE_PACKAGES}/pynlm/cmake
)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message(STATUS "pyNLM Bindings Configuration Complete")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")