#!/usr/bin/env bash
# NLM - Neural Learning Machine - Demonstration Script
# Demonstrates the improved modular architecture and new features

echo "========================================="
echo "NLM Neural Learning Machine - Demo Script"
echo "========================================="
echo ""
echo "This script demonstrates the improvements made to the NLM project."
echo ""

# Check if the project builds
if [ -d "build" ]; then
    echo "Found existing build directory."
else
    echo "Creating build directory..."
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    echo "Running make..."
    make -j4
    cd ..
fi

# Check if the nlm executable exists
if [ ! -f "build/nlm" ]; then
    echo "Error: NLM executable not found. Build failed."
    echo "Please check the build output for errors."
    exit 1
fi

echo "✓ NLM executable built successfully"
echo ""

# Show the help message
echo "========================================="
echo "Demonstrating Improved CLI Interface"
echo "========================================="
echo ""
echo "Running: build/nlm --help"
echo ""
build/nlm --help

echo ""
echo "========================================="
echo "Demonstrating Statistics Export"
echo "========================================="
echo ""
echo "Running basic connectivity test with JSON export..."
echo ""
build/nlm --test basic-connectivity --output test_output.json

echo ""
echo "========================================="
echo "Checking Output Files"
echo "========================================="
echo ""

if [ -f "test_output.json" ]; then
    echo "✓ JSON output file created: test_output.json"
    echo ""
    echo "First few lines of JSON output:"
    echo ""
    head -20 test_output.json
else
    echo "✗ JSON output file not created"
fi

echo ""
echo "========================================="
echo "Demonstrating Error Handling"
echo "========================================="
echo ""
echo "Testing error handling with unknown option..."
echo ""
build/nlm --unknown-option 2>&1 | head -10

echo ""
echo "========================================="
echo "Demonstrating Version Information"
echo "========================================="
echo ""
echo "Running: build/nlm --version"
echo ""
build/nlm --version

echo ""
echo "========================================="
echo "Summary of Improvements"
echo "========================================="
echo ""
echo "The NLM project has been significantly improved with:"
echo ""
echo "1. ✓ Fixed critical bug in configuration file (duplicate dopamine_baseline)"
echo "2. ✓ Modular CLI interface with proper argument parsing"
echo "3. ✓ Comprehensive statistics export system (JSON, CSV, HTML)"
echo "4. ✓ Refactored monolithic main.cpp into modular components"
echo "5. ✓ Enhanced Python bindings with new modular components"
echo "6. ✓ Improved CMake configuration for better build management"
echo "7. ✓ Better error handling and user feedback"
echo "8. ✓ Enhanced documentation and examples"
echo ""
echo "Key Features:"
echo "- Modern command-line interface with type-safe parsing"
echo "- Multiple output formats for statistics (JSON, CSV, HTML)"
echo "- Modular architecture for easier maintenance and extension"
echo "- Backward compatible with existing functionality"
echo "- Better error messages and user guidance"
echo ""
echo "The NLM brain is now a more robust, user-friendly, and extensible neural simulation platform."
echo ""
echo "For more information, see IMPROVEMENTS_SUMMARY.md"
echo "========================================="
echo "Demo completed successfully!"
echo "========================================="