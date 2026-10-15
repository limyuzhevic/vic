#!/usr/bin/env python3

"""
Example 6: Developmental Processes

Demonstrates how developmental processes change over time.
This example shows age-dependent development.
"""

import sys
import os

# Add parent directory to path for NLM imports
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import pynlm
import json
from pathlib import Path

def main():
    print("=== NLM Example 6: Developmental Processes ===\n")
    
    # Create brain
    print("1. Creating developmental brain...")
    brain = pynlm.create_brain()
    brain.initialize()
    
    print(f"   Created developmental brain")
    print(f"   Initial developmental stage: {brain.get_developmental_stage()}")
    
    # Simulate development over time
    print(f"\n2. Simulating development over time...")
    
    developmental_stages = []
    developmental_ages = []
    
    for timestep in range(0, 1000, 100):
        # Get current developmental stage
        stage = brain.get_developmental_stage()
        developmental_stages.append(str(stage))
        
        # Get developmental age (simulation time)
        developmental_ages.append(timestep)
        
        # Simulate some development changes
        if timestep % 300 == 0:
            print(f"\n   Development checkpoint at timestep {timestep}:")
            print(f"     Stage: {stage}")
            print(f"     Neurons: {brain.get_total_neuron_count()}")
            print(f"     Synapses: {brain.get_total_synapse_count()}")
        
        # Run brain step with development
        brain.step(timestep, timestep * 0.001)
        
        # Apply some neuromodulation to affect development
        brain.inject_current_to_neurons(pynlm.constants.NEURON_TYPES.Modulatory, 5.0f)
    
    # Get final developmental status
    print(f"\n3. Collecting final developmental status...")
    
    final_stage = brain.get_developmental_stage()
    final_plasticity = brain.get_config().get_or("plasticity.structural.enable", True)
    
    print(f"\n4. Developmental complete:")
    print(f"   Final developmental stage: {final_stage}")
    print(f"   Structural plasticity enabled: {final_plasticity}")
    print(f"   Final neuron count: {brain.get_total_neuron_count()}")
    print(f"   Final synapse count: {brain.get_total_synapse_count()}")
    
    # Save results
    print(f"\n5. Saving developmental results...")
    
    results = {
        "example": "developmental",
        "developmental_stages": developmental_stages,
        "developmental_ages": developmental_ages,
        "final_stage": str(final_stage),
        "final_plasticity": final_plasticity,
        "final_neuron_count": brain.get_total_neuron_count(),
        "final_synapse_count": brain.get_total_synapse_count()
    }
    
    results_file = Path("results") / "developmental_results.json"
    with open(results_file, "w") as f:
        json.dump(results, f, indent=2)
    
    print(f"   Results saved to: {results_file}")
    
    print(f"\n✓ Example completed successfully!")
    return True

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\n✗ Error: {e}")
        sys.exit(1)