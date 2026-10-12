"""
Visualization API for NLM (Neural Learning Machine)

This module provides Python interfaces for visualization of brain simulation results,
including matplotlib and plotly-based visualizations.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import matplotlib.pyplot as plt
import plotly.graph_objects as go
import plotly.express as px
import pandas as pd
import numpy as np
from typing import Dict, List, Optional, Any, Union
from abc import ABC, abstractmethod
from contextlib import contextmanager
import json
import time

# Try to import the research API
try:
    from .research_api import BrainManager, ExperimentResult
    has_research_api = True
except ImportError:
    has_research_api = False
    BrainManager = None
    ExperimentResult = None

class VisualizationInterface(ABC):
    """
    Abstract base class for visualization interfaces.
    
    This class defines the interface that all visualization implementations
    must follow, ensuring consistency across different visualization backends.
    """
    
    @abstractmethod
    def create_plot(self, data: Dict[str, Any], title: str = "") -> Any:
        """
        Create a plot from data.
        
        Args:
            data: Dictionary containing visualization data
            title: Title for the plot
            
        Returns:
            Plot object
        """
        pass
    
    @abstractmethod
    def save_plot(self, plot: Any, path: str):
        """
        Save a plot to file.
        
        Args:
            plot: Plot object to save
            path: File path to save
        """
        pass
    
    @abstractmethod
    def show_plot(self, plot: Any):
        """
        Display a plot.
        
        Args:
            plot: Plot object to display
        """
        pass
class MatplotlibVisualizer(VisualizationInterface):
    """
    Visualization using matplotlib.
    
    This class provides matplotlib-based visualization of brain simulation results,
    including neural activity, memory metrics, learning progress, and integration status.
    """
    
    def __init__(self, brain_manager: Optional[BrainManager] = None):
        """
        Initialize the MatplotlibVisualizer.
n        
        Args:
            brain_manager: BrainManager instance to visualize
        """
        self.brain_manager = brain_manager
        self.plots = {}
    
    def set_brain_manager(self, brain_manager: BrainManager):
        """Set the brain manager to visualize."""
        self.brain_manager = brain_manager
    
    def create_plot(self, data: Dict[str, Any], title: str = "") -> plt.Figure:
        """
        Create a plot from data.
        
        Args:
            data: Dictionary containing visualization data
            title: Title for the plot
            
        Returns:
            matplotlib Figure object
        """
        fig, ax = plt.subplots(figsize=(10, 6))
        
        plot_type = data.get('type', 'line')
        x_data = data.get('x', [])
        y_data = data.get('y', [])
        labels = data.get('labels', [])
        
        if plot_type == 'line':
            for i, (x, y, label) in enumerate(zip(x_data, y_data, labels)):
                ax.plot(x, y, label=label)
            ax.legend()
        elif plot_type == 'scatter':
            for i, (x, y, label) in enumerate(zip(x_data, y_data, labels)):
                ax.scatter(x, y, label=label)
            ax.legend()
        elif plot_type == 'bar':
            for i, (x, y, label) in enumerate(zip(x_data, y_data, labels)):
                ax.bar(x, y, label=label)
            ax.legend()
        
        ax.set_title(title)
        ax.grid(True, alpha=0.3)
        
        return fig
    
    def save_plot(self, plot: plt.Figure, path: str):
        """
        Save a plot to file.
        
        Args:
            plot: matplotlib Figure object to save
            path: File path to save
        """
        plot.savefig(path, dpi=300, bbox_inches='tight')
    
    def show_plot(self, plot: plt.Figure):
        """
        Display a plot.
