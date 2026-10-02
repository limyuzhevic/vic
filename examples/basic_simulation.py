"""
Basic brain simulation example for NLM.

This example demonstrates the basic usage of the NLM Python bindings,
including brain creation, configuration, and simple simulation.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from pynlm import create_default_config, create_brain, create_simple_world, create_agent_brain
import time

def basic_simulation():
    """Run a basic simulation."""
    print("=== Basic NLM Simulation Example ===")
    
    # Create configuration
    print("Creating configuration...")
    config = create_default_config()
    
    # Set some parameters
    config.set("neuron_count", 500)
    config.set("region_count", 2)
    config.set("connection_probability", 0.1)
    config.set("seed", 42)
    
    print(f"Configuration: {config.summary()}")
    
    # Create brain
    print("\nInitializing brain...")
    brain = create_brain(config)
    brain.initialize()
    
    print(f"Brain created with {brain.get_total_neuron_count()} neurons and {brain.get_total_synapse_count()} synapses")
    
    # Run simulation
    print("\nRunning simulation...")
    start_time = time.time()
    
    for step in range(100):
        brain.step(step)
        if step % 20 == 0:
            print(f"Step {step}: {brain.get_firing_neuron_count()} firing neurons")
    
    end_time = time.time()
    
    print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
    print(f"Total spikes: {brain.get_total_spike_count()}")
    print(f"Final firing rate: {brain.get_average_firing_rate():.2f} Hz")
    
    # Save brain state
    brain.save("basic_simulation_checkpoint.nml")
    print("\nBrain state saved to 'basic_simulation_checkpoint.nml'")
    
    return brain

if __name__ == "__main__":
    basic_simulation()