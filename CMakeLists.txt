# NLM (Neural Learning Machine) Build System
# Modern CMake configuration with best practices

cmake_minimum_required(VERSION 3.16)
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "An Experimental Artificial Developmental Brain"
    HOMEPAGE_URL "https://github.com/nlm-project/nlm"
)

# C++20 standard with strict compliance
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Enable warnings and position independent code
if(MSVC)
    add_compile_options(/W4 /permissive-)
    add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_definitions(-DNOMINMAX)
endif()

# Position Independent Code for shared libraries
if(UNIX)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Set project-specific compile definitions
add_definitions(-DNLM_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_definitions(-DNLM_VERSION_MINOR=${PROJECT_VERSION_MINOR})
add_definitions(-DNLM_VERSION_PATCH=${PROJECT_VERSION_PATCH})

# Create build configurations for different build types
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")

# Source directory setup
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(NLM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Library and executable output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Package configuration for find_package support
include(GNUInstallDirs)

# Core library - the brain computation engine
add_library(nlm_core STATIC
    # Core system components
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    
    # Brain components
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    
    # Neural dynamics and spike management
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    
    # Learning and plasticity mechanisms
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
    
    # Developmental system
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
    
    # Neuromodulation system
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
    
    # Cognitive functions
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
    
    # Sensory processing
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    
    # Prediction and planning
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    
    # Motor system
    src/motor/MotorSystem.cpp
    
    # Environment simulation
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
    
    # Experimental framework
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    
    # Visualization interface
    src/visualization/VisualizationInterface.cpp
)

# Target properties for core library
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(nlm_core PUBLIC cxx_std_20)

# Agent system - brain-to-world interface
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC ${NLM_INCLUDE_DIR})

target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system - environment simulation
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC ${NLM_INCLUDE_DIR})

target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experimental library
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC ${NLM_INCLUDE_DIR})

target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experimental library
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)


target_include_directories(nlm_phase4 PUBLIC ${NLM_INCLUDE_DIR})


target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Main simulation executable (Phase 2 - basic neural computation)
add_executable(nlm
    src/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demonstration executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)

target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demonstration executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)


target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 6 integration demo executable (requires all components)
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)


target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Optional: Python bindings build
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)

# Python module for NLM
add_library(pynlm MODULE
    python/bindings.cpp
)

target_include_directories(pynlm PRIVATE
    ${Python3_INCLUDE_DIRS}
    ${PYBIND11_INCLUDE_DIR}
    ${NLM_INCLUDE_DIR}
)

target_link_libraries(pynlm PRIVATE
    nlm_core
    pybind11::module
)

# Set module properties
set_target_properties(pynlm PROPERTIES
    PREFIX ""
    SUFFIX ".so"
    OUTPUT_NAME "pynlm"
)

# Installation targets
include(GNUInstallDirs)

# Install core libraries
install(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install executables
install(TARGETS nlm nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install Python module
install(TARGETS pynlm
    LIBRARY DESTINATION ${Python3_SITEARCH}
)

# Install headers
install(DIRECTORY ${NLM_INCLUDE_DIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

# Install configuration files
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/default.cfg
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/nlm
)

# Create a package config file for find_package support
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

install(EXPORT NLMTargets
    FILE NLMTargets.cmake
    NAMESPACE NLM::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# Testing setup
enable_testing()

# Add test directory if it exists
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests)
    add_subdirectory(tests)
endif()

# Build options
option(NLM_BUILD_EXAMPLES "Build example demonstrations" ON)
option(NLM_BUILD_TESTS "Build test suite" ON)
option(NLM_BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(NLM_ENABLE_VISUALIZATION "Enable visualization support" OFF)

# Summary message
message(STATUS "NLM Build Configuration")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Python bindings: ${NLM_BUILD_PYTHON_BINDINGS}")
message(STATUS "  Examples: ${NLM_BUILD_EXAMPLES}")
message(STATUS "  Tests: ${NLM_BUILD_TESTS}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
