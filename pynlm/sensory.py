"""
NLM Sensory classes
"""

from typing import List, Optional, Any

class SensoryInputError(Exception):
    """Raised when sensory input operations fail."""
    pass
class SensoryInput:
    """Base class for sensory input."""
    
    def __init__(self):
        self._timestamp = 0.0
    
    def get_type(self) -> str:
        """Get the type of sensory input.
        
        Returns:
            Type of sensory input
        """
        return "Base"
    
    def get_data(self) -> List[float]:
        """Get the raw data as a vector.
        
        Returns:
            Raw data vector
        """
        return []
    
    def get_dimensions(self) -> int:
        """Get the dimensionality.
        
        Returns:
            Number of dimensions
        """
        return 0
    
    def get_timestamp(self) -> float:
        """Get the timestamp.
        
        Returns:
            Timestamp
        """
        return self._timestamp
    
    def set_timestamp(self, timestamp: float) -> None:
        """Set the timestamp.
        
        Args:
            timestamp: Timestamp
        """
        self._timestamp = timestamp
class Vision(SensoryInput):
    """Vision sensory input."""
    
    def __init__(self, width: int = 0, height: int = 0, channels: int = 3):
        """Create vision sensory input.
        
        Args:
            width: Image width
            height: Image height
            channels: Number of channels (default: 3 for RGB)
        """
        super().__init__()
        self._width = width
        self._height = height
        self._channels = channels
        self._data = []
        
        # Initialize with zeros
        for _ in range(width * height * channels):
            self._data.append(0.0)
    
    def set_data(self, data: List[float]) -> None:
        """Set vision data.
        
        Args:
            data: Vision data (list of floats, should match width*height*channels)
        """
        self._data = data
    
    def get_width(self) -> int:
        """Get vision width.
        
        Returns:
            Vision width
        """
        return self._width
    
    def get_height(self) -> int:
        """Get vision height.
        
        Returns:
            Vision height
        """
        return self._height
    
    def get_channels(self) -> int:
        """Get number of channels.
        
        Returns:
            Number of channels
        """
        return self._channels
    
    def get_type(self) -> str:
        """Get the type of sensory input.
        
        Returns:
            "Vision"
        """
        return "Vision"
    
    def get_data(self) -> List[float]:
        """Get the raw data as a vector.
        
        Returns:
            Raw data vector
        """
        return self._data
class Audio(SensoryInput):
    """Audio sensory input."""
    
    def __init__(self, sample_rate: int = 0, num_samples: int = 0):
        """Create audio sensory input.
        
        Args:
            sample_rate: Sample rate (Hz)
            num_samples: Number of audio samples
        """
        super().__init__()
        self._sample_rate = sample_rate
        self._num_samples = num_samples
        self._data = []
        
        # Initialize with zeros
        for _ in range(num_samples):
            self._data.append(0.0)
    
    def set_data(self, data: List[float]) -> None:
        """Set audio data.
        
        Args:
            data: Audio data (list of floats)
        """
        self._data = data
        self._num_samples = len(data)
    
    def set_sample_rate(self, sample_rate: int) -> None:
        """Set sample rate.
        
        Args:
            sample_rate: Sample rate (Hz)
        """
        self._sample_rate = sample_rate
    
    def get_sample_rate(self) -> int:
        """Get sample rate.
        
        Returns:
            Sample rate (Hz)
        """
        return self._sample_rate
    
    def get_num_samples(self) -> int:
        """Get number of samples.
        
        Returns:
            Number of audio samples
        """
        return self._num_samples
    
    def get_type(self) -> str:
        """Get the type of sensory input.
        
        Returns:
            "Audio"
        """
        return "Audio"
    
    def get_data(self) -> List[float]:
        """Get the raw data as a vector.
        
        Returns:
            Raw data vector
        """
        return self._data
class InternalSignals(SensoryInput):
    """Internal signals sensory input."""
    
    def __init__(self):
        super().__init__()
        self._signals = []
    
    def add_signal(self, value: float) -> None:
        """Add an internal signal.
        
        Args:
            value: Signal value
        """
        self._signals.append(value)
    
    def clear_signals(self) -> None:
        """Clear all signals."""
        self._signals.clear()
    
    def get_type(self) -> str:
        """Get the type of sensory input.
        
        Returns:
            "InternalSignals"
        """
        return "InternalSignals"
    
    def get_data(self) -> List[float]:
        """Get the raw data as a vector.
        
        Returns:
            Raw data vector
        """
        return self._signals
class SensoryPercept:
    """Sensory percept data.
    
    Represents the processed sensory information that the agent
    receives from the world and passes to the brain.
    """
    
    def __init__(self):
        """Create a sensory percept."""
        self._vision = []
        self._touch = []
        self._internal = []
        self._proprioception = []
        self._audio = []
        self._timestamp = 0.0
    
    def get_vision(self) -> List[float]:
        """Get vision data.
        
        Returns:
            Vision data
        """
        return self._vision
    
    def set_vision(self, vision: List[float]) -> None:
        """Set vision data.
        
        Args:
            vision: Vision data
        """
        self._vision = vision
    
    def get_vision_width(self) -> int:
        """Get vision width.
        
        Returns:
            Vision width
        """
        return len(self._vision) if self._vision else 0
    
    def get_vision_height(self) -> int:
        """Get vision height.
        
        Returns:
            Vision height
        """
        # Assuming square vision for now
        return len(self._vision) if self._vision else 0
    
    def get_touch(self) -> List[float]:
        """Get touch data.
        
        Returns:
            Touch data
        """
        return self._touch
    
    def set_touch(self, touch: List[float]) -> None:
        """Set touch data.
        
        Args:
            touch: Touch data
        """
        self._touch = touch
    
    def get_internal(self) -> List[float]:
        """Get internal signals.
        
        Returns:
            Internal signals
        """
        return self._internal
    
    def set_internal(self, internal: List[float]) -> None:
        """Set internal signals.
        
        Args:
            internal: Internal signals
        """
        self._internal = internal
    
    def get_proprioception(self) -> List[float]:
        """Get proprioception data.
        
        Returns:
            Proprioception data
        """
        return self._proprioception
    
    def set_proprioception(self, proprioception: List[float]) -> None:
        """Set proprioception data.
        
        Args:
            proprioception: Proprioception data
        """
        self._proprioception = proprioception
    
    def get_audio(self) -> List[float]:
        """Get audio data.
        
        Returns:
            Audio data
        """
        return self._audio
    
    def set_audio(self, audio: List[float]) -> None:
        """Set audio data.
        
        Args:
            audio: Audio data
        """
        self._audio = audio
    
    def get_all_signals(self) -> List[float]:
        """Get all sensory signals combined.
        
        Returns:
            Combined sensory signals
        """
        all_signals = []
        all_signals.extend(self._vision)
        all_signals.extend(self._touch)
        all_signals.extend(self._internal)
        all_signals.extend(self._proprioception)
        all_signals.extend(self._audio)
        return all_signals
    
    def get_timestamp(self) -> float:
        """Get the timestamp.
        
        Returns:
            Timestamp
        """
        return self._timestamp
    
    def set_timestamp(self, timestamp: float) -> None:
        """Set the timestamp.
        
        Args:
            timestamp: Timestamp
        """
        self._timestamp = timestamp