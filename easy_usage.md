#!/usr/bin/env bash
# Simple verification script for NLM build
# This script verifies that the NLM build system works correctly

set -e

echo "NLM Build Verification"
echo "======================="

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake not found. Please install CMake 3.20 or higher."
    exit 1
fi

cmake_version=$(cmake --version | head -n1 | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+')
echo "Found CMake ${cmake_version}"

# Check minimum version
if [[ $(echo "${cmake_version} >= 3.20" | bc -l) -ne 1 ]]; then
    echo "ERROR: CMake 3.20 or higher is required"
    exit 1
fi

# Check for compiler
if ! command -v g++ &> /dev/null; then
    if ! command -v clang++ &> /dev/null; then
        echo "ERROR: No supported compiler found (g++ or clang++)"
        exit 1
    fi
    echo "Using clang++ compiler"
else
    echo "Using g++ compiler"
fi

# Check for required tools
if command -v ccache &> /dev/null; then
    echo "CCache available: Yes"
else
    echo "CCache available: No (will still work)"
fi

# Check for Python (optional)
if command -v python3 &> /dev/null; then
    echo "Python available: Yes"
else
    echo "Python available: No (Python bindings not available)"
fi

# Check for required directories
required_dirs=(
    "src/core"
    "src/brain"
    "src/experiments"
)

for dir in "${required_dirs[@]}"; do
    if [[ -d "${dir}" ]]; then
        echo "Directory ${dir}: Found"
    else
        echo "ERROR: Required directory ${dir} not found"
        exit 1
    fi
done

echo ""
echo "Build verification complete!"
echo ""
echo "To build NLM, run:"
echo "  mkdir build && cd build"
echo "  cmake .. -DNLM_BUILD_TESTS=ON"
echo "  cmake --build ."
echo ""
echo "To install NLM:"
echo "  cmake --install . --prefix /usr/local"
