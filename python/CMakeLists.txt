# Enhanced CMake configuration for NLM Python bindings
# Version 2.0.0 - Enhanced with Python ecosystem integration

cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXTENSIONS OFF)

# Build configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enhanced compiler flags for optimized performance
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /EHsc /O2 /openmp")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -O3 -march=native -fopenmp")
endif()

# Find Python and pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Optional Python ecosystem packages
find_package(Python3 NumPy QUIET)
find_package(Python3 Pandas QUIET)
find_package(Python3 Matplotlib QUIET)

# Set source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Core NLM libraries
add_library(nlm_core STATIC
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

add_library(nlm_agent STATIC
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)

add_library(nlm_world STATIC
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)

# Configure core library
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)
target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -O3>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /O2>
)

# Configure agent and world libraries
target_link_libraries(nlm_agent PUBLIC nlm_core)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Enhanced Python bindings with advanced features
pybind11_add_module(pynlm_enhanced bindings.cpp)
target_link_libraries(pynlm_enhanced PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm_enhanced PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Add preprocessor definitions for enhanced features
target_compile_definitions(pynlm_enhanced PRIVATE
    PYNLM_ENHANCED
    PYNLM_EXPERIMENTAL
    PYNLM_OPTIMIZED
    PYNLM_NUMPY_INTEGRATION
    PYNLM_PANDAS_INTEGRATION
    PYNLM_MATPLOTLIB_INTEGRATION
)

# Main Python bindings for backward compatibility
pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Platform-specific configurations
if(UNIX)
    target_link_libraries(pynlm PRIVATE -fPIC)
    target_link_libraries(pynlm_enhanced PRIVATE -fPIC)
    target_link_libraries(nlm_core PRIVATE -fPIC)
    target_link_libraries(nlm_agent PRIVATE -fPIC)
    target_link_libraries(nlm_world PRIVATE -fPIC)
endif()

# Installation rules
install(TARGETS pynlm pynlm_enhanced
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

# Install core headers
install(DIRECTORY ${NLM_SOURCE_DIR}/brain/ DESTINATION pynlm/include/brain
    FILES_MATCHING "*.hpp"
)
install(DIRECTORY ${NLM_SOURCE_DIR}/core/ DESTINATION pynlm/include/core
    FILES_MATCHING "*.hpp"
)
install(DIRECTORY ${NLM_SOURCE_DIR}/agent/ DESTINATION pynlm/include/agent
    FILES_MATCHING "*.hpp"
)
install(DIRECTORY ${NLM_SOURCE_DIR}/world/ DESTINATION pynlm/include/world
    FILES_MATCHING "*.hpp"
)
install(DIRECTORY ${NLM_SOURCE_DIR}/sensory/ DESTINATION pynlm/include/sensory
    FILES_MATCHING "*.hpp"
)
install(DIRECTORY ${NLM_SOURCE_DIR}/motor/ DESTINATION pynlm/include/motor
    FILES_MATCHING "*.hpp"
)
install(DIRECTORY ${NLM_SOURCE_DIR}/plasticity/ DESTINATION pynlm/include/plasticity
    FILES_MATCHING "*.hpp"
)
install(DIRECTORY ${NLM_SOURCE_DIR}/development/ DESTINATION pynlm/include/development
    FILES_MATCHING "*.hpp"
)
install(DIRECTORY ${NLM_SOURCE_DIR}/neuromodulation/ DESTINATION pynlm/include/neuromodulation
    FILES_MATCHING "*.hpp"
)
install(DIRECTORY ${NLM_SOURCE_DIR}/memory/ DESTINATION pynlm/include/memory
    FILES_MATCHING "*.hpp"
)
install(DIRECTORY ${NLM_SOURCE_DIR}/cognition/ DESTINATION pynlm/include/cognition
    FILES_MATCHING "*.hpp"
)
install(DIRECTORY ${NLM_SOURCE_DIR}/sensory/ DESTINATION pynlm/include/sensory
    FILES_MATCHING "*.hpp"
)
install(DIRECTORY ${NLM_SOURCE_DIR}/prediction/ DESTINATION pynlm/include/prediction
    FILES_MATCHING "*.hpp"
)
install(DIRECTORY ${NLM_SOURCE_DIR}/motor/ DESTINATION pynlm/include/motor
    FILES_MATCHING "*.hpp"
)
install(DIRECTORY ${NLM_SOURCE_DIR}/environment/ DESTINATION pynlm/include/environment
    FILES_MATCHING "*.hpp"
)

# Python package structure
install(DIRECTORY
    experiments/ performance/ visualization/ data/ integration/
    DESTINATION pynlm/python
    FILES_MATCHING "*.py"
)

# Python module metadata
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml.in"
    "${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml"
    @ONLY
)

# Generate export configuration
include(CMakePackageConfigHelpers)

# Write package version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/pynlmConfigVersion.cmake"
    VERSION 2.0.0
    COMPATIBILITY SameMajorVersion
)

# Generate compile commands for build analysis
export(TARGETS nlm_core nlm_agent nlm_world pynlm pynlm_enhanced
    FILE "${CMAKE_CURRENT_BINARY_DIR}/pynlmTargets.cmake"
)

# Build summary
file(WRITE "${CMAKE_BINARY_DIR}/build_summary.txt"
    "NLM Python Bindings - Enhanced Build Configuration\n"
    "=============================================\n\n"
    "Build Configuration:\n"
    "  C++ Standard: C++20\n"
    "  Build Type: ${CMAKE_BUILD_TYPE}\n"
    "  Optimization: -O3 -march=native -fopenmp\n\n"
    "Components:\n"
    "  - Core neural simulation library (nlm_core)\n"
    "  - Agent interaction library (nlm_agent)\n"
    "  - World simulation library (nlm_world)\n"
    "  - Basic Python bindings (pynlm)\n"
    "  - Enhanced Python bindings (pynlm_enhanced)\n\n"
    "Features:\n"
    "  - Enhanced Brain API with Python-style method names\n"
    "  - Batch processing and vectorized operations\n"
    "  - Comprehensive error handling with Python exceptions\n"
    "  - NumPy integration for numerical computing\n"
    "  - Pandas integration for data analysis\n"
    "  - Matplotlib integration for visualization\n"
    "  - Experiment runner with progress monitoring\n"
    "  - Performance profiler with system monitoring\n"
    "  - Visualization tools for analysis\n"
    "  - Data processing utilities\n"
    "  - Optional Python ecosystem packages\n\n"
    "Installation:\n"
    "  - Python modules: pynlm/python/\n"
    "  - C++ libraries: pynlm/\n"
    "  - Headers: pynlm/include/\n"
    "  - Configuration: pynlmConfig.cmake\n"
)

# Summary message
message(STATUS "Enhanced NLM Python bindings configured successfully!")
message(STATUS "  - C++ Standard: C++20")
message(STATUS "  - Optimization: -O3 -march=native -fopenmp")
message(STATUS "  - Libraries: nlm_core, nlm_agent, nlm_world")
message(STATUS "  - Python modules: pynlm (basic), pynlm_enhanced (advanced)")
message(STATUS "  - Python ecosystem integration: NumPy, Pandas, Matplotlib (optional)")
message(STATUS "  - Enhanced features: batch processing, error handling, profiling")
message(STATUS "\nBuild summary written to: ${CMAKE_BINARY_DIR}/build_summary.txt")
