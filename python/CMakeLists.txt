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
    ${NLM_SOURCE_DIR}/environment/Action.cpp
    ${NLM_SOURCE_DIR}/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/experiments/Metrics.cpp
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

add_library(nlm_agent STATIC
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

add_library(nlm_world STATIC
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Main pynlm bindings module
pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

# Enhanced Python bindings modules

# pynlm_advanced module
pybind11_add_module(pynlm_advanced python_bindings/pynlm_advanced.cpp)
target_link_libraries(pynlm_advanced PRIVATE nlm_core nlm_agent nlm_world pybind11::module)
target_include_directories(pynlm_advanced PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Add pynlm_advanced Python module capabilities
set_target_properties(pynlm_advanced PROPERTIES
    OUTPUT_NAME "pynlm_advanced"
    PREFIX ""
    SUFFIX ".so"
)

# pynlm_profiling module
pybind11_add_module(pynlm_profiling python_bindings/pynlm_profiling.cpp)
target_link_libraries(pynlm_profiling PRIVATE nlm_core nlm_agent nlm_world pybind11::module)
target_include_directories(pynlm_profiling PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Add pynlm_profiling Python module capabilities
set_target_properties(pynlm_profiling PROPERTIES
    OUTPUT_NAME "pynlm_profiling"
    PREFIX ""
    SUFFIX ".so"
)

# pynlm_analysis module
pybind11_add_module(pynlm_analysis python_bindings/pynlm_analysis.cpp)
target_link_libraries(pynlm_analysis PRIVATE nlm_core nlm_agent nlm_world pybind11::module)
target_include_directories(pynlm_analysis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Add pynlm_analysis Python module capabilities
set_target_properties(pynlm_analysis PROPERTIES
    OUTPUT_NAME "pynlm_analysis"
    PREFIX ""
    SUFFIX ".so"
)

# pynlm_plugin module
pybind11_add_module(pynlm_plugin python_bindings/pynlm_plugin.cpp)
target_link_libraries(pynlm_plugin PRIVATE nlm_core nlm_agent nlm_world pybind11::module)
target_include_directories(pynlm_plugin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Add pynlm_plugin Python module capabilities
set_target_properties(pynlm_plugin PROPERTIES
    OUTPUT_NAME "pynlm_plugin"
    PREFIX ""
    SUFFIX ".so"
)

# Installation targets
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

install(TARGETS pynlm_advanced pynlm_profiling pynlm_analysis pynlm_plugin
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

# Installation of Python source files
install(DIRECTORY python_bindings/ DESTINATION pynlm
    FILES_MATCHING PATTERN "*.py"
)

# Installation of examples
install(DIRECTORY examples/ DESTINATION pynlm/examples
    FILES_MATCHING PATTERN "*.py"
)

# Installation of documentation
install(DIRECTORY docs/ DESTINATION pynlm/docs
    FILES_MATCHING PATTERN "*.md"
)

# Add Python module dependency information
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pynlm_config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/pynlm_config.cmake
    @ONLY
)

include(${CMAKE_CURRENT_BINARY_DIR}/pynlm_config.cmake)