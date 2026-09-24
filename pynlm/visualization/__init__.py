"""
pynlm.visualization - Visualization helpers for NLM project

This module provides visualization helpers including:
- Neural activity plotting
- Simulation visualization
- Result graphing
- Interactive visualization helpers
"""

import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import pandas as pd
from typing import Dict, Any, List, Optional, Tuple, Callable
import plotly.graph_objects as go
import plotly.express as px
from plotly.subplots import make_subplots
from pathlib import Path
import json
import logging

logger = logging.getLogger(__name__)


class PlotStyle:
    """Manage plot styles and themes."""
    
    STYLES = {
        "default": {
            "style": "seaborn-v0_8-whitegrid",
            "palette": "husl",
            "font_size": 12,
            "title_font_size": 16,
            "axis_label_size": 12,
            "tick_label_size": 10
        },
        "scientific": {
            "style": "seaborn-v0_8-notebook",
            "palette": "colorblind",
            "font_size": 11,
            "title_font_size": 14,
            "axis_label_size": 11,
            "tick_label_size": 9
        },
        "publication": {
            "style": "classic",
            "palette": "pastel2",
            "font_size": 10,
            "title_font_size": 14,
            "axis_label_size": 10,
            "tick_label_size": 8
        }
    }
    
    def __init__(self, style_name: str = "default"):
        """
        Initialize plot style.
        
        Parameters
        ----------
        style_name : str, optional
            Style name, by default "default"
        """
        if style_name not in self.STYLES:
            raise ValueError(f"Unknown style: {style_name}")
        
        self.style_config = self.STYLES[style_name]
        self._apply_style()
    
    def _apply_style(self) -> None:
        """Apply plot style."""
        plt.style.use(self.style_config["style"])
        
        # Set default colors
        self.color_palette = self.style_config.get("palette", "husl")
    
    def get_color_palette(self, n_colors: int = 10) -> List[str]:
        """
        Get color palette.
        
        Parameters
        ----------
        n_colors : int, optional
            Number of colors, by default 10
            
        Returns
        -------
        List[str]
            Color palette
        """
        return plt.cm.get_cmap(self.color_palette, n_colors).colors
    
    def get_font_dict(self) -> Dict[str, Any]:
        """Get font configuration dictionary."""
        return {
            "font": {
                "family": "sans-serif",
                "size": self.style_config["font_size"]
            },
            "title": {
                "fontsize": self.style_config["title_font_size"],
                "fontweight": "bold"
            },
            "axis": {
                "labelsize": self.style_config["axis_label_size"],
                "titlesize": self.style_config["axis_label_size"]
            },
            "xtick": {
                "labelsize": self.style_config["tick_label_size"]
            },
            "ytick": {
                "labelsize": self.style_config["tick_label_size"]
            }
        }


class ColorMap:
    """Manage color maps for visualizations."""
    
    DEFAULT_COLORMAPS = {
        "activity": "viridis",
        "gradient": "plasma",
        "correlation": "coolwarm",
        "confidence": "RdYlBu",
        "time": "viridis",
        "neuron": "nipy_spectral"
    }
    
    def __init__(self, colormap_name: str = "viridis"):
        """
        Initialize color map.
        
        Parameters
        ----------
        colormap_name : str, optional
            Colormap name, by default "viridis"
        """
        self.colormap_name = colormap_name
        self.cmap = plt.cm.get_cmap(colormap_name)
    
    def get_colormap(self):
        """Get matplotlib colormap."""
        return self.cmap
    
    def normalize(self, data: np.ndarray) -> np.ndarray:
        """
        Normalize data for colormap.
        
        Parameters
        ----------
        data : np.ndarray
            Input data
            
        Returns
        -------
        np.ndarray
            Normalized data
        """
        return (data - data.min()) / (data.max() - data.min())
    
    def get_colored_data(self, data: np.ndarray) -> np.ndarray:
        """
        Get colored data using colormap.
        
        Parameters
        ----------
        data : np.ndarray
            Input data
            
        Returns
        -------
        np.ndarray
            Colored data (RGBA)
        """
        normalized = self.normalize(data)
        return self.cmap(normalized)


