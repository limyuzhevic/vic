#!/bin/bash

# Continuous Integration validation script

echo "=== NLM Brain Simulation CI Validation ==="

# Build environment
./build_environment.sh

# Run quick test subset
echo "Running quick validation tests..."
ctest -R "brain_constructor|agent_brain_null_safety" --output-on-failure

# Memory leak detection (optional)
echo "Running memory leak detection..."
valgrind --leak-check=full --track-origins=yes ./test_brain_constructor 2>&1 | tail -20

# Performance sanity check
echo "Running performance sanity check..."
./benchmark_brain_step --quick

echo ""
echo "=== CI Validation Complete ==="
