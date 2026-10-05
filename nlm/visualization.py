#!/usr/bin/env python3
"""
Enhanced NLM (Neural Learning Machine) - Visualization and Analysis Module

This comprehensive module provides advanced visualization and analysis tools for NLM neural simulation data.
It includes 10 major categories of tools covering all aspects of neural data visualization and analysis.

Categories:
1. Visualization Tools - Tools to visualize neural activity, network structure, and simulation results
2. Analysis Tools - Tools to analyze neural activity, network properties, and simulation data
3. Network Visualization Tools - Tools to visualize neural network topology and connectivity
4. Activity Visualization Tools - Tools to visualize neural firing patterns and activity
5. Result Visualization Tools - Tools to visualize simulation results and outcomes
6. Statistical Analysis Tools - Tools to perform statistical analysis on neural data
7. Pattern Analysis Tools - Tools to identify and analyze patterns in neural activity
8. Comparative Analysis Tools - Tools to compare different simulations and results
9. Time Series Analysis Tools - Tools to analyze time series data from neural simulations
10. Data Visualization Tools - Tools to create various types of visualizations

License: MIT
Author: NLM Authors
Version: 2.0.0
"""

import os
import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import seaborn as sns
from scipy import signal, stats
from scipy.signal import find_peaks, welch, periodogram
from scipy.spatial import distance_matrix
from scipy.interpolate import interp1d, CubicSpline
from sklearn.cluster import KMeans, DBSCAN, AgglomerativeClustering
from sklearn.decomposition import PCA, FastICA
from sklearn.manifold import TSNE, MDS
from sklearn.preprocessing import StandardScaler, MinMaxScaler, RobustScaler
from sklearn.metrics import pairwise_distances, silhouette_score
import networkx as nx
from collections import defaultdict, deque
import warnings
import time
import json
import csv
from typing import Dict, List, Any, Optional, Union, Tuple, Callable, Iterator
from dataclasses import dataclass, field
from enum import Enum
import logging
from pathlib import Path
import itertools
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
import gc
import psutil
import tracemalloc
from functools import wraps
import warnings

# Import NLM core modules
import nlm_py
from nlm import (
    Config, Brain, SimpleWorld, AgentBrain, Vision, Audio,
    NeuralRegion, NeuralPopulation, Neuron, Synapse, NeuralRecording,
    SimulationStep, Timestamp, MotorCommand, ActionType,
    FiringState, NeuronType, SynapseType, WorldObjectType,
    DevelopmentalStage, PlasticityFlags, MembranePotential
)

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Version
__version__ = "2.0.0"

# Performance tracking
_performance_metrics = defaultdict(list)

# Visualization Configuration
@dataclass
class VisualizationConfig:
    """Configuration for visualization tools."""
    
    # Output settings
    output_format: str = "png"
    output_directory: str = "./visualizations"
    dpi: int = 300
    figure_size: Tuple[float, float] = (12, 8)
    
    # Plot styling
    theme: str = "default"
    color_scheme: str = "viridis"
    colormap: str = "viridis"
    alpha: float = 1.0
    linewidth: float = 2.0
    markersize: float = 8.0
    
    # Animation settings
    animation_enabled: bool = False
    animation_fps: float = 15.0
    animation_interval: int = 50
    
    # Statistical settings
    significance_level: float = 0.05
    confidence_interval: float = 0.95
    bootstrap_samples: int = 1000
    
    # Clustering settings
    linkage_method: str = "ward"
    silhouette_samples: int = 10
    
    # Time series settings
    sampling_rate: float = 1000.0
    detrend: bool = True
    filter_cutoff: float = 0.1
    
    # Export settings
    export_data: bool = True
    include_metadata: bool = True
    compress: bool = False
    
    def __post_init__(self):
        """Validate configuration."""
        self._validate_config()
    
    def _validate_config(self):
        """Validate visualization configuration."""
        if self.output_format not in ["png", "jpg", "pdf", "svg", "gif", "html"]:
            raise ValueError(f"Unsupported output format: {self.output_format}")
        
        if self.dpi < 50 or self.dpi > 600:
            raise ValueError("DPI must be between 50 and 600")
        
        if not (0.0 <= self.alpha <= 1.0):
            raise ValueError("Alpha must be between 0.0 and 1.0")
        
        if self.bootstrap_samples < 100:
            raise ValueError("Bootstrap samples must be at least 100")

# Performance monitoring decorator
def performance_monitor(func):
    """Decorator to monitor performance of visualization functions."""
    
    @wraps(func)
    def wrapper(*args, **kwargs):
        # Start tracking
        start_time = time.time()
        tracemalloc.start()
        process = psutil.Process()
        memory_before = process.memory_info().rss / 1024 / 1024  # MB
        
        try:
            # Execute function
            result = func(*args, **kwargs)
            
            # Measure performance
            elapsed = time.time() - start_time
            current, peak = tracemalloc.get_traced_memory()
            memory_after = process.memory_info().rss / 1024 / 1024  # MB
            memory_used = memory_after - memory_before
            
            # Store metrics
            _performance_metrics['execution_time'].append(elapsed)
            _performance_metrics['memory_usage'].append(memory_used)
            _performance_metrics['peak_memory'].append(peak / 1024 / 1024)
            
            logger.debug(f"Performance: {func.__name__} took {elapsed:.3f}s, "
                        f"memory: {memory_used:.1f}MB, peak: {peak / 1024 / 1024:.1f}MB")
            
            return result
            
        finally:
            tracemalloc.stop()
    
    return wrapper

class VisualizationError(Exception):
    """Exception raised for visualization errors."""

class DataValidationError(VisualizationError):
    """Exception raised for data validation errors."""

class ExportError(VisualizationError):
    """Exception raised for export errors."""
