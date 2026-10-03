"""
Sensory processing for NLM Python bindings.

This module provides enhanced sensory processing functionality,
including vision, audio, internal signals, and proprioception processing
with Pythonic interfaces.
"""

import numpy as np
from typing import Optional, Dict, Any, List, Tuple

from .exceptions import ValidationError
from .types import NeuronType, SensoryPerceptType

# Import C++ types
try:
    from ..bindings import (
        Vision, Audio, InternalSignals, SensoryPercept,
        SensoryInput as CPPSensoryInput
    )
except ImportError as e:
    raise ImportError(f"Failed to import NLM sensory bindings: {e}")


class NMLSensoryProcessor:
    """
    Enhanced sensory processor with Pythonic interface.
n    
    Provides unified sensory processing interface for all modalities,
    including vision, audio, internal signals, and proprioception.
    """
    
    def __init__(self):
        """Initialize sensory processor."""
        self._sensory_percept = CPPSensoryInput()
        self._enabled_modalities = {
            'vision': True,
            'audio': True,
            'internal': True,
            'proprioception': True,
            'touch': True,
        }
    
    def set_vision(self, vision_data: List[float]) -> None:
        """
        Set vision data.
n        
        Args:
            vision_data: Vision data as list of pixel values
        """
        if not self._enabled_modalities.get('vision', True):
            raise ValidationError("Vision modality is not enabled")
        
        self._sensory_percept.setVision(vision_data)
    
    def set_vision_array(self, vision_array: np.ndarray) -> None:
        """
        Set vision data as NumPy array.
n        
        Args:
            vision_array: Vision data as NumPy array (H, W, C)
        """
        if vision_array.size == 0:
            raise ValidationError("Vision array cannot be empty")
        
        # Convert to flat list
        vision_data = vision_array.flatten().tolist()
        self.set_vision(vision_data)
    
    def get_vision(self) -> List[float]:
        """Get vision data as list."""
        return self._sensory_percept.getVision()
    
    def get_vision_array(self) -> np.ndarray:
        """Get vision data as NumPy array."""
        vision_data = self.get_vision()
        if vision_data:
            # Try to infer dimensions from data
            # For simplicity, return 2D grayscale
            height = int(np.sqrt(len(vision_data)))
            if height * height == len(vision_data):
                return np.array(vision_data).reshape(height, height)
            else:
                # Assume 3D RGB
                channels = len(vision_data) // (self._sensory_percept.getVisionWidth() * self._sensory_percept.getVisionHeight())
                if channels > 0:
                    return np.array(vision_data).reshape(
                        self._sensory_percept.getVisionHeight(),
                        self._sensory_percept.getVisionWidth(),
                        channels
                    )
        
        # Return empty 2D array
        return np.array([])
    
    def set_audio(self, audio_data: List[float]) -> None:
        """
        Set audio data.
n        
        Args:
            audio_data: Audio data as list of samples
        """
        if not self._enabled_modalities.get('audio', True):
            raise ValidationError("Audio modality is not enabled")
        
        self._sensory_percept.setAudio(audio_data)
    
    def set_audio_array(self, audio_array: np.ndarray) -> None:
        """
        Set audio data as NumPy array.
n        
        Args:
            audio_array: Audio data as NumPy array
        """
        audio_data = audio_array.flatten().tolist()
        self.set_audio(audio_data)
    
    def get_audio(self) -> List[float]:
        """Get audio data as list."""
        return self._sensory_percept.getAudio()
    
    def get_audio_array(self) -> np.ndarray:
        """Get audio data as NumPy array."""
        return np.array(self.get_audio())
    
    def set_internal(self, internal_data: List[float]) -> None:
        """
        Set internal signals.
n        
        Args:
            internal_data: Internal signal data
        """
        if not self._enabled_modalities.get('internal', True):
            raise ValidationError("Internal signals modality is not enabled")
        
        self._sensory_percept.setInternal(internal_data)
    
    def get_internal(self) -> List[float]:
        """Get internal signals."""
        return self._sensory_percept.getInternal()
    
    def set_proprioception(self, proprio_data: List[float]) -> None:
        """
        Set proprioception data.
n        
        Args:
            proprio_data: Proprioception data
        """
        if not self._enabled_modalities.get('proprioception', True):
            raise ValidationError("Proprioception modality is not enabled")
        
        self._sensory_percept.setProprioception(proprio_data)
    
    def get_proprioception(self) -> List[float]:
        """Get proprioception data."""
        return self._sensory_percept.getProprioception()
    
    def set_touch(self, touch_data: List[float]) -> None:
        """
        Set touch data.
n        
        Args:
            touch_data: Touch data
        """
        if not self._enabled_modalities.get('touch', True):
            raise ValidationError("Touch modality is not enabled")
        
        self._sensory_percept.setTouch(touch_data)
    
    def get_touch(self) -> List[float]:
        """Get touch data."""
        return self._sensory_percept.getTouch()
    
    def get_all_sensors(self) -> Dict[str, Any]:
        """
        Get all sensory data.
n        
        Returns:
            Dictionary with all sensory data
        """
        return {
            'vision': self.get_vision(),
            'audio': self.get_audio(),
            'internal': self.get_internal(),
            'proprioception': self.get_proprioception(),
            'touch': self.get_touch(),
            'timestamp': self.get_timestamp(),
        }
    
    def get_timestamp(self) -> float:
        """Get timestamp."""
        return self._sensory_percept.getTimestamp()
    
    def set_timestamp(self, timestamp: float) -> None:
        """Set timestamp."""
        self._sensory_percept.setTimestamp(timestamp)
    
    def enable_modality(self, modality: str, enable: bool = True) -> None:
        """
        Enable or disable sensory modality.
n        
        Args:
            modality: Modality name ('vision', 'audio', 'internal', 'proprioception', 'touch')
            enable: Enable state
        """
        if modality not in self._enabled_modalities:
            raise ValidationError(f"Unknown modality: {modality}")
        
        self._enabled_modalities[modality] = enable
    
    def is_modality_enabled(self, modality: str) -> bool:
        """
        Check if modality is enabled.
n        
        Args:
            modality: Modality name
            
        Returns:
            True if modality is enabled
        """
        return self._enabled_modalities.get(modality, False)
    
    def get_enabled_modalities(self) -> List[str]:
        """Get list of enabled modalities."""
        return [m for m, enabled in self._enabled_modalities.items() if enabled]
    
    def get_sensor_dimensions(self) -> Dict[str, Dict[str, int]]:
        """
        Get dimensions for each sensory modality.
n        
        Returns:
            Dictionary with sensor dimensions
        """
        dimensions = {}
        
        if self.is_modality_enabled('vision'):
            dimensions['vision'] = {
                'width': self._sensory_percept.getVisionWidth(),
                'height': self._sensory_percept.getVisionHeight(),
                'channels': 3,  # RGB
            }
        
        if self.is_modality_enabled('audio'):
            # Audio dimensions would depend on sample rate and duration
            dimensions['audio'] = {
                'sample_rate': 44100,
                'channels': 1,
            }
        
        if self.is_modality_enabled('internal'):
            # Internal signals dimensions are configurable
            dimensions['internal'] = {
                'signal_count': 4,
                'range': (-1.0, 1.0),
            }
        
        if self.is_modality_enabled('proprioception'):
            # Proprioception dimensions
            dimensions['proprioception'] = {
                'signal_count': 6,
                'range': (-1.0, 1.0),
            }
        
        if self.is_modality_enabled('touch'):
            # Touch dimensions
            dimensions['touch'] = {
                'sensor_count': 8,
                'range': (0.0, 1.0),
            }
        
        return dimensions
    
    def normalize_all(self) -> 'NMLSensoryProcessor':
        """
        Normalize all sensory data to range [0, 1].
n        
        Returns:
            Self for method chaining
        """
        if self.is_modality_enabled('vision'):
            vision = self.get_vision()
            if vision:
                max_val = max(vision) if vision else 1.0
                if max_val > 0:
                    normalized = [min(1.0, v / max_val) for v in vision]
                    self.set_vision(normalized)
        
        if self.is_modality_enabled('audio'):
            audio = self.get_audio()
            if audio:
                max_val = max(abs(v) for v in audio) if audio else 1.0
                if max_val > 0:
                    normalized = [max(-1.0, min(1.0, v / max_val)) for v in audio]
                    self.set_audio(normalized)
        
        if self.is_modality_enabled('internal'):
            internal = self.get_internal()
            if internal:
                min_val = min(internal)
                max_val = max(internal)
                if max_val > min_val:
                    normalized = [((v - min_val) / (max_val - min_val) * 2.0 - 1.0) for v in internal]
                    self.set_internal(normalized)
        
        return self
    
    def to_dict(self) -> Dict[str, Any]:
        """
        Convert sensory processor to dictionary.
n        
        Returns:
            Dictionary with sensory data
        """
        return {
            'enabled_modalities': self._enabled_modalities.copy(),
            'vision': self.get_vision(),
            'audio': self.get_audio(),
            'internal': self.get_internal(),
            'proprioception': self.get_proprioception(),
            'touch': self.get_touch(),
            'timestamp': self.get_timestamp(),
        }
    
    def from_dict(self, data: Dict[str, Any]) -> 'NMLSensoryProcessor':
        """
        Create sensory processor from dictionary.
n        
        Args:
            data: Sensory data dictionary
            
        Returns:
            Created sensory processor
        """
        processor = NMLSensoryProcessor()
        
        # Restore enabled modalities
        processor._enabled_modalities = data.get('enabled_modalities', processor._enabled_modalities)
        
        # Restore sensory data
        if 'vision' in data:
            processor.set_vision(data['vision'])
        
        if 'audio' in data:
            processor.set_audio(data['audio'])
        
        if 'internal' in data:
            processor.set_internal(data['internal'])
        
        if 'proprioception' in data:
            processor.set_proprioception(data['proprioception'])
        
        if 'touch' in data:
            processor.set_touch(data['touch'])
        
        if 'timestamp' in data:
            processor.set_timestamp(data['timestamp'])
        
        return processor
    
    def __repr__(self) -> str:
        modalities = self.get_enabled_modalities()
        return f"NMLSensoryProcessor(enabled_modalities={modalities})"
# Alias for backward compatibility
SensoryProcessor = NMLSensoryProcessor


# Convenience constructors
def create_sensory_processor() -> NMLSensoryProcessor:
    """Create sensory processor."""
    return NMLSensoryProcessor()


# Type aliases for convenience
SensoryProcessorType = NMLSensoryProcessor

# Export all main classes
__all__.extend([
    'NMLSensoryProcessor', 'create_sensory_processor', 'SensoryProcessorType'
])
