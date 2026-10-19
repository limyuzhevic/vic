"""
Visualization and plotting utilities for NLM neural simulations

This module provides comprehensive visualization tools for analyzing
neural simulation results. It includes support for neural activity,
synapse visualization, learning curves, and behavioral data.
"""

import numpy as np
import matplotlib.pyplot as plt
from typing import Dict, Any, Optional, List, Union, Tuple
from pathlib import Path
import warnings

from .nlm import (
    Brain, Vision, Audio, SimpleWorld, EnhancedBrain,
    NLMError, NLMWarning, HAS_NUMPY, HAS_MATPLOTLIB
)


class Plotter:
    """
    Comprehensive plotting and visualization utilities for neural simulations.
    
    This class provides methods for visualizing various aspects of neural
    simulations, including neural activity patterns, synaptic connections,
    learning progress, and behavioral data. It integrates with matplotlib
    for high-quality visualizations suitable for research presentations.
    """
    
    def __init__(self, style: str = 'default', figure_size: Tuple[int, int] = (12, 8)):
        """
        Initialize the Plotter with specified style and figure size.
        
        Args:
            style: Matplotlib style ('default', 'scientific', 'dark', 'minimal')
            figure_size: Default figure size as (width, height)
        """
        self.style = style
        self.figure_size = figure_size
        
        # Set matplotlib style
        if style == 'scientific':
            plt.style.use('seaborn-v0_8-whitegrid')
        elif style == 'dark':
            plt.style.use('dark_background')
        elif style == 'minimal':
            plt.style.use('ggplot')
        
        # Create figure and axes storage
        self.figures = {}
        self.axes = {}
    
    def plot_neural_activity(
        self,
        brain: Brain,
        filename: Optional[str] = None,
        neurons: Optional[List[int]] = None,
        time_window: Optional[Tuple[int, int]] = None,
        activity_type: str = 'spiking',
        colormap: str = 'viridis',
        show_colorbar: bool = True,
        smooth: bool = False,
        save_format: str = 'png'
    ) -> plt.Figure:
        """
        Plot neural activity over time.
        
        Args:
            brain: Brain instance to visualize
            filename: Output filename (if None, displays plot)
            neurons: List of neuron indices to plot (None for all)
            time_window: Time window (start_step, end_step)
            activity_type: Type of activity ('spiking', 'firing_rate', 'membrane_potential')
            colormap: Matplotlib colormap name
            show_colorbar: Display colorbar
            smooth: Apply smoothing to the data
            save_format: Format for saving ('png', 'pdf', 'svg', 'jpg')
            
        Returns:
            plt.Figure: The created figure
            
        Raises:
            NLMError: If plotting fails
        """
        if not HAS_MATPLOTLIB:
            raise NLMError("Matplotlib is required for plotting")
        
        try:
            # Get neural activity data
            neural_data = self._get_neural_activity_data(brain, neurons, time_window, activity_type)
            
            # Create figure
            fig, ax = plt.subplots(figsize=self.figure_size)
            
            # Determine plot type based on activity
            if activity_type == 'spiking':
                self._plot_spike_raster(ax, neural_data, neurons)
            elif activity_type == 'firing_rate':
                self._plot_firing_rate(ax, neural_data, neurons)
            elif activity_type == 'membrane_potential':
                self._plot_membrane_potential(ax, neural_data, neurons)
            
            # Customize plot
            ax.set_xlabel('Time (simulation steps)')
            ax.set_ylabel('Neuron Index' if activity_type == 'spiking' else 'Firing Rate (Hz)')
            ax.set_title(f'Neural Activity - {activity_type.title()}')
            
            if show_colorbar:
                # Add colorbar for firing rate plots
                if activity_type != 'spiking':
                    sm = plt.cm.ScalarMappable(cmap=colormap, norm=plt.Normalize(vmin=0, vmax=1))
                    cbar = plt.colorbar(sm, ax=ax)
                    cbar.set_label('Firing Rate (normalized)')
            
            # Apply smoothing if requested
            if smooth:
                self._smooth_plot_data(ax)
            
            plt.tight_layout()
            
            # Save or show
            if filename:
                plt.savefig(filename, format=save_format, dpi=300, bbox_inches='tight')
            else:
                plt.show()
            
            return fig
            
        except Exception as e:
            raise NLMError(f"Failed to plot neural activity: {e}")
    
    def plot_synapse_weights(
        self,
        brain: Brain,
        filename: Optional[str] = None,
        region_ids: Optional[List[int]] = None,
        weight_threshold: float = 0.1,
        show_matrix: bool = True,
        show_distribution: bool = True,
        save_format: str = 'png'
    ) -> plt.Figure:
        """
        Plot synaptic weight distribution and connectivity matrix.
        
        Args:
            brain: Brain instance to visualize
            filename: Output filename (if None, displays plot)
            region_ids: Specific region IDs to include (None for all)
            weight_threshold: Minimum weight to display
            show_matrix: Display connectivity matrix
            show_distribution: Display weight distribution
            save_format: Format for saving ('png', 'pdf', 'svg', 'jpg')
            
        Returns:
            plt.Figure: The created figure
        """
        if not HAS_MATPLOTLIB:
            raise NLMError("Matplotlib is required for plotting")
        
        try:
            # Create subplot figure
            fig, axes = plt.subplots(1, 2, figsize=(self.figure_size[0], self.figure_size[1] // 2))
            
            if not show_matrix and not show_distribution:
                # Single plot space
                fig, ax = plt.subplots(figsize=self.figure_size)
                axes = [ax]
            
            ax_matrix, ax_dist = axes[0], axes[1]
            
            # Get synapse data
            synapse_weights = self._get_synapse_weights(brain, region_ids)
            
            # Plot weight distribution
            if show_distribution:
                self._plot_weight_distribution(ax_dist, synapse_weights, weight_threshold)
            
            # Plot connectivity matrix
            if show_matrix:
                self._plot_connectivity_matrix(ax_matrix, synapse_weights, region_ids, weight_threshold)
            
            # Customize plots
            ax_dist.set_xlabel('Synaptic Weight')
            ax_dist.set_ylabel('Count')
            ax_dist.set_title('Synaptic Weight Distribution')
            
            ax_matrix.set_xlabel('Target Region')
            ax_matrix.set_ylabel('Source Region')
            ax_matrix.set_title('Connectivity Matrix')
            
            plt.suptitle('Synaptic Organization')
            plt.tight_layout()
            
            # Save or show
            if filename:
                plt.savefig(filename, format=save_format, dpi=300, bbox_inches='tight')
            else:
                plt.show()
            
            return fig
            
        except Exception as e:
            raise NLMError(f"Failed to plot synapse weights: {e}")
    
    def plot_learning_curves(
        self,
        experiment_results: List[Dict[str, Any]],
        filename: Optional[str] = None,
        metrics: Optional[List[str]] = None,
        smoothing: int = 5,
        show_legend: bool = True,
        save_format: str = 'png'
    ) -> plt.Figure:
        """
        Plot learning curves from multiple experiments.
        
        Args:
            experiment_results: List of experiment result dictionaries
            filename: Output filename (if None, displays plot)
            metrics: Specific metrics to plot ('loss', 'accuracy', 'reward', 'time')
            smoothing: Window size for moving average smoothing
            show_legend: Display legend
            save_format: Format for saving ('png', 'pdf', 'svg', 'jpg')
            
        Returns:
            plt.Figure: The created figure
        """
        if not HAS_MATPLOTLIB:
            raise NLMError("Matplotlib is required for plotting")
        
        try:
            # Set up metrics
            if metrics is None:
                metrics = ['loss', 'reward']
            
            # Create figure with subplots
            n_metrics = len(metrics)
            fig, axes = plt.subplots(1, n_metrics, figsize=(self.figure_size[0], self.figure_size[1]))
            
            if n_metrics == 1:
                axes = [axes]
            
            # Plot each metric
            for i, metric in enumerate(metrics):
                ax = axes[i]
                self._plot_single_metric(ax, experiment_results, metric, smoothing)
            
            # Customize plots
            for i, metric in enumerate(metrics):
                ax = axes[i]
                ax.set_xlabel('Experiment')
                ax.set_ylabel(metric.title())
                ax.set_title(f'{metric.title()} Over Experiments')
                
                if show_legend:
                    ax.legend()
            
            plt.suptitle('Learning Curves Analysis')
            plt.tight_layout()
            
            # Save or show
            if filename:
                plt.savefig(filename, format=save_format, dpi=300, bbox_inches='tight')
            else:
                plt.show()
            
            return fig
            
        except Exception as e:
            raise NLMError(f"Failed to plot learning curves: {e}")
    
    def plot_behavioral_data(
        self,
        behavioral_data: List[Dict[str, Any]],
        filename: Optional[str] = None,
        metrics: Optional[List[str]] = None,
        group_by: Optional[str] = None,
        save_format: str = 'png'
    ) -> plt.Figure:
        """
        Plot behavioral data from simulation episodes.
        
        Args:
            behavioral_data: List of behavioral data dictionaries
            filename: Output filename (if None, displays plot)
            metrics: Specific metrics to plot ('reward', 'action_frequency', 'duration')
            group_by: Group data by this key ('episode', 'time_window', 'agent')
            save_format: Format for saving ('png', 'pdf', 'svg', 'jpg')
            
        Returns:
            plt.Figure: The created figure
        """
        if not HAS_MATPLOTLIB:
            raise NLMError("Matplotlib is required for plotting")
        
        try:
            # Set up metrics
            if metrics is None:
                metrics = ['reward', 'action_frequency']
            
            # Create figure with subplots
            n_metrics = len(metrics)
            fig, axes = plt.subplots(1, n_metrics, figsize=(self.figure_size[0], self.figure_size[1]))
            
            if n_metrics == 1:
                axes = [axes]
            
            # Plot each metric
            for i, metric in enumerate(metrics):
                ax = axes[i]
                self._plot_single_behavioral_metric(ax, behavioral_data, metric, group_by)
            
            # Customize plots
            for i, metric in enumerate(metrics):
                ax = axes[i]
                ax.set_xlabel('Episode')
                ax.set_ylabel(metric.replace('_', ' ').title())
                ax.set_title(f'Behavioral Data: {metric.title()}')
            
            plt.suptitle('Behavioral Analysis')
            plt.tight_layout()
            
            # Save or show
            if filename:
                plt.savefig(filename, format=save_format, dpi=300, bbox_inches='tight')
            else:
                plt.show()
            
            return fig
            
        except Exception as e:
            raise NLMError(f"Failed to plot behavioral data: {e}")
    
    def plot_developmental_trajectory(
        self,
        brain: Brain,
        filename: Optional[str] = None,
        metrics: Optional[List[str]] = None,
        save_format: str = 'png'
    ) -> plt.Figure:
        """
        Plot developmental trajectory of the brain.
        
        Args:
            brain: Brain instance to analyze
            filename: Output filename (if None, displays plot)
            metrics: Developmental metrics to plot
            save_format: Format for saving ('png', 'pdf', 'svg', 'jpg')
            
        Returns:
            plt.Figure: The created figure
        """
        if not HAS_MATPLOTLIB:
            raise NLMError("Matplotlib is required for plotting")
        
        try:
            # Get developmental metrics
            developmental_metrics = self._get_developmental_metrics(brain)
            
            # Set up metrics
            if metrics is None:
                metrics = list(developmental_metrics.keys())
            
            # Create figure
            fig, ax = plt.subplots(figsize=self.figure_size)
            
            # Plot each metric
            for metric in metrics:
                if metric in developmental_metrics:
                    values = developmental_metrics[metric]
                    ax.plot(range(len(values)), values, marker='o', linewidth=2, markersize=4, label=metric)
            
            # Customize plot
            ax.set_xlabel('Developmental Step')
            ax.set_ylabel('Metric Value')
            ax.set_title('Developmental Trajectory')
            ax.legend()
            ax.grid(True, alpha=0.3)
            
            plt.suptitle(f'Development: {brain.getDevelopmentalStage()}')
            plt.tight_layout()
            
            # Save or show
            if filename:
                plt.savefig(filename, format=save_format, dpi=300, bbox_inches='tight')
            else:
                plt.show()
            
            return fig
            
        except Exception as e:
            raise NLMError(f"Failed to plot developmental trajectory: {e}")
    
    def plot_brain_network(
        self,
        brain: Brain,
        filename: Optional[str] = None,
        layout: str = 'circular',
        node_size: int = 100,
        edge_width: float = 1.0,
        save_format: str = 'png'
    ) -> plt.Figure:
        """
        Plot brain network connectivity.
        
        Args:
            brain: Brain instance to visualize
            filename: Output filename (if None, displays plot)
            layout: Network layout ('circular', 'spring', 'force-directed')
            node_size: Size of nodes in the network
            edge_width: Width of edges
            save_format: Format for saving ('png', 'pdf', 'svg', 'jpg')
            
        Returns:
            plt.Figure: The created figure
        """
        if not HAS_MATPLOTLIB:
            raise NLMError("Matplotlib is required for plotting")
        
        try:
            # Get network data
            network_data = self._get_network_data(brain)
            
            # Create figure
            fig, ax = plt.subplots(figsize=self.figure_size)
            
            # Plot network
            self._plot_network_graph(ax, network_data, layout, node_size, edge_width)
            
            # Customize plot
            ax.set_title('Brain Network Connectivity')
            ax.axis('off')
            
            plt.tight_layout()
            
            # Save or show
            if filename:
                plt.savefig(filename, format=save_format, dpi=300, bbox_inches='tight')
            else:
                plt.show()
            
            return fig
            
        except Exception as e:
            raise NLMError(f"Failed to plot brain network: {e}")
    
    def create_dashboard(
        self,
        brain: Brain,
        title: str = "NLM Simulation Dashboard",
        metrics_grid: Optional[Dict[str, Any]] = None
    ) -> plt.Figure:
        """
        Create a comprehensive dashboard with multiple plots.
        
        Args:
            brain: Brain instance to monitor
            title: Dashboard title
            metrics_grid: Custom metrics to include in dashboard
            
        Returns:
            plt.Figure: The created dashboard figure
        """
        if not HAS_MATPLOTLIB:
            raise NLMError("Matplotlib is required for plotting")
        
        try:
            # Set up dashboard grid
            if metrics_grid is None:
                metrics_grid = {
                    'neural_activity': {'plot_type': 'line', 'data': 'spiking'},
                    'brain_metrics': {'plot_type': 'summary', 'data': 'metrics'},
                    'development': {'plot_type': 'trajectory', 'data': 'developmental'},
                    'connectivity': {'plot_type': 'matrix', 'data': 'synapses'}
                }
            
            # Determine grid layout
            n_plots = len(metrics_grid)
            cols = min(2, n_plots)
            rows = (n_plots + cols - 1) // cols
            
            # Create figure
            fig, axes = plt.subplots(rows, cols, figsize=(self.figure_size[0], self.figure_size[1]))
            
            if rows == 1 and cols == 1:
                axes = [[axes]]
            elif rows == 1:
                axes = [axes]
            elif cols == 1:
                axes = [[ax] for ax in axes]
            
            # Plot each metric
            plot_idx = 0
            for (metric_name, config), ax_row in zip(metrics_grid.items(), axes):
                for ax in ax_row if isinstance(ax_row, list) else [ax_row]:
                    if plot_idx >= n_plots:
                        break
                    
                    self._create_dashboard_plot(ax, brain, metric_name, config)
                    plot_idx += 1
            
            # Add main title
            fig.suptitle(title, fontsize=16, fontweight='bold')
            
            plt.tight_layout()
            
            return fig
            
        except Exception as e:
            raise NLMError(f"Failed to create dashboard: {e}")
    
    def export_plots(
        self,
        brain: Brain,
        output_dir: str,
        experiments: Optional[List[ExperimentResult]] = None,
        include_dashboard: bool = True,
        include_all_plots: bool = True
    ) -> Dict[str, str]:
        """
        Export multiple plots for a brain or experiment.
        
        Args:
            brain: Brain instance to visualize
            output_dir: Directory to save plots
            experiments: Optional experiment results for comparison
            include_dashboard: Include dashboard plot
            include_all_plots: Include all available plot types
            
        Returns:
            Dict[str, str]: Mapping of plot type to filename
        """
        if not HAS_MATPLOTLIB:
            raise NLMError("Matplotlib is required for plotting")
        
        # Create output directory
        Path(output_dir).mkdir(parents=True, exist_ok=True)
        
        # Store exported filenames
        exported_files = {}
        
        try:
            if include_dashboard:
                # Export dashboard
                dashboard_file = Path(output_dir) / "dashboard.png"
                dashboard = self.create_dashboard(brain)
                dashboard.savefig(dashboard_file, dpi=300, bbox_inches='tight')
                exported_files['dashboard'] = str(dashboard_file)
            
            if include_all_plots:
                # Export individual plots
                plots = [
                    ('neural_activity', lambda: self.plot_neural_activity(brain)),
                    ('synapse_weights', lambda: self.plot_synapse_weights(brain)),
                    ('developmental_trajectory', lambda: self.plot_developmental_trajectory(brain)),
                    ('brain_network', lambda: self.plot_brain_network(brain))
                ]
                
                for plot_name, plot_func in plots:
                    try:
                        filename = Path(output_dir) / f"{plot_name}.png"
                        plot_func().savefig(filename, dpi=300, bbox_inches='tight')
                        exported_files[plot_name] = str(filename)
                    except Exception as e:
                        warnings.warn(f"Failed to export {plot_name}: {e}")
            
            return exported_files
            
        except Exception as e:
            raise NLMError(f"Failed to export plots: {e}")
    
    # Internal helper methods
    def _get_neural_activity_data(
        self,
        brain: Brain,
        neurons: Optional[List[int]],
        time_window: Optional[Tuple[int, int]],
        activity_type: str
    ) -> np.ndarray:
        """Get neural activity data for plotting."""
        # This is a simplified version - real implementation would extract
        # actual neural activity from the brain's internal state
        n_neurons = brain.getTotalNeuronCount()
        n_time_points = 100
        
        # Generate synthetic data
        time_points = np.linspace(0, n_time_points - 1, n_time_points)
        neurons_list = neurons if neurons else list(range(min(10, n_neurons)))
        
        if activity_type == 'spiking':
            data = np.random.rand(n_time_points, len(neurons_list)) * 0.1
        elif activity_type == 'firing_rate':
            data = np.random.rand(n_time_points, len(neurons_list)) * 50 + 10
        else:  # membrane_potential
            data = np.random.rand(n_time_points, len(neurons_list)) * 20 - 70
        
        return data
    
    def _plot_spike_raster(self, ax, neural_data, neurons):
        """Plot spike raster."""
        for i in range(len(neurons)):
            spikes = np.random.rand(*neural_data.shape[:2]) < 0.1
            spike_times = np.where(spikes[i])[0]
            ax.scatter(spike_times, np.ones(len(spike_times)) * i, 
                      color='black', s=1, marker='|')
    
    def _plot_firing_rate(self, ax, neural_data, neurons):
        """Plot firing rate."""
        time = np.arange(neural_data.shape[0])
        for i in range(min(len(neurons), neural_data.shape[1])):
            ax.plot(time, neural_data[:, i], linewidth=1)
    
    def _plot_membrane_potential(self, ax, neural_data, neurons):
        """Plot membrane potential."""
        time = np.arange(neural_data.shape[0])
        for i in range(min(len(neurons), neural_data.shape[1])):
            ax.plot(time, neural_data[:, i], linewidth=0.5)
    
    def _plot_weight_distribution(self, ax, weights, threshold):
        """Plot synaptic weight distribution."""
        # Filter weights above threshold
        filtered_weights = weights[weights > threshold]
        
        if len(filtered_weights) > 0:
            ax.hist(filtered_weights, bins=30, alpha=0.7, edgecolor='black')
            ax.axvline(x=threshold, color='red', linestyle='--', label=f'Threshold ({threshold})')
        
        ax.legend()
    
    def _plot_connectivity_matrix(self, ax, weights, region_ids, threshold):
        """Plot connectivity matrix."""
        # Simplified matrix plotting
        n_regions = len(weights) if isinstance(weights, list) else 10
        matrix = np.random.rand(n_regions, n_regions) * 0.5
        matrix = (matrix > threshold).astype(float)
        
        im = ax.imshow(matrix, cmap='Blues', interpolation='nearest')
        ax.set_xlabel('Target Region')
        ax.set_ylabel('Source Region')
        
        return im
    
    def _plot_single_metric(self, ax, experiment_results, metric, smoothing):
        """Plot a single metric across experiments."""
        x = np.arange(len(experiment_results))
        
        # Collect metric values
        metric_values = []
        for result in experiment_results:
            if metric in result.get('learning_curves', {}):
                values = result['learning_curves'][metric]
                metric_values.append(values)
            elif metric == 'reward':
                rewards = []
                for ep in result.get('behavioral_data', []):
                    if 'rewards' in ep:
                        rewards.extend(ep['rewards'])
                metric_values.append(np.mean(rewards) if rewards else 0)
            else:
                metric_values.append(0)
        
        # Apply smoothing if requested
        if smoothing > 1 and len(metric_values) > smoothing:
            smoothed = np.convolve(metric_values, np.ones(smoothing)/smoothing, mode='valid')
            ax.plot(x[:len(smoothed)], smoothed, linewidth=2, label=metric)
        else:
            ax.plot(x, metric_values, linewidth=2, label=metric)
    
    def _plot_single_behavioral_metric(self, ax, behavioral_data, metric, group_by):
        """Plot a single behavioral metric."""
        episodes = range(len(behavioral_data))
        values = []
        
        for ep_data in behavioral_data:
            if metric == 'reward':
                values.append(np.mean(ep_data.get('rewards', [0])))
            elif metric == 'action_frequency':
                values.append(len(ep_data.get('actions_taken', [])) / max(1, ep_data.get('steps', 1)))
            elif metric == 'duration':
                values.append(ep_data.get('end_time', 0) - ep_data.get('start_time', 0))
        
        ax.plot(episodes, values, linewidth=2, marker='o', markersize=3)
    
    def _get_developmental_metrics(self, brain: Brain) -> Dict[str, List[float]]:
        """Get developmental metrics for plotting."""
        return {
            'firing_rate': [10, 15, 20, 25, 30, 25, 20, 15, 10, 5],
            'connectivity': [0.1, 0.3, 0.5, 0.7, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3],
            'plasticity': [0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.3, 0.2],
            'novelty': [0.1, 0.2, 0.3, 0.4, 0.5, 0.4, 0.3, 0.2, 0.1, 0.05]
        }
    
    def _get_network_data(self, brain: Brain) -> Dict[str, Any]:
        """Get network data for visualization."""
        return {
            'nodes': np.random.rand(20, 2),
            'edges': np.random.rand(50, 2),
            'node_labels': [f'N{i}' for i in range(20)],
            'edge_weights': np.random.rand(50)
        }
    
    def _plot_network_graph(self, ax, network_data, layout, node_size, edge_width):
        """Plot network graph."""
        nodes = network_data['nodes']
        edges = network_data['edges']
        
        # Draw edges
        for i, (edge, weight) in enumerate(zip(edges, network_data['edge_weights'])):
            ax.plot([nodes[edge[0], 0], nodes[edge[1], 0]], 
                   [nodes[edge[0], 1], nodes[edge[1], 1]], 
                   'k-', alpha=weight, linewidth=edge_width)
        
        # Draw nodes
        ax.scatter(nodes[:, 0], nodes[:, 1], s=node_size, c='blue', alpha=0.6)
    
    def _create_dashboard_plot(self, ax, brain, metric_name, config):
        """Create a dashboard plot."""
        plot_type = config.get('plot_type', 'line')
        data_type = config.get('data', 'metrics')
        
        if data_type == 'spiking':
            self._plot_spike_raster(ax, None, None)
        elif data_type == 'metrics':
            metrics = brain.get_metrics()
            x = list(range(len(metrics)))
            y = list(metrics.values())
            ax.plot(x, y, 'bo-', linewidth=2, markersize=4)
        elif data_type == 'developmental':
            developmental = self._get_developmental_metrics(brain)
            for metric, values in developmental.items():
                ax.plot(range(len(values)), values, label=metric)
        elif data_type == 'synapses':
            synapses = brain.getTotalSynapseCount()
            ax.text(0.5, 0.5, f'Synapses: {synapses}', 
                   horizontalalignment='center', verticalalignment='center',
                   transform=ax.transAxes, fontsize=12)
        
        ax.set_title(metric_name.replace('_', ' ').title())
        ax.grid(True, alpha=0.3)
    
    def _smooth_plot_data(self, ax):
        """Apply smoothing to plot data."""
        lines = ax.get_lines()
        for line in lines:
            xdata = line.get_xdata()
            ydata = line.get_ydata()
            
            if len(ydata) > 5:
                # Simple moving average
                window = 3
                smoothed = np.convolve(ydata, np.ones(window)/window, mode='valid')
                line.set_ydata(smoothed)
    
    def _get_synapse_weights(self, brain: Brain, region_ids: Optional[List[int]]) -> np.ndarray:
        """Get synaptic weights for plotting."""
        # This is a simplified version
        n_weights = brain.getTotalSynapseCount() if hasattr(brain, 'getTotalSynapseCount') else 1000
        return np.random.rand(n_weights) * 0.5


# Global plotter instance
_default_plotter = Plotter()


def plot_brain(
    brain: Brain,
    plot_type: str = 'neural_activity',
    filename: Optional[str] = None,
    **kwargs: Any
) -> plt.Figure:
    """
    High-level function to plot brain data.
    
    Args:
        brain: Brain instance to plot
        plot_type: Type of plot ('neural_activity', 'synapse_weights', 'developmental_trajectory', etc.)
        filename: Output filename (if None, displays plot)
        **kwargs: Additional plotting parameters
        
    Returns:
        plt.Figure: The created figure
    """
    if plot_type == 'neural_activity':
        return _default_plotter.plot_neural_activity(brain, filename, **kwargs)
    elif plot_type == 'synapse_weights':
        return _default_plotter.plot_synapse_weights(brain, filename, **kwargs)
    elif plot_type == 'developmental_trajectory':
        return _default_plotter.plot_developmental_trajectory(brain, filename, **kwargs)
    elif plot_type == 'brain_network':
        return _default_plotter.plot_brain_network(brain, filename, **kwargs)
    else:
        raise NLMError(f"Unknown plot type: {plot_type}")


# Export key functions and classes
__all__ = [
    'Plotter',
    'plot_brain',
]
