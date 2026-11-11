# Sensory module - Enhanced Python interface for sensory processing
"""
Enhanced Python interface for sensory input processing with additional
functionality and better integration with Python.
"""

from typing import List, Optional, Any, Dict
import numpy as np

# Try to import C++ bindings
try:
    from ._pynlm import Vision as _Vision, Audio as _Audio
    CPLUSPLUS_AVAILABLE = True
except ImportError:
    CPLUSPLUS_AVAILABLE = False

class SensoryInput:
    """Enhanced Python interface for SensoryInput.
    
    This class provides a more Pythonic interface for sensory data handling,
    with additional convenience methods and better data management.
    """
    
    def __init__(self):
        """Initialize sensory input."""
        pass  # Would be initialized with C++ counterpart
    
    def get_type(self):
        """Get sensory input type.
        
        Returns:
            NeuronType enum value.
        """
        raise NotImplementedError("Abstract base class")
    
    def get_data(self):
        """Get raw sensory data.
        
        Returns:
            List of float values.
        """
        raise NotImplementedError("Abstract base class")
    
    def get_dimensions(self):
        """Get dimensionality of sensory data.
        
        Returns:
            Tuple of (width, height, channels) for vision, etc.
        """
        raise NotImplementedError("Abstract base class")
    
    def get_timestamp(self):
        """Get timestamp.
        
        Returns:
            Timestamp value.
        """
        raise NotImplementedError("Abstract base class")
    
    def set_timestamp(self, timestamp):
        """Set timestamp.
        
        Args:
            timestamp: New timestamp.
        """
        raise NotImplementedError("Abstract base class")
    
    def to_dict(self):
        """Convert sensory input to dictionary.
        
        Returns:
            Dictionary representation.
        """
        return {
            'type': str(self.get_type()),
            'data': self.get_data(),
            'dimensions': self.get_dimensions(),
            'timestamp': self.get_timestamp(),
        }
    
    def __repr__(self):
        """String representation."""
        return f"<SensoryInput: type={self.get_type()}, dims={self.get_dimensions()}>"


