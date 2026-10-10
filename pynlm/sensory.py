"""
Pythonic interface for NLM sensory systems.

This module provides interfaces for sensory processing in the NLM system,
including vision, audio, and internal signal processing.
"""

from typing import Optional, List, Any, Union
from dataclasses import dataclass

# Import the C++ extension module directly
import pynlm as _c_extension
from .core import Config, ConfigError

if _c_extension is None:
    raise ImportError(
        "NLM C++ extension not found. Please build the extension first:\n"
        "mkdir build && cd build && cmake .. && make"
    )
class SensoryError(Exception):
    """Raised when sensory operations fail."""
    pass
class Vision:
    """Vision sensory input for NLM system.
    
    Vision represents a 2D camera input with configurable resolution and
    color channels, providing visual information to the brain.
    
    Examples:
        >>> from pynlm import Vision
        >>> vision = Vision(64, 64)  # 64x64 grayscale
        >>> vision = Vision(128, 128, 3)  # 128x128 RGB
        >>> vision.setData([0.1, 0.2, 0.3, ...])  # pixel data
        >>> width = vision.getWidth()
        >>> height = vision.getHeight()
        >>> channels = vision.getChannels()
    """
    
    def __init__(self, width: Optional[int] = None, height: Optional[int] = None, channels: int = 3) -> None:
        """Create vision sensory input.
        
        Args:
            width: Image width in pixels (optional, for empty vision).
            height: Image height in pixels (optional, for empty vision).
            channels: Number of color channels (1=grayscale, 3=RGB, etc.).
            
        Examples:
            >>> vision = Vision()  # Empty vision
            >>> vision = Vision(64, 64)  # 64x64 grayscale
            >>> vision = Vision(128, 128, 3)  # 128x128 RGB
        """
        if width is not None and height is not None:
            self._vision = _c_extension.Vision(width, height, channels)
        else:
            self._vision = _c_extension.Vision()
    
    def set_data(self, data: List[float]) -> None:
        """Set vision data.
        
        Args:
            data: List of pixel values. For grayscale: [width*height],
                for RGB: [width*height*3] in row-major order.
                
        Examples:
            >>> vision.setData([0.1, 0.2, 0.3, ...])
        """
        self._vision.setData(data)
    
    def get_width(self) -> int:
        """Get vision width.
        
        Returns:
            Image width in pixels.
            
        Examples:
            >>> width = vision.getWidth()
        """
        return self._vision.getWidth()
    
    def get_height(self) -> int:
        """Get vision height.
        
        Returns:
            Image height in pixels.
            
        Examples:
            >>> height = vision.getHeight()
        """
        return self._vision.getHeight()
    
    def get_channels(self) -> int:
        """Get number of color channels.
        
        Returns:
            Number of color channels.
            
        Examples:
            >>> channels = vision.getChannels()
        """
        return self._vision.getChannels()
    
    def get_dimensions(self) -> tuple[int, int, int]:
        """Get vision dimensions.
        
        Returns:
            Tuple of (width, height, channels).
            
        Examples:
            >>> width, height, channels = vision.getDimensions()
        """
        return (self.get_width(), self.get_height(), self.get_channels())
    
    def __repr__(self) -> str:
        """String representation of vision."""
        return f"<Vision: {self.get_width()}x{self.get_height()}x{self.get_channels()}>"
    
    def __str__(self) -> str:
        """String representation of vision."""
        return f"Vision({self.get_width()}x{self.get_height()}x{self.get_channels()})"
