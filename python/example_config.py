#!/usr/bin/env python3
"""
NLM Configuration Example - Python API

This script demonstrates how to create and use NLM configurations programmatically.
"""

import pynlm
import json
import os

def create_minimal_config():
    """Create a minimal configuration with just required parameters."""
    config = pynlm.createDefaultConfig()
    return config

def create_research_config():
    """Create a configuration optimized for research."""
    config = pynlm.createDefaultConfig()
    
    # Brain configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.05)
    config.set("brain.v_thresh", -55.0)
    
    # Plasticity configuration
    config.set("plasticity.stdp.enable", true)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.hebbian.enable", true)
    config.set("plasticity.structural.enable", true)
    
    # Neuromodulation
    config.set("neuromod.dopamine.scale", 2.0)
    config.set("neuromod.curiosity.enable", true)
    
    # Environment
    config.set("environment.name", "GridWorld")
    config.set("environment.width", 20)
    config.set("environment.height", 20)
    
    # Logging
    config.set("logging.level", "INFO")
    config.set("logging.to_file", true)
    config.set("logging.filename", "research_simulation.log")
    
    return config

def create_debug_config():
    """Create a configuration optimized for debugging."""
    config = pynlm.createDefaultConfig()
    
    # Small scale for debugging
    config.set("brain.neuron_count", 100)
    config.set("brain.synapse_density", 0.2)
    
    # Verbose logging
    config.set("logging.level", "DEBUG")
    config.set("logging.to_file", true)
    config.set("logging.filename", "debug.log")
    
    # Visualization
    config.set("visualization.enabled", true)
    config.set("visualization.update_rate", 60.0)
    
    # Long simulation for debugging
    config.set("simulation.max_steps", 5000)
    
    return config

def save_config(config, filename):
    """Save configuration to a JSON file."""
    config.saveToFile(filename)
    print(f"Configuration saved to {filename}")

def load_config(filename):
    """Load configuration from a JSON file."""
    if os.path.exists(filename):
        config = pynlm.createDefaultConfig()
        config.loadFromFile(filename)
        print(f"Configuration loaded from {filename}")
        return config
    else:
        print(f"Configuration file {filename} not found")
        return None

def validate_config(config):
    """Validate configuration and print results."""
    is_valid = config.isValid()
    
    if is_valid:
        print("✓ Configuration is valid")
    else:
        print("✗ Configuration is invalid")
        
        # Show validation results
        results = config.validate()
        for result in results:
            if !result.isValid:
                print(f"  ERROR: {result.parameterName}: {result.errorMessage}")
    
    # Show configuration summary
    summary = config.summary()
    print("\nConfiguration Summary:")
    print(summary)
    
    return is_valid

def main():
    """Main function demonstrating configuration usage."""
    print("NLM Configuration Examples")
    print("=========================")
    
    # Example 1: Minimal configuration
    print("\n1. Minimal Configuration:")
    print("-" * 40)
    minimal_config = create_minimal_config()
    validate_config(minimal_config)
    
    # Example 2: Research configuration
    print("\n2. Research Configuration:")
    print("-" * 40)
    research_config = create_research_config()
    validate_config(research_config)
    
    # Example 3: Debug configuration
    print("\n3. Debug Configuration:")
    print("-" * 40)
    debug_config = create_debug_config()
    validate_config(debug_config)
    
    # Example 4: Save and load configuration
    print("\n4. Save and Load Configuration:")
    print("-" * 40)
    save_config(research_config, "example_research_config.json")
    
    loaded_config = load_config("example_research_config.json")
    if loaded_config:
        validate_config(loaded_config)
    
    # Example 5: Command-line style configuration
    print("\n5. Command-line Style Configuration:")
    print("-" * 40)
    cmd_config = pynlm.createDefaultConfig()
    cmd_config.set("brain.neuron_count", 500)
    cmd_config.set("plasticity.stdp.enable", false)
    cmd_config.set("logging.level", "WARNING")
    
    validate_config(cmd_config)
    
    print("\nConfiguration examples completed successfully!")

if __name__ == "__main__":
    main()
