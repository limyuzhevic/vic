cmake_minimum_required(VERSION 3.19)  # Increased from 3.16 for better C++20 support
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Neural Learning Machine - Experimental Artificial Developmental Brain"
)

# Set project properties
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use GNUInstallDirs for standard installation directories
include(GNUInstallDirs)

# Configure build types with recommended compiler flags
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# Enhanced warning configuration based on compiler
if(MSVC)
    # MSVC warnings
    add_compile_options(
        /W4
        /WX  # Treat warnings as errors
        /permissive-  # Disable permissive mode
    )
    # Disable specific warnings that are not critical
    add_compile_options(/wd4251 /wd4275 /wd4267)
else()
    # GCC/Clang warnings
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror  # Treat warnings as errors
        -Wshadow
        -Wformat=2
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wduplicated-branches
        -Wlogical-op
        -Wuseless-cast
        -Wdouble-promotion
        -Wheader-hygiene
        -Wmissing-include-dirs
        -Wreorder
        -Wdocumentation
        -Wdeprecated-declarations
        -Wimplicit-fallthrough
    )
    
    # Position independent code for static libraries (better for sharing)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Source directory configuration
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" CACHE PATH "Source directory for NLM")

# Find required packages
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# Core library with modern CMake practices
add_library(nlm_core STATIC
    # Core utilities
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
    
    # Neural dynamics
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    
    # Plasticity rules
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
    
    # Development system
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
    
    # Cognitive systems
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
    
    # Sensory systems
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    
    # Prediction systems
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    
    # Motor system
    src/motor/MotorSystem.cpp
    
    # Environment interface
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
    
    # Experiment framework
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    
    # Visualization
    src/visualization/VisualizationInterface.cpp
)

# Core library include directories
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Compiler-specific optimizations based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(nlm_core PRIVATE -O3 -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(nlm_core PRIVATE -O0 -g -DDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_compile_options(nlm_core PRIVATE -O2 -g -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    target_compile_options(nlm_core PRIVATE -Os -DNDEBUG)
endif()

# Add C++17 compatibility flags for older compilers
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
    target_compile_options(nlm_core PRIVATE -std=gnu++17)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8)
    target_compile_options(nlm_core PRIVATE -std=libc++17)
endif()

# Agent system
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

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

# Python bindings (via pybind11 using scikit-build)
# Note: This would typically be handled by pyproject.toml and scikit-build
# For manual CMake integration, you would add:
# find_package(pybind11 REQUIRED)
# pybind11_add_module(pynlm src/python_bindings.cpp)

# Testing framework
enable_testing()

# Add tests directory
add_subdirectory(tests)

# Installation targets
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/nlm)

install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(TARGETS nlm_phase3_demo nlm_phase4_demo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*/tests/*" EXCLUDE
)

# Create config file for CMake package
install(EXPORT NLMTargets
    FILE NLMConfig.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

# Add alias targets for easier usage
add_library(nlm::core ALIAS nlm_core)
add_library(nlm::agent ALIAS nlm_agent)
add_library(nlm::world ALIAS nlm_world)
add_executable(nlm::nlm ALIAS nlm)

# Summary output
message(STATUS "NLM Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Source directory: ${NLM_SOURCE_DIR}")
message(STATUS "  Python executable: ${Python3_EXECUTABLE}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
message(STATUS "Targets available:")
message(STATUS "  nlm_core - Core neural computation library")
message(STATUS "  nlm_agent - Agent system")
message(STATUS "  nlm_world - World simulation")
message(STATUS "  nlm_phase3 - Phase 3 experiments")
message(STATUS "  nlm_phase4 - Phase 4 experiments")
message(STATUS "  nlm - Main executable")
message(STATUS "  nlm_phase3_demo - Phase 3 demo")
message(STATUS "  nlm_phase4_demo - Phase 4 demo")