class ActivityPlotter:
    """Plot neural activity patterns."""
    
    def __init__(self, style: PlotStyle = None):
        """
        Initialize activity plotter.
        
        Parameters
        ----------
        style : PlotStyle, optional
            Plot style, by default None
        """
        self.style = style or PlotStyle()
        self.cmap = ColorMap()
    
    def plot_neural_activity(self, activity: np.ndarray, time_steps: Optional[List[int]] = None,
                           neurons: Optional[List[int]] = None, title: str = "Neural Activity",
                           save_path: Optional[str] = None) -> plt.Figure:
        """
        Plot neural activity over time.
        
        Parameters
        ----------
        activity : np.ndarray
            Neural activity data (time x neurons)
        time_steps : Optional[List[int]], optional
            Time steps, by default None
        neurons : Optional[List[int]], optional
            Neuron indices to plot, by default None
        title : str, optional
            Plot title, by default "Neural Activity"
        save_path : Optional[str], optional
            Path to save figure, by default None
            
        Returns
        -------
        plt.Figure
            Matplotlib figure
        """
        if time_steps is None:
            time_steps = list(range(activity.shape[0]))
        
        if neurons is None:
            # Plot average activity across all neurons
            fig, axes = plt.subplots(2, 1, figsize=(12, 10), gridspec_kw={'height_ratios': [3, 1]})
            
            # Neural activity heatmap
            im = axes[0].imshow(activity.T, aspect='auto', cmap=self.cmap.get_colormap(),
                              interpolation='nearest', origin='lower')
            axes[0].set_xlabel('Time Step')
            axes[0].set_ylabel('Neuron Index')
            axes[0].set_title(f'{title} (Average over Neurons)')
            plt.colorbar(im, ax=axes[0])
            
            # Time series of average activity
            avg_activity = np.mean(activity, axis=1)
            axes[1].plot(time_steps, avg_activity, color='blue', linewidth=2)
            axes[1].set_xlabel('Time Step')
            axes[1].set_ylabel('Average Activity')
            axes[1].set_title('Average Neural Activity Over Time')
            axes[1].grid(True, alpha=0.3)
            
        else:
            # Plot specific neurons
            fig, axes = plt.subplots(len(neurons), 1, figsize=(12, 4 * len(neurons)))
            
            if len(neurons) == 1:
                axes = [axes]
            
            for i, neuron_idx in enumerate(neurons):
                if neuron_idx < activity.shape[1]:
                    axes[i].plot(time_steps, activity[:, neuron_idx], 
                                label=f'Neuron {neuron_idx}', linewidth=2)
                    axes[i].set_xlabel('Time Step')
                    axes[i].set_ylabel('Activity')
                    axes[i].set_title(f'Neural Activity - Neuron {neuron_idx}')
                    axes[i].legend()
                    axes[i].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Saved activity plot to {save_path}")
        
        return fig
    
    def plot_activity_distribution(self, activity: np.ndarray, title: str = "Activity Distribution",
                                  save_path: Optional[str] = None) -> plt.Figure:
        """
        Plot distribution of neural activity.
        
        Parameters
        ----------
        activity : np.ndarray
            Neural activity data (time x neurons)
        title : str, optional
            Plot title, by default "Activity Distribution"
        save_path : Optional[str], optional
            Path to save figure, by default None
            
        Returns
        -------
        plt.Figure
            Matplotlib figure
        """
        fig, axes = plt.subplots(2, 2, figsize=(15, 12))
        axes = axes.flatten()
        
        # Flatten activity data
        flat_activity = activity.flatten()
        
        # Histogram of activity values
        axes[0].hist(flat_activity, bins=50, alpha=0.7, color='skyblue', edgecolor='black')
        axes[0].set_xlabel('Activity Value')
        axes[0].set_ylabel('Frequency')
        axes[0].set_title('Activity Value Distribution')
        axes[0].grid(True, alpha=0.3)
        
        # Box plot of activity per time step
        axes[1].boxplot(activity, vert=False)
        axes[1].set_xlabel('Activity Value')
        axes[1].set_title('Activity Distribution Per Time Step')
        axes[1].grid(True, alpha=0.3)
        
        # Activity over time (if multiple time steps)
        if activity.shape[0] > 1:
            time_avg = np.mean(activity, axis=1)
            axes[2].plot(range(len(time_avg)), time_avg, linewidth=2)
            axes[2].set_xlabel('Time Step')
            axes[2].set_ylabel('Average Activity')
            axes[2].set_title('Average Activity Over Time')
            axes[2].grid(True, alpha=0.3)
        
        # Neuron activity statistics
        neuron_stats = np.mean(activity, axis=0)
        axes[3].bar(range(len(neuron_stats)), neuron_stats[:100], alpha=0.7)  # First 100 neurons
        axes[3].set_xlabel('Neuron Index')
        axes[3].set_ylabel('Average Activity')
        axes[3].set_title('Average Activity Per Neuron (First 100)')
        axes[3].grid(True, alpha=0.3)
        
        plt.suptitle(title, fontsize=16, fontweight='bold')
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Saved activity distribution plot to {save_path}")
        
        return fig
    
    def plot_spike_raster(self, spikes: np.ndarray, neuron_indices: List[int],
                        time_steps: Optional[List[int]] = None, title: str = "Spike Raster Plot",
                        save_path: Optional[str] = None) -> plt.Figure:
        """
        Plot spike raster plot.
        
        Parameters
        ----------
        spikes : np.ndarray
            Spike data (neurons x time)
        neuron_indices : List[int]
            Neuron indices to plot
        time_steps : Optional[List[int]], optional
            Time steps, by default None
        title : str, optional
            Plot title, by default "Spike Raster Plot"
        save_path : Optional[str], optional
            Path to save figure, by default None
            
        Returns
        -------
        plt.Figure
            Matplotlib figure
        """
        if time_steps is None:
            time_steps = list(range(spikes.shape[1]))
        
        fig, ax = plt.subplots(figsize=(12, 8))
        
        for i, neuron_idx in enumerate(neuron_indices):
            if neuron_idx < spikes.shape[0]:
                spike_times = np.where(spikes[neuron_idx] > 0)[0]
                if len(spike_times) > 0:
                    ax.scatter(spike_times, np.full_like(spike_times, neuron_idx), 
                              s=10, color='black', marker='|', linewidths=0.5)
        
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Neuron Index')
        ax.set_title(title)
        ax.set_xlim(min(time_steps), max(time_steps))
        ax.set_ylim(-1, max(neuron_indices) + 1)
        ax.grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Saved spike raster plot to {save_path}")
        
        return fig


