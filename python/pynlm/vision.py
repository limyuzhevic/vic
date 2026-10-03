"""
Sensor processing utilities for NLM Python bindings.

This module provides enhanced sensor processing functionality,
including vision, audio, internal signals, and proprioception processing.
"""

import numpy as np
from typing import Optional, Dict, Any, List, Tuple

from .exceptions import ValidationError

# Import C++ types
try:
    from ..bindings import Vision, Audio, InternalSignals, SensoryPercept
    from ..types import NeuronType
except ImportError as e:
    raise ImportError(f"Failed to import NLM sensor bindings: {e}")


class NMLVision:
    """
    Enhanced vision sensor with Pythonic interface.
    
    Provides image processing and analysis for visual sensors.
    """
    
    def __init__(self, width: int, height: int, channels: int = 3):
        """
        Initialize vision sensor.
        
        Args:
            width: Image width
            height: Image height
            channels: Number of color channels (1=gray, 3 RGB, etc.)
            
        Raises:
            ValidationError: If parameters are invalid
        """
        if width <= 0 or height <= 0:
            raise ValidationError(f"Vision dimensions must be positive: {width}x{height}")
        
        if channels not in [1, 3, 4]:
            raise ValidationError(f"Unsupported channel count: {channels}. Use 1, 3, or 4")
        
        self._width = width
        self._height = height
        self._channels = channels
        self._vision = Vision(width, height, channels)
    
    def set_data(self, data: List[float]) -> None:
        """
        Set vision data.
        
        Args:
            data: List of pixel values (width * height * channels)
            
        Raises:
            ValidationError: If data size is incorrect
        """
        expected_size = self._width * self._height * self._channels
        if len(data) != expected_size:
            raise ValidationError(
                f"Vision data size mismatch: expected {expected_size} values, "
                f"got {len(data)}"
            )
        
        self._vision.setData(data)
    
    def get_data(self) -> List[float]:
        """Get vision data as list."""
        return self._vision.getData()
    
    def get_data_array(self) -> np.ndarray:
        """Get vision data as NumPy array."""
        data = self.get_data()
        return np.array(data).reshape(self._height, self._width, self._channels)
    
    def get_width(self) -> int:
        """Get vision width."""
        return self._vision.getWidth()
    
    def get_height(self) -> int:
        """Get vision height."""
        return self._vision.getHeight()
    
    def get_channels(self) -> int:
        """Get number of channels."""
        return self._vision.getChannels()
    
    def get_timestamp(self) -> float:
        """Get timestamp."""
        return self._vision.getTimestamp()
    
    def set_timestamp(self, timestamp: float) -> None:
        """Set timestamp."""
        self._vision.setTimestamp(timestamp)
    
    def get_type(self) -> NeuronType:
        """Get sensor type."""
        return NeuronType.Sensory
    
    def normalize(self) -> 'NMLVision':
        """
        Normalize pixel values to range [0, 1].
        
        Returns:
            Self for method chaining
        """
        data = self.get_data()
        normalized = [min(1.0, max(0.0, x / 255.0)) for x in data]
        self.set_data(normalized)
        return self
    
    def invert(self) -> 'NMLVision':
        """
        Invert pixel values.
n        
        Returns:
            Self for method chaining
        """
        data = self.get_data()
        inverted = [1.0 - x for x in data]
        self.set_data(inverted)
        return self
    
    def blur(self, kernel_size: int = 3) -> 'NMLVision':
        """
        Apply simple blur filter.
        
        Args:
            kernel_size: Size of blur kernel (must be odd)
            
        Returns:
            Self for method chaining
            
        Raises:
            ValidationError: If kernel size is invalid
        """
        if kernel_size % 2 == 0 or kernel_size < 3:
            raise ValidationError(f"Kernel size must be odd and >= 3, got {kernel_size}")
        
        # Simple blur implementation
        data = self.get_data()
        if len(data) == 0:
            return self
        
        half_kernel = kernel_size // 2
        blurred = []
        
        for row in range(self._height):
            for col in range(self._width):
                for channel in range(self._channels):
                    # Calculate average over kernel area
                    total = 0.0
                    count = 0
                    
                    for kr in range(-half_kernel, half_kernel + 1):
                        for kc in range(-half_kernel, half_kernel + 1):
                            r = row + kr
                            c = col + kc
                            
                            if 0 <= r < self._height and 0 <= c < self._width:
                                idx = ((r * self._width) + c) * self._channels + channel
                                total += data[idx]
                                count += 1
                    
                    blurred.append(total / count if count > 0 else 0.0)
        
        self.set_data(blurred)
        return self
    
    def edge_detection(self, threshold: float = 0.5) -> 'NMLVision':
        """
        Detect edges using simple gradient method.
        
        Args:
            threshold: Edge detection threshold (0-1)
            
        Returns:
            Self for method chaining
        """
        if not 0.0 <= threshold <= 1.0:
            raise ValidationError(f"Threshold must be between 0.0 and 1.0, got {threshold}")
        
        # Simple edge detection using Sobel-like operator
        data = self.get_data_array()
        height, width, channels = data.shape
        
        # Calculate gradients
        grad_x = np.zeros_like(data)
        grad_y = np.zeros_like(data)
        
        # Simple gradient calculation
        for c in range(channels):
            for y in range(1, height - 1):
                for x in range(1, width - 1):
                    # X gradient (horizontal edges)
                    grad_x[y, x, c] = (data[y, x + 1, c] - data[y, x - 1, c]) / 2.0
                    
                    # Y gradient (vertical edges)
                    grad_y[y, x, c] = (data[y + 1, x, c] - data[y - 1, x, c]) / 2.0
        
        # Combine gradients to get edge magnitude
        edge_magnitude = np.sqrt(grad_x**2 + grad_y**2)
        
        # Apply threshold
        binary_edges = (edge_magnitude > threshold).astype(float)
        
        # Convert back to flat list
        self.set_data(binary_edges.flatten().tolist())
        return self
    
    def __repr__(self) -> str:
        return (f"NMLVision({self._width}x{self._height}x{self._channels}, "
                f"type={self.get_type()})")


