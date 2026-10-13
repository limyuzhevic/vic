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
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Core library
add_library(nlm_core STATIC
    # Core
    ${NLM_SOURCE_DIR}/src/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/src/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/src/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/src/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/src/core/SimulationClock/SimulationClock.cpp
    # Brain
    ${NLM_SOURCE_DIR}/src/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/src/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/src/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/src/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/src/brain/Brain.cpp
    # Dynamics
    ${NLM_SOURCE_DIR}/src/dynamics/NeuralDynamics.cpp
    ${NLM_SOURCE_DIR}/src/dynamics/SpikeSystem.cpp
    # Plasticity
    ${NLM_SOURCE_DIR}/src/plasticity/PlasticityRule.cpp
    ${NLM_SOURCE_DIR}/src/plasticity/Hebbian.cpp
    ${NLM_SOURCE_DIR}/src/plasticity/STDP.cpp
    ${NLM_SOURCE_DIR}/src/plasticity/StructuralPlasticity.cpp
    # Development
    ${NLM_SOURCE_DIR}/src/development/DevelopmentSystem.cpp
    ${NLM_SOURCE_DIR}/src/development/Synaptogenesis.cpp
    ${NLM_SOURCE_DIR}/src/development/Pruning.cpp
    ${NLM_SOURCE_DIR}/src/development/Maturation.cpp
    # Neuromodulation
    ${NLM_SOURCE_DIR}/src/neuromodulation/Neuromodulator.cpp
    ${NLM_SOURCE_DIR}/src/neuromodulation/Reward.cpp
    ${NLM_SOURCE_DIR}/src/neuromodulation/PredictionError.cpp
    ${NLM_SOURCE_DIR}/src/neuromodulation/Novelty.cpp
    ${NLM_SOURCE_DIR}/src/neuromodulation/Curiosity.cpp
    # Memory
    ${NLM_SOURCE_DIR}/src/memory/Memory.cpp
    ${NLM_SOURCE_DIR}/src/memory/NeuralWorkingMemory.cpp
    ${NLM_SOURCE_DIR}/src/memory/NeuralEpisodicMemory.cpp
    # Cognition
    ${NLM_SOURCE_DIR}/src/cognition/ConceptFormation.cpp
    ${NLM_SOURCE_DIR}/src/cognition/NeuralPlanner.cpp
    # Sensory
    ${NLM_SOURCE_DIR}/src/sensory/SensoryInput.cpp
    ${NLM_SOURCE_DIR}/src/sensory/Vision.cpp
    ${NLM_SOURCE_DIR}/src/sensory/Audio.cpp
    ${NLM_SOURCE_DIR}/src/sensory/InternalSignals.cpp
    # Prediction
    ${NLM_SOURCE_DIR}/src/prediction/PredictionSystem.cpp
    ${NLM_SOURCE_DIR}/src/prediction/NeuralPrediction.cpp
    # Motor
    ${NLM_SOURCE_DIR}/src/motor/MotorSystem.cpp
    ${NLM_SOURCE_DIR}/src/motor/Action.cpp
    # Environment
    ${NLM_SOURCE_DIR}/src/environment/Environment.cpp
    ${NLM_SOURCE_DIR}/src/environment/Observation.cpp
    # Experiments
    ${NLM_SOURCE_DIR}/src/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/src/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/src/experiments/Metrics.cpp
    # Visualization
    ${NLM_SOURCE_DIR}/src/visualization/VisualizationInterface.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Phase 6 demo executable (Integration Test)
add_executable(nlm_phase6_demo
    ${NLM_SOURCE_DIR}/src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core)
