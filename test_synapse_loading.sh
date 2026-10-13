#!/usr/bin/env bash

# Test script for SynapseLoading implementation

# Set up environment
PROJECT_DIR="/workspace/app"
BUILD_DIR="${PROJECT_DIR}/build"

# Create build directory
mkdir -p "${BUILD_DIR}"

cd "${BUILD_DIR}"

# Configure with CMake
echo "Configuring project with CMake..."
cmake "${PROJECT_DIR}" -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTING=ON \
    -DCMAKE_CXX_STANDARD=17 \
    -DCMAKE_CXX_FLAGS="-Wall -Wextra -Werror"

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

echo "CMake configuration successful!"

# Build the project
echo "Building project..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build successful!"

# Run tests if they exist
if [ -f "${BUILD_DIR}/test_brain" ]; then
    echo "Running brain tests..."
    ./test_brain
    if [ $? -eq 0 ]; then
        echo "Brain tests passed!"
    else
        echo "Brain tests failed!"
        exit 1
    fi
fi

if [ -f "${BUILD_DIR}/test_synapse" ]; then
    echo "Running synapse tests..."
    ./test_synapse
    if [ $? -eq 0 ]; then
        echo "Synapse tests passed!"
    else
        echo "Synapse tests failed!"
        exit 1
    fi
fi

if [ -f "${BUILD_DIR}/test_synapse_loading" ]; then
    echo "Running synapse loading tests..."
    ./test_synapse_loading
    if [ $? -eq 0 ]; then
        echo "Synapse loading tests passed!"
    else
        echo "Synapse loading tests failed!"
        exit 1
    fi
fi

echo "All tests passed successfully!"
