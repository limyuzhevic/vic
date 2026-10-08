#!/usr/bin/env python3
"""
NLM - Configuration Management Example

This example demonstrates advanced configuration management including:
- JSON configuration files
- YAML configuration files  
- Configuration validation
- Environment variable substitution
- Configuration merging
- Schema validation
"""

import pynlm
import tempfile
import os

def create_example_json_config():
    """Create a JSON configuration string."""
    return '''{
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.1,
        "connection_probability": 0.05,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true
        },
        "novelty": {
            "enable": true
        }
    },
    "random": {
        "seed": 42
    },
    "simulation": {
        "timestep": 0.001
    }
}'''

def create_example_yaml_config():
    """Create a YAML configuration string."""
    return '''brain:
  neuron_count: 1500
  synapse_density: 0.08
  connection_probability: 0.04
  v_thresh: -50.0
  v_rest: -70.0
  v_reset: -75.0

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
    tau_plus: 20.0
    tau_minus: 20.0
  hebbian:
    enable: true
  structural:
    enable: true
    synaptogenesis_rate: 0.0001
    pruning_rate: 0.00001

neuromod:
  dopamine:
    scale: 1.0
  curiosity:
    enable: true
  novelty:
    enable: true

random:
  seed: 123

simulation:
  timestep: 0.001

memory:
  working:
    capacity: 300
  episodic:
    max_episodes: 2000
  associative:
    pattern_count: 500
'''

def example_json_usage():
    """Demonstrate JSON configuration usage."""
    print("=== JSON Configuration Example ===")
    
    # Create config and load from JSON string
    config = pynlm.createDefaultConfig()
    
    json_string = create_example_json_config()
    print(f"Loading from JSON string ({len(json_string)} characters)...")
    
    success = config.loadFromJSON(json_string)
    if not success:
        print("Failed to load JSON configuration!")
        return None
    
    print("✓ JSON configuration loaded successfully!")
    print(f"  Loaded {config.getKeys().size()} configuration keys")
    
    # Show key configuration values
    print("\nKey configuration values:")
    print(f"  Brain neurons: {config.getIntOr('brain.neuron_count', 0)}")
    print(f"  Synapse density: {config.getFloatOr('brain.synapse_density', 0.0)}")
    print(f"  STDP enabled: {config.getBoolOr('plasticity.stdp.enable', false)}")
    print(f"  Curiosity enabled: {config.getBoolOr('neuromod.curiosity.enable', false)}")
    print(f"  Random seed: {config.getIntOr('random.seed', 0)}")
    
    # Validate configuration
    is_valid = config.validate()
    print(f"\nConfiguration validation: {'✓ PASS' if is_valid else '✗ FAIL'}")
    
    # Show validation errors if any
    if not is_valid:
        errors = config.getValidationErrors()
        print(f"  Errors: {errors}")
    
    return config

def example_yaml_usage():
    """Demonstrate YAML configuration usage."""
    print("\n=== YAML Configuration Example ===")
    
    # Create config and load from YAML string
    config = pynlm.createDefaultConfig()
    
    yaml_string = create_example_yaml_config()
    print(f"Loading from YAML string ({len(yaml_string)} characters)...")
    
    success = config.loadFromYAML(yaml_string)
    if not success:
        print("Failed to load YAML configuration!")
        return None
    
    print("✓ YAML configuration loaded successfully!")
    print(f"  Loaded {config.getKeys().size()} configuration keys")
    
    # Show key configuration values
    print("\nKey configuration values:")
    print(f"  Brain neurons: {config.getIntOr('brain.neuron_count', 0)}")
    print(f"  Synapse density: {config.getFloatOr('brain.synapse_density', 0.0)}")
    print(f"  STDP enabled: {config.getBoolOr('plasticity.stdp.enable', false)}")
    print(f"  Curiosity enabled: {config.getBoolOr('neuromod.curiosity.enable', false)}")
    print(f"  Working memory capacity: {config.getIntOr('memory.working.capacity', 0)}")
    
    # Validate configuration
    is_valid = config.validate()
    print(f"\nConfiguration validation: {'✓ PASS' if is_valid else '✗ FAIL'}")
    
    return config

def example_file_saving():
    """Demonstrate saving configuration to files."""
    print("\n=== File Saving Example ===")
    
    # Create a configuration
    config = pynlm.createDefaultConfig()
    config.setInt("brain.neuron_count", 3000)
    config.setFloat("brain.synapse_density", 0.12)
    config.setBool("neuromod.curiosity.enable", True)
    config.setBool("plasticity.stdp.enable", True)
    
    # Save as JSON
    json_file = "/tmp/test_config.json"
    success = config.saveToFile(json_file)
    if success:
        print(f"✓ Configuration saved to JSON file: {json_file}")
        
        # Verify we can load it back
        config2 = pynlm.createDefaultConfig()
        success2 = config2.loadFromFile(json_file)
        if success2:
            print("✓ Configuration loaded successfully from JSON file")
            print(f"  Neuron count: {config2.getIntOr('brain.neuron_count', 0)}")
    else:
        print("✗ Failed to save configuration to JSON file")
    
    # Save as JSON string
    json_string = config.saveToJSON()
    print(f"\nConfiguration as JSON string ({len(json_string)} characters):")
    print(json_string[:200] + "..." if len(json_string) > 200 else json_string)
    
    # Save as YAML string
    yaml_string = config.saveToYAML()
    print(f"\nConfiguration as YAML string ({len(yaml_string)} characters):")
    print(yaml_string[:200] + "..." if len(yaml_string) > 200 else yaml_string)
    
    return config

