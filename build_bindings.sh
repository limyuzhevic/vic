#!/bin/bash

# Build script for NLM Python bindings

echo "Building NLM Python bindings..."

# Create build directory
mkdir -p build

# Configure CMake
CMAKE_ARGS="-DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON"

cd build
echo "Running: cmake $CMAKE_ARGS .."
cmake $CMAKE_ARGS ..

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

echo "Running: make -j$(nproc)"
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build successful!"
echo "You can now install with: pip install ."