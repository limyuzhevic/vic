r"""
NLM (Neural Learning Machine) Python Bindings
===========================================

This package provides Python access to the NLM neural simulation framework.
NLM implements a brain-inspired spiking neural network simulator with
memory, learning, and cognition capabilities.

Key Features:
- Real neural computation with spike-based dynamics
- Multiple memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty detection)
- Developmental stages affecting plasticity
- Action selection and motor control
- Checkpoint system for persistence
- Complete brain-world-environment loop

Installation:
    pip install pynlm

Quick Start:
    >>> import pynlm
    >>> config = pynlm.create_default_config()
    >>> brain = pynlm.create_brain(config)
    >>> brain.initialize()
    >>> for i in range(100):
    ...     brain.step(i)
    >>> print(f"Spikes: {brain.get_total_spike_count()}")

Documentation:
    https://github.com/nlm-project/nlm/docs/PYTHON_API.md
    https://github.com/nlm-project/nlm/easy_usage.md

Getting Help:
    - Bug reports: https://github.com/nlm-project/nlm/issues
    - Questions: https://github.com/nlm-project/nlm/discussions
    - Community: https://discord.gg/nlm
"""

from .bindings import (
    # Core classes
    Config,
    Brain,
    SimpleWorld,
    AgentBrain,
    SensoryPercept,
    Action,
    Neuromodulator,
    Dopamine,
    Curiosity,
    Novelty,
    PredictionError,
    DevelopmentSystem,
    WorkingMemory,
    EpisodicMemory,
    AssociativeMemory,
    PredictionSystem,
    NeuralPlanner,
    ConceptFormation,
    AttentionalSelection,
    Logger,
    
    # Enumerations
    NeuronType,
    DevelopmentalStage,
    ActionType,
    LogLevel,
    ConfigSource,
    
    # Factory functions
    create_default_config,
    create_brain,
    create_simple_world,
    create_agent_brain,
    
    # Helper functions
    run_simple_simulation,
    run_agent_simulation,
    
    # Constants
    DEFAULT_SIMULATION_TIMESTEP,
    DEFAULT_NEURON_COUNT,
    DEFAULT_REGION_COUNT,
    DEFAULT_CONNECTION_PROBABILITY,
)

# Version information
__version__ = "0.1.0"
__author__ = "NLM Research Team"
__description__ = "Neural Learning Machine Python Bindings"
__all__ = [
    # Core classes
    'Config', 'Brain', 'SimpleWorld', 'AgentBrain', 'SensoryPercept', 'Action',
    'Neuromodulator', 'Dopamine', 'Curiosity', 'Novelty', 'PredictionError',
    'DevelopmentSystem', 'WorkingMemory', 'EpisodicMemory', 'AssociativeMemory',
    'PredictionSystem', 'NeuralPlanner', 'ConceptFormation', 'AttentionalSelection',
    'Logger',
    # Enumerations
    'NeuronType', 'DevelopmentalStage', 'ActionType', 'LogLevel', 'ConfigSource',
    # Factory functions
    'create_default_config', 'create_brain', 'create_simple_world', 'create_agent_brain',
    # Helper functions
    'run_simple_simulation', 'run_agent_simulation',
    # Constants
    'DEFAULT_SIMULATION_TIMESTEP', 'DEFAULT_NEURON_COUNT', 'DEFAULT_REGION_COUNT',
    'DEFAULT_CONNECTION_PROBABILITY',
    # Metadata
    '__version__', '__author__', '__description__'
]

# Common aliases for convenience
from .bindings import (
    # Common usage patterns
    createBrain as create_brain,
    createSimpleWorld as create_simple_world,
    createAgentBrain as create_agent_brain,
    createDefaultConfig as create_default_config,
)

# Convenience functions for common use cases
def quick_start_brain(neuron_count=1000, timestep=0.001, steps=100):
    """
    Quick start function to create and run a simple brain simulation.
    
    Args:
        neuron_count: Number of neurons to create
        timestep: Simulation timestep
        steps: Number of simulation steps to run
    
    Returns:
        Tuple of (config, brain) after simulation
    """
    config = create_default_config()
    config.set("neuron_count", neuron_count)
    config.set("simulation_timestep", timestep)
    
    brain = create_brain(config)
    brain.initialize()
    
    for i in range(steps):
        brain.step(i)
    
    return config, brain

