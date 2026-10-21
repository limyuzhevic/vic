#!/usr/bin/env python3
"""
Signal Processor Tool

Signal processing for neural signals in NLM.
Provides comprehensive signal processing capabilities for neural data.
"""

import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as signal
import scipy.fft as fft
from scipy import stats
import pandas as pd
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, asdict
from enum import Enum
import sys
import os

@dataclass
class SignalProcessorConfig:
    """Configuration for signal processor"""
    sampling_rate: float = 1000.0  # Hz
    signal_length: float = 10.0  # seconds
    filter_type: str = "butter"  # "butter", "fir", "kalman"
    filter_order: int = 4
    frequency_bands: List[Tuple[float, float]] = None  # "delta", "theta", "alpha", "beta", "gamma"
    notch_frequency: float = 50.0  # Hz (for power line interference)
    notch_width: float = 5.0  # Hz
    detrend: bool = True
    normalize: bool = True
    high_pass_freq: float = 0.5  # Hz
    low_pass_freq: float = 200.0  # Hz
    apply_window: bool = True
    window_type: str = "hann"  # "hann", "hamming", "blackman", "rect"
    resample_rate: Optional[float] = None
    epoching: bool = False
    epoch_length: float = 1.0  # seconds
    epoch_overlap: float = 0.5  # overlap
    feature_extraction: bool = True
    feature_types: List[str] = None  # "fft", "wavelet", "stft", "correlation"
    output_path: str = "processed_signals"
    
    # Visualization
    show_spectrum: bool = True
    show_waveform: bool = True
    show_filter_response: bool = True
    show_frequency_analysis: bool = True
    show_time_frequency_analysis: bool = True
    
    # Processing options
    remove_artifacts: bool = True
    interpolate_missing: bool = True
    baseline_correction: bool = True
    motion_correction: bool = True
