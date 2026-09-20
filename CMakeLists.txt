# NLM Build Configuration
# Modern CMake configuration with feature toggles and best practices

# Project version
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++ standard requirements
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    add_compile_options(-Werror)  # Enable errors for warnings
endif()

# Code coverage (optional)
option(NLM_ENABLE_COVERAGE "Enable code coverage reporting" OFF)
if(NLM_ENABLE_COVERAGE)
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif()

# Installation prefix
if(NOT CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation prefix" FORCE)
endif()

# Feature options
global option(NLM_BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
global option(NLM_BUILD_TESTS "Build tests" ON)
global option(NLM_BUILD_DOCS "Build documentation" ON)
global option(NLM_BUILD_EXPERIMENTS "Build experiment executables" ON)
global option(NLM_ENABLE_VISUALIZATION "Enable visualization components" OFF)
global option(NLM_ENABLE_PERFORMANCE_OPTIMIZATIONS "Enable performance optimizations" ON)
global option(NLM_ENABLE_DEBUG_ASSERTIONS "Enable debug assertions" OFF)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Core library configuration
add_library(nlm_core STATIC)

# Core source files
set(CORE_SOURCES
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
)

# Brain components
set(BRAIN_SOURCES
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
)

# Dynamics
set(DYNAMICS_SOURCES
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
)

# Plasticity
set(PLASTICITY_SOURCES
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
)

# Development
set(DEVELOPMENT_SOURCES
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
)

# Neuromodulation
set(NEUROMODULATION_SOURCES
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
)

# Memory
set(MEMORY_SOURCES
    src/memory/WorkingMemory.cpp
    src/memory/EpisodicMemory.cpp
    src/memory/SemanticMemory.cpp
    src/memory/ProceduralMemory.cpp
    src/memory/AssociativeMemory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
)

# Cognition
set(COGNITION_SOURCES
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
)

# Sensory
set(SENSORY_SOURCES
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
)

# Prediction
set(PREDICTION_SOURCES
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
)

# Motor
set(MOTOR_SOURCES
    src/motor/MotorSystem.cpp
)

# Environment
set(ENVIRONMENT_SOURCES
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
)

# Experiments
set(EXPERIMENT_SOURCES
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
)

# Visualization
set(VISUALIZATION_SOURCES
    src/visualization/VisualizationInterface.cpp
)

# Combine all core sources
list(APPEND CORE_SOURCES
    ${BRAIN_SOURCES}
    ${DYNAMICS_SOURCES}
    ${PLASTICITY_SOURCES}
    ${DEVELOPMENT_SOURCES}
    ${NEUROMODULATION_SOURCES}
    ${MEMORY_SOURCES}
    ${COGNITION_SOURCES}
    ${SENSORY_SOURCES}
    ${PREDICTION_SOURCES}
    ${MOTOR_SOURCES}
    ${ENVIRONMENT_SOURCES}
    ${EXPERIMENT_SOURCES}
)

if(NLM_ENABLE_VISUALIZATION)
    list(APPEND CORE_SOURCES ${VISUALIZATION_SOURCES})
endif()

# Add core library target
add_library(nlm_core ${CORE_SOURCES})

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Set target properties
target_compile_features(nlm_core PUBLIC cxx_std_20)

if(NLM_ENABLE_PERFORMANCE_OPTIMIZATIONS)
    target_compile_options(nlm_core PRIVATE -O3 -march=native)
endif()

if(NLM_ENABLE_DEBUG_ASSERTIONS)
    target_compile_definitions(nlm_core PRIVATE NLM_DEBUG_ASSERTIONS)
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
if(NLM_BUILD_EXPERIMENTS)
    add_library(nlm_phase3 STATIC
        src/experiments/Phase3Experiment.cpp
    )

    target_include_directories(nlm_phase3 PUBLIC src)
    target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Phase 4 experiments
if(NLM_BUILD_EXPERIMENTS)
    add_library(nlm_phase4 STATIC
        src/experiments/Phase4Experiment.cpp
    )

    target_include_directories(nlm_phase4 PUBLIC src)
    target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Python bindings
global if(NLM_BUILD_PYTHON_BINDINGS)
    # Find Python
    find_package(Python REQUIRED COMPONENTS Development)
    find_package(pybind11 REQUIRED)

    # Python bindings source
    add_library(pynlm MODULE python/bindings.cpp)

    target_include_directories(pynlm PRIVATE python)
    target_link_libraries(pynlm PRIVATE nlm_core)
    target_compile_features(pynlm PRIVATE cxx_std_20)

    # Set output name
    set_target_properties(pynlm PROPERTIES
        PREFIX ""
        OUTPUT_NAME "pynlm"
        SUFFIX "${PYTHON_MODULE_EXTENSION}"
    )

    # Install Python module
    install(TARGETS pynlm
        LIBRARY DESTINATION ${PYTHON_SITEARCH}
    )
endif()

# Main executable
add_executable(nlm src/main.cpp)

target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
if(NLM_BUILD_EXPERIMENTS)
    add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
    target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
endif()

# Phase 4 demo executable
if(NLM_BUILD_EXPERIMENTS)
    add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)
    target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
endif()

# Testing
if(NLM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Documentation
if(NLM_BUILD_DOCS)
    find_program(SPHINX_EXECUTABLE NAMES sphinx-build doxygen OPTIONAL)
    if(SPHINX_EXECUTABLE)
        add_custom_target(docs
            COMMAND ${SPHINX_EXECUTABLE} -b html docs _build/html
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Building documentation"
        )
    endif()
endif()

# Package configuration
include(CMakePackageConfigMacros)

# Generate package config
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/nlm-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    INSTALL_DESTINATION lib/cmake/nlm
)

# Generate version file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install package config
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
    DESTINATION lib/cmake/nlm
)

# Install targets
install(TARGETS nlm nlm_core nlm_agent nlm_world
    EXPORT nlmTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

if(NLM_BUILD_EXPERIMENTS)
    install(TARGETS nlm_phase3 nlm_phase4
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
endif()

# Install headers
install(DIRECTORY src/
    DESTINATION include/nlm
    FILES_MATCHING PATTERN "*.hpp"
    FILES_MATCHING PATTERN "*.h"
)

# CPack configuration
if(UNIX)
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_DESCRIPTION "NLM - Neural Learning Machine")
    set(CPACK_PACKAGE_CONTACT "nlm-project@nlm.org")
    set(CPACK_PACKAGE_MAINTAINER "NLM Authors")
    set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/nlm-project/nlm")
    set(CPACK_PACKAGE_CHECKSUM_DIGEST "SHA256")
endif()

include(CPack)

# Summary
message(STATUS "NLM Build Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Python bindings: ${NLM_BUILD_PYTHON_BINDINGS}")
message(STATUS "  Tests: ${NLM_BUILD_TESTS}")
message(STATUS "  Documentation: ${NLM_BUILD_DOCS}")
message(STATUS "  Experiments: ${NLM_BUILD_EXPERIMENTS}")
message(STATUS "  Visualization: ${NLM_ENABLE_VISUALIZATION}")
message(STATUS "  Performance optimizations: ${NLM_ENABLE_PERFORMANCE_OPTIMIZATIONS}")