def create_agent_world(config=None, world_width=20, world_height=20,
                      vision_width=8, vision_height=8, steps=1000):
    """
    Create a complete agent-world setup for simulation.
    
    Args:
        config: Optional configuration (uses defaults if None)
        world_width: Width of the simulation world
        world_height: Height of the simulation world
        vision_width: Width of agent's vision field
        vision_height: Height of agent's vision field
        steps: Number of simulation steps
    
    Returns:
        Tuple of (brain, world, agent) after simulation
    """
    if config is None:
        config = create_default_config()
    
    brain = create_brain(config)
    brain.initialize()
    
    world = create_simple_world()
    world.configure(world_width, world_height, vision_width, vision_height)
    world.reset()
    world.set_agent_start(world_width / 2.0, world_height / 2.0)
    
    agent = create_agent_brain(brain)
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # Run simulation
    for i in range(steps):
        world.update(0.1)
        
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        
        brain.step(i)
        
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Apply reward based on world state
        reward = 0.0
        if hasattr(world, 'get_sensory_percept'):
            internal = world.get_sensory_percept().get_internal()
            if internal and len(internal) > 0:
                reward = internal[0]
        
        agent.apply_reward_modulation(reward, 0.0)
        
        agent.update_development(0.1)
    
    return brain, world, agent

def create_development_demo(neuron_count=500, developmental_stage="initial"):
    """
    Create a brain with developmental stages for demonstration.
    
    Args:
        neuron_count: Number of neurons
        developmental_stage: Starting developmental stage (initial, critical_period, maturation, adult)
    
    Returns:
        Tuple of (config, brain) with developmental settings
    """
    config = create_default_config()
    config.set("neuron_count", neuron_count)
    
    brain = create_brain(config)
    brain.initialize()
    
    # Set developmental stage
    if developmental_stage == "critical_period":
        brain.set_developmental_stage("critical_period")
    elif developmental_stage == "maturation":
        brain.set_developmental_stage("maturation")
    elif developmental_stage == "adult":
        brain.set_developmental_stage("adult")
    else:
        brain.set_developmental_stage("initial")
    
    return config, brain

def create_plasticity_demo(neuron_count=200, stdp_enabled=True, hebbian_enabled=True,
                           structural_enabled=True):
    """
    Create a brain with various plasticity mechanisms for demonstration.
    
    Args:
        neuron_count: Number of neurons
        stdp_enabled: Enable STDP plasticity
        hebbian_enabled: Enable Hebbian plasticity
        structural_enabled: Enable structural plasticity
    
    Returns:
        Tuple of (config, brain) with plasticity settings
    """
    config = create_default_config()
    config.set("neuron_count", neuron_count)
    
    brain = create_brain(config)
    brain.initialize()
    
    # Configure plasticity systems
    if hasattr(brain, 'get_stdp'):
        stdp = brain.get_stdp()
        if stdp and stdp_enabled:
            stdp.configure(0.01, 0.012, 20.0)  # LTP, LTD, tau
    
    if hasattr(brain, 'get_hebbian'):
        hebbian = brain.get_hebbian()
        if hebbian and hebbian_enabled:
            hebbian.configure(0.001, 0.01)  # learning rate, efficacy
    
    if hasattr(brain, 'get_structural_plasticity'):
        structural = brain.get_structural_plasticity()
        if structural and structural_enabled:
            structural.set_synaptogenesis_rate(0.0001)
            structural.set_pruning_rate(0.00001)
    
    return config, brain

# Export version information
__version__ = "0.1.0"
__author__ = "NLM Research Team"
__description__ = "Neural Learning Machine Python Bindings"

# Help function for users
help_message = """
NLM Python Bindings Help
=======================

Getting Started:
    import pynlm
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()

Basic Usage:
    brain.step(step_number)  # Run one simulation step
    brain.get_total_spike_count()  # Get statistics
    brain.save("checkpoint.bin")  # Save state
    brain.load("checkpoint.bin")  # Load state

Agent Simulation:
    world = pynlm.create_simple_world()
    world.configure(width=20, height=20)
    agent = pynlm.create_agent_brain(brain)
    agent.initialize(world)
    
    for step in range(1000):
        world.update(0.1)
        agent.process_sensory_input(world.get_sensory_percept())
        brain.step(step)
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())

Memory Systems:
    working_memory = brain.get_working_memory()
    episodic_memory = brain.get_episodic_memory()
    associative_memory = brain.get_associative_memory()

Neuromodulation:
    dopamine = brain.get_dopamine()
    curiosity = brain.get_curiosity()
    novelty = brain.get_novelty()

Development:
    development = brain.get_development_system()
    stage = brain.get_developmental_stage()
    brain.set_developmental_stage(stage)

Documentation:
    https://github.com/nlm-project/nlm/docs/PYTHON_API.md
    https://github.com/nlm-project/nlm/easy_usage.md

Getting Help:
    - Issues: https://github.com/nlm-project/nlm/issues
    - Discussions: https://github.com/nlm-project/nlm/discussions
    - Discord: https://discord.gg/nlm
"""

print(help_message)
