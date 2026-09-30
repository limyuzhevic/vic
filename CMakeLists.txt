# NLM (Neural Learning Machine) - Enhanced CMake Build System
# Modern CMake configuration with platform detection, dependency management,
# and comprehensive build options for different use cases.

cmake_minimum_required(VERSION 3.16)
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Neural Learning Machine - Experimental artificial developmental brain"
)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Configure output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Python interpreter detection for Python module build
find_package(Python 3.8 REQUIRED COMPONENTS Interpreter Development)

# Include GNUInstallDirs for standard installation directories
include(GNUInstallDirs)

# Platform detection and configuration
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlatformDetection.cmake)

# Dependency management
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Dependencies.cmake)

# Feature options - customizable build configurations
option(NLM_ENABLE_PYTHON_BINDINGS "Enable Python bindings via pybind11" ON)
option(NLM_ENABLE_TESTS "Enable unit tests" ON)
option(NLM_ENABLE_PHASE3_EXPERIMENTS "Enable Phase 3 experiments" ON)
option(NLM_ENABLE_PHASE4_EXPERIMENTS "Enable Phase 4 experiments" ON)
option(NLM_ENABLE_PHASE5_EXPERIMENTS "Enable Phase 5 experiments" ON)
option(NLM_ENABLE_PHASE6_EXPERIMENTS "Enable Phase 6 experiments" ON)
option(NLM_ENABLE_PERFORMANCE_OPTIMIZATIONS "Enable performance optimizations" ON)
option(NLM_ENABLE_COVERAGE "Enable code coverage reporting" OFF)
option(NLM_ENABLE_STATIC_ANALYSIS "Enable static analysis" OFF)

# Configuration options for different use cases
option(NLM_BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(NLM_BUILD_EXAMPLES "Build example applications" ON)
option(NLM_INSTALL_DOCS "Install documentation" ON)
option(NLM_INSTALL_PYTHON_MODULE "Install Python bindings" ON)

# Performance and optimization settings
set(NLM_NUM_CORES_FOR_BUILD "$ENV{JOBS}" CACHE STRING "Number of parallel jobs for build")
if(NLM_NUM_CORES_FOR_BUILD)
    set(CMAKE_JOB_POOLS "compile=${NLM_NUM_CORES_FOR_BUILD}" "link=${NLM_NUM_CORES_FOR_BUILD}" "preprocess=${NLM_NUM_CORES_FOR_BUILD}")
endif()

# Cross-compilation support
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific optimizations
if(MSVC)
    add_compile_options(/W4 /WX /permissive-)
    add_definitions(-DNLM_WINDOWS)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_compile_options(-fPIC)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-ftemplate-depth=256)
    endif()
endif()

# Performance optimization flags
if(NLM_ENABLE_PERFORMANCE_OPTIMIZATIONS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-march=native -flto)
        add_link_options(-flto)
    elseif(MSVC)
        add_compile_options(/O2 /GL /LTCG)
    endif()
endif()

# Coverage configuration
if(NLM_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif()

# Include source directory
set(NLM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Core library - main neural computation engine
add_library(nlm_core
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
    src/memory/WorkingMemory.cpp
    src/memory/EpisodicMemory.cpp
    src/memory/SemanticMemory.cpp
    src/memory/ProceduralMemory.cpp
    src/memory/AssociativeMemory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    src/motor/MotorSystem.cpp
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    src/visualization/VisualizationInterface.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(NLM_ENABLE_PERFORMANCE_OPTIMIZATIONS)
    target_compile_options(nlm_core PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-march=native -flto>
        $<$<CXX_COMPILER_ID:MSVC>:/O2 /GL /LTCG>
    )
endif()

# Agent system - interface between brain and world
add_library(nlm_agent
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system - environment simulation
add_library(nlm_world
    src/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
if(NLM_ENABLE_PHASE3_EXPERIMENTS)
    add_library(nlm_phase3
        src/experiments/Phase3Experiment.cpp
    )
    target_include_directories(nlm_phase3 PUBLIC ${NLM_SOURCE_DIR})
    target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Phase 4 experiments
if(NLM_ENABLE_PHASE4_EXPERIMENTS)
    add_library(nlm_phase4
        src/experiments/Phase4Experiment.cpp
    )
    target_include_directories(nlm_phase4 PUBLIC ${NLM_SOURCE_DIR})
    target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Phase 5 experiments
if(NLM_ENABLE_PHASE5_EXPERIMENTS)
    add_library(nlm_phase5
        src/experiments/Phase5Experiment.cpp
    )
    target_include_directories(nlm_phase5 PUBLIC ${NLM_SOURCE_DIR})
    target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Phase 6 experiments
if(NLM_ENABLE_PHASE6_EXPERIMENTS)
    add_library(nlm_phase6
        src/experiments/Phase6IntegratedExperiment.cpp
    )
    target_include_directories(nlm_phase6 PUBLIC ${NLM_SOURCE_DIR})
    target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Main CLI application
add_executable(nlm
    src/main.cpp
    src/cli/CLIManager.cpp
)

target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
if(NLM_ENABLE_PHASE3_EXPERIMENTS)
    add_executable(nlm_phase3_demo
        src/experiments/Phase3Demo.cpp
    )
    target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
endif()

# Phase 4 demo executable
if(NLM_ENABLE_PHASE4_EXPERIMENTS)
    add_executable(nlm_phase4_demo
        src/experiments/Phase4Demo.cpp
    )
    target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
endif()

# Python bindings
if(NLM_ENABLE_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Testing framework
if(NLM_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Install.cmake)

# Documentation
if(NLM_INSTALL_DOCS)
    add_subdirectory(docs)
endif()

# Export configuration for downstream usage
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ExportConfig.cmake)

# Summary message
message(STATUS "NLM Build Configuration Summary:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Python: ${Python_VERSION}")
message(STATUS "  Python bindings: ${NLM_ENABLE_PYTHON_BINDINGS}")
message(STATUS "  Tests enabled: ${NLM_ENABLE_TESTS}")
message(STATUS "  Phase 3 experiments: ${NLM_ENABLE_PHASE3_EXPERIMENTS}")
message(STATUS "  Phase 4 experiments: ${NLM_ENABLE_PHASE4_EXPERIMENTS}")
message(STATUS "  Phase 5 experiments: ${NLM_ENABLE_PHASE5_EXPERIMENTS}")
message(STATUS "  Phase 6 experiments: ${NLM_ENABLE_PHASE6_EXPERIMENTS}")
message(STATUS "  Performance optimizations: ${NLM_ENABLE_PERFORMANCE_OPTIMIZATIONS}")
message(STATUS "  Output directories:")
message(STATUS "    Binaries: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "    Libraries: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message(STATUS "    Archives: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
