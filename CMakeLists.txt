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
    src/memory/Memory.cpp
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
    src/motor/Action.cpp
    # Environment
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    # Experiments
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    # Visualization
    src/visualization/VisualizationInterface.cpp
    # Performance
    src/performance/PerformanceProfiler.cpp
    src/performance/FunctionCallProfiler.cpp
    src/performance/AlgorithmProfiler.cpp
    src/performance/BottleneckDetector.cpp
    src/performance/MemoryProfiler.cpp
    src/performance/ResourceMonitor.cpp
    src/performance/ScalingAnalyzer.cpp
    src/performance/ParallelPerformanceAnalyzer.cpp
    src/performance/VectorizationAnalyzer.cpp
    src/performance/CacheAnalyzer.cpp
    src/performance/MemoryAccessPatternAnalyzer.cpp
    src/performance/OptimizationRecommender.cpp
)

# Analysis library
add_library(nlm_analysis STATIC
    # Analysis components
    src/analysis/NetworkAnalyzer.cpp
    src/analysis/ActivityAnalyzer.cpp
    src/analysis/PerformanceAnalyzer.cpp
    src/analysis/StateAnalyzer.cpp
    src/analysis/PlottingTools.cpp
)

# Debugging library
add_library(nlm_debug STATIC
    # Debugging components
    src/debug/DebugTrace.cpp
    src/debug/DebuggingSystem.cpp
    src/debug/PerformanceProfiler.cpp
    src/debug/EventLogger.cpp
    src/debug/MemoryProfiler.cpp
    src/debug/DebugVisualizer.cpp
)

target_include_directories(nlm_debug PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_debug PUBLIC nlm_core nlm_analysis)

target_include_directories(nlm_analysis PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Link analysis library to main executables
target_link_libraries(nlm_analysis PUBLIC nlm_core)

# Learning components
add_library(nlm_learning STATIC
    src/learning/AdvancedAgentLearning.cpp
    src/learning/ReinforcementLearning.cpp
    src/learning/SocialLearning.cpp
    src/learning/ExperienceReplay.cpp
    src/learning/TransferLearning.cpp
)

target_include_directories(nlm_learning PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_learning PUBLIC nlm_core nlm_analysis nlm_debug)

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

# Main executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core nlm_learning)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_learning)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_learning)

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo nlm_learning
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)