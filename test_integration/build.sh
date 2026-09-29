#!/bin/bash

# NLM Integration Test Builder and Runner
# Compiles and runs comprehensive integration tests for the NLM brain

set -e

echo "=== NLM Integration Test Builder ==="

# Check if g++ is available
if ! command -v g++ &> /dev/null; then
    echo "ERROR: g++ compiler not found"
    echo "Please install g++: sudo apt-get install g++"
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Compile with all necessary flags
echo "Compiling integration test..."
g++ -std=c++17 -Wall -Wextra -I../src -I../src/core -I../src/brain -I../src/memory -I../src/prediction -I../src/cognition -I../src/development -I../src/neuromodulation -I../src/sensory -I../src/motor -I../src/dynamics -I../src/plasticity -I../src/experiments -I../src/visualization -I../src/agent -I../src/world -c ../test_integration.cpp

if [ $? -eq 0 ]; then
    echo "✓ Compilation successful"
    
    # Create executable
    echo "Creating test executable..."
    g++ -std=c++17 -o test_integration test_integration.o
    
    if [ $? -eq 0 ]; then
        echo "✓ Test executable created successfully"
        
        # Run the tests
        echo "\n=== Running NLM Integration Tests ==="
        echo "Phase 6 Final Audit: Target >100/120 integration score"
        echo "==============================================="
        echo ""
        
        ./test_integration
        
        EXIT_CODE=$?
        
        if [ $EXIT_CODE -eq 0 ]; then
            echo ""
            echo "=== SUCCESS: All integration tests completed ==="
        else
            echo ""
            echo "=== WARNING: Integration tests completed with issues ==="
        fi
        
        # Cleanup
        echo "Cleaning up..."
        rm -f test_integration.o test_integration
        cd ..
        rm -rf build
        
        exit $EXIT_CODE
    else
        echo "ERROR: Failed to create executable"
        exit 1
    fi
else
    echo "ERROR: Compilation failed"
    exit 1
fi