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

set(NLM_AGENT_SOURCES
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)

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

# Performance optimization options
option(NLM_ENABLE_VECTORIZATION "Enable SIMD vectorization for performance" ON)
option(NLM_ENABLE_SPARSE_CONNECTIVITY "Enable sparse connectivity optimization" ON)
option(NLM_ENABLE_MEMORY_POOLS "Enable memory pools for better allocation" ON)
option(NLM_ENABLE_EVENT_QUEUES "Enable event-driven computation" ON)

# Vectorization definitions
if(NLM_ENABLE_VECTORIZATION)
    target_compile_definitions(nlm_core PRIVATE NLM_ENABLE_VECTORIZATION)
    target_compile_options(nlm_core PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-msse4 -msse2 -mavx>
        $<$<CXX_COMPILER_ID:Clang>:-msse4 -msse2 -mavx>
    )
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

add_library(nlm_agent STATIC ${NLM_AGENT_SOURCES})
target_include_directories(nlm_agent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

add_library(nlm_world STATIC ${NLM_WORLD_SOURCES})
target_include_directories(nlm_world PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Create a main simulation library that combines all components
add_library(nlm_simulation STATIC
    ${NLM_CORE_SOURCES}
    ${NLM_AGENT_SOURCES}
    ${NLM_WORLD_SOURCES}
)
target_include_directories(nlm_simulation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)

pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_simulation)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

# Install targets for Python package
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

# Install headers for Python development
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

# Create a custom target for building Python bindings
add_custom_target(build_python_bindings ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Building NLM Python bindings..."
    COMMAND ${Python_EXECUTABLE} -c "import pybind11; print('Python bindings configuration complete')"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Configuring NLM Python bindings for development"
    VERBATIM
)

# Print build information
message(STATUS "NLM Python Bindings Configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Python Executable: ${Python_EXECUTABLE}")
message(STATUS "  Python Version: ${Python_VERSION}")
message(STATUS "  Vectorization: ${NLM_ENABLE_VECTORIZATION}")
message(STATUS "  Sparse Connectivity: ${NLM_ENABLE_SPARSE_CONNECTIVITY}")
message(STATUS "  Memory Pools: ${NLM_ENABLE_MEMORY_POOLS}")
message(STATUS "  Event Queues: ${NLM_ENABLE_EVENT_QUEUES}")
message(STATUS "  Installation Prefix: ${CMAKE_INSTALL_PREFIX}")
