"""Basic sensory input implementation."""

from typing import List, Any

class SensoryInput:
    """Base class for sensory input."""
    
    def __init__(self):
        self._timestamp = 0.0
    
    def getType(self) -> int:
        return 0  # Internal
    
    def getData(self) -> List[float]:
        return []
    
    def getDimensions(self) -> int:
        return 0
    
    def getTimestamp(self) -> float:
        return self._timestamp
    
    def setTimestamp(self, timestamp: float):
        self._timestamp = timestamp

class Vision(SensoryInput):
    """Vision sensory input."""
    
    def __init__(self, width: int = 0, height: int = 0, channels: int = 3):
        super().__init__()
        self._width = width
        self._height = height
        self._channels = channels
        self._data: List[float] = []
    
    def setData(self, data: List[float]):
        self._data = data
    
    def getWidth(self) -> int:
        return self._width
    
    def getHeight(self) -> int:
        return self._height
    
    def getChannels(self) -> int:
        return self._channels

class Audio(SensoryInput):
    """Audio sensory input."""
    
    def __init__(self, sample_rate: int = 0, num_samples: int = 0):
        super().__init__()
        self._sample_rate = sample_rate
        self._num_samples = num_samples
        self._data: List[float] = []
    
    def setData(self, data: List[float]):
        self._data = data
    
    def setSampleRate(self, sample_rate: int):
        self._sample_rate = sample_rate
    
    def getSampleRate(self) -> int:
        return self._sample_rate
    
    def getNumSamples(self) -> int:
        return self._num_samples

class InternalSignals(SensoryInput):
    """Internal signals sensory input."""
    
    def __init__(self):
        super().__init__()
        self._signals: List[float] = []
    
    def addSignal(self, value: float):
        self._signals.append(value)
    
    def clearSignals(self):
        self._signals.clear()

__all__ = ['SensoryInput', 'Vision', 'Audio', 'InternalSignals']