# NLM CMake Configuration
# Project root CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(NLM LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Google Test
enable_testing()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Source files
set(NLM_SOURCES
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/Brain.cpp
    src/plasticity/STDP.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/PlasticityRule.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    src/neuromodulation/Curiosity.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Reward.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
)

# Create library
add_library(nlm ${NLM_SOURCES})

# Tests
add_subdirectory(tests)
