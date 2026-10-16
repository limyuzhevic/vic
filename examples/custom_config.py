#!/usr/bin/env python3
"""Demonstrate custom configuration setup.

This example shows how to create and use custom brain configurations:
- Loading from JSON files
- Modifying configuration parameters
- Creating specialized configurations for different tasks
- Configuration validation and error handling
"""

import pynlm
import sys
import json
import os

def create_basic_config():
    """Create a basic configuration for simple tasks."""
    config = pynlm.createDefaultConfig()
    
    # Basic settings
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.1)
    
    # Simple learning
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.01)
    
    # Basic development
    config.set("development.enable", False)  # Disable for stability
    
    return config

def create_advanced_config():
    """Create an advanced configuration for complex tasks."""
    config = pynlm.createDefaultConfig()
    
    # Large brain
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.connection_probability", 0.08)
    
    # Advanced learning
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.005)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Strong neuromodulation
    config.set("neuromod.dopamine.scale", 2.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Full development
    config.set("development.enable", True)
    config.set("development.critical_period_length", 200)
    
    # Enhanced environment
    config.set("environment.width", 30)
    config.set("environment.height", 30)
    
    return config

def create_training_config():
    """Create configuration optimized for training/learning."""
    config = pynlm.createDefaultConfig()
    
    # Medium-sized brain
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.12)
    
    # Strong plasticity
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.02)
    config.set("plasticity.stdp.tau_plus", 30.0)
    config.set("plasticity.stdp.tau_minus", 40.0)
    
    # Curiosity-driven exploration
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.exploration_rate", 0.3)
    
    # Memory systems
    config.set("memory.working_memory.enable", True)
    config.set("memory.episodic.enable", True)
    
    # Development critical period
    config.set("development.enable", True)
    config.set("development.critical_period_length", 100)
    
    return config

def create_visualization_config():
    """Create configuration for visualization/debugging."""
    config = pynlm.createDefaultConfig()
    
    # Small brain for fast testing
    config.set("brain.neuron_count", 100)
    config.set("brain.synapse_density", 0.2)
    
    # Basic learning
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.01)
    
    # Disable some systems for simplicity
    config.set("development.enable", False)
    config.set("neuromod.curiosity.enable", False)
    
    # Enable logging
    config.set("log_level", "DEBUG")
    config.set("log_to_file", True)
    
    return config

def demonstrate_config_creation():
    """Demonstrate creating different configuration types."""
    print("\n=== Configuration Creation Examples ===\n")
    
    # 1. Basic configuration
    print("1. Basic Configuration (create_basic_config):")
    basic_config = create_basic_config()
    print(f"   Neurons: {basic_config.getKeys().get('brain.neuron_count', 'N/A')}")
    print(f"   STDP enabled: {basic_config.getKeys().get('plasticity.stdp.enable', 'N/A')}")
    print()
    
    # 2. Advanced configuration
    print("2. Advanced Configuration (create_advanced_config):")
    advanced_config = create_advanced_config()
    print(f"   Neurons: {advanced_config.getKeys().get('brain.neuron_count', 'N/A')}")
    print(f"   Development enabled: {advanced_config.getKeys().get('development.enable', 'N/A')}")
    print()
    
    # 3. Training configuration
    print("3. Training Configuration (create_training_config):")
    training_config = create_training_config()
    print(f"   Neurons: {training_config.getKeys().get('brain.neuron_count', 'N/A')}")
    print(f"   STDP learning rate: {training_config.getKeys().get('plasticity.stdp.learning_rate', 'N/A')}")
    print()
    
    # 4. Visualization configuration
    print("4. Visualization Configuration (create_visualization_config):")
    viz_config = create_visualization_config()
    print(f"   Neurons: {viz_config.getKeys().get('brain.neuron_count', 'N/A')}")
    print(f"   Logging enabled: {viz_config.getKeys().get('log_to_file', 'N/A')}")
    print()
    
    return basic_config, advanced_config, training_config, viz_config

