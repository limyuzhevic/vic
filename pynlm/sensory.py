# Sensory system implementation for NLM Python API

"""
Sensory processing system for NLM Python API

Provides Pythonic interfaces to sensory input systems:
Vision, Audio, InternalSignals, and base SensoryInput classes.
"""

from typing import List, Any, Optional, Union, Dict
from pathlib import Path

# Import the C++ binding and type definitions
from pynlm.types import SensoryInput as CppSensoryInput
from pynlm.types import Timestamp, Nanoseconds, Microvolts

class NLMError(Exception):
    """Base exception for NLM Python API errors"""
    pass

class SensoryError(NLMError):
    """Raised when sensory operations fail"""
    pass

class SensoryInput:
    """
    Base class for sensory input
    
    Abstract base class for all sensory input types.
    Provides common functionality for sensory data handling.
    
    Examples:
        >>> vision = Vision(width=8, height=8)
        >>> audio = Audio(sample_rate=44100, num_samples=1024)
        >>> internal = InternalSignals()
    """
    
    def __init__(self):
        """
        Initialize sensory input.
        """
        # Note: In actual implementation, this would wrap C++ sensory input object
        self._initialized = False
    
    def get_type(self) -> str:
        """
        Get the type of sensory input.
        
        Returns:
            Sensory input type as string
            
        Raises:
            SensoryError: If type cannot be determined
        """
        # Note: In actual implementation, this would get from C++
        return "SensoryInput"
    
    def get_data(self) -> List[float]:
        """
        Get the raw data as a vector.
        
        Returns:
            Raw sensory data as list of floats
            
        Raises:
            SensoryError: If data cannot be retrieved
        """
        # Note: In actual implementation, this would get from C++
        return []
    
    def get_dimensions(self) -> int:
        """
        Get the dimensionality of sensory data.
        
        Returns:
            Number of dimensions in sensory data
            
        Raises:
            SensoryError: If dimensions cannot be determined
        """
        # Note: In actual implementation, this would get from C++
        return 0
    
    def get_timestamp(self) -> Timestamp:
        """
        Get the timestamp.
        
        Returns:
            Timestamp of sensory data
            
        Raises:
            SensoryError: If timestamp cannot be retrieved
        """
        # Note: In actual implementation, this would get from C++
        return 0.0
    
    def set_timestamp(self, timestamp: Timestamp) -> None:
        """
        Set the timestamp.
        
        Args:
            timestamp: Timestamp value
            
        Raises:
            SensoryError: If timestamp cannot be set
        """
        # Note: In actual implementation, this would set in C++
        pass
    
    def __repr__(self) -> str:
        """String representation of sensory input"""
        return f"<{self.get_type()}>"
    
    def __str__(self) -> str:
        """String representation of sensory input"""
        return self.__repr__()

