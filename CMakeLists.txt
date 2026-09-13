cmake_minimum_required(VERSION 3.19)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configuration options
option(NLM_ENABLE_PERFORMANCE_OPTIMIZATION "Enable performance optimizations" ON)
option(NLM_ENABLE_HYBRID_ACCELERATION "Enable hybrid CPU/GPU acceleration" OFF)
option(NLM_ENABLE_RESEARCH_FEATURES "Enable research-specific features" ON)
option(NLM_ENABLE_PARALLEL_PROCESSING "Enable parallel processing capabilities" ON)
option(NLM_ENABLE_MEMORY_POOLS "Enable memory pool allocators" ON)
option(NLM_ENABLE_EVENT_QUEUE "Enable lock-free event queue" ON)
option(NLM_ENABLE_SANITIZERS "Enable address and memory sanitizers for development" OFF)
option(NLM_ENABLE_CODE_COVERAGE "Enable code coverage reporting" OFF)
option(NLM_BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)

# Build type configuration
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Build type: Debug")
    add_compile_options(-g -O0 -DDEBUG)
    if(NLM_ENABLE_SANITIZERS)
        add_compile_options(-fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
    endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Build type: Release")
    add_compile_options(-O3 -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    message(STATUS "Build type: RelWithDebInfo")
    add_compile_options(-O2 -g -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    message(STATUS "Build type: MinSizeRel")
    add_compile_options(-Os -DNDEBUG)
endif()

# Compiler warnings configuration
if(MSVC)
    add_compile_options(/W4 /WX)
    if(CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 1910)
        add_compile_options(/permissive-)
    endif()
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_compile_options(-Wshadow -Wformat=2 -Wconversion -Wsign-conversion)
    add_compile_options(-Wnull-dereference -Wdouble-promotion -Wtrampolines)
endif()

# Platform-specific configurations
if(UNIX AND NOT APPLE)
    add_compile_options(-fPIC -fstack-protector-strong)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(APPLE)
    add_compile_options(-fstack-protector-strong)
endif()

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/stackcheck)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Performance infrastructure configuration
if(NLM_ENABLE_PERFORMANCE_OPTIMIZATION)
    add_compile_options(-march=native -flto)
    if(MSVC)
        add_compile_options(/openmp /fp:fast)
    else()
        add_compile_options(-fopenmp -ffast-math -mavx2)
    endif()
endif()

if(NLM_ENABLE_MEMORY_POOLS)
    add_definitions(-DNLM_USE_MEMORY_POOL)
endif()

if(NLM_ENABLE_EVENT_QUEUE)
    add_definitions(-DNLM_USE_EVENT_QUEUE)
endif()

if(NLM_ENABLE_RESEARCH_FEATURES)
    add_definitions(-DNLM_RESEARCH_ENABLED)
endif()

# Research system configuration
if(NLM_ENABLE_RESEARCH_FEATURES)
    message(STATUS "Research features: ENABLED")
    add_definitions(-DNLM_RESEARCH_ENABLED)
    
    # Research libraries
    add_library(nlm_research STATIC
        src/research/ResearchConfig.cpp
        src/research/ResearchWorkflow.cpp
        src/research/ExperimentManager.cpp
        src/research/ParallelBatchProcessor.cpp
    )
    target_include_directories(nlm_research PUBLIC src)
    target_link_libraries(nlm_research PUBLIC nlm_core nlm_agent nlm_world)
    
    # Phase 6 integration library
    add_library(nlm_phase6 STATIC
        src/experiments/Phase6IntegratedExperiment.cpp
    )
    target_include_directories(nlm_phase6 PUBLIC src)
    target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_research)
    
    # Research tools and utilities
    add_executable(nlm_research_config
        src/research/config_tool.cpp
    )
    target_link_libraries(nlm_research_config PUBLIC nlm_core nlm_research)
    
    add_executable(nlm_research_workflow
        src/research/workflow_tool.cpp
    )
    target_link_libraries(nlm_research_workflow PUBLIC nlm_core nlm_research)
else()
    message(STATUS "Research features: DISABLED")
endif()

if(NLM_ENABLE_PARALLEL_PROCESSING)
    add_definitions(-DNLM_PARALLEL_PROCESSING)
    add_compile_options(-pthread)
    find_package(Threads REQUIRED)
    target_link_libraries(nlm_core PUBLIC Threads::Threads)
endif()

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
    # Research
    src/research/ResearchConfig.cpp
    src/research/ResearchWorkflow.cpp
    src/research/ExperimentManager.cpp
    src/research/ParallelBatchProcessor.cpp
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

# Phase 6 demo executable
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase6 nlm_research)

# Phase 5 performance experiments (Phase 5 optimized builds)
if(NLM_ENABLE_PERFORMANCE_OPTIMIZATION)
    add_library(nlm_performance STATIC
        src/experiments/Phase5Experiment.cpp
    )
    target_include_directories(nlm_performance PUBLIC src)
    target_link_libraries(nlm_performance PUBLIC nlm_core nlm_agent nlm_world)
    
    add_executable(nlm_phase5_perf
        src/experiments/benchmark.cpp
    )
    target_link_libraries(nlm_phase5_perf PUBLIC nlm_core nlm_performance)
endif()

# Performance benchmarking tools
add_executable(nlm_benchmark
    src/benchmark.cpp
)
target_link_libraries(nlm_benchmark PUBLIC nlm_core)

# Research tools
add_executable(nlm_research_config
    src/research/config_tool.cpp
)
target_link_libraries(nlm_research_config PUBLIC nlm_core nlm_research)

add_executable(nlm_research_workflow
    src/research/workflow_tool.cpp
)
target_link_libraries(nlm_research_workflow PUBLIC nlm_core nlm_research)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
