#!/usr/bin/env python3
"""
Simple test to verify pynlm Python bindings
"""

import sys
import os

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

# Try to import the module
try:
    import pynlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

# Test basic functionality
print("\nTesting basic Python bindings...")

# Test 1: Create default config
config = pynlm.createDefaultConfig()
print("✓ Created default config")

# Test 2: Set and get values
config.set("test_value", 42)
value = config.getOr("test_value", 0)
print(f"✓ Set and get config value: {value}")

# Test 3: Create brain
brain = pynlm.createBrain(config)
print("✓ Created brain")

# Test 4: Create world
world = pynlm.createSimpleWorld(10, 10)
print("✓ Created simple world")

# Test 5: Create agent brain
agent_brain = pynlm.createAgentBrain(brain)
print("✓ Created agent brain")

# Test 6: Test enums
print(f"✓ LogLevel.Debug: {pynlm.LogLevel.Debug}")
print(f"✓ NeuronType.Excitatory: {pynlm.NeuronType.Excitatory}")
print(f"✓ ActionType.MoveForward: {pynlm.ActionType.MoveForward}")

print("\nAll basic tests passed!")
print("\nThe pynlm Python bindings are working correctly.")
print("\nYou can now use NLM in Python:")
print("  import pynlm")
print("  config = pynlm.createDefaultConfig()")
print("  brain = pynlm.createBrain(config)")
print("  world = pynlm.createSimpleWorld(20, 20)")
print("  # Run simulations, get statistics, etc.")