def demonstrate_config_modification():
    """Demonstrate modifying existing configurations."""
    print("\n=== Configuration Modification Examples ===\n")
    
    # Start with default
    config = pynlm.createDefaultConfig()
    
    print("Initial configuration:")
    print(f"   Default neuron count: {config.getKeys().get('brain.neuron_count', 'N/A')}")
    print()
    
    # Modify various parameters
    print("Modifying configuration parameters:")
    
    # Brain parameters
    config.set("brain.neuron_count", 750)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.v_thresh", -40.0)  # Higher threshold
    
    print(f"   Set neuron count to: {config.getKeys().get('brain.neuron_count', 'N/A')}")
    print(f"   Set synapse density to: {config.getKeys().get('brain.synapse_density', 'N/A')}")
    print()
    
    # Plasticity parameters
    config.set("plasticity.stdp.learning_rate", 0.008)
    config.set("plasticity.hebbian.enable", True)
    
    print(f"   Set STDP learning rate to: {config.getKeys().get('plasticity.stdp.learning_rate', 'N/A')}")
    print(f"   Enable Hebbian learning: {config.getKeys().get('plasticity.hebbian.enable', 'N/A')}")
    print()
    
    # Neuromodulation parameters
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.exploration_rate", 0.5)
    
    print(f"   Enable curiosity: {config.getKeys().get('neuromod.curiosity.enable', 'N/A')}")
    print(f"   Set exploration rate to: {config.getKeys().get('neuromod.curiosity.exploration_rate', 'N/A')}")
    print()
    
    return config

def demonstrate_config_validation():
    """Demonstrate configuration validation and error handling."""
    print("\n=== Configuration Validation Examples ===\n")
    
    config = pynlm.createDefaultConfig()
    
    print("Testing configuration validation:")
    
    # Test valid values
    try:
        config.set("brain.neuron_count", 1000)
        print("✓ Valid neuron count accepted")
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
    
    try:
        config.set("brain.synapse_density", 0.15)
        print("✓ Valid synapse density accepted")
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
    
    try:
        config.set("plasticity.stdp.learning_rate", 0.01)
        print("✓ Valid learning rate accepted")
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
    
    print()
    
    # Test edge cases
    print("Testing edge cases:")
    
    # Very small values
    try:
        config.set("brain.neuron_count", 10)
        print("✓ Small neuron count accepted")
    except Exception as e:
        print(f"✗ Small value rejected: {e}")
    
    # Large values
    try:
        config.set("brain.neuron_count", 10000)
        print("✓ Large neuron count accepted")
    except Exception as e:
        print(f"✗ Large value rejected: {e}")
    
    # Fractional learning rate
    try:
        config.set("plasticity.stdp.learning_rate", 0.00001)
        print("✓ Very small learning rate accepted")
    except Exception as e:
        print(f"✗ Small learning rate rejected: {e}")
    
    print()
    return config

def demonstrate_json_config():
    """Demonstrate working with JSON configuration files."""
    print("\n=== JSON Configuration File Examples ===\n")
    
    # 1. Create JSON config file
    print("1. Creating JSON configuration file:")
    
    json_config = {
        "brain": {
            "neuron_count": 800,
            "synapse_density": 0.12,
            "v_thresh": -45.0,
            "v_rest": -70.0
        },
        "plasticity": {
            "stdp": {
                "enable": True,
                "learning_rate": 0.015,
                "tau_plus": 25.0,
                "tau_minus": 30.0
            },
            "hebbian": {
                "enable": True
            }
        },
        "development": {
            "enable": True,
            "critical_period_length": 150
        },
        "neuromodulation": {
            "curiosity": {
                "enable": True,
                "exploration_rate": 0.4
            }
        }
    }
    
    json_filename = "custom_config.json"
    with open(json_filename, 'w') as f:
        json.dump(json_config, f, indent=2)
    
    print(f"   ✓ Created {json_filename}")
    print(f"   File size: {os.path.getsize(json_filename)} bytes")
    print()
    
    # 2. Load JSON config
    print("2. Loading JSON configuration:")
    
    try:
        config = pynlm.createDefaultConfig()
        config.loadFromFile(json_filename)
        
        print("✓ Successfully loaded JSON configuration")
        print(f"   Loaded neuron count: {config.getKeys().get('brain.neuron_count', 'N/A')}")
        print(f"   Loaded STDP learning rate: {config.getKeys().get('plasticity.stdp.learning_rate', 'N/A')}")
        
    except Exception as e:
        print(f"✗ Failed to load JSON config: {e}")
    
    print()
    
    # 3. Save current config to JSON
    print("3. Saving current config to JSON:")
    
    save_config = pynlm.createDefaultConfig()
    save_config.set("brain.neuron_count", 600)
    save_config.set("plasticity.stdp.learning_rate", 0.02)
    save_config.set("development.enable", True)
    
    save_filename = "exported_config.json"
    save_config.saveToFile(save_filename)
    print(f"   ✓ Saved to {save_filename}")
    
    # Load and verify
    loaded_config = pynlm.createDefaultConfig()
    loaded_config.loadFromFile(save_filename)
    
    print(f"   ✓ Verified: neuron_count = {loaded_config.getKeys().get('brain.neuron_count', 'N/A')}")
    
    print()
    
    # 4. Cleanup
    print("4. Cleaning up:")
    
    files_to_remove = [json_filename, "exported_config.json"]
    
    for filename in files_to_remove:
        if os.path.exists(filename):
            os.remove(filename)
            print(f"   ✓ Removed {filename}")
    
    print()
    return config

