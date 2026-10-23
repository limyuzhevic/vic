#!/usr/bin/env python3
"""
Example 1: Basic Brain Simulation

This is a minimal example demonstrating how to create and run a basic NLM brain simulation.
It shows the fundamental workflow: create config -> create brain -> initialize -> simulate.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

import pynlm
import time

def main():
    print("=== NLM Basic Brain Simulation Example ===")
    print()
    
    # Create configuration
    config = pynlm.create_default_config()
    config.set("neuron_count", 100)  # Smaller for demo
    config.set("region_count", 1)
    
    # Create brain
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Running simulation for 500 steps...")
    print()
    
    # Run simulation
    for step in range(500):
        brain.step(step)
        
        if step % 100 == 0:
            firing_rate = brain.getAverageFiringRate()
            print(f"Step {step:3d}: Firing rate = {firing_rate:.3f}")
    
    print()
    print("Final statistics:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print()
    
    print("Example completed successfully!")

if __name__ == "__main__":
    main()
