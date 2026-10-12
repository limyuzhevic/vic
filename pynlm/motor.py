"""
Motor and action module for NLM Python bindings.

Provides action classes for motor commands and output generation, including
various movement types and interaction behaviors.
"""

import pybind11
from typing import List, Any, Optional

# Import the C++ extensions
import pynlm
from pynlm import Action, ActionType, MotorCommand, AgentBody, ActionResult

__all__ = ['Action', 'ActionType', 'MotorCommand', 'AgentBody', 'ActionResult']
