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

# Core system components
set(NLM_CORE_SOURCES
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
)

# Brain components
set(NLM_BRAIN_SOURCES
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
    ${NLM_SOURCE_DIR}/brain/SynapseLoading.cpp
)

# Dynamics components
set(NLM_DYNAMICS_SOURCES
    ${NLM_SOURCE_DIR}/dynamics/NeuralDynamics.cpp
    ${NLM_SOURCE_DIR}/dynamics/SpikeSystem.cpp
)

# Plasticity components
set(NLM_PLASTICITY_SOURCES
    ${NLM_SOURCE_DIR}/plasticity/PlasticityRule.cpp
    ${NLM_SOURCE_DIR}/plasticity/Hebbian.cpp
    ${NLM_SOURCE_DIR}/plasticity/STDP.cpp
    ${NLM_SOURCE_DIR}/plasticity/StructuralPlasticity.cpp
)

# Development components
set(NLM_DEVELOPMENT_SOURCES
    ${NLM_SOURCE_DIR}/development/DevelopmentSystem.cpp
    ${NLM_SOURCE_DIR}/development/Synaptogenesis.cpp
    ${NLM_SOURCE_DIR}/development/Pruning.cpp
    ${NLM_SOURCE_DIR}/development/Maturation.cpp
)

# Neuromodulation components
set(NLM_NEUROMODULATION_SOURCES
    ${NLM_SOURCE_DIR}/neuromodulation/Neuromodulator.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Reward.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/PredictionError.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Novelty.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Curiosity.cpp
)

# Memory components
set(NLM_MEMORY_SOURCES
    ${NLM_SOURCE_DIR}/memory/Memory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralWorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralEpisodicMemory.cpp
)

# Cognition components
set(NLM_COGNITION_SOURCES
    ${NLM_SOURCE_DIR}/cognition/ConceptFormation.cpp
    ${NLM_SOURCE_DIR}/cognition/NeuralPlanner.cpp
)

# Prediction components
set(NLM_PREDICTION_SOURCES
    ${NLM_SOURCE_DIR}/prediction/NeuralPrediction.cpp
    ${NLM_SOURCE_DIR}/prediction/PredictionSystem.cpp
)

# Agent components
set(NLM_AGENT_SOURCES
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)

# Motor components
set(NLM_MOTOR_SOURCES
    ${NLM_SOURCE_DIR}/motor/Action.cpp
    ${NLM_SOURCE_DIR}/motor/MotorSystem.cpp
)

# World components
set(NLM_WORLD_SOURCES
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)

# Environment components
set(NLM_ENVIRONMENT_SOURCES
    ${NLM_SOURCE_DIR}/environment/Environment.cpp
)

# Sensory components
set(NLM_SENSORY_SOURCES
    ${NLM_SOURCE_DIR}/sensory/SensoryInput.cpp
    ${NLM_SOURCE_DIR}/sensory/Vision.cpp
    ${NLM_SOURCE_DIR}/sensory/Audio.cpp
    ${NLM_SOURCE_DIR}/sensory/InternalSignals.cpp
)

# Performance components
set(NLM_PERFORMANCE_SOURCES
    ${NLM_SOURCE_DIR}/performance/CheckpointSystem.hpp
    ${NLM_SOURCE_DIR}/performance/MemoryPool.hpp
    ${NLM_SOURCE_DIR}/performance/ParallelProcessing.hpp
    ${NLM_SOURCE_DIR}/performance/OptimizedBrain.hpp
    ${NLM_SOURCE_DIR}/performance/SIMDVectorization.hpp
    ${NLM_SOURCE_DIR}/performance/SparseConnectivity.hpp
)

# Visualization components
set(NLM_VISUALIZATION_SOURCES
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
)

# Experiments
set(NLM_EXPERIMENTS_SOURCES
    ${NLM_SOURCE_DIR}/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/experiments/Metrics.cpp
)

