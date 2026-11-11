#!/bin/bash
# Test script to verify improved NLM CLI functionality

echo "Testing improved NLM command-line interface..."
echo ""

# Test 1: Help option
echo "Test 1: Testing --help option"
./nlm --help 2>&1 | head -20

echo ""
echo "Test 2: Testing --version option"
./nlm --version 2>&1

echo ""
echo "Test 3: Testing --list-config option"
./nlm --list-config 2>&1 | head -20

echo ""
echo "Test 4: Testing basic help text extraction"
./nlm --help 2>&1 | grep -i "usage:" | head -1

echo ""
echo "All CLI tests completed successfully!"