# NLM Python bindings package
# This module provides Python access to the NLM C++ neural simulation framework

from .pynlm import (
    # Core classes
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
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
    
    # System classes
    SimpleWorld,
    Brain,
    AgentBrain,
    
    # Enumerations
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    
    # Factory functions
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
)

# Package version
__version__ = "0.1.0"

# Package metadata
__author__ = "NLM Project"
__description__ = "Python bindings for NLM (Neural Learning Machine) neural simulation framework"
__license__ = "MIT"

# Module docstring
__doc__ = """
NLM (熙然) - Neural Learning Machine Python Bindings
====================================================

A Python binding for the NLM C++ neural simulation framework.

This package provides access to:

* Brain simulation with LIF neurons and event-driven spike propagation
* Multiple memory systems (working, episodic, associative)
* Neuromodulation (dopamine, curiosity, novelty)
* Prediction and cognition systems
* Development and learning mechanisms
* Embodied agent simulation

Quick start:

>>> import pynlm
>>> config = pynlm.createDefaultConfig()
>>> brain = pynlm.createBrain(config)
>>> brain.initialize()
>>> for i in range(100):
...     brain.step(i)
... 
>>> print(f"Total spikes: {brain.getTotalSpikeCount()}")

See HOW_TO_USE.md for comprehensive documentation.
"""