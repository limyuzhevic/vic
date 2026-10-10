# NLM Python Package - Main Module

"""
NLM (Neural Learning Machine) Python Bindings

This module provides Python bindings for the NLM C++ neural simulation framework.
It enables users to create and simulate artificial brains using Python.

Example usage:
    import pynlm
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    for step in range(1000):
        brain.step(step)
"""

__version__ = "0.1.0"

# Import all public classes and functions from the C++ bindings
from .pynlm_core import (
    # Core types
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    
    # Enumerations
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    ActionType,
    WorldObjectType,
    
    # Main classes
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    SensoryInput,
    Action,
    WorldObject,
    Observation,
    AgentBody,
    SensoryPercept,
)

# Factory functions
def createDefaultConfig():
    """Create a default NLM configuration."""
    return Config()

def createBrain(config=None):
    """Create and initialize an NLM brain.
    
    Args:
        config: Configuration object (optional, creates default if None)
        
    Returns:
        Brain: An initialized NLM brain
    """
    if config is None:
        config = createDefaultConfig()
    return Brain(config)

def createAgentBrain(brain):
    """Create an agent brain interface for the given brain.
    
    Args:
        brain: Brain instance to interface with
        
    Returns:
        AgentBrain: An agent brain interface
    """
    return AgentBrain(brain)

def createSimpleWorld():
    """Create a simple NLM world.
    
    Returns:
        SimpleWorld: An initialized NLM world
    """
    return SimpleWorld()

# Utility functions
def listAvailableExamples():
    """List available example scripts and tutorials."""
    return [
        "minimal_example.py",
        "agent_example.py",
        "world_example.py",
        "complete_agent_simulation.py",
        "learning_experiment.py",
    ]

# Submodules (for forward compatibility)
class _NLMInternal:
    """Internal NLM implementation details."""
    pass

internal = _NLMInternal()

__all__ = [
    # Types
    "NeuronId",
    "SynapseId", 
    "RegionId",
    "PopulationId",
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    "ActionType",
    "WorldObjectType",
    
    # Main classes
    "Config",
    "Brain",
    "AgentBrain",
    "SimpleWorld",
    "SensoryInput",
    "Action",
    "WorldObject",
    "Observation",
    "AgentBody",
    "SensoryPercept",
    
    # Factory functions
    "createDefaultConfig",
    "createBrain",
    "createAgentBrain",
    "createSimpleWorld",
    
    # Utility functions
    "listAvailableExamples",
    
    # Package metadata
    "__version__",
    "internal",
]

print(f"NLM Python bindings v{__version__} loaded successfully")