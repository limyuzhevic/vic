# NLM Python Package
# Python interface to the Neural Learning Machine (NLM) C++ neural simulation framework

__version__ = "0.1.0"

# Public API
try:
    import pynlm
except ImportError as e:
    raise ImportError(
        "Failed to import NLM Python bindings. "
        "Please ensure the package is properly installed using 'pip install pynlm'."
    ) from e

# Re-export main classes from the C++ bindings
from pynlm import (
    # Core classes
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    Action,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    SensoryPercept,
    ActionResult,
    WorldObject,
    AgentBody,
    
    # Enums
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    
    # Constants
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
    
    # Factory functions
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
)

# Convenience functions
def run_minimal_simulation(steps=100):
    """Run a minimal NLM simulation for demonstration purposes."""
    brain = createBrain(createDefaultConfig())
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    return brain

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world integration."""
    import pynlm
    
    # Create configuration
    config = createDefaultConfig()
    
    # Create brain and initialize
    brain = createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = createAgentBrain(brain)
    
    # Create and configure world
    world = createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    return brain, agent, world

# Package metadata
__all__ = [
    # Classes
    'Config',
    'Brain', 
    'AgentBrain',
    'SimpleWorld',
    'Action',
    'SensoryInput',
    'Vision',
    'Audio',
    'InternalSignals',
    'SensoryPercept',
    'ActionResult',
    'WorldObject',
    'AgentBody',
    
    # Enums
    'NeuronType',
    'SynapseType', 
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    
    # Constants
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Factory functions
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    
    # Convenience functions
    'run_minimal_simulation',
    'run_agent_simulation',
    
    # Package info
    '__version__',
]

# Module documentation
__doc__ = """
Neural Learning Machine (NLM) Python Interface
============================================

This package provides Python access to the NLM brain simulation framework.
It offers a high-level interface for creating, running, and interacting with
neural brains in simulated environments.

Key Features:
- Real spiking neuron dynamics (LIF model)
- Multiple plasticity mechanisms (STDP, Hebbian, structural)
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Cognitive functions (prediction, planning, attention)
- Developmental plasticity
- World simulation and interaction
- Full Python API with type safety

Installation:
    pip install pynlm

Basic Usage:
    import pynlm
    
    # Create a brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation
    for i in range(100):
        brain.step(i)

For more information, see the documentation in the main repository.
"""
