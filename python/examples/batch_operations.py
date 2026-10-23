#!/usr/bin/env python3
"""
NLM Batch Operations Example

This example demonstrates batch operations for multiple simulations.
"""

import pynlm

def main():
    print("=== Batch Operations Example ===")
    
    # Create multiple brains
    print("1. Creating multiple brains...")
    brains = []
    for i in range(3):
        config = pynlm.create_default_config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
    
    # Run batch simulation
    print("2. Running batch simulation...")
    with pynlm.batch_simulation(brains, stepsPerBrain=100) as simulator:
        results = simulator.run()
    
    print("3. Batch results:")
    for i, spikes in enumerate(results):
        print(f"   - Brain {i}: {spikes} spikes")
    
    print("=== Complete ===")

if __name__ == "__main__":
    main()