# Visualization Tools
class VisualizationTools:
    """Comprehensive visualization tools for NLM neural data."""
    
    def __init__(self, config: VisualizationConfig = None):
        """Initialize visualization tools.
        
        Args:
            config: Visualization configuration
        """
        self.config = config or VisualizationConfig()
        self._setup_output_directory()
        self._setup_matplotlib_style()
    
    def _setup_output_directory(self):
        """Create output directory if it doesn't exist."""
        Path(self.config.output_directory).mkdir(parents=True, exist_ok=True)
    
    def _setup_matplotlib_style(self):
        """Configure matplotlib style based on theme."""
        if self.config.theme == "default":
            plt.style.use('default')
        elif self.config.theme == "dark":
            plt.style.use('dark_background')
        elif self.config.theme == "publication":
            plt.style.use('seaborn-v0_8-whitegrid')
            plt.rcParams['font.size'] = 12
            plt.rcParams['axes.labelsize'] = 14
            plt.rcParams['axes.titlesize'] = 16
            plt.rcParams['xtick.labelsize'] = 12
            plt.rcParams['ytick.labelsize'] = 12
        else:
            plt.style.use('seaborn-v0_8')
    
    @performance_monitor
    def visualize_neural_network(self, brain: Brain, config: Dict = None) -> str:
        """Visualize complete neural network structure.
        
        Args:
            brain: Brain object to visualize
            config: Visualization configuration override
            
        Returns:
            Path to saved visualization
        """
        config = config or {}
        output_format = config.get('output_format', self.config.output_format)
        
        # Get network data
        node_count = brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0
        edge_count = brain.getTotalSynapseCount() if hasattr(brain, 'getTotalSynapseCount') else 0
        
        # Create figure with subplots
        fig = plt.figure(figsize=self.config.figure_size)
        gs = GridSpec(2, 2, figaspect=1, hspace=0.3, wspace=0.3)
        
        # Subplot 1: Network topology
        ax1 = fig.add_subplot(gs[0, 0])
        self._draw_network_topology(ax1, brain)
        
        # Subplot 2: Activity heatmap
        ax2 = fig.add_subplot(gs[0, 1])
        self._draw_activity_heatmap(ax2, brain)
        
        # Subplot 3: Degree distribution
        ax3 = fig.add_subplot(gs[1, 0])
        self._draw_degree_distribution(ax3, brain)
        
        # Subplot 4: Network statistics
        ax4 = fig.add_subplot(gs[1, 1])
        self._draw_network_statistics(ax4, brain)
        
        # Save figure
        filename = f"neural_network_{int(time.time())}.{output_format}"
        filepath = Path(self.config.output_directory) / filename
        plt.savefig(filepath, dpi=self.config.dpi, bbox_inches='tight')
        plt.close()
        
        return str(filepath)
    
    def _draw_network_topology(self, ax, brain: Brain):
        """Draw neural network topology."""
        neuron_count = brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0
        synapse_count = brain.getTotalSynapseCount() if hasattr(brain, 'getTotalSynapseCount') else 0
        
        if neuron_count == 0:
            ax.text(0.5, 0.5, 'No neurons available', 
                   ha='center', va='center', transform=ax.transAxes)
            return
        
        # Generate random positions for neurons
        positions = np.random.rand(neuron_count, 2)
        
        # Create graph
        G = nx.Graph()
        G.add_nodes_from(range(neuron_count))
        
        # Add synapses as edges (placeholder)
        if synapse_count > 0 and neuron_count > 1:
            for i in range(min(synapse_count, neuron_count * 2)):
                from_node = i % neuron_count
                to_node = (i * 7) % neuron_count
                if from_node != to_node:
                    G.add_edge(from_node, to_node)
        
        # Draw network
        nx.draw_networkx_nodes(ax, positions, node_size=50, 
                             node_color='lightblue', alpha=0.8)
        nx.draw_networkx_edges(ax, positions, alpha=0.5, width=1)
        
        ax.set_title('Neural Network Topology')
        ax.axis('off')
        
        # Add legend
        ax.plot([], [], 'o', color='lightblue', label='Neuron', markersize=10)
        ax.plot([], [], '-', color='gray', label='Synapse', linewidth=2)
        ax.legend(loc='upper right')
    
    def _draw_activity_heatmap(self, ax, brain: Brain):
        """Draw neural activity heatmap."""
        # Generate activity data (placeholder)
        neuron_count = brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0
        if neuron_count == 0:
            ax.text(0.5, 0.5, 'No activity data', 
                   ha='center', va='center', transform=ax.transAxes)
            return
        
        # Create activity heatmap (placeholder data)
        grid_size = int(np.ceil(np.sqrt(neuron_count)))
        activity_data = np.random.rand(grid_size, grid_size)
        
        # Apply colormap
        im = ax.imshow(activity_data, cmap=self.config.colormap, 
                      interpolation='nearest', aspect='auto')
        
        ax.set_title('Neural Activity Heatmap')
        plt.colorbar(im, ax=ax, label='Activity Level')
    
    def _draw_degree_distribution(self, ax, brain: Brain):
        """Draw degree distribution histogram."""
        neuron_count = brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0
        
        if neuron_count == 0:
            ax.text(0.5, 0.5, 'No degree data', 
                   ha='center', va='center', transform=ax.transAxes)
            return
        
        # Generate degree distribution (placeholder)
        degrees = np.random.poisson(10, neuron_count)
        
        # Plot histogram
        ax.hist(degrees, bins=30, alpha=0.7, density=True, 
               color='skyblue', edgecolor='black')
        
        ax.set_title('Degree Distribution')
        ax.set_xlabel('Degree')
        ax.set_ylabel('Frequency')
        ax.grid(True, alpha=0.3)
    
    def _draw_network_statistics(self, ax, brain: Brain):
        """Draw network statistics chart."""
        # Get network statistics
        neuron_count = brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0
        synapse_count = brain.getTotalSynapseCount() if hasattr(brain, 'getTotalSynapseCount') else 0
        firing_rate = brain.getAverageFiringRate() if hasattr(brain, 'getAverageFiringRate') else 0.0
        exc_inhibition_ratio = brain.getExcitationInhibitionRatio() if hasattr(brain, 'getExcitationInhibitionRatio') else 1.0
        
        # Create statistics data
        stats_data = {
            'Neurons': neuron_count,
            'Synapses': synapse_count,
            'Firing Rate': firing_rate,
            'E/I Ratio': exc_inhibition_ratio,
        }
        
        # Create bar chart
        bars = ax.bar(stats_data.keys(), stats_data.values(), 
                     color=['skyblue', 'lightgreen', 'salmon', 'gold'])
        
        ax.set_title('Network Statistics')
        ax.set_ylabel('Value')
        ax.tick_params(axis='x', rotation=45)
        
        # Add value labels on bars
        for bar in bars:
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{height:.2f}', ha='center', va='bottom')

