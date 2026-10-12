#!/usr/bin/env python3

"""
NLM Core Module - Neural Network Brain Simulation

This module provides core neural network functionality including:
- Brain simulation and management
- Configuration management
- World simulation
- Agent brains
- Sensory systems
- Action systems
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

# Re-export core classes for easy access
from nlm.core.brain import Brain
from nlm.core.config import Config
from nlm.core.world import SimpleWorld
from nlm.core.agent import AgentBrain
from nlm.core.sensory import Vision, Audio, InternalSignals
from nlm.core.action import Action, MotorCommand
from nlm.core.types import *

__all__ = [
    "Brain", "Config", "SimpleWorld", "AgentBrain",
    "Vision", "Audio", "InternalSignals", "Action", "MotorCommand",
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    "NeuronType", "SynapseType", "ActionType", "MotorCommand",
    "__version__", "__author__"
]