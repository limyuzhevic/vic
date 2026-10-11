# CMake configuration for NLM Python bindings
# This file builds the modular Python bindings for the NLM project

# Set minimum CMake version
cmake_minimum_required(VERSION 3.16)

# Project name and language
set(PROJECT_NAME "pynlm_bindings")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Find Python and pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Set source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Core library containing all NLM C++ components
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
    ${NLM_SOURCE_DIR}/memory/SemanticMemory.cpp
    ${NLM_SOURCE_DIR}/memory/ProceduralMemory.cpp
    ${NLM_SOURCE_DIR}/memory/AssociativeMemory.cpp
    
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
    
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBody.cpp
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
    ${NLM_SOURCE_DIR}/world/WorldObject.cpp
    ${NLM_SOURCE_DIR}/world/WorldObjectType.cpp
    
    ${NLM_SOURCE_DIR}/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/experiments/Metrics.cpp
    ${NLM_SOURCE_DIR}/experiments/Phase3Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/Phase4Experiment.cpp
    
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
)

# Build core static library
add_library(nlm_core STATIC ${NLM_CORE_SOURCES})
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Optional: Build separate libraries for better modularity
# Uncomment if you want separate libraries for different components

# Core brain library
add_library(nlm_brain STATIC
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
)
target_include_directories(nlm_brain PUBLIC ${NLM_SOURCE_DIR})

# Memory library
add_library(nlm_memory STATIC
    ${NLM_SOURCE_DIR}/memory/Memory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralWorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralEpisodicMemory.cpp
)
target_include_directories(nlm_memory PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_memory PUBLIC nlm_core)

# Agent library
add_library(nlm_agent STATIC
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core nlm_memory)

# World library
add_library(nlm_world STATIC
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_world PUBLIC nlm_core)

# Python module configuration
set(PYTHON_MODULE_NAME "pynlm")
set(PYTHON_MODULE_OUTPUT_NAME "pynlm")

# Create the Python module
pybind11_add_module(${PYTHON_MODULE_NAME} nlm_bindings.cpp)

# Link the Python module with all necessary libraries
target_link_libraries(${PYTHON_MODULE_NAME} PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(${PYTHON_MODULE_NAME} PRIVATE ${NLM_SOURCE_DIR})

# Add Windows-specific options
if(WIN32)
    target_compile_options(${PYTHON_MODULE_NAME} PRIVATE /permissive-)
    target_link_libraries(${PYTHON_MODULE_NAME} PRIVATE ws2_32)
endif()

# Installation targets
install(TARGETS ${PYTHON_MODULE_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Package configuration for pip installation
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pynlmConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/pynlmConfig.cmake
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pynlmConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pynlm
)

# Build options
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Package summary
message(STATUS "Building NLM Python bindings")
message(STATUS "  Python version: ${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Core library: ${NLM_CORE_SOURCES}")
message(STATUS "  Number of source files: ${NLM_CORE_SOURCES}")
