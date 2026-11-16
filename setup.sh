#!/bin/bash
# Set executable permissions for NLM scripts

chmod +x nlm-cli.sh build.sh build.py

# Create a simple test to verify the fixes
echo "Setting up NLM improvements..."

# Test 1: Verify NeuralWorkingMemory fix
if grep -q "Always add index to activeTraces_ if neuron is stored" src/memory/NeuralWorkingMemory.cpp; then
    echo "✓ NeuralWorkingMemory fix applied"
else
    echo "✗ NeuralWorkingMemory fix not found"
fi

# Test 2: Verify config file fix
if grep -q "dopamine_baseline = 0.1" configs/default.cfg | grep -v "dopamine_baseline = 0.0"; then
    echo "✓ Config file fix applied (removed duplicate)"
else
    echo "✗ Config file fix not found"
fi

# Test 3: Verify CLI script exists
if [[ -f "nlm-cli.sh" ]]; then
    echo "✓ CLI script created"
else
    echo "✗ CLI script missing"
fi

# Test 4: Verify build scripts exist
if [[ -f "build.sh" && -f "build.py" ]]; then
    echo "✓ Build scripts created"
else
    echo "✗ Build scripts missing"
fi

echo "Setup completed!"
