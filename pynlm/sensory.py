"""
Enhanced Python interface for NLM sensory processing classes
"""

from typing import List, Dict, Any, Optional, Tuple
from enum import Enum
from .exceptions import NLMError

class NeuronType(Enum):
    """Neuron type enumeration"""
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    SENSORY = "sensory"
    MOTOR = "motor"
    INTERNAL = "internal"

class SensoryInput:
    """Base class for sensory input"""
    
    def __init__(self, input_type: str = "generic"):
        self.type = input_type
        self.timestamp = 0.0
        self.data = []
        
    def get_type(self) -> str:
        """Get sensory input type"""
        return self.type
        
    def get_data(self) -> List[float]:
        """Get raw sensory data"""
        return self.data
        
    def set_data(self, data: List[float]) -> None:
        """Set raw sensory data"""
        self.data = data
        
    def get_dimensions(self) -> int:
        """Get dimensionality of sensory data"""
        return len(self.data)
        
    def get_timestamp(self) -> float:
        """Get timestamp"""
        return self.timestamp
        
    def set_timestamp(self, timestamp: float) -> None:
        """Set timestamp"""
        self.timestamp = timestamp
        
    def __repr__(self):
        return f"<SensoryInput: {self.type}, dims={self.get_dimensions()}>"

class Vision(SensoryInput):
    """Vision sensory input"""
    
    def __init__(self, width: int = 0, height: int = 0, channels: int = 3):
        super().__init__("vision")
        self.width = width
        self.height = height
        self.channels = channels
        
    def set_data(self, data: List[float]) -> None:
        """Set vision data (expects flat list)"""
        self.data = data
        self.width = int(len(data) ** 0.5) if data else 0
        self.height = self.width
        
    def get_width(self) -> int:
        """Get vision width"""
        return self.width
        
    def get_height(self) -> int:
        """Get vision height"""
        return self.height
        
    def get_channels(self) -> int:
        """Get vision channels"""
        return self.channels
        
    def get_pixels(self) -> List[List[List[float]]]:
        """Get vision data as 2D array of RGB values"""
        if not self.data:
            return []
            
        pixels = []
        for y in range(self.height):
            row = []
            for x in range(self.width):
                idx = y * self.width + x
                if idx < len(self.data):
                    pixel = []
                    for c in range(min(self.channels, len(self.data) - idx)):
                        pixel.append(self.data[idx + c] if idx + c < len(self.data) else 0.0)
                    while len(pixel) < self.channels:
                        pixel.append(0.0)
                    row.append(pixel)
                else:
                    row.append([0.0] * self.channels)
            pixels.append(row)
        return pixels
        
    def __repr__(self):
        return f"<Vision: {self.width}x{self.height}x{self.channels}>"

class Audio(SensoryInput):
    """Audio sensory input"""
    
    def __init__(self, sample_rate: int = 0, num_samples: int = 0):
        super().__init__("audio")
        self.sample_rate = sample_rate
        self.num_samples = num_samples
        
    def set_data(self, data: List[float]) -> None:
        """Set audio data"""
        self.data = data
        self.num_samples = len(data)
        if self.sample_rate == 0:
            self.sample_rate = 44100  # Default sample rate
            
    def set_sample_rate(self, sample_rate: int) -> None:
        """Set sample rate"""
        self.sample_rate = sample_rate
        
    def get_sample_rate(self) -> int:
        """Get sample rate"""
        return self.sample_rate
        
    def get_num_samples(self) -> int:
        """Get number of samples"""
        return self.num_samples
        
    def get_duration(self) -> float:
        """Get audio duration in seconds"""
        return self.num_samples / self.sample_rate if self.sample_rate > 0 else 0.0
        
    def __repr__(self):
        return f"<Audio: {self.num_samples} samples at {self.sample_rate} Hz, duration={self.get_duration():.2f}s>"

class InternalSignals(SensoryInput):
    """Internal signals sensory input"""
    
    def __init__(self):
        super().__init__("internal")
        
    def add_signal(self, value: float) -> None:
        """Add internal signal value"""
        self.data.append(value)
        
    def clear_signals(self) -> None:
        """Clear all signals"""
        self.data.clear()
        
    def get_signal_count(self) -> int:
        """Get number of signals"""
        return len(self.data)
        
    def get_mean(self) -> float:
        """Get mean of signals"""
        if not self.data:
            return 0.0
        return sum(self.data) / len(self.data)
        
    def get_std(self) -> float:
        """Get standard deviation of signals"""
        if len(self.data) <= 1:
            return 0.0
        mean = self.get_mean()
        variance = sum((x - mean) ** 2 for x in self.data) / (len(self.data) - 1)
        return variance ** 0.5
        
    def __repr__(self):
        return f"<InternalSignals: {self.get_signal_count()} signals, mean={self.get_mean():.3f}, std={self.get_std():.3f}>"
