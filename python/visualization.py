#!/usr/bin/env python3
"""
NLM Visualization Tools

Visualization tools for NLM simulations including:
- Network connectivity visualization
- Neural activity visualization
- Memory trace visualization
- Learning curve analysis
"""

import sys
from pathlib import Path
import math
import random

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, str(Path(__file__).parent))

import pynlm as pynlm_core


def plot_brain_connectivity(brain, output_file=None):
    """
    Create a visual representation of brain connectivity.
    
    Args:
        brain: Brain object to visualize
        output_file: File path to save visualization (if None, displays)
    
    Returns:
        matplotlib figure (if available)
    """
    try:
        import matplotlib.pyplot as plt
        import networkx as nx
        
        # Create graph
        G = nx.Graph()
        
        # Add neurons as nodes
        total_neurons = brain.getTotalNeuronCount()
        regions = brain.getRegionCount() if hasattr(brain, 'getRegionCount') else 1
        
        neurons_per_region = total_neurons // regions
        
        for region_id in range(1, regions + 1):
            for neuron_id in range((region_id - 1) * neurons_per_region, region_id * neurons_per_region):
                G.add_node(neuron_id, region=region_id)
        
        # Get synapses and add connections
        if hasattr(brain, 'getSynapses'):
            synapses = brain.getSynapses()
            for syn in synapses:
                source = syn.getSourceNeuron().index()
                dest = syn.getDestinationNeuron().index()
                G.add_edge(source, dest, weight=syn.getWeight())
        
        # Create layout
        pos = nx.spring_layout(G, seed=42)
        
        # Create figure
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Plot 1: Network topology
        ax1 = axes[0, 0]
        nx.draw_networkx_nodes(G, pos, ax=ax1, node_color='lightblue', node_size=50)
        nx.draw_networkx_edges(G, pos, ax=ax1, alpha=0.3, width=0.5)
        ax1.set_title("Brain Network Topology")
        ax1.axis('off')
        
        # Plot 2: Region distribution
        ax2 = axes[0, 1]
        region_weights = {}
        for node in G.nodes():
            region = G.nodes[node].get('region', 1)
            if region not in region_weights:
                region_weights[region] = 0
            region_weights[region] += 1
        
        regions = list(region_weights.keys())
        counts = list(region_weights.values())
        
        bars = ax2.bar(regions, counts, color='skyblue', alpha=0.7)
        ax2.set_title("Neuron Distribution by Region")
        ax2.set_ylabel("Number of Neurons")
        
        # Add value labels on bars
        for bar, count in zip(bars, counts):
            ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                    str(count), ha='center', va='bottom')
        
        # Plot 3: Connection density
        ax3 = axes[1, 0]
        degrees = [d for n, d in G.degree()]
        ax3.hist(degrees, bins=20, alpha=0.7, color='lightgreen', edgecolor='black')
        ax3.set_title("Connection Distribution (Node Degree)")
        ax3.set_xlabel("Number of Connections")
        ax3.set_ylabel("Number of Neurons")
        
        # Plot 4: Synaptic weight distribution
        ax4 = axes[1, 1]
        if hasattr(brain, 'getSynapses'):
            weights = [syn.getWeight() for syn in brain.getSynapses()]
            if weights:
                ax4.hist(weights, bins=20, alpha=0.7, color='salmon', edgecolor='black')
                ax4.set_title("Synaptic Weight Distribution")
                ax4.set_xlabel("Synaptic Weight")
                ax4.set_ylabel("Number of Synapses")
            else:
                ax4.text(0.5, 0.5, "No synapses data", ha='center', va='center',
                        transform=ax4.transAxes)
                ax4.set_title("Synaptic Weight Distribution")
        else:
            ax4.text(0.5, 0.5, "No synapse data", ha='center', va='center',
                    transform=ax4.transAxes)
            ax4.set_title("Synaptic Weight Distribution")
        
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"Connectivity plot saved to {output_file}")
        else:
            plt.show()
        
        return fig
        
    except ImportError:
        print("Matplotlib or NetworkX not available. Install with:")
        print("  pip install matplotlib networkx")
        return None


