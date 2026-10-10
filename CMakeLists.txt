cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable profiling and benchmarking
option(NLM_ENABLE_PROFILING "Enable profiling support" ON)
option(NLM_ENABLE_BENCHMARKING "Enable benchmarking support" ON)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Build options
option(NLM_BUILD_TESTS "Build NLM tests" ON)
option(NLM_BUILD_EXAMPLES "Build examples" ON)
option(NLM_BUILD_DOCS "Build documentation" OFF)
option(NLM_ENABLE_VALIDATION "Enable validation checks" ON)

# Platform-specific definitions
if(WIN32)
    add_definitions(-DNLM_WINDOWS)
elseif(APPLE)
    add_definitions(-DNLM_MACOS)
elseif(UNIX)
    add_definitions(-DNLM_LINUX)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core module (contains fundamental types and utilities)
add_library(nlm_core
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Neural system module (brain and dynamics)
add_library(nlm_neural
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
)

target_include_directories(nlm_neural PUBLIC src)

target_link_libraries(nlm_neural PUBLIC nlm_core)

# Memory system module
add_library(nlm_memory
    src/memory/Memory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
)

target_include_directories(nlm_memory PUBLIC src)
target_link_libraries(nlm_memory PUBLIC nlm_core)

# Cognition system module
add_library(nlm_cognition
    src/cognition/NeuralPlanner.cpp
    src/cognition/ConceptFormation.cpp
    src/cognition/AttentionalSelection.cpp
)

target_include_directories(nlm_cognition PUBLIC src)
target_link_libraries(nlm_cognition PUBLIC nlm_core)

# Plasticity system module
add_library(nlm_plasticity
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
    src/plasticity/PlasticityRule.cpp
)

target_include_directories(nlm_plasticity PUBLIC src)
target_link_libraries(nlm_plasticity PUBLIC nlm_core)

# Systems module (high-level integration)
add_library(nlm_systems
    src/agent/AgentBrain.cpp
    src/agent/SensoryPercept.cpp
    src/world/SimpleWorld.cpp
)

target_include_directories(nlm_systems PUBLIC src)
target_link_libraries(nlm_systems PUBLIC nlm_core nlm_neural nlm_memory nlm_cognition nlm_plasticity)

# Installation targets
install(TARGETS nlm
    RUNTIME DESTINATION bin
)

install(TARGETS nlm_core nlm_neural nlm_memory nlm_cognition nlm_plasticity nlm_systems
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# Install headers
install(DIRECTORY src/brain/include/ DESTINATION include/brain)
install(DIRECTORY src/memory/include/ DESTINATION include/memory)
install(DIRECTORY src/cognition/include/ DESTINATION include/cognition)
install(DIRECTORY src/plasticity/include/ DESTINATION include/plasticity)
install(DIRECTORY src/agent/include/ DESTINATION include/agent)
install(DIRECTORY src/world/include/ DESTINATION include/world)
