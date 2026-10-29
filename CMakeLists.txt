cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# NLM - Neural Learning Machine (熙然)
# Phase 6: FINAL INTEGRATION

# C++20 standard
# This project implements a complete artificial brain simulation with:
# - Real spiking neural computation (LIF neurons)
# - Multiple memory systems (working, episodic, semantic, procedural)
# - Neuromodulation (dopamine, curiosity, novelty)
# - Predictive processing and planning
# - Development and learning mechanisms
# - Sensorimotor integration

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
# Use Release by default, but allow Debug builds for development
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler warnings and diagnostics
# Enable comprehensive warnings for both MSVC and GCC/Clang
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source directory configuration
# All source files are located under src/
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library (nlm_core)
# This is the fundamental building block containing all core neural components
# Organized by system type for maintainability and clarity
add_library(nlm_core STATIC
    # Core utilities
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp

    # Neural components
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp

    # Neural dynamics and propagation
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp

    # Plasticity mechanisms (learning rules)
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp

    # Developmental system
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp

    # Neuromodulation systems
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp

    # Memory systems
    src/memory/WorkingMemory.cpp
    src/memory/EpisodicMemory.cpp
    src/memory/SemanticMemory.cpp
    src/memory/ProceduralMemory.cpp
    src/memory/AssociativeMemory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp

    # Cognitive mechanisms
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp

    # Sensory processing
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp

    # Prediction systems
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp

    # Motor system
    src/motor/MotorSystem.cpp

    # Environment interface
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp

    # Experiment framework
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp

    # Visualization interface
    src/visualization/VisualizationInterface.cpp
)

# Include directories for the core library
# Make all headers available to consumers of this library
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Agent system library (nlm_agent)
# Provides the interface between the brain and the external world
# Includes sensory perception and motor command decoding
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system library (nlm_world)
# Represents the external environment with physics, objects, and simulation
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiment library (nlm_phase3)
# Contains the original world interaction experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiment library (nlm_phase4)
# Contains advanced cognitive experiments with prediction and planning
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable (nlm)
# Phase 6 complete integration demonstration
# This is the primary executable that users should run
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 6 demo executable (nlm_phase6_demo)
# Alternative Phase 6 demonstration with additional features
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Testing infrastructure
# Enable CTest for running unit tests and benchmarks
enable_testing()
add_subdirectory(tests)

# Installation targets
# Currently includes libraries and main executables
# Phase 3 and Phase 4 demo executables are available but not installed by default
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase6_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)