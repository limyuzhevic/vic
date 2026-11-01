#!/bin/bash

# Test script for NLM improved command-line interface

set -e

echo "Testing NLM improved command-line interface..."

echo "\n1. Testing help option:"
./nlm --help 2>&1 | head -20

echo "\n2. Testing version option:"
./nlm --version 2>&1

echo "\n3. Testing basic configuration:"
./nlm --neurons 100 --steps 100 --verbose 2>&1 | head -30

echo "\n4. Testing plasticity mode:"
./nlm --neurons 100 --steps 100 --run-mode plasticity --enable-plasticity --enable-stdp 2>&1 | head -40

echo "\n5. Testing all features together:"
./nlm --neurons 50 --steps 50 --run-mode full --enable-plasticity --enable-stdp --enable-hebbian --verbose 2>&1 | head -50

echo "\nTest completed successfully!"