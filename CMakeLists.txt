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
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library
add_library(nlm_core STATIC
    # Core
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    # Brain factory
    src/factory/BrainFactory.cpp
    # Error handling
    src/core/ErrorHandling.cpp
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

# Threading library
add_library(nlm_threading STATIC
    src/threading/ThreadSafeSimulationManager.cpp
)
target_include_directories(nlm_threading PUBLIC src)
target_link_libraries(nlm_threading PUBLIC nlm_core)

# Topology library
add_library(nlm_topology STATIC
    src/topology/AdvancedTopologyManager.cpp
)
target_include_directories(nlm_topology PUBLIC src)
target_link_libraries(nlm_topology PUBLIC nlm_core)

# Command system
add_library(nlm_command STATIC
    src/command/Command.cpp
)
target_include_directories(nlm_command PUBLIC src)
target_link_libraries(nlm_command PUBLIC nlm_core)

# Brain
add_library(nlm_brain STATIC
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
)
target_include_directories(nlm_brain PUBLIC src)
target_link_libraries(nlm_brain PUBLIC nlm_core)

# Dynamics
add_library(nlm_dynamics STATIC
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
)
target_include_directories(nlm_dynamics PUBLIC src)
target_link_libraries(nlm_dynamics PUBLIC nlm_core)

# Plasticity
add_library(nlm_plasticity STATIC
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
)
target_include_directories(nlm_plasticity PUBLIC src)
target_link_libraries(nlm_plasticity PUBLIC nlm_core)

# Development
add_library(nlm_development STATIC
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
)
target_include_directories(nlm_development PUBLIC src)
target_link_libraries(nlm_development PUBLIC nlm_core)

# Neuromodulation
add_library(nlm_neuromodulation STATIC
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
)
target_include_directories(nlm_neuromodulation PUBLIC src)
target_link_libraries(nlm_neuromodulation PUBLIC nlm_core)

# Memory
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
target_link_libraries(nlm_memory PUBLIC nlm_core)

# Cognition
add_library(nlm_cognition STATIC
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
)
target_include_directories(nlm_cognition PUBLIC src)
target_link_libraries(nlm_cognition PUBLIC nlm_core)

# Sensory
add_library(nlm_sensory STATIC
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
)
target_include_directories(nlm_sensory PUBLIC src)
target_link_libraries(nlm_sensory PUBLIC nlm_core)

# Prediction
add_library(nlm_prediction STATIC
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
)
target_include_directories(nlm_prediction PUBLIC src)
target_link_libraries(nlm_prediction PUBLIC nlm_core)

# Motor
add_library(nlm_motor STATIC
    src/motor/MotorSystem.cpp
)
target_include_directories(nlm_motor PUBLIC src)
target_link_libraries(nlm_motor PUBLIC nlm_core)

# Environment
add_library(nlm_environment STATIC
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
)
target_include_directories(nlm_environment PUBLIC src)
target_link_libraries(nlm_environment PUBLIC nlm_core)

# Experiments
add_library(nlm_experiments STATIC
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
)
target_include_directories(nlm_experiments PUBLIC src)
target_link_libraries(nlm_experiments PUBLIC nlm_core)

# Visualization
add_library(nlm_visualization STATIC
    src/visualization/VisualizationInterface.cpp
)
target_include_directories(nlm_visualization PUBLIC src)
target_link_libraries(nlm_visualization PUBLIC nlm_core)

# Main executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core nlm_agent nlm_world nlm_threading nlm_topology nlm_command nlm_brain nlm_dynamics nlm_plasticity nlm_development nlm_neuromodulation nlm_memory nlm_cognition nlm_sensory nlm_prediction nlm_motor nlm_environment nlm_experiments nlm_visualization)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_threading nlm_topology nlm_command nlm_brain nlm_dynamics nlm_plasticity nlm_development nlm_neuromodulation nlm_memory nlm_cognition nlm_sensory nlm_prediction nlm_motor nlm_environment nlm_experiments nlm_visualization)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_threading nlm_topology nlm_command nlm_brain nlm_dynamics nlm_plasticity nlm_development nlm_neuromodulation nlm_memory nlm_cognition nlm_sensory nlm_prediction nlm_motor nlm_environment nlm_experiments nlm_visualization)

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)