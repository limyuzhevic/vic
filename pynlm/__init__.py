# NLM Python Bindings
# This module provides Python interface to the NLM (Neural Learning Machine) neural simulation framework

from .core import *
from .brain import *
from .agent import *
from .world import *
from .sensory import *
from .motor import *
from .environment import *

__version__ = "0.1.0"

# Module-level functions (these correspond to the create* functions in bindings.cpp)
from .bindings import createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain

# Convenience function for quick testing
__all__ = [
    'createDefaultConfig',
    'createBrain', 
    'createSimpleWorld',
    'createAgentBrain',
    '__version__'
]