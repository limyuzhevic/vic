"""
NLM SensoryInput classes.

This module provides sensory input classes for the NLM system.
The sensory system captures information from the environment,
including vision, audio, touch, proprioception, and internal signals.
"""

import numpy as np
from typing import List, Any, Optional, Union, Dict, Tuple

# Import C++ bindings
from .bindings import (
    SensoryInput as _SensoryInput,
    Vision as _Vision,
    Audio as _Audio,
    InternalSignals as _InternalSignals,
    SensoryPercept as _SensoryPercept,
    Vision as VisionClass,
    Audio as AudioClass,
    ActionType,
)
class EnhancedSensoryPercept:
    """Enhanced SensoryPercept class with better error handling.
    
    This class provides a Pythonic interface to sensory percepts,
    making it easier to work with sensory inputs through properties
    and methods that follow Python conventions.
    
    The enhanced sensory percept provides:
    - Better error handling with custom exceptions
    - Convenience methods for accessing sensory data
    - Type-safe data access
    - Data manipulation utilities
    
    Example:
        >>> # Create percept from world
        >>> percept = world.getSensoryPercept()
        >>> 
        >>> # Access vision data
        >>> vision_data = percept.getVision()
        >>> print(f"Vision shape: {len(vision_data)}x{len(vision_data[0]) if vision_data else 0}")
        >>> 
        >>> # Access touch data
        >>> touch_data = percept.getTouch()
        >>> print(f"Touch sensors: {len(touch_data)} active")
        >>> 
        >>> # Access audio data
        >>> audio_data = percept.getAudio()
        >>> print(f"Audio samples: {len(audio_data)}")
    """
    
    def __init__(self, percept: Optional[_SensoryPercept] = None):
        """Initialize sensory percept.
        
        Args:
            percept: Existing SensoryPercept object to wrap. If None,
                    a new percept is created.
        """
        if percept is None:
            self._percept = _SensoryPercept()
        else:
            self._percept = percept
    
    def getVision(self) -> List[List[float]]:
        """Get vision data.
        
        Returns:
            Vision data as 2D list of floats.
            Returns empty list if no vision data.
        """
        try:
            return self._percept.getVision()
        except Exception as e:
            raise SensoryError(f"Failed to get vision data: {e}")
    
    def setVision(self, vision: List[List[float]]) -> None:
        """Set vision data.
        
        Args:
            vision: Vision data as 2D list of floats.
        """
        try:
            self._percept.setVision(vision)
        except Exception as e:
            raise SensoryError(f"Failed to set vision data: {e}")
    
    def getVisionWidth(self) -> int:
        """Get vision width.
        
        Returns:
            Vision width in pixels.
        """
        try:
            return self._percept.getVisionWidth()
        except Exception as e:
            raise SensoryError(f"Failed to get vision width: {e}")
    
    def getVisionHeight(self) -> int:
        """Get vision height.
        
        Returns:
            Vision height in pixels.
        """
        try:
            return self._percept.getVisionHeight()
        except Exception as e:
            raise SensoryError(f"Failed to get vision height: {e}")
    
    def getTouch(self) -> List[float]:
        """Get touch data.
        
        Returns:
            Touch data as list of floats.
            Returns empty list if no touch data.
        """
        try:
            return self._percept.getTouch()
        except Exception as e:
            raise SensoryError(f"Failed to get touch data: {e}")
    
    def setTouch(self, touch: List[float]) -> None:
        """Set touch data.
        
        Args:
            touch: Touch data as list of floats.
        """
        try:
            self._percept.setTouch(touch)
        except Exception as e:
            raise SensoryError(f"Failed to set touch data: {e}")
    
    def getInternal(self) -> List[float]:
        """Get internal signals data.
        
        Returns:
            Internal signals data as list of floats.
            Returns empty list if no internal data.
        """
        try:
            return self._percept.getInternal()
        except Exception as e:
            raise SensoryError(f"Failed to get internal data: {e}")
    
    def setInternal(self, internal: List[float]) -> None:
        """Set internal signals data.
        
        Args:
            internal: Internal signals data as list of floats.
        """
        try:
            self._percept.setInternal(internal)
        except Exception as e:
            raise SensoryError(f"Failed to set internal data: {e}")
    
    def getProprioception(self) -> List[float]:
        """Get proprioception data.
        
        Returns:
            Proprioception data as list of floats.
            Returns empty list if no proprioception data.
        """
        try:
            return self._percept.getProprioception()
        except Exception as e:
            raise SensoryError(f"Failed to get proprioception data: {e}")
    
    def setProprioception(self, proprioception: List[float]) -> None:
        """Set proprioception data.
        
        Args:
            proprioception: Proprioception data as list of floats.
        """
        try:
            self._percept.setProprioception(proprioception)
        except Exception as e:
            raise SensoryError(f"Failed to set proprioception data: {e}")
    
    def getAudio(self) -> List[float]:
        """Get audio data.
        
        Returns:
            Audio data as list of floats.
            Returns empty list if no audio data.
        """
        try:
            return self._percept.getAudio()
        except Exception as e:
            raise SensoryError(f"Failed to get audio data: {e}")
    
    def setAudio(self, audio: List[float]) -> None:
        """Set audio data.
        
        Args:
            audio: Audio data as list of floats.
        """
        try:
            self._percept.setAudio(audio)
        except Exception as e:
            raise SensoryError(f"Failed to set audio data: {e}")
    
    def getAllSignals(self) -> Dict[str, List[float]]:
        """Get all sensory signals.
        
        Returns:
            Dictionary mapping signal names to signal data.
        """
        try:
            # Get individual signals
            signals = {}
            
            vision = self.getVision()
            if vision:
                signals['vision'] = vision
            
            touch = self.getTouch()
            if touch:
                signals['touch'] = touch
            
            internal = self.getInternal()
            if internal:
                signals['internal'] = internal
            
            proprioception = self.getProprioception()
            if proprioception:
                signals['proprioception'] = proprioception
            
            audio = self.getAudio()
            if audio:
                signals['audio'] = audio
            
            return signals
        except Exception as e:
            raise SensoryError(f"Failed to get all signals: {e}")
    
    def getTimestamp(self) -> float:
        """Get timestamp.
        
        Returns:
            Timestamp of percept.
        """
        try:
            return self._percept.getTimestamp()
        except Exception as e:
            raise SensoryError(f"Failed to get timestamp: {e}")
    
    def setTimestamp(self, timestamp: float) -> None:
        """Set timestamp.
        
        Args:
            timestamp: Timestamp to set.
        """
        try:
            self._percept.setTimestamp(timestamp)
        except Exception as e:
            raise SensoryError(f"Failed to set timestamp: {e}")
    
    def clear(self) -> None:
        """Clear all sensory data."""
        try:
            # Clear each signal individually
            self.setVision([])
            self.setTouch([])
            self.setInternal([])
            self.setProprioception([])
            self.setAudio([])
            self.setTimestamp(0.0)
        except Exception as e:
            raise SensoryError(f"Failed to clear percept: {e}")
    
    def __repr__(self) -> str:
        return f"EnhancedSensoryPercept(signals={list(self.getAllSignals().keys())})"
    
    def __str__(self) -> str:
        signals = self.getAllSignals()
        return f"SensoryPercept({len(signals)} signal types: {list(signals.keys())})"
