#!/usr/bin/env python3
"""
Example 2: Configuration Management with Error Handling
Demonstrates robust configuration loading, validation, and error handling
"""
import pynlm
import os
import json

def safe_config_operations():
    """Demonstrate safe configuration operations with error handling"""
    print("=== Example 2: Safe Configuration Operations ===\n")
    
    # 1. Create default config and modify it safely
    print("1. Creating and modifying configuration safely:")
    config = pynlm.createDefaultConfig()
    
    # Safe way to set configuration values with error handling
    def safe_set(config, key, value):
        try:
            if key.endswith(".neuron_count"):
                config.set(key, int(value))
                print(f"   ✓ Set {key} = {value} (as int)")
            elif key.endswith(".scale") or key.endswith(".rate") or key.endswith(".value"):
                config.set(key, float(value))
                print(f"   ✓ Set {key} = {value} (as float)")
            elif key.endswith(".enable") or key.endswith(".bool"):
                config.set(key, bool(value))
                print(f"   ✓ Set {key} = {value} (as bool)")
            elif key.endswith(".name") or key.endswith(".path"):
                config.set(key, str(value))
                print(f"   ✓ Set {key} = '{value}' (as string)")
            else:
                config.set(key, value)
                print(f"   ✓ Set {key} = {value}")
        except Exception as e:
            print(f"   ✗ Error setting {key}: {e}")
    
    # Set various configuration values
    safe_set(config, "brain.neuron_count", 2000)
    safe_set(config, "brain.synapse_density", 0.15)
    safe_set(config, "plasticity.stdp.enable", True)
    safe_set(config, "neuromod.dopamine.scale", 2.0)
    safe_set(config, "neuromod.curiosity.enable", True)
    safe_set(config, "development.maturation_rate", 0.01)
    
    print()
    
    # 2. Demonstrate safe configuration retrieval
    print("2. Safely retrieving configuration values:")
    config_safety_checks = [
        ("brain.neuron_count", 1000),
        ("brain.synapse_density", 0.1),
        ("plasticity.stdp.enable", True),
        ("neuromod.dopamine.scale", 1.0),
        ("nonexistent.param", "should_use_default")
    ]
    
    for key, expected_default in config_safety_checks:
        try:
            if isinstance(expected_default, int):
                value = config.getInt(key, expected_default)
            elif isinstance(expected_default, float):
                value = config.getFloat(key, expected_default)
            elif isinstance(expected_default, bool):
                value = config.getBool(key, expected_default)
            else:
                value = config.getString(key, expected_default)
            
            exists = " (exists)" if config.has(key) else " (new)"
            print(f"   ✓ {key}: {value}{exists}")
            
        except Exception as e:
            print(f"   ✗ Error getting {key}: {e}")
    
    print()
    
    # 3. Save and load configuration
    print("3. Configuration persistence:")
    
    # Save to file with error handling
    try:
        save_path = "demo_config.json"
        if config.saveToFile(save_path):
            print(f"   ✓ Configuration saved to {save_path}")
        else:
            print(f"   ✗ Failed to save configuration to {save_path}")
    except Exception as e:
        print(f"   ✗ Error saving config: {e}")
    
    # Create a new config and load from file
    try:
        new_config = pynlm.createDefaultConfig()
        load_path = "demo_config.json"
        if os.path.exists(load_path):
            if new_config.loadFromFile(load_path):
                print(f"   ✓ Configuration loaded from {load_path}")
                print(f"   Loaded neuron count: {new_config.getInt('brain.neuron_count', 'N/A')}")
            else:
                print(f"   ✗ Failed to load configuration from {load_path}")
    except Exception as e:
        print(f"   ✗ Error loading config: {e}")
    
    # Clean up
    try:
        if os.path.exists("demo_config.json"):
            os.remove("demo_config.json")
            print(f"   ✓ Cleaned up demo_config.json")
    except Exception:
        pass
    
    print("\n=== Safe configuration operations example complete ===\n")

if __name__ == "__main__":
    safe_config_operations()