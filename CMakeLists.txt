cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.6.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Debug build options
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0)
    add_definitions(-DDEBUG -D_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_options(-O2 -g -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    add_compile_options(-Os -DNDEBUG)
endif()

# Platform-specific settings
if(WIN32)
    add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/W4 /WX)
    # Windows-specific fixes
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    # POSIX settings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

# Project information
string(TIMESTAMP PROJECT_BUILD_TIME "%Y-%m-%d %H:%M:%S")

# Find required packages
find_package(Threads REQUIRED)

# Optional dependencies (for future enhancement)
option(NLM_ENABLE_PYTHON "Enable Python bindings" ON)
option(NLM_ENABLE_TESTS "Enable unit tests" ON)
option(NLM_ENABLE_EXPERIMENTS "Enable experiments" ON)
option(NLM_ENABLE_PLOTTING "Enable plotting support" OFF)
option(NLM_ENABLE_DEBUGGING "Enable debugging features" OFF)

# Python bindings dependencies (for future)
if(NLM_ENABLE_PYTHON)
    find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
endif()

# Library output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Install directories
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "Installation prefix")

# Source directory structure
set(NLM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(NLM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Core components
add_library(nlm_core STATIC
    # Core utilities
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Config/ConfigEnhanced.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    
    # Neural components
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
    src/motor/Action.cpp
    
    # Environment
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    
    # Framework (Phase 6)
    src/framework/NLMApplication.cpp
    src/framework/TestFramework.cpp
    src/framework/test_utils/test_utils.cpp
    
    # Experiments
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    
    # Visualization
    src/visualization/VisualizationInterface.cpp
)

# Application framework (new Phase 6 structure)
add_library(nlm_framework STATIC
    src/framework/NLMApplication.cpp
    src/framework/TestFramework.cpp
    src/framework/test_utils/test_utils.cpp
)

target_include_directories(nlm_framework PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include/nlm/framework>
)

target_link_libraries(nlm_framework PUBLIC nlm_core)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}>
)

target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
    src/world/Observation.cpp
)

target_include_directories(nlm_world PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}>
)

target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase-specific experiment libraries
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}>
)

target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}>
)

target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 5 experiments
add_library(nlm_phase5 STATIC
    src/experiments/Phase5Experiment.cpp
)

target_include_directories(nlm_phase5 PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}>
)

target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 6 integration (main application)
add_library(nlm_phase6 STATIC
    src/experiments/Phase6Demo.cpp
    src/experiments/Phase6IntegratedExperiment.cpp
)

target_include_directories(nlm_phase6 PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}>
)

target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5)

# Main executables
add_executable(nlm_main
    src/main.cpp
)

target_link_libraries(nlm_main PUBLIC nlm_core nlm_framework)

# Demo executables
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)

target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)

target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)

target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6 nlm_framework)

add_executable(nlm_benchmark
    src/benchmark.cpp
)

target_link_libraries(nlm_benchmark PUBLIC nlm_core nlm_framework)

# Tests
option(NLM_BUILD_TESTS "Build tests" ON)

if(NLM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Python bindings
option(NLM_ENABLE_PYTHON_BINDINGS "Enable Python bindings" OFF)

if(NLM_ENABLE_PYTHON_BINDINGS)
    # Python extension module
    add_library(pynlm MODULE
        python/bindings.cpp
    )
    
    target_link_libraries(pynlm PUBLIC nlm_core)
    
    set_target_properties(pynlm PROPERTIES
        PREFIX ""
        SUFFIX ".so"
        OUTPUT_NAME "pynlm"
    )
    
    target_include_directories(pynlm PUBLIC
        ${Python3_INCLUDE_DIRS}
        ${NLM_INCLUDE_DIR}
    )
    
    # Install Python module
    install(TARGETS pynlm
        LIBRARY DESTINATION ${Python3_SITEARCH}
    )
endif()

# Installation
install(TARGETS nlm_core nlm_agent nlm_world nlm_framework
    nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY src/ DESTINATION include/nlm
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*Test*" EXCLUDE
)

# Package configuration
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/nlm-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    INSTALL_DESTINATION lib/cmake/nlm
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
    DESTINATION lib/cmake/nlm
)

# Summary
message(STATUS "NLM Build Configuration Summary:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Project version: ${PROJECT_VERSION}")
message(STATUS "  Build time: ${PROJECT_BUILD_TIME}")
message(STATUS "  Python bindings: ${NLM_ENABLE_PYTHON_BINDINGS}")
message(STATUS "  Tests: ${NLM_BUILD_TESTS}")
message(STATUS "  Experiments: ${NLM_ENABLE_EXPERIMENTS}")
message(STATUS "  Core library: nlm_core")
message(STATUS "  Framework library: nlm_framework")
message(STATUS "  Agent library: nlm_agent")
message(STATUS "  World library: nlm_world")
message(STATUS "  Phase libraries: nlm_phase3, nlm_phase4, nlm_phase5, nlm_phase6")

# Custom targets for common operations
add_custom_target(build_docs
    COMMAND ${CMAKE_COMMAND} -E echo "Building documentation..."
    COMMENT "Building project documentation"
)

add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS nlm_core nlm_agent nlm_world
    COMMENT "Running unit tests"
)

add_custom_target(run_demo
    COMMAND ./nlm_phase6_demo
    DEPENDS nlm_phase6_demo
    COMMENT "Running Phase 6 demo"
)

# RPATH settings
if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
elseif(UNIX)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()