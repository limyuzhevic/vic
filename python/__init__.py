# pynlm - Python bindings for NLM (Neural Learning Machine)
# Automatically generated enhanced bindings with comprehensive Pythonic APIs

"""
NLM (Neural Learning Machine) Python Bindings
---------------------------------------------

A comprehensive Python interface for the NLM neural simulation framework.

This module provides enhanced Python bindings with:
- Context manager support for automatic resource management
- Enhanced configuration with convenience methods
- Simulation session management with save/load capabilities
- High-level agent APIs for easier development
- Visualization support for brain state monitoring
- Batch operations for efficient simulation
- Event system for real-time simulation monitoring
- Advanced configuration setup
- Memory management utilities
- Comprehensive documentation and examples

Key Classes:
    Brain - Central neural simulation brain
    Config - Configuration system
    AgentBrain - Interface connecting brain to world
    SimpleWorld - 2D simulation world
    SensoryInput, Vision, Audio, InternalSignals - Sensory modalities
    Action - Motor output representation
    AgentBody - Physical agent state
    ActionResult - Action outcomes
    SensoryPercept - Processed sensory input
    WorldObject - World entities

Example Usage:
    import pynlm

    # Create and configure brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()

    # Run simulation with context manager
    with pynlm.simulation_session(brain, num_steps=1000) as session:
        session.run()

    # Save/load capabilities
    brain.save("checkpoint.brain")
    brain.load("checkpoint.brain")

See examples/ directory for more comprehensive examples.
"""

__version__ = "0.1.0"
__author__ = "NLM Authors"

# Import main classes
from pynlm.bindings import (
    # Core classes
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    
    # Main classes
    Config, Brain, AgentBrain, SimpleWorld,
    SensoryInput, Vision, Audio, InternalSignals,
    Action, WorldObject, AgentBody, ActionResult,
    SensoryPercept,
    
    # Convenience functions
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    
    # Invalid IDs
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID,
)

# Simulation session management
from pynlm.bindings import simulation_session, batch_simulation

# Event system
from pynlm.bindings import EventManager, SpikeEvent, LearningEvent, SimulationEvent

# Visualization
from pynlm.bindings import VisualizationInterface, BrainVisualizer

# Configuration helpers
from pynlm.bindings import (
    create_default_config, create_experimental_config,
    create_agent_config, create_world_config,
)

# Agent utilities
from pynlm.bindings import (
    create_agent, agent_episode, agent_train,
    simulate_agent,
)

# Memory management
from pynlm.bindings import MemoryManager, CheckpointManager

# Batch operations
from pynlm.bindings import BatchRunner, ParallelSimulator

# Export all public APIs
__all__ = [
    # Core types
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    "NeuronType", "SynapseType", "DevelopmentalStage", "FiringState",
    "ActionType", "MotorCommand", "WorldObjectType",
    
    # Main classes
    "Config", "Brain", "AgentBrain", "SimpleWorld",
    "SensoryInput", "Vision", "Audio", "InternalSignals",
    "Action", "WorldObject", "AgentBody", "ActionResult",
    "SensoryPercept",
    
    # Convenience functions
    "createDefaultConfig", "createBrain", "createSimpleWorld", "createAgentBrain",
    
    # Invalid IDs
    "INVALID_NEURON_ID", "INVALID_SYNAPSE_ID", "INVALID_REGION_ID", "INVALID_POPULATION_ID",
    
    # Simulation management
    "simulation_session", "batch_simulation",
    
    # Event system
    "EventManager", "SpikeEvent", "LearningEvent", "SimulationEvent",
    
    # Visualization
    "VisualizationInterface", "BrainVisualizer",
    
    # Configuration helpers
    "create_default_config", "create_experimental_config",
    "create_agent_config", "create_world_config",
    
    # Agent utilities
    "create_agent", "agent_episode", "agent_train",
    "simulate_agent",
    
    # Memory management
    "MemoryManager", "CheckpointManager",
    
    # Batch operations
    "BatchRunner", "ParallelSimulator",
    
    # Version and metadata
    "__version__", "__author__",
]

# Set up module metadata
import sys
if sys.version_info >= (3, 8):
    from importlib.metadata import version, PackageNotFoundError
    try:
        __version__ = version("pynlm")
    except PackageNotFoundError:
        pass

# Create module-level convenience functions
def quick_start_demo():
    """Run a quick demonstration of NLM capabilities"""
    print("=== NLM Quick Start Demo ===")
    print("1. Creating brain with default config...")
    config = create_default_config()
    brain = createBrain(config)
    brain.initialize()
    
    print("2. Running 100 simulation steps...")
    brain.step(0, 0.0)
    
    print("3. Brain statistics:")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    print("=== Demo Complete ===")

def get_nlm_info():
    """Get information about NLM version and capabilities"""
    info = {
        "version": __version__,
        "description": "NLM (Neural Learning Machine) Python Bindings",
        "features": [
            "Context manager support",
            "Enhanced configuration",
            "Simulation session management",
            "Agent integration APIs",
            "Visualization support",
            "Batch operations",
            "Event system",
            "Advanced configuration",
            "Memory management",
        ],
        "supported_python": ">= 3.8",
        "license": "MIT",
    }
    return info

# Example functions
def example_brain_creation():
    """Example: Basic brain creation and initialization"""
    config = create_default_config()
    brain = createBrain(config)
    brain.initialize()
    return brain

def example_simulation_session():
    """Example: Using simulation session context manager"""
    config = create_default_config()
    brain = createBrain(config)
    brain.initialize()
    
    with simulation_session(brain, num_steps=1000) as session:
        session.run()
    
    return brain

def example_agent_workflow():
    """Example: Complete agent workflow"""
    config = create_experimental_config()
    brain = createBrain(config)
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(20.0, 20.0, 10, 10)
    
    agent_brain = createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # Run agent episode
    percept = world.getSensoryPercept()
    agent_brain.processSensoryInput(percept)
    
    action = agent_brain.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    agent_brain.applyRewardModulation(result.reward, 0.0)
    
    return brain, agent_brain, world