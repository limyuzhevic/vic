"""
Agent-related classes for NLM
"""

import time
from typing import Optional, List
from dataclasses import dataclass, field
from .types import Timestamp, AgentBody, Action, Reward

@dataclass
class AgentBody(AgentBody):
    """Agent body state wrapper for Python API"""
    
    def __init__(self):
        super().__init__()
        # Set default values matching C++ structure
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocityX = 0.0
        self.velocityY = 0.0
        self.angularVelocity = 0.0
        self.energy = 100.0
        self.health = 100.0
        self.age = 0
        self.isMoving = False
        self.isTurning = False
        self.lastActionTime = Timestamp(0.0)
    
    def getPosition(self) -> tuple:
        """Get agent position as (x, y)"""
        return (self.x, self.y)
    
    def setPosition(self, x: float, y: float) -> None:
        """Set agent position"""
        self.x = x
        self.y = y
    
    def getVelocity(self) -> tuple:
        """Get agent velocity as (vx, vy)"""
        return (self.velocityX, self.velocityY)
    
    def getDistanceTo(self, other_body: 'AgentBody') -> float:
        """Calculate distance to another agent body"""
        dx = self.x - other_body.x
        dy = self.y - other_body.y
        return (dx * dx + dy * dy) ** 0.5
    
    def isCollidingWith(self, other_body: 'AgentBody', threshold: float = 1.0) -> bool:
        """Check if colliding with another agent body"""
        distance = self.getDistanceTo(other_body)
        return distance < threshold
    
    def __repr__(self) -> str:
        return f"<AgentBody: pos=({self.x:.1f}, {self.y:.1f}), energy={self.energy:.1f}, health={self.health:.1f}>"


@dataclass
class SensoryPercept:
    """
    Sensory percept data - combined sensory inputs for agent processing
    """
    
    def __init__(self):
        # Vision data
        self.vision_data = []
        self.vision_width = 0
        self.vision_height = 0
        self.vision_channels = 3
        
        # Audio data
        self.audio_data = []
        self.sample_rate = 0
        self.num_samples = 0
        
        # Touch/Proprioception data
        self.touch_signals = []
        self.internal_signals = []
        self.proprioception_signals = []
        
        # Timestamp
        self.timestamp = Timestamp(time.time())
    
    def add_vision(self, data: List[float], width: int, height: int, channels: int = 3) -> None:
        """Add vision data"""
        self.vision_data = data
        self.vision_width = width
        self.vision_height = height
        self.vision_channels = channels
    
    def add_audio(self, data: List[float], sample_rate: int) -> None:
        """Add audio data"""
        self.audio_data = data
        self.sample_rate = sample_rate
        self.num_samples = len(data)
    
    def add_touch(self, signals: List[float]) -> None:
        """Add touch signals"""
        self.touch_signals = signals
    
    def add_internal(self, signals: List[float]) -> None:
        """Add internal signals"""
        self.internal_signals = signals
    
    def add_proprioception(self, signals: List[float]) -> None:
        """Add proprioception signals"""
        self.proprioception_signals = signals
    
    def get_vision_dimensions(self) -> tuple:
        """Get vision dimensions"""
        return (self.vision_width, self.vision_height, self.vision_channels)
    
    def get_total_signal_count(self) -> int:
        """Get total number of signal inputs"""
        return (len(self.vision_data) + len(self.audio_data) + 
                len(self.touch_signals) + len(self.internal_signals) + 
                len(self.proprioception_signals))
    
    def get_sensory_summary(self) -> Dict[str, Any]:
        """Get summary of all sensory inputs"""
        return {
            'vision': {
                'width': self.vision_width,
                'height': self.vision_height,
                'channels': self.vision_channels,
                'data_points': len(self.vision_data)
            },
            'audio': {
                'sample_rate': self.sample_rate,
                'num_samples': self.num_samples,
                'data_points': len(self.audio_data)
            },
            'touch': len(self.touch_signals),
            'internal': len(self.internal_signals),
            'proprioception': len(self.proprioception_signals),
            'total_inputs': self.get_total_signal_count()
        }
    
    def __repr__(self) -> str:
        summary = self.get_sensory_summary()
        return (f"<SensoryPercept: vision={summary['vision']['data_points']}pts, "
                f"audio={summary['audio']['num_samples']} samples, "
                f"touch={summary['touch']}, internal={summary['internal']}, "
                f"proprioception={summary['proprioception']}>")
