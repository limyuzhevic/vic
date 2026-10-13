"""
NLM Python Bindings - Neural Learning Machine

This module provides Python access to the NLM (Neural Learning Machine) 
C++ neural simulation framework. NLM implements an experimental artificial
developmental brain with integrated memory, prediction, cognition, and
neuromodulation systems.

Key Features:
- Brain simulation with spiking neurons and synaptic plasticity
- Integrated memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty, prediction error)
- Development stages affecting plasticity rates
- Prediction and planning systems
- Real-world simulation environment

Usage Example:
    import nlm
    from nlm import Brain, Config, createSimpleWorld
    
    # Create configuration
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    config.set("simulation_timestep", 0.001)
    
    # Create brain
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(20, 20, 16, 16)
    
    # Run simulation
    for step in range(1000):
        percept = world.getSensoryPercept()
        brain.receiveSensoryInput(percept)
        brain.step(step)
        action = brain.produceAction()
        # ... process action ...

For more detailed documentation, see: https://nlm.readthedocs.io
"""

# Import core classes for easy access
from pynlm import (
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
    SimpleWorld,
    Brain,
    AgentBrain,
)

# Factory functions
def createDefaultConfig():
    """Create a default configuration."""
    return Config()

def createBrain(config=None):
    """Create a new brain with optional configuration."""
    if config is None:
        config = createDefaultConfig()
    return Brain(config)

def createSimpleWorld():
    """Create a new simple world."""
    return SimpleWorld()

def createAgentBrain(brain):
    """Create an agent brain interface."""
    return AgentBrain(brain)

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"

# Check for required dependencies
try:
    import pybind11
    # Pybind11 is automatically available through the C++ extension
except ImportError:
    raise ImportError(
        "pybind11 is required for NLM Python bindings. "
        "It should be automatically installed with the package."
    )

# Validate that the core module is available
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    _import_error = e

if not PYNLM_AVAILABLE:
    raise ImportError(
        f"Failed to import NLM Python bindings (pynlm): {_import_error}\n"
        "This is usually due to missing C++ extension compilation. "
        "Please install the package with: pip install -e ."
    )
