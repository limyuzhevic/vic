#!/usr/bin/env python3
"""
NLM Python Example: Basic Brain Simulation

This is the simplest possible example to get started with NLM.
"""

import pynlm
import time

def basic_brain_example():
    """Create and run a basic brain simulation"""
    print("=== NLM Basic Brain Example ===")
    
    # Step 1: Create a default configuration
    config = pynlm.createDefaultConfig()
    print(f"Created default config")
    
    # Step 2: Create the brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Step 3: Run simulation
    print("Running simulation...")
    for i in range(100):
        brain.step(i)
        if i % 20 == 0:
            print(f"  Step {i}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    # Step 4: Check results
    print(f"\nSimulation Results:")
    print(f"  Total Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total Spikes: {brain.getTotalSpikeCount()}")
    print(f"  Current Firing Rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Active Neurons: {brain.getFiringNeuronCount()}")
    print(f"  E/I Ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"  Developmental Stage: {brain.getDevelopmentalStage()}")
    
    return brain

if __name__ == "__main__":
    try:
        brain = basic_brain_example()
        print("\n✓ Example completed successfully!")
    except Exception as e:
        print(f"\n✗ Error: {e}")
        import traceback
        traceback.print_exc()
