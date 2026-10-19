#!/bin/bash

# NLM Test Runner Script
# Runs all tests and provides detailed output

set -e

echo "=========================================="
echo "NLM Test Suite"
echo "=========================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to run a test
run_test() {
    local test_name="$1"
    local test_file="$2"
    
    echo -n "Running ${YELLOW}${test_name}${NC}... "
    
    if [ -f "$test_file" ]; then
        if g++ -std=c++20 -O2 "tests/${test_file}" "-I src" "-I src/core/Types" "-I src/core/Random" "-I src/core/Logger" "-I src/core/SimulationClock" "-I src/core/Config" "-I src/brain" "-I src/memory" "-I src/neuromodulation" "-I src/prediction" "-I src/cognition" "-I src/dynamics" "-I src/plasticity" "-I src/development" "-I src/sensory" "-I src/motor" "-I src/environment" "-I src/world" "-I src/experiments" "-I src/agent" "-I src/visualization" "-I src/performance" -o "tests/${test_file%.cpp}.out" && ./"tests/${test_file%.cpp}.out" >/dev/null 2>&1 && echo -e "${GREEN}PASSED${NC}"
        then
            echo -e "${GREEN}PASSED${NC}"
        else
            echo -e "${RED}FAILED${NC}"
            return 1
        fi
    else
        echo -e "${RED}NOT FOUND${NC}"
        return 1
    fi
}

# Function to run memory leak tests
run_memory_tests() {
    echo ""
    echo "=========================================="
    echo "Memory Leak Tests"
    echo "=========================================="
    
    # Compile with address sanitizer for memory leak detection
    echo "Compiling tests with address sanitizer..."
    
    # Test memory management in core classes
    if g++ -std=c++20 -O2 -g -fsanitize=address "tests/test_memory_management.cpp" "-I src" "-I src/core/Types" "-I src/core/Random" "-I src/core/Logger" "-I src/core/SimulationClock" "-I src/core/Config" "-I src/brain" "-I src/memory" "-I src/neuromodulation" "-I src/prediction" "-I src/cognition" "-I src/dynamics" "-I src/plasticity" "-I src/development" "-I src/sensory" "-I src/motor" "-I src/environment" "-I src/world" "-I src/experiments" "-I src/agent" "-I src/visualization" "-I src/performance" -o "tests/test_memory_management" 2>/dev/null; then
        echo -n "Running memory management tests... "
        if ./tests/test_memory_management; then
            echo -e "${GREEN}PASSED${NC}"
        else
            echo -e "${RED}FAILED${NC}"
            return 1
        fi
    else
        echo -e "${RED}COMPILATION FAILED${NC}"
        return 1
    fi
}

# Function to run integration tests
run_integration_tests() {
    echo ""
    echo "=========================================="
    echo "Integration Tests"
    echo "=========================================="
    
    # Test the complete brain system
    run_test "Brain Integration Test" "test_brain.cpp"
    
    # Test memory systems
    run_test "Working Memory Test" "test_memory.cpp"
    
    # Test neuromodulation
    run_test "Neuromodulation Test" "test_neuromodulation.cpp"
    
    # Test prediction system
    run_test "Prediction System Test" "test_prediction.cpp"
    
    # Test cognition systems
    run_test "Cognition Systems Test" "test_cognition.cpp"
}

# Function to run unit tests
run_unit_tests() {
    echo ""
    echo "=========================================="
    echo "Unit Tests"
    echo "=========================================="
    
    # Test core types
    run_test "Types Test" "test_types.cpp"
    
    # Test configuration system
    run_test "Config Test" "test_config.cpp"
    
    # Test random generator
    run_test "Random Test" "test_random.cpp"
    
    # Test neuron
    run_test "Neuron Test" "test_neuron.cpp"
    
    # Test synapse
    run_test "Synapse Test" "test_synapse.cpp"
    
    # Test STDP
    run_test "STDP Test" "test_stdp.cpp"
    
    # Test simulation clock
    run_test "Clock Test" "test_clock.cpp"
}

# Function to run performance tests
run_performance_tests() {
    echo ""
    echo "=========================================="
    echo "Performance Tests"
    echo "=========================================="
    
    # Test brain performance with different neuron counts
    echo -n "Performance test with 100 neurons... "
    if ./tests/test_performance 100 2>/dev/null; then
        echo -e "${GREEN}PASSED${NC}"
    else
        echo -e "${RED}FAILED${NC}"
    fi
    
    # Test memory usage
    echo -n "Memory usage test... "
    if ./tests/test_memory_usage 2>/dev/null; then
        echo -e "${GREEN}PASSED${NC}"
    else
        echo -e "${RED}FAILED${NC}"
    fi
}

# Function to run edge case tests
run_edge_case_tests() {
    echo ""
    echo "=========================================="
    echo "Edge Case Tests"
    echo "=========================================="
    
    # Test invalid inputs
    run_test "Invalid Input Handling Test" "test_edge_cases.cpp"
    
    # Test boundary conditions
    run_test "Boundary Conditions Test" "test_boundaries.cpp"
    
    # Test error recovery
    run_test "Error Recovery Test" "test_error_recovery.cpp"
}

# Main test execution
main() {
    echo "Starting NLM Test Suite..."
    echo "This will run all tests and report results."
    
    # Check if we have the required tools
    if ! command -v g++ &> /dev/null; then
        echo -e "${RED}Error: g++ compiler not found${NC}"
        exit 1
    fi
    
    if ! command -v cmake &> /dev/null; then
        echo -e "${RED}Error: CMake not found${NC}"
        exit 1
    fi
    
    # Run unit tests
    run_unit_tests
    
    # Run memory tests
    run_memory_tests
    
    # Run integration tests
    run_integration_tests
    
    # Run performance tests
    run_performance_tests
    
    # Run edge case tests
    run_edge_case_tests
    
    echo ""
    echo "=========================================="
    echo "Test Suite Complete"
    echo "=========================================="
    echo "All tests have been executed."
    echo "Check the output above for detailed results."
    echo ""
    echo "Summary:"
    echo "- Unit tests verify individual components"
    echo "- Memory tests ensure no memory leaks"
    echo "- Integration tests verify complete system functionality"
    echo "- Performance tests check system efficiency"
    echo "- Edge case tests verify robustness"
}

# Run main function with all arguments
main "$@"
