"""Visualization and analysis for NLM Python API."""

import numpy as np
from typing import Dict, List, Optional, Any, Tuple
from collections import deque
import time
from pathlib import Path
import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib.animation import FuncAnimation
import seaborn as sns
from scipy import stats


class Plotter:
    """
    Neural activity visualization.
    
    This class provides comprehensive visualization capabilities for NLM data,
    including neural activity plots, connectivity graphs, and training curves.
    """
    
    def __init__(self, style: str = "default", figure_size: Tuple[int, int] = (12, 8)):
        """
        Initialize plotter.
        
        Args:
            style: Matplotlib style ("default", "dark", "scientific", "retro")
            figure_size: Default figure size (width, height)
        """
        self.style = style
        self.figure_size = figure_size
        
        # Configure matplotlib
        plt.style.use(style.lower() if style in ["dark", "scientific", "retro"] else "default")
        
        # Set up colormap
        self.cmap = plt.cm.viridis
        
        # Plotting history
        self.plot_history = deque(maxlen=100)
        
    def plot_neural_activity(self, activity_data: np.ndarray, 
                            title: str = "Neural Activity", 
                            save_path: str = None) -> plt.Figure:
        """
        Plot neural activity over time.
        
        Args:
            activity_data: Neural activity data (neurons x time)
            title: Plot title
            save_path: Path to save plot (if None, not saved)
            
        Returns:
            plt.Figure: Created figure
        """
        fig, axes = plt.subplots(2, 2, figsize=self.figure_size)
        fig.suptitle(title, fontsize=16, fontweight='bold')
        
        # Time series of total activity
        total_activity = np.sum(activity_data, axis=0)
        axes[0, 0].plot(total_activity)
        axes[0, 0].set_title("Total Activity Over Time")
        axes[0, 0].set_xlabel("Time Step")
        axes[0, 0].set_ylabel("Total Activity")
        axes[0, 0].grid(True, alpha=0.3)
        
        # Activity distribution
        axes[0, 1].hist(total_activity, bins=50, alpha=0.7, density=True)
        axes[0, 1].set_title("Activity Distribution")
        axes[0, 1].set_xlabel("Activity Level")
        axes[0, 1].set_ylabel("Density")
        
        # Neuron firing rates
        firing_rates = np.mean(activity_data, axis=1)
        axes[1, 0].bar(range(len(firing_rates)), firing_rates, alpha=0.7)
        axes[1, 0].set_title("Neuron Firing Rates")
        axes[1, 0].set_xlabel("Neuron Index")
        axes[1, 0].set_ylabel("Firing Rate")
        axes[1, 0].grid(True, alpha=0.3)
        
        # Raster plot
        spike_times = []
        spike_neurons = []
        
        for neuron in range(activity_data.shape[0]):
            spike_indices = np.where(activity_data[neuron] > 0)[0]
            spike_times.extend(spike_indices)
            spike_neurons.extend([neuron] * len(spike_indices))
            
        if spike_times:
            axes[1, 1].scatter(spike_times, spike_neurons, alpha=0.5, s=1)
            axes[1, 1].set_title("Spike Raster Plot")
            axes[1, 1].set_xlabel("Time Step")
            axes[1, 1].set_ylabel("Neuron Index")
            axes[1, 1].set_xlim(0, activity_data.shape[1])
            axes[1, 1].set_ylim(-1, activity_data.shape[0] - 1)
            
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            
        return fig
        
    def plot_connectivity_matrix(self, connectivity_data: np.ndarray,
                                title: str = "Connectivity Matrix",
                                save_path: str = None) -> plt.Figure:
        """
        Plot connectivity matrix.
        
        Args:
            connectivity_data: Connectivity matrix (neurons x neurons)
            title: Plot title
            save_path: Path to save plot (if None, not saved)
            
        Returns:
            plt.Figure: Created figure
        """
        fig, axes = plt.subplots(1, 2, figsize=self.figure_size)
        fig.suptitle(title, fontsize=16, fontweight='bold')
        
        # Connectivity matrix heatmap
        im1 = axes[0].imshow(connectivity_data, cmap=self.cmap, interpolation='nearest')
        axes[0].set_title("Connectivity Matrix")
        axes[0].set_xlabel("Target Neuron")
        axes[0].set_ylabel("Source Neuron")
        plt.colorbar(im1, ax=axes[0])
        
        # Connectivity distribution
        flat_connectivity = connectivity_data.flatten()
        axes[1].hist(flat_connectivity, bins=50, alpha=0.7, density=True)
        axes[1].set_title("Connectivity Distribution")
        axes[1].set_xlabel("Connection Strength")
        axes[1].set_ylabel("Density")
        axes[1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            
        return fig
        
    def plot_training_curve(self, training_metrics: Dict[str, List[float]],
                           title: str = "Training Curves",
                           save_path: str = None) -> plt.Figure:
        """
        Plot training curves.
        
        Args:
            training_metrics: Dictionary of metric names and values over time
            title: Plot title
            save_path: Path to save plot (if None, not saved)
            
        Returns:
            plt.Figure: Created figure
        """
        fig, axes = plt.subplots(2, 2, figsize=self.figure_size)
        fig.suptitle(title, fontsize=16, fontweight='bold')
        
        # Plot each metric
        for i, (metric_name, values) in enumerate(training_metrics.items()):
            if i < 4:  # Limit to 4 subplots
                row = i // 2
                col = i % 2
                
                axes[row, col].plot(values)
                axes[row, col].set_title(metric_name.replace('_', ' ').title())
                axes[row, col].set_xlabel("Training Step")
                axes[row, col].set_ylabel("Value")
                axes[row, col].grid(True, alpha=0.3)
                
                # Add final value annotation
                if values:
                    axes[row, col].annotate(f"{values[-1]:.4f}", 
                                          xy=(1, values[-1]),
                                          xytext=(1.02, values[-1]),
                                          arrowprops=dict(arrowstyle="->"),
                                          fontsize=9)
                
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            
        return fig
        
    def plot_activity_heatmap(self, activity_data: np.ndarray,
                             title: str = "Activity Heatmap",
                             save_path: str = None) -> plt.Figure:
        """
        Plot activity heatmap.
        
        Args:
            activity_data: Activity data (neurons x time)
            title: Plot title
            save_path: Path to save plot (if None, not saved)
            
        Returns:
            plt.Figure: Created figure
        """
        fig, axes = plt.subplots(1, 2, figsize=self.figure_size)
        fig.suptitle(title, fontsize=16, fontweight='bold')
        
        # Activity heatmap
        im1 = axes[0].imshow(activity_data, aspect='auto', cmap=self.cmap,
                            extent=[0, activity_data.shape[1], 0, activity_data.shape[0]])
        axes[0].set_title("Activity Heatmap")
        axes[0].set_xlabel("Time Step")
        axes[0].set_ylabel("Neuron Index")
        plt.colorbar(im1, ax=axes[0])
        
        # Activity statistics heatmap
        neuron_stats = np.array([
            np.mean(activity_data, axis=1),
            np.std(activity_data, axis=1),
            np.percentile(activity_data, 25, axis=1),
            np.percentile(activity_data, 75, axis=1)
        ])
        
        im2 = axes[1].imshow(neuron_stats, aspect='auto', cmap=self.cmap,
                            extent=[0, activity_data.shape[1], 0, neuron_stats.shape[0]])
        axes[1].set_title("Neuron Statistics")
        axes[1].set_xlabel("Time Step")
        axes[1].set_ylabel("Statistic (row 0: mean, 1: std, 2: 25th percentile, 3: 75th percentile)")
        plt.colorbar(im2, ax=axes[1])
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            
        return fig
        
    def plot_metric_comparison(self, metric_data: Dict[str, List[float]],
                              metric_names: List[str] = None,
                              title: str = "Metric Comparison",
                              save_path: str = None) -> plt.Figure:
        """
        Plot metric comparison.
        
        Args:
            metric_data: Dictionary of metric names and values
            metric_names: Names of metrics to plot (if None, uses keys)
            title: Plot title
            save_path: Path to save plot (if None, not saved)
            
        Returns:
            plt.Figure: Created figure
        """
        if metric_names is None:
            metric_names = list(metric_data.keys())
            
        fig, axes = plt.subplots(1, min(len(metric_names), 3), figsize=self.figure_size)
        if len(metric_names) == 1:
            axes = [axes]
        
        fig.suptitle(title, fontsize=16, fontweight='bold')
        
        for i, metric_name in enumerate(metric_names[:3]):  # Limit to 3 plots
            if metric_name in metric_data:
                values = metric_data[metric_name]
                
                # Box plot
                axes[i].boxplot(values)
                axes[i].set_title(metric_name.replace('_', ' ').title())
                axes[i].set_ylabel("Value")
                axes[i].grid(True, alpha=0.3)
                
                # Add mean annotation
                if values:
                    axes[i].annotate(f"Mean: {np.mean(values):.4f}",
                                   xy=(1, 0.5), xytext=(1.1, 0.5),
                                   arrowprops=dict(arrowstyle="->"),
                                   fontsize=9)
                
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            
        return fig
        
    def create_animation(self, data_sequence: List[np.ndarray],
                        title: str = "Animation",
                        save_path: str = None) -> FuncAnimation:
        """
        Create animation from data sequence.
        
        Args:
            data_sequence: Sequence of data frames
            title: Animation title
            save_path: Path to save animation (if None, not saved)
            
        Returns:
            FuncAnimation: Animation object
        """
        fig, axes = plt.subplots(1, 3, figsize=self.figure_size)
        fig.suptitle(title, fontsize=16, fontweight='bold')
        
        # Initialize plots
        lines = []
        for ax in axes:
            line, = ax.plot([], [], 'b-')
            lines.append(line)
            ax.set_xlim(0, len(data_sequence[0]) if data_sequence else 100)
            ax.set_ylim(-1, 1)
            ax.grid(True, alpha=0.3)
            
        def update(frame):
            if frame < len(data_sequence):
                for i, ax in enumerate(axes):
                    line = lines[i]
                    line.set_data(range(len(data_sequence[frame])), data_sequence[frame])
            
            return lines
            
        # Create animation
        anim = FuncAnimation(fig, update, frames=len(data_sequence), 
                            interval=50, blit=False, repeat=True)
                            
        if save_path:
            anim.save(save_path, writer='ffmpeg', fps=20, dpi=100)
            
        return anim
        
    def get_plotting_statistics(self) -> Dict[str, Any]:
        """Get plotting statistics."""
        return {
            "style": self.style,
            "figure_size": self.figure_size,
            "cmap": str(self.cmap),
            "plot_count": len(self.plot_history),
            "recent_plots": list(self.plot_history)[-5:]
        }
        
    def save_figure(self, fig: plt.Figure, filepath: str) -> None:
        """
        Save figure to file.
        
        Args:
            fig: Figure to save
            filepath: Output file path
        """
        filepath = Path(filepath)
        filepath.parent.mkdir(parents=True, exist_ok=True)
        fig.savefig(filepath, dpi=300, bbox_inches='tight')
        
    def close_all_figures(self) -> None:
        """Close all open figures."""
        plt.close('all')
        
    def reset(self) -> None:
        """Reset plotter to initial state."""
        self.plot_history.clear()
        self.close_all_figures()


class ActivityAnalyzer:
    """
    Analyze neural patterns and activity.
    
    This class provides analysis capabilities for NLM neural activity,
    including pattern recognition, statistical analysis, and feature extraction.
    """
    
    def __init__(self, analysis_methods: List[str] = None):
        """
        Initialize activity analyzer.
        
        Args:
            analysis_methods: Analysis methods to use
        """
        if analysis_methods is None:
            analysis_methods = ["basic", "advanced", "statistical"]
            
        self.analysis_methods = analysis_methods
        self.analysis_history = deque(maxlen=100)
        
    def analyze_activity(self, activity_data: np.ndarray,
                        analysis_types: List[str] = None) -> Dict[str, Any]:
        """
        Analyze neural activity.
        
        Args:
            activity_data: Neural activity data (neurons x time)
            analysis_types: Types of analysis to perform
            
        Returns:
            Dict[str, Any]: Analysis results
        """
        if analysis_types is None:
            analysis_types = self.analysis_methods
            
        analysis_result = {}
        
        if "basic" in analysis_types:
            analysis_result["basic"] = self._basic_analysis(activity_data)
            
        if "statistical" in analysis_types:
            analysis_result["statistical"] = self._statistical_analysis(activity_data)
            
        if "advanced" in analysis_types:
            analysis_result["advanced"] = self._advanced_analysis(activity_data)
            
        # Store analysis
        self.analysis_history.append({
            "timestamp": time.time(),
            "activity_shape": activity_data.shape,
            "analysis_types": analysis_types,
            "result": analysis_result
        })
        
        return analysis_result
        
    def _basic_analysis(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Perform basic activity analysis."""
        # Calculate basic statistics
        total_activity = np.sum(activity_data, axis=0)
        
        return {
            "neuron_count": activity_data.shape[0],
            "time_steps": activity_data.shape[1],
            "total_activity": total_activity.tolist(),
            "peak_activity": np.max(activity_data),
            "mean_activity": np.mean(activity_data),
            "std_activity": np.std(activity_data),
            "activity_variance": np.var(activity_data),
            "firing_rate": np.mean(activity_data > 0, axis=1).tolist(),
            "active_neurons": np.sum(activity_data > 0, axis=1).tolist()
        }
        
    def _statistical_analysis(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Perform statistical analysis."""
        # Calculate statistical features
        neurons, times = activity_data.shape
        
        # Distribution analysis
        activity_flat = activity_data.flatten()
        
        # Skewness and kurtosis
        skew = stats.skew(activity_flat)
        kurt = stats.kurtosis(activity_flat)
        
        # Correlation matrix
        if neurons < times:
            correlation_matrix = np.corrcoef(activity_data.T)
        else:
            correlation_matrix = np.corrcoef(activity_data)
            
        # Autocorrelation
        autocorr = []
        for neuron in range(min(neurons, 10)):  # Limit to first 10 neurons
            if len(activity_data[neuron]) > 1:
                corr = np.corrcoef(activity_data[neuron][:-1], activity_data[neuron][1:])[0, 1]
                autocorr.append(corr)
                
        return {
            "distribution": {
                "skewness": skew,
                "kurtosis": kurt,
                "mean": np.mean(activity_flat),
                "median": np.median(activity_flat),
                "std": np.std(activity_flat),
                "min": np.min(activity_flat),
                "max": np.max(activity_flat)
            },
            "correlation": {
                "matrix": correlation_matrix.tolist() if neurons < 10 and times < 10 else "too_large",
                "average_correlation": np.mean(np.abs(correlation_matrix)) if neurons < times else np.mean(np.abs(correlation_matrix)),
                "autocorrelation": autocorr
            },
            "time_domain": {
                "mean_power_spectrum": self._calculate_power_spectrum(activity_data),
                "dominant_frequencies": self._find_dominant_frequencies(activity_data)
            }
        }
        
    def _advanced_analysis(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Perform advanced analysis."""
        # Advanced pattern recognition
        neurons, times = activity_data.shape
        
        # Network motifs
        motifs = self._detect_network_motifs(activity_data)
        
        # Burst detection
        bursts = self._detect_bursts(activity_data)
        
        # Synchrony analysis
        synchrony = self._calculate_synchrony(activity_data)
        
        # Information measures
        information = self._calculate_information_measures(activity_data)
        
        return {
            "network_motifs": motifs,
            "bursts": bursts,
            "synchrony": synchrony,
            "information": information
        }
        
    def _calculate_power_spectrum(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Calculate power spectrum."""
        neurons, times = activity_data.shape
        
        # Use FFT for first neuron (if available)
        if neurons > 0 and times > 1:
            fft_result = np.fft.fft(activity_data[0])
            freqs = np.fft.fftfreq(times)
            power = np.abs(fft_result) ** 2
            
            # Find dominant frequency
            dominant_idx = np.argmax(power[1:times//2]) + 1  # Skip DC component
            dominant_freq = freqs[dominant_idx]
            dominant_power = power[dominant_idx]
            
            return {
                "frequencies": freqs.tolist(),
                "power_spectrum": power.tolist(),
                "dominant_frequency": dominant_freq,
                "dominant_power": dominant_power
            }
        else:
            return {"error": "Insufficient data for power spectrum calculation"}
            
    def _find_dominant_frequencies(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Find dominant frequencies."""
        neurons, times = activity_data.shape
        
        if neurons == 0 or times == 0:
            return {"error": "No data available"}
            
        # Average across neurons
        avg_activity = np.mean(activity_data, axis=0)
        
        # Calculate FFT
        fft_result = np.fft.fft(avg_activity)
        freqs = np.fft.fftfreq(times)
        power = np.abs(fft_result) ** 2
        
        # Find top frequencies
        positive_freqs = freqs[:times//2]
        positive_power = power[:times//2]
        
        # Get top 5 frequencies
        top_indices = np.argsort(positive_power)[-5:][::-1]
        top_freqs = positive_freqs[top_indices]
        top_powers = positive_power[top_indices]
        
        return {
            "frequencies": top_freqs.tolist(),
            "powers": top_powers.tolist(),
            "peak_frequency": top_freqs[0] if len(top_freqs) > 0 else 0,
            "peak_power": top_powers[0] if len(top_powers) > 0 else 0
        }
        
    def _detect_network_motifs(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Detect network motifs."""
        neurons, times = activity_data.shape
        
        if neurons < 3:
            return {"error": "Need at least 3 neurons for motif detection"}
            
        # Simple motif detection (placeholder)
        # In a real implementation, this would be more sophisticated
        
        motifs = {
            "pairwise_synchrony": [],
            "triadic_patterns": [],
            "cluster_patterns": []
        }
        
        # Calculate pairwise synchrony
        for i in range(neurons):
            for j in range(i + 1, neurons):
                corr = np.corrcoef(activity_data[i], activity_data[j])[0, 1]
                if abs(corr) > 0.7:  # High synchrony threshold
                    motifs["pairwise_synchrony"].append({
                        "neuron1": i,
                        "neuron2": j,
                        "correlation": corr
                    })
                    
        return motifs
        
    def _detect_bursts(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Detect bursts in neural activity."""
        neurons, times = activity_data.shape
        
        if times < 10:
            return {"error": "Need at least 10 time steps for burst detection"}
            
        bursts = []
        
        # Simple burst detection
        for neuron in range(neurons):
            activity = activity_data[neuron]
            
            # Find high-activity periods
            threshold = np.mean(activity) + np.std(activity)
            high_activity = activity > threshold
            
            # Find bursts
            in_burst = False
            burst_start = 0
            
            for t in range(times):
                if high_activity[t] and not in_burst:
                    in_burst = True
                    burst_start = t
                elif not high_activity[t] and in_burst:
                    in_burst = False
                    if t - burst_start >= 5:  # Minimum burst length
                        bursts.append({
                            "neuron": neuron,
                            "start_time": burst_start,
                            "duration": t - burst_start,
                            "max_activity": np.max(activity[burst_start:t])
                        })
                        
            if in_burst and times - burst_start >= 5:
                bursts.append({
                    "neuron": neuron,
                    "start_time": burst_start,
                    "duration": times - burst_start,
                    "max_activity": np.max(activity[burst_start:])
                })
                
        return {
            "bursts": bursts,
            "total_bursts": len(bursts),
            "neurons_with_bursts": len(set(b["neuron"] for b in bursts)),
            "average_burst_duration": np.mean([b["duration"] for b in bursts]) if bursts else 0
        }
        
    def _calculate_synchrony(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Calculate neural synchrony."""
        neurons, times = activity_data.shape
        
        if neurons < 2:
            return {"error": "Need at least 2 neurons for synchrony calculation"}
            
        # Calculate pairwise synchrony
        synchrony_matrix = np.zeros((neurons, neurons))
        
        for i in range(neurons):
            for j in range(neurons):
                if i != j:
                    corr = np.corrcoef(activity_data[i], activity_data[j])[0, 1]
                    synchrony_matrix[i, j] = abs(corr)
                    
        # Calculate overall synchrony
        synchrony_values = synchrony_matrix[np.triu_indices(neurons, k=1)]
        
        return {
            "synchrony_matrix": synchrony_matrix.tolist(),
            "average_synchrony": np.mean(synchrony_values),
            "max_synchrony": np.max(synchrony_values),
            "synchrony_threshold": 0.7,  # Threshold for high synchrony
            "highly_synchronous_pairs": np.sum(synchrony_values > 0.7)
        }
        
    def _calculate_information_measures(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Calculate information measures."""
        neurons, times = activity_data.shape
        
        # Calculate entropy
        entropy = np.zeros(neurons)
        for i in range(neurons):
            # Discretize activity
            discretized = np.digitize(activity_data[i], bins=np.linspace(
                np.min(activity_data), np.max(activity_data), 10))
            
            # Calculate entropy
            counts = np.bincount(discretized)
            probabilities = counts / np.sum(counts)
            entropy[i] = -np.sum(probabilities * np.log2(probabilities + 1e-8))
            
        # Calculate mutual information between neurons
        mutual_info = np.zeros((neurons, neurons))
        
        for i in range(neurons):
            for j in range(neurons):
                if i != j:
                    # Calculate mutual information (simplified)
                    mi = 0.5 * (entropy[i] + entropy[j]) - np.corrcoef(activity_data[i], activity_data[j])[0, 1]
                    mutual_info[i, j] = max(0, mi)
                    
        return {
            "entropy": entropy.tolist(),
            "average_entropy": np.mean(entropy),
            "mutual_information_matrix": mutual_info.tolist(),
            "average_mutual_information": np.mean(mutual_info[np.triu_indices(neurons, k=1)])
        }
        
    def get_analysis_statistics(self) -> Dict[str, Any]:
        """Get analysis statistics."""
        return {
            "analysis_methods": self.analysis_methods,
            "total_analyses": len(self.analysis_history),
            "recent_analyses": list(self.analysis_history)[-5:]
        }
        
    def reset(self) -> None:
        """Reset analyzer to initial state."""
        self.analysis_history.clear()


class HeatmapGenerator:
    """
    Generate neural heatmaps.
    
    This class provides functionality to generate various types of neural heatmaps,
    including spatial heatmaps, activity heatmaps, and connectivity heatmaps.
    """
    
    def __init__(self, colormap: str = "viridis", normalize: bool = True):
        """
        Initialize heatmap generator.
        
        Args:
            colormap: Colormap for heatmaps
            normalize: Whether to normalize data
        """
        self.colormap = colormap
        self.normalize = normalize
        self.heatmap_history = deque(maxlen=100)
        
    def generate_neural_heatmap(self, activity_data: np.ndarray,
                               title: str = "Neural Activity Heatmap",
                               save_path: str = None) -> plt.Figure:
        """
        Generate neural activity heatmap.
        
        Args:
            activity_data: Neural activity data (neurons x time)
            title: Plot title
            save_path: Path to save plot (if None, not saved)
            
        Returns:
            plt.Figure: Created figure
        """
        fig, axes = plt.subplots(2, 2, figsize=(15, 12))
        fig.suptitle(title, fontsize=16, fontweight='bold')
        
        # Normalize data if requested
        if self.normalize:
            data_min, data_max = np.min(activity_data), np.max(activity_data)
            normalized_data = (activity_data - data_min) / (data_max - data_min + 1e-8)
        else:
            normalized_data = activity_data
            
        # Activity heatmap (neurons x time)
        im1 = axes[0, 0].imshow(normalized_data.T, aspect='auto', 
                              cmap=self.colormap, interpolation='nearest',
                              extent=[0, activity_data.shape[1], 0, activity_data.shape[0]])
        axes[0, 0].set_title("Activity Heatmap (Neurons x Time)")
        axes[0, 0].set_xlabel("Time Step")
        axes[0, 0].set_ylabel("Neuron Index")
        plt.colorbar(im1, ax=axes[0, 0])
        
        # Correlation heatmap
        if activity_data.shape[0] < activity_data.shape[1]:
            correlation_matrix = np.corrcoef(activity_data.T)
        else:
            correlation_matrix = np.corrcoef(activity_data)
            
        im2 = axes[0, 1].imshow(correlation_matrix, cmap=self.colormap,
                               vmin=-1, vmax=1, interpolation='nearest')
        axes[0, 1].set_title("Correlation Matrix")
        axes[0, 1].set_xlabel("Neuron Index")
        axes[0, 1].set_ylabel("Neuron Index")
        plt.colorbar(im2, ax=axes[0, 1])
        
        # Time-averaged activity heatmap (for neurons)
        time_avg = np.mean(activity_data, axis=1)
        im3 = axes[1, 0].barh(range(len(time_avg)), time_avg, 
                            color=plt.cm.get_cmap(self.colormap)(time_avg/np.max(time_avg)))
        axes[1, 0].set_title("Time-Averaged Activity")
        axes[1, 0].set_xlabel("Average Activity")
        axes[1, 0].set_ylabel("Neuron Index")
        
        # Activity statistics heatmap
        neuron_stats = np.array([
            np.mean(activity_data, axis=1),
            np.std(activity_data, axis=1),
            np.percentile(activity_data, 25, axis=1),
            np.percentile(activity_data, 75, axis=1)
        ])
        
        im4 = axes[1, 1].imshow(neuron_stats, aspect='auto',
                               cmap=self.colormap,
                               extent=[0, activity_data.shape[1], 0, neuron_stats.shape[0]])
        axes[1, 1].set_title("Neuron Statistics (Rows: mean, std, 25th, 75th percentile)")
        axes[1, 1].set_xlabel("Time Step")
        axes[1, 1].set_ylabel("Statistic")
        plt.colorbar(im4, ax=axes[1, 1])
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            
        return fig
        
    def generate_spatial_heatmap(self, positions: np.ndarray, values: np.ndarray,
                               title: str = "Spatial Heatmap",
                               save_path: str = None) -> plt.Figure:
        """
        Generate spatial heatmap.
        
        Args:
            positions: Neuron positions (neurons x 2)
            values: Values at positions (e.g., activity levels)
            title: Plot title
            save_path: Path to save plot (if None, not saved)
            
        Returns:
            plt.Figure: Created figure
        """
        fig, axes = plt.subplots(1, 3, figsize=(18, 5))
        fig.suptitle(title, fontsize=16, fontweight='bold')
        
        # Spatial heatmap
        im1 = axes[0].scatter(positions[:, 0], positions[:, 1], 
                            c=values, cmap=self.colormap, s=100, alpha=0.8)
        axes[0].set_title("Spatial Distribution")
        axes[0].set_xlabel("X Position")
        axes[0].set_ylabel("Y Position")
        plt.colorbar(im1, ax=axes[0])
        
        # Value distribution
        axes[1].hist(values, bins=30, alpha=0.7, density=True)
        axes[1].set_title("Value Distribution")
        axes[1].set_xlabel("Value")
        axes[1].set_ylabel("Density")
        axes[1].grid(True, alpha=0.3)
        
        # Kernel density estimate
        try:
            from scipy.stats import gaussian_kde
            kde = gaussian_kde(values)
            x_range = np.linspace(np.min(values), np.max(values), 100)
            axes[2].plot(x_range, kde(x_range), 'r-', linewidth=2)
            axes[2].hist(values, bins=30, density=True, alpha=0.5)
            axes[2].set_title("Density Estimation")
            axes[2].set_xlabel("Value")
            axes[2].set_ylabel("Density")
            axes[2].grid(True, alpha=0.3)
        except ImportError:
            axes[2].text(0.5, 0.5, "KDE not available", 
                        horizontalalignment='center',
                        verticalalignment='center',
                        transform=axes[2].transAxes)
            axes[2].set_title("Density Estimation")
            
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            
        return fig
        
    def generate_connectivity_heatmap(self, connectivity_matrix: np.ndarray,
                                     title: str = "Connectivity Heatmap",
                                     save_path: str = None) -> plt.Figure:
        """
        Generate connectivity heatmap.
        
        Args:
            connectivity_matrix: Connectivity matrix (neurons x neurons)
            title: Plot title
            save_path: Path to save plot (if None, not saved)
            
        Returns:
            plt.Figure: Created figure
        """
        fig, axes = plt.subplots(1, 3, figsize=(18, 5))
        fig.suptitle(title, fontsize=16, fontweight='bold')
        
        # Full connectivity matrix
        im1 = axes[0].imshow(connectivity_matrix, cmap=self.colormap,
                           vmin=np.min(connectivity_matrix), vmax=np.max(connectivity_matrix),
                           interpolation='nearest')
        axes[0].set_title("Full Connectivity Matrix")
        axes[0].set_xlabel("Target Neuron")
        axes[0].set_ylabel("Source Neuron")
        plt.colorbar(im1, ax=axes[0])
        
        # Upper triangular matrix
        tri_upper = np.triu(connectivity_matrix)
        im2 = axes[1].imshow(tri_upper, cmap=self.colormap,
                           vmin=np.min(tri_upper), vmax=np.max(tri_upper),
                           interpolation='nearest')
        axes[1].set_title("Upper Triangular Matrix")
        axes[1].set_xlabel("Target Neuron")
        axes[1].set_ylabel("Source Neuron")
        plt.colorbar(im2, ax=axes[1])
        
        # Connectivity distribution
        flat_conn = connectivity_matrix.flatten()
        axes[2].hist(flat_conn, bins=50, alpha=0.7, density=True)
        axes[2].set_title("Connectivity Distribution")
        axes[2].set_xlabel("Connection Strength")
        axes[2].set_ylabel("Density")
        axes[2].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            
        return fig
        
    def generate_activity_heatmap_2d(self, activity_2d: np.ndarray,
                                    title: str = "2D Activity Heatmap",
                                    save_path: str = None) -> plt.Figure:
        """
        Generate 2D activity heatmap.
        
        Args:
            activity_2d: 2D activity data
            title: Plot title
            save_path: Path to save plot (if None, not saved)
            
        Returns:
            plt.Figure: Created figure
        """
        fig, axes = plt.subplots(2, 2, figsize=(16, 12))
        fig.suptitle(title, fontsize=16, fontweight='bold')
        
        # Original heatmap
        im1 = axes[0, 0].imshow(activity_2d, cmap=self.colormap, 
                               interpolation='nearest')
        axes[0, 0].set_title("Original Activity")
        axes[0, 0].set_xlabel("Column")
        axes[0, 0].set_ylabel("Row")
        plt.colorbar(im1, ax=axes[0, 0])
        
        # Gaussian smoothed heatmap
        from scipy.ndimage import gaussian_filter
        smoothed = gaussian_filter(activity_2d, sigma=1)
        im2 = axes[0, 1].imshow(smoothed, cmap=self.colormap,
                               interpolation='nearest')
        axes[0, 1].set_title("Gaussian Smoothed")
        axes[0, 1].set_xlabel("Column")
        axes[0, 1].set_ylabel("Row")
        plt.colorbar(im2, ax=axes[0, 1])
        
        # Difference heatmap
        diff = smoothed - activity_2d
        im3 = axes[1, 0].imshow(diff, cmap=self.colormap,
                               vmin=np.min(diff), vmax=np.max(diff),
                               interpolation='nearest')
        axes[1, 0].set_title("Difference (Smoothed - Original)")
        axes[1, 0].set_xlabel("Column")
        axes[1, 0].set_ylabel("Row")
        plt.colorbar(im3, ax=axes[1, 0])
        
        # Edge detection
        from scipy import ndimage
        edges = ndimage.sobel(activity_2d)
        im4 = axes[1, 1].imshow(edges, cmap='gray', interpolation='nearest')
        axes[1, 1].set_title("Edge Detection")
        axes[1, 1].set_xlabel("Column")
        axes[1, 1].set_ylabel("Row")
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            
        return fig
        
    def get_heatmap_statistics(self) -> Dict[str, Any]:
        """Get heatmap generation statistics."""
        return {
            "colormap": self.colormap,
            "normalize": self.normalize,
            "heatmap_count": len(self.heatmap_history),
            "recent_heatmaps": list(self.heatmap_history)[-5:]
        }
        
    def reset(self) -> None:
        """Reset heatmap generator."""
        self.heatmap_history.clear()


class ActivityStatistics:
    """
    Compute statistics from neural activity.
    
    This class provides comprehensive statistical analysis of neural activity,
    including descriptive statistics, inferential statistics, and advanced metrics.
    """
    
    def __init__(self, statistics_types: List[str] = None):
        """
        Initialize activity statistics calculator.
        
        Args:
            statistics_types: Types of statistics to compute
        """
        if statistics_types is None:
            statistics_types = ["descriptive", "temporal", "spatial", "advanced"]
            
        self.statistics_types = statistics_types
        self.statistics_history = deque(maxlen=100)
        
    def compute_statistics(self, activity_data: np.ndarray,
                          statistic_types: List[str] = None) -> Dict[str, Any]:
        """
        Compute statistics from neural activity.
        
        Args:
            activity_data: Neural activity data (neurons x time)
            statistic_types: Types of statistics to compute
            
        Returns:
            Dict[str, Any]: Computed statistics
        """
        if statistic_types is None:
            statistic_types = self.statistics_types
            
        statistics_result = {}
        
        if "descriptive" in statistic_types:
            statistics_result["descriptive"] = self._compute_descriptive_stats(activity_data)
            
        if "temporal" in statistic_types:
            statistics_result["temporal"] = self._compute_temporal_stats(activity_data)
            
        if "spatial" in statistic_types:
            statistics_result["spatial"] = self._compute_spatial_stats(activity_data)
            
        if "advanced" in statistic_types:
            statistics_result["advanced"] = self._compute_advanced_stats(activity_data)
            
        # Store statistics
        self.statistics_history.append({
            "timestamp": time.time(),
            "activity_shape": activity_data.shape,
            "statistic_types": statistic_types,
            "result": statistics_result
        })
        
        return statistics_result
        
    def _compute_descriptive_stats(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Compute descriptive statistics."""
        neurons, times = activity_data.shape
        
        # Overall statistics
        flat_data = activity_data.flatten()
        
        return {
            "overall": {
                "mean": np.mean(flat_data),
                "median": np.median(flat_data),
                "std": np.std(flat_data),
                "variance": np.var(flat_data),
                "skewness": stats.skew(flat_data),
                "kurtosis": stats.kurtosis(flat_data),
                "min": np.min(flat_data),
                "max": np.max(flat_data),
                "range": np.max(flat_data) - np.min(flat_data),
                "interquartile_range": np.percentile(flat_data, 75) - np.percentile(flat_data, 25)
            },
            "neurons": {
                "average_activity": np.mean(activity_data, axis=1).tolist(),
                "activity_std": np.std(activity_data, axis=1).tolist(),
                "firing_rate": np.mean(activity_data > 0, axis=1).tolist(),
                "activity_cv": np.std(activity_data, axis=1) / (np.mean(activity_data, axis=1) + 1e-8)
            },
            "time": {
                "average_activity_over_time": np.mean(activity_data, axis=0).tolist(),
                "activity_autocorrelation": self._calculate_autocorrelation(activity_data),
                "activity_periodicity": self._find_periodicity(activity_data)
            }
        }
        
    def _compute_temporal_stats(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Compute temporal statistics."""
        neurons, times = activity_data.shape
        
        # Temporal dynamics
        temporal_stats = {}
        
        # Change statistics
        for neuron in range(min(neurons, 10)):  # Limit for performance
            activity = activity_data[neuron]
            
            # First differences
            diff = np.diff(activity)
            temporal_stats[f"neuron_{neuron}"] = {
                "mean_change": np.mean(diff),
                "std_change": np.std(diff),
                "change_variance": np.var(diff),
                "positive_changes": np.sum(diff > 0),
                "negative_changes": np.sum(diff < 0),
                "zero_changes": np.sum(diff == 0)
            }
            
        # Cross-temporal correlations
        if neurons < times:
            corr_matrix = np.corrcoef(activity_data.T)
        else:
            corr_matrix = np.corrcoef(activity_data)
            
        temporal_stats["cross_neuron_correlations"] = {
            "average_correlation": np.mean(np.abs(corr_matrix[np.triu_indices(neurons, k=1)])),
            "max_correlation": np.max(np.abs(corr_matrix[np.triu_indices(neurons, k=1)])),
            "min_correlation": np.min(np.abs(corr_matrix[np.triu_indices(neurons, k=1)])),
            "correlation_distribution": self._get_correlation_distribution(corr_matrix)
        }
        
        return temporal_stats
        
    def _compute_spatial_stats(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Compute spatial statistics."""
        neurons, times = activity_data.shape
        
        # Activity ranking
        sorted_neurons = np.argsort(np.mean(activity_data, axis=1))
        
        return {
            "activity_ranking": {
                "sorted_neuron_indices": sorted_neurons.tolist(),
                "sorted_activity_levels": np.mean(activity_data, axis=1)[sorted_neurons].tolist(),
                "activity_percentile_ranks": self._compute_percentile_ranks(activity_data)
            },
            "activity_clusters": self._find_activity_clusters(activity_data),
            "synchrony_measures": self._compute_synchrony_measures(activity_data)
        }
        
    def _compute_advanced_stats(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Compute advanced statistics."""
        neurons, times = activity_data.shape
        
        # Advanced metrics
        advanced_stats = {}
        
        # Information theory measures
        advanced_stats["information_theory"] = {
            "entropy": self._calculate_neuron_entropy(activity_data),
            "mutual_information": self._calculate_mutual_information(activity_data),
            "transfer_entropy": self._calculate_transfer_entropy(activity_data)
        }
        
        # Nonlinear dynamics
        advanced_stats["nonlinear_dynamics"] = {
            "lyapunov_exponent": self._estimate_lyapunov_exponent(activity_data),
            "fractal_dimension": self._estimate_fractal_dimension(activity_data),
            "attractor_reconstruction": self._reconstruct_attractors(activity_data)
        }
        
        # Burst statistics
        advanced_stats["burst_analysis"] = self._analyze_bursts(activity_data)
        
        return advanced_stats
        
    def _calculate_autocorrelation(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Calculate autocorrelation."""
        neurons, times = activity_data.shape
        
        autocorr_values = {}
        
        for neuron in range(min(neurons, 5)):  # Limit for performance
            activity = activity_data[neuron]
            
            # Normalize
            activity_norm = (activity - np.mean(activity)) / (np.std(activity) + 1e-8)
            
            # Calculate autocorrelation
            autocorr = np.correlate(activity_norm, activity_norm, mode='full')
            autocorr = autocorr[len(autocorr)//2:]
            autocorr = autocorr / autocorr[0]
            
            autocorr_values[f"neuron_{neuron}"] = autocorr.tolist()
            
        return autocorr_values
        
    def _find_periodicity(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Find periodicity in activity."""
        neurons, times = activity_data.shape
        
        periodicities = {}
        
        for neuron in range(min(neurons, 5)):  # Limit for performance
            activity = activity_data[neuron]
            
            # Apply FFT
            fft_result = np.fft.fft(activity)
            freqs = np.fft.fftfreq(times)
            power = np.abs(fft_result) ** 2
            
            # Find peak frequency
            positive_freqs = freqs[:times//2]
            positive_power = power[:times//2]
            
            if len(positive_power) > 1:
                peak_idx = np.argmax(positive_power[1:]) + 1  # Skip DC
                peak_freq = positive_freqs[peak_idx]
                peak_power = positive_power[peak_idx]
                
                periodicities[f"neuron_{neuron}"] = {
                    "dominant_frequency": peak_freq,
                    "dominant_period": 1.0 / peak_freq if peak_freq > 0 else float('inf'),
                    "peak_power": peak_power,
                    "power_spectrum": positive_power.tolist()
                }
                
        return periodicities
        
    def _get_correlation_distribution(self, corr_matrix: np.ndarray) -> Dict[str, Any]:
        """Get correlation distribution."""
        if corr_matrix.size == 0:
            return {"error": "Empty correlation matrix"}
            
        # Get upper triangular elements (excluding diagonal)
        triu_indices = np.triu_indices_from(corr_matrix, k=1)
        correlations = corr_matrix[triu_indices]
        
        # Create histogram
        hist, bin_edges = np.histogram(correlations, bins=50)
        
        return {
            "histogram": hist.tolist(),
            "bin_edges": bin_edges.tolist(),
            "mean": np.mean(correlations),
            "median": np.median(correlations),
            "std": np.std(correlations),
            "min": np.min(correlations),
            "max": np.max(correlations)
        }
        
    def _compute_percentile_ranks(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Compute percentile ranks."""
        neurons, times = activity_data.shape
        
        percentile_ranks = {}
        
        for neuron in range(neurons):
            activity = activity_data[neuron]
            percentile_ranks[f"neuron_{neuron}"] = {
                "50th_percentile": np.percentile(activity, 50),
                "75th_percentile": np.percentile(activity, 75),
                "90th_percentile": np.percentile(activity, 90),
                "95th_percentile": np.percentile(activity, 95),
                "99th_percentile": np.percentile(activity, 99)
            }
            
        return percentile_ranks
        
    def _find_activity_clusters(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Find clusters of similar activity."""
        neurons, times = activity_data.shape
        
        # Simple clustering based on correlation
        correlation_matrix = np.corrcoef(activity_data.T)
        
        # Use hierarchical clustering (simplified)
        from scipy.cluster.hierarchy import linkage, fcluster
        from scipy.spatial.distance import squareform
        
        # Convert correlation to distance
        distance_matrix = 1 - np.abs(correlation_matrix)
        
        try:
            # Perform hierarchical clustering
            Z = linkage(squareform(distance_matrix), method='average')
            clusters = fcluster(Z, t=0.7, criterion='distance')
            
            cluster_info = {}
            for i in range(len(clusters)):
                cluster_id = clusters[i]
                if cluster_id not in cluster_info:
                    cluster_info[cluster_id] = []
                cluster_info[cluster_id].append(i)
                
            return {
                "cluster_assignments": clusters.tolist(),
                "num_clusters": len(cluster_info),
                "cluster_sizes": [len(cluster_info[cluster_id]) for cluster_id in cluster_info],
                "average_cluster_activity": {
                    cluster_id: np.mean(activity_data[cluster_info[cluster_id]].flatten())
                    for cluster_id in cluster_info
                }
            }
        except Exception as e:
            return {"error": f"Clustering failed: {e}"}
            
    def _compute_synchrony_measures(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Compute synchrony measures."""
        neurons, times = activity_data.shape
        
        synchrony_measures = {}
        
        # Pairwise synchrony
        synchrony_matrix = np.zeros((neurons, neurons))
        
        for i in range(neurons):
            for j in range(neurons):
                if i != j:
                    corr = np.corrcoef(activity_data[i], activity_data[j])[0, 1]
                    synchrony_matrix[i, j] = abs(corr)
                    
        synchrony_measures["pairwise_synchrony"] = synchrony_matrix.tolist()
        
        # Global synchrony
        triu_indices = np.triu_indices(neurons, k=1)
        synchrony_values = synchrony_matrix[triu_indices]
        
        synchrony_measures["global_synchrony"] = {
            "mean_synchrony": np.mean(synchrony_values),
            "max_synchrony": np.max(synchrony_values),
            "min_synchrony": np.min(synchrony_values),
            "synchrony_threshold": 0.7,
            "highly_synchronous_pairs": np.sum(synchrony_values > 0.7)
        }
        
        return synchrony_measures
        
    def _calculate_neuron_entropy(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Calculate neuron entropy."""
        neurons, times = activity_data.shape
        
        entropies = {}
        
        for neuron in range(neurons):
            activity = activity_data[neuron]
            
            # Discretize activity
            discretized = np.digitize(activity, bins=np.linspace(
                np.min(activity), np.max(activity), 10))
            
            # Calculate entropy
            counts = np.bincount(discretized)
            probabilities = counts / np.sum(counts)
            entropy = -np.sum(probabilities * np.log2(probabilities + 1e-8))
            
            entropies[f"neuron_{neuron}"] = entropy
            
        return {
            "entropies": entropies,
            "average_entropy": np.mean(list(entropies.values())),
            "max_entropy": np.max(list(entropies.values())),
            "min_entropy": np.min(list(entropies.values()))
        }
        
    def _calculate_mutual_information(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Calculate mutual information."""
        neurons, times = activity_data.shape
        
        if neurons < 2:
            return {"error": "Need at least 2 neurons for mutual information calculation"}
            
        mutual_info = np.zeros((neurons, neurons))
        
        for i in range(neurons):
            for j in range(neurons):
                if i != j:
                    # Calculate mutual information (simplified)
                    entropy_i = self._calculate_neuron_entropy(activity_data[[i]])["average_entropy"]
                    entropy_j = self._calculate_neuron_entropy(activity_data[[j]])["average_entropy"]
                    
                    # Joint entropy approximation
                    joint_entropy = entropy_i + entropy_j - 2 * np.corrcoef(activity_data[i], activity_data[j])[0, 1]
                    
                    mutual_info[i, j] = max(0, joint_entropy)
                    
        return {
            "mutual_information_matrix": mutual_info.tolist(),
            "average_mutual_information": np.mean(mutual_info[np.triu_indices(neurons, k=1)])
        }
        
    def _calculate_transfer_entropy(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Calculate transfer entropy."""
        neurons, times = activity_data.shape
        
        # Simple transfer entropy calculation (placeholder)
        # In a real implementation, this would use proper information theory
        
        transfer_entropy = np.zeros((neurons, neurons))
        
        for i in range(neurons):
            for j in range(neurons):
                if i != j:
                    # Simplified transfer entropy calculation
                    activity_i = activity_data[i]
                    activity_j = activity_data[j]
                    
                    # Calculate correlation with lag
                    max_lag = min(10, times // 2)
                    max_te = 0
                    
                    for lag in range(1, max_lag + 1):
                        if lag < times:
                            te = np.corrcoef(activity_i[:-lag], activity_j[lag:])[0, 1]
                            max_te = max(max_te, abs(te))
                            
                    transfer_entropy[i, j] = max_te
                    
        return {
            "transfer_entropy_matrix": transfer_entropy.tolist(),
            "average_transfer_entropy": np.mean(transfer_entropy[np.triu_indices(neurons, k=1)])
        }
        
    def _estimate_lyapunov_exponent(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Estimate Lyapunov exponent."""
        neurons, times = activity_data.shape
        
        if times < 10:
            return {"error": "Need more time steps for Lyapunov exponent estimation"}
            
        # Simple Lyapunov exponent estimation (placeholder)
        # In a real implementation, this would use proper nonlinear dynamics methods
        
        lyapunov_values = np.zeros(neurons)
        
        for neuron in range(min(neurons, 5)):  # Limit for performance
            activity = activity_data[neuron]
            
            # Simple approximation using Poincaré map
            # This is a placeholder - real implementation would be more complex
            lyapunov_values[neuron] = np.random.random() * 0.1  # Random value for demonstration
            
        return {
            "lyapunov_exponents": lyapunov_values.tolist(),
            "average_lyapunov_exponent": np.mean(lyapunov_values),
            "positive_exponents": np.sum(lyapunov_values > 0),
            "negative_exponents": np.sum(lyapunov_values < 0),
            "zero_exponents": np.sum(lyapunov_values == 0)
        }
        
    def _estimate_fractal_dimension(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Estimate fractal dimension."""
        neurons, times = activity_data.shape
        
        if times < 5:
            return {"error": "Need more time steps for fractal dimension estimation"}
            
        # Simple fractal dimension estimation (placeholder)
        # In a real implementation, this would use box-counting or other methods
        
        fractal_dimensions = np.zeros(neurons)
        
        for neuron in range(min(neurons, 5)):  # Limit for performance
            activity = activity_data[neuron]
            
            # Placeholder implementation
            fractal_dimensions[neuron] = np.random.random()  # Random value for demonstration
            
        return {
            "fractal_dimensions": fractal_dimensions.tolist(),
            "average_fractal_dimension": np.mean(fractal_dimensions),
            "max_fractal_dimension": np.max(fractal_dimensions),
            "min_fractal_dimension": np.min(fractal_dimensions)
        }
        
    def _reconstruct_attractors(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Reconstruct attractors."""
        neurons, times = activity_data.shape
        
        if times < 5:
            return {"error": "Need more time steps for attractor reconstruction"}
            
        # Simple attractor reconstruction (placeholder)
        # In a real implementation, this would use time-delay embedding
        
        attractors = {}
        
        for neuron in range(min(neurons, 3)):  # Limit for performance
            activity = activity_data[neuron]
            
            # Time-delay embedding
            embedding_dim = 3
            delay = 1
            
            if times >= embedding_dim * delay:
                embedded = np.zeros((times - (embedding_dim - 1) * delay, embedding_dim))
                
                for i in range(embedded.shape[0]):
                    embedded[i] = [activity[i + j * delay] for j in range(embedding_dim)]
                    
                attractors[f"neuron_{neuron}_attractor"] = embedded.tolist()
            else:
                attractors[f"neuron_{neuron}_attractor"] = []
                
        return attractors
        
    def _analyze_bursts(self, activity_data: np.ndarray) -> Dict[str, Any]:
        """Analyze bursts."""
        neurons, times = activity_data.shape
        
        burst_analysis = {}
        
        for neuron in range(min(neurons, 5)):  # Limit for performance
            activity = activity_data[neuron]
            
            # Find bursts (simplified)
            threshold = np.mean(activity) + np.std(activity)
            high_activity = activity > threshold
            
            bursts = []
            in_burst = False
            burst_start = 0
            
            for t in range(times):
                if high_activity[t] and not in_burst:
                    in_burst = True
                    burst_start = t
                elif not high_activity[t] and in_burst:
                    in_burst = False
                    if t - burst_start >= 3:  # Minimum burst length
                        bursts.append({
                            "start": burst_start,
                            "duration": t - burst_start,
                            "max_activity": np.max(activity[burst_start:t])
                        })
                        
            if in_burst and times - burst_start >= 3:
                bursts.append({
                    "start": burst_start,
                    "duration": times - burst_start,
                    "max_activity": np.max(activity[burst_start:])
                })
                
            burst_analysis[f"neuron_{neuron}"] = {
                "bursts": bursts,
                "burst_count": len(bursts),
                "average_burst_duration": np.mean([b["duration"] for b in bursts]) if bursts else 0,
                "average_burst_max_activity": np.mean([b["max_activity"] for b in bursts]) if bursts else 0
            }
            
        return burst_analysis
        
    def get_statistics_history(self) -> Dict[str, Any]:
        """Get statistics computation history."""
        return {
            "statistics_types": self.statistics_types,
            "total_computations": len(self.statistics_history),
            "recent_computations": list(self.statistics_history)[-5:]
        }
        
    def reset(self) -> None:
        """Reset statistics calculator."""
        self.statistics_history.clear()


# Re-export commonly used classes and functions
__all__ = [
    "Plotter", "ActivityAnalyzer", "HeatmapGenerator", "ActivityStatistics"
]