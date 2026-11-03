"""Configuration management examples for NLM"""

import pynlm
import json

print("=== NLM Configuration Management Examples ===")

# Example 1: Basic configuration
print("\n1. Basic Configuration:")
config = pynlm.createDefaultConfig()
print(f"   Default config has {len(config.getKeys())} entries")

# Modify configuration
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.05)
config.set("plasticity.stdp.learning_rate", 0.001)

print(f"   Modified config has {len(config.getKeys())} entries")
print(f"   Neuron count: {config.getOr('brain.neuron_count', 0)}")

# Example 2: Save and load configurations
print("\n2. Configuration Serialization:")
config2 = pynlm.createDefaultConfig()
config2.set("brain.neuron_count", 2000)
config2.set("brain.synapse_density", 0.1)
config2.set("neuromod.curiosity.scale", 1.5)
config2.set("development.enable_critical_period", true)

# Save to JSON file
config_file = "/tmp/nlm_config.json"
config2.saveToFile(config_file)
print(f"   Saved config to {config_file}")

# Load configuration
config3 = pynlm.createDefaultConfig()
config3.loadFromFile(config_file)
print(f"   Loaded config has {len(config3.getKeys())} entries")

# Compare configurations
print("\n3. Configuration Comparison:")
keys1 = config2.getKeys()
keys2 = config3.getKeys()

common_keys = set(keys1) & set(keys2)
diff_keys = set(keys1) ^ set(keys2)

print(f"   Common keys: {len(common_keys)}")
print(f"   Different keys: {len(diff_keys)}")
if diff_keys:
    print(f"   Different keys: {', '.join(sorted(diff_keys))}")

# Example 4: Configuration templates and functions
print("\n4. Configuration Templates:")

# Template functions
def create_default_brain_config(neuron_count=1000):
    """Create a basic brain configuration"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.1)
    config.set("brain.connection_probability", 0.05)
    config.set("plasticity.stdp.learning_rate", 0.001)
    return config

def create_learning_config(neuron_count=1000):
    """Create a configuration optimized for learning"""
    config = create_default_brain_config(neuron_count)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    return config

def create_development_config(neuron_count=1000):
    """Create a configuration for developmental learning"""
    config = create_learning_config(neuron_count)
    config.set("development.enable", True)
    config.set("development.critical_period_length", 1000)
    config.set("development.enable_maturation", True)
    return config

def create_social_config(neuron_count=1000):
    """Create a configuration for social learning scenarios"""
    config = create_development_config(neuron_count)
    config.set("prediction.enable", True)
    config.set("cognition.enable_planning", True)
    config.set("cognition.enable_concept_formation", True)
    return config

# Use templates
research_config = create_default_brain_config(500)
learning_config = create_learning_config(1000)
dev_config = create_development_config(750)
social_config = create_social_config(1500)

print(f"   Default config: {research_config.getOr('brain.neuron_count', 0)} neurons")
print(f"   Learning config: {learning_config.getOr('brain.neuron_count', 0)} neurons")
print(f"   Development config: {dev_config.getOr('brain.neuron_count', 0)} neurons")
print(f"   Social config: {social_config.getOr('brain.neuron_count', 0)} neurons")

# Example 5: Configuration validation and checking
print("\n5. Configuration Validation:")

valid_config = pynlm.createDefaultConfig()
valid_config.set("brain.neuron_count", 100)
valid_config.set("brain.connection_probability", 0.1)

# Check for required keys
required_keys = ["brain.neuron_count", "brain.connection_probability", "plasticity.stdp.enable"]
for key in required_keys:
    if valid_config.has(key):
        print(f"   ✓ Has key: {key}")
    else:
        print(f"   ✗ Missing key: {key}")

# Example 6: Configuration comparison and merging
print("\n6. Configuration Comparison:")
config_a = pynlm.createDefaultConfig()
config_a.set("brain.neuron_count", 1000)
config_a.set("brain.connection_probability", 0.05)

config_b = pynlm.createDefaultConfig()
config_b.set("brain.neuron_count", 2000)
config_b.set("plasticity.stdp.learning_rate", 0.01)

# Find differences
all_keys = set(config_a.getKeys()) | set(config_b.getKeys())
differences = []

for key in all_keys:
    val_a = config_a.getOr(key, "NOT_SET")
    val_b = config_b.getOr(key, "NOT_SET")
    
    if val_a != val_b:
        differences.append((key, val_a, val_b))

print(f"   Config A has {len(config_a.getKeys())} entries")
print(f"   Config B has {len(config_b.getKeys())} entries")
print(f"   Found {len(differences)} differences")
for key, val_a, val_b in differences[:3]:  # Show first 3 differences
    print(f"     {key}: A='{val_a}' vs B='{val_b}'")

# Example 7: Advanced configuration
print("\n7. Advanced Configuration:")
advanced_config = pynlm.createDefaultConfig()

# Set complex values
advanced_config.set("brain.neuron_count", 5000)
advanced_config.set("brain.synapse_density", 0.2)
advanced_config.set("brain.initial_weight_mean", 0.5)
advanced_config.set("brain.initial_weight_std", 0.1)

# Set list values
advanced_config.set("plasticity.types", [1.0, 0.8, 0.6, 0.4, 0.2])

# Set boolean values
advanced_config.set("development.enable_structural_plasticity", True)
advanced_config.set("development.enable_pruning", True)

print(f"   Advanced config has {len(advanced_config.getKeys())} entries")
print(f"   Neuron count: {advanced_config.getOr('brain.neuron_count', 0)}")
print(f"   Has list config: {'plasticity.types' in advanced_config.getKeys()}")

print("\n=== Configuration examples completed! ===")
