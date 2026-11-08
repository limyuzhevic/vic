#!/usr/bin/env python3
"""
Example demonstrating how to create and use a simple NLM brain simulation.

This example shows the fundamental steps for creating and running a basic
NLM brain simulation using the pynlm Python package.
"""

import pynlm
from pynlm import createBrain, createDefaultConfig
def basic_brain_example(neuron_count=1000, steps=1000, plasticity_enabled=True, development_enabled=False):
    """
    Create a basic NLM brain and run a simple simulation.
    
    Args:
        neuron_count: Number of neurons to create (default: 1000)
        steps: Number of simulation steps to run (default: 1000)
        plasticity_enabled: Whether to enable plasticity mechanisms (default: True)
        development_enabled: Whether to enable development system (default: False)
    
    Returns:
        Dictionary containing simulation statistics
    """
    # Step 1: Create configuration
    config = createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.1)
    config.set("plasticity.stdp.enable", plasticity_enabled)
    config.set("plasticity.hebbian.enable", plasticity_enabled)
    config.set("development.enabled", development_enabled)
    config.set("neuromod.curiosity.enable", development_enabled)
    
    # Step 2: Create and initialize brain
    brain = createBrain(config)
    brain.initialize()
    
    # Step 3: Run simulation
    for step in range(steps):
        brain.step(step)
        
        # Log progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    # Step 4: Collect statistics
    stats = {
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'development_stage': brain.getDevelopmentalStage(),
        'total_neuron_count': brain.getTotalNeuronCount(),
        'total_synapse_count': brain.getTotalSynapseCount(),
        'active_neuron_count': brain.getActiveNeuronCount(),
        'neuron_count': neuron_count,
        'steps': steps,
        'plasticity_enabled': plasticity_enabled,
        'development_enabled': development_enabled
    }
    
    print(f"\nBasic brain example completed:")
    print(f"  Total spikes: {stats['total_spikes']}")
    print(f"  Neurons: {stats['neuron_count']}")
    print(f"  Steps: {stats['steps']}")
    print(f"  Average firing rate: {stats['avg_firing_rate']:.3f} Hz")
    print(f"  E/I ratio: {stats['excitation_inhibition_ratio']:.3f}")
    print(f"  Development stage: {stats['development_stage']}")
    
    return stats
if __name__ == "__main__":
    # Run example with default parameters
    stats = basic_brain_example()
    
    # Example with custom parameters
    custom_stats = basic_brain_example(
        neuron_count=500,
        steps=500,
        plasticity_enabled=True,
        development_enabled=False
    )
    
    print("\nBasic brain example completed successfully!")