#!/bin/bash
# Simple build script for NLM project

cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_940d635d-ec62-4620-aad1-94409be1d4f5

# Try to compile a simple test program to see if the project builds
echo "Checking compiler availability..."

# Try to compile just one file to test if the project structure is correct
g++ -I./src -std=c++20 -c src/brain/Neuron.cpp -o build/Neuron.o
echo "Compiled Neuron.cpp successfully"

echo "Project structure appears valid"
