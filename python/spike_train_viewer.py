#!/usr/bin/env python3
"""
Spike Train Viewer Tool

Real-time spike train visualization for neural activity monitoring.
Provides interactive visualization of spike times, firing rates,
and temporal patterns in neural recordings.
"""

import numpy as np
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, asdict
from enum import Enum
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.dates as mdates
from matplotlib.colors import LinearSegmentedColormap
from datetime import datetime, timedelta
import scipy.signal as signal
import sys
import os

@dataclass
class SpikeTrainViewerConfig:
    """Configuration for spike train viewer"""
    time_window: float = 10.0  # seconds
    refresh_rate: float = 100.0  # Hz
    show_raster: bool = True
    show_intervals: bool = True
    show_firing_rate: bool = True
    show_cumulative: bool = False
    bin_size: float = 0.001  # seconds
    show_labels: bool = True
    show_statistics: bool = True
    color_scheme: str = "default"  # "default", "activity", "cluster", "gradient"
    save_format: str = "png"  # "png", "gif", "mp4", "html"
    output_path: str = "spike_train_viewer"
    
    # Filtering and analysis
    filter_type: str = None  # "butterworth", "hilbert", "moving_average"
    filter_cutoff: float = 10.0  # Hz
    highlight_patterns: bool = False
    show_isi_histogram: bool = True
    show_cross_corr: bool = False
    
    # Display settings
    figure_size: Tuple[int, int] = (14, 10)
    dpi: int = 150
    animation_interval: int = 50  # milliseconds
    show_legend: bool = True
