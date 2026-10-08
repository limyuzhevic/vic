#!/usr/bin/env python3
"""
NLM Python Example: Basic Brain Simulation
============================================

This example demonstrates the basic usage of the enhanced NLM Python API.
It creates a simple brain, world, and runs a basic simulation.
"""

import pynlm as nlm
import numpy as np

def main():
    print("NLM Python Basic Example")
    print("=" * 50)
    
    # Create configuration
    print("1. Creating configuration...")
    config = nlm.create_default_config()
    config.set("neuron_count", 500)
    config.set("region_count", 3)
    print(f"   Neuron count: {config.get('neuron_count')}")
    print(f"   Region count: {config.get('neuron_count')}")
    
    # Create brain
    print("\n2. Creating brain...")
    brain = nlm.create_brain(config)
    print(f"   Brain initialized: {brain.is_initialized()}")
    print(f"   Total neurons: {brain.get_total_neuron_count()}")
    
    # Create world
    print("\n3. Creating world...")
    world = nlm.create_simple_world()
    world.configure(100, 100, 20, 20)
    print(f"   World size: {world.get_width()}x{world.get_height()}")
    
    # Run simulation
    print("\n4. Running simulation...")
    steps = 10
    for step in range(steps):
        # Process sensory input
        percept = world.get_sensory_percept()
        if percept:
            brain.receive_sensory_input(percept)
        
        # Update world
        world.update(step)
        
        # Print progress every 5 steps
        if step % 5 == 0:
            stats = brain.get_neuron_statistics()
            print(f"   Step {step}: {stats['active_neurons']}/{stats['total_neurons']} active neurons")
            print(f"             Firing rate: {stats['average_firing_rate']:.2f} Hz")
    
    # Show final statistics
    print("\n5. Final statistics...")
    stats = brain.get_neuron_statistics()
    print(f"   Total neurons: {stats['total_neurons']}")
    print(f"   Active neurons: {stats['active_neurons']}")
    print(f"   Firing neurons: {stats['firing_neurons']}")
    print(f"   Total spikes: {stats['spike_count']}")
    print(f"   E/I ratio: {stats['e_i_ratio']:.2f}")
    print(f"   Average firing rate: {stats['avg_rate']:.2f} Hz")
    
    print("\n6. Saving brain state...")
    brain.save("example_brain.nlm")
    print("   Saved to: example_brain.nlm")
    
    print("\nExample completed successfully!")

if __name__ == "__main__":
    main()
