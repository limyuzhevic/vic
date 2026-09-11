"""
NLM Motor module for action output and control

This module provides action classes and interfaces for controlling
agent behavior in the simulation environment. The motor system handles:
- Action selection and execution
- Motor command decoding
- Action result processing
- Behavioral control and coordination
- Action history and learning
"""

import sys
from typing import List, Dict, Any

from .action import Action
from .action_result import ActionResult

__all__ = [
    'Action',
    'ActionResult'
]

# Type aliases for convenience
ActionParameters = Dict[str, Any]
ActionSequence = List['Action']

__all__.extend(['ActionParameters', 'ActionSequence'])
