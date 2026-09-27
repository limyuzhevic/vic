#!/usr/bin/env python3
"""Example 1: Silent Brain (Just Neurons)

This example demonstrates the simplest possible use of NLM - creating a virtual brain
and running it without any world interaction. This is useful for testing the neural
network itself and understanding how neurons work.
"""

import pynlm

def silent_brain_example():
    """Create a virtual brain and run it silently."""
    print("=== Silent Brain Example ===")
    
    # One line to create a virtual brain with default settings
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Initialize it (always call this before using the brain!)
    brain.initialize()
    
    # Make it "think" for 10 steps (just neurons firing)
    for i in range(10):
        brain.step(i)
    
    print("Silent brain test done!")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")

if __name__ == "__main__":
    silent_brain_example()