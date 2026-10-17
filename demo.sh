#!/usr/bin/env bash

# NLM New CLI and Test Framework - Demonstration Script
# This script demonstrates the new CLI system and test framework functionality

set -e

echo "=== NLM CLI and Test Framework Demonstration ==="
echo

# Check if NLM binary exists
if [ ! -f "./nlm" ]; then
    echo "Error: NLM binary not found. Please build the project first." >&2
    echo "Run: cmake -B build && cmake --build build" >&2
    exit 1
fi

echo "1. Testing CLI Help System"
echo "=========================="
./nlm --help

echo
echo "2. Testing CLI Version"
echo "==================="
./nlm --version

echo
echo "3. Testing CLI Test Listing"
echo "=========================="
./nlm list-tests

echo
echo "4. Testing CLI Test Help"
echo "========================="
./nlm help test

echo
echo "5. Testing CLI Test Execution (Basic Connectivity)"
echo "=================================================="
./nlm connectivity --neuron-count 50 --timestep 0.001

echo
echo "6. Testing CLI Test Execution with Configuration File"
echo "===================================================="
./nlm connectivity --config configs/test.cfg --output connectivity_results.txt
echo "   Results saved to connectivity_results.txt"
echo
echo "7. Testing Test Framework Programmatic API"
echo "=========================================="
echo "This would require building and running the examples directory"
echo "Check examples/cli_test.cpp for programmatic usage examples"
echo
echo "8. Testing Test Framework Suite Execution"
echo "======================================"
./nlm test --help
echo "Available test suites:"
./nlm list-tests
echo
echo "To run specific test suites:"
echo "  ./nlm neural-computation"
echo "  ./nlm brain-functionality"
echo
echo "Example with custom configuration:"
echo "  ./nlm neural-computation --neuron-count 200 --timestep 0.0005 --output suite_results.json"
echo
echo "9. Testing Experiment Mode"
echo "=========================="
./nlm experiment --help
echo
echo "To run an experiment:"
echo "  ./nlm experiment grid-world --neuron-count 100"
echo
echo "10. Testing Benchmark Mode"
echo "========================="
./nlm benchmark --help

echo
echo "=== Demonstration Complete ==="
echo
echo "Key Improvements:"
echo "✓ Comprehensive CLI with argument parsing and help system"
echo "✓ Test framework with isolation and reporting"
echo "✓ Programmatic API for library usage"
echo "✓ Configuration validation and type safety"
echo "✓ Multiple output formats (JSON, CSV, detailed, compact)"
echo "✓ Extensible architecture for new tests and experiments"
echo "✓ Better error handling and user feedback"
echo
echo "Next Steps:"
echo "1. Build the project: cmake -B build && cmake --build build"
echo "2. Run tests: ./build/nlm test connectivity"
echo "3. Explore examples in examples/ directory"
echo "4. Customize configuration in configs/ directory"