"""
Test configuration file I/O.
"""
import pynlm

# Test configuration save/load
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("simulation_timestep", 0.001)

config.saveToFile("test_config.json")
print("✓ Configuration saved")

# Load configuration
new_config = pynlm.createDefaultConfig()
if new_config.loadFromFile("test_config.json"):
    print("✓ Configuration loaded successfully")
    print(f"  Neuron count: {new_config.getOr('brain.neuron_count', 1000)}")
else:
    print("✗ Failed to load configuration")
