#!/usr/bin/env python3
"""
Configuration Management Example
Demonstrates advanced configuration management using NLM's Python API.
"""

import pynlm
import json
import yaml
import tempfile
import os

def create_default_config():
    """Create a default configuration."""
    config = pynlm.createDefaultConfig()
    
    # Set various parameters
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.1)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    return config

def demonstrate_config_operations(config):
    """Demonstrate various configuration operations."""
    print("=== Configuration Operations ===")
    
    # Save configuration to JSON
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config.saveToFile(f.name)
        json_file = f.name
    
    print(f"Configuration saved to: {json_file}")
    
    # Load configuration from JSON
    new_config = pynlm.createDefaultConfig()
    new_config.loadFromFile(json_file)
    
    print(f"Loaded configuration:")
    print(f"  Neuron count: {new_config.get('brain.neuron_count')}")
    print(f"  Synapse density: {new_config.get('brain.synapse_density')}")
    print(f"  STDP enabled: {new_config.get('plasticity.stdp.enable')}")
    
    # Modify configuration
    new_config.set("brain.neuron_count", 1000)
    new_config.set("plasticity.stdp.learning_rate", 0.002)
    
    print(f"\nModified configuration:")
    print(f"  Neuron count: {new_config.get('brain.neuron_count')}")
    print(f"  STDP learning rate: {new_config.get('plasticity.stdp.learning_rate')}")
    
    # Save modified configuration to YAML
    with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
        # Convert to dict for YAML serialization
        config_dict = new_config.toDict()
        yaml.dump(config_dict, f)
        yaml_file = f.name
    
    print(f"\nConfiguration saved to YAML: {yaml_file}")
    
    # Load from YAML
    loaded_config = pynlm.createDefaultConfig()
    with open(yaml_file, 'r') as f:
        config_dict = yaml.safe_load(f)
    loaded_config.fromDict(config_dict)
    
    print(f"Loaded from YAML:")
    print(f"  Neuron count: {loaded_config.get('brain.neuron_count')}")
    
    # Clean up temporary files
    os.unlink(json_file)
    os.unlink(yaml_file)
    
    print("\n=== Configuration Operations Complete ===")

def demonstrate_config_comparison(config1, config2):
    """Compare two configurations."""
    print("\n=== Configuration Comparison ===")
    
    # Get all keys from both configurations
    keys1 = config1.getKeys()
    keys2 = config2.getKeys()
    
    all_keys = set(keys1) | set(keys2)
    
    print("Configuration differences:")
    for key in sorted(all_keys):
        value1 = config1.get(key, "NOT_SET")
        value2 = config2.get(key, "NOT_SET")
        
        if value1 != value2:
            print(f"  {key}:")
            print(f"    Config 1: {value1}")
            print(f"    Config 2: {value2}")
    
    print("\n=== Configuration Comparison Complete ===")

def main():
    print("=== Configuration Management Example ===")
    
    # Create initial configuration
    config = create_default_config()
    
    # Demonstrate basic operations
    demonstrate_config_operations(config)
    
    # Create modified configuration for comparison
    config2 = pynlm.createDefaultConfig()
    config2.set("brain.neuron_count", 750)
    config2.set("brain.synapse_density", 0.2)
    
    # Compare configurations
    demonstrate_config_comparison(config, config2)
    
    print("\n=== Example Complete ===")

if __name__ == "__main__":
    main()