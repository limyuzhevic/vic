#!/usr/bin/env python3
# Simple Brain Example
# Demonstrates basic NLM brain creation and usage

import sys
sys.path.insert(0, '../')

import pynlm as nlm
import time

def main():
    print("=== NLM Simple Brain Example ===")
    print("This example shows how to create and run a basic neural simulation.\n")
    
    # Create configuration
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 100)
    config.set("simulation_timestep", 0.001)
    
    print("Configuration:")
    print(f"  Neuron count: {config.getOr('neuron_count', 1000)}")
    print(f"  Simulation timestep: {config.getOr('simulation_timestep', 0.001)}s")
    
    # Create brain
    print("\nCreating brain...")
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print(f"  Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation
    print("\nRunning simulation...")
    start_time = time.time()
    
    for step in range(5):
        brain.step(step)
        print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    end_time = time.time()
    
    print(f"\nSimulation completed in {end_time - start_time:.3f} seconds")
    print("\nFinal statistics:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons (this step): {brain.getFiringNeuronCount()}")
    
    print("\nExample completed successfully!")

if __name__ == "__main__":
    main()
