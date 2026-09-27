# NLM Python Module
# This module provides a Pythonic interface to the NLM neural simulation framework.

"""
NLM (Neural Learning Machine) - Python Bindings

This module provides a Pythonic interface to the NLM C++ neural simulation framework.
It wraps the low-level pybind11 bindings with a more Pythonic API that includes:

- Property-based access to brain state
- Context managers for resource management
- Enhanced error handling with descriptive messages
- Convenience functions for common operations
- Support for numpy arrays for sensory input
- Serialization utilities

Typical usage:

```python
import pynlm

# Create a brain with default configuration
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

# Run simulation
for i in range(1000):
    brain.step(i)
    if i % 100 == 0:
        print(f"Step {i}: {brain.firing_neurons} neurons firing")

# Save brain state
brain.save("checkpoint.bin")
```
"""

__version__ = "0.1.0"

from .bindings import (
    # Core classes
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
    
    # Main classes
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
)

# Convenience functions
def create_brain(config=None):
    """Create a brain with optional configuration.
    
    Args:
        config: Optional Config object. If None, creates a default config.
        
    Returns:
        Brain object
    """
    if config is None:
        config = create_default_config()
    return Brain(config)


def create_default_config():
    """Create a default NLM configuration.
    
    Returns:
        Config object with sensible defaults for brain simulation.
    """
    return Config()


def create_simple_world():
    """Create a simple 2D world for simulation.
    
    Returns:
        SimpleWorld object
    """
    return SimpleWorld()


def create_agent_brain(brain):
    """Create an agent brain interface.
    
    Args:
        brain: Brain object to wrap
        
    Returns:
        AgentBrain object
    """
    return AgentBrain(brain)


# Validation and utility functions
def validate_config(config):
    """Validate a configuration object.
    
    Args:
        config: Config object to validate
        
    Raises:
        ValueError: If configuration is invalid
    """
    if config is None:
        raise ValueError("Configuration cannot be None")
    
    # Check if config has any required keys
    keys = config.get_keys()
    if not keys:
        print("Warning: Configuration has no keys set")


def simulate_episode(brain, world, agent, num_steps, timestep=0.1):
    """Run a complete simulation episode.
    
    Args:
        brain: Brain object
        world: SimpleWorld object
        agent: AgentBrain object
        num_steps: Number of simulation steps to run
        timestep: Time step duration
        
    Returns:
        dict: Episode statistics
    """
    agent.initialize(world)
    
    stats = {
        'steps': num_steps,
        'final_neurons': 0,
        'total_spikes': 0,
        'average_firing_rate': 0.0,
    }
    
    for step in range(num_steps):
        world.update(timestep)
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        brain.step(step)
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Log stats periodically
        if step % 100 == 0:
            stats['final_neurons'] = brain.get_firing_neuron_count()
            stats['total_spikes'] = brain.get_total_spike_count()
            stats['average_firing_rate'] = brain.get_average_firing_rate()
    
    return stats


def create_checkpoint(brain, filepath):
    """Create a checkpoint file.
    
    Args:
        brain: Brain object to save
        filepath: Path to save checkpoint file
        
    Returns:
        bool: True if successful, False otherwise
    """
    try:
        return brain.save(filepath)
    except Exception as e:
        print(f"Error saving checkpoint: {e}")
        return False


def load_checkpoint(brain, filepath):
    """Load a checkpoint file.
    
    Args:
        brain: Brain object to load into
        filepath: Path to checkpoint file
        
    Returns:
        bool: True if successful, False otherwise
    """
    try:
        return brain.load(filepath)
    except Exception as e:
        print(f"Error loading checkpoint: {e}")
        return False


# Context managers for resource management
class BrainContext:
    """Context manager for brain simulation.
    
    Example:
        with BrainContext(config, num_steps=1000) as brain:
            # Brain is already initialized and ready to step
            for i in range(num_steps):
                brain.step(i)
    """
    
    def __init__(self, config, num_steps=1000):
        self.config = config
        self.num_steps = num_steps
        self.brain = None
        
    def __enter__(self):
        self.brain = create_brain(self.config)
        self.brain.initialize()
        return self.brain
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.brain:
            self.brain.reset()
            self.brain = None


