"""
Example demonstrating configuration capabilities
and parameter tuning.
"""
import pynlm
import json

def config_examples():
    """Demonstrate configuration options and file I/O."""
    print("=== Configuration Examples ===")
    
    # Example 1: Create and modify configuration
    print("\n1. Creating and modifying configuration:")
    config = pynlm.createDefaultConfig()
    
    # Get current configuration
    print(f"Default neuron count: {config.getOr('brain.neuron_count', 1000)}")
    print(f"Default connection probability: {config.getOr('brain.connection_probability', 0.1):.3f}")
    
    # Modify configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.connection_probability", 0.15)
    config.set("simulation_timestep", 0.0005)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    
    # Verify changes
    print(f"Modified neuron count: {config.getOr('brain.neuron_count', 1000)}")
    print(f"Modified connection probability: {config.getOr('brain.connection_probability', 0.1):.3f}")
    
    # Example 2: Configuration keys and validation
    print("\n2. Configuration keys and validation:")
    keys = config.getKeys()
    print(f"All configuration keys: {keys}")
    
    # Check specific keys exist
    important_keys = ["brain.neuron_count", "simulation_timestep", "plasticity.stdp.enable"]
    for key in important_keys:
        if config.has(key):
            print(f"✓ '{key}' is configured")
        else:
            print(f"✗ '{key}' is missing")
    
    # Example 3: Save and load configuration
    print("\n3. Configuration file I/O:")
    
    # Save to JSON file
    config.saveToFile("example_config.json")
    print("Configuration saved to 'example_config.json'")
    
    # Load from JSON file
    new_config = pynlm.createDefaultConfig()
    if new_config.loadFromFile("example_config.json"):
        print("✓ Configuration loaded successfully")
        print(f"Loaded neuron count: {new_config.getOr('brain.neuron_count', 1000)}")
    else:
        print("✗ Failed to load configuration (likely not JSON format)")
    
    # Example 4: Create brain with custom configuration
    print("\n4. Brain creation with configuration:")
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    return config, brain

if __name__ == "__main__":
    config, brain = config_examples()
