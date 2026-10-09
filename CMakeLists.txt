# NLM Modular Build System
# This CMake module provides a comprehensive build system for the Neuromodular Learning Machine project

# CMake version requirements
cmake_minimum_required(VERSION 3.20)

# Project declaration
project(NLM VERSION 0.1.0 LANGUAGES CXX
    DESCRIPTION "Neuromodular Learning Machine - A modular neural network simulation framework"
)

# Set C++20 standard
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

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
    set(NLM_PLATFORM "Windows")
    set(NLM_COMPILER_RUNTIME "/MD")
elseif(APPLE)
    set(NLM_PLATFORM "macOS")
    set(NLM_COMPILER_RUNTIME "")
elseif(UNIX)
    set(NLM_PLATFORM "Linux")
    set(NLM_COMPILER_RUNTIME "")
endif()

# Source directory setup
set(NLM_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Core library definition with actual source files
add_library(nlm_core STATIC
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
    src/memory/Memory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    src/motor/MotorSystem.cpp
    src/motor/Action.cpp
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
)

# Agent library
add_library(nlm_agent STATIC
    src/agent/AgentBrain.cpp
    src/agent/SensoryPercept.cpp
)

# World library
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
    src/world/Observation.cpp
)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)

# Main executable
add_executable(nlm src/main.cpp)

# Demo executables
add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)

# Python bindings
if(NLM_ENABLE_PYTHON_BINDINGS)
    include(cmake/NLMPythonBindings.cmake)
endif()

# Tests
if(NLM_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples (simplified with placeholder files)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/examples)
    add_subdirectory(examples)
endif()

# Experiments (simplified with placeholder files)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/experiments)
    add_subdirectory(experiments)
endif()

# Documentation
if(NLM_ENABLE_DOCS)
    include(cmake/NLMDocumentation.cmake)
endif()

# Summary
message(STATUS "NLM Build System Configuration:")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Platform: ${NLM_PLATFORM}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Python Bindings: ${NLM_ENABLE_PYTHON_BINDINGS}")
message(STATUS "  Tests: ${NLM_ENABLE_TESTS}")