class NMLAudio:
    """
    Enhanced audio sensor with Pythonic interface.
    
    Provides audio processing and analysis functionality.
    """
    
    def __init__(self, sample_rate: int, num_samples: int):
        """
        Initialize audio sensor.
        
        Args:
            sample_rate: Samples per second
            num_samples: Number of audio samples
            
        Raises:
            ValidationError: If parameters are invalid
        """
        if sample_rate <= 0:
            raise ValidationError(f"Sample rate must be positive, got {sample_rate}")
        
        if num_samples <= 0:
            raise ValidationError(f"Number of samples must be positive, got {num_samples}")
        
        self._sample_rate = sample_rate
        self._num_samples = num_samples
        self._audio = Audio(sample_rate, num_samples)
    
    def set_data(self, data: List[float]) -> None:
        """
        Set audio data.
        
        Args:
            data: List of audio samples (num_samples)
            
        Raises:
            ValidationError: If data size is incorrect
        """
        if len(data) != self._num_samples:
            raise ValidationError(
                f"Audio data size mismatch: expected {self._num_samples} samples, "
                f"got {len(data)}"
            )
        
        self._audio.setData(data)
    
    def get_data(self) -> List[float]:
        """Get audio data as list."""
        return self._audio.getData()
    
    def get_data_array(self) -> np.ndarray:
        """Get audio data as NumPy array."""
        return np.array(self.get_data())
    
    def get_sample_rate(self) -> int:
        """Get sample rate."""
        return self._audio.getSampleRate()
    
    def get_num_samples(self) -> int:
        """Get number of samples."""
        return self._audio.getNumSamples()
    
    def get_duration(self) -> float:
        """Get audio duration in seconds."""
        return self._num_samples / self._sample_rate
    
    def set_sample_rate(self, sample_rate: int) -> None:
        """Set sample rate."""
        self._audio.setSampleRate(sample_rate)
        self._sample_rate = sample_rate
    
    def get_frequency_spectrum(self) -> np.ndarray:
        """
        Compute frequency spectrum using FFT.
        
        Returns:
            Frequency spectrum as NumPy array
        """
        data = self.get_data_array()
        spectrum = np.fft.rfft(data)
        magnitude = np.abs(spectrum)
        return magnitude
    
    def get_dominant_frequency(self) -> float:
        """
        Find dominant frequency in audio.
        
        Returns:
            Dominant frequency in Hz
        """
        spectrum = self.get_frequency_spectrum()
        frequencies = np.fft.rfftfreq(self._num_samples, 1.0 / self._sample_rate)
        
        # Find peak frequency
        if len(spectrum) > 0:
            peak_idx = np.argmax(spectrum)
            return frequencies[peak_idx]
        
        return 0.0
    
    def normalize(self) -> 'NMLAudio':
        """
        Normalize audio amplitude to range [-1, 1].
        
        Returns:
            Self for method chaining
        """
        data = self.get_data()
        max_val = max(abs(x) for x in data) if data else 1.0
        
        if max_val > 0:
            normalized = [x / max_val for x in data]
            self.set_data(normalized)
        
        return self
    
    def add_silence(self, duration: float) -> 'NMLAudio':
        """
        Add silence to beginning of audio.
        
        Args:
            duration: Duration of silence in seconds
            
        Returns:
            Self for method chaining
        """
        silence_samples = int(duration * self._sample_rate)
        total_samples = self._num_samples + silence_samples
        
        # For simplicity, we'll just return self (in real implementation,
        # this would create new array with silence)
        return self
    
    def get_type(self) -> NeuronType:
        """Get sensor type."""
        return NeuronType.Sensory
    
    def __repr__(self) -> str:
        return (f"NMLAudio(sample_rate={self._sample_rate}, "
                f"num_samples={self._num_samples}, duration={self.get_duration():.3f}s)")


