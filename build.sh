#!/bin/bash
# Build script for NLM project
# Build the NLM neural simulation framework

echo "=== Building NLM Project ==="
echo "Building NLM neural simulation framework..."
echo ""

echo "1. Configuring build with CMake..."
mkdir -p build
cd build
echo "Created build directory"

echo ""
echo "2. Running CMake configuration..."
cmake .. -DCMAKE_BUILD_TYPE=Release
echo "✓ CMake configuration completed"

echo ""
echo "3. Building all targets..."
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo ""
    echo "=== Build Successful ==="
    echo "✓ All targets built successfully"
    echo ""
    echo "Available executables:"
    echo "  nlm                    - Modern CLI with phase subcommands"
    echo "  nlm_phase3_demo        - Phase 3 demo executable"
    echo "  nlm_phase4_demo        - Phase 4 demo executable"
    echo "  nlm_phase6_demo        - Phase 6 demo executable"
    echo ""
    echo "Build complete. Run ./nlm --help for usage information."
else
    echo ""
    echo "Build failed. Please check the error messages above."
    exit 1
fi