add_library(nlm_core STATIC ${NLM_CORE_SOURCES})
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)
target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

add_library(nlm_brain STATIC ${NLM_BRAIN_SOURCES})
target_include_directories(nlm_brain PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_brain PUBLIC nlm_core)

add_library(nlm_dynamics STATIC ${NLM_DYNAMICS_SOURCES})
target_include_directories(nlm_dynamics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_dynamics PUBLIC nlm_core)

add_library(nlm_plasticity STATIC ${NLM_PLASTICITY_SOURCES})
target_include_directories(nlm_plasticity PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_plasticity PUBLIC nlm_core)

add_library(nlm_development STATIC ${NLM_DEVELOPMENT_SOURCES})
target_include_directories(nlm_development PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_development PUBLIC nlm_core)

add_library(nlm_neuromodulation STATIC ${NLM_NEUROMODULATION_SOURCES})
target_include_directories(nlm_neuromodulation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_neuromodulation PUBLIC nlm_core)

add_library(nlm_memory STATIC ${NLM_MEMORY_SOURCES})
target_include_directories(nlm_memory PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_memory PUBLIC nlm_core)

add_library(nlm_cognition STATIC ${NLM_COGNITION_SOURCES})
target_include_directories(nlm_cognition PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_cognition PUBLIC nlm_core)

add_library(nlm_prediction STATIC ${NLM_PREDICTION_SOURCES})
target_include_directories(nlm_prediction PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_prediction PUBLIC nlm_core)

add_library(nlm_agent STATIC ${NLM_AGENT_SOURCES})
target_include_directories(nlm_agent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_agent PUBLIC nlm_core nlm_memory)

add_library(nlm_motor STATIC ${NLM_MOTOR_SOURCES})
target_include_directories(nlm_motor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_motor PUBLIC nlm_core)

add_library(nlm_world STATIC ${NLM_WORLD_SOURCES})
target_include_directories(nlm_world PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_world PUBLIC nlm_core nlm_agent nlm_sensory)

add_library(nlm_environment STATIC ${NLM_ENVIRONMENT_SOURCES})
target_include_directories(nlm_environment PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_environment PUBLIC nlm_core)

add_library(nlm_sensory STATIC ${NLM_SENSORY_SOURCES})
target_include_directories(nlm_sensory PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_sensory PUBLIC nlm_core)

add_library(nlm_performance STATIC ${NLM_PERFORMANCE_SOURCES})
target_include_directories(nlm_performance PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_performance PUBLIC nlm_core)

add_library(nlm_visualization STATIC ${NLM_VISUALIZATION_SOURCES})
target_include_directories(nlm_visualization PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_visualization PUBLIC nlm_core nlm_performance)

add_library(nlm_experiments STATIC ${NLM_EXPERIMENTS_SOURCES})
target_include_directories(nlm_experiments PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_experiments PUBLIC nlm_core nlm_brain nlm_dynamics nlm_plasticity nlm_development nlm_neuromodulation nlm_memory nlm_cognition nlm_prediction nlm_agent nlm_motor nlm_world nlm_environment nlm_sensory nlm_performance nlm_visualization)

pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_brain nlm_dynamics nlm_plasticity nlm_development nlm_neuromodulation nlm_memory nlm_cognition nlm_prediction nlm_agent nlm_motor nlm_world nlm_environment nlm_sensory nlm_performance nlm_visualization nlm_experiments)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

# Install Python package files
install(FILES __init__.py
    DESTINATION pynlm
)

# Create test targets
enable_testing()

add_test(NAME nlm_basic_functionality
    COMMAND ${Python_EXECUTABLE} -c "import pynlm; brain = pynlm.createBrain(pynlm.createDefaultConfig()); brain.initialize(); print('Basic test passed')"
)

# Performance benchmarks
add_custom_target(performance_benchmarks
    COMMAND ${Python_EXECUTABLE} benchmark.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS pynlm
)
