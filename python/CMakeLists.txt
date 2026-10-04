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

if(EXISTS ${NLM_SOURCE_DIR}/command/Command.cpp)
    list(APPEND NLM_AGENT_SOURCES ${NLM_SOURCE_DIR}/command/Command.cpp)
endif()

if(EXISTS ${NLM_SOURCE_DIR}/factory/BrainFactory.cpp)
    list(APPEND NLM_AGENT_SOURCES ${NLM_SOURCE_DIR}/factory/BrainFactory.cpp)
endif()

if(EXISTS ${NLM_SOURCE_DIR}/core/ErrorHandling.cpp)
    list(APPEND NLM_AGENT_SOURCES ${NLM_SOURCE_DIR}/core/ErrorHandling.cpp)
endif()

if(EXISTS ${NLM_SOURCE_DIR}/threading/ThreadSafeSimulationManager.cpp)
    list(APPEND NLM_AGENT_SOURCES ${NLM_SOURCE_DIR}/threading/ThreadSafeSimulationManager.cpp)
endif()

if(EXISTS ${NLM_SOURCE_DIR}/topology/AdvancedTopologyManager.cpp)
    list(APPEND NLM_AGENT_SOURCES ${NLM_SOURCE_DIR}/topology/AdvancedTopologyManager.cpp)
endif()

set(NLM_WORLD_SOURCES
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
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

add_library(nlm_world STATIC ${NLM_WORLD_SOURCES})
target_include_directories(nlm_world PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_world PUBLIC nlm_core)

add_library(nlm_agent STATIC ${NLM_AGENT_SOURCES})
target_include_directories(nlm_agent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

if(EXISTS ${NLM_SOURCE_DIR}/command/Command.cpp)
    add_library(nlm_command STATIC
        ${NLM_SOURCE_DIR}/command/Command.cpp
    )
    target_include_directories(nlm_command PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_link_libraries(nlm_command PUBLIC nlm_core)
endif()

if(EXISTS ${NLM_SOURCE_DIR}/factory/BrainFactory.cpp)
    add_library(nlm_factory STATIC
        ${NLM_SOURCE_DIR}/factory/BrainFactory.cpp
    )
    target_include_directories(nlm_factory PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_link_libraries(nlm_factory PUBLIC nlm_core)
endif()

if(EXISTS ${NLM_SOURCE_DIR}/core/ErrorHandling.cpp)
    add_library(nlm_error_handling STATIC
        ${NLM_SOURCE_DIR}/core/ErrorHandling.cpp
    )
    target_include_directories(nlm_error_handling PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_link_libraries(nlm_error_handling PUBLIC nlm_core)
endif()

if(EXISTS ${NLM_SOURCE_DIR}/threading/ThreadSafeSimulationManager.cpp)
    add_library(nlm_threading STATIC
        ${NLM_SOURCE_DIR}/threading/ThreadSafeSimulationManager.cpp
    )
    target_include_directories(nlm_threading PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_link_libraries(nlm_threading PUBLIC nlm_core)
endif()

if(EXISTS ${NLM_SOURCE_DIR}/topology/AdvancedTopologyManager.cpp)
    add_library(nlm_topology STATIC
        ${NLM_SOURCE_DIR}/topology/AdvancedTopologyManager.cpp
    )
    target_include_directories(nlm_topology PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_link_libraries(nlm_topology PUBLIC nlm_core)
endif()

pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)

if(TARGET nlm_command)
    target_link_libraries(pynlm PRIVATE nlm_command)
endif()

if(TARGET nlm_factory)
    target_link_libraries(pynlm PRIVATE nlm_factory)
endif()

if(TARGET nlm_error_handling)
    target_link_libraries(pynlm PRIVATE nlm_error_handling)
endif()

if(TARGET nlm_threading)
    target_link_libraries(pynlm PRIVATE nlm_threading)
endif()

if(TARGET nlm_topology)
    target_link_libraries(pynlm PRIVATE nlm_topology)
endif()

target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)
