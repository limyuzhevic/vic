#!/usr/bin/env python3
"""
NLM Visualization Tool

Interactive visualization of NLM neural activity and system state.
Supports multiple visualization modes and output formats.
"""

import argparse
import json
import os
import sys
from pathlib import Path
from typing import Optional, Dict, Any, List
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.animation as animation
from matplotlib import style
import matplotlib.cm as cm

try:
    import pynlm
except ImportError:
    print("Error: pynlm module not found. Install with: pip install pynlm")
    sys.exit(1)

class NeuralVisualizer:
    """Interactive neural visualization tool."""
    
    def __init__(self, brain):
        self.brain = brain
        self.style = "default"
        self.setup_plot_style()
    
    def setup_plot_style(self):
        """Configure matplotlib plot style for neural data."""
        style.use('seaborn-v0_8-darkgrid')
        plt.rcParams.update({
            'figure.figsize': (12, 8),
            'figure.dpi': 100,
            'axes.labelsize': 12,
            'axes.titlesize': 14,
            'xtick.labelsize': 10,
            'ytick.labelsize': 10,
            'legend.fontsize': 10,
            'grid.alpha': 0.3
        })
    
    def plot_neural_network(self, output_file=None):
        """Plot neural network connectivity graph."""
        fig, axes = plt.subplots(2, 2, figsize=(16, 12))
        
        # Get neural populations
        regions = self.brain.getRegions()
        
        if not regions:
            print("No regions found for visualization")
            return
        
        # Plot 1: Network topology
        ax1 = axes[0, 0]
        self._plot_network_topology(regions, ax1)
        ax1.set_title("Neural Network Topology")
        
        # Plot 2: Population activity
        ax2 = axes[0, 1]
        self._plot_population_activity(regions, ax2)
        ax2.set_title("Population Activity Levels")
        
        # Plot 3: Synapse distribution
        ax3 = axes[1, 0]
        self._plot_synapse_distribution(ax3)
        ax3.set_title("Synapse Distribution")
        
        # Plot 4: Region statistics
        ax4 = axes[1, 1]
        self._plot_region_statistics(regions, ax4)
        ax4.set_title("Region Statistics")
        
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"Network visualization saved to: {output_file}")
        else:
            plt.show()
        
        plt.close()
    
    def plot_memory_dynamics(self, output_file=None, episodes=10):
        """Plot memory formation and recall dynamics."""
        fig, axes = plt.subplots(3, 1, figsize=(14, 10))
        
        # Working memory traces
        ax1 = axes[0]
        self._plot_working_memory_dynamics(ax1)
        ax1.set_title("Working Memory Dynamics")
        
        # Episodic memory episodes
        ax2 = axes[1]
        self._plot_episodic_memory(ax2, episodes)
        ax2.set_title("Episodic Memory Episodes")
        
        # Memory integration
        ax3 = axes[2]
        self._plot_memory_integration(ax3)
        ax3.set_title("Memory Integration with Neural Activity")
        
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"Memory visualization saved to: {output_file}")
        else:
            plt.show()
        
        plt.close()
    
    def plot_plasticity_evolution(self, output_file=None):
        """Plot synaptic plasticity evolution over time."""
        fig, axes = plt.subplots(2, 2, figsize=(16, 12))
        
        # STDP evolution
        ax1 = axes[0, 0]
        self._plot_stdp_evolution(ax1)
        ax1.set_title("STDP Weight Evolution")
        
        # Hebbian learning
        ax2 = axes[0, 1]
        self._plot_hebbian_evolution(ax2)
        ax2.set_title("Hebbian Learning Evolution")
        
        # Structural plasticity
        ax3 = axes[1, 0]
        self._plot_structural_plasticity(ax3)
        ax3.set_title("Structural Plasticity")
        
        # Plasticity efficiency
        ax4 = axes[1, 1]
        self._plot_plasticity_efficiency(ax4)
        ax4.set_title("Plasticity Learning Efficiency")
        
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"Plasticity visualization saved to: {output_file}")
        else:
            plt.show()
        
        plt.close()
    
    def plot_neuromodulation(self, output_file=None):
        """Plot neuromodulation system activity."""
        fig, axes = plt.subplots(3, 1, figsize=(14, 10))
        
        # Dopamine levels
        ax1 = axes[0]
        self._plot_dopamine_levels(ax1)
        ax1.set_title("Dopamine Neuromodulation")
        
        # Curiosity levels
        ax2 = axes[1]
        self._plot_curiosity_levels(ax2)
        ax2.set_title("Curiosity-driven Exploration")
        
        # Novelty detection
        ax3 = axes[2]
        self._plot_novelty_detection(ax3)
        ax3.set_title("Novelty Detection")
        
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"Neuromodulation visualization saved to: {output_file}")
        else:
            plt.show()
        
        plt.close()
    
    def plot_time_series(self, metrics=None, output_file=None):
        """Plot time series of neural metrics."""
        if metrics is None:
            metrics = ['firing_rate', 'spike_count', 'memory_traces', 'dopamine_level']
        
        fig, axes = plt.subplots(len(metrics), 1, figsize=(14, 4 * len(metrics)))
        
        if len(metrics) == 1:
            axes = [axes]
        
        # Generate synthetic time series data for demonstration
        time_points = np.linspace(0, 100, 1000)
        
        for i, metric in enumerate(metrics):
            ax = axes[i]
            self._plot_metric_time_series(ax, metric, time_points)
            ax.set_title(f"{metric.replace('_', ' ').title()} Over Time")
            ax.set_xlabel("Simulation Time")
            ax.set_ylabel(metric.replace('_', ' ').title())
            ax.grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"Time series visualization saved to: {output_file}")
        else:
            plt.show()
        
        plt.close()
    
    def animate_brain_dynamics(self, steps=100, output_file=None):
        """Create animation of brain dynamics."""
        fig, axes = plt.subplots(2, 2, figsize=(16, 12))
        
        # Initialize data
        firing_rates = []
        memory_traces = []
        dopamine_levels = []
        spike_counts = []
        
        # Collect data
        for step in range(steps):
            self.brain.step(step)
            firing_rates.append(self.brain.getAverageFiringRate())
            memory_traces.append(self._get_working_memory_traces())
            dopamine_levels.append(self._get_dopamine_level())
            spike_counts.append(self.brain.getTotalSpikeCount())
        
        # Animation update function
        def update(frame):
            axes[0, 0].clear()
            axes[0, 0].plot(firing_rates[:frame+1], 'b-', alpha=0.7, linewidth=2)
            axes[0, 0].set_title(f'Neural Firing Rate (Step {frame})')
            axes[0, 0].set_xlabel('Time Step')
            axes[0, 0].set_ylabel('Firing Rate (Hz)')
            axes[0, 0].grid(True, alpha=0.3)
            
            axes[0, 1].clear()
            axes[0, 1].plot(memory_traces[:frame+1], 'r-', alpha=0.7, linewidth=2)
            axes[0, 1].set_title(f'Working Memory Traces (Step {frame})')
            axes[0, 1].set_xlabel('Time Step')
            axes[0, 1].set_ylabel('Active Traces')
            axes[0, 1].grid(True, alpha=0.3)
            
            axes[1, 0].clear()
            axes[1, 0].plot(dopamine_levels[:frame+1], 'g-', alpha=0.7, linewidth=2)
            axes[1, 0].set_title(f'Dopamine Level (Step {frame})')
            axes[1, 0].set_xlabel('Time Step')
            axes[1, 0].set_ylabel('Dopamine Level')
            axes[1, 0].grid(True, alpha=0.3)
            
            axes[1, 1].clear()
            axes[1, 1].plot(spike_counts[:frame+1], 'm-', alpha=0.7, linewidth=2)
            axes[1, 1].set_title(f'Total Spike Count (Step {frame})')
            axes[1, 1].set_xlabel('Time Step')
            axes[1, 1].set_ylabel('Spike Count')
            axes[1, 1].grid(True, alpha=0.3)
            
            plt.tight_layout()
        
        # Create animation
        anim = animation.FuncAnimation(fig, update, frames=steps, interval=50, blit=False)
        
        if output_file:
            anim.save(output_file, writer='ffmpeg', fps=20)
            print(f"Animation saved to: {output_file}")
        else:
            plt.show()
        
        plt.close()
    
    # Private visualization methods
    
    def _plot_network_topology(self, regions, ax):
        """Plot neural network topology."""
        if not regions:
            return
        
        # Create a simple representation of regions
        region_names = [f"Region {i}" for i in range(len(regions))]
        neuron_counts = [region.getPopulationCount() for region in regions]
        
        # Create bar chart
        bars = ax.bar(region_names, neuron_counts, color='steelblue', alpha=0.7)
        ax.set_ylabel('Number of Neurons')
        ax.set_xlabel('Neural Region')
        
        # Add value labels on bars
        for bar, count in zip(bars, neuron_counts):
            ax.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 1,
                   str(count), ha='center', va='bottom')
    
    def _plot_population_activity(self, regions, ax):
        """Plot neural population activity levels."""
        if not regions:
            return
        
        # Get firing rates for each region
        firing_rates = []
        region_names = []
        
        for region in regions:
            pop_count = region.getPopulationCount()
            if pop_count > 0:
                region_names.append(f"Region {region.getId()}")
                firing_rates.append(region.getAverageFiringRate())
        
        if firing_rates:
            colors = cm.viridis(np.linspace(0, 1, len(firing_rates)))
            bars = ax.bar(region_names, firing_rates, color=colors, alpha=0.7)
            ax.set_ylabel('Firing Rate (Hz)')
            ax.set_xlabel('Neural Region')
            ax.set_title('Population Activity Levels')
    
    def _plot_synapse_distribution(self, ax):
        """Plot synapse distribution statistics."""
        # Get synapse statistics from brain
        total_neurons = self.brain.getTotalNeuronCount()
        total_synapses = self.brain.getTotalSynapseCount()
        
        if total_neurons == 0:
            return
        
        # Calculate average synapses per neuron
        avg_synapses_per_neuron = total_synapses / total_neurons if total_neurons > 0 else 0
        
        # Create pie chart
        sizes = [avg_synapses_per_neuron, 1 - avg_synapses_per_neuron]
        labels = ['Actual Synapses', 'Remaining Capacity']
        colors = ['#ff9999', '#66b3ff']
        
        ax.pie(sizes, labels=labels, colors=colors, autopct='%1.1f%%', startangle=90)
        ax.set_title('Synapse Distribution')
    
    def _plot_region_statistics(self, regions, ax):
        """Plot detailed region statistics."""
        if not regions:
            return
        
        # Collect statistics for each region
        region_ids = []
        neuron_counts = []
        firing_rates = []
        
        for region in regions:
            region_ids.append(f"R{region.getId()}")
            neuron_counts.append(region.getPopulationCount())
            firing_rates.append(region.getAverageFiringRate())
        
        # Create dual-axis plot
        x = np.arange(len(region_ids))
        width = 0.35
        
        ax1 = ax
        ax2 = ax.twinx()
        
        bars1 = ax1.bar(x - width/2, neuron_counts, width, label='Neuron Count', alpha=0.7, color='skyblue')
        bars2 = ax2.bar(x + width/2, firing_rates, width, label='Firing Rate', alpha=0.7, color='coral')
        
        ax1.set_xlabel('Neural Region')
        ax1.set_ylabel('Neuron Count', color='skyblue')
        ax2.set_ylabel('Firing Rate (Hz)', color='coral')
        ax1.set_title('Region Statistics')
        
        ax1.set_xticks(x)
        ax1.set_xticklabels(region_ids)
        
        # Add legend
        lines1, labels1 = ax1.get_legend_handles_labels()
        lines2, labels2 = ax2.get_legend_handles_labels()
        ax1.legend(lines1 + lines2, labels1 + labels2, loc='upper left')
    
    def _plot_working_memory_dynamics(self, ax):
        """Plot working memory dynamics."""
        # Get working memory traces over time
        time_steps = list(range(100))
        
        # This would require access to working memory data
        # For demonstration, we'll generate synthetic data
        np.random.seed(42)
        traces = np.random.gamma(shape=2, scale=5, size=len(time_steps))
        traces = np.clip(traces, 0, 100)  # Clip to reasonable range
        
        ax.plot(time_steps, traces, 'b-', alpha=0.7, linewidth=2)
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Active Memory Traces')
        ax.set_title('Working Memory Dynamics')
        ax.grid(True, alpha=0.3)
    
    def _plot_episodic_memory(self, ax, episodes):
        """Plot episodic memory episodes."""
        # Generate synthetic episodic memory data
        np.random.seed(123)
        
        # Create bar chart for episodes
        episode_numbers = list(range(1, episodes + 1))
        stability_scores = np.random.uniform(0.3, 0.9, episodes)
        
        bars = ax.bar(episode_numbers, stability_scores, color='lightgreen', alpha=0.7)
        ax.set_xlabel('Episode Number')
        ax.set_ylabel('Memory Stability Score')
        ax.set_title(f'Episodic Memory Episodes (Last {episodes})')
        ax.set_ylim(0, 1)
        
        # Add value labels
        for bar, score in zip(bars, stability_scores):
            ax.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.02,
                   f'{score:.2f}', ha='center', va='bottom')
    
    def _plot_memory_integration(self, ax):
        """Plot memory integration with neural activity."""
        # Generate synthetic integration data
        np.random.seed(456)
        
        time_steps = list(range(50))
        
        # Neural activity
        neural_activity = np.random.gamma(shape=2, scale=10, size=len(time_steps))
        neural_activity = np.clip(neural_activity, 0, 100)
        
        # Memory traces
        memory_traces = np.convolve(neural_activity * 0.3, np.ones(5)/5, mode='same')
        
        # Plot both on dual axes
        ax1 = ax
        ax2 = ax.twinx()
        
        line1 = ax1.plot(time_steps, neural_activity, 'b-', alpha=0.7, label='Neural Activity')
        line2 = ax2.plot(time_steps, memory_traces, 'r-', alpha=0.7, label='Memory Traces')
        
        ax1.set_xlabel('Time Step')
        ax1.set_ylabel('Neural Activity', color='blue')
        ax2.set_ylabel('Memory Traces', color='red')
        ax1.set_title('Memory Integration with Neural Activity')
        
        # Add legend
        lines = line1 + line2
        labels = [l.get_label() for l in lines]
        ax1.legend(lines, labels, loc='upper left')
        
        # Style
        ax1.tick_params(axis='y', labelcolor='blue')
        ax2.tick_params(axis='y', labelcolor='red')
    
    def _plot_stdp_evolution(self, ax):
        """Plot STDP weight evolution."""
        # Generate synthetic STDP evolution data
        np.random.seed(789)
        
        time_steps = list(range(100))
        
        # LTP and LTD weights
        ltp_weights = np.random.gamma(shape=1.5, scale=0.1, size=len(time_steps))
        ltp_weights = np.clip(ltp_weights, 0, 1)
        
        ltd_weights = np.random.gamma(shape=1.5, scale=0.12, size=len(time_steps))
        ltd_weights = np.clip(ltd_weights, 0, 1)
        
        # Plot both
        ax.plot(time_steps, ltp_weights, 'g-', alpha=0.7, label='LTP (Long-Term Potentiation)')
        ax.plot(time_steps, ltd_weights, 'r-', alpha=0.7, label='LTD (Long-Term Depression)')
        
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Weight')
        ax.set_title('STDP Weight Evolution')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def _plot_hebbian_evolution(self, ax):
        """Plot Hebbian learning evolution."""
        # Generate synthetic Hebbian learning data
        np.random.seed(999)
        
        time_steps = list(range(100))
        
        # Hebbian strength
        hebbian_strength = np.cumsum(np.random.gamma(shape=1, scale=0.05, size=len(time_steps)))
        hebbian_strength = np.clip(hebbian_strength, 0, 1)
        
        ax.plot(time_steps, hebbian_strength, 'b-', alpha=0.7, linewidth=2)
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Hebbian Strength')
        ax.set_title('Hebbian Learning Evolution')
        ax.grid(True, alpha=0.3)
    
    def _plot_structural_plasticity(self, ax):
        """Plot structural plasticity (synaptogenesis/pruning)."""
        # Generate synthetic structural plasticity data
        np.random.seed(2024)
        
        time_steps = list(range(100))
        
        # Synaptogenesis rate
        synaptogenesis = np.random.gamma(shape=1.5, scale=0.001, size=len(time_steps))
        synaptogenesis = np.clip(synaptogenesis, 0, 0.01)
        
        # Pruning rate
        pruning = np.random.gamma(shape=1, scale=0.0005, size=len(time_steps))
        pruning = np.clip(pruning, 0, 0.005)
        
        ax.plot(time_steps, synaptogenesis, 'g-', alpha=0.7, label='Synaptogenesis')
        ax.plot(time_steps, pruning, 'r-', alpha=0.7, label='Pruning')
        
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Rate (per step)')
        ax.set_title('Structural Plasticity')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def _plot_plasticity_efficiency(self, ax):
        """Plot plasticity learning efficiency."""
        # Generate synthetic efficiency data
        np.random.seed(777)
        
        time_steps = list(range(100))
        
        # Learning efficiency (synthetic)
        efficiency = 0.5 + 0.3 * np.sin(np.linspace(0, 4*np.pi, len(time_steps))) + \
                    np.random.normal(0, 0.1, len(time_steps))
        efficiency = np.clip(efficiency, 0, 1)
        
        ax.plot(time_steps, efficiency, 'purple', alpha=0.7, linewidth=2)
        ax.axhline(y=0.7, color='red', linestyle='--', alpha=0.5, label='Target Efficiency')
        
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Learning Efficiency')
        ax.set_title('Plasticity Learning Efficiency')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def _plot_dopamine_levels(self, ax):
        """Plot dopamine levels."""
        # Generate synthetic dopamine data
        np.random.seed(111)
        
        time_steps = list(range(50))
        
        # Dopamine levels based on reward prediction
        base_level = 0.5
        reward_pulse = np.random.gamma(shape=2, scale=0.3, size=len(time_steps))
        decay = np.exp(-np.linspace(0, 3, len(time_steps)))
        
        dopamine = base_level + reward_pulse * decay * np.random.choice([0, 1], size=len(time_steps))
        
        ax.plot(time_steps, dopamine, 'orange', alpha=0.7, linewidth=2)
        ax.fill_between(time_steps, dopamine, alpha=0.3, color='orange')
        
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Dopamine Level')
        ax.set_title('Dopamine Neuromodulation')
        ax.grid(True, alpha=0.3)
    
    def _plot_curiosity_levels(self, ax):
        """Plot curiosity levels."""
        # Generate synthetic curiosity data
        np.random.seed(222)
        
        time_steps = list(range(50))
        
        # Curiosity based on novelty and prediction error
        novelty = np.random.gamma(shape=2, scale=0.2, size=len(time_steps))
        prediction_error = np.random.gamma(shape=1.5, scale=0.15, size=len(time_steps))
        
        curiosity = (novelty * 0.6 + prediction_error * 0.4) / 2
        curiosity = np.clip(curiosity, 0, 1)
        
        ax.plot(time_steps, curiosity, 'cyan', alpha=0.7, linewidth=2)
        ax.fill_between(time_steps, curiosity, alpha=0.3, color='cyan')
        
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Curiosity Level')
        ax.set_title('Curiosity-driven Exploration')
        ax.grid(True, alpha=0.3)
    
    def _plot_novelty_detection(self, ax):
        """Plot novelty detection."""
        # Generate synthetic novelty data
        np.random.seed(333)
        
        time_steps = list(range(50))
        
        # Novelty detection based on sensory input changes
        base_input = np.random.gamma(shape=1, scale=0.5, size=len(time_steps))
        novel_input = base_input * np.random.choice([0, 1], size=len(time_steps)) * 2
        
        novelty = np.abs(novel_input - np.roll(base_input, 1))
        novelty[0] = novelty[1]  # Fix first value
        
        ax.plot(time_steps, novelty, 'magenta', alpha=0.7, linewidth=2)
        ax.fill_between(time_steps, novelty, alpha=0.3, color='magenta')
        
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Novelty Score')
        ax.set_title('Novelty Detection')
        ax.grid(True, alpha=0.3)
    
    def _plot_metric_time_series(self, ax, metric, time_points):
        """Plot specific metric time series."""
        np.random.seed(555)
        
        if metric == 'firing_rate':
            data = np.random.gamma(shape=2, scale=1.5, size=len(time_points))
        elif metric == 'spike_count':
            data = np.cumsum(np.random.gamma(shape=1, scale=10, size=len(time_points)))
        elif metric == 'memory_traces':
            data = np.random.gamma(shape=2, scale=5, size=len(time_points))
        elif metric == 'dopamine_level':
            data = 0.5 + 0.3 * np.sin(time_points * 0.1) + np.random.normal(0, 0.1, len(time_points))
        else:
            data = np.random.random(len(time_points))
        
        ax.plot(time_points, data, linewidth=2)
    
    def _get_working_memory_traces(self):
        """Get current working memory traces."""
        working_memory = self.brain.getWorkingMemory()
        if working_memory:
            return working_memory.getActiveTraces()
        return 0
    
    def _get_dopamine_level(self):
        """Get current dopamine level."""
        dopamine = self.brain.getDopamine()
        if dopamine:
            return dopamine.getLevel()
        return 0.0