class SensoryPercept(EnhancedSensoryPercept):
    """Sensory percept class for NLM.
    
    This class represents sensory percept data from the environment,
    including vision, audio, touch, proprioception, and internal signals.
    
    Example:
        >>> # Create percept
        >>> percept = SensoryPercept()
        >>> 
        >>> # Set vision data
        >>> vision_data = [[0.1, 0.2, 0.3], [0.4, 0.5, 0.6]]
        >>> percept.setVision(vision_data)
        >>> 
        >>> # Get vision data
        >>> vision = percept.getVision()
        >>> print(f"Vision: {vision}")
    """
    
    def __init__(self):
        """Initialize sensory percept."""
        super().__init__()
class EnhancedVision:
    """Enhanced Vision class with better error handling.
    
    This class provides a Pythonic interface to vision sensory input,
    making it easier to work with visual data through properties
    and methods that follow Python conventions.
    
    The enhanced vision provides:
    - Better error handling with custom exceptions
    - Convenience methods for vision data manipulation
    - Type-safe data access
    - Data validation
    
    Example:
        >>> # Create vision input
        >>> vision = EnhancedVision(32, 32, 3)
        >>> 
        >>> # Set data from numpy array
        >>> import numpy as np
        >>> data = np.random.rand(32, 32, 3).astype(np.float32)
        >>> vision.setData(data)
        >>> 
        >>> # Access vision properties
        >>> print(f"Vision: {vision.width}x{vision.height}x{vision.channels}")
    """
    
    def __init__(self, width: int = 0, height: int = 0, channels: int = 3):
        """Initialize vision input.
        
        Args:
            width: Vision width in pixels.
            height: Vision height in pixels.
            channels: Number of color channels.
        """
        try:
            self._vision = _Vision(width, height, channels)
        except Exception as e:
            raise SensoryError(f"Failed to create vision input: {e}")
    
    def setData(self, data: List[float]) -> None:
        """Set vision data.
        
        Args:
            data: Vision data as flattened list of floats.
            
        Raises:
            ValueError: If data dimensions don't match vision dimensions.
        """
        try:
            expected_size = self.width * self.height * self.channels
            if len(data) != expected_size:
                raise ValueError(f"Data size ({len(data)}) doesn't match "
                               f"vision dimensions ({self.width}x{self.height}x{self.channels}) "
                               f"={ expected_size}")
            
            self._vision.setData(data)
        except Exception as e:
            raise SensoryError(f"Failed to set vision data: {e}")
    
    def getWidth(self) -> int:
        """Get vision width.
        
        Returns:
            Vision width in pixels.
        """
        try:
            return self._vision.getWidth()
        except Exception as e:
            raise SensoryError(f"Failed to get vision width: {e}")
    
    def getHeight(self) -> int:
        """Get vision height.
        
        Returns:
            Vision height in pixels.
        """
        try:
            return self._vision.getHeight()
        except Exception as e:
            raise SensoryError(f"Failed to get vision height: {e}")
    
    def getChannels(self) -> int:
        """Get number of channels.
        
        Returns:
            Number of color channels.
        """
        try:
            return self._vision.getChannels()
        except Exception as e:
            raise SensoryError(f"Failed to get vision channels: {e}")
    
    def getSize(self) -> int:
        """Get total data size.
        
        Returns:
            Total size of vision data.
        """
        return self.width * self.height * self.channels
    
    def from_numpy(self, data: np.ndarray, normalize: bool = True) -> None:
        """Create vision data from numpy array.
        
        Args:
            data: Numpy array with vision data.
            normalize: Whether to normalize data to [0, 1] range.
            
        Raises:
            ValueError: If numpy array dimensions don't match vision dimensions.
        """
        try:
            if normalize:
                data = np.clip(data, 0, 1).astype(np.float32)
            else:
                data = data.astype(np.float32)
            
            expected_shape = (self.height, self.width, self.channels)
            if data.shape != expected_shape:
                raise ValueError(f"Data shape {data.shape} doesn't match "
                               f"vision dimensions {expected_shape}")
            
            self.setData(data.flatten().tolist())
        except Exception as e:
            raise SensoryError(f"Failed to set vision data from numpy array: {e}")
    
    def to_numpy(self) -> np.ndarray:
        """Convert vision data to numpy array.
        
        Returns:
            Numpy array with vision data.
        """\n        try:
            data = self._vision.getData()
            return np.array(data, dtype=np.float32).reshape(
                self.height, self.width, self.channels
            )
        except Exception as e:
            raise SensoryError(f"Failed to convert vision data to numpy: {e}")
    
    # Pythonic properties
    
    @property
    def width(self) -> int:
        """Vision width."""
        return self.getWidth()
    
    @property
    def height(self) -> int:
        """Vision height."""
        return self.getHeight()
    
    @property
    def channels(self) -> int:
        """Number of channels."""
        return self.getChannels()
    
    @property
    def size(self) -> int:
        """Total data size."""
        return self.getSize()
    
    def __repr__(self) -> str:
        return f"EnhancedVision({self.width}x{self.height}x{self.channels})"
