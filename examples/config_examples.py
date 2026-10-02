#!/usr/bin/env python3
"""
Example script showing how to use NLM configuration files.

This script demonstrates loading configurations from different file formats
(JSON, YAML, and INI) and using them to create NLM brains.
"""

import os
import json
import yaml
import tempfile
import pynlm

def load_json_config(filepath):
    """Load configuration from JSON file."""
    print(f"Loading JSON configuration from {filepath}")
    
    with open(filepath, 'r') as f:
        config_data = json.load(f)
    
    # Create config object from JSON data
    config = pynlm.create_default_config()
    
    # Apply configuration values
    if 'brain' in config_data:
        brain_config = config_data['brain']
        for key, value in brain_config.items():
            config.set(f"brain.{key}", value)
    
    if 'environment' in config_data:
        env_config = config_data['environment']
        for key, value in env_config.items():
            config.set(f"environment.{key}", value)
    
    return config

def load_yaml_config(filepath):
    """Load configuration from YAML file."""
    print(f"Loading YAML configuration from {filepath}")
    
    with open(filepath, 'r') as f:
        config_data = yaml.safe_load(f)
    
    # Create config object from YAML data
    config = pynlm.create_default_config()
    
    # Apply configuration values
    if 'brain' in config_data:
        brain_config = config_data['brain']
        for key, value in brain_config.items():
            config.set(f"brain.{key}", value)
    
    if 'environment' in config_data:
        env_config = config_data['environment']
        for key, value in env_config.items():
            config.set(f"environment.{key}", value)
    
    return config

