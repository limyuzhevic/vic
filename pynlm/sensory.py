# Sensory processing interfaces for NLM
from typing import List, Optional, Any
import numpy as np

class SensoryInput:
    """Base class for sensory input in NLM."""
    
    def __init__(self):
        """Initialize sensory input."""
        self._timestamp = 0.0
    
    def getType(self) -> str:
        """Get the type of sensory input."""
        return self.__class__.__name__
    
    def getData(self) -> List[float]:
        """Get the raw data as a list of floats."""
        return []
    
    def getDimensions(self) -> int:
        """Get the dimensionality of the sensory data."""
        return len(self.getData())
    
    def getTimestamp(self) -> float:
        """Get the timestamp of this sensory input."""
        return self._timestamp
    
    def setTimestamp(self, timestamp: float) -> None:
        """Set the timestamp of this sensory input."""
        self._timestamp = timestamp

class Vision(SensoryInput):
    """Visual sensory input for NLM."""
    
    def __init__(self, width: int = 0, height: int = 0, channels: int = 3):
        """Initialize vision sensory."""
        super().__init__()
        self._width = width
        self._height = height
        self._channels = channels
        self._data = np.zeros(width * height * channels, dtype=np.float32)
    
    def setData(self, data: List[float]) -> None:
        """Set vision data."""
        if len(data) == self._width * self._height * self._channels:
            self._data = np.array(data, dtype=np.float32)
        else:
            raise ValueError(f"Data length {len(data)} doesn't match expected {self._width * self._height * self._channels}")
    
    def getWidth(self) -> int:
        """Get vision width."""
        return self._width
    
    def getHeight(self) -> int:
        """Get vision height."""
        return self._height
    
    def getChannels(self) -> int:
        """Get number of channels."""
        return self._channels
    
    def getData(self) -> np.ndarray:
        """Get vision data as numpy array."""
        return self._data.copy()
    
    def getNormalizedData(self) -> np.ndarray:
        """Get normalized vision data (0-1)."""
        return np.clip(self._data, 0.0, 1.0)

class Audio(SensoryInput):
    """Audio sensory input for NLM."""
    
    def __init__(self, sampleRate: int = 0, numSamples: int = 0):
        """Initialize audio sensory."""
        super().__init__()
        self._sample_rate = sampleRate
        self._num_samples = numSamples
        self._data = np.zeros(numSamples, dtype=np.float32)
    
    def setData(self, data: List[float]) -> None:
        """Set audio data."""
        if len(data) == self._num_samples:
            self._data = np.array(data, dtype=np.float32)
        else:
            raise ValueError(f"Data length {len(data)} doesn't match expected {self._num_samples}")
    
    def setSampleRate(self, sampleRate: int) -> None:
        """Set sample rate."""
        self._sample_rate = sampleRate
    
    def getSampleRate(self) -> int:
        """Get sample rate."""
        return self._sample_rate
    
    def getNumSamples(self) -> int:
        """Get number of samples."""
        return self._num_samples
    
    def getData(self) -> np.ndarray:
        """Get audio data as numpy array."""
        return self._data.copy()

class InternalSignals(SensoryInput):
    """Internal signals sensory input for NLM."""
    
    def __init__(self):
        """Initialize internal signals."""
        super().__init__()
        self._signals = []
    
    def addSignal(self, value: float) -> None:
        """Add a signal value."""
        self._signals.append(value)
    
    def clearSignals(self) -> None:
        """Clear all signals."""
        self._signals.clear()
    
    def getData(self) -> List[float]:
        """Get all signal values."""
        return self._signals.copy()
    
    def getSignal(self, index: int) -> float:
        """Get signal at specific index."""
        if 0 <= index < len(self._signals):
            return self._signals[index]
        return 0.0
    
    def getNumSignals(self) -> int:
        """Get number of signals."""
        return len(self._signals)
