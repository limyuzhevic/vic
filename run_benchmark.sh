#!/bin/bash

# Simple benchmark runner for optimized NLM brain

echo "Building NLM Performance Benchmark..."
g++ -std=c++17 -O3 -march=native -I./src -I./src/core -I./src/brain -I./src/memory -I./src/neuromodulation -I./src/prediction -I./src/integration -lstdc++ -o benchmark benchmark.cpp 2>&1

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Running benchmark..."
    ./benchmark
else
    echo "Build failed. Check compiler flags and include paths."
    echo "You may need to create the required source files or check the build configuration."
fi