class Vision(SensoryInput):
    """
    Vision sensory input
    
    Represents visual sensory input as 2D image data with color channels.
    Used for vision processing in NLM agents.
    
    Examples:
        >>> vision = Vision(width=8, height=8, channels=3)
        >>> vision.setData([0.0, 1.0, 0.0] * 64)  # Green data
        >>> width = vision.getWidth()
        >>> height = vision.getHeight()
        >>> channels = vision.getChannels()
    """
    
    def __init__(self, width: int = 0, height: int = 0, channels: int = 3):
        """
        Initialize vision sensory input.
        
        Args:
            width: Image width in pixels
            height: Image height in pixels  
            channels: Number of color channels (typically 3 for RGB)
            
        Raises:
            SensoryError: If vision cannot be created
        """
        super().__init__()
        
        if width <= 0 or height <= 0:
            raise SensoryError(f"Invalid vision dimensions: {width}x{height}")
        if channels <= 0:
            raise SensoryError(f"Invalid number of channels: {channels}")
        
        # Note: In actual implementation, this would create C++ Vision object
        self._width = width
        self._height = height
        self._channels = channels
        self._initialized = True
    
    def set_data(self, data: List[float]) -> None:
        """
        Set vision data.
        
        Args:
            data: Vision data as list of floats (flattened RGB or grayscale)
            
        Raises:
            SensoryError: If data cannot be set
            
        Examples:
            >>> vision = Vision(width=2, height=2, channels=3)
            >>> vision.setData([1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0])
            >>> # Red, green, blue at (0,0), green at (1,0), blue at (2,0)
        """
        if not self._initialized:
            raise SensoryError("Vision not initialized")
        
        expected_size = self._width * self._height * self._channels
        if len(data) != expected_size:
            raise SensoryError(f"Vision data size mismatch: expected {expected_size}, got {len(data)}")
        
        # Note: In actual implementation, this would set in C++
        self._data = data
    
    def set_data_2d(self, data: List[List[List[float]]]) -> None:
        """
        Set vision data as 2D array of color channels.
        
        Args:
            data: 2D array where each element is [R, G, B] or [grayscale]
            
        Raises:
            SensoryError: If data cannot be set
            
        Examples:
            >>> vision = Vision(width=2, height=2, channels=3)
            >>> data = [
            ...     [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0]],
            ...     [[0.0, 0.0, 1.0], [0.5, 0.5, 0.5]]
            ... ]
            >>> vision.setData2D(data)
        """
        if not self._initialized:
            raise SensoryError("Vision not initialized")
        
        if len(data) != self._height:
            raise SensoryError(f"Vision height mismatch: expected {self._height}, got {len(data)}")
        
        for y, row in enumerate(data):
            if len(row) != self._width:
                raise SensoryError(f"Vision width mismatch at row {y}: expected {self._width}, got {len(row)}")
            for x, pixel in enumerate(row):
                if len(pixel) != self._channels:
                    raise SensoryError(f"Vision channels mismatch at ({x}, {y}): expected {self._channels}, got {len(pixel)}")
        
        # Flatten data for internal storage
        flat_data = []
        for y in range(self._height):
            for x in range(self._width):
                flat_data.extend(data[y][x])
        
        self.set_data(flat_data)
    
    def get_width(self) -> int:
        """
        Get vision width.
        
        Returns:
            Image width in pixels
            
        Raises:
            SensoryError: If width cannot be retrieved
            
        Examples:
            >>> vision = Vision(width=640, height=480)
            >>> width = vision.getWidth()
            >>> print(f"Vision width: {width} pixels")
        """
        return self._width
    
    def get_height(self) -> int:
        """
        Get vision height.
        
        Returns:
            Image height in pixels
            
        Raises:
            SensoryError: If height cannot be retrieved
            
        Examples:
            >>> vision = Vision(width=640, height=480)
            >>> height = vision.getHeight()
            >>> print(f"Vision height: {height} pixels")
        """
        return self._height
    
    def get_channels(self) -> int:
        """
        Get number of color channels.
        
        Returns:
            Number of color channels
            
        Raises:
            SensoryError: If channels cannot be retrieved
            
        Examples:
            >>> vision = Vision(width=640, height=480, channels=3)
            >>> channels = vision.getChannels()
            >>> print(f"Vision channels: {channels}")
        """
        return self._channels
    
    def get_grayscale(self) -> List[float]:
        """
        Get vision data as grayscale (single channel).
        
        Returns:
            Grayscale vision data
            
        Raises:
            SensoryError: If grayscale conversion fails
            
        Examples:
            >>> vision = Vision(width=2, height=2, channels=3)
            >>> vision.setData([1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0])
            >>> gray = vision.getGrayscale()
            >>> # gray = [0.299, 0.587, 0.114] (approximate)
        """
        if self._channels == 1:
            return self._data
        elif self._channels == 3:
            # Convert RGB to grayscale using luminance formula
            gray_data = []
            for i in range(0, len(self._data), self._channels):
                r = self._data[i]
                g = self._data[i + 1] if i + 1 < len(self._data) else 0.0
                b = self._data[i + 2] if i + 2 < len(self._data) else 0.0
                gray = 0.299 * r + 0.587 * g + 0.114 * b
                gray_data.append(gray)
            return gray_data
        else:
            raise SensoryError(f"Unsupported number of channels: {self._channels}")
    
    def normalize(self) -> None:
        """
        Normalize vision data to [0, 1] range.
        
        Raises:
            SensoryError: If normalization fails
            
        Examples:
            >>> vision = Vision(width=2, height=2, channels=3)
            >>> vision.setData([255, 128, 0, 0, 255, 128, 0, 0, 255])
            >>> vision.normalize()  # Now values are in [0, 1]
        """
        if not hasattr(self, '_data'):
            return
        
        max_val = max(self._data) if self._data else 1.0
        if max_val > 1.0:
            self._data = [x / max_val for x in self._data]
    
    def __repr__(self) -> str:
        """String representation of vision"""
        return f"<Vision {self._width}x{self._height}x{self._channels}>"
    
    def __str__(self) -> str:
        """String representation of vision"""
        return self.__repr__()

