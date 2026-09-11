#!/bin/bash
echo "Starting Phase 6 build test..."
echo "Working directory: $(pwd)"
echo "Files in workspace:"
ls -la

echo ""
echo "=== Checking CMakeLists.txt ==="
grep -n "Phase6Demo" CMakeLists.txt

echo ""
echo "=== Checking if Phase6Demo.cpp exists ==="
if [ -f "src/experiments/Phase6Demo.cpp" ]; then
    echo "✓ Phase6Demo.cpp exists"
else
    echo "✗ Phase6Demo.cpp missing"
fi

echo ""
echo "=== Attempting to build Phase 6 demo ==="
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
if [ $? -eq 0 ]; then
    echo "✓ CMake completed successfully"
    make -j4
    if [ $? -eq 0 ]; then
        echo "✓ Make completed successfully"
        if [ -f nlm_phase6_demo ]; then
            echo "✓ nlm_phase6_demo executable found"
            echo "=== Running Phase 6 demo ==="
            ./nlm_phase6_demo
        else
            echo "✗ nlm_phase6_demo executable not found"
            find . -name "*phase6*" -type f 2>/dev/null
        fi
    else
        echo "✗ Make failed"
    fi
else
    echo "✗ CMake failed"
fi