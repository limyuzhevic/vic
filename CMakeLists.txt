# Improved CMake configuration for NLM neural simulation framework
# Better organization, build options, and installation support

cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Enable comprehensive warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wconversion)
endif()

# Add safety checks
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wdouble-promotion -Wnull-dereference -Wduplicated-cond)
endif()

# Option to build with Python bindings
option(NLM_BUILD_PYTHON "Build Python bindings" ON)

# Option to build tests
option(NLM_BUILD_TESTS "Build tests" ON)

# Option to build demo executables
option(NLM_BUILD_DEMOS "Build demo executables" ON)

# Source directory
file(GLOB_RECURSE NLM_CORE_SOURCES
    "src/core/Types/*.cpp"
    "src/core/Config/*.cpp"
    "src/core/Random/*.cpp"
    "src/core/Logger/*.cpp"
    "src/core/SimulationClock/*.cpp"
    "src/brain/*.cpp"
    "src/dynamics/*.cpp"
    "src/plasticity/*.cpp"
    "src/development/*.cpp"
    "src/neuromodulation/*.cpp"
    "src/memory/*.cpp"
    "src/cognition/*.cpp"
    "src/sensory/*.cpp"
    "src/prediction/*.cpp"
    "src/motor/*.cpp"
    "src/environment/*.cpp"
    "src/experiments/*.cpp"
    "src/visualization/*.cpp"
)

# Core neural simulation library
add_library(nlm_core STATIC ${NLM_CORE_SOURCES})

# Public include directories
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Core configuration and utilities
target_compile_features(nlm_core PUBLIC cxx_std_20)
set_target_properties(nlm_core PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

# Agent system library (cognitive capabilities)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)
set_target_properties(nlm_agent PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    CXX_VISIBILITY_PRESET hidden
)

# World system library (environment and interaction)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)
set_target_properties(nlm_world PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# Phase 3 experiments library (biologically-inspired learning)
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
set_target_properties(nlm_phase3 PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# Phase 4 experiments library (cognitive architectures)
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)
set_target_properties(nlm_phase4 PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# Main neural simulation executable
add_executable(nlm src/main.cpp)
target_link_libraries(nlm PUBLIC nlm_core)
set_target_properties(nlm PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Phase 3 demo executable (biologically-inspired learning demonstration)
if(NLM_BUILD_DEMOS)
    add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
    target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
    set_target_properties(nlm_phase3_demo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

# Phase 4 demo executable (cognitive architecture demonstration)
if(NLM_BUILD_DEMOS)
    add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)
    target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
    set_target_properties(nlm_phase4_demo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

# Testing configuration
if(NLM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Python bindings
if(NLM_BUILD_PYTHON)
    add_subdirectory(python)
endif()

# Installation configuration
set(NLM_INSTALL_INCDIR include)
set(NLM_INSTALL_LIBDIR lib)
set(NLM_INSTALL_BINDIR bin)

# Install core library
install(TARGETS nlm_core
    ARCHIVE DESTINATION ${NLM_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${NLM_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${NLM_INSTALL_BINDIR}
)

# Install agent and world libraries
install(TARGETS nlm_agent nlm_world
    ARCHIVE DESTINATION ${NLM_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${NLM_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${NLM_INSTALL_BINDIR}
)

# Install demo executables
if(NLM_BUILD_DEMOS)
    install(TARGETS nlm nlm_phase3_demo nlm_phase4_demo
        RUNTIME DESTINATION ${NLM_INSTALL_BINDIR}
    )
endif()

# Install headers
install(DIRECTORY src/
    DESTINATION ${NLM_INSTALL_INCDIR}
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

# Export configuration for find_package support
include(GNUInstallDirs)

# Create a config file for the package
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    @ONLY
)

# Create a targets file for modern CMake consumption
export(TARGETS nlm_core nlm_agent nlm_world EXPORT NLMTargets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/NLMTargets.cmake
)

# Generate and install pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/nlm.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/nlm.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# Summary of build configuration
message(STATUS "NLM Neural Simulation Framework Build Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Python bindings: ${NLM_BUILD_PYTHON}")
message(STATUS "  Tests: ${NLM_BUILD_TESTS}")
message(STATUS "  Demos: ${NLM_BUILD_DEMOS}")
message(STATUS "  Core library: ${NLM_CORE_SOURCES}")
