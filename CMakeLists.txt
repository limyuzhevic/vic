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
    # Plugin Framework
    src/plugin/PluginFactory.cpp
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

# CLI Tools

# Core CLI Tools
add_executable(nlm_config
    src/tools/nlm_config/nlm_config.cpp
)
target_link_libraries(nlm_config PUBLIC nlm_core)

add_executable(nlm_profile
    src/tools/nlm_profile/nlm_profile.cpp
)
target_link_libraries(nlm_profile PUBLIC nlm_core)

add_executable(nlm_validate
    src/tools/nlm_validate/nlm_validate.cpp
)
target_link_libraries(nlm_validate PUBLIC nlm_core)

add_executable(nlm_benchmark
    src/tools/nlm_benchmark/nlm_benchmark.cpp
)
target_link_libraries(nlm_benchmark PUBLIC nlm_core)

add_executable(nlm_explore
    src/tools/nlm_explore/nlm_explore.cpp
)
target_link_libraries(nlm_explore PUBLIC nlm_core)

# Plugin Management Tools
add_executable(nlm_plugin
    src/tools/plugin/nlm_plugin/nlm_plugin.cpp
)
target_link_libraries(nlm_plugin PUBLIC nlm_core)

add_executable(nlm_plugins
    src/tools/plugin/nlm_plugins/nlm_plugins.cpp
)
target_link_libraries(nlm_plugins PUBLIC nlm_core)

add_executable(nlm_plugin_info
    src/tools/plugin/nlm_plugin_info/nlm_plugin_info.cpp
)
target_link_libraries(nlm_plugin_info PUBLIC nlm_core)

# Data Analysis Tools
add_executable(nlm_analyze
    src/tools/analysis/nlm_analyze/nlm_analyze.cpp
)
target_link_libraries(nlm_analyze PUBLIC nlm_core)

add_executable(nlm_visualize
    src/tools/analysis/nlm_visualize/nlm_visualize.cpp
)
target_link_libraries(nlm_visualize PUBLIC nlm_core)

add_executable(nlm_export
    src/tools/analysis/nlm_export/nlm_export.cpp
)
target_link_libraries(nlm_export PUBLIC nlm_core)

add_executable(nlm_compare
    src/tools/analysis/nlm_compare/nlm_compare.cpp
)
target_link_libraries(nlm_compare PUBLIC nlm_core)

# System Administration Tools
add_executable(nlm_monitor
    src/tools/system/nlm_monitor/nlm_monitor.cpp
)
target_link_libraries(nlm_monitor PUBLIC nlm_core)

add_executable(nlm_checkpoint
    src/tools/system/nlm_checkpoint/nlm_checkpoint.cpp
)
target_link_libraries(nlm_checkpoint PUBLIC nlm_core)

add_executable(nlm_log_viewer
    src/tools/system/nlm_log_viewer/nlm_log_viewer.cpp
)
target_link_libraries(nlm_log_viewer PUBLIC nlm_core)

add_executable(nlm_stats
    src/tools/system/nlm_stats/nlm_stats.cpp
)
target_link_libraries(nlm_stats PUBLIC nlm_core)

# Performance infrastructure
add_library(nlm_performance STATIC
    # Performance modules
    src/performance/CheckpointSystem.hpp
    src/performance/EventQueue.hpp
    src/performance/MemoryPool.hpp
    src/performance/OptimizedBrain.hpp
    src/performance/ParallelProcessing.hpp
    src/performance/SIMDVectorization.hpp
    src/performance/SparseConnectivity.hpp
    # Advanced performance optimizations
    src/performance/CheckpointOptimizer.hpp
    src/performance/BenchmarkSuite.hpp
    src/performance/ResourceProfiler.hpp
    src/performance/HardwareOptimizer.hpp
    src/performance/PerformanceMonitor.hpp
)

target_include_directories(nlm_performance PUBLIC src)

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
