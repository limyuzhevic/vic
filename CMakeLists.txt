# cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX CUDA)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# CUDA configuration
if(NOT CMAKE_CUDA_STANDARD)
    set(CMAKE_CUDA_STANDARD 14)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
    set(CMAKE_CUDA_EXTENSIONS OFF)
endif()

# CUDA version requirements
if(CUDA_VERSION VERSION_LESS 11000)
    message(FATAL_ERROR "CUDA version 11.0 or higher is required")
endif()

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

# CUDA compilation flags
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -lineinfo -Xptxas -v")

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library (CPU-only)
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

# CUDA libraries (GPU-accelerated)

# Neuron dynamics CUDA library
add_library(nlm_cuda_neuron STATIC
    src/cuda/NeuronKernels.cu
    src/cuda/NeuronDynamics.cu
)
target_include_directories(nlm_cuda_neuron PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/cuda>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_link_libraries(nlm_cuda_neuron PUBLIC nlm_core)

# Synapse processing CUDA library
add_library(nlm_cuda_synapse STATIC
    src/cuda/SynapseKernels.cu
    src/cuda/SynapseProcessing.cu
)
target_include_directories(nlm_cuda_synapse PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/cuda>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_link_libraries(nlm_cuda_synapse PUBLIC nlm_core)

# Plasticity calculations CUDA library
add_library(nlm_cuda_plasticity STATIC
    src/cuda/PlasticityKernels.cu
    src/cuda/PlasticityCalculations.cu
)
target_include_directories(nlm_cuda_plasticity PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/cuda>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_link_libraries(nlm_cuda_plasticity PUBLIC nlm_core)

# Event queue processing CUDA library
add_library(nlm_cuda_eventqueue STATIC
    src/cuda/EventQueueKernels.cu
    src/cuda/EventQueueProcessing.cu
)
target_include_directories(nlm_cuda_eventqueue PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/cuda>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_link_libraries(nlm_cuda_eventqueue PUBLIC nlm_core)

# Performance optimization CUDA library
add_library(nlm_cuda_performance STATIC
    src/cuda/PerformanceKernels.cu
    src/cuda/PerformanceOptimization.cu
)
target_include_directories(nlm_cuda_performance PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/cuda>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_link_libraries(nlm_cuda_performance PUBLIC nlm_core)

# CUDA memory management library
add_library(nlm_cuda_memory STATIC
    src/cuda/MemoryManager.cu
)
target_include_directories(nlm_cuda_memory PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/cuda>
)
target_link_libraries(nlm_cuda_memory PUBLIC nlm_core)

# Main CPU-GPU integration library
add_library(nlm_cuda_brain STATIC
    src/cuda/CudaBrain.cpp
)
target_include_directories(nlm_cuda_brain PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/cuda>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_link_libraries(nlm_cuda_brain PUBLIC nlm_core nlm_cuda_neuron nlm_cuda_synapse nlm_cuda_plasticity nlm_cuda_eventqueue nlm_cuda_performance nlm_cuda_memory)

# GPU accelerator abstraction library
add_library(nlm_cuda_accelerator STATIC
    src/cuda/CudaAccelerator.cpp
)
target_include_directories(nlm_cuda_accelerator PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/cuda>
)
target_link_libraries(nlm_cuda_accelerator PUBLIC nlm_core)

# Memory transfer manager library
add_library(nlm_cuda_memcpy STATIC
    src/cuda/MemcpyManager.cpp
)
target_include_directories(nlm_cuda_memcpy PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/cuda>
)
target_link_libraries(nlm_cuda_memcpy PUBLIC nlm_core)

# Asynchronous transfer manager library
add_library(nlm_cuda_async TRANSFER
    src/cuda/AsyncTransferManager.cpp
)
target_include_directories(nlm_cuda_async PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/cuda>
)
target_link_libraries(nlm_cuda_async PUBLIC nlm_core)

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

# Main executable (CPU-only for now)
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

# GPU-accelerated main executable
add_executable(nlm_gpu
    src/main.cpp
    src/cuda/GpuMain.cpp
)
target_link_libraries(nlm_gpu PUBLIC nlm_core nlm_cuda_brain nlm_cuda_accelerator)

# Phase 3 demo executable (CPU-only)
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 3 demo executable (GPU-accelerated)
add_executable(nlm_phase3_gpu_demo
    src/experiments/Phase3Demo.cpp
    src/cuda/GpuPhase3Demo.cpp
)
target_link_libraries(nlm_phase3_gpu_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_cuda_brain nlm_cuda_accelerator)

# Phase 4 demo executable (CPU-only)
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 4 demo executable (GPU-accelerated)
add_executable(nlm_phase4_gpu_demo
    src/experiments/Phase4Demo.cpp
    src/cuda/GpuPhase4Demo.cpp
)
target_link_libraries(nlm_phase4_gpu_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_cuda_brain nlm_cuda_accelerator)

# Testing
enable_testing()
add_subdirectory(tests)

# Installation
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(TARGETS nlm_gpu nlm_phase3_gpu_demo nlm_phase4_gpu_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# CUDA configuration options
option(BUILD_WITH_CUDA "Build with CUDA acceleration support" ON)
option(USE_GPU_ACCELERATION "Use GPU acceleration when available" ON)
option(AUTOMATIC_FALLBACK "Automatically fall back to CPU if GPU not available" ON)

# Set CUDA flags based on build configuration
if(BUILD_WITH_CUDA)
    add_definitions(-DNLM_USE_CUDA)
endif()

if(USE_GPU_ACCELERATION)
    add_definitions(-DUSE_GPU_ACCELERATION)
endif()

if(AUTOMATIC_FALLBACK)
    add_definitions(-DAUTOMATIC_FALLBACK)
endif()

# CUDA device query utility
add_executable(cuda_device_query
    src/cuda/CudaDeviceQuery.cpp
)
target_link_libraries(cuda_device_query PUBLIC nlm_core)

# CUDA memory test
add_executable(cuda_memory_test
    src/cuda/CudaMemoryTest.cpp
)
target_link_libraries(cuda_memory_test PUBLIC nlm_core nlm_cuda_memory)

# CUDA performance benchmark
add_executable(cuda_performance_benchmark
    src/cuda/CudaPerformanceBenchmark.cpp
)
target_link_libraries(cuda_performance_benchmark PUBLIC nlm_core nlm_cuda_performance)

# Print CUDA configuration
message(STATUS "NLM Configuration:")
message(STATUS "  CUDA Support: ${BUILD_WITH_CUDA}")
message(STATUS "  GPU Acceleration: ${USE_GPU_ACCELERATION}")
message(STATUS "  Automatic Fallback: ${AUTOMATIC_FALLBACK}")
message(STATUS "  CUDA Version: ${CUDA_VERSION}")