def visualize_neural_activity(brain, duration=100, output_file=None):
    """
    Visualize neural activity over time.
    
    Args:
        brain: Brain object to simulate and visualize
        duration: Number of simulation steps
        output_file: File path to save visualization
    
    Returns:
        matplotlib figure
    """
    try:
        import matplotlib.pyplot as plt
        import numpy as np
        
        # Initialize brain if needed
        if not hasattr(brain, '_initialized'):
            brain.initialize()
            brain._initialized = True
        
        # Collect activity data
        time_points = []
        firing_rates = []
        total_spikes = []
        
        for step in range(duration):
            # Get current statistics
            firing_rate = brain.getAverageFiringRate()
            spikes = brain.getTotalSpikeCount()
            
            time_points.append(step)
            firing_rates.append(firing_rate)
            total_spikes.append(spikes)
            
            # Advance simulation
            brain.step(step)
        
        # Create figure
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Plot 1: Firing rate over time
        ax1 = axes[0, 0]
        ax1.plot(time_points, firing_rates, 'b-', linewidth=2, alpha=0.7)
        ax1.set_title("Firing Rate Over Time")
        ax1.set_xlabel("Simulation Step")
        ax1.set_ylabel("Firing Rate (spikes/sec)")
        ax1.grid(True, alpha=0.3)
        
        # Add moving average
        window_size = 10
        if len(firing_rates) >= window_size:
            moving_avg = np.convolve(firing_rates, np.ones(window_size)/window_size, mode='valid')
            valid_time = time_points[window_size-1:]
            ax1.plot(valid_time, moving_avg, 'r-', linewidth=2, label=f'{window_size}-step MA')
            ax1.legend()
        
        # Plot 2: Total spikes over time
        ax2 = axes[0, 1]
        ax2.plot(time_points, total_spikes, 'g-', linewidth=2, alpha=0.7)
        ax2.set_title("Total Spikes Over Time")
        ax2.set_xlabel("Simulation Step")
        ax2.set_ylabel("Total Spikes")
        ax2.grid(True, alpha=0.3)
        
        # Plot 3: Firing rate histogram
        ax3 = axes[1, 0]
        ax3.hist(firing_rates, bins=30, alpha=0.7, color='skyblue', edgecolor='black')
        ax3.set_title("Firing Rate Distribution")
        ax3.set_xlabel("Firing Rate")
        ax3.set_ylabel("Frequency")
        
        # Plot 4: Activity heatmap
        ax4 = axes[1, 1]
        if len(time_points) > 10:
            # Downsample for heatmap
            subsample = max(1, len(time_points) // 50)
            time_subset = time_points[::subsample]
            rate_subset = firing_rates[::subsample]
            
            # Create heatmap
            im = ax4.scatter(time_subset, rate_subset, c=rate_subset, 
                           cmap='hot', s=20, alpha=0.8)
            ax4.set_title("Activity Heatmap")
            ax4.set_xlabel("Simulation Step")
            ax4.set_ylabel("Firing Rate")
            plt.colorbar(im, ax=ax4)
        else:
            ax4.text(0.5, 0.5, "Insufficient data for heatmap", ha='center', va='center',
                    transform=ax4.transAxes)
            ax4.set_title("Activity Heatmap")
        
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"Activity visualization saved to {output_file}")
        else:
            plt.show()
        
        return fig
        
    except ImportError:
        print("Matplotlib not available. Install with:")
        print("  pip install matplotlib")
        return None


