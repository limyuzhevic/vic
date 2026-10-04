#!/usr/bin/env python3
"""
Example 01: Basic Brain - Simplest possible brain usage

This is the most basic example for getting started with pynlm.
It demonstrates creating a brain and running a simple simulation.

Author: NLM Project
Date: 2026
Version: Wave 5 Enhanced
"""

import pynlm

def main():
    print("=== NLM Python Bindings - Basic Brain Example ===")
    print()
    print("This example demonstrates the simplest possible brain creation and simulation.")
    print()
    
    # Create a brain with default settings (using simplified beginner API)
    print("1. Creating brain with default configuration...")
    brain = pynlm.beginner_createBrain()
    print("   ✓ Brain created using beginner_createBrain()")
    
    # Initialize the brain
    print("2. Initializing brain...")
    brain.initialize()
    print("   ✓ Brain initialized")
    
    # Get brain statistics
    print("3. Brain statistics...")
    total_neurons = brain.getTotalNeuronCount()
    print(f"   - Total neurons: {total_neurons}")
    
    # Run a single simulation step
    print("4. Running simulation step...")
    brain.step(0)
    print("   ✓ Step completed")
    
    # Get spike statistics
    print("5. Simulation results...")
    total_spikes = brain.getTotalSpikeCount()
    print(f"   - Total spikes: {total_spikes}")
    
    print()
    print("=== Example Complete ===")
    print()
    print("This brain example shows the essential steps for working with pynlm:")
    print("- Create brain with beginner-friendly API")
    print("- Initialize the brain")
    print("- Run simulation steps")
    print("- Get statistics and results")
    print()
    print("For more complex examples, see:")
    print("- examples/02_agent_simulation.py - Agent with world interaction")
    print("- examples/03_advanced_features.py - Advanced features and learning")

if __name__ == "__main__":
    main()