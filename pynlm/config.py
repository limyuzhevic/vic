"""
Configuration module for NLM Python bindings.

Provides configuration management for neural network parameters, including
settings for brain structure, plasticity, neuromodulation, and development.
"""

import pybind11
from typing import Dict, Any, Optional, List

# Import the C++ extensions
import pynlm
from pynlm import Config, createDefaultConfig

__all__ = ['Config', 'createDefaultConfig']
