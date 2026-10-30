"""
Python bindings for NLM (Neural Learning Machine)

This module provides Python bindings to the C++ NLM neural simulation framework.
It enables creating, configuring, and running neural networks through a Python API.

The NLM (熙然, meaning "serene flow") brain simulator allows you to create
virtual brains that can learn, remember things, and make decisions - just like
how your brain works!
"""

__version__ = "0.1.0"

# Import the core classes and functions from the C++ bindings
try:
    from .core import (
        Config,
        Brain,
        MotorCommand,
        NeuronType,
        DevelopmentalStage,
        WorldObjectType,
        WorldObject
    )
    from .agent import AgentBrain
    from .world import SimpleWorld
    from .sensory import SensoryPercept
    from .motor import MotorSystem
    from .neuromodulation import Neuromodulator
    from .memory import WorkingMemory, EpisodicMemory
    from .cognition import NeuralPlanner, ConceptFormation, Attention
    from .performance import MemoryPool, EventQueue, CheckpointSystem
    from .development import DevelopmentSystem, Synaptogenesis, Pruning, Maturation
    from .plasticity import PlasticityRule, STDP, Hebbian, StructuralPlasticity
    from .prediction import PredictionSystem, NeuralPrediction
    from .dynamics import NeuralDynamics, SpikeSystem
    from .environment import Environment, Observation
    from .visualization import VisualizationInterface
    from .experiments import (
        Experiment,
        ExperimentRunner,
        Metrics,
        Phase3Experiment,
        Phase4Experiment,
        Phase5Experiment,
        Phase6IntegratedExperiment
    )
except ImportError:
    # If C++ bindings aren't available, provide minimal stubs for development
    Config = None
    Brain = None
    MotorCommand = None
    NeuronType = None
    DevelopmentalStage = None
    WorldObjectType = None
    WorldObject = None
    AgentBrain = None
    SimpleWorld = None
    SensoryPercept = None
    MotorSystem = None
    Neuromodulator = None
    WorkingMemory = None
    EpisodicMemory = None
    NeuralPlanner = None
    ConceptFormation = None
    Attention = None
    MemoryPool = None
    EventQueue = None
    CheckpointSystem = None
    DevelopmentSystem = None
    Synaptogenesis = None
    Pruning = None
    Maturation = None
    PlasticityRule = None
    STDP = None
    Hebbian = None
    StructuralPlasticity = None
    PredictionSystem = None
    NeuralPrediction = None
    NeuralDynamics = None
    SpikeSystem = None
    Environment = None
    Observation = None
    VisualizationInterface = None
    Experiment = None
    ExperimentRunner = None
    Metrics = None
    Phase3Experiment = None
    Phase4Experiment = None
    Phase5Experiment = None
    Phase6IntegratedExperiment = None
    
    print("Warning: C++ bindings not available. Some functionality may be limited.")

# Convenience functions for common usage patterns
def createDefaultConfig():
    """Create a default configuration for a brain."""
    if Config is None:
        raise ImportError("Config not available - C++ bindings not installed")
    
    config = Config()
    # Set sensible defaults
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    config.set("neuron_count", 1000)
    config.set("region_count", 1)
    config.set("connection_probability", 0.1)
    config.set("stdp_ltp_weight", 0.01f)
    config.set("stdp_ltd_weight", 0.012f)
    config.set("plasticity_learning_rate", 0.01)
    config.set("synaptogenesis_rate", 0.001)
    config.set("pruning_rate", 0.0001)
    config.set("development_synaptogenesis_rate", 0.001)
    config.set("development_pruning_rate", 0.0001)
    return config


def createBrain(config=None):
    """Create and initialize a brain with the given configuration."""
    if Brain is None:
        raise ImportError("Brain not available - C++ bindings not installed")
    
    if config is None:
        config = createDefaultConfig()
    
    brain = Brain(config)
    brain.initialize()
    return brain


def createAgentBrain(brain):
    """Create an agent brain interface for the given brain."""
    if AgentBrain is None:
        raise ImportError("AgentBrain not available - C++ bindings not installed")
    
    return AgentBrain(brain)


def createSimpleWorld(width=20, height=20, vision_width=8, vision_height=8):
    """Create a simple world with the given dimensions."""
    if SimpleWorld is None:
        raise ImportError("SimpleWorld not available - C++ bindings not installed")
    
    world = SimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    return world


def run_agent_simulation(brain, world, agent, num_steps=1000, timestep=0.1):
    """Run a complete agent simulation with brain and world."""
    for step in range(num_steps):
        # Update world
        world.update(timestep)
        
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
        agent.updateDevelopment(timestep)
    
    return brain, world, agent


def run_simple_brain_simulation(brain, num_steps=1000):
    """Run a simple brain simulation without a world."""
    for step in range(num_steps):
        brain.step(step)
    
    return brain


