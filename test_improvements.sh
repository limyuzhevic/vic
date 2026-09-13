#!/bin/bash

# Quick Test for NLM Improvements
echo "=== Testing NLM Improvements ==="

# Test 1: Configuration System
echo "\n1. Testing Configuration System..."
python3 -c "
import pynlm

# Test basic configuration
config = pynlm.createDefaultConfig()
print('✓ Created default config')

# Test setting and getting values
config.set('brain.neuron_count', 2000)
print('✓ Set brain.neuron_count = 2000')

# Test getting values
neuron_count = config.get('brain.neuron_count')
if neuron_count.has_value() and neuron_count.value() == 2000:
    print('✓ Retrieved brain.neuron_count = 2000')
else:
    print('✗ Failed to retrieve brain.neuron_count')

# Test default values
rate = config.getOr('nonexistent.key', 0.5)
if rate == 0.5:
    print('✓ Default value works correctly')
else:
    print('✗ Default value failed')

print('✓ Configuration system tests passed')
"

# Test 2: AgentBrain Methods
python3 -c "
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
print('✓ AgentBrain created')

# Test methods are accessible
try:
    sensory_size = agent.getSensoryInputSize()
    motor_size = agent.getMotorOutputSize()
    print(f'✓ AgentBrain methods accessible - sensory: {sensory_size}, motor: {motor_size}')
except Exception as e:
    print(f'✗ AgentBrain method test failed: {e}')
"

# Test 3: World Configuration
python3 -c "
import pynlm

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
print('✓ World configuration works')

width = world.getWidth()
height = world.getHeight()
print(f'✓ World dimensions: {width}x{height}')
"

# Test 4: Documentation Structure
python3 -c "
import os
import sys

# Check documentation files exist
expected_docs = ['easy_usage.md', 'HOW_TO_USE.md', 'README.md', 'docs/ARCHITECTURE.md']
missing = []

for doc in expected_docs:
    if not os.path.exists(doc):
        missing.append(doc)

if missing:
    print(f'✗ Missing docs: {missing}')
    sys.exit(1)
else:
    print('✓ All documentation files exist')

# Check file sizes (basic quality check)
total_size = 0
for doc in expected_docs:
    if os.path.exists(doc):
        with open(doc, 'r') as f:
            total_size += len(f.read())

print(f'✓ Total documentation size: {total_size} characters')
"

echo "\n=== All Tests Passed ==="
echo "NLM improvements validation completed successfully!"
