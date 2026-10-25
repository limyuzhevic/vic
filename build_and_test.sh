#!/bin/bash

# Try to build the project
cd /workspace/app

if command -v cmake >/dev/null 2>&1 && command -v make >/dev/null 2>&1; then
    echo "Building NLM project..."
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    if [ $? -eq 0 ]; then
        make -j4
        if [ $? -eq 0 ]; then
            echo "Build successful!"
            # Try to run tests
            if [ -f ./nlm_test ]; then
                echo "Running tests..."
                ./nlm_test
            fi
        else
            echo "Build failed but continuing with code analysis..."
        fi
    else
        echo "CMake configuration failed..."
    fi
else
    echo "CMake or make not available, proceeding with code analysis..."
fi