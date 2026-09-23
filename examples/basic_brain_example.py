#!/usr/bin/env python3
"""
NLM Example 1: Basic Brain Initialization

This script demonstrates the most basic NLM usage pattern: creating a brain,
initializing it, and running a simple simulation.

Key concepts covered:
- Creating default configuration
- Brain initialization
- Basic simulation steps
- Statistics collection
"""
import pynlm
import sys

def main():
    """Create and run a basic brain simulation"""
    print("=== Basic Brain Example ===")
    
    # 1. Create brain with default configuration
    print("1. Creating brain with default configuration...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # 2. Initialize the brain
    print("2. Initializing brain...")
    if not brain.initialize():
        print("ERROR: Brain initialization failed!")
        return False
    print(f"   - Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # 3. Run simulation steps
    print("3. Running simulation steps...")
    num_steps = 100
    for step in range(num_steps):
        brain.step(step)
        
        # Print statistics every 20 steps
        if step % 20 == 0:
            print(f"   Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    # 4. Print final statistics
    print("4. Final statistics:")
    print(f"   - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   - Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   - Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"   - Developmental stage: {brain.getDevelopmentalStage()}")
    
    return True

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)