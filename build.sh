#!/bin/bash

# NLM Build Script Wrapper
# This script provides a simple interface to the NLM build system

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

# Default options
BUILD_TYPE="Release"
BUILD_TESTS="ON"
BUILD_PYTHON="ON"
ENABLE_PROFILING="OFF"
ENABLE_COVERAGE="OFF"
BUILD_DOCS="OFF"
CONFIG_FILE=""

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --tests)
            if [[ "$2" == "ON" || "$2" == "OFF" ]]; then
                BUILD_TESTS="$2"
                shift 2
            else
                echo "Error: --tests requires ON or OFF"
                exit 1
            fi
            ;;
        --python-bindings)
            if [[ "$2" == "ON" || "$2" == "OFF" ]]; then
                BUILD_PYTHON="$2"
                shift 2
            else
                echo "Error: --python-bindings requires ON or OFF"
                exit 1
            fi
            ;;
        --profiling)
            if [[ "$2" == "ON" || "$2" == "OFF" ]]; then
                ENABLE_PROFILING="$2"
                shift 2
            else
                echo "Error: --profiling requires ON or OFF"
                exit 1
            fi
            ;;
        --coverage)
            if [[ "$2" == "ON" || "$2" == "OFF" ]]; then
                ENABLE_COVERAGE="$2"
                shift 2
            else
                echo "Error: --coverage requires ON or OFF"
                exit 1
            fi
            ;;
        --docs)
            BUILD_DOCS="ON"
            shift
            ;;
        --config)
            CONFIG_FILE="$2"
            shift 2
            ;;
        --help|-h)
            echo "Usage: $0 [options]"
            echo ""
            echo "Options:"
            echo "  --build-type TYPE        Build type (Debug, Release, RelWithDebInfo)"
            echo "  --tests ON|OFF           Enable/disable tests (default: ON)"
            echo "  --python-bindings ON|OFF Enable/disable Python bindings (default: ON)"
            echo "  --profiling ON|OFF       Enable performance profiling (default: OFF)"
            echo "  --coverage ON|OFF        Enable code coverage (default: OFF)"
            echo "  --docs                   Build documentation (default: OFF)"
            echo "  --config FILE            Configuration file (JSON or simple format)"
            echo "  -h, --help               Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0 --build-type Debug --tests ON"
            echo "  $0 --build-type Release --coverage ON"
            echo "  $0 --config configs/debug.json"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Build script
cmake_args="-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
cmake_args="${cmake_args} -DBUILD_TESTS=${BUILD_TESTS}"
cmake_args="${cmake_args} -DBUILD_PYTHON=${BUILD_PYTHON}"
cmake_args="${cmake_args} -DENABLE_PROFILING=${ENABLE_PROFILING}"
cmake_args="${cmake_args} -DENABLE_COVERAGE=${ENABLE_COVERAGE}"
cmake_args="${cmake_args} -DBUILD_DOCS=${BUILD_DOCS}"

if [[ -n "${CONFIG_FILE}" ]]; then
    cmake_args="${cmake_args} -DCONFIG_FILE=${CONFIG_FILE}"
fi

# Create build directory
if [[ -d "${BUILD_DIR}" ]]; then
    echo "Build directory '${BUILD_DIR}' already exists."
    read -p "Do you want to remove it and recreate? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        rm -rf "${BUILD_DIR}"
        mkdir "${BUILD_DIR}"
        echo "Build directory removed and recreated."
    else
        echo "Using existing build directory."
    fi
else
    mkdir "${BUILD_DIR}"
    echo "Build directory created."
fi

# Configure CMake
echo "Configuring CMake..."
cd "${BUILD_DIR}"
if ! cmake ${cmake_args} ..; then
    echo "CMake configuration failed."
    exit 1
fi

# Build project
echo "Building project..."
cpu_count=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
if ! make -j${cpu_count}; then
    echo "Build failed."
    exit 1
fi

# Summary
echo ""
echo "Build completed successfully!"
echo ""
echo "To run tests:"
echo "  cd ${BUILD_DIR}"
echo "  ./nlm_test                    # Run unit tests"
echo "  ./nlm_phase6_demo              # Run integration tests"
echo ""
echo "To run benchmarks:"
echo "  cd ${BUILD_DIR}"
echo "  ./benchmark"
echo ""
echo "To build documentation:"
echo "  cd ${BUILD_DIR}"
echo "  make doc"
echo ""
echo "To generate coverage report:"
echo "  cd ${BUILD_DIR}"
echo "  make coverage"