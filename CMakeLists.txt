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

# Core library components
add_library(nlm_core_types STATIC src/core/Types/Types.cpp)
add_library(nlm_core_config STATIC src/core/Config/Config.cpp)
add_library(nlm_core_random STATIC src/core/Random/Random.cpp)
add_library(nlm_core_logger STATIC src/core/Logger/Logger.cpp)
add_library(nlm_core_clock STATIC src/core/SimulationClock/SimulationClock.cpp)

# Brain components
add_library(nlm_brain_neuron STATIC src/brain/Neuron.cpp)
add_library(nlm_brain_synapse STATIC src/brain/Synapse.cpp)
add_library(nlm_brain_population STATIC src/brain/NeuralPopulation.cpp)
add_library(nlm_brain_region STATIC src/brain/NeuralRegion.cpp)
add_library(nlm_brain_core STATIC src/brain/Brain.cpp)

# Dynamics components
add_library(nlm_dynamics_neural STATIC src/dynamics/NeuralDynamics.cpp)
add_library(nlm_dynamics_spike STATIC src/dynamics/SpikeSystem.cpp)

# Plasticity components
add_library(nlm_plasticity_rule STATIC src/plasticity/PlasticityRule.cpp)
add_library(nlm_plasticity_hebbian STATIC src/plasticity/Hebbian.cpp)
add_library(nlm_plasticity_stdp STATIC src/plasticity/STDP.cpp)
add_library(nlm_plasticity_structural STATIC src/plasticity/StructuralPlasticity.cpp)

# Development components
add_library(nlm_dev_system STATIC src/development/DevelopmentSystem.cpp)
add_library(nlm_dev_synaptogenesis STATIC src/development/Synaptogenesis.cpp)
add_library(nlm_dev_pruning STATIC src/development/Pruning.cpp)
add_library(nlm_dev_maturation STATIC src/development/Maturation.cpp)

# Neuromodulation components
add_library(nlm_neuromodulator_base STATIC src/neuromodulation/Neuromodulator.cpp)
add_library(nlm_neuromodulator_reward STATIC src/neuromodulation/Reward.cpp)
add_library(nlm_neuromodulator_predictionerror STATIC src/neuromodulation/PredictionError.cpp)
add_library(nlm_neuromodulator_novelty STATIC src/neuromodulation/Novelty.cpp)
add_library(nlm_neuromodulator_curiosity STATIC src/neuromodulation/Curiosity.cpp)

# Memory components
add_library(nlm_memory_working STATIC src/memory/WorkingMemory.cpp)
add_library(nlm_memory_episodic STATIC src/memory/EpisodicMemory.cpp)
add_library(nlm_memory_semantic STATIC src/memory/SemanticMemory.cpp)
add_library(nlm_memory_procedural STATIC src/memory/ProceduralMemory.cpp)
add_library(nlm_memory_associative STATIC src/memory/AssociativeMemory.cpp)
add_library(nlm_memory_neural_working STATIC src/memory/NeuralWorkingMemory.cpp)
add_library(nlm_memory_neural_episodic STATIC src/memory/NeuralEpisodicMemory.cpp)

# Cognition components
add_library(nlm_cognition_concept STATIC src/cognition/ConceptFormation.cpp)
add_library(nlm_cognition_planner STATIC src/cognition/NeuralPlanner.cpp)

# Sensory components
add_library(nlm_sensory_base STATIC src/sensory/SensoryInput.cpp)
add_library(nlm_sensory_vision STATIC src/sensory/Vision.cpp)
add_library(nlm_sensory_audio STATIC src/sensory/Audio.cpp)
add_library(nlm_sensory_internal STATIC src/sensory/InternalSignals.cpp)

# Prediction components
add_library(nlm_prediction_system STATIC src/prediction/PredictionSystem.cpp)
add_library(nlm_prediction_neural STATIC src/prediction/NeuralPrediction.cpp)

# Motor components
add_library(nlm_motor_system STATIC src/motor/MotorSystem.cpp)

# Environment components
add_library(nlm_env_core STATIC src/environment/Environment.cpp)
add_library(nlm_env_observation STATIC src/environment/Observation.cpp)
add_library(nlm_env_action STATIC src/environment/Action.cpp)

# Experiments components
add_library(nlm_exp_core STATIC src/experiments/Experiment.cpp)
add_library(nlm_exp_runner STATIC src/experiments/ExperimentRunner.cpp)
add_library(nlm_exp_metrics STATIC src/experiments/Metrics.cpp)

# Visualization components
add_library(nlm_viz_interface STATIC src/visualization/VisualizationInterface.cpp)

# Create modular core library that groups all components
generated_file(REMOVE)  # Clean up any generated files
add_library(nlm_core STATIC
    nlm_core_types
    nlm_core_config
    nlm_core_random
    nlm_core_logger
    nlm_core_clock
    nlm_brain_neuron
    nlm_brain_synapse
    nlm_brain_population
    nlm_brain_region
    nlm_brain_core
    nlm_dynamics_neural
    nlm_dynamics_spike
    nlm_plasticity_rule
    nlm_plasticity_hebbian
    nlm_plasticity_stdp
    nlm_plasticity_structural
    nlm_dev_system
    nlm_dev_synaptogenesis
    nlm_dev_pruning
    nlm_dev_maturation
    nlm_neuromodulator_base
    nlm_neuromodulator_reward
    nlm_neuromodulator_predictionerror
    nlm_neuromodulator_novelty
    nlm_neuromodulator_curiosity
    nlm_memory_working
    nlm_memory_episodic
    nlm_memory_semantic
    nlm_memory_procedural
    nlm_memory_associative
    nlm_memory_neural_working
    nlm_memory_neural_episodic
    nlm_cognition_concept
    nlm_cognition_planner
    nlm_sensory_base
    nlm_sensory_vision
    nlm_sensory_audio
    nlm_sensory_internal
    nlm_prediction_system
    nlm_prediction_neural
    nlm_motor_system
    nlm_env_core
    nlm_env_observation
    nlm_env_action
    nlm_exp_core
    nlm_exp_runner
    nlm_exp_metrics
    nlm_viz_interface
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Agent system (body, sensory input, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryInput.cpp
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
