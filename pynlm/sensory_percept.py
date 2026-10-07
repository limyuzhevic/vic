# Sensory percept implementation for NLM Python API

"""
Sensory percept data for NLM Python API

Represents sensory percept data from the world, containing
vision, touch, internal signals, proprioception, and audio.
"""

from typing import List, Any, Optional, Union, Dict
from pathlib import Path

# Import the C++ binding and type definitions
from pynlm.types import Vision as CppVision
from pynlm.types import Audio as CppAudio
from pynlm.types import InternalSignals as CppInternalSignals
from pynlm.types import SensoryInput as CppSensoryInput
from pynlm.types import Timestamp, Microvolts

class NLMError(Exception):
    """Base exception for NLM Python API errors"""
    pass

class SensoryPerceptError(NLMError):
    """Raised when sensory percept operations fail"""
    pass

class SensoryPercept:
    """
    Sensory percept data from world
    
    Contains vision, touch, internal signals, proprioception, and audio.
    Used for agent perception in the NLM simulation.
    
    The percept acts as the interface between the world and the agent's
    sensory processing systems, providing all available sensory information
    at a given timestep.
    
    Examples:
        >>> percept = SensoryPercept()
        >>> vision = Vision(width=8, height=8)
        >>> vision.setData([0.0, 1.0, 0.0] * 64)  # Green data
        >>> percept.setVision(vision)
        >>> internal = InternalSignals()
        >>> internal.addSignal(0.5)
        >>> internal.addSignal(0.8)
        >>> percept.setInternal(internal)
        >>> 
        >>> # Access sensory data
        >>> vision = percept.getVision()
        >>> vision_data = vision.getGrayscale()
        >>> internal_signals = percept.getInternal()
        >>> signals = internal_signals.getSignals()
    """
    
    def __init__(self):
        """
        Initialize sensory percept.
        """
        # Note: In actual implementation, this would wrap C++ SensoryPercept object
        self._vision = None
        self._touch = None
        self._internal = None
        self._proprioception = None
        self._audio = None
        self._timestamp = 0.0
        self._initialized = True
    
    def get_vision(self) -> Any:
        """
        Get vision data.
        
        Returns:
            Vision sensory input
            
        Raises:
            SensoryPerceptError: If vision cannot be retrieved
        """
        if self._vision is None:
            raise SensoryPerceptError("No vision data in percept")
        return self._vision
    
    def set_vision(self, vision: Any) -> None:
        """
        Set vision data.
        
        Args:
            vision: Vision sensory input
            
        Raises:
            SensoryPerceptError: If vision cannot be set
        """
        if vision is not None:
            self._vision = vision
    
    def get_vision_width(self) -> int:
        """
        Get vision width.
        
        Returns:
            Vision width in pixels
            
        Raises:
            SensoryPerceptError: If vision width cannot be retrieved
        """
        if self._vision is None:
            return 0
        return self._vision.get_width()
    
    def get_vision_height(self) -> int:
        """
        Get vision height.
        
        Returns:
            Vision height in pixels
            
        Raises:
            SensoryPerceptError: If vision height cannot be retrieved
        """
        if self._vision is None:
            return 0
        return self._vision.get_height()
    
    def get_touch(self) -> Any:
        """
        Get touch sensory data.
        
        Returns:
            Touch sensory input
            
        Raises:
            SensoryPerceptError: If touch cannot be retrieved
        """
        if self._touch is None:
            raise SensoryPerceptError("No touch data in percept")
        return self._touch
    
    def set_touch(self, touch: Any) -> None:
        """
        Set touch sensory data.
        
        Args:
            touch: Touch sensory input
            
        Raises:
            SensoryPerceptError: If touch cannot be set
        """
        if touch is not None:
            self._touch = touch
    
    def get_internal(self) -> Any:
        """
        Get internal signals.
        
        Returns:
            Internal signals sensory input
            
        Raises:
            SensoryPerceptError: If internal signals cannot be retrieved
        """
        if self._internal is None:
            raise SensoryPerceptError("No internal signals in percept")
        return self._internal
    
    def set_internal(self, internal: Any) -> None:
        """
        Set internal signals.
        
        Args:
            internal: Internal signals sensory input
            
        Raises:
            SensoryPerceptError: If internal signals cannot be set
        """
        if internal is not None:
            self._internal = internal
    
    def get_proprioception(self) -> Any:
        """
        Get proprioception data.
        
        Returns:
            Proprioception sensory input
            
        Raises:
            SensoryPerceptError: If proprioception cannot be retrieved
        """
        if self._proprioception is None:
            raise SensoryPerceptError("No proprioception data in percept")
        return self._proprioception
    
    def set_proprioception(self, proprioception: Any) -> None:
        """
        Set proprioception data.
        
        Args:
            proprioception: Proprioception sensory input
            
        Raises:
            SensoryPerceptError: If proprioception cannot be set
        """
        if proprioception is not None:
            self._proprioception = proprioception
    
    def get_audio(self) -> Any:
        """
        Get audio data.
        
        Returns:
            Audio sensory input
            
        Raises:
            SensoryPerceptError: If audio cannot be retrieved
        """
        if self._audio is None:
            raise SensoryPerceptError("No audio data in percept")
        return self._audio
    
    def set_audio(self, audio: Any) -> None:
        """
        Set audio data.
        
        Args:
            audio: Audio sensory input
            
        Raises:
            SensoryPerceptError: If audio cannot be set
        """
        if audio is not None:
            self._audio = audio
    
    def get_all_signals(self) -> Dict[str, Any]:
        """
        Get all sensory signals.
        
        Returns:
            Dictionary of all sensory signals
            
        Examples:
            >>> percept = SensoryPercept()
            >>> percept.setVision(vision)
            >>> percept.setInternal(internal)
            >>> signals = percept.getAllSignals()
            >>> print(f"Has vision: {'vision' in signals}")
            >>> print(f"Has internal: {'internal' in signals}")
        """
        signals = {}
        if self._vision is not None:
            signals['vision'] = self._vision
        if self._touch is not None:
            signals['touch'] = self._touch
        if self._internal is not None:
            signals['internal'] = self._internal
        if self._proprioception is not None:
            signals['proprioception'] = self._proprioception
        if self._audio is not None:
            signals['audio'] = self._audio
        return signals
    
    def get_sensory_types(self) -> List[str]:
        """
        Get list of sensory types present in percept.
        
        Returns:
            List of sensory type names
            
        Examples:
            >>> percept = SensoryPercept()
            >>> percept.setVision(vision)
            >>> percept.setInternal(internal)
            >>> types = percept.getSensoryTypes()
            >>> print(f"Sensory types: {types}")  # ['vision', 'internal']
        """
        return list(self.get_all_signals().keys())
    
    def has_sensory(self, sensory_type: str) -> bool:
        """
        Check if percept has sensory data of given type.
        
        Args:
            sensory_type: Sensory type (e.g., 'vision', 'audio')
            
        Returns:
            True if percept has sensory data of that type
            
        Examples:
            >>> percept = SensoryPercept()
            >>> percept.setVision(vision)
            >>> if percept.hasSensory('vision'):
            ...     print("Percept has vision data")
        """
        return sensory_type in self.get_all_signals()
    
    def get_timestamp(self) -> Timestamp:
        """
        Get percept timestamp.
        
        Returns:
            Percept timestamp
            
        Examples:
            >>> percept = SensoryPercept()
            >>> timestamp = percept.getTimestamp()
        """
        return self._timestamp
    
    def set_timestamp(self, timestamp: Timestamp) -> None:
        """
        Set percept timestamp.
        
        Args:
            timestamp: Percept timestamp
            
        Examples:
            >>> percept = SensoryPercept()
            >>> percept.setTimestamp(0.1)
        """
        self._timestamp = float(timestamp)
    
    def get_vision_grayscale(self) -> List[float]:
        """
        Get vision data as grayscale.
        
        Returns:
            Grayscale vision data
            
        Raises:
            SensoryPerceptError: If vision data cannot be converted to grayscale
        """
        if self._vision is None:
            return []
        return self._vision.get_grayscale()
    
    def get_signal_count(self, sensory_type: str) -> int:
        """
        Get number of signals for given sensory type.
        
        Args:
            sensory_type: Sensory type
            
        Returns:
            Number of signals, 0 if type not present
            
        Examples:
            >>> percept = SensoryPercept()
            >>> percept.setInternal(internal)
            >>> count = percept.getSignalCount('internal')
            >>> print(f"Internal signal count: {count}")
        """
        signals = self.get_all_signals()
        if sensory_type not in signals:
            return 0
        
        sensory_input = signals[sensory_type]
        if hasattr(sensory_input, 'get_signal_count'):
            return sensory_input.get_signal_count()
        elif hasattr(sensory_input, 'getNumSamples') and sensory_type == 'audio':
            return sensory_input.getNumSamples()
        elif hasattr(sensory_input, 'get_height') and hasattr(sensory_input, 'get_width') and sensory_type == 'vision':
            # For vision, count flattened pixels
            width = sensory_input.get_width()
            height = sensory_input.get_height()
            channels = sensory_input.get_channels()
            return width * height * channels
        else:
            return 0
    
    def get_signal_mean(self, sensory_type: str) -> float:
        """
        Get mean of signals for given sensory type.
        
        Args:
            sensory_type: Sensory type
            
        Returns:
            Mean signal value, 0.0 if type not present or has no signals
            
        Examples:
            >>> percept = SensoryPercept()
            >>> percept.setInternal(internal)
            >>> mean = percept.getSignalMean('internal')
            >>> print(f"Internal signal mean: {mean}")
        """
        signals = self.get_all_signals()
        if sensory_type not in signals:
            return 0.0
        
        sensory_input = signals[sensory_type]
        if hasattr(sensory_input, 'get_signal_mean'):
            return sensory_input.get_signal_mean()
        elif hasattr(sensory_input, 'getNumSamples') and sensory_type == 'audio':
            # For audio, get average sample
            samples = sensory_input.getData()
            return sum(samples) / len(samples) if samples else 0.0
        else:
            return 0.0
    
    def to_dict(self) -> dict:
        """
        Convert percept to dictionary.
        
        Returns:
            Dictionary representation
            
        Examples:
            >>> percept = SensoryPercept()
            >>> percept.setVision(vision)
            >>> data = percept.toDict()
            >>> print(f"Sensory types: {list(data.keys())}")
        """
        return {
            'vision': self._vision,
            'touch': self._touch,
            'internal': self._internal,
            'proprioception': self._proprioception,
            'audio': self._audio,
            'timestamp': self._timestamp,
            'sensory_types': self.get_sensory_types(),
            'signal_counts': {
                st: self.get_signal_count(st) for st in self.get_sensory_types()
            }
        }
    
    def __repr__(self) -> str:
        """String representation of sensory percept"""
        sensory_types = self.get_sensory_types()
        return f"<SensoryPercept {len(sensory_types)} types: {', '.join(sensory_types)}>"
    
    def __str__(self) -> str:
        """String representation of sensory percept"""
        return self.__repr__()

# Factory function for sensory percept creation
def create_sensory_percept() -> SensoryPercept:
    """
    Create a sensory percept.
    
    Returns:
        New SensoryPercept instance
        
    Examples:
        >>> percept = createSensoryPercept()
    """
    return SensoryPercept()