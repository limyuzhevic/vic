#!/usr/bin/env python3

"""
Example 1: Basic Brain Operations

Demonstrates basic brain creation, initialization, and simulation.
This is the simplest example to get started with NLM.
"""

import sys
import os

# Add parent directory to path for NLM imports
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import pynlm
import json
from pathlib import Path

def main():
    print("=== NLM Example 1: Basic Brain Operations ===\n")
    
    # Create brain
    print("1. Creating brain with default configuration...")
    brain = pynlm.create_brain()
    brain.initialize()
    
    print(f"   Created brain with {brain.get_total_neuron_count()} neurons")
    print(f"   and {brain.get_total_synapse_count()} synapses")
    
    # Run simulation
    print("\n2. Running simulation for 100 steps...")
    for step in range(100):
        brain.step(step)
    
    # Get statistics
    print("\n3. Collecting statistics...")
    spikes = brain.get_total_spike_count()
    firing = brain.get_firing_neuron_count()
    rate = brain.get_average_firing_rate()
    e_i_ratio = brain.get_excitation_inhibition_ratio()
    
    print(f"\n   Simulation Results:")
    print(f"     Total spikes: {spikes}")
    print(f"     Firing neurons: {firing}")
    print(f"     Average firing rate: {rate:.2f}")
    print(f"     Excitation/Inhibition ratio: {e_i_ratio:.3f}")
    
    # Save brain state
    print("\n4. Saving brain state...")
    brain_file = Path("results") / "basic_brain.bin"
    brain.save(str(brain_file))
    print(f"   Brain saved to: {brain_file}")
    
    # Save results
    results = {
        "example": "basic_brain",
        "neurons": brain.get_total_neuron_count(),
        "synapses": brain.get_total_synapse_count(),
        "total_spikes": spikes,
        "firing_neurons": firing,
        "average_firing_rate": rate,
        "excitation_inhibition_ratio": e_i_ratio
    }
    
    results_file = Path("results") / "basic_brain_results.json"
    with open(results_file, "w") as f:
        json.dump(results, f, indent=2)
    
    print(f"\n   Results saved to: {results_file}")
    
    print(f"\n✓ Example completed successfully!")
    return True

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\n✗ Error: {e}")
        sys.exit(1)