"""NLM Python Package

This package provides Python bindings for the NLM (Neural Learning Machine)
neural network simulation framework.

Usage:
    import pynlm
    
    # Create a basic brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
"""

__version__ = "0.1.0"

# Import core classes and functions from bindings
from .bindings import (
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
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    SimpleWorld,
    Brain,
    AgentBrain,
    
    # Factory functions
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
)

# Version info
__all__ = [
    # Version
    "__version__",
    
    # Core types
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    "NeuronType", "SynapseType", "DevelopmentalStage", "FiringState",
    "ActionType", "MotorCommand", "WorldObjectType",
    
    # Main classes
    "Config", "SensoryInput", "Vision", "Audio", "InternalSignals",
    "Action", "WorldObject", "AgentBody", "ActionResult", "SensoryPercept",
    "SimpleWorld", "Brain", "AgentBrain",
    
    # Factory functions
    "createDefaultConfig", "createBrain", "createSimpleWorld", "createAgentBrain",
]