"""
Simple world simulation module for NLM Python bindings.

Provides the SimpleWorld class for 2D environment simulation, handling
agent perception, action execution, and world state management.
"""

import pybind11
from typing import List, Optional

# Import the C++ extensions
import pynlm
from pynlm import SimpleWorld, Vision, Audio, WorldObject, Action, WorldObjectType

__all__ = ['SimpleWorld', 'Vision', 'Audio', 'WorldObject', 'WorldObjectType']
