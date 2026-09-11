#!/bin/bash
echo "=== Simple Phase 6 Build Test ==="
echo "Current working directory: $(pwd)"
echo ""
echo "Files:"
ls -la

echo ""
echo "=== Checking Phase6Demo.cpp ==="
if [ -f "src/experiments/Phase6Demo.cpp" ]; then
    echo "✓ Phase6Demo.cpp exists"
    echo "Content preview:"
    head -20 src/experiments/Phase6Demo.cpp
else
    echo "✗ Phase6Demo.cpp does not exist"
fi

echo ""
echo "=== Attempting to compile Phase6Demo.cpp manually ==="
g++ -std=c++20 -Isrc -g src/experiments/Phase6Demo.cpp -o phase6_manual_test

echo ""
if [ -f phase6_manual_test ]; then
    echo "✓ Manual compilation successful"
    echo "=== Running Phase 6 demo ==="
    ./phase6_manual_test
else
    echo "✗ Manual compilation failed"
    echo "Trying with debug info..."
    g++ -std=c++20 -Isrc -g src/experiments/Phase6Demo.cpp -o phase6_manual_test2 2>&1 | tail -30
fi