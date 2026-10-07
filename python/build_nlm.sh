#!/usr/bin/env bash

# Simple script to build the NLM Python package
# This script can be used as an alternative to the setup.py file

set -e

PACKAGE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
NLM_SOURCE_DIR="$(cd "$PACKAGE_DIR/../src" && pwd)"

# Check if we're in the right directory
if [ ! -f "$NLM_SOURCE_DIR/CMakeLists.txt" ]; then
    echo "Error: NLM source directory not found"
    exit 1
fi

# Build NLM
BUILD_DIR="$PACKAGE_DIR/build"
mkdir -p "$BUILD_DIR"

cd "$BUILD_DIR"
cmake "$NLM_SOURCE_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_PYTHON_BINDINGS=ON \
    -DPYTHON_EXECUTABLE="$(which python3)" \
    "$@"

cmake --build "$BUILD_DIR" --config Release -j$(nproc)

# Install Python package
cd "$PACKAGE_DIR"

# Create Python package directory
PYTHON_PACKAGE_DIR="$PACKAGE_DIR/python/nlm"
mkdir -p "$PYTHON_PACKAGE_DIR"

# Create __init__.py files in all subdirectories
for dir in core brain agent world sensory motor environment logging config exceptions; do
    mkdir -p "$PYTHON_PACKAGE_DIR/$dir"
    touch "$PYTHON_PACKAGE_DIR/$dir/__init__.py"
    echo "# $dir module for NLM Python package" > "$PYTHON_PACKAGE_DIR/$dir/__init__.py"
    echo "__version__ = \"0.1.0\"" >> "$PYTHON_PACKAGE_DIR/$dir/__init__.py"
done

# Copy main __init__.py
touch "$PYTHON_PACKAGE_DIR/__init__.py"

echo "NLM Python package built successfully!"
echo "To install the package, run:"
echo "  pip install -e ."
echo ""
echo "To use NLM in Python:"
echo "  import nlm"
echo "  from nlm import NLM, create_agent"
echo "  # Create an agent"
echo "  agent = create_agent(config_file='my_config.json')"
echo "  # Or create a complete simulation"
echo "  sim = nlm.NLM(config_file='my_config.json')"