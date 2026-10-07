cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-independent warning configuration
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    # Use compiler-independent warning flags
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Source directory (use relative paths)
set(NLM_SOURCE_DIR src)

# Core component libraries
add_library(nlm_types STATIC src/core/Types/Types.cpp)
add_library(nlm_config STATIC src/core/Config/Config.cpp)
add_library(nlm_random STATIC src/core/Random/Random.cpp)
add_library(nlm_logger STATIC src/core/Logger/Logger.cpp)
add_library(nlm_simulation_clock STATIC src/core/SimulationClock/SimulationClock.cpp)

# Brain component libraries
add_library(nlm_neuron STATIC src/brain/Neuron.cpp)
add_library(nlm_synapse STATIC src/brain/Synapse.cpp)
add_library(nlm_neural_population STATIC src/brain/NeuralPopulation.cpp)
add_library(nlm_neural_region STATIC src/brain/NeuralRegion.cpp)
add_library(nlm_brain STATIC src/brain/Brain.cpp)

# Dynamics component libraries
add_library(nlm_neural_dynamics STATIC src/dynamics/NeuralDynamics.cpp)
add_library(nlm_spike_system STATIC src/dynamics/SpikeSystem.cpp)

# Plasticity component libraries
add_library(nlm_plasticity_rule STATIC src/plasticity/PlasticityRule.cpp)
add_library(nlm_hebbian STATIC src/plasticity/Hebbian.cpp)
add_library(nlm_stdp STATIC src/plasticity/STDP.cpp)
add_library(nlm_structural_plasticity STATIC src/plasticity/StructuralPlasticity.cpp)

# Development component libraries
add_library(nlm_development_system STATIC src/development/DevelopmentSystem.cpp)
add_library(nlm_synaptogenesis STATIC src/development/Synaptogenesis.cpp)
add_library(nlm_pruning STATIC src/development/Pruning.cpp)
add_library(nlm_maturation STATIC src/development/Maturation.cpp)

# Neuromodulation component libraries
add_library(nlm_neuromodulator STATIC src/neuromodulation/Neuromodulator.cpp)
add_library(nlm_reward STATIC src/neuromodulation/Reward.cpp)
add_library(nlm_prediction_error STATIC src/neuromodulation/PredictionError.cpp)
add_library(nlm_novelty STATIC src/neuromodulation/Novelty.cpp)
add_library(nlm_curiosity STATIC src/neuromodulation/Curiosity.cpp)

# Memory component libraries
add_library(nlm_working_memory STATIC src/memory/WorkingMemory.cpp)
add_library(nlm_episodic_memory STATIC src/memory/EpisodicMemory.cpp)
add_library(nlm_semantic_memory STATIC src/memory/SemanticMemory.cpp)
add_library(nlm_procedural_memory STATIC src/memory/ProceduralMemory.cpp)
add_library(nlm_associative_memory STATIC src/memory/AssociativeMemory.cpp)
add_library(nlm_neural_working_memory STATIC src/memory/NeuralWorkingMemory.cpp)
add_library(nlm_neural_episodic_memory STATIC src/memory/NeuralEpisodicMemory.cpp)

# Cognition component libraries
add_library(nlm_concept_formation STATIC src/cognition/ConceptFormation.cpp)
add_library(nlm_neural_planner STATIC src/cognition/NeuralPlanner.cpp)

# Sensory component libraries
add_library(nlm_sensory_input STATIC src/sensory/SensoryInput.cpp)
add_library(nlm_vision STATIC src/sensory/Vision.cpp)
add_library(nlm_audio STATIC src/sensory/Audio.cpp)
add_library(nlm_internal_signals STATIC src/sensory/InternalSignals.cpp)

# Prediction component libraries
add_library(nlm_prediction_system STATIC src/prediction/PredictionSystem.cpp)
add_library(nlm_neural_prediction STATIC src/prediction/NeuralPrediction.cpp)

# Motor component libraries
add_library(nlm_motor_system STATIC src/motor/MotorSystem.cpp)

# Environment component libraries
add_library(nlm_environment STATIC src/environment/Environment.cpp)
add_library(nlm_observation STATIC src/environment/Observation.cpp)

# Experiment component libraries
add_library(nlm_experiment STATIC src/experiments/Experiment.cpp)
add_library(nlm_experiment_runner STATIC src/experiments/ExperimentRunner.cpp)
add_library(nlm_metrics STATIC src/experiments/Metrics.cpp)

