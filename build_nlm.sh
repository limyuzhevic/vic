#!/bin/bash
# Build script for NLM Phase 6 improvements

set -e

echo "Building NLM Phase 6 with improved integration..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=20

# Build the project
make -j4

echo "Build complete!"
echo "Key executables created:"
find . -maxdepth 1 -type f -name "nlm*" -executable | sort