#!/usr/bin/env python3
"""
NLM Python package structure test module.

This module provides the Python API for NLM (Neural Learning Machine).
It serves as a test file to verify that the basic Python package structure
is correctly set up.
"""

# Import all core modules for easy access
from .config import Config, create_config_from_dict, create_config_from_json
from .brain import Brain, create_brain, demo_brain_simulation
from .agent import AgentBrain, create_agent, demo_agent_behavior
from .world import SimpleWorld, WorldObject, create_world, demo_world_environment
from .sensory import Vision, Audio, InternalSignals, SensoryInput
from .motor import Action, MotorCommand, create_action

# Package metadata
__version__ = "0.1.0"
__all__ = [
    # Core classes
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld', 'WorldObject',
    'Vision', 'Audio', 'InternalSignals', 'SensoryInput',
    'Action', 'MotorCommand',
    # Factory functions
    'create_config_from_dict', 'create_config_from_json',
    'create_brain', 'demo_brain_simulation',
    'create_agent', 'demo_agent_behavior',
    'create_world', 'demo_world_environment',
    'create_action',
]

