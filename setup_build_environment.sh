#!/usr/bin/env bash
# NLM Build System Environment Setup Script
# This script sets up the development environment for NLM

set -e

echo "=== NLM Build System Environment Setup ==="

# Detect system information
echo "Detected system: $(uname -s)"
echo "Processor count: $(nproc)"

# Check for required tools
command -v cmake >/dev/null 2>&1 || { echo "Error: cmake not found. Install with: sudo apt-get install cmake"; exit 1; }
command -v g++ >/dev/null 2>&1 || { echo "Error: g++ not found. Install with: sudo apt-get install g++"; exit 1; }
command -v make >/dev/null 2>&1 || { echo "Error: make not found. Install with: sudo apt-get install make"; exit 1; }

# Check CMake version
CMAKE_VERSION=$(cmake --version | head -1 | awk '{print $3}')
echo "CMake version: $CMAKE_VERSION"

# Create build directories
echo "Creating build directories..."
mkdir -p build/{linux,x64,macos,windows}
mkdir -p build/debug build/release build/relwithdebinfo build/minsizerel

# Create test directories
echo "Creating test directories..."
mkdir -p tests/data/{neuron,synapse,plasticity,memory}
mkdir -p tests/results/{unit,integration,performance,coverage}
mkdir -p benchmarks/{cpu,memory,performance}
mkdir -p docs/build-system

# Configure build options
echo "Configuring build options..."
cat > build/CMakeOptions.cmake << EOF
# Build configuration options
option(NLM_BUILD_SHARED "Build shared libraries instead of static" OFF)
option(NLM_BUILD_TESTS "Build tests" ON)
option(NLM_BUILD_EXAMPLES "Build examples" ON)
option(NLM_ENABLE_COVERAGE "Enable code coverage reporting" OFF)
option(NLM_ENABLE_SANITIZERS "Enable memory/debug sanitizers" OFF)
option(NLM_BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(NLM_ENABLE_PERF_MEASUREMENTS "Enable build-time performance measurements" ON)

# Platform-specific options
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
endif()
EOF

# Create default configuration files
echo "Creating default configuration files..."
cat > tests/data/default_test_config.json << EOF
{
    "test_framework": "google_test",
    "test_language": "C++",
    "test_standard": "C++20",
    "performance": {
        "benchmark_enabled": true,
        "benchmark_iterations": 1000,
        "benchmark_warmup": 100,
        "benchmark_tolerance": 0.01
    },
    "coverage": {
        "enabled": false,
        "report_format": "json",
        "threshold": 90.0
    }
}
EOF

echo "=== Environment setup complete ==="
echo "To build NLM: cd build && cmake .. && make -j$(nproc)"
echo "To run tests: cd build && ctest --output-on-failure"
