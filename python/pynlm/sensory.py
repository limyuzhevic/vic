# Sensory module for NLM
from typing import Optional, List, Union
import time

class SensoryInput:
    """
    Base class for all sensory input.
    
    Provides common interface for sensory data from the environment
    and internal brain state.
    """
    
    def __init__(self):
        self._timestamp = time.time()
    
    def getType(self) -> str:
        """Get input type."""
        return self.__class__.__name__
    
    def getData(self) -> List[float]:
        """Get raw data as vector."""
        return []
    
    def getDimensions(self) -> int:
        """Get dimensionality."""
        return len(self.getData())
    
    def getTimestamp(self) -> float:
        """Get timestamp."""
        return self._timestamp
    
    def setTimestamp(self, timestamp: float) -> None:
        """Set timestamp."""
        self._timestamp = timestamp
    
    def clone(self):
        """Clone sensory input."""
        raise NotImplementedError("Subclass must implement clone method")

class Vision(SensoryInput):
    """
    Vision sensory input.
    
    Represents visual data from the agent's vision sensor.
    """
    
    def __init__(self, width: int = 0, height: int = 0, channels: int = 3):
        super().__init__()
        self._width = width
        self._height = height
        self._channels = channels
        self._data = []
    
    def setData(self, data: Union[List[float], 'Vision']) -> None:
        """
        Set image data.
        
        Args:
            data: Image data as list of floats or another Vision object
        """
        if isinstance(data, Vision):
            self._data = data.getData()
            self._width = data.getWidth()
            self._height = data.getHeight()
            self._channels = data.getChannels()
        else:
            self._data = data
    
    def setData(self, width: int, height: int, data: List[float]) -> None:
        """
        Set image data with dimensions.
        
        Args:
            width: Image width
            height: Image height
            data: Image data as list of floats
        """
        self._width = width
        self._height = height
        self._data = data
    
    def getWidth(self) -> int:
        """Get image width."""
        return self._width
    
    def getHeight(self) -> int:
        """Get image height."""
        return self._height
    
    def getChannels(self) -> int:
        """Get number of channels."""
        return self._channels
    
    def getType(self) -> str:
        return "Vision"
    
    def getData(self) -> List[float]:
        return self._data
    
    def getDimensions(self) -> int:
        return self._width * self._height * self._channels
    
    def clone(self):
        """Clone vision data."""
        new_vision = Vision(self._width, self._height, self._channels)
        new_vision.setData(self._data)
        new_vision.setTimestamp(self.getTimestamp())
        return new_vision

class Audio(SensoryInput):
    """
    Audio sensory input.
    
    Represents audio data from the agent's audio sensor.
    """
    
    def __init__(self, sample_rate: int = 0, num_samples: int = 0):
        super().__init__()
        self._sample_rate = sample_rate
        self._num_samples = num_samples
        self._data = []
    
    def setData(self, data: Union[List[float], 'Audio']) -> None:
        """
        Set audio data.
        
        Args:
            data: Audio data as list of floats or another Audio object
        """
        if isinstance(data, Audio):
            self._data = data.getData()
            self._sample_rate = data.getSampleRate()
            self._num_samples = data.getNumSamples()
        else:
            self._data = data
            self._num_samples = len(data)
    
    def setSampleRate(self, rate: int) -> None:
        """
        Set sample rate.
        
        Args:
            rate: Sample rate in Hz
        """
        self._sample_rate = rate
    
    def getType(self) -> str:
        return "Audio"
    
    def getData(self) -> List[float]:
        return self._data
    
    def getDimensions(self) -> int:
        return len(self._data)
    
    def getSampleRate(self) -> int:
        return self._sample_rate
    
    def getNumSamples(self) -> int:
        return self._num_samples
    
    def clone(self):
        """Clone audio data."""
        new_audio = Audio(self._sample_rate, self._num_samples)
        new_audio.setData(self._data)
        new_audio.setTimestamp(self.getTimestamp())
        return new_audio

class InternalSignals(SensoryInput):
    """
    Internal signals sensory input.
    
    Represents internal brain state signals (e.g., dopamine, curiosity,
    prediction error, etc.).
    """
    
    def __init__(self):
        super().__init__()
        self._signals = []
    
    def addSignal(self, value: float) -> None:
        """
        Add a signal value.
        
        Args:
            value: Signal value to add
        """
        self._signals.append(value)
    
    def clearSignals(self) -> None:
        """Clear all signals."""
        self._signals.clear()
    
    def getType(self) -> str:
        return "InternalSignals"
    
    def getData(self) -> List[float]:
        return self._signals
    
    def getDimensions(self) -> int:
        return len(self._signals)
    
    def clone(self):
        """Clone internal signals."""
        new_signals = InternalSignals()
        new_signals._signals = self._signals.copy()
        new_signals.setTimestamp(self.getTimestamp())
        return new_signals

# Factory functions for backward compatibility
def createVision(width: int = 0, height: int = 0, channels: int = 3) -> Vision:
    """Create a vision sensory input.
    
    Args:
        width: Image width
        height: Image height
        channels: Number of channels
        
    Returns:
        Vision: New vision object
    """
    return Vision(width, height, channels)

def createAudio(sample_rate: int = 0, num_samples: int = 0) -> Audio:
    """Create an audio sensory input.
    
    Args:
        sample_rate: Sample rate in Hz
        num_samples: Number of samples
        
    Returns:
        Audio: New audio object
    """
    return Audio(sample_rate, num_samples)

def createInternalSignals() -> InternalSignals:
    """Create internal signals sensory input.
    
    Returns:
        InternalSignals: New internal signals object
    """
    return InternalSignals()
