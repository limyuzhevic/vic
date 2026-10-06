cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source directory - CONSISTENT with root CMakeLists.txt
# Use the NLM_SOURCE_DIR from parent or define it relative to this file
if(DEFINED NLM_SOURCE_DIR)
    # NLM_SOURCE_DIR is already defined from parent CMakeLists.txt
    message(STATUS "Using NLM_SOURCE_DIR from parent: ${NLM_SOURCE_DIR}")
else()
    # Define relative to python directory
    set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src" CACHE STRING "Path to NLM source directory")
endif()

# Function to create a library target with common configuration
# This mirrors the helper functions from root CMakeLists.txt
function(add_nlm_library target_name target_sources)
    add_library(${target_name} STATIC ${target_sources})
    
    # Common include directories
    target_include_directories(${target_name} PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    )
    
    # Common compile options
    target_compile_options(${target_name} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
    
    # Set C++ standard properties
    set_target_properties(${target_name} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
    
    message(STATUS "Added NLM library target: ${target_name}")
endfunction()

# Function to link libraries to a target
function(link_nlm_target target_name)
    foreach(lib ${ARGN})
        target_link_libraries(${target_name} PUBLIC ${lib})
    endforeach()
endfunction()

# Core library components - CONSISTENT with root CMakeLists.txt
# Define source files using relative paths from NLM_SOURCE_DIR
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
    ${NLM_SOURCE_DIR}/memory/WorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/EpisodicMemory.cpp
    ${NLM_SOURCE_DIR}/memory/SemanticMemory.cpp
    ${NLM_SOURCE_DIR}/memory/ProceduralMemory.cpp
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
    ${NLM_SOURCE_DIR}/environment/Action.cpp
    ${NLM_SOURCE_DIR}/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/experiments/Metrics.cpp
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
)

# Agent system sources
set(NLM_AGENT_SOURCES
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)

# World system sources
set(NLM_WORLD_SOURCES
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)

# Phase 3 experiments
set(NLM_PHASE3_SOURCES
    ${NLM_SOURCE_DIR}/experiments/Phase3Experiment.cpp
)

# Phase 4 experiments
set(NLM_PHASE4_SOURCES
    ${NLM_SOURCE_DIR}/experiments/Phase4Experiment.cpp
)

# Create core library using helper function
add_nlm_library(nlm_core "${NLM_CORE_SOURCES}")

# Agent system using helper function
add_nlm_library(nlm_agent "${NLM_AGENT_SOURCES}")
link_nlm_target(nlm_agent nlm_core)

# World system using helper function
add_nlm_library(nlm_world "${NLM_WORLD_SOURCES}")
link_nlm_target(nlm_world nlm_core)

# Phase 3 experiments using helper function
add_nlm_library(nlm_phase3 "${NLM_PHASE3_SOURCES}")
link_nlm_target(nlm_phase3 nlm_core nlm_agent nlm_world)

# Phase 4 experiments using helper function
add_nlm_library(nlm_phase4 "${NLM_PHASE4_SOURCES}")
link_nlm_target(nlm_phase4 nlm_core nlm_agent nlm_world)

# Python bindings
set(PYTHON_BINDINGS_SOURCES
    bindings.cpp
)

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(pynlm ${PYTHON_BINDINGS_SOURCES})

# Link with NLM libraries
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)

# Include directories for Python bindings
target_include_directories(pynlm PRIVATE ${NLM_SOURCE_DIR})

# MSVC specific compile options
if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

# Install Python module
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

message(STATUS "Python bindings CMake configuration complete")