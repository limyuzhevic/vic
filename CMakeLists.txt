# CMake Configuration for NLM (Neural Learning Machine)
# Modern CMake configuration with best practices

cmake_minimum_required(VERSION 3.16)
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "An Experimental Artificial Developmental Brain"
)

# Set C++ standard
cmake_policy(SET CMP0063 NEW)  # Require exact C++ standard

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories for better organization
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Compiler-specific configurations
if(MSVC)
    # Windows MSVC settings
    add_compile_options(/W4 /WX)
    add_definitions(-DNLM_WINDOWS)
else()
    # Unix-like systems (Linux, macOS)
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_compile_options(-fvisibility=hidden -fPIC)
    add_definitions(-DNLM_UNIX)
endif()

# Platform-specific configurations
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DLINUX)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_definitions(-DMACOS)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(-DWINDOWS)
endif()

# Source directory configuration
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Build configurations with optimizations
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Performance optimizations for different build types
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG -march=native")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O2 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Os -DNDEBUG")

# Option to build shared libraries
option(BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)

# Python bindings configuration
option(WITH_PYTHON_BINDINGS "Build Python bindings" ON)
if(WITH_PYTHON_BINDINGS)
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    find_package(pybind11 REQUIRED)
endif()

# Testing configuration
option(BUILD_TESTS "Build unit tests" ON)

# Installation configuration
set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH "Installation prefix")

# Core library
add_library(nlm_core
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
    
    # Memory systems
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

# Core library properties
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Add definitions for core library
target_compile_definitions(nlm_core PUBLIC NLM_CORE_EXPORT)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core)
target_compile_definitions(nlm_agent PUBLIC NLM_AGENT_EXPORT)

# World system (environment, objects, simple world)
add_library(nlm_world
    src/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_world PUBLIC nlm_core)
target_compile_definitions(nlm_world PUBLIC NLM_WORLD_EXPORT)

# Phase experiments (linking all core systems)
add_library(nlm_phase3
    src/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
target_compile_definitions(nlm_phase3 PUBLIC NLM_PHASE3_EXPORT)

add_library(nlm_phase4
    src/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
target_compile_definitions(nlm_phase4 PUBLIC NLM_PHASE4_EXPORT)

# Main executable (basic simulation)
add_executable(nlm
    src/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core)
target_compile_definitions(nlm PUBLIC NLM_MAIN_EXPORT)

# Phase 3 demo executable (basic agent example)
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)

target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
target_compile_definitions(nlm_phase3_demo PUBLIC NLM_PHASE3_DEMO_EXPORT)

# Phase 4 demo executable (advanced agent example)
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)

target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
target_compile_definitions(nlm_phase4_demo PUBLIC NLM_PHASE4_DEMO_EXPORT)

# Phase 6 demo executable (integrated brain)
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)

target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
target_compile_definitions(nlm_phase6_demo PUBLIC NLM_PHASE6_DEMO_EXPORT)

# Python bindings
if(WITH_PYTHON_BINDINGS)
    # Python package name
    set(PYTHON_PACKAGE_NAME "pynlm")
    
    # Python module
    pybind11_add_module(${PYTHON_PACKAGE_NAME}
        python/bindings.cpp
    )
    
    target_link_libraries(${PYTHON_PACKAGE_NAME}
        PUBLIC
        nlm_core
        nlm_agent
        nlm_world
        nlm_phase3
        nlm_phase4
    )
    
    target_include_directories(${PYTHON_PACKAGE_NAME} PRIVATE ${NLM_SOURCE_DIR})
    
    # Python module properties
    set_target_properties(${PYTHON_PACKAGE_NAME} PROPERTIES
        PREFIX ""
        SUFFIX ".so"
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/python
    )
    
    # Install Python package
    install(TARGETS ${PYTHON_PACKAGE_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages
    )
    
    # Install Python package configuration
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml
        ${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml
        COPYONLY
    )
    
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages
    )
endif()

# Testing setup
if(BUILD_TESTS)
    enable_testing()
    
    # Add tests subdirectory
    add_subdirectory(tests)
endif()

# Installation targets
install(TARGETS nlm
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# Install headers
install(DIRECTORY ${NLM_SOURCE_DIR}/core
    DESTINATION include/nlm/core
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY ${NLM_SOURCE_DIR}/brain
    DESTINATION include/nlm/brain
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY ${NLM_SOURCE_DIR}/agent
    DESTINATION include/nlm/agent
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY ${NLM_SOURCE_DIR}/world
    DESTINATION include/nlm/world
    FILES_MATCHING PATTERN "*.hpp"
)

# Configuration files
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/default.cfg
    DESTINATION etc/nlm
)

# Documentation
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    ${CMAKE_CURRENT_SOURCE_DIR}/HOW_TO_USE.md
    ${CMAKE_CURRENT_SOURCE_DIR}/easy_usage.md
    DESTINATION doc/nlm
)

# Export configuration for downstream projects
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Create NLMConfig.cmake for find_package support
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
    DESTINATION lib/cmake/nlm
)

# Summary
message(STATUS "NLM Configuration Summary:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Python bindings: ${WITH_PYTHON_BINDINGS}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  Shared libraries: ${BUILD_SHARED_LIBS}")
message(STATUS "  Installation prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Source directory: ${NLM_SOURCE_DIR}")
message(STATUS "  Binary output directory: ${CMAKE_BINARY_DIR}")
