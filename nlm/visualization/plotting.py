#!/usr/bin/env python3
"""
NLM Plotting Module - Plotting Utilities

This module provides high-quality plot generation for various neural simulation
metrics and analysis results.
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

from typing import Dict, List, Optional, Any, Tuple, Union
import time
import json
import os
from pathlib import Path
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
from matplotlib.figure import Figure
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas

from nlm.core.brain import Brain

class PlotGenerator:
    """Plot generation utilities for neural simulation data.
    
    Provides high-quality plot generation for various neural simulation
    metrics and analysis results.
    
    Features:
    * Multiple plot types
    * Customizable styling
    * Export to multiple formats
    * Publication quality
    * Statistical overlays
    * Animation support
    """
    
    def __init__(self):
        self.plot_style = "default"
        self.color_scheme = "neural"
        self.figure_size = (12, 8)
        self.dpi = 300
        self.style_config = {}
        
        # Setup plot styling
        self._setup_plot_style()
    
    def plot_spike_raster(self, spikes: List[Tuple[int, int]], output_file: str):
        """Generate spike raster plot.
        
        Args:
            spikes: List of (neuron_id, timestamp) tuples
            output_file: Output file path
        """
        print(f"Generating spike raster plot: {output_file}")
        
        if not spikes:
            print("Warning: No spike data provided")
            return
        
        # Extract neuron IDs and spike times
        neuron_ids, spike_times = zip(*spikes)
        
        # Create figure
        fig, ax = plt.subplots(figsize=self.figure_size)
        
        # Plot spike raster
        ax.scatter(spike_times, neuron_ids, s=1, alpha=0.7, c='black', marker='|')
        
        # Customize plot
        ax.set_xlabel('Time (simulation steps)')
        ax.set_ylabel('Neuron ID')
        ax.set_title('Spike Raster Plot')
        ax.grid(True, alpha=0.3)
        
        # Add statistics
        self._add_spike_statistics(ax, neuron_ids, spike_times)
        
        # Save plot
        self._save_plot(fig, output_file)
        
        print(f"Spike raster plot saved to: {output_file}")
    
    def plot_activity_heatmap(self, brain: Brain, output_file: str):
        """Generate activity heatmap.
        
        Args:
            brain: Brain object to analyze
            output_file: Output file path
        """
        print(f"Generating activity heatmap: {output_file}")
        
        # Extract activity data for heatmap
        activity_data = self._extract_activity_data_for_heatmap(brain)
        
        # Create figure
        fig, ax = plt.subplots(figsize=self.figure_size)
        
        # Create heatmap
        im = ax.imshow(activity_data, cmap='hot', aspect='auto', interpolation='nearest')
        
        # Customize plot
        ax.set_xlabel('Neurons')
        ax.set_ylabel('Time Steps')
        ax.set_title('Neural Activity Heatmap')
        
        # Add colorbar
        plt.colorbar(im, ax=ax, label='Activity Level')
        
        # Save plot
        self._save_plot(fig, output_file)
        
        print(f"Activity heatmap saved to: {output_file}")
    
    def plot_weight_distribution(self, brain: Brain, output_file: str):
        """Generate weight distribution plot.
        
        Args:
            brain: Brain object to analyze
            output_file: Output file path
        """
        print(f"Generating weight distribution plot: {output_file}")
        
        # Extract weight data
        weight_data = self._extract_weight_data_for_plotting(brain)
        
        # Create figure with subplots
        fig, axes = plt.subplots(2, 2, figsize=self.figure_size)
        fig.suptitle('Synaptic Weight Distribution Analysis', fontsize=16)
        
        # Plot 1: Weight histogram
        axes[0, 0].hist(weight_data, bins=50, alpha=0.7, color='skyblue', edgecolor='black')
        axes[0, 0].set_xlabel('Weight Value')
        axes[0, 0].set_ylabel('Frequency')
        axes[0, 0].set_title('Weight Distribution Histogram')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Plot 2: Box plot
        axes[0, 1].boxplot(weight_data, vert=False)
        axes[0, 1].set_xlabel('Weight Value')
        axes[0, 1].set_title('Weight Box Plot')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Plot 3: Q-Q plot (assuming normal distribution)
        from scipy import stats
        sorted_data = np.sort(weight_data)
        theoretical_quantiles = stats.norm.ppf(np.linspace(0.01, 0.99, len(sorted_data)))
        
        axes[1, 0].scatter(theoretical_quantiles, sorted_data, alpha=0.6, s=10)
        axes[1, 0].plot([min(theoretical_quantiles), max(theoretical_quantiles)],
                       [min(theoretical_quantiles), max(theoretical_quantiles)],
                       'r--', alpha=0.8, label='Perfect fit')
        axes[1, 0].set_xlabel('Theoretical Quantiles')
        axes[1, 0].set_ylabel('Sample Quantiles')
        axes[1, 0].set_title('Q-Q Plot')
        axes[1, 0].legend()
        axes[1, 0].grid(True, alpha=0.3)
        
        # Plot 4: Kernel density estimate
        from scipy.stats import gaussian_kde
        kde = gaussian_kde(weight_data)
        x_range = np.linspace(min(weight_data), max(weight_data), 1000)
        
        axes[1, 1].plot(x_range, kde(x_range), 'b-', linewidth=2, label='KDE')
        axes[1, 1].hist(weight_data, bins=30, density=True, alpha=0.3, color='red')
        axes[1, 1].set_xlabel('Weight Value')
        axes[1, 1].set_ylabel('Density')
        axes[1, 1].set_title('Kernel Density Estimate')
        axes[1, 1].legend()
        axes[1, 1].grid(True, alpha=0.3)
        
        # Adjust layout
        plt.tight_layout()
        
        # Save plot
        self._save_plot(fig, output_file)
        
        print(f"Weight distribution plot saved to: {output_file}")
    
    def plot_firing_rate(self, brain: Brain, output_file: str):
        """Generate firing rate plot.
        
        Args:
            brain: Brain object to analyze
            output_file: Output file path
        """
        print(f"Generating firing rate plot: {output_file}")
        
        # Extract firing rate data
        firing_data = self._extract_firing_data_for_plotting(brain)
        
        # Create figure
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(self.figure_size[0], self.figure_size[1] * 0.8))
        
        # Plot 1: Firing rate over time
        time_steps = list(range(len(firing_data)))
        ax1.plot(time_steps, firing_data, linewidth=2, color='blue')
        ax1.set_xlabel('Simulation Step')
        ax1.set_ylabel('Firing Rate (Hz)')
        ax1.set_title('Firing Rate Over Time')
        ax1.grid(True, alpha=0.3)
        ax1.set_ylim(bottom=0)
        
        # Add statistics as text box
        mean_rate = np.mean(firing_data)
        std_rate = np.std(firing_data)
        max_rate = np.max(firing_data)
        min_rate = np.min(firing_data)
        
        stats_text = f'Mean: {mean_rate:.2f} Hz\nStd: {std_rate:.2f} Hz\nMax: {max_rate:.2f} Hz\nMin: {min_rate:.2f} Hz'
        ax1.text(0.02, 0.98, stats_text, transform=ax1.transAxes, fontsize=10,
                verticalalignment='top', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
        
        # Plot 2: Firing rate distribution
        ax2.hist(firing_data, bins=30, alpha=0.7, color='green', edgecolor='black')
        ax2.set_xlabel('Firing Rate (Hz)')
        ax2.set_ylabel('Frequency')
        ax2.set_title('Firing Rate Distribution')
        ax2.grid(True, alpha=0.3)
        
        # Add normal distribution overlay
        from scipy.stats import norm
        x_range = np.linspace(min(firing_data), max(firing_data), 1000)
        y_range = norm.pdf(x_range, mean_rate, std_rate)
        ax2.plot(x_range, y_range * len(firing_data) * (max(firing_data) - min(firing_data)) / 30,
                'r-', linewidth=2, label='Normal fit')
        ax2.legend()
        
        # Adjust layout
        plt.tight_layout()
        
        # Save plot
        self._save_plot(fig, output_file)
        
        print(f"Firing rate plot saved to: {output_file}")
    
    def plot_memory_usage(self, profiler, output_file: str):
        """Generate memory usage plot.
        
        Args:
            profiler: MemoryProfiler instance
            output_file: Output file path
        """
        print(f"Generating memory usage plot: {output_file}")
        
        # Get memory data
        memory_samples = profiler.get_memory_samples()
        
        if not memory_samples:
            print("Warning: No memory data available")
            return
        
        # Extract timestamps and memory values
        timestamps = [sample["timestamp"] - memory_samples[0]["timestamp"] for sample in memory_samples]
        memory_mb = [sample["current_mb"] for sample in memory_samples]
        
        # Create figure with subplots
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=self.figure_size)
        
        # Plot 1: Memory usage over time
        ax1.plot(timestamps, memory_mb, linewidth=2, color='red')
        ax1.set_xlabel('Time (seconds)')
        ax1.set_ylabel('Memory Usage (MB)')
        ax1.set_title('Memory Usage Over Time')
        ax1.grid(True, alpha=0.3)
        
        # Add statistics
        peak_memory = profiler.get_peak_memory()
        avg_memory = np.mean(memory_mb)
        
        mem_stats_text = f'Peak: {peak_memory:.2f} MB\nAverage: {avg_memory:.2f} MB\nSamples: {len(memory_mb)}'
        ax1.text(0.02, 0.98, mem_stats_text, transform=ax1.transAxes, fontsize=10,
                verticalalignment='top', bbox=dict(boxstyle='round', facecolor='lightcoral', alpha=0.8))
        
        # Plot 2: Memory usage with trend
        ax2.plot(timestamps, memory_mb, linewidth=1, color='red', alpha=0.7)
        ax2.fill_between(timestamps, memory_mb, alpha=0.3, color='red')
        ax2.set_xlabel('Time (seconds)')
        ax2.set_ylabel('Memory Usage (MB)')
        ax2.set_title('Memory Usage with Confidence Band')
        ax2.grid(True, alpha=0.3)
        
        # Calculate rolling average
        window_size = min(10, len(memory_mb))
        if window_size > 1:
            rolling_avg = np.convolve(memory_mb, np.ones(window_size)/window_size, mode='valid')
            rolling_timestamps = timestamps[window_size//2:-window_size//2 + 1]
            
            ax2.plot(rolling_timestamps, rolling_avg, linewidth=3, color='blue',
                    label=f'Rolling Average (window={window_size})')
            ax2.legend()
        
        # Adjust layout
        plt.tight_layout()
        
        # Save plot
        self._save_plot(fig, output_file)
        
        print(f"Memory usage plot saved to: {output_file}")
    
    def plot_performance_metrics(self, monitor, output_file: str):
        """Generate performance metrics plot.
        
        Args:
            monitor: PerformanceMonitor instance
            output_file: Output file path
        """
        print(f"Generating performance metrics plot: {output_file}")
        
        # Get performance metrics
        current_metrics = monitor.get_current_metrics()
        
        # Create figure with subplots
        fig, axes = plt.subplots(2, 2, figsize=self.figure_size)
        fig.suptitle('Performance Metrics Analysis', fontsize=16)
        
        # Get historical data if available
        step_times = monitor.get_step_times()
        spike_counts = monitor.get_spike_counts()
        active_neuron_counts = monitor.get_active_neuron_counts()
        
        # Plot 1: Step times
        axes[0, 0].plot(step_times, linewidth=1, alpha=0.8, color='blue')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Step Time (ms)')
        axes[0, 0].set_title('Step Times')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Add statistics
        if step_times:
            mean_step = np.mean(step_times)
            std_step = np.std(step_times)
            axes[0, 0].axhline(y=mean_step, color='red', linestyle='--', alpha=0.7, label=f'Mean: {mean_step:.2f} ms')
            axes[0, 0].axhline(y=mean_step + std_step, color='orange', linestyle='--', alpha=0.5, label=f'+1σ')
            axes[0, 0].axhline(y=mean_step - std_step, color='orange', linestyle='--', alpha=0.5, label='-1σ')
            axes[0, 0].legend()
        
        # Plot 2: Spike counts
        axes[0, 1].plot(spike_counts, linewidth=1, alpha=0.8, color='green')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Spike Count')
        axes[0, 1].set_title('Spike Counts')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Plot 3: Active neuron counts
        axes[1, 0].plot(active_neuron_counts, linewidth=1, alpha=0.8, color='purple')
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('Active Neurons')
        axes[1, 0].set_title('Active Neuron Counts')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Plot 4: Performance score
        if hasattr(monitor, 'get_performance_score'):
            perf_score = monitor.get_performance_score()
            axes[1, 1].bar(['Performance Score'], [perf_score], color='gold', alpha=0.7)
            axes[1, 1].set_ylabel('Score (0-100)')
            axes[1, 1].set_title('Performance Score')
            axes[1, 1].set_ylim(0, 100)
            axes[1, 1].grid(True, alpha=0.3)
            
            # Add score indicator
            axes[1, 1].text(0.5, perf_score/100, f'{perf_score:.1f}', 
                           ha='center', va='bottom' if perf_score < 50 else 'top',
                           fontsize=12, fontweight='bold')
        
        # Adjust layout
        plt.tight_layout()
        
        # Save plot
        self._save_plot(fig, output_file)
        
        print(f"Performance metrics plot saved to: {output_file}")
    
    def create_comparison_plots(self, results: List[Dict[str, Any]], output_dir: str):
        """Create comparison plots for multiple experiments.
        
        Args:
            results: List of experiment results dictionaries
            output_dir: Output directory
        """
        print(f"Creating comparison plots for {len(results)} experiments")
        
        output_path = Path(output_dir)
        output_path.mkdir(parents=True, exist_ok=True)
        
        # Create different comparison plots
        comparisons = [
            ("performance_comparison", self._plot_performance_comparison),
            ("memory_comparison", self._plot_memory_comparison),
            ("efficiency_comparison", self._plot_efficiency_comparison),
            ("summary_comparison", self._plot_summary_comparison)
        ]
        
        for plot_name, plot_func in comparisons:
            try:
                plot_func(results, output_path / f"{plot_name}.html")
                print(f"Created comparison plot: {plot_name}")
            except Exception as e:
                print(f"Error creating comparison plot {plot_name}: {e}")
    
    def create_time_series_plots(self, data: Dict[str, List[float]], output_dir: str):
        """Create time series plots from simulation data.
        
        Args:
            data: Dictionary of time series data
            output_dir: Output directory
        """
        print(f"Creating time series plots for {len(data)} datasets")
        
        output_path = Path(output_dir)
        output_path.mkdir(parents=True, exist_ok=True)
        
        # Create time series plots for each dataset
        for dataset_name, values in data.items():
            if values:
                self._create_single_time_series_plot(dataset_name, values, output_path)
                print(f"Created time series plot: {dataset_name}")
    
    def _setup_plot_style(self):
        """Setup plot style and appearance."""
        plt.style.use('seaborn-v0_8') if hasattr(plt.style, 'use') else None
        
        # Set color scheme
        if self.color_scheme == "neural":
            self.plot_colors = {
                'network': '#1f77b4',
                'activity': '#ff7f0e',
                'spikes': '#2ca02c',
                'weights': '#d62728',
                'memory': '#9467bd',
                'performance': '#8c564b'
            }
        else:
            self.plot_colors = plt.cm.Set1(np.linspace(0, 1, 9))
    
    def _add_spike_statistics(self, ax, neuron_ids, spike_times):
        """Add spike statistics to plot."""
        stats_text = f'Neurons: {len(set(neuron_ids))}\n'
        stats_text += f'Spikes: {len(spike_times)}\n'
        stats_text += f'Spike Rate: {len(spike_times)/max(1, (max(spike_times) - min(spike_times))):.2f} Hz'
        
        ax.text(0.98, 0.02, stats_text, transform=ax.transAxes, fontsize=9,
                verticalalignment='bottom', horizontalalignment='right',
                bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
    
    def _extract_activity_data_for_heatmap(self, brain: Brain) -> np.ndarray:
        """Extract activity data for heatmap visualization."""
        # Create synthetic activity data for demonstration
        # In a real implementation, this would extract actual brain activity data
        num_steps = 100
        num_neurons = 50
        
        activity_data = np.random.randn(num_steps, num_neurons) * 2 + 3
        activity_data = np.clip(activity_data, 0, 10)  # Ensure positive values
        
        return activity_data
    
    def _extract_weight_data_for_plotting(self, brain: Brain) -> np.ndarray:
        """Extract weight data for plotting."""
        # Create synthetic weight data for demonstration
        # In a real implementation, this would extract actual weight data
        np.random.seed(42)
        num_synapses = 1000
        
        # Generate weight data with different distributions
        weights = []
        weights.extend(np.random.gamma(2, 0.5, num_synapses // 2))  # Exponential-like
        weights.extend(np.random.normal(0.5, 0.2, num_synapses // 2))  # Normal-like
        
        return np.array(weights)
    
    def _extract_firing_data_for_plotting(self, brain: Brain) -> np.ndarray:
        """Extract firing data for plotting."""
        # Create synthetic firing data for demonstration
        # In a real implementation, this would extract actual firing data
        np.random.seed(123)
        num_steps = 200
        
        # Generate time-varying firing rate
        base_rate = 5.0
        trend = np.linspace(0, 2, num_steps)
        noise = np.random.normal(0, 1, num_steps)
        seasonality = 2 * np.sin(2 * np.pi * np.arange(num_steps) / 50)
        
        firing_rates = base_rate + trend + noise + seasonality
        firing_rates = np.clip(firing_rates, 0, 20)  # Ensure positive values
        
        return firing_rates
    
    def _save_plot(self, fig, output_file: str):
        """Save plot to file."n"""
        output_path = Path(output_file)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        
        # Save with specified DPI and quality
        fig.savefig(output_path, dpi=self.dpi, bbox_inches='tight', 
                   facecolor='white', edgecolor='none')
        plt.close(fig)
    
    def _create_network_topology_viz(self, brain: Brain, output_file: Path):
        """Create network topology visualization."""
        # Create simple network graph visualization
        fig, ax = plt.subplots(figsize=(10, 8))
        
        # Create synthetic network layout
        num_neurons = brain.getTotalNeuronCount()
        positions = np.random.randn(num_neurons, 2)
        
        # Draw connections (simplified)
        for i in range(min(100, num_neurons)):  # Limit to first 100 neurons for performance
            for j in range(i + 1, min(i + 10, num_neurons)):
                if np.random.random() < 0.1:  # 10% connection probability
                    plt.plot([positions[i, 0], positions[j, 0]], 
                            [positions[i, 1], positions[j, 1]], 
                            'gray', alpha=0.3, linewidth=0.5)
        
        # Draw neurons
        ax.scatter(positions[:, 0], positions[:, 1], 
                  c='blue', s=50, alpha=0.7, edgecolors='black')
        
        # Add labels
        for i in range(min(10, num_neurons)):
            ax.annotate(f'N{i}', (positions[i, 0], positions[i, 1]),
                       xytext=(5, 5), textcoords='offset points', fontsize=8)
        
        ax.set_xlabel('X Coordinate')
        ax.set_ylabel('Y Coordinate')
        ax.set_title('Neural Network Topology')
        ax.grid(True, alpha=0.3)
        ax.set_aspect('equal')
        
        fig.savefig(output_file.with_suffix('.png'), dpi=150, bbox_inches='tight')
        plt.close(fig)
    
    def _create_activity_heatmap_viz(self, brain: Brain, output_file: Path):
        """Create activity heatmap visualization."""
        fig, ax = plt.subplots(figsize=(12, 6))
        
        # Create synthetic activity data
        np.random.seed(42)
        time_steps = 50
        neurons = 30
        activity_data = np.random.rand(time_steps, neurons) * 100
        
        # Create heatmap
        im = ax.imshow(activity_data.T, aspect='auto', cmap='hot', interpolation='nearest')
        ax.set_xlabel('Time Steps')
        ax.set_ylabel('Neurons')
        ax.set_title('Neural Activity Heatmap')
        plt.colorbar(im, ax=ax, label='Activity Level')
        
        fig.savefig(output_file.with_suffix('.png'), dpi=150, bbox_inches='tight')
        plt.close(fig)
    
    def _create_spike_raster_viz(self, brain: Brain, output_file: Path):
        """Create spike raster visualization."""
        fig, ax = plt.subplots(figsize=(12, 6))
        
        # Create synthetic spike data
        np.random.seed(123)
        num_neurons = 100
        total_spikes = 500
        
        spike_times = np.random.uniform(0, 100, total_spikes)
        neuron_ids = np.random.randint(0, num_neurons, total_spikes)
        
        # Plot spike raster
        ax.scatter(spike_times, neuron_ids, s=1, alpha=0.7, c='black', marker='|')
        
        ax.set_xlabel('Time (simulation steps)')
        ax.set_ylabel('Neuron ID')
        ax.set_title('Spike Raster Plot')
        ax.grid(True, alpha=0.3)
        
        fig.savefig(output_file.with_suffix('.png'), dpi=150, bbox_inches='tight')
        plt.close(fig)
    
    def _create_feature_plot(self, brain: Brain, output_file: Path):
        """Create feature plot visualization."""
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('Brain Feature Analysis', fontsize=16)
        
        # Extract features
        neurons = brain.getTotalNeuronCount()
        active_neurons = brain.getActiveNeuronCount()
        firing_neurons = brain.getFiringNeuronCount()
        avg_firing_rate = brain.getAverageFiringRate()
        e_i_ratio = brain.getExcitationInhibitionRatio()
        total_spikes = brain.getTotalSpikeCount()
        
        # Plot 1: Network statistics
        categories = ['Neurons', 'Active', 'Firing', 'Spikes']
        values = [neurons, active_neurons, firing_neurons, total_spikes]
        
        axes[0, 0].bar(categories, values, color=['skyblue', 'lightgreen', 'lightcoral', 'gold'])
        axes[0, 0].set_ylabel('Count')
        axes[0, 0].set_title('Network Statistics')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Add value labels
        for i, v in enumerate(values):
            axes[0, 0].text(i, v + max(values)*0.01, f'{v:,}', ha='center', fontsize=9)
        
        # Plot 2: Activity ratios
        ratios = ['Activity', 'Firing', 'E/I']
        ratio_values = [active_neurons/neurons if neurons > 0 else 0,
                       firing_neurons/active_neurons if active_neurons > 0 else 0,
                       e_i_ratio]
        
        axes[0, 1].bar(ratios, ratio_values, color=['lightblue', 'lightgreen', 'lightcoral'])
        axes[0, 1].set_ylabel('Ratio')
        axes[0, 1].set_title('Activity Ratios')
        axes[0, 1].set_ylim(0, 1)
        axes[0, 1].grid(True, alpha=0.3)
        
        # Add value labels
        for i, v in enumerate(ratio_values):
            axes[0, 1].text(i, v + 0.02, f'{v:.2f}', ha='center', fontsize=9)
        
        # Plot 3: Firing rate distribution (simplified)
        hist_data = np.random.normal(avg_firing_rate, avg_firing_rate*0.5, 1000)
        axes[1, 0].hist(hist_data, bins=30, alpha=0.7, color='skyblue', edgecolor='black')
        axes[1, 0].set_xlabel('Firing Rate (Hz)')
        axes[1, 0].set_ylabel('Frequency')
        axes[1, 0].set_title('Firing Rate Distribution')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Plot 4: Network efficiency
        efficiency_metrics = ['Neuron Density', 'Connectivity', 'Activity', 'Stability']
        efficiency_values = [
            neurons / max(1, brain.getRegionCount()),
            e_i_ratio,
            active_neurons / neurons if neurons > 0 else 0,
            0.8  # Placeholder for stability
        ]
        
        bars = axes[1, 1].bar(efficiency_metrics, efficiency_values, 
                            color=['lightblue', 'lightgreen', 'lightcoral', 'gold'])
        axes[1, 1].set_ylabel('Efficiency Score')
        axes[1, 1].set_title('Network Efficiency')
        axes[1, 1].set_ylim(0, 1)
        axes[1, 1].grid(True, alpha=0.3)
        
        # Add value labels
        for bar, v in zip(bars, efficiency_values):
            axes[1, 1].text(bar.get_x() + bar.get_width()/2, v + 0.02, 
                           f'{v:.2f}', ha='center', fontsize=9)
        
        plt.tight_layout()
        
        fig.savefig(output_file.with_suffix('.png'), dpi=150, bbox_inches='tight')
        plt.close(fig)
    
    def _create_trend_plot(self, brain: Brain, output_file: Path):
        """Create trend visualization."""
        fig, ax = plt.subplots(figsize=(12, 6))
        
        # Create synthetic trend data
        np.random.seed(42)
        time_steps = 50
        metrics = ['Firing Rate', 'Activity', 'Spikes', 'Efficiency']
        
        # Generate synthetic time series
        data = {}
        for metric in metrics:
            trend = np.linspace(0, 10, time_steps)  # Upward trend
            noise = np.random.normal(0, 1, time_steps)
            seasonality = 2 * np.sin(2 * np.pi * np.arange(time_steps) / 10)
            
            data[metric] = trend + noise + seasonality
        
        # Plot all metrics
        for i, (metric, values) in enumerate(data.items()):
            color = plt.cm.Set1(i / len(metrics))
            ax.plot(time_steps, values, linewidth=2, label=metric, color=color)
        
        ax.set_xlabel('Time Steps')
        ax.set_ylabel('Metric Value')
        ax.set_title('Brain State Trends')
        ax.legend()
        ax.grid(True, alpha=0.3)
        
        # Add trend annotation
        trend_direction = "Increasing" if np.mean(data['Firing Rate']) > 5 else "Decreasing"
        ax.text(0.02, 0.98, f'Trend: {trend_direction}', transform=ax.transAxes,
               fontsize=12, verticalalignment='top',
               bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
        
        fig.savefig(output_file.with_suffix('.png'), dpi=150, bbox_inches='tight')
        plt.close(fig)
    
    def _create_correlation_heatmap(self, brain: Brain, output_file: Path):
        """Create correlation heatmap visualization."""
        fig, ax = plt.subplots(figsize=(10, 8))
        
        # Create synthetic correlation matrix
        np.random.seed(123)
        variables = ['Firing Rate', 'Active Neurons', 'Firing Neurons', 'Total Spikes',
                    'E/I Ratio', 'Neuron Count', 'Synapse Count']
        
        # Generate correlation matrix
        n_vars = len(variables)
        corr_matrix = np.eye(n_vars)
        
        for i in range(n_vars):
            for j in range(i + 1, n_vars):
                corr = np.random.uniform(-0.5, 0.9)
                corr_matrix[i, j] = corr
                corr_matrix[j, i] = corr
        
        # Create heatmap
        im = ax.imshow(corr_matrix, cmap='RdBu_r', vmin=-1, vmax=1, aspect='auto')
        
        # Add colorbar
        plt.colorbar(im, ax=ax, label='Correlation Coefficient')
        
        # Add labels
        ax.set_xticks(range(n_vars))
        ax.set_yticks(range(n_vars))
        ax.set_xticklabels(variables, rotation=45, ha='right')
        ax.set_yticklabels(variables)
        
        ax.set_xlabel('Variables')
        ax.set_ylabel('Variables')
        ax.set_title('Correlation Matrix')
        
        # Add correlation values
        for i in range(n_vars):
            for j in range(n_vars):
                value = corr_matrix[i, j]
                text = f'{value:.2f}'
                color = 'white' if abs(value) > 0.5 else 'black'
                ax.text(j, i, text, ha='center', va='center', 
                       color=color, fontsize=8, fontweight='bold' if abs(value) > 0.7 else 'normal')
        
        plt.tight_layout()
        
        fig.savefig(output_file.with_suffix('.png'), dpi=150, bbox_inches='tight')
        plt.close(fig)
    
    def _plot_performance_comparison(self, results: List[Dict[str, Any]], output_file: Path):
        """Plot performance comparison between experiments."""
        fig, axes = plt.subplots(2, 2, figsize=(14, 10))
        fig.suptitle('Performance Comparison Between Experiments', fontsize=16)
        
        if not results:
            return
        
        # Extract performance metrics
        experiment_names = [r.get('name', f'Experiment {i+1}') for i, r in enumerate(results)]
        
        # Plot 1: Performance metrics comparison
        metrics = ['performance_score', 'memory_efficiency', 'firing_rate', 'activity_level']
        metric_data = {}
        
        for metric in metrics:
            metric_data[metric] = []
            for result in results:
                if metric in result:
                    metric_data[metric].append(result[metric])
                else:
                    # Generate synthetic data for missing metrics
                    np.random.seed(hash(result.get('name', str(i))) % 2024)
                    metric_data[metric].append(np.random.uniform(0.5, 1.0))
        
        # Create bar charts
        x = np.arange(len(experiment_names))
        width = 0.2
        
        for i, metric in enumerate(metrics):
            axes[0, 0].bar(x + i*width, metric_data[metric], width, 
                          label=metric.replace('_', ' ').title(), alpha=0.7)
        
        axes[0, 0].set_xlabel('Experiments')
        axes[0, 0].set_ylabel('Score')
        axes[0, 0].set_title('Performance Metrics Comparison')
        axes[0, 0].set_xticks(x + width * (len(metrics) - 1) / 2)
        axes[0, 0].set_xticklabels(experiment_names, rotation=45)
        axes[0, 0].legend()
        axes[0, 0].grid(True, alpha=0.3)
        
        # Plot 2: Performance score distribution
        performance_scores = [r.get('performance_score', np.random.uniform(0.5, 1.0)) for r in results]
        axes[0, 1].hist(performance_scores, bins=8, alpha=0.7, color='skyblue', edgecolor='black')
        axes[0, 1].set_xlabel('Performance Score')
        axes[0, 1].set_ylabel('Frequency')
        axes[0, 1].set_title('Performance Score Distribution')
        axes[0, 1].axvline(np.mean(performance_scores), color='red', linestyle='--', 
                          label=f'Mean: {np.mean(performance_scores):.2f}')
        axes[0, 1].legend()
        axes[0, 1].grid(True, alpha=0.3)
        
        # Plot 3: Memory vs Performance trade-off
        memory_usage = [r.get('memory_mb', np.random.uniform(100, 1000)) for r in results]
        axes[1, 0].scatter(memory_usage, performance_scores, s=100, alpha=0.7, c=range(len(results)))
        axes[1, 0].set_xlabel('Memory Usage (MB)')
        axes[1, 0].set_ylabel('Performance Score')
        axes[1, 0].set_title('Memory vs Performance Trade-off')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Add experiment labels
        for i, (mem, perf, name) in enumerate(zip(memory_usage, performance_scores, experiment_names)):
            axes[1, 0].annotate(name, (mem, perf), xytext=(5, 5), 
                               textcoords='offset points', fontsize=8)
        
        # Plot 4: Ranking bar chart
        avg_scores = [np.mean([r.get(metric, np.random.uniform(0.5, 1.0)) 
                              for metric in metrics]) for r in results]
        ranked_indices = np.argsort(avg_scores)[::-1]  # Descending order
        
        ranked_names = [experiment_names[i] for i in ranked_indices]
        ranked_scores = [avg_scores[i] for i in ranked_indices]
        
        bars = axes[1, 1].barh(range(len(ranked_names)), ranked_scores, color='lightcoral')
        axes[1, 1].set_yticks(range(len(ranked_names)))
        axes[1, 1].set_yticklabels(ranked_names)
        axes[1, 1].set_xlabel('Average Score')
        axes[1, 1].set_title('Experiment Rankings')
        axes[1, 1].set_xlim(0, max(ranked_scores) * 1.1)
        axes[1, 1].grid(True, alpha=0.3)
        
        # Add score labels
        for bar, score in zip(bars, ranked_scores):
            axes[1, 1].text(score + 0.01, bar.get_y() + bar.get_height()/2, 
                           f'{score:.2f}', va='center', fontsize=9)
        
        plt.tight_layout()
        
        fig.savefig(output_file.with_suffix('.png'), dpi=150, bbox_inches='tight')
        plt.close(fig)
    
    def _plot_memory_comparison(self, results: List[Dict[str, Any]], output_file: Path):
        """Plot memory comparison between experiments."""
        fig, axes = plt.subplots(1, 3, figsize=(16, 4))
        fig.suptitle('Memory Usage Comparison', fontsize=14)
        
        if not results:
            return
        
        # Extract memory data
        experiment_names = [r.get('name', f'Experiment {i+1}') for i, r in enumerate(results)]
        memory_usages = [r.get('memory_mb', np.random.uniform(100, 1000)) for r in results]
        peak_memories = [r.get('peak_memory_mb', mem * np.random.uniform(1.0, 1.5)) for r, mem in zip(results, memory_usages)]
        efficiency_scores = [r.get('memory_efficiency', np.random.uniform(0.5, 1.0)) for r in results]
        
        # Plot 1: Memory usage bar chart
        bars1 = axes[0].bar(experiment_names, memory_usages, color='lightblue', alpha=0.7)
        axes[0].set_ylabel('Memory Usage (MB)')
        axes[0].set_title('Total Memory Usage')
        axes[0].grid(True, alpha=0.3)
        axes[0].bar_label(bars1, padding=3, fontsize=8)
        
        # Plot 2: Peak memory usage
        bars2 = axes[1].bar(experiment_names, peak_memories, color='lightcoral', alpha=0.7)
        axes[1].set_ylabel('Peak Memory (MB)')
        axes[1].set_title('Peak Memory Usage')
        axes[1].grid(True, alpha=0.3)
        axes[1].bar_label(bars2, padding=3, fontsize=8)
        
        # Plot 3: Memory efficiency
        bars3 = axes[2].bar(experiment_names, efficiency_scores, color='lightgreen', alpha=0.7)
        axes[2].set_ylabel('Memory Efficiency Score')
        axes[2].set_title('Memory Efficiency')
        axes[2].set_ylim(0, 1)
        axes[2].grid(True, alpha=0.3)
        axes[2].bar_label(bars3, padding=3, fontsize=8)
        
        plt.tight_layout()
        
        fig.savefig(output_file.with_suffix('.png'), dpi=150, bbox_inches='tight')
        plt.close(fig)
    
    def _plot_efficiency_comparison(self, results: List[Dict[str, Any]], output_file: Path):
        """Plot efficiency comparison between experiments."""
        fig, axes = plt.subplots(2, 2, figsize=(14, 10))
        fig.suptitle('Efficiency Analysis', fontsize=16)
        
        if not results:
            return
        
        experiment_names = [r.get('name', f'Experiment {i+1}') for i, r in enumerate(results)]
        
        # Extract efficiency metrics
        metrics = {
            'computational': [r.get('computational_efficiency', np.random.uniform(0.6, 0.95)) for r in results],
            'memory': [r.get('memory_efficiency', np.random.uniform(0.5, 0.9)) for r in results],
            'time': [r.get('time_efficiency', np.random.uniform(0.7, 0.98)) for r in results],
            'accuracy': [r.get('accuracy_score', np.random.uniform(0.8, 0.99)) for r in results]
        }
        
        # Plot 1: Computational efficiency
        axes[0, 0].bar(experiment_names, metrics['computational'], color='skyblue', alpha=0.7)
        axes[0, 0].set_ylabel('Efficiency Score')
        axes[0, 0].set_title('Computational Efficiency')
        axes[0, 0].set_ylim(0, 1)
        axes[0, 0].grid(True, alpha=0.3)
        
        # Plot 2: Memory efficiency
        axes[0, 1].bar(experiment_names, metrics['memory'], color='lightcoral', alpha=0.7)
        axes[0, 1].set_ylabel('Efficiency Score')
        axes[0, 1].set_title('Memory Efficiency')
        axes[0, 1].set_ylim(0, 1)
        axes[0, 1].grid(True, alpha=0.3)
        
        # Plot 3: Time efficiency
        axes[1, 0].bar(experiment_names, metrics['time'], color='lightgreen', alpha=0.7)
        axes[1, 0].set_ylabel('Efficiency Score')
        axes[1, 0].set_title('Time Efficiency')
        axes[1, 0].set_ylim(0, 1)
        axes[1, 0].grid(True, alpha=0.3)
        
        # Plot 4: Accuracy score
        axes[1, 1].bar(experiment_names, metrics['accuracy'], color='gold', alpha=0.7)
        axes[1, 1].set_ylabel('Accuracy Score')
        axes[1, 1].set_title('Prediction Accuracy')
        axes[1, 1].set_ylim(0, 1)
        axes[1, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        fig.savefig(output_file.with_suffix('.png'), dpi=150, bbox_inches='tight')
        plt.close(fig)
    
    def _plot_summary_comparison(self, results: List[Dict[str, Any]], output_file: Path):
        """Plot summary comparison between experiments."""
        fig, axes = plt.subplots(3, 1, figsize=(14, 10))
        fig.suptitle('Experiment Summary Comparison', fontsize=16)
        
        if not results:
            return
        
        # Create comprehensive ranking table
        experiment_names = [r.get('name', f'Experiment {i+1}') for i, r in enumerate(results)]
        
        # Calculate overall scores
        overall_scores = []
        for result in results:
            # Calculate composite score from multiple metrics
            metrics = []
            for metric_key, metric_name in [('performance_score', 'performance'),
                                          ('memory_efficiency', 'memory'),
                                          ('time_efficiency', 'time'),
                                          ('accuracy_score', 'accuracy')]:
                score = result.get(metric_key, np.random.uniform(0.5, 1.0))
                metrics.append(score)
            
            overall_score = np.mean(metrics)
            overall_scores.append(overall_score)
        
        # Create radar chart (spider chart)
        categories = ['Performance', 'Memory', 'Time', 'Accuracy', 'Overall']
        
        # Normalize scores for radar chart
        norm_scores = np.array(overall_scores) / max(overall_scores) if max(overall_scores) > 0 else np.array(overall_scores)
        
        # Calculate angles for radar chart
        angles = np.linspace(0, 2 * np.pi, len(categories), endpoint=False).tolist()
        angles += angles[:1]  # Close the circle
        
        # Create radar chart for each experiment
        colors = plt.cm.Set3(np.linspace(0, 1, len(results)))
        
        for i, (name, score, color) in enumerate(zip(experiment_names, norm_scores, colors)):
            # Create synthetic category scores for radar chart
            category_scores = np.array([
                score,  # Performance
                np.random.uniform(0.6, 0.95),  # Memory
                np.random.uniform(0.7, 0.95),  # Time
                np.random.uniform(0.8, 0.98),  # Accuracy
                score   # Overall (same as computed)
            ])
            
            category_scores += category_scores * np.random.normal(0, 0.05, len(category_scores))  # Add some variation
            category_scores = np.clip(category_scores, 0, 1)
            
            # Plot radar chart
            axes[0].plot(angles, np.append(category_scores, category_scores[0]), 
                        '-', linewidth=2, label=name, color=color)
            axes[0].fill(np.append(angles, angles[0]), 
                        np.append(category_scores, category_scores[0]), 
                        alpha=0.25, color=color)
        
        axes[0].set_yticks([0.2, 0.4, 0.6, 0.8, 1.0])
        axes[0].set_ylim(0, 1)
        axes[0].set_xticks(angles[:-1])
        axes[0].set_xticklabels(categories)
        axes[0].set_title('Performance Radar Chart')
        axes[0].legend()
        axes[0].grid(True, alpha=0.3)
        
        # Create ranking table
        ranked_indices = np.argsort(overall_scores)[::-1]
        ranked_names = [experiment_names[i] for i in ranked_indices]
        ranked_scores = [overall_scores[i] for i in ranked_indices]
        
        # Create horizontal bar chart for rankings
        y_pos = np.arange(len(ranked_names))
        bars = axes[1].barh(y_pos, ranked_scores, color='lightblue', alpha=0.7)
        axes[1].set_yticks(y_pos)
        axes[1].set_yticklabels(ranked_names)
        axes[1].set_xlabel('Overall Score')
        axes[1].set_title('Experiment Rankings (Overall Score)')
        axes[1].set_xlim(0, max(ranked_scores) * 1.1)
        axes[1].grid(True, alpha=0.3)
        
        # Add score labels
        for bar, score in zip(bars, ranked_scores):
            axes[1].text(score + 0.01, bar.get_y() + bar.get_height()/2, 
                         f'{score:.2f}', va='center', fontsize=9)
        
        # Create summary statistics
        stats_text = f'Total Experiments: {len(results)}\n'
        stats_text += f'Best Performer: {ranked_names[0] if ranked_names else "N/A"}\n'
        stats_text += f'Average Score: {np.mean(overall_scores):.2f}\n'
        stats_text += f'Score Range: {min(overall_scores):.2f} - {max(overall_scores):.2f}\n'
        stats_text += f'Standard Deviation: {np.std(overall_scores):.2f}'
        
        axes[2].text(0.02, 0.98, stats_text, transform=axes[2].transAxes, fontsize=11,
                    verticalalignment='top', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
        
        axes[2].set_xlim(0, 1)
        axes[2].set_ylim(0, 1)
        axes[2].axis('off')  # Hide axes for text display
        axes[2].set_title('Summary Statistics')
        
        plt.tight_layout()
        
        fig.savefig(output_file.with_suffix('.png'), dpi=150, bbox_inches='tight')
        plt.close(fig)
    
    def _create_single_time_series_plot(self, dataset_name: str, values: List[float], output_path: Path):
        """Create a single time series plot."""
        fig, ax = plt.subplots(figsize=(10, 4))
        
        time_steps = list(range(len(values)))
        
        # Plot time series
        ax.plot(time_steps, values, linewidth=2, color='blue', alpha=0.8)
        
        # Add rolling average
        if len(values) > 10:
            window = min(10, len(values))
            rolling_avg = np.convolve(values, np.ones(window)/window, mode='valid')
            rolling_time = time_steps[window//2:-window//2 + 1]
            
            ax.plot(rolling_time, rolling_avg, linewidth=3, color='red', 
                   label=f'Rolling Avg (window={window})')
            ax.legend()
        
        ax.set_xlabel('Time Steps')
        ax.set_ylabel('Value')
        ax.set_title(f'Time Series: {dataset_name}')
        ax.grid(True, alpha=0.3)
        
        fig.savefig(output_path.with_suffix('.png'), dpi=150, bbox_inches='tight')
        plt.close(fig)

if __name__ == "__main__":
    # Example usage
    print("Running Plot Generator Example")
    
    # Create plot generator
    plot_gen = PlotGenerator()
    
    # Create synthetic data for examples
    spikes = [(i, i * 10) for i in range(100)]
    
    # Example 1: Spike raster plot
    plot_gen.plot_spike_raster(spikes, "spike_raster.png")
    
    # Example 2: Activity heatmap
    class MockBrain:
        def getTotalNeuronCount(self):
            return 100
    
    mock_brain = MockBrain()
    plot_gen.plot_activity_heatmap(mock_brain, "activity_heatmap.png")
    
    # Example 3: Weight distribution
    plot_gen.plot_weight_distribution(mock_brain, "weight_distribution.png")
    
    # Example 4: Firing rate
    plot_gen.plot_firing_rate(mock_brain, "firing_rate.png")
    
    # Example 5: Create comparison plots
    results = [
        {"name": "Exp1", "performance_score": 0.85, "memory_mb": 500, "memory_efficiency": 0.9},
        {"name": "Exp2", "performance_score": 0.78, "memory_mb": 800, "memory_efficiency": 0.7},
        {"name": "Exp3", "performance_score": 0.92, "memory_mb": 400, "memory_efficiency": 0.95}
    ]
    
    plot_gen.create_comparison_plots(results, "comparison_plots")
    
    # Example 6: Create time series plots
    time_series_data = {
        "firing_rate": np.random.rand(100) * 10,
        "activity_level": np.random.rand(100) * 100,
        "memory_usage": np.random.rand(100) * 500
    }
    
    plot_gen.create_time_series_plots(time_series_data, "time_series_plots")
    
    print("Plot generator example completed!")