class SimulationVisualizer:
    """Visualize simulation results."""
    
    def __init__(self, style: PlotStyle = None):
        """
        Initialize simulation visualizer.
        
        Parameters
        ----------
        style : PlotStyle, optional
            Plot style, by default None
        """
        self.style = style or PlotStyle()
        self.cmap = ColorMap()
    
    def plot_simulation_state(self, state_history: List[Dict[str, Any]],
                            state_variables: List[str], title: str = "Simulation State",
                            save_path: Optional[str] = None) -> plt.Figure:
        """
        Plot simulation state over time.
        
        Parameters
        ----------
        state_history : List[Dict[str, Any]]
            Simulation state history
        state_variables : List[str]
            State variables to plot
        title : str, optional
            Plot title, by default "Simulation State"
        save_path : Optional[str], optional
            Path to save figure, by default None
            
        Returns
        -------
        plt.Figure
            Matplotlib figure
        """
        fig, axes = plt.subplots(len(state_variables), 1, figsize=(12, 5 * len(state_variables)))
        
        if len(state_variables) == 1:
            axes = [axes]
        
        for i, var_name in enumerate(state_variables):
            if var_name in state_history[0]:
                values = [state[var_name] for state in state_history]
                axes[i].plot(range(len(values)), values, linewidth=2, color='blue')
                axes[i].set_xlabel('Time Step')
                axes[i].set_ylabel(var_name)
                axes[i].set_title(f'Simulation State: {var_name}')
                axes[i].grid(True, alpha=0.3)
        
        plt.suptitle(title, fontsize=16, fontweight='bold')
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Saved simulation state plot to {save_path}")
        
        return fig
    
    def plot_simulation_metrics(self, metrics_history: List[Dict[str, Any]],
                               metric_names: Optional[List[str]] = None,
                               title: str = "Simulation Metrics", save_path: Optional[str] = None) -> plt.Figure:
        """
        Plot simulation metrics over time.
        
        Parameters
        ----------
        metrics_history : List[Dict[str, Any]]
            Simulation metrics history
        metric_names : Optional[List[str]], optional
            Metric names to plot, by default None
        title : str, optional
            Plot title, by default "Simulation Metrics"
        save_path : Optional[str], optional
            Path to save figure, by default None
            
        Returns
        -------
        plt.Figure
            Matplotlib figure
        """
        if metric_names is None:
            if metrics_history:
                metric_names = list(metrics_history[0].keys())
            else:
                metric_names = []
        
        fig, axes = plt.subplots(len(metric_names), 1, figsize=(12, 4 * len(metric_names)))
        
        if len(metric_names) == 1:
            axes = [axes]
        
        for i, metric_name in enumerate(metric_names):
            if metric_name in metrics_history[0]:
                values = [m[metric_name] for m in metrics_history]
                axes[i].plot(range(len(values)), values, linewidth=2, color='green')
                axes[i].set_xlabel('Time Step')
                axes[i].set_ylabel(metric_name)
                axes[i].set_title(f'Simulation Metric: {metric_name}')
                axes[i].grid(True, alpha=0.3)
        
        plt.suptitle(title, fontsize=16, fontweight='bold')
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Saved simulation metrics plot to {save_path}")
        
        return fig
    
    def plot_agent_trajectories(self, agent_trajectories: List[List[Tuple[float, float]]],
                              title: str = "Agent Trajectories", save_path: Optional[str] = None) -> plt.Figure:
        """
        Plot agent trajectories.
        
        Parameters
        ----------
        agent_trajectories : List[List[Tuple[float, float]]]
            List of agent trajectories
        title : str, optional
            Plot title, by default "Agent Trajectories"
        save_path : Optional[str], optional
            Path to save figure, by default None
            
        Returns
        -------
        plt.Figure
            Matplotlib figure
        """
        fig, ax = plt.subplots(figsize=(10, 8))
        
        for i, trajectory in enumerate(agent_trajectories):
            if len(trajectory) > 1:
                x_coords = [pos[0] for pos in trajectory]
                y_coords = [pos[1] for pos in trajectory]
                ax.plot(x_coords, y_coords, linewidth=2, alpha=0.7, label=f'Agent {i}')
                ax.scatter([x_coords[0]], [y_coords[0]], color='green', s=100, marker='o')
                ax.scatter([x_coords[-1]], [y_coords[-1]], color='red', s=100, marker='s')
        
        ax.set_xlabel('X Position')
        ax.set_ylabel('Y Position')
        ax.set_title(title)
        ax.legend()
        ax.grid(True, alpha=0.3)
        ax.set_aspect('equal', adjustable='box')
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Saved agent trajectories plot to {save_path}")
        
        return fig


