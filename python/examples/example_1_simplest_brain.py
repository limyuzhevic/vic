"""
Example 1: Simplest Brain Simulation

This is the most basic NLM example - creating a brain and running it.
This example matches the "Simplest Brain" example from the documentation.
"""

import pynlm as nlm

def main():
    print("=== Example 1: Simplest Brain Simulation ===\n")
    
    # Step 1: Make a brain
    print("1. Creating brain with default configuration...")
    brain = nlm.createBrain(nlm.createDefaultConfig())
    
    # Step 2: Turn it on
    print("2. Initializing brain...")
    brain.initialize()
    
    # Step 3: Make it think for 100 steps
    print("3. Running brain simulation (100 steps)...")
    for i in range(100):
        brain.step(i)  # One step of thinking
    
    # Step 4: Get results
    print("4. Displaying results...")
    print(f"   Done! Your brain thought {brain.getTotalSpikeCount()} times")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Current firing rate: {brain.getAverageFiringRate():.2f}")
    
    print("\n=== Example 1 Complete ===\n")

if __name__ == "__main__":
    main()
