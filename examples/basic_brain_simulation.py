#!/usr/bin/env python3
"""
Basic NLM Brain Simulation Example
===================================

This example demonstrates the fundamental usage of NLM (Neural Learning Machine)
for simulating artificial neural networks and learning processes.

The example shows:
1. Basic brain creation and initialization
2. Simple neural network simulation
3. Recording and analyzing neural activity
4. Understanding network dynamics
"""

import pynlm
import time
import matplotlib.pyplot as plt

def main():
    print("=== NLM Basic Brain Simulation Example ===\n")
    
    # 1. Create and configure a basic brain
    print("1. Creating and configuring the brain...")
    config = pynlm.createDefaultConfig()
    
    # Customize brain size for this example
    config.set("brain.neuron_count", 500)
    config.set("brain.connection_probability", 0.05)
    config.set("brain.initial_weight_mean", 0.5)
    config.set("brain.initial_weight_std", 0.1)
    
    brain = pynlm.createBrain(config)
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"   Expected synapses: {int(brain.getTotalNeuronCount() * 0.05)}")
    
    # 2. Initialize the brain
    print("\n2. Initializing the brain...")
    brain.initialize()
    print("   Brain initialization complete")
    
    # 3. Run a simulation and collect data
    print("\n3. Running simulation...")
    num_steps = 200
    
    # Data collection arrays
    firing_counts = []
    total_spikes_history = []
    average_firing_rates = []
    excitation_inhibition_ratios = []
    
    start_time = time.time()
    
    for step in range(num_steps):
        # Run one simulation step
        brain.step(step)
        
        # Collect statistics
        firing_count = brain.getFiringNeuronCount()
        total_spikes = brain.getTotalSpikeCount()
        avg_firing_rate = brain.getAverageFiringRate()
        ei_ratio = brain.getExcitationInhibitionRatio()
        
        firing_counts.append(firing_count)
        total_spikes_history.append(total_spikes)
        average_firing_rates.append(avg_firing_rate)
        excitation_inhibition_ratios.append(ei_ratio)
        
        # Print progress every 50 steps
        if (step + 1) % 50 == 0:
            print(f"   Step {step + 1}/{num_steps}: "
                  f"{firing_count} firing neurons, "
                  f"{total_spikes} total spikes, "
                  f"firing rate: {avg_firing_rate:.2f}")
    
    end_time = time.time()
    simulation_time = end_time - start_time
    
    print(f"\n4. Simulation completed in {simulation_time:.2f} seconds")
    print(f"   Average speed: {num_steps/simulation_time:.1f} steps/second")
    
    # 5. Analyze and visualize results
    print("\n5. Analyzing results...")
    
    # Basic statistics
    final_firing = firing_counts[-1]
    final_spikes = total_spikes_history[-1]
    mean_firing_rate = sum(average_firing_rates) / len(average_firing_rates)
    mean_ei_ratio = sum(excitation_inhibition_ratios) / len(excitation_inhibition_ratios)
    
    print(f"\n   Final Network State:")
    print(f"     - Firing neurons: {final_firing}")
    print(f"     - Total spikes: {final_spikes}")
    print(f"     - Average firing rate: {mean_firing_rate:.3f} Hz")
    print(f"     - E/I ratio: {mean_ei_ratio:.3f}")
    print(f"     - Network utilization: {final_firing/brain.getTotalNeuronCount()*100:.1f}%")
    
    # 6. Detect learning or dynamics
    print("\n6. Analyzing network dynamics...")
    
    # Calculate changes over time
    if len(firing_counts) >= 10:
        early_firing = sum(firing_counts[:len(firing_counts)//3]) / (len(firing_counts)//3)
        late_firing = sum(firing_counts[-len(firing_counts)//3:]) / (len(firing_counts)//3)
        
        firing_change = late_firing - early_firing
        firing_trend = "increasing" if firing_change > 0 else "decreasing"
        
        print(f"   Firing activity trend: {firing_trend} ({firing_change:+.2f} neurons)")
        
        if abs(firing_change) > early_firing * 0.1:  # More than 10% change
            print("   -> Significant network dynamics detected!")
        else:
            print("   -> Network reached relative stability.")
    
    # 7. Create visualization
    print("\n7. Generating visualization...")
    
    try:
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('NLM Brain Simulation Results', fontsize=16)
        
        # Plot 1: Firing neuron count over time
        axes[0, 0].plot(firing_counts, 'b-', linewidth=2, alpha=0.7)
        axes[0, 0].set_title('Firing Neuron Count')
        axes[0, 0].set_xlabel('Simulation Step')
        axes[0, 0].set_ylabel('Number of Firing Neurons')
        axes[0, 0].grid(True, alpha=0.3)
        axes[0, 0].set_ylim(bottom=0)
        
        # Plot 2: Total spikes over time
        axes[0, 1].plot(total_spikes_history, 'r-', linewidth=2, alpha=0.7)
        axes[0, 1].set_title('Total Spike Count')
        axes[0, 1].set_xlabel('Simulation Step')
        axes[0, 1].set_ylabel('Total Spikes')
        axes[0, 1].grid(True, alpha=0.3)
        axes[0, 1].set_ylim(bottom=0)
        
        # Plot 3: Average firing rate over time
        axes[1, 0].plot(average_firing_rates, 'g-', linewidth=2, alpha=0.7)
        axes[1, 0].set_title('Average Firing Rate')
        axes[1, 0].set_xlabel('Simulation Step')
        axes[1, 0].set_ylabel('Firing Rate (Hz)')
        axes[1, 0].grid(True, alpha=0.3)
        axes[1, 0].set_ylim(bottom=0)
        
        # Plot 4: E/I ratio over time
        axes[1, 1].plot(excitation_inhibition_ratios, 'm-', linewidth=2, alpha=0.7)
        axes[1, 1].set_title('Excitation/Inhibition Ratio')
        axes[1, 1].set_xlabel('Simulation Step')
        axes[1, 1].set_ylabel('E/I Ratio')
        axes[1, 1].grid(True, alpha=0.3)
        axes[1, 1].set_ylim(bottom=0)
        
        plt.tight_layout()
        plt.savefig('nlm_simulation_results.png', dpi=150, bbox_inches='tight')
        print("   Visualization saved as 'nlm_simulation_results.png'")
        
    except ImportError:
        print("   Note: Matplotlib not available, skipping visualization")
        print("   Install matplotlib for plotting: pip install matplotlib")
    
    # 8. Summary
    print("\n" + "="*50)
    print("SIMULATION SUMMARY")
    print("="*50)
    print(f"Brain Configuration:")
    print(f"  - Neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Synapses: {brain.getTotalSynapseCount()}")
    print(f"  - Connection density: {brain.getTotalSynapseCount()/brain.getTotalNeuronCount()*100:.1f}%")
    print(f"\nSimulation Results:")
    print(f"  - Duration: {num_steps} steps")
    print(f"  - Final firing: {final_firing} neurons")
    print(f"  - Total activity: {final_spikes} spikes")
    print(f"  - Average firing rate: {mean_firing_rate:.3f} Hz")
    print(f"  - E/I balance: {mean_ei_ratio:.3f}")
    print(f"  - Network efficiency: {final_spikes/num_steps:.1f} spikes/step")
    
    print(f"\nKey Insights:")
    if mean_firing_rate > 1.0:
        print("  - Network shows sustained activity")
    if mean_ei_ratio > 1.5:
        print("  - Network is excitation-dominant")
    elif mean_ei_ratio < 0.5:
        print("  - Network is inhibition-dominant")
    else:
        print("  - Network shows balanced excitation/inhibition")
    
    print(f"\nExample completed successfully!")
    
if __name__ == "__main__":
    main()
