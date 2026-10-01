#!/usr/bin/env bash
# NLM CLI Test Script

# This script demonstrates the new NLM CLI functionality
# and can be used to verify the implementation

set -e

echo "=== NLM Command Line Interface Test ==="
echo ""

# Test 1: Help command
echo "Test 1: Displaying help"
./nlm_cli --help

# Test 2: Version command
echo ""
echo "Test 2: Displaying version"
./nlm_cli --version

# Test 3: Help for specific command
echo ""
echo "Test 3: Help for 'save' command"
./nlm_cli save --help

# Test 4: Test argument parsing (dry run)
echo ""
echo "Test 4: Testing argument parsing"
./nlm_cli --set neuron_count=500 --set region_count=2 --steps 100 --verbose 2>&1 | head -20

echo ""
echo "=== Test Complete ==="
