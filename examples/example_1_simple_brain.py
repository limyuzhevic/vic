#!/usr/bin/env python3
"""Example 1: Simple Brain Simulation"""

import nlm

def main():
    print("=== Example 1: Simple Brain Simulation ===\n")
    
    # Create a brain with default configuration
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    
    print("1. Created brain with configuration")
    print(f"   - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   - Regions: {brain.getRegionCount()}\n")
    
    # Initialize the brain
    brain.initialize()
    print("2. Brain initialized successfully")
    print(f"   - Initial spikes: {brain.getTotalSpikeCount()}\n")
    
    # Run a simple simulation
    print("3. Running simulation (10 steps)...")
    for step in range(10):
        brain.step(step)
        if step % 5 == 0:
            print(f"   - Step {step}: spikes={brain.getTotalSpikeCount()}")
    
    print(f"\n4. Simulation completed")
    print(f"   - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   - Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"   - Current developmental stage: {brain.getDevelopmentalStage()}")
    
    print("\n=== Example 1 Complete ===")
    return brain

if __name__ == "__main__":
    main()