class SpikeTrainViewer:
    """
    Real-time spike train visualization for neural activity monitoring.
    
    Features:
    - Real-time spike raster plotting
    - Firing rate calculation and visualization
    - Inter-spike interval (ISI) analysis
    - Cross-correlation analysis
    - Pattern detection and highlighting
    - Export capabilities for scientific analysis
    """
    
    def __init__(self, config: SpikeTrainViewerConfig = None):
        self.config = config or SpikeTrainViewerConfig()
        self.spike_data = []
        self.current_time = 0.0
        self.is_running = False
        self.animation_obj = None
        self.fig = None
        self.axes = None
        
        # Initialize viewer components
        self._initialize_components()
    
    def _initialize_components(self):
        """Initialize viewer components and data structures"""
        self.spike_traces = []  # List of spike time arrays for different neurons
        self.filtered_traces = []  # Filtered spike traces
        self.firing_rates = []  # Calculated firing rates
        self.isi_distributions = []  # ISI distributions
        self.cluster_labels = []  # For pattern detection
        
        # Color maps for different neuron types/patterns
        self._initialize_colormaps()
    
    def _initialize_colormaps(self):
        """Initialize colormaps for different visualization schemes"""
        # Default colormap (viridis-like)
        self.default_cmap = plt.cm.plasma
        
        # Activity colormap (red to white to blue)
        self.activity_cmap = LinearSegmentedColormap.from_list(
            'activity', ['red', 'yellow', 'white', 'lightblue', 'blue']
        )
        
        # Cluster colormap
        self.cluster_cmap = plt.cm.Set3
        
        # Gradient colormap for firing rates
        self.gradient_cmap = LinearSegmentedColormap.from_list(
            'gradient', ['darkblue', 'blue', 'lightblue', 'lightgreen', 'yellow', 'orange', 'red']
        )
    
    def add_spike_data(self, spike_times: np.ndarray, neuron_id: int = 0, 
                      neuron_type: str = "unknown", metadata: Dict[str, Any] = None):
        """
        Add spike train data for a neuron.
        
        Args:
            spike_times: Array of spike times (seconds)
            neuron_id: Unique identifier for the neuron
            neuron_type: Type of neuron ("excitatory", "inhibitory", etc.)
            metadata: Additional metadata about the neuron
        """
        if metadata is None:
            metadata = {}
        
        metadata['neuron_id'] = neuron_id
        metadata['neuron_type'] = neuron_type
        
        spike_times = np.asarray(spike_times)
        
        # Validate spike times
        if len(spike_times) == 0:
            return
        
        if np.any(spike_times < 0):
            raise ValueError("Spike times cannot be negative")
        
        if np.any(np.diff(spike_times) < 0):
            raise ValueError("Spike times must be in ascending order")
        
        # Store spike data
        self.spike_traces.append(spike_times)
        self.filtered_traces.append(spike_times.copy())
        self.isi_distributions.append(self._calculate_isi(spike_times))
        
        # Update cluster labels for pattern detection
        self.cluster_labels.append(self._detect_patterns(spike_times))
        
        # Recalculate firing rates
        self._update_firing_rates()
    
    def _calculate_isi(self, spike_times: np.ndarray) -> np.ndarray:
        """Calculate inter-spike intervals"""
        if len(spike_times) < 2:
            return np.array([])
        
        return np.diff(spike_times)
    
    def _detect_patterns(self, spike_times: np.ndarray) -> np.ndarray:
        """Detect firing patterns in spike train"""
        if len(spike_times) < 10:
            return np.zeros(len(spike_times))
        
        isis = self._calculate_isi(spike_times)
        
        # Detect bursting behavior
        burst_threshold = np.mean(isis) * 2.5  # Bursts have shorter ISIs
        burst_mask = isis < burst_threshold
        
        # Label pattern: 0 = regular, 1 = burst, 2 = silent, 3 = irregular
        pattern = np.zeros(len(spike_times))
        for i, is_burst in enumerate(burst_mask):
            if is_burst:
                pattern[i] = 1
                pattern[i+1] = 1
        
        return pattern
    
    def _update_firing_rates(self):
        """Calculate and store firing rates for all neurons"""
        self.firing_rates = []
        
        for spike_times in self.spike_traces:
            if len(spike_times) > 1:
                duration = spike_times[-1] - spike_times[0]
                if duration > 0:
                    firing_rate = len(spike_times) / duration
                else:
                    firing_rate = len(spike_times)
            else:
                firing_rate = 0.0
            
            self.firing_rates.append(firing_rate)
    
    def start_real_time(self, initial_data: Dict[str, Any] = None):
        """
        Start real-time visualization.
        
        Args:
            initial_data: Optional initial spike data
        """
        if initial_data:
            for i, (spike_times, neuron_id, neuron_type) in enumerate(zip(
                initial_data['spike_times'],
                initial_data['neuron_ids'],
                initial_data['neuron_types']
            )):
                self.add_spike_data(spike_times, neuron_id, neuron_type)
        
        self.is_running = True
        
        # Create visualization
        self.fig, self.axes = plt.subplots(
            3, 1, figsize=self.config.figure_size,
            gridspec_kw={'height_ratios': [3, 1, 2], 'hspace': 0.3}
        )
        
        # Setup subplots
        self._setup_subplots()
        
        # Start animation
        self.animation_obj = animation.FuncAnimation(
            self.fig, self._update_plot, interval=self.config.animation_interval,
            blit=False, cache_frame_data=False
        )
        
        plt.show()
    
    def _setup_subplots(self):
        """Setup the subplot layout and formatting"""
        # Raster plot (top subplot)
        ax_raster = self.axes[0]
        ax_raster.set_title("Spike Raster Plot", fontsize=14, fontweight='bold')
        ax_raster.set_xlabel("Time (s)")
        ax_raster.set_ylabel("Neuron Index")
        ax_raster.grid(True, alpha=0.3)
        
        # Current spike rates (middle subplot)
        ax_rates = self.axes[1]
        ax_rates.set_title("Firing Rates", fontsize=14, fontweight='bold')
        ax_rates.set_xlabel("Time (s)")
        ax_rates.set_ylabel("Firing Rate (Hz)")
        ax_rates.grid(True, alpha=0.3)
        ax_rates.set_ylim(0, max(self.firing_rates) * 1.2 if self.firing_rates else 10)
        
        # ISI histogram (bottom subplot)
        ax_isi = self.axes[2]
        ax_isi.set_title("Inter-Spike Interval Distribution", fontsize=14, fontweight='bold')
        ax_isi.set_xlabel("ISI (s)")
        ax_isi.set_ylabel("Count")
        ax_isi.grid(True, alpha=0.3)
        
        # Store axes references
        self.ax_raster = ax_raster
        self.ax_rates = ax_rates
        self.ax_isi = ax_isi
        
        # Initialize plot elements
        self._initialize_plot_elements()
    
    def _initialize_plot_elements(self):
        """Initialize plot elements for animation"""
        n_neurons = len(self.spike_traces)
        
        # Raster plot elements
        if self.config.show_raster:
            self.raster_scatter = ax_raster.scatter([], [], s=10, c='blue', alpha=0.6)
        
        # Firing rate line
        if self.config.show_firing_rate:
            self.rate_lines = []
            for i in range(n_neurons):
                line, = ax_rates.plot([], [], label=f'Neuron {i}', linewidth=2)
                self.rate_lines.append(line)
        
        # ISI histogram bars
        if self.config.show_isi_histogram:
            self.isi_bins = ax_isi.bar([], [], color='skyblue', alpha=0.7)
    
    def _update_plot(self, frame):
        """Update plot for animation"""
        current_time = frame * self.config.bin_size
        
        # Update current time
        self.current_time = current_time
        
        # Update raster plot
        if self.config.show_raster and hasattr(self, 'raster_scatter'):
            self._update_raster_plot(current_time)
        
        # Update firing rate plot
        if self.config.show_firing_rate and hasattr(self, 'rate_lines'):
            self._update_firing_rate_plot(current_time)
        
        # Update ISI histogram
        if self.config.show_isi_histogram and hasattr(self, 'isi_bins'):
            self._update_isi_histogram(current_time)
        
        return []
    
    def _update_raster_plot(self, current_time: float):
        """Update spike raster plot"""
        # Get spikes within current time window
        spikes_in_window = []
        neuron_ids = []
        
        for i, spike_times in enumerate(self.spike_traces):
            recent_spikes = spike_times[spike_times <= current_time]
            spikes_in_window.extend(recent_spikes)
            neuron_ids.extend([i] * len(recent_spikes))
        
        # Update scatter plot
        self.raster_scatter.set_offsets(np.column_stack([spikes_in_window, neuron_ids]))
    
    def _update_firing_rate_plot(self, current_time: float):
        """Update firing rate plot"""
        # Calculate instantaneous firing rates
        inst_rates = []
        for i, spike_times in enumerate(self.spike_traces):
            # Count spikes in sliding window around current_time
            window_start = max(0, current_time - self.config.time_window/2)
            window_end = current_time + self.config.time_window/2
            
            window_spikes = spike_times[(spike_times >= window_start) & (spike_times <= window_end)]
            inst_rate = len(window_spikes) / self.config.time_window
            inst_rates.append(inst_rate)
        
        # Update lines
        x_data = [current_time] * len(inst_rates)
        for i, (line, rate) in enumerate(zip(self.rate_lines, inst_rates)):
            line.set_data(x_data[:i+1], [self.firing_rates[i]] * (i+1))
    
    def _update_isi_histogram(self, current_time: float):
        """Update ISI histogram"""
        # Calculate ISIs for spikes up to current_time
        all_isis = []
        for i, spike_times in enumerate(self.spike_traces):
            recent_spikes = spike_times[spike_times <= current_time]
            isis = self._calculate_isi(recent_spikes)
            all_isis.extend(isis)
        
        if all_isis:
            # Update histogram
            counts, bins = np.histogram(all_isis, bins=50)
            bin_width = bins[1] - bins[0]
            
            self.isi_bins[0].set_height(counts)
            self.isi_bins[0].set_xerr(bin_width / 2)
        
        # Set x-axis limits based on current time window
        self.ax_isi.set_xlim(max(0, current_time - 5), current_time + 0.1)
    
    def create_static_visualization(self, output_file: str = None) -> Dict[str, Any]:
        """
        Create static visualization for analysis and reporting.
        
        Args:
            output_file: Optional output filename
            
        Returns:
            Dictionary with visualization data and results
        """
        result = {
            "success": False,
            "data": {},
            "plots": {},
            "statistics": {},
            "output_files": []
        }
        
        try:
            # Create figure with multiple subplots
            fig, axes = plt.subplots(
                3, 3, figsize=(15, 12),
                gridspec_kw={'height_ratios': [3, 1, 1], 'hspace': 0.4, 'wspace': 0.3}
            )
            
            axes = axes.flatten()
            
            # 1. Raster plot (top left)
            self._plot_raster(axes[0])
            
            # 2. Firing rate over time (top middle)
            self._plot_firing_rates(axes[1])
            
            # 3. ISI histogram (top right)
            self._plot_isi_histogram(axes[2])
            
            # 4. Spike frequency spectrum (middle left)
            self._plot_frequency_spectrum(axes[3])
            
            # 5. Autocorrelation (middle middle)
            self._plot_autocorrelation(axes[4])
            
            # 6. Cross-correlation (middle right)
            if self.config.show_cross_corr:
                self._plot_cross_correlation(axes[5])
            else:
                axes[5].text(0.5, 0.5, 'Cross-correlation\n(Diabled)', 
                           ha='center', va='center', transform=axes[5].transAxes)
            
            # 7. Cumulative spike count (bottom left)
            self._plot_cumulative_spikes(axes[6])
            
            # 8. Pattern analysis (bottom middle)
            if self.config.highlight_patterns:
                self._plot_pattern_analysis(axes[7])
            else:
                axes[7].text(0.5, 0.5, 'Pattern Analysis\n(Diabled)', 
                           ha='center', va='center', transform=axes[7].transAxes)
            
            # 9. Statistics panel (bottom right)
            self._plot_statistics_panel(axes[8])
            
            # Save figure
            if output_file is None:
                output_file = f"{self.config.output_path}.{self.config.save_format}"
            
            os.makedirs(os.path.dirname(output_file) if os.path.dirname(output_file) else '.', exist_ok=True)
            
            if self.config.save_format == "html":
                # For HTML, we could use plotly or save as interactive HTML
                fig.savefig(output_file, dpi=self.config.dpi, bbox_inches='tight')
            elif self.config.save_format == "mp4":
                # Save animation (would need to be recorded separately)
                fig.savefig(output_file, dpi=self.config.dpi, bbox_inches='tight')
            else:
                fig.savefig(output_file, dpi=self.config.dpi, bbox_inches='tight')
            
            # Compute statistics
            stats = self._compute_spike_statistics()
            
            result["plots"] = "multi-panel visualization"
            result["statistics"] = stats
            result["output_files"].append(output_file)
            result["success"] = True
            
            # Store figure for potential later use
            result["figure"] = fig
            
        except Exception as e:
            result["error"] = str(e)
            result["success"] = False
        
        return result
    
    def _plot_raster(self, ax):
        """Plot spike raster"""
        if not self.spike_traces:
            ax.text(0.5, 0.5, 'No spike data', ha='center', va='center', transform=ax.transAxes)
            return
        
        colors = self._get_neuron_colors()
        
        for i, spike_times in enumerate(self.spike_traces):
            # Plot spikes for each neuron
            ax.vlines(spike_times, i - 0.4, i + 0.4, 
                     colors=colors[i], linewidths=1, alpha=0.7)
        
        ax.set_xlabel("Time (s)")
        ax.set_ylabel("Neuron Index")
        ax.set_title("Spike Raster Plot", fontsize=12, fontweight='bold')
        ax.grid(True, alpha=0.3)
        ax.set_xlim(0, max([spike[-1] if len(spike) > 0 else 0 for spike in self.spike_traces]) + 1)
        ax.set_ylim(-0.5, len(self.spike_traces) - 0.5)
        
        if self.config.show_legend:
            # Create custom legend
            from matplotlib.patches import Patch
            legend_elements = []
            for i, (neuron_id, neuron_type) in enumerate(self._get_neuron_info()):
                color = self._get_neuron_color(i, neuron_type)
                legend_elements.append(Patch(facecolor=color, label=f'Neuron {neuron_id} ({neuron_type})', alpha=0.7))
            
            ax.legend(handles=legend_elements, loc='upper right', fontsize=8)
    
    def _plot_firing_rates(self, ax):
        """Plot firing rates over time"""
        if not self.firing_rates:
            ax.text(0.5, 0.5, 'No firing rate data', ha='center', va='center', transform=ax.transAxes)
            return
        
        n_neurons = len(self.firing_rates)
        time_points = np.linspace(0, max([spike[-1] if len(spike) > 0 else 0 for spike in self.spike_traces]), 100)
        
        for i in range(n_neurons):
            color = self._get_neuron_color(i, self._get_neuron_type(i))
            ax.plot(time_points, np.ones_like(time_points) * self.firing_rates[i],
                   color=color, linewidth=2, alpha=0.7, label=f'N{i}' if n_neurons <= 10 else None)
        
        ax.set_xlabel("Time (s)")
        ax.set_ylabel("Firing Rate (Hz)")
        ax.set_title("Firing Rate Over Time", fontsize=12, fontweight='bold')
        ax.grid(True, alpha=0.3)
        ax.set_ylim(0, max(self.firing_rates) * 1.2 if self.firing_rates else 10)
        
        if self.config.show_legend and n_neurons <= 10:
            ax.legend(loc='upper right', fontsize=8)
    
    def _plot_isi_histogram(self, ax):
        """Plot ISI histogram"""
        all_isis = []
        
        for spike_times in self.spike_traces:
            isis = self._calculate_isi(spike_times)
            all_isis.extend(isis)
        
        if all_isis:
            ax.hist(all_isis, bins=50, alpha=0.7, edgecolor='black', color='skyblue')
            ax.set_xlabel("ISI (s)")
            ax.set_ylabel("Count")
            ax.set_title("Inter-Spike Interval Distribution", fontsize=12, fontweight='bold')
            ax.grid(True, alpha=0.3)
        else:
            ax.text(0.5, 0.5, 'No ISI data', ha='center', va='center', transform=ax.transAxes)
    
    def _plot_frequency_spectrum(self, ax):
        """Plot frequency spectrum of spike train"""
        if not self.spike_traces:
            ax.text(0.5, 0.5, 'No data for frequency analysis', ha='center', va='center', transform=ax.transAxes)
            return
        
        # Combine all spike trains for spectral analysis
        all_spikes = []
        for spike_times in self.spike_traces:
            all_spikes.extend(spike_times)
        
        if len(all_spikes) > 1:
            # Create spike train binary vector
            dt = self.config.bin_size
            duration = max(all_spikes)
            t = np.arange(0, duration, dt)
            spike_train = np.zeros_like(t)
            
            for spike_time in all_spikes:
                idx = int(spike_time / dt)
                if idx < len(spike_train):
                    spike_train[idx] = 1
            
            # Compute power spectrum
            fs = 1.0 / dt
            f, Pxx = signal.welch(spike_train, fs, nperseg=min(1024, len(spike_train)))
            
            ax.semilogy(f, Pxx, linewidth=1)
            ax.set_xlabel("Frequency (Hz)")
            ax.set_ylabel("Power Spectral Density")
            ax.set_title("Frequency Spectrum", fontsize=12, fontweight='bold')
            ax.grid(True, alpha=0.3)
            ax.set_xlim(0, fs/2)
        else:
            ax.text(0.5, 0.5, 'Insufficient data for frequency analysis', ha='center', va='center', transform=ax.transAxes)
    
    def _plot_autocorrelation(self, ax):
        """Plot autocorrelation of spike train"""
        if not self.spike_traces:
            ax.text(0.5, 0.5, 'No data for autocorrelation', ha='center', va='center', transform=ax.transAxes)
            return
        
        # Combine all spike trains
        all_spikes = []
        for spike_times in self.spike_traces:
            all_spikes.extend(spike_times)
        
        if len(all_spikes) > 10:
            dt = self.config.bin_size
            duration = max(all_spikes)
            t = np.arange(0, duration, dt)
            spike_train = np.zeros_like(t)
            
            for spike_time in all_spikes:
                idx = int(spike_time / dt)
                if idx < len(spike_train):
                    spike_train[idx] = 1
            
            # Compute autocorrelation
            autocorr = signal.correlate(spike_train - np.mean(spike_train), 
                                       spike_train - np.mean(spike_train), mode='full')
            autocorr = autocorr[len(autocorr)//2:]
            
            lags = np.arange(0, len(autocorr)) * dt
            
            ax.plot(lags[:1000], autocorr[:1000], linewidth=1)
            ax.set_xlabel("Lag (s)")
            ax.set_ylabel("Autocorrelation")
            ax.set_title("Spike Train Autocorrelation", fontsize=12, fontweight='bold')
            ax.grid(True, alpha=0.3)
        else:
            ax.text(0.5, 0.5, 'Insufficient data for autocorrelation', ha='center', va='center', transform=ax.transAxes)
    
    def _plot_cross_correlation(self, ax):
        """Plot cross-correlation between neurons"""
        if len(self.spike_traces) < 2:
            ax.text(0.5, 0.5, 'Need at least 2 neurons for cross-correlation', ha='center', va='center', transform=ax.transAxes)
            return
        
        # Simple cross-correlation between first two neurons
        if len(self.spike_traces) >= 2:
            spikes1 = self.spike_traces[0]
            spikes2 = self.spike_traces[1]
            
            dt = self.config.bin_size
            duration = max(spikes1[-1], spikes2[-1])
            t = np.arange(0, duration, dt)
            
            train1 = np.zeros_like(t)
            train2 = np.zeros_like(t)
            
            for spike in spikes1:
                idx = int(spike / dt)
                if idx < len(train1):
                    train1[idx] = 1
            
            for spike in spikes2:
                idx = int(spike / dt)
                if idx < len(train2):
                    train2[idx] = 1
            
            # Compute cross-correlation
            cross_corr = signal.correlate(train1 - np.mean(train1), 
                                         train2 - np.mean(train2), mode='full')
            
            lags = np.arange(-len(cross_corr)//2, len(cross_corr)//2) * dt
            cross_corr = cross_corr[len(cross_corr)//2:]
            
            ax.plot(lags[:len(cross_corr)//2], cross_corr[:len(cross_corr)//2], linewidth=1)
            ax.set_xlabel("Lag (s)")
            ax.set_ylabel("Cross-correlation")
            ax.set_title("Cross-correlation Between Neurons", fontsize=12, fontweight='bold')
            ax.grid(True, alpha=0.3)
    
    def _plot_cumulative_spikes(self, ax):
        """Plot cumulative spike count"""
        if not self.spike_traces:
            ax.text(0.5, 0.5, 'No spike data', ha='center', va='center', transform=ax.transAxes)
            return
        
        time_points = []
        cumulative_counts = []
        
        all_spikes = []
        for spike_times in self.spike_traces:
            all_spikes.extend(spike_times)
        
        all_spikes.sort()
        
        cumulative = 0
        for spike_time in all_spikes:
            time_points.append(spike_time)
            cumulative += 1
            cumulative_counts.append(cumulative)
        
        ax.plot(time_points, cumulative_counts, linewidth=2, color='darkred')
        ax.set_xlabel("Time (s)")
        ax.set_ylabel("Cumulative Spike Count")
        ax.set_title("Cumulative Spike Count", fontsize=12, fontweight='bold')
        ax.grid(True, alpha=0.3)
        ax.set_xlim(0, max(time_points) if time_points else 1)
    
    def _plot_pattern_analysis(self, ax):
        """Plot pattern analysis results"""
        if not self.spike_traces:
            ax.text(0.5, 0.5, 'No pattern data', ha='center', va='center', transform=ax.transAxes)
            return
        
        # Calculate pattern statistics
        burst_counts = []
        regular_counts = []
        
        for i, spike_times in enumerate(self.spike_traces):
            pattern = self.cluster_labels[i]
            burst_counts.append(np.sum(pattern == 1))
            regular_counts.append(np.sum(pattern == 0))
        
        # Create bar chart
        x = np.arange(len(self.spike_traces))
        width = 0.35
        
        ax.bar(x - width/2, regular_counts, width, label='Regular', alpha=0.7, color='blue')
        ax.bar(x + width/2, burst_counts, width, label='Burst', alpha=0.7, color='red')
        
        ax.set_xlabel("Neuron Index")
        ax.set_ylabel("Pattern Count")
        ax.set_title("Pattern Analysis", fontsize=12, fontweight='bold')
        ax.set_xticks(x)
        ax.set_xticklabels([f'N{i}' for i in range(len(self.spike_traces))])
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def _plot_statistics_panel(self, ax):
        """Plot statistics panel with key metrics"""
        stats = self._compute_spike_statistics()
        
        # Display statistics in a grid
        ax.text(0.1, 0.9, "Spike Statistics", fontsize=14, fontweight='bold', transform=ax.transAxes)
        
        y_pos = 0.8
        line_height = 0.05
        
        for key, value in stats.items():
            if isinstance(value, (int, float)):
                if isinstance(value, float):
                    text = f"{key}: {value:.4f}"
                else:
                    text = f"{key}: {value}"
            elif isinstance(value, dict):
                continue  # Skip nested dicts for now
            else:
                text = f"{key}: {str(value)[:30]}"
            
            ax.text(0.1, y_pos, text, transform=ax.transAxes, fontsize=10)
            y_pos -= line_height
        
        ax.set_xlim(0, 1)
        ax.set_ylim(0, 1)
        ax.axis('off')
    
    def _get_neuron_colors(self):
        """Get colors for neurons based on configuration"""
        n_neurons = len(self.spike_traces)
        
        if self.config.color_scheme == "activity":
            colors = self.activity_cmap(self.firing_rates)
        elif self.config.color_scheme == "cluster":
            cluster_ids = []
            for i in range(n_neurons):
                cluster_id = int(self.cluster_labels[i].mean())
                cluster_ids.append(cluster_id)
            colors = self.cluster_cmap(np.array(cluster_ids) / max(1, max(cluster_ids)))
        elif self.config.color_scheme == "gradient":
            colors = self.gradient_cmap(np.array(self.firing_rates) / max(1, max(self.firing_rates)))
        else:  # default
            colors = self.default_cmap(np.linspace(0, 1, n_neurons))
        
        return colors
    
    def _get_neuron_color(self, neuron_id: int, neuron_type: str = "unknown"):
        """Get color for specific neuron"""
        color_map = {
            'excitatory': '#FF6B6B',
            'inhibitory': '#4ECDC4',
            'modulatory': '#45B7D1',
            'sensory': '#96CEB4',
            'motor': '#FFEAA7',
            'associative': '#DDA0DD',
            'unknown': '#CCCCCC'
        }
        
        return color_map.get(neuron_type, color_map['unknown'])
    
    def _get_neuron_type(self, neuron_id: int) -> str:
        """Get neuron type from metadata"""
        if neuron_id < len(self.spike_traces):
            # Extract type from internal tracking (would need to store this properly)
            return "unknown"
        return "unknown"
    
    def _get_neuron_info(self):
        """Get neuron information (id, type)"""
        info = []
        for i in range(len(self.spike_traces)):
            info.append((i, self._get_neuron_type(i)))
        return info
    
    def _compute_spike_statistics(self) -> Dict[str, Any]:
        """Compute comprehensive spike statistics"""
        stats = {
            "total_spikes": 0,
            "total_neurons": len(self.spike_traces),
            "mean_firing_rate": 0.0,
            "max_firing_rate": 0.0,
            "min_firing_rate": 0.0,
            "median_isi": 0.0,
            "mean_isi": 0.0,
            "isi_std": 0.0,
            "bursting_neurons": 0,
            "regular_neurons": 0,
            "inactive_neurons": 0
        }
        
        # Total spikes
        for spike_times in self.spike_traces:
            stats["total_spikes"] += len(spike_times)
        
        # Firing rate statistics
        if self.firing_rates:
            stats["mean_firing_rate"] = np.mean(self.firing_rates)
            stats["max_firing_rate"] = np.max(self.firing_rates)
            stats["min_firing_rate"] = np.min(self.firing_rates)
        
        # ISI statistics
        all_isis = []
        burst_neurons = 0
        regular_neurons = 0
        inactive_neurons = 0
        
        for i, spike_times in enumerate(self.spike_traces):
            isis = self._calculate_isi(spike_times)
            all_isis.extend(isis)
            
            if len(isis) == 0:
                inactive_neurons += 1
            elif len(np.unique(self.cluster_labels[i])) == 1 and self.cluster_labels[i][0] == 1:
                burst_neurons += 1
            else:
                regular_neurons += 1
        
        # ISI statistics
        if all_isis:
            stats["median_isi"] = np.median(all_isis)
            stats["mean_isi"] = np.mean(all_isis)
            stats["isi_std"] = np.std(all_isis)
            stats["isi_cv"] = stats["isi_std"] / stats["mean_isi"] if stats["mean_isi"] > 0 else 0.0
        
        # Neuron classification
        stats["bursting_neurons"] = burst_neurons
        stats["regular_neurons"] = regular_neurons
        stats["inactive_neurons"] = inactive_neurons
        
        return stats
    
    def export_spike_data(self, filename: str) -> str:
        """
        Export spike data to file.
        
        Args:
            filename: Output filename
            
        Returns:
            Path to saved file
        """
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        export_data = {
            "config": asdict(self.config),
            "spike_traces": []
        }
        
        for i, spike_times in enumerate(self.spike_traces):
            export_data["spike_traces"].append({
                "neuron_id": i,
                "spike_times": spike_times.tolist(),
                "neuron_type": self._get_neuron_type(i),
                "isi": self.isi_distributions[i].tolist() if i < len(self.isi_distributions) else [],
                "pattern": self.cluster_labels[i].tolist() if i < len(self.cluster_labels) else []
            })
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2)
        
        return filename
    
    def add_real_time_spikes(self, spike_times: List[float], neuron_id: int = 0):
        """
        Add spikes in real-time (for live monitoring).
        
        Args:
            spike_times: List of spike times (seconds)
            neuron_id: Neuron identifier
        """
        if self.is_running and hasattr(self, 'ax_raster') and hasattr(self, 'raster_scatter'):
            # Add to data
            self.add_spike_data(np.array(spike_times), neuron_id)
            
            # Update real-time plot
            current_time = max(spike_times) if spike_times else 0
            self._update_raster_plot(current_time)
            
            # Force plot update
            plt.draw()
            plt.pause(0.001)


def create_spike_train_viewer(config_dict: Dict[str, Any] = None) -> SpikeTrainViewer:
    """
    Factory function to create SpikeTrainViewer with configuration.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        Configured SpikeTrainViewer instance
    """
    if config_dict is None:
        config_dict = {}
    
    config = SpikeTrainViewerConfig(**config_dict)
    return SpikeTrainViewer(config)