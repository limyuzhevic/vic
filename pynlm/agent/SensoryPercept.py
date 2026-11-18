'''Sensory percept data for pynlm'''

from __future__ import annotations

from typing import Optional, List, Dict, Any

from .sensory import Vision, Audio, InternalSignals


class SensoryPercept:
    """Sensory percept data.
    
    The SensoryPercept class represents processed sensory data
    from all modalities, providing a unified view of the agent's
    perception of the world.
    """
    
    def __init__(self, vision_width: int = 0, vision_height: int = 0):
        """Initialize a SensoryPercept instance.
        
        Args:
            vision_width: Width of vision field.
            vision_height: Height of vision field.
        """
        # Vision
        self.vision: Optional[Vision] = None
        if vision_width > 0 and vision_height > 0:
            self.vision = Vision(vision_width, vision_height)
        
        # Touch/perception
        self.touch: List[float] = []
        
        # Internal signals
        self.internal: Optional[InternalSignals] = None
        
        # Proprioception (body position in space)
        self.proprioception: List[float] = []
        
        # Audio
        self.audio: Optional[Audio] = None
        
        # Timestamp
        self.timestamp: float = 0.0
    
    def getVision(self) -> Optional[Vision]:
        """Get vision percept.
        
        Returns:
            Vision percept.
        """
        return self.vision
    
    def setVision(self, vision: Vision):
        """Set vision percept.
        
        Args:
            vision: Vision percept to set.
        """
        self.vision = vision
    
    def getVisionWidth(self) -> int:
        """Get vision width.
        
        Returns:
            Vision width.
        """
        return self.vision.getWidth() if self.vision else 0
    
    def getVisionHeight(self) -> int:
        """Get vision height.
        
        Returns:
            Vision height.
        """
        return self.vision.getHeight() if self.vision else 0
    
    def getTouch(self) -> List[float]:
        """Get touch percept.
        
        Returns:
            Touch percept data.
        """
        return self.touch.copy()
    
    def setTouch(self, touch: List[float]):
        """Set touch percept.
        
        Args:
            touch: Touch percept data.
        """
        self.touch = touch.copy()
    
    def getInternal(self) -> Optional[InternalSignals]:
        """Get internal signals percept.
        
        Returns:
            Internal signals percept.
        """
        return self.internal
    
    def setInternal(self, internal: InternalSignals):
        """Set internal signals percept.
        
        Args:
            internal: Internal signals percept to set.
        """
        self.internal = internal
    
    def getProprioception(self) -> List[float]:
        """Get proprioception percept.
        
        Returns:
            Proprioception percept data.
        """
        return self.proprioception.copy()
    
    def setProprioception(self, proprioception: List[float]):
        """Set proprioception percept.
        
        Args:
            proprioception: Proprioception percept data.
        """
        self.proprioception = proprioception.copy()
    
    def getAudio(self) -> Optional[Audio]:
        """Get audio percept.
        
        Returns:
            Audio percept.
        """
        return self.audio
    
    def setAudio(self, audio: Audio):
        """Set audio percept.
        
        Args:
            audio: Audio percept to set.
        """
        self.audio = audio
    
    def getAllSignals(self) -> Dict[str, Any]:
        """Get all sensory signals as a dictionary.
        
        Returns:
            Dictionary of all sensory signals.
        """
        signals = {
            'vision': self.vision,
            'touch': self.touch,
            'internal': self.internal,
            'proprioception': self.proprioception,
            'audio': self.audio,
        }
        return signals
    
    def getTimestamp(self) -> float:
        """Get percept timestamp.
        
        Returns:
            Percept timestamp.
        """
        return self.timestamp
    
    def setTimestamp(self, timestamp: float):
        """Set percept timestamp.
        
        Args:
            timestamp: New timestamp.
        """
        self.timestamp = timestamp
    
    def clearAllSignals(self):
        """Clear all sensory signals."""
        if self.vision:
            self.vision.data = [0.0] * self.vision.getDimensions()
        
        self.touch.clear()
        
        if self.internal:
            self.internal.clearSignals()
        
        self.proprioception.clear()
        
        if self.audio:
            self.audio.data = [0.0] * self.audio.getNumSamples()
    
    def __repr__(self) -> str:
        """Get string representation of the sensory percept.
        
        Returns:
            String representation of the sensory percept.
        """
        signals = []
        if self.vision:
            signals.append(f"vision({self.vision.getWidth()}x{self.vision.getHeight()})")
        if self.touch:
            signals.append(f"touch({len(self.touch)})")
        if self.internal:
            signals.append(f"internal")
        if self.proprioception:
            signals.append(f"proprioception({len(self.proprioception)})")
        if self.audio:
            signals.append(f"audio({self.audio.getSampleRate()}Hz)")
        
        return f"<SensoryPercept ({', '.join(signals)}, timestamp: {self.timestamp:.2f}s)>"


__all__ = ['SensoryPercept']