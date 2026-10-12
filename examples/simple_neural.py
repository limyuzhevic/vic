#!/usr/bin/env python3
"""
NLM Example 1: Simple Neural Network - "Hello World"

This is the simplest possible NLM example, demonstrating:
- How to create a brain
- How to initialize it
- How to run basic neural operations
- How to interpret the results

Learning Objectives:
- Understand the basic NLM workflow
- Learn how to create and configure a brain
- See what neural activity looks like
- Practice with the Python API

Time Complexity: O(n_steps × n_neurons) where n_steps is typically 1-100

Expected Output:
```
=== Simple Neural Network Example ===
Created brain with 500 neurons and 2500 synapses
Initialized brain successfully
Step 0: 0 neurons firing
Step 1: 0 neurons firing
Step 2: 2 neurons firing
Step 3: 5 neurons firing
...
Final firing rate: 3.14%
=== Example Complete ===
```

Tips for Customization:
- Change neuron_count for different brain sizes
- Increase steps for more activity
- Add sensory input to make neurons fire
- Connect to a world for interaction

This example is inspired by the NLM's biological inspiration: a simple
neural network that begins with primitive connections and develops
through activity.
"""

import pynlm
import time

def main():
    print("=== Simple Neural Network Example ===")
    print("This example shows the basic workflow of creating and running")
    print("a simple neural network using NLM's Leaky Integrate-and-Fire")
    print("neuron model.\n")
    
    # Step 1: Create configuration
    print("Step 1: Creating configuration...")
    config = pynlm.createDefaultConfig()
    
    # Customize for this simple example
    config.set("brain.neuron_count", 100)  # Small brain for quick demo
    config.set("brain.synapse_density", 0.05)  # Sparse connections
    config.set("random_seed", 42)  # Reproducible results
    
    # Step 2: Create and initialize brain
    print("Step 2: Creating and initializing brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"and {brain.getTotalSynapseCount()} synapses")
    
    # Step 3: Run simulation
    print("\nStep 3: Running simulation...")
    print("Note: Neurons need input to fire. This brain is mostly silent")
    print("because no external input is provided.\n")
    
    # Run for 20 steps to see development
    for step in range(20):
        brain.step(step)
        
        if step % 5 == 0:  # Log every 5 steps
            firing = brain.getFiringNeuronCount()
            total = brain.getTotalNeuronCount()
            rate = (firing / total) * 100 if total > 0 else 0
            print(f"Step {step}: {firing}/{total} neurons firing ({rate:.1f}%)")
    
    # Step 4: Analyze results
    print("\nStep 4: Analyzing results...")
    total_spikes = brain.getTotalSpikeCount()
    firing_rate = brain.getAverageFiringRate()
    neurons_firing = brain.getFiringNeuronCount()
    
    print(f"Total spikes generated: {total_spikes}")
    print(f"Average firing rate: {firing_rate:.2f} Hz")
    print(f"Neurons currently firing: {neurons_firing}/{brain.getTotalNeuronCount()}")
    
    # Step 5: Explanation
    print("\n=== Understanding the Results ===")
    print("This brain is mostly silent because:")
    print("1. No external sensory input is provided")
    print("2. No motor commands are sent")
    print("3. The brain needs stimulation to generate activity")
    print("\nNext steps would involve:")
    print("- Adding sensory input (vision, touch, etc.)")
    print("- Creating a world environment")
    print("- Adding an agent to interact with the world")
    print("- Enabling neuromodulation and learning")
    
    print("\n=== Example Complete ===")
    print("You now understand the basic NLM workflow!")
    print("Next: Try examples/agent_control.py for a complete agent.")

if __name__ == "__main__":
    start_time = time.time()
    main()
    end_time = time.time()
    print(f"\nExecution time: {end_time - start_time:.2f} seconds")
