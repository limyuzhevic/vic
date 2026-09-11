#!/bin/bash

cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_7d39c6dd-3f9c-4f65-ae6b-9977838e06d0

echo "=== C++ Source File Analysis ==="
echo ""

echo "1. Checking for .cpp files without corresponding .hpp files..."
missing_headers=0
for cpp in $(find src -name "*.cpp"); do
    base=$(basename "$cpp" .cpp)
    if [ ! -f "$(dirname "$cpp")/$base.hpp" ]; then
        echo "  MISSING HEADER: $cpp"
        ((missing_headers++))
    fi
done
echo "Total files missing headers: $missing_headers"
echo ""

echo "2. Checking header guard presence..."
headers_with_no_guard=0
for hpp in $(find src -name "*.hpp"); do
    if ! grep -q "#pragma once" "$hpp"; then
        echo "  NO GUARD: $hpp"
        ((headers_with_no_guard++))
    fi
done
echo "Total headers without guard: $headers_with_no_guard"
echo ""

echo "3. Checking for incomplete implementations (TODO comments)..."
todo_count=0
for cpp in $(find src -name "*.cpp"); do
    todos=$(grep -c "TODO" "$cpp" 2>/dev/null || echo 0)
    if [ "$todos" -gt 0 ]; then
        echo "  $cpp: $todos TODOs"
        ((todo_count+=todos))
    fi
done
echo "Total TODOs in .cpp files: $todo_count"
echo ""

echo "4. Checking CMakeLists.txt references..."
echo "Checking core library CMakeLists.txt..."
grep -o "src/[^ ]*\.cpp" CMakeLists.txt | wc -l
echo "Checking tests/CMakeLists.txt..."
grep -o "test_[^ ]*\.cpp" tests/CMakeLists.txt | wc -l
echo ""

echo "5. Scanning for obvious syntax errors..."
errors=0
for cpp in $(find src -name "*.cpp" -not -path "*/.git/*"); do
    # Basic syntax check with g++ -fsyntax-only
    if ! g++ -std=c++20 -fsyntax-only -I. "$cpp" 2>/dev/null; then
        echo "  POTENTIAL ISSUE: $cpp"
        ((errors++))
    fi
done
echo "Files with potential syntax issues: $errors"

echo ""
echo "=== Analysis Complete ==="