# NLM Modular Build System
# This CMake module provides a comprehensive build system for the Neuromodular Learning Machine project
# 
# Key Features:
# - Layered architecture with separate CMake files for each layer
# - Automated dependency detection and management
# - Multiple build configurations with optimizations
# - Package integration (FindPython, FindPybind11)
# - Configuration file support
# - Build optimization features
# - Cross-platform support
# - Documentation and examples
# - CI/CD support
# - Advanced build options

# CMake version requirements
cmake_minimum_required(VERSION 3.20)

# Project declaration
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Neuromodular Learning Machine - A modular neural network simulation framework"
)

# Include core build modules
include(cmake/NLMBuildCore.cmake)
include(cmake/NLMDependencyManagement.cmake)
include(cmake/NLMOptimization.cmake)
include(cmake/NLMPythonIntegration.cmake)
include(cmake/NLMPythonBindings.cmake)
include(cmake/NLMCiCd.cmake)
include(cmake/NLMDocumentation.cmake)

# Global configuration options
option(NLM_ENABLE_PYTHON_BINDINGS "Enable Python bindings" ON)
option(NLM_ENABLE_TESTS "Enable tests" ON)
option(NLM_ENABLE_DOCS "Enable documentation generation" OFF)
option(NLM_ENABLE_COVERAGE "Enable code coverage reporting" OFF)
option(NLM_ENABLE_SANITIZERS "Enable memory leak detection" OFF)
option(NLM_ENABLE_PERFORMANCE_PROFILING "Enable performance profiling" OFF)
option(NLM_ENABLE_DISTRIBUTED_BUILD "Enable distributed build support" OFF)
option(NLM_BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Platform-specific configuration
if(WIN32)
    set(NLM_PLATFORM_WINDOWS TRUE)
    set(NLM_COMPILER_RUNTIME "/MD")
elseif(APPLE)
    set(NLM_PLATFORM_MACOS TRUE)
    set(NLM_COMPILER_RUNTIME "")
elseif(UNIX)
    set(NLM_PLATFORM_LINUX TRUE)
    set(NLM_COMPILER_RUNTIME "")
endif()

# Build configurations with optimizations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Set default build configurations
foreach(CONF IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER ${CONF} CONF_UPPER)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_${CONF_UPPER}_COMPILE_OPTIONS ${NLM_${CONF_UPPER}_BASE_COMPILE_OPTIONS})
    endif()
endforeach()

# Compiler optimization flags
set(NLM_DEBUG_BASE_COMPILE_OPTIONS
    -O0
    -g
    -DDEBUG
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wconversion>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
)

set(NLM_RELEASE_BASE_COMPILE_OPTIONS
    -O3
    -DNDEBUG
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wconversion -flto>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /GL /LTCG>
)

set(NLM_RELWITHDEBINFO_BASE_COMPILE_OPTIONS
    -O2
    -g
    -DNDEBUG
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wconversion>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /Zi /Ob2>
)

set(NLM_MINSIZEREL_BASE_COMPILE_OPTIONS
    -Os
    -DNDEBUG
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wconversion>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /O2>
)

# Source directory setup
set(NLM_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(NLM_BINARY_ROOT ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(NLM_LIBRARY_ROOT ${CMAKE_CURRENT_BINARY_DIR}/lib)

# Configure external projects (CMake 3.20+)
if(NLM_ENABLE_DISTRIBUTED_BUILD)
    include(cmake/NLMDistributedBuild.cmake)
endif()

# Add core library
set(NLM_CORE_SOURCES
    ${NLM_SOURCE_ROOT}/core/Types/Types.cpp
    ${NLM_SOURCE_ROOT}/core/Config/Config.cpp
    ${NLM_SOURCE_ROOT}/core/Random/Random.cpp
    ${NLM_SOURCE_ROOT}/core/Logger/Logger.cpp
    ${NLM_SOURCE_ROOT}/core/SimulationClock/SimulationClock.cpp
)

add_library(nlm_core ${NLM_BUILD_TYPE} ${NLM_CORE_SOURCES})

# Add agent library
set(NLM_AGENT_SOURCES
    ${NLM_SOURCE_ROOT}/agent/AgentBrain.cpp
    ${NLM_SOURCE_ROOT}/agent/SensoryPercept.cpp
)

add_library(nlm_agent ${NLM_BUILD_TYPE} ${NLM_AGENT_SOURCES})
target_link_libraries(nlm_agent PUBLIC nlm_core)

# Add world library
set(NLM_WORLD_SOURCES
    ${NLM_SOURCE_ROOT}/world/SimpleWorld.cpp
)

add_library(nlm_world ${NLM_BUILD_TYPE} ${NLM_WORLD_SOURCES})
target_link_libraries(nlm_world PUBLIC nlm_core)

# Add python bindings if enabled
if(NLM_ENABLE_PYTHON_BINDINGS)
    include(cmake/NLMPythonBindings.cmake)
endif()

# Add tests if enabled
if(NLM_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Add examples
add_subdirectory(examples)

# Add experiments
add_subdirectory(experiments)

# Add documentation if enabled
if(NLM_ENABLE_DOCS)
    include(cmake/NLMDocumentation.cmake)
endif()

# Installation
install(TARGETS nlm_core nlm_agent nlm_world
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${NLM_LIBRARY_ROOT}
    LIBRARY DESTINATION ${NLM_LIBRARY_ROOT}
    RUNTIME DESTINATION ${NLM_BINARY_ROOT}
)

# Export targets for downstream projects
export(EXPORT NLMTargets
    FILE "${NLM_LIBRARY_ROOT}/NLMConfig.cmake"
)

# Generate build information file
include(cmake/NLMBuildInfo.cmake)

# Add benchmark
set(NLM_BENCHMARK_SOURCES
    ${NLM_SOURCE_ROOT}/benchmark.cpp
)

add_executable(nlm_benchmark ${NLM_BENCHMARK_SOURCES})
target_link_libraries(nlm_benchmark PRIVATE nlm_core)

# Add main executable
set(NLM_MAIN_SOURCES
    ${NLM_SOURCE_ROOT}/main.cpp
)

add_executable(nlm ${NLM_MAIN_SOURCES})
target_link_libraries(nlm PRIVATE nlm_core)

# Summary
message(STATUS "NLM Build System Configuration:")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Platform: ${NLM_PLATFORM}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Python Bindings: ${NLM_ENABLE_PYTHON_BINDINGS}")
message(STATUS "  Tests: ${NLM_ENABLE_TESTS}")
message(STATUS "  Documentation: ${NLM_ENABLE_DOCS}")
