#!/usr/bin/env bash
# Simple test script for NLM enhanced CLI

set -e

echo "=== Testing NLM Enhanced CLI ==="

echo "1. Checking CMake configuration..."
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir -p build
fi

cd build

echo "2. Running CMake configuration..."
cmake .. -DCMAKE_BUILD_TYPE=Release

echo "3. Building project..."
make -j4

echo "4. Checking available executables..."
for exe in nlm nlm_legacy nlm_cli nlm_phase3_demo nlm_phase4_demo; do
    if [ -f "$exe" ]; then
        echo "  ✓ $exe exists"
    else
        echo "  ✗ $exe not found"
    fi
done

echo "5. Testing CLI help..."
if [ -f "./nlm" ]; then
    ./nlm --help > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        echo "  ✓ nlm --help works"
    else
        echo "  ✗ nlm --help failed"
    fi
else
    echo "  ✗ nlm executable not found"
fi

echo "6. Testing version information..."
if [ -f "./nlm" ]; then
    ./nlm --version > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        echo "  ✓ nlm --version works"
    else
        echo "  ✗ nlm --version failed"
    fi
else
    echo "  ✗ nlm executable not found"
fi

echo "7. Testing basic execution..."
if [ -f "./nlm" ]; then
    timeout 30 ./nlm --mode phase2 --preset minimal --quiet > /tmp/nlm_test.log 2>&1
    if [ $? -eq 0 ]; then
        echo "  ✓ Basic execution works"
        echo "  Output preview:"
        head -20 /tmp/nlm_test.log
    else
        echo "  ✗ Basic execution failed"
        cat /tmp/nlm_test.log
    fi
else
    echo "  ✗ nlm executable not found"
fi

echo "=== Test Complete ==="
