#!/bin/bash

# Simple build script for NLM with fallback dependencies
# This script creates a build directory, runs CMake, and builds the project

set -e

echo "=== NLM Build with Fallback Dependencies ==="
echo "Step 1: Creating build directory..."
mkdir -p build
cd build

echo "Step 2: Running CMake configuration..."
cmake -DNLOHMANN_JSON_FALLBACK=ON -DYAML_CPP_FALLBACK=ON ..

echo "Step 3: Building project..."
cmake --build . --config Release

echo "=== Build Completed Successfully ==="
echo "Generated files:"
echo "  - nlm (main executable)"
echo "  - libnlm_core.a (core library)"
echo "  - nlm_test (test executable)"
