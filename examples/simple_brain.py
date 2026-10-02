#!/usr/bin/env python3
"""
Simple NLM brain example demonstrating basic neural simulation.

This example shows how to create and run a basic NLM brain simulation
with minimal configuration.
"""

import pynlm

def main():
    """Run a simple NLM simulation."""
    print("NLM Simple Brain Example")
    print("=" * 50)
    
    # Create brain with default configuration
    config = pynlm.create_default_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation for 100 steps
    print("\nRunning simulation...")
    for step in range(100):
        brain.step(step)
        
        if step % 20 == 0:
            firing = brain.getFiringNeuronCount()
            print(f"Step {step}: {firing} neurons firing")
    
    print(f"\nFinal state:")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    # Save brain state
    brain.save("simple_brain_checkpoint.bin")
    print(f"\nSaved brain state to 'simple_brain_checkpoint.bin'")
    
    print("\nExample completed successfully!")

if __name__ == "__main__":
    main()
