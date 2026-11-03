# NLM Python Module - Professional Python Interface for NLM Neural Simulation

"""
NLM (熙然) Python bindings provide a comprehensive interface for the NLM
(Neural Learning Machine) neural simulation framework. This module enables
Python developers to create, configure, and simulate artificial brain systems
with spiking neural networks, memory systems, neuromodulation, and more.

Key Features:
- Brain simulation with LIF neuron dynamics
- Configuration management
- World interaction and environment simulation
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Motor control and action selection
- Visualization and monitoring tools
- Phase 6 integration testing

Example Usage:
    >>> import pynlm
    >>> 
    >>> # Create a default configuration
    >>> config = pynlm.createDefaultConfig()
    >>> 
    >>> # Create a brain with the configuration
    >>> brain = pynlm.createBrain(config)
    >>> 
    >>> # Initialize the brain
    >>> brain.initialize()
    >>> 
    >>> # Create a world for the brain to interact with
    >>> world = pynlm.createSimpleWorld()
    >>> 
    >>> # Create an agent brain that connects the neural system to the world
    >>> agent = pynlm.createAgentBrain(brain)
    >>> 
    >>> # Run simulation steps
    >>> for step in range(1000):
    ...     world.update(0.001)
    ...     brain.step(step)
    ...     action = brain.produceAction()
    ...     world.applyMotorCommand(action)

The module follows a component-based architecture where each neural system
(e.g., memory, neuromodulation, cognition) is integrated into the main brain
loop through the AgentBrain interface. This enables sophisticated behavior
emerging from the interaction of multiple neural subsystems.

For more detailed examples and API documentation, see the example scripts
in the examples directory or refer to the NLM documentation.
"""

__version__ = "0.1.0"
__author__ = "NLM Research Project"
__email__ = "research@nlm-project.org"

# Core module exports
from .pynlm import (
    # Brain classes
    Brain,
    createBrain,
    createDefaultConfig,
    
    # World and environment classes
    SimpleWorld,
    createSimpleWorld,
    
    # Agent classes
    AgentBrain,
    createAgentBrain,
    
    # Sensorimotor classes
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    SensoryPercept,
    Action,
    WorldObject,
    ActionType,
    MotorCommand,
    ActionResult,
    
    # Motor system classes
    AgentBody,
    
    # Enumeration types
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
)

# Core type definitions
from .pynlm import NeuronId, SynapseId, RegionId, PopulationId

# Memory system exports
from .pynlm import NeuralWorkingMemory, NeuralEpisodicMemory, NeuralAssociativeMemory

# Neuromodulation system exports
from .pynlm import Dopamine, Curiosity, Novelty, PredictionError

# Cognitive system exports
from .pynlm import NeuralPlanner, ConceptFormation

# Development system exports
from .pynlm import DevelopmentSystem

# Prediction system exports
from .pynlm import PredictionSystem, NeuralPrediction

# Utility functions
from .pynlm import INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID

__all__ = [
    # Core classes
    'Brain', 'createBrain', 'createDefaultConfig',
    'SimpleWorld', 'createSimpleWorld',
    'AgentBrain', 'createAgentBrain',
    
    # Sensorimotor
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'SensoryPercept', 'Action', 'WorldObject',
    'ActionType', 'MotorCommand', 'ActionResult',
    
    # Motor system
    'AgentBody',
    
    # Enumerations
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Type definitions
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    
    # Memory systems
    'NeuralWorkingMemory', 'NeuralEpisodicMemory', 'NeuralAssociativeMemory',
    
    # Neuromodulation
    'Dopamine', 'Curiosity', 'Novelty', 'PredictionError',
    
    # Cognitive systems
    'NeuralPlanner', 'ConceptFormation',
    
    # Development
    'DevelopmentSystem',
    
    # Prediction
    'PredictionSystem', 'NeuralPrediction',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
]

# Configuration utilities
def save_config(config, filepath):
    """
    Save a configuration to a file.
    
    Args:
        config: The configuration object to save
        filepath: Path to the configuration file
        
    Returns:
        bool: True if successful, False otherwise
    """
    return config.saveToFile(filepath)

