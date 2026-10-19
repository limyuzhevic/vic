"""
NLM Agent Module

This module contains agent-related components including agent body, sensory percepts,
and agent brain interface. These components connect the neural network to the
environment.
"""

from .agent_body import AgentBody
from .sensory_percept import SensoryPercept
from .agent_brain import AgentBrain

__all__ = ['AgentBody', 'SensoryPercept', 'AgentBrain']
