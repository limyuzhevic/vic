#!/usr/bin/env python3

"""
Example 4: Configuration Management

Demonstrates configuration loading, validation, and saving.
This example shows how to customize brain configurations.
"""

import sys
import os

# Add parent directory to path for NLM imports
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import pynlm
import json
from pathlib import Path

def main():
    print("=== NLM Example 4: Configuration Management ===\n")
    
    # Create configuration
    print("1. Creating default configuration...")
    config = pynlm.create_default_config()
    
    print(f"   Created default configuration")
    
    # Modify configuration
    print("\n2. Modifying configuration...")
    config.set("neuron_count", 2000)
    config.set("region_count", 2)
    config.set("connection_probability", 0.05)
    config.set("stdp_ltp_weight", 0.015f)
    config.set("stdp_ltd_weight", 0.012f)
    
    print(f"   Modified configuration:")
    print(f"     Neuron count: {config.get_or('neuron_count', 1000)}")
    print(f"     Region count: {config.get_or('region_count', 1)}")
    print(f"     Connection probability: {config.get_or('connection_probability', 0.05)}")
    
    # Validate configuration
    print("\n3. Validating configuration...")
    warnings = pynlm.utils.validate_config(config)
    
    if warnings:
        print(f"   Configuration warnings:")
        for warning in warnings:
            print(f"     - {warning}")
    else:
        print(f"   ✓ Configuration is valid")
    
    # Save configuration
    print("\n4. Saving configuration...")
    
    config_path = Path("results") / "example_config.json"
    pynlm.save_config(config, str(config_path))
    print(f"   Configuration saved to: {config_path}")
    
    # Load configuration
    print("\n5. Loading configuration...")
    
    loaded_config = pynlm.load_config(str(config_path))
    print(f"   Loaded configuration from file:")
    print(f"     Keys: {loaded_config.get_keys()}")
    
    # Create brain with loaded config
    print("\n6. Creating brain with loaded configuration...")
    
    brain = pynlm.create_brain(loaded_config)
    brain.initialize()
    
    print(f"   Created brain with loaded configuration:")
    print(f"     Neurons: {brain.get_total_neuron_count()}")
    print(f"     Synapses: {brain.get_total_synapse_count()}")
    
    # Run simulation with new config
    print("\n7. Running simulation with new configuration...")
    
    for step in range(50):
        brain.step(step)
    
    print(f"   Simulation completed: {brain.get_total_spike_count()} spikes")
    
    # Save results
    print("\n8. Saving results...")
    
    results = {
        "example": "configuration",
        "neuron_count": brain.get_total_neuron_count(),
        "synapse_count": brain.get_total_synapse_count(),
        "total_spikes": brain.get_total_spike_count(),
        "config_keys": loaded_config.get_keys()
    }
    
    results_file = Path("results") / "configuration_results.json"
    with open(results_file, "w") as f:
        json.dump(results, f, indent=2)
    
    print(f"   Results saved to: {results_file}")
    
    print(f"\n✓ Example completed successfully!")
    return True

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\n✗ Error: {e}")
        sys.exit(1)