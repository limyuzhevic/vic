#!/usr/bin/env python3
"""
NLM Quick Start Example

This is the simplest example that demonstrates the essential usage
of NLM Python bindings.
"""

import pynlm

def quick_start():
    """Quick start demonstration"""
    print("=== NLM Quick Start ===")
    
    # Step 1: Create brain with default configuration
    print("1. Creating brain with default configuration...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Step 2: Initialize the brain
    print("2. Initializing brain...")
    brain.initialize()
    
    # Step 3: Run some simulation steps
    print("3. Running 10 simulation steps...")
    for i in range(10):
        brain.step(i)
    
    # Step 4: Show results
    print("4. Results:")
    print(f"   - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   - Spike count: {brain.getTotalSpikeCount()}")
    
    print("=== Quick Start Complete ===")

if __name__ == "__main__":
    quick_start()