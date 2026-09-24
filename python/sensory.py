"""
Sensory processing systems for NLM - vision, audio, internal signals.

This module provides the sensory processing systems that interface the
NLM artificial brain with the environment.
"""

import sys
from typing import List, Dict, Any

__all__ = [
    'Vision',
    'Audio',
    'InternalSignals',
    'SensoryInput',
]


class SensoryInput:
    """Base class for sensory input from environment.
    
    This abstract base class provides the interface for all sensory
    modalities. Specific implementations include vision, audio,
    and internal signals.
    
    Attributes:
        data: Raw sensory data (list of floats).
        timestamp: Time when data was captured.
        dimensions: Dimensionality of sensory data.
    """
    
    def __init__(self, data: List[float], timestamp: float = 0.0):
        self._sensory_input = data
        self._timestamp = timestamp
        self._dimensions = len(data) if data else 0
    
    def getType(self) -> str:
        """Get sensory modality type.
        
        Returns:
            str: Sensory modality type (e.g., "Vision", "Audio", "Internal").
        """
        raise NotImplementedError("Subclasses must implement getType()")
    
    def getData(self) -> List[float]:
        """Get raw sensory data.
        
        Returns:
            List[float]: Raw sensory data.
        """
        return self._sensory_input
    
    def setData(self, data: List[float]) -> bool:
        """Set sensory data.
        
        Args:
            data: New sensory data.
            
        Returns:
            bool: True if data set successfully.
        """
        self._sensory_input = data
        self._dimensions = len(data) if data else 0
        return True
    
    def getDimensions(self) -> int:
        """Get sensory data dimensionality.
        
        Returns:
            int: Number of dimensions in sensory data.
        """
        return self._dimensions
    
    def getTimestamp(self) -> float:
        """Get sensory data timestamp.
        
        Returns:
            float: Timestamp when data was captured.
        """
        return self._timestamp
    
    def setTimestamp(self, timestamp: float) -> bool:
        """Set sensory data timestamp.
        
        Args:
            timestamp: New timestamp.
            
        Returns:
            bool: True if timestamp set successfully.
        """
        self._timestamp = timestamp
        return True
    
    def __repr__(self) -> str:
        return f"<{self.getType()}: dimensions={self._dimensions}, timestamp={self._timestamp}>"


class Vision(SensoryInput):
    """Visual sensory input for image processing.
    
    Vision captures visual information from the environment as a 2D array
    of pixel values. It provides vision-specific functionality for
    object detection, feature extraction, and visual scene analysis.
    
    Attributes:
        width: Image width in pixels.
        height: Image height in pixels.
        channels: Number of color channels (typically 3 for RGB).
    """
    
    def __init__(self, width: int = 0, height: int = 0, channels: int = 3):
        super().__init__([])
        self._width = width
        self._height = height
        self._channels = channels
    
    def getType(self) -> str:
        return "Vision"
    
    def getWidth(self) -> int:
        return self._width
    
    def getHeight(self) -> int:
        return self._height
    
    def getChannels(self) -> int:
        return self._channels
    
    def setImage(self, width: int, height: int, data: List[float]) -> bool:
        """Set vision image data.
        
        Args:
            width: Image width.
            height: Image height.
            data: Image data (flattened array).
            
        Returns:
            bool: True if image set successfully.
        """
        self._width = width
        self._height = height
        self._channels = 3 if len(data) == width * height * 3 else 1
        return self.setData(data)
    
    def getPixel(self, x: int, y: int) -> List[float]:
        """Get pixel value at given coordinates.
        
        Args:
            x: X coordinate.
            y: Y coordinate.
            
        Returns:
            List[float]: Pixel values (RGB or grayscale).
        """
        if self._width <= 0 or self._height <= 0:
            return []
        
        index = ((y * self._width) + x) * self._channels
        if index + self._channels > len(self._sensory_input):
            return []
        
        return self._sensory_input[index:index + self._channels]
    
    def __repr__(self) -> str:
        return f"<Vision: {self._width}x{self._height}x{self._channels}>"


class Audio(SensoryInput):
    """Auditory sensory input for sound processing.
    
    Audio captures sound information as a time series of samples. It
    provides audio-specific functionality for frequency analysis,
    sound localization, and speech recognition.
    
    Attributes:
        sample_rate: Audio sample rate (Hz).
        num_samples: Number of audio samples.
    """
    
    def __init__(self, sample_rate: int = 0, num_samples: int = 0):
        super().__init__([])
        self._sample_rate = sample_rate
        self._num_samples = num_samples
    
    def getType(self) -> str:
        return "Audio"
    
    def getSampleRate(self) -> int:
        return self._sample_rate
    
    def getNumSamples(self) -> int:
        return self._num_samples
    
    def setAudio(self, sample_rate: int, num_samples: int, data: List[float]) -> bool:
        """Set audio data.
        
        Args:
            sample_rate: Audio sample rate.
            num_samples: Number of samples.
            data: Audio samples.
            
        Returns:
            bool: True if audio set successfully.
        """
        self._sample_rate = sample_rate
        self._num_samples = num_samples
        return self.setData(data)
    
    def getSample(self, index: int) -> float:
        """Get audio sample at given index.
        
        Args:
            index: Sample index.
            
        Returns:
            float: Audio sample value.
        """
        if index < 0 or index >= len(self._sensory_input):
            return 0.0
        return self._sensory_input[index]
    
    def __repr__(self) -> str:
        return f"<Audio: {self._sample_rate} Hz, {self._num_samples} samples>"


class InternalSignals(SensoryInput):
    """Internal state sensory input for self-monitoring.
    
    Internal signals capture the brain's own internal state, including
    neural activity, memory traces, and attention levels. It provides
    the brain with feedback about its own processing.
    
    Attributes:
        signal_types: List of signal types (e.g., "activity", "attention").
    """
    
    def __init__(self):
        super().__init__([])
        self._signal_types = []
    
    def getType(self) -> str:
        return "InternalSignals"
    
    def addSignal(self, value: float) -> bool:
        """Add internal signal value.
        
        Args:
            value: Signal value to add.
            
        Returns:
            bool: True if signal added successfully.
        """
        self._sensory_input.append(value)
        return True
    
    def clearSignals(self) -> bool:
        """Clear all internal signals.
        
        Returns:
            bool: True if cleared successfully.
        """
        self._sensory_input.clear()
        return True
    
    def getSignalTypes(self) -> List[str]:
        """Get list of signal types.
        
        Returns:
            List[str]: List of signal types.
        """
        return self._signal_types
    
    def setSignalTypes(self, types: List[str]) -> bool:
        """Set signal types.
        
        Args:
            types: List of signal types.
            
        Returns:
            bool: True if types set successfully.
        """
        self._signal_types = types
        return True
    
    def __repr__(self) -> str:
        return f"<InternalSignals: {len(self._sensory_input)} signals, {len(self._signal_types)} types>"
