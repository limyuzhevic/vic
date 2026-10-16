cmake_minimum_required(VERSION 3.19)

# Project configuration
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "An Experimental Artificial Developmental Brain"
)

# Modern CMake policies for best practices
cmake_policy(SET CMP0091 NEW)  # MSVC runtime library

# C++20 standard with features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Multi-configuration build support (Debug, Release, RelWithDebInfo, MinSizeRel)
if(NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")
endif()

# Build type settings for multi-configuration generators
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra -Wpedantic -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -Wall -Wextra -Wpedantic -DNDEBUG")

# Platform detection and configuration
include(CMakeFindDependencyMacro)

# Check compiler
if(MSVC)
    add_compile_options(/W4 /permissive-)
    add_definitions(-DNOMINMAX)
else()
    # GCC/Clang optimizations
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror=return-type
        -Werror=uninitialized
        -Werror=overloaded-virtual
        -Werror=shadow
    )
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        add_compile_options(-fexceptions -fPIC)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-fexceptions -fPIC)
    endif()
endif()

# Source directory - exclude tests from the main library
file(GLOB_RECURSE NLM_SOURCES
    "src/core/*.cpp"
    "src/core/*/*.cpp"
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
    "src/agent/*.cpp"
    "src/world/*.cpp"
)

# Performance optimization flags per build type
string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_UPPER)
if(BUILD_TYPE_UPPER STREQUAL "RELEASE")
    add_compile_options(-march=native -flto)
elseif(BUILD_TYPE_UPPER STREQUAL "MINSIZEREL")
    add_compile_options(-Os -flto)
elseif(BUILD_TYPE_UPPER STREQUAL "RELWITHDEBINFO")
    add_compile_options(-O2 -g -DNDEBUG)
endif()

# Core library - restructured from original
add_library(nlm_core STATIC
    # Core modules
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    src/core/Types/Types.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Brain modules
add_library(nlm_brain STATIC
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
)

target_include_directories(nlm_brain PUBLIC src)
target_link_libraries(nlm_brain PUBLIC nlm_core)

# Dynamics modules
add_library(nlm_dynamics STATIC
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
)

target_include_directories(nlm_dynamics PUBLIC src)
target_link_libraries(nlm_dynamics PUBLIC nlm_core nlm_brain)

# Plasticity modules
add_library(nlm_plasticity STATIC
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
)

target_include_directories(nlm_plasticity PUBLIC src)
target_link_libraries(nlm_plasticity PUBLIC nlm_core nlm_brain nlm_dynamics)

# Development modules
add_library(nlm_development STATIC
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
)

target_include_directories(nlm_development PUBLIC src)
target_link_libraries(nlm_development PUBLIC nlm_core nlm_brain nlm_plasticity)

# Neuromodulation modules
add_library(nlm_neuromodulation STATIC
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
)

target_include_directories(nlm_neuromodulation PUBLIC src)
target_link_libraries(nlm_neuromodulation PUBLIC nlm_core nlm_brain nlm_plasticity)

# Memory modules
add_library(nlm_memory STATIC
    src/memory/WorkingMemory.cpp
    src/memory/EpisodicMemory.cpp
    src/memory/SemanticMemory.cpp
    src/memory/ProceduralMemory.cpp
    src/memory/AssociativeMemory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
)

target_include_directories(nlm_memory PUBLIC src)
target_link_libraries(nlm_memory PUBLIC nlm_core nlm_brain nlm_plasticity nlm_neuromodulation)

# Cognition modules
add_library(nlm_cognition STATIC
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
)

target_include_directories(nlm_cognition PUBLIC src)
target_link_libraries(nlm_cognition PUBLIC nlm_core nlm_memory)

# Sensory modules
add_library(nlm_sensory STATIC
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
)

target_include_directories(nlm_sensory PUBLIC src)
target_link_libraries(nlm_sensory PUBLIC nlm_core)

# Prediction modules
add_library(nlm_prediction STATIC
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
)

target_include_directories(nlm_prediction PUBLIC src)
target_link_libraries(nlm_prediction PUBLIC nlm_core nlm_cognition nlm_sensory)

# Motor modules
add_library(nlm_motor STATIC
    src/motor/MotorSystem.cpp
)

target_include_directories(nlm_motor PUBLIC src)
target_link_libraries(nlm_motor PUBLIC nlm_core nlm_cognition nlm_sensory nlm_prediction)

# Environment modules
add_library(nlm_environment STATIC
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
)

target_include_directories(nlm_environment PUBLIC src)
target_link_libraries(nlm_environment PUBLIC nlm_core)

# Experiment modules
add_library(nlm_experiments STATIC
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    src/experiments/AdvancedExperimentConfig.hpp
    src/experiments/AdvancedExperiment.hpp
    src/experiments/AdvancedExperimentImpl.cpp
)

target_include_directories(nlm_experiments PUBLIC src)
target_link_libraries(nlm_experiments PUBLIC nlm_core nlm_agent nlm_world nlm_brain nlm_memory)

# Agent modules
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core nlm_brain nlm_sensory nlm_prediction nlm_motor)

# World modules
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core nlm_environment)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world nlm_experiments)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_experiments)

# Main executable
add_executable(nlm
    src/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core nlm_agent nlm_world nlm_brain nlm_memory nlm_cognition nlm_sensory nlm_prediction nlm_motor nlm_environment nlm_experiments nlm_phase3 nlm_phase4)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)

