cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
# =============================================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
# =============================================================================
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type specified, defaulting to Release")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Build configuration options
# =============================================================================
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler warnings
# =============================================================================
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Build directories
# =============================================================================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Source directory setup
# =============================================================================
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories(${NLM_SOURCE_DIR})

# Helper function to add library with source list
# =============================================================================
function(add_nlm_library name)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(ARG "" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    add_library(${name} STATIC ${ARG_SOURCES})
    target_include_directories(${name} PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_features(${name} PUBLIC cxx_std_20)
    
    message(STATUS "Created library: ${name} with ${ARG_SOURCES_COUNT} sources")
endfunction()

# Core library
# =============================================================================
add_nlm_library(nlm_core
    SOURCES
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

# Agent system
# =============================================================================
add_nlm_library(nlm_agent
    SOURCES
        src/agent/SensoryPercept.cpp
        src/agent/AgentBrain.cpp
)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system
# =============================================================================
add_nlm_library(nlm_world
    SOURCES
        src/world/SimpleWorld.cpp
)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase experiments
# =============================================================================
add_nlm_library(nlm_phase3
    SOURCES
        src/experiments/Phase3Experiment.cpp
)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

add_nlm_library(nlm_phase4
    SOURCES
        src/experiments/Phase4Experiment.cpp
)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
# =============================================================================
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

# Demo executables
# =============================================================================
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 6 integration executables
# =============================================================================
if(EXISTS ${NLM_SOURCE_DIR}/experiments/Phase6Demo.cpp)
    add_executable(nlm_phase6_demo
        src/experiments/Phase6Demo.cpp
    )
    target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
    message(STATUS "Phase 6 demo executable enabled")
endif()

if(EXISTS ${NLM_SOURCE_DIR}/experiments/Phase6IntegratedExperiment.cpp)
    add_executable(nlm_phase6_experiment
        src/experiments/Phase6IntegratedExperiment.cpp
    )
    target_link_libraries(nlm_phase6_experiment PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
    message(STATUS "Phase 6 integration experiment executable enabled")
endif()

# Testing configuration
# =============================================================================
enable_testing()

# Configure testing directory
# =============================================================================
add_subdirectory(tests)

# Installation targets
# =============================================================================
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    EXPORT NLMTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(TARGETS nlm nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo nlm_phase6_experiment
    RUNTIME DESTINATION bin
)

# Install headers
# =============================================================================
install(DIRECTORY src/
    DESTINATION include/nlm
    FILES_MATCHING PATTERN "*.hpp"
)

# Generate and install package configuration
# =============================================================================
include(GNUInstallDirs)

# Create config file for package management
# =============================================================================
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    @ONLY
)

# Build summary
# =============================================================================
message(STATUS "========================================")
message(STATUS "NLM Build Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Installation prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Source directory: ${NLM_SOURCE_DIR}")
message(STATUS "========================================")
message(STATUS "Libraries built: nlm_core, nlm_agent, nlm_world, nlm_phase3, nlm_phase4")
message(STATUS "Executables built: nlm, nlm_phase3_demo, nlm_phase4_demo")
if(TARGET nlm_phase6_demo)
    message(STATUS "Executables built: nlm_phase6_demo")
endif()
if(TARGET nlm_phase6_experiment)
    message(STATUS "Executables built: nlm_phase6_experiment")
endif()
message(STATUS "========================================")
