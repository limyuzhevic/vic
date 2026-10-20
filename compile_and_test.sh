#!/bin/bash
echo "Testing NLM Config System Compilation..."
cd /workspace/app

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building project..."
make -j4

# Run the config test
echo "Running config test..."
./config_test

# Check if tests passed
if [ $? -eq 0 ]; then
    echo "✓ All tests passed successfully!"
else
    echo "✗ Tests failed"
    exit 1
fi
