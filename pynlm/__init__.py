"""
NLM (Neural Learning Machine) Python Bindings

A Python binding for the NLM C++ neural simulation framework.

This package provides a Python interface to the computational brain simulation
through the pynlm module. It exposes neural components, world simulation,
memory systems, and learning mechanisms.

Key Features:
- Neural brain simulation with spiking neurons
- World-environment interaction
- Memory systems (working, episodic, semantic, procedural)
- Neuromodulation (dopamine, curiosity, novelty)
- Plasticity rules (STDP, Hebbian, structural)
- Developmental stages
- Predictive planning and concept formation

Quick Start:
    import pynlm

    # Create and initialize a brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()

    # Run simulation
    for step in range(100):
        brain.step(step)

    # Get statistics
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    print(f"Spikes: {brain.getTotalSpikeCount()}")

Complete API Reference:
- See HOW_TO_USE.md for detailed documentation
- See easy_usage.md for beginner tutorials
- See docs/ARCHITECTURE.md for system overview
- See docs/SCIENCE.md for scientific background

Module Contents:
    __version__: Version of pynlm module
    createDefaultConfig: Create default configuration
    createBrain: Create neural brain instance
    createSimpleWorld: Create world simulation
    createAgentBrain: Create agent interface
    Vision, Audio, InternalSignals: Sensory input classes
    Action, WorldObject, AgentBody: Output/state classes
    various enums for NeuronType, ActionType, etc.
"""

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"

# Import core classes for easier access
from pynlm import (
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    Vision,
    Audio,
    InternalSignals,
    SensoryInput,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    # Enumerations
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
)

# Import helper functions
def createDefaultConfig():
    """Create a default configuration object for NLM."""
    from pynlm import Config
    return Config()

def createBrain(config):
    """Create a new neural brain instance with the given configuration."""
    from pynlm import Brain
    return Brain(config)

def createSimpleWorld():
    """Create a new simple 2D world simulation."""
    from pynlm import SimpleWorld
    return SimpleWorld()

def createAgentBrain(brain):
    """Create an agent brain interface for the given brain."""
    from pynlm import AgentBrain
    return AgentBrain(brain)

# Common constants for convenience
INVALID_NEURON_ID = -1
INVALID_SYNAPSE_ID = -1
INVALID_REGION_ID = -1
INVALID_POPULATION_ID = -1