class SignalProcessor:
    """
    Signal processor tool for NLM neural signal processing.
    
    Features:
    - Filter design and application
    - Frequency analysis
    - Feature extraction
    - Artifact removal
    - Epoch creation
    - Spectral analysis
    - Wavelet transform
    - Short-time Fourier transform
    - Correlation analysis
    - Export capabilities for scientific analysis
    """
    
    def __init__(self, config: SignalProcessorConfig = None):
        self.config = config or SignalProcessorConfig()
        self.signals = []  # List of signal arrays
        self.filtered_signals = []  # List of filtered signal arrays
        self.features = []  # List of feature arrays
        self.processed_signals = []  # List of processed signal arrays
        self.epochs = []  # List of epochs
        self.frequency_data = []  # List of frequency domain data
        
        # Initialize processor components
        self._initialize_components()
    
    def _initialize_components(self):
        """Initialize signal processor components and data structures"""
        # Default frequency bands
        if self.config.frequency_bands is None:
            self.config.frequency_bands = [
                (1, 4),    # Delta
                (4, 8),    # Theta
                (8, 13),   # Alpha
                (13, 30),  # Beta
                (30, 100)  # Gamma
            ]
        
        # Default feature types
        if self.config.feature_types is None:
            self.config.feature_types = ["fft", "wavelet", "stft", "correlation"]
        
        # Initialize signal buffers
        self.signal_buffer = []
        self.filtered_buffer = []
        self.feature_buffer = []
    
    def add_signals(self, 
                    signals: np.ndarray,
                    channel_names: Optional[List[str]] = None,
                    sampling_rate: Optional[float] = None,
                    time_step: float = 1.0):
        """
        Add signals for processing.
        
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
        
        # Update time
        self.current_time = getattr(self, 'current_time', 0.0) + time_step
    
    def _process_signals(self, signals: np.ndarray, sampling_rate: float):
        """Process signals for analysis"""
        # Apply filtering
        filtered = self._apply_filters(signals, sampling_rate)
        self.filtered_signals.append(filtered)
        
        # Apply preprocessing
        processed = self._preprocess_signals(filtered, sampling_rate)
        self.processed_signals.append(processed)
        
        # Extract features
        if self.config.feature_extraction:
            features = self._extract_features(filtered, sampling_rate)
            self.features.append(features)
        
        # Create epochs if requested
        if self.config.epoching:
            epochs = self._create_epochs(processed, sampling_rate)
            self.epochs.append(epochs)
        
        # Compute frequency domain data
        freq_data = self._compute_frequency_domain(processed, sampling_rate)
        self.frequency_data.append(freq_data)
    
    def _apply_filters(self, signals: np.ndarray, sampling_rate: float) -> np.ndarray:
        """Apply filters to signals"""
        n_channels, n_samples = signals.shape
        filtered = np.zeros_like(signals)
        
        # Design filters based on configuration
        if self.config.filter_type == "butter":
            # High-pass and low-pass filters
            nyquist = 0.5 * sampling_rate
            
            if self.config.high_pass_freq > 0:
                hp_cutoff = self.config.high_pass_freq / nyquist
                hp_b, hp_a = signal.butter(self.config.filter_order, hp_cutoff, btype='high')
                
                for i in range(n_channels):
                    filtered[i, :] = signal.filtfilt(hp_b, hp_a, signals[i, :])
            
            if self.config.low_pass_freq < nyquist:
                lp_cutoff = self.config.low_pass_freq / nyquist
                lp_b, lp_a = signal.butter(self.config.filter_order, lp_cutoff, btype='low')
                
                for i in range(n_channels):
                    filtered[i, :] = signal.filtfilt(lp_b, lp_a, filtered[i, :])
        
        elif self.config.filter_type == "fir":
            # FIR filter design
            nyquist = 0.5 * sampling_rate
            
            if self.config.high_pass_freq > 0 and self.config.low_pass_freq < nyquist:
                cutoff = [self.config.high_pass_freq / nyquist, self.config.low_pass_freq / nyquist]
                fir_coeffs = signal.firwin(self.config.filter_order, cutoff, window=self.config.window_type)
                
                for i in range(n_channels):
                    filtered[i, :] = signal.filtfilt(fir_coeffs, 1.0, signals[i, :])
            
            else:
                # Single cutoff filter
                cutoff = max(self.config.high_pass_freq, self.config.low_pass_freq) / nyquist
                fir_coeffs = signal.firwin(self.config.filter_order, cutoff, window=self.config.window_type)
                
                for i in range(n_channels):
                    filtered[i, :] = signal.filtfilt(fir_coeffs, 1.0, signals[i, :])
        
        else:
            # Use original signals
            filtered = signals.copy()
        
        # Apply notch filter to remove power line interference
        if self.config.notch_frequency > 0:
            nyquist = 0.5 * sampling_rate
            notch_cutoff = self.config.notch_frequency / nyquist
            
            # Design notch filter
            Q = self.config.notch_frequency / self.config.notch_width
            b, a = signal.iirnotch(notch_cutoff, Q)
            
            for i in range(n_channels):
                filtered[i, :] = signal.filtfilt(b, a, filtered[i, :])
        
        return filtered
    
    def _preprocess_signals(self, signals: np.ndarray, sampling_rate: float) -> np.ndarray:
        """Preprocess signals"""
        n_channels, n_samples = signals.shape
        processed = np.zeros_like(signals)
        
        for i in range(n_channels):
            signal_data = signals[i, :].copy()
            
            # Detrend if requested
            if self.config.detrend:
                # Linear detrending
                x = np.arange(n_samples)
                slope, intercept = np.polyfit(x, signal_data, 1)
                signal_data = signal_data - (slope * x + intercept)
            
            # Baseline correction if requested
            if self.config.baseline_correction:
                # Remove mean and DC offset
                signal_data = signal_data - np.mean(signal_data)
            
            # Normalize if requested
            if self.config.normalize:
                signal_data = (signal_data - np.min(signal_data)) / (np.max(signal_data) - np.min(signal_data) + 1e-10)
            
            # Apply window if requested
            if self.config.apply_window:
                window = getattr(signal, f"{self.config.window_type}_window")(n_samples)
                signal_data = signal_data * window
            
            # Resample if requested
            if self.config.resample_rate is not None and self.config.resample_rate != sampling_rate:
                from scipy.signal import resample
                new_length = int(n_samples * self.config.resample_rate / sampling_rate)
                signal_data = resample(signal_data, new_length)
                sampling_rate = self.config.resample_rate
            
            processed[i, :] = signal_data
        
        return processed
    
    def _extract_features(self, signals: np.ndarray, sampling_rate: float) -> Dict[str, Any]:n        """Extract features from signals"""
        n_channels, n_samples = signals.shape
        features = {}
        
        for feature_type in self.config.feature_types:
            if feature_type == "fft":
                features["fft"] = self._compute_fft(signals, sampling_rate)
            elif feature_type == "wavelet":
                features["wavelet"] = self._compute_wavelet_transform(signals, sampling_rate)
            elif feature_type == "stft":
                features["stft"] = self._compute_stft(signals, sampling_rate)
            elif feature_type == "correlation":
                features["correlation"] = self._compute_correlation_matrix(signals)
        
        return features
    
    def _compute_fft(self, signals: np.ndarray, sampling_rate: float) -> Dict[str, Any]:
        """Compute FFT of signals"""
        n_channels, n_samples = signals.shape
        
        # Compute FFT
        freqs = fft.rfftfreq(n_samples, 1.0 / sampling_rate)
        fft_data = np.zeros((n_channels, len(freqs)), dtype=complex)
        
        for i in range(n_channels):
            fft_data[i, :] = fft.rfft(signals[i, :])
        
        # Compute power spectrum
        power_spectrum = np.abs(fft_data) ** 2
        
        # Extract power in frequency bands
        band_power = {}
        for low, high in self.config.frequency_bands:
            band_indices = np.where((freqs >= low) & (freqs <= high))[0]
            if len(band_indices) > 0:
                band_power[f"{low}_{high}"] = np.sum(power_spectrum[:, band_indices], axis=1)
        
        return {
            "frequencies": freqs.tolist(),
            "power_spectrum": power_spectrum.tolist(),
            "band_power": band_power,
            "total_power": np.sum(power_spectrum, axis=(0, 1))
        }
    
    def _compute_wavelet_transform(self, signals: np.ndarray, sampling_rate: float) -> Dict[str, Any]:
        """Compute wavelet transform of signals"""
        try:
            import pywt
            
            n_channels, n_samples = signals.shape
            
            # Use Morlet wavelet
            wavelet = "morl"
            levels = int(np.log2(n_samples))
            
            coefficients = {}
            for i in range(n_channels):
                cwt, freqs = pywt.cwt(signals[i, :], wavelet, levels)
                coefficients[f"channel_{i}"] = {
                    "coefficients": cwt.tolist(),
                    "frequencies": freqs.tolist()
                }
            
            return {
                "wavelet_coefficients": coefficients,
                "frequencies": freqs.tolist(),
                "mean_energy": [np.mean(np.abs(cwt)) for cwt in cwt.T]
            }
            
        except ImportError:
            return {
                "error": "pywt not installed - wavelet transform not available"
            }
    
    def _compute_stft(self, signals: np.ndarray, sampling_rate: float) -> Dict[str, Any]:
        """Compute short-time Fourier transform of signals"""
        n_channels, n_samples = signals.shape
        
        # STFT parameters
        nperseg = min(256, n_samples)
        noverlap = nperseg // 2
        
        # Compute STFT for each channel
        stft_data = {}
        for i in range(n_channels):
            f, t, Zxx = signal.stft(signals[i, :], fs=sampling_rate, nperseg=nperseg, noverlap=noverlap)
            stft_data[f"channel_{i}"] = {
                "frequencies": f.tolist(),
                "times": t.tolist(),
                "stft": Zxx.tolist()
            }
        
        return {
            "stft_data": stft_data,
            "frequencies": f.tolist(),
            "time_segments": t.tolist()
        }
    
    def _compute_correlation_matrix(self, signals: np.ndarray) -> Dict[str, Any]:
        """Compute correlation matrix between channels"""
        n_channels, n_samples = signals.shape
        
        # Compute correlation matrix
        correlation_matrix = np.corrcoef(signals)
        
        # Compute partial correlation matrix
        partial_correlation = self._compute_partial_correlation(signals)
        
        return {
            "correlation_matrix": correlation_matrix.tolist(),
            "partial_correlation_matrix": partial_correlation.tolist(),
            "mean_correlation": np.mean(correlation_matrix),
            "mean_partial_correlation": np.mean(partial_correlation)
        }
    
    def _compute_partial_correlation(self, signals: np.ndarray) -> np.ndarray:
        """Compute partial correlation matrix"""
        n_channels = signals.shape[0]
        partial_corr = np.zeros((n_channels, n_channels))
        
        for i in range(n_channels):
            for j in range(n_channels):
                if i != j:
                    # Remove influence of other channels
                    X = np.delete(signals, [j], axis=0)
                    y = signals[i, :]
                    z = signals[j, :]
                    
                    # Compute partial correlation
                    corr = np.corrcoef(X.T, y, z.T)
                    partial_corr[i, j] = corr[0, 1]
        
        return partial_corr
    
    def _create_epochs(self, signals: np.ndarray, sampling_rate: float) -> Dict[str, Any]:
        """Create epochs from signals"""
        n_channels, n_samples = signals.shape
        
        # Calculate epoch length in samples
        epoch_samples = int(self.config.epoch_length * sampling_rate)
        overlap_samples = int(self.config.epoch_overlap * sampling_rate)
        step_samples = epoch_samples - overlap_samples
        
        # Create epochs
        epochs = []
        epoch_info = []
        
        for start in range(0, n_samples - epoch_samples + 1, step_samples):
            end = start + epoch_samples
            epoch = signals[:, start:end]
            
            # Add to epochs list
            epochs.append(epoch)
            
            # Store epoch information
            epoch_info.append({
                "start": start / sampling_rate,
                "end": end / sampling_rate,
                "length": epoch_samples / sampling_rate,
                "overlap": overlap_samples / sampling_rate if overlap_samples > 0 else 0
            })
        
        return {
            "epochs": np.array(epochs).tolist(),
            "epoch_info": epoch_info,
            "n_epochs": len(epochs),
            "epoch_duration": self.config.epoch_length
        }
    
    def _compute_frequency_domain(self, signals: np.ndarray, sampling_rate: float) -> Dict[str, Any]:
        """Compute frequency domain representation"""
        n_channels, n_samples = signals.shape
        
        # Compute power spectral density using Welch's method
        f, Pxx = signal.welch(signals[0, :], fs=sampling_rate, nperseg=min(1024, n_samples))
        
        # Compute for all channels
        all_psd = np.zeros((n_channels, len(f)))
        for i in range(n_channels):
            _, Pxx_i = signal.welch(signals[i, :], fs=sampling_rate, nperseg=min(1024, n_samples))
            all_psd[i, :] = Pxx_i
        
        # Compute cross-spectral density
        f, Pxy = signal.csd(signals[0, :], signals[1, :], fs=sampling_rate, nperseg=min(1024, n_samples))
        
        return {
            "frequencies": f.tolist(),
            "power_spectral_density": Pxx.tolist(),
            "all_channel_psd": all_psd.tolist(),
            "cross_spectral_density": Pxy.tolist() if len(signals) > 1 else None,
            "dominant_frequencies": self._find_dominant_frequencies(all_psd, f),
            "bandwidth": self._estimate_bandwidth(all_psd, f)
        }
    
    def _find_dominant_frequencies(self, psd: np.ndarray, freqs: np.ndarray) -> List[Dict[str, Any]]:
        """Find dominant frequencies"""
        dominant_freqs = []
        
        for i in range(psd.shape[0]):
            # Find peak frequency for each channel
            peak_idx = np.argmax(psd[i, :])
            peak_freq = freqs[peak_idx]
            peak_power = psd[i, peak_idx]
            
            dominant_freqs.append({
                "channel": i,
                "frequency": peak_freq,
                "power": peak_power,
                "relative_power": peak_power / np.sum(psd[i, :]) if np.sum(psd[i, :]) > 0 else 0
            })
        
        return dominant_freqs
    
    def _estimate_bandwidth(self, psd: np.ndarray, freqs: np.ndarray) -> List[Dict[str, Any]]:
        """Estimate bandwidth of signals"""
        bandwidths = []
        
        for i in range(psd.shape[0]):
            # Calculate full width at half maximum (FWHM)
            peak_idx = np.argmax(psd[i, :])
            peak_power = psd[i, peak_idx]
            half_power = peak_power / 2
            
            # Find indices where power is above half power
            above_half = psd[i, :] > half_power
            
            if np.any(above_half):
                # Calculate bandwidth
                indices = np.where(above_half)[0]
                bandwidth = freqs[indices[-1]] - freqs[indices[0]]
                
                bandwidths.append({
                    "channel": i,
                    "bandwidth": bandwidth,
                    "half_power_frequency": freqs[peak_idx],
                    "fwhm": bandwidth
                })
            else:
                bandwidths.append({
                    "channel": i,
                    "bandwidth": 0,
                    "half_power_frequency": freqs[peak_idx],
                    "fwhm": 0
                })
        
        return bandwidths
    
    def create_visualization(self) -> Dict[str, Any]:
        """
        Create signal processing visualization.
        
        Returns:
            Dictionary with visualization data
        """
        result = {
            "success": False,
            "visualization": {},
            "statistics": {},
            "plots": {}
        }
        
        try:
            if not self.signals:
                result["error"] = "No signals available for visualization"
                return result
            
            # Create waveform visualization
            waveform = self._create_waveform_visualization()
            
            # Create spectrum visualization
            spectrum = self._create_spectrum_visualization()
            
            # Create filter response visualization
            filter_response = self._create_filter_response_visualization()
            
            # Create frequency analysis visualization
            frequency_analysis = self._create_frequency_analysis_visualization()
            
            # Create time-frequency analysis visualization
            time_frequency = self._create_time_frequency_visualization()
            
            # Combine all visualizations
            result["visualization"] = {
                "waveform": waveform,
                "spectrum": spectrum,
                "filter_response": filter_response,
                "frequency_analysis": frequency_analysis,
                "time_frequency": time_frequency
            }
            
            # Compute statistics
            result["statistics"] = self._compute_signal_statistics()
            
            # Generate plots
            result["plots"] = self._generate_signal_plots()
            
            result["success"] = True
            
        except Exception as e:
            result["error"] = str(e)
            result["success"] = False
        
        return result
    
    def _create_waveform_visualization(self) -> Dict[str, Any]:
        """Create waveform visualization"""
        if not self.signals:
            return {}
        
        # Use first signal for visualization
        signal_data = self.signals[0]
        
        if self.config.normalize:
            signal_data = (signal_data - np.min(signal_data)) / (np.max(signal_data) - np.min(signal_data))
        
        return {
            "time": np.arange(signal_data.shape[1]) / self.config.sampling_rate,
            "signal": signal_data[0].tolist(),
            "channels": list(range(signal_data.shape[0])),
            "title": "Signal Waveforms",
            "xlabel": "Time (s)",
            "ylabel": "Amplitude"
        }
    
    def _create_spectrum_visualization(self) -> Dict[str, Any]:
        """Create spectrum visualization"""
        if not self.signals:
            return {}
        
        # Use first signal for visualization
        signal_data = self.signals[0]
        
        # Compute FFT
        n_samples = signal_data.shape[1]
        freqs = fft.rfftfreq(n_samples, 1.0 / self.config.sampling_rate)
        fft_data = fft.rfft(signal_data[0])
        power = np.abs(fft_data) ** 2
        
        return {
            "frequencies": freqs.tolist(),
            "power": power.tolist(),
            "title": "Power Spectrum",
            "xlabel": "Frequency (Hz)",
            "ylabel": "Power"
        }
    
    def _create_filter_response_visualization(self) -> Dict[str, Any]:
        """Create filter response visualization"""
        # Design and plot filter response
        nyquist = 0.5 * self.config.sampling_rate
        
        if self.config.high_pass_freq > 0 and self.config.low_pass_freq < nyquist:
            # Bandpass filter
            cutoff = [self.config.high_pass_freq / nyquist, self.config.low_pass_freq / nyquist]
            b, a = signal.butter(self.config.filter_order, cutoff, btype='band')
        elif self.config.high_pass_freq > 0:
            # Highpass filter
            cutoff = self.config.high_pass_freq / nyquist
            b, a = signal.butter(self.config.filter_order, cutoff, btype='high')
        elif self.config.low_pass_freq < nyquist:
            # Lowpass filter
            cutoff = self.config.low_pass_freq / nyquist
            b, a = signal.butter(self.config.filter_order, cutoff, btype='low')
        else:
            return {}
        
        # Compute frequency response
        w, h = signal.freqz(b, a, worN=1024)
        frequencies = w * nyquist / np.pi
        
        return {
            "frequencies": frequencies.tolist(),
            "magnitude": np.abs(h).tolist(),
            "phase": np.angle(h).tolist(),
            "title": "Filter Response",
            "xlabel": "Frequency (Hz)",
            "ylabel": "Gain"
        }
    
    def _create_frequency_analysis_visualization(self) -> Dict[str, Any]:
        """Create frequency analysis visualization"""
        # Compute frequency analysis
        if not self.signals:
            return {}
        
        # Use first signal for visualization
        signal_data = self.signals[0]
        
        # Compute Welch's PSD
        f, Pxx = signal.welch(signal_data[0], fs=self.config.sampling_rate, nperseg=min(1024, signal_data.shape[1]))
        
        return {
            "frequencies": f.tolist(),
            "power_spectral_density": Pxx.tolist(),
            "title": "Power Spectral Density (Welch)",
            "xlabel": "Frequency (Hz)",
            "ylabel": "Power/Frequency"
        }
    
    def _create_time_frequency_visualization(self) -> Dict[str, Any]:
        """Create time-frequency analysis visualization"""
        # Compute STFT
        if not self.signals:
            return {}
        
        # Use first signal for visualization
        signal_data = self.signals[0]
        
        # Compute STFT
        nperseg = min(256, signal_data.shape[1])
        noverlap = nperseg // 2
        f, t, Zxx = signal.stft(signal_data[0], fs=self.config.sampling_rate, nperseg=nperseg, noverlap=noverlap)
        
        return {
            "frequencies": f.tolist(),
            "times": t.tolist(),
            "stft": Zxx.tolist(),
            "title": "Short-Time Fourier Transform",
            "xlabel": "Time (s)",
            "ylabel": "Frequency (Hz)"
        }
    
    def _compute_signal_statistics(self) -> Dict[str, Any]:
        """Compute signal statistics"""
        if not self.signals:
            return {}
        
        statistics = {}
        
        for i, signal_data in enumerate(self.signals):
            # Basic statistics
            stats = {
                "channel": i,
                "mean": float(np.mean(signal_data)),
                "std": float(np.std(signal_data)),
                "min": float(np.min(signal_data)),
                "max": float(np.max(signal_data)),
                "median": float(np.median(signal_data)),
                "range": float(np.max(signal_data) - np.min(signal_data)),
                "skewness": float(stats.skew(signal_data.flatten())),
                "kurtosis": float(stats.kurtosis(signal_data.flatten()))
            }
            
            # Frequency domain statistics
            if self.frequency_data and i < len(self.frequency_data):
                freq_data = self.frequency_data[i]
                if "dominant_frequencies" in freq_data:
                    stats["dominant_frequency"] = freq_data["dominant_frequencies"][0]["frequency"] if freq_data["dominant_frequencies"] else 0
                if "bandwidth" in freq_data:
                    stats["bandwidth"] = freq_data["bandwidth"][0]["fwhm"] if freq_data["bandwidth"] else 0
            
            statistics[f"channel_{i}"] = stats
        
        return statistics
    
    def _generate_signal_plots(self) -> Dict[str, Any]:
        """Generate signal processing plots"""
        plots = {}
        
        if self.config.show_waveform and self.signals:
            plots["waveform"] = self._create_waveform_plot()
        
        if self.config.show_spectrum and self.signals:
            plots["spectrum"] = self._create_spectrum_plot()
        
        if self.config.show_frequency_analysis and self.signals:
            plots["frequency_analysis"] = self._create_frequency_analysis_plot()
        
        if self.config.show_time_frequency_analysis and self.signals:
            plots["time_frequency_analysis"] = self._create_time_frequency_plot()
        
        return plots
    
    def _create_waveform_plot(self) -> Dict[str, Any]:
        """Create waveform plot"""
        # Create simple waveform plot
        fig, ax = plt.subplots(figsize=(10, 6))
        
        for i, signal_data in enumerate(self.signals):
            ax.plot(signal_data[0], label=f"Channel {i}")
        
        ax.set_title("Signal Waveforms")
        ax.set_xlabel("Time (s)")
        ax.set_ylabel("Amplitude")
        ax.legend()
        ax.grid(True)
        
        # Save plot
        plot_path = f"{self.config.output_path}/waveform.png"
        os.makedirs(os.path.dirname(plot_path), exist_ok=True)
        fig.savefig(plot_path, dpi=300, bbox_inches='tight')
        plt.close(fig)
        
        return {
            "path": plot_path,
            "type": "waveform",
            "channels": len(self.signals)
        }
    
    def _create_spectrum_plot(self) -> Dict[str, Any]:
        """Create spectrum plot"""
        # Create simple spectrum plot
        fig, ax = plt.subplots(figsize=(10, 6))
        
        for i, signal_data in enumerate(self.signals):
            n_samples = signal_data.shape[1]
            freqs = fft.rfftfreq(n_samples, 1.0 / self.config.sampling_rate)
            fft_data = fft.rfft(signal_data[0])
            power = np.abs(fft_data) ** 2
            
            ax.semilogy(freqs, power, label=f"Channel {i}")
        
        ax.set_title("Power Spectrum")
        ax.set_xlabel("Frequency (Hz)")
        ax.set_ylabel("Power")
        ax.legend()
        ax.grid(True)
        
        # Save plot
        plot_path = f"{self.config.output_path}/spectrum.png"
        fig.savefig(plot_path, dpi=300, bbox_inches='tight')
        plt.close(fig)
        
        return {
            "path": plot_path,
            "type": "spectrum",
            "channels": len(self.signals)
        }
    
    def _create_frequency_analysis_plot(self) -> Dict[str, Any]:
        """Create frequency analysis plot"""
        # Create frequency analysis plot
        fig, ax = plt.subplots(figsize=(10, 6))
        
        if self.signals:
            signal_data = self.signals[0]
            f, Pxx = signal.welch(signal_data[0], fs=self.config.sampling_rate, nperseg=min(1024, signal_data.shape[1]))
            
            ax.semilogy(f, Pxx)
        
        ax.set_title("Power Spectral Density (Welch)")
        ax.set_xlabel("Frequency (Hz)")
        ax.set_ylabel("Power/Frequency")
        ax.grid(True)
        
        # Save plot
        plot_path = f"{self.config.output_path}/frequency_analysis.png"
        fig.savefig(plot_path, dpi=300, bbox_inches='tight')
        plt.close(fig)
        
        return {
            "path": plot_path,
            "type": "frequency_analysis",
            "channels": len(self.signals) if self.signals else 0
        }
    
    def _create_time_frequency_plot(self) -> Dict[str, Any]:
        """Create time-frequency analysis plot"""
        # Create time-frequency analysis plot
        fig, ax = plt.subplots(figsize=(10, 6))
        
        if self.signals:
            signal_data = self.signals[0]
            nperseg = min(256, signal_data.shape[1])
            noverlap = nperseg // 2
            f, t, Zxx = signal.stft(signal_data[0], fs=self.config.sampling_rate, nperseg=nperseg, noverlap=noverlap)
            
            # Create spectrogram
            im = ax.pcolormesh(t, f, np.abs(Zxx), shading='auto', cmap='viridis', alpha=0.8)
            ax.set_xlabel("Time (s)")
            ax.set_ylabel("Frequency (Hz)")
            ax.set_title("Short-Time Fourier Transform")
            
            # Add colorbar
            plt.colorbar(im, ax=ax)
        
        # Save plot
        plot_path = f"{self.config.output_path}/time_frequency_analysis.png"
        fig.savefig(plot_path, dpi=300, bbox_inches='tight')
        plt.close(fig)
        
        return {
            "path": plot_path,
            "type": "time_frequency_analysis",
            "channels": len(self.signals) if self.signals else 0
        }
    
    def export_processed_signals(self, filename: str) -> str:
        """
        Export processed signals to file.
        
        Args:
            filename: Output filename
            
        Returns:
            Path to saved file
        """
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        # Create processed signal data structure
        processed_data = {
            "config": asdict(self.config),
            "signals": [sig.tolist() for sig in self.signals],
            "filtered_signals": [sig.tolist() for sig in self.filtered_signals],
            "processed_signals": [sig.tolist() for sig in self.processed_signals],
            "features": self.features,
            "epochs": self.epochs,
            "frequency_data": self.frequency_data,
            "statistics": self._compute_signal_statistics()
        }
        
        # Save based on extension
        if filename.endswith('.h5') or filename.endswith('.hdf5'):
            return self._save_hdf5(filename, processed_data)
        elif filename.endswith('.json'):
            return self._save_json(filename, processed_data)
        elif filename.endswith('.csv'):
            return self._save_csv(filename, processed_data)
        elif filename.endswith('.npy'):
            return self._save_npy(filename, processed_data)
        elif filename.endswith('.mat'):
            return self._save_mat(filename, processed_data)
        else:
            return self._save_default(filename, processed_data)
    
    def _save_hdf5(self, filename: str, data: Dict[str, Any]) -> str:
        """Save as HDF5"""
        import h5py
        
        with h5py.File(filename, 'w') as f:
            self._dict_to_hdf5(data, f, "root")
        
        return filename
    
    def _save_json(self, filename: str, data: Dict[str, Any]) -> str:
        """Save as JSON"""
        import json
        
        with open(filename, 'w') as f:
            json.dump(self._convert_to_json_serializable(data), f, indent=2)
        
        return filename
    
    def _save_csv(self, filename: str, data: Dict[str, Any]) -> str:
        """Save as CSV"""
        import pandas as pd
        
        # Convert to DataFrame
        df = pd.json_normalize(data)
        df.to_csv(filename, index=False)
        
        return filename
    
    def _save_npy(self, filename: str, data: Dict[str, Any]) -> str:
        """Save as NumPy file"""
        import numpy as np
        
        np.savez(filename, **self._convert_to_numpy(data))
        
        return filename
    
    def _save_mat(self, filename: str, data: Dict[str, Any]) -> str:
        """Save as MATLAB file"""
        try:
            import scipy.io as sio
            sio.savemat(filename, data)
            return filename
        except ImportError:
            # Fallback to JSON
            return self._save_json(filename, data)
    
    def _save_default(self, filename: str, data: Dict[str, Any]) -> str:
        """Default save method"""
        import json
        
        with open(filename, 'w') as f:
            json.dump(self._convert_to_json_serializable(data), f, indent=2)
        
        return filename
    
    def _dict_to_hdf5(self, data: Dict[str, Any], h5_file, group_name: str):
        """Convert dictionary to HDF5"""
        for key, value in data.items():
            current_path = f"{group_name}/{key}" if group_name else key
            if isinstance(value, dict):
                h5_file.create_group(current_path)
                self._dict_to_hdf5(value, h5_file, current_path)
            elif isinstance(value, np.ndarray):
                if self.config.compress_output:
                    h5_file.create_dataset(current_path, data=value, compression="gzip", compression_opts=4)
                else:
                    h5_file.create_dataset(current_path, data=value)
            elif isinstance(value, (str, int, float, bool)):
                h5_file.attrs[current_path] = value
            else:
                h5_file.create_dataset(current_path, data=str(value))
    
    def _convert_to_json_serializable(self, data: Any) -> Any:
        """Convert data to JSON-serializable format"""
        if isinstance(data, dict):
            return {key: self._convert_to_json_serializable(value) for key, value in data.items()}
        elif isinstance(data, list):
            return [self._convert_to_json_serializable(item) for item in data]
        elif isinstance(data, np.ndarray):
            return data.tolist()
        elif isinstance(data, np.integer):
            return int(data)
        elif isinstance(data, np.floating):
            return float(data)
        else:
            return data
    
    def _convert_to_numpy(self, data: Dict[str, Any]) -> Dict[str, np.ndarray]:
        """Convert data to NumPy arrays"""
        np_data = {}
        
        for key, value in data.items():
            if isinstance(value, dict):
                # Recursively convert nested dictionaries
                self._convert_dict_to_numpy(value, f"{key}/")
            elif isinstance(value, list):
                np_data[key] = np.array(value)
            elif isinstance(value, np.ndarray):
                np_data[key] = value
            elif isinstance(value, (int, float, str, bool)):
                np_data[key] = np.array(value)
            else:
                np_data[key] = np.array(str(value))
        
        return np_data
    
    def _convert_dict_to_numpy(self, data: Dict[str, Any], prefix: str):
        """Convert nested dictionary to NumPy arrays"""
        for key, value in data.items():
            full_key = f"{prefix}{key}"
            if isinstance(value, dict):
                self._convert_dict_to_numpy(value, f"{full_key}/")
            elif isinstance(value, list):
                # Store as separate NumPy arrays
                for i, item in enumerate(value):
                    np_data[f"{full_key}_{i}"] = np.array(item)
            elif isinstance(value, np.ndarray):
                np_data[full_key] = value
            elif isinstance(value, (int, float, str, bool)):
                np_data[full_key] = np.array(value)
            else:
                np_data[full_key] = np.array(str(value))
    
    def _get_timestamp(self) -> str:
        """Get current timestamp"""
        from datetime import datetime
        return datetime.now().isoformat()


def create_signal_processor(config_dict: Dict[str, Any] = None) -> SignalProcessor:
    """
    Factory function to create SignalProcessor with configuration.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        Configured SignalProcessor instance
    """
    if config_dict is None:
        config_dict = {}
    
    config = SignalProcessorConfig(**config_dict)
    return SignalProcessor(config)