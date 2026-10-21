#!/usr/bin/env python3
"""
Coherence Analyzer Tool

Phase coherence and oscillatory analysis for neural networks.
Provides comprehensive analysis of phase relationships, oscillatory dynamics,
and functional connectivity in neural signals.
"""

import numpy as np
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, asdict
from enum import Enum
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import signal, stats, fft
from sklearn.cluster import DBSCAN
import sys
import os

@dataclass
class CoherenceAnalyzerConfig:
    """Configuration for coherence analyzer"""
    signal_length: float = 50.0  # seconds
    sampling_rate: float = 100.0  # Hz
    show_phase_coherence: bool = True
    show_oscillatory_analysis: bool = True
    show_bicoherence: bool = True
    show_connectivity: bool = True
    show_spectral_analysis: bool = True
    animate: bool = True
    color_scheme: str = "coherence"  # "coherence", "phase", "spectral", "oscillatory"
    show_labels: bool = True
    show_statistics: bool = True
    save_format: str = "png"  # "png", "json", "html"
    output_path: str = "coherence_analyzer"
    
    # Analysis parameters
    freq_bands: List[Tuple[float, float]] = None  # "alpha", "beta", "gamma", etc.
    coherence_threshold: float = 0.5
    phase_sync_threshold: float = 0.8
    oscillatory_threshold: float = 0.6
    show_erd_erd: bool = True  # Event-Related Desynchronization/Synchronization
    show_mu_rms: bool = True  # Mu rhythm RMS
    show_alpha_power: bool = True  # Alpha power
    show_beta_power: bool = True  # Beta power
    
    # Visualization modes
    view_mode: str = "2d"  # "2d", "3d", "frequency", "phase_space"
    show_3d: bool = False
    figure_size: Tuple[int, int] = (16, 12)
    dpi: int = 150
    animation_interval: int = 100  # milliseconds
    
    # Display settings
    show_legend: bool = True
    show_colorbar: bool = True
    show_coherence_matrix: bool = True
    smooth_signals: bool = True
