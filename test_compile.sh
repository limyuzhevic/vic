#!/bin/bash
# Compile test to check for compilation errors

echo "Compiling project..."
g++ -std=c++17 -I./src -I./src/brain -I./src/agent -I./src/core -I./src/plasticity -I./src/dynamics -I./src/neuromodulation -I./src/environment -I./src/memory -I./src/cognition -I./src/sensory -I./src/motor -I./src/world -I./src/visualization -I./src/performance -I./src/experiments -I./src/development src/brain/Neuron.cpp src/brain/Brain.cpp tests/test_neuron.cpp -o test_neuron 2>&1

echo "Compilation complete. Checking for errors..."