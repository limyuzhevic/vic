#!/usr/bin/env python3
"""
Example 1: Simplest Brain - Just Neurons

This is the absolute minimum example to get NLM working.
It creates a brain with default settings and lets it "think".
"""

import sys
import pynlm

def main():
    print("=" * 50)
    print("Example 1: Simplest Brain - Just Neurons")
    print("=" * 50)
    
    try:
        # One line to create a virtual brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        # Initialize it (always call this before using the brain!)
        brain.initialize()
        
        # Make it active for 10 steps
        for i in range(10):
            brain.step(i)
        
        print("✓ Silent brain test done!")
        print(f"  - Brain has {brain.getTotalNeuronCount()} neurons")
        print(f"  - Total spikes generated: {brain.getTotalSpikeCount()}")
        print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  - Development stage: {brain.getDevelopmentalStage()}")
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return 1
    
    print("=" * 50)
    return 0
if __name__ == "__main__":
    sys.exit(main())