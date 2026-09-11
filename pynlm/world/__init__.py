"""
NLM World module for simulation environments

This module provides the SimpleWorld class which represents the
environment in which NLM agents operate. The world handles:
- 2D spatial simulation
- Object management (resources, hazards, walls)
- Agent sensing and perception
- Action execution and feedback
- Environmental state updates
- Reward computation
"""

import sys
from typing import List, Dict, Any

from .world import SimpleWorld
from .world_object import WorldObject
from .agent_body import AgentBody

__all__ = [
    'SimpleWorld',
    'WorldObject',
    'AgentBody'
]

# Factory function for backward compatibility
def create_simple_world():
    """Create a new simple world for NLM simulation.
    
    Returns:
        SimpleWorld: New world instance
    """
    return SimpleWorld()

__all__.append('create_simple_world')
