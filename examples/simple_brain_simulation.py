#!/usr/bin/env python3
"""
Basic Brain Simulation Example
Demonstrates basic NLM brain simulation without world interaction.
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt

def main():
    print("=== Simple Brain Simulation ===")
    
    # Create brain configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    config.set("brain.synapse_density", 0.1)
    
    # Initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Simulation parameters
    num_steps = 200
    firing_rates = []
    
    # Run simulation
    for step in range(num_steps):
        brain.step(step)
        
        # Collect statistics
        firing_rates.append(brain.getAverageFiringRate())
        
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"avg rate: {firing_rates[-1]:.3f} Hz")
    
    # Analysis
    print(f"\n=== Simulation Results ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Average firing rate: {np.mean(firing_rates):.3f} Hz")
    print(f"Max firing rate: {np.max(firing_rates):.3f} Hz")
    print(f"Firing rate std: {np.std(firing_rates):.3f} Hz")
    
    # Visualization
    plt.figure(figsize=(12, 8))
    
    # Plot firing rates
    plt.subplot(2, 2, 1)
    plt.plot(firing_rates)
    plt.title('Firing Rate Over Time')
    plt.xlabel('Time Step')
    plt.ylabel('Firing Rate (Hz)')
    plt.grid(True, alpha=0.3)
    
    # Plot excitation/inhibition ratio
    plt.subplot(2, 2, 2)
    ei_ratios = []
    for i in range(0, num_steps, 20):
        ei_ratios.append(brain.getExcitationInhibitionRatio())
    plt.plot(range(0, num_steps, 20), ei_ratios)
    plt.title('E/I Balance Over Time')
    plt.xlabel('Time Step')
    plt.ylabel('E/I Ratio')
    plt.grid(True, alpha=0.3)
    
    # Plot development stage
    plt.subplot(2, 2, 3)
    developmental_stages = [brain.getDevelopmentalStage() for _ in range(0, num_steps, 20)]
    unique_stages, counts = np.unique(developmental_stages, return_counts=True)
    plt.bar(unique_stages, counts)
    plt.title('Development Stage Distribution')
    plt.xlabel('Development Stage')
    plt.ylabel('Count')
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('simple_brain_simulation.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    print(f"\nPlot saved to 'simple_brain_simulation.png'")

if __name__ == "__main__":
    main()