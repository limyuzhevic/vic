#!/bin/bash

# Integration Test Runner for NLM
# Runs comprehensive integration tests for all codebase improvements

echo "========================================"
echo "NLM Integration Test Suite"
echo "========================================"
echo "Running comprehensive tests for all codebase improvements..."
echo ""

# Compile the integration test
if g++ -std=c++20 -I./src -I./src/core -I./src/brain -I./src/agent -I./src/world -c "./tests/test_integration.cpp" -o "./tests/test_integration.o" 2>/dev/null; then
    echo "✓ Integration test compiled successfully"
    
    # Link the integration test
    if g++ "./tests/test_integration.o" -o "./tests/test_integration" 2>/dev/null; then
        echo "✓ Integration test linked successfully"
        
        # Run the integration test
        echo ""
        ./tests/test_integration
        result=$?
        
        if [ $result -eq 0 ]; then
            echo ""
            echo "========================================"
            echo "✅ INTEGRATION TESTS PASSED!"
            echo "========================================"
            echo ""
            echo "The comprehensive integration test suite verifies:"
            echo "1. JSON Parser implementation with backward compatibility"
            echo "2. AgentBrain advanced features and bug fixes"
            echo "3. Backward compatibility with old key=value format"
            echo "4. Python bindings enhancements and error handling"
            echo "5. Constants centralization and magic number elimination"
            echo "6. Improved error handling and graceful degradation"
            echo "7. Performance optimizations and memory safety"
            echo ""
            echo "All improvements have been successfully integrated into the codebase."
        else
            echo ""
            echo "========================================"
            echo "❌ INTEGRATION TESTS FAILED"
            echo "========================================"
            echo ""
            exit $result
        fi
    else
        echo "❌ Failed to link integration test"
        echo ""
        echo "Common linking issues:"
        echo "1. Missing source files in the tests/ directory"
        echo "2. Library dependency issues"
        echo "3. Compiler version compatibility"
        echo ""
        echo "Please ensure all required source files are present in tests/:"
        ls -la ./tests/*.cpp 2>/dev/null | head -20
        exit 1
    fi
else
    echo "❌ Failed to compile integration test"
    echo ""
    echo "This may be due to:"
    echo "1. Missing source files"
    echo "2. Compiler flags or version issues"
    echo "3. Include path problems"
    echo ""
    echo "Please check if all required source files are present:"
    ls -la ./src/core/*.cpp 2>/dev/null | head -10
    echo "  (and other source files in src/ subdirectories)"
    exit 1
fi