def visualize_working_memory(nlm_api, output_file=None):
    """
    Visualize working memory usage over time.
    
    Args:
        nlm_api: NLMAdvancedAPI instance
        output_file: File path to save visualization
    
    Returns:
        matplotlib figure
    """
    try:
        import matplotlib.pyplot as plt
        import numpy as np
        
        if not hasattr(nlm_api, '_memory_stats'):
            print("No memory monitoring data available")
            return None
        
        memory_stats = nlm_api._memory_stats
        
        if not memory_stats['working_memory_usage']:
            print("No working memory data available")
            return None
        
        # Extract data
        steps = [data['step'] for data in memory_stats['working_memory_usage']]
        neuron_counts = [data['neuron_count'] for data in memory_stats['working_memory_usage']]
        spike_counts = [data['spike_count'] for data in memory_stats['working_memory_usage']]
        
        # Create figure
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Plot 1: Working memory usage
        ax1 = axes[0, 0]
        ax1.plot(steps, neuron_counts, 'b-', linewidth=2, alpha=0.7)
        ax1.set_title("Working Memory Usage")
        ax1.set_xlabel("Simulation Step")
        ax1.set_ylabel("Active Neurons")
        ax1.grid(True, alpha=0.3)
        
        # Plot 2: Spike activity
        ax2 = axes[0, 1]
        ax2.plot(steps, spike_counts, 'r-', linewidth=2, alpha=0.7)
        ax2.set_title("Working Memory Spike Activity")
        ax2.set_xlabel("Simulation Step")
        ax2.set_ylabel("Spike Count")
        ax2.grid(True, alpha=0.3)
        
        # Plot 3: Memory efficiency (spikes per neuron)
        ax3 = axes[1, 0]
        efficiency = []
        for i in range(len(steps)):
            if neuron_counts[i] > 0:
                efficiency.append(spike_counts[i] / neuron_counts[i])
            else:
                efficiency.append(0)
        
        ax3.plot(steps, efficiency, 'g-', linewidth=2, alpha=0.7)
        ax3.set_title("Memory Efficiency (Spikes/Neuron)")
        ax3.set_xlabel("Simulation Step")
        ax3.set_ylabel("Efficiency")
        ax3.grid(True, alpha=0.3)
        
        # Plot 4: Memory usage histogram
        ax4 = axes[1, 1]
        ax4.hist(neuron_counts, bins=20, alpha=0.7, color='skyblue', edgecolor='black')
        ax4.set_title("Working Memory Usage Distribution")
        ax4.set_xlabel("Number of Neurons")
        ax4.set_ylabel("Frequency")
        
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"Working memory visualization saved to {output_file}")
        else:
            plt.show()
        
        return fig
        
    except ImportError:
        print("Matplotlib not available. Install with:")
        print("  pip install matplotlib")
        return None


