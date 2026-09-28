#!/usr/bin/env python3
"""
Example 1: Basic NLM Brain Simulation

This example demonstrates the simplest possible NLM brain simulation,
showing how to create, initialize, and run a basic neural network.
"""

import pynlm

def basic_brain_simulation():
    """Run a basic brain simulation."""
    
    print("=== Example 1: Basic Brain Simulation ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.05)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Initial synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation
    print("\nRunning simulation...")
    for step in range(100):
        brain.step(step)
        
        # Print progress every 25 steps
        if step % 25 == 0:
            firing = brain.getFiringNeuronCount()
            spikes = brain.getTotalSpikeCount()
            print(f"  Step {step}: {firing} neurons firing, {spikes} total spikes")
    
    # Final statistics
    final_firing = brain.getFiringNeuronCount()
    final_spikes = brain.getTotalSpikeCount()
    avg_firing_rate = brain.getAverageFiringRate()
    
    print(f"\nSimulation complete!")
    print(f"  Final firing neurons: {final_firing}")
    print(f"  Total spikes: {final_spikes}")
    print(f"  Average firing rate: {avg_firing_rate:.3f}")
    
    return brain

if __name__ == "__main__":
    basic_brain_simulation()