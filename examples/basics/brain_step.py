"""
Example demonstrating different brain stepping methods
and advanced simulation control.
"""
import pynlm
import matplotlib.pyplot as plt
import numpy as np

def brain_step_example():
    """Demonstrate different stepping methods and monitoring."""
    print("=== Brain Step Examples ===")
    
    # Create brain with monitoring
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("simulation_timestep", 0.001)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Collect statistics
    steps = []
    firing_counts = []
    spike_counts = []
    firing_rates = []
    
    print("Running simulation with detailed monitoring...")
    
    for step in range(500):
        # Use timed stepping for precise timing
        time = step * config.getOr("simulation_timestep", 0.001)
        brain.step(step, time)
        
        # Collect statistics
        steps.append(step)
        firing_counts.append(brain.getFiringNeuronCount())
        spike_counts.append(brain.getTotalSpikeCount())
        firing_rates.append(brain.getAverageFiringRate())
        
        # Apply periodic input to observe effects
        if step % 50 == 0:
            # Inject current to sensory neurons
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
    
    # Plot results
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Firing neuron count
    axes[0, 0].plot(steps, firing_counts, 'b-', linewidth=1, alpha=0.8)
    axes[0, 0].set_xlabel('Simulation Step')
    axes[0, 0].set_ylabel('Firing Neurons')
    axes[0, 0].set_title('Firing Neuron Count Over Time')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Spike count
    axes[0, 1].plot(steps, np.diff(spike_counts), 'r-', linewidth=1, alpha=0.8)
    axes[0, 1].set_xlabel('Simulation Step')
    axes[0, 1].set_ylabel('Spikes per Step')
    axes[0, 1].set_title('Spike Rate Over Time')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Firing rate
    axes[1, 0].plot(steps, firing_rates, 'g-', linewidth=1, alpha=0.8)
    axes[1, 0].set_xlabel('Simulation Step')
    axes[1, 0].set_ylabel('Firing Rate (Hz)')
    axes[1, 0].set_title('Average Firing Rate Over Time')
    axes[1, 0].grid(True, alpha=0.3)
    
    # E/I ratio
    ei_ratios = [brain.getExcitationInhibitionRatio() for _ in steps]
    axes[1, 1].plot(steps, ei_ratios, 'm-', linewidth=1, alpha=0.8)
    axes[1, 1].set_xlabel('Simulation Step')
    axes[1, 1].set_ylabel('E/I Ratio')
    axes[1, 1].set_title('Excitation/Inhibition Balance')
    axes[1, 1].grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('brain_simulation_results.png', dpi=150)
    print("Results saved to brain_simulation_results.png")
    
    return steps, firing_counts, spike_counts, firing_rates

if __name__ == "__main__":
    steps, firing_counts, spike_counts, firing_rates = brain_step_example()
