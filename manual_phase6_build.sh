#!/bin/bash
echo "=== Manual Phase 6 Demo Build ==="
cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_3084bd7b-4d0b-47e2-a85b-4a2bd920bad5
echo "Current directory: $(pwd)"
echo "Files:"
ls -la

echo ""
echo "Compiling Phase6Demo.cpp manually..."
g++ -std=c++20 -Isrc src/experiments/Phase6Demo.cpp -o phase6_manual_test

echo ""
echo "Checking compilation result..."
if [ -f phase6_manual_test ]; then
    echo "✓ Compilation successful"
    echo "Running Phase6Demo..."
    ./phase6_manual_test
else
    echo "✗ Compilation failed"
    g++ -std=c++20 -Isrc src/experiments/Phase6Demo.cpp -v 2>&1 | tail -20
fi