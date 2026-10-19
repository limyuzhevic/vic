# NLM CMake Configuration - Modernized Build System
# 
# This CMake configuration has been optimized for:
# - Better organization and maintainability
# - Target-based dependencies
# - Cleaner separation of concerns
# - Support for all demo modes and experiments

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
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# ==============================================================================
# Core Libraries
# ==============================================================================

# Core utilities library
add_library(nlm_core STATIC
    # Core system components
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
)

# Brain system library (depends on core)
add_library(nlm_brain STATIC
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
)

target_link_libraries(nlm_brain PUBLIC nlm_core)
target_include_directories(nlm_brain PUBLIC src)

# Neural dynamics library
add_library(nlm_dynamics STATIC
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
)
target_link_libraries(nlm_dynamics PUBLIC nlm_core)

# Plasticity systems library
add_library(nlm_plasticity STATIC
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
)
target_link_libraries(nlm_plasticity PUBLIC nlm_core)

# Development systems library
add_library(nlm_development STATIC
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
)
target_link_libraries(nlm_development PUBLIC nlm_core)

# Neuromodulation library
add_library(nlm_neuromodulation STATIC
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
)
target_link_libraries(nlm_neuromodulation PUBLIC nlm_core)

# Memory systems library
add_library(nlm_memory STATIC
    src/memory/WorkingMemory.cpp
    src/memory/EpisodicMemory.cpp
    src/memory/SemanticMemory.cpp
    src/memory/ProceduralMemory.cpp
    src/memory/AssociativeMemory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
)
target_link_libraries(nlm_memory PUBLIC nlm_core nlm_neuromodulation)

# Cognition library
add_library(nlm_cognition STATIC
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
)
target_link_libraries(nlm_cognition PUBLIC nlm_core nlm_brain)

# Sensory processing library
add_library(nlm_sensory STATIC
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
)
target_link_libraries(nlm_sensory PUBLIC nlm_core)

# Prediction system library
add_library(nlm_prediction STATIC
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
)
target_link_libraries(nlm_prediction PUBLIC nlm_core nlm_cognition)

# Motor system library
add_library(nlm_motor STATIC
    src/motor/MotorSystem.cpp
)
target_link_libraries(nlm_motor PUBLIC nlm_core)

# Environment library
add_library(nlm_environment STATIC
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
)
target_link_libraries(nlm_environment PUBLIC nlm_core nlm_sensory)

# Visualization library
add_library(nlm_visualization STATIC
    src/visualization/VisualizationInterface.cpp
)
target_link_libraries(nlm_visualization PUBLIC nlm_core)

# ==============================================================================
# Experiment and Demo Libraries
# ==============================================================================

# Base experiment framework
add_library(nlm_experiments STATIC
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
)
target_link_libraries(nlm_experiments PUBLIC nlm_core nlm_brain nlm_environment)

# Phase 3 experiment
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_brain nlm_agent nlm_world)

# Phase 4 experiment  
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_brain nlm_agent nlm_world nlm_phase3)

# Phase 5 experiment
add_library(nlm_phase5 STATIC
    src/experiments/Phase5Experiment.cpp
)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_brain nlm_agent nlm_world nlm_phase4)

# Phase 6 integration experiment
add_library(nlm_phase6 STATIC
    src/experiments/Phase6IntegratedExperiment.cpp
)
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_brain nlm_agent nlm_world nlm_phase5 nlm_cognition nlm_neuromodulation nlm_memory)

# ==============================================================================
# Application Libraries
# ==============================================================================

# Agent system (brain-world interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_link_libraries(nlm_agent PUBLIC nlm_core nlm_brain)

# World system (simulation environment)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)
target_link_libraries(nlm_world PUBLIC nlm_core nlm_environment)

# ==============================================================================
# Demo and Executable Targets
# ==============================================================================

# Advanced configuration library
add_library(nlm_advanced_config STATIC
    src/core/AdvancedConfig.cpp
)
target_link_libraries(nlm_advanced_config PUBLIC nlm_core)

# ==============================================================================
# Demo and Executable Targets
# ==============================================================================

# Main demo executable - Phase 6 integration focused
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core nlm_brain nlm_agent nlm_world nlm_phase6 nlm_advanced_config)

# Legacy Phase 2 demo executable (for backward compatibility)
add_executable(nlm_phase2_legacy
    src/experiments/Phase2LegacyDemo.cpp
)
target_link_libraries(nlm_phase2_legacy PUBLIC nlm_core nlm_brain nlm_plasticity nlm_neuromodulation)

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

# Phase 6 demo executable (simple integration test)
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase6)

# Phase 6 benchmark demo executable
add_executable(nlm_phase6_benchmark
    src/experiments/Phase6BenchmarkDemo.cpp
)
target_link_libraries(nlm_phase6_benchmark PUBLIC nlm_core nlm_agent nlm_world nlm_phase6 nlm_advanced_config)

# ==============================================================================
# Testing
# ==============================================================================

enable_testing()
add_subdirectory(tests)

# ==============================================================================
# Performance Optimization Options
# ==============================================================================

# Option to enable SIMD optimizations
if(DEFINED ENV{ENABLE_SIMD} AND "$ENV{ENABLE_SIMD}" STREQUAL "ON")
    target_compile_options(nlm_core PRIVATE -mavx2 -mfma)
    target_compile_options(nlm_brain PRIVATE -mavx2 -mfma)
endif()

# Option for memory pool optimizations  
if(DEFINED ENV{ENABLE_MEMORY_POOLS} AND "$ENV{ENABLE_MEMORY_POOLS}" STREQUAL "ON")
    target_compile_definitions(nlm_core PRIVATE NLM_ENABLE_MEMORY_POOLS)
endif()

# ==============================================================================
# Installation
# ==============================================================================

install(TARGETS nlm nlm_core nlm_brain nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase6 nlm_advanced_config
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install demo executables
install(TARGETS nlm_phase2_legacy nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo nlm_phase6_benchmark
    RUNTIME DESTINATION bin
)

# ==============================================================================
# Configuration Summary
# ==============================================================================

message(STATUS "NLM Build Configuration:")
message(STATUS "  Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Core libraries: nlm_core, nlm_brain, nlm_dynamics, nlm_plasticity, nlm_development, nlm_neuromodulation, nlm_memory, nlm_cognition, nlm_sensory, nlm_prediction, nlm_motor, nlm_environment, nlm_visualization")
message(STATUS "  System libraries: nlm_agent, nlm_world")
message(STATUS "  Experiment libraries: nlm_experiments, nlm_phase3, nlm_phase4, nlm_phase5, nlm_phase6")
message(STATUS "  Executables: nlm (main), nlm_phase2_legacy, nlm_phase3_demo, nlm_phase4_demo, nlm_phase6_demo, nlm_phase6_benchmark")