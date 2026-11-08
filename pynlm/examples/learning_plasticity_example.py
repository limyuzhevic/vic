#!/usr/bin/env python3
"""
Learning and plasticity example demonstrating STDP, Hebbian, and structural plasticity.

This example demonstrates how different plasticity mechanisms work in NLM brains,
showing synaptic weight changes and learning patterns.
"""

import pynlm
from pynlm import createBrain, createDefaultConfig
import numpy as np
def learning_plasticity_example(steps=2000, show_detailed_stats=True):
    """
    Create a brain and run a learning simulation demonstrating plasticity mechanisms.
    
    Args:
        steps: Number of simulation steps (default: 2000)
        show_detailed_stats: Whether to show detailed learning statistics (default: True)
    
    Returns:
        Dictionary containing learning and plasticity statistics
    """
    # Step 1: Create configuration optimized for learning
    config = createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("brain.synapse_density", 0.12)
    
    # Enable all plasticity mechanisms
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.stdp.tau_plus", 20.0)
    config.set("plasticity.stdp.tau_minus", 20.0)
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.01)
    
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0001)
    config.set("plasticity.structural.pruning_rate", 0.00001)
    
    config.set("development.enabled", True)
    config.set("development.initial_plasticity", 1.0)
    config.set("development.plasticity_decay_rate", 0.01)
    
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    
    # Step 2: Create and initialize brain
    brain = createBrain(config)
    brain.initialize()
    
    # Record initial state
    initial_stats = {
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'development_stage': brain.getDevelopmentalStage(),
        'total_neuron_count': brain.getTotalNeuronCount(),
        'total_synapse_count': brain.getTotalSynapseCount(),
    }
    
    if show_detailed_stats:
        print("Learning and Plasticity Example")
        print("================================")
        print(f"Initial state:")
        print(f"  Total spikes: {initial_stats['total_spikes']}")
        print(f"  Firing neurons: {initial_stats['firing_neurons']}")
        print(f"  Average firing rate: {initial_stats['avg_firing_rate']:.3f} Hz")
        print(f"  E/I ratio: {initial_stats['excitation_inhibition_ratio']:.3f}")
        print(f"  Development stage: {initial_stats['development_stage']}")
        print()
    
    # Step 3: Run learning simulation
    for step in range(steps):
        brain.step(step)
        
        # Log progress every 200 steps
        if step % 200 == 0:
            current_stats = {
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'development_stage': brain.getDevelopmentalStage(),
            }
            print(f"Step {step}: {current_stats['firing_neurons']} firing neurons, "
                  f"{current_stats['avg_firing_rate']:.3f} Hz avg rate, "
                  f"stage {current_stats['development_stage']}")
    
    # Record final state
    final_stats = {
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'development_stage': brain.getDevelopmentalStage(),
        'total_neuron_count': brain.getTotalNeuronCount(),
        'total_synapse_count': brain.getTotalSynapseCount(),
    }
    
    # Calculate plasticity metrics
    weight_changes = []
    for region in brain.getRegions():
        for synapse in region.getSynapses():
            weight_changes.append(synapse.getWeight())
    
    if len(weight_changes) > 0:
        initial_mean_weight = np.mean(weight_changes[:10] if len(weight_changes) >= 10 else weight_changes)
        final_mean_weight = np.mean(weight_changes[-10:] if len(weight_changes) >= 10 else weight_changes)
        weight_change = final_mean_weight - initial_mean_weight
        
        learning_detected = abs(weight_change) > 0.001
    else:
        initial_mean_weight = 0.0
        final_mean_weight = 0.0
        weight_change = 0.0
        learning_detected = False
    
    # Collect plasticity system status
    plasticity_status = {}
    
    # Get STDP status
    if brain.getSTDP():
        stdp_active = brain.getSTDP().isEnabled()
        plasticity_status['stdp_active'] = stdp_active
    else:
        plasticity_status['stdp_active'] = False
    
    # Get Hebbian status
    if brain.getHebbian():
        hebbian_active = brain.getHebbian().isEnabled()
        plasticity_status['hebbian_active'] = hebbian_active
    else:
        plasticity_status['hebbian_active'] = False
    
    # Get structural plasticity status
    if brain.getStructuralPlasticity():
        structural_active = brain.getStructuralPlasticity().isEnabled()
        plasticity_status['structural_active'] = structural_active
    else:
        plasticity_status['structural_active'] = False
    
    # Collect learning metrics
    learning_stats = {
        'initial_mean_weight': initial_mean_weight,
        'final_mean_weight': final_mean_weight,
        'weight_change': weight_change,
        'learning_detected': learning_detected,
        'steps': steps,
        'initial_stats': initial_stats,
        'final_stats': final_stats,
        'plasticity_status': plasticity_status,
        'synapse_count': len(weight_changes),
        'weight_change_magnitude': abs(weight_change)
    }
    
    print(f"\nLearning and Plasticity Example Results:")
    print(f"  Initial mean weight: {learning_stats['initial_mean_weight']:.4f}")
    print(f"  Final mean weight: {learning_stats['final_mean_weight']:.4f}")
    print(f"  Weight change: {learning_stats['weight_change']:.4f}")
    print(f"  Learning detected: {learning_stats['learning_detected']}")
    print(f"  Synapses analyzed: {learning_stats['synapse_count']}")
    print(f"  Plasticity systems:")
    print(f"    STDP: {'Active' if plasticity_status['stdp_active'] else 'Inactive'}")
    print(f"    Hebbian: {'Active' if plasticity_status['hebbian_active'] else 'Inactive'}")
    print(f"    Structural: {'Active' if plasticity_status['structural_active'] else 'Inactive'}")
    
    return learning_stats
if __name__ == "__main__":
    # Run learning and plasticity example
    stats = learning_plasticity_example(steps=2000, show_detailed_stats=True)
    
    print("\nLearning and Plasticity Example completed successfully!")
    print(f"Learning detected: {stats['learning_detected']}")
    print(f"Total weight change: {stats['weight_change']:.4f}")