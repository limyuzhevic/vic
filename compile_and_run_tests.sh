#!/bin/bash

# Compile the tests

echo "Compiling NLM tests..."

# Create build directory
mkdir -p build

cd build

# Try to compile the test program
if g++ -std=c++20 -I../src -I. \
    ../tests/test_main.cpp \
    ../tests/test_types.cpp \
    ../tests/test_config.cpp \
    ../tests/test_random.cpp \
    ../tests/test_clock.cpp \
    ../tests/test_neuron.cpp \
    ../tests/test_synapse.cpp \
    ../tests/test_stdp.cpp \
    ../tests/test_brain.cpp \
    ../test_lif_stability.cpp \
    ../src/core/Types/Types.cpp \
    ../src/core/Config/Config.cpp \
    ../src/core/Random/Random.cpp \
    ../src/core/Logger/Logger.cpp \
    ../src/core/SimulationClock/SimulationClock.cpp \
    ../src/brain/Neuron.cpp \
    ../src/brain/Synapse.cpp \
    ../src/brain/NeuralPopulation.cpp \
    ../src/brain/NeuralRegion.cpp \
    ../src/brain/Brain.cpp \
    ../src/dynamics/NeuralDynamics.cpp \
    ../src/dynamics/SpikeSystem.cpp \
    ../src/plasticity/PlasticityRule.cpp \
    ../src/plasticity/Hebbian.cpp \
    ../src/plasticity/STDP.cpp \
    ../src/plasticity/StructuralPlasticity.cpp \
    ../src/development/DevelopmentSystem.cpp \
    ../src/development/Synaptogenesis.cpp \
    ../src/development/Pruning.cpp \
    ../src/development/Maturation.cpp \
    -o nlm_test \
    -Wall -Wextra -Wpedantic -O2 2>&1

then
    echo "Compilation successful!"
    echo "Running tests..."
    echo ""
    ./nlm_test
else
    echo "Compilation failed!"
    echo ""
    echo "Testing LIF stability with Python script instead..."
    echo ""
    python3 test_lif_stability.py
fi

