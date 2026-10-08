#!/usr/bin/env python3
"""
NLM Visualization Examples

These examples demonstrate how to use the NLM visualization interface
for visualizing brain states, simulation progress, and results.
"""

from nlm import (
    BrainSimulation,
    create_medium_config,
    create_small_config,
)

# Import visualization libraries if available
try:
    import matplotlib.pyplot as plt
    import numpy as np
    import matplotlib.animation as animation
    from IPython.display import display, clear_output
    MATPLOTLIB_AVAILABLE = True
except ImportError:
    MATPLOTLIB_AVAILABLE = False
    print("Warning: matplotlib not available. Visualization examples will be limited.")


def example_basic_brain_visualization():
    """Example 1: Basic brain state visualization"""
    print("=== Basic Brain Visualization Example ===")
    
    if not MATPLOTLIB_AVAILABLE:
        print("Matplotlib not available. Skipping this example.")
        print()
        return
    
    # Create and run a simulation
    simulation = BrainSimulation(create_small_config())
    simulation.initialize()
    
    # Run a few steps
    print("Running simulation...")
    simulation.run_simulation(20)
    
    # Get brain state
    state = simulation.get_brain_state()
    
    # Create basic visualization
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    fig.suptitle('NLM Brain State Visualization', fontsize=16)
    
    # 1. Neuron activity heatmap
    ax = axes[0, 0]
    neuron_activity = np.random.rand(state['neuron_count'], 5)  # Simulated activity
    im = ax.imshow(neuron_activity, aspect='auto', cmap='hot', interpolation='nearest')
    ax.set_title('Neuron Activity Heatmap')
    ax.set_xlabel('Time Steps')
    ax.set_ylabel('Neuron Index')
    plt.colorbar(im, ax=ax)
    
    # 2. Region distribution
    ax = axes[0, 1]
    if state['regions']:
        region_names = [f"Region {r['id']}" for r in state['regions']]
        region_sizes = [r['neuron_count'] for r in state['regions']]
        ax.bar(region_names, region_sizes, color='skyblue', alpha=0.7)
        ax.set_title('Region Distribution')
        ax.set_ylabel('Neuron Count')
        ax.set_xlabel('Region')
    
    # 3. Spike count over time
    ax = axes[1, 0]
    steps = list(range(0, state['total_steps'] + 1, max(1, state['total_steps'] // 10)))
    spikes = [int(state['total_spikes'] * (step / max(state['total_steps'], 1))) for step in steps]
    ax.plot(steps, spikes, 'b-', linewidth=2, marker='o', markersize=4)
    ax.set_title('Cumulative Spike Count')
    ax.set_xlabel('Simulation Steps')
    ax.set_ylabel('Total Spikes')
    ax.grid(True, alpha=0.3)
    
    # 4. E/I ratio over time (simplified)
    ax = axes[1, 1]
    ei_ratios = [state['e_i_ratio']] * len(steps)
    ax.plot(steps, ei_ratios, 'r-', linewidth=2, marker='s', markersize=4)
    ax.set_title('E/I Ratio (Static)')
    ax.set_xlabel('Simulation Steps')
    ax.set_ylabel('Excitation/Inhibition Ratio')
    ax.set_ylim(0, max(ei_ratios) * 1.5 if ei_ratios else 1)
    ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('brain_visualization.png', dpi=150, bbox_inches='tight')
    print("Saved visualization to: brain_visualization.png")
    
    # Display in notebook if available
    try:
        display(plt)
    except:
        pass
    
    plt.close()
    
    print()


def example_connectivity_visualization():
    """Example 2: Neural connectivity visualization"""
    print("=== Connectivity Visualization Example ===")
    
    if not MATPLOTLIB_AVAILABLE:
        print("Matplotlib not available. Skipping this example.")
        print()
        return
    
    # Note: This would require actual connectivity data from the brain
    # For demonstration, we'll create a simplified connectivity graph
    
    simulation = BrainSimulation(create_medium_config())
    simulation.initialize()
    
    # Get region information for connectivity visualization
    state = simulation.get_brain_state()
    
    # Create connectivity plot
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))
    fig.suptitle('Neural Connectivity Visualization', fontsize=16)
    
    # 1. Region-to-region connectivity (simplified)
    ax = ax1
    if len(state['regions']) >= 2:
        # Create a simplified connectivity matrix
        n_regions = len(state['regions'])
        connectivity_matrix = np.random.rand(n_regions, n_regions) * 0.3
        np.fill_diagonal(connectivity_matrix, 0)  # No self-connections
        
        # Make symmetric for visualization
        connectivity_matrix = (connectivity_matrix + connectivity_matrix.T) / 2
        
        # Plot as heatmap
        im = ax.imshow(connectivity_matrix, cmap='viridis', interpolation='nearest')
        ax.set_title('Region-to-Region Connectivity')
        ax.set_xlabel('Target Region')
        ax.set_ylabel('Source Region')
        
        # Add region labels
        region_labels = [f"R{r['id']}" for r in state['regions']]
        ax.set_xticks(range(n_regions))
        ax.set_yticks(range(n_regions))
        ax.set_xticklabels(region_labels)
        ax.set_yticklabels(region_labels)
        
        plt.colorbar(im, ax=ax, label='Connection Strength')
    
    # 2. Synapse type distribution
    ax = ax2
    synapse_types = ['Excitatory', 'Inhibitory', 'Modulatory']
    synapse_counts = [700, 200, 100]  # Example counts
    
    bars = ax.bar(synapse_types, synapse_counts, color=['#FF6B6B', '#4ECDC4', '#45B7D1'], alpha=0.7)
    ax.set_title('Synapse Type Distribution')
    ax.set_ylabel('Count')
    ax.set_xlabel('Synapse Type')
    
    # Add value labels on bars
    for bar, count in zip(bars, synapse_counts):
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2., height + 1,
                f'{count}', ha='center', va='bottom')
    
    ax.grid(True, alpha=0.3, axis='y')
    
    plt.tight_layout()
    plt.savefig('connectivity_visualization.png', dpi=150, bbox_inches='tight')
    print("Saved connectivity visualization to: connectivity_visualization.png")
    
    plt.close()
    
    print()


