#!/bin/bash
# NLM Example Runner Script

# This script provides easy access to NLM example demonstrations
# Run with: ./run_examples.sh

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "$SCRIPT_DIR"

echo "NLM Example Scripts Runner"
echo "=========================="

echo ""
echo "Available examples:"
echo "1. Beginner Example - Simple neural simulation"
echo "2. Intermediate Example - Agent with world interaction"
echo "3. Advanced Example - Custom configuration experiments"
echo "4. Performance Example - Benchmarking and profiling"
echo ""

echo "Would you like to run all examples? (y/n):"
read -r response

if [[ "$response" =~ ^[Yy]$ ]]; then
    echo ""
    echo "Running Beginner Example..."
    echo "==============================="
    python3 nlm_examples.py 2>&1 | head -50
    
    echo ""
    echo "Press Enter to continue to Intermediate Example..."
    read -r _
    
    echo ""
    echo "Running Intermediate Example..."
    echo "============================= "
    # Note: The Python script runs all examples sequentially
    # For interactive running, we'll modify the script
    echo "To run examples interactively, run: python3 nlm_examples.py"
    echo "This will prompt you through each example step by step."
else
    echo ""
    echo "To run the examples interactively, use:"
    echo "  python3 nlm_examples.py"
    echo ""
    echo "This will guide you through each example level by level."
fi