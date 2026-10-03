#!/usr/bin/env python3
"""
Example 1: Simplest Brain

This is the most basic example of using NLM. It creates a brain,
initializes it, and runs a few steps to show how the brain works.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def main():
    print("=== Example 1: Simplest Brain ===\n")
    
    # Step 1: Make a brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    print("✓ Created brain")
    
    # Step 2: Turn it on
    brain.initialize()
    print("✓ Initialized brain")
    
    # Step 3: Make it think for 100 steps
    for i in range(100):
        brain.step(i)  # One step of thinking
        if i % 20 == 0:
            print(f"  Step {i}")
    
    print(f"✓ Done! Your brain thought {brain.getTotalSpikeCount()} times")
    print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}")
    print()

if __name__ == "__main__":
    main()
