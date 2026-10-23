# Update Python bindings CMakeLists.txt to use scikit-build-core properly

cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Find Python and pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Source directory setup
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Set up source groups for better organization
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

# Create core library
add_library(nlm_core STATIC ${NLM_CORE_SOURCES})
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/core/Types>
)
target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Create agent library
add_library(nlm_agent STATIC ${NLM_AGENT_SOURCES})
target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core)

# Create world library
add_library(nlm_world STATIC ${NLM_WORLD_SOURCES})
target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_world PUBLIC nlm_core)

# Create Python module using pybind11
pybind11_add_module(pynlm python/bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${NLM_SOURCE_DIR})

# Set Python module properties
set_target_properties(pynlm PROPERTIES
    PREFIX ""
    SUFFIX ".so"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/python
)

# Installation
install(TARGETS pynlm
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages
)

# Add a custom target for installing the Python module
add_custom_target(install_python
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:pynlm> ${Python_SITEARCH}/pynlm.so
    COMMENT "Installing Python module to site-packages"
    DEPENDS pynlm
)

# Documentation target
add_custom_target(doc
    COMMAND ${CMAKE_COMMAND} -E echo "Documentation generation would go here"
    COMMENT "Generate documentation"
)