def plot_learning_curves(nlm_api, output_file=None):
    """
    Plot learning curves from simulation data.
    
    Args:
        nlm_api: NLMAdvancedAPI instance
        output_file: File path to save visualization
    
    Returns:
        matplotlib figure
    """
    try:
        import matplotlib.pyplot as plt
        import numpy as np
        
        if not hasattr(nlm_api, '_performance_stats'):
            print("No performance data available")
            return None
        
        perf_stats = nlm_api._performance_stats
        
        if not perf_stats['prediction_accuracies']:
            print("No prediction data available for learning curves")
            return None
        
        # Extract prediction accuracies
        steps = [data['step'] for data in perf_stats['prediction_accuracies']]
        accuracies = [data['accuracy'] for data in perf_stats['prediction_accuracies']]
        
        # Create figure
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Plot 1: Prediction accuracy over time
        ax1 = axes[0, 0]
        ax1.plot(steps, accuracies, 'b-', linewidth=2, alpha=0.7)
        ax1.set_title("Prediction Accuracy Over Time")
        ax1.set_xlabel("Simulation Step")
        ax1.set_ylabel("Accuracy")
        ax1.set_ylim(0, 1)
        ax1.grid(True, alpha=0.3)
        
        # Add moving average
        window_size = 10
        if len(accuracies) >= window_size:
            moving_avg = np.convolve(accuracies, np.ones(window_size)/window_size, mode='valid')
            valid_steps = steps[window_size-1:]
            ax1.plot(valid_steps, moving_avg, 'r-', linewidth=2, label=f'{window_size}-step MA')
            ax1.legend()
        
        # Plot 2: Accuracy histogram
        ax2 = axes[0, 1]
        ax2.hist(accuracies, bins=20, alpha=0.7, color='lightgreen', edgecolor='black')
        ax2.set_title("Prediction Accuracy Distribution")
        ax2.set_xlabel("Accuracy")
        ax2.set_ylabel("Frequency")
        ax2.axvline(np.mean(accuracies), color='red', linestyle='--', label=f"Mean: {np.mean(accuracies):.2%}")
        ax2.legend()
        
        # Plot 3: Confidence intervals
        ax3 = axes[1, 0]
        
        # Calculate rolling statistics
        window = 20
        if len(accuracies) >= window:
            rolling_mean = np.convolve(accuracies, np.ones(window)/window, mode='valid')
            rolling_std = np.array([np.std(accuracies[max(0,i-window+1):i+1]) 
                                   for i in range(len(accuracies)-window+1)])
            valid_steps = steps[window-1:]
            
            ax3.plot(valid_steps, rolling_mean, 'b-', linewidth=2, label='Rolling Mean')
            ax3.fill_between(valid_steps, 
                            rolling_mean - rolling_std,
                            rolling_mean + rolling_std,
                            alpha=0.3, color='blue', label='±1 Std Dev')
            ax3.set_title("Rolling Prediction Statistics")
            ax3.set_xlabel("Simulation Step")
            ax3.set_ylabel("Accuracy")
            ax3.legend()
            ax3.grid(True, alpha=0.3)
        
        # Plot 4: Performance summary
        ax4 = axes[1, 1]
        ax4.axis('off')
        
        summary_text = f"Prediction Performance Summary:\n\n"
        summary_text += f"Total evaluations: {len(accuracies)}\n"
        summary_text += f"Final accuracy: {accuracies[-1]:.2%}\n"
        summary_text += f"Average accuracy: {np.mean(accuracies):.2%}\n"
        summary_text += f"Best accuracy: {max(accuracies):.2%}\n"
        summary_text += f"Accuracy std: {np.std(accuracies):.3f}\n"
        
        if len(accuracies) >= window:
            summary_text += f"\nLast {window} steps:\n"
            summary_text += f"Mean: {rolling_mean[-1]:.2%}\n"
            summary_text += f"Std: {rolling_std[-1]:.3f}\n"
        
        ax4.text(0.1, 0.5, summary_text, fontsize=10, fontfamily='monospace',
                verticalalignment='center', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
        ax4.set_title("Performance Summary")
        
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"Learning curves saved to {output_file}")
        else:
            plt.show()
        
        return fig
        
    except ImportError:
        print("Matplotlib not available. Install with:")
        print("  pip install matplotlib")
        return None


# Convenience functions
def create_example_visualization(brain, output_dir="examples/visualizations"):
    """
    Create example visualizations for documentation.
    
    Args:
        brain: Brain object to simulate
        output_dir: Directory to save visualizations
    """
    os.makedirs(output_dir, exist_ok=True)
    
    # Create connectivity plot
    plot_brain_connectivity(brain, os.path.join(output_dir, "connectivity.png"))
    
    # Create activity visualization
    visualize_neural_activity(brain, duration=200, 
                             output_file=os.path.join(output_dir, "activity.png"))
    
    print(f"Example visualizations saved to {output_dir}")


def create_advanced_visualization(nlm_api, output_dir="examples/advanced_visualizations"):
    """
    Create advanced visualizations with monitoring data.
    
    Args:
        nlm_api: NLMAdvancedAPI instance
        output_dir: Directory to save visualizations
    """
    os.makedirs(output_dir, exist_ok=True)
    
    # Create working memory visualization
    visualize_working_memory(nlm_api, os.path.join(output_dir, "working_memory.png"))
    
    # Create learning curves
    plot_learning_curves(nlm_api, os.path.join(output_dir, "learning_curves.png"))
    
    print(f"Advanced visualizations saved to {output_dir}")


if __name__ == "__main__":
    print("NLM Visualization Tools")
    print("=" * 30)
    print("This module provides visualization tools for NLM simulations.")
    print("Import the functions directly or use the convenience functions.")
    print("\nAvailable functions:")
    print("  - plot_brain_connectivity(brain, output_file)")
    print("  - visualize_neural_activity(brain, duration, output_file)")
    print("  - visualize_working_memory(nlm_api, output_file)")
    print("  - plot_learning_curves(nlm_api, output_file)")
    print("  - create_example_visualization(brain, output_dir)")
    print("  - create_advanced_visualization(nlm_api, output_dir)")
