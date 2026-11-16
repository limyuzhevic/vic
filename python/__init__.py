"""
NLM (熙然) - Neural Learning Machine Python Bindings

This is a Python package that provides bindings to the NLM C++ neural simulation framework.
It allows users to create and interact with artificial neural networks through Python.

Main Features:
- Create and configure NLM brains with Python
- Run neural simulations through Python
- Interact with world simulations
- Apply reward-based learning
- Access neural network statistics and diagnostics

Quick Start:
```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize a brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run some steps
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

See the documentation and examples for more advanced usage.
"""

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Package metadata
__all__ = [
    # Core classes
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    
    # Main classes
    'Config',
    'Brain',
    'AgentBrain',
    'SimpleWorld',
    
    # Enums and types
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    
    # Sensory classes
    'SensoryInput',
    'Vision',
    'Audio',
    'InternalSignals',
    
    # Motor classes
    'Action',
    'ActionResult',
    
    # World classes
    'WorldObject',
    'AgentBody',
    'SensoryPercept',
    'ActionResult',
    
    # Constants and utilities
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Convenience functions
    'run_simulation',
    'create_demo_brain',
    'create_demo_world',
    'create_demo_agent',
]

# Import main bindings
from .pynlm import (
    # Factory functions
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    
    # Main classes
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    
    # Enums and types
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    ActionType,
    MotorCommand,
    WorldObjectType,
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    
    # Sensory classes
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    
    # Motor classes
    Action,
    ActionResult,
    
    # World classes
    WorldObject,
    AgentBody,
    SensoryPercept,
    
    # Constants
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
)

# Pythonic convenience functions
def run_simulation(brain, world, agent, num_steps, verbose=False):
    """
    Run a complete simulation with brain, world, and agent.
    
    This is a convenience function that runs the main simulation loop
    for agent-based learning.
    
    Args:
        brain: NLM brain instance
        world: SimpleWorld instance
        agent: AgentBrain instance
        num_steps: Number of simulation steps to run
        verbose: If True, print progress information
        
    Returns:
        dict: Dictionary containing simulation statistics
    """
    stats = {
        'steps': num_steps,
        'final_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'development_stage': brain.getDevelopmentalStage(),
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel(),
        'dopamine': agent.getNeuromodulationLevel(),
        'memory_traces': brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0,
    }
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        if verbose and step % 100 == 0:
            print(f"Step {step}: "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Novelty: {agent.getNoveltyLevel():.3f}")
    
    return stats

def create_demo_brain(neuron_count=1000):
    """Create a demo brain with default configuration."""
    config = createDefaultConfig()
    config.set("brain.neuron_count", int64_t(neuron_count))
    config.set("brain.synapse_density", 0.1f)
    config.set("brain.initial_weight_mean", 0.5f)
    
    brain = createBrain(config)
    brain.initialize()
    return brain

def create_demo_world(width=20, height=20, vision_width=8, vision_height=8):
    """Create a demo world for agent simulation."""
    world = createSimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    world.reset()
    return world

def create_demo_agent(brain):
    """Create a demo agent brain interface."""
    agent = createAgentBrain(brain)
    return agent

# Utility functions
def validate_config(config):
    """Validate that a configuration has required settings."""
    required_keys = [
        "brain.neuron_count",
        "brain.synapse_density",
        "brain.connection_probability",
    ]
    
    for key in required_keys:
        if not config.has(key):
            raise ValueError(f"Missing required configuration key: {key}")
    
    return True

def get_brain_summary(brain):
    """Get a human-readable summary of brain statistics."""
    if not brain:
        return "No brain instance"
    
    stats = []
    stats.append(f"Neurons: {brain.getTotalNeuronCount()}")
    stats.append(f"Synapses: {brain.getTotalSynapseCount()}")
    stats.append(f"Firing neurons: {brain.getFiringNeuronCount()}")
    stats.append(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    stats.append(f"Total spikes: {brain.getTotalSpikeCount()}")
    stats.append(f"Development stage: {brain.getDevelopmentalStage()}")
    
    if brain.getWorkingMemory():
        stats.append(f"Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
    
    return " | ".join(stats)

# Error classes
class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigError(NLMError):
    """Exception raised for configuration errors."""
    pass

class SimulationError(NLMError):
    """Exception raised for simulation errors."""
    pass

# Expose the error classes
__all__.extend(['NLMError', 'ConfigError', 'SimulationError'])
