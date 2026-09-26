#!/bin/bash
# Test script for NLM Advanced CLI

set -e

echo "=== NLM Advanced CLI Test Script ==="
echo "Testing basic functionality and compilation..."

echo ""
echo "1. Testing file structure..."

# Check for required files
if [ ! -f "src/cli/AdvancedCLI.hpp" ]; then
    echo "✗ Missing AdvancedCLI.hpp"
    exit 1
else
    echo "✓ AdvancedCLI.hpp exists"
fi

if [ ! -f "src/cli/AdvancedCLI.cpp" ]; then
    echo "✗ Missing AdvancedCLI.cpp"
    exit 1
else
    echo "✓ AdvancedCLI.cpp exists"
fi

if [ ! -f "AdvancedCLI.md" ]; then
    echo "✗ Missing AdvancedCLI.md"
    exit 1
else
    echo "✓ AdvancedCLI.md exists"
fi

if [ ! -f "IMPLEMENTATION_SUMMARY.md" ]; then
    echo "✗ Missing IMPLEMENTATION_SUMMARY.md"
    exit 1
else
    echo "✓ IMPLEMENTATION_SUMMARY.md exists"
fi

if [ ! -f "CMakeLists.txt" ]; then
    echo "✗ Missing CMakeLists.txt"
    exit 1
else
    echo "✓ CMakeLists.txt exists"
fi

echo ""
echo "2. Testing CMake configuration..."

# Create build directory
mkdir -p build
cd build

# Configure CMake
echo "Configuring CMake..."
if ! cmake .. > cmake_output.txt 2>&1; then
    echo "✗ CMake configuration failed"
    cat cmake_output.txt
    exit 1
else
    echo "✓ CMake configuration successful"
fi

# Build the project
echo ""
echo "3. Building NLM with CLI support..."
echo "Building nlm_cli library..."
if ! make nlm_cli > build_output.txt 2>&1; then
    echo "✗ Build failed for nlm_cli"
    cat build_output.txt | tail -20
    exit 1
else
    echo "✓ nlm_cli library built successfully"
fi

# Build main executable
echo ""
echo "Building nlm executable..."
if ! make nlm > build_output.txt 2>&1; then
    echo "✗ Build failed for nlm"
    cat build_output.txt | tail -20
    exit 1
else
    echo "✓ nlm executable built successfully"
fi

echo ""
echo "4. Testing basic CLI functionality..."

# Test help command
echo "Testing help command..."
if ! ./nlm --help > help_output.txt 2>&1; then
    echo "✗ Help command failed"
    exit 1
else
    echo "✓ Help command executed successfully"
fi

# Test version command
echo ""
echo "Testing version command..."
if ! ./nlm --version > version_output.txt 2>&1; then
    echo "✗ Version command failed"
    exit 1
else
    echo "✓ Version command executed successfully"
fi

# Test unknown command (should fail gracefully)
echo ""
echo "Testing unknown command handling..."
if ./nlm unknown_command > unknown_output.txt 2>&1; then
    echo "✗ Unknown command should have failed"
    exit 1
else
    echo "✓ Unknown command handled gracefully"
fi

echo ""
echo "5. Testing configuration file parsing..."

# Test config validate (with default config)
echo "Testing config validation..."
if ! ./nlm config validate > validate_output.txt 2>&1; then
    echo "✗ Config validation failed"
    exit 1
else
    echo "✓ Config validation executed successfully"
fi

echo ""
echo "6. Testing configuration listing..."

# Test config list
echo "Testing config listing..."
if ! ./nlm config list > list_output.txt 2>&1; then
    echo "✗ Config list failed"
    exit 1
else
    echo "✓ Config listing executed successfully"
fi

echo ""
echo "=== All tests passed! ==="
echo ""
echo "Summary:"
echo "- CLI implementation: Complete"
echo "- CMake integration: Successful"
echo "- Basic functionality: Working"
echo "- Error handling: Proper"
echo ""
echo "The NLM Advanced CLI is ready for use."
echo ""
echo "Usage examples:"
echo "  nlm --help                    # Show help"
echo "  nlm --version                 # Show version"
echo "  nlm config list               # List configuration"
echo "  nlm config validate           # Validate configuration"
echo ""
echo "For detailed documentation, see AdvancedCLI.md"