def load_ini_config(filepath):
    """Load configuration from INI file."""
    print(f"Loading INI configuration from {filepath}")
    
    config = pynlm.create_default_config()
    
    # Read INI file manually
    current_section = ""
    with open(filepath, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            
            # Check for section header
            if line.startswith('[') and line.endswith(']'):
                current_section = line[1:-1].lower()
                continue
            
            # Parse key-value pair
            if '=' in line:
                key, value = line.split('=', 1)
                key = key.strip()
                value = value.strip()
                
                # Convert to appropriate type
                if value.lower() in ('true', 'false'):
                    value = value.lower() == 'true'
                elif value.isdigit():
                    value = int(value)
                elif '.' in value and value.replace('.', '').isdigit():
                    value = float(value)
                
                # Set config value
                if current_section:
                    config.set(f"{current_section}.{key}", value)
                else:
                    config.set(key, value)
    
    return config

def example_json_config():
    """Example using JSON configuration."""
    print("\n" + "="*60)
    print("Example 1: JSON Configuration")
    print("="*60)
    
    # Create temporary JSON file
    json_config = {
        "brain": {
            "neuron_count": 1000,
            "connection_probability": 0.1
        },
        "environment": {
            "name": "GridWorld",
            "width": 15,
            "height": 15
        }
    }
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        json.dump(json_config, f, indent=2)
        temp_json_path = f.name
    
    try:
        # Load JSON configuration
        config = load_json_config(temp_json_path)
        
        # Create brain with JSON config
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
        print(f"✓ Configuration loaded successfully from JSON")
        
        return brain
    finally:
        os.unlink(temp_json_path)

def example_yaml_config():
    """Example using YAML configuration."""
    print("\n" + "="*60)
    print("Example 2: YAML Configuration")
    print("="*60)
    
    # Create temporary YAML file
    yaml_config = """
brain:
  neuron_count: 2000
  region_count: 3
  connection_probability: 0.05

environment:
  name: GridWorld
  width: 25
  height: 25
  vision_width: 10
  vision_height: 10

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
  hebbian:
    enable: true
    learning_rate: 0.0005

logging:
  level: INFO
  to_file: false
"""
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
        f.write(yaml_config)
        temp_yaml_path = f.name
    
    try:
        # Load YAML configuration
        config = load_yaml_config(temp_yaml_path)
        
        # Create brain with YAML config
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
        print(f"✓ Configuration loaded successfully from YAML")
        
        return brain
    finally:
        os.unlink(temp_yaml_path)

def example_ini_config():
    """Example using INI configuration."""
    print("\n" + "="*60)
    print("Example 3: INI Configuration")
    print("="*60)
    
    # Create temporary INI file
    ini_config = """
[brain]
neuron_count = 500
region_count = 2
connection_probability = 0.08
initial_weight_mean = 0.5

[environment]
name = GridWorld
width = 20
height = 20
vision_width = 8
vision_height = 8

[plasticity.stdp]
enable = true
learning_rate = 0.002

[logging]
level = WARNING
to_file = false
"""
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        f.write(ini_config)
        temp_ini_path = f.name
    
    try:
        # Load INI configuration
        config = load_ini_config(temp_ini_path)
        
        # Create brain with INI config
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
        print(f"✓ Configuration loaded successfully from INI")
        
        return brain
    finally:
        os.unlink(temp_ini_path)

def example_config_comparison():
    """Example comparing different configuration formats."""
    print("\n" + "="*60)
    print("Example 4: Configuration Format Comparison")
    print("="*60)
    
    # Different configurations in different formats
    json_data = {
        "brain": {"neuron_count": 1000},
        "environment": {"name": "GridWorld"}
    }
    
    yaml_data = """
brain:
  neuron_count: 1000
environment:
  name: GridWorld
"""
    
    ini_data = """
[brain]
neuron_count = 1000

[environment]
name = GridWorld
"""
    
    print("Comparing configuration approaches:")
    print("JSON: Concise, easy to parse programmatically")
    print("YAML: Readable, human-friendly")
    print("INI: Simple, widely supported")
    
    # Create brains with different formats
    brains = []
    
    # JSON
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        json.dump(json_data, f)
        json_path = f.name
    
    brains.append(("JSON", load_json_config(json_path)))
    os.unlink(json_path)
    
    # YAML
    with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
        f.write(yaml_data)
        yaml_path = f.name
    
    brains.append(("YAML", load_yaml_config(yaml_path)))
    os.unlink(yaml_path)
    
    # INI
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        f.write(ini_data)
        ini_path = f.name
    
    brains.append(("INI", load_ini_config(ini_path)))
    os.unlink(ini_path)
    
    # Test all configurations
    for format_name, config in brains:
        brain = pynlm.createBrain(config)
        brain.initialize()
        print(f"{format_name:6s}: {brain.getTotalNeuronCount()} neurons")
    
    print("\n✓ All configuration formats work correctly")

def example_serialization():
    """Example demonstrating configuration serialization."""
    print("\n" + "="*60)
    print("Example 5: Configuration Serialization")
    print("="*60)
    
    # Create brain with custom configuration
    config = pynlm.create_default_config()
    config.set("brain.neuron_count", 1500)
    config.set("environment.width", 30)
    config.set("environment.height", 30)
    
    print("Original configuration:")
    print(f"  Brain neurons: {config.get('brain.neuron_count')}")
    print(f"  Environment size: {config.get('environment.width')}x{config.get('environment.height')}")
    
    # Save configuration to file
    print("\nSaving configuration to file...")
    config.saveToFile("example_config.json")
    
    # Load configuration from file
    print("Loading configuration from file...")
    new_config = pynlm.create_default_config()
    new_config.loadFromFile("example_config.json")
    
    print(f"Loaded configuration:")
    print(f"  Brain neurons: {new_config.get('brain.neuron_count')}")
    print(f"  Environment size: {new_config.get('environment.width')}x{new_config.get('environment.height')}")
    
    # Create brain from loaded configuration
    brain = pynlm.createBrain(new_config)
    brain.initialize()
    
    print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # Clean up
    if os.path.exists("example_config.json"):
        os.remove("example_config.json")
    
    print("✓ Configuration serialization test completed")

def main():
    """Run all configuration examples."""
    print("NLM Configuration File Examples")
    print("This script demonstrates loading and using NLM configurations")
    print("from different file formats (JSON, YAML, INI).")
    
    # Run all examples
    example_json_config()
    example_yaml_config()
    example_ini_config()
    example_config_comparison()
    example_serialization()
    
    print("\n" + "="*60)
    print("All configuration examples completed successfully!")
    print("="*60)

if __name__ == "__main__":
    main()
