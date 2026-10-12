"""
Sensory input module for NLM Python bindings.

Provides sensory input classes for vision, audio, and internal signals
that feed into the neural network.
"""

import pybind11
from typing import List, Any, Optional

# Import the C++ extensions
import pynlm
from pynlm import Vision, Audio, InternalSignals, SensoryInput, SensoryPercept

__all__ = ['Vision', 'Audio', 'InternalSignals', 'SensoryInput', 'SensoryPercept']