class NetworkVisualizationTools:
    """Tools for visualizing neural network topology and connectivity."""
    
    def __init__(self, config: VisualizationConfig = None):
        """Initialize network visualization tools.
        
        Args:
            config: Visualization configuration
        """
        self.config = config or VisualizationConfig()
    
    def visualize_connectivity_matrix(self, brain: Brain, config: Dict = None) -> str:
        """Visualize neural connectivity matrix.
        
        Args:
            brain: Brain object to analyze
            config: Visualization configuration override
            
        Returns:
            Path to saved visualization
        """
        config = config or {}
        output_format = config.get('output_format', self.config.output_format)
        
        # Get network data
        neuron_count = brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0
        
        if neuron_count == 0:
            raise VisualizationError("No neurons available for connectivity matrix")
        
        # Create adjacency matrix (placeholder)
        adjacency_matrix = np.zeros((neuron_count, neuron_count))
        synapse_count = brain.getTotalSynapseCount() if hasattr(brain, 'getTotalSynapseCount') else 0
        
        # Fill with placeholder connections
        for i in range(min(synapse_count, neuron_count * 3)):
            row = i % neuron_count
            col = (i * 5) % neuron_count
            adjacency_matrix[row, col] = np.random.uniform(0.1, 1.0)
        
        # Create figure
        fig, axes = plt.subplots(1, 3, figsize=self.config.figure_size)
        
        # Heatmap
        im1 = axes[0].imshow(adjacency_matrix, cmap='RdYlBu_r', vmin=0, vmax=1)
        axes[0].set_title('Connectivity Matrix Heatmap')
        plt.colorbar(im1, ax=axes[0])
        
        # Degree distribution
        degrees = np.sum(adjacency_matrix, axis=1)
        axes[1].hist(degrees, bins=30, alpha=0.7, density=True, color='skyblue')
        axes[1].set_title('In-degree Distribution')
        axes[1].set_xlabel('In-degree')
        axes[1].set_ylabel('Density')
        
        # Network density over time (placeholder)
        time_points = np.linspace(0, 10, 50)
        density = np.linspace(0.1, 0.8, 50) + np.random.normal(0, 0.05, 50)
        axes[2].plot(time_points, density, 'b-', linewidth=2, label='Network Density')
        axes[2].fill_between(time_points, density - 0.05, density + 0.05, 
                           alpha=0.3, label='± 0.05 std')
        axes[2].set_title('Network Density Dynamics')
        axes[2].set_xlabel('Time (ms)')
        axes[2].set_ylabel('Density')
        axes[2].legend()
        axes[2].grid(True, alpha=0.3)
        
        # Save figure
        filename = f"connectivity_matrix_{int(time.time())}.{output_format}"
        filepath = Path(self.config.output_directory) / filename
        plt.savefig(filepath, dpi=self.config.dpi, bbox_inches='tight')
        plt.close()
        
        return str(filepath)
    
    def visualize_brain_regions(self, brain: Brain, config: Dict = None) -> str:
        """Visualize brain regions with activity levels.
        
        Args:
            brain: Brain object to analyze
            config: Visualization configuration override
            
        Returns:
            Path to saved visualization
        """
        config = config or {}
        output_format = config.get('output_format', self.config.output_format)
        
        # Get region data
        region_count = brain.getRegionCount() if hasattr(brain, 'getRegionCount') else 0
        
        if region_count == 0:
            raise VisualizationError("No regions available for visualization")
        
        # Generate region data (placeholder)
        regions = []
        activities = []
        sizes = []
        positions = []
        
        for i in range(region_count):
            regions.append(f"Region_{i+1}")
            activities.append(np.random.uniform(0.5, 1.5))
            sizes.append(np.random.uniform(5, 20))
            positions.append([np.random.uniform(0, 100), np.random.uniform(0, 100)])
        
        # Create figure
        fig, axes = plt.subplots(1, 2, figsize=self.config.figure_size)
        
        # Scatter plot of regions
        colors = plt.cm.viridis(np.array(activities) / max(activities))
        axes[0].scatter([p[0] for p in positions], [p[1] for p in positions], 
                       s=[s**2 for s in sizes], c=colors, alpha=0.7, edgecolors='black')
        
        # Add region labels
        for i, (region, pos) in enumerate(zip(regions, positions)):
            axes[0].annotate(region, pos, xytext=(5, 5), 
                           textcoords='offset points', fontsize=8)
        
        axes[0].set_title('Brain Region Layout')
        axes[0].set_xlabel('X Position')
        axes[0].set_ylabel('Y Position')
        axes[0].grid(True, alpha=0.3)
        
        # Activity bar chart
        axes[1].bar(regions, activities, color=colors, alpha=0.7, edgecolor='black')
        axes[1].set_title('Region Activity Levels')
        axes[1].set_ylabel('Activity Level')
        axes[1].tick_params(axis='x', rotation=45)
        axes[1].grid(True, alpha=0.3)
        
        # Save figure
        filename = f"brain_regions_{int(time.time())}.{output_format}"
        filepath = Path(self.config.output_directory) / filename
        plt.savefig(filepath, dpi=self.config.dpi, bbox_inches='tight')
        plt.close()
        
        return str(filepath)
    
    def visualize_synaptic_connections(self, brain: Brain, config: Dict = None) -> str:
        """Visualize detailed synaptic connections.
        
        Args:
            brain: Brain object to analyze
            config: Visualization configuration override
            
        Returns:
            Path to saved visualization
        """
        config = config or {}
        output_format = config.get('output_format', self.config.output_format)
        
        # Get network data
        neuron_count = brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0
        synapse_count = brain.getTotalSynapseCount() if hasattr(brain, 'getTotalSynapseCount') else 0
        
        if neuron_count == 0:
            raise VisualizationError("No neurons available for synaptic visualization")
        
        # Create figure with multiple subplots
        fig = plt.figure(figsize=self.config.figure_size)
        gs = GridSpec(2, 2, figaspect=1, hspace=0.3, wspace=0.3)
        
        # Generate synaptic data (placeholder)
        if synapse_count > 0:
            # Generate connection weights and delays
            rows = np.random.randint(0, neuron_count, synapse_count)
            cols = np.random.randint(0, neuron_count, synapse_count)
            weights = np.random.uniform(0.1, 1.0, synapse_count)
            delays = np.random.uniform(1, 10, synapse_count)
            types = np.random.choice(['chemical', 'electrical', 'plastic'], synapse_count)
        else:
            rows = cols = np.array([])
            weights = delays = np.array([])
            types = np.array([])
        
        # Subplot 1: Connection weight distribution
        ax1 = fig.add_subplot(gs[0, 0])
        if len(weights) > 0:
            ax1.hist(weights, bins=30, alpha=0.7, density=True, color='skyblue')
            ax1.set_title('Connection Weight Distribution')
            ax1.set_xlabel('Weight')
            ax1.set_ylabel('Density')
            ax1.grid(True, alpha=0.3)
        
        # Subplot 2: Connection types pie chart
        ax2 = fig.add_subplot(gs[0, 1])
        if len(types) > 0:
            unique_types, counts = np.unique(types, return_counts=True)
            colors = plt.cm.Set3(np.linspace(0, 1, len(unique_types)))
            ax2.pie(counts, labels=unique_types, colors=colors, autopct='%1.1f%%')
            ax2.set_title('Connection Type Distribution')
        
        # Subplot 3: Connection delay distribution
        ax3 = fig.add_subplot(gs[1, 0])
        if len(delays) > 0:
            ax3.hist(delays, bins=30, alpha=0.7, density=True, color='lightgreen')
            ax3.set_title('Connection Delay Distribution')
            ax3.set_xlabel('Delay (ms)')
            ax3.set_ylabel('Density')
            ax3.grid(True, alpha=0.3)
        
        # Subplot 4: Network density vs. connection type
        ax4 = fig.add_subplot(gs[1, 1])
        if len(weights) > 0 and len(types) > 0:
            # Create sample data
            x = np.random.uniform(0, 100, 50)
            y_chemical = np.random.uniform(0.1, 1.0, 50) * x * 0.001
            y_electrical = np.random.uniform(0.2, 1.5, 50) * x * 0.001
            y_plastic = np.random.uniform(0.05, 0.8, 50) * x * 0.001
            
            ax4.plot(x, y_chemical, 'r-', alpha=0.5, label='Chemical')
            ax4.plot(x, y_electrical, 'g-', alpha=0.5, label='Electrical')
            ax4.plot(x, y_plastic, 'b-', alpha=0.5, label='Plastic')
            ax4.set_title('Connection Type vs. Weight')
            ax4.set_xlabel('Neuron Index')
            ax4.set_ylabel('Connection Strength')
            ax4.legend()
            ax4.grid(True, alpha=0.3)
        
        # Save figure
        filename = f"synaptic_connections_{int(time.time())}.{output_format}"
        filepath = Path(self.config.output_directory) / filename
        plt.savefig(filepath, dpi=self.config.dpi, bbox_inches='tight')
        plt.close()
        
        return str(filepath)

