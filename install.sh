#!/bin/bash
# Installation script for NLM

set -e

echo "=== NLM Installation ==="
echo "Installing NLM Phase 6 brain simulator..."

# Create build directory
mkdir -p build
cd build

# Configure installation
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON

# Build and install
make -j$(nproc)
make install

echo "=== Installation Complete ==="
echo "NLM installed to /usr/local/bin"
echo "Core library installed to /usr/local/lib"
echo "Headers installed to /usr/local/include/nlm"

echo ""
echo "To use NLM:"
echo "1. Run demo: nlm_phase6_demo"
echo "2. Run main simulation: nlm"
echo "3. Install Python bindings: pip install pyproject.toml"
