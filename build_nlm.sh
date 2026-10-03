#!/usr/bin/env bash
# Build script for NLM improvements
# This script builds the NLM framework with all improvements

set -e

echo "Building NLM (熙然) Neural Learning Machine with improvements..."

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir -p build
    cd build
else
    cd build
fi

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DPYTHON_EXECUTABLE=$(which python) \
    -DENABLE_TESTS=ON \
    -DENABLE_PYTHON_BINDINGS=ON \
    -DENABLE_JSON_CONFIG=ON \
    -DENABLE_YAML_CONFIG=ON

# Build the project
echo "Building project..."
make -j$(nproc)

# Build Python bindings (if in separate directory)
cd ..
if [ -d "python" ]; then
    echo "Building Python bindings..."
    mkdir -p python/build
    cd python/build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=$(which python)
    make -j$(nproc)
    cd ../..
fi

# Install Python bindings
echo "Installing Python bindings..."
pip install -e . --no-build-isolation

# Run tests if build directory exists
echo "Running tests..."
if [ -f "build/nlm_test" ]; then
    ./build/nlm_test
fi

echo "Build completed successfully!"
echo "You can run the demo with: ./build/nlm"
echo "Python bindings can be imported with: import pynlm"
echo "\nTo run JSON/YAML config tests:"
echo "  cd build"
echo "  ./nlm_test (this will run all tests including new JSON/YAML config tests)"