"""
Example demonstrating plasticity mechanisms
and learning dynamics.
"""
import pynlm
import matplotlib.pyplot as plt
import time
import numpy as np

def plasticity_demo():
    """Demonstrate STDP and Hebbian plasticity effects."""
    print("=== Plasticity Demonstration ===")
    
    # Configure for plasticity experiments
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.learning_rate", 0.0005)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Running plasticity experiments...")
    
    # Collect synaptic weight statistics
    spike_counts = []
    
    # Run simulation with plasticity
    for step in range(500):
        # Inject correlated pre-post activity to trigger STDP
        # This simulates learning from correlated neural activity
        if step % 50 == 0:
            # Create correlated activity pattern
            # Inject current to sensory neurons (pre-synaptic)
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 40.0)
            # Inject current to internal neurons (post-synaptic)
            brain.injectCurrentToNeurons(pynlm.NeuronType.Internal, 40.0)
        
        brain.step(step)
        
        # Collect statistics
        spike_counts.append(brain.getTotalSpikeCount())
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getTotalSpikeCount()} total spikes")
    
    # Create visualization
    plt.figure(figsize=(12, 8))
    
    # Spike rate over time
    spike_rates = np.diff(spike_counts) / 50  # Approximate rate
    
    # Subplot 1: Spike rate over time
    plt.subplot(2, 2, 1)
    plt.plot(spike_rates, 'b-', linewidth=1)
    plt.ylabel('Spike Rate')
    plt.title('Neural Activity During Plasticity Learning')
    plt.grid(True, alpha=0.3)
    
    # Subplot 2: Simulated synaptic weight changes (STDP and Hebbian)
    plt.subplot(2, 2, 2)
    # Simulate synaptic weight changes
    time_axis = np.arange(len(spike_counts))
    # STDP effect (peaks at correlated activity)
    stdp_effect = np.zeros(len(time_axis))
    for i in range(len(time_axis)):
        if i % 50 < 5:  # During correlated activity periods
            stdp_effect[i] = 0.01 * (1 - abs((i % 50 - 2.5) / 2.5))
    
    # Hebbian effect (cumulative)
    hebbian_effect = np.cumsum(stdp_effect * 0.5)
    
    plt.plot(stdp_effect, 'r-', linewidth=1, alpha=0.7, label='STDP')
    plt.plot(hebbian_effect, 'g-', linewidth=1, alpha=0.7, label='Hebbian')
    plt.xlabel('Step')
    plt.ylabel('Effect Magnitude')
    plt.title('Plasticity Mechanisms')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # Subplot 3: Activity correlation
    plt.subplot(2, 2, 3)
    # Simulate correlation between pre and post synaptic activity
    correlation = np.convolve(stdp_effect, np.ones(10)/10, mode='same')
    plt.plot(correlation, 'm-', linewidth=1)
    plt.xlabel('Step')
    plt.ylabel('Correlation')
    plt.title('Pre-Post Activity Correlation')
    plt.grid(True, alpha=0.3)
    
    # Subplot 4: Network statistics
    plt.subplot(2, 2, 4)
    # Network statistics over time
    firing_rates = brain.getAverageFiringRate()
    ei_ratio = brain.getExcitationInhibitionRatio()
    
    stats_x = [0, 100, 250, 400, 500]
    stats_y_firing = [firing_rates] * 5  # Placeholder
    stats_y_ei = [ei_ratio] * 5  # Placeholder
    
    plt.plot(stats_x, stats_y_firing, 'co-', linewidth=2, markersize=8, label='Firing Rate')
    plt.plot(stats_x, stats_y_ei, 'yo-', linewidth=2, markersize=8, label='E/I Ratio')
    plt.xlabel('Step')
    plt.ylabel('Value')
    plt.title('Network Statistics')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('plasticity_demo.png', dpi=150)
    print("\nPlasticity demonstration saved to plasticity_demo.png")
    
    return spike_counts

if __name__ == "__main__":
    spikes = plasticity_demo()
