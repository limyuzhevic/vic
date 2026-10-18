# Visualization and Analysis Tools for NLM
# Advanced visualization and analysis utilities for NLM

import numpy as np
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import json
import time
import os
from typing import Dict, List, Any, Optional, Tuple, Callable
from dataclasses import dataclass
from enum import Enum
import seaborn as sns
from scipy import signal, stats
import networkx as nx
import pandas as pd

# Set style for better visualizations
sns.set_style("whitegrid")
plt.rcParams['figure.figsize'] = (12, 8)
plt.rcParams['font.size'] = 10

class VisualizationType(Enum):
    """Types of visualizations supported by NLM."""
    
    NEURAL_ACTIVITY = "neural_activity"
    SPIKE_RATERS = "spike_rates"
    NETWORK_STRUCTURE = "network_structure"
    CONNECTION_WEIGHTS = "connection_weights"
    DEVELOPMENTAL_CHANGES = "developmental_changes"
    NEUROMODULATION = "neuromodulation"
    SPATIAL_ACTIVITY = "spatial_activity"
    TEMPORAL_PATTERNS = "temporal_patterns"
    STATISTICAL_ANALYSIS = "statistical_analysis"
    COMPARATIVE_ANALYSIS = "comparative_analysis"
class AnalysisResult:
    """Result of neural analysis."""
    
    def __init__(self, analysis_type: str):
        self.analysis_type = analysis_type
        self.results: Dict[str, Any] = {}
        self.metadata: Dict[str, Any] = {}
        self.timestamp = time.time()
    
    def add_result(self, key: str, value: Any):
        """Add a result to the analysis."""
        self.results[key] = value
    
    def add_metadata(self, key: str, value: Any):
        """Add metadata to the analysis."""
        self.metadata[key] = value
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert analysis result to dictionary."""
        return {
            'analysis_type': self.analysis_type,
            'results': self.results,
            'metadata': self.metadata,
            'timestamp': self.timestamp
        }
class NeuralActivityVisualizer:
    """
    Visualization of neural activity patterns.
    
    Provides comprehensive visualization of neural activity including
    spike trains, firing rates, network dynamics, and temporal patterns.
    """
    
    def __init__(self, style: str = "default"):
        self.style = style
        self.figure_size = (12, 8)
        self.color_scheme = self._get_color_scheme(style)
    
    def _get_color_scheme(self, style: str) -> Dict[str, str]:
        """Get color scheme based on style."""
        if style == "scientific":
            return {
                'excitatory': '#1f77b4',
                'inhibitory': '#d62728',
                'modulatory': '#2ca02c',
                'background': '#f8f8f8',
                'grid': '#cccccc'
            }
        elif style == "dark":
            return {
                'excitatory': '#4e79a7',
                'inhibitory': '#f28e2c',
                'modulatory': '#e15759',
                'background': '#1a1a1a',
                'grid': '#404040'
            }
        else:  # default
            return {
                'excitatory': '#1f77b4',
                'inhibitory': '#d62728',
                'modulatory': '#2ca02c',
                'background': '#ffffff',
                'grid': '#e0e0e0'
            }
    
    def plot_spike_trains(self, spike_data: Dict[int, List[float]], 
                         output_path: Optional[str] = None,
                         title: str = "Spike Train Visualization",
                         **kwargs) -> plt.Figure:
        """
        Plot spike trains for individual neurons.
        
        Args:
            spike_data: Dictionary mapping neuron IDs to spike times
            output_path: Path to save the plot
            title: Plot title
            **kwargs: Additional plotting parameters
            
        Returns:
            Matplotlib figure
        """
        fig, ax = plt.subplots(figsize=self.figure_size)
        
        # Determine time range
        all_spikes = []
        for spikes in spike_data.values():
            all_spikes.extend(spikes)
        
        if not all_spikes:
            print("Warning: No spike data provided")
            return fig
        
        t_min, t_max = min(all_spikes), max(all_spikes)
        time_window = max(t_max - t_min, 100.0)  # Ensure minimum window
        
        # Plot spike trains
        neurons = list(spike_data.keys())
        for i, neuron_id in enumerate(neurons):
            spikes = spike_data[neuron_id]
            if not spikes:
                continue
            
            # Normalize neuron positions to prevent overcrowding
            y_pos = (i + 1) * 10  # Space neurons vertically
            
            # Plot spike lines
            for spike_time in spikes:
                ax.axvline(x=spike_time, color=self.color_scheme['excitatory'],
                          alpha=0.7, linewidth=1)
                ax.axhline(y=y_pos, color=self.color_scheme['excitatory'],
                          alpha=0.3, linewidth=2)
        
        # Configure plot
        ax.set_xlabel('Time (ms)')
        ax.set_ylabel('Neuron ID')
        ax.set_title(title)
        ax.set_xlim(t_min, t_max + time_window * 0.1)
        ax.set_ylim(0, (len(neurons) + 1) * 10)
        ax.grid(True, alpha=0.3, linestyle='--')
        
        # Add legend
        from matplotlib.patches import Patch
        legend_elements = [
            Patch(facecolor=self.color_scheme['excitatory'], alpha=0.7, label='Spikes'),
            Patch(facecolor=self.color_scheme['inhibitory'], alpha=0.7, label='Neuron Traces')
        ]
        ax.legend(handles=legend_elements, loc='upper right')
        
        plt.tight_layout()
        
        if output_path:
            plt.savefig(output_path, dpi=300, bbox_inches='tight')
        
        return fig
    
    def plot_firing_rate_histogram(self, firing_rates: List[float],
                                   output_path: Optional[str] = None,
                                   title: str = "Firing Rate Distribution",
                                   **kwargs) -> plt.Figure:
        """
        Plot histogram of firing rates.
        
        Args:
            firing_rates: List of firing rates
            output_path: Path to save the plot
            title: Plot title
            **kwargs: Additional plotting parameters
            
        Returns:
            Matplotlib figure
        """
        fig, axes = plt.subplots(1, 2, figsize=(15, 6))
        
        # Original histogram
        axes[0].hist(firing_rates, bins=50, alpha=0.7, color=self.color_scheme['excitatory'],
                    edgecolor='black', linewidth=0.5)
        axes[0].set_xlabel('Firing Rate (Hz)')
        axes[0].set_ylabel('Count')
        axes[0].set_title(title + ' - Original Data')
        axes[0].grid(True, alpha=0.3, linestyle='--')
        
        # Normal distribution fit
        if len(firing_rates) > 10:
            mean_rate = np.mean(firing_rates)
            std_rate = np.std(firing_rates)
            
            # Create kernel density estimate
            kde = stats.gaussian_kde(firing_rates)
            x_range = np.linspace(min(firing_rates), max(firing_rates), 1000)
            kde_values = kde(x_range)
            
            axes[1].hist(firing_rates, bins=50, alpha=0.5, color=self.color_scheme['excitatory'],
                        density=True, edgecolor='black', linewidth=0.5)
            axes[1].plot(x_range, kde_values, 'r-', linewidth=2, 
                        label=f'Gaussian fit: μ={mean_rate:.2f}, σ={std_rate:.2f}')
            
            axes[1].set_xlabel('Firing Rate (Hz)')
            axes[1].set_ylabel('Density')
            axes[1].set_title(title + ' - With Distribution Fit')
            axes[1].legend()
            axes[1].grid(True, alpha=0.3, linestyle='--')
        
        plt.tight_layout()
        
        if output_path:
            plt.savefig(output_path, dpi=300, bbox_inches='tight')
        
        return fig
    
    def plot_activity_patterns(self, activity_matrix: np.ndarray,
                               output_path: Optional[str] = None,
                               title: str = "Activity Pattern Heatmap",
                               **kwargs) -> plt.Figure:
        """
        Plot activity patterns as heatmap.
        
        Args:
            activity_matrix: 2D array of activity values (time x neurons or time x regions)
            output_path: Path to save the plot
            title: Plot title
            **kwargs: Additional plotting parameters
            
        Returns:
            Matplotlib figure
        """
        fig, axes = plt.subplots(2, 2, figsize=(16, 12))
        
        # Original heatmap
        im1 = axes[0, 0].imshow(activity_matrix, aspect='auto', cmap='RdYlBu_r', 
                               interpolation='nearest', origin='lower')
        axes[0, 0].set_title('Original Activity Matrix')
        axes[0, 0].set_xlabel('Time Steps')
        axes[0, 0].set_ylabel('Neurons/Regions')
        plt.colorbar(im1, ax=axes[0, 0])
        
        # 2D Fourier transform (if enough data)
        if activity_matrix.shape[0] > 10:
            # Compute 2D FFT
            fft_result = np.fft.fft2(activity_matrix)
            fft_shifted = np.fft.fftshift(fft_result)
            magnitude_spectrum = np.abs(fft_shifted)
            
            # Show power spectrum
            im2 = axes[0, 1].imshow(np.log(magnitude_spectrum + 1), aspect='auto',
                                   interpolation='nearest', origin='lower')
            axes[0, 1].set_title('Frequency Spectrum (log scale)')
            axes[0, 1].set_xlabel('Frequency X')
            axes[0, 1].set_ylabel('Frequency Y')
            plt.colorbar(im2, ax=axes[0, 1])
        
        # Temporal evolution of mean activity
        mean_activity = np.mean(activity_matrix, axis=1)
        axes[1, 0].plot(mean_activity, color=self.color_scheme['excitatory'], 
                      linewidth=2, label='Mean Activity')
        axes[1, 0].set_title('Temporal Evolution of Mean Activity')
        axes[1, 0].set_xlabel('Time Step')
        axes[1, 0].set_ylabel('Activity')
        axes[1, 0].legend()
        axes[1, 0].grid(True, alpha=0.3, linestyle='--')
        
        # Distribution of activity values
        flat_activity = activity_matrix.flatten()
        axes[1, 1].hist(flat_activity, bins=50, alpha=0.7, 
                      color=self.color_scheme['modulatory'], density=True,
                      edgecolor='black', linewidth=0.5)
        
        # Add normal distribution fit
        if len(flat_activity) > 10:
            kde = stats.gaussian_kde(flat_activity)
            x_range = np.linspace(min(flat_activity), max(flat_activity), 1000)
            kde_values = kde(x_range)
            axes[1, 1].plot(x_range, kde_values, 'r-', linewidth=2,
                           label=f'Fit: μ={np.mean(flat_activity):.3f}, σ={np.std(flat_activity):.3f}')
        
        axes[1, 1].set_title('Distribution of Activity Values')
        axes[1, 1].set_xlabel('Activity Value')
        axes[1, 1].set_ylabel('Density')
        axes[1, 1].legend()
        axes[1, 1].grid(True, alpha=0.3, linestyle='--')
        
        plt.suptitle(title, fontsize=16)
        plt.tight_layout()
        
        if output_path:
            plt.savefig(output_path, dpi=300, bbox_inches='tight')
        
        return fig
    
    def create_activity_animation(self, spike_data: Dict[int, List[float]],
                                  output_path: Optional[str] = None,
                                  fps: int = 30,
                                  title: str = "Neural Activity Animation",
                                  **kwargs) -> Optional[FuncAnimation]:
        """
        Create animation of neural activity over time.
        
        Args:
            spike_data: Dictionary mapping neuron IDs to spike times
            output_path: Path to save the animation
            fps: Frames per second
            title: Animation title
            **kwargs: Additional animation parameters
            
        Returns:
            Matplotlib animation object (or None if creation fails)
        """
        try:
            # Get all spike times
            all_spikes = []
            for spikes in spike_data.values():
                all_spikes.extend(spikes)
            
            if not all_spikes:
                print("Warning: No spike data provided for animation")
                return None
            
            t_min, t_max = min(all_spikes), max(all_spikes)
            
            # Create figure
            fig, ax = plt.subplots(figsize=self.figure_size)
            
            # Create line plots for each neuron
            neurons = list(spike_data.keys())
            lines = []
            for i, neuron_id in enumerate(neurons):
                line, = ax.plot([], [], lw=2, color=self.color_scheme['excitatory'])
                lines.append(line)
            
            # Set up plot
            ax.set_xlim(t_min, t_max)
            ax.set_ylim(0, len(neurons) + 1)
            ax.set_xlabel('Time (ms)')
            ax.set_ylabel('Neuron ID')
            ax.set_title(title)
            ax.grid(True, alpha=0.3, linestyle='--')
            
            # Animation update function
            def update(frame):
                time_point = t_min + (t_max - t_min) * (frame / (500 - 1))  # 500 frames
                
                for i, (neuron_id, spikes) in enumerate(spike_data.items()):
                    # Find spikes within small time window around current frame
                    spike_at_frame = any(abs(spike - time_point) < 1.0 for spike in spikes)
                    lines[i].set_data([time_point], [i + 1] if spike_at_frame else [0])
                
                return lines
            
            # Create animation
            animation = FuncAnimation(fig, update, frames=500, interval=1000/fps,
                                    blit=True, repeat=True)
            
            if output_path:
                try:
                    animation.save(output_path, writer='ffmpeg', fps=fps)
                except Exception as e:
                    print(f"Warning: Could not save animation to {output_path}: {e}")
            
            return animation
            
        except Exception as e:
            print(f"Warning: Could not create animation: {e}")
            return None
class NetworkStructureVisualizer:
    """
    Visualization of network structure and connectivity.
    
    Provides comprehensive visualization of neural network structure,
    including connectivity patterns, graph theory metrics, and
    network topology analysis.
    """
    
    def __init__(self, style: str = "default"):
        self.style = style
        self.color_scheme = self._get_color_scheme(style)
    
    def _get_color_scheme(self, style: str) -> Dict[str, str]:
        """Get color scheme based on style."""
        if style == "dark":
            return {
                'excitatory': '#4e79a7',
                'inhibitory': '#f28e2c',
                'modulatory': '#e15759',
                'background': '#1a1a1a',
                'text': '#ffffff'
            }
        else:
            return {
                'excitatory': '#1f77b4',
                'inhibitory': '#d62728',
                'modulatory': '#2ca02c',
                'background': '#ffffff',
                'text': '#000000'
            }
    
    def plot_connectivity_matrix(self, adjacency_matrix: np.ndarray,
                                neuron_labels: Optional[List[str]] = None,
                                output_path: Optional[str] = None,
                                title: str = "Connectivity Matrix",
                                **kwargs) -> plt.Figure:
        """
        Plot connectivity matrix as heatmap.
        
        Args:
            adjacency_matrix: Adjacency matrix of connections
            neuron_labels: Labels for neurons
            output_path: Path to save the plot
            title: Plot title
            **kwargs: Additional plotting parameters
            
        Returns:
            Matplotlib figure
        """
        fig, ax = plt.subplots(figsize=(12, 10))
        
        # Plot heatmap
        im = ax.imshow(adjacency_matrix, cmap='RdYlBu_r', aspect='auto',
                      interpolation='nearest', origin='lower')
        
        # Configure plot
        ax.set_title(title)
        ax.set_xlabel('Target Neuron')
        ax.set_ylabel('Source Neuron')
        
        # Add colorbar
        plt.colorbar(im, ax=ax, label='Connection Weight')
        
        # Add neuron labels if provided
        if neuron_labels:
            n = len(neuron_labels)
            ax.set_xticks(np.arange(n))
            ax.set_yticks(np.arange(n))
            ax.set_xticklabels(neuron_labels, rotation=45, ha='right')
            ax.set_yticklabels(neuron_labels)
        
        plt.tight_layout()
        
        if output_path:
            plt.savefig(output_path, dpi=300, bbox_inches='tight')
        
        return fig
    
    def plot_network_graph(self, adjacency_matrix: np.ndarray,
                          neuron_types: Optional[List[str]] = None,
                          output_path: Optional[str] = None,
                          title: str = "Network Graph",
                          **kwargs) -> plt.Figure:
        """
        Plot network as graph using networkx.
        
        Args:
            adjacency_matrix: Adjacency matrix of connections
            neuron_types: Types of neurons (e.g., 'excitatory', 'inhibitory')
            output_path: Path to save the plot
            title: Plot title
            **kwargs: Additional plotting parameters
            
        Returns:
            Matplotlib figure
        """
        try:
            import networkx as nx
            
            # Create graph from adjacency matrix
            n = adjacency_matrix.shape[0]
            G = nx.Graph()
            
            # Add nodes
            for i in range(n):
                node_type = neuron_types[i] if neuron_types and i < len(neuron_types) else 'unknown'
                G.add_node(i, type=node_type)
            
            # Add edges
            for i in range(n):
                for j in range(n):
                    if adjacency_matrix[i, j] != 0:
                        G.add_edge(i, j, weight=adjacency_matrix[i, j])
            
            # Set up layout
            pos = nx.spring_layout(G, seed=42)
            
            # Create figure with multiple subplots
            fig, axes = plt.subplots(1, 2, figsize=(16, 7))
            
            # Basic graph visualization
            ax1 = axes[0]
            
            # Draw nodes with different colors based on type
            node_colors = []
            for node in G.nodes():
                node_type = G.nodes[node].get('type', 'unknown')
                if node_type == 'excitatory':
                    node_colors.append(self.color_scheme['excitatory'])
                elif node_type == 'inhibitory':
                    node_colors.append(self.color_scheme['inhibitory'])
                else:
                    node_colors.append(self.color_scheme['modulatory'])
            
            nx.draw_networkx_nodes(G, pos, ax=ax1, node_color=node_colors, 
                                  node_size=500, alpha=0.8)
            nx.draw_networkx_edges(G, pos, ax=ax1, alpha=0.5, width=1)
            nx.draw_networkx_labels(G, pos, ax=ax1, font_size=8)
            
            ax1.set_title("Network Structure")
            ax1.axis('off')
            
            # Graph theory metrics subplot
            ax2 = axes[1]
            
            # Calculate graph metrics
            metrics = {
                'Number of nodes': G.number_of_nodes(),
                'Number of edges': G.number_of_edges(),
                'Density': nx.density(G),
                'Average clustering coefficient': nx.average_clustering(G),
                'Global clustering coefficient': nx.transitivity(G),
                'Average shortest path length': nx.average_shortest_path_length(G) if nx.is_connected(G) else 'N/A',
                'Network diameter': nx.diameter(G) if nx.is_connected(G) else 'N/A'
            }
            
            # Plot metrics as bar chart
            metric_names = list(metrics.keys())
            metric_values = list(metrics.values())
            
            # Filter out 'N/A' values
            valid_indices = [i for i, v in enumerate(metric_values) if v != 'N/A']
            valid_names = [metric_names[i] for i in valid_indices]
            valid_values = [metric_values[i] for i in valid_indices]
            
            bars = ax2.bar(range(len(valid_names)), valid_values, alpha=0.7)
            ax2.set_xlabel('Graph Metrics')
            ax2.set_ylabel('Value')
            ax2.set_title('Graph Theory Metrics')
            ax2.set_xticks(range(len(valid_names)))
            ax2.set_xticklabels(valid_names, rotation=45, ha='right')
            ax2.set_ylim(0, max(valid_values) * 1.1 if valid_values else 1)
            
            # Add value labels on bars
            for bar, value in zip(bars, valid_values):
                height = bar.get_height()
                ax2.text(bar.get_x() + bar.get_width()/2., height,
                        f'{value:.3f}', ha='center', va='bottom')
            
            ax2.grid(True, alpha=0.3, linestyle='--')
            
            plt.suptitle(title, fontsize=14)
            plt.tight_layout()
            
            if output_path:
                plt.savefig(output_path, dpi=300, bbox_inches='tight')
            
            return fig
            
        except ImportError:
            print("Warning: networkx not installed, falling back to matrix visualization")
            return self.plot_connectivity_matrix(adjacency_matrix, output_path, title, **kwargs)
    
    def plot_degree_distribution(self, adjacency_matrix: np.ndarray,
                                 output_path: Optional[str] = None,
                                 title: str = "Degree Distribution",
                                 **kwargs) -> plt.Figure:
        """
        Plot degree distribution of network.
        
        Args:
            adjacency_matrix: Adjacency matrix of connections
            output_path: Path to save the plot
            title: Plot title
            **kwargs: Additional plotting parameters
            
        Returns:
            Matplotlib figure
        """
        try:
            import networkx as nx
            
            # Create graph
            n = adjacency_matrix.shape[0]
            G = nx.Graph()
            G.add_nodes_from(range(n))
            
            # Add edges based on adjacency matrix
            for i in range(n):
                for j in range(n):
                    if adjacency_matrix[i, j] != 0:
                        G.add_edge(i, j)
            
            # Calculate degree distribution
            degrees = [d for n, d in G.degree()]
            degree_counts = nx.degree_histogram(G)
            
            # Create figure with multiple subplots
            fig, axes = plt.subplots(1, 2, figsize=(14, 6))
            
            # Degree distribution histogram
            axes[0].hist(degrees, bins=len(set(degrees)), alpha=0.7,
                       color=self.color_scheme['excitatory'], edgecolor='black', linewidth=0.5)
            axes[0].set_xlabel('Degree')
            axes[0].set_ylabel('Count')
            axes[0].set_title('Degree Distribution')
            axes[0].grid(True, alpha=0.3, linestyle='--')
            
            # Plot degree counts
            if degree_counts:
                axes[1].bar(range(len(degree_counts)), degree_counts,
                           alpha=0.7, color=self.color_scheme['modulatory'])
                axes[1].set_xlabel('Degree')
                axes[1].set_ylabel('Count')
                axes[1].set_title('Degree Counts')
                axes[1].grid(True, alpha=0.3, linestyle='--')
            
            plt.suptitle(title, fontsize=14)
            plt.tight_layout()
            
            if output_path:
                plt.savefig(output_path, dpi=300, bbox_inches='tight')
            
            return fig
            
        except ImportError:
            print("Warning: networkx not installed")
            return self.plot_connectivity_matrix(adjacency_matrix, output_path, title, **kwargs)
class TemporalPatternAnalyzer:
    """
    Analysis of temporal patterns in neural activity.
    
    Provides tools for analyzing temporal dynamics including
    autocorrelation, power spectra, and pattern detection.
    """
    
    def __init__(self, sampling_rate: float = 1000.0):
        self.sampling_rate = sampling_rate  # Hz
        self.time_units = 'ms'
        if sampling_rate > 100:
            self.time_units = 'ms'
        else:
            self.time_units = 's'
    
    def analyze_autocorrelation(self, signal_data: np.ndarray, 
                               max_lag: Optional[int] = None,
                               output_path: Optional[str] = None,
                               title: str = "Autocorrelation Analysis",
                               **kwargs) -> Dict[str, Any]:
        """
        Analyze autocorrelation of neural signal.
        
        Args:
            signal_data: Time series data
            max_lag: Maximum lag for analysis
            output_path: Path to save results
            title: Plot title
            **kwargs: Additional parameters
            
        Returns:
            Dictionary with analysis results
        """
        analysis_result = {
            'signal_length': len(signal_data),
            'mean': np.mean(signal_data),
            'std': np.std(signal_data),
            'skewness': stats.skew(signal_data),
            'kurtosis': stats.kurtosis(signal_data)
        }
        
        # Calculate autocorrelation
        if max_lag is None:
            max_lag = min(1000, len(signal_data) // 2)
        
        autocorr = np.correlate(signal_data - np.mean(signal_data), 
                               signal_data - np.mean(signal_data), mode='full')
        autocorr = autocorr[len(autocorr)//2:len(autocorr)//2 + max_lag]
        autocorr = autocorr / autocorr[0]  # Normalize
        
        analysis_result['autocorrelation'] = autocorr
        analysis_result['autocorrelation_lags'] = np.arange(max_lag)
        
        # Calculate autocorrelation metrics
        analysis_result['autocorrelation_metrics'] = {
            'zero_lag': autocorr[0],
            'half_life': self._find_autocorrelation_half_life(autocorr),
            'decay_constant': self._estimate_decay_constant(autocorr),
            'effective_memory': self._estimate_effective_memory(autocorr)
        }
        
        # Create visualization
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        
        # Signal plot
        time_axis = np.arange(len(signal_data)) / self.sampling_rate * 1000
        axes[0, 0].plot(time_axis, signal_data, color=self.color_scheme['excitatory'],
                       linewidth=1, alpha=0.8)
        axes[0, 0].set_xlabel(f'Time ({self.time_units})')
        axes[0, 0"title('Original Signal')
        axes[0, 0].grid(True, alpha=0.3, linestyle='--')
        
        # Autocorrelation plot
        axes[0, 1].plot(analysis_result['autocorrelation_lags'], autocorr,
                       color=self.color_scheme['inhibitory'], linewidth=2)
        axes[0, 1].set_xlabel('Lag')
        axes[0, 1].set_ylabel('Autocorrelation')
        axes[0, 1].set_title('Autocorrelation Function')
        axes[0, 1].grid(True, alpha=0.3, linestyle='--')
        axes[0, 1].set_xlim(0, len(autocorr) - 1)
        
        # Histogram of signal values
        axes[1, 0].hist(signal_data, bins=50, alpha=0.7, color=self.color_scheme['modulatory'],
                       density=True, edgecolor='black', linewidth=0.5)
        axes[1, 0].set_xlabel('Signal Value')
        axes[1, 0].set_ylabel('Density')
        axes[1, 0].set_title('Distribution of Signal Values')
        axes[1, 0].grid(True, alpha=0.3, linestyle='--')
        
        # Probability density function
        from scipy import stats
        xmin, xmax = axes[1, 0].get_xlim()
        x = np.linspace(xmin, xmax, 100)
        pdf = stats.norm.pdf(x, np.mean(signal_data), np.std(signal_data))
        axes[1, 0].plot(x, pdf, 'k', linewidth=2, label='Normal Distribution')
        axes[1, 0].legend()
        
        plt.suptitle(title, fontsize=14)
        plt.tight_layout()
        
        if output_path:
            plt.savefig(output_path, dpi=300, bbox_inches='tight')
        
        # Save results
        if output_path:
            results_file = output_path.replace('.png', '.json')
            with open(results_file, 'w') as f:
                json.dump(analysis_result, f, indent=2, default=str)
        
        return analysis_result
    
    def _find_autocorrelation_half_life(self, autocorr: np.ndarray) -> int:
        """Find half-life of autocorrelation."""
        for i, corr in enumerate(autocorr[1:], 1):
            if abs(corr) <= 0.5:  # Half-life: when correlation drops to 0.5
                return i
        return len(autocorr) - 1
    
    def _estimate_decay_constant(self, autocorr: np.ndarray) -> float:
        """Estimate decay constant from autocorrelation."""
        # Fit exponential decay: exp(-t/tau)
        lags = np.arange(len(autocorr))
        
        # Use linear regression on log autocorrelation
        valid_indices = np.where(autocorr > 0)[0]
        if len(valid_indices) < 2:
            return float('inf')
        
        log_corr = np.log(autocorr[valid_indices])
        valid_lags = lags[valid_indices]
        
        # Linear regression: log(r) = -t/tau + const
        coeffs = np.polyfit(valid_lags, log_corr, 1)
        decay_constant = -1.0 / coeffs[0] if coeffs[0] < 0 else float('inf')
        
        return decay_constant
    
    def _estimate_effective_memory(self, autocorr: np.ndarray) -> float:
        """Estimate effective memory from autocorrelation."""
        # Effective memory: sum of autocorrelation over time
        effective_memory = np.sum(np.abs(autocorr))
        return effective_memory
    
    def analyze_power_spectrum(self, signal_data: np.ndarray,
                              output_path: Optional[str] = None,
                              title: str = "Power Spectrum Analysis",
                              **kwargs) -> Dict[str, Any]:
        """
        Analyze power spectrum of neural signal.
        
        Args:
            signal_data: Time series data
            output_path: Path to save results
            title: Plot title
            **kwargs: Additional parameters
            
        Returns:
            Dictionary with analysis results
        """
        analysis_result = {}
        
        # Compute power spectrum using Welch's method
        frequencies, psd = signal.welch(signal_data, fs=self.sampling_rate, 
                                      nperseg=min(256, len(signal_data) // 4))
        
        analysis_result['frequencies'] = frequencies
        analysis_result['power_spectrum'] = psd
        
        # Calculate spectral metrics
        analysis_result['spectral_metrics'] = {
            'mean_power': np.mean(psd),
            'peak_frequency': frequencies[np.argmax(psd)],
            'mean_frequency': np.sum(frequencies * psd) / np.sum(psd),
            'bandwidth': self._calculate_bandwidth(frequencies, psd),
            'quality_factor': self._calculate_quality_factor(frequencies, psd)
        }
        
        # Create visualization
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        
        # Signal plot
        time_axis = np.arange(len(signal_data)) / self.sampling_rate * 1000
        axes[0, 0].plot(time_axis, signal_data, color=self.color_scheme['excitatory'],
                       linewidth=1, alpha=0.8)
        axes[0, 0].set_xlabel(f'Time ({self.time_units})')
        axes[0, 0].set_ylabel('Amplitude')
        axes[0, 0].set_title('Original Signal')
        axes[0, 0].grid(True, alpha=0.3, linestyle='--')
        
        # Power spectrum plot
        axes[0, 1].semilogy(frequencies, psd, color=self.color_scheme['inhibitory'],
                          linewidth=2)
        axes[0, 1].set_xlabel('Frequency (Hz)')
        axes[0, 1].set_ylabel('Power (dB/Hz)')
        axes[0, 1].set_title('Power Spectrum')
        axes[0, 1].grid(True, alpha=0.3, linestyle='--')
        
        # Lomb-Scargle periodogram for unevenly sampled data
        if len(np.unique(np.diff(time_axis))) > 1:  # Non-uniform sampling
            from astropy.timeseries import LombScargle
            
            # Prepare data for Lomb-Scargle
            y = signal_data
            t = time_axis
            
            # Calculate periodogram
            frequency, power = LombScargle(t, y).autopower()
            
            axes[1, 0].plot(frequency, power, color=self.color_scheme['modulatory'],
                           linewidth=2)
            axes[1, 0].set_xlabel('Frequency (Hz)')
            axes[1, 0].set_ylabel('Power')
            axes[1, 0].set_title('Lomb-Scargle Periodogram')
            axes[1, 0].grid(True, alpha=0.3, linestyle='--')
        
        # Wavelet transform
        try:
            from scipy.signal import cwt, ricker
            
            widths = np.arange(1, 31)
            cwtmatr = cwt(signal_data, ricker, widths)
            
            axes[1, 1].imshow(cwtmatr, extent=[0, len(signal_data), 1, 31],
                            aspect='auto', cmap='RdBu_r', interpolation='bilinear')
            axes[1, 1].set_xlabel('Time Index')
            axes[1, 1].set_ylabel('Scale (Frequency)')
            axes[1, 1].set_title('Continuous Wavelet Transform')
            
        except ImportError:
            axes[1, 1].text(0.5, 0.5, 'Wavelet transform not available',
                          ha='center', va='center', transform=axes[1, 1].transAxes)
        
        plt.suptitle(title, fontsize=14)
        plt.tight_layout()
        
        if output_path:
            plt.savefig(output_path, dpi=300, bbox_inches='tight')
        
        # Save results
        if output_path:
            results_file = output_path.replace('.png', '.json')
            with open(results_file, 'w') as f:
                json.dump(analysis_result, f, indent=2, default=str)
        
        return analysis_result
    
    def _calculate_bandwidth(self, frequencies: np.ndarray, psd: np.ndarray) -> float:
        """Calculate bandwidth of power spectrum."""
        # Full width at half maximum
        peak_power = np.max(psd)
        half_max = peak_power / 2
        
        above_half_max = psd >= half_max
        if np.any(above_half_max):
            freq_indices = np.where(above_half_max)[0]
            bandwidth = frequencies[freq_indices[-1]] - frequencies[freq_indices[0]]
        else:
            bandwidth = 0.0
        
        return bandwidth
    
    def _calculate_quality_factor(self, frequencies: np.ndarray, psd: np.ndarray) -> float:
        """Calculate quality factor (Q) of power spectrum."""
        peak_frequency = frequencies[np.argmax(psd)]
        half_max = np.max(psd) / 2
        
        # Find frequencies at half maximum
        above_half_max = psd >= half_max
        if np.any(above_half_max):
            freq_indices = np.where(above_half_max)[0]
            f_full_width = frequencies[freq_indices[-1]] - frequencies[freq_indices[0]]
            quality_factor = peak_frequency / f_full_width if f_full_width > 0 else float('inf')
        else:
            quality_factor = 0.0
        
        return quality_factor
class ComparativeAnalyzer:
    """
    Comparative analysis between different neural network configurations.
    
    Provides tools for comparing different simulation results,
    parameter variations, and experimental conditions.
    """
    
    def __init__(self):
        self.comparison_results = []
    
    def add_comparison_result(self, name: str, results: Dict[str, Any],
                            parameters: Dict[str, Any]) -> None:
        """
        Add a comparison result.
        
        Args:
            name: Name of the comparison
            results: Results to compare
            parameters: Parameters used in the experiment
        """
        comparison = {
            'name': name,
            'results': results,
            'parameters': parameters,
            'timestamp': time.time()
        }
        
        self.comparison_results.append(comparison)
    
    def create_parameter_sensitivity_analysis(self, param_name: str,
                                             param_values: List[Any],
                                             baseline_results: Dict[str, Any],
                                             output_path: Optional[str] = None,
                                             title: str = "Parameter Sensitivity Analysis",
                                             **kwargs) -> Dict[str, Any]:
        """
        Create parameter sensitivity analysis.
        
        Args:
            param_name: Parameter name to vary
            param_values: Values to test
            baseline_results: Baseline results
            output_path: Path to save results
            title: Plot title
            **kwargs: Additional parameters
            
        Returns:
            Dictionary with analysis results
        """
        analysis_result = {
            'param_name': param_name,
            'param_values': param_values,
            'baseline_results': baseline_results,
            'sensitivity_indices': []
        }
        
        # Create comparison plots
        fig, axes = plt.subplots(2, 2, figsize=(14, 12))
        
        # Parameter vs metric 1
        param_values_array = np.array(param_values)
        metric_1_values = [r.get('metric_1', 0) for r in baseline_results]
        
        axes[0, 0].plot(param_values_array, metric_1_values, 'o-', linewidth=2,
                       markersize=8, color=self.color_scheme['excitatory'])
        axes[0, 0].set_xlabel(param_name)
        axes[0, 0].set_ylabel('Metric 1')
        axes[0, 0].set_title(f'{param_name} vs Metric 1')
        axes[0, 0].grid(True, alpha=0.3, linestyle='--')
        
        # Parameter vs metric 2
        metric_2_values = [r.get('metric_2', 0) for r in baseline_results]
        
        axes[0, 1].plot(param_values_array, metric_2_values, 'o-', linewidth=2,
                       markersize=8, color=self.color_scheme['inhibitory'])
        axes[0, 1].set_xlabel(param_name)
        axes[0, 1].set_ylabel('Metric 2')
        axes[0, 1].set_title(f'{param_name} vs Metric 2')
        axes[0, 1].grid(True, alpha=0.3, linestyle='--')
        
        # Sensitivity index (normalized derivative)
        sensitivity_index = []
        for i, value in enumerate(param_values):
            if i > 0 and i < len(param_values) - 1:
                # Central difference approximation
                d_metric1 = (metric_1_values[i+1] - metric_1_values[i-1]) / (param_values_array[i+1] - param_values_array[i-1])
                sensitivity_index.append(abs(d_metric1))
            else:
                sensitivity_index.append(0)
        
        axes[1, 0].plot(param_values_array, sensitivity_index, 's-', 
                       linewidth=2, markersize=8, color=self.color_scheme['modulatory'])
        axes[1, 0].set_xlabel(param_name)
        axes[1, 0].set_ylabel('Sensitivity Index')
        axes[1, 0].set_title(f'Sensitivity Analysis')
        axes[1, 0].grid(True, alpha=0.3, linestyle='--')
        
        # Contour plot for two parameters (if we have enough data)
        if len(param_values) >= 3:
            # Create a simple 2D parameter space visualization
            # This is a simplified version - in practice you'd need more systematic parameter sweeps
            X, Y = np.meshgrid(param_values_array, param_values_array)
            
            # Dummy sensitivity surface for demonstration
            Z = np.zeros_like(X)
            for i in range(len(param_values_array)):
                for j in range(len(param_values_array)):
                    # Calculate some sensitivity measure
                    idx1 = i if i < len(metric_1_values) else 0
                    idx2 = j if j < len(metric_2_values) else 0
                    Z[j, i] = (metric_1_values[idx1] + metric_2_values[idx2]) / 2
            
            # Normalize Z
            Z_norm = (Z - np.min(Z)) / (np.max(Z) - np.min(Z) + 1e-8)
            
            contour = axes[1, 1].contourf(X, Y, Z_norm, levels=20, cmap='RdYlBu_r')
            axes[1, 1].set_xlabel(param_name)
            axes[1, 1].set_ylabel(param_name)
            axes[1, 1].set_title('Parameter Space Sensitivity')
            axes[1, 1].grid(True, alpha=0.3, linestyle='--')
            plt.colorbar(contour, ax=axes[1, 1])
        
        plt.suptitle(title, fontsize=14)
        plt.tight_layout()
        
        if output_path:
            plt.savefig(output_path, dpi=300, bbox_inches='tight')
        
        analysis_result['sensitivity_indices'] = sensitivity_index
        
        # Save results
        if output_path:
            results_file = output_path.replace('.png', '.json')
            with open(results_file, 'w') as f:
                json.dump(analysis_result, f, indent=2, default=str)
        
        return analysis_result
    
    def create_result_comparison(self, result_groups: List[Dict[str, Any]],
                                metrics: List[str],
                                output_path: Optional[str] = None,
                                title: str = "Result Comparison",
                                **kwargs) -> Dict[str, Any]:
        """
        Create comparison between different result groups.
        
        Args:
            result_groups: List of result groups to compare
            metrics: List of metrics to compare
            output_path: Path to save results
            title: Plot title
            **kwargs: Additional parameters
            
        Returns:
            Dictionary with comparison results
        """
        comparison_result = {
            'result_groups': result_groups,
            'metrics': metrics,
            'comparison_summary': {}
        }
        
        # Create box plot comparison
        fig, axes = plt.subplots(1, len(metrics), figsize=(5 * len(metrics), 6))
        
        if len(metrics) == 1:
            axes = [axes]
        
        for i, metric in enumerate(metrics):
            ax = axes[i]
            
            # Collect metric values from all groups
            metric_values = []
            group_names = []
            
            for group in result_groups:
                if metric in group['results']:
                    if isinstance(group['results'][metric], list):
                        values = group['results'][metric]
                    else:
                        values = [group['results'][metric]]
                    
                    metric_values.extend(values)
                    group_names.extend([group['name']] * len(values))
            
            # Create box plot
            if metric_values:
                boxes = ax.boxplot(metric_values, labels=group_names, patch_artist=True)
                
                # Color the boxes based on group
                colors = [self.color_scheme['excitatory'], self.color_scheme['inhibitory'],
                         self.color_scheme['modulatory']][:len(group_names)]
                
                for box, color in zip(boxes['boxes'], colors):
                    box.set_facecolor(color)
                    box.set_alpha(0.7)
                
                ax.set_title(f'{metric} Distribution')
                ax.set_ylabel(metric)
                ax.grid(True, alpha=0.3, linestyle='--')
            else:
                ax.text(0.5, 0.5, f'No data for metric: {metric}',
                       ha='center', va='center', transform=ax.transAxes)
                ax.set_title(f'{metric} Distribution')
        
        plt.suptitle(title, fontsize=14)
        plt.tight_layout()
        
        if output_path:
            plt.savefig(output_path, dpi=300, bbox_inches='tight')
        
        # Calculate comparison statistics
        comparison_stats = {}
        for metric in metrics:
            metric_values_by_group = {}
            
            for group in result_groups:
                if metric in group['results']:
                    if isinstance(group['results'][metric], list):
                        metric_values_by_group[group['name']] = group['results'][metric]
                    else:
                        metric_values_by_group[group['name']] = [group['results'][metric]]
            
            # Calculate statistics for each group
            group_stats = {}
            for group_name, values in metric_values_by_group.items():
                if values:
                    group_stats[group_name] = {
                        'mean': np.mean(values),
                        'std': np.std(values),
                        'min': np.min(values),
                        'max': np.max(values),
                        'median': np.median(values),
                        'count': len(values)
                    }
            
            comparison_stats[metric] = group_stats
        
        comparison_result['comparison_summary'] = comparison_stats
        
        return comparison_result
class VisualizationExporter:
    """
    Export visualization results to various formats.
    
    Provides functionality to export visualization results to different
    formats including images, JSON, and other file formats for
    integration with other tools.
    """
    
    @staticmethod
    def export_to_json(data: Dict[str, Any], output_path: str) -> bool:
        """
        Export data to JSON file.
        
        Args:
            data: Data to export
            output_path: Path to output file
            
        Returns:
            True if export succeeded, False otherwise
        """
        try:
            with open(output_path, 'w') as f:
                json.dump(data, f, indent=2, default=str)
            return True
        except Exception as e:
            print(f"Warning: Could not export to JSON {output_path}: {e}")
            return False
    
    @staticmethod
    def export_to_csv(data: Dict[str, Any], output_path: str) -> bool:
        """
        Export data to CSV file.
        
        Args:
            data: Data to export
            output_path: Path to output file
            
        Returns:
            True if export succeeded, False otherwise
        """
        try:
            # Convert data to pandas DataFrame if possible
            import pandas as pd
            
            # Flatten data structure for CSV export
            flattened_data = []
            
            def flatten_dict(d: Dict[str, Any], prefix: str = "") -> List[Dict[str, Any]]:
                rows = []
                for key, value in d.items():
                    full_key = f"{prefix}_{key}" if prefix else key
                    if isinstance(value, dict):
                        rows.extend(flatten_dict(value, full_key))
                    elif isinstance(value, list):
                        for i, item in enumerate(value):
                            if isinstance(item, dict):
                                rows.extend(flatten_dict(item, f"{full_key}_{i}"))
                            else:
                                rows.append({full_key: item})
                    else:
                        rows.append({full_key: value})
                return rows
            
            rows = flatten_dict(data)
            df = pd.DataFrame(rows)
            df.to_csv(output_path, index=False)
            
            return True
            
        except Exception as e:
            print(f"Warning: Could not export to CSV {output_path}: {e}")
            return False
    
    @staticmethod
    def export_analysis_results(analysis_result: Dict[str, Any],
                               output_path: str,
                               formats: List[str] = None) -> Dict[str, bool]:
        """
        Export analysis results to multiple formats.
        
        Args:
            analysis_result: Analysis results to export
            output_path: Base path for output files
            formats: List of formats to export (json, csv, etc.)
            
        Returns:
            Dictionary with export status for each format
        """
        if formats is None:
            formats = ['json', 'csv']
        
        export_status = {}
        
        for format_name in formats:
            if format_name.lower() == 'json':
                json_path = output_path.replace('.', '_analysis.json')
                export_status['json'] = VisualizationExporter.export_to_json(analysis_result, json_path)
            
            elif format_name.lower() == 'csv':
                csv_path = output_path.replace('.', '_analysis.csv')
                export_status['csv'] = VisualizationExporter.export_to_csv(analysis_result, csv_path)
            
            elif format_name.lower() == 'png':
                # For PNG, we'd need the actual plot object
                export_status['png'] = False  # Placeholder
            
            else:
                export_status[format_name] = False  # Unknown format
        
        return export_status
# Main visualization class that combines all functionality
class NLMVisualizer:
    """
    Comprehensive visualization system for NLM.
    
    This class provides a unified interface for all visualization and analysis
    functionality for NLM simulations, including neural activity visualization,
    network structure analysis, temporal pattern analysis, and comparative analysis.
    """
    
    def __init__(self, style: str = "default", sampling_rate: float = 1000.0):
        self.visualizer = NeuralActivityVisualizer(style)
        self.structure_visualizer = NetworkStructureVisualizer(style)
        self.temporal_analyzer = TemporalPatternAnalyzer(sampling_rate)
        self.comparative_analyzer = ComparativeAnalyzer()
        self.exporter = VisualizationExporter()
        
        # Set color scheme for all components
        self.color_scheme = self.visualizer.color_scheme
    
    def visualize_neural_activity(self, spike_data: Dict[int, List[float]],
                                 output_path: Optional[str] = None,
                                 title: str = "Neural Activity Visualization",
                                 **kwargs) -> Dict[str, Any]:
        """
        Visualize neural activity.
        
        Args:
            spike_data: Spike data for visualization
            output_path: Path to save visualization
            title: Title for visualization
            **kwargs: Additional parameters
            
        Returns:
            Dictionary with visualization results
        """
        result = {
            'analysis_type': VisualizationType.NEURAL_ACTIVITY.value,
            'timestamp': time.time()
        }
        
        # Create spike train visualization
        spike_fig = self.visualizer.plot_spike_trains(spike_data, output_path,
                                                    title + " - Spike Trains", **kwargs)
        
        result['spike_trains'] = {
            'figure_type': 'spike_trains',
            'has_data': len(spike_data) > 0,
            'neuron_count': len(spike_data)
        }
        
        if output_path:
            result['output_files'] = {
                'spike_trains': output_path
            }
        
        return result
    
    def visualize_network_structure(self, adjacency_matrix: np.ndarray,
                                   neuron_types: Optional[List[str]] = None,
                                   output_path: Optional[str] = None,
                                   title: str = "Network Structure Visualization",
                                   **kwargs) -> Dict[str, Any]:
        """
        Visualize network structure.
        
        Args:
            adjacency_matrix: Adjacency matrix of network
            neuron_types: Types of neurons
            output_path: Path to save visualization
            title: Title for visualization
            **kwargs: Additional parameters
            
        Returns:
            Dictionary with visualization results
        """
        result = {
            'analysis_type': VisualizationType.NETWORK_STRUCTURE.value,
            'timestamp': time.time()
        }
        
        # Create network graph visualization
        graph_fig = self.structure_visualizer.plot_network_graph(
            adjacency_matrix, neuron_types, output_path, title, **kwargs)
        
        result['network_structure'] = {
            'figure_type': 'network_graph',
            'adjacency_matrix_shape': adjacency_matrix.shape,
           n('neuron_count', adjacency_matrix.shape[0]),
            'edge_density': np.count_nonzero(adjacency_matrix) / (adjacency_matrix.shape[0] ** 2)
        }
        
        if output_path:
            result['output_files'] = {
                'network_structure': output_path
            }
        
        return result
    
    def analyze_temporal_patterns(self, signal_data: np.ndarray,
                                 output_path: Optional[str] = None,
                                 title: str = "Temporal Pattern Analysis",
                                 **kwargs) -> Dict[str, Any]:
        """
        Analyze temporal patterns in signal data.
        
        Args:
            signal_data: Signal data for analysis
            output_path: Path to save results
            title: Title for analysis
            **kwargs: Additional parameters
            
        Returns:
            Dictionary with analysis results
        """
        result = {
            'analysis_type': VisualizationType.TEMPORAL_PATTERNS.value,
            'timestamp': time.time()
        }
        
        # Perform autocorrelation analysis
        autocorr_result = self.temporal_analyzer.analyze_autocorrelation(
            signal_data, output_path, title + " - Autocorrelation", **kwargs)
        
        result['autocorrelation_analysis'] = autocorr_result
        
        if output_path:
            result['output_files'] = {
                'autocorrelation': output_path.replace('.', '_autocorr.')
            }
        
        return result
    
    def export_results(self, data: Dict[str, Any], output_path: str,
                      formats: List[str] = None) -> Dict[str, bool]:
        """
        Export visualization results to multiple formats.
        
        Args:
            data: Data to export
            output_path: Base path for output files
            formats: List of formats to export
            
        Returns:
            Dictionary with export status for each format
        """
        if formats is None:
            formats = ['json', 'csv']
        
        return self.exporter.export_analysis_results(data, output_path, formats)
    
    def create_comprehensive_report(self, all_results: Dict[str, Any],
                                   output_path: str,
                                   title: str = "Comprehensive NLM Analysis Report") -> Dict[str, Any]:
        """
        Create a comprehensive analysis report.
        
        Args:
            all_results: All analysis results
            output_path: Path to save report
            title: Title for report
            
        Returns:
            Dictionary with report information
        """
        report = {
            'report_title': title,
            'generation_timestamp': time.time(),
            'results_summary': {},
            'visualizations': [],
            'analyses': []
        }
        
        # Summarize results
        for analysis_type, result in all_results.items():
            report['results_summary'][analysis_type] = {
                'has_data': bool(result.get('has_data', False)),
                'output_files': result.get('output_files', {}),
                'timestamp': result.get('timestamp', time.time())
            }
            
            # Add to visualizations/analyses list
            if 'figure_type' in str(result):
                report['visualizations'].append({
                    'type': analysis_type,
                    'figure_type': result.get('figure_type', 'unknown'),
                    'output_files': result.get('output_files', {})
                })
            else:
                report['analyses'].append({
                    'type': analysis_type,
                    'results': result
                })
        
        # Export report
        report_file = output_path.replace('.json', '_report.json')
        self.exporter.export_to_json(report, report_file)
        
        report['report_file'] = report_file
        
        return report
# Utility functions for common visualization tasks

def create_spike_train_plot(spike_data: Dict[int, List[float]],
                           output_path: Optional[str] = None,
                           **kwargs) -> plt.Figure:
    """Create spike train plot from spike data."""
    visualizer = NeuralActivityVisualizer()
    return visualizer.plot_spike_trains(spike_data, output_path, **kwargs)

def create_activity_heatmap(activity_matrix: np.ndarray,
                            output_path: Optional[str] = None,
                            **kwargs) -> plt.Figure:
    """Create activity heatmap from activity matrix."""
    visualizer = NeuralActivityVisualizer()
    return visualizer.plot_activity_patterns(activity_matrix, output_path, **kwargs)

def create_network_graph(adjacency_matrix: np.ndarray,
                        neuron_types: Optional[List[str]] = None,
                        output_path: Optional[str] = None,
                        **kwargs) -> plt.Figure:
    """Create network graph visualization."""
    visualizer = NetworkStructureVisualizer()
    return visualizer.plot_network_graph(adjacency_matrix, neuron_types, output_path, **kwargs)

def analyze_signal_autocorrelation(signal_data: np.ndarray,
                                 output_path: Optional[str] = None,
                                 **kwargs) -> Dict[str, Any]:
    """Analyze autocorrelation of signal data."""
    analyzer = TemporalPatternAnalyzer()
    return analyzer.analyze_autocorrelation(signal_data, output_path, **kwargs)
# Export key classes and functions
__all__ = [
    'VisualizationType',
    'AnalysisResult',
    'NeuralActivityVisualizer',
    'NetworkStructureVisualizer',
    'TemporalPatternAnalyzer',
    'ComparativeAnalyzer',
    'VisualizationExporter',
    'NLMVisualizer',
    'create_spike_train_plot',
    'create_activity_heatmap',
    'create_network_graph',
    'analyze_signal_autocorrelation'
]
