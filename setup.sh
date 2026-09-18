#!/bin/bash

# NLM Development Environment Setup Script
# This script sets up the development environment for NLM Phase 6

set -e

echo "=== NLM Development Environment Setup ==="

# Check for required tools
check_command() {
    if ! command -v $1 &> /dev/null; then
        echo "Error: $1 is required but not installed."
        echo "Please install $1 and try again."
        exit 1
    fi
}

check_command "cmake"
check_command "make"
check_command "g++"
check_command "python3"
check_command "pip3"

echo "✓ All required tools are installed"

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir -p build
fi

echo "=== Configuring CMake ==="
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -GNinja

echo "=== Building NLM ==="
make -j$(nproc)

echo "=== Building Python bindings ==="
cd ..
if [ -f "pyproject.toml" ]; then
    pip3 install -e .
else
    echo "Warning: pyproject.toml not found. Python bindings may not be installed."
fi

echo "=== Setup completed successfully ==="
echo ""
echo "You can now build and run NLM with:"
echo "  cd build"
echo "  ./nlm           # Run main executable"
echo "  ./nlm_phase6_demo  # Run Phase 6 integration test"
echo ""
echo "To install development dependencies, run:"
echo "  pip3 install pytest numpy scikit-build pybind11"
echo ""
echo "To run tests:"
echo "  cd build"
echo "  make test"
