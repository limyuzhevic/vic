#!/usr/bin/env python3
"""
Visualization and Plotting Utilities for NLM

This module provides visualization and plotting capabilities for NLM simulations,
including neural activity visualization, simulation progress tracking, and
data analysis plots.
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.widgets import Slider, Button
import seaborn as sns
from typing import Dict, Any, List, Optional, Tuple, Callable
import pandas as pd
import io
import base64
from datetime import datetime
import threading
import queue
import time
from pathlib import Path
import json


class NeuralVisualization:
    """
    Comprehensive visualization system for NLM neural activity and simulation results.
    
    Provides various visualization modes for:
    - Neural network structure
    - Spike activity over time
    - Synaptic connectivity
    - Simulation progress
    - Performance metrics
    """
    
    def __init__(self, width: int = 12, height: int = 8):
        self.width = width
        self.height = height
        self.figure = None
        self.axes = None
        self.current_plot_type = None
        self.visualization_data = {}
        
        # Setup visualization style
        plt.style.use('default')
        sns.set_palette("husl")
    
    def create_interactive_dashboard(self, brain: pynlm.Brain,
                                    world: Optional[pynlm.SimpleWorld] = None,
                                    agent: Optional[pynlm.AgentBrain] = None) -> 'InteractiveDashboard':
        """
        Create an interactive dashboard for real-time visualization.
        
        Args:
            brain: The brain to visualize
            world: Optional world state
            agent: Optional agent state
            
        Returns:
            InteractiveDashboard instance
        """
        return InteractiveDashboard(self, brain, world, agent)
    
    def plot_neural_network(self, brain: pynlm.Brain, 
                           layout: str = 'force-directed',
                           show_synapses: bool = True,
                           highlight_active: bool = True) -> plt.Figure:
        """
        Create visualization of neural network structure.
        
        Args:
            brain: Brain to visualize
            layout: Layout algorithm ('force-directed', 'circular', 'grid')
            show_synapses: Whether to show synaptic connections
            highlight_active: Highlight active neurons
            
        Returns:
            Matplotlib figure
        """
        fig, ax = plt.subplots(self.width, self.height, figsize=(self.width*2, self.height*2))
        
        # Get neuron information
        regions = brain.getRegions()
        neurons_per_region = []
        region_ids = []
        
        for region in regions:
            population = region.getPopulation()
            neurons = population.getNeurons()
            neurons_per_region.append(len(neurons))
            region_ids.append(region.getRegionId())
        
        # Create network visualization
        if layout == 'force-directed':
            self._plot_force_directed_network(ax, neurons_per_region, region_ids, show_synapses, highlight_active)
        elif layout == 'circular':
            self._plot_circular_network(ax, neurons_per_region, region_ids, show_synapses, highlight_active)
        elif layout == 'grid':
            self._plot_grid_network(ax, neurons_per_region, region_ids, show_synapses, highlight_active)
        
        ax.set_title(f"Neural Network Structure ({brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses)")
        
        return fig
    
    def _plot_force_directed_network(self, ax, neurons_per_region: List[int], 
                                    region_ids: List[Any], show_synapses: bool, 
                                    highlight_active: bool):
        """Create force-directed network visualization"""
        # Simplified force-directed layout
        n_regions = len(neurons_per_region)
        
        # Calculate positions using simple spring embedding
        positions = []
        for i, neurons in enumerate(neurons_per_region):
            angle = 2 * np.pi * i / n_regions
            x = np.cos(angle)
            y = np.sin(angle)
            positions.append((x, y, neurons))
        
        # Plot regions as circles
        for i, (x, y, neurons) in enumerate(positions):
            radius = np.sqrt(neurons / np.pi) * 0.1
            circle = plt.Circle((x, y), radius, alpha=0.6, label=f"Region {region_ids[i]}")
            ax.add_patch(circle)
            
            if highlight_active:
                # Add active indicator
                center_circle = plt.Circle((x, y), radius * 0.8, alpha=0.8, color='red')
                ax.add_patch(center_circle)
        
        # Plot synapses as lines between regions
        if show_synapses:
            for i in range(n_regions):
                for j in range(i + 1, n_regions):
                    x1, y1, _ = positions[i]
                    x2, y2, _ = positions[j]
                    distance = np.sqrt((x2 - x1)**2 + (y2 - y1)**2)
                    
                    # Only show connections if regions are close enough
                    if distance < 2.5:
                        alpha = 1.0 / (distance + 0.1)
                        ax.plot([x1, x2], [y1, y2], 'gray', alpha=alpha, linewidth=0.5)
        
        ax.set_xlim(-1.5, 1.5)
        ax.set_ylim(-1.5, 1.5)
        ax.axis('off')
    
    def _plot_circular_network(self, ax, neurons_per_region: List[int],
                              region_ids: List[Any], show_synapses: bool,
                              highlight_active: bool):
        """Create circular network visualization"""
        n_regions = len(neurons_per_region)
        
        # Place regions in a circle
        for i, neurons in enumerate(neurons_per_region):
            angle = 2 * np.pi * i / n_regions
            x = np.cos(angle)
            y = np.sin(angle)
            
            # Draw region circle
            radius = np.sqrt(neurons / np.pi) * 0.15
            circle = plt.Circle((x, y), radius, alpha=0.7, label=f"Region {region_ids[i]}")
            ax.add_patch(circle)
            
            if highlight_active:
                # Add neuron count
                ax.text(x, y, str(neurons), ha='center', va='center', 
                       fontsize=8, fontweight='bold')
        
        ax.set_xlim(-1.2, 1.2)
        ax.set_ylim(-1.2, 1.2)
        ax.axis('off')
    
    def _plot_grid_network(self, ax, neurons_per_region: List[int],
                          region_ids: List[Any], show_synapses: bool,
                          highlight_active: bool):
        """Create grid network visualization"""
        n_regions = len(neurons_per_region)
        grid_size = int(np.ceil(np.sqrt(n_regions)))
        
        for i, neurons in enumerate(neurons_per_region):
            row = i // grid_size
            col = i % grid_size
            
            # Normalize position
            x = (col - (grid_size - 1) / 2) * 0.4
            y = (row - (grid_size - 1) / 2) * 0.4
            
            # Draw region
            radius = np.sqrt(neurons / np.pi) * 0.2
            circle = plt.Circle((x, y), radius, alpha=0.7, label=f"Region {region_ids[i]}")
            ax.add_patch(circle)
            
            if highlight_active:
                ax.text(x, y, str(neurons), ha='center', va='center', 
                       fontsize=8, fontweight='bold')
        
        ax.set_xlim(-1.5, 1.5)
        ax.set_ylim(-1.5, 1.5)
        ax.axis('off')
    
    def plot_activity_over_time(self, brain: pynlm.Brain, steps: int = 1000,
                               window_size: int = 50) -> plt.Figure:
        """
        Plot neural activity over time.
        
        Args:
            brain: Brain to analyze
            steps: Number of steps to simulate
            window_size: Window size for moving average
            
        Returns:
            Matplotlib figure
        """
        # Collect activity data
        spike_counts = []
        firing_rates = []
        
        for step in range(steps):
            brain.step(step)
            
            # Get current activity
            firing_rate = brain.getAverageFiringRate()
            spike_count = brain.getTotalSpikeCount()
            
            spike_counts.append(spike_count)
            firing_rates.append(firing_rate)
        
        # Calculate moving averages
        if window_size > 1 and len(spike_counts) > window_size:
            spike_ma = np.convolve(spike_counts, np.ones(window_size)/window_size, mode='valid')
            firing_ma = np.convolve(firing_rates, np.ones(window_size)/window_size, mode='valid')
            time_ma = np.arange(window_size - 1, len(spike_counts))
        else:
            spike_ma = spike_counts
            firing_ma = firing_rates
            time_ma = np.arange(len(spike_counts))
        
        # Create figure with two subplots
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(self.width, self.height * 2))
        
        # Plot spike counts
        ax1.plot(time_ma, spike_ma, 'r-', alpha=0.7, linewidth=1, label='Spike Count')
        ax1.set_ylabel('Spike Count')
        ax1.set_title('Neural Activity Over Time')
        ax1.legend()
        ax1.grid(True, alpha=0.3)
        
        # Plot firing rates
        ax2.plot(time_ma, firing_ma, 'b-', alpha=0.7, linewidth=1, label='Firing Rate')
        ax2.set_xlabel('Time (steps)')
        ax2.set_ylabel('Firing Rate (Hz)')
        ax2.legend()
        ax2.grid(True, alpha=0.3)
        
        plt.tight_layout()
        return fig
    
    def plot_synapse_dynamics(self, brain: pynlm.Brain, steps: int = 500) -> plt.Figure:
        """
        Plot synaptic weight dynamics over time.
        
        Args:
            brain: Brain to analyze
            steps: Number of steps to simulate
            
        Returns:
            Matplotlib figure
        """
        # Collect synapse weight data
        weights_history = []
        
        for step in range(steps):
            # Record current synapse weights
            current_weights = []
            for region in brain.getRegions():
                for synapse in region.getSynapses():
                    current_weights.append(synapse.getWeight())
            
            weights_history.append(current_weights)
            
            # Step brain
            brain.step(step)
        
        # Calculate statistics
        weight_means = [np.mean(weights) if weights else 0 for weights in weights_history]
        weight_stds = [np.std(weights) if weights else 0 for weights in weights_history]
        weight_min = [min(weights) if weights else 0 for weights in weights_history]
        weight_max = [max(weights) if weights else 0 for weights in weights_history]
        
        # Create figure
        fig, axes = plt.subplots(3, 1, figsize=(self.width, self.height * 3))
        
        # Plot weight means
        axes[0].plot(weight_means, 'g-', alpha=0.7, linewidth=1, label='Mean Weight')
        axes[0].set_ylabel('Mean Weight')
        axes[0].set_title('Synaptic Weight Dynamics')
        axes[0].legend()
        axes[0].grid(True, alpha=0.3)
        
        # Plot weight standard deviation
        axes[1].plot(weight_stds, 'r-', alpha=0.7, linewidth=1, label='Std Dev')
        axes[1].set_ylabel('Weight Standard Deviation')
        axes[1].legend()
        axes[1].grid(True, alpha=0.3)
        
        # Plot weight range
        axes[2].plot(weight_min, 'b-', alpha=0.7, linewidth=1, label='Min')
        axes[2].plot(weight_max, 'r-', alpha=0.7, linewidth=1, label='Max')
        axes[2].fill_between(range(len(weight_min)), weight_min, weight_max, alpha=0.3, label='Range')
        axes[2].set_ylabel('Weight Value')
        axes[2].set_xlabel('Time Step')
        axes[2].legend()
        axes[2].grid(True, alpha=0.3)
        
        plt.tight_layout()
        return fig
    
    def plot_performance_metrics(self, performance_data: Dict[str, Any]) -> plt.Figure:
        """
        Create visualization of performance metrics.
        
        Args:
            performance_data: Performance data dictionary
            
        Returns:
            Matplotlib figure
        """
        # Create multi-panel figure
        fig, axes = plt.subplots(3, 3, figsize=(self.width * 3, self.height * 3))
        fig.suptitle('NLM Performance Metrics', fontsize=16, fontweight='bold')
        
        # Flatten axes for easy access
        axes_flat = axes.flatten()
        
        # Panel 1: Performance score distribution
        if 'overall_performance' in performance_data:
            scores = []
            for sim_name, sim_data in performance_data['simulations'].items():
                if 'overall_performance' in sim_data:
                    scores.append(sim_data['overall_performance'].get('overall_score', 0))
            
            if scores:
                axes_flat[0].hist(scores, bins=10, alpha=0.7, color='skyblue', edgecolor='black')
                axes_flat[0].set_title('Performance Score Distribution')
                axes_flat[0].set_xlabel('Score (0-100)')
                axes_flat[0].set_ylabel('Frequency')
                axes_flat[0].axvline(np.mean(scores), color='red', linestyle='--', label=f'Mean: {np.mean(scores):.1f}')
                axes_flat[0].legend()
        
        # Panel 2: Time vs Memory tradeoff
        if 'time_analysis' in performance_data and 'memory_analysis' in performance_data:
            time_scores = []
            memory_scores = []
            for sim_name, sim_data in performance_data['simulations'].items():
                if 'time_analysis' in sim_data and 'memory_analysis' in sim_data:
                    time_scores.append(sim_data['time_analysis'].get('steps_per_second', 0) / 1000)  # Normalize
                    memory_scores.append(sim_data['memory_analysis'].get('brain_memory', 0) / 100)  # Normalize
            
            if time_scores and memory_scores:
                axes_flat[1].scatter(time_scores, memory_scores, alpha=0.7, s=50)
                axes_flat[1].set_title('Time vs Memory Efficiency')
                axes_flat[1].set_xlabel('Time Efficiency')
                axes_flat[1].set_ylabel('Memory Efficiency')
                axes_flat[1].grid(True, alpha=0.3)
        
        # Panel 3: Bottleneck analysis
        bottleneck_counts = []
        for sim_name, sim_data in performance_data.get('simulations', {}).items():
            bottlenecks = sim_data.get('performance_metrics', {}).get('bottleneck_analysis', {})
            bottleneck_counts.append(len(bottlenecks))
        
        if bottleneck_counts:
            axes_flat[2].bar(range(len(bottleneck_counts)), bottleneck_counts, alpha=0.7, color='lightgreen')
            axes_flat[2].set_title('Bottlenecks per Simulation')
            axes_flat[2].set_xlabel('Simulation Index')
            axes_flat[2].set_ylabel('Number of Bottlenecks')
            axes_flat[2].grid(True, alpha=0.3)
        
        # Additional panels (simplified for brevity)
        for i in range(3, len(axes_flat)):
            axes_flat[i].text(0.5, 0.5, 'Additional plots available\nin performance_profiler module', 
                           ha='center', va='center', transform=axes_flat[i].transAxes)
            axes_flat[i].set_title('Extended Analysis')
            axes_flat[i].axis('off')
        
        plt.tight_layout()
        return fig


class InteractiveDashboard:
    """
    Interactive dashboard for real-time NLM visualization and control.
    
    Provides an interactive interface for:
    - Real-time monitoring of neural activity
    - Control over simulation parameters
    - Visualization switching
    - Performance tracking
    """
    
    def __init__(self, visualizer: NeuralVisualization, brain: pynlm.Brain,
                 world: Optional[pynlm.SimpleWorld] = None,
                 agent: Optional[pynlm.AgentBrain] = None):
        self.visualizer = visualizer
        self.brain = brain
        self.world = world
        self.agent = agent
        
        self.current_step = 0
        self.auto_update = True
        self.update_interval = 100  # milliseconds
        
        # Create main figure
        self.fig = plt.figure(figsize=(15, 10))
        self.fig.suptitle('NLM Interactive Dashboard', fontsize=16, fontweight='bold')
        
        # Create subplot grid
        self.gs = self.fig.add_gridspec(3, 4, hspace=0.3, wspace=0.3)
        
        # Initialize plots
        self._create_plots()
        
        # Create control panels
        self._create_controls()
        
        # Start update thread
        self.update_thread = threading.Thread(target=self._update_loop, daemon=True)
        self.update_thread.start()
    
    def _create_plots(self):
        """Create visualization plots"""
        # Network structure plot
        self.ax_network = self.fig.add_subplot(self.gs[0, :2])
        
        # Activity over time plot
        self.ax_activity = self.fig.add_subplot(self.gs[0, 2:])
        
        # Performance metrics plot
        self.ax_performance = self.fig.add_subplot(self.gs[1, :2])
        
        # Simulation controls
        self.ax_controls = self.fig.add_subplot(self.gs[1, 2:])
        
        # Real-time metrics
        self.ax_metrics = self.fig.add_subplot(self.gs[2, :4])
    
    def _create_controls(self):
        """Create interactive controls"""
        # Simulation control buttons
        self.btn_start = Button(self.ax_controls, 'Start/Stop', 
                               on_click=self._toggle_simulation)
        self.btn_step = Button(self.ax_controls, 'Step', 
                              on_click=lambda event: self._manual_step())
        self.btn_reset = Button(self.ax_controls, 'Reset', 
                               on_click=lambda event: self._reset_simulation())
        
        # Parameter sliders
        self.slider_speed = Slider(self.ax_controls, 'Speed', 0.1, 10.0, 
                                  valinit=self.update_interval/100, valstep=0.1)
        self.slider_steps = Slider(self.ax_controls, 'Steps', 100, 5000, 
                                  valinit=1000, valstep=100)
    
    def _update_loop(self):
        """Main update loop for real-time visualization"""
        while True:
            if self.auto_update:
                self._update_plots()
            time.sleep(self.update_interval / 1000)
    
    def _update_plots(self):
        """Update all plots with current simulation state"""
        try:
            # Update network visualization
            self._update_network_plot()
            
            # Update activity plot
            self._update_activity_plot()
            
            # Update performance plot
            self._update_performance_plot()
            
            # Update metrics
            self._update_metrics()
            
            # Refresh display
            self.fig.canvas.draw()
            
        except Exception as e:
            print(f"Error updating plots: {e}")
    
    def _update_network_plot(self):
        """Update network structure plot"""
        self.ax_network.clear()
        
        # Get regions and neurons
        regions = self.brain.getRegions()
        
        # Create simple representation
        for i, region in enumerate(regions):
            neurons = region.getPopulation().getNeurons()
            
            # Draw region as point
            x = (i % 3) * 2 - 1
            y = -(i // 3) * 2
            
            # Determine color based on activity
            activity = min(1.0, len(neurons) / 100)
            color = 'red' if activity > 0.5 else 'blue'
            
            self.ax_network.scatter(x, y, s=len(neurons) * 10, c=color, alpha=0.7)
            self.ax_network.text(x, y + 0.5, str(len(neurons)), 
                               ha='center', va='bottom')
        
        self.ax_network.set_xlim(-3, 3)
        self.ax_network.set_ylim(-3, 3)
        self.ax_network.set_title('Neural Network')
        self.ax_network.grid(True, alpha=0.3)
    
    def _update_activity_plot(self):
        """Update activity over time plot"""
        # Simplified activity tracking
        if not hasattr(self, 'activity_history'):
            self.activity_history = []
        
        # Get current firing rate
        firing_rate = self.brain.getAverageFiringRate()
        self.activity_history.append(firing_rate)
        
        # Keep history manageable
        if len(self.activity_history) > 100:
            self.activity_history = self.activity_history[-100:]
        
        self.ax_activity.clear()
        self.ax_activity.plot(self.activity_history, 'b-', alpha=0.7, linewidth=2)
        self.ax_activity.set_title('Activity Over Time')
        self.ax_activity.set_xlabel('Time')
        self.ax_activity.set_ylabel('Firing Rate (Hz)')
        self.ax_activity.grid(True, alpha=0.3)
    
    def _update_performance_plot(self):
        """Update performance metrics plot"""
        # Get current metrics
        neuron_count = self.brain.getTotalNeuronCount()
        synapse_count = self.brain.getTotalSynapseCount()
        spike_count = self.brain.getTotalSpikeCount()
        
        # Create bar chart
        categories = ['Neurons', 'Synapses', 'Spikes']
        values = [neuron_count, synapse_count, spike_count]
        
        self.ax_performance.clear()
        bars = self.ax_performance.bar(categories, values, alpha=0.7, 
                                     color=['skyblue', 'lightgreen', 'salmon'])
        
        # Add value labels on bars
        for bar, value in zip(bars, values):
            height = bar.get_height()
            self.ax_performance.text(bar.get_x() + bar.get_width()/2., height,
                                    f'{value:,}', ha='center', va='bottom')
        
        self.ax_performance.set_title('Current Metrics')
        self.ax_performance.set_ylabel('Count')
        self.ax_performance.grid(True, alpha=0.3)
    
    def _update_metrics(self):
        """Update real-time metrics display"""
        # Collect metrics
        metrics_text = f"""
        Step: {self.current_step}
        Neurons: {self.brain.getTotalNeuronCount():,}
        Synapses: {self.brain.getTotalSynapseCount():,}
        Spikes: {self.brain.getTotalSpikeCount():,}
        Firing Rate: {self.brain.getAverageFiringRate():.2f} Hz
        Performance: {self._calculate_performance_score():.1f}/100
        """
        
        self.ax_metrics.clear()
        self.ax_metrics.text(0.1, 0.5, metrics_text, fontsize=10, 
                           verticalalignment='center', fontfamily='monospace')
        self.ax_metrics.set_xlim(0, 1)
        self.ax_metrics.set_ylim(0, 1)
        self.ax_metrics.axis('off')
    
    def _calculate_performance_score(self) -> float:
        """Calculate overall performance score"""
        # Simple scoring based on metrics
        neuron_count = self.brain.getTotalNeuronCount()
        synapse_count = self.brain.getTotalSynapseCount()
        spike_count = self.brain.getTotalSpikeCount()
        firing_rate = self.brain.getAverageFiringRate()
        
        # Normalize and combine scores
        neuron_score = min(100, neuron_count / 100)  # Score based on neuron count
        synapse_score = min(100, synapse_count / 500)  # Score based on synapse count
        spike_score = min(100, spike_count / 1000)  # Score based on spike count
        firing_score = min(100, firing_rate / 10)  # Score based on firing rate
        
        return (neuron_score + synapse_score + spike_score + firing_score) / 4
    
    def _toggle_simulation(self, event):
        """Toggle auto-update"""
        self.auto_update = not self.auto_update
        print(f"Simulation auto-update: {'ON' if self.auto_update else 'OFF'}")
    
    def _manual_step(self, event):
        """Manual step simulation"""
        self.current_step += 1
        self.brain.step(self.current_step)
        self._update_plots()
    
    def _reset_simulation(self, event):
        """Reset simulation"""
        self.current_step = 0
        self.brain.reset()
        self.brain.initialize()
        self.activity_history = []
        self._update_plots()


class VisualizationExporter:
    """
    Export visualization results and data.
    
    Provides capabilities for:
    - Export visualization data to various formats
    - Generate reports and summaries
    - Create animation files
    - Export for machine learning integration
    """
    
    @staticmethod
    def export_to_csv(data: Dict[str, Any], filepath: str):
        """
        Export visualization data to CSV format.
        
        Args:
            data: Visualization data
            filepath: Output file path
        """
        try:
            import pandas as pd
            
            # Convert data to DataFrame
            csv_data = []
            for key, value in data.items():
                if isinstance(value, list):
                    for i, item in enumerate(value):
                        csv_data.append({'metric': key, 'index': i, 'value': item})
                elif isinstance(value, dict):
                    for subkey, subvalue in value.items():
                        csv_data.append({'metric': key, 'submetric': subkey, 'value': subvalue})
            
            df = pd.DataFrame(csv_data)
            df.to_csv(filepath, index=False)
            print(f"Exported data to CSV: {filepath}")
            
        except Exception as e:
            print(f"Error exporting to CSV: {e}")
    
    @staticmethod
    def export_to_json(data: Dict[str, Any], filepath: str):
        """
        Export visualization data to JSON format.
        
        Args:
            data: Visualization data
            filepath: Output file path
        """
        try:
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2, default=str)
            print(f"Exported data to JSON: {filepath}")
            
        except Exception as e:
            print(f"Error exporting to JSON: {e}")
    
    @staticmethod
    def create_animation(data: Dict[str, Any], filepath: str, fps: int = 10):
        """
        Create animation from time-series visualization data.
        
        Args:
            data: Animation data
            filepath: Output file path
            fps: Frames per second
        """
        try:
            # Create simple animation
            fig, ax = plt.subplots(figsize=(10, 6))
            
            def animate(frame):
                ax.clear()
                # Update plot based on frame data
                if 'time_series' in data:
                    x = data['time_series']['x'][:frame+1]
                    y = data['time_series']['y'][:frame+1]
                    ax.plot(x, y, 'b-', linewidth=2)
                    ax.set_title(f'Neural Activity: Step {frame}')
                    ax.set_xlabel('Time')
                    ax.set_ylabel('Activity')
                    ax.grid(True, alpha=0.3)
            
            anim = animation.FuncAnimation(fig, animate, frames=len(data['time_series']['x']),
                                         interval=1000/fps, repeat=False)
            
            # Save animation
            anim.save(filepath, writer='pillow', fps=fps)
            print(f"Created animation: {filepath}")
            
        except Exception as e:
            print(f"Error creating animation: {e}")
    
    @staticmethod
    def generate_html_report(data: Dict[str, Any], filepath: str):
        """
        Generate HTML report with visualization and analysis.
        
        Args:
            data: Report data
            filepath: Output file path
        """
        html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Visualization Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; }}
        .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
        .metric {{ background-color: #e8f4f8; padding: 15px; margin: 10px 0; border-radius: 3px; }}
        .chart {{ border: 1px solid #ddd; padding: 10px; margin: 10px 0; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Visualization Report</h1>
        <p>Generated on {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
    </div>
    
    <h2>Summary</h2>
    <div class="metric">
        <h3>Performance Overview</h3>
        <p>Overall Score: {data.get('overall_score', 0):.1f}/100</p>
        <p>Grade: {data.get('grade', 'N/A')}</p>
        <p>Simulations: {data.get('simulation_count', 0)}</p>
    </div>
    
    <h2>Key Metrics</h2>
    <div class="metric">
        <h3>Neural Statistics</h3>
        <p>Neurons: {data.get('neural_stats', {}).get('neuron_count', 0):,}</p>
        <p>Synapses: {data.get('neural_stats', {}).get('synapse_count', 0):,}</p>
        <p>Spikes: {data.get('neural_stats', {}).get('spike_count', 0):,}</p>
        <p>Firing Rate: {data.get('neural_stats', {}).get('firing_rate', 0):.2f} Hz</p>
    </div>
    
    <h2>Performance Analysis</h2>
    <div class="chart">
        <h3>Performance Distribution</h3>
        <img src="data:image/png;base64,{data.get('performance_chart', '')}" 
             alt="Performance Chart" style="max-width: 100%; height: auto;">
    </div>
    
    <h2>Recommendations</h2>
    <div class="metric">
        <h3>Optimization Suggestions</h3>
        <ul>
"""
        
        # Add recommendations
        recommendations = data.get('recommendations', [])
        for rec in recommendations:
            html_content += f"<li><strong>{rec.get('category', '')}:</strong> {rec.get('issue', '')}</li>\n"
        
        html_content += """
        </ul>
    </div>
    
</body>
</html>
        """
        
        try:
            with open(filepath, 'w') as f:
                f.write(html_content)
            print(f"Generated HTML report: {filepath}")
            
        except Exception as e:
            print(f"Error generating HTML report: {e}")


