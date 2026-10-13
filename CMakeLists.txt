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

# Core library - Basic types and utilities
add_library(nlm_core_basic STATIC
    src/core/Types/Types.cpp
    src/core/Logger/Logger.cpp
    src/core/Random/Random.cpp
    src/core/SimulationClock/SimulationClock.cpp
)

# Brain library - Core neural components
add_library(nlm_brain_basic STATIC
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
)

# Include directories
target_include_directories(nlm_brain_basic PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Config library - Configuration management
add_library(nlm_config STATIC
    src/core/Config/Config.cpp
    src/core/Config/OptimizedConfig.cpp
)

# Include directories
target_include_directories(nlm_config PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Dynamics library - Neural dynamics
add_library(nlm_dynamics STATIC
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
)

# Include directories
target_include_directories(nlm_dynamics PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Plasticity library - Learning mechanisms
add_library(nlm_plasticity STATIC
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
)

# Include directories
target_include_directories(nlm_plasticity PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Development library - Brain development
add_library(nlm_development STATIC
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
)

# Include directories
target_include_directories(nlm_development PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Neuromodulation library - Learning signals
add_library(nlm_neuromodulation STATIC
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
)

# Include directories
target_include_directories(nlm_neuromodulation PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Memory library - Memory systems
add_library(nlm_memory STATIC
    src/memory/WorkingMemory.cpp
    src/memory/EpisodicMemory.cpp
    src/memory/SemanticMemory.cpp
    src/memory/ProceduralMemory.cpp
    src/memory/AssociativeMemory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
)

# Include directories
target_include_directories(nlm_memory PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Cognition library - Cognitive processes
add_library(nlm_cognition STATIC
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
)

# Include directories
target_include_directories(nlm_cognition PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Sensory library - Sensory processing
add_library(nlm_sensory STATIC
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
)

# Include directories
target_include_directories(nlm_sensory PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Prediction library - Predictive processing
add_library(nlm_prediction STATIC
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
)

# Include directories
target_include_directories(nlm_prediction PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Motor library - Motor control
add_library(nlm_motor STATIC
    src/motor/MotorSystem.cpp
)

# Include directories
target_include_directories(nlm_motor PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Environment library - Environment interaction
add_library(nlm_environment STATIC
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
)

# Include directories
target_include_directories(nlm_environment PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Experiments library - Experimentation
add_library(nlm_experiments STATIC
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
)

# Include directories
target_include_directories(nlm_experiments PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Visualization library - Visualization
add_library(nlm_visualization STATIC
    src/visualization/VisualizationInterface.cpp
)

# Include directories
target_include_directories(nlm_visualization PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Core library - High-level brain integration
add_library(nlm_core STATIC
    src/brain/Brain.cpp
)

# Main core library target that links all basic components
target_link_libraries(nlm_core PUBLIC
    nlm_core_basic
    nlm_brain_basic
    nlm_config
    nlm_dynamics
    nlm_plasticity
    nlm_development
    nlm_neuromodulation
    nlm_memory
    nlm_cognition
    nlm_sensory
    nlm_prediction
    nlm_motor
    nlm_environment
    nlm_experiments
    nlm_visualization
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
target_link_libraries(nlm_agent PUBLIC nlm_core nlm_environment)

# World system (environment, objects, simple world)
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

# Advanced library for enhanced features
add_library(nlm_advanced STATIC
    # Advanced CLI
    src/advanced/cli/AdvancedCLI.cpp
    # Advanced experiment features
    src/advanced/experiments/AdvancedExperimentFeatures.cpp
    # Performance monitoring
    src/advanced/monitoring/PerformanceMonitor.cpp
    # Debugging and profiling
    src/advanced/debugging/DebuggingTools.cpp
    # Configuration management
    src/advanced/batch/ConfigurationManager.cpp
    # Batch processing
    src/advanced/batch/BatchProcessor.cpp
    # Plugin system
    src/advanced/plugins/PluginSystem.cpp
    # Performance optimization
    src/advanced/optimization/PerformanceOptimizer.cpp
)

target_include_directories(nlm_advanced PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Link advanced features to core libraries
target_link_libraries(nlm_advanced PUBLIC
    nlm_core
    nlm_agent
    nlm_world
    nlm_neuromodulation
    nlm_plasticity
    nlm_cognition
)

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core_basic nlm_brain_basic nlm_config nlm_dynamics nlm_plasticity nlm_development nlm_neuromodulation nlm_memory nlm_cognition nlm_sensory nlm_prediction nlm_motor nlm_environment nlm_experiments nlm_visualization nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo nlm_advanced
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