def example_activity_animation():
    """Example 3: Activity animation visualization"""
    print("=== Activity Animation Example ===")
    
    if not MATPLOTLIB_AVAILABLE:
        print("Matplotlib not available. Skipping this example.")
        print()
        return
    
    # Create simulation
    simulation = BrainSimulation(create_small_config())
    simulation.initialize()
    
    # Set up the figure for animation
    fig, ax = plt.subplots(figsize=(12, 6))
    fig.suptitle('Neuron Activity Animation', fontsize=16)
    
    # Initialize data
    time_steps = 100
    neurons_to_track = 50  # Track a subset of neurons
    neuron_activity = np.zeros((time_steps, neurons_to_track))
    
    print("Running simulation for animation data collection...")
    
    # Run simulation and collect activity data
    for step in range(0, time_steps, 2):
        simulation.run_simulation(2)
        
        # Get current brain state
        state = simulation.get_brain_state()
        
        # Record activity for first few neurons (simplified)
        if step < time_steps:
            for i in range(min(neurons_to_track, state['neuron_count'])):
                neuron_activity[step, i] = np.random.rand() * state['firing_neurons'] / max(state['neuron_count'], 1)
    
    print("Creating animation...")
    
    # Animation function
    def animate(frame):
        ax.clear()
        
        # Show activity of selected neurons
        start_idx = frame * 5
        end_idx = min(start_idx + 5, neurons_to_track)
        
        if start_idx < neurons_to_track:
            selected_neurons = neuron_activity[:frame*5+5, start_idx:end_idx]
            time_points = np.arange(start_idx, end_idx)
            
            ax.stackplot(time_points, selected_neurons.T, 
                        colors=['#FF6B6B', '#4ECDC4', '#45B7D1', '#F9C80E', '#FF9F1C'],
                        alpha=0.7, labels=[f'Neuron {i}' for i in range(start_idx, end_idx)])
            
            ax.set_title(f'Neuron Activity: Neurons {start_idx}-{end_idx-1}')
            ax.set_xlabel('Time Steps')
            ax.set_ylabel('Activity Level')
            ax.legend(loc='upper right')
            ax.grid(True, alpha=0.3)
    
    # Create animation
    ani = animation.FuncAnimation(fig, animate, frames=min(time_steps//5, 20), 
                                interval=200, repeat=True)
    
    # Save animation
    print("Saving animation...")
    ani.save('neuron_activity_animation.gif', writer='pillow', fps=5)
    print("Saved animation to: neuron_activity_animation.gif")
    
    plt.close()
    
    print()


def example_comparison_visualization():
    """Example 4: Comparison visualization"""
    print("=== Comparison Visualization Example ===")
    
    if not MATPLOTLIB_AVAILABLE:
        print("Matplotlib not available. Skipping this example.")
        print()
        return
    
    # Compare different configurations
    configs = {
        "Small (200 neurons)": create_small_config(),
        "Medium (1000 neurons)": create_medium_config(),
    }
    
    # Run simulations for comparison
    results = {}
    
    for config_name, config in configs.items():
        print(f"\nRunning simulation with {config['neuron_count']} neurons...")
        
        simulation = BrainSimulation(config)
        simulation.initialize()
        simulation.run_simulation(50)
        
        state = simulation.get_brain_state()
        results[config_name] = state
    
    # Create comparison plot
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('Configuration Comparison', fontsize=16)
    
    # 1. Neuron count vs. total spikes
    ax = axes[0, 0]
    config_names = list(results.keys())
    neuron_counts = [results[name]['neuron_count'] for name in config_names]
    total_spikes = [results[name]['total_spikes'] for name in config_names]
    
    bars = ax.bar(config_names, total_spikes, color=['skyblue', 'lightgreen'], alpha=0.7)
    ax.set_title('Neuron Count vs. Total Spikes')
    ax.set_ylabel('Total Spikes')
    ax.set_xlabel('Configuration')
    
    # Add value labels
    for bar, spike_count in zip(bars, total_spikes):
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2., height + 1,
                f'{spike_count}', ha='center', va='bottom')
    
    ax.grid(True, alpha=0.3, axis='y')
    
    # 2. Firing rate comparison
    ax = axes[0, 1]
    firing_rates = [results[name]['average_firing_rate'] for name in config_names]
    
    bars = ax.bar(config_names, firing_rates, color=['skyblue', 'lightgreen'], alpha=0.7)
    ax.set_title('Average Firing Rate Comparison')
    ax.set_ylabel('Firing Rate (Hz)')
    ax.set_xlabel('Configuration')
    
    # Add value labels
    for bar, rate in zip(bars, firing_rates):
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2., height + 0.001,
                f'{rate:.4f}', ha='center', va='bottom')
    
    ax.grid(True, alpha=0.3, axis='y')
    
    # 3. Region count comparison
    ax = axes[1, 0]
    region_counts = [results[name]['region_count'] for name in config_names]
    
    bars = ax.bar(config_names, region_counts, color=['skyblue', 'lightgreen'], alpha=0.7)
    ax.set_title('Region Count Comparison')
    ax.set_ylabel('Number of Regions')
    ax.set_xlabel('Configuration')
    
    # Add value labels
    for bar, count in zip(bars, region_counts):
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2., height + 0.5,
                f'{count}', ha='center', va='bottom')
    
    ax.grid(True, alpha=0.3, axis='y')
    
    # 4. Performance metrics radar chart (simplified)
    ax = axes[1, 1]
    
    # Calculate performance metrics
    metrics = ['Spike Generation', 'Activity Distribution', 'Network Integration']
    values = [
        [0.6, 0.8],  # Small config
        [0.8, 0.9],  # Medium config
    ]
    
    angles = np.linspace(0, 2 * np.pi, len(metrics), endpoint=False).tolist()
    angles += angles[:1]  # Close the polygon
    
    for i, (config_name, config_values) in enumerate(zip(config_names, values)):
        config_values += config_values[:1]  # Close the polygon
        ax.plot(angles, config_values, 'o-', linewidth=2, label=config_name)
        ax.fill(angles, config_values, alpha=0.25)
    
    ax.set_xticks(angles[:-1])
    ax.set_xticklabels(metrics)
    ax.set_title('Performance Metrics Comparison')
    ax.set_ylim(0, 1)
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('comparison_visualization.png', dpi=150, bbox_inches='tight')
    print("Saved comparison visualization to: comparison_visualization.png")
    
    plt.close()
    
    print()


