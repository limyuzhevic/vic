"""
NLM Python Package

This package provides Python bindings for the NLM (熙然) brain simulator.
The NLM is an experimental artificial developmental brain that learns,
remembers, and makes decisions through neural computation.
"""

from .pynlm import *

__all__ = [
    # Configuration
    'Config',
    # Neural Components
    'NeuronType', 'DevelopmentalStage', 'ActionType', 'WorldObjectType',
    # Brain Components
    'SensoryPercept', 'Action', 'WorldObject', 'World', 'Brain', 'AgentBrain',
    # Core API
    'create_default_config', 'create_brain', 'create_simple_world', 'create_agent_brain',
    # Version
    '__version__',
]

__version__ = "1.0.0"