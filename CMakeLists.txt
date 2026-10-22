cmake_minimum_required(VERSION 3.16)

# Project name and version
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(src)

# Core libraries
add_library(nlm_core OBJECT
    src/core/Config/Config.hpp
    src/core/Config/Config.cpp
    src/core/Logger/Logger.hpp
    src/core/Logger/Logger.cpp
    src/core/Random/Random.hpp
    src/core/Random/Random.cpp
    src/core/SimulationClock/SimulationClock.hpp
    src/core/SimulationClock/SimulationClock.cpp
    src/core/Types/Types.hpp
    src/core/Types/Types.cpp
    src/core/Serialization/Checkpoint.hpp
    src/core/Serialization/Checkpoint.cpp
)

target_compile_features(nlm_core PUBLIC cxx_std_20)

# Neural components
add_library(nlm_neural OBJECT
    src/brain/Brain.cpp
    src/brain/Brain.hpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralPopulation.hpp
    src/brain/NeuralRegion.cpp
    src/brain/NeuralRegion.hpp
    src/brain/Neuron.cpp
    src/brain/Neuron.hpp
    src/brain/Synapse.cpp
    src/brain/Synapse.hpp
)

target_compile_features(nlm_neural PUBLIC cxx_std_20)

# Plasticity systems
add_library(nlm_plasticity OBJECT
    src/plasticity/STDP.cpp
    src/plasticity/STDP.hpp
    src/plasticity/Hebbian.cpp
    src/plasticity/Hebbian.hpp
    src/plasticity/StructuralPlasticity.cpp
    src/plasticity/StructuralPlasticity.hpp
)

target_compile_features(nlm_plasticity PUBLIC cxx_std_20)

# Memory systems
add_library(nlm_memory OBJECT
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralWorkingMemory.hpp
    src/memory/NeuralEpisodicMemory.cpp
    src/memory/NeuralEpisodicMemory.hpp
    src/memory/NeuralAssociativeMemory.cpp
    src/memory/NeuralAssociativeMemory.hpp
)

target_compile_features(nlm_memory PUBLIC cxx_std_20)

# Prediction system
add_library(nlm_prediction OBJECT
    src/prediction/PredictionSystem.cpp
    src/prediction/PredictionSystem.hpp
)

target_compile_features(nlm_prediction PUBLIC cxx_std_20)

# Cognition systems
add_library(nlm_cognition OBJECT
    src/cognition/NeuralPlanner.cpp
    src/cognition/NeuralPlanner.hpp
    src/cognition/ConceptFormation.cpp
    src/cognition/ConceptFormation.hpp
    src/cognition/AttentionalSelection.cpp
    src/cognition/AttentionalSelection.hpp
)

target_compile_features(nlm_cognition PUBLIC cxx_std_20)

# Neuromodulation systems
add_library(nlm_neuromodulation OBJECT
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Neuromodulator.hpp
    src/neuromodulation/Dopamine.cpp
    src/neuromodulation/Dopamine.hpp
    src/neuromodulation/Curiosity.cpp
    src/neuromodulation/Curiosity.hpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/PredictionError.hpp
    src/neuromodulation/Judgment.cpp
    src/neuromodulation/Judgment.hpp
)

target_compile_features(nlm_neuromodulation PUBLIC cxx_std_20)

# Development system
add_library(nlm_development OBJECT
    src/development/DevelopmentSystem.cpp
    src/development/DevelopmentSystem.hpp
)

target_compile_features(nlm_development PUBLIC cxx_std_20)

# Agent systems
add_library(nlm_agent OBJECT
    src/agent/AgentBrain.cpp
    src/agent/AgentBrain.hpp
    src/agent/AgentBody.cpp
    src/agent/AgentBody.hpp
    src/agent/SensoryPercept.cpp
    src/agent/SensoryPercept.hpp
)

target_compile_features(nlm_agent PUBLIC cxx_std_20)

# World systems
add_library(nlm_world OBJECT
    src/world/SimpleWorld.cpp
    src/world/SimpleWorld.hpp
)

target_compile_features(nlm_world PUBLIC cxx_std_20)

# Spike system
add_library(nlm_spike OBJECT
    src/dynamics/SpikeSystem.cpp
    src/dynamics/SpikeSystem.hpp
)

target_compile_features(nlm_spike PUBLIC cxx_std_20)

# Python bindings
add_library(pynlm MODULE
    src/bindings.cpp
    src/python/bindings.cpp
)

set_target_properties(pynlm PROPERTIES
    PREFIX ""
    SUFFIX ".so"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/python
)

target_link_libraries(pynlm PRIVATE
    nlm_core
    nlm_neural
    nlm_plasticity
    nlm_memory
    nlm_prediction
    nlm_cognition
    nlm_neuromodulation
    nlm_development
    nlm_agent
    nlm_world
    nlm_spike
)

# Main NLM program
add_executable(nlm_main
    src/main.cpp
)

target_link_libraries(nlm_main PRIVATE
    nlm_core
    nlm_neural
    nlm_plasticity
    nlm_memory
    nlm_prediction
    nlm_cognition
    nlm_neuromodulation
    nlm_development
    nlm_agent
    nlm_world
    nlm_spike
)

# BrainManager - Improved high-level interface
add_library(brain_manager STATIC
    src/BrainManager.cpp
)

target_include_directories(brain_manager PUBLIC
    src
)

target_link_libraries(brain_manager PRIVATE
    nlm_core
    nlm_neural
    nlm_plasticity
    nlm_memory
    nlm_prediction
    nlm_cognition
    nlm_neuromodulation
    nlm_development
    nlm_agent
    nlm_world
    nlm_spike
)

# Demo programs
add_executable(brain_manager_demo
    demo/main.cpp
)

target_link_libraries(brain_manager_demo PRIVATE
    brain_manager
)

add_executable(brain_manager_demo_advanced
    demo/BrainManagerDemo.cpp
)

target_link_libraries(brain_manager_demo_advanced PRIVATE
    brain_manager
)

# Tests
add_executable(nlm_test
    tests/test_main.cpp
    tests/test_types.cpp
    tests/test_config.cpp
    tests/test_random.cpp
    tests/test_clock.cpp
    tests/test_neuron.cpp
    tests/test_synapse.cpp
    tests/test_stdp.cpp
    tests/test_brain.cpp
    tests/test_brain_manager.cpp
)

target_link_libraries(nlm_test PRIVATE
    nlm_core
    nlm_neural
    nlm_plasticity
    nlm_memory
    nlm_prediction
    nlm_cognition
    nlm_neuromodulation
    nlm_development
    nlm_agent
    nlm_world
    nlm_spike
    brain_manager
)

enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Installation
install(TARGETS nlm_main DESTINATION bin)
install(TARGETS pynlm DESTINATION python)
install(DIRECTORY src/ DESTINATION include/nlm FILES_MATCHING PATTERN "*.hpp")
