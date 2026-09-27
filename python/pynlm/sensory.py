"""
Sensory processing modules for NLM.

This module contains classes for sensory input processing, including
Vision, Audio, InternalSignals, and base SensoryInput classes.
"""

import numpy as np
from typing import List, Any, Optional
from enum import Enum

# Import from C++ bindings
import pynlm._bindings as bindings
class SensoryInput:
    """Base class for all sensory input."""
    
    def __init__(self, input_type: Any = None):
        self._input = bindings.create_sensory_input()
        if input_type:
            self._input.set_type(input_type)
    
    def get_type(self) -> Any:
        """Get the type of sensory input."""
        return self._input.getType()
    
    def get_data(self) -> List[float]:
        """Get the raw data as a vector."""
        return self._input.getData()
    
    def get_dimensions(self) -> int:
        """Get the dimensionality."""
        return self._input.getDimensions()
    
    def get_timestamp(self) -> float:
        """Get the timestamp."""
        return self._input.getTimestamp()
    
    def set_timestamp(self, timestamp: float) -> None:
        """Set the timestamp."""
        self._input.setTimestamp(timestamp)
    
    def set_data(self, data: List[float]) -> None:
        """Set the data (override implementation)."""
        self._input.setData(data)

class Vision(SensoryInput):
    """Vision sensory input (camera-like input)."""
    
    def __init__(self, width: int = 0, height: int = 0, channels: int = 3):
        super().__init__()
        self._vision = bindings.create_vision(width, height, channels)
    
    def set_data(self, data: List[float]) -> None:
        """Set vision data."""
        if len(data) != self.get_channels() * self.get_width() * self.get_height():
            raise ValueError(f"Data size mismatch. Expected {self.get_channels() * self.get_width() * self.get_height()} values, got {len(data)}")
        
        self._vision.setData(data)
    
    def get_width(self) -> int:
        """Get image width."""
        return self._vision.getWidth()
    
    def get_height(self) -> int:
        """Get image height."""
        return self._vision.getHeight()
    
    def get_channels(self) -> int:
        """Get number of color channels."""
        return self._vision.getChannels()
    
    def to_numpy(self) -> np.ndarray:
        """Convert vision data to NumPy array."""
        data = self.get_data()
        channels = self.get_channels()
        height = self.get_height()
        width = self.get_width()
        
        # Reshape to (height, width, channels) if channels > 1
        if channels > 1:
            return np.array(data).reshape(height, width, channels)
        else:
            return np.array(data).reshape(height, width)
    
    def from_numpy(self, array: np.ndarray) -> None:
        """Load vision data from NumPy array."""
        if array.ndim == 3:
            height, width, channels = array.shape
        else:
            height, width = array.shape
            channels = 1
        
        # Store dimensions
        if channels > 1:
            self._vision.setData(array.flatten().tolist())
        else:
            self._vision.setData(array.flatten().tolist())
    
    @classmethod
    def from_array(cls, array: np.ndarray, **kwargs) -> 'Vision':
        """Create Vision from NumPy array."""
        if array.ndim == 3:
            height, width, channels = array.shape
        else:
            height, width = array.shape
            channels = 1
        
        vision = cls(height, width, channels)
        vision.from_numpy(array)
        return vision

class Audio(SensoryInput):
    """Audio sensory input (sound signals)."""
    
    def __init__(self, sample_rate: int = 0, num_samples: int = 0):
        super().__init__()
        self._audio = bindings.create_audio(sample_rate, num_samples)
    
    def set_data(self, data: List[float]) -> None:
        """Set audio data."""
        if len(data) != self.get_num_samples():
            raise ValueError(f"Data size mismatch. Expected {self.get_num_samples()} samples, got {len(data)}")
        
        self._audio.setData(data)
    
    def set_sample_rate(self, sample_rate: int) -> None:
        """Set sample rate."""
        self._audio.setSampleRate(sample_rate)
    
    def get_sample_rate(self) -> int:
        """Get sample rate."""
        return self._audio.getSampleRate()
    
    def get_num_samples(self) -> int:
        """Get number of samples."""
        return self._audio.getNumSamples()
    
    def to_numpy(self) -> np.ndarray:
        """Convert audio data to NumPy array."""
        data = self.get_data()
        return np.array(data)
    
    def from_numpy(self, array: np.ndarray) -> None:
        """Load audio data from NumPy array."""
        if not np.issubdtype(array.dtype, np.floating):
            array = array.astype(np.float32)
        
        self._audio.setData(array.tolist())
    
    @classmethod
    def from_array(cls, array: np.ndarray, sample_rate: int = 44100) -> 'Audio':
        """Create Audio from NumPy array."""
        audio = cls(sample_rate, len(array))
        audio.from_numpy(array)
        return audio

class InternalSignals(SensoryInput):
    """Internal neural signals (proprioception, interoception)."""
    
    def __init__(self):
        super().__init__()
        self._internal = bindings.create_internal_signals()
    
    def add_signal(self, value: float) -> None:
        """Add a signal value."""
        self._internal.addSignal(value)
    
    def clear_signals(self) -> None:
        """Clear all signals."""
        self._internal.clearSignals()
    
    def get_signals(self) -> List[float]:
        """Get all signals."""
        return self._internal.getSignals()
    
    def get_num_signals(self) -> int:
        """Get number of signals."""
        return self.get_dimensions()

# Convenience factory functions
def create_vision(width: int, height: int, channels: int = 3) -> Vision:
    """Create vision sensory input."""
    return Vision(width, height, channels)

def create_audio(sample_rate: int, num_samples: int) -> Audio:
    """Create audio sensory input."""
    return Audio(sample_rate, num_samples)

def create_internal_signals() -> InternalSignals:
    """Create internal signals sensory input."""
    return InternalSignals()

def create_sensory_input(input_type: Any = None) -> SensoryInput:
    """Create base sensory input."""
    return SensoryInput(input_type)