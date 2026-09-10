#!/bin/bash
# Quick build and test script for NLM

# Build the project
mkdir -p build
cd build
cmake ..
make -j4

# Run tests
./tests/test_neuron
./tests/test_synapse
./tests/test_stdp
./tests/test_brain
./tests/test_config
./tests/test_clock
./tests/test_random
./tests/test_main