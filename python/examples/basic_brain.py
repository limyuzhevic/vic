#!/usr/bin/env python3
"""
NLM Basic Brain Example

This example demonstrates the fundamental usage of NLM Python bindings
for creating a brain and running basic simulation steps.
"""

import pynlm

def main():
    print("=== Basic Brain Example ===")
    
    # Step 1: Create a brain
    print("1. Creating a brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Step 2: Initialize the brain
    print("2. Initializing the brain...")
    brain.initialize()
    
    # Step 3: Run 100 steps
    print("3. Running 100 simulation steps...")
    for i in range(100):
        brain.step(i)
    
    # Step 4: Print statistics
    print("4. Brain statistics:")
    print(f"   - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   - Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    print("=== Complete ===")

if __name__ == "__main__":
    main()