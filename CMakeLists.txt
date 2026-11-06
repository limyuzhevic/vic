cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# ==============================================================================
# NLM (熙然) - Neural Learning Machine Build Configuration
# ==============================================================================
# This CMake configuration builds the NLM neural simulation framework.
# 
# Build Types:
#   Release   - Optimized builds with minimal debug information (default)
#   Debug     - Unoptimized builds with full debug information  
#   RelWithDebInfo - Optimized with debug symbols
#   MinSizeRel - Optimized for minimum binary size
#
# ==============================================================================

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
    message(STATUS "NLM Build Type: ${CMAKE_BUILD_TYPE}")
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
    message(STATUS "Using MSVC with /W4 warnings")
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    message(STATUS "Using GCC/Clang with -Wall -Wextra -Wpedantic warnings")
endif()

# Python bindings build option
# ==============================================================================
option(NLM_BUILD_PYTHON_BINDINGS "Build the pynlm Python module (requires pybind11)" ON)
if(NLM_BUILD_PYTHON_BINDINGS)
    message(STATUS "Python bindings enabled - will build pynlm module")
else()
    message(STATUS "Python bindings disabled - will skip pynlm module build")
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
    src/performance/CheckpointSystem.cpp
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
    src/motor/Action.cpp
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
target_link_libraries(nlm_world PUBLIC nlm_core nlm_agent)

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

# Phase 5 experiments
add_library(nlm_phase5 STATIC
    src/experiments/Phase5Experiment.cpp
)
target_include_directories(nlm_phase5 PUBLIC src)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 6 experiments
add_library(nlm_phase6 STATIC
    src/experiments/Phase6IntegratedExperiment.cpp
)
target_include_directories(nlm_phase6 PUBLIC src)
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world)

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

# Phase 5 demo executable (Phase 5, added here: pattern probe,
# sandbox run, and checkpoint round-trip for the brain-personality
# step — previously Phase 5 was library-only with no demo).
add_executable(nlm_phase5_demo
    src/experiments/Phase5Demo.cpp
)
target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase5)

# Phase 6 demo executable
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase6)

# Phase 6 text/thought demo (new in Phase 6): text-as-a-sense
# association probe, concept/social probes, and a sandbox run with
# a teacher, planner overrides, and a checkpoint round-trip. The old
# nlm_phase6_demo integration demo is left unchanged.
add_executable(nlm_phase6_text_demo
    src/experiments/Phase6TextDemo.cpp
)
target_link_libraries(nlm_phase6_text_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase5 nlm_phase6)

# Benchmark executable
add_executable(nlm_benchmark
    src/benchmark.cpp
)
target_link_libraries(nlm_benchmark PUBLIC nlm_core)

# Python bindings module (pynlm)
# ==============================================================================
# Python bindings enable high-level interaction with NLM via Python API.
# This is the primary interface for users who want to:
# - Create and simulate neural networks
# - Use NLM as part of Python-based AI systems
# - Integrate with Python data science tools (numpy, pandas, etc.)
if(NLM_BUILD_PYTHON_BINDINGS)
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    find_package(pybind11 CONFIG REQUIRED)
    
    message(STATUS "Building Python module with pybind11 ${pybind11_VERSION}")
    
    # The module is shared, so the static libs it links need PIC.
    set_target_properties(nlm_core nlm_agent nlm_world PROPERTIES
        POSITION_INDEPENDENT_CODE ON)
    
    pybind11_add_module(pynlm python/bindings.cpp)
    target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
    target_include_directories(pynlm PRIVATE src)
    
    install(TARGETS pynlm LIBRARY DESTINATION .)
    
    message(STATUS "Python module pynlm will be installed to ${CMAKE_INSTALL_PREFIX}/")
endif()

# Testing configuration
# ==============================================================================

# Testing configuration
# ==============================================================================
# Comprehensive testing framework for all NLM components:
# - Unit tests for core neural computation (brain, neurons, synapses)
# - Integration tests for complete system functionality
# - Performance benchmarks and regression testing
# - Python bindings testing

enable_testing()
add_subdirectory(tests)

# Installation targets (future releases)
# ==============================================================================
# Installation configuration for NLM components:
# - Core libraries (nlm_core, nlm_agent, nlm_world) for development
# - Demo executables for example usage and verification
# - Python module for Python-based applications
# - Benchmark executable for performance testing

install(TARGETS nlm nlm_core nlm_agent nlm_world
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

# Additional executable components
# ==============================================================================
# User-facing executables for different use cases:
# - nlm: Basic neural computation demonstration (Phase 2)
# - nlm_phase3_demo: World interaction and sensory-motor loops
# - nlm_phase4_demo: Cognitive capabilities and working memory
# - nlm_phase5_demo: Personality traits and sandbox learning
# - nlm_phase6_demo: Full integration of all brain systems
# - nlm_phase6_text_demo: Text processing and concept formation
# - nlm_benchmark: Performance benchmarking

install(TARGETS nlm_phase3_demo nlm_phase4_demo nlm_phase5_demo nlm_phase6_demo nlm_phase6_text_demo
        RUNTIME DESTINATION bin)

install(TARGETS nlm_benchmark
        RUNTIME DESTINATION bin)

message(STATUS "NLM build configuration complete")
message(STATUS "  - Core libraries: nlm_core, nlm_agent, nlm_world")
message(STATUS "  - Phase libraries: nlm_phase3, nlm_phase4, nlm_phase5, nlm_phase6")
message(STATUS "  - Executables: nlm, nlm_phase3_demo, nlm_phase4_demo, nlm_phase5_demo, nlm_phase6_demo, nlm_phase6_text_demo, nlm_benchmark")
if(NLM_BUILD_PYTHON_BINDINGS)
    message(STATUS "  - Python module: pynlm")
endif()
message(STATUS "  - Testing framework: enabled")
message(STATUS "  - Installation targets: configured")
