# Enhanced world interface for NLM simulations
# This module provides the Pythonic interface to world and environment simulation

import numpy as np
from typing import Optional, List, Dict, Any, Tuple
from enum import Enum
from .core import NLMError, ValidationError, SimulationError
from .brain import BrainSession

class SensoryData:
    """
    Enhanced container for sensory input data with Pythonic access methods.
    
    This class provides a structured interface to sensory data from different
    modalities (vision, audio, internal signals) with convenient access methods
    and statistics.
    """
    
    def __init__(self, vision_data=None, audio_data=None, internal_data=None, timestamp=0.0):
        """
        Initialize sensory data.
        
        Args:
            vision_data: Visual input data (numpy array)
            audio_data: Audio input data (numpy array)
            internal_data: Internal signal data (numpy array)
            timestamp: Timestamp of sensory data collection
        """
        self.vision_data = vision_data
        self.audio_data = audio_data
        self.internal_data = internal_data
        self.timestamp = timestamp
        
    @property
    def has_vision(self) -> bool:
        """Check if visual data is available."""
        return self.vision_data is not None and len(self.vision_data) > 0
        
    @property
    def has_audio(self) -> bool:
        """Check if audio data is available."""
        return self.audio_data is not None and len(self.audio_data) > 0
        
    @property
    def has_internal(self) -> bool:
        """Check if internal signal data is available."""
        return self.internal_data is not None and len(self.internal_data) > 0
        
    def get_vision_shape(self) -> Optional[Tuple[int, ...]]:
        """Get the shape of vision data."""
        if self.vision_data is not None:
            return self.vision_data.shape
        return None
        
    def get_audio_shape(self) -> Optional[Tuple[int, ...]]:
        """Get the shape of audio data."""
        if self.audio_data is not None:
            return self.audio_data.shape
        return None
        
    def get_internal_shape(self) -> Optional[Tuple[int, ...]]:
        """Get the shape of internal signal data."""
        if self.internal_data is not None:
            return self.internal_data.shape
        return None
        
    def get_vision_stats(self) -> Dict[str, float]:
        """Get statistics about vision data."""
        if self.vision_data is not None:
            return {
                'mean': float(np.mean(self.vision_data)),
                'std': float(np.std(self.vision_data)),
                'min': float(np.min(self.vision_data)),
                'max': float(np.max(self.vision_data)),
                'shape': self.vision_data.shape,
            }
        return {}
        
    def get_audio_stats(self) -> Dict[str, float]:
        """Get statistics about audio data."""
        if self.audio_data is not None:
            return {
                'mean': float(np.mean(self.audio_data)),
                'std': float(np.std(self.audio_data)),
                'min': float(np.min(self.audio_data)),
                'max': float(np.max(self.audio_data)),
                'shape': self.audio_data.shape,
            }
        return {}
        
    def get_internal_stats(self) -> Dict[str, float]:
        """Get statistics about internal signal data."""
        if self.internal_data is not None:
            return {
                'mean': float(np.mean(self.internal_data)),
                'std': float(np.std(self.internal_data)),
                'min': float(np.min(self.internal_data)),
                'max': float(np.max(self.internal_data)),
                'shape': self.internal_data.shape,
            }
        return {}
        
    def to_dict(self) -> Dict[str, Any]:
        """Convert sensory data to dictionary."""
        return {
            'vision': self.vision_data.tolist() if self.vision_data is not None else None,
            'audio': self.audio_data.tolist() if self.audio_data is not None else None,
            'internal': self.internal_data.tolist() if self.internal_data is not None else None,
            'timestamp': self.timestamp,
        }
        
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'SensoryData':
        """
        Create SensoryData from dictionary.
        
        Args:
            data: Dictionary with sensory data
            
        Returns:
            SensoryData instance
        """
        return cls(
            vision_data=np.array(data['vision']) if data.get('vision') is not None else None,
            audio_data=np.array(data['audio']) if data.get('audio') is not None else None,
            internal_data=np.array(data['internal']) if data.get('internal') is not None else None,
            timestamp=data.get('timestamp', 0.0),
        )
        
    def __repr__(self) -> str:
        """String representation of sensory data."""
        modalities = []
        if self.has_vision:
            modalities.append(f"vision(shape={self.get_vision_shape()})")
        if self.has_audio:
            modalities.append(f"audio(shape={self.get_audio_shape()})")
        if self.has_internal:
            modalities.append(f"internal(shape={self.get_internal_shape()})")
            
        return f"SensoryData({', '.join(modalities)}, timestamp={self.timestamp:.3f})"
