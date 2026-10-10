#!/usr/bin/env python3
"""
Simple NLM brain simulation example.

This example demonstrates basic NLM brain simulation functionality.
"""

import pynlm

def main():
    print("=== NLM Brain Simulation Example ===")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    for step in range(10):
        brain.step(step)
    
    # Get statistics
    total_spikes = brain.getTotalSpikeCount()
    firing_rate = brain.getAverageFiringRate()
    firing_neurons = brain.getFiringNeuronCount()
    
    print(f"Simulation completed:")
    print(f"  Total spikes: {total_spikes}")
    print(f"  Firing rate: {firing_rate:.2f}")
    print(f"  Firing neurons: {firing_neurons}")
    print(f"  Success: {total_spikes > 0}")

if __name__ == "__main__":
    main()