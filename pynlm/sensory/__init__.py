"""
NLM Sensory module for input processing

This module provides sensory input classes and interfaces for processing
information from the environment. The sensory system handles:
- Vision (visual perception)
- Audio (auditory perception) 
- Internal signals (proprioception, interoception)
- Sensory integration and processing
- Percept generation and interpretation
"""

import sys
from typing import List, Dict, Any

from .sensory_input import SensoryInput
from .vision import Vision
from .audio import Audio
from .internal_signals import InternalSignals

__all__ = [
    'SensoryInput',
    'Vision',
    'Audio',
    'InternalSignals'
]

# Type aliases for convenience
VisionData = List[List[float]]
AudioData = List[float]
InternalData = List[float]
SignalData = List[float]

__all__.extend(['VisionData', 'AudioData', 'InternalData', 'SignalData'])