def example_merge_configuration():
    """Demonstrate configuration merging."""
    print("\n=== Configuration Merging Example ===")
    
    # Base configuration
    base_config = pynlm.createDefaultConfig()
    base_config.setInt("brain.neuron_count", 1000)
    base_config.setBool("neuromod.curiosity.enable", True)
    base_config.setBool("plasticity.stdp.enable", True)
    base_config.setString("description", "Base configuration")
    
    print("Base configuration:")
    print(f"  Neurons: {base_config.getIntOr('brain.neuron_count', 0)}")
    print(f"  Curiosity: {base_config.getBoolOr('neuromod.curiosity.enable', false)}")
    print(f"  STDP: {base_config.getBoolOr('plasticity.stdp.enable', false)}")
    
    # Override configuration
    override_config = pynlm.createDefaultConfig()
    override_config.setInt("brain.neuron_count", 2000)  # Override
    override_config.setBool("neuromod.curiosity.enable", False)  # Override
    override_config.setString("optimization", "research")
    override_config.setString("description", "Override configuration")
    
    print("\nOverride configuration:")
    print(f"  Neurons: {override_config.getIntOr('brain.neuron_count', 0)}")
    print(f"  Curiosity: {override_config.getBoolOr('neuromod.curiosity.enable', false)}")
    print(f"  Optimization: {override_config.get('optimization', 'Not set')}")
    
    # Merge configurations
    merged_config = base_config.copy()
    merged_config.merge(override_config)
    
    print("\nMerged configuration:")
    print(f"  Neurons: {merged_config.getIntOr('brain.neuron_count', 0)} (from override)")
    print(f"  Curiosity: {merged_config.getBoolOr('neuromod.curiosity.enable', false)} (from override)")
    print(f"  STDP: {merged_config.getBoolOr('plasticity.stdp.enable', false)} (from base)")
    print(f"  Optimization: {merged_config.get('optimization', 'Not set')} (from override)")
    print(f"  Description: {merged_config.get('description', 'Not set')} (from override)")
    
    return merged_config

def example_environment_substitution():
    """Demonstrate environment variable substitution."""
    print("\n=== Environment Variable Substitution Example ===")
    
    # Create config with environment variables
    config = pynlm.createDefaultConfig()
    config.setString("brain.name", "Brain_${PROJECT_NAME}")
    config.setString("paths.data", "/data/${USER}/nlm")
    config.setString("simulation.seed", "${RANDOM_SEED:-42}")  # Default if not set
    
    print("Configuration with environment variables:")
    print(f"  Brain name: {config.get('brain.name', 'Not set')}")
    print(f"  Data path: {config.get('paths.data', 'Not set')}")
    print(f"  Simulation seed: {config.get('simulation.seed', 'Not set')}")
    
    # Set some environment variables for demonstration
    os.environ["PROJECT_NAME"] = "ResearchProject"
    os.environ["USER"] = "alice"
    os.environ["RANDOM_SEED"] = "12345"
    
    # Substitute environment variables
    config.substituteEnvironmentVariables()
    
    print("\nAfter environment variable substitution:")
    print(f"  Brain name: {config.get('brain.name', 'Not set')}")
    print(f"  Data path: {config.get('paths.data', 'Not set')}")
    print(f"  Simulation seed: {config.get('simulation.seed', 'Not set')}")
    
    # Clean up
    del os.environ["PROJECT_NAME"]
    del os.environ["USER"]
    del os.environ["RANDOM_SEED"]
    
    return config

def main():
    """Main function to run the configuration example."""
    print("NLM Configuration Management Example")
    print("=" * 50)
    print("This example demonstrates advanced configuration features.\n")
    
    # Run all configuration examples
    example_json_usage()
    example_yaml_usage()
    example_file_saving()
    example_merge_configuration()
    example_environment_substitution()
    
    print("\n" + "=" * 50)
    print("Configuration management examples complete!")
    print("\nKey features demonstrated:")
    print("  ✓ JSON and YAML configuration support")
    print("  ✓ Configuration validation")
    print("  ✓ File saving and loading")
    print("  ✓ Configuration merging")
    print("  ✓ Environment variable substitution")
    print("  ✓ Type-safe configuration access")

if __name__ == "__main__":
    main()