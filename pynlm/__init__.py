"""
NLM (Neural Learning Machine) Python Package

A Python binding for the NLM C++ neural simulation framework.

This package provides access to:
- Brain simulation with LIF neurons
- World simulation for agent-environment interaction  
- Agent brain interface
- Sensory and motor systems
- Learning mechanisms (STDP, Hebbian, neuromodulation)
- Memory systems (working, episodic)
- Development and maturation
- Checkpoint persistence

For usage examples, see:
- easy_usage.md - Beginner-friendly guide
- HOW_TO_USE.md - Comprehensive documentation
"""

import sys
import os

# Add version to package
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine)"

# Import all the pybind11 exposed classes and functions
# These are exposed via the pynlm module built by scikit-build-core

try:
    # Core classes
    from .pynlm_core import (
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
    
    # Configuration
    from .pynlm_core import Config, createDefaultConfig
    
    # Sensory systems
    from .pynlm_core import (
        SensoryInput,
        Vision,
        Audio,
        InternalSignals,
        SensoryPercept,
    )
    
    # Actions and motor
    from .pynlm_core import Action, ActionResult, WorldObject
    
    # Brain components
    from .pynlm_core import AgentBody, AgentBrain
    
    # World simulation
    from .pynlm_core import SimpleWorld
    
    # Core brain
    from .pynlm_core import Brain, createBrain
    
    # Factory functions
    from .pynlm_core import createSimpleWorld, createAgentBrain
    
    # Constants
    from .pynlm_core import (
        INVALID_NEURON_ID,
        INVALID_SYNAPSE_ID,
        INVALID_REGION_ID,
        INVALID_POPULATION_ID,
    )
    
except ImportError as e:
    # If the native module isn't available, provide helpful error
    def __getattr__(name):
        if name in (
            'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
            'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
            'ActionType', 'MotorCommand', 'WorldObjectType', 'Config', 'Vision',
            'Audio', 'InternalSignals', 'Action', 'ActionResult', 'WorldObject',
            'AgentBody', 'SensoryPercept', 'SimpleWorld', 'Brain', 'AgentBrain',
            'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
        ):
            raise ImportError(
                f"The NLM Python bindings are not available. "
                f"Please build and install the package. "
                f"Run 'pip install -e .' or 'pip install .' from the project root. "
                f"Original error: {e}"
            )
        raise AttributeError(f"module '{__name__}' has no attribute '{name}'")

# Convenience functions for common use cases
def run_simple_simulation(neuron_count=1000, steps=100, timestep=1.0):
    """
    Run a simple simulation with default configuration.
    
    This is a convenience function for getting started with NLM.
    
    Args:
        neuron_count: Number of neurons in the brain
        steps: Number of simulation steps to run
        timestep: Time step size for simulation
        
    Returns:
        dict: Statistics about the simulation run
    """
    config = createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    
    brain = createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    return {
        'neurons': brain.getTotalNeuronCount(),
        'spikes': brain.getTotalSpikeCount(),
        'firing': brain.getFiringNeuronCount(),
        'rate': brain.getAverageFiringRate(),
        'steps': steps,
    }
def create_agent_environment(width=20, height=20, vision_width=8, vision_height=8):
    """
    Create a basic agent-environment setup.
    
    This is a convenience function for creating a complete agent setup
    with a brain, world, and agent interface.
    
    Args:
        width: World width in cells
        height: World height in cells
        vision_width: Agent vision width
        vision_height: Agent vision height
        
    Returns:
        dict: Contains brain, world, and agent objects
    """
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
    }
def help_getting_started():
    """
    Print a getting-started guide for NLM.
    
    This provides a quick reference for getting started with NLM.
    """
    guide = """
NLM Getting Started Guide
=========================

1. Create a brain:
   config = pynlm.createDefaultConfig()
   brain = pynlm.createBrain(config)
   brain.initialize()

2. Run simulation:
   for step in range(100):
       brain.step(step)

3. Create agent environment:
   world = pynlm.createSimpleWorld()
   world.configure(20, 20, 8, 8)
   agent = pynlm.createAgentBrain(brain)
   agent.initialize(world)

4. Use agent in world:
   for step in range(100):
       world.update(0.1)
       percept = world.getSensoryPercept()
       agent.processSensoryInput(percept)
       brain.step(step)
       action = agent.decodeMotorCommand()
       world.applyMotorCommand(action, world.getSimulationTime())

5. Enable learning (optional):
   agent.enableRewardModulation(True)
   agent.enableStructuralPlasticity(True)
   agent.enableDevelopment(True)
   agent.enableCuriosity(True)
    """
    print(guide)
# Export all public API functions
__all__ = [
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Configuration
    'Config', 'createDefaultConfig',
    
    # Sensory
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals', 'SensoryPercept',
    
    # Actions
    'Action', 'ActionResult', 'WorldObject',
    
    # Brain components
    'AgentBody', 'SimpleWorld', 'Brain', 'AgentBrain',
    
    # Factory functions
    'createBrain', 'createSimpleWorld', 'createAgentBrain',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Convenience functions
    'run_simple_simulation', 'create_agent_environment', 'help_getting_started',
]

# Package metadata
__license__ = "MIT"
__copyright__ = "Copyright (c) 2026 NLM Authors"