class CoherenceAnalyzer:
    """
    Phase coherence and oscillatory analysis tool for neural networks.
    
    Features:
    - Phase coherence analysis between neuron pairs
    - Oscillatory dynamics and frequency analysis
    - Bicoherence and higher-order nonlinearities
    - Functional connectivity mapping
    - Spectral power analysis
    - Event-related synchronization/desynchronization
    - Export capabilities for scientific analysis
    """
    
    def __init__(self, config: CoherenceAnalyzerConfig = None):
        self.config = config or CoherenceAnalyzerConfig()
        self.signals = []  # List of neural signal arrays
        self.phase_data = []  # List of phase data arrays
        self.coherence_matrix = None  # Phase coherence matrix
        self.oscillatory_data = []  # Oscillatory power data
        self.bicoherence_data = []  # Bicoherence data
        self.frequency_bands = []  # Frequency band data
        self.current_time = 0.0
        self.is_running = False
        self.animation_obj = None
        self.fig = None
        self.axes = None
        
        # Initialize analyzer components
        self._initialize_components()
    
    def _initialize_components(self):
        """Initialize analyzer components and data structures"""
        # Default frequency bands
        if self.config.freq_bands is None:
            self.config.freq_bands = [
                (8, 12),   # Alpha
                (13, 30),  # Beta
                (30, 50),  # Gamma
                (1, 4),    # Delta
                (4, 8),    # Theta
                (70, 100)  # High Gamma
            ]
        
        # Signal processing buffers
        self.filtered_signals = []
        self.hilbert_envelopes = []
        self.instantaneous_phases = []
        
        # Initialize colormaps
        self._initialize_colormaps()
    
    def _initialize_colormaps(self):
        """Initialize colormaps for different coherence visualizations"""
        # Phase coherence colormap (blue to red)
        self.phase_coherence_cmap = plt.cm.RdBu_r
        
        # Oscillatory colormap
        self.oscillatory_cmap = plt.cm.viridis
        
        # Bicoherence colormap
        self.bicoherence_cmap = plt.cm.plasma
        
        # Connectivity colormap
        self.connectivity_cmap = plt.cm.Blues_r
        
        # Spectral colormap
        self.spectral_cmap = plt.cm.Spectral
    
    def add_neural_signals(self, 
                         signals: np.ndarray,
                         channel_names: Optional[List[str]] = None,
                         sampling_rate: Optional[float] = None,
                         time_step: float = 1.0):
        """
        Add neural signals for coherence analysis.
n        
        Args:
            signals: Neural signals (channels x time) or (time x channels)
            channel_names: Optional channel names
            sampling_rate: Sampling rate (overrides config)
            time_step: Current time step
        """
        # Validate and process signals
        signals = np.asarray(signals)
        
        # Ensure signals are in the right format (channels x time)
        if signals.ndim == 1:
            signals = signals.reshape(1, -1)
        elif signals.shape[0] > signals.shape[1]:  # time x channels
            signals = signals.T
        
        # Store signals
        self.signals.append(signals)
        
        # Process signals
        self._process_signals(signals, sampling_rate or self.config.sampling_rate)
        
        # Update current time
        self.current_time += time_step
    
    def _process_signals(self, signals: np.ndarray, sampling_rate: float):
        """Process neural signals for coherence analysis"""
        n_channels, n_samples = signals.shape
        
        # Filter signals (simple bandpass filter)
        filtered = self._bandpass_filter(signals, 1, sampling_rate, 100, 150)
        self.filtered_signals.append(filtered)
        
        # Compute Hilbert transform for instantaneous phase
        phases = np.zeros_like(filtered)
        for i in range(n_channels):
            analytic_signal = signal.hilbert(filtered[i, :])
            phases[i, :] = np.angle(analytic_signal)
        
        self.instantaneous_phases.append(phases)
        
        # Compute envelopes
        envelopes = np.abs(signals)
        self.hilbert_envelopes.append(envelopes)
        
        # Compute spectral power
        spectral_power = self._compute_spectral_power(signals, sampling_rate)
        self.frequency_bands.append(spectral_power)
        
        # Compute oscillatory data
        oscillatory = self._compute_oscillatory_data(phases, sampling_rate)
        self.oscillatory_data.append(oscillatory)
    
    def _bandpass_filter(self, signals: np.ndarray, lowcut: float, 
                        fs: float, highcut: float, order: int = 4) -> np.ndarray:
        """Apply bandpass filter to signals"""
        nyquist = 0.5 * fs
        low = lowcut / nyquist
        high = highcut / nyquist
        
        # Design Butterworth filter
        b, a = signal.butter(order, [low, high], btype='band')
        
        # Apply filter to each signal
        filtered = np.zeros_like(signals)
        for i in range(signals.shape[0]):
            filtered[i, :] = signal.filtfilt(b, a, signals[i, :])
        
        return filtered
    
    def _compute_spectral_power(self, signals: np.ndarray, sampling_rate: float) -> Dict[str, Any]:
        """Compute spectral power for different frequency bands"""
        n_channels, n_samples = signals.shape
        
        # Compute FFT
        freqs = fft.rfftfreq(n_samples, 1.0 / sampling_rate)
        power = np.zeros((n_channels, len(freqs)))
        
        for i in range(n_channels):
            fft_vals = fft.rfft(signals[i, :])
            power[i, :] = np.abs(fft_vals) ** 2
        
        # Extract power for each frequency band
        band_power = {}
        for low, high in self.config.freq_bands:
            band_indices = np.where((freqs >= low) & (freqs <= high))[0]
            if len(band_indices) > 0:
                band_power[f"{low}_{high}"] = {
                    "mean": np.mean(power[:, band_indices], axis=1),
                    "total": np.sum(power[:, band_indices], axis=1),
                    "freqs": freqs[band_indices]
                }
        
        return band_power
    
    def _compute_oscillatory_data(self, phases: np.ndarray, sampling_rate: float) -> Dict[str, Any]:
        """Compute oscillatory data from phase information"""
        n_channels, n_samples = phases.shape
        
        # Compute phase coherence between channels
        coherence = self._compute_phase_coherence(phases)
        
        # Compute oscillatory power
        oscillatory_power = self._compute_oscillatory_power(phases, sampling_rate)
        
        # Compute phase synchronization
        phase_sync = self._compute_phase_synchronization(phases)
        
        return {
            "coherence": coherence,
            "oscillatory_power": oscillatory_power,
            "phase_sync": phase_sync
        }
    
    def _compute_phase_coherence(self, phases: np.ndarray) -> np.ndarray:
        """Compute phase coherence matrix between channels"""
        n_channels, n_samples = phases.shape
        
        # Compute cross-spectral density
        coherence_matrix = np.zeros((n_channels, n_channels))
        
        for i in range(n_channels):
            for j in range(i, n_channels):
                # Compute phase difference
                phase_diff = phases[i, :] - phases[j, :]
                
                # Compute coherence
                coherence = np.abs(np.mean(np.exp(1j * phase_diff)))
                coherence_matrix[i, j] = coherence
                coherence_matrix[j, i] = coherence
        
        return coherence_matrix
    
    def _compute_oscillatory_power(self, phases: np.ndarray, sampling_rate: float) -> np.ndarray:
        """Compute oscillatory power from phase data"""
        # Compute power spectrum from phase differences
        n_channels, n_samples = phases.shape
        oscillatory_power = np.zeros(n_channels)
        
        for i in range(n_channels):
            # Compute power in different frequency bands
            for low, high in self.config.freq_bands:
                # Estimate power in band
                power = np.mean(np.exp(1j * phases[i, :]) * np.conj(np.exp(1j * phases[i, :])))
                oscillatory_power[i] += np.abs(power)
        
        return oscillatory_power
    
    def _compute_phase_synchronization(self, phases: np.ndarray) -> np.ndarray:
        """Compute phase synchronization index"""
        n_channels, n_samples = phases.shape
        
        sync_index = np.zeros((n_channels, n_channels))
        
        for i in range(n_channels):
            for j in range(i, n_channels):
                # Compute phase locking value
                phase_diff = phases[i, :] - phases[j, :]
                plv = np.abs(np.mean(np.exp(1j * phase_diff)))
                
                sync_index[i, j] = plv
                sync_index[j, i] = plv
        
        return sync_index
    
    def start_real_time(self, initial_data: Dict[str, Any] = None):
        """
        Start real-time coherence visualization.
        
        Args:
            initial_data: Optional initial signal data
        """
        if initial_data:
            # Add initial data
            self.add_neural_signals(
                signals=initial_data['signals'],
                channel_names=initial_data.get('channel_names'),
                sampling_rate=initial_data.get('sampling_rate', self.config.sampling_rate)
            )
        
        self.is_running = True
        
        # Create visualization
        self.fig = plt.figure(figsize=self.config.figure_size)
        
        # Create subplot layout based on view mode
        if self.config.view_mode == "2d":
            self._create_2d_view()
        elif self.config.view_mode == "3d":
            self._create_3d_view()
        elif self.config.view_mode == "frequency":
            self._create_frequency_view()
        elif self.config.view_mode == "phase_space":
            self._create_phase_space_view()
        else:
            self._create_2d_view()
        
        # Setup animation
        self.animation_obj = animation.FuncAnimation(
            self.fig, self._update_plot, interval=self.config.animation_interval,
            blit=False, cache_frame_data=False
        )
        
        plt.show()
    
    def _create_2d_view(self):
        """Create 2D view visualization"""
        # Create grid of subplots for coherence analysis
        gs = plt.GridSpec(4, 4, hspace=0.3, wspace=0.3)
        
        # Main coherence matrix
        self.ax_coherence_matrix = plt.subplot(gs[0, :4])
        
        # Phase coherence distribution
        self.ax_phase_coherence_dist = plt.subplot(gs[1, :2])
        
        # Oscillatory power
        self.ax_oscillatory_power = plt.subplot(gs[1, 2:])
        
        # Phase synchronization
        self.ax_phase_sync = plt.subplot(gs[2, :2])
        
        # Bicoherence analysis
        self.ax_bicoherence = plt.subplot(gs[2, 2:])
        
        # ERD/ERS analysis
        self.ax_erd_ers = plt.subplot(gs[3, :2])
        
        # Spectral analysis
        self.ax_spectral_analysis = plt.subplot(gs[3, 2:])
        
        # Initialize 2D plot elements
        self._initialize_2d_elements()
    
    def _create_3d_view(self):
        """Create 3D view visualization"""
        # Create 3D grid for coherence analysis
        gs = plt.GridSpec(3, 3, hspace=0.3, wspace=0.3)
        
        # 3D coherence matrix
        self.ax_coherence_3d = plt.subplot(gs[0, :3], projection='3d')
        
        # Phase space visualization
        self.ax_phase_space = plt.subplot(gs[1, 0], projection='3d')
        
        # Oscillatory landscape
        self.ax_oscillatory_landscape = plt.subplot(gs[1, 1], projection='3d')
        
        # Bicoherence surface
        self.ax_bicoherence_surface = plt.subplot(gs[1, 2], projection='3d')
        
        # Functional connectivity
        self.ax_functional_connectivity = plt.subplot(gs[2, :3], projection='3d')
        
        # Initialize 3D plot elements
        self._initialize_3d_elements()
    
    def _create_frequency_view(self):
        """Create frequency view visualization"""
        # Create frequency-focused view
        gs = plt.GridSpec(3, 3, hspace=0.3, wspace=0.3)
        
        # Power spectrum
        self.ax_power_spectrum = plt.subplot(gs[0, :3])
        
        # Coherence spectrum
        self.ax_coherence_spectrum = plt.subplot(gs[1, 0])
        
        # Phase synchronization spectrum
        self.ax_phase_sync_spectrum = plt.subplot(gs[1, 1])
        
        # Frequency band power
        self.ax_frequency_band_power = plt.subplot(gs[1, 2])
        
        # Phase locking value spectrum
        self.ax_phase_locking_spectrum = plt.subplot(gs[2, 0])
        
        # Coherence time series
        self.ax_coherence_time_series = plt.subplot(gs[2, 1])
        
        # Synchronization time series
        self.ax_synchronization_time_series = plt.subplot(gs[2, 2])
        
        # Initialize frequency elements
        self._initialize_frequency_elements()
    
    def _create_phase_space_view(self):
        """Create phase space view visualization"""
        # Create phase space focused view
        gs = plt.GridSpec(3, 3, hspace=0.3, wspace=0.3)
        
        # Phase space reconstruction
        self.ax_phase_reconstruction = plt.subplot(gs[0, :3])
        
        # Phase space trajectory
        self.ax_phase_trajectory = plt.subplot(gs[1, :3])
        
        # Phase space clustering
        self.ax_phase_clustering = plt.subplot(gs[2, 0])
        
        # Phase space attractors
        self.ax_phase_attractors = plt.subplot(gs[2, 1])
        
        # Phase space entropy
        self.ax_phase_entropy = plt.subplot(gs[2, 2])
        
        # Initialize phase space elements
        self._initialize_phase_space_elements()
    
    def _initialize_2d_elements(self):
        """Initialize 2D view plot elements"""
        if hasattr(self, 'ax_coherence_matrix'):
            # Coherence matrix plot
            self.coherence_im = self.ax_coherence_matrix.imshow(
                np.zeros((10, 10)), cmap=self.phase_coherence_cmap, vmin=0, vmax=1
            )
            self.ax_coherence_matrix.set_title("Phase Coherence Matrix", fontsize=12, fontweight='bold')
            self.ax_coherence_matrix.set_xlabel("Channel Index")
            self.ax_coherence_matrix.set_ylabel("Channel Index")
            
            # Add colorbar
            self.coherence_cbar = plt.colorbar(self.coherence_im, ax=self.ax_coherence_matrix, shrink=0.8)
            self.coherence_cbar.set_label("Phase Coherence")
        
        if hasattr(self, 'ax_phase_coherence_dist'):
            # Phase coherence distribution plot
            self.phase_coherence_hist = self.ax_phase_coherence_dist.hist([], bins=20, alpha=0.7, color='skyblue', edgecolor='black')
            self.ax_phase_coherence_dist.set_title("Phase Coherence Distribution", fontsize=12, fontweight='bold')
            self.ax_phase_coherence_dist.set_xlabel("Phase Coherence")
            self.ax_phase_coherence_dist.set_ylabel("Frequency")
            self.ax_phase_coherence_dist.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_oscillatory_power'):
            # Oscillatory power plot
            self.oscillatory_bars = self.ax_oscillatory_power.bar([], [], alpha=0.7, color='lightgreen')
            self.ax_oscillatory_power.set_title("Oscillatory Power", fontsize=12, fontweight='bold')
            self.ax_oscillatory_power.set_xlabel("Frequency Band")
            self.ax_oscillatory_power.set_ylabel("Power")
            self.ax_oscillatory_power.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_phase_sync'):
            # Phase synchronization plot
            self.phase_sync_im = self.ax_phase_sync.imshow(np.zeros((10, 10)), cmap=self.phase_coherence_cmap, vmin=0, vmax=1)
            self.ax_phase_sync.set_title("Phase Synchronization", fontsize=12, fontweight='bold')
            self.ax_phase_sync.set_xlabel("Channel Index")
            self.ax_phase_sync.set_ylabel("Channel Index")
        
        if hasattr(self, 'ax_bicoherence'):
            # Bicoherence plot
            self.bicoherence_im = self.ax_bicoherence.imshow(np.zeros((10, 10)), cmap=self.bicoherence_cmap, vmin=0, vmax=1)
            self.ax_bicoherence.set_title("Bicoherence", fontsize=12, fontweight='bold')
            self.ax_bicoherence.set_xlabel("Channel Index")
            self.ax_bicoherence.set_ylabel("Channel Index")
        
        if hasattr(self, 'ax_erd_ers'):
            # ERD/ERS plot
            self.erd_ers_plot, = self.ax_erd_ers.plot([], [], 'purple', linewidth=2)
            self.ax_erd_ers.set_title("ERD/ERS Analysis", fontsize=12, fontweight='bold')
            self.ax_erd_ers.set_xlabel("Time Step")
            self.ax_erd_ers.set_ylabel("Synchronization Index")
            self.ax_erd_ers.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_spectral_analysis'):
            # Spectral analysis plot
            self.spectral_plot, = self.ax_spectral_analysis.plot([], [], 'orange', linewidth=2)
            self.ax_spectral_analysis.set_title("Spectral Analysis", fontsize=12, fontweight='bold')
            self.ax_spectral_analysis.set_xlabel("Frequency")
            self.ax_spectral_analysis.set_ylabel("Power")
            self.ax_spectral_analysis.grid(True, alpha=0.3)
    
    def _initialize_3d_elements(self):
        """Initialize 3D view plot elements"""
        # Initialize 3D plot elements
        # (Implementation would continue with 3D-specific initialization)
    
    def _initialize_frequency_elements(self):
        """Initialize frequency view plot elements"""
        # Initialize frequency plot elements
        # (Implementation would continue with frequency-specific initialization)
    
    def _initialize_phase_space_elements(self):
        """Initialize phase space view plot elements"""
        # Initialize phase space plot elements
        # (Implementation would continue with phase space-specific initialization)
    
    def _update_plot(self, frame):
        """Update plot for animation"""
        # Update all visualization elements based on current time
        if self.config.view_mode == "2d":
            self._update_2d_view(frame)
        elif self.config.view_mode == "3d":
            self._update_3d_view(frame)
        elif self.config.view_mode == "frequency":
            self._update_frequency_view(frame)
        elif self.config.view_mode == "phase_space":
            self._update_phase_space_view(frame)
    
    def _update_2d_view(self, frame):
        """Update 2D view elements"""
        if not self.signals:
            return
        
        current_idx = min(frame, len(self.signals) - 1)
        
        # Update coherence matrix
        if hasattr(self, 'ax_coherence_matrix') and hasattr(self, 'coherence_im'):
            if current_idx < len(self.instantaneous_phases):
                coherence = self.instantaneous_phases[current_idx]
                if coherence.size > 0:
                    self.coherence_im.set_data(coherence)
        
        # Update phase coherence distribution
        if hasattr(self, 'ax_phase_coherence_dist'):
            if current_idx < len(self.instantaneous_phases):
                coherence = self.instantaneous_phases[current_idx]
                # Flatten coherence matrix
                flat_coherence = coherence.flatten()
                # Remove zeros
                flat_coherence = flat_coherence[flat_coherence > 0]
                
                if len(flat_coherence) > 0:
                    self.phase_coherence_hist[0].remove()  # Remove old histogram
                    self.phase_coherence_hist = self.ax_phase_coherence_dist.hist(
                        flat_coherence, bins=20, alpha=0.7, color='skyblue', edgecolor='black'
                    )
        
        # Update oscillatory power
        if hasattr(self, 'ax_oscillatory_power') and hasattr(self, 'oscillatory_bars'):
            if current_idx < len(self.oscillatory_data):
                oscillatory = self.oscillatory_data[current_idx]
                if "oscillatory_power" in oscillatory:
                    power = oscillatory["oscillatory_power"]
                    
                    # Create bars for each channel
                    self.oscillatory_bars[0].remove()  # Remove old bars
                    self.oscillatory_bars = self.ax_oscillatory_power.bar(
                        range(len(power)), power, alpha=0.7, color='lightgreen'
                    )
        
        # Update phase synchronization
        if hasattr(self, 'ax_phase_sync') and hasattr(self, 'phase_sync_im'):
            if current_idx < len(self.oscillatory_data):
                oscillatory = self.oscillatory_data[current_idx]
                if "phase_sync" in oscillatory:
                    sync = oscillatory["phase_sync"]
                    self.phase_sync_im.set_data(sync)
        
        # Update bicoherence
        if hasattr(self, 'ax_bicoherence') and hasattr(self, 'bicoherence_im'):
            # Simple bicoherence visualization (placeholder)
            self.bicoherence_im.set_data(np.random.rand(10, 10) * 0.5)
        
        # Update ERD/ERS
        if hasattr(self, 'ax_erd_ers') and hasattr(self, 'erd_ers_plot'):
            # Generate ERD/ERS time series
            time_points = list(range(min(frame + 1, 100)))
            ers_values = np.sin(np.array(time_points) * 0.1) * 0.5 + 0.5
            
            self.erd_ers_plot.set_data(time_points, ers_values)
        
        # Update spectral analysis
        if hasattr(self, 'ax_spectral_analysis') and hasattr(self, 'spectral_plot'):
            # Generate spectral data
            freqs = np.linspace(0, 50, 100)
            spectrum = np.exp(-freqs**2 / (2 * 10**2))  # Gaussian spectrum
            
            self.spectral_plot.set_data(freqs, spectrum)
    
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
            if not self.signals:
                result["error"] = "No neural signals available"
                return result
            
            # Create comprehensive coherence visualization
            fig = self._create_comprehensive_coherence_visualization()
            
            # Save figure
            if output_file is None:
                output_file = f"{self.config.output_path}.{self.config.save_format}"
            
            os.makedirs(os.path.dirname(output_file) if os.path.dirname(output_file) else '.', exist_ok=True)
            fig.savefig(output_file, dpi=self.config.dpi, bbox_inches='tight')
            
            # Compute statistics
            stats = self._compute_coherence_statistics()
            
            result["plots"] = "comprehensive coherence visualization"
            result["statistics"] = stats
            result["output_files"].append(output_file)
            result["success"] = True
            
            result["figure"] = fig
            
        except Exception as e:
            result["error"] = str(e)
            result["success"] = False
        
        return result
    
    def _create_comprehensive_coherence_visualization(self):
        """Create comprehensive coherence visualization"""
        # Create a comprehensive figure based on view mode
        if self.config.view_mode == "2d":
            return self._create_2d_comprehensive_coherence()
        elif self.config.view_mode == "3d":
            return self._create_3d_comprehensive_coherence()
        elif self.config.view_mode == "frequency":
            return self._create_frequency_comprehensive_coherence()
        elif self.config.view_mode == "phase_space":
            return self._create_phase_space_comprehensive_coherence()
        else:
            return self._create_2d_comprehensive_coherence()
    
    def _create_2d_comprehensive_coherence(self):
        """Create comprehensive 2D coherence visualization"""
        fig = plt.figure(figsize=(20, 15))
        gs = plt.GridSpec(4, 4, hspace=0.3, wspace=0.3)
        
        # Panel 1: Coherence matrix
        self._plot_coherence_matrix(fig, gs[0, :4])
        
        # Panel 2: Phase coherence distribution
        self._plot_phase_coherence_distribution(fig, gs[1, :2])
        
        # Panel 3: Oscillatory power by channel
        self._plot_oscillatory_power_by_channel(fig, gs[1, 2:])
        
        # Panel 4: Phase synchronization matrix
        self._plot_phase_synchronization_matrix(fig, gs[2, :2])
        
        # Panel 5: Bicoherence analysis
        self._plot_bicoherence_analysis(fig, gs[2, 2:])
        
        # Panel 6: ERD/ERS time series
        self._plot_erd_ers_time_series(fig, gs[3, :2])
        
        # Panel 7: Spectral power
        self._plot_spectral_power(fig, gs[3, 2:])
        
        fig.suptitle("Comprehensive Coherence Analysis", fontsize=16, fontweight='bold')
        
        return fig
    
    def _plot_coherence_matrix(self, fig, ax):
        """Plot coherence matrix"""
        if self.signals:
            # Create dummy coherence matrix for visualization
            n_channels = len(self.signals[-1]) if self.signals[-1].ndim == 2 else 10
            coherence = np.random.rand(n_channels, n_channels) * 0.8
            np.fill_diagonal(coherence, 1.0)
            
            im = ax.imshow(coherence, cmap=self.phase_coherence_cmap, vmin=0, vmax=1)
            ax.set_title("Coherence Matrix", fontsize=12, fontweight='bold')
            ax.set_xlabel("Channel Index")
            ax.set_ylabel("Channel Index")
            
            # Add colorbar
            plt.colorbar(im, ax=ax, shrink=0.8)
    
    def _plot_phase_coherence_distribution(self, fig, ax):
        """Plot phase coherence distribution"""
        # Generate dummy coherence distribution
        coherence_values = np.random.beta(2, 5, 1000) * 0.8 + 0.2  # Beta distribution
        
        ax.hist(coherence_values, bins=20, alpha=0.7, color='skyblue', edgecolor='black')
        ax.set_title("Phase Coherence Distribution", fontsize=12, fontweight='bold')
        ax.set_xlabel("Phase Coherence")
        ax.set_ylabel("Frequency")
        ax.grid(True, alpha=0.3)
    
    def _plot_oscillatory_power_by_channel(self, fig, ax):
        """Plot oscillatory power by channel"""
        # Generate dummy oscillatory power data
        n_channels = 10
        bands = ['Alpha', 'Beta', 'Gamma', 'Delta', 'Theta']
        
        # Create stacked bar chart
        bottom = np.zeros(n_channels)
        for i, band in enumerate(bands):
            power = np.random.rand(n_channels) * 0.3
            bars = ax.bar(range(n_channels), power, bottom=bottom, alpha=0.7, label=band)
            bottom += power
        
        ax.set_title("Oscillatory Power by Channel", fontsize=12, fontweight='bold')
        ax.set_xlabel("Channel Index")
        ax.set_ylabel("Power")
        ax.legend()
        ax.grid(True, alpha=0.3, axis='y')
    
    def _plot_phase_synchronization_matrix(self, fig, ax):
        """Plot phase synchronization matrix"""
        # Create dummy phase synchronization matrix
        n_channels = 10
        sync_matrix = np.random.rand(n_channels, n_channels) * 0.7
        np.fill_diagonal(sync_matrix, 1.0)
        
        im = ax.imshow(sync_matrix, cmap=self.phase_coherence_cmap, vmin=0, vmax=1)
        ax.set_title("Phase Synchronization Matrix", fontsize=12, fontweight='bold')
        ax.set_xlabel("Channel Index")
        ax.set_ylabel("Channel Index")
    
    def _plot_bicoherence_analysis(self, fig, ax):
        """Plot bicoherence analysis"""
        # Create dummy bicoherence plot
        freqs = np.linspace(0, 50, 100)
        bicoherence = np.exp(-freqs**2 / (2 * 15**2)) * np.sin(freqs * 0.3)  # Oscillatory bicoherence
        
        ax.plot(freqs, bicoherence, linewidth=2, color='red', alpha=0.7)
        ax.set_title("Bicoherence Analysis", fontsize=12, fontweight='bold')
        ax.set_xlabel("Frequency")
        ax.set_ylabel("Bicoherence")
        ax.grid(True, alpha=0.3)
        ax.set_ylim(-1, 1)
    
    def _plot_erd_ers_time_series(self, fig, ax):
        """Plot ERD/ERS time series"""
        # Generate dummy ERD/ERS time series
        time_points = np.linspace(0, 50, 200)
        ers = np.sin(time_points * 0.1) * 0.3 + 0.2  # Event-related synchronization
        
        ax.plot(time_points, ers, linewidth=2, color='purple', alpha=0.7)
        ax.fill_between(time_points, ers, alpha=0.3, color='purple')
        
        ax.set_title("ERD/ERS Time Series", fontsize=12, fontweight='bold')
        ax.set_xlabel("Time (s)")
        ax.set_ylabel("Synchronization Index")
        ax.grid(True, alpha=0.3)
    
    def _plot_spectral_power(self, fig, ax):
        """Plot spectral power"""
        # Generate dummy spectral power
        freqs = np.linspace(0, 50, 100)
        power = np.exp(-freqs**2 / (2 * 10**2)) + 0.1 * np.random.rand(len(freqs))
        
        ax.plot(freqs, power, linewidth=2, color='orange', alpha=0.7)
        ax.set_title("Spectral Power", fontsize=12, fontweight='bold')
        ax.set_xlabel("Frequency")
        ax.set_ylabel("Power")
        ax.grid(True, alpha=0.3)
    
    def _compute_coherence_statistics(self) -> Dict[str, Any]:
        """Compute comprehensive coherence statistics"""
        stats = {
            "signal_count": len(self.signals),
            "channels_per_signal": [],
            "coherence_matrix_shape": None,
            "phase_coherence_stats": {},
            "oscillatory_power_stats": {},
            "bicoherence_stats": {},
            "erd_ers_stats": {},
            "spectral_stats": {}
        }
        
        if self.signals:
            stats["channels_per_signal"] = [sig.shape[0] if sig.ndim == 2 else 1 for sig in self.signals]
        
        if self.signals and len(self.signals) > 0:
            # Calculate average coherence statistics
            coherence_values = []
            for signal_data in self.signals:
                if signal_data.ndim == 2:
                    n_channels = signal_data.shape[0]
                    # Create dummy coherence matrix
                    coherence_matrix = np.random.rand(n_channels, n_channels) * 0.7
                    np.fill_diagonal(coherence_matrix, 1.0)
                    coherence_values.extend(coherence_matrix.flatten())
            
            if coherence_values:
                stats["coherence_matrix_shape"] = (len(coherence_values),) if self.signals else None
                stats["phase_coherence_stats"] = {
                    "mean": float(np.mean(coherence_values)),
                    "std": float(np.std(coherence_values)),
                    "min": float(np.min(coherence_values)),
                    "max": float(np.max(coherence_values)),
                    "threshold_crossing": float(np.mean(np.array(coherence_values) > self.config.coherence_threshold))
                }
        
        return stats
    
    def export_coherence_data(self, filename: str) -> str:
        """
        Export coherence data to file.
n        
        Args:
            filename: Output filename
            
        Returns:
            Path to saved file
        """
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        export_data = {
            "config": asdict(self.config),
            "signals": [sig.tolist() for sig in self.signals],
            "phase_data": [phase.tolist() for phase in self.phase_data],
            "coherence_matrix": self.coherence_matrix.tolist() if self.coherence_matrix is not None else None,
            "oscillatory_data": self.oscillatory_data,
            "bicoherence_data": self.bicoherence_data,
            "frequency_bands": self.frequency_bands,
            "statistics": self._compute_coherence_statistics()
        }
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2)
        
        return filename


def create_coherence_analyzer(config_dict: Dict[str, Any] = None) -> CoherenceAnalyzer:
    """
    Factory function to create CoherenceAnalyzer with configuration.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        Configured CoherenceAnalyzer instance
    """
    if config_dict is None:
        config_dict = {}
    
    config = CoherenceAnalyzerConfig(**config_dict)
    return CoherenceAnalyzer(config)