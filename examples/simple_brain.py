#!/usr/bin/env python3
"""
Simple Brain Simulation Example
===========================
This example demonstrates basic NLM brain simulation functionality.
It's designed to be the simplest possible introduction to working with NLM.

Key Features:
- Creates a basic brain with default configuration
- Runs a simple simulation loop
- Shows basic brain statistics
- Demonstrates basic input/output operations
"""

import sys
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def main():
    print("=" * 70)
    print("NLM Simple Brain Simulation Example")
    print("=" * 70)
    
    # 1. Create a basic brain with default configuration
    print("\n1. Creating brain with default configuration...")
    config = pynlm.createDefaultConfig()
    print(f"   Configuration created with {config.getKeys()} parameters")
    
    # 2. Create and initialize the brain
    print("\n2. Creating and initializing brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"   Brain created: {brain.getTotalNeuronCount()} neurons")
    print(f"   Brain created: {brain.getTotalSynapseCount()} synapses")
    
    # 3. Run a simple simulation
    print("\n3. Running simple simulation...")
    num_steps = 100
    spike_counts = []
    
    for step in range(num_steps):
        brain.step(step)
        firing = brain.getFiringNeuronCount()
        spike_counts.append(firing)
        
        if step % 20 == 0:
            print(f"   Step {step}: {firing} firing neurons")
    
    # 4. Show basic statistics
    print("\n4. Simulation Results:")
    print(f"   Total simulation steps: {num_steps}")
    print(f"   Average firing neurons: {sum(spike_counts)/len(spike_counts):.1f}")
    print(f"   Maximum firing neurons: {max(spike_counts)}")
    print(f"   Total spikes across simulation: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"   Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # 5. Demonstrate brain reset
    print("\n5. Testing brain reset...")
    brain.reset()
    print(f"   After reset - Neurons: {brain.getTotalNeuronCount()}")
    print(f"   After reset - Synapses: {brain.getTotalSynapseCount()}")
    
    # 6. Final statistics
    print("\n6. Final Brain State:")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   Active neurons: {brain.getActiveNeuronCount()}")
    print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"   Developmental stage: {brain.getDevelopmentalStage()}")
    
    print("\n" + "=" * 70)
    print("Simple Brain Simulation Complete")
    print("=" * 70)
    print("\nNext Steps:")
    print("  - Try agent_world.py for environment interaction")
    print("  - Try advanced_config.py for configuration examples")
    print("  - Try plasticity_experiment.py for learning experiments")
    print("  - Try neuromodulation_study.py for neuromodulation analysis")
    print("\nFor more examples, see the documentation in how_to_use.md")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\nError: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)