class Vision(EnhancedVision):
    """Vision class for NLM.
    
    This class provides vision sensory input for the NLM system,
    capturing visual information from the environment.
    
    Example:
        >>> # Create vision input
        >>> vision = Vision(32, 32, 3)
        >>> 
        >>> # Set data from numpy array
        >>> import numpy as np
        >>> data = np.random.rand(32, 32, 3).astype(np.float32)
        >>> vision.setData(data)
        >>> 
        >>> # Get data as numpy array
        >>> data_array = vision.to_numpy()
    """
    
    def __init__(self, width: int = 0, height: int = 0, channels: int = 3):
        """Initialize vision input."""
        super().__init__(width, height, channels)
class EnhancedAudio:
    """Enhanced Audio class with better error handling.
    
    This class provides a Pythonic interface to audio sensory input,
    making it easier to work with audio data through properties
    and methods that follow Python conventions.
    
    The enhanced audio provides:
    - Better error handling with custom exceptions
    - Convenience methods for audio data manipulation
    - Type-safe data access
    - Audio sample rate management
    
    Example:
        >>> # Create audio input
        >>> audio = EnhancedAudio(44100, 1024)
        >>> 
        >>> # Set data from numpy array
        >>> import numpy as np
        >>> data = np.random.randn(1024).astype(np.float32)
        >>> audio.setData(data)
        >>> 
        >>> # Access audio properties
        >>> print(f"Audio: {audio.sample_rate} Hz, {audio.num_samples} samples")
    """
    
    def __init__(self, sample_rate: int = 0, num_samples: int = 0):
        """Initialize audio input.
        
        Args:
            sample_rate: Audio sample rate in Hz.
            num_samples: Number of audio samples.
        """
        try:
            self._audio = _Audio(sample_rate, num_samples)
        except Exception as e:
            raise SensoryError(f"Failed to create audio input: {e}")
    
    def setData(self, data: List[float]) -> None:
        """Set audio data.
        
        Args:
            data: Audio data as list of floats.
            
        Raises:
            ValueError: If data size doesn't match audio sample count.
        """
        try:
            if self.num_samples > 0 and len(data) != self.num_samples:
                raise ValueError(f"Data size ({len(data)}) doesn't match "
                               f"audio sample count ({self.num_samples})")
            
            self._audio.setData(data)
        except Exception as e:
            raise SensoryError(f"Failed to set audio data: {e}")
    
    def setSampleRate(self, sample_rate: int) -> None:
        """Set sample rate.
        
        Args:
            sample_rate: Sample rate in Hz.
        """
        try:
            self._audio.setSampleRate(sample_rate)
        except Exception as e:
            raise SensoryError(f"Failed to set sample rate: {e}")
    
    def getSampleRate(self) -> int:
        """Get sample rate.
        
        Returns:
            Sample rate in Hz.
        """
        try:
            return self._audio.getSampleRate()
        except Exception as e:
            raise SensoryError(f"Failed to get sample rate: {e}")
    
    def getNumSamples(self) -> int:
        """Get number of samples.
        
        Returns:
            Number of audio samples.
        """
        try:
            return self._audio.getNumSamples()
        except Exception as e:
            raise SensoryError(f"Failed to get num samples: {e}")
    
    # Pythonic properties
    
    @property
    def sample_rate(self) -> int:
        """Sample rate."""
        return self.getSampleRate()
    
    @property
    def num_samples(self) -> int:
        """Number of samples."""
        return self.getNumSamples()
    
    @property
    def duration(self) -> float:
        """Audio duration in seconds."""
        if self.sample_rate == 0:
            return 0.0
        return self.num_samples / self.sample_rate
    
    def from_numpy(self, data: np.ndarray, sample_rate: Optional[int] = None) -> None:
        """Create audio data from numpy array.
        
        Args:
            data: Numpy array with audio data.
            sample_rate: Sample rate in Hz. If None, uses current sample rate.
            
        Raises:
            ValueError: If numpy array doesn't match sample count.
        """
        try:
            if sample_rate is not None:
                self.setSampleRate(sample_rate)
            
            expected_samples = self.num_samples if self.num_samples > 0 else len(data)
            if len(data) != expected_samples:
                raise ValueError(f"Data size ({len(data)}) doesn't match "
                               f"expected samples ({expected_samples})")
            
            self.setData(data.flatten().tolist())
        except Exception as e:
            raise SensoryError(f"Failed to set audio data from numpy array: {e}")
    
    def to_numpy(self) -> np.ndarray:
        """Convert audio data to numpy array.
        
        Returns:
            Numpy array with audio data.
        """
        try:
            data = self._audio.getData()
            return np.array(data, dtype=np.float32)
        except Exception as e:
            raise SensoryError(f"Failed to convert audio data to numpy: {e}")
    
    def __repr__(self) -> str:
        return f"EnhancedAudio({self.sample_rate} Hz, {self.num_samples} samples)"
