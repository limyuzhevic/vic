#!/bin/bash
# examples/explore_example.sh

# NLM explore example

echo "NLM Explore Example"
echo "=================="
echo ""
echo "This example demonstrates curiosity-driven exploration with different patterns."
echo ""

# Exploration with random pattern
echo "1. Random exploration (60 seconds):"
./nlm --mode explore --pattern random --duration 60

echo ""
echo "2. Sinusoidal exploration (60 seconds):"
./nlm --mode explore --pattern sinusoidal --duration 60

echo ""
echo "3. Timeout handling test (10 seconds):"
./nlm --mode explore --pattern random --duration 10

echo ""
echo "4. Batch exploration (300 seconds total):"
./nlm --mode explore --batch --duration 300 --pattern random

echo ""
echo "5. Exploration with verbose output:"
./nlm --mode explore --pattern random --duration 30 --verbose

echo ""
echo "Explore example completed successfully!"
