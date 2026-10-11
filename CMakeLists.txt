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

# Find nlohmann/json
find_package(nlohmann_json 3.2.0 REQUIRED)

# Enable performance optimizations by default
if(NOT DEFINED NLM_PERFORMANCE_ENABLED)
    set(NLM_PERFORMANCE_ENABLED ON)
endif()

# Enable visualization support
if(NOT DEFINED NLM_VISUALIZATION_ENABLED)
    set(NLM_VISUALIZATION_ENABLED ON)
endif()

# Performance optimization options
option(NLM_PERFORMANCE_ENABLED "Enable performance optimizations (memory pools, SOA, event queues)" ON)
option(NLM_VISUALIZATION_ENABLED "Enable visualization support" ON)
option(NLM_ENABLE_PROFILING "Enable detailed performance profiling" OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Compiler flags for optimization
add_compile_options(
    $<$<CXX_COMPILER_ID:GNU,Clang>:-O3 -march=native -mtune=native>
    $<$<CXX_COMPILER_ID:MSVC>:/O2 /arch:AVX2>
)

# Optimization flags
if(MSVC)
    add_compile_options(/fp:fast /vec-report2 /vec:vector
                         /Qvec-report2 /Qvec-report3 /Qvec-report4
                         /Qipo-report-all /Qipo-report-detailed /Qipo-report-unrolled /Qipo-report-subsumed)
else()
    add_compile_options(-ffast-math -funroll-loops -fomit-frame-pointer
                        -fprofile-generate -fprofile-use
                        -falign-functions=32 -falign-loops=32 -falign-jumps=32
                        -flto -fipa-sra -fcommon -fno-protect-parity
                        -Wframe-larger-than=4096)
endif()

# SIMD optimization flags
add_compile_options(
    $<$<CXX_COMPILER_ID:GNU,Clang>:-msse4.1 -mavx -mavx2>
    $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
)

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

target_link_libraries(nlm_core PUBLIC nlohmann_json::nlohmann_json)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Threading and parallelism options
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

# Parallel build options
include(ProcessorCount)
ProcessorCount(NLM_THREAD_COUNT)
if(NLM_THREAD_COUNT EQUAL 0)
    set(NLM_THREAD_COUNT 4)
endif()

# Add performance and visualization directories
set(NLM_PERFORMANCE_SOURCES
    src/performance/MemoryPool.cpp
    src/performance/EventQueue.cpp
    src/performance/SparseConnectivity.cpp
    src/performance/ParallelProcessing.cpp
    src/performance/SIMDVectorization.cpp
)

set(NLM_VISUALIZATION_SOURCES
    src/visualization/VisualizationInterface.cpp
)

# Add performance libraries if enabled
if(NLM_PERFORMANCE_ENABLED)
    add_library(nlm_performance STATIC ${NLM_PERFORMANCE_SOURCES})
    target_include_directories(nlm_performance PUBLIC src/performance)
    target_compile_definitions(nlm_performance PUBLIC NLM_PERFORMANCE_ENABLED)
    
    if(NLM_ENABLE_PROFILING)
        target_compile_definitions(nlm_performance PUBLIC NLM_PROFILING_ENABLED)
    endif()
endif()

# Add visualization library if enabled
if(NLM_VISUALIZATION_ENABLED)
    add_library(nlm_visualization STATIC ${NLM_VISUALIZATION_SOURCES})
    target_include_directories(nlm_visualization PUBLIC src/visualization)
    target_compile_definitions(nlm_visualization PUBLIC NLM_VISUALIZATION_ENABLED)
endif()

# Update core library to link with performance and visualization
if(NLM_PERFORMANCE_ENABLED)
    target_link_libraries(nlm_core PUBLIC nlm_performance)
endif()

if(NLM_VISUALIZATION_ENABLED)
    target_link_libraries(nlm_core PUBLIC nlm_visualization)
endif()

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

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)