class WorldInterface:
    """
    Enhanced world interface with Pythonic API.
    
    This class provides an enhanced interface to the SimpleWorld class with
    better error handling, type hints, and convenience methods.
    """
    
    def __init__(self, width: int = 100, height: int = 100, 
                 vision_width: int = 32, vision_height: int = 32):
        """
        Initialize world interface.
        
        Args:
            width: World width in units
            height: World height in units
            vision_width: Vision width in pixels
            vision_height: Vision height in pixels
        """
        self._width = width
        self._height = height
        self._vision_width = vision_width
        self._vision_height = vision_height
        
    def configure(self, width: int, height: int, vision_width: int, vision_height: int):
        """
        Configure world dimensions.
        
        Args:
            width: World width in units
            height: World height in units
            vision_width: Vision width in pixels
            vision_height: Vision height in pixels
        """
        if width <= 0:
            raise ValueError("Width must be positive")
        if height <= 0:
            raise ValueError("Height must be positive")
        if vision_width <= 0:
            raise ValueError("Vision width must be positive")
        if vision_height <= 0:
            raise ValueError("Vision height must be positive")
            
        self._width = width
        self._height = height
        self._vision_width = vision_width
        self._vision_height = vision_height
        
    def get_sensory_percept(self) -> SensoryData:
        """
        Get sensory percept from the world.
        
        Returns:
            SensoryData object with world observations
            
        Raises:
            SimulationError: If percept cannot be retrieved
        """
        # This would get actual percept from C++ world
        # For now, return empty percept as placeholder
        return SensoryData()
        
    def apply_motor_command(self, action, current_time: float):
        """
        Apply motor command to the world.
        
        Args:
            action: Action to apply
            current_time: Current simulation time
            
        Raises:
            SimulationError: If action cannot be applied
        """
        pass
        
    @property
    def width(self) -> int:
        """Get world width."""
        return self._width
        
    @width.setter
    def width(self, value: int):
        """Set world width."""
        if value <= 0:
            raise ValueError("Width must be positive")
        self._width = value
        
    @property
    def height(self) -> int:
        """Get world height."""
        return self._height
        
    @height.setter
    def height(self, value: int):
        """Set world height."""
        if value <= 0:
            raise ValueError("Height must be positive")
        self._height = value
        
    @property
    def vision_width(self) -> int:
        """Get vision width."""
        return self._vision_width
        
    @vision_width.setter
    def vision_width(self, value: int):
        """Set vision width."""
        if value <= 0:
            raise ValueError("Vision width must be positive")
        self._vision_width = value
        
    @property
    def vision_height(self) -> int:
        """Get vision height."""
        return self._vision_height
        
    @vision_height.setter
    def vision_height(self, value: int):
        """Set vision height."""
        if value <= 0:
            raise ValueError("Vision height must be positive")
        self._vision_height = value
        
    def get_environment_stats(self) -> Dict[str, Any]:
        """
        Get environment statistics.
        
        Returns:
            Dictionary with environment statistics
        """
        return {
            'width': self.width,
            'height': self.height,
            'vision_width': self.vision_width,
            'vision_height': self.vision_height,
            'total_area': self.width * self.height,
            'vision_area': self.vision_width * self.vision_height,
        }
        
    def __repr__(self) -> str:
        """String representation of world interface."""
        return (f"WorldInterface(width={self.width}, height={self.height}, "
                f"vision_width={self.vision_width}, vision_height={self.vision_height})")