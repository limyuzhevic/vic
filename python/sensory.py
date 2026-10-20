"""
Sensory NLM functionality.

This module contains sensory-related classes and operations.
"""

from typing import Dict, Any, Optional, List, Union
from ..bindings import (
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    createSimpleWorld as create_world_native,
    SensoryPercept
)

def create_vision_input(width: int = 8, height: int = 8,
                        channels: int = 3) -> Vision:
    """
    Create a vision sensory input.
    
    Args:
        width: Vision width in pixels.
        height: Vision height in pixels.
        channels: Number of color channels.
        
    Returns:
        Vision input instance.
    """
    vision = Vision(width, height, channels)
    return vision

def create_audio_input(sample_rate: int = 44100,
                       num_samples: int = 16384) -> Audio:
    """
    Create an audio sensory input.
    
    Args:
        sample_rate: Audio sample rate in Hz.
        num_samples: Number of audio samples.
        
    Returns:
        Audio input instance.
    """
    return Audio(sample_rate, num_samples)

def create_internal_signals() -> InternalSignals:
    """
    Create internal signals sensory input.
    
    Returns:
        Internal signals instance.
    """
    return InternalSignals()

def create_sensory_percept(config: Optional[Dict[str, Any]] = None) -> SensoryPercept:
    """
    Create a sensory percept with optional configuration.
    
    Args:
        config: Optional configuration dictionary.
        
    Returns:
        Sensory percept instance.
    """
    percept = SensoryPercept()
    
    if config:
        if 'vision' in config and config['vision']:
            percept.setVision(config['vision'])
        
        if 'audio' in config and config['audio']:
            percept.setAudio(config['audio'])
        
        if 'internal' in config and config['internal']:
            percept.setInternal(config['internal'])
        
        if 'touch' in config and config['touch']:
            percept.setTouch(config['touch'])
        
        if 'proprioception' in config and config['proprioception']:
            percept.setProprioception(config['proprioception'])
    
    return percept

def process_multimodal_input(percept: SensoryPercept, 
                            config: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
    """
    Process multimodal sensory input.
    
    Args:
        percept: Sensory percept to process.
        config: Optional processing configuration.
        
    Returns:
        Dictionary containing processed sensory features.
    """
    features = {}
    
    # Extract vision features
    vision = percept.getVision()
    if vision:
        features['vision'] = {
            'width': vision.getWidth(),
            'height': vision.getHeight(),
            'channels': vision.getChannels(),
            'data': vision.getData(),
        }
    
    # Extract audio features
    audio = percept.getAudio()
    if audio:
        features['audio'] = {
            'sample_rate': audio.getSampleRate(),
            'num_samples': audio.getNumSamples(),
            'data': audio.getData(),
        }
    
    # Extract internal features
    internal = percept.getInternal()
    if internal:
        features['internal'] = internal
    
    # Extract touch features
    touch = percept.getTouch()
    if touch:
        features['touch'] = touch
    
    # Extract proprioception features
    proprioception = percept.getProprioception()
    if proprioception:
        features['proprioception'] = proprioception
    
    return features

def create_test_sensory_percept() -> SensoryPercept:
    """
    Create a test sensory percept with sample data.
    
    Returns:
        Test sensory percept instance.
    """
    percept = create_sensory_percept()
    
    # Create sample vision data
    vision_data = []
    for i in range(8 * 8):
        vision_data.append(float(i) / 100.0)
    
    vision = createVisionInput(8, 8)
    vision.setData(vision_data)
    percept.setVision(vision)
    
    # Create sample internal signals
    for i in range(5):
        percept.addInternalSignal(float(i) * 0.2)
    
    return percept