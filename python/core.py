"""NLM Core Classes Module"""

# Core NLM classes are defined in the C++ bindings and exposed through the pybind11 module
# This file contains Python stubs and type definitions for documentation and IDE support

from typing import Dict, List, Tuple, Any, Optional
from .exceptions import NLMError, ConfigurationError, SimulationError, ValidationError
from .types import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType
)

# Forward references for type hints
class Brain:
    """Brain class - stub for type hints"""
    pass

class AgentBrain:
    """AgentBrain class - stub for type hints"""
    pass

class SimpleWorld:
    """SimpleWorld class - stub for type hints"""
    pass

class WorldObject:
    """WorldObject class - stub for type hints"""
    pass

class Vision:
    """Vision class - stub for type hints"""
    pass

class Audio:
    """Audio class - stub for type hints"""
    pass

class InternalSignals:
    """InternalSignals class - stub for type hints"""
    pass

class SensoryInput:
    """SensoryInput class - stub for type hints"""
    pass

class Action:
    """Action class - stub for type hints"""
    pass

class ActionResult:
    """ActionResult class - stub for type hints"""
    pass

class AgentBody:
    """AgentBody class - stub for type hints"""
    pass

class Config:
    """Config class - stub for type hints"""
    pass

class Environment:
    """Environment class - stub for type hints"""
    pass

class Experiment:
    """Experiment class - stub for type hints"""
    pass

class ExperimentRunner:
    """ExperimentRunner class - stub for type hints"""
    pass

class Metrics:
    """Metrics class - stub for type hints"""
    pass

# Re-export all types for convenience
__all__ = [
    # Types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Exception types
    'NLMError', 'ConfigurationError', 'SimulationError', 'ValidationError',
    
    # Core classes (forward references)
    'Brain', 'AgentBrain', 'SimpleWorld', 'WorldObject',
    'Vision', 'Audio', 'InternalSignals', 'SensoryInput',
    'Action', 'ActionResult', 'AgentBody', 'Config',
    'Environment', 'Experiment', 'ExperimentRunner', 'Metrics',
]