class Audio(SensoryInput):
    """
    Audio sensory input
    
    Represents audio sensory input as sampled sound data.
    Used for auditory processing in NLM agents.
    
    Examples:
        >>> audio = Audio(sample_rate=44100, num_samples=1024)
        >>> audio.setData([0.0, 0.5, 1.0, 0.5, 0.0])
        >>> sample_rate = audio.getSampleRate()
        >>> samples = audio.getNumSamples()
    """
    
    def __init__(self, sample_rate: int = 44100, num_samples: int = 0):
        """
        Initialize audio sensory input.
        
        Args:
            sample_rate: Audio sample rate in Hz
            num_samples: Number of audio samples
            
        Raises:
            SensoryError: If audio cannot be created
        """
        super().__init__()
        
        if sample_rate <= 0:
            raise SensoryError(f"Invalid sample rate: {sample_rate}")
        
        # Note: In actual implementation, this would create C++ Audio object
        self._sample_rate = sample_rate
        self._num_samples = num_samples
        self._initialized = True
    
    def set_data(self, data: List[float]) -> None:
        """
        Set audio data.
        
        Args:
            data: Audio samples as list of floats (-1.0 to 1.0)
            
        Raises:
            SensoryError: If data cannot be set
        """
        if not self._initialized:
            raise SensoryError("Audio not initialized")
        
        if len(data) != self._num_samples and self._num_samples > 0:
            raise SensoryError(f"Audio data size mismatch: expected {self._num_samples}, got {len(data)}")
        
        # Note: In actual implementation, this would set in C++
        self._data = data
    
    def set_sample_rate(self, sample_rate: int) -> None:
        """
        Set audio sample rate.
        
        Args:
            sample_rate: New sample rate in Hz
            
        Raises:
            SensoryError: If sample rate cannot be set
        """
        if sample_rate <= 0:
            raise SensoryError(f"Invalid sample rate: {sample_rate}")
        
        self._sample_rate = sample_rate
    
    def get_sample_rate(self) -> int:
        """
        Get audio sample rate.
        
        Returns:
            Sample rate in Hz
            
        Raises:
            SensoryError: If sample rate cannot be retrieved
        """
        return self._sample_rate
    
    def get_num_samples(self) -> int:
        """
        Get number of audio samples.
        
        Returns:
            Number of samples
            
        Raises:
            SensoryError: If sample count cannot be retrieved
        """
        return len(self._data) if hasattr(self, '_data') else self._num_samples
    
    def get_duration(self) -> float:
        """
        Get audio duration.
        
        Returns:
            Audio duration in seconds
            
        Raises:
            SensoryError: If duration cannot be calculated
        """
        if self._sample_rate <= 0:
            return 0.0
        return len(self._data) / self._sample_rate
    
    def normalize(self) -> None:
        """
        Normalize audio data to [-1, 1] range.
        
        Raises:
            SensoryError: If normalization fails
        """
        if not hasattr(self, '_data'):
            return
        
        max_val = max(abs(x) for x in self._data) if self._data else 1.0
        if max_val > 1.0:
            self._data = [x / max_val for x in self._data]
    
    def __repr__(self) -> str:
        """String representation of audio"""
        return f"<Audio {self.getSampleRate()} Hz, {self.getNumSamples()} samples>"
    
    def __str__(self) -> str:
        """String representation of audio"""
        return self.__repr__()

