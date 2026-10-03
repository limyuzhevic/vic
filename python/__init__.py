#!/usr/bin/env python3
"""
NLM Python Package

This is the main entry point for the NLM Python package. It provides high-level APIs
for working with neural brains, worlds, and agents.

The NLM framework implements an artificial developmental brain with:
- Real spiking neural computation (LIF neurons)
- Event-driven spike propagation
- Multiple plasticity rules (STDP, Hebbian, structural)
- Integrated memory systems (working, episodic, associative)
- Prediction systems
- Neuromodulation (dopamine, curiosity, novelty)
- Developmental stages
- Sensorimotor control

Usage Example:
    >>> import pynlm
    >>> brain = pynlm.createBrain(pynlm.createDefaultConfig())
    >>> brain.initialize()
    >>> brain.step(0)
    >>> brain.getTotalNeuronCount()
    500
"""

import sys
from .version import __version__

# Import the low-level bindings
from .bindings import *

# Import high-level abstractions
from .brain import Brain, createBrain
from .world import World, createSimpleWorld
from .agent import Agent, createAgentBrain

__all__ = [
    # Version
    '__version__',
    
    # Core types and enums
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand',
    'WorldObjectType',
    
    # Core classes
    'Config',
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'WorldObject', 'AgentBody', 'ActionResult',
    'SensoryPercept', 'SimpleWorld',
    'Brain', 'AgentBrain',
    
    # Factory functions
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    
    # High-level abstractions
    'Brain', 'World', 'Agent',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
]

def __getattr__(name):
    """Handle lazy imports for high-level modules."""
    if name == 'Brain':
        from .brain import Brain
        return Brain
    elif name == 'World':
        from .world import World
        return World
    elif name == 'Agent':
        from .agent import Agent
        return Agent
    elif name == 'createBrain':
        from .brain import createBrain
        return createBrain
    elif name == 'createSimpleWorld':
        from .world import createSimpleWorld
        return createSimpleWorld
    elif name == 'createAgentBrain':
        from .agent import createAgentBrain
        return createAgentBrain
    elif name == 'BrainVisualization':
        from .visualization import BrainVisualization
        return BrainVisualization
    elif name == 'SimulationMonitor':
        from .monitoring import SimulationMonitor
        return SimulationMonitor
    
    raise AttributeError(f"module '{__name__}' has no attribute '{name}'")