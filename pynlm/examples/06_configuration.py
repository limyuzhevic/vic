#!/usr/bin/env python3
"""
Example 6: Configuration Management

This example demonstrates different ways to work with configuration in NLM,
including file operations and programmatic configuration.
"""

import pynlm
import tempfile
import os

def create_config_example():
    """Create configuration management example."""
    print("=== Configuration Management Example ===")
    
    # Method 1: Default configuration
    print("1. Using default configuration...")
    config1 = pynlm.createDefaultConfig()
    config1.set("brain.neuron_count", 100)
    config1.set("brain.region_count", 2)
    print(f"   Default config keys: {config1.keys()}")
    print(f"   Neuron count: {config1.getInt('brain.neuron_count')}")
    print(f"   Region count: {config1.getInt('brain.region_count')}")
    
    # Method 2: Programmatic configuration with type safety
    print("2. Programmatic configuration with type safety...")
    config2 = pynlm.createDefaultConfig()
    config2.set("brain.neuron_count", 500)  # int
    config2.set("simulation.learning_rate", 0.01)  # double
    config2.set("agent.enable_curiosity", True)  # bool
    config2.set("brain.name", "MyFirstBrain")  # string
    config2.set("sensor.channels", [1, 2, 3, 4])  # int vector
    config2.set("weights", [0.1, 0.2, 0.3, 0.4, 0.5])  # double vector
    config2.set("tags", ["tag1", "tag2", "tag3"])  # string vector
    
    print(f"   Configuration keys: {config2.keys()}")
    print(f"   Neuron count: {config2.getInt('brain.neuron_count')}")
    print(f"   Learning rate: {config2.getDouble('simulation.learning_rate')}")
    print(f"   Curiosity enabled: {config2.getBool('agent.enable_curiosity')}")
    print(f"   Brain name: {config2.getString('brain.name')}")
    print(f"   Sensor channels: {config2.getIntVector('sensor.channels')}")
    print(f"   Weights: {config2.getDoubleVector('weights')}")
    print(f"   Tags: {config2.getStringVector('tags')}")
    
    # Method 3: File operations
    print("3. File operations...")
    config3 = pynlm.createDefaultConfig()
    config3.set("brain.neuron_count", 1000)
    config3.set("brain.region_count", 4)
    config3.set("brain.connection_probability", 0.15)
    
    # Save to temporary file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        temp_file = f.name
        print(f"   Saving to: {temp_file}")
        
        success = config3.saveToFile(temp_file)
        if success:
            print("   ✓ Configuration saved successfully")
            
            # Load from file
            config4 = pynlm.createDefaultConfig()
            success = config4.loadFromFile(temp_file)
            if success:
                print("   ✓ Configuration loaded successfully")
                print(f"   Loaded neuron count: {config4.getInt('brain.neuron_count')}")
                print(f"   Loaded region count: {config4.getInt('brain.region_count')}")
            else:
                print("   ✗ Failed to load configuration")
        else:
            print("   ✗ Failed to save configuration")
        
        # Clean up
        if os.path.exists(temp_file):
            os.unlink(temp_file)
            print(f"   Cleaned up temp file: {temp_file}")
    
    # Method 4: Pythonic dictionary-style access
    print("4. Pythonic dictionary-style access...")
    config5 = pynlm.createDefaultConfig()
    config5.set("brain.neuron_count", 100)
    config5.set("simulation.learning_rate", 0.01)
    config5.set("agent.enable_curiosity", True)
    
    # Using property-style access
    print(f"   Config['brain.neuron_count']: {config5['brain.neuron_count']}")
    print(f"   Config['simulation.learning_rate']: {config5['simulation.learning_rate']}")
    print(f"   Config['agent.enable_curiosity']: {config5['agent.enable_curiosity']}")
    
    # Check key existence
    print(f"   Has 'brain.neuron_count': {'brain.neuron_count' in config5}")
    print(f"   Has 'nonexistent': {'nonexistent' in config5}")
    
    # Method 5: Configuration with default values
    print("5. Configuration with default values...")
    config6 = pynlm.createDefaultConfig()
    config6.set("brain.neuron_count", 500)
    
    # Use getOr for safe access with defaults
    neurons = config6.getInt('brain.neuron_count', 100)  # 100 is default
    regions = config6.getInt('brain.region_count', 2)   # 2 is default
    unknown = config6.getInt('brain.unknown_setting', 999)  # 999 is default
    
    print(f"   Neurons (explicit): {neurons}")
    print(f"   Regions (default): {regions}")
    print(f"   Unknown (default): {unknown}")
    
    # Method 6: Configuration summary
    print("6. Configuration summary...")
    config7 = pynlm.createDefaultConfig()
    config7.set("brain.neuron_count", 750)
    config7.set("brain.region_count", 3)
    config7.set("simulation.learning_rate", 0.005)
    
    summary = config7.summary()
    print(f"   Configuration summary:\n{summary}")
    
    return [config1, config2, config3, config4, config5, config6, config7]

if __name__ == "__main__":
    configs = create_config_example()
    print(f"\n✓ Configuration management example completed successfully!")
    print(f"   Created {len(configs)} different configurations!")