class NMLInternalSignals:
    """
    Enhanced internal signals sensor with Pythonic interface.
    
    Provides internal signal monitoring and analysis.
    """
    
    def __init__(self):
        """Initialize internal signals sensor."""
        self._internal = InternalSignals()
    
    def add_signal(self, value: float) -> None:
        """Add internal signal value."""
        self._internal.addSignal(value)
    
    def add_signals(self, values: List[float]) -> None:
        """Add multiple internal signal values."""
        for value in values:
            self.add_signal(value)
    
    def get_signals(self) -> List[float]:
        """Get all internal signal values."""
        return self._internal.getData()
    
    def get_all_signals(self) -> List[float]:
        """Get all signals (alias for get_signals)."""
        return self.get_signals()
    
    def clear_signals(self) -> None:
        """Clear all internal signals."""
        self._internal.clearSignals()
    
    def get_signal_count(self) -> int:
        """Get number of signal values."""
        return len(self.get_signals())
    
    def get_mean(self) -> float:
        """Get mean of signal values."""
        signals = self.get_signals()
        return sum(signals) / len(signals) if signals else 0.0
    
    def get_std_dev(self) -> float:
        """Get standard deviation of signal values."""
        signals = self.get_signals()
        if not signals:
            return 0.0
        
        mean = self.get_mean()
        variance = sum((x - mean) ** 2 for x in signals) / len(signals)
        return variance ** 0.5
    
    def get_min(self) -> float:
        """Get minimum signal value."""
        signals = self.get_signals()
        return min(signals) if signals else 0.0
    
    def get_max(self) -> float:
        """Get maximum signal value."""
        signals = self.get_signals()
        return max(signals) if signals else 0.0
    
    def normalize(self, target_range: Tuple[float, float] = (-1.0, 1.0)) -> 'NMLInternalSignals':
        """
        Normalize signal values to target range.
        
        Args:
            target_range: Target range as (min, max)
            
        Returns:
            Self for method chaining
        """
        signals = self.get_signals()
        if not signals:
            return self
        
        min_val = self.get_min()
        max_val = self.get_max()
        
        if min_val == max_val:
            # All values are the same, scale to middle of range
            target_min, target_max = target_range
            normalized = [(target_min + target_max) / 2] * len(signals)
        else:
            # Scale to target range
            target_min, target_max = target_range
            normalized = []
            
            for value in signals:
                normalized_value = (
                    (value - min_val) / (max_val - min_val) *
                    (target_max - target_min) + target_min
                )
                normalized.append(normalized_value)
        
        # For InternalSignals, we need to clear and add signals individually
        self.clear_signals()
        for value in normalized:
            self.add_signal(value)
        
        return self
    
    def get_type(self) -> NeuronType:
        """Get sensor type."""
        return NeuronType.Sensory
    
    def __repr__(self) -> str:
        return f"NMLInternalSignals(signals={self.get_signal_count()})"


