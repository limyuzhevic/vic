#!/bin/bash

# NLM Build and Test Script
# This script builds the NLM project and runs tests

set -e

# Configuration
BUILD_DIR="build"
CONFIG_TYPE="Release"
SKIP_TESTS=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            CONFIG_TYPE="Debug"
            shift
            ;;
        --skip-tests)
            SKIP_TESTS=true
            shift
            ;;
        --help)
            echo "Usage: $0 [--debug] [--skip-tests]"
            echo "  --debug: Build with debug configuration"
            echo "  --skip-tests: Skip running tests"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Create build directory
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure CMake
echo "Configuring NLM with CMake ($CONFIG_TYPE)..."
cmake .. -DCMAKE_BUILD_TYPE=$CONFIG_TYPE

# Build the project
echo "Building NLM..."
make -j4

# Run tests if not skipped
if [ "$SKIP_TESTS" = false ]; then
    echo "Running NLM tests..."
    ./nlm --test
    
    echo "Running NLM integration tests..."
    ./nlm_phase3_demo
    ./nlm_phase4_demo
else
    echo "Skipping tests as requested"
fi

# Return to original directory
cd ..

echo "Build and test completed successfully!"