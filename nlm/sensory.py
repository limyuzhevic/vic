"""
Sensory system classes and interfaces
"""

from typing import List, Optional, Dict, Any

class SensoryInput:
    """Base class for sensory input"""
    
    def getType(self):
        """Get the type of sensory input"""
        return None
    
    def getData(self) -> List[float]:
        """Get the raw data as a vector"""
        return []
    
    def getDimensions(self) -> int:
        """Get the dimensionality"""
        return 0
    
    def getTimestamp(self) -> float:
        """Get the timestamp"""
        return 0.0
    
    def setTimestamp(self, timestamp: float):
        """Set the timestamp"""
        pass
class Vision(SensoryInput):
    """Vision sensory input"""
    
    def __init__(self, width: int = 80, height: int = 80, channels: int = 3):
        self._width = width
        self._height = height
        self._channels = channels
        self._data = []
    
    def getType(self):
        return "vision"
    
    def setData(self, data: List[List[float]]):
        """Set data"""
        self._data = data
    
    def getData(self) -> List[List[float]]:
        """Get data"""
        return self._data
    
    def getDimensions(self) -> int:
        return self._width * self._height * self._channels
    
    def getWidth(self) -> int:
        return self._width
    
    def getHeight(self) -> int:
        return self._height
    
    def getChannels(self) -> int:
        return self._channels

    def get_center_brightness(self) -> float:
        """Get center brightness of vision"""
        if self._data:
            center_x, center_y = len(self._data) // 2, len(self._data[0]) // 2
            if center_x < len(self._data) and center_y < len(self._data[0]):
                return self._data[center_x][center_y]
        return 0.0
class Audio(SensoryInput):
    """Audio sensory input"""
    
    def __init__(self, sample_rate: int = 44100, num_samples: int = 1024):
        self._sample_rate = sample_rate
        self._num_samples = num_samples
        self._data = []
    
    def getType(self):
        return "audio"
    
    def setData(self, data: List[float]):
        """Set data"""
        self._data = data
    
    def getData(self) -> List[float]:
        """Get data"""
        return self._data
    
    def getDimensions(self) -> int:
        return len(self._data)
    
    def setSampleRate(self, sample_rate: int):
        """Set sample rate"""
        self._sample_rate = sample_rate
    
    def getSampleRate(self) -> int:
        """Get sample rate"""
        return self._sample_rate
    
    def getNumSamples(self) -> int:
        """Get number of samples"""
        return self._num_samples
class InternalSignals(SensoryInput):
    """Internal signals sensory input"""
    
    def __init__(self):
        self._signals = []
    
    def getType(self):
        return "internal"
    
    def addSignal(self, value: float):
        """Add signal"""
        self._signals.append(value)
    
    def getData(self) -> List[float]:
        """Get signals"""
        return self._signals
    
    def getDimensions(self) -> int:
        return len(self._signals)
    
    def clearSignals(self):
        """Clear signals"""
        self._signals.clear()

    def get_signal_range(self) -> Dict[str, float]:
        """Get signal statistics"""
        if not self._signals:
            return {"count": 0, "min": 0.0, "max": 0.0, "mean": 0.0}
        
        return {
            "count": len(self._signals),
            "min": min(self._signals),
            "max": max(self._signals),
            "mean": sum(self._signals) / len(self._signals),
        }
class SensoryPercept:
    """Sensory percept data"""
    
    def __init__(self):
        self._vision = None
        self._touch = None
        self._internal = None
        self._proprioception = None
        self._audio = None
        self._timestamp = 0.0
    
    def getVision(self):
        """Get vision"""
        return self._vision
    
    def setVision(self, vision: Vision):
        """Set vision"""
        self._vision = vision
    
    def getVisionWidth(self) -> int:
        """Get vision width"""
        return self._vision.getWidth() if self._vision else 0
    
    def getVisionHeight(self) -> int:
        """Get vision height"""
        return self._vision.getHeight() if self._vision else 0
    
    def getTouch(self):
        """Get touch"""
        return self._touch
    
    def setTouch(self, touch):
        """Set touch"""
        self._touch = touch
    
    def getInternal(self):
        """Get internal signals"""
        return self._internal
    
    def setInternal(self, internal: InternalSignals):
        """Set internal signals"""
        self._internal = internal
    
    def getProprioception(self):
        """Get proprioception"""
        return self._proprioception
    
    def setProprioception(self, proprioception):
        """Set proprioception"""
        self._proprioception = proprioception
    
    def getAudio(self):
        """Get audio"""
        return self._audio
    
    def setAudio(self, audio: Audio):
        """Set audio"""
        self._audio = audio
    
    def getAllSignals(self):
        """Get all signals"""
        return []
    
    def getTimestamp(self) -> float:
        """Get timestamp"""
        return self._timestamp
    
    def setTimestamp(self, timestamp: float):
        """Set timestamp"""
        self._timestamp = timestamp
    
    def get_vision_center(self) -> float:
        """Get center brightness of vision"""
        if self._vision:
            return self._vision.get_center_brightness() if hasattr(self._vision, 'get_center_brightness') else 0.0
        return 0.0
    
    def get_total_signal_strength(self) -> Dict[str, float]:
        """Get total signal strength from all modalities"""
        internal_signals = self._internal.get_signal_range() if self._internal else {"mean": 0.0}
        
        return {
            "vision_strength": self.get_vision_center(),
            "internal_strength": internal_signals["mean"],
            "total_strength": self.get_vision_center() + internal_signals["mean"],
        }
    
    def get_vision_center(self) -> float:
        """Get center brightness of vision"""
        if self._vision:
            return self._vision.get_center_brightness() if hasattr(self._vision, 'get_center_brightness') else 0.0
        return 0.0

    def get_total_signal_strength(self) -> Dict[str, float]:
        """Get total signal strength from all modalities"""
        internal_signals = self._internal.get_signal_range() if self._internal else {"mean": 0.0}
        
        return {
            "vision_strength": self.get_vision_center(),
            "internal_strength": internal_signals["mean"],
            "total_strength": self.get_vision_center() + internal_signals["mean"],
        }