# Common brain configuration presets
def get_phase2_config():
    """Get Phase 2 configuration (real neural computation)."""
    config = createDefaultConfig()
    config.set("neuron_count", 500)  # Smaller for faster tests
    config.set("region_count", 1)
    config.set("stdp_ltp_weight", 0.02f)
    config.set("stdp_ltd_weight", 0.015f)
    config.set("stdp_tau", 20.0f)
    config.set("synaptogenesis_rate", 0.0001f)
    config.set("pruning_rate", 0.00001f)
    return config


def get_phase3_config():
    """Get Phase 3 configuration (world interaction)."""
    config = createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("region_count", 2)
    config.set("connection_probability", 0.1f)
    config.set("stdp_ltp_weight", 0.01f)
    config.set("stdp_ltd_weight", 0.012f)
    config.set("stdp_tau", 20.0f)
    config.set("environment_width", 20)
    config.set("environment_height", 20)
    config.set("vision_width", 16)
    config.set("vision_height", 16)
    return config


def get_phase6_config():
    """Get Phase 6 configuration (integration)."""
    config = createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("region_count", 1)
    config.set("connection_probability", 0.1f)
    config.set("max_simulation_steps", 2000)
    config.set("enable_checkpointing", True)
    config.set("enable_replay", True)
    config.set("enable_development", True)
    return config


# Utility functions for common operations
def get_brain_stats(brain):
    """Get comprehensive brain statistics as a dictionary."""
    if brain is None:
        return {}
    
    return {
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount(),
        'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'developmental_stage': brain.getDevelopmentalStage().value if hasattr(brain.getDevelopmentalStage(), 'value') else str(brain.getDevelopmentalStage()),
        'has_working_memory': brain.getWorkingMemory() is not None,
        'has_episodic_memory': brain.getEpisodicMemory() is not None,
        'has_prediction_system': brain.getPredictionSystem() is not None,
        'has_development_system': brain.getDevelopmentSystem() is not None,
        'has_neuromodulation': brain.getNeuromodulationSystem() is not None,
    }


def print_brain_stats(brain, title="Brain Stats"):
    """Print brain statistics in a readable format."""
    stats = get_brain_stats(brain)
    
    print(f"\n{title}")
    print("=" * len(title))
    for key, value in stats.items():
        print(f"{key.replace('_', ' ').title()}: {value}")


# Debug and logging helpers
def enable_logging(level="INFO"):
    """Enable logging for NLM operations."""
    # This would enable logging in the C++ backend if available
    print(f"Logging enabled at level: {level}")


# Example demonstrations
def simple_brain_example():
    """Demonstrate the simplest possible brain example."""
    print("=== Simple Brain Example ===")
    
    brain = createBrain()
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons!")
    
    for i in range(10):
        brain.step(i)
    
    print(f"After 10 steps: {brain.getFiringNeuronCount()} neurons firing")
    print("Simple brain example complete!")


def agent_world_example():
    """Demonstrate brain-world-agent interaction."""
    print("=== Brain-World-Agent Example ===")
    
    # Setup
    brain = createBrain()
    brain.initialize()
    
    world = createSimpleWorld(width=15, height=15, vision_width=8, vision_height=8)
    world.reset()
    world.setAgentStart(7.0, 7.0)
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run agent
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print("Agent simulation complete!")
    print(f"Final energy: {world.getAgentBody().energy}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.3f}")


__all__ = [
    # Core classes
    'Config',
    'Brain',
    'MotorCommand',
    'NeuronType',
    'DevelopmentalStage',
    'WorldObjectType',
    'WorldObject',
    'AgentBrain',
    'SimpleWorld',
    'SensoryPercept',
    'MotorSystem',
    'Neuromodulator',
    'WorkingMemory',
    'EpisodicMemory',
    'NeuralPlanner',
    'ConceptFormation',
    'Attention',
    'MemoryPool',
    'EventQueue',
    'CheckpointSystem',
    'DevelopmentSystem',
    'Synaptogenesis',
    'Pruning',
    'Maturation',
    'PlasticityRule',
    'STDP',
    'Hebbian',
    'StructuralPlasticity',
    'PredictionSystem',
    'NeuralPrediction',
    'NeuralDynamics',
    'SpikeSystem',
    'Environment',
    'Observation',
    'VisualizationInterface',
    'Experiment',
    'ExperimentRunner',
    'Metrics',
    'Phase3Experiment',
    'Phase4Experiment',
    'Phase5Experiment',
    'Phase6IntegratedExperiment',
    
    # Convenience functions
    'createDefaultConfig',
    'createBrain',
    'createAgentBrain',
    'createSimpleWorld',
    'run_agent_simulation',
    'run_simple_brain_simulation',
    
    # Config presets
    'get_phase2_config',
    'get_phase3_config',
    'get_phase6_config',
    
    # Utility functions
    'get_brain_stats',
    'print_brain_stats',
    'enable_logging',
    
    # Examples
    'simple_brain_example',
    'agent_world_example',
]