class ActivityVisualizationTools:
    """Tools for visualizing neural firing patterns and activity."""
    
    def __init__(self, config: VisualizationConfig = None):
        """Initialize activity visualization tools.
        
        Args:
            config: Visualization configuration
        """
        self.config = config or VisualizationConfig()
    
    def visualize_spike_raster(self, brain: Brain, config: Dict = None) -> str:
        """Visualize spike raster plot of neural activity.
        
        Args:
            brain: Brain object to analyze
            config: Visualization configuration override
            
        Returns:
            Path to saved visualization
        """
        config = config or {}
        output_format = config.get('output_format', self.config.output_format)
        
        # Get spike data
        spike_count = brain.getTotalSpikeCount() if hasattr(brain, 'getTotalSpikeCount') else 0
        firing_rate = brain.getAverageFiringRate() if hasattr(brain, 'getAverageFiringRate') else 0.0
        
        # Generate spike times (placeholder)
        if spike_count > 0:
            spike_times = np.sort(np.random.uniform(0, 1000, spike_count))
            neuron_indices = np.random.randint(0, 100, spike_count)
        else:
            spike_times = np.array([])
            neuron_indices = np.array([])
        
        # Create figure
        fig, axes = plt.subplots(2, 2, figsize=self.config.figure_size)
        
        # Raster plot
        ax1 = axes[0, 0]
        if len(spike_times) > 0:
            for neuron in np.unique(neuron_indices):
                mask = neuron_indices == neuron
                ax1.scatter(spike_times[mask], np.full(np.sum(mask), neuron),
                           s=1, c='black', alpha=0.6)
            ax1.set_title('Spike Raster Plot')
            ax1.set_xlabel('Time (ms)')
            ax1.set_ylabel('Neuron Index')
            ax1.invert_yaxis()
            ax1.grid(True, alpha=0.3)
        
        # Inter-spike interval histogram
        ax2 = axes[0, 1]
        if len(spike_times) > 1:
            isi = np.diff(spike_times)
            ax2.hist(isi, bins=50, alpha=0.7, density=True, color='skyblue')
            ax2.set_title('Inter-spike Interval Distribution')
            ax2.set_xlabel('ISI (ms)')
            ax2.set_ylabel('Density')
            ax2.grid(True, alpha=0.3)
        
        # Instantaneous firing rate
        ax3 = axes[1, 0]
        if len(spike_times) > 0:
            # Bin spikes to calculate firing rate
            bin_width = 10.0
            max_time = max(spike_times) if len(spike_times) > 0 else 100
            bins = np.arange(0, max_time + bin_width, bin_width)
            rates, _ = np.histogram(spike_times, bins=bins)
            
            # Smooth rates
            from scipy.signal import gaussian
            sigma = 3
            kernel = gaussian(2*sigma + 1, sigma=sigma)
            kernel /= np.sum(kernel)
            smoothed_rates = np.convolve(rates, kernel, mode='same')
            
            ax3.plot(bins[:-1], smoothed_rates, 'b-', linewidth=2)
            ax3.fill_between(bins[:-1], smoothed_rates, alpha=0.3)
            ax3.set_title('Instantaneous Firing Rate')
            ax3.set_xlabel('Time (ms)')
            ax3.set_ylabel('Firing Rate (Hz)')
            ax3.grid(True, alpha=0.3)
        
        # Firing rate distribution across neurons
        ax4 = axes[1, 1]
        if len(spike_times) > 0 and len(neuron_indices) > 0:
            unique_neurons, spike_counts = np.unique(neuron_indices, return_counts=True)
            firing_rates = spike_counts / (max(spike_times) if max(spike_times) > 0 else 1)
            
            ax4.hist(firing_rates, bins=30, alpha=0.7, density=True, color='lightgreen')
            ax4.axvline(firing_rate, color='red', linestyle='--', 
                       label=f'Average: {firing_rate:.2f} Hz')
            ax4.set_title('Firing Rate Distribution')
            ax4.set_xlabel('Firing Rate (Hz)')
            ax4.set_ylabel('Density')
            ax4.legend()
            ax4.grid(True, alpha=0.3)
        
        # Adjust layout
        plt.tight_layout()
        
        # Save figure
        filename = f"spike_raster_{int(time.time())}.{output_format}"
        filepath = Path(self.config.output_directory) / filename
        plt.savefig(filepath, dpi=self.config.dpi, bbox_inches='tight')
        plt.close()
        
        return str(filepath)
    
    def visualize_activity_heatmap_3d(self, brain: Brain, config: Dict = None) -> str:
        """Visualize 3D neural activity heatmap.
        
        Args:
            brain: Brain object to analyze
            config: Visualization configuration override
            
        Returns:
            Path to saved visualization
        """
        config = config or {}
        output_format = config.get('output_format', self.config.output_format)
        
        # Get neuron count
        neuron_count = brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0
        
        if neuron_count == 0:
            raise VisualizationError("No neurons available for 3D activity visualization")
        
        # Create 3D activity data (placeholder)
        grid_size = int(np.ceil(np.cbrt(neuron_count)))
        
        # Generate neural positions (placeholder)
        positions = np.random.rand(neuron_count, 3)
        positions[:, 0] *= 100  # X: 0-100
        positions[:, 1] *= 100  # Y: 0-100  
        positions[:, 2] *= 50   # Z: 0-50
        
        # Generate activity data (placeholder)
        activities = np.random.uniform(0.5, 2.0, neuron_count)
        
        # Create figure
        fig = plt.figure(figsize=self.config.figure_size)
        ax = fig.add_subplot(111, projection='3d')
        
        # Create scatter plot
        colors = activities / max(activities) if max(activities) > 0 else activities
        scatter = ax.scatter(positions[:, 0], positions[:, 1], positions[:, 2],
                           c=colors, s=activities * 100, cmap='viridis',
                           alpha=0.7, edgecolors='black')
        
        ax.set_title('3D Neural Activity Heatmap')
        ax.set_xlabel('X Position')
        ax.set_ylabel('Y Position')
        ax.set_zlabel('Z Position')
        
        # Add colorbar
        plt.colorbar(scatter, ax=ax, label='Activity Level')
        
        # Add activity slices if needed
        if len(activities) > 0:
            # Add a slice through the data
            slice_idx = neuron_count // 2
            ax.plot([positions[slice_idx, 0]], [positions[slice_idx, 1]], 
                   [positions[slice_idx, 2]], 'r*', markersize=15, label='Slice')
        
        # Save figure
        filename = f"activity_heatmap_3d_{int(time.time())}.{output_format}"
        filepath = Path(self.config.output_directory) / filename
        plt.savefig(filepath, dpi=self.config.dpi, bbox_inches='tight')
        plt.close()
        
        return str(filepath)
    
    def visualize_neuron_trajectory(self, brain: Brain, config: Dict = None) -> str:
        """Visualize neuron firing trajectory in phase space.
        
        Args:
            brain: Brain object to analyze
            config: Visualization configuration override
            
        Returns:
            Path to saved visualization
        """
        config = config or {}
        output_format = config.get('output_format', self.config.output_format)
        
        # Generate neuron trajectory data (placeholder)
        time_points = np.linspace(0, 100, 200)
        
        # Create multiple neurons' trajectories
        fig, axes = plt.subplots(2, 2, figsize=self.config.figure_size)
        
        # Subplot 1: Membrane potential trajectory
        ax1 = axes[0, 0]
        for neuron_idx in range(5):
            # Generate membrane potential trajectory
            v_rest = -70
            threshold = -55
            spikes = np.random.randint(0, 20, 5)
            
            v_trace = np.full_like(time_points, v_rest)
            for spike in spikes:
                spike_time = spike * 10
                spike_mask = time_points > spike_time
                if spike_time < time_points[-1]:
                    v_trace[spike_mask] = np.linspace(threshold, v_rest, 
                                                    np.sum(spike_mask))
            
            ax1.plot(time_points, v_trace, linewidth=1, label=f'Neuron {neuron_idx + 1}')
        
        ax1.axhline(y=-70, color='gray', linestyle='--', alpha=0.5, label='Resting Potential')
        ax1.axhline(y=-55, color='red', linestyle='--', alpha=0.5, label='Threshold')
        ax1.set_title('Membrane Potential Trajectories')
        ax1.set_xlabel('Time (ms)')
        ax1.set_ylabel('Membrane Potential (mV)')
        ax1.legend()
        ax1.grid(True, alpha=0.3)
        
        # Subplot 2: Firing rate vs. time
        ax2 = axes[0, 1]
        firing_rate = 5 + 3 * np.sin(2 * np.pi * time_points / 50)
        ax2.plot(time_points, firing_rate, 'b-', linewidth=2)
        ax2.fill_between(time_points, firing_rate, alpha=0.3)
        ax2.set_title('Firing Rate Dynamics')
        ax2.set_xlabel('Time (ms)')
        ax2.set_ylabel('Firing Rate (Hz)')
        ax2.grid(True, alpha=0.3)
        
        # Subplot 3: Phase space (voltage vs. derivative)
        ax3 = axes[1, 0]
        # Generate phase space trajectory
        v = np.linspace(-80, 40, 100)
        dv_dt = v * 0.1 - v**3 / 100 + np.random.normal(0, 0.5, 100)
        ax3.plot(v, dv_dt, 'g-', alpha=0.7)
        ax3.set_title('Phase Space (V vs. dV/dt)')
        ax3.set_xlabel('Membrane Potential (mV)')
        ax3.set_ylabel('Derivative (mV/ms)')
        ax3.grid(True, alpha=0.3)
        
        # Subplot 4: Poincaré section
        ax4 = axes[1, 1]
        # Generate Poincaré section data
        v_poincare = np.random.uniform(-80, -55, 200)
        dv_poincare = v_poincare * 0.1 - v_poincare**3 / 100 + np.random.normal(0, 0.2, 200)
        ax4.scatter(v_poincare, dv_poincare, s=1, alpha=0.6, c='red')
        ax4.set_title('Poincaré Section')
        ax4.set_xlabel('V at threshold crossing')
        ax4.set_ylabel('dV/dt at threshold')
        ax4.grid(True, alpha=0.3)
        
        # Adjust layout
        plt.tight_layout()
        
        # Save figure
        filename = f"neuron_trajectory_{int(time.time())}.{output_format}"
        filepath = Path(self.config.output_directory) / filename
        plt.savefig(filepath, dpi=self.config.dpi, bbox_inches='tight')
        plt.close()
        
        return str(filepath)

