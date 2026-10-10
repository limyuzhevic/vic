#!/usr/bin/env python3

"""
Example: Minimal NLM Simulation

This script demonstrates the simplest possible NLM usage.
It creates a virtual brain and runs it for a few steps.
"""

import pynlm

def main():
    print("=== Minimal NLM Example ===")
    print("Creating a virtual brain...")
    
    # Step 1: Create a brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Expected synapses: {brain.getTotalSynapseCount()}")
    
    # Step 2: Initialize the brain
    print("Initializing brain...")
    brain.initialize()
    
    print("Starting simulation...")
    
    # Step 3: Run the brain for a few steps
    for step in range(10):
        brain.step(step)
        
        # Log progress every 2 steps
        if step % 2 == 0:
            print(f"Step {step:2d}: {brain.getFiringNeuronCount():3d} neurons firing, "
                  f"{brain.getTotalSpikeCount():4d} total spikes")
    
    print("\n=== Example Complete ===")
    print(f"Final statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    print("\nNext steps:")
    print("  - Try the agent example: python examples/agent_example.py")
    print("  - Run a benchmark: python examples/benchmark.py")
    print("  - Explore the complete agent simulation: python examples/complete_agent.py")

if __name__ == "__main__":
    main()