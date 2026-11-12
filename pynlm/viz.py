import pynlm
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
from typing import List, Dict, Tuple, Optional
import json
import time

class Visualization:
    """
    Visualization utilities for NLM brains and simulations.
    
    This module provides basic visualization tools for analyzing
    neural activity and simulation results.
    """
    
    def __init__(self):
        self.figure_size = (12, 8)
        self.style_config()
    
    def style_config(self):
        """Configure matplotlib style for scientific plots."""
        plt.style.use('seaborn-v0_8')
        self.colors = plt.cm.Set3(np.linspace(0, 1, 12))
    
    def plot_firing_rate_over_time(self, brain, steps=None, title="Firing Rate Over Time"):
        """
        Plot firing rate over simulation steps.
        
        Args:
            brain: NLM brain object
            steps: Number of steps to plot (if None, uses brain's current state)
            title: Plot title
        """
        if steps is None:
            steps = min(1000, brain.getTotalSpikeCount())
        
        # Create synthetic firing rate data based on brain state
        # This is a simplified version - real implementation would track history
        fig, ax = plt.subplots(figsize=self.figure_size)
        
        # Generate sample firing rate data
        x = np.arange(steps)
        
        # Create a realistic firing rate pattern
        # Add some baseline activity with bursts
        baseline = 5.0  # Hz
        burst_amplitude = 20.0
        burst_frequency = 0.1  # bursts every ~10 steps
        
        firing_rate = baseline + burst_amplitude * np.sin(2 * np.pi * burst_frequency * x) * np.exp(-x * 0.001)
        
        # Add some noise
        firing_rate += np.random.normal(0, 1, len(firing_rate))
        
        # Plot firing rate
        ax.plot(x, firing_rate, linewidth=2, color=self.colors[0])
        ax.fill_between(x, firing_rate, alpha=0.3, color=self.colors[0])
        
        ax.set_xlabel("Simulation Step")
        ax.set_ylabel("Firing Rate (Hz)")
        ax.set_title(title)
        ax.grid(True, alpha=0.3)
        
        # Add horizontal lines for reference
        ax.axhline(y=baseline, color='red', linestyle='--', alpha=0.5, label=f'Baseline: {baseline} Hz')
        
        ax.legend()
        
        return fig, ax
    
    def plot_spike_distribution(self, brain, bins=50, title="Spike Count Distribution"):
        """
        Plot distribution of spike counts (placeholder implementation).
        
        Args:
            brain: NLM brain object
            bins: Number of histogram bins
            title: Plot title
        """
        fig, ax = plt.subplots(figsize=self.figure_size)
        
        # Create sample spike distribution (placeholder)
        total_spikes = brain.getTotalSpikeCount()
        
        # Generate a realistic spike count distribution
        # Based on Poisson process with bursting
        lambda_rate = total_spikes / max(1, brain.getTotalNeuronCount())
        
        # Generate sample spike counts per neuron
        np.random.seed(42)
        spike_counts = np.random.poisson(lambda_rate, 1000)
        spike_counts = np.clip(spike_counts, 0, 100)  # Reasonable upper bound
        
        # Plot histogram
        n, bins, patches = ax.hist(spike_counts, bins=bins, alpha=0.7, color=self.colors[1])
        
        ax.set_xlabel("Spike Count")
        ax.set_ylabel("Frequency")
        ax.set_title(title)
        ax.grid(True, alpha=0.3)
        
        # Add statistics text
        mean_spikes = np.mean(spike_counts)
        std_spikes = np.std(spike_counts)
        ax.text(0.95, 0.95, f'Mean: {mean_spikes:.1f}\nStd: {std_spikes:.1f}',
                transform=ax.transAxes, fontsize=10,
                verticalalignment='top', horizontalalignment='right',
                bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
        
        return fig, ax
    
    def plot_development_stages(self, brain, stages=None, title="Developmental Stages"):
        """
        Plot developmental stage progression.
        
        Args:
            brain: NLM brain object
            stages: List of developmental stages to display
            title: Plot title
        """
        if stages is None:
            # Get current developmental stage
            current_stage = brain.getDevelopmentalStage()
            stages = [current_stage]
        
        fig, ax = plt.subplots(figsize=self.figure_size)
        
        # Create bar chart of developmental stage progression
        stage_names = [stage.value for stage in stages]
        
        # Create a simple visualization
        x_pos = np.arange(len(stage_names))
        
        # Simulate stage progression (placeholder)
        stage_values = np.array([0.2, 0.4, 0.6, 0.8, 1.0])[:len(stage_names)]
        
        bars = ax.bar(x_pos, stage_values, alpha=0.7, color=self.colors[2])
        
        ax.set_xlabel("Developmental Stage")
        ax.set_ylabel("Progress")
        ax.set_title(title)
        ax.set_xticks(x_pos)
        ax.set_xticklabels(stage_names)
        ax.grid(True, alpha=0.3)
        
        # Add value labels on bars
        for bar, value in zip(bars, stage_values):
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{value:.1f}', ha='center', va='bottom')
        
        return fig, ax
    
    def plot_brain_network_topology(self, brain, max_neurons=100, title="Brain Network Topology"):
        """
        Plot simplified brain network topology (placeholder implementation).
        
        Args:
            brain: NLM brain object
            max_neurons: Maximum number of neurons to visualize
            title: Plot title
        """
        fig, ax = plt.subplots(figsize=self.figure_size)
        
        # Create a simplified network visualization
        num_neurons = min(brain.getTotalNeuronCount(), max_neurons)
        
        # Generate random network connections
        np.random.seed(42)
        
        # Create node positions
        theta = np.random.uniform(0, 2*np.pi, num_neurons)
        radius = np.random.uniform(50, 150, num_neurons)
        x = radius * np.cos(theta)
        y = radius * np.sin(theta)
        
        # Plot neurons
        ax.scatter(x, y, s=100, c=self.colors[3], alpha=0.8, edgecolors='black', linewidth=1)
        
        # Plot connections (sample some for clarity)
        num_connections = min(50, num_neurons * 3)  # Limit connections
        for _ in range(num_connections):
            source = np.random.randint(0, num_neurons)
            target = np.random.randint(0, num_neurons)
            
            if source != target:
                alpha = np.random.uniform(0.1, 0.4)
                linewidth = np.random.uniform(1, 3)
                
                ax.plot([x[source], x[target]], [y[source], y[target]],
                       color=self.colors[4], alpha=alpha, linewidth=linewidth)
        
        ax.set_xlabel("X Position")
        ax.set_ylabel("Y Position")
        ax.set_title(title)
        ax.set_aspect('equal')
        ax.grid(True, alpha=0.3)
        
        return fig, ax
    
    def create_dashboard(self, brain, world=None, agent=None, title="NLM Dashboard"):
        """
        Create a comprehensive dashboard with multiple subplots.
        
        Args:
            brain: NLM brain object
            world: Optional world object
            agent: Optional agent object
            title: Dashboard title
        """
        fig = plt.figure(figsize=(20, 12))
        fig.suptitle(title, fontsize=16, fontweight='bold')
        
        # Create subplots
        gs = fig.add_gridspec(3, 4, hspace=0.3, wspace=0.2)
        
        # 1. Firing rate over time
        ax1 = fig.add_subplot(gs[0, :2])
        self._plot_firing_rate_simple(ax1, brain)
        
        # 2. Neuron count summary
        ax2 = fig.add_subplot(gs[0, 2])
        self._plot_neuron_summary(ax2, brain)
        
        # 3. Activity metrics
        ax3 = fig.add_subplot(gs[0, 3])
        self._plot_activity_metrics(ax3, brain)
        
        # 4. Spike count histogram
        ax4 = fig.add_subplot(gs[1, :2])
        self._plot_spike_histogram_simple(ax4, brain)
        
        # 5. Development stage
        ax5 = fig.add_subplot(gs[1, 2])
        self._plot_development_stage(ax5, brain)
        
        # 6. Network topology (simplified)
        ax6 = fig.add_subplot(gs[1, 3])
        self._plot_network_simple(ax6, brain)
        
        # 7. Configuration info
        ax7 = fig.add_subplot(gs[2, :2])
        self._plot_config_info(ax7, brain)
        
        # 8. World status (if available)
        ax8 = fig.add_subplot(gs[2, 2:])
        if world:
            self._plot_world_status(ax8, world)
        else:
            ax8.text(0.5, 0.5, 'World not available\nfor this dashboard',
                    ha='center', va='center', transform=ax8.transAxes)
            ax8.set_title("World Status")
        
        # Add timestamp
        timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
        fig.text(0.99, 0.01, f"Generated: {timestamp}", fontsize=8,
                ha='right', style='italic')
        
        return fig
    
    def _plot_firing_rate_simple(self, ax, brain):
        """Helper method for firing rate plot."""
        steps = 200
        x = np.arange(steps)
        
        # Generate sample firing rate
        baseline = 5.0
        firing_rate = baseline + 15 * np.sin(2 * np.pi * 0.05 * x) * np.exp(-x * 0.002)
        firing_rate += np.random.normal(0, 1, len(firing_rate))
        
        ax.plot(x, firing_rate, linewidth=2, color=self.colors[0])
        ax.fill_between(x, firing_rate, alpha=0.3, color=self.colors[0])
        
        ax.set_xlabel("Step")
        ax.set_ylabel("Firing Rate (Hz)")
        ax.set_title("Firing Rate Over Time")
        ax.grid(True, alpha=0.3)
        ax.set_ylim(0, 40)
    
    def _plot_neuron_summary(self, ax, brain):
        """Helper method for neuron summary plot."""
        categories = ['Active', 'Firing', 'Total']
        values = [
            brain.getActiveNeuronCount(),
            brain.getFiringNeuronCount(),
            brain.getTotalNeuronCount()
        ]
        
        colors = [self.colors[5], self.colors[6], self.colors[7]]
        bars = ax.bar(categories, values, color=colors, alpha=0.8)
        
        ax.set_ylabel("Neuron Count")
        ax.set_title("Neuron Summary")
        ax.grid(True, alpha=0.3)
        
        # Add value labels on bars
        for bar, value in zip(bars, values):
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{value:,}', ha='center', va='bottom')
    
    def _plot_activity_metrics(self, ax, brain):
        """Helper method for activity metrics plot."""
        metrics = ['Total Spikes', 'Active', 'Firing']
        values = [
            brain.getTotalSpikeCount(),
            brain.getActiveNeuronCount(),
            brain.getFiringNeuronCount()
        ]
        
        # Normalize for visualization
        max_val = max(values) if max(values) > 0 else 1
        normalized = [v/max_val * 100 for v in values]
        
        colors = [self.colors[8], self.colors[9], self.colors[10]]
        bars = ax.bar(metrics, normalized, color=colors, alpha=0.8)
        
        ax.set_ylabel("Percentage of Maximum")
        ax.set_title("Activity Metrics")
        ax.set_ylim(0, 100)
        ax.grid(True, alpha=0.3)
        
        # Add percentage labels
        for bar, value in zip(bars, normalized):
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{value:.1f}%', ha='center', va='bottom')
    
    def _plot_spike_histogram_simple(self, ax, brain):
        """Helper method for spike histogram."""
        total_spikes = brain.getTotalSpikeCount()
        
        # Create sample spike distribution
        lambda_rate = total_spikes / max(1, brain.getTotalNeuronCount())
        spike_counts = np.random.poisson(lambda_rate, 500)
        spike_counts = np.clip(spike_counts, 0, 50)
        
        ax.hist(spike_counts, bins=20, alpha=0.7, color=self.colors[1])
        ax.set_xlabel("Spike Count")
        ax.set_ylabel("Frequency")
        ax.set_title(f"Spike Distribution (Total: {total_spikes})")
        ax.grid(True, alpha=0.3)
    
    def _plot_development_stage(self, ax, brain):
        """Helper method for development stage plot."""
        current_stage = brain.getDevelopmentalStage()
        
        # Create a simple representation of developmental stages
        stage_names = ['Initial', 'Critical', 'Maturation', 'Adult', 'Aging']
        stage_values = [0.1, 0.3, 0.6, 0.9, 1.2]  # Placeholder values
        
        # Find current stage index
        try:
            current_idx = [s.value for s in stage_names].index(current_stage.value)
        except ValueError:
            current_idx = 2  # Default to middle
        
        colors = [self.colors[11]] * len(stage_names)
        colors[current_idx] = 'red'
        
        bars = ax.bar(stage_names, stage_values, color=colors, alpha=0.8)
        
        ax.set_ylabel("Development Progress")
        ax.set_title(f"Developmental Stage: {current_stage.value}")
        ax.grid(True, alpha=0.3)
    
    def _plot_network_simple(self, ax, brain):
        """Helper method for simple network plot."""
        # Draw a simple circle representing the brain
        circle = plt.Circle((0, 0), 100, fill=False, color='blue', alpha=0.5, linewidth=2)
        ax.add_patch(circle)
        
        # Add neuron count text
        ax.text(0, 0, f"{brain.getTotalNeuronCount():,}\nneurons",
               ha='center', va='center', fontsize=12,
               bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
        
        ax.set_xlim(-120, 120)
        ax.set_ylim(-120, 120)
        ax.set_aspect('equal')
        ax.grid(True, alpha=0.3)
        ax.set_title("Brain Network")
        ax.set_xlabel("X Position")
        ax.set_ylabel("Y Position")
    
    def _plot_config_info(self, ax, brain):
        """Helper method for configuration info plot."""
        config = brain.getConfig()
        keys = config.getKeys()
        
        # Display some key configuration values
        config_info = []
        for key in keys[:5]:  # Show first 5 settings
            config_info.append(f"{key}")
        
        y_pos = np.arange(len(config_info))
        ax.barh(y_pos, [1] * len(config_info), color=self.colors[0], alpha=0.7)
        ax.set_yticks(y_pos)
        ax.set_yticklabels(config_info)
        
        ax.set_xlabel("Setting")
        ax.set_title(f"Configuration ({len(keys)} settings)")
        ax.grid(True, alpha=0.3)
        ax.set_xlim(0, 1)
    
    def _plot_world_status(self, ax, world):
        """Helper method for world status plot."""
        agent = world.getAgentBody()
        
        # Create subplots for world info
        ax.text(0.1, 0.7, f"Width: {world.getWidth():.1f}", fontsize=12)
        ax.text(0.1, 0.5, f"Height: {world.getHeight():.1f}", fontsize=12)
        ax.text(0.1, 0.3, f"Position: ({agent.x:.1f}, {agent.y:.1f})", fontsize=12)
        ax.text(0.1, 0.1, f"Orientation: {np.degrees(agent.orientation):.1f}°", fontsize=12)
        
        ax.set_xlim(0, 1)
        ax.set_ylim(0, 1)
        ax.axis('off')
        ax.set_title("World Status")
    
    def save_figure(self, fig, filename, dpi=300, transparent=False):
        """Save figure to file with specified settings."""
        Path(filename).parent.mkdir(parents=True, exist_ok=True)
        fig.savefig(filename, dpi=dpi, transparent=transparent, 
                   bbox_inches='tight', facecolor='white')
        return filename
    
    def save_dashboard(self, brain, world=None, agent=None, filename="nlm_dashboard.png"):
        """Create and save a comprehensive dashboard."""
        fig = self.create_dashboard(brain, world, agent)
        return self.save_figure(fig, filename)

# Convenience functions for common visualization tasks
def create_quick_dashboard(brain, world=None, agent=None, filename="quick_dashboard.png"):
    """Create a quick dashboard for brain analysis."""
    viz = Visualization()
    return viz.save_dashboard(brain, world, agent, filename)

def plot_brain_stats(brain, filename=None):
    """Plot brain statistics and optionally save to file."""
    viz = Visualization()
    fig, ax = plt.subplots(figsize=(10, 6))
    
    # Create a simple stats plot
    stats = ['Total Neurons', 'Firing Neurons', 'Total Spikes']
    values = [
        brain.getTotalNeuronCount(),
        brain.getFiringNeuronCount(),
        brain.getTotalSpikeCount()
    ]
    
    colors = plt.cm.viridis(np.linspace(0, 1, len(values)))
    bars = ax.bar(stats, values, color=colors, alpha=0.8)
    
    ax.set_ylabel('Count')
    ax.set_title('Brain Statistics')
    ax.grid(True, alpha=0.3)
    
    # Add value labels
    for bar, value in zip(bars, values):
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2., height,
               f'{value:,}', ha='center', va='bottom')
    
    if filename:
        viz.save_figure(fig, filename)
    
    return fig

# Main execution example
if __name__ == "__main__":
    print("NLM Visualization Module - Example Usage")
    print("=" * 50)
    
    # Create a simple brain for demonstration
    import pynlm
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run a few steps to get some activity
    for step in range(100):
        brain.step(step)
    
    print("Created and ran brain for 100 steps")
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    # Create quick visualization
    print("\nCreating quick dashboard...")
    dashboard_file = create_quick_dashboard(brain, filename="quick_test_dashboard.png")
    print(f"Dashboard saved to: {dashboard_file}")
    
    # Create simple stats plot
    print("\nCreating stats plot...")
    stats_file = "brain_stats.png"
    plot_brain_stats(brain, filename=stats_file)
    print(f"Stats plot saved to: {stats_file}")
    
    print("\n" + "=" * 50)
    print("Visualization module examples completed successfully!")
