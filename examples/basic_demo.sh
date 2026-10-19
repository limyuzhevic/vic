#!/bin/bash
# examples/basic_demo.sh

# Basic NLM demo usage example

echo "NLM Basic Demo Example"
echo "======================="
echo ""
echo "This example demonstrates the basic usage of NLM's enhanced CLI interface."
echo ""

# Run demo mode
echo "1. Running demo mode (default):"
./nlm

echo ""
echo "2. Running demo mode with verbose output:"
./nlm --mode demo --verbose

echo ""
echo "3. Running demo mode with custom configuration:"
./nlm --mode demo --config examples/config.cfg

echo ""
echo "4. Running demo mode with validation:"
./nlm --mode demo --validate

echo ""
echo "Demo example completed successfully!"
