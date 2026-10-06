#!/usr/bin/env bash
# Build script for NLM - Phase 6 Integration Test

# Exit on error
 set -e

# Colors for output
 RED='\033[0;31m'
 GREEN='\033[0;32m'
 YELLOW='\033[1;33m'
 NC='\033[0m' # No Color

echo "Building NLM Phase 6 Integration Test..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring CMake..."
if command -v cmake &> /dev/null; then
    cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DCMAKE_CXX_STANDARD=20 \
             -DCMAKE_CXX_STANDARD_REQUIRED=ON \
             -DPYTHON_EXECUTABLE=$(which python3)
else
    echo -e "${RED}CMake not found! Please install CMake.${NC}"
    exit 1
fi

# Build the project
echo "Building NLM..."
if command -v make &> /dev/null; then
    make -j$(nproc)
else
    echo -e "${RED}make not found! Please install make.${NC}"
    exit 1
fi

# Check if build succeeded
if [ -f "nlm_phase6_demo" ]; then
    echo -e "${GREEN}Build successful!${NC}"
    echo "To run the integration test:"
    echo "  ./build/nlm_phase6_demo"
else
    echo -e "${RED}Build failed! Executable not found.${NC}"
    exit 1
fi
