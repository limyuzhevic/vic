#!/usr/bin/env python3
"""
Basic Brain Simulation Example

This example demonstrates the enhanced NLM Python bindings with context managers,
property-based access, and error handling.
"""

import pynlm_enhanced as nlm
import time

def main():
    print("=== Basic Brain Simulation (Enhanced Bindings) ===\n")
    
    # Example 1: Basic brain simulation with context manager
    print("1. Creating enhanced brain with context manager...")
    
    # The enhanced brain automatically manages resources
    with nlm.create_enhanced_brain() as brain:
        print(f"   Brain created with {brain.get_total_neuron_count()} neurons")
        
        # Initialize the brain
        print("   Initializing brain...")
        brain.initialize()
        
        # Run simulation
        print("   Running simulation...")
        start_time = time.time()
        
        for step in range(100):
            brain.step(step)
            
            # Print progress every 20 steps
            if step % 20 == 0:
                print(f"     Step {step}: {brain.get_firing_neuron_count()} neurons firing, "
                      f"spikes: {brain.get_total_spike_count()}")
        
        elapsed = time.time() - start_time
        print(f"   Simulation completed in {elapsed:.3f} seconds")
        print(f"   Total neurons: {brain.get_total_neuron_count()}")
        print(f"   Total synapses: {brain.get_total_synapse_count()}")
        print(f"   Total spikes: {brain.get_total_spike_count()}")
        print(f"   Average firing rate: {brain.get_average_firing_rate():.3f}")
        print(f"   Developmental stage: {brain.get_developmental_stage()}")
    
    print("\n2. Testing error handling...")
    
    # Test error handling with uninitialized brain
    try:
        brain = nlm.create_enhanced_brain()
        # Don't initialize - this should cause an error when stepping
        brain.step(0)
        print("   ERROR: Should have thrown SimulationError")
    except nlm.SimulationError as e:
        print(f"   ✓ Caught SimulationError as expected: {e}")
    
    print("\n3. Testing save/load functionality...")
    
    # Create and initialize a new brain
    brain = nlm.create_enhanced_brain()
    brain.initialize()
    
    # Run a few steps
    for step in range(10):
        brain.step(step)
    
    # Save brain state
    try:
        brain.save("test_brain.bin")
        print("   ✓ Brain saved successfully")
        
        # Create new brain and load state
        brain2 = nlm.create_enhanced_brain()
        brain2.load("test_brain.bin")
        print("   ✓ Brain loaded successfully")
        print(f"   Loaded brain has {brain2.get_total_spike_count()} spikes (expected: {brain.get_total_spike_count()})")
        
        # Clean up
        import os
        os.remove("test_brain.bin")
        
    except nlm.SimulationError as e:
        print(f"   ✗ Save/load failed: {e}")
    
    print("\n=== Example completed successfully! ===")

if __name__ == "__main__":
    main()
