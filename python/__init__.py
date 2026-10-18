"""
NLM (Neural Learning Machine) Python Package

This package provides Python bindings for the NLM C++ neural simulation framework.
It exposes the core functionality for creating artificial brains, agents, and worlds.

Example usage:

    import nlm
    
    # Create a configuration
    config = nlm.createDefaultConfig()
    
    # Create a brain
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create a world
    world = nlm.createSimpleWorld()
    
    # Create an agent
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

Author: NLM Authors
License: MIT
"""

__version__ = "0.1.0"

from .core import *
from .brain import *
from .agent import *
from .world import *
from .sensory import *
from .motor import *
from .environment import *

__all__ = [
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    'NeuronType',
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    'Config',
    'SensoryInput',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    'SensoryPercept',
    'SimpleWorld',
    'Brain',
    'AgentBrain',
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
]