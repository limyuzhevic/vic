#!/bin/bash

# Build script for NLM (Neural Learning Machine)
# Updated configuration

cd /workspace/app

# Set up build directory
BUILD_DIR="build"
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure with CMake
cmake ..

# Build all targets
make -j$(nproc)

# Build Python bindings
make pynlm

# Run tests
ctest --output-on-failure
