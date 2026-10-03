cmake_minimum_required(VERSION 3.20)

# Project configuration
project(NLM 
    VERSION 0.1.0 
    LANGUAGES CXX
    DESCRIPTION "Neural Learning Machine - Advanced Artificial Neural Simulation Framework"
)

# Set C++ standard
cmake_policy(SET CMP0083 NEW) # Make CMAKE_<LANG>_COMPILER_ID match the result of check_compiler_flag

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build types with defaults
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Compiler feature detection and optimization
include(CheckCXXCompilerFlag)

# Detect and enable compiler optimizations
check_cxx_compiler_flag(-march=native HAS_AVX512)
if(HAS_AVX512)
    add_compile_options(-march=native)
    message(STATUS "Using AVX-512 optimized builds")
endif()

check_cxx_compiler_flag(-flto HAS_LTO)
if(HAS_LTO)
    add_compile_options(-flto)
    message(STATUS "Using Link Time Optimization")
endif()

# Enhanced warning configuration
function(setup_compiler_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
        target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror=return-type)
        target_compile_options(${target} PRIVATE -Wshadow -Wnon-virtual-dtor -Wold-style-cast)
        target_compile_options(${target} PRIVATE -Wcast-align -Wnull-dereference)
        target_compile_options(${target} PRIVATE -Wduplicated-cond -Wduplicated-branches)
        target_compile_options(${target} PRIVATE -Wlogical-op -Wvoid-pointer-to-function-pointer)
        target_compile_options(${target} PRIVATE -Wcovered-switch-default)
        target_compile_options(${target} PRIVATE -Wswitch-default -Wswitch-enum)
        target_compile_options(${target} PRIVATE -Wformat=2)
        target_compile_options(${target} PRIVATE -Wdate-time)
        target_compile_options(${target} PRIVATE -Wdocumentation)
    endif()
endfunction()

# Platform detection and configuration
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_definitions(LINUX_BUILD)
    message(STATUS "Linux build configuration")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_compile_definitions(WINDOWS_BUILD)
    message(STATUS "Windows build configuration")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_compile_definitions(MACOS_BUILD)
    message(STATUS "macOS build configuration")
endif()

# Installation configuration
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation prefix")
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# Source directory configuration
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(NLM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Option configuration
option(NLM_ENABLE_PYTHON_BINDINGS "Enable Python bindings" ON)
option(NLM_ENABLE_TESTS "Enable unit tests" ON)
option(NLM_ENABLE_DOCS "Enable documentation generation" OFF)
option(NLM_ENABLE_EXPERIMENTS "Enable experimental features" ON)
option(NLM_ENABLE_VALIDATION "Enable runtime validation" ON)
option(NLM_ENABLE_PROFILING "Enable performance profiling" OFF)

# Core library
add_library(nlm_core STATIC
    # Core utilities
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

# Apply compiler warnings to core library
setup_compiler_warnings(nlm_core)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC src)
setup_compiler_warnings(nlm_agent)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC src)
setup_compiler_warnings(nlm_world)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
if(NLM_ENABLE_EXPERIMENTS)
    add_library(nlm_phase3 STATIC
        src/experiments/Phase3Experiment.cpp
    )

    target_include_directories(nlm_phase3 PUBLIC src)
    setup_compiler_warnings(nlm_phase3)
    target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Phase 4 experiments
if(NLM_ENABLE_EXPERIMENTS)
    add_library(nlm_phase4 STATIC
        src/experiments/Phase4Experiment.cpp
    )

    target_include_directories(nlm_phase4 PUBLIC src)
    setup_compiler_warnings(nlm_phase4)
    target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Python bindings (if enabled)
if(NLM_ENABLE_PYTHON_BINDINGS)
    find_package(pybind11 REQUIRED)
    
    pybind11_add_module(pynlm
        src/python/Bindings.cpp
    )
    
    target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
    target_include_directories(pynlm PRIVATE src)
    setup_compiler_warnings(pynlm)
    
    # Set Python module properties
    set_target_properties(pynlm PROPERTIES
        PREFIX ""
        SUFFIX ".so"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python"
    )
endif()

# Main executable
add_executable(nlm
    src/main.cpp
)

setup_compiler_warnings(nlm)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
if(NLM_ENABLE_EXPERIMENTS)
    add_executable(nlm_phase3_demo
        src/experiments/Phase3Demo.cpp
    )

    setup_compiler_warnings(nlm_phase3_demo)
    target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
endif()

# Phase 4 demo executable
if(NLM_ENABLE_EXPERIMENTS)
    add_executable(nlm_phase4_demo
        src/experiments/Phase4Demo.cpp
    )

    setup_compiler_warnings(nlm_phase4_demo)
    target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
endif()

# Testing configuration
if(NLM_ENABLE_TESTS)
    enable_testing()
    
    # Add all test targets
    add_test(NAME test_config COMMAND test_config)
    add_test(NAME test_brain COMMAND test_brain)
    add_test(NAME test_clock COMMAND test_clock)
    add_test(NAME test_neuron COMMAND test_neuron)
    add_test(NAME test_stdp COMMAND test_stdp)
    add_test(NAME test_synapse COMMAND test_synapse)
    add_test(NAME test_types COMMAND test_types)
    add_test(NAME test_random COMMAND test_random)
    add_test(NAME test_memory COMMAND test_memory)
    add_test(NAME test_neuromodulation COMMAND test_neuromodulation)
    add_test(NAME test_cognition COMMAND test_cognition)
    add_test(NAME test_development COMMAND test_development)
    add_test(NAME test_integration COMMAND test_integration)
endif()

# Installation targets
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

if(NLM_ENABLE_PYTHON_BINDINGS)
    install(TARGETS pynlm
        LIBRARY DESTINATION python
    )
endif()

install(DIRECTORY src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

# Package configuration for find_package support
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    INSTALL_DESTINATION lib/cmake/NLM
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    DESTINATION lib/cmake/NLM
)

# Summary
message(STATUS "NLM Build Configuration Summary:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Python bindings: ${NLM_ENABLE_PYTHON_BINDINGS}")
message(STATUS "  Tests enabled: ${NLM_ENABLE_TESTS}")
message(STATUS "  Experiments enabled: ${NLM_ENABLE_EXPERIMENTS}")
message(STATUS "  Installation prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")