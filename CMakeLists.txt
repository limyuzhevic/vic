# Enhanced CMake Configuration for NLM (Neural Learning Machine)
# This file provides comprehensive build options and platform support
cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard with modern C++ features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configuration options
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_DEMOS "Build demonstration programs" ON)
option(BUILD_BENCHMARKS "Build benchmark programs" ON)
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)
option(ENABLE_SANITIZERS "Enable address sanitizer and other sanitizers" OFF)
option(ENABLE_PERF_COUNTERS "Enable performance counter support" ON)
option(ENABLE_DEBUG "Enable debug mode" OFF)
option(ENABLE_VECTORIZATION "Enable SIMD vectorization optimizations" ON)
option(ENABLE_PARALLEL "Enable parallel processing" ON)
option(ENABLE_THREADS "Enable multithreading support" ON)

# Installation configuration
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation prefix")

# Platform detection
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(NLM_PLATFORM "Linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(NLM_PLATFORM "macOS")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(NLM_PLATFORM "Windows")
else()
    set(NLM_PLATFORM "Unknown")
endif()

# Compiler-specific optimizations and warnings
if(MSVC)
    # MSVC warnings
    add_compile_options(/W4 /WX /permissive-)
    add_definitions(-DNOMINMAX)
    # MSVC specific optimizations
    add_compile_options(/fp:fast /arch:AVX2)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # GCC warnings and optimizations
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wshadow -Wformat=2)
    add_compile_options(-Wimplicit-fallthrough -Wduplicated-cond -Wlogical-op)
    add_compile_options(-Wnull-dereference -Winit-self -Wswitch-default)
    add_compile_options(-Wvla-extension)
    # Performance optimizations
    add_compile_options(-march=native -ffast-math -fomit-frame-pointer)
    if(ENABLE_VECTORIZATION)
        add_compile_options(-mavx2 -mfma)
    endif()
    if(ENABLE_THREADS)
        add_compile_options(-pthread)
        add_link_options(-pthread)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Clang warnings and optimizations
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wshadow -Wformat=2)
    add_compile_options(-Wimplicit-fallthrough -Wduplicated-cond -Wlogical-op)
    add_compile_options(-Wnull-dereference -Winit-self -Wswitch-default)
    add_compile_options(-Wvla-extension)
    # Performance optimizations
    add_compile_options(-march=native -ffast-math -fomit-frame-pointer)
    if(ENABLE_VECTORIZATION)
        add_compile_options(-mavx2 -mfma)
    endif()
    if(ENABLE_THREADS)
        add_compile_options(-pthread)
        add_link_options(-pthread)
    endif()
endif()

# Position-independent code for shared libraries
if(BUILD_SHARED_LIBS)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Optimization levels based on build type
if(NOT CMAKE_BUILD_TYPE)
    if(ENABLE_DEBUG)
        set(CMAKE_BUILD_TYPE Debug)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")
    else()
        set(CMAKE_BUILD_TYPE Release)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
    endif()
endif()

# Compiler-specific flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -DDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_options(-O2 -g -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    add_compile_options(-Os -DNDEBUG)
endif()

message(STATUS "Building for platform: ${NLM_PLATFORM}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Find required packages
find_package(nlohmann_json 3.11.2 REQUIRED)

# Add definitions based on build configuration
if(ENABLE_PERF_COUNTERS)
    add_definitions(-DHAVE_PERF_COUNTERS)
endif()

if(ENABLE_PARALLEL)
    add_definitions(-DENABLE_PARALLEL)
endif()

if(ENABLE_VECTORIZATION)
    add_definitions(-DHAVE_SSE -DHAVE_AVX -DHAVE_AVX2)
endif()

# Core library configuration
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library with component-based structure
add_library(nlm_core STATIC
    # Core utilities
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    
    # Brain system
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    
    # Dynamics and spiking
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    
    # Plasticity mechanisms
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
    
    # Sensory processing
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    
    # Prediction system
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

# Include directories
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${NLOHMANN_JSON_INCLUDE_DIR}>
)

# Compiler-specific optimizations for core library
target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-march=native -ffast-math>
)

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

# Python bindings
if(BUILD_PYTHON_BINDINGS)
    # Find Python
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
    
    # Add pybind11 subdirectory if not already present
    include(FetchContent)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG v2.10.0
    )
    FetchContent_MakeAvailable(pybind11)
    
    add_library(pynlm MODULE
        python/bindings.cpp
    )
    
    target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
    target_include_directories(pynlm PRIVATE
        ${Python3_INCLUDE_DIRS}
        ${pybind11_INCLUDE_DIR}
    )
    
    set_target_properties(pynlm PROPERTIES
        PREFIX ""
        SUFFIX ".so"
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
    
    # Install Python module
    install(TARGETS pynlm
        LIBRARY DESTINATION lib/python
    )
endif()

# Main executable
add_executable(nlm
    src/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 5 experiment (final integration)
add_library(nlm_phase5 STATIC
    src/experiments/Phase5Experiment.cpp
)

target_include_directories(nlm_phase5 PUBLIC src)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world)

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

# Phase 6 demo (final integration test)
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)

if(TARGET nlm_phase5)
    target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase5)
else()
    target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Benchmark executable
if(BUILD_BENCHMARKS)
    add_executable(nlm_benchmark
        src/benchmark.cpp
    )
    
    target_link_libraries(nlm_benchmark PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Testing
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation targets
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY src/
    DESTINATION include/nlm
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*/test/*" EXCLUDE
    PATTERN "*/demo/*" EXCLUDE
)

# Create a config file for the project
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

# Summary of configuration
message(STATUS "=== NLM Configuration Summary ===")
message(STATUS "Platform: ${NLM_PLATFORM}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Shared libraries: ${BUILD_SHARED_LIBS}")
message(STATUS "Tests: ${BUILD_TESTS}")
message(STATUS "Python bindings: ${BUILD_PYTHON_BINDINGS}")
message(STATUS " Demos: ${BUILD_DEMOS}")
message(STATUS " Benchmarks: ${BUILD_BENCHMARKS}")
message(STATUS "Performance counters: ${ENABLE_PERF_COUNTERS}")
message(STATUS "Coverage: ${ENABLE_COVERAGE}")
message(STATUS "Sanitizers: ${ENABLE_SANITIZERS}")
message(STATUS "Vectorization: ${ENABLE_VECTORIZATION}")
message(STATUS "Parallel processing: ${ENABLE_PARALLEL}")
message(STATUS "=== Configuration Complete ===")