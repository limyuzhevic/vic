# NLM Python Bindings Package
# This package provides Python access to the NLM (Neural Learning Machine) neural simulation framework

__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Import all public API from submodules
from .brain import Brain, Config, Neuron, Synapse, NeuralRegion, Action, ActionTypes
from .agent import AgentBrain, createAgentBrain, create_default_agent
from .world import SimpleWorld, createSimpleWorld, createWorld, WorldObject, WorldObjectTypes
from .sensory import Vision, Audio, InternalSignals, SensoryPercept
from .motor import Action, ActionTypes, MotorCommand, WorldObjectTypes
from .environment import WorldObject

__all__ = [
    # Core classes
    'Brain', 'Config', 'Neuron', 'Synapse', 'NeuralRegion',
    'AgentBrain', 'Action', 'ActionTypes',
    'SimpleWorld', 'Vision', 'Audio', 'InternalSignals', 'SensoryPercept',
    'WorldObject',
    
    # Common types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand',
    'WorldObjectType',
    
    # Enum-like classes
    'NeuronTypes', 'SynapseTypes', 'DevelopmentalStages',
    'FiringStates', 'ActionTypes', 'MotorCommands', 'WorldObjectTypes',
    
    # Utility functions
    'createDefaultConfig', 'createBrain', 'createAgentBrain', 'createSimpleWorld',
    'createWorld', 'create_default_agent',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    'MAX_ENERGY', 'ENERGY_DECAY_RATE',
    
    # Helper functions
    'get_examples', 'get_default_world_objects',
]

# Package metadata
package_info = {
    'name': 'pynlm',
    'version': __version__,
    'description': __description__,
    'author': __author__,
    'license': 'MIT',
    'requires_python': '>=3.8',
    'keywords': ['neural', 'simulation', 'spiking', 'brain', 'learning'],
    'classifiers': [
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
        'Topic :: Scientific/Engineering :: Artificial Intelligence',
    ],
}

# Set up logger
import logging
def _get_logger():
    return logging.getLogger(__name__)

logger = _get_logger()

# Convenience factory functions

def createBrain(config=None):
    """Create a new brain with optional configuration.
    
    Args:
        config: Optional configuration. If None, uses default config.
        
    Returns:
        Brain: A new brain instance.
    """
    if config is None:
        config = createDefaultConfig()
    return Brain(config)


def createAgentBrain(brain):
    """Create an agent brain interface from an existing brain.
    
    Args:
        brain: Existing Brain instance.
        
    Returns:
        AgentBrain: Agent brain interface.
    """
    return AgentBrain(brain)


def createSimpleWorld():
    """Create a new simple world.
    
    Returns:
        SimpleWorld: New simple world.
    """
    return SimpleWorld()


# Example usage patterns
def get_examples():
    """Get example code snippets for common usage patterns.
    
    Returns:
        dict: Dictionary of example code.
    """
    return {
        'basic_brain': """
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain()
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
""",
        
        'brain_world_agent': """
import pynlm

# Create components
brain = pynlm.createBrain()
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, vision_width=8, vision_height=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation loop
for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
""",
        
        'learning_agent': """
import pynlm

# Setup learning agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, vision_width=8, vision_height=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)

# Run agent simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Optional: Apply reward modulation based on performance
    if step % 20 == 0:
        reward = world.getAgentBody().energy / world.getMaxEnergy()
        agent.applyRewardModulation(reward, agent.getPredictionError())
""",
    
        'brain_analysis': """
import pynlm
import numpy as np

brain = pynlm.createBrain()
brain.initialize()

# Run some simulation steps
for i in range(1000):
    brain.step(i)

# Analyze brain properties
stats = {
    'total_neurons': brain.getTotalNeuronCount(),
    'total_synapses': brain.getTotalSynapseCount(),
    'active_neurons': brain.getActiveNeuronCount(),
    'firing_neurons': brain.getFiringNeuronCount(),
    'total_spikes': brain.getTotalSpikeCount(),
    'avg_firing_rate': brain.getAverageFiringRate(),
    'exc_inhibition_ratio': brain.getExcitationInhibitionRatio(),
    'developmental_stage': brain.getDevelopmentalStage(),
}

print("Brain Statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")
"""
    }
