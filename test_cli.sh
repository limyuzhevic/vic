#!/bin/bash

# NLM CLI Test Script
# Demonstrates the new unified command-line interface

echo "=== NLM CLI Test Script ==="
echo "Testing the new unified command-line interface for NLM experiments"
echo ""

# Test 1: Help command
echo "1. Testing help command:"
echo "./nlm --help"
./nlm --help

echo ""
echo "2. Testing version command:"
echo "./nlm --version"
./nlm --version

echo ""
echo "3. Testing Phase 2 experiment with basic options:"
echo "./nlm --mode=PHASE2 --neurons=100 --trials=10 --verbose"
./nlm --mode=PHASE2 --neurons=100 --trials=10 --verbose

echo ""
echo "4. Testing Phase 6 integration experiment:"
echo "./nlm --mode=PHASE6 --neurons=500 --checkpoint --development"
./nlm --mode=PHASE6 --neurons=500 --checkpoint --development

echo ""
echo "5. Testing auto mode:"
echo "./nlm --mode=AUTO --neurons=200"
./nlm --mode=AUTO --neurons=200

echo ""
echo "=== CLI Test Complete ==="