class Audio:
    """Audio sensory input for NLM system.
    
    Audio represents sound input with configurable sample rate and duration,
    providing auditory information to the brain.
    
    Examples:
        >>> from pynlm import Audio
        >>> audio = Audio(44100, 1024)  # CD quality audio
        >>> audio.setData([0.1, 0.2, 0.3, ...])  # audio samples
        >>> sample_rate = audio.getSampleRate()
        >>> num_samples = audio.getNumSamples()
    """
    
    def __init__(self, sample_rate: Optional[int] = None, num_samples: Optional[int] = None) -> None:
        """Create audio sensory input.
        
        Args:
            sample_rate: Audio sample rate in Hz (optional, for empty audio).
            num_samples: Number of audio samples (optional, for empty audio).
            
        Examples:
            >>> audio = Audio()  # Empty audio
            >>> audio = Audio(44100, 1024)  # CD quality, 1 second
        """
        if sample_rate is not None and num_samples is not None:
            self._audio = _c_extension.Audio(sample_rate, num_samples)
        else:
            self._audio = _c_extension.Audio()
    
    def set_data(self, data: List[float]) -> None:
        """Set audio data.
        
        Args:
            data: List of audio sample values (-1.0 to 1.0).
            
        Examples:
            >>> audio.setData([0.1, -0.2, 0.3, ...])
        """
        self._audio.setData(data)
    
    def set_sample_rate(self, sample_rate: int) -> None:
        """Set audio sample rate.
        
        Args:
            sample_rate: Audio sample rate in Hz.
            
        Examples:
            >>> audio.setSampleRate(44100)
        """
        self._audio.setSampleRate(sample_rate)
    
    def get_sample_rate(self) -> int:
        """Get audio sample rate.
        
        Returns:
            Audio sample rate in Hz.
            
        Examples:
            >>> sample_rate = audio.getSampleRate()
        """
        return self._audio.getSampleRate()
    
    def get_num_samples(self) -> int:
        """Get number of audio samples.
        
        Returns:
            Number of audio samples.
            
        Examples:
            >>> num_samples = audio.getNumSamples()
        """
        return self._audio.getNumSamples()
    
    def get_duration(self) -> float:
        """Get audio duration in seconds.
        
        Returns:
            Audio duration in seconds.
            
        Examples:
            >>> duration = audio.getDuration()
        """
        if self.get_sample_rate() > 0:
            return self.get_num_samples() / self.get_sample_rate()
        return 0.0
    
    def __repr__(self) -> str:
        """String representation of audio."""
        return f"<Audio: {self.getSampleRate()}Hz, {self.getNumSamples()}samples>"
    
    def __str__(self) -> str:
        """String representation of audio."""
        return f"Audio({self.getSampleRate()}Hz, {self.getNumSamples()}samples)"
class InternalSignals:
    """Internal signals sensory input for NLM system.
    
    InternalSignals represents internal brain state information,
    such as attention, arousal, or other internal physiological states.
    
    Examples:
        >>> from pynlm import InternalSignals
        >>> signals = InternalSignals()
        >>> signals.addSignal(0.5)  # Add signal value
        >>> signals.addSignal(0.8)
        >>> signals.clearSignals()
    """
    
    def __init__(self) -> None:
        """Create empty internal signals."""
        self._signals = _c_extension.InternalSignals()
    
    def add_signal(self, value: float) -> None:
        """Add signal value.
        
        Args:
            value: Signal value to add (-1.0 to 1.0).
            
        Examples:
            >>> signals.addSignal(0.5)
        """
        self._signals.addSignal(value)
    
    def clear_signals(self) -> None:
        """Clear all signals.
        
        Examples:
            >>> signals.clearSignals()
        """
        self._signals.clearSignals()
    
    def get_signal_count(self) -> int:
        """Get number of signals.
        
        Returns:
            Number of stored signal values.
            
        Examples:
            >>> count = signals.getSignalCount()
        """
        # Note: This would need to be implemented in C++ if not already available
        return 0
    
    def __repr__(self) -> str:
        """String representation of internal signals."""
        return f"<InternalSignals: {self.getSignalCount()} signals>"
    
    def __str__(self) -> str:
        """String representation of internal signals."""
        return f"InternalSignals({self.getSignalCount()} signals)"