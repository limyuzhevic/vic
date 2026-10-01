#!/usr/bin/env python3
"""
Example 1: Basic Brain Creation and Initialization
=================================================

This example demonstrates the simplest way to create and initialize a brain
using the NLM Python API.
"""

import pynlm
import time

def basic_brain_example():
    """Create a basic brain and run a simple simulation."""
    print("=== Basic Brain Creation Example ===")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    
    # Optionally customize the configuration
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.15)
    
    # Create and initialize the brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run a simulation
    for step in range(100):
        brain.step(step)
        
        # Log statistics periodically
        if step % 25 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print("Simulation completed successfully!")
    return brain

if __name__ == "__main__":
    basic_brain_example()