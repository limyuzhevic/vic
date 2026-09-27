"""
NLM (Neural Learning Machine) Python Bindings
================================================

A Python binding for the NLM C++ neural simulation framework.
Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.

This package provides access to the complete NLM neural simulation framework,
including working memory, episodic memory, prediction systems, cognition,
neuromodulation, and development systems.

The NLM framework simulates brain-like systems with:
- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation
- Multiple plasticity mechanisms (STDP, Hebbian, structural)
- Memory systems (working, episodic, associative)
- Prediction and cognition systems
- Neuromodulation (dopamine, curiosity, novelty)
- Developmental stages affecting plasticity
- Checkpoint save/load capabilities

Installation:
    pip install pynlm

Quick Start:
    >>> import pynlm
    >>> config = pynlm.createDefaultConfig()
    >>> brain = pynlm.createBrain(config)
    >>> brain.initialize()
    >>> brain.step(0)
    >>> print(f"Neurons: {brain.getTotalNeuronCount()}")
"""

__version__ = "0.1.0"

# Python 3.8+ type hints support
if hasattr(__builtins__, '__annotations__'):
    import typing
    if typing.TYPE_CHECKING:
        from typing import Optional, Dict, List, Any, Union, Callable

# Import all main classes from their modules
from .core import (
    Config,
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
)

from .brain import Brain
from .agent import AgentBrain
from .world import SimpleWorld
from .sensory import (
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    SensoryPercept,
)
from .motor import Action
from .world import WorldObject, ActionResult

# Factory functions
def createDefaultConfig():
    """Create a default configuration for NLM simulation.
    
    Returns:
        Config: A new configuration object with default settings.
        
    Example:
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
    """
    return Config()

def createBrain(config=None):
    """Create a new brain with the given configuration.
    
    Args:
        config: Configuration object. If None, a default config is used.
        
    Returns:
        Brain: A new NLM brain instance.
        
    Example:
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> brain.initialize()
    """
    if config is None:
        config = createDefaultConfig()
    return Brain(config)

def createSimpleWorld():
    """Create a new simple 2D world for NLM simulation.
    
    Returns:
        SimpleWorld: A new world instance.
        
    Example:
        >>> world = pynlm.createSimpleWorld()
        >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    """
    return SimpleWorld()

def createAgentBrain(brain):
    """Create an agent brain interface for connecting NLM brain to world.
    
    Args:
        brain: Brain object to connect to the agent interface.
        
    Returns:
        AgentBrain: An agent brain interface.
        
    Example:
        >>> brain = pynlm.createBrain(pynlm.createDefaultConfig())
        >>> agent = pynlm.createAgentBrain(brain)
    """
    return AgentBrain(brain)

# Invalid ID constants (used as invalid identifiers)
INVALID_NEURON_ID = NeuronId()
INVALID_SYNAPSE_ID = SynapseId()
INVALID_REGION_ID = RegionId()
INVALID_POPULATION_ID = PopulationId()

# Public API
__all__ = [
    # Version
    "__version__",
    
    # Config and Types
    "Config",
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    "NeuronType", "SynapseType", "DevelopmentalStage", "FiringState",
    "ActionType", "MotorCommand", "WorldObjectType",
    
    # Core Components
    "Brain", "AgentBrain", "SimpleWorld",
    
    # I/O Components
    "SensoryInput", "Vision", "Audio", "InternalSignals",
    "SensoryPercept", "Action", "WorldObject", "ActionResult",
    
    # Factory Functions
    "createDefaultConfig", "createBrain", "createSimpleWorld", "createAgentBrain",
    
    # Constants
    "INVALID_NEURON_ID", "INVALID_SYNAPSE_ID", "INVALID_REGION_ID", "INVALID_POPULATION_ID",
]
