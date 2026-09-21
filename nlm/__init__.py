"""NLM - Neural Learning Machine Python Package

This package provides Python bindings for the NLM (Neural Learning Machine) C++ neural simulation framework.
It enables users to create, simulate, and experiment with spiking neural networks in Python.
"""

__version__ = "0.1.0"
__author__ = "NLM Authors"

from .brain import Brain, createBrain
from .config import Config, createDefaultConfig
from .agent import AgentBrain, createAgentBrain
from .world import SimpleWorld, createSimpleWorld
from .sensory import Vision, Audio, InternalSignals, SensoryInput
from .motor import Action, MotorCommand
from .types import NeuronId, SynapseId, RegionId, PopulationId, NeuronType, SynapseType
from .enums import DevelopmentalStage, FiringState, ActionType, WorldObjectType

__all__ = [
    # Main classes
    'Brain', 'createBrain',
    'Config', 'createDefaultConfig', 
    'AgentBrain', 'createAgentBrain',
    'SimpleWorld', 'createSimpleWorld',
    'Vision', 'Audio', 'InternalSignals', 'SensoryInput',
    'Action', 'MotorCommand',
    # Types and enumerations
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType',
    'DevelopmentalStage', 'FiringState', 'ActionType', 'WorldObjectType',
    # Version info
    '__version__'
]

# Convenience functions for quick usage
def create_and_run_brain(neuron_count=1000, steps=100):
    """Create a brain and run a simple simulation.
    
    Args:
        neuron_count: Number of neurons in the brain
        steps: Number of simulation steps to run
        
    Returns:
        dict: Statistics about the simulation
    """
    config = createDefaultConfig()
    config.set("neuron_count", neuron_count)
    config.set("region_count", 1)
    config.set("connection_probability", 0.1f)
    
    brain = createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    return {
        'neurons': brain.getTotalNeuronCount(),
        'synapses': brain.getTotalSynapseCount(),
        'spikes': brain.getTotalSpikeCount(),
        'firing': brain.getFiringNeuronCount(),
        'rate': brain.getAverageFiringRate()
    }

def create_complete_agent_simulation(num_steps=1000, enable_learning=True):
    """Create a complete agent with brain, world, and sensory-motor loop.
    
    Args:
        num_steps: Number of simulation steps
        enable_learning: Whether to enable learning subsystems
        
    Returns:
        dict: Simulation results and statistics
    """
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    agent = createAgentBrain(brain)
    
    world = createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent.initialize(world)
    
    if enable_learning:
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        if enable_learning:
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
    
    return {
        'steps': num_steps,
        'neurons': brain.getTotalNeuronCount(),
        'spikes': brain.getTotalSpikeCount(),
        'firing': brain.getFiringNeuronCount(),
        'rate': brain.getAverageFiringRate(),
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel(),
        'stage': str(agent.getDevelopmentalStage()),
        'world_size': (world.getWidth(), world.getHeight())
    }

# Export exception classes
class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Exception raised for configuration errors."""
    pass

class SimulationError(NLMError):
    """Exception raised for simulation errors."""
    pass
