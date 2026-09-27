"""
NLM (Neural Learning Machine) Python Bindings
================================================

A comprehensive Python binding for the NLM C++ neural simulation framework.

This package provides a Pythonic interface to the NLM neural simulation framework,
encompassing working memory, episodic memory, prediction systems, cognition,
neuromodulation, and development systems. The NLM framework simulates brain-like
systems with:

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

# Custom exception classes for better error handling
class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Error in configuration."""
    pass

class SimulationError(NLMError):
    """Error during simulation."""
    pass

class SerializationError(NLMError):
    """Error during file I/O."""
    pass


# Factory functions
def createDefaultConfig() -> Config:
    """Create a default configuration for NLM simulation.
    
    Returns:
        Config: A new configuration object with default settings.
        
    Example:
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
    """
    return Config()

def createBrain(config: Optional[Config] = None) -> Brain:
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

def createSimpleWorld() -> SimpleWorld:
    """Create a new simple world for NLM simulation.
    
    Returns:
        SimpleWorld: A new NLM simple world instance.
        
    Example:
        >>> world = pynlm.createSimpleWorld()
        >>> world.configure(width=20, height=20, vision_width=8, vision_height=8)
    """
    return SimpleWorld()

def createAgentBrain(brain: Brain) -> AgentBrain:
    """Create a new agent brain interface.
    
    Args:
        brain: Brain instance to wrap.
        
    Returns:
        AgentBrain: A new agent brain interface instance.
        
    Example:
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> agent = pynlm.createAgentBrain(brain)
    """
    return AgentBrain(brain)

# Re-export constants for convenience
__all__ = [
    # Core types
    "Config",
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    "NeuronType", "SynapseType", "DevelopmentalStage", "FiringState",
    "ActionType", "MotorCommand", "WorldObjectType",
    
    # Main classes
    "Brain", "AgentBrain", "SimpleWorld",
    "SensoryInput", "Vision", "Audio", "InternalSignals", "SensoryPercept",
    "Action", "WorldObject", "AgentBody", "ActionResult",
    
    # Exceptions
    "NLMError", "ConfigurationError", "SimulationError", "SerializationError",
    
    # Factory functions
    "createDefaultConfig", "createBrain", "createSimpleWorld", "createAgentBrain",
]
