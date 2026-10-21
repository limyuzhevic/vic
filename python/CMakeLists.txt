cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# ==============================================================================
# CORE NLM LIBRARY COMPONENTS
# ==============================================================================

# Core neural simulation library (all fundamental components)
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

# Agent system library (brain-world interface)
set(NLM_AGENT_SOURCES
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBody.cpp
)

# World system library (environment simulation)
set(NLM_WORLD_SOURCES
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)

# ==============================================================================
# LIBRARY TARGETS
# ==============================================================================

# Core NLM library (all fundamental neural components)
add_library(nlm_core STATIC ${NLM_CORE_SOURCES})
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)
target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -O3>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /O2>
)

# Agent system library (brain-world interface)
add_library(nlm_agent STATIC ${NLM_AGENT_SOURCES})
target_include_directories(nlm_agent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system library (environment simulation)
add_library(nlm_world STATIC ${NLM_WORLD_SOURCES})
target_include_directories(nlm_world PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# ==============================================================================
# PYTHON MODULE
# ==============================================================================

# Main Python module (pynlm)
pybind11_add_module(pynlm bindings.cpp)

# Link all required libraries
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)

# Include directories for Python module
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Compiler optimizations for Python module
target_compile_options(pynlm PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -O3 -DNDEBUG>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /O2 /DNDEBUG>
)

if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

# ==============================================================================
# INSTALLATION
# ==============================================================================

# Install Python module
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

# ==============================================================================
# ADDITIONAL TARGETS FOR DEVELOPMENT
# ==============================================================================

# Static library for C++ development
add_library(nlm_full STATIC
    nlm_core
    nlm_agent
    nlm_world
)

# Demo executables (commented out by default)
# add_executable(nlm_demo ../src/main.cpp)
# target_link_libraries(nlm_demo PRIVATE nlm_core)

# Test executable (commented out by default)
# add_executable(nlm_test ../tests/test_main.cpp)
# target_link_libraries(nlm_test PRIVATE nlm_core)

# ==============================================================================
# CONFIGURATION OPTIONS
# ==============================================================================

option(BUILD_PYTHON_DEMOS "Build Python demonstration scripts" ON)
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_PERFORMANCE_TESTS "Build performance benchmarks" OFF)
option(ENABLE_PYTHON_EXCEPTIONS "Enable Python-style exception handling" ON)
option(MAXIMIZE_PERFORMANCE "Maximize performance at the cost of debug info" OFF)

# Apply configuration options
if(MAXIMIZE_PERFORMANCE)
    target_compile_options(nlm_core PRIVATE -O3 -DNDEBUG -march=native)
    target_compile_options(nlm_agent PRIVATE -O3 -DNDEBUG -march=native)
    target_compile_options(nlm_world PRIVATE -O3 -DNDEBUG -march=native)
    target_compile_options(pynlm PRIVATE -O3 -DNDEBUG -march=native)
endif()

if(ENABLE_PYTHON_EXCEPTIONS)
    target_compile_definitions(pynlm PRIVATE NLM_PYTHON_EXCEPTIONS)
endif()

# ==============================================================================
# PYTHON PACKAGE METADATA
# ==============================================================================

# Set Python module properties
set_target_properties(pynlm PROPERTIES
    OUTPUT_NAME "pynlm"
    PREFIX ""
    SUFFIX ".so"
)

# Install Python package metadata
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pynlm.pyproject.toml.in
    ${CMAKE_CURRENT_BINARY_DIR}/pynlm.pyproject.toml
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pynlm.pyproject.toml
    DESTINATION pynlm
)
