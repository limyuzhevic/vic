# pynlm - NLM Python Package Structure
# 
# This is the main Python module for NLM (Neural Learning Machine).
# Provides Python bindings for the C++ neural simulation framework.
# 
# Usage:
#   import pynlm
#   brain = pynlm.createBrain(pynlm.createDefaultConfig())
#   brain.initialize()
#   for step in range(100):
#       brain.step(step)
#
# The package provides a clean, Pythonic interface to the powerful
# C++ neural simulation engine while maintaining high performance
# through native bindings.

__version__ = "0.1.0"
__author__ = "NLM Project"
__license__ = "MIT"
__description__ = "Python bindings for NLM (Neural Learning Machine) neural simulation framework"

# Import all public classes and functions into the package namespace
from pynlm.bindings import (
    # Core configuration and initialization
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    
    # Main classes
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    MotorCommand,
    ActionResult,
    SensoryPercept,
    WorldObject,
    AgentBody,
    
    # Enums
    NeuronType,
    SynapseType,
    ActionType,
    MotorCommand,
    WorldObjectType,
    DevelopmentalStage,
    FiringState,
    
    # Type definitions
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    SimulationStep,
    Timestamp,
    TimestepDuration,
    MembranePotential,
    SynapticWeight,
    
    # Error types
    RuntimeError,
)

# Package metadata
__all__ = [
    # Core API
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    
    # Main classes
    'Config',
    'Brain',
    'AgentBrain',
    'SimpleWorld',
    'SensoryInput',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'MotorCommand',
    'ActionResult',
    'SensoryPercept',
    'WorldObject',
    'AgentBody',
    
    # Enums
    'NeuronType',
    'SynapseType',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    'DevelopmentalStage',
    'FiringState',
    
    # Types
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    'SimulationStep',
    'Timestamp',
    'TimestepDuration',
    'MembranePotential',
    'SynapticWeight',
    
    # Errors
    'RuntimeError',
    
    # Package metadata
    '__version__',
    '__author__',
    '__license__',
    '__description__',
]

# Package-level documentation and examples
__doc__ = """
NLM (Neural Learning Machine) Python Package
============================================

The NLM package provides Python bindings for the powerful C++ neural
learning machine framework. It enables users to create, configure, and
run complex neural simulations with intuitive Python interfaces.

Key Features:
- Intuitive Python API for neural simulation
- Access to advanced brain features: memory, neuromodulation, development
- Integration with simulation environments (worlds)
- Support for agent-based behavior and learning
- High performance through native C++ bindings
- Comprehensive error handling and type safety

Quick Start:
------------
import pynlm

# Create and configure a brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Create a simple environment
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learn from consequences
    reward = 0.0  # Get from world
    agent.applyRewardModulation(reward, 0.0)

print("Simulation complete!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")

Further Documentation:
---------------------
For comprehensive documentation, including:
- Advanced configuration options
- Memory system usage
- Neuromodulation mechanisms
- Development system
- Error handling and troubleshooting

See the official NLM documentation at: https://nlm.readthedocs.io
"""

# Package initialization check
if __name__ != "__main__":
    # Verify that all required bindings are available
    try:
        # Test basic functionality
        _ = createDefaultConfig
        _ = createBrain
        _ = createSimpleWorld
        _ = createAgentBrain
    except Exception as e:
        raise ImportError(f"Failed to load pynlm bindings: {e}") from e
