#!/usr/bin/env python3
"""Simple demo script for NLM."""

import sys
import pynlm

def main():
    print("NLM Demo - Neural Learning Machine")
    print("=" * 50)
    
    # Show version
    info = pynlm.getVersionInfo()
    print(f"Version: {info['version']}")
    print(f"Author: {info['author']}")
    print()
    
    # Create brain
    print("Creating brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Total synapses: {brain.getTotalSynapseCount()}")
    print()
    
    # Run simulation
    print("Running simulation for 50 steps...")
    for step in range(50):
        brain.step(step)
        
        if step % 10 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print(f"✓ Simulation complete!")
    print(f"  Final spike count: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print()
    
    print("Demo completed successfully!")
    return True

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)