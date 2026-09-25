"""
NLM Core API - Core types and classes

This module contains the core NLM classes and types that are exposed through
the main pynlm bindings.
"""

import pynlm
from typing import Dict, Any, List, Optional
from enum import Enum


class SimulationPhase(Enum):
    """Simulation phases for lifecycle management."""
    SETUP = "setup"
    RUNNING = "running"
    PAUSED = "paused"
    COMPLETED = "completed"
    TERMINATED = "terminated"


class NLMError(Exception):
    """Base exception for all NLM errors."""
    pass


class ConfigError(NLMError):
    """Configuration-related errors."""
    pass


class BrainError(NLMError):
    """Brain-related errors."""
    pass


class WorldError(NLMError):
    """World-related errors."""
    pass


# Re-export core types from pynlm
NeuronId = pynlm.NeuronId
SynapseId = pynlm.SynapseId
RegionId = pynlm.RegionId
PopulationId = pynlm.PopulationId
NeuronType = pynlm.NeuronType
SynapseType = pynlm.SynapseType
DevelopmentalStage = pynlm.DevelopmentalStage
FiringState = pynlm.FiringState
ActionType = pynlm.ActionType
MotorCommand = pynlm.MotorCommand
WorldObjectType = pynlm.WorldObjectType

# Re-export core classes
Config = pynlm.Config
Brain = pynlm.Brain
AgentBrain = pynlm.AgentBrain
SimpleWorld = pynlm.SimpleWorld
SensoryInput = pynlm.SensoryInput
Vision = pynlm.Vision
Audio = pynlm.Audio
InternalSignals = pynlm.InternalSignals
Action = pynlm.Action
WorldObject = pynlm.WorldObject
AgentBody = pynlm.AgentBody
ActionResult = pynlm.ActionResult
SensoryPercept = pynlm.SensoryPercept

# Package metadata
__version__ = "0.1.0"
__all__ = [
    # Types
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    "NeuronType", "SynapseType", "DevelopmentalStage", "FiringState",
    "ActionType", "MotorCommand", "WorldObjectType",
    
    # Classes
    "Config", "Brain", "AgentBrain", "SimpleWorld",
    "SensoryInput", "Vision", "Audio", "InternalSignals",
    "Action", "WorldObject", "AgentBody", "ActionResult",
    "SensoryPercept",
    
    # Exceptions
    "NLMError", "ConfigError", "BrainError", "WorldError",
    
    # Enum
    "SimulationPhase",
]