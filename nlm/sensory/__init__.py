"""
NLM Sensory Module

This module contains sensory-related classes and functionality:
- Vision: Visual sensory input
- Audio: Audio sensory input
- InternalSignals: Internal signals
"""

# Import sensory classes from their respective modules
from .vision import Vision
from .audio import Audio
from .internal_signals import InternalSignals

__all__ = ['Vision', 'Audio', 'InternalSignals']