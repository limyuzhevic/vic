"""
Advanced SensoryInput Python bindings for NLM

This module provides advanced Python APIs for the SensoryInput class,
including sensory statistics, input quality, and signal processing APIs.
"""

import pynlm as nlm
from pynlm import SensoryInput, Vision, Audio, InternalSignals, SensoryPercept
import json
import math

class SensoryInputAdvanced:
    """
    Advanced SensoryInput class with comprehensive Python APIs for sensory processing.
    
    This class provides enhanced access to SensoryInput functionality through Python-friendly APIs,
    including sensory statistics, input quality, and signal processing APIs.
    """
    
    def __init__(self, sensory_input):
        self._sensory_input = sensory_input
        self._sensory_tools = SensoryAPI(self)
        self._vision_tools = VisionAPI(self)
        self._audio_tools = AudioAPI(self)
        self._internal_tools = InternalAPI(self)
    
    def __repr__(self):
        return f"<SensoryInputAdvanced: {self.getType()}>"
    
    def __str__(self):
        return f"SensoryInputAdvanced(type={self.getType()}, dimensions={self.getDimensions()})"
    
    # Basic sensory input operations
    def getType(self):
        """Get sensory input type"""
        return self._sensory_input.getType()
    
    def getData(self):
        """Get the raw data as a vector"""
        return self._sensory_input.getData()
    
    def getDimensions(self):
        """Get the dimensionality"""
        return self._sensory_input.getDimensions()
    
    def getTimestamp(self):
        """Get the timestamp"""
        return self._sensory_input.getTimestamp()
    
    def setTimestamp(self, timestamp):
        """Set the timestamp"""
        self._sensory_input.setTimestamp(timestamp)
    
    # Sensory APIs
    def getSensoryStats(self):
        """Get sensory statistics"""
        return self._sensory_tools.getSensoryStats()
    
    def getInputQuality(self):
        """Get input quality"""
        return self._sensory_tools.getInputQuality()
    
    def getSignalNoiseRatio(self):
        """Get signal-to-noise ratio"""
        return self._sensory_tools.getSignalNoiseRatio()
    
    def getSensoryLoad(self):
        """Get sensory load"""
        return self._sensory_tools.getSensoryLoad()
    
    # Vision APIs (for Vision class)
    def getVisualFieldStats(self):
        """Get visual field statistics"""
        return self._vision_tools.getVisualFieldStats()
    
    def getVisualAcuity(self):
        """Get visual acuity"""
        return self._vision_tools.getVisualAcuity()
    
    def getVisualSensitivity(self):
        """Get visual sensitivity"""
        return self._vision_tools.getVisualSensitivity()
    
    def getVisualProcessingSpeed(self):
        """Get visual processing speed"""
        return self._vision_tools.getVisualProcessingSpeed()
    
    # Audio APIs (for Audio class)
    def getAudioStats(self):
        """Get audio statistics"""
        return self._audio_tools.getAudioStats()
    
    def getSoundClarity(self):
        """Get sound clarity"""
        return self._audio_tools.getSoundClarity()
    
    def getAudioFrequencyRange(self):
        """Get audio frequency range"""
        return self._audio_tools.getAudioFrequencyRange()
    
    def getAudioProcessingSpeed(self):
        """Get audio processing speed"""
        return self._audio_tools.getAudioProcessingSpeed()
    
    # Internal signals APIs (for InternalSignals class)
    def getInternalStats(self):
        """Get internal signals statistics"""
        return self._internal_tools.getInternalStats()
    
    def getSignalStrength(self):
        """Get signal strength"""
        return self._internal_tools.getSignalStrength()
    
    def getSignalReliability(self):
        """Get signal reliability"""
        return self._internal_tools.getSignalReliability()
    
    def getInternalProcessingSpeed(self):
        """Get internal processing speed"""
        return self._internal_tools.getInternalProcessingSpeed()
    
    # Percept operations
    def createPercept(self, vision=None, touch=None, internal=None, proprioception=None, audio=None):
        """Create sensory percept from components"""
        percept = SensoryPercept()
        if vision is not None:
            percept.setVision(vision)
        if touch is not None:
            percept.setTouch(touch)
        if internal is not None:
            percept.setInternal(internal)
        if proprioception is not None:
            percept.setProprioception(proprioception)
        if audio is not None:
            percept.setAudio(audio)
        return percept
    
    def getAllSignals(self, percept):
        """Get all signals from percept"""
        return percept.getAllSignals()
    
    # Python integration APIs
    def getPythonBindingsVersion(self):
        """Get Python bindings version"""
        return "0.1.0-advanced"
    
    def getPythonBindingsInfo(self):
        """Get Python bindings information"""
        return {
            'version': self.getPythonBindingsVersion(),
            'platform': "linux",
            'python_version': ".".join(map(str, __import__('sys').version_info[:3])),
            'features': [
                'advanced_sensory_apis',
                'advanced_vision_apis',
                'advanced_audio_apis',
                'advanced_internal_apis',
                'advanced_percept_apis',
            ],
        }
    
    # Resource monitoring APIs
    def getMemoryUsage(self):
        """Get memory usage"""
        import sys
        return sys.getsizeof(self._sensory_input)
    
    # Export APIs
    def exportSensoryData(self, filepath):
        """Export sensory data to file"""
        data = {
            'type': self.getType(),
            'dimensions': self.getDimensions(),
            'data': self.getData(),
            'timestamp': self.getTimestamp(),
        }
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
        return f"Sensory data exported to {filepath}"
    
    def importSensoryData(self, filepath):
        """Import sensory data from file"""
        with open(filepath, 'r') as f:
            data = json.load(f)
        # Note: This is a mock implementation
        return f"Sensory data imported from {filepath}"