class Vision(SensoryInput):
    """Vision sensory input with enhanced functionality."""
    
    def __init__(self, width=64, height=64, channels=3):
        """Initialize vision sensory input.
        
        Args:
            width: Image width.
            height: Image height.
            channels: Number of color channels.
        """
        super().__init__()
        self.width = width
        self.height = height
        self.channels = channels
        self._data = np.zeros((height, width, channels), dtype=np.float32)
    
    def set_data(self, data):
        """Set vision data.
        
        Args:
            data: Vision data as numpy array or list.
        """
        if isinstance(data, np.ndarray):
            self._data = data.astype(np.float32)
        elif isinstance(data, list):
            self._data = np.array(data, dtype=np.float32).reshape(self.height, self.width, self.channels)
        else:
            raise TypeError(f"Vision data must be numpy array or list, got {type(data)}")
    
    def get_type(self):
        """Get sensory input type."""
        from .types import NeuronType
        return NeuronType.Vision
    
    def get_data(self):
        """Get raw vision data as flattened list."""
        return self._data.flatten().tolist()
    
    def get_array(self):
        """Get vision data as numpy array."""
        return self._data.copy()
    
    def get_dimensions(self):
        """Get dimensions."""
        return (self.width, self.height, self.channels)
    
    def normalize(self, mean=None, std=None):
        """Normalize vision data.
        
        Args:
            mean: Mean for normalization (optional, uses data mean if None).
            std: Standard deviation (optional, uses data std if None).
        """
        if mean is None:
            mean = np.mean(self._data)
        if std is None:
            std = np.std(self._data)
        
        if std > 0:
            self._data = (self._data - mean) / std
        return self
    
    def threshold(self, threshold, invert=False):
        """Apply threshold to vision data.
        
        Args:
            threshold: Threshold value.
            invert: Whether to invert threshold (background=1, foreground=0).
        """
        binary = self._data > threshold
        if invert:
            binary = ~binary
        return binary
    
    def get_edges(self, threshold=0.1):
        """Extract edges from vision data using simple gradient.
        
        Args:
            threshold: Edge detection threshold.
            
        Returns:
            Binary edge map.
        """
        # Simple Sobel-like edge detection
        if self.channels == 1:
            gray = self._data.squeeze()
        else:
            # Convert to grayscale
            gray = np.dot(self._data[..., :3], [0.299, 0.587, 0.114])
        
        # Compute gradients
        grad_x = np.gradient(gray, axis=1)
        grad_y = np.gradient(gray, axis=0)
        gradient_magnitude = np.sqrt(grad_x**2 + grad_y**2)
        
        # Apply threshold
        edges = gradient_magnitude > threshold
        
        return edges
    
    def get_features(self):
        """Extract basic features from vision data.
        
        Returns:
            Dictionary of features (mean, std, min, max, edges_count, etc.).
        """
        if self.channels == 1:
            gray = self._data.squeeze()
        else:
            gray = np.dot(self._data[..., :3], [0.299, 0.587, 0.114])
        
        features = {
            'mean': float(np.mean(gray)),
            'std': float(np.std(gray)),
            'min': float(np.min(gray)),
            'max': float(np.max(gray)),
            'edges_count': np.sum(self.get_edges()),
            'center_region': float(np.mean(gray[gray.shape[0]//4:3*gray.shape[0]//4, gray.shape[1]//4:3*gray.shape[1]//4])),
        }
        
        return features
    
    def display(self, scale=1.0):
        """Display vision data as text (ASCII art).
        
        Args:
            scale: Scale factor for downsampling.
        """
        if self.channels == 1:
            gray = self._data.squeeze()
        else:
            # Convert to grayscale
            gray = np.dot(self._data[..., :3], [0.299, 0.587, 0.114])
        
        # Downsample
        if scale != 1.0:
            new_height = max(1, int(gray.shape[0] * scale))
            new_width = max(1, int(gray.shape[1] * scale))
            gray = np.resize(gray, (new_height, new_width))
        
        # Normalize to ASCII range
        gray_normalized = (gray - gray.min()) / (gray.max() - gray.min() + 1e-8)
        
        # Display
        print(f"Vision ({self.width}x{self.height}x{self.channels}):")
        for y in range(len(gray_normalized)):
            row = ""
            for x in range(len(gray_normalized[0])):
                intensity = int(gray_normalized[y][x] * 255)
                if intensity > 200:
                    row += "#"
                elif intensity > 150:
                    row += "@"
                elif intensity > 100:
                    row += "*"
                elif intensity > 50:
                    row += "."
                else:
                    row += " "
            print(row)
        
        print(f"Features: mean={np.mean(gray):.3f}, std={np.std(gray):.3f}, edges={np.sum(self.get_edges()):d}")


class Audio(SensoryInput):
    """Audio sensory input with enhanced functionality."""
    
    def __init__(self, sample_rate=44100, num_samples=8192):
        """Initialize audio sensory input.
        
        Args:
            sample_rate: Samples per second.
            num_samples: Number of samples.
        """
        super().__init__()
        self.sample_rate = sample_rate
        self.num_samples = num_samples
        self._data = np.zeros(num_samples, dtype=np.float32)
    
    def set_data(self, data):
        """Set audio data.
        
        Args:
            data: Audio data as numpy array or list.
        """
        if isinstance(data, np.ndarray):
            self._data = data.astype(np.float32)
        elif isinstance(data, list):
            self._data = np.array(data, dtype=np.float32)
        else:
            raise TypeError(f"Audio data must be numpy array or list, got {type(data)}")
    
    def get_type(self):
        """Get sensory input type."""
        from .types import NeuronType
        return NeuronType.Audio
    
    def get_data(self):
        """Get raw audio data."""
        return self._data.tolist()
    
    def get_array(self):
        """Get audio data as numpy array."""
        return self._data.copy()
    
    def get_dimensions(self):
        """Get dimensions."""
        return (self.sample_rate, self.num_samples)
    
    def normalize(self, max_val=None):
        """Normalize audio data.
        
        Args:
            max_val: Maximum value for normalization (optional).
        """
        if max_val is None:
            max_val = np.max(np.abs(self._data))
        
        if max_val > 0:
            self._data = self._data / max_val
        return self
    
    def apply_filter(self, filter_coeffs):
        """Apply digital filter to audio data.
        
        Args:
            filter_coeffs: Filter coefficients.
        """
        from scipy import signal
        
        # Apply filter
        filtered = signal.lfilter(filter_coeffs, 1.0, self._data)
        self._data = filtered
        return self
    
    def compute_fft(self, n_fft=None):
        """Compute FFT of audio data.
        
        Args:
            n_fft: Number of FFT points (optional).
            
        Returns:
            Tuple of (frequencies, magnitude_spectrum).
        """
        if n_fft is None:
            n_fft = min(4096, self.num_samples)
        
        # Compute FFT
        fft_result = np.fft.rfft(self._data[:n_fft])
        freqs = np.fft.rfftfreq(n_fft, 1.0 / self.sample_rate)
        magnitude = np.abs(fft_result) / n_fft
        
        return freqs, magnitude
    
    def detect_pitch(self):
        """Detect fundamental frequency (pitch) of audio.
        
        Returns:
            Fundamental frequency in Hz.
        """
        # Simple pitch detection using autocorrelation
        if len(self._data) < 100:
            return 0.0
        
        # Normalize
        normalized = self._data / np.max(np.abs(self._data))
        
        # Autocorrelation
        autocorr = np.correlate(normalized, normalized, mode='full')
        autocorr = autocorr[len(autocorr)//2:]  # Take positive lags
        
        # Find first peak after zero crossing
        zero_crossings = np.where(np.diff(np.signbit(autocorr)))[0]
        if len(zero_crossings) > 0:
            first_peak = zero_crossings[0]
            pitch = self.sample_rate / first_peak if first_peak > 0 else 0.0
            return pitch
        
        return 0.0
    
    def compute_features(self):
        """Compute audio features.
        
        Returns:
            Dictionary of audio features.
        """
        # Basic statistics
        features = {
            'mean': float(np.mean(self._data)),
            'std': float(np.std(self._data)),
            'min': float(np.min(self._data)),
            'max': float(np.max(self._data)),
            'duration': self.num_samples / self.sample_rate,
        }
        
        # Pitch detection
        pitch = self.detect_pitch()
        features['pitch'] = pitch
        
        # Zero crossing rate
        zero_crossings = np.sum(np.diff(np.signbit(self._data))) / len(self._data)
        features['zero_crossing_rate'] = zero_crossings
        
        # Spectral features
        freqs, magnitude = self.compute_fft()
        features['spectral_centroid'] = float(np.average(freqs, weights=magnitude))
        features['spectral_spread'] = float(np.sqrt(np.average((freqs - features['spectral_centroid'])**2, weights=magnitude)))
        
        return features


class InternalSignals(SensoryInput):
    """Internal signals sensory input."""
    
    def __init__(self):
        """Initialize internal signals."""
        super().__init__()
        self._signals = []
    
    def add_signal(self, value):
        """Add internal signal value.
        
        Args:
            value: Signal value to add.
        """
        self._signals.append(value)
    
    def clear_signals(self):
        """Clear all signals."""
        self._signals.clear()
    
    def get_type(self):
        """Get sensory input type."""
        from .types import NeuronType
        return NeuronType.Internal
    
    def get_data(self):
        """Get all signal values."""
        return self._signals.copy()
    
    def get_array(self):
        """Get signals as numpy array."""
        return np.array(self._signals, dtype=np.float32)
    
    def get_dimensions(self):
        """Get dimensions."""
        return (len(self._signals),)
    
    def get_statistics(self):
        """Get signal statistics.
        
        Returns:
            Dictionary with signal statistics.
        """
        if not self._signals:
            return {
                'count': 0,
                'mean': 0.0,
                'std': 0.0,
                'min': 0.0,
                'max': 0.0,
            }
        
        arr = np.array(self._signals)
        return {
            'count': len(arr),
            'mean': float(np.mean(arr)),
            'std': float(np.std(arr)),
            'min': float(np.min(arr)),
            'max': float(np.max(arr)),
            'range': float(np.max(arr) - np.min(arr)),
        }
    
    def get_recent(self, n=10):
        """Get most recent n signals.
        
        Args:
            n: Number of recent signals to return.
            
        Returns:
            List of recent signal values.
        """
        return self._signals[-n:] if n <= len(self._signals) else self._signals.copy()


# Re-export sensory classes for convenience
SensoryInput = SensoryInput