#!/usr/bin/env python3
"""
NLM - Minimal Working Example for Beginners

This is the simplest possible example that demonstrates the basic NLM functionality.
Perfect for those who have never used NLM before.
"""

import pynlm

def main():
    print("=== NLM Minimal Working Example ===")
    print("Creating a virtual brain...")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    print(f"Brain configuration: {config.summary()}")
    
    # Create and initialize the brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run a simple simulation for 10 steps
    print("\nRunning simulation for 10 steps...")
    for i in range(10):
        brain.step(i)
        if i % 5 == 0:
            print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
    
    print("\nSimulation complete!")
    print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Current developmental stage: {brain.getDevelopmentalStage()}")
    
    print("\n=== Example Complete ===")

if __name__ == "__main__":
    main()