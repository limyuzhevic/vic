cmake_minimum_required(VERSION 3.16)
project(nlm-python-bindings LANGUAGES CXX)

# NLM Python Bindings Build Script
#
# This script builds the Python bindings for NLM (熙然) - Neural Learning Machine.
# It provides enhanced build options for performance, modularity, and Python integration.

# Set minimum CMake version
if(CMAKE_VERSION VERSION_LESS 3.16)
    message(FATAL_ERROR "NLM Python bindings requires CMake 3.16 or later")
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Find Python and pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

message(STATUS "Found Python: ${Python_EXECUTABLE}")
message(STATUS "Found pybind11: ${pybind11_INCLUDE_DIR}")

# NLM source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../src")

# Source file groups
set(NLM_CORE_SOURCES
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
    ${NLM_SOURCE_DIR}/dynamics/NeuralDynamics.cpp
    ${NLM_SOURCE_DIR}/dynamics/SpikeSystem.cpp
    ${NLM_SOURCE_DIR}/plasticity/PlasticityRule.cpp
    ${NLM_SOURCE_DIR}/plasticity/Hebbian.cpp
    ${NLM_SOURCE_DIR}/plasticity/STDP.cpp
    ${NLM_SOURCE_DIR}/plasticity/StructuralPlasticity.cpp
    ${NLM_SOURCE_DIR}/development/DevelopmentSystem.cpp
    ${NLM_SOURCE_DIR}/development/Synaptogenesis.cpp
    ${NLM_SOURCE_DIR}/development/Pruning.cpp
    ${NLM_SOURCE_DIR}/development/Maturation.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Neuromodulator.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Reward.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/PredictionError.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Novelty.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Curiosity.cpp
    ${NLM_SOURCE_DIR}/memory/Memory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralWorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralEpisodicMemory.cpp
    ${NLM_SOURCE_DIR}/cognition/ConceptFormation.cpp
    ${NLM_SOURCE_DIR}/cognition/NeuralPlanner.cpp
    ${NLM_SOURCE_DIR}/sensory/SensoryInput.cpp
    ${NLM_SOURCE_DIR}/sensory/Vision.cpp
    ${NLM_SOURCE_DIR}/sensory/Audio.cpp
    ${NLM_SOURCE_DIR}/sensory/InternalSignals.cpp
    ${NLM_SOURCE_DIR}/prediction/PredictionSystem.cpp
    ${NLM_SOURCE_DIR}/prediction/NeuralPrediction.cpp
    ${NLM_SOURCE_DIR}/motor/MotorSystem.cpp
    ${NLM_SOURCE_DIR}/environment/Environment.cpp
    ${NLM_SOURCE_DIR}/environment/Observation.cpp
    ${NLM_SOURCE_DIR}/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/experiments/Metrics.cpp
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
)

set(NLM_AGENT_SOURCES
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)

set(NLM_WORLD_SOURCES
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)

# Build options for performance and functionality
option(NLM_ENABLE_VECTORIZATION "Enable SIMD vectorization for performance" ON)
option(NLM_ENABLE_SPARSE_CONNECTIVITY "Enable sparse connectivity optimization" ON)
option(NLM_ENABLE_MEMORY_POOLS "Enable memory pools for better allocation" ON)
option(NLM_ENABLE_EVENT_QUEUES "Enable event-driven computation" ON)
option(NLM_ENABLE_PROFILING "Enable performance profiling" OFF)
option(NLM_ENABLE_DEBUG_OUTPUT "Enable debug logging" OFF)

# Vectorization compilation flags
if(NLM_ENABLE_VECTORIZATION)
    target_compile_definitions(nlm_core PRIVATE NLM_ENABLE_VECTORIZATION)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(nlm_core PRIVATE -msse4 -msse2)
    endif()
endif()

if(NLM_ENABLE_SPARSE_CONNECTIVITY)
    target_compile_definitions(nlm_core PRIVATE NLM_ENABLE_SPARSE_CONNECTIVITY)
endif()

if(NLM_ENABLE_MEMORY_POOLS)
    target_compile_definitions(nlm_core PRIVATE NLM_ENABLE_MEMORY_POOLS)
endif()

if(NLM_ENABLE_EVENT_QUEUES)
    target_compile_definitions(nlm_core PRIVATE NLM_ENABLE_EVENT_QUEUES)
endif()

if(NLM_ENABLE_PROFILING)
    target_compile_definitions(nlm_core PRIVATE NLM_ENABLE_PROFILING)
endif()

if(NLM_ENABLE_DEBUG_OUTPUT)
    target_compile_definitions(nlm_core PRIVATE NLM_ENABLE_DEBUG_OUTPUT)
endif()

# Core library with proper include directories
add_library(nlm_core STATIC ${NLM_CORE_SOURCES})
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>
    $<INSTALL_INTERFACE:include>
)
target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Agent and world libraries
add_library(nlm_agent STATIC ${NLM_AGENT_SOURCES})
target_include_directories(nlm_agent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

add_library(nlm_world STATIC ${NLM_WORLD_SOURCES})
target_include_directories(nlm_world PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Main simulation library for Python bindings
add_library(nlm_simulation STATIC
    ${NLM_CORE_SOURCES}
    ${NLM_AGENT_SOURCES}
    ${NLM_WORLD_SOURCES}
)
target_include_directories(nlm_simulation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../src)

# Python module using pybind11
pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_simulation)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../src)

if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

# Installation targets
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

# Install C++ headers for development
install(DIRECTORY ${NLM_SOURCE_DIR}/core/Types
    DESTINATION include/nlm/core/Types
)

install(DIRECTORY ${NLM_SOURCE_DIR}/core/Config
    DESTINATION include/nlm/core/Config
)

install(DIRECTORY ${NLM_SOURCE_DIR}/brain
    DESTINATION include/nlm/brain
)

install(DIRECTORY ${NLM_SOURCE_DIR}/agent
    DESTINATION include/nlm/agent
)

install(DIRECTORY ${NLM_SOURCE_DIR}/world
    DESTINATION include/nlm/world
)

# Create convenience target for developers
add_custom_target(build_python_bindings ALL
    COMMAND ${CMAKE_COMMAND} -E echo "=== NLM Python Bindings Build ==="
    COMMAND ${CMAKE_COMMAND} -E echo "Building NLM Python bindings..."
    COMMAND ${Python_EXECUTABLE} -c "import sys; print(f'Python version: {sys.version}')"
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target pynlm
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building NLM Python bindings with all optimizations enabled"
    VERBATIM
)

# Print build configuration summary
message(STATUS "")
message(STATUS "====================================================")
message(STATUS "NLM Python Bindings - Build Configuration")
message(STATUS "====================================================")
message(STATUS "Python executable: ${Python_EXECUTABLE}")
message(STATUS "Python version: ${Python_VERSION}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "")
message(STATUS "Build options:")
message(STATUS "  Vectorization: ${NLM_ENABLE_VECTORIZATION}")
message(STATUS "  Sparse connectivity: ${NLM_ENABLE_SPARSE_CONNECTIVITY}")
message(STATUS "  Memory pools: ${NLM_ENABLE_MEMORY_POOLS}")
message(STATUS "  Event queues: ${NLM_ENABLE_EVENT_QUEUES}")
message(STATUS "  Profiling: ${NLM_ENABLE_PROFILING}")
message(STATUS "  Debug output: ${NLM_ENABLE_DEBUG_OUTPUT}")
message(STATUS "")
message(STATUS "Installation prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "====================================================")
message(STATUS "Build complete. Run 'make install' to install or use 'python -m pip install .' for Python package.")
message(STATUS "====================================================")