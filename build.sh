#!/bin/bash
# Build script for NLM Phase 6

echo "Building NLM Phase 6..."

# Create build directory
mkdir -p build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=20

# Build
make -j$(nproc)

# Run tests if available
if [ -f "Makefile" ]; then
    if ctest --output-on-failure; then
        echo "Tests passed!"
    else
        echo "Tests failed!"
        exit 1
    fi
fi

echo "Build complete!"