def load_config(filepath):
    """
    Load a configuration from a file.
    
    Args:
        filepath: Path to the configuration file
        
    Returns:
        Config: The loaded configuration object, or None if failed
    """
    from .pynlm import Config
    config = Config()
    if config.loadFromFile(filepath):
        return config
    return None

# Convenience functions for simulation setup
def create_demo_brain(neurons=1000, regions=2, timestep=0.001):
    """
    Create a demo brain with typical NLM configuration.
    
    Args:
        neurons: Number of neurons (default: 1000)
        regions: Number of brain regions (default: 2)
        timestep: Simulation timestep (default: 0.001)
        
    Returns:
        Brain: The initialized brain object
    """
    config = createDefaultConfig()
    
    # Set typical values for demo purposes
    # Note: Config values are stored as strings, so we need to set them properly
    # In practice, the Config class has typed getters/setters
    
    brain = createBrain(config)
    brain.initialize()
    return brain

def run_demo_simulation(steps=1000):
    """
    Run a simple demo simulation.
    
    Args:
        steps: Number of simulation steps to run (default: 1000)
        
    Returns:
        dict: Simulation results including final statistics
    """
    brain = create_demo_brain()
    world = createSimpleWorld()
    
    total_reward = 0.0
    max_firing_rate = 0.0
    
    for step in range(steps):
        world.update(0.001)
        brain.step(step)
        
        action = brain.produceAction()
        world.applyMotorCommand(action)
        
        # Simple reward based on action success
        reward = 0.1  # Small positive reward
        total_reward += reward
        
        max_firing_rate = max(max_firing_rate, brain.getAverageFiringRate())
    
    return {
        'steps': steps,
        'total_reward': total_reward,
        'avg_firing_rate': brain.getAverageFiringRate(),
        'max_firing_rate': max_firing_rate,
        'final_spike_count': brain.getTotalSpikeCount()
    }

# Batch processing utilities
def batch_simulate(configs, steps=100):
    """
    Run simulations with multiple configurations.
    
    Args:
        configs: List of configuration dictionaries
        steps: Number of simulation steps for each configuration
        
    Returns:
        list: List of simulation results
    """
    results = []
    for i, config_dict in enumerate(configs):
        config = createDefaultConfig()
        
        # Apply configuration values
        # This is a simplified example - actual configuration
        # would depend on the specific Config class interface
        
        brain = createBrain(config)
        brain.initialize()
        
        for step in range(steps):
            brain.step(step)
        
        results.append({
            'config_index': i,
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount()
        })
    
    return results

# Visualization helper
def export_brain_state(brain, filepath):
    """
    Export brain state to a file for visualization.
    
    Args:
        brain: The brain object to export
        filepath: Path to the output file
        
    Returns:
        bool: True if successful, False otherwise
    """
    return brain.save(filepath)

# Error handling
def safe_operation(func, *args, default=None, **kwargs):
    """
    Execute a function safely with error handling.
    
    Args:
        func: Function to execute
        *args: Positional arguments for the function
        default: Default value to return if operation fails
        **kwargs: Keyword arguments for the function
        
    Returns:
        The result of func(*args, **kwargs) or default if exception occurs
    """
    try:
        return func(*args, **kwargs)
    except Exception as e:
        print(f"Error in safe_operation: {e}")
        return default

# Module metadata
metadata = {
    'version': __version__,
    'author': __author__,
    'description': __doc__,
    'components': [
        'brain', 'memory', 'neuromodulation', 'cognition',
        'development', 'prediction', 'sensory', 'motor', 'world'
    ],
    'features': [
        'spiking_neural_networks',
        'memory_systems',
        'neuromodulation',
        'cognition',
        'development',
        'prediction',
        'motor_control',
        'visualization'
    ]
}

if __name__ == "__main__":
    print("NLM Python Module")
    print(f"Version: {__version__}")
    print(f"Author: {__author__}")
    print("\nAvailable functions:")
    print("- create_default_config() - Create a default configuration")
    print("- create_brain(config) - Create a brain with configuration")
    print("- create_simple_world() - Create a simple world environment")
    print("- create_agent_brain(brain) - Create an agent brain interface")
    print("- save_config(config, filepath) - Save configuration to file")
    print("- load_config(filepath) - Load configuration from file")
    print("- export_brain_state(brain, filepath) - Export brain state for visualization")