class Audio(EnhancedAudio):
    """Audio class for NLM.
    
    This class provides audio sensory input for the NLM system,
    capturing sound information from the environment.
    
    Example:
        >>> # Create audio input
        >>> audio = Audio(44100, 1024)
        >>> 
        >>> # Set data from numpy array
        >>> import numpy as np
        >>> data = np.random.randn(1024).astype(np.float32)
        >>> audio.setData(data)
        >>> 
        >>> # Get data as numpy array
        >>> data_array = audio.to_numpy()
    """
    
    def __init__(self, sample_rate: int = 0, num_samples: int = 0):
        """Initialize audio input."""
        super().__init__(sample_rate, num_samples)
class EnhancedInternalSignals:
    """Enhanced InternalSignals class with better error handling.
    
    This class provides a Pythonic interface to internal signals sensory input,
    making it easier to work with internal neural data through properties
    and methods that follow Python conventions.
    
    The enhanced internal signals provide:
    - Better error handling with custom exceptions
    - Convenience methods for signal management
    - Type-safe signal access
    - Signal validation
    
    Example:
        >>> # Create internal signals
        >>> internal = EnhancedInternalSignals()
        >>> 
        >>> # Add signals
        >>> internal.addSignal(0.5)
        >>> internal.addSignal(-0.2)
        >>> internal.addSignal(1.0)
        >>> 
        >>> # Get signals
        >>> signals = internal.getSignals()
        >>> print(f"Signals: {signals}")
    """
    
    def __init__(self):
        """Initialize internal signals."""
        try:
            self._signals = _InternalSignals()
        except Exception as e:
            raise SensoryError(f"Failed to create internal signals: {e}")
    
    def addSignal(self, value: float) -> None:
        """Add signal value.
        
        Args:
            value: Signal value to add.
        """
        try:
            self._signals.addSignal(value)
        except Exception as e:
            raise SensoryError(f"Failed to add signal: {e}")
    
    def getSignals(self) -> List[float]:
        """Get all signals.
        
        Returns:
            List of signal values.
        """
        try:
            return self._signals.getSignals()
        except Exception as e:
            raise SensoryError(f"Failed to get signals: {e}")
    
    def clearSignals(self) -> None:
        """Clear all signals."""
        try:
            self._signals.clearSignals()
        except Exception as e:
            raise SensoryError(f"Failed to clear signals: {e}")
    
    def getSignalCount(self) -> int:
        """Get number of signals.
        
        Returns:
            Number of signals.
        """
        try:
            return len(self.getSignals())
        except Exception as e:
            raise SensoryError(f"Failed to get signal count: {e}")
    
    @property
    def signal_count(self) -> int:
        """Number of signals."""
        return self.getSignalCount()
    
    def __repr__(self) -> str:
        return f"EnhancedInternalSignals({self.signal_count} signals)"
