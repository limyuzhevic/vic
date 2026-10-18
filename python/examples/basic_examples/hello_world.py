"""
Hello World - Basic NLM Python Example
=====================================

This is the simplest possible example demonstrating NLM Python API usage.
It shows how to create a brain and initialize it.

Expected Output:
---------------
Hello, NLM World!
Created brain with 500 neurons
"""

import pynlm

def main():
    print("Hello, NLM World!")
    
    # Create a default brain with standard settings (Phase 2)
    brain = pynlm.createDefaultBrain()
    brain.initialize()
    
    # Display brain statistics
    total_neurons = brain.getTotalNeuronCount()
    print(f"Created brain with {total_neurons} neurons")
    
    # Run a few steps to demonstrate basic functionality
    for i in range(5):
        brain.step(i)
        firing = brain.getFiringNeuronCount()
        print(f"Step {i}: {firing} neurons firing")

if __name__ == "__main__":
    main()