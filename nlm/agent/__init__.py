"""
NLM Agent Module

This module contains agent-related classes and functionality:
- AgentBrain: Interface between brain and world
- AgentBody: Agent state representation
- SensoryPercept: Sensory input representation
"""

# Import agent classes from their respective modules
from .agent_brain import AgentBrain
from .agent_body import AgentBody
from .sensory_percept import SensoryPercept

__all__ = ['AgentBrain', 'AgentBody', 'SensoryPercept']