target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Advanced experiment demo executable
add_executable(nlm_advanced_experiment_demo
    src/experiments/AdvancedExperimentDemo.cpp
)

target_link_libraries(nlm_advanced_experiment_demo PUBLIC nlm_core nlm_agent nlm_world nlm_experiments)

# Install targets
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_brain nlm_dynamics nlm_plasticity nlm_development nlm_neuromodulation nlm_memory nlm_cognition nlm_sensory nlm_prediction nlm_motor nlm_environment nlm_experiments nlm_phase3 nlm_phase4
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# Platform detection
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Platform: Linux")
    set(PLATFORM_LINUX 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Platform: macOS")
    set(PLATFORM_MACOS 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Platform: Windows")
    set(PLATFORM_WINDOWS 1)
else()
    message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Detect compiler and architecture
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(COMPILER_SUPPORTS_CXX17 1)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 9.0)
        set(COMPILER_SUPPORTS_CXX20 1)
    endif()
endif()

# Architecture detection
include(ProcessorCount)
ProcessorCount(NPROC)
if(NOT NPROC)
    set(NPROC 2)
endif()

# Build optimization options
option(NLM_BUILD_SHARED "Build shared libraries instead of static" OFF)
option(NLM_BUILD_TESTS "Build tests" ON)
option(NLM_BUILD_EXAMPLES "Build examples" ON)
option(NLM_ENABLE_COVERAGE "Enable code coverage reporting" OFF)
option(NLM_ENABLE_SANITIZERS "Enable memory/debug sanitizers" OFF)
option(NLM_BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(NLM_ENABLE_PERF_MEASUREMENTS "Enable build-time performance measurements" ON)

# Performance optimizations based on build type
if(NLM_ENABLE_PERF_MEASUREMENTS)
    add_custom_target(measure_build_time
        COMMAND ${CMAKE_COMMAND} -E time cmake --build ${CMAKE_BINARY_DIR} --target nlm_core
        COMMENT "Measuring build time for nlm_core"
        VERBATIM
    )
endif()

# Dependency management - check for required packages
find_package(Threads REQUIRED)

# Set C++ standard properties
function(setup_target_cxx_features target)
    target_compile_features(${target} PUBLIC cxx_std_20)
    if(NLM_ENABLE_COVERAGE)
        target_compile_options(${target} PRIVATE --coverage)
    endif()
    if(NLM_ENABLE_SANITIZERS)
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            target_compile_options(${target} PRIVATE -fsanitize=address,undefined)
            target_link_options(${target} PRIVATE -fsanitize=address,undefined)
        endif()
    endif()
endfunction()

# Apply C++ features to all targets
foreach(target nlm_core nlm_brain nlm_dynamics nlm_plasticity nlm_development nlm_neuromodulation nlm_memory nlm_cognition nlm_sensory nlm_prediction nlm_motor nlm_environment nlm_experiments nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm)
    setup_target_cxx_features(${target})
endforeach()

# Apply C++ features to executables
foreach(target nlm nlm_phase3_demo nlm_advanced_experiment_demo)
    setup_target_cxx_features(${target})
endforeach()

# Export compilation database for tooling (useful for IDEs, clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build cache configuration
set(CMAKE_TRY_COMPILE_CONFIGURATION "Release")
set(CMAKE_BUILD_RPATH "$ORIGIN")
set(CMAKE_INSTALL_RPATH "$ORIGIN")

# Cross-compilation support
if(CMAKE_CROSSCOMPILING)
    message(STATUS "Cross-compilation enabled")
    # Additional cross-compilation configurations can be added here
endif()

# Build-time metrics collection
if(NLM_ENABLE_PERF_MEASUREMENTS)
    add_custom_target(measure_build_metrics
        COMMAND ${CMAKE_COMMAND} -E echo "Build metrics collection enabled"
        COMMAND ${CMAKE_COMMAND} -E time cmake --build ${CMAKE_BINARY_DIR} --target all
        COMMENT "Collecting build metrics"
        VERBATIM
    )
endif()

# Custom build targets for different configurations
add_custom_target(speed_test
    COMMAND ./nlm --benchmark
    COMMENT "Run performance benchmark"
    VERBATIM
)

add_custom_target(validation
    COMMAND ./nlm --validate-config
    COMMENT "Validate configuration"
    VERBATIM
)

# Provide summary of build configuration
function(print_build_summary)
    message(STATUS "=== NLM Build Summary ===")
    message(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
    message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")
    message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}")
    message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
    message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
    message(STATUS "Processor count: ${NPROC}")
    message(STATUS "Build shared libraries: ${NLM_BUILD_SHARED}")
    message(STATUS "Build tests: ${NLM_BUILD_TESTS}")
    message(STATUS "Enable coverage: ${NLM_ENABLE_COVERAGE}")
    message(STATUS "Enable sanitizers: ${NLM_ENABLE_SANITIZERS}")
    message(STATUS "Build Python bindings: ${NLM_BUILD_PYTHON_BINDINGS}")
    message(STATUS "Performance measurements: ${NLM_ENABLE_PERF_MEASUREMENTS}")
    message(STATUS "============================")
endfunction()

# Print summary when building
add_custom_target(print_summary ALL
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_FILE}
    COMMENT "Printing build summary"
    VERBATIM
)
