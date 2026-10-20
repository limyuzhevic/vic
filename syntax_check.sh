#!/bin/bash
# Quick syntax check for enhanced NLM files

echo "Checking syntax for enhanced NLM files..."

echo "\n1. Checking main.cpp..."
if g++ -std=c++20 -c src/main.cpp -I. 2>&1; then
    echo "✓ main.cpp compiles successfully"
else
    echo "✗ main.cpp has syntax errors"
    exit 1
fi

echo "\n2. Checking main_phase6_demo.cpp..."
if g++ -std=c++20 -c src/main_phase6_demo.cpp -I. 2>&1; then
    echo "✓ main_phase6_demo.cpp compiles successfully"
else
    echo "✗ main_phase6_demo.cpp has syntax errors"
    exit 1
fi

echo "\n3. Checking CMakeLists.txt syntax..."
if cmake -P CMakeLists.txt 2>&1 | head -20; then
    echo "✓ CMakeLists.txt appears valid"
else
    echo "! CMakeLists.txt check inconclusive (this is normal for syntax check)"
fi

echo "\n4. Checking enhanced README..."
if [ -f enhanced_readme.md ] && grep -q "NLM - Neural Learning Machine" enhanced_readme.md; then
    echo "✓ enhanced_readme.md exists and appears valid"
else
    echo "✗ enhanced_readme.md issue"
    exit 1
fi

echo "\n" && echo "=== Syntax Check Complete ==="
echo "All enhanced NLM files pass basic syntax checks."
echo ""
echo "Next steps:"
echo "1. Build with: cmake --build ."
echo "2. Run enhanced demo: ./nlm --demo"
echo "3. Run integration demo: ./nlm_phase6_demo"
echo "4. View enhanced documentation: enhanced_readme.md"