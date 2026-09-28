cmake_minimum_required(VERSION 3.20)

# Project metadata
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "NLM (Neural Learning Machine) - Experimental Artificial Developmental Brain"
    HOMEPAGE_URL "https://github.com/nlm-project/nlm"
)

# Set C++ standard and compiler features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable modern CMake policies
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)  # option() honors normal variables
endif()

# Better default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Enable comprehensive warnings
if(MSVC)
    add_compile_options(/W4 /WX /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wshadow -Wformat=2 -Wconversion)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library with modern CMake practices
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
    # Memory (Phase 2 completed)
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

# Modern target properties and interface
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(nlm_core PUBLIC cxx_std_20)

# Set target properties for better build output
set_target_properties(nlm_core PROPERTIES
    OUTPUT_NAME "nlm_core"
    POSITION_INDEPENDENT_CODE ON
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/agent>
    $<INSTALL_INTERFACE:include/agent>
)

target_compile_features(nlm_agent PUBLIC cxx_std_20)
set_target_properties(nlm_agent PROPERTIES
    OUTPUT_NAME "nlm_agent"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/world>
    $<INSTALL_INTERFACE:include/world>
)

target_compile_features(nlm_world PUBLIC cxx_std_20)
set_target_properties(nlm_world PROPERTIES
    OUTPUT_NAME "nlm_world"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/experiments>
    $<INSTALL_INTERFACE:include/experiments>
)

target_compile_features(nlm_phase3 PUBLIC cxx_std_20)
set_target_properties(nlm_phase3 PROPERTIES
    OUTPUT_NAME "nlm_phase3"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/experiments>
    $<INSTALL_INTERFACE:include/experiments>
)

target_compile_features(nlm_phase4 PUBLIC cxx_std_20)
set_target_properties(nlm_phase4 PROPERTIES
    OUTPUT_NAME "nlm_phase4"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Main executable
add_executable(nlm
    src/main.cpp
)

target_compile_features(nlm PRIVATE cxx_std_20)
set_target_properties(nlm PROPERTIES
    OUTPUT_NAME "nlm"
)

target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)

target_compile_features(nlm_phase3_demo PRIVATE cxx_std_20)
set_target_properties(nlm_phase3_demo PROPERTIES
    OUTPUT_NAME "nlm_phase3_demo"
)

target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)

target_compile_features(nlm_phase4_demo PRIVATE cxx_std_20)
set_target_properties(nlm_phase4_demo PROPERTIES
    OUTPUT_NAME "nlm_phase4_demo"
)

target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 6 demo executable (main integration test)
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)

target_compile_features(nlm_phase6_demo PRIVATE cxx_std_20)
set_target_properties(nlm_phase6_demo PROPERTIES
    OUTPUT_NAME "nlm_phase6_demo"
)

target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Comprehensive test setup
enable_testing()

# Add tests directory with comprehensive test coverage
add_subdirectory(tests)

# Advanced installation configuration
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION "NLM - Neural Learning Machine: An Experimental Artificial Developmental Brain")
set(CPACK_PACKAGE_CONTACT "nlm-research@example.com")
set(CPACK_PACKAGE_VENDOR "NLM Research Team")
set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")

# Platform-specific package configurations
if(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "TGZ;DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_NAME "nlm")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "NLM - Neural Learning Machine simulation framework")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "NLM Research Team")
    set(CPACK_DEBIAN_PACKAGE_SECTION "libs")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_RPM_PACKAGE_NAME "nlm")
    set(CPACK_RPM_PACKAGE_DESCRIPTION "NLM - Neural Learning Machine simulation framework")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    set(CPACK_RPM_PACKAGE_GROUP "Science/Math")
endif()

# Installation configuration
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install headers with proper organization
install(DIRECTORY src/core/
    DESTINATION include/nlm/core
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/brain/
    DESTINATION include/nlm/brain
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/memory/
    DESTINATION include/nlm/memory
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/dynamics/
    DESTINATION include/nlm/dynamics
    FILES_MATCHING PATTERN "*.hpp"
)

# Package configuration (modern CMake)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Generate and install package configuration
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nlm
)