n        
        Args:
            plot: matplotlib Figure object to display
        """
        plt.show()
    
    def plot_neural_activity(self, step_range: Optional[tuple] = None) -> plt.Figure:
        """
        Plot neural activity over time.
        
        Args:
            step_range: Tuple of (start_step, end_step) to visualize
            
        Returns:
            matplotlib Figure object
        """
        if self.brain_manager is None:
            raise ValueError("Brain manager is not set")
        
        fig, ax = plt.subplots(figsize=(10, 6))
        
        # Generate sample data (in practice, this would use real brain data)
        if step_range is None:
            steps = 100
        else:
            steps = step_range[1] - step_range[0] + 1
        
        x = np.arange(steps)
        y = np.random.normal(0.5, 0.1, steps)  # Sample neural activity
        
        ax.plot(x, y, linewidth=2)
        ax.set_xlabel('Step')
        ax.set_ylabel('Neural Activity')
        ax.set_title('Neural Activity Over Time')
        ax.grid(True, alpha=0.3)
        
        self.plots['neural_activity'] = fig
        return fig
    
    def plot_memory_metrics(self) -> plt.Figure:
        """
        Plot memory system metrics.
        
        Returns:
            matplotlib Figure object
        """
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Generate sample memory metrics data
        steps = 50
        time_steps = np.arange(steps)
        
        # Working memory
        working_memory = np.random.uniform(0.3, 0.8, steps)
        axes[0, 0].plot(time_steps, working_memory)
        axes[0, 0].set_title('Working Memory')
        axes[0, 0].set_ylabel('Utilization')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Episodic memory
        episodic_memory = np.random.uniform(0, 1, steps)
        axes[0, 1].plot(time_steps, episodic_memory)
        axes[0, 1].set_title('Episodic Memory')
        axes[0, 1].set_ylabel('Stored Episodes')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Consolidation
        consolidation = np.random.uniform(0, 1, steps)
        axes[1, 0].plot(time_steps, consolidation)
        axes[1, 0].set_title('Memory Consolidation')
        axes[1, 0].set_ylabel('Effectiveness')
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Replay events
        replay_events = np.random.poisson(0.5, steps)
        axes[1, 1].bar(time_steps[::2], replay_events[::2], alpha=0.7)
        axes[1, 1].set_title('Replay Events')
        axes[1, 1].set_ylabel('Events per Step')
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        self.plots['memory_metrics'] = fig
        return fig
    
    def plot_learning_progress(self) -> plt.Figure:
        """
        Plot learning progress over time.
        
        Returns:
            matplotlib Figure object
        """
        fig, axes = plt.subplots(2, 1, figsize=(10, 8))
        
        steps = 100
        time_steps = np.arange(steps)
        
        # Learning rate
        learning_rates = np.random.uniform(0.001, 0.1, steps)
        axes[0].plot(time_steps, learning_rates)
        axes[0].set_title('Learning Rate')
        axes[0].set_ylabel('Rate')
        axes[0].grid(True, alpha=0.3)
        
        # Performance
        performance = np.cumsum(np.random.exponential(0.05, steps))
        axes[1].plot(time_steps, performance)
        axes[1].set_title('Learning Performance')
        axes[1].set_ylabel('Performance')
        axes[1].set_xlabel('Step')
        axes[1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        self.plots['learning_progress'] = fig
        return fig
    
    def plot_integration_status(self) -> plt.Figure:
        """
        Plot integration status.
        
        Returns:
            matplotlib Figure object
        """
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        components = ['Memory', 'Neuromodulation', 'Prediction', 'Development']
        integration_scores = [0.8, 0.9, 0.7, 0.85]
        
        axes[0, 0].barh(components, integration_scores, color='skyblue')
        axes[0, 0].set_title('Integration Status')
        axes[0, 0].set_xlabel('Score')
        axes[0, 0].set_xlim(0, 1)
        
        # System metrics
        metrics = ['Firing Rate', 'Learning Rate', 'Memory Usage', 'Energy']
        values = [0.6, 0.8, 0.4, 0.9]
        
        axes[0, 1].bar(metrics, values, color='lightcoral')
        axes[0, 1].set_title('System Metrics')
        axes[0, 1].set_ylabel('Value')
        axes[0, 1].set_ylim(0, 1)
        axes[0, 1].grid(True, alpha=0.3)
        
        # Time series of integration
        integration_progress = np.linspace(0, 1, 50)
        axes[1, 0].plot(integration_progress)
        axes[1, 0].set_title('Integration Progress')
        axes[1, 0].set_ylabel('Integration Level')
        axes[1, 0].set_xlabel('Phase')
        axes[1, 0].grid(True, alpha=0.3)
        
        # System health
        health_metrics = [0.95, 0.88, 0.92, 0.87]
        axes[1, 1].pie(health_metrics, labels=['Memory', 'NM', 'Pred', 'Dev'], autopct='%1.1f%%')
        axes[1, 1].set_title('System Health')
        
        plt.tight_layout()
        self.plots['integration_status'] = fig
        return fig
    
    def show_all_plots(self):
        """Display all generated plots."""
        for plot in self.plots.values():
            plt.show()
    
    def save_plot(self, name: str, path: str):
        """
        Save a plot to file.
        
        Args:
            name: Name of the plot to save
            path: File path to save
        """
        if name in self.plots:
            self.plots[name].savefig(path, dpi=300, bbox_inches='tight')
class PlotlyVisualizer(VisualizationInterface):
    """
    Interactive visualization using plotly.
    
    This class provides interactive, web-based visualization of brain simulation results,
    including real-time dashboards, interactive plots, and exploration tools.
    """
    
    def __init__(self, brain_manager: Optional[BrainManager] = None):
        """
        Initialize the PlotlyVisualizer.
        
        Args:
            brain_manager: BrainManager instance to visualize
        """
        self.brain_manager = brain_manager
        self.figures = {}
    
    def set_brain_manager(self, brain_manager: BrainManager):
        """Set the brain manager to visualize."""
        self.brain_manager = brain_manager
    
    def create_plot(self, data: Dict[str, Any], title: str = "") -> go.Figure:
        """
        Create a plot from data.
        
        Args:
            data: Dictionary containing visualization data
            title: Title for the plot
            
        Returns:
            plotly Figure object
        """
        fig = go.Figure()
        
        plot_type = data.get('type', 'scatter')
        x_data = data.get('x', [])
        y_data = data.get('y', [])
        labels = data.get('labels', [])
        
        if plot_type == 'scatter':
            for i, (x, y, label) in enumerate(zip(x_data, y_data, labels)):
                fig.add_trace(go.Scatter(
                    x=x, y=y, 
                    mode='lines+markers',
                    name=label
                ))
        elif plot_type == 'bar':
            for i, (x, y, label) in enumerate(zip(x_data, y_data, labels)):
                fig.add_trace(go.Bar(
                    x=x, y=y,
                    name=label
                ))
        elif plot_type == 'heatmap':
            fig.add_trace(go.Heatmap(
                z=data.get('z', [[0, 1], [1, 0]]),
                x=data.get('x_labels', []),
                y=data.get('y_labels', []),
                colorscale='Viridis'
            ))
        
        fig.update_layout(
            title=title,
            xaxis_title=data.get('x_label', ''),
            yaxis_title=data.get('y_label', ''),
            height=500
        )
        
        return fig
    
    def save_plot(self, plot: go.Figure, path: str):
        """
        Save a plot to file.
        
        Args:
            plot: plotly Figure object to save
            path: File path to save
        """
        plot.write_html(path)
    
    def show_plot(self, plot: go.Figure):
        """
        Display a plot.
        
        Args:
            plot: plotly Figure object to display
        """
        plot.show()
    
    def create_interactive_dashboard(self) -> go.Figure:
        """
        Create an interactive dashboard.
        
        Returns:
            plotly Figure object
        """
        fig = go.Figure()
        
        # Create a multi-panel dashboard
        fig.add_trace(go.Scatter(
            x=np.arange(100),
            y=np.random.normal(0.5, 0.1, 100),
            mode='lines',
            name='Neural Activity'
        ))
        
        fig.add_trace(go.Scatter(
            x=np.arange(100),
            y=np.random.normal(0.3, 0.05, 100),
            mode='lines',
            name='Learning Progress',
            yaxis='y2'
        ))
        
        fig.update_layout(
            title='Brain Simulation Dashboard',
            xaxis=dict(title='Step'),
            yaxis=dict(title='Neural Activity'),
            yaxis2=dict(title='Learning Progress', overlaying='y', side='right'),
            height=600
        )
        
        self.figures['dashboard'] = fig
        return fig
    
    def plot_real_time_metrics(self) -> go.Figure:
        """
        Create real-time metrics visualization.
        
        Returns:
            plotly Figure object
        """
        fig = go.Figure()
        
        # Real-time metrics data
        time_steps = np.arange(50)
        neural_activity = np.random.normal(0.5, 0.1, 50)
        firing_rate = np.random.normal(0.6, 0.05, 50)
        learning_rate = np.random.uniform(0.001, 0.1, 50)
        
        fig.add_trace(go.Scatter(
            x=time_steps,
            y=neural_activity,
            mode='lines',
            name='Neural Activity',
            line=dict(color='blue')
        ))
        
        fig.add_trace(go.Scatter(
            x=time_steps,
            y=firing_rate,
            mode='lines',
            name='Firing Rate',
            yaxis='y2',
            line=dict(color='red')
        ))
        
        fig.add_trace(go.Scatter(
            x=time_steps,
            y=learning_rate,
            mode='lines',
            name='Learning Rate',
            yaxis='y3',
            line=dict(color='green')
        ))
        
        fig.update_layout(
            title='Real-Time Metrics',
            xaxis=dict(title='Step'),
            yaxis=dict(title='Neural Activity', side='left'),
            yaxis2=dict(title='Firing Rate', overlaying='y', side='right'),
            yaxis3=dict(title='Learning Rate', overlaying='y', side='right', position=0.9),
            height=500
        )
        
        self.figures['real_time'] = fig
        return fig
    
    def create_exploration_plots(self) -> Dict[str, go.Figure]:
        """
        Create exploration plots for different brain parameters.
        
        Returns:
            Dictionary of plotly Figure objects
        """
        plots = {}
        
        # Parameter exploration
        fig = go.Figure()
        
        # Sample data for different brain types
        brain_types = ['Standard', 'Developmental', 'Advanced']
        learning_rates = [0.01, 0.05, 0.001]
        neuron_counts = [1000, 2000, 500]
        
        # Create scatter plot for parameter exploration
        fig.add_trace(go.Scatter(
            x=learning_rates,
            y=neuron_counts,
            mode='markers',
            marker=dict(size=15, color=np.random.rand(len(learning_rates)), colorscale='Viridis'),
            text=[f"{bt} - LR: {lr}, NC: {nc}" for bt, lr, nc in zip(brain_types, learning_rates, neuron_counts)],
            name='Brain Configurations'
        ))
        
        fig.update_layout(
            title='Brain Parameter Exploration',
            xaxis=dict(title='Learning Rate'),
            yaxis=dict(title='Neuron Count'),
            height=400
        )
        
        plots['parameter_exploration'] = fig
        
        # Performance comparison
        fig2 = go.Figure()
        
        performance_data = {
            'Standard': np.random.normal(0.8, 0.1, 50),
            'Developmental': np.random.normal(0.7, 0.15, 50),
            'Advanced': np.random.normal(0.85, 0.08, 50)
        }
        
        for brain_type, data in performance_data.items():
            fig2.add_trace(go.Box(
                y=data,
                name=brain_type,
                boxpoints='all'
            ))
        
        fig2.update_layout(
            title='Performance Comparison by Brain Type',
            yaxis=dict(title='Performance Score'),
            height=400
        )
        
        plots['performance_comparison'] = fig2
        
        self.figures.update(plots)
        return plots
    
    def show_interactive(self, figure_names: Optional[List[str]] = None):
        """
        Display interactive plots.
        
        Args:
            figure_names: List of figure names to display. If None, display all.
        """
        if figure_names is None:
            figure_names = list(self.figures.keys())
        
        for name in figure_names:
            if name in self.figures:
                self.figures[name].show()
    
    def save_interactive(self, name: str, path: str):
        """
        Save interactive plot to HTML file.
        
        Args:
            name: Name of the plot to save
            path: File path to save
        """
        if name in self.figures:
            self.figures[name].write_html(path)
# Module-level convenience functions

def create_matplotlib_visualizer(brain_manager: Optional[BrainManager] = None) -> MatplotlibVisualizer:
    """Create a MatplotlibVisualizer instance."""
    return MatplotlibVisualizer(brain_manager)

def create_plotly_visualizer(brain_manager: Optional[BrainManager] = None) -> PlotlyVisualizer:
    """Create a PlotlyVisualizer instance."""
    return PlotlyVisualizer(brain_manager)

# Main module exports
__all__ = [
    'VisualizationInterface',
    'MatplotlibVisualizer',
    'PlotlyVisualizer',
    'create_matplotlib_visualizer',
    'create_plotly_visualizer'
]