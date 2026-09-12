"""Advanced visualization tools for NLM brain activity and simulation data."""

import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np
import pandas as pd
from typing import List, Optional, Dict, Tuple, Any
import json
import os
from pathlib import Path

from pynlm import (
    Brain,
    NeuronId,
    SpikeEvent,
    RegionId,
    NeuronType,
    SynapseType,
    SimulationStep,
    Timestamp,
)

class Visualization:
    """Advanced visualization tools for NLM brain activity and simulation data.
    
    Provides comprehensive plotting capabilities for brain activity visualization,
    spike raster plots, network topology, membrane potentials, and comparative analysis.
    
    Attributes:
        brain: Optional brain instance to visualize
        figure_size: Default figure size for plots
        style: Matplotlib style to use
    """
    
    def __init__(self, brain: Optional[Brain] = None, 
                 figure_size: Tuple[int, int] = (12, 8),
                 style: str = "seaborn-v0_8-darkgrid"):
        """Initialize visualization module.
        
        Args:
            brain: Optional brain instance to visualize
            figure_size: Default figure size for plots
            style: Matplotlib style to use
        """
        self.brain = brain
        self.figure_size = figure_size
        self.style = style
        self._setup_style()
        
        # Visualization history
        self._spike_history: List[SpikeEvent] = []
        self._activity_data: Dict[str, Any] = {}
        self._network_data: Dict[str, Any] = {}
    
    def _setup_style(self):
        """Setup matplotlib style for consistent plots."""
        plt.style.use(self.style)
        plt.rcParams['figure.figsize'] = self.figure_size
        plt.rcParams['font.size'] = 10
        plt.rcParams['axes.labelsize'] = 12
        plt.rcParams['axes.titlesize'] = 14
        plt.rcParams['xtick.labelsize'] = 10
        plt.rcParams['ytick.labelsize'] = 10
        plt.rcParams['legend.fontsize'] = 10
    
    def plot_spike_raster(self, spikes: Optional[List[SpikeEvent]] = None,
                         neurons: Optional[List[NeuronId]] = None,
                         duration: Optional[int] = None,
                         filename: Optional[str] = None,
                         colormap: str = "viridis"):
        """Plot spike raster diagram showing spike times across neurons.
        
        Args:
            spikes: List of spike events to plot (uses brain's spikes if None)
            neurons: List of neuron IDs to include (all neurons if None)
            duration: Duration in steps to plot (uses max spike time if None)
            filename: Output file path (optional)
            colormap: Colormap for coloring neurons
            
        Returns:
            matplotlib.Figure: The generated figure
            
        Raises:
            ValueError: If no spikes are provided or available
        """
        if spikes is None:
            if self.brain is None:
                raise ValueError("Must provide spikes or brain instance")
            spikes = self._get_brain_spikes()
        
        if not spikes:
            raise ValueError("No spike events to plot")
        
        # Extract neuron IDs and spike times
        neuron_ids = []
        spike_times = []
        
        for spike in spikes:
            neuron_ids.append(spike.source_neuron.value)
            spike_times.append(spike.step)
        
        # Filter by neurons if specified
        if neurons is not None:
            neuron_set = {n.value for n in neurons}
            filtered_indices = [i for i, nid in enumerate(neuron_ids) 
                              if nid in neuron_set]
            neuron_ids = [neuron_ids[i] for i in filtered_indices]
            spike_times = [spike_times[i] for i in filtered_indices]
        
        # Create figure
        fig, ax = plt.subplots(figsize=self.figure_size)
        
        # Create scatter plot for spikes
        colors = cm.get_cmap(colormap)
        norm = plt.Normalize(min(neuron_ids), max(neuron_ids))
        
        for i, (neuron_id, time) in enumerate(zip(neuron_ids, spike_times)):
            color = colors(norm(neuron_id))
            ax.scatter(time, neuron_id, c=[color], s=10, alpha=0.7, 
                      edgecolors='black', linewidth=0.5)
        
        # Customize plot
        ax.set_xlabel("Simulation Step", fontsize=12)
        ax.set_ylabel("Neuron ID", fontsize=12)
        ax.set_title("Spike Raster Plot", fontsize=14, fontweight='bold')
        
        # Add colorbar if multiple neurons
        if len(set(neuron_ids)) > 1:
            sm = plt.cm.ScalarMappable(norm=norm, cmap=colors)
            sm.set_array([])
            cbar = plt.colorbar(sm, ax=ax)
            cbar.set_label("Neuron ID", rotation=270, labelpad=20)
        
        # Add horizontal lines for neuron regions if brain is available
        if self.brain is not None:
            self._add_region_lines(ax)
        
        plt.tight_layout()
        
        if filename:
            self._save_figure(fig, filename)
        
        return fig
    
    def plot_membrane_potentials(self, neurons: Optional[List[NeuronId]] = None,
                                duration: Optional[int] = 100,
                                filename: Optional[str] = None,
                                show_threshold: bool = True,
                                show_refractory: bool = True):
        """Plot membrane potential time series for neurons.
        
        Args:
            neurons: List of neuron IDs to plot (all neurons if None)
            duration: Number of steps to plot
            filename: Output file path (optional)
            show_threshold: Whether to show firing threshold lines
            show_refractory: Whether to show refractory periods
            
        Returns:
            matplotlib.Figure: The generated figure
            
        Raises:
            ValueError: If brain is not available or neurons not found
        """
        if self.brain is None:
            raise ValueError("Brain instance is required for membrane potential plotting")
        
        # Get neuron IDs
        if neurons is None:
            neuron_ids = self.brain.getNeuronIds()
        else:
            neuron_ids = neurons
        
        if not neuron_ids:
            raise ValueError("No neurons specified for membrane potential plotting")
        
        # Simulate potential data (placeholder - would need real neuron state access)
        time_steps = list(range(min(duration, 1000)))
        
        fig, axes = plt.subplots(len(neuron_ids), 1, figsize=self.figure_size,
                               sharex=True)
        
        if len(neuron_ids) == 1:
            axes = [axes]
        
        # For each neuron, create membrane potential trace
        for idx, neuron_id in enumerate(neuron_ids):
            ax = axes[idx]
            
            # Generate synthetic membrane potential trace
            np.random.seed(int(neuron_id.value) % 1000)
            baseline = -70.0  # Resting potential
            noise = np.random.normal(0, 1, len(time_steps))
            
            # Add spikes
            spike_indices = np.random.choice(len(time_steps), 
                                           size=np.random.randint(5, 20), 
                                           replace=False)
            spike_amplitudes = np.random.uniform(-20, 20, len(spike_indices))
            
            membrane_potential = baseline + noise
            membrane_potential[spike_indices] += spike_amplitudes
            
            # Plot membrane potential
            ax.plot(time_steps, membrane_potential, 
                   linewidth=1, label='Membrane Potential')
            
            # Add threshold line if requested
            if show_threshold:
                threshold = -55.0  # Example threshold
                ax.axhline(y=threshold, color='red', linestyle='--', 
                          alpha=0.7, label='Threshold')
            
            # Add refractory line if requested
            if show_refractory:
                refractory = -70.0  # Reset potential
                ax.axhline(y=refractory, color='green', linestyle='--', 
                          alpha=0.7, label='Refractory')
            
            # Set title
            ax.set_title(f"Neuron {neuron_id.value} - Membrane Potential", 
                       fontsize=12, fontweight='bold')
            
            # Format y-axis
            ax.set_ylabel("Potential (mV)", fontsize=10)
            ax.legend(loc='upper right', fontsize=8)
            ax.grid(True, alpha=0.3)
        
        # Format x-axis
        axes[-1].set_xlabel("Simulation Step", fontsize=12)
        
        # Adjust layout
        plt.tight_layout()
        
        if filename:
            self._save_figure(fig, filename)
        
        return fig
    
    def plot_network_topology(self, brain: Optional[Brain] = None,
                             layout: str = "spring",
                             filename: Optional[str] = None,
                             show_neurons: bool = True,
                             show_synapses: bool = True,
                             highlight_regions: bool = True):
        """Plot brain network topology using graph visualization.
        
        Args:
            brain: Brain instance to visualize (uses stored brain if None)
            layout: Network layout algorithm ('spring', 'circular', 'hierarchical')
            filename: Output file path (optional)
            show_neurons: Whether to show neurons in plot
            show_synapses: Whether to show synapses/edges
            highlight_regions: Whether to color neurons by region
            
        Returns:
            matplotlib.Figure: The generated figure
        """
        brain = brain or self.brain
        if brain is None:
            raise ValueError("Brain instance is required for network topology plotting")
        
        # Get brain regions and neurons
        regions = brain.getRegionIds()
        
        fig, ax = plt.subplots(figsize=self.figure_size)
        
        if layout == "spring":
            self._plot_spring_layout(ax, brain, regions, show_neurons, 
                                   show_synapses, highlight_regions)
        elif layout == "circular":
            self._plot_circular_layout(ax, brain, regions, show_neurons,
                                     show_synapses, highlight_regions)
        elif layout == "hierarchical":
            self._plot_hierarchical_layout(ax, brain, regions, show_neurons,
                                         show_synapses, highlight_regions)
        
        ax.set_title("Brain Network Topology", fontsize=14, fontweight='bold')
        ax.axis('off')
        
        plt.tight_layout()
        
        if filename:
            self._save_figure(fig, filename)
        
        return fig
    
    def _plot_spring_layout(self, ax, brain, regions, show_neurons, 
                          show_synapses, highlight_regions):
        """Plot network using spring layout algorithm."""
        # Generate random positions for neurons
        num_neurons = brain.getTotalNeuronCount()
        
        if show_neurons:
            # Plot neurons as points
            for i, region_id in enumerate(regions):
                region_neurons = brain.getRegionNeurons(region_id)
                if highlight_regions:
                    # Use different colors for different regions
                    color = plt.cm.Set3(i % 9)
                    for neuron in region_neurons:
                        # Simplified: would need actual neuron positions
                        x = np.random.uniform(0, 100)
                        y = np.random.uniform(0, 100)
                        ax.scatter(x, y, c=[color], s=50, alpha=0.7, 
                                  edgecolors='black', linewidth=1)
                        ax.text(x, y + 2, str(neuron.value), 
                               fontsize=8, ha='center')
                else:
                    # Single color for all neurons
                    for neuron in region_neurons:
                        x = np.random.uniform(0, 100)
                        y = np.random.uniform(0, 100)
                        ax.scatter(x, y, c='lightblue', s=30, alpha=0.7)
                        ax.text(x, y + 2, str(neuron.value), fontsize=8, ha='center')
        
        # Plot regions if available
        if highlight_regions and regions:
            self._plot_regions(ax, regions, brain)
    
    def _plot_circular_layout(self, ax, brain, regions, show_neurons,
                            show_synapses, highlight_regions):
        """Plot network using circular layout algorithm."""
        num_regions = len(regions)
        
        for i, region_id in enumerate(regions):
            angle = 2 * np.pi * i / num_regions
            radius = 50
            
            if highlight_regions:
                color = plt.cm.Set3(i % 9)
            else:
                color = 'lightblue'
            
            if show_neurons:
                region_neurons = brain.getRegionNeurons(region_id)
                for j, neuron in enumerate(region_neurons):
                    neuron_angle = angle + 2 * np.pi * j / len(region_neurons)
                    x = radius * np.cos(neuron_angle)
                    y = radius * np.sin(neuron_angle)
                    
                    ax.scatter(x, y, c=[color], s=30, alpha=0.7, 
                              edgecolors='black', linewidth=1)
                    ax.text(x, y + 3, str(neuron.value), fontsize=8, ha='center')
    
    def _plot_hierarchical_layout(self, ax, brain, regions, show_neurons,
                                 show_synapses, highlight_regions):
        """Plot network using hierarchical layout algorithm."""
        # Arrange regions in layers
        num_layers = min(3, len(regions) if regions else 1)
        
        for layer in range(num_layers):
            layer_regions = []
            if regions:
                # Distribute regions across layers
                for i, region_id in enumerate(regions):
                    if i % num_layers == layer:
                        layer_regions.append(region_id)
            
            for region_id in layer_regions:
                region_neurons = brain.getRegionNeurons(region_id)
                
                for i, neuron in enumerate(region_neurons):
                    x = layer * 40
                    y = i * 5
                    
                    color = plt.cm.Set3(layer % 9)
                    ax.scatter(x, y, c=[color], s=30, alpha=0.7, 
                              edgecolors='black', linewidth=1)
                    ax.text(x, y + 2, str(neuron.value), fontsize=8, ha='center')
    
    def _plot_regions(self, ax, regions, brain):
        """Plot region boundaries and connections."""
        # Simplified region plotting - in real implementation, would use actual region geometry
        for i, region_id in enumerate(regions):
            # Create a circular region representation
            radius = 30 + i * 5
            circle = plt.Circle((0, 0), radius, fill=False, 
                              edgecolor='gray', alpha=0.3, linestyle='--')
            ax.add_patch(circle)
    
    def plot_simulation_progress(self, steps_data: Optional[Dict[str, Any]] = None,
                                 filename: Optional[str] = None,
                                 metrics: Optional[List[str]] = None):
        """Plot simulation progress metrics over time.
        
        Args:
            steps_data: Dictionary of step metrics (neuron counts, firing rates, etc.)
            filename: Output file path (optional)
            metrics: List of metrics to plot (all if None)
            
        Returns:
            matplotlib.Figure: The generated figure
        """
        if steps_data is None:
            if self.brain is None:
                raise ValueError("Must provide steps_data or brain instance")
            
            # Collect data from brain if available
            steps = []
            neuron_counts = []
            firing_rates = []
            total_spikes = []
            
            # This would need actual brain history collection
            # Placeholder implementation
            for step in range(100):
                steps.append(step)
                neuron_counts.append(np.random.randint(800, 1200))
                firing_rates.append(np.random.uniform(5, 50))
                total_spikes.append(np.random.randint(0, 100))
            
            steps_data = {
                'steps': steps,
                'neuron_count': neuron_counts,
                'firing_rate': firing_rates,
                'total_spikes': total_spikes
            }
        
        if metrics is None:
            metrics = ['neuron_count', 'firing_rate', 'total_spikes']
        
        fig, axes = plt.subplots(len(metrics), 1, figsize=self.figure_size,
                               sharex=True)
        
        if len(metrics) == 1:
            axes = [axes]
        
        for idx, metric in enumerate(metrics):
            ax = axes[idx]
            
            if metric in steps_data:
                ax.plot(steps_data['steps'], steps_data[metric],
                       linewidth=2, label=metric.replace('_', ' ').title())
                
                # Add moving average for smoother trends
                window = min(10, len(steps_data[metric]))
                if window > 1:
                    ma = np.convolve(steps_data[metric], 
                                   np.ones(window)/window, mode='valid')
                    ma_steps = steps_data['steps'][window-1:]
                    ax.plot(ma_steps, ma, linewidth=2, alpha=0.7,
                           label=f'{window}-step MA')
                
                ax.set_ylabel(metric.replace('_', ' ').title())
                ax.legend()
                ax.grid(True, alpha=0.3)
        
        axes[-1].set_xlabel("Simulation Step")
        axes[-1].set_title("Simulation Progress")
        
        plt.tight_layout()
        
        if filename:
            self._save_figure(fig, filename)
        
        return fig
    
    def compare_brain_states(self, brain1: Brain, brain2: Brain,
                            metrics: Optional[List[str]] = None,
                            filename: Optional[str] = None):
        """Compare two brain states visually.
        
        Args:
            brain1: First brain instance
            brain2: Second brain instance
            metrics: List of metrics to compare
            filename: Output file path (optional)
            
        Returns:
            matplotlib.Figure: The generated comparison plot
        """
        if metrics is None:
            metrics = ['neuron_count', 'firing_rate', 'spike_count', 'e_i_ratio']
        
        fig, axes = plt.subplots(len(metrics), 1, figsize=self.figure_size,
                               sharex=True)
        
        if len(metrics) == 1:
            axes = [axes]
        
        for idx, metric in enumerate(metrics):
            ax = axes[idx]
            
            # Get metric values for both brains
            val1 = self._get_metric_value(brain1, metric)
            val2 = self._get_metric_value(brain2, metric)
            
            # Create bar chart
            ax.bar(['Brain 1', 'Brain 2'], [val1, val2], 
                   color=['skyblue', 'lightcoral'], alpha=0.7)
            
            # Add value labels on bars
            for i, v in enumerate([val1, val2]):
                ax.text(i, v + max(v*0.05, 0.1), f'{v:.2f}', 
                       ha='center', va='bottom', fontweight='bold')
            
            # Calculate and display difference
            diff = abs(val1 - val2)
            max_val = max(val1, val2)
            diff_pct = (diff / max_val) * 100 if max_val > 0 else 0
            
            ax.set_title(f"{metric.replace('_', ' ').title()}: "
                        f"Difference = {diff_pct:.1f}%")
            ax.set_ylabel(metric.replace('_', ' ').title())
            ax.grid(True, alpha=0.3, axis='y')
        
        axes[-1].set_xlabel("Brain Instance")
        
        plt.tight_layout()
        
        if filename:
            self._save_figure(fig, filename)
        
        return fig
    
    def _get_brain_spikes(self) -> List[SpikeEvent]:
        """Get spike events from the brain instance."""
        # This would need to access brain's spike system
        # Placeholder implementation
        return []
    
    def _get_metric_value(self, brain: Brain, metric: str) -> float:
        """Get a specific metric value from brain."""
        # Placeholder implementation
        if metric == 'neuron_count':
            return float(brain.getTotalNeuronCount())
        elif metric == 'firing_rate':
            return float(brain.getAverageFiringRate())
        elif metric == 'spike_count':
            return float(brain.getTotalSpikeCount())
        elif metric == 'e_i_ratio':
            return float(brain.getExcitationInhibitionRatio())
        else:
            return 0.0
    
    def _add_region_lines(self, ax):
        """Add horizontal lines to indicate neuron regions."""
        # Simplified region line addition
        for y_pos in [10, 20, 30, 40, 50]:
            ax.axhline(y=y_pos, color='gray', linestyle=':', alpha=0.5)
    
    def _save_figure(self, fig, filename: str):
        """Save figure to file with appropriate format."""
        Path(filename).parent.mkdir(parents=True, exist_ok=True)
        fig.savefig(filename, dpi=300, bbox_inches='tight')
        plt.close(fig)
    
    def close(self):
        """Close visualization resources and cleanup."""
        plt.close('all')
        self._spike_history.clear()
        self._activity_data.clear()
        self._network_data.clear()


# Create a convenience function for easy visualization
def create_visualization(brain: Optional[Brain] = None) -> Visualization:
    """Create a visualization module.
    
    Args:
        brain: Optional brain instance to visualize
        
    Returns:
        Visualization: Visualization module instance
    """
    return Visualization(brain)
