#!/usr/bin/env python3
"""
NLM Basic Brain Example
=======================

This example demonstrates creating a simple spiking neural network
using the NLM (Neural Learning Machine) Python bindings.

The brain will:
1. Initialize with 1000 neurons
2. Simulate for 100 steps
3. Print statistics about activity

This is the simplest possible example for beginners.
"""

import pynlm

def basic_brain_example():
    print("=== NLM Basic Brain Example ===")
    print("Creating and running a simple brain...")
    
    # Create a brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # Run the brain for 100 steps
    for i in range(100):
        brain.step(i)
        
        # Print statistics every 25 steps
        if i % 25 == 0:
            print(f"Step {i}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
            print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
            print()
    
    print("=== Simulation Complete ===")
    print(f"Final statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    return brain

if __name__ == "__main__":
    basic_brain_example()