'''Sensory input classes for pynlm'''

from __future__ import annotations

from typing import List, Optional, Any
from .core.types import SimulationStep, Timestamp
from .core.enums import NeuronType
from .core.types import NeuronId


class SensoryInput:
    """Base class for sensory input.
    
    The SensoryInput class provides the foundation for all sensory modalities,
    including vision, audio, and internal signals. It defines the interface
    for processing and transmitting sensory data to the brain.
    """
    
    def __init__(self):
        """Initialize a SensoryInput instance."""
        self.timestamp: Timestamp = 0.0
        self.data: List[float] = []
        self.type: str = "base"
        self.dimensions: int = 0
    
    def getType(self) -> str:
        """Get the type of sensory input.
        
        Returns:
            String representing the type of sensory input.
        """
        return self.type
    
    def getData(self) -> List[float]:
        """Get the raw data as a vector.
        
        Returns:
            List of floating-point values representing the data.
        """
        return self.data.copy()
    
    def getDimensions(self) -> int:
        """Get the dimensionality of the data.
        
        Returns:
            Number INSTRUCTIONS
        """
        return self.dimensions
    
    def getTimestamp(self) -> Timestamp:
        """Get the timestamp.
        
        Returns:
            Timestamp when the data was generated.
        """
        return self.timestamp
    
    def setTimestamp(self, timestamp: Timestamp):
        """Set the timestamp.
        
        Args:
            timestamp: New timestamp.
        """
        self.timestamp = timestamp
    
    def __repr__(self) -> str:
        """Get string representation of the sensory input.
        
        Returns:
            String representation of the sensory input.
        """
        return f"<{self.__class__.__name__} (type: {self.type}, " \
               f"data size: {len(self.data)})>"


class Vision(SensoryInput):
    """Vision sensory input for visual perception.
    
    The Vision class represents visual input from the environment,
    including information about colors, shapes, and spatial relationships.
    """
    
    def __init__(self, width: int = 0, height: int = 0, channels: int = 3):
        """Initialize a Vision instance.
        
        Args:
            width: Width of the visual field in pixels.
            height: Height of the visual field in pixels.
            channels: Number of color channels (typically 3 for RGB).
        """
        super().__init__()
        self.type = "vision"
        self.width = width
        self.height = height
        self.channels = channels
        self.dimensions = width * height * channels
        
        # Initialize visual data
        self.data = [0.0] * self.dimensions
    
    def setData(self, data: List[float]):
        """Set visual data.
        
        Args:
            data: List of RGB values for each pixel.
        """
        if len(data) != self.dimensions:
            raise ValueError(f"Expected {self.dimensions} values for vision data, "
                           f"got {len(data)}")
        
        self.data = data.copy()
    
    def getWidth(self) -> int:
        """Get vision width.
        
        Returns:
            Width in pixels.
        """
        return self.width
    
    def getHeight(self) -> int:
        """Get vision height.
        
        Returns:
            Height in pixels.
        """
        return self.height
    
    def getChannels(self) -> int:
        """Get number of color channels.
        
        Returns:
            Number of color channels.
        """
        return self.channels
    
    def __repr__(self) -> str:
        """Get string representation of vision data.
        
        Returns:
            String representation of vision data.
        """
        return f"<Vision ({self.width}x{self.height}x{self.channels}, " \
               f"timestamp: {self.timestamp:.2f}s)>"


class Audio(SensoryInput):
    """Audio sensory input for sound perception.
    
    The Audio class represents auditory input from the environment,
    including frequency, amplitude, and temporal information.
    """
    
    def __init__(self, sample_rate: int = 0, num_samples: int = 0):
        """Initialize an Audio instance.
        
        Args:
            sample_rate: Number of samples per second.
            num_samples: Number of audio samples.
        """
        super().__init__()
        self.type = "audio"
        self.sample_rate = sample_rate
        self.num_samples = num_samples
        self.dimensions = num_samples
        
        # Initialize audio data
        self.data = [0.0] * self.num_samples
    
    def setData(self, data: List[float]):
        """Set audio data.
        
        Args:
            data: List of audio sample values.
        """
        if len(data) != self.num_samples:
            raise ValueError(f"Expected {self.num_samples} audio samples, "
                           f"got {len(data)}")
        
        self.data = data.copy()
    
    def setSampleRate(self, sample_rate: int):
        """Set audio sample rate.
        
        Args:
            sample_rate: Number of samples per second.
        """
        self.sample_rate = sample_rate
    
    def getSampleRate(self) -> int:
        """Get audio sample rate.
        
        Returns:
            Number of samples per second.
        """
        return self.sample_rate
    
    def getNumSamples(self) -> int:
        """Get number of audio samples.
        
        Returns:
            Number of audio samples.
        """
        return self.num_samples
    
    def __repr__(self) -> str:
        """Get string representation of audio data.
        
        Returns:
            String representation of audio data.
        """
        return f"<Audio (sample_rate: {self.sample_rate}, " \
               f"samples: {self.num_samples}, timestamp: {self.timestamp:.2f}s)>"


class InternalSignals(SensoryInput):
    """Internal signals sensory input for body state.
    
    The InternalSignals class represents internal physiological signals
    such as energy, health, and other body state information.
    """
    
    def __init__(self):
        """Initialize InternalSignals instance."""
        super().__init__()
        self.type = "internal"
        self.dimensions = 16  # Number of internal signal channels
        
        # Initialize internal signals
        self.data = [0.0] * self.dimensions
    
    def addSignal(self, value: float):
        """Add an internal signal value.
        
        Args:
            value: Signal value to add.
        """
        # Add signal to first available slot
        for i, existing_value in enumerate(self.data):
            if existing_value == 0.0:
                self.data[i] = value
                return
        
        # If no slot available, add to end (simple implementation)
        if len(self.data) < self.dimensions:
            self.data.append(value)
            self.dimensions += 1
    
    def clearSignals(self):
        """Clear all internal signals."""
        self.data = [0.0] * self.dimensions
    
    def __repr__(self) -> str:
        """Get string representation of internal signals.
        
        Returns:
            String representation of internal signals.
        """
        active_signals = sum(1 for v in self.data if v != 0.0)
        return f"<InternalSignals (active: {active_signals}, " \
               f"timestamp: {self.timestamp:.2f}s)>"