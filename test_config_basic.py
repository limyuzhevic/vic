# Test Configuration Validation

# Test basic configuration functionality
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()
print(f"Created config with {len(config.getKeys())} default keys")

# Test getting values
neuron_count = config.getOr("brain.neuron_count", 1000)
print(f"Default neuron count: {neuron_count}")

# Test setting values
config.set("brain.neuron_count", 2000)
neuron_count = config.getOr("brain.neuron_count", 1000)
print(f"After setting: {neuron_count}")

# Test configuration summary
summary = config.summary()
print(f"Config summary (first 200 chars): {summary[:200]}...")

print("✓ Basic configuration test passed!")
