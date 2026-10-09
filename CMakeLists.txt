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
    src/visualization/AdvancedVisualization.cpp
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

# Performance and optimization system
add_library(nlm_performance STATIC
    src/performance/PerformanceProfiler.cpp
    src/performance/ThreadProfiler.cpp
    src/performance/MemoryProfiler.cpp
    src/performance/CPUProfiler.cpp
    src/performance/IOProfiler.cpp
    src/performance/PerformanceMonitor.cpp
    src/performance/PerformanceStatistics.cpp
    src/performance/NeuralPerformanceAnalyzer.cpp
)
target_include_directories(nlm_performance PUBLIC src)

# Optimized brain implementation
add_library(nlm_optimized_brain STATIC
    src/performance/OptimizedBrain.cpp
)
target_include_directories(nlm_optimized_brain PUBLIC src)
target_link_libraries(nlm_optimized_brain PUBLIC nlm_performance)

# Memory pool system
add_library(nlm_memory_pool STATIC
    src/performance/MemoryPool.cpp
)
target_include_directories(nlm_memory_pool PUBLIC src)
target_link_libraries(nlm_memory_pool PUBLIC nlm_performance)

# SIMD vectorization
add_library(nlm_simd STATIC
    src/performance/SIMDVectorization.cpp
)
target_include_directories(nlm_simd PUBLIC src)
target_link_libraries(nlm_simd PUBLIC nlm_performance)

# Event queue system
add_library(nlm_event_queue STATIC
    src/performance/EventQueue.cpp
)
target_include_directories(nlm_event_queue PUBLIC src)
target_link_libraries(nlm_event_queue PUBLIC nlm_performance)

# Checkpoint system
add_library(nlm_checkpoint_system STATIC
    src/performance/CheckpointSystem.cpp
)
target_include_directories(nlm_checkpoint_system PUBLIC src)
target_link_libraries(nlm_checkpoint_system PUBLIC nlm_performance)

# Parallel processing
add_library(nlm_parallel_processing STATIC
    src/performance/ParallelProcessing.cpp
)
target_include_directories(nlm_parallel_processing PUBLIC src)
target_link_libraries(nlm_parallel_processing PUBLIC nlm_performance)

# Sparse connectivity optimization
add_library(nlm_sparse_connectivity STATIC
    src/performance/SparseConnectivity.cpp
)
target_include_directories(nlm_sparse_connectivity PUBLIC src)
target_link_libraries(nlm_sparse_connectivity PUBLIC nlm_performance)

# Phase 5 performance optimizations
add_library(nlm_phase5 STATIC
    src/plasticity/RewardModulatedSTDP.cpp
    src/neuromodulation/Dopamine.cpp
)
target_include_directories(nlm_phase5 PUBLIC src)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_performance nlm_memory_pool nlm_simd nlm_event_queue)

# Main executable with performance optimizations
target_link_libraries(nlm PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_optimized_brain nlm_memory_pool nlm_simd nlm_event_queue nlm_checkpoint_system nlm_parallel_processing nlm_sparse_connectivity)

# Phase 3 demo with performance optimizations
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_optimized_brain nlm_memory_pool nlm_simd nlm_event_queue nlm_checkpoint_system)

# Phase 4 demo with performance optimizations
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_optimized_brain nlm_memory_pool nlm_simd nlm_event_queue nlm_checkpoint_system)

# Advanced visualization demo with performance optimizations
target_link_libraries(nlm_advanced_viz_demo PUBLIC nlm_core nlm_optimized_brain nlm_memory_pool nlm_simd nlm_event_queue nlm_checkpoint_system)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
