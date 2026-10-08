cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configuration options
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_DEMOS "Build demo executables" ON)
option(BUILD_EXPERIMENTS "Build experiment libraries" ON)
option(BUILD_RESEARCH_MODELS "Build research-specific modules" OFF)
option(BUILD_PERFORMANCE_OPTIMIZATIONS "Build with performance optimizations" ON)

# Installation prefix
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "Installation prefix")

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

# Performance optimizations
if(BUILD_PERFORMANCE_OPTIMIZATIONS)
    add_compile_options(-O3 -march=native -fomit-frame-pointer)
    add_definitions(-DNLM_PERFORMANCE_OPTIMIZED)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core neural computation library (smallest, most stable core)
add_library(nlm_core STATIC
    # Core utilities
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    # Phase 2: Real neural computation (stable, well-tested)
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Agent system (connects brain to world)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
    src/agent/AgentBody.hpp
)

target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment and agent interaction)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
    src/world/Observation.cpp
    src/environment/Environment.cpp
    src/environment/Action.cpp
)

target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Memory systems (core memory functionality)
add_library(nlm_memory STATIC
    src/memory/WorkingMemory.cpp
    src/memory/EpisodicMemory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
)

target_include_directories(nlm_memory PUBLIC src)
target_link_libraries(nlm_memory PUBLIC nlm_core)

# Neuromodulation systems (learning and adaptation)
add_library(nlm_neuromodulation STATIC
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
)

target_include_directories(nlm_neuromodulation PUBLIC src)
target_link_libraries(nlm_neuromodulation PUBLIC nlm_core)

# Plasticity systems (synaptic learning rules)
add_library(nlm_plasticity STATIC
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
)

target_include_directories(nlm_plasticity PUBLIC src)
target_link_libraries(nlm_plasticity PUBLIC nlm_core)

# Development system (brain maturation and plasticity)
add_library(nlm_development STATIC
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
)

target_include_directories(nlm_development PUBLIC src)
target_link_libraries(nlm_development PUBLIC nlm_core)

# Cognition systems (thinking and planning)
add_library(nlm_cognition STATIC
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
)

target_include_directories(nlm_cognition PUBLIC src)
target_link_libraries(nlm_cognition PUBLIC nlm_core nlm_memory)

# Prediction system (expectation and error)
add_library(nlm_prediction STATIC
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
)

target_include_directories(nlm_prediction PUBLIC src)
target_link_libraries(nlm_prediction PUBLIC nlm_core nlm_neuromodulation)

# Main executable (core simulation)
add_executable(nlm
    src/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core nlm_agent nlm_world nlm_memory nlm_plasticity)

# High-level integration libraries (combining multiple systems)

# Full research brain (all systems integrated)
add_library(nlm_research_brain STATIC
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    src/experiments/Phase6IntegratedExperiment.cpp
)

target_include_directories(nlm_research_brain PUBLIC src)
target_link_libraries(nlm_research_brain PUBLIC
    nlm_core nlm_agent nlm_world nlm_memory nlm_neuromodulation
    nlm_plasticity nlm_development nlm_cognition nlm_prediction
)

# Demo executables (user-friendly demonstrations)

# Phase 2 demo (basic neural computation)
if(BUILD_DEMOS)
    add_executable(nlm_phase2_demo
        src/experiments/Phase2Demo.cpp
    )
    target_link_libraries(nlm_phase2_demo PUBLIC nlm_core nlm_agent nlm_world nlm_plasticity)
endif()

# Phase 6 demo (full integration)
if(BUILD_DEMOS)
    add_executable(nlm_phase6_demo
        src/experiments/Phase6Demo.cpp
    )
    target_link_libraries(nlm_phase6_demo PUBLIC nlm_research_brain)
endif()

# Python bindings
if(BUILD_PYTHON_BINDINGS)
    add_library(pynlm MODULE
        python/bindings.cpp
    )
    
    target_include_directories(pynlm PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/python>
        $<INSTALL_INTERFACE:include>
    )
    
    target_link_libraries(pynlm PUBLIC nlm_core nlm_agent nlm_world nlm_memory nlm_neuromodulation)
    
    # Python-specific settings
    set_target_properties(pynlm PROPERTIES
        PREFIX ""
        SUFFIX ".so"
        POSITION_INDEPENDENT_CODE ON
    )
    
    if(MSVC)
        target_compile_definitions(pynlm PUBLIC "PYBIND11_MODULE_EXPORTED")
    endif()
    
    # Install Python module
    install(TARGETS pynlm
        LIBRARY DESTINATION lib/python/${PROJECT_NAME}
        ARCHIVE DESTINATION lib/python/${PROJECT_NAME}
    )
    
    # Install Python headers
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/pynlm
        DESTINATION include/pynlm
    )
endif()

# Testing
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Research-specific modules (advanced functionality)
if(BUILD_RESEARCH_MODELS)
    add_library(nlm_research_extensions STATIC
        src/experiments/ResearchExtensions.cpp
        src/experiments/AblationSystem.hpp
        src/experiments/ScalingBenchmark.hpp
    )
    
    target_include_directories(nlm_research_extensions PUBLIC src)
    target_link_libraries(nlm_research_extensions PUBLIC nlm_core nlm_research_brain)
endif()

# Installation
install(TARGETS nlm
    RUNTIME DESTINATION bin
)

install(TARGETS nlm_core nlm_agent nlm_world nlm_memory nlm_neuromodulation
         nlm_plasticity nlm_development nlm_cognition nlm_prediction
         nlm_research_brain
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

if(BUILD_DEMOS)
    install(TARGETS nlm_phase2_demo nlm_phase6_demo
        RUNTIME DESTINATION bin
    )
endif()

# Documentation and examples
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    ${CMAKE_CURRENT_SOURCE_DIR}/easy_usage.md
    ${CMAKE_CURRENT_SOURCE_DIR}/HOW_TO_USE.md
    DESTINATION share/doc/${PROJECT_NAME}
)

# Configuration file for build
target_compile_definitions(nlm_core PUBLIC
    NLM_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    NLM_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    NLM_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

# Summary message
message(STATUS "NLM Build Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Python bindings: ${BUILD_PYTHON_BINDINGS}")
message(STATUS "  Tests: ${BUILD_TESTS}")
message(STATUS "  Demos: ${BUILD_DEMOS}")
message(STATUS "  Experiments: ${BUILD_EXPERIMENTS}")
message(STATUS "  Performance optimizations: ${BUILD_PERFORMANCE_OPTIMIZATIONS}")
message(STATUS "  Research modules: ${BUILD_RESEARCH_MODELS}")
message(STATUS "  Installation prefix: ${CMAKE_INSTALL_PREFIX}")