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

# Performance optimization options
option(NLM_PERFORMANCE_ENABLED "Enable performance optimizations (Phase 5)" ON)
option(NLM_PERFORMANCE_LEVEL "Performance optimization level (LOW, MEDIUM, HIGH)" "HIGH")
option(NLM_ENABLE_SIMD "Enable SIMD vectorization" ON)
option(NLM_ENABLE_MULTITHREADING "Enable multithreading" ON)
option(NLM_ENABLE_MEMORY_POOLS "Enable memory pools" ON)
option(NLM_ENABLE_SOA "Enable Structure-of-Arrays layout" ON)
option(NLM_ENABLE_EVENT_QUEUE "Enable event queue optimization" ON)
option(NLM_ENABLE_SPARSE_CONNECTIVITY "Enable sparse connectivity" ON)
option(NLM_NUM_THREADS "Number of threads for parallel processing (0 = auto-detect)" 0)

# SIMD and architecture detection
if(NLM_ENABLE_SIMD)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
            execute_process(
                COMMAND ${CMAKE_CXX_COMPILER} -dM -E - < /dev/null
                OUTPUT_VARIABLE COMPILER_DEFINES
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            if("${COMPILER_DEFINES}" MATCHES "__AVX512F__")
                add_definitions(-DNLM_SIMD_AVX512)
                message(STATUS "AVX-512 SIMD detected and enabled")
            elseif("${COMPILER_DEFINES}" MATCHES "__AVX2__")
                add_definitions(-DNLM_SIMD_AVX2)
                message(STATUS "AVX2 SIMD detected and enabled")
            elseif("${COMPILER_DEFINES}" MATCHES "__SSE4_1__")
                add_definitions(-DNLM_SIMD_SSE)
                message(STATUS "SSE SIMD detected and enabled")
            else()
                message(STATUS "No SIMD extensions detected, falling back to scalar")
            endif()
        else()
            message(STATUS "Non-x86 architecture detected, using scalar operations")
        endif()
    endif()
endif()

# Thread count configuration
if(NLM_NUM_THREADS EQUAL 0)
    include(ProcessorCount)
    ProcessorCount(NLM_THREAD_COUNT)
else()
    set(NLM_THREAD_COUNT ${NLM_NUM_THREADS})
endif()
if(NLM_THREAD_COUNT GREATER 1)
    add_definitions(-DNLM_THREAD_COUNT=${NLM_THREAD_COUNT})
    message(STATUS "Multithreading enabled with ${NLM_THREAD_COUNT} threads")
endif()

# Performance level configuration
if(NLM_PERFORMANCE_LEVEL STREQUAL "LOW")
    add_definitions(-DNLM_PERFORMANCE_LOW)
elseif(NLM_PERFORMANCE_LEVEL STREQUAL "MEDIUM")
    add_definitions(-DNLM_PERFORMANCE_MEDIUM)
elseif(NLM_PERFORMANCE_LEVEL STREQUAL "HIGH")
    add_definitions(-DNLM_PERFORMANCE_HIGH)
endif()

# Enable performance optimizations
if(NLM_PERFORMANCE_ENABLED)
    add_definitions(-DNLM_PERFORMANCE_ENABLED)
    message(STATUS "Performance optimizations enabled")
    
    if(NLM_ENABLE_MEMORY_POOLS)
        add_definitions(-DNLM_USE_MEMORY_POOL=1)
        message(STATUS "Memory pools enabled")
    endif()
    
    if(NLM_ENABLE_SOA)
        add_definitions(-DNLM_USE_SOA=1)
        message(STATUS "Structure-of-Arrays layout enabled")
    endif()
    
    if(NLM_ENABLE_EVENT_QUEUE)
        add_definitions(-DNLM_USE_EVENT_QUEUE=1)
        message(STATUS "Event queue optimization enabled")
    endif()
    
    if(NLM_ENABLE_SPARSE_CONNECTIVITY)
        add_definitions(-DNLM_USE_SPARE_CONNECTIVITY=1)
        message(STATUS "Sparse connectivity enabled")
    endif()
    
    if(NLM_ENABLE_MULTITHREADING AND NLM_THREAD_COUNT GREATER 1)
        add_definitions(-DNLM_USE_MULTITHREADING=1)
        message(STATUS "Multithreading enabled")
    endif()
    
    if(NLM_ENABLE_SIMD)
        add_definitions(-DNLM_USE_SIMD=1)
        message(STATUS "SIMD vectorization enabled")
    endif()
else()
    message(STATUS "Performance optimizations disabled")
endif()

# Performance statistics collection
option(NLM_PERFORMANCE_STATS "Collect performance statistics" ON)
if(NLM_PERFORMANCE_STATS)
    add_definitions(-DNLM_PERFORMANCE_STATS)
    message(STATUS "Performance statistics collection enabled")
endif()

# Core library with performance optimizations
add_library(nlm_core STATIC
    # Core
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    # Brain with performance optimizations
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    # Performance integration
    src/performance/MemoryPool.cpp
    src/performance/SIMDVectorization.cpp
    # Dynamics with performance
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
    # Memory systems
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
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/performance>
    $<INSTALL_INTERFACE:include>
)

# Performance-specific compile options
if(NLM_PERFORMANCE_ENABLED)
    target_compile_options(nlm_core PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-O3 -march=native>
        $<$<CXX_COMPILER_ID:MSVC>:/O2 /arch:AVX2>
    )
    
    # Position independent code for better linking
    set_target_properties(nlm_core PROPERTIES POSITION_INDEPENDENT_CODE ON)
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
