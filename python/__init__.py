"""
NLM (Neural Learning Machine) Python Bindings

An experimental artificial brain simulation system with:
- Spiking neural networks
- Memory systems (working, episodic, associative)
- Prediction and cognitive systems
- Neuromodulation (dopamine, curiosity, novelty)
- Developmental stages
- Event-driven simulation

This package provides access to the NLM C++ neural simulation framework
through comprehensive Python bindings, enabling research and development
of artificial cognitive systems.
"""

import sys
import os

# Add the current directory to Python path for local development
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import core classes from the C++ bindings
from .core import Config, Brain, Neuron, Synapse
from .memory import WorkingMemory, EpisodicMemory, AssociativeMemory
from .prediction import PredictionSystem
from .cognition import NeuralPlanner, ConceptFormation, AttentionalSelection
from .neuromodulation import Dopamine, Curiosity, Novelty, PredictionError
from .sensory import Vision, Audio, InternalSignals
from .motor import Action
from .world import SimpleWorld, WorldObject, AgentBody
from .agent import AgentBrain, SensoryPercept, ActionResult

# Version information
__version__ = "0.1.0"
__author__ = "NLM Research Project"
__email__ = "research@nlm.org"

# Factory functions for easy usage
def createDefaultConfig():
    """Create a default configuration object.
    
    Returns:
        Config: A default configuration object with sensible defaults.
        
    Example:
        >>> config = createDefaultConfig()
        >>> config.set("neuron_count", 1000)
    """
    return Config()


def createBrain(config=None):
    """Create a new brain with the specified configuration.
    
    Args:
        config: Configuration object. If None, uses default config.
        
    Returns:
        Brain: A new brain object ready for simulation.
        
    Example:
        >>> brain = createBrain()
        >>> brain.initialize()
    """
    if config is None:
        config = createDefaultConfig()
    return Brain(config)


def createSimpleWorld():
    """Create a simple 2D world for simulation.
    
    Returns:
        SimpleWorld: A new world object for agent-environment interaction.
        
    Example:
        >>> world = createSimpleWorld()
        >>> world.configure(width=20, height=20)
    """
    return SimpleWorld()


def createAgentBrain(brain):
    """Create an agent brain interface connecting brain to world.
    
    Args:
        brain: Brain object to interface with.
        
    Returns:
        AgentBrain: An agent brain object for simulation control.
        
    Example:
        >>> brain = createBrain()
        >>> agent = createAgentBrain(brain)
    """
    return AgentBrain(brain)

# Import commonly used types and enums
from .core import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage,
    FiringState, ActionType, MotorCommand,
    WorldObjectType, MembranePotential, SimulationStep,
    Timestamp, Delay
)

# Create a namespace for convenience functions
class _NLM:
    """Internal namespace for NLM convenience functions."""
    
    @staticmethod
    def createConfig():
        return createDefaultConfig()
    
    @staticmethod
    def createBrain(config=None):
        return createBrain(config)
    
    @staticmethod
    def createWorld():
        return createSimpleWorld()
    
    @staticmethod
    def createAgent(brain):
        return createAgentBrain(brain)

# Alias for convenience
NLM = _NLM

# Documentation
__doc__ = """
NLM Python API
===============

This is the main package for the NLM (Neural Learning Machine) Python bindings.
It provides access to a complete artificial brain simulation system.

Quick Start:
------------

Create a basic brain simulation:

>>> from pynlm import createBrain, createSimpleWorld, createAgentBrain
>>> 
>>> brain = createBrain()
>>> brain.initialize()
>>> 
>>> world = createSimpleWorld()
>>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
>>> 
>>> agent = createAgentBrain(brain)
>>> agent.initialize(world)
>>> 
>>> # Run simulation
>>> for i in range(100):
...     brain.step(i)
...
>>> 
>>> print(f"Total spikes: {brain.getTotalSpikeCount()}")

Complete Agent in World:
--------------------------

>>> from pynlm import createBrain, createSimpleWorld, createAgentBrain
>>> 
>>> brain = createBrain()
>>> brain.initialize()
>>> 
>>> world = createSimpleWorld()
>>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
>>> world.reset()
>>> 
>>> agent = createAgentBrain(brain)
>>> agent.initialize(world)
>>> agent.enableRewardModulation(True)
>>> agent.enableCuriosity(True)
>>> 
>>> for step in range(100):
...     world.update(0.1)
...     percept = world.getSensoryPercept()
...     agent.processSensoryInput(percept)
...     brain.step(step)
...     action = agent.decodeMotorCommand()
...     world.applyMotorCommand(action, world.getSimulationTime())
... 
>>> print("Agent simulation complete!")

For complete documentation, see the main documentation file:
https://github.com/nlm-project/nlm/blob/main/python/nlm_documentation.md
"""

# Export all public classes and functions
__all__ = [
    # Core classes
    'Config', 'Brain', 'Neuron', 'Synapse',
    
    # Memory systems
    'WorkingMemory', 'EpisodicMemory', 'AssociativeMemory',
    
    # Prediction system
    'PredictionSystem',
    
    # Cognitive systems
    'NeuralPlanner', 'ConceptFormation', 'AttentionalSelection',
    
    # Neuromodulation
    'Dopamine', 'Curiosity', 'Novelty', 'PredictionError',
    
    # Sensory and motor
    'Vision', 'Audio', 'InternalSignals',
    'Action',
    
    # World and agent
    'SimpleWorld', 'WorldObject', 'AgentBody',
    'AgentBrain', 'SensoryPercept', 'ActionResult',
    
    # Types and enums
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand',
    'WorldObjectType', 'MembranePotential', 'SimulationStep',
    'Timestamp', 'Delay',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    'NLM',
    
    # Version
    '__version__',
]
