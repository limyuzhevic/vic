"""
SensoryPercept class for NLM neural simulation framework.

This module provides a Python interface to the C++ SensoryPercept class with enhanced
Python features including automatic numpy conversion, error formatting, and
time measurement utilities.
"""

import time
import numpy as np
from typing import Dict, Any, Optional, List, Union
from dataclasses import dataclass
import traceback

# Python utilities
from .python_utils import (
    PythonSessionLogger,
    PythonTimer,
    numpy_utils,
)

# Forward references
Vision = Any
Audio = Any
InternalSignals = Any
Touch = Any
Proprioception = Any

@dataclass
class SensoryPerceptState:
    """State information for sensory percept"""
    vision_data: Any = None
    audio_data: Any = None
    internal_data: Any = None
    touch_data: Any = None
    proprioception_data: Any = None
    timestamp: float = 0.0

class SensoryPercept:
    """
    Sensory percept data class for NLM neural simulation framework.
    
    This class provides a Python interface to the underlying C++ SensoryPercept class
    with enhanced Python features including numpy integration, error formatting,
    and performance timing utilities.
    """
    
    def __init__(self):
        """Initialize sensory percept."""
        self._logger = PythonSessionLogger()
        self._state = SensoryPerceptState()
        
        # Log initialization
        self._logger.logEntry("SensoryPercept initialized")
    
    # Vision methods
    
    def getVision(self) -> Any:
        """Get vision data."""
        return self._state.vision_data
    
    def setVision(self, vision: Vision) -> None:
        """
        Set vision data.
        
        Args:
            vision: Vision data (numpy array or Vision object)
        """
        try:
            if isinstance(vision, np.ndarray):
                # Convert to Vision object or keep as numpy array
                self._state.vision_data = vision
            elif hasattr(vision, 'getData'):
                # Convert Vision object to numpy array
                self._state.vision_data = vision.getData()
            else:
                self._state.vision_data = vision
            
            self._update_timestamp()
            self._logger.logEntry(f"Set vision data: shape={np.array(self._state.vision_data).shape if hasattr(self._state.vision_data, 'shape') else 'N/A'}")
            
        except Exception as e:
            self._format_python_error(f"Failed to set vision data: {e}")
    
    def getVisionWidth(self) -> int:
        """Get vision width."""
        if self._state.vision_data is None:
            return 0
        
        vision = self._state.vision_data
        if isinstance(vision, np.ndarray):
            return vision.shape[1] if len(vision.shape) >= 2 else 0
        elif hasattr(vision, 'getWidth'):
            return vision.getWidth()
        else:
            return 0
    
    def getVisionHeight(self) -> int:
        """Get vision height."""
        if self._state.vision_data is None:
            return 0
        
        vision = self._state.vision_data
        if isinstance(vision, np.ndarray):
            return vision.shape[0] if len(vision.shape) >= 2 else 0
        elif hasattr(vision, 'getHeight'):
            return vision.getHeight()
        else:
            return 0
    
    # Audio methods
    
    def getAudio(self) -> Any:
        """Get audio data."""
        return self._state.audio_data
    
    def setAudio(self, audio: Audio) -> None:
        """
        Set audio data.
        
        Args:
            audio: Audio data (numpy array or Audio object)
        """
        try:
            if isinstance(audio, np.ndarray):
                self._state.audio_data = audio
            elif hasattr(audio, 'getData'):
                self._state.audio_data = audio.getData()
            else:
                self._state.audio_data = audio
            
            self._update_timestamp()
            self._logger.logEntry(f"Set audio data: shape={np.array(self._state.audio_data).shape if hasattr(self._state.audio_data, 'shape') else 'N/A'}")
            
        except Exception as e:
            self._format_python_error(f"Failed to set audio data: {e}")
    
    def getAudioSampleRate(self) -> int:
        """Get audio sample rate."""
        if self._state.audio_data is None:
            return 0
        
        audio = self._state.audio_data
        if hasattr(audio, 'getSampleRate'):
            return audio.getSampleRate()
        else:
            return 0
    
    def getAudioNumSamples(self) -> int:
        """Get number of audio samples."""
        if self._state.audio_data is None:
            return 0
        
        audio = self._state.audio_data
        if isinstance(audio, np.ndarray):
            return len(audio)
        elif hasattr(audio, 'getNumSamples'):
            return audio.getNumSamples()
        else:
            return 0
    
    # Internal signals methods
    
    def getInternal(self) -> Any:
        """Get internal signals data."""
        return self._state.internal_data
    
    def setInternal(self, internal: InternalSignals) -> None:
        """
        Set internal signals data.
        
        Args:
            internal: Internal signals data
        """
        try:
            self._state.internal_data = internal
            self._update_timestamp()
            self._logger.logEntry(f"Set internal data: {type(internal).__name__}")
            
        except Exception as e:
            self._format_python_error(f"Failed to set internal data: {e}")
    
    # Touch methods
    
    def getTouch(self) -> Any:
        """Get touch data."""
        return self._state.touch_data
    
    def setTouch(self, touch: Touch) -> None:
        """
        Set touch data.
        
        Args:
            touch: Touch data
        """
        try:
            self._state.touch_data = touch
            self._update_timestamp()
            self._logger.logEntry(f"Set touch data: {type(touch).__name__}")
            
        except Exception as e:
            self._format_python_error(f"Failed to set touch data: {e}")
    
    # Proprioception methods
    
    def getProprioception(self) -> Any:
        """Get proprioception data."""
        return self._state.proprioception_data
    
    def setProprioception(self, proprioception: Proprioception) -> None:
        """
        Set proprioception data.
        
        Args:
            proprioception: Proprioception data
        """
        try:
            self._state.proprioception_data = proprioception
            self._update_timestamp()
            self._logger.logEntry(f"Set proprioception data: {type(proprioception).__name__}")
            
        except Exception as e:
            self._format_python_error(f"Failed to set proprioception data: {e}")
    
    # All signals method
    
    def getAllSignals(self) -> Dict[str, Any]:
        """
        Get all sensory signals as dictionary.
        
        Returns:
            Dictionary containing all sensory signals
        """
        signals = {}
        
        if self._state.vision_data is not None:
            signals['vision'] = self._state.vision_data
        if self._state.audio_data is not None:
            signals['audio'] = self._state.audio_data
        if self._state.internal_data is not None:
            signals['internal'] = self._state.internal_data
        if self._state.touch_data is not None:
            signals['touch'] = self._state.touch_data
        if self._state.proprioception_data is not None:
            signals['proprioception'] = self._state.proprioception_data
        
        return signals
    
    # Timestamp methods
    
    def getTimestamp(self) -> float:
        """Get timestamp."""
        return self._state.timestamp
    
    def setTimestamp(self, timestamp: float) -> None:
        """
        Set timestamp.
        
        Args:
            timestamp: Timestamp
        """
        self._state.timestamp = timestamp
    
    # Update timestamp
    
    def _update_timestamp(self) -> None:
        """Update timestamp to current time."""
        self._state.timestamp = time.time()
    
    # Validation methods
    
    def validateVisionData(self) -> bool:
        """
        Validate vision data.
        
        Returns:
            True if vision data is valid, False otherwise
        """
        if self._state.vision_data is None:
            return False
        
        try:
            vision = self._state.vision_data
            if isinstance(vision, np.ndarray):
                # Check if array has expected dimensions
                if len(vision.shape) < 2:
                    return False
                if vision.shape[2] not in [1, 3, 4]:  # grayscale, RGB, RGBA
                    return False
            elif hasattr(vision, 'isValidNumpyShape'):
                # Convert to numpy array for validation
                data_array = vision.getData()
                if isinstance(data_array, np.ndarray):
                    # Check expected dimensions
                    expected_dims = vision.getWidth() * vision.getHeight() * vision.getChannels()
                    if len(data_array) != expected_dims:
                        return False
            
            return True
            
        except Exception:
            return False
    
    def validateAudioData(self) -> bool:
        """
        Validate audio data.
        
        Returns:
            True if audio data is valid, False otherwise
        """
        if self._state.audio_data is None:
            return False
        
        try:
            audio = self._state.audio_data
            if isinstance(audio, np.ndarray):
                # Check if array is 1D
                if len(audio.shape) != 1:
                    return False
            elif hasattr(audio, 'isValidNumpyShape'):
                # Convert to numpy array for validation
                data_array = audio.getData()
                if isinstance(data_array, np.ndarray):
                    expected_size = audio.getNumSamples()
                    if len(data_array) != expected_size:
                        return False
            
            return True
            
        except Exception:
            return False
    
    def validateAndConvertNumpy(self, obj: Any) -> tuple:
        """
        Validate and convert numpy array to vision/audio data.
        
        Args:
            obj: Object to validate and convert
            
        Returns:
            Tuple of (is_valid, converted_data, dimensions)
        """
        try:
            if not isinstance(obj, np.ndarray):
                return False, None, None
            
            # Check if array has valid shape (placeholder for vision/audio validation)
            # In real implementation, this would check against specific sensor types
            if len(obj.shape) < 1:
                return False, None, None
            
            # Convert using numpy_utils if available
            if 'numpy_utils' in globals():
                from .python_utils import numpy_utils
                # This would be specific to sensor type
                pass
            
            return True, obj, obj.shape
            
        except Exception:
            return False, None, None
    
    # Reset and copy methods
    
    def reset(self) -> None:
        """Reset sensory percept."""
        self._state = SensoryPerceptState()
        self._logger.logEntry("SensoryPercept reset")
    
    def copy(self) -> 'SensoryPercept':
        """
        Create a copy of sensory percept.
        
        Returns:
            New SensoryPercept instance
        """
        new_percept = SensoryPercept()
        
        # Copy data
        new_percept._state = self._state
        
        return new_percept
    
    def __repr__(self) -> str:
        """String representation of sensory percept."""
        return f"<SensoryPercept: vision={self.getVisionWidth()}x{self.getVisionHeight()}, " \
               f"audio={self.getAudioNumSamples()} samples>"
    
    def __str__(self) -> str:
        """String representation of sensory percept."""
        return f"NLM SensoryPercept with vision and audio inputs"
    
    def __eq__(self, other: Any) -> bool:
        """Check if sensory percepts are equal."""
        if not isinstance(other, SensoryPercept):
            return False
        
        return (self._state.vision_data == other._state.vision_data and
                self._state.audio_data == other._state.audio_data and
                self._state.internal_data == other._state.internal_data)
    
    def __ne__(self, other: Any) -> bool:
        """Check if sensory percepts are not equal."""
        return not self.__eq__(other)
    
    # Helper methods
    
    def _format_python_error(self, error_msg: str) -> None:
        """Format Python error with traceback for better debugging."""
        try:
            # Format traceback for better debugging
            import traceback
            traceback.print_exc()
        except Exception:
            print(f"Error: {error_msg}")

__all__ = ['SensoryPercept']
