#!/usr/bin/env python3
"""
NLM (熙然) Brain Simulation - Basic Usage Examples
================================================

This file contains comprehensive examples demonstrating how to use the NLM
brain simulation framework. It covers basic to advanced usage patterns
for both the Python bindings (pynlm) and command-line tools.

Examples are organized by complexity and use case:

1. Basic Examples - Getting started with minimal code
2. Simulation Examples - Complete simulation workflows
3. Experiment Examples - Running NLM experiments
4. Advanced Examples - Complex scenarios and advanced features
5. Integration Examples - Combining different components
6. Debugging Examples - Troubleshooting and monitoring

Each example includes:
- Complete code with clear comments
- Expected output and behavior
- Common pitfalls and solutions
- Performance considerations

Usage:
    python3 examples/basic_example.py
    python3 examples/simulation_example.py
    # ... etc
"""

"""
Module: Basic NLM Usage
=======================

This example demonstrates the absolute minimum required to create and run
a simple brain simulation with NLM.

Key concepts introduced:
- Brain creation and initialization
- Configuration management
- Step-by-step simulation
- State management

Expected output will vary but should show:
- Brain initialization success
- Progress through simulation steps
- Basic statistics about neuron activity

Time complexity: O(num_steps × neuron_count)
Space complexity: O(neuron_count × connectivity)
"""

import pynlm as nlm

def basic_example():
    """
    Basic example: Create and run a simple brain simulation.
    
    This demonstrates the minimal code needed to:
    1. Create a brain with default configuration
    2. Initialize the brain
    3. Run a few simulation steps
    4. Print basic statistics
    """
    print("=== Basic NLM Example ===\n")
    
    # Step 1: Create a brain with default configuration
    print("1. Creating brain with default configuration...")
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    
    # Step 2: Initialize the brain
    print("2. Initializing brain...")
    brain.initialize()
    
    # Step 3: Run a few simulation steps
    print("3. Running simulation (100 steps)...")
    for step in range(100):
        brain.step(step)
    
    # Step 4: Print statistics
    print("4. Brain statistics:")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    
    print("\n✓ Basic example completed successfully!")

if __name__ == "__main__":
    basic_example()