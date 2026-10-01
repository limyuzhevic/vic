# CMake Configuration for NLM (Neural Learning Machine)
# Modern CMake with best practices

cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
strict_cxx(20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific configurations
if(WIN32)
    add_compile_definitions(NLM_WINDOWS)
elseif(APPLE)
    add_compile_definitions(NLM_MACOS)
elseif(UNIX)
    add_compile_definitions(NLM_LINUX)
endif()

# Build type configurations
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Compiler-specific configurations
if(MSVC)
    add_compile_options(/W4 /WX /permissive-)
    add_definitions(-DNOMINMAX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wshadow -Wformat=2 -Wconversion)
    add_definitions(-D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700)
endif()

# Python and pybind11 configuration
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

# Installation directories
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation prefix")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Source directory configuration
set(NLM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(NLM_INCLUDE_DIR ${NLM_SOURCE_DIR})

# Core library components
add_library(nlm_core STATIC)

# Core utilities
add_library(core_types STATIC src/core/Types/Types.cpp)
add_library(core_config STATIC src/core/Config/Config.cpp)
add_library(core_random STATIC src/core/Random/Random.cpp)
add_library(core_logger STATIC src/core/Logger/Logger.cpp)
add_library(core_simulation_clock STATIC src/core/SimulationClock/SimulationClock.cpp)

# Brain components
add_library(brain_neuron STATIC src/brain/Neuron.cpp)
add_library(brain_synapse STATIC src/brain/Synapse.cpp)
add_library(brain_neural_population STATIC src/brain/NeuralPopulation.cpp)
add_library(brain_neural_region STATIC src/brain/NeuralRegion.cpp)
add_library(brain_brain STATIC src/brain/Brain.cpp)

# Neural dynamics
add_library(dynamics_neural_dynamics STATIC src/dynamics/NeuralDynamics.cpp)
add_library(dynamics_spike_system STATIC src/dynamics/SpikeSystem.cpp)

# Plasticity
add_library(plasticity_plasticity_rule STATIC src/plasticity/PlasticityRule.cpp)
add_library(plasticity_hebbian STATIC src/plasticity/Hebbian.cpp)
add_library(plasticity_stdp STATIC src/plasticity/STDP.cpp)
add_library(plasticity_structural_plasticity STATIC src/plasticity/StructuralPlasticity.cpp)

# Development
add_library(development_development_system STATIC src/development/DevelopmentSystem.cpp)
add_library(development_synaptogenesis STATIC src/development/Synaptogenesis.cpp)
add_library(development_pruning STATIC src/development/Pruning.cpp)
add_library(development_maturation STATIC src/development/Maturation.cpp)

# Neuromodulation
add_library(neuromodulation_neuromodulator STATIC src/neuromodulation/Neuromodulator.cpp)
add_library(neuromodulation_reward STATIC src/neuromodulation/Reward.cpp)
add_library(neuromodulation_prediction_error STATIC src/neuromodulation/PredictionError.cpp)
add_library(neuromodulation_novelty STATIC src/neuromodulation/Novelty.cpp)
add_library(neuromodulation_curiosity STATIC src/neuromodulation/Curiosity.cpp)

# Memory systems
add_library(memory_working_memory STATIC src/memory/NeuralWorkingMemory.cpp)
add_library(memory_episodic_memory STATIC src/memory/NeuralEpisodicMemory.cpp)
add_library(memory_associative_memory STATIC src/memory/AssociativeMemory.cpp)
add_library(memory_semantic_memory STATIC src/memory/SemanticMemory.cpp)
add_library(memory_procedural_memory STATIC src/memory/ProceduralMemory.cpp)

# Cognition
add_library(cognition_neural_planner STATIC src/cognition/NeuralPlanner.cpp)
add_library(cognition_concept_formation STATIC src/cognition/ConceptFormation.cpp)

# Sensory
add_library(sensory_sensory_input STATIC src/sensory/SensoryInput.cpp)
add_library(sensory_vision STATIC src/sensory/Vision.cpp)
add_library(sensory_audio STATIC src/sensory/Audio.cpp)
add_library(sensory_internal_signals STATIC src/sensory/InternalSignals.cpp)

# Prediction
add_library(prediction_prediction_system STATIC src/prediction/PredictionSystem.cpp)
add_library(prediction_neural_prediction STATIC src/prediction/NeuralPrediction.cpp)

# Motor
add_library(motor_motor_system STATIC src/motor/MotorSystem.cpp)

# Environment
add_library(environment_environment STATIC src/environment/Environment.cpp)
add_library(environment_observation STATIC src/environment/Observation.cpp)
add_library(environment_action STATIC src/environment/Action.cpp)

# Experiments
add_library(experiments_experiment STATIC src/experiments/Experiment.cpp)
add_library(experiments_experiment_runner STATIC src/experiments/ExperimentRunner.cpp)
add_library(experiments_metrics STATIC src/experiments/Metrics.cpp)

# Phase experiments
add_library(experiments_phase3 STATIC src/experiments/Phase3Experiment.cpp)
add_library(experiments_phase4 STATIC src/experiments/Phase4Experiment.cpp)
add_library(experiments_phase5 STATIC src/experiments/Phase5Experiment.cpp)
add_library(experiments_phase6 STATIC src/experiments/Phase6IntegratedExperiment.cpp)

# Performance optimizations
add_library(performance_simd_vectorization STATIC src/performance/SIMDVectorization.cpp)
add_library(performance_memory_pool STATIC src/performance/MemoryPool.cpp)
add_library(performance_event_queue STATIC src/performance/EventQueue.cpp)
add_library(performance_optimized_brain STATIC src/performance/OptimizedBrain.cpp)
add_library(performance_sparse_connectivity STATIC src/performance/SparseConnectivity.cpp)
add_library(performance_checkpoint_system STATIC src/performance/CheckpointSystem.cpp)

# Visualization
add_library(visualization_visualization_interface STATIC src/visualization/VisualizationInterface.cpp)

# Agent system
add_library(agent_sensory_percept STATIC src/agent/SensoryPercept.cpp)
add_library(agent_agent_brain STATIC src/agent/AgentBrain.cpp)
add_library(agent_agent_body STATIC src/agent/AgentBody.cpp)

# World system
add_library(world_simple_world STATIC src/world/SimpleWorld.cpp)

# Main executable
add_executable(nlm src/main.cpp)

# Main libraries
add_library(nlm_core STATIC
    # Core
    ${CMAKE_CURRENT_BINARY_DIR}/core_types.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/core_config.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/core_random.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/core_logger.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/core_simulation_clock.cpp
    
    # Brain
    ${CMAKE_CURRENT_BINARY_DIR}/brain_neuron.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/brain_synapse.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/brain_neural_population.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/brain_neural_region.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/brain_brain.cpp
    
    # Dynamics
    ${CMAKE_CURRENT_BINARY_DIR}/dynamics_neural_dynamics.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/dynamics_spike_system.cpp
    
    # Plasticity
    ${CMAKE_CURRENT_BINARY_DIR}/plasticity_plasticity_rule.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/plasticity_hebbian.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/plasticity_stdp.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/plasticity_structural_plasticity.cpp
    
    # Development
    ${CMAKE_CURRENT_BINARY_DIR}/development_development_system.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/development_synaptogenesis.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/development_pruning.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/development_maturation.cpp
    
    # Neuromodulation
    ${CMAKE_CURRENT_BINARY_DIR}/neuromodulation_neuromodulator.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/neuromodulation_reward.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/neuromodulation_prediction_error.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/neuromodulation_novelty.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/neuromodulation_curiosity.cpp
    
    # Memory
    ${CMAKE_CURRENT_BINARY_DIR}/memory_working_memory.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/memory_episodic_memory.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/memory_associative_memory.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/memory_semantic_memory.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/memory_procedural_memory.cpp
    
    # Cognition
    ${CMAKE_CURRENT_BINARY_DIR}/cognition_neural_planner.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/cognition_concept_formation.cpp
    
    # Sensory
    ${CMAKE_CURRENT_BINARY_DIR}/sensory_sensory_input.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/sensory_vision.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/sensory_audio.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/sensory_internal_signals.cpp
    
    # Prediction
    ${CMAKE_CURRENT_BINARY_DIR}/prediction_prediction_system.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/prediction_neural_prediction.cpp
    
    # Motor
    ${CMAKE_CURRENT_BINARY_DIR}/motor_motor_system.cpp
    
    # Environment
    ${CMAKE_CURRENT_BINARY_DIR}/environment_environment.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/environment_observation.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/environment_action.cpp
    
    # Experiments
    ${CMAKE_CURRENT_BINARY_DIR}/experiments_experiment.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/experiments_experiment_runner.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/experiments_metrics.cpp
    
    # Visualization
    ${CMAKE_CURRENT_BINARY_DIR}/visualization_visualization_interface.cpp
)

# Python bindings
set(PYBIND11_SRC python/bindings.cpp)
pybind11_add_module(pynlm ${PYBIND11_SRC})

# Agent system libraries
set(AGENT_LIBS agent_sensory_percept agent_agent_brain agent_agent_body)
set(WORLD_LIBS world_simple_world)

# Phase 3 executable
add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
target_link_libraries(nlm_phase3_demo nlm_core ${AGENT_LIBS} ${WORLD_LIBS} experiments_phase3)

# Phase 4 executable
add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)
target_link_libraries(nlm_phase4_demo nlm_core ${AGENT_LIBS} ${WORLD_LIBS} experiments_phase3 experiments_phase4)

# Phase 6 demo executable
add_executable(nlm_phase6_demo src/experiments/Phase6Demo.cpp)
target_link_libraries(nlm_phase6_demo nlm_core ${AGENT_LIBS} ${WORLD_LIBS} experiments_phase6)

# Benchmark executable
add_executable(nlm_benchmark src/benchmark.cpp)
target_link_libraries(nlm_benchmark nlm_core)

# Testing configuration
enable_testing()
add_subdirectory(tests)

# Installation
install(TARGETS nlm nlm_core pynlm
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Export configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Package export for find_package support
export(TARGETS nlm nlm_core pynlm
    FILE ${CMAKE_CURRENT_BINARY_DIR}/NLMTargets.cmake
)

# Configure header for version information
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/nlm_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm_config.h
)

# Summary message
message(STATUS "NLM Configuration Summary:")
message(STATUS "  Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Python found: ${Python_VERSION_STRING}")
message(STATUS "  pybind11 found: ${pybind11_VERSION_STRING}")
message(STATUS "  Core library: ${NLM_SOURCE_DIR}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")