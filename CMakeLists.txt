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
    # Brain
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    # Dynamics
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    # Plasticity
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
    # Development
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
    # Neuromodulation
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
    # Memory
    src/memory/WorkingMemory.cpp
    src/memory/EpisodicMemory.cpp
    src/memory/SemanticMemory.cpp
    src/memory/ProceduralMemory.cpp
    src/memory/AssociativeMemory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    # Cognition
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
    # Sensory
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    # Prediction
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    # Motor
    src/motor/MotorSystem.cpp
    # Environment
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
    # Experiments
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    # Visualization
    src/visualization/VisualizationInterface.cpp
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

# Core experiment system (integration framework)
add_library(nlm_core_experiment STATIC
    src/core/experiment/CoreExperimentSystem.cpp
)
target_include_directories(nlm_core_experiment PUBLIC src)
target_link_libraries(nlm_core_experiment PUBLIC nlm_core)

# Brain system experiments (integrated from Phase 4)
add_library(nlm_brain_experiments STATIC
    src/brain/experiment/PredictionExperimentIntegration.cpp
)
target_include_directories(nlm_brain_experiments PUBLIC src)
target_link_libraries(nlm_brain_experiments PUBLIC nlm_core nlm_agent)

# Neuromodulation system experiments (integrated from Phase 4)
add_library(nlm_neuromodulation_experiments STATIC
    src/neuromodulation/experiment/NeuromodulationExperimentIntegration.cpp
)
target_include_directories(nlm_neuromodulation_experiments PUBLIC src)
target_link_libraries(nlm_neuromodulation_experiments PUBLIC nlm_core)

# Memory system experiments (integrated from Phase 4)
add_library(nlm_memory_experiments STATIC
    src/memory/experiment/MemoryExperimentIntegration.cpp
)
target_include_directories(nlm_memory_experiments PUBLIC src)
target_link_libraries(nlm_memory_experiments PUBLIC nlm_core nlm_agent)

# Cognition system experiments (integrated from Phase 4)
add_library(nlm_cognition_experiments STATIC
    src/cognition/experiment/CognitiveExperimentIntegration.cpp
)
target_include_directories(nlm_cognition_experiments PUBLIC src)
target_link_libraries(nlm_cognition_experiments PUBLIC nlm_core nlm_agent)

# Performance experiments and benchmarks
add_library(nlm_performance_experiments STATIC
    src/performance/experiment/PerformanceBenchmarks.cpp
)
target_include_directories(nlm_performance_experiments PUBLIC src)
target_link_libraries(nlm_performance_experiments PUBLIC nlm_core)

# Main executable (core system only)
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demonstration (now in tests)
add_executable(nlm_phase3_demo
    src/experiments/tests/BasicWorldInteractionDemo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_core_experiment)

# Phase 4 demonstration (now in tests)
add_executable(nlm_phase4_demo
    src/experiments/tests/AdvancedSystemIntegrationDemo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_core_experiment nlm_brain_experiments nlm_memory_experiments)

# Phase 5 demonstration (now integrated in core)
add_executable(nlm_phase5_demo
    src/experiments/tests/LifetimeLearningDemo.cpp
)
target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_core_experiment nlm_brain_experiments nlm_neuromodulation_experiments)

# Phase 6 demonstration (now integrated in core)
add_executable(nlm_phase6_demo
    src/experiments/tests/FullSystemDemo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_core_experiment nlm_brain_experiments nlm_memory_experiments nlm_cognition_experiments nlm_neuromodulation_experiments)

# Integration tests (Phase 4, 5, 6 specific)
add_executable(nlm_integration_tests
    src/experiments/tests/Phase4IntegrationTest.cpp
    src/experiments/tests/Phase5LifetimeLearningTest.cpp
    src/experiments/tests/Phase6IntegrationTest.cpp
)
target_link_libraries(nlm_integration_tests PUBLIC nlm_core_experiment nlm_brain_experiments nlm_memory_experiments nlm_cognition_experiments nlm_neuromodulation_experiments)

# Comprehensive integration tests framework
add_executable(nlm_comprehensive_integration_tests
    src/tests/integration/MainIntegrationTestRunner.cpp
    src/tests/integration/IntegrationTestFramework.cpp
    src/tests/integration/TestSystemIntegration.cpp
    src/tests/integration/TestMemoryIntegration.cpp
    src/tests/integration/TestCognitionIntegration.cpp
    src/tests/integration/TestNeuromodulationIntegration.cpp
    src/tests/integration/TestDevelopmentIntegration.cpp
    src/tests/integration/TestCrossSystemIntegration.cpp
)
target_link_libraries(nlm_comprehensive_integration_tests
    PUBLIC
    nlm_core
    nlm_agent
    nlm_world
    nlm_core_experiment
    nlm_brain_experiments
    nlm_memory_experiments
    nlm_cognition_experiments
    nlm_neuromodulation_experiments
)
target_include_directories(nlm_comprehensive_integration_tests
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_core_experiment nlm_phase3_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
