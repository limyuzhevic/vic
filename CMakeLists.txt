cmake_minimum_required(VERSION 3.24)

project(NLM
    VERSION 0.1.0
    DESCRIPTION "Neural Learning Machine"
    LANGUAGES CXX
    HOMEPAGE_URL "https://github.com/NLM/NLM"
)

# Modern CMake policies
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()

if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Build type not specified, using Release")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# C++20 standard with explicit features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Modern warning configuration
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Platform-specific configurations
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_options(-fPIC)
endif()

# Configuration options
option(NLM_BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)
option(NLM_ENABLE_TESTS "Enable tests" ON)
option(NLM_ENABLE_PERFORMANCE "Enable performance monitoring" ON)
option(NLM_ENABLE_PYTHON_BINDINGS "Enable Python bindings" OFF)

# Include directories for core headers
add_library(nlm_headers INTERFACE)
target_include_directories(nlm_headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Core library (modular organization)
add_library(nlm_core
    STATIC
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
)

target_link_libraries(nlm_core PUBLIC nlm_headers)

# Brain system
add_library(nlm_brain
    STATIC
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
)

target_link_libraries(nlm_brain PUBLIC nlm_core nlm_headers)

# Dynamics system
add_library(nlm_dynamics
    STATIC
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
)

target_link_libraries(nlm_dynamics PUBLIC nlm_brain nlm_headers)

# Plasticity system
add_library(nlm_plasticity
    STATIC
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
)

target_link_libraries(nlm_plasticity PUBLIC nlm_dynamics nlm_headers)

# Development system
add_library(nlm_development
    STATIC
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
)

target_link_libraries(nlm_development PUBLIC nlm_plasticity nlm_headers)

# Neuromodulation system
add_library(nlm_neuromodulation
    STATIC
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
)

target_link_libraries(nlm_neuromodulation PUBLIC nlm_development nlm_headers)

# Memory system
add_library(nlm_memory
    STATIC
    src/memory/WorkingMemory.cpp
    src/memory/EpisodicMemory.cpp
    src/memory/SemanticMemory.cpp
    src/memory/ProceduralMemory.cpp
    src/memory/AssociativeMemory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
)

target_link_libraries(nlm_memory PUBLIC nlm_neuromodulation nlm_headers)

# Cognition system
add_library(nlm_cognition
    STATIC
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
)

target_link_libraries(nlm_cognition PUBLIC nlm_memory nlm_headers)

# Sensory system
add_library(nlm_sensory
    STATIC
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
)

target_link_libraries(nlm_sensory PUBLIC nlm_cognition nlm_headers)

# Prediction system
add_library(nlm_prediction
    STATIC
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
)

target_link_libraries(nlm_prediction PUBLIC nlm_sensory nlm_headers)

# Motor system
add_library(nlm_motor
    STATIC
    src/motor/MotorSystem.cpp
)

target_link_libraries(nlm_motor PUBLIC nlm_prediction nlm_headers)

# Environment system
add_library(nlm_environment
    STATIC
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
)

target_link_libraries(nlm_environment PUBLIC nlm_motor nlm_headers)

# Experiment system
add_library(nlm_experiments
    STATIC
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
)

target_link_libraries(nlm_experiments PUBLIC nlm_environment nlm_headers)

# Visualization system
add_library(nlm_visualization
    STATIC
    src/visualization/VisualizationInterface.cpp
)

target_link_libraries(nlm_visualization PUBLIC nlm_experiments nlm_headers)

# Agent system
add_library(nlm_agent
    STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)

target_link_libraries(nlm_agent PUBLIC nlm_core nlm_sensory nlm_headers)

# World system
add_library(nlm_world
    STATIC
    src/world/SimpleWorld.cpp
)

target_link_libraries(nlm_world PUBLIC nlm_environment nlm_headers)

# Phase 3 experiments
add_library(nlm_phase3
    STATIC
    src/experiments/Phase3Experiment.cpp
)

target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world nlm_experiments nlm_headers)

# Phase 4 experiments
add_library(nlm_phase4
    STATIC
    src/experiments/Phase4Experiment.cpp
)

target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_headers)

# Performance monitoring and benchmarking
add_library(nlm_performance
    STATIC
    src/performance/PerformanceMonitoring.cpp
    src/performance/PerformanceProfiler.cpp
    src/performance/PerformanceMetrics.cpp
    src/performance/PerformanceAlerting.cpp
)

target_link_libraries(nlm_performance PUBLIC nlm_core nlm_headers)

if(NLM_ENABLE_PYTHON_BINDINGS)
    add_library(nlm_performance_bindings
        MODULE
        python/performance_bindings.cpp
    )
    target_link_libraries(nlm_performance_bindings PUBLIC nlm_performance nlm_headers)
    set_target_properties(nlm_performance_bindings PROPERTIES
        PREFIX ""
        SUFFIX ".so"
    )
endif()

# Performance benchmark
add_library(nlm_benchmark
    STATIC
    src/performance/PerformanceBenchmark.cpp
)

target_link_libraries(nlm_benchmark PUBLIC nlm_performance nlm_headers)

# Main executable
add_executable(nlm src/main.cpp)
target_link_libraries(nlm PRIVATE nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
target_link_libraries(nlm_phase3_demo PRIVATE nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)
target_link_libraries(nlm_phase4_demo PRIVATE nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Performance benchmark executable
add_executable(nlm_benchmark_exe src/benchmark.cpp)
target_link_libraries(nlm_benchmark_exe PRIVATE nlm_core)

# Testing configuration
if(NLM_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation rules
install(TARGETS nlm
    RUNTIME DESTINATION bin
)

install(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    nlm_performance nlm_benchmark nlm_brain nlm_dynamics nlm_plasticity
    nlm_development nlm_neuromodulation nlm_memory nlm_cognition nlm_sensory
    nlm_prediction nlm_motor nlm_environment nlm_experiments nlm_visualization
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY src/
    DESTINATION include/nlm
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

install(FILES
    python/bindings.h
    python/performance_bindings.h
    DESTINATION include/nlm/python
)

if(NLM_ENABLE_PYTHON_BINDINGS)
    install(TARGETS nlm_performance_bindings
        LIBRARY DESTINATION lib/python3.X/site-packages/nlm
    )
endif()

# Package configuration
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/nlm-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    INSTALL_DESTINATION lib/cmake/nlm
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
    DESTINATION lib/cmake/nlm
)