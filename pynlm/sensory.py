"""
Sensory input modules for NLM
"""

import time
from typing import List, Optional
from dataclasses import dataclass
from .types import Timestamp, SensoryInput, Vision, Audio, InternalSignals, SensoryPercept

@dataclass
class Vision(Vision):
    """Vision sensory input - camera-like visual processing"""
    
    def __init__(self, width: int = 0, height: int = 0, channels: int = 3):
        super().__init__()
        self.width = width
        self.height = height
        self.channels = channels
        self.data = []
        self.timestamp = Timestamp(time.time())
    
    def setData(self, data: List[float]) -> None:
        """Set raw pixel data"""
        self.data = data
    
    def getData(self) -> List[float]:
        """Get the raw data as a vector"""
        return self.data
    
    def getWidth(self) -> int:
        """Get the width of the image"""
        return self.width
    
    def getHeight(self) -> int:
        """Get the height of the image"""
        return self.height
    
    def getChannels(self) -> int:
        """Get the number of color channels"""
        return self.channels
    
    def getDimensions(self) -> List[int]:
        """Get the dimensionality of the visual input"""
        return [self.width, self.height, self.channels]
    
    def __repr__(self) -> str:
        return f"<Vision: {self.width}x{self.height}x{self.channels}>"


@dataclass
class Audio(Audio):
    """Audio sensory input - sound processing"""
    
    def __init__(self, sample_rate: int = 0, num_samples: int = 0):
        super().__init__()
        self.sample_rate = sample_rate
        self.num_samples = num_samples
        self.data = []
        self.timestamp = Timestamp(time.time())
    
    def setData(self, data: List[float]) -> None:
        """Set audio sample data"""
        self.data = data
        self.num_samples = len(data)
    
    def getData(self) -> List[float]:
        """Get the raw data as a vector"""
        return self.data
    
    def setSampleRate(self, sample_rate: int) -> None:
        """Set sample rate"""
        self.sample_rate = sample_rate
    
    def getSampleRate(self) -> int:
        """Get sample rate"""
        return self.sample_rate
    
    def getNumSamples(self) -> int:
        """Get number of samples"""
        return self.num_samples
    
    def getDimensions(self) -> List[int]:
        """Get the dimensionality of the audio input"""
        return [self.num_samples]
    
    def __repr__(self) -> str:
        return f"<Audio: {self.num_samples} samples @ {self.sample_rate} Hz>"


@dataclass
class InternalSignals(InternalSignals):
    """Internal signals sensory input - proprioception and internal state"""
    
    def __init__(self):
        super().__init__()
        self.signals = []
        self.timestamp = Timestamp(time.time())
    
    def getType(self) -> str:
        """Get the type of sensory input"""
        return "InternalSignals"
    
    def getData(self) -> List[float]:
        """Get the raw data as a vector"""
        return self.signals
    
    def getDimensions(self) -> List[int]:
        """Get the dimensionality"""
        return [len(self.signals)]
    
    def getTimestamp(self) -> Timestamp:
        """Get the timestamp"""
        return self.timestamp
    
    def setTimestamp(self, timestamp: Timestamp) -> None:
        """Set the timestamp"""
        self.timestamp = timestamp
    
    def addSignal(self, value: float) -> None:
        """Add a signal value"""
        self.signals.append(value)
    
    def clearSignals(self) -> None:
        """Clear all signals"""
        self.signals.clear()
    
    def __repr__(self) -> str:
        return f"<InternalSignals: {len(self.signals)} signals>"


@dataclass
class SensoryPercept(SensoryPercept):
    """Sensory percept data - combined sensory inputs"""
    
    def __init__(self):
        super().__init__()
        self.vision = Vision()
        self.touch = InternalSignals()
        self.internal = InternalSignals()
        self.proprioception = InternalSignals()
        self.audio = Audio()
        self.timestamp = Timestamp(time.time())
    
    def getVision(self) -> Vision:
        """Get vision data"""
        return self.vision
    
    def setVision(self, vision: Vision) -> None:
        """Set vision data"""
        self.vision = vision
    
    def getVisionWidth(self) -> int:
        """Get vision width"""
        return self.vision.getWidth()
    
    def getVisionHeight(self) -> int:
        """Get vision height"""
        return self.vision.getHeight()
    
    def getTouch(self) -> InternalSignals:
        """Get touch data"""
        return self.touch
    
    def setTouch(self, touch: InternalSignals) -> None:
        """Set touch data"""
        self.touch = touch
    
    def getInternal(self) -> InternalSignals:
        """Get internal signals"""
        return self.internal
    
    def setInternal(self, internal: InternalSignals) -> None:
        """Set internal signals"""
        self.internal = internal
    
    def getProprioception(self) -> InternalSignals:
        """Get proprioception data"""
        return self.proprioception
    
    def setProprioception(self, proprioception: InternalSignals) -> None:
        """Set proprioception data"""
        self.proprioception = proprioception
    
    def getAudio(self) -> Audio:
        """Get audio data"""
        return self.audio
    
    def setAudio(self, audio: Audio) -> None:
        """Set audio data"""
        self.audio = audio
    
    def getAllSignals(self) -> Dict[str, Any]:
        """Get all sensory signals"""
        return {
            'vision': self.vision.getData(),
            'touch': self.touch.getData(),
            'internal': self.internal.getData(),
            'proprioception': self.proprioception.getData(),
            'audio': self.audio.getData()
        }
    
    def getTimestamp(self) -> Timestamp:
        """Get timestamp"""
        return self.timestamp
    
    def setTimestamp(self, timestamp: Timestamp) -> None:
        """Set timestamp"""
        self.timestamp = timestamp
    
    def __repr__(self) -> str:
        return f"<SensoryPercept: vision={self.vision.getDimensions()}, audio={self.audio.getNumSamples()} samples>"
