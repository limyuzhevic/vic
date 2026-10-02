try:
    import pynlm
    __version__ = pynlm.__version__
except ImportError:
    __version__ = "unknown"

# Core functionality
from .bindings import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType
)

# Main classes
from .bindings import (
    Config, SensoryInput, Vision, Audio, InternalSignals, Action,
    WorldObject, AgentBody, ActionResult, SensoryPercept,
    SimpleWorld, Brain, AgentBrain
)

# Factory functions
from .bindings import (
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain
)

# Error classes
from .bindings import RuntimeError

# Constants
__all__ = [
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    'Config', 'SensoryInput', 'Vision', 'Audio', 'InternalSignals', 
    'Action', 'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept',
    'SimpleWorld', 'Brain', 'AgentBrain',
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    'RuntimeError', 'version'
]

# Configure docstring
__doc__ = """
NLM (Neural Learning Machine) Python Bindings
============================================

This package provides Python bindings for the NLM neural simulation framework.
It enables creating and simulating artificial neural networks with advanced
learning mechanisms.

Main components:
- Brain: Central neural simulation brain class
- SimpleWorld: 2D world for NLM simulation  
- AgentBrain: Interface connecting brain to world
- Config: Configuration class for NLM system
- Various sensory, motor, and memory systems

Basic usage:
    import pynlm
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    for step in range(1000):
        brain.step(step)

For more information, see documentation in the main project.
"""