class ResultGrapher:
    """Graph experimental results."""
    
    def __init__(self, style: PlotStyle = None):
        """
        Initialize result grapher.
        
        Parameters
        ----------
        style : PlotStyle, optional
            Plot style, by default None
        """
        self.style = style or PlotStyle()
    
    def plot_experiment_results(self, results: Dict[str, Any], metrics: List[str],
                               title: str = "Experiment Results", save_path: Optional[str] = None) -> plt.Figure:
        """
        Plot experiment results.
        
        Parameters
        ----------
        results : Dict[str, Any]
            Experiment results
        metrics : List[str]
            Metrics to plot
        title : str, optional
            Plot title, by default "Experiment Results"
        save_path : Optional[str], optional
            Path to save figure, by default None
            
        Returns
        -------
        plt.Figure
            Matplotlib figure
        """
        fig, axes = plt.subplots(2, 2, figsize=(15, 12))
        axes = axes.flatten()
        
        # Plot results bar chart
        ax_idx = 0
        if metrics:
            metric_values = [results.get(metric, 0) for metric in metrics]
            axes[ax_idx].bar(range(len(metrics)), metric_values, alpha=0.7, color='skyblue', edgecolor='black')
            axes[ax_idx].set_xlabel('Metrics')
            axes[ax_idx].set_ylabel('Value')
            axes[ax_idx].set_title('Experiment Results Summary')
            axes[ax_idx].set_xticks(range(len(metrics)))
            axes[ax_idx].set_xticklabels(metrics, rotation=45)
            axes[ax_idx].grid(True, alpha=0.3)
            ax_idx += 1
        
        # Plot comparison if available
        if 'comparison' in results:
            comparison_data = results['comparison']
            if isinstance(comparison_data, dict):
                for i, (name, values) in enumerate(comparison_data.items()):
                    if i < 4 - ax_idx:  # Ensure we don't exceed subplot count
                        axes[ax_idx].plot(range(len(values)), values, linewidth=2, label=name)
                        axes[ax_idx].set_xlabel('Trial')
                        axes[ax_idx].set_ylabel('Value')
                        axes[ax_idx].set_title(f'Comparison: {name}')
                        axes[ax_idx].legend()
                        axes[ax_idx].grid(True, alpha=0.3)
                        ax_idx += 1
        
        # Plot convergence if available
        if 'convergence' in results:
            convergence_data = results['convergence']
            if isinstance(convergence_data, dict):
                for i, (name, values) in enumerate(convergence_data.items()):
                    if i < 4 - ax_idx:
                        axes[ax_idx].plot(range(len(values)), values, linewidth=2, label=name)
                        axes[ax_idx].set_xlabel('Iteration')
                        axes[ax_idx].set_ylabel('Value')
                        axes[ax_idx].set_title(f'Convergence: {name}')
                        axes[ax_idx].legend()
                        axes[ax_idx].grid(True, alpha=0.3)
                        ax_idx += 1
        
        # Hide unused subplots
        for j in range(ax_idx, 4):
            axes[j].set_visible(False)
        
        plt.suptitle(title, fontsize=16, fontweight='bold')
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Saved experiment results plot to {save_path}")
        
        return fig
    
    def plot_parameter_sweep_results(self, parameter_data: List[Dict[str, Any]],
                                    metric: str, title: str = "Parameter Sweep Results",
                                    save_path: Optional[str] = None) -> plt.Figure:
        """
        Plot parameter sweep results.
        
        Parameters
        ----------
        parameter_data : List[Dict[str, Any]]
            Parameter sweep data
        metric : str
            Metric to plot
        title : str, optional
            Plot title, by default "Parameter Sweep Results"
        save_path : Optional[str], optional
            Path to save figure, by default None
            
        Returns
        -------
        plt.Figure
            Matplotlib figure
        """
        if not parameter_data:
            fig, ax = plt.subplots(figsize=(8, 6))
            ax.text(0.5, 0.5, 'No data available', ha='center', va='center', fontsize=12)
            ax.set_title(title)
            return fig
        
        fig, axes = plt.subplots(2, 2, figsize=(15, 12))
        axes = axes.flatten()
        
        # Extract parameter names
        param_names = []
        for param_data in parameter_data:
            for key in param_data.keys():
                if key not in param_names and key != metric:
                    param_names.append(key)
        
        # Plot parameter vs metric for first two parameters
        for i in range(min(2, len(param_names))):
            param_name = param_names[i]
            
            # Get unique values for this parameter
            param_values = []
            metric_values = []
            
            for data_point in parameter_data:
                if param_name in data_point and metric in data_point:
                    param_values.append(data_point[param_name])
                    metric_values.append(data_point[metric])
            
            if param_values and metric_values:
                axes[i].scatter(param_values, metric_values, alpha=0.7, s=50)
                axes[i].set_xlabel(param_name)
                axes[i].set_ylabel(metric)
                axes[i].set_title(f'{metric} vs {param_name}')
                axes[i].grid(True, alpha=0.3)
        
        # Plot heatmap if we have 2D parameter space
        if len(param_names) >= 2 and len(parameter_data) > 0:
            param1, param2 = param_names[0], param_names[1]
            
            # Create pivot table
            pivot_data = []
            for data_point in parameter_data:
                if param1 in data_point and param2 in data_point and metric in data_point:
                    pivot_data.append({
                        param1: data_point[param1],
                        param2: data_point[param2],
                        metric: data_point[metric]
                    })
            
            if pivot_data:
                pivot_df = pd.DataFrame(pivot_data)
                try:
                    pivot_table = pivot_df.pivot_table(
                        values=metric, 
                        index=param1, 
                        columns=param2, 
                        aggfunc='mean'
                    )
                    
                    im = axes[2].imshow(pivot_table.values, aspect='auto', cmap='viridis',
                                      interpolation='nearest')
                    axes[2].set_xlabel(param2)
                    axes[2].set_ylabel(param1)
                    axes[2].set_title(f'Heatmap: {metric}')
                    plt.colorbar(im, ax=axes[2])
                    
                except Exception as e:
                    logger.warning(f"Could not create heatmap: {e}")
        
        # Hide unused subplots
        for j in range(len(param_names), 4):
            axes[j].set_visible(False)
        
        plt.suptitle(title, fontsize=16, fontweight='bold')
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Saved parameter sweep results plot to {save_path}")
        
        return fig


