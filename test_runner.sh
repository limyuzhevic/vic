#!/usr/bin/env bash
# NLM Testing and Validation Script
# Run tests to ensure everything works correctly

echo "=== NLM Testing Suite ==="
echo ""

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: Not in NLM root directory (should contain CMakeLists.txt)"
    echo "Please run this script from the NLM root directory"
    exit 1
fi

# Function to run a test with logging
run_test() {
    local test_name=$1
    local test_path=$2
    echo "=== Running $test_name ==="
    if [ -f "$test_path" ]; then
        g++ -std=c++17 -I./src -I./tests -I./src/core -I./src/brain -I./src/plasticity -I./src/sensory -I./src/motor -I./src/neuromodulation -I./src/memory -I./src/prediction -I./src/development -I./src/experiments -I./src/visualization -I./src/world -I./src/agent -D_TEST_MODE "$test_path" -o test_binary
        if [ $? -eq 0 ]; then
            ./test_binary
            echo "=== $test_name completed successfully ==="
        else
            echo "=== $test_name failed ==="
            echo "Check compiler errors above"
        fi
    else
        echo "Error: Test file not found: $test_path"
    fi
    echo ""
}

# Run specific tests
run_test "Config Tests" "./tests/test_config.cpp"
run_test "Brain Tests" "./tests/test_brain.cpp"
run_test "Neuron Tests" "./tests/test_neuron.cpp"
run_test "Synapse Tests" "./tests/test_synapse.cpp"
run_test "STDP Tests" "./tests/test_stdp.cpp"
run_test "Random Tests" "./tests/test_random.cpp"

echo "=== All tests completed ==="
echo ""
echo "=== Summary ==="
echo "If any tests failed, check the error messages above for details."
echo "Common issues:")
echo "- Missing dependencies or include paths"
echo "- Compiler version (requires C++17)"
echo "- Build directory issues"
echo ""
echo "=== Build Instructions ==="
echo "For a full build, run:"
echo "  mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4"
echo ""
echo "=== Testing Notes ==="
echo "This script provides basic compilation tests. For comprehensive testing,"
echo "refer to the official test suite or documentation."
echo ""
echo "Happy testing!"