# Visualization component libraries
add_library(nlm_visualization STATIC src/visualization/VisualizationInterface.cpp)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC src)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC src)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC src)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC src)

# Unified core library that links all components
add_library(nlm_core STATIC
    $<TARGET_OBJECTS:nlm_types>
    $<TARGET_OBJECTS:nlm_config>
    $<TARGET_OBJECTS:nlm_random>
    $<TARGET_OBJECTS:nlm_logger>
    $<TARGET_OBJECTS:nlm_simulation_clock>
    $<TARGET_OBJECTS:nlm_neuron>
    $<TARGET_OBJECTS:nlm_synapse>
    $<TARGET_OBJECTS:nlm_neural_population>
    $<TARGET_OBJECTS:nlm_neural_region>
    $<TARGET_OBJECTS:nlm_brain>
    $<TARGET_OBJECTS:nlm_neural_dynamics>
    $<TARGET_OBJECTS:nlm_spike_system>
    $<TARGET_OBJECTS:nlm_plasticity_rule>
    $<TARGET_OBJECTS:nlm_hebbian>
    $<TARGET_OBJECTS:nlm_stdp>
    $<TARGET_OBJECTS:nlm_structural_plasticity>
    $<TARGET_OBJECTS:nlm_development_system>
    $<TARGET_OBJECTS:nlm_synaptogenesis>
    $<TARGET_OBJECTS:nlm_pruning>
    $<TARGET_OBJECTS:nlm_maturation>
    $<TARGET_OBJECTS:nlm_neuromodulator>
    $<TARGET_OBJECTS:nlm_reward>
    $<TARGET_OBJECTS:nlm_prediction_error>
    $<TARGET_OBJECTS:nlm_novelty>
    $<TARGET_OBJECTS:nlm_curiosity>
    $<TARGET_OBJECTS:nlm_working_memory>
    $<TARGET_OBJECTS:nlm_episodic_memory>
    $<TARGET_OBJECTS:nlm_semantic_memory>
    $<TARGET_OBJECTS:nlm_procedural_memory>
    $<TARGET_OBJECTS:nlm_associative_memory>
    $<TARGET_OBJECTS:nlm_neural_working_memory>
    $<TARGET_OBJECTS:nlm_neural_episodic_memory>
    $<TARGET_OBJECTS:nlm_concept_formation>
    $<TARGET_OBJECTS:nlm_neural_planner>
    $<TARGET_OBJECTS:nlm_sensory_input>
    $<TARGET_OBJECTS:nlm_vision>
    $<TARGET_OBJECTS:nlm_audio>
    $<TARGET_OBJECTS:nlm_internal_signals>
    $<TARGET_OBJECTS:nlm_prediction_system>
    $<TARGET_OBJECTS:nlm_neural_prediction>
    $<TARGET_OBJECTS:nlm_motor_system>
    $<TARGET_OBJECTS:nlm_environment>
    $<TARGET_OBJECTS:nlm_observation>
    $<TARGET_OBJECTS:nlm_experiment>
    $<TARGET_OBJECTS:nlm_experiment_runner>
    $<TARGET_OBJECTS:nlm_metrics>
    $<TARGET_OBJECTS:nlm_visualization>
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Set up component dependencies
target_link_libraries(nlm_brain PUBLIC nlm_types nlm_config nlm_random nlm_logger nlm_neuron nlm_synapse nlm_neural_population nlm_neural_region nlm_neural_dynamics nlm_spike_system nlm_plasticity_rule nlm_hebbian nlm_stdp nlm_structural_plasticity)
target_link_libraries(nlm_development_system PUBLIC nlm_brain nlm_synaptogenesis nlm_pruning nlm_maturation)
target_link_libraries(nlm_neuromodulator PUBLIC nlm_brain nlm_reward nlm_prediction_error nlm_novelty nlm_curiosity)
target_link_libraries(nlm_prediction_system PUBLIC nlm_brain nlm_neural_prediction)
target_link_libraries(nlm_motor_system PUBLIC nlm_brain)
target_link_libraries(nlm_environment PUBLIC nlm_brain nlm_observation)
target_link_libraries(nlm_experiment PUBLIC nlm_environment nlm_experiment_runner nlm_metrics)

# Main executable
add_executable(nlm src/main.cpp)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)
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
