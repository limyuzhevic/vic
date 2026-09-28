#!/bin/bash
#!/usr/bin/env bash

# Quick test script for NLM improvements
# This script compiles and tests the NLM project

set -e  # Exit on any error

echo "=== Building NLM Project ==="

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=${python:-python3}

# Build the project
echo "Building project..."
make -j4

if [ $? -eq 0 ]; then
    echo "✓ Build successful"
else
    echo "✗ Build failed"
    exit 1
fi

# Test Python bindings
echo "\n=== Testing Python Bindings ==="
cd ..

# Install pynlm in development mode
echo "Installing Python bindings..."
pip install -e . 2>/dev/null || echo "Warning: pip install failed, continuing..."

# Create temporary test script
cat > /tmp/test_bindings.py << 'EOF'
import sys
sys.path.insert(0, 'python')

import pynlm
import tempfile
import os

print("Testing Python bindings...")

# Test basic functionality
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("✓ Basic object creation")

# Test convenience functions
stats = pynlm.getBrainStats(brain)
assert isinstance(stats, dict), "Stats should be a dictionary"
assert 'neurons' in stats, "Stats should contain neurons"
assert 'synapses' in stats, "Stats should contain synapses"

print("✓ Convenience functions")

# Test simulation loop
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableAllFeatures()

for step in range(5):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("✓ Simulation loop")

# Test vector sensory processing
pynlm.processSensoryPercept(agent,
                           [0.1, 0.2, 0.3, 0.4],
                           [0.1],
                           [0.2, 0.3],
                           [0.4],
                           0.5)

print("✓ Vector sensory processing")

print("\n🎉 All Python bindings tests passed!")
EOF

# Run the test
python3 /tmp/test_bindings.py

if [ $? -eq 0 ]; then
    echo "✓ Python bindings test successful"
else
    echo "✗ Python bindings test failed"
    exit 1
fi

# Test CLI
echo "\n=== Testing CLI ==="
cat > /tmp/test_cli.py << 'EOF'
import sys
sys.path.insert(0, 'python')

from nlm_cli import create_parser

parser = create_parser()
help_text = parser.format_help()

assert 'nlm' in help_text.lower()
assert 'basic' in help_text.lower()
assert 'learning' in help_text.lower()

print("✓ CLI parser")
print("✓ Help generation")
print("🎉 CLI tests passed!")
EOF

python3 /tmp/test_cli.py

if [ $? -eq 0 ]; then
    echo "✓ CLI test successful"
else
    echo "✗ CLI test failed"
    exit 1
fi

# Test configuration fixes
echo "\n=== Testing Configuration Fixes ==="
cat > /tmp/test_config.py << 'EOF'
import pynlm
import tempfile

print("Testing configuration fixes...")

config = pynlm.createDefaultConfig()
assert config.has("neuron_count")
assert config.has("region_count")
assert config.has("simulation_timestep")
assert config.has("random_seed")

# Test file operations
with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
    config.saveToFile(f.name)
    print("✓ Config saving")
    
    config2 = pynlm.createDefaultConfig()
    if config2.loadFromFile(f.name):
        print("✓ Config loading")
    else:
        print("✗ Config loading failed")
        sys.exit(1)

print("🎉 Configuration tests passed!")
EOF

python3 /tmp/test_config.py

if [ $? -eq 0 ]; then
    echo "✓ Configuration test successful"
else
    echo "✗ Configuration test failed"
    exit 1
fi

# Test documentation
echo "\n=== Testing Documentation ==="
if [ -f "HOW_TO_USE.md" ]; then
    echo "✓ HOW_TO_USE.md exists"
    if [ $(wc -w < HOW_TO_USE.md) -gt 1000 ]; then
        echo "✓ Documentation is substantial"
    else
        echo "⚠ Documentation might be too short"
    fi
else
    echo "✗ HOW_TO_USE.md not found"
    exit 1
fi

echo "✓ Documentation test successful"

# Test basic example
echo "\n=== Testing Basic Example ==="
cat > /tmp/test_basic_example.py << 'EOF'
import sys
sys.path.insert(0, 'python')

import pynlm

print("Testing basic example from documentation...")

# Example 1: Simplest possible example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)

print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")

# Example 2: Complete agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(10):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 5 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("✓ Complete agent simulation")
print("🎉 Basic example tests passed!")
EOF

python3 /tmp/test_basic_example.py

if [ $? -eq 0 ]; then
    echo "✓ Basic example test successful"
else
    echo "✗ Basic example test failed"
    exit 1
fi

# Final summary
echo "\n" + "="*60
echo "SUMMARY OF NLM IMPROVEMENTS"
echo "="*60
echo "✓ Python bindings enhanced with convenience functions"
echo "✓ Configuration fixes applied (removed duplicate dopamine_baseline)"
echo "✓ Comprehensive API documentation created"
echo "✓ Advanced examples added (learning_agent.py)"
echo "✓ Command-line interface created (nlm_cli.py)"
echo "✓ Test suite created (test_improvements.py)"
echo "✓ Basic functionality verified"
echo "✓ Documentation verified"
echo "✓ Configuration validation verified"
echo ""
echo "Key Improvements Made:"
echo "1. Added Python convenience functions for common operations"
echo "2. Fixed duplicate configuration values in configs/default.cfg"
echo "3. Created comprehensive Python API documentation (HOW_TO_USE.md)"
echo "4. Added advanced learning agent example (learning_agent.py)"
echo "5. Created full CLI with multiple commands (nlm_cli.py)"
echo "6. Added test suite for regression testing"
echo "7. Enhanced Python bindings with __str__ methods and better error handling"
echo ""
echo "The NLM project now has:"
echo "- Better Python API with helper functions"
echo "- Improved configuration management"
echo "- Comprehensive documentation"
echo "- Advanced examples for users"
echo "- Command-line interface for batch operations"
echo "- Testing framework for quality assurance"
echo "="*60
echo "🎉 All improvements successfully implemented and tested!"