class InteractiveVisualizer:
    """Create interactive visualizations using Plotly."""
    
    def __init__(self):
        """Initialize interactive visualizer."""
        pass
    
    def create_interactive_neural_activity(self, activity: np.ndarray,
                                         time_steps: Optional[List[int]] = None,
                                         neuron_indices: Optional[List[int]] = None) -> go.Figure:
        """
        Create interactive neural activity plot.
        
        Parameters
        ----------
        activity : np.ndarray
            Neural activity data (time x neurons)
        time_steps : Optional[List[int]], optional
            Time steps, by default None
        neuron_indices : Optional[List[int]], optional
            Neuron indices to plot, by default None
            
        Returns
        -------
        go.Figure
            Plotly figure
        """
        if time_steps is None:
            time_steps = list(range(activity.shape[0]))
        
        if neurons is None:
            # Plot heatmap
            fig = go.Figure(data=go.Heatmap(
                x=time_steps,
                y=list(range(activity.shape[1])),
                z=activity,
                colorscale='Viridis',
                colorbar=dict(title='Activity')
            ))
            
            fig.update_layout(
                title='Neural Activity Heatmap',
                xaxis_title='Time Step',
                yaxis_title='Neuron Index',
                width=800,
                height=600
            )
            
        else:
            # Plot traces for selected neurons
            fig = go.Figure()
            
            for neuron_idx in neuron_indices:
                if neuron_idx < activity.shape[1]:
                    fig.add_trace(go.Scatter(
                        x=time_steps,
                        y=activity[:, neuron_idx],
                        mode='lines',
                        name=f'Neuron {neuron_idx}',
                        line=dict(width=2)
                    ))
            
            fig.update_layout(
                title='Neural Activity Traces',
                xaxis_title='Time Step',
                yaxis_title='Activity',
                width=800,
                height=600,
                showlegend=True
            )
        
        return fig
    
    def create_interactive_simulation_trajectories(self, trajectories: List[List[Tuple[float, float]]],
                                                 agent_ids: Optional[List[int]] = None) -> go.Figure:
        """
        Create interactive simulation trajectory plot.
        
        Parameters
        ----------
        trajectories : List[List[Tuple[float, float]]]
            List of agent trajectories
        agent_ids : Optional[List[int]], optional
            Agent IDs, by default None
            
        Returns
        -------
        go.Figure
            Plotly figure
        """
        if agent_ids is None:
            agent_ids = list(range(len(trajectories)))
        
        fig = go.Figure()
        
        for i, trajectory in enumerate(trajectories):
            if len(trajectory) > 1:
                x_coords = [pos[0] for pos in trajectory]
                y_coords = [pos[1] for pos in trajectory]
                
                fig.add_trace(go.Scatter(
                    x=x_coords,
                    y=y_coords,
                    mode='lines+markers',
                    name=f'Agent {agent_ids[i]}',
                    line=dict(width=2),
                    marker=dict(size=6)
                ))
        
        fig.update_layout(
            title='Agent Trajectories',
            xaxis_title='X Position',
            yaxis_title='Y Position',
            width=800,
            height=600,
            showlegend=True,
            hovermode='closest'
        )
        
        return fig
    
    def create_interactive_3d_activity(self, activity_3d: np.ndarray) -> go.Figure:
        """
        Create interactive 3D activity plot.
        
        Parameters
        ----------
        activity_3d : np.ndarray
            3D activity data (time x neurons x features)
            
        Returns
        -------
        go.Figure
            Plotly figure
        """
        fig = go.Figure(data=[go.Surface(z=activity_3d)])
        
        fig.update_layout(
            title='3D Neural Activity',
            scene=dict(
                xaxis_title='Time',
                yaxis_title='Neuron',
                zaxis_title='Feature'
            ),
            width=800,
            height=700
        )
        
        return fig
    
    def save_plotly_figure(self, fig: go.Figure, path: str) -> None:
        """
        Save Plotly figure to HTML file.
        
        Parameters
        ----------
        fig : go.Figure
            Plotly figure
        path : str
            Path to save HTML file
        """
        fig.write_html(path)
        logger.info(f"Saved interactive plot to {path}")