class InternalSignals(EnhancedInternalSignals):
    """InternalSignals class for NLM.
    
    This class provides internal signals sensory input for the NLM system,
    capturing internal neural signals such as membrane potentials,
    synaptic currents, etc.
    
    Example:
        >>> # Create internal signals
        >>> internal = InternalSignals()
        >>> 
        >>> # Add signals
        >>> internal.addSignal(0.5)
        >>> internal.addSignal(-0.2)
        >>> 
        >>> # Get signals
        >>> signals = internal.getSignals()
        >>> print(f"Signals: {signals}")
    """
    
    def __init__(self):
        """Initialize internal signals."""
        super().__init__()


# Custom exception for sensory errors
class SensoryError(NLMError):
    """Error in sensory processing."""
    pass


# Type aliases for better documentation
VisionShape = Tuple[int, int, int]
AudioShape = Tuple[int, int]

__all__ = [
    # Main classes
    "SensoryPercept", "EnhancedSensoryPercept",
    "Vision", "EnhancedVision",
    "Audio", "EnhancedAudio",
    "InternalSignals", "EnhancedInternalSignals",
    
    # Exceptions
    "SensoryError",
    
    # Type aliases
    "VisionShape", "AudioShape",
    
    # Utility functions
    "from_numpy_to_vision",
    "from_numpy_to_audio",
]


def from_numpy_to_vision(data: np.ndarray, width: int, height: int, channels: int = 3, normalize: bool = True) -> Vision:
    """Create vision from numpy array.
    
    Args:
        data: Numpy array with vision data.
        width: Vision width in pixels.
        height: Vision height in pixels.
        channels: Number of color channels.
        normalize: Whether to normalize data to [0, 1] range.
        
    Returns:
        Vision object with data.
        
    Raises:
        SensoryError: If vision creation fails.
    """
    vision = Vision(width, height, channels)
    vision.from_numpy(data, normalize)
    return vision


def from_numpy_to_audio(data: np.ndarray, sample_rate: int) -> Audio:
    """Create audio from numpy array.
    
    Args:
        data: Numpy array with audio data.
        sample_rate: Sample rate in Hz.
        
    Returns:
        Audio object with data.
        
    Raises:
        SensoryError: If audio creation fails.
    """
    audio = Audio(sample_rate=len(data))
    audio.setSampleRate(sample_rate)
    audio.setData(data.flatten().tolist())
    return audio
