#!/usr/bin/env cmake -P
# Simple CMake script to test NLM functionality
# This script builds and runs a basic NLM example

cmake_minimum_required(VERSION 3.10)
project(NLMDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(nlm_demo
    src/main.cpp
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    src/dynamics/SpikeSystem.cpp
    src/plasticity/STDP.cpp
    src/plasticity/RewardModulatedSTDP.cpp
    src/plasticity/TripletSTDP.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/StructuralPlasticity.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    src/core/Types/Types.cpp
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    src/experiments/ExperimentRunner.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    src/prediction/PredictionSystem.cpp
)

# Target include directories
target_include_directories(nlm_demo PRIVATE
    src
    src/brain
    src/core
    src/dynamics
    src/plasticity
    src/memory
    src/prediction
    src/experiments
    src/neuromodulation
)

# Link libraries if needed
target_link_libraries(nlm_demo PRIVATE pthread)

# Add test target
add_test(NAME nlm_demo
    COMMAND nlm_demo --run-mode demo
)

# Print instructions for running the demo
message("NLM Demo Application")
message("===================")
message("")
message("To build and run the NLM demo:")
message("  mkdir build && cd build")
message("  cmake .. && make")
message("  ./nlm_demo --run-mode demo")
message("")
message("Available options:")
message("  --run-mode demo     - Basic demonstration with all plasticity rules")
message("  --run-mode experiment - Comprehensive learning experiment")
message("  --run-mode benchmark - Performance benchmark")
message("  --help              - Show help and usage information")
message("")
message("Configuration files:")
message("  configs/default.cfg - Full configuration with all features")
message("  configs/simple.cfg  - Minimal configuration for quick testing")
