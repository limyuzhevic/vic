# NLM CMake Build System - Modern CMake Configuration
# Version: 0.1.0

# CMake version and policies
cmake_minimum_required(VERSION 3.16)

# Set default policies for modern CMake
if(POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW) # Honor visibility properties
endif()

if(POLICY CMP0069)
  cmake_policy(SET CMP0069 NEW) # Honor RPATH
endif()

if(POLICY CMP0070)
  cmake_policy(SET CMP0070 NEW) # Use target compile features
endif()

if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW) # Option reference not quoted
endif()

if(POLICY CMP0078)
  cmake_policy(SET CMP0078 NEW) # Support new operator deletion
endif()

if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW) # MSVC runtime library
endif()

project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "An Experimental Artificial Developmental Brain"
    HOMEPAGE_URL "https://github.com/"
    CONTACT "Research project"
)

# Include modern CMake configuration
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMCMakeConfig.cmake)

# Set C++ standard requirements
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build types configuration
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build")

# Platform-specific configurations
if(NLM_PLATFORM_LINUX)
    add_compile_options(-fstack-protector-strong -fPIC -pthread)
    add_link_options(-pthread)
endif()

if(NLM_PLATFORM_MACOS)
    add_compile_options(-fstack-protector-strong -fPIC)
endif()

# Optimization flags based on build type
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")

# Modern compiler warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wshadow
        -Wformat=2
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
        -Wlogical-op
    )
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library using modern target-based approach
nlm_add_library(nlm_core STATIC
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

# Configure include directories for core library
nlm_target_includes(nlm_core
    core
    brain
    dynamics
    plasticity
    development
    neuromodulation
    memory
    cognition
    sensory
    prediction
    motor
    environment
    experiments
    visualization
)

# Agent system
nlm_add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)

nlm_target_includes(nlm_agent agent)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system
nlm_add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)

nlm_target_includes(nlm_world world)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
nlm_add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)

nlm_target_includes(nlm_phase3 experiments)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
nlm_add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)

nlm_target_includes(nlm_phase4 experiments)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
add_executable(nlm src/main.cpp)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 6 integration demo (integration test)
add_executable(nlm_phase6_demo src/experiments/Phase6IntegrationDemo.cpp)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Testing configuration
if(NLM_ENABLE_TESTS AND NLM_HAS_GTEST)
    enable_testing()
    add_subdirectory(tests)
elseif(NLM_ENABLE_TESTS)
    message(WARNING "GTest not found. Tests are disabled.")
endif()

# Installation configuration
include(GNUInstallDirs)

# Install core library
install(TARGETS nlm_core
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT core
)

# Install agent library
install(TARGETS nlm_agent
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT agent
)

# Install world library
install(TARGETS nlm_world
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT world
)

# Install phase 3 library
install(TARGETS nlm_phase3
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT phase3
)

# Install phase 4 library
install(TARGETS nlm_phase4
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT phase4
)

# Install headers
install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
    FILES_MATCHING PATTERN "*.h"
    COMPONENT headers
)

# Install executables
install(TARGETS nlm
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT nlm
)

install(TARGETS nlm_phase3_demo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT phase3_demo
)

install(TARGETS nlm_phase4_demo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT phase4_demo
)

install(TARGETS nlm_phase6_demo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT phase6_demo
)

# Install CMake configuration files
install(EXPORT NLMTargets
    FILE NLMConfig.cmake
    NAMESPACE NLM::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
    COMPONENT cmake_config
)

# Install package configuration
include(CMakePackageConfigHelpers)

# Generate and install NLMConfigVersion.cmake
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
    COMPONENT cmake_config
)

# Package support (CPack)
include(CPack)

# Set package metadata
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_CONTACT "${PROJECT_CONTACT}")
set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An Experimental Artificial Developmental Brain")

# Set package files
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)

# Set packaging format and components
set(CPACK_GENERATOR "RPM;DEB")
set(CPACK_COMPONENTS_ALL core agent world phase3 phase4 headers nlm phase3_demo phase4_demo phase6_demo cmake_config)

# RPM package configuration
set(CPACK_RPM_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_RPM_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}")
set(CPACK_RPM_PACKAGE_RELEASE 1)
set(CPACK_RPM_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")
set(CPACK_RPM_PACKAGE_VENDOR "NLM Project")
set(CPACK_RPM_PACKAGE_GROUP "Development")
set(CPACK_RPM_PACKAGE_URL "${CPACK_PACKAGE_HOMEPAGE_URL}")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")

# DEB package configuration
set(CPACK_DEBIAN_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_DEBIAN_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "NLM Project")
set(CPACK_DEBIAN_PACKAGE_LICENSE "MIT")

# Installation documentation
if(NLM_ENABLE_DOCUMENTATION)
    install(DIRECTORY docs/
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
        FILES_MATCHING PATTERN "*.md"
    )
endif()

# Test installation
enable_testing()
add_test(NAME nlm_test_build
    COMMAND ${CMAKE_COMMAND} -E echo "Build test passed"
)

# Summary
message(STATUS "NLM Build Configuration Summary:")
message(STATUS "  Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Libraries: core agent world phase3 phase4")
message(STATUS "  Executables: nlm nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo")