class InternalSignals(SensoryInput):
    """
    Internal signals sensory input
    
    Represents internal neural and physiological signals from the agent.
    Used for self-model and homeostasis processing in NLM agents.
    
    Examples:
        >>> internal = InternalSignals()
        >>> internal.addSignal(0.5)  # Add signal value
        >>> internal.addSignal(0.8)
        >>> signals = internal.getSignals()
    """
    
    def __init__(self):
        """
        Initialize internal signals sensory input.
        """
        super().__init__()
        
        # Note: In actual implementation, this would create C++ InternalSignals object
        self._signals = []
        self._initialized = True
    
    def add_signal(self, value: float) -> None:
        """
        Add a signal value.
        
        Args:
            value: Signal value to add
            
        Raises:
            SensoryError: If signal cannot be added
        """
        if not self._initialized:
            raise SensoryError("InternalSignals not initialized")
        
        self._signals.append(value)
    
    def add_signal_list(self, values: List[float]) -> None:
        """
        Add multiple signal values.
        
        Args:
            values: Signal values to add
            
        Raises:
            SensoryError: If signals cannot be added
        """
        if not self._initialized:
            raise SensoryError("InternalSignals not initialized")
        
        self._signals.extend(values)
    
    def clear_signals(self) -> None:
        """
        Clear all signals.
        
        Raises:
            SensoryError: If signals cannot be cleared
        """
        if not self._initialized:
            raise SensoryError("InternalSignals not initialized")
        
        self._signals = []
    
    def get_signals(self) -> List[float]:
        """
        Get all signal values.
        
        Returns:
            List of signal values
            
        Raises:
            SensoryError: If signals cannot be retrieved
        """
        return self._signals.copy()
    
    def get_signal_count(self) -> int:
        """
        Get number of signals.
        
        Returns:
            Number of signals
            
        Raises:
            SensoryError: If signal count cannot be retrieved
        """
        return len(self._signals)
    
    def get_signal_mean(self) -> float:
        """
        Get mean of all signals.
        
        Returns:
            Mean signal value
            
        Raises:
            SensoryError: If mean cannot be calculated
        """
        if not self._signals:
            return 0.0
        return sum(self._signals) / len(self._signals)
    
    def get_signal_std(self) -> float:
        """
        Get standard deviation of all signals.
        
        Returns:
            Standard deviation of signals
            
        Raises:
            SensoryError: If std cannot be calculated
        """
        if len(self._signals) <= 1:
            return 0.0
        
        mean = self.get_signal_mean()
        variance = sum((x - mean) ** 2 for x in self._signals) / len(self._signals)
        return variance ** 0.5
    
    def normalize(self) -> None:
        """
        Normalize signals to [0, 1] range.
        
        Raises:
            SensoryError: If normalization fails
        """
        if not self._signals:
            return
        
        max_val = max(self._signals)
        min_val = min(self._signals)
        
        if max_val > min_val:
            self._signals = [(x - min_val) / (max_val - min_val) for x in self._signals]
        else:
            self._signals = [0.5] * len(self._signals)
    
    def __repr__(self) -> str:
        """String representation of internal signals"""
        return f"<InternalSignals {len(self._signals)} signals>"
    
    def __str__(self) -> str:
        """String representation of internal signals"""
        return self.__repr__()

# Factory functions for sensory input
def create_vision(width: int, height: int, channels: int = 3) -> Vision:
    """
    Create vision sensory input.
    
    Args:
        width: Image width in pixels
        height: Image height in pixels
        channels: Number of color channels (typically 3 for RGB)
        
    Returns:
        New Vision instance
        
    Examples:
        >>> vision = createVision(640, 480, 3)
    """
    return Vision(width, height, channels)

def create_audio(sample_rate: int, num_samples: int = 0) -> Audio:
    """
    Create audio sensory input.
    
    Args:
        sample_rate: Audio sample rate in Hz
        num_samples: Number of audio samples
        
    Returns:
        New Audio instance
        
    Examples:
        >>> audio = createAudio(44100, 1024)
    """
    return Audio(sample_rate, num_samples)

def create_internal_signals() -> InternalSignals:
    """
    Create internal signals sensory input.
    
    Returns:
        New InternalSignals instance
        
    Examples:
        >>> internal = createInternalSignals()
    """
    return InternalSignals()