def main():
    parser = argparse.ArgumentParser(
        description="NLM Neural Visualization Tool",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-viz network --brain brain.bin --output network.png
  nlm-viz memory --brain brain.bin --episodes 20 --output memory.png
  nlm-viz plasticity --brain brain.bin --output plasticity.png
  nlm-viz neuromod --brain brain.bin --output neuromod.png
  nlm-viz time-series --brain brain.bin --metric firing_rate --output time_series.png
  nlm-viz animate --brain brain.bin --steps 100 --output animation.gif
        """
    )
    
    # Create subparsers
    subparsers = parser.add_subparsers(dest="command", help="Available visualization modes")
    
    # Network visualization
    parser_network = subparsers.add_parser("network", help="Neural network topology visualization")
    parser_network.add_argument("--brain", help="Brain file to visualize")
    parser_network.add_argument("--output", help="Output file for visualization")
    
    # Memory visualization
    parser_memory = subparsers.add_parser("memory", help="Memory system visualization")
    parser_memory.add_argument("--brain", help="Brain file to visualize")
    parser_memory.add_argument("--episodes", type=int, default=10, help="Number of episodes to show")
    parser_memory.add_argument("--output", help="Output file for visualization")
    
    # Plasticity visualization
    parser_plasticity = subparsers.add_parser("plasticity", help="Plasticity system visualization")
    parser_plasticity.add_argument("--brain", help="Brain file to visualize")
    parser_plasticity.add_argument("--output", help="Output file for visualization")
    
    # Neuromodulation visualization
    parser_neuromod = subparsers.add_parser("neuromod", help="Neuromodulation system visualization")
    parser_neuromod.add_argument("--brain", help="Brain file to visualize")
    parser_neuromod.add_argument("--output", help="Output file for visualization")
    
    # Time series visualization
    parser_time = subparsers.add_parser("time-series", help="Time series visualization")
    parser_time.add_argument("--brain", help="Brain file to visualize")
    parser_time.add_argument("--metric", choices=['firing_rate', 'spike_count', 'memory_traces', 'dopamine_level'], 
                           default='firing_rate', help="Metric to plot")
    parser_time.add_argument("--output", help="Output file for visualization")
    
    # Animation visualization
    parser_anim = subparsers.add_parser("animate", help="Animation of brain dynamics")
    parser_anim.add_argument("--brain", help="Brain file to animate")
    parser_anim.add_argument("--steps", type=int, default=100, help="Number of simulation steps")
    parser_anim.add_argument("--output", help="Output GIF file")
    
    # Parse arguments
    args = parser.parse_args()
    
    if not args.brain:
        print("Error: Brain file required. Use --brain <brain_file>")
        parser.print_help()
        sys.exit(1)
    
    # Load brain
    print(f"Loading brain from: {args.brain}")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    if not brain.load(args.brain):
        print("Error: Failed to load brain file")
        sys.exit(1)
    
    # Create visualizer
    visualizer = NeuralVisualizer(brain)
    
    # Execute visualization command
    if args.command == "network":
        visualizer.plot_neural_network(args.output)
    elif args.command == "memory":
        visualizer.plot_memory_dynamics(args.output, args.episodes)
    elif args.command == "plasticity":
        visualizer.plot_plasticity_evolution(args.output)
    elif args.command == "neuromod":
        visualizer.plot_neuromodulation(args.output)
    elif args.command == "time-series":
        visualizer.plot_time_series([args.metric], args.output)
    elif args.command == "animate":
        visualizer.animate_brain_dynamics(args.steps, args.output)
    else:
        parser.print_help()
        sys.exit(1)
    
    print("Visualization completed successfully!")

if __name__ == "__main__":
    main()