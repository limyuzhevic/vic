#!/usr/bin/env python3
"""
Example 1: Basic Brain Simulation
Simple example demonstrating the most basic NLM functionality.
"""

import pynlm
import time

def basic_brain_simulation():
    """Run a basic brain simulation."""
    print("=== Basic NLM Brain Simulation ===")
    
    # Create brain with default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Initialize brain
    brain.initialize()
    print(f"✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation steps
    print("Running simulation steps...")
    start_time = time.time()
    
    for step in range(100):
        brain.step(step)
        
        if (step + 1) % 20 == 0:
            print(f"  Step {step + 1}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{brain.getAverageFiringRate():.2f} Hz average firing rate")
    
    end_time = time.time()
    print(f"✓ Simulation completed in {end_time - start_time:.2f} seconds")
    
    # Show final statistics
    print(f"\nFinal Statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    # Save brain state for later use
    brain.save("basic_brain_checkpoint.bin")
    print(f"✓ Brain state saved to basic_brain_checkpoint.bin")

if __name__ == "__main__":
    basic_brain_simulation()