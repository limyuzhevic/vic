#!/bin/bash
# Build script for testing error handling enhancements

set -e  # Exit on any error

PROJECT_DIR="/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_5f7d145d-d3ef-44a3-9694-72e4baa627e4"

# Check for required dependencies
if ! command -v g++ &> /dev/null; then
    echo "Error: g++ compiler not found"
    exit 1
fi

if ! command -v cmake &> /dev/null; then
    echo "Error: cmake not found"
    exit 1
fi

# Create build directory
BUILD_DIR="${PROJECT_DIR}/build_error_handling_test"
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure with CMake
cmake "${PROJECT_DIR}" -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=OFF \
    -DCMAKE_CXX_FLAGS="-std=c++20 -Wall -Wextra -Wpedantic"

# Build the project
echo "Building NLM with error handling enhancements..."
make -j$(nproc)

# Check if build succeeded
if [ $? -ne 0 ]; then
    echo "Error: Build failed"
    exit 1
fi

# Run the test program
if [ -f "test_error_handling" ]; then
    echo "\nRunning error handling test..."
    ./test_error_handling
    TEST_RESULT=$?
else
    echo "Error: Test executable not found"
    TEST_RESULT=1
fi

# Clean up
cd "${PROJECT_DIR}"
rm -rf "${BUILD_DIR}"

if [ $TEST_RESULT -eq 0 ]; then
    echo "\n✓ Error handling test completed successfully"
else
    echo "\n✗ Error handling test failed"
    exit 1
fi
