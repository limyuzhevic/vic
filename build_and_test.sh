#!/bin/bash
# Build script for testing NLM enhanced Python bindings

set -e

echo "=== Building NLM Enhanced Python Bindings ==="

# Build C++ core library
echo "Building C++ core library..."
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

echo "Core library built successfully"

# Install Python package
echo "Building Python bindings..."
pip install -e .

echo "Python bindings built successfully"

# Run Python tests
echo "Running Python tests..."
python test_enhanced_bindings.py

# Return to workspace root
cd ..

echo "=== All builds and tests completed successfully ==="
echo "You can now run:"
echo "  - Python tests: python test_enhanced_bindings.py"
echo "  - C++ tests: ./build/nlm_test"
echo "  - Example usage: python -c 'import pynlm; brain = pynlm.createBrain(pynlm.createDefaultConfig()); print(f\"Neurons: {brain.getTotalNeuronCount()}\")'"