# Simple CMakeLists.txt for testing the refactored Brain.cpp

cmake_minimum_required(VERSION 3.16)

project(NLM_Test VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/brain)

# Core library
add_library(nlm_core
    ../src/core/Config/Config.cpp
    ../src/core/Random/Random.cpp
    ../src/core/Logger/Logger.cpp
    ../src/core/SimulationClock/SimulationClock.cpp
)

# Refactored Brain
add_library(nlm_brain
    ../src/brain/Brain.cpp
    ../src/brain/BrainInitialization.cpp
    ../src/brain/BrainProcessingLoop.cpp
    ../src/brain/BrainIO.cpp
    ../src/brain/BrainUtilities.cpp
)

target_link_libraries(nlm_brain PRIVATE nlm_core)

# Test executable
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
    tests/test_memory_checkpointing.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_brain)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
