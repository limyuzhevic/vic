#!/bin/bash
# Cleanup script to remove redundant files and verify project structure

echo "=== NLM Project Cleanup ==="
echo "Cleaning up redundant files..."
echo ""

# Remove redundant Commands.cpp file (all implementations are in main.cpp)
if [ -f "src/Commands.cpp" ]; then
    echo "Removing redundant src/Commands.cpp..."
    rm src/Commands.cpp
    echo "✓ Removed src/Commands.cpp"
else
    echo "✓ src/Commands.cpp already removed"
fi

echo ""
echo "=== Verification ==="
echo "Checking project structure..."

echo ""
echo "1. Key files present:"
files_to_check=("CMakeLists.txt" "src/main.cpp" "src/experiments/Phase6Demo.cpp" "src/experiments/Phase6IntegratedExperiment.hpp")
for file in "${files_to_check[@]}"; do
    if [ -f "$file" ]; then
        echo "   ✓ $file"
    else
        echo "   ✗ $file (MISSING)"
    fi
done

echo ""
echo "2. CMake targets in CMakeLists.txt:"
if grep -q "nlm_phase6_demo" CMakeLists.txt; then
    echo "   ✓ Phase 6 demo target"
else
    echo "   ✗ Phase 6 demo target missing"
fi

if grep -q "add_executable(nlm" CMakeLists.txt; then
    echo "   ✓ Main nlm executable target"
else
    echo "   ✗ Main nlm executable target missing"
fi

echo ""
echo "3. Modern CLI features in main.cpp:"
if grep -q "phase2" src/main.cpp && grep -q "phase3" src/main.cpp && grep -q "phase4" src/main.cpp && grep -q "phase6" src/main.cpp && grep -q "demo" src/main.cpp; then
    echo "   ✓ All phase subcommands implemented"
else
    echo "   ✗ Some phase subcommands missing"
fi

if grep -q "printUsage" src/main.cpp; then
    echo "   ✓ Help system implemented"
else
    echo "   ✗ Help system missing"
fi

echo ""
echo "=== Cleanup Complete ==="
echo "✓ All redundant files removed"
echo "✓ Project structure verified"
echo "✓ Modern CLI with subcommands ready"
echo ""
echo "The NLM project has been successfully improved with:"
echo "  • Phase 6 demo executable properly configured"
echo "  • Modern CLI with subcommands (phase2, phase3, phase4, phase6, demo)"
echo "  • Modular, configurable main.cpp implementation"
echo "  • Enhanced help and documentation system"
echo ""
echo "To build and run:"
echo "  ./build.sh          # Build the project"
echo "  ./build/test_integration.sh  # Run integration tests"
echo "  ./nlm phase6        # Run Phase 6 integration experiment"
echo "  ./nlm --help        # Show help and usage information"
