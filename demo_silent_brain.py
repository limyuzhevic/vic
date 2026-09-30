#!/usr/bin/env python3
"""
NLM Demo 1: Silent Brain

This demo shows the simplest possible example of creating and using a virtual brain
without any sensory input or motor output. The brain will generate its own internal
activity through neural dynamics.

This is perfect for understanding how neurons and synapses work in isolation.
"""

import pynlm
import time

def main():
    print("=== NLM Silent Brain Demo ===")
    print("This demo creates a virtual brain that thinks on its own.\n")
    
    # Create a simple brain
    print("1. Creating brain with default configuration...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Initialize the brain
    print("2. Initializing brain...")
    brain.initialize()
    
    # Display brain statistics
    print(f"3. Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   Default configuration used\n")
    
    # Run silent thinking
    print("4. Running silent brain for 10 steps...")
    start_time = time.time()
    
    for step in range(10):
        brain.step(step)
        
        if step % 5 == 0:
            firing = brain.getFiringNeuronCount()
            print(f"   Step {step}: {firing} neurons firing")
    
    end_time = time.time()
    
    # Final statistics
    print(f"\n5. Silent brain test complete!")
    print(f"   Simulation took {end_time - start_time:.3f} seconds")
    print(f"   Final firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"   Total spikes generated: {brain.getTotalSpikeCount()}")
    
    print("\n=== Silent Brain Demo Complete ===")
    print("\nWhat you learned:")
    print("- How to create a basic virtual brain")
    print("- How neurons can fire on their own")
    print("- Basic brain statistics and metrics")

if __name__ == "__main__":
    main()