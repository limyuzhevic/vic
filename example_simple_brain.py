#!/usr/bin/env python3
"""
NLM Example: Simple Brain Simulation
=====================================

This is the simplest example demonstrating how to create and use a basic NLM brain.

Expected output:
- Brain created with 1000 neurons
- Brain initialized successfully
- Simulation runs for 50 steps
- Shows firing activity over time
"""

import pynlm

def main():
    # Create brain
    print("Creating NLM brain...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Initialize brain
    print("Initializing brain...")
    brain.initialize()
    
    print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation
    print("\nRunning simulation...")
    for i in range(50):
        brain.step(i)
        
        if i % 10 == 0:
            print(f"  Step {i}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"\n✓ Simulation complete!")
    print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