# Factory functions for creating visualizations
def create_neural_visualizer(width: int = 12, height: int = 8) -> NeuralVisualization:
    """Create a new neural visualizer instance"""
    return NeuralVisualization(width, height)


def create_interactive_dashboard(brain: pynlm.Brain, 
                                world: Optional[pynlm.SimpleWorld] = None,
                                agent: Optional[pynlm.AgentBrain] = None) -> InteractiveDashboard:
    """Create an interactive dashboard for real-time visualization"""
    visualizer = create_neural_visualizer()
    return InteractiveDashboard(visualizer, brain, world, agent)


def export_visualization_data(data: Dict[str, Any], output_format: str = 'json',
                             filepath: str = 'visualization_output'):
    """
    Export visualization data to specified format.
    
    Args:
        data: Visualization data
        output_format: Output format ('json', 'csv', 'html')
        filepath: Output file path (without extension)
    """
    if output_format == 'json':
        VisualizationExporter.export_to_json(data, f"{filepath}.json")
    elif output_format == 'csv':
        VisualizationExporter.export_to_csv(data, f"{filepath}.csv")
    elif output_format == 'html':
        VisualizationExporter.generate_html_report(data, f"{filepath}.html")
    else:
        print(f"Unsupported format: {output_format}")


def run_visualization_example():
    """Example demonstrating visualization capabilities"""
    print("=== NLM Visualization System Example ===\\n")
    
    # Create example NLM system
    print("1. Creating example NLM brain...")
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    print("2. Creating world and agent...")
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Create visualizer
    print("3. Creating visualizer...")
    visualizer = create_neural_visualizer()
    
    # Run simulation while collecting data
    print("4. Running simulation with visualization...")
    simulation_data = {
        'time_series': {'x': [], 'y': []},
        'neural_stats': {},
        'performance_metrics': {},
    }
    
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        agent.processSensoryInput(world.getSensoryPercept())
        
        # Run brain
        brain.step(step)
        
        # Record data
        simulation_data['time_series']['x'].append(step)
        simulation_data['time_series']['y'].append(brain.getAverageFiringRate())
        
        if step % 20 == 0:
            print(f"  Step {step}: Firing rate = {brain.getAverageFiringRate():.2f} Hz")
    
    # Get neural statistics
    simulation_data['neural_stats'] = {
        'neuron_count': brain.getTotalNeuronCount(),
        'synapse_count': brain.getTotalSynapseCount(),
        'spike_count': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
    }
    
    # Create plots
    print("5. Creating visualizations...")
    
    # Network visualization
    network_fig = visualizer.plot_neural_network(brain)
    network_fig.savefig('neural_network.png', dpi=150, bbox_inches='tight')
    
    # Activity over time
    activity_fig = visualizer.plot_activity_over_time(brain, steps=50)
    activity_fig.savefig('activity_over_time.png', dpi=150, bbox_inches='tight')
    
    # Performance metrics
    performance_fig = visualizer.plot_performance_metrics(simulation_data)
    performance_fig.savefig('performance_metrics.png', dpi=150, bbox_inches='tight')
    
    # Export visualization data
    print("6. Exporting visualization data...")
    export_visualization_data(simulation_data, 'simulation_results')
    
    print("\\nVisualization example completed!")
    print("Generated files:")
    print("  - neural_network.png")
    print("  - activity_over_time.png")
    print("  - performance_metrics.png")
    print("  - simulation_results.json")
    
    # Display example dashboard
    print("\\n7. Creating interactive dashboard...")
    dashboard = create_interactive_dashboard(brain, world, agent)
    
    print("\nNote: Interactive dashboard is ready for real-time visualization")
    print("Dashboard features:")
    print("  - Real-time neural activity monitoring")
    print("  - Network structure visualization")
    print("  - Performance metrics tracking")
    print("  - Simulation controls")
if __name__ == "__main__":
    run_visualization_example()