class NMLSensoryPercept:
    """
    Enhanced sensory percept with Pythonic interface.
    
    Provides unified interface for all sensory modalities.
    """
    
    def __init__(self):
        """Initialize sensory percept."""
        self._percept = SensoryPercept()
    
    def get_vision(self) -> NMLVision:
        """Get vision component."""
        return NMLVision(
            self._percept.getVisionWidth(),
            self._percept.getVisionHeight()
        )
    
    def set_vision(self, vision_data) -> None:
        """
        Set vision component.
        
        Args:
            vision_data: Vision data (list or NMLVision)
        """
        if isinstance(vision_data, NMLVision):
            self._percept.setVision(vision_data)
        else:
            self._percept.setVision(vision_data)
    
    def get_vision_width(self) -> int:
        """Get vision width."""
        return self._percept.getVisionWidth()
    
    def get_vision_height(self) -> int:
        """Get vision height."""
        return self._percept.getVisionHeight()
    
    def get_touch(self) -> List[float]:
        """Get touch component."""
        return self._percept.getTouch()
    
    def set_touch(self, touch_data: List[float]) -> None:
        """Set touch component."""
        self._percept.setTouch(touch_data)
    
    def get_internal(self) -> List[float]:
        """Get internal signals component."""
        return self._percept.getInternal()
    
    def set_internal(self, internal_data: List[float]) -> None:
        """Set internal signals component."""
        self._percept.setInternal(internal_data)
    
    def get_proprioception(self) -> List[float]:
        """Get proprioception component."""
        return self._percept.getProprioception()
    
    def set_proprioception(self, proprio_data: List[float]) -> None:
        """Set proprioception component."""
        self._percept.setProprioception(proprio_data)
    
    def get_audio(self) -> NMLAudio:
        """Get audio component."""
        audio_data = self._percept.getAudio()
        if audio_data:
            return NMLAudio(44100, len(audio_data))  # Assume 44.1kHz
    
    def set_audio(self, audio_data) -> None:
        """
        Set audio component.
        
        Args:
            audio_data: Audio data (list or NMLAudio)
        """
        if isinstance(audio_data, NMLAudio):
            self._percept.setAudio(audio_data.get_data())
        else:
            self._percept.setAudio(audio_data)
    
    def get_all_signals(self) -> Dict[str, Any]:
        """
        Get all sensory signals as dictionary.
        
        Returns:
            Dictionary with all signal components
        """
        return {
            'vision': self.get_vision(),
            'touch': self.get_touch(),
            'internal': self.get_internal(),
            'proprioception': self.get_proprioception(),
            'audio': self.get_audio(),
            'timestamp': self.get_timestamp(),
        }
    
    def get_timestamp(self) -> float:
        """Get timestamp."""
        return self._percept.getTimestamp()
    
    def set_timestamp(self, timestamp: float) -> None:
        """Set timestamp."""
        self._percept.setTimestamp(timestamp)
    
    def get_type(self) -> NeuronType:
        """Get sensor type."""
        return NeuronType.Sensory
    
    def __repr__(self) -> str:
        return f"NMLSensoryPercept(vision={self.get_vision_width()}x{self.get_vision_height()})"


# Convenience constructors
def create_vision(width: int, height: int, channels: int = 3) -> NMLVision:
    """Create vision sensor."""
    return NMLVision(width, height, channels)
def create_audio(sample_rate: int, num_samples: int) -> NMLAudio:
    """Create audio sensor."""
    return NMLAudio(sample_rate, num_samples)
def create_internal_signals() -> NMLInternalSignals:
    """Create internal signals sensor."""
    return NMLInternalSignals()
def create_sensory_percept() -> NMLSensoryPercept:
    """Create sensory percept."""
    return NMLSensoryPercept()

# Type aliases
VisionType = NMLVision
AudioType = NMLAudio
InternalSignalsType = NMLInternalSignals
SensoryPerceptType = NMLSensoryPercept

# Export all main classes
__all__.extend([
    'NMLVision', 'NMLAudio', 'NMLInternalSignals', 'NMLSensoryPercept',
    'create_vision', 'create_audio', 'create_internal_signals', 
    'create_sensory_percept',
    'VisionType', 'AudioType', 'InternalSignalsType', 'SensoryPerceptType'
])