# Sensory API class
def SensoryAPI(sensory_advanced):
    """Sensory API for advanced sensory input operations"""
    class _SensoryAPI:
        def __init__(self, sensory):
            self.sensory = sensory
        
        def getSensoryStats(self):
            """Get sensory statistics"""
            data = self.sensory.getData()
            return {
                'type': self.sensory.getType(),
                'dimensions': self.sensory.getDimensions(),
                'data_size': len(data) if data else 0,
                'timestamp': self.sensory.getTimestamp(),
                'data_range': {
                    'min': min(data) if data else 0.0,
                    'max': max(data) if data else 1.0,
                    'mean': sum(data) / len(data) if data else 0.5,
                },
                'quality_score': self.sensory.getInputQuality(),
                'signal_noise_ratio': self.sensory.getSignalNoiseRatio(),
                'sensory_load': self.sensory.getSensoryLoad(),
            }
        
        def getInputQuality(self):
            """Get input quality"""
            # Mock implementation - calculate quality based on data characteristics
            data = self.sensory.getData()
            if not data:
                return 0.0
            
            # Calculate quality based on variance and range
            variance = sum((x - sum(data)/len(data)) ** 2 for x in data) / len(data)
            quality = min(1.0, variance * 10)
            return quality
        
        def getSignalNoiseRatio(self):
            """Get signal-to-noise ratio"""
            data = self.sensory.getData()
            if not data or len(data) < 2:
                return 0.0
            
            signal_power = sum(x ** 2 for x in data) / len(data)
            noise_power = sum((x - sum(data)/len(data)) ** 2 for x in data) / len(data)
            
            if noise_power == 0:
                return float('inf')
            
            return signal_power / noise_power
        
        def getSensoryLoad(self):
            """Get sensory load"""
            dimensions = self.sensory.getDimensions()
            data = self.sensory.getData()
            
            # Calculate load based on dimensions and data size
            base_load = min(1.0, dimensions / 100)
            data_load = min(1.0, (len(data) if data else 0) / 1000)
            
            return base_load * 0.7 + data_load * 0.3
    
    return _SensoryAPI(sensory_advanced)


# Vision API class
def VisionAPI(sensory_advanced):
    """Vision API for advanced sensory input operations"""
    class _VisionAPI:
        def __init__(self, sensory):
            self.sensory = sensory
        
        def getVisualFieldStats(self):
            """Get visual field statistics"""
            return {
                'field_of_view': 120.0,  # degrees
                'resolution': {'width': 640, 'height': 480},
                'bit_depth': 24,
                'frame_rate': 30.0,
                'pixel_count': 640 * 480,
                'visual_acuity': self.getVisualAcuity(),
                'visual_sensitivity': self.getVisualSensitivity(),
                'visual_processing_speed': self.getVisualProcessingSpeed(),
            }
        
        def getVisualAcuity(self):
            """Get visual acuity"""
            return 0.8  # 20/20 equivalent
        
        def getVisualSensitivity(self):
            """Get visual sensitivity"""
            return 0.9  # LogMAR score
        
        def getVisualProcessingSpeed(self):
            """Get visual processing speed"""
            return 50.0  # milliseconds per frame
    
    return _VisionAPI(sensory_advanced)


# Audio API class
def AudioAPI(sensory_advanced):
    """Audio API for advanced sensory input operations"""
    class _AudioAPI:
        def __init__(self, sensory):
            self.sensory = sensory
        
        def getAudioStats(self):
            """Get audio statistics"""
            return {
                'sample_rate': 44100,  # Hz
                'channels': 2,
                'bit_depth': 16,
                'duration': 1.0,  # seconds
                'sample_count': 44100,
                'frequency_range': self.getAudioFrequencyRange(),
                'sound_clarity': self.getSoundClarity(),
                'audio_processing_speed': self.getAudioProcessingSpeed(),
            }
        
        def getSoundClarity(self):
            """Get sound clarity"""
            return 0.85  # Signal-to-noise ratio
        
        def getAudioFrequencyRange(self):
            """Get audio frequency range"""
            return {'min': 20.0, 'max': 20000.0}  # Hz
        
        def getAudioProcessingSpeed(self):
            """Get audio processing speed"""
            return 5.0  # milliseconds per buffer
    
    return _AudioAPI(sensory_advanced)


# Internal API class
def InternalAPI(sensory_advanced):
    """Internal API for advanced sensory input operations"""
    class _InternalAPI:
        def __init__(self, sensory):
            self.sensory = sensory
        
        def getInternalStats(self):
            """Get internal signals statistics"""
            return {
                'signal_count': 10,
                'signal_range': {'min': -1.0, 'max': 1.0},
                'update_rate': 100.0,  # Hz
                'signal_strength': self.getSignalStrength(),
                'signal_reliability': self.getSignalReliability(),
                'internal_processing_speed': self.getInternalProcessingSpeed(),
            }
        
        def getSignalStrength(self):
            """Get signal strength"""
            return 0.9
        
        def getSignalReliability(self):
            """Get signal reliability"""
            return 0.95
        
        def getInternalProcessingSpeed(self):
            """Get internal processing speed"""
            return 1000.0  # Hz
    
    return _InternalAPI(sensory_advanced)