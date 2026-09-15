#!/usr/bin/env python3
"""
Example: Running a Simple NLM Brain Simulation

This example demonstrates the basic usage of the NLM (Neural Learning Machine)
Python bindings to create and run a simple neural brain simulation.

The NLM brain is an artificial developmental system that:
- Contains thousands of spiking neurons
- Learns through synaptic plasticity
- Develops over time
- Can interact with environments

Expected output: "Simple brain test completed successfully!"
"""

import pynlm

def main():
    print("=== NLM Simple Brain Example ===")
    print("Creating and running a basic neural brain simulation...")
    
    # Step 1: Create a brain with default configuration
    print("\n1. Creating brain with default configuration...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Step 2: Initialize the brain
    print("2. Initializing brain...")
    if not brain.initialize():
        print("ERROR: Failed to initialize brain!")
        return 1
    print(f"   ✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Step 3: Run simulation steps
    print("\n3. Running simulation...")
    num_steps = 100
    for step in range(num_steps):
        brain.step(step)
        
        # Print progress every 20 steps
        if step % 20 == 0:
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getFiringNeuronCount()
            print(f"   Step {step:3d}: {active_neurons:4d} firing, {firing_rate:.2f} Hz")
    
    # Step 4: Display results
    print("\n4. Simulation Results:")
    print(f"   Total steps: {num_steps}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   Final firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Step 5: Verify success
    if brain.getTotalSpikeCount() > 0 and brain.getAverageFiringRate() > 0:
        print("\n" + "="*50)
        print("✓ SIMPLE BRAIN TEST COMPLETED SUCCESSFULLY!")
        print("="*50)
        return 0
    else:
        print("\n⚠ WARNING: Brain showed minimal activity")
        return 0

if __name__ == "__main__":
    exit(main())