class WorldContext:
    """Context manager for world simulation.
    
    Example:
        with WorldContext(width=20, height=20, vision_width=8, vision_height=8) as world:
            world.configure(width, height, vision_width, vision_height)
            world.reset()
            # World is ready for agent interaction
    """
    
    def __init__(self, width=20, height=20, vision_width=8, vision_height=8):
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        self.world = None
        
    def __enter__(self):
        self.world = create_simple_world()
        self.world.configure(self.width, self.height, self.vision_width, self.vision_height)
        self.world.reset()
        return self.world
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.world = None


# Batch processing utilities
class BatchSimulator:
    """Batch simulator for running multiple simulation episodes.
    
    Example:
        simulator = BatchSimulator(config, num_episodes=100)
        results = simulator.run_episodes()
        print(f"Average firing rate: {results['avg_firing_rate']:.2f}")
    """
    
    def __init__(self, config, num_episodes=100):
        self.config = config
        self.num_episodes = num_episodes
        self.episode_results = []
        
    def run_episodes(self, world_config=None):
        """Run multiple simulation episodes.
        
        Args:
            world_config: Optional dict with world configuration
            
        Returns:
            dict: Aggregated statistics from all episodes
        """
        if world_config is None:
            world_config = {'width': 20, 'height': 20, 'vision_width': 8, 'vision_height': 8}
            
        for episode in range(self.num_episodes):
            brain = create_brain(self.config)
            brain.initialize()
            
            world = create_simple_world()
            world.configure(**world_config)
            world.reset()
            
            agent = create_agent_brain(brain)
            agent.initialize(world)
            
            # Run episode
            for step in range(100):
                world.update(0.1)
                percept = world.get_sensory_percept()
                agent.process_sensory_input(percept)
                brain.step(step)
                action = agent.decode_motor_command()
                world.apply_motor_command(action, world.get_simulation_time())
            
            # Record statistics
            self.episode_results.append({
                'episode': episode,
                'neurons': brain.get_firing_neuron_count(),
                'spikes': brain.get_total_spike_count(),
                'firing_rate': brain.get_average_firing_rate(),
                'development': brain.get_developmental_stage(),
                'curiosity': agent.get_curiosity_level(),
                'novelty': agent.get_novelty_level(),
                'prediction_error': agent.get_prediction_error(),
            })
        
        # Aggregate results
        neurons = [r['neurons'] for r in self.episode_results]
        spikes = [r['spikes'] for r in self.episode_results]
        firing_rates = [r['firing_rate'] for r in self.episode_results]
        
        return {
            'num_episodes': self.num_episodes,
            'min_neurons': min(neurons),
            'max_neurons': max(neurons),
            'avg_neurons': sum(neurons) / len(neurons),
            'min_spikes': min(spikes),
            'max_spikes': max(spikes),
            'avg_spikes': sum(spikes) / len(spikes),
            'min_firing_rate': min(firing_rates),
            'max_firing_rate': max(firing_rates),
            'avg_firing_rate': sum(firing_rates) / len(firing_rates),
            'all_results': self.episode_results,
        }


# Export all public symbols
__all__ = [
    # Version
    '__version__',
    
    # Core types
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    
    # Main classes
    'Config',
    'Brain',
    'AgentBrain',
    'SimpleWorld',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    'SensoryPercept',
    
    # Convenience functions
    'create_brain',
    'create_default_config',
    'create_simple_world',
    'create_agent_brain',
    'validate_config',
    'simulate_episode',
    'create_checkpoint',
    'load_checkpoint',
    
    # Context managers
    'BrainContext',
    'WorldContext',
    
    # Batch simulator
    'BatchSimulator',
]
