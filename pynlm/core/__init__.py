"""
NLM Core module for Python bindings
This module provides the core NLM (Neural Learning Machine) C++ bindings
and high-level Python APIs for neural simulation.

The core module contains the main classes and factory functions for creating
and managing NLM simulations, including brains, worlds, and agents.
"""

import sys
from typing import Dict, Any, List, Optional, Union, Callable

from .config import Config
from .brain import Brain
from .agent import AgentBrain
from .world import SimpleWorld
from .sensory import Vision, Audio, InternalSignals, SensoryInput
from .motor import Action, ActionResult
from .agent import AgentBody, SensoryPercept

__all__ = [
    # Core classes
    'Config',
    'Brain',
    'AgentBrain', 
    'SimpleWorld',
    'Vision',
    'Audio',
    'InternalSignals',
    'SensoryInput',
    'Action',
    'ActionResult',
    'AgentBody',
    'SensoryPercept',
    
    # Error classes
    'NLMRuntimeError',
    'ConfigurationError',
    'SimulationError',
    'MemoryError',
    'ValidationError',
]
