cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type and configuration options
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# Enable position independent code (for static libraries)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Enable warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Platform-specific optimizations
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-march=native -fopenmp)
    add_link_options(-march=native -fopenmp)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Advanced learning and extended modules
option(NLM_ENABLE_ADVANCED_LEARNING "Build advanced learning algorithms" ON)
option(NLM_ENABLE_PYTHON_EXPERIMENTAL "Build experimental Python extensions" OFF)
option(NLM_ENABLE_CUDA_ACCELERATION "Enable CUDA acceleration" OFF)
option(NLM_ENABLE_MPI_PARALLEL "Enable MPI parallelization" OFF)

# Collect advanced source files
if(NLM_ENABLE_ADVANCED_LEARNING)
    file(GLOB_RECURSE ADVANCED_SOURCES "src/advanced/*.cpp")
endif()

# Performance optimization flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g -D_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_options(-O2 -g -DNDEBUG)
endif()

# Code coverage configuration
if(NLM_ENABLE_COVERAGE)
    if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(--coverage)
        add_link_options(--coverage)
    endif()
endif()

# Sanitizer configuration
if(NLM_ENABLE_SANITIZERS)
    if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()

# Add version and compile definitions
add_compile_definitions(NLM_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_compile_definitions(NLM_VERSION_MINOR=${PROJECT_VERSION_MINOR})
add_compile_definitions(NLM_VERSION_PATCH=${PROJECT_VERSION_PATCH})

# Add script directory for custom build scripts
include(${CMAKE_CURRENT_SOURCE_DIR}/scripts/CMakeHelpers.cmake OPTIONAL)

# Collect source files using glob patterns for better maintainability
file(GLOB_RECURSE CORE_SOURCES src/core/*.cpp)
file(GLOB_RECURSE BRAIN_SOURCES src/brain/*.cpp)
file(GLOB_RECURSE DYNAMICS_SOURCES src/dynamics/*.cpp)
file(GLOB_RECURSE PLASTICITY_SOURCES src/plasticity/*.cpp)
file(GLOB_RECURSE DEVELOPMENT_SOURCES src/development/*.cpp)
file(GLOB_RECURSE NEUROMODULATION_SOURCES src/neuromodulation/*.cpp)
file(GLOB_RECURSE MEMORY_SOURCES src/memory/*.cpp)
file(GLOB_RECURSE COGNITION_SOURCES src/cognition/*.cpp)
file(GLOB_RECURSE SENSORY_SOURCES src/sensory/*.cpp)
file(GLOB_RECURSE PREDICTION_SOURCES src/prediction/*.cpp)
file(GLOB_RECURSE MOTOR_SOURCES src/motor/*.cpp)
file(GLOB_RECURSE ENVIRONMENT_SOURCES src/environment/*.cpp)
file(GLOB_RECURSE EXPERIMENT_SOURCES src/experiments/*.cpp)
file(GLOB_RECURSE VISUALIZATION_SOURCES src/visualization/*.cpp)

# Core library with all components
add_library(nlm_core STATIC
    ${CORE_SOURCES}
    ${BRAIN_SOURCES}
    ${DYNAMICS_SOURCES}
    ${PLASTICITY_SOURCES}
    ${DEVELOPMENT_SOURCES}
    ${NEUROMODULATION_SOURCES}
    ${MEMORY_SOURCES}
    ${COGNITION_SOURCES}
    ${SENSORY_SOURCES}
    ${PREDICTION_SOURCES}
    ${MOTOR_SOURCES}
    ${ENVIRONMENT_SOURCES}
    ${EXPERIMENT_SOURCES}
    ${VISUALIZATION_SOURCES}
    $<$<BOOL:${NLM_ENABLE_ADVANCED_LEARNING}>:${ADVANCED_SOURCES}>
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Configuration for core library
set_target_properties(nlm_core PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    DEBUG_POSTFIX "_debug"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
