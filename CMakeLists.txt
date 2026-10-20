cmake_minimum_required(VERSION 3.20)

# Project configuration
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Neurally-Learnable Machine - Integrated Artificial Brain System"
    HOMEPAGE_URL "https://github.com/nlm/nlm"
)

# Options and configuration
option(NLM_ENABLE_PYTHON_BINDINGS "Enable Python bindings (requires pybind11)" ON)
option(NLM_ENABLE_TESTS "Enable testing framework" ON)
option(NLM_ENABLE_BENCHMARKS "Enable performance benchmarks" ON)
option(NLM_ENABLE_COVERAGE "Enable code coverage reporting" OFF)
option(NLM_BUILD_SHARED "Build shared libraries instead of static" OFF)
option(NLM_INSTALL_DEVELOPER "Install development targets" ON)
option(NLM_EXAMPLE_BUILD "Build example applications" ON)

# Build type selection
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler optimizations and warnings based on build type
if(MSVC)
    add_compile_options(/W4 /permissive-)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/Od /GS)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/O2 /GL /DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        add_compile_options(/O2 /GL /Zi /DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        add_compile_options(/O1 /GL /DNDEBUG)
    endif()
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-O0 -g -DDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        add_compile_options(-O2 -g -DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        add_compile_options(-Os -DNDEBUG)
    endif()
endif()

# Source directory structure
set(NLM_SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Define source groups for better IDE organization
function(nlm_add_sources target_name)
    foreach(dir ${ARGN})
        file(GLOB_RECURSE sources "${NLM_SOURCE_ROOT}/${dir}/*.cpp")
        if(sources)
            target_sources(${target_name} PRIVATE ${sources})
            target_include_directories(${target_name} PUBLIC
                $<BUILD_INTERFACE:${NLM_SOURCE_ROOT}>
                $<INSTALL_INTERFACE:include>
            )
            message(STATUS "Added ${dir} sources to ${target_name}: ${sources}")
        endif()
    endforeach()
endfunction()

# Core library - Components
add_library(nlm_core ${NLM_BUILD_SHARED})

# Core components
nlm_add_sources(nlm_core core/Types)
nlm_add_sources(nlm_core core/Config)
nlm_add_sources(nlm_core core/Random)
nlm_add_sources(nlm_core core/Logger)
nlm_add_sources(nlm_core core/SimulationClock)

# Brain components
nlm_add_sources(nlm_core brain)

# Dynamics components
nlm_add_sources(nlm_core dynamics)

# Plasticity components
nlm_add_sources(nlm_core plasticity)

# Development components
nlm_add_sources(nlm_core development)

# Neuromodulation components
nlm_add_sources(nlm_core neuromodulation)

# Memory components
nlm_add_sources(nlm_core memory)

# Cognition components
nlm_add_sources(nlm_core cognition)

# Sensory components
nlm_add_sources(nlm_core sensory)

# Prediction components
nlm_add_sources(nlm_core prediction)

# Motor components
nlm_add_sources(nlm_core motor)

# Environment components
nlm_add_sources(nlm_core environment)

# Experiment components
nlm_add_sources(nlm_core experiments)

# Visualization components
nlm_add_sources(nlm_core visualization)

# Agent system
add_library(nlm_agent ${NLM_BUILD_SHARED})
nlm_add_sources(nlm_agent agent)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system
add_library(nlm_world ${NLM_BUILD_SHARED})
nlm_add_sources(nlm_world world)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 ${NLM_BUILD_SHARED})
target_sources(nlm_phase3 PRIVATE ${NLM_SOURCE_ROOT}/experiments/Phase3Experiment.cpp)
target_include_directories(nlm_phase3 PUBLIC ${NLM_SOURCE_ROOT})
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 ${NLM_BUILD_SHARED})
target_sources(nlm_phase4 PRIVATE ${NLM_SOURCE_ROOT}/experiments/Phase4Experiment.cpp)
target_include_directories(nlm_phase4 PUBLIC ${NLM_SOURCE_ROOT})
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
add_executable(nlm ${NLM_SOURCE_ROOT}/main.cpp)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo ${NLM_SOURCE_ROOT}/experiments/Phase3Demo.cpp)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo ${NLM_SOURCE_ROOT}/experiments/Phase4Demo.cpp)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Python bindings (if enabled)
if(NLM_ENABLE_PYTHON_BINDINGS)
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    find_package(pybind11 CONFIG REQUIRED)
    
    # Python module
    pybind11_add_module(pynlm ${NLM_SOURCE_ROOT}/python/bindings.cpp)
    target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
    target_include_directories(pynlm PRIVATE ${NLM_SOURCE_ROOT})
    
    if(MSVC)
        target_compile_options(pynlm PRIVATE /permissive-)
    endif()
    
    # Python package installation
    install(TARGETS pynlm
        LIBRARY DESTINATION pynlm
        ARCHIVE DESTINATION pynlm
        RUNTIME DESTINATION pynlm
    )
