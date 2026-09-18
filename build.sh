#!/bin/bash
# Build script for NLM

set -e  # Exit on error

echo "=== NLM Build Script ==="
echo "Building NLM Phase 6 integrated brain..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON

# Build the project
echo "Building NLM..."
make -j$(nproc)

# Run tests if available
if [ -f ./nlm_test ]; then
    echo "Running tests..."
    ./nlm_test
fi

echo "=== Build Complete ==="
echo "Binaries available in build/ directory"
echo "Main executable: build/nlm"
echo "Phase 6 demo: build/nlm_phase6_demo"
