"""Sensory input implementation with Pythonic interface"""

from typing import List, Dict, Any, Optional
from dataclasses import dataclass
from enum import Enum

class SensoryModality(Enum):
    """Sensory modalities."""
    
    VISION = "vision"
    AUDIO = "audio"
    TOUCH = "touch"
    PROPRIOCEPTION = "proprioception"
    INTERNAL = "internal"
    INTEROCEPTIVE = "interoceptive"
@dataclass
class SensoryMetadata:
    """Metadata for sensory input."""
    
    timestamp: float = 0.0
    modality: SensoryModality = SensoryModality.VISION
    confidence: float = 1.0
    noise_level: float = 0.0
    reliability: float = 1.0
class VisionInput:
    """Vision sensory input with Pythonic interface."""
    
    def __init__(self, width: int = 16, height: int = 16, channels: int = 3):
        self._width = width
        self._height = height
        self._channels = channels
        self._data: List[float] = [0.0] * (width * height * channels)
        self._metadata = SensoryMetadata(SensoryModality.VISION)
    
    def set_data(self, data: List[float]):
        """Set vision data."""
        if len(data) == self._width * self._height * self._channels:
            self._data = data[:]
        else:
            raise ValueError(f"Vision data size mismatch. Expected {self._width * self._height * self._channels}, got {len(data)}")
    
    def get_data(self) -> List[float]:
        """Get vision data."""
        return self._data.copy()
    
    def get_width(self) -> int:
        """Get vision width."""
        return self._width
    
    def get_height(self) -> int:
        """Get vision height."""
        return self._height
    
    def get_channels(self) -> int:
        """Get vision channels."""
        return self._channels
    
    def resize(self, width: int, height: int, channels: int = 3):
        """Resize vision input."""
        self._width = width
        self._height = height
        self._channels = channels
        self._data = [0.0] * (width * height * channels)
    
    def get_dimensions(self) -> tuple:
        """Get vision dimensions."""
        return (self._width, self._height, self._channels)
    
    def normalize(self, min_val: float = 0.0, max_val: float = 255.0):
        """Normalize vision data to [0, 1] range."""
        for i in range(len(self._data)):
            self._data[i] = (self._data[i] - min_val) / (max_val - min_val)
    
    def get_center_of_mass(self) -> tuple:
        """Get center of mass of vision data (simplified)."""
        total_weight = sum(self._data)
        if total_weight <= 0:
            return (self._width // 2, self._height // 2)
        
        cx = sum(i * v for i, v in enumerate(self._data)) / total_weight
        cy = sum((i // self._width) * v for i, v in enumerate(self._data)) / total_weight
        
        return (int(cx), int(cy))
    
    def get_edge_detection(self, threshold: float = 0.5) -> List[float]:
        """Detect edges in vision data."""
        edges = []
        
        if self._width < 3 or self._height < 3:
            return edges
        
        for y in range(self._height):
            for x in range(self._width):
                idx = y * self._width + x
                
                # Simple edge detection using neighboring pixels
                if x > 0 and x < self._width - 1:
                    left = self._data[idx - 1]
                    right = self._data[idx + 1]
                    edge_strength = abs(right - left)
                    edges.append(min(edge_strength, 1.0))
                else:
                    edges.append(0.0)
        
        return edges
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert vision input to dictionary."""
        return {
            'width': self._width,
            'height': self._height,
            'channels': self._channels,
            'data': self._data,
            'center_of_mass': self.get_center_of_mass(),
            'total_intensity': sum(self._data),
            'average_intensity': sum(self._data) / len(self._data) if self._data else 0
        }
    
    def __repr__(self):
        return f"<VisionInput({self._width}x{self._height}x{self._channels}, intensity={sum(self._data):.3f})>"
class AudioInput:
    """Audio sensory input with Pythonic interface."""
    
    def __init__(self, sample_rate: int = 44100, duration: float = 1.0):
        self._sample_rate = sample_rate
        self._duration = duration
        self._num_samples = int(sample_rate * duration)
        self._data: List[float] = [0.0] * self._num_samples
        self._metadata = SensoryMetadata(SensoryModality.AUDIO)
    
    def set_data(self, data: List[float]):
        """Set audio data."""
        if len(data) == self._num_samples:
            self._data = data[:]
        else:
            raise ValueError(f"Audio data size mismatch. Expected {self._num_samples}, got {len(data)}")
    
    def get_data(self) -> List[float]:
        """Get audio data."""
        return self._data.copy()
    
    def set_sample_rate(self, sample_rate: int):
        """Set sample rate."""
        self._sample_rate = sample_rate
        self._num_samples = int(sample_rate * self._duration)
        # Keep existing samples, pad with zeros if needed
        if len(self._data) < self._num_samples:
            self._data.extend([0.0] * (self._num_samples - len(self._data)))
        elif len(self._data) > self._num_samples:
            self._data = self._data[:self._num_samples]
    
    def get_sample_rate(self) -> int:
        """Get sample rate."""
        return self._sample_rate
    
    def get_num_samples(self) -> int:
        """Get number of samples."""
        return self._num_samples
    
    def get_duration(self) -> float:
        """Get duration in seconds."""
        return self._duration
    
    def set_duration(self, duration: float):
        """Set duration."""
        self._duration = duration
        self._num_samples = int(self._sample_rate * duration)
        # Keep existing samples, pad with zeros if needed
        if len(self._data) < self._num_samples:
            self._data.extend([0.0] * (self._num_samples - len(self._data)))
        elif len(self._data) > self._num_samples:
            self._data = self._data[:self._num_samples]
    
    def get_dimensions(self) -> tuple:
        """Get audio dimensions."""
        return (self._sample_rate, self._duration)
    
    def get_frequency_analysis(self, fft_size: int = 1024) -> Dict[str, Any]:
        """Get frequency analysis of audio data."""
        import numpy as np
        
        if len(self._data) < fft_size:
            return {'error': 'Insufficient data for FFT analysis'}
        
        # Perform FFT
        fft_result = np.fft.fft(self._data[:fft_size])
        freqs = np.fft.fftfreq(fft_size, 1.0 / self._sample_rate)
        
        # Get magnitude spectrum
        magnitudes = np.abs(fft_result)
        
        # Find dominant frequency
        positive_freqs = freqs[:fft_size//2]
        positive_magnitudes = magnitudes[:fft_size//2]
        
        if len(positive_magnitudes) > 0:
            dominant_idx = np.argmax(positive_magnitudes)
            dominant_freq = positive_freqs[dominant_idx]
            dominant_magnitude = positive_magnitudes[dominant_idx]
            
            return {
                'dominant_frequency': dominant_freq,
                'dominant_magnitude': dominant_magnitude,
                'mean_frequency': np.mean(positive_freqs),
                'frequency_range': (np.min(positive_freqs), np.max(positive_freqs)),
                'power_spectrum': positive_magnitudes.tolist()
            }
        
        return {'error': 'Could not compute frequency analysis'}
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert audio input to dictionary."""
        return {
            'sample_rate': self._sample_rate,
            'duration': self._duration,
            'num_samples': self._num_samples,
            'data': self._data,
            'mean_amplitude': sum(self._data) / len(self._data) if self._data else 0,
            'peak_amplitude': max(abs(x) for x in self._data) if self._data else 0,
            'rms_amplitude': np.sqrt(np.mean(np.square(self._data))) if self._data else 0
        }
    
    def __repr__(self):
        return f"<AudioInput({self._sample_rate}Hz, {self._duration}s, {self._num_samples} samples)>"
class ProprioceptiveInput:
    """Proprioceptive sensory input with Pythonic interface."""
    
    def __init__(self, joint_count: int = 6):
        self._joint_count = joint_count
        self._data: List[float] = [0.0] * joint_count
        self._metadata = SensoryMetadata(SensoryModality.PROPRIOCEPTION)
    
    def set_data(self, data: List[float]):
        """Set proprioceptive data."""
        if len(data) == self._joint_count:
            self._data = data[:]
        else:
            raise ValueError(f"Proprioceptive data size mismatch. Expected {self._joint_count}, got {len(data)}")
    
    def get_data(self) -> List[float]:
        """Get proprioceptive data."""
        return self._data.copy()
    
    def get_joint_angles(self) -> List[float]:
        """Get joint angles."""
        return self._data.copy()
    
    def set_joint_angles(self, angles: List[float]):
        """Set joint angles."""
        if len(angles) == self._joint_count:
            self._data = angles[:]
        else:
            raise ValueError(f"Joint angles size mismatch. Expected {self._joint_count}, got {len(angles)}")
    
    def get_magnitude(self) -> float:
        """Get magnitude of proprioceptive input."""
        return sum(abs(x) for x in self._data)
    
    def normalize(self, max_magnitude: float = 1.0):
        """Normalize proprioceptive data."""
        magnitude = self.get_magnitude()
        if magnitude > 0:
            scale = max_magnitude / magnitude
            for i in range(len(self._data)):
                self._data[i] *= scale
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert proprioceptive input to dictionary."""
        return {
            'joint_count': self._joint_count,
            'joint_angles': self._data,
            'magnitude': self.get_magnitude(),
            'normalized_magnitude': self.get_magnitude() / (self._joint_count * 2.0)  # Assuming max angle of 2 radians
        }
    
    def __repr__(self):
        return f"<ProprioceptiveInput({self._joint_count} joints, magnitude={self.get_magnitude():.3f})>"
class InternalSignals:
    """Internal signals sensory input with Pythonic interface."""
    
    def __init__(self, signal_count: int = 4):
        self._signal_count = signal_count
        self._data: List[float] = [0.0] * signal_count
        self._metadata = SensoryMetadata(SensoryModality.INTERNAL)
    
    def add_signal(self, value: float, signal_index: int = None):
        """Add signal value."""
        if signal_index is None:
            # Add to next available slot
            for i in range(self._signal_count):
                if self._data[i] == 0.0:
                    self._data[i] = value
                    return
            # If all slots filled, append (resize)
            self._signal_count += 1
            self._data.append(value)
        else:
            if 0 <= signal_index < self._signal_count:
                self._data[signal_index] = value
            else:
                raise IndexError(f"Signal index {signal_index} out of range [0, {self._signal_count - 1}]")
    
    def clear_signals(self):
        """Clear all signals."""
        self._data = [0.0] * self._signal_count
    
    def get_signals(self) -> List[float]:
        """Get all signals."""
        return self._data.copy()
    
    def get_signal(self, index: int) -> float:
        """Get signal at specific index."""
        if 0 <= index < self._signal_count:
            return self._data[index]
        return 0.0
    
    def get_signal_count(self) -> int:
        """Get number of signals."""
        return self._signal_count
    
    def get_magnitude(self) -> float:
        """Get magnitude of internal signals."""
        return sum(abs(x) for x in self._data)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert internal signals to dictionary."""
        return {
            'signal_count': self._signal_count,
            'signals': self._data,
            'magnitude': self.get_magnitude(),
            'average_signal': sum(self._data) / len(self._data) if self._data else 0
        }
    
    def __repr__(self):
        return f"<InternalSignals({self._signal_count} signals, magnitude={self.get_magnitude():.3f})>"
# Main sensory input class (abstract base)

class SensoryInput:
    """Base class for sensory input with Pythonic interface."""
    
    def __init__(self):
        self._metadata = SensoryMetadata()
    
    def getType(self) -> str:
        """Get sensory input type."""
        return self._metadata.modality.value
    
    def getData(self) -> List[float]:
        """Get raw data as list."""
        return self._get_raw_data()
    
    def getDimensions(self) -> int:
        """Get dimensionality."""
        return len(self._get_raw_data())
    
    def getTimestamp(self) -> float:
        """Get timestamp."""
        return self._metadata.timestamp
    
    def setTimestamp(self, timestamp: float):
        """Set timestamp."""
        self._metadata.timestamp = timestamp
    
    def _get_raw_data(self) -> List[float]:
        """Get raw data (to be implemented by subclasses)."""
        return []
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert sensory input to dictionary."""
        return {
            'type': self.getType(),
            'timestamp': self.getTimestamp(),
            'data': self.getData(),
            'dimensions': self.getDimensions()
        }
    
    def __repr__(self):
        return f"<{self.__class__.__name__} type={self.getType()}>"


# Factory functions

def create_vision(width: int = 16, height: int = 16, channels: int = 3) -> VisionInput:
    """Create vision input."""
    return VisionInput(width, height, channels)


def create_audio(sample_rate: int = 44100, duration: float = 1.0) -> AudioInput:
    """Create audio input."""
    return AudioInput(sample_rate, duration)


def create_proprioceptive(joint_count: int = 6) -> ProprioceptiveInput:
    """Create proprioceptive input."""
    return ProprioceptiveInput(joint_count)


def create_internal_signals(signal_count: int = 4) -> InternalSignals:
    """Create internal signals."""
    return InternalSignals(signal_count)


# Export for backward compatibility
Vision = VisionInput
Audio = AudioInput
InternalSignals = InternalSignals
SensoryInput = SensoryInput