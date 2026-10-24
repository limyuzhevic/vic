#!/usr/bin/env python3
"""
NLM Configuration Examples and Tutorials

This script demonstrates various configuration examples for NLM (Neural Learning Machine).
It shows how to create and use different types of configurations for various use cases.

Usage:
    python config_examples.py [example_name]

Examples:
    python config_examples.py minimal          # Minimal configuration for testing
    python config_examples.py exploration      # Exploration-focused configuration
    python config_examples.py memory           # Memory-intensive configuration
    python config_examples.py standard         # Standard configuration (Phase 6)
    python config_examples.py all              # Show all examples
"""

import json
import yaml
import os
from pathlib import Path

# Add the parent directory to the path so we can import pynlm
import sys
sys.path.insert(0, str(Path(__file__).parent.parent))

import pynlm

def create_minimal_config():
    """Create a minimal configuration for testing and debugging."""
    config = pynlm.Config()
    
    # Minimal settings
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    config.set("connection_probability", 0.2)
    
    return config

def create_exploration_config():
    """Create an exploration-focused configuration for active learning."""
    config = pynlm.Config()
    
    # Larger network for exploration
    config.set("neuron_count", 2000)
    config.set("region_count", 2)
    config.set("connection_probability", 0.05)
    
    # Strong exploration mechanisms
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.scale", 1.0)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.scale", 1.0)
    
    # High plasticity for learning
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0005)
    
    # Development for exploration
    config.set("development.enable", True)
    config.set("development.stage", "CriticalPeriod")
    
    return config

def create_memory_config():
    """Create a memory-intensive configuration for experience storage."""
    config = pynlm.Config()
    
    # Network optimized for memory
    config.set("neuron_count", 1500)
    config.set("region_count", 3)
    config.set("connection_probability", 0.08)
    
    # Strong memory systems
    config.set("memory.working.capacity", 500)
    config.set("memory.episodic.max_episodes", 2000)
    config.set("memory.associative.capacity", 1000)
    
    # Memory consolidation
    config.set("replay.enable", True)
    config.set("replay.interval", 100)
    config.set("consolidation.enable", True)
    config.set("consolidation.interval", 1000)
    
    # Development for memory consolidation
    config.set("development.enable", True)
    config.set("development.synaptogenesis_rate", 0.001)
    config.set("development.pruning_rate", 0.0001)
    
    return config

def create_standard_config():
    """Create a standard Phase 6 configuration for balanced operation."""
    config = pynlm.Config()
    
    # Standard Phase 6 settings
    config.set("neuron_count", 1000)
    config.set("region_count", 1)
    config.set("connection_probability", 0.1)
    
    # Balanced neuromodulation
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.scale", 0.5)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.scale", 0.5)
    
    # Balanced plasticity
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0001)
    config.set("plasticity.structural.pruning_rate", 0.00001)
    
    # Development
    config.set("development.enable", True)
    config.set("development.stage", "Adult")
    
    return config

def save_config_to_file(config, filepath):
    """Save configuration to various formats."""
    
    # Convert config to dictionary for saving
    config_dict = {}
    for key in config.getKeys():
        try:
            config_dict[key] = config.get(key).value()
        except:
            # Handle complex types or values that can't be directly converted
            config_dict[key] = f"[{type(config.get(key).value()).__name__}]"
    
    # Save as JSON
    json_path = Path(filepath).with_suffix('.json')
    try:
        with open(json_path, 'w') as f:
            json.dump(config_dict, f, indent=2)
        print(f"✓ Saved JSON configuration to {json_path}")
    except Exception as e:
        print(f"✗ Failed to save JSON configuration to {json_path}: {e}")
    
    # Save as YAML
    yaml_path = Path(filepath).with_suffix('.yaml')
    try:
        with open(yaml_path, 'w') as f:
            yaml.dump(config_dict, f, default_flow_style=False)
        print(f"✓ Saved YAML configuration to {yaml_path}")
    except Exception as e:
        print(f"✗ Failed to save YAML configuration to {yaml_path}: {e}")
    
    # Save as simple key=value format (for backward compatibility)
    cfg_path = Path(filepath).with_suffix('.cfg')
    try:
        with open(cfg_path, 'w') as f:
            for key, value in config_dict.items():
                f.write(f"# {key}\n")
                f.write(f"{key} = {value}\n\n")
        print(f"✓ Saved simple configuration to {cfg_path}")
    except Exception as e:
        print(f"✗ Failed to save simple configuration to {cfg_path}: {e}")

def print_config_summary(config, name):
    """Print a summary of the configuration."""
    print(f"\n{'='*60}")
    print(f"Configuration: {name}")
    print(f"{'='*60}")
    
    print(f"Total keys: {len(config.getKeys())}")
    print(f"Keys: {', '.join(sorted(config.getKeys()))}")
    
    # Key settings
    important_keys = [
        "neuron_count",
        "region_count", 
        "connection_probability",
        "simulation_timestep"
    ]
    
    print("\nKey Settings:")
    for key in important_keys:
        if config.has(key):
            value = config.get(key).value()
            print(f"  {key}: {value}")

def demonstrate_usage(config, name):
    """Demonstrate how to use the configuration."""
    print(f"\n{'-'*40}")
    print(f"Using {name} Configuration")
    print(f"{'-'*40}")
    
    # Create brain with this configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run a simple test
    steps = 100
    for step in range(steps):
        brain.step(step)
        
        if (step + 1) % 20 == 0:
            print(f"Step {step + 1}: {brain.getFiringNeuronCount()} firing neurons")
    
    print(f"Test completed. Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    # Clean up
    brain.reset()

def main():
    """Main function."""
    examples = {
        "minimal": create_minimal_config,
        "exploration": create_exploration_config,
        "memory": create_memory_config,
        "standard": create_standard_config,
        "all": lambda: None  # Special case for showing all examples
    }
    
    if len(sys.argv) < 2:
        print(__doc__)
        return
    
    example_name = sys.argv[1].lower()
    
    if example_name not in examples:
        print(f"Unknown example: {example_name}")
        print(f"Available examples: {', '.join(examples.keys())}")
        return
    
    # Create the specified configuration
    if example_name == "all":
        for name, creator in examples.items():
            if name != "all":
                print(f"\n{'='*80}")
                print(f"Example: {name.upper()}")
                print(f"{'='*80}")
                
                config = creator()
                save_config_to_file(config, name)
                print_config_summary(config, name)
                
                # Skip demonstration for 'all' to avoid too much output
                if name != "all":
                    try:
                        demonstrate_usage(config, name)
                    except Exception as e:
                        print(f"✗ Demonstration failed: {e}")
    else:
        config = examples[example_name]()
        save_config_to_file(config, example_name)
        print_config_summary(config, example_name)
        
        try:
            demonstrate_usage(config, example_name)
        except Exception as e:
            print(f"✗ Demonstration failed: {e}")
    
    print(f"\n{'='*60}")
    print("Configuration examples completed successfully!")
    print(f"{'='*60}")

if __name__ == "__main__":
    main()