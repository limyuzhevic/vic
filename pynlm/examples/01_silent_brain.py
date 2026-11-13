#!/usr/bin/env python3
"""
Example 1: Silent Brain Simulation

This example demonstrates the simplest possible use of NLM - creating a brain
and making it simulate neural activity without any external interaction.
"""

import pynlm
import time

def create_silent_brain_example():
    """Create and run a simple brain simulation."""
    print("=== Silent Brain Example ===")
    
    # Step 1: Create a brain with default configuration
    print("1. Creating brain with default configuration...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Step 2: Initialize the brain
    print("2. Initializing brain...")
    brain.initialize()
    
    # Step 3: Run simulation for multiple steps
    print("3. Running simulation for 50 steps...")
    start_time = time.time()
    
    for step in range(50):
        brain.step(step)
        
        if step % 10 == 0:
            print(f"   Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    end_time = time.time()
    
    # Step 4: Display results
    print("4. Simulation completed!")
    print(f"   Total time: {end_time - start_time:.2f} seconds")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   Developmental stage: {brain.getDevelopmentalStage()}")
    
    return brain

if __name__ == "__main__":
    brain = create_silent_brain_example()
    print("\n✓ Silent brain example completed successfully!")