"""
NLM (Neural Learning Machine) Python Bindings

This module provides Python bindings for the NLM C++ neural simulation framework.
It enables creating and controlling artificial neural networks in Python.

Key Components:
- Brain: Central neural simulation brain class
- Config: Configuration management
- AgentBrain: Interface between brain and world
- SimpleWorld: Simple 2D world for simulation
- Various neuron types and neural components

Usage:
    import pynlm
    
    # Create a brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(1000):
        brain.step(step)
"""

__version__ = "0.1.0"

from .pynlm_core import (
    # Core classes
    NeuronId,
    SynapseId, 
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    
    # Main classes
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
)

# Factory functions
from .pynlm_core import (
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
)

# Constants
from .pynlm_core import (
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
)

# Submodules
from . import (
    core,
    agent,
    world,
    sensory,
    motor,
)

__all__ = [
    # Version
    "__version__",
    
    # Core types
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    "NeuronType", "SynapseType", "DevelopmentalStage", "FiringState",
    "ActionType", "MotorCommand", "WorldObjectType",
    
    # Main classes
    "Config", "Brain", "AgentBrain", "SimpleWorld",
    "SensoryInput", "Vision", "Audio", "InternalSignals",
    "Action", "WorldObject", "AgentBody", "ActionResult",
    "SensoryPercept",
    
    # Factory functions
    "createDefaultConfig", "createBrain", "createSimpleWorld", "createAgentBrain",
    
    # Constants
    "INVALID_NEURON_ID", "INVALID_SYNAPSE_ID", "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    
    # Submodules
    "core", "agent", "world", "sensory", "motor",
]