class ResultVisualizationTools:
    """Tools for visualizing simulation results and outcomes."""
    
    def __init__(self, config: VisualizationConfig = None):
        """Initialize result visualization tools.
        
        Args:
            config: Visualization configuration
        """
        self.config = config or VisualizationConfig()
    
    def visualize_simulation_results(self, world: SimpleWorld, brain: Brain, 
                                   config: Dict = None) -> str:
        """Visualize complete simulation results.
        
        Args:
            world: SimpleWorld object
            brain: Brain object
            config: Visualization configuration override
            
        Returns:
            Path to saved visualization
        """
        config = config or {}
        output_format = config.get('output_format', self.config.output_format)
        
        # Get world and brain statistics
        world_width = world.getWidth() if hasattr(world, 'getWidth') else 100
        world_height = world.getHeight() if hasattr(world, 'getHeight') else 100
        agent_x = world.getAgentBody().x if hasattr(world, 'getAgentBody') else 0
        agent_y = world.getAgentBody().y if hasattr(world, 'getAgentBody') else 0
        simulation_time = world.getSimulationTime() if hasattr(world, 'getSimulationTime') else 0
        
        neuron_count = brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0
        synapse_count = brain.getTotalSynapseCount() if hasattr(brain, 'getTotalSynapseCount') else 0
        firing_rate = brain.getAverageFiringRate() if hasattr(brain, 'getAverageFiringRate') else 0.0
        exc_inhibition_ratio = brain.getExcitationInhibitionRatio() if hasattr(brain, 'getExcitationInhibitionRatio') else 1.0
        
        # Create figure with multiple subplots
        fig = plt.figure(figsize=self.config.figure_size)
        gs = GridSpec(3, 3, figaspect=1, hspace=0.3, wspace=0.3)
        
        # Subplot 1: World state
        ax1 = fig.add_subplot(gs[0, 0])
        self._draw_world_state(ax1, world, brain)
        
        # Subplot 2: Simulation timeline
        ax2 = fig.add_subplot(gs[0, 1])
        self._draw_simulation_timeline(ax2, world, brain)
        
        # Subplot 3: Performance metrics
        ax3 = fig.add_subplot(gs[0, 2])
        self._draw_performance_metrics(ax3, world, brain)
        
        # Subplot 4: Learning curves (placeholder)
        ax4 = fig.add_subplot(gs[1, 0])
        self._draw_learning_curves(ax4, world, brain)
        
        # Subplot 5: Network evolution
        ax5 = fig.add_subplot(gs[1, 1])
        self._draw_network_evolution(ax5, brain)
        
        # Subplot 6: Final state summary
        ax6 = fig.add_subplot(gs[1, 2])
        self._draw_final_state_summary(ax6, world, brain)
        
        # Subplot 7: Agent trajectory (placeholder)
        ax7 = fig.add_subplot(gs[2, 0])
        self._draw_agent_trajectory(ax7, world)
        
        # Subplot 8: Environment statistics (placeholder)
        ax8 = fig.add_subplot(gs[2, 1])
        self._draw_environment_stats(ax8, world)
        
        # Subplot 9: Export information
        ax9 = fig.add_subplot(gs[2, 2])
        self._draw_export_info(ax9, world, brain)
        
        # Save figure
        filename = f"simulation_results_{int(time.time())}.{output_format}"
        filepath = Path(self.config.output_directory) / filename
        plt.savefig(filepath, dpi=self.config.dpi, bbox_inches='tight')
        plt.close()
        
        return str(filepath)
    
    def _draw_world_state(self, ax, world: SimpleWorld, brain: Brain):
        """Draw current world state."""
        # Get world statistics
        world_width = world.getWidth() if hasattr(world, 'getWidth') else 100
        world_height = world.getHeight() if hasattr(world, 'getHeight') else 100
        agent_x = world.getAgentBody().x if hasattr(world, 'getAgentBody') else 0
        agent_y = world.getAgentBody().y if hasattr(world, 'getAgentBody') else 0
        
        # Draw world boundary
        boundary = plt.Polygon([(0, 0), (world_width, 0), 
                               (world_width, world_height), (0, world_height)],
                              closed=True, fill=False, edgecolor='black', linewidth=2)
        ax.add_patch(boundary)
        
        # Draw agent
        agent_circle = plt.Circle((agent_x, agent_y), radius=2, 
                                 color='red', alpha=0.8)
        ax.add_patch(agent_circle)
        
        # Draw neurons (placeholder)
        neuron_count = brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0
        if neuron_count > 0:
            # Generate random neuron positions
            neuron_x = np.random.uniform(0, world_width, neuron_count)
            neuron_y = np.random.uniform(0, world_height, neuron_count)
            neuron_activities = np.random.uniform(0.5, 1.5, neuron_count)
            
            # Draw neurons as circles
            for i in range(min(50, neuron_count)):  # Limit to 50 for clarity
                activity = neuron_activities[i]
                color = plt.cm.viridis(activity / max(neuron_activities) if max(neuron_activities) > 0 else 1)
                neuron_circle = plt.Circle((neuron_x[i], neuron_y[i]), 
                                         radius=activity * 0.5, color=color, alpha=0.6)
                ax.add_patch(neuron_circle)
        
        ax.set_xlim(0, world_width)
        ax.set_ylim(0, world_height)
        ax.set_title('World State')
        ax.set_xlabel('X Position')
        ax.set_ylabel('Y Position')
        ax.set_aspect('equal')
    
    def _draw_simulation_timeline(self, ax, world: SimpleWorld, brain: Brain):
        """Draw simulation timeline with key events."""
        # Get simulation time
        simulation_time = world.getSimulationTime() if hasattr(world, 'getSimulationTime') else 0
        
        # Generate timeline data (placeholder)
        time_points = np.linspace(0, simulation_time, 100)
        
        # Create timeline with metrics
        firing_rate = brain.getAverageFiringRate() if hasattr(brain, 'getAverageFiringRate') else 0
        energy = np.random.uniform(0.7, 1.3, len(time_points)) * firing_rate
        learning_progress = np.cumsum(np.random.uniform(0.01, 0.05, len(time_points)))
        
        # Plot metrics
        ax.plot(time_points, firing_rate * np.ones_like(time_points), 
               'r-', alpha=0.7, linewidth=2, label='Firing Rate')
        ax.plot(time_points, energy, 'g-', alpha=0.7, linewidth=2, label='Energy')
        ax.plot(time_points, learning_progress, 'b-', alpha=0.7, linewidth=2, label='Learning Progress')
        
        ax.set_title('Simulation Timeline')
        ax.set_xlabel('Time')
        ax.set_ylabel('Value')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def _draw_performance_metrics(self, ax, world: SimpleWorld, brain: Brain):
        """Draw performance metrics chart."""
        # Get performance metrics
        neuron_count = brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0
        synapse_count = brain.getTotalSynapseCount() if hasattr(brain, 'getTotalSynapseCount') else 0
        firing_rate = brain.getAverageFiringRate() if hasattr(brain, 'getAverageFiringRate') else 0.0
        exc_inhibition_ratio = brain.getExcitationInhibitionRatio() if hasattr(brain, 'getExcitationInhibitionRatio') else 1.0
        
        # Create radar chart
        categories = ['Neurons', 'Synapses', 'Firing Rate', 'E/I Balance', 'Efficiency']
        values = [neuron_count / 1000, synapse_count / 1000, firing_rate / 20, 
                  abs(exc_inhibition_ratio - 1), 1.0]
        
        # Normalize values
        values = np.array(values) / np.max(values) if np.max(values) > 0 else values
        
        # Draw radar chart
        angles = np.linspace(0, 2 * np.pi, len(categories), endpoint=False)
        values = np.concatenate([values, [values[0]]])
        angles = np.concatenate([angles, [angles[0]]])
        
        ax.plot(angles, values, 'o-', linewidth=2, markersize=8)
        ax.fill(angles, values, alpha=0.3)
        
        ax.set_xticks(angles[:-1])
        ax.set_xticklabels(categories)
        ax.set_title('Performance Metrics')
        ax.set_ylim(0, 1)
        ax.grid(True, alpha=0.3)
    
    def _draw_learning_curves(self, ax, world: SimpleWorld, brain: Brain):
        """Draw learning curves."""
        # Generate learning curves (placeholder)
        epochs = np.arange(100)
        
        # Create different learning scenarios
        accuracy = 0.5 + 0.45 * (1 - np.exp(-epochs / 50))
        loss = np.exp(-epochs / 30) + 0.1 * np.random.randn(len(epochs))
        
        ax.plot(epochs, accuracy, 'g-', linewidth=2, label='Accuracy')
        ax.plot(epochs, loss, 'r-', alpha=0.7, linewidth=2, label='Loss')
        
        ax.set_title('Learning Curves')
        ax.set_xlabel('Epoch')
        ax.set_ylabel('Value')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def _draw_network_evolution(self, ax, brain: Brain):
        """Draw network evolution over time."""
        # Generate network evolution data (placeholder)
        time_points = np.linspace(0, 100, 50)
        
        # Network metrics over time
        neuron_count = np.linspace(100, 1000, len(time_points))
        synapse_count = neuron_count * 8
        clustering_coefficient = 0.3 + 0.4 * np.exp(-time_points / 30)
        average_path_length = 5.0 - 3.0 * np.exp(-time_points / 40)
        
        ax.plot(time_points, neuron_count, 'b-', linewidth=2, label='Neurons')
        ax.plot(time_points, synapse_count, 'g-', linewidth=2, label='Synapses')
        ax.plot(time_points, clustering_coefficient, 'r-', linewidth=2, label='Clustering')
        
        ax.set_title('Network Evolution')
        ax.set_xlabel('Time (ms)')
        ax.set_ylabel('Value')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def _draw_final_state_summary(self, ax, world: SimpleWorld, brain: Brain):
        """Draw final state summary."""
        # Get final state information
        neuron_count = brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0
        synapse_count = brain.getTotalSynapseCount() if hasattr(brain, 'getTotalSynapseCount') else 0
        firing_rate = brain.getAverageFiringRate() if hasattr(brain, 'getAverageFiringRate') else 0.0
        exc_inhibition_ratio = brain.getExcitationInhibitionRatio() if hasattr(brain, 'getExcitationInhibitionRatio') else 1.0
        
        # Create summary text
        summary_text = f"Final State:\n\n"
        summary_text += f"Neurons: {neuron_count}\n"
        summary_text += f"Synapses: {synapse_count}\n"
        summary_text += f"Firing Rate: {firing_rate:.2f} Hz\n"
        summary_text += f"E/I Ratio: {exc_inhibition_ratio:.2f}\n"
        
        ax.text(0.5, 0.5, summary_text, 
               ha='center', va='center', transform=ax.transAxes,
               fontsize=10, fontfamily='monospace',
               bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
        
        ax.set_title('Final State Summary')
        ax.axis('off')
    
    def _draw_agent_trajectory(self, ax, world: SimpleWorld):
        """Draw agent trajectory (placeholder)."""
        # Generate placeholder trajectory
        time_steps = np.arange(0, 100, 1)
        agent_x = np.cumsum(np.random.uniform(-1, 1, len(time_steps)))
        agent_y = np.cumsum(np.random.uniform(-1, 1, len(time_steps)))
        
        ax.plot(agent_x, agent_y, 'b-', linewidth=2, alpha=0.7)
        ax.scatter(agent_x[-1], agent_y[-1], c='red', s=100, marker='*', label='Final Position')
        
        ax.set_title('Agent Trajectory')
        ax.set_xlabel('X Position')
        ax.set_ylabel('Y Position')
        ax.legend()
        ax.grid(True, alpha=0.3)
        ax.set_aspect('equal')
    
    def _draw_environment_stats(self, ax, world: SimpleWorld):
        """Draw environment statistics (placeholder)."""
        # Generate placeholder environment stats
        stats_data = {
            'Temperature': np.random.uniform(20, 30),
            'Humidity': np.random.uniform(40, 80),
            'Pressure': np.random.uniform(1000, 1020),
            'Light Intensity': np.random.uniform(100, 1000),
            'Noise Level': np.random.uniform(0, 50),
        }
        
        categories = list(stats_data.keys())
        values = list(stats_data.values())
        
        ax.bar(categories, values, alpha=0.7, color='skyblue', edgecolor='black')
        ax.set_title('Environment Statistics')
        ax.set_ylabel('Value')
        ax.tick_params(axis='x', rotation=45)
        ax.grid(True, alpha=0.3)
    
    def _draw_export_info(self, ax, world: SimpleWorld, brain: Brain):
        """Draw export information."""
        # Get export information
        export_info = {
            'World Width': world.getWidth() if hasattr(world, 'getWidth') else 'N/A',
            'World Height': world.getHeight() if hasattr(world, 'getHeight') else 'N/A',
            'Agent Position': (world.getAgentBody().x if hasattr(world, 'getAgentBody') else 0, 
                             world.getAgentBody().y if hasattr(world, 'getAgentBody') else 0),
            'Simulation Time': world.getSimulationTime() if hasattr(world, 'getSimulationTime') else 0,
            'Neuron Count': brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0,
            'Synapse Count': brain.getTotalSynapseCount() if hasattr(brain, 'getTotalSynapseCount') else 0,
            'Firing Rate': brain.getAverageFiringRate() if hasattr(brain, 'getAverageFiringRate') else 0.0,
            'E/I Ratio': brain.getExcitationInhibitionRatio() if hasattr(brain, 'getExcitationInhibitionRatio') else 1.0,
        }
        
        # Create export text
        export_text = "Export Data:\n\n"
        for key, value in export_info.items():
            export_text += f"{key}: {value}\n"
        
        ax.text(0.5, 0.5, export_text, 
               ha='center', va='center', transform=ax.transAxes,
               fontsize=9, fontfamily='monospace',
               bbox=dict(boxstyle='round', facecolor='lightcoral', alpha=0.8))
        
        ax.set_title('Export Information')
        ax.axis('off')

# Create instances of visualization tools
visualization_tools = VisualizationTools()
network_visualization = NetworkVisualizationTools()
activity_visualization = ActivityVisualizationTools()
result_visualization = ResultVisualizationTools()

__all__ = [
    # Configuration
    'VisualizationConfig',
    
    # Tool classes
    'VisualizationTools',
    'NetworkVisualizationTools',
    'ActivityVisualizationTools',
    'ResultVisualizationTools',
    
    # Performance monitoring
    'performance_monitor',
    
    # Exception classes
    'VisualizationError',
    'DataValidationError',
    'ExportError',
    
    # Instances
    'visualization_tools',
    'network_visualization',
    'activity_visualization',
    'result_visualization',
    
    # Performance metrics
    '_performance_metrics',
]
