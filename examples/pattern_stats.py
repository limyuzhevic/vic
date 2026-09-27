#!/usr/bin/env python3
"""Pattern 2: Get Brain Statistics

This demonstrates how to monitor and track brain statistics during simulation.
"""

import pynlm

def get_brain_stats(brain):
    """Get comprehensive statistics from a brain."""
    return {
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'development_stage': brain.getDevelopmentalStage().name,
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'e_i_ratio': brain.getExcitationInhibitionRatio(),
        'pending_spikes': brain.getPendingSpikeEventCount(),
        'active_neurons': brain.getActiveNeuronCount(),
    }

def example_get_stats():
    """Example of using brain statistics."""
    print("=== Brain Statistics Pattern ===")
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Just add these inside your loop:
    for step in range(50):
        brain.step(step)
        
        # Get stats
        stats = get_brain_stats(brain)
        
        print(f"Step {step}:")
        print(f"  Neurons firing: {stats['firing_neurons']}")
        print(f"  Total spikes: {stats['total_spikes']}")
        print(f"  Avg firing rate: {stats['average_firing_rate']:.3f}")
        print(f"  Dev Stage: {stats['development_stage']}")

if __name__ == "__main__":
    example_get_stats()