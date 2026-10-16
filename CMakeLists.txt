# Modern root CMakeLists.txt for NLM project
# Uses target-based approach with component architecture

cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# Import modern CMake utilities
include(cmake/NLMConfig.cmake)

# Validate project configuration
validate_nlm_config()

# Set modern CMake policies for consistency
cmake_policy(SET CMP0074 NEW)  # Use <Package>_ROOT variables
cmake_policy(SET CMP0091 NEW)  # MSVC runtime library selection

# Configure build options
option(BUILD_PERFORMANCE_TESTS "Build performance benchmarks" OFF)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_EXPERIMENTS "Build experimental modules" ON)
option(INSTALL_DEVELOPMENT_PACKAGES "Install development headers" ON)

# Compiler and standard settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Advanced build configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -DDEBUG)
else()
    add_compile_options(-O3 -DNDEBUG)
endif()

# Compiler-specific settings
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -fvisibility=hidden)
endif()

# Source directory configuration
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" CACHE PATH "NLM source directory")

# Component discovery and library creation
# Core systems
discover_sources(NLM_CORE_SOURCES "core/")
discover_sources(NLM_BRAIN_SOURCES "brain/")
discover_sources(NLM_DYNAMICS_SOURCES "dynamics/")
discover_sources(NLM_PLASTICITY_SOURCES "plasticity/")
discover_sources(NLM_DEVELOPMENT_SOURCES "development/")
discover_sources(NLM_NEUROMODULATION_SOURCES "neuromodulation/")
discover_sources(NLM_MEMORY_SOURCES "memory/")
discover_sources(NLM_COGNITION_SOURCES "cognition/")
discover_sources(NLM_SENSORY_SOURCES "sensory/")
discover_sources(NLM_PREDICTION_SOURCES "prediction/")
discover_sources(NLM_MOTOR_SOURCES "motor/")
discover_sources(NLM_ENVIRONMENT_SOURCES "environment/")
discover_sources(NLM_EXPERIMENTS_SOURCES "experiments/")
discover_sources(NLM_VISUALIZATION_SOURCES "visualization/")
discover_sources(NLM_AGENT_SOURCES "agent/")
discover_sources(NLM_WORLD_SOURCES "world/")

# Core library (foundation)
add_nlm_component(nlm_core
    ${NLM_CORE_SOURCES}
    ${NLM_BRAIN_SOURCES}
    ${NLM_DYNAMICS_SOURCES}
    ${NLM_PLASTICITY_SOURCES}
    ${NLM_DEVELOPMENT_SOURCES}
    ${NLM_NEUROMODULATION_SOURCES}
    ${NLM_MEMORY_SOURCES}
    ${NLM_COGNITION_SOURCES}
    ${NLM_SENSORY_SOURCES}
    ${NLM_PREDICTION_SOURCES}
    ${NLM_MOTOR_SOURCES}
    ${NLM_ENVIRONMENT_SOURCES}
    ${NLM_VISUALIZATION_SOURCES}
)

# Agent system
add_nlm_component(nlm_agent ${NLM_AGENT_SOURCES})
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system
add_nlm_component(nlm_world ${NLM_WORLD_SOURCES})
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments (conditional)
if(BUILD_EXPERIMENTS)
    add_nlm_component(nlm_phase3 ${NLM_EXPERIMENTS_SOURCES})
    target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Phase 4 experiments (conditional)
if(BUILD_EXPERIMENTS)
    add_nlm_component(nlm_phase4 ${NLM_EXPERIMENTS_SOURCES})
    target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Phase 5 experiments (conditional)
if(BUILD_EXPERIMENTS)
    add_nlm_component(nlm_phase5 ${NLM_EXPERIMENTS_SOURCES})
    target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Phase 6 integrated experiments (conditional)
if(BUILD_EXPERIMENTS)
    add_nlm_component(nlm_phase6 ${NLM_EXPERIMENTS_SOURCES})
    target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Main executable
add_executable(nlm src/main.cpp)
target_link_libraries(nlm PRIVATE nlm_core)

# Demo executables
if(BUILD_EXPERIMENTS)
    add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
    target_link_libraries(nlm_phase3_demo PRIVATE nlm_core nlm_agent nlm_world nlm_phase3)
    
    add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)
    target_link_libraries(nlm_phase4_demo PRIVATE nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
    
    add_executable(nlm_phase5_demo src/experiments/Phase5Demo.cpp)
    target_link_libraries(nlm_phase5_demo PRIVATE nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5)
    
    add_executable(nlm_phase6_demo src/experiments/Phase6Demo.cpp)
    target_link_libraries(nlm_phase6_demo PRIVATE nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6)
endif()

# Performance monitoring
add_performance_monitoring_targets()

# Python bindings integration
if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Testing infrastructure
enable_testing()
add_subdirectory(tests)

# Installation configuration
if(INSTALL_DEVELOPMENT_PACKAGES)
    configure_install_targets()
endif()

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    DESTINATION lib/cmake/nlm
)

# Summary message
message(STATUS "NLM Project configured with CMake ${CMAKE_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Python bindings: ${BUILD_PYTHON_BINDINGS}")
message(STATUS "Experiments enabled: ${BUILD_EXPERIMENTS}")
