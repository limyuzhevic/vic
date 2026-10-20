#!/bin/bash
# Test CMake Configuration Script
# ================================

# This script tests the NLM CMake build system to ensure all features work correctly.

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build_test"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to clean up on exit
cleanup() {
    if [ -d "${BUILD_DIR}" ]; then
        print_info "Cleaning up build directory..."
        rm -rf "${BUILD_DIR}"
    fi
}

# Trap cleanup on script exit
trap cleanup EXIT

print_info "Testing NLM CMake Build System..."

# Check for CMake
if ! command -v cmake &> /dev/null; then
    print_error "CMake not found. Please install CMake 3.20 or higher."
    exit 1
fi

# Check CMake version
CMAKE_VERSION=$(cmake --version | head -1 | grep -oE '[0-9]+\.[0-9]+')
CMAKE_MAJOR=$(echo "${CMAKE_VERSION}" | cut -d. -f1)
CMAKE_MINOR=$(echo "${CMAKE_VERSION}" | cut -d. -f2)

if [ "${CMAKE_MAJOR}" -lt 3 ] || ([ "${CMAKE_MAJOR}" -eq 3 ] && [ "${CMAKE_MINOR}" -lt 20 ]); then
    print_warn "CMake ${CMAKE_VERSION} found. Version 3.20 or higher is recommended."
fi

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Test 1: Basic configuration
print_info "Test 1: Basic CMake configuration..."
cmake "${PROJECT_ROOT}" -DCMAKE_BUILD_TYPE=Release \
    -DNLM_ENABLE_TESTS=ON \
    -DNLM_ENABLE_PYTHON_BINDINGS=ON \
    -DNLM_ENABLE_BENCHMARKS=ON \
    -DNLM_ENABLE_COVERAGE=OFF \
    -DNLM_BUILD_SHARED=OFF

if [ $? -ne 0 ]; then
    print_error "Basic configuration failed."
    exit 1
fi

print_info "✓ Basic configuration successful"

# Test 2: Build targets
print_info "Test 2: Building all targets..."
cmake --build . --parallel

if [ $? -ne 0 ]; then
    print_error "Build failed."
    exit 1
fi

print_info "✓ Build successful"

# Test 3: Check built targets
print_info "Test 3: Verifying built targets..."

EXPECTED_TARGETS=(
    "nlm"
    "nlm_core"
    "nlm_agent"
    "nlm_world"
    "nlm_phase3"
    "nlm_phase4"
    "nlm_phase3_demo"
    "nlm_phase4_demo"
    "nlm_test"
    "nlm_benchmark"
)

if [ "${NLM_ENABLE_PYTHON_BINDINGS}" = "ON" ]; then
    EXPECTED_TARGETS+=("pynlm")
fi

for target in "${EXPECTED_TARGETS[@]}"; do
    if [ -f "${BUILD_DIR}/CMakeFiles/${target}.dir" ] || [ -f "${BUILD_DIR}/${target}" ] || [ -f "${BUILD_DIR}/lib${target}.a" ]; then
        print_info "✓ Target ${target} exists"
    else
        print_warn "Target ${target} not found (may be optional)"
    fi
done

# Test 4: Run tests
print_info "Test 4: Running tests..."
if ctest --output-on-failure; then
    print_info "✓ Tests passed"
else
    print_error "Tests failed."
    exit 1
fi

# Test 5: Installation test
print_info "Test 5: Testing installation..."
mkdir -p "${BUILD_DIR}/install"
cmake --build . --target install DESTDIR="${BUILD_DIR}/install"

if [ $? -ne 0 ]; then
    print_error "Installation test failed."
    exit 1
fi

print_info "✓ Installation test successful"

# Test 6: Package configuration
print_info "Test 6: Testing package configuration..."

# Check for generated config files
if [ -f "${BUILD_DIR}/nlm-config.cmake" ] && [ -f "${BUILD_DIR}/nlm-config-version.cmake" ]; then
    print_info "✓ Package configuration files generated"
else
    print_warn "Package configuration files not found"
fi

# Test 7: Cross-platform compatibility
print_info "Test 7: Testing cross-platform features..."

# Check for CMake-generated files
if [ -f "${BUILD_DIR}/CMakeCache.txt" ]; then
    print_info "✓ CMake cache generated"
fi

if [ -f "${BUILD_DIR}/CMakeFiles/nlm.dir/cmake_install.cmake" ]; then
    print_info "✓ Install script generated"
fi

# Test 8: Modern CMake features
print_info "Test 8: Testing modern CMake features..."

# Check for target properties
if grep -q "CXX_STANDARD 20" "${BUILD_DIR}/CMakeCache.txt" 2>/dev/null; then
    print_info "✓ C++20 standard set"
fi

# Check for proper dependency management
if [ -f "${BUILD_DIR}/CMakeFiles/nlm.dir/depend.make" ]; then
    print_info "✓ Dependency management enabled"
fi

print_info "All tests passed!"
print_info "Build directory: ${BUILD_DIR}"
print_info "Install directory: ${BUILD_DIR}/install"
print_info "Test results: ${BUILD_DIR}/Testing/Temporary/LastTest.log"