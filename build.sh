#!/bin/bash
# Build script for NLM (Neural Learning Machine)

# Set up build directory
BUILD_DIR="build"
SOURCE_DIR="."

# Create build directory
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure with CMake
echo "Configuring NLM with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Check if configuration was successful
if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

echo "Build configuration completed successfully."
echo "To build the project, run: make -j4"
echo "To run tests: ctest --output-on-failure"