endif()

# Testing framework
if(NLM_ENABLE_TESTS)
    enable_testing()
    
    # Create comprehensive test targets
    add_executable(nlm_test
        tests/test_main.cpp
        tests/test_types.cpp
        tests/test_config.cpp
        tests/test_random.cpp
        tests/test_clock.cpp
        tests/test_neuron.cpp
        tests/test_synapse.cpp
        tests/test_stdp.cpp
        tests/test_brain.cpp
    )
    
    target_link_libraries(nlm_test PRIVATE nlm_core)
    target_compile_features(nlm_test PRIVATE cxx_std_20)
    
    # Add tests to CTest
    add_test(NAME nlm_test COMMAND nlm_test)
    
    # Generate coverage reports if enabled
    if(NLM_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(nlm_test PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        add_custom_command(TARGET nlm_test POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E env SHUFFLE_OUTPUT=1 ./nlm_test
            COMMAND ${CMAKE_COMMAND} -E echo "Running tests..."
        )
        
        # Coverage reporting
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
            COMMAND lcov --directory . --capture --output-file ${CMAKE_BINARY_DIR}/coverage/${PROJECT_NAME}.coverage
            COMMAND genhtml ${CMAKE_BINARY_DIR}/coverage/${PROJECT_NAME}.coverage --output-directory ${CMAKE_BINARY_DIR}/coverage/html
            DEPENDS nlm_test
            COMMENT "Generating coverage report"
        )
    endif()
endif()

# Performance benchmarks
if(NLM_ENABLE_BENCHMARKS)
    add_executable(nlm_benchmark ${NLM_SOURCE_ROOT}/benchmark.cpp)
    target_link_libraries(nlm_benchmark PRIVATE nlm_core nlm_agent nlm_world)
    add_test(NAME nlm_benchmark COMMAND nlm_benchmark)
endif()

# Installation targets
install(TARGETS nlm
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION bin
)

if(NLM_BUILD_SHARED)
    install(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
    )
else()
    install(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
endif()

# Installation of headers
install(DIRECTORY ${NLM_SOURCE_ROOT}/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

# Installation of configuration files
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/nlm_config.ini.example
    DESTINATION etc/nlm
)

# Package configuration for find_package()
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/nlm-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
    DESTINATION lib/cmake/nlm
)

# Build-time generation of configuration files
if(NOT DEFINED NLM_CONFIG_FILE)
    set(NLM_CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/nlm_generated_config.cmake)
endif()

file(WRITE ${NLM_CONFIG_FILE}
    "# NLM Generated Configuration File\n"
    "# Generated by CMake build system\n\n"
    "set(NLM_VERSION ${PROJECT_VERSION})\n"
    "set(NLM_BUILD_TYPE \"${CMAKE_BUILD_TYPE}\")\n"
    "set(NLM_BUILD_SHARED ${NLM_BUILD_SHARED})\n"
    "set(NLM_ENABLE_PYTHON_BINDINGS ${NLM_ENABLE_PYTHON_BINDINGS})\n"
    "set(NLM_ENABLE_TESTS ${NLM_ENABLE_TESTS})\n"
    "set(NLM_ENABLE_BENCHMARKS ${NLM_ENABLE_BENCHMARKS})\n"
)

# Developer installation mode
if(NLM_INSTALL_DEVELOPER)
    # Install development tools and headers
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/docs/ARCHITECTURE.md
        ${CMAKE_CURRENT_SOURCE_DIR}/docs/BIOLOGICAL_FIDELITY.md
        ${CMAKE_CURRENT_SOURCE_DIR}/docs/EXPERIMENTS.md
        DESTINATION docs
    )
    
    # Install example build scripts
    if(NLM_EXAMPLE_BUILD)
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/
            DESTINATION examples
        )
    endif()
endif()

# Summary message
message(STATUS "NLM Build Configuration Summary:")
message(STATUS "  Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Build Shared: ${NLM_BUILD_SHARED}")
message(STATUS "  Python Bindings: ${NLM_ENABLE_PYTHON_BINDINGS}")
message(STATUS "  Tests: ${NLM_ENABLE_TESTS}")
message(STATUS "  Benchmarks: ${NLM_ENABLE_BENCHMARKS}")
message(STATUS "  Coverage: ${NLM_ENABLE_COVERAGE}")