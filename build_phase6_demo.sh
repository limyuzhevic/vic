#!/bin/bash

# Build script for NLM Phase 6 Integration Demo
# ==============================================
# 
# This script builds the enhanced NLM Phase 6 demo with all the
# integration improvements we've implemented.
# 
# Phase 6 Integration improvements:
# - Fixed memory system accessors (getWorkingMemory, getEpisodicMemory)
# - Enhanced episodic memory storage (every step, not just every 10)
# - Improved working memory integration with recurrent connectivity
# - Complete neuromodulation (dopamine, serotonin, acetylcholine)
# - Enhanced concept formation and cognitive integration
# - Sleep/rest cycle with memory consolidation
# - Replay mechanisms for episodic memory
# - Enhanced checkpoint save/load with memory systems
# - Better sensory input processing and memory connectivity

set -e  # Exit on error

echo "Building NLM Phase 6 Integration Demo..."
echo "=========================================="
echo()

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: CMakeLists.txt not found in current directory"
    echo "Please run this script from the project root directory"
    exit 1
fi

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir -p build
    cd build
else
    cd build
fi

# Configure CMake with Release build type
echo "Configuring CMake (Release build)..."
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_STANDARD=20 \
         -DCMAKE_CXX_STANDARD_REQUIRED=ON \
         -DCMAKE_CXX_EXTENSIONS=OFF

if [ $? -ne 0 ]; then
    echo "Error: CMake configuration failed"
    exit 1
fi

# Build the project
echo "Building project..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "Error: Build failed"
    exit 1
fi

# Check if the demo executable was created
demo_path="nlm_phase6_demo"
if [ ! -f "$demo_path" ]; then
    echo "Error: Demo executable not found: $demo_path"
    echo "Available executables in build directory:"
    ls -la *.exe 2>/dev/null || ls -la
    exit 1
fi

echo()
echo "Build completed successfully!"
echo()
echo "Demo executable created: $(pwd)/$demo_path"
echo()
echo "You can now run the demo with:"
echo "  ./nlm_phase6_demo"
echo()
echo "Or run the Python demo script:"
echo "  cd .."
echo("  python3 demo_phase6_integration.py")
