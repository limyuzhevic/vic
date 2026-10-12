"""
Brain simulation module for NLM Python bindings.

Provides the main Brain class for neural network simulation with support for
sensory input, motor output, learning, development, and memory systems.
"""

import pybind11
from typing import Optional, List, Dict, Any, Union

# Import the C++ extensions
import pynlm
from pynlm import Brain, Config, createBrain, createDefaultConfig

__all__ = ['Brain', 'Config', 'createBrain', 'createDefaultConfig']
