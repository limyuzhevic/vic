#!/usr/bin/env python3
import sys
sys.path.insert(0, '.')
import pynlm

# Test basic config functionality
config = pynlm.createDefaultConfig()

# Test setting and getting values
print("Testing config operations...")

# Test 1: Setting different types
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)
config.set("plasticity.enable", True)
config.set("world.width", 20.0)
config.set("debug.enabled", False)

# Test 2: Getting values
print(f"Neuron count: {config.get('brain.neuron_count', 500)}")
print(f"Learning rate: {config.get('learning.rate', 0.01)}")
print(f"Plasticity enabled: {config.get('plasticity.enable', False)}")
print(f"World width: {config.get('world.width', 10.0)}")
print(f"Debug enabled: {config.get('debug.enabled', True)}")

# Test 3: Checking if keys exist
print(f"\nHas brain.neuron_count: {config.has('brain.neuron_count')}")
print(f"Has non.existent.key: {config.has('non.existent.key')}")

# Test 4: Getting all keys
keys = config.getKeys()
print(f"\nAll keys: {keys}")

# Test 5: Summary
print(f"\nConfig summary:\n{config.summary()}")

# Test 6: JSON serialization (if implemented)
try:
    config.saveToFile("test_config.json")
    print("JSON save succeeded")
except Exception as e:
    print(f"JSON save failed: {e}")

print("\nConfig tests completed!")