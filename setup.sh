#!/usr/bin/env bash
# NLM - Setup and Build Script
# This script helps set up the NLM environment for new users

set -e

echo "=== NLM Environment Setup ==="
echo "Setting up Neural Learning Machine for development..."

# Check for required tools
check_tool() {
    if ! command -v $1 &> /dev/null; then
        echo "ERROR: $1 is required but not installed. Please install $1 and try again."
        exit 1
    fi
}

check_tool "g++"
check_tool "cmake"
check_tool "git"

# Create build directory
mkdir -p build
cd build

# Configure CMake
echo "Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building NLM..."
make -j$(nproc)

echo ""
echo "=== Build Complete ==="
echo ""
echo "The following executables have been built:"
echo "  nlm              - Main neural network simulation (Phase 2)"
echo "  nlm_phase3_demo   - Phase 3 environment interaction demo"
echo "  nlm_phase4_demo   - Phase 4 cognition demo"
echo "  nlm_phase5_demo   - Phase 5 development demo"
echo "  nlm_phase6_demo   - Phase 6 integration demo"
echo "  brain_tool        - Advanced brain analysis tool"
echo ""
echo "To run the main simulation:"
echo "  cd /path/to/nlm/build"
echo "  ./nlm"
echo ""
echo "To view help and usage:"
echo "  ./nlm --help"
echo ""
echo "To run the integration demo (Phase 6):"
echo "  ./nlm_phase6_demo"
echo ""
echo "To use the brain tool:"
echo "  ./brain_tool"
echo ""
echo "For more documentation, see:"
echo "  docs/ARCHITECTURE.md"
echo "  docs/SCIENCE.md"
echo "  docs/ROADMAP.md"
echo ""
echo "Enjoy experimenting with NLM!" >&2
exit 0
