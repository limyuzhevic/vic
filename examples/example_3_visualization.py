import pynlm
import matplotlib.pyplot as plt

def analyze_brain_activity(brain, num_steps=200):
    """Analyze brain activity over time and create visualizations."""
    
    # Initialize brain
    brain.initialize()
    
    # Collect statistics
    step_data = []
    firing_rates = []
    spike_counts = []
    development_stages = []
    neuromodulation_levels = []
    
    for step in range(num_steps):
        brain.step(step)
        
        step_data.append({
            'step': step,
            'firing_neutrons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'development_stage': brain.getDevelopmentalStage()
        })
        
        # If we have an agent brain, collect neuromodulation data
        if hasattr(brain, 'getCuriosityLevel'):
            # This is an agent brain, collect more data
            curiosity = brain.getCuriosityLevel()
            novelty = brain.getNoveltyLevel()
            neuromodulation = brain.getNeuromodulationLevel()
            
            neuromodulation_levels.append(neuromodulation)
            firing_rates.append(brain.getAverageFiringRate())
            spike_counts.append(brain.getTotalSpikeCount())
            development_stages.append(brain.getDevelopmentalStage())
    
    # Create analysis plots
    fig, axes = plt.subplots(2, 2, figsize=(15, 10))
    
    # Plot 1: Firing neuron count over time
    axes[0, 0].plot([d['step'] for d in step_data], 
                   [d['firing_neutrons'] for d in step_data],
                   'b-', linewidth=2)
    axes[0, 0].set_title('Firing Neuron Count Over Time')
    axes[0, 0].set_xlabel('Step')
    axes[0, 0].set_ylabel('Number of Neurons')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plot 2: Spike count over time
    axes[0, 1].plot([d['step'] for d in step_data],
                   [d['total_spikes'] for d in step_data],
                   'r-', linewidth=2)
    axes[0, 1].set_title('Total Spike Count Over Time')
    axes[0, 1].set_xlabel('Step')
    axes[0, 1].set_ylabel('Total Spikes')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Plot 3: Firing rate over time
    axes[1, 0].plot([d['step'] for d in step_data],
                   [d['avg_firing_rate'] for d in step_data],
                   'g-', linewidth=2)
    axes[1, 0].set_title('Average Firing Rate Over Time')
    axes[1, 0].set_xlabel('Step')
    axes[1, 0].set_ylabel('Firing Rate (Hz)')
    axes[1, 0].grid(True, alpha=0.3)
    
    # Plot 4: E/I ratio over time
    axes[1, 1].plot([d['step'] for d in step_data],
                   [d['e_i_ratio'] for d in step_data],
                   'm-', linewidth=2)
    axes[1, 1].set_title('Excitation/Inhibition Ratio Over Time')
    axes[1, 1].set_xlabel('Step')
    axes[1, 1].set_ylabel('E/I Ratio')
    axes[1, 1].grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.show()
    
    return step_data

if __name__ == "__main__":
    # Create a brain for analysis
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.05)
    
    brain = pynlm.createBrain(config)
    
    print("Analyzing brain activity...")
    analysis_data = analyze_brain_activity(brain, 200)
    
    print(f"Analysis complete!")
    print(f"Average firing rate: {sum(d['avg_firing_rate'] for d in analysis_data)/len(analysis_data):.2f} Hz")
    print(f"Maximum firing neurons: {max(d['firing_neutrons'] for d in analysis_data)}")
    print(f"Total spikes: {analysis_data[-1]['total_spikes']}")