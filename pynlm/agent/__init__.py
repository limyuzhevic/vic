"""
NLM Agent module for brain-world interface

This module provides the AgentBrain class which serves as the interface
between the NLM brain and the simulation world. The agent handles:
- Sensory input processing and injection into the brain
- Motor output decoding from neural activity
- Reward-modulated learning and neuromodulation
- Development and maturation processes
- Curiosity-driven exploration behavior
"""

import sys
from typing import List, Dict, Any, Optional, Union

from .agent import AgentBrain
from .sensory_percept import SensoryPercept
from .action_result import ActionResult
from .action import Action

__all__ = [
    'AgentBrain',
    'SensoryPercept', 
    'ActionResult',
    'Action'
]

# Factory function for backward compatibility
def create_agent_brain(brain):
    """Create a new agent brain interface.
    
    Args:
        brain: Brain instance to interface with
        
    Returns:
        AgentBrain: New agent brain instance
    """
    return AgentBrain(brain)

__all__.append('create_agent_brain')
