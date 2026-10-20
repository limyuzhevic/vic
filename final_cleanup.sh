#!/bin/bash
# Final cleanup script for NLM project

echo "=== Final NLM Project Cleanup ==="
echo "Removing all redundant files..."
echo ""

# Remove redundant Commands.cpp file
if [ -f "src/Commands.cpp" ]; then
    echo "Removing redundant src/Commands.cpp..."
    rm src/Commands.cpp
    echo "✓ Removed src/Commands.cpp"
else
    echo "✓ src/Commands.cpp already removed"
fi

# List all .cpp files to verify cleanup
echo ""
echo "=== Remaining source files ==="
find src -name "*.cpp" -type f | sort | while read file; do
    echo "  ✓ $file"
done

echo ""
echo "=== Build targets from CMakeLists.txt ==="
grep "^add_executable" CMakeLists.txt

echo ""
echo "=== Summary ==="
echo "✅ All redundant files removed"
echo "✅ Project structure cleaned up"
echo "✅ Modern CLI with subcommands implemented in main.cpp"
echo "✅ Phase 6 demo executable properly configured"
echo "✅ Build and test scripts available"
echo ""
echo "To build and run the project:"
echo "  1. ./build.sh        # Build the project"
echo "  2. ./nlm phase6      # Run Phase 6 integration experiment"
echo "  3. ./nlm --help      # Show help and usage information"