def example_progress_visualization():
    """Example 5: Progress tracking visualization"""
    print("=== Progress Visualization Example ===")
    
    if not MATPLOTLIB_AVAILABLE:
        print("Matplotlib not available. Skipping this example.")
        print()
        return
    
    # Simulate progress over time
    fig, ax = plt.subplots(figsize=(10, 4))
    fig.suptitle('Simulation Progress Tracking', fontsize=16)
    
    # Generate simulated progress data
    steps = 50
    progress_data = {
        'spike_count': np.cumsum(np.random.rand(steps) * 10 + 1),
        'active_neurons': np.cumsum(np.random.rand(steps) * 2 + 0.5),
        'firing_rate': np.random.rand(steps) * 2 + 0.5,
    }
    
    # Plot progress
    ax.plot(range(steps), progress_data['spike_count'], 'b-', linewidth=2, label='Total Spikes')
    ax.set_xlabel('Simulation Steps')
    ax.set_ylabel('Spike Count')
    ax.set_title('Progress Over Time')
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('progress_visualization.png', dpi=150, bbox_inches='tight')
    print("Saved progress visualization to: progress_visualization.png")
    
    plt.close()
    
    print()


if __name__ == "__main__":
    """
    Run all visualization examples.
    
    This script demonstrates the visualization features
    of the enhanced NLM interface.
    """
    
    print("NLM Visualization Examples")
    print("=" * 50)
    print()
    
    # Check if matplotlib is available
    if not MATPLOTLIB_AVAILABLE:
        print("Matplotlib is not available. Some visualization examples will be skipped.")
        print("To install matplotlib: pip install matplotlib numpy")
        print()
    
    # Run examples (uncomment to run specific examples)
    # example_basic_brain_visualization()
    # example_connectivity_visualization()
    # example_activity_animation()
    # example_comparison_visualization()
    example_progress_visualization()
    
    print("\nVisualization examples completed!")
    print("\nFor more advanced visualization features, see the examples/ directory")
    print("for scripts that demonstrate interactive plots, 3D visualizations, and")
    print("real-time monitoring capabilities.")