def demonstrate_config_comparison():
    """Demonstrate comparing different configurations."""
    print("\n=== Configuration Comparison Examples ===\n")
    
    # Create different configs
    basic_config = create_basic_config()
    training_config = create_training_config()
    advanced_config = create_advanced_config()
    
    print("Comparing configuration parameters:")
    print("\nBasic Config vs Training Config:")
    basic_neurons = basic_config.getKeys().get("brain.neuron_count", 0)
    training_neurons = training_config.getKeys().get("brain.neuron_count", 0)
    print(f"   Neuron count: {basic_neurons} vs {training_neurons}")
    
    basic_stdp_lr = basic_config.getKeys().get("plasticity.stdp.learning_rate", 0)
    training_stdp_lr = training_config.getKeys().get("plasticity.stdp.learning_rate", 0)
    print(f"   STDP learning rate: {basic_stdp_lr} vs {training_stdp_lr}")
    
    basic_dev = basic_config.getKeys().get("development.enable", False)
    training_dev = training_config.getKeys().get("development.enable", False)
    print(f"   Development enabled: {basic_dev} vs {training_dev}")
    
    print("\nTraining Config vs Advanced Config:")
    training_neurons = training_config.getKeys().get("brain.neuron_count", 0)
    advanced_neurons = advanced_config.getKeys().get("brain.neuron_count", 0)
    print(f"   Neuron count: {training_neurons} vs {advanced_neurons}")
    
    training_curiosity = training_config.getKeys().get("neuromod.curiosity.enable", False)
    advanced_curiosity = advanced_config.getKeys().get("neuromod.curiosity.enable", False)
    print(f"   Curiosity enabled: {training_curiosity} vs {advanced_curiosity}")
    
    # Show recommended use cases
    print("\nConfiguration recommendations:")
    print("• Basic config: Quick testing, prototyping")
    print("• Training config: Learning and adaptation tasks")
    print("• Advanced config: Complex behaviors, full development")
    
    print()
    return basic_config, training_config, advanced_config

def main():
    print("=== NLM Custom Configuration Demo ===\n")
    
    # Run all demonstrations
    demonstrate_config_creation()
    
    modified_config = demonstrate_config_modification()
    
    validated_config = demonstrate_config_validation()
    
    json_config = demonstrate_json_config()
    
    configs = demonstrate_config_comparison()
    
    print("\n=== Custom Configuration Demo Complete ===")
    print("\nSummary of configuration types demonstrated:")
    print("1. Basic configuration for simple tasks")
    print("2. Advanced configuration for complex behaviors")
    print("3. Training configuration optimized for learning")
    print("4. Visualization configuration for debugging")
    print("5. JSON file I/O for configuration sharing")
    print("6. Configuration comparison and validation")

if __name__ == "__main__":
    try:
        main()
    except ImportError as e:
        print(f"Error importing pynlm: {e}")
        print("Make sure NLM Python bindings are installed.")
        sys.exit(1)
    except Exception as e:
        print(f"Error running configuration demo: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
