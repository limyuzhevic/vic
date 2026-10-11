"""
NLM - Neural Learning Machine

A comprehensive Python API for the experimental artificial developmental brain system.

This module provides access to:
- Neural brain simulation with spiking neurons
- Working, episodic, and semantic memory systems
- Neuromodulation (dopamine, curiosity, novelty detection)
- Prediction and planning systems
- Cognitive mechanisms (concept formation, attention)
- Developmental stages and structural plasticity
- Sensory and motor processing
- Environment interaction

Example usage:

    import pynlm
    
    # Create a brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simple simulation
    for step in range(100):
        brain.step(step)
    
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
"""

import sys
from typing import Optional, List, Dict, Any

# Import the native C++ bindings
try:
    from .native import pynlm
except ImportError:
    # Fallback for development - compile native module if not available
    try:
        import pynlm
    except ImportError:
        raise ImportError(
            "NLM native bindings not found. Please build the native module first:\n"
            "  mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make\n"
            "  Then install the Python bindings:\n"
            "  pip install -e ."
        )

# Re-export all the core classes and functions from the native module
from pynlm import (
    # ID types
    NeuronId, SynapseId, RegionId, PopulationId,
    
    # Enumerations
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    
    # Configuration and core classes
    Config, Brain, AgentBrain, AgentBody, SimpleWorld,
    SensoryInput, Vision, Audio, InternalSignals,
    Action, WorldObject, ActionResult, SensoryPercept,
)

# Factory functions for easy usage
def create_default_config() -> Config:
    """Create a default NLM configuration with sensible defaults.
    
    Returns:
        A Config instance ready for brain creation.
        
    Example:
        >>> config = create_default_config()
        >>> brain = create_brain(config)
    """
    return Config()

def create_brain(config: Optional[Config] = None) -> Brain:
    """Create a new NLM brain with the given configuration.
    
    Args:
        config: Brain configuration. If None, a default config is used.
        
    Returns:
        A new Brain instance ready for simulation.
        
    Example:
        >>> brain = create_brain()
        >>> brain.initialize()
    """
    if config is None:
        config = create_default_config()
    return Brain(config)

def create_simple_world(width: int = 20, height: int = 20,
                       vision_width: int = 8, vision_height: int = 8) -> SimpleWorld:
    """Create a simple 2D world for agent simulation.
    
    Args:
        width: World width in cells.
        height: World height in cells.
        vision_width: Agent's vision width.
        vision_height: Agent's vision height.
        
    Returns:
        A SimpleWorld instance.
        
    Example:
        >>> world = create_simple_world(30, 30)
        >>> world.configure(30, 30, 8, 8)
    """
    world = SimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    return world

def create_agent_brain(brain: Brain, world: SimpleWorld) -> AgentBrain:
    """Create an agent brain interface for brain-world interaction.
    
    Args:
        brain: The brain to wrap.
        world: The world to interact with.
        
    Returns:
        An AgentBrain instance.
        
    Example:
        >>> agent = create_agent_brain(brain, world)
        >>> agent.initialize(world)
    """
    agent = AgentBrain(brain)
    agent.initialize(world)
    return agent

# High-level convenience classes and functions

class NLMSimulation:
    """High-level interface for running NLM simulations.
    
    This class provides a convenient way to run complete simulation
    episodes with proper initialization, cleanup, and statistics collection.
    
    Example:
        >>> sim = NLMSimulation()
        >>> result = sim.run_episode(num_steps=500)
        >>> print(f"Firing rate: {result['avg_firing_rate']:.3f}")
    """
    
    def __init__(self, config: Optional[Config] = None, 
                 world: Optional[SimpleWorld] = None,
                 enable_neuromodulation: bool = True,
                 enable_development: bool = True,
                 enable_curiosity: bool = True):
        """Initialize the simulation.
        
        Args:
            config: Brain configuration.
            world: Optional world for agent simulation.
            enable_neuromodulation: Enable reward-based learning.
            enable_development: Enable developmental stages.
            enable_curiosity: Enable curiosity-driven exploration.
        """
        self.config = config or create_default_config()
        self.world = world
        self.brain = create_brain(self.config)
        self.agent = None
        
        # Enable learning subsystems if world is provided
        if self.world:
            self.agent = create_agent_brain(self.brain, self.world)
            if enable_neuromodulation:
                self.agent.enableRewardModulation(True)
            if enable_curiosity:
                self.agent.enableCuriosity(True)
            if enable_development:
                self.agent.enableDevelopment(True)
        
        self.episode_stats = {}
    
    def initialize(self) -> None:
        """Initialize the simulation components."""
        self.brain.initialize()
        if self.agent:
            self.agent.initialize(self.world)
        self.episode_stats = {
            'steps': 0,
            'total_spikes': 0,
            'firing_neurons': 0,
            'avg_firing_rate': 0.0,
            'total_reward': 0.0,
            'curiosity_level': 0.0,
            'novelty_level': 0.0,
            'development_stage': 0.0,
        }
    
    def run_step(self, step: int, timestep: float = 0.1) -> Dict[str, Any]:
        """Run a single simulation step.
        
        Args:
            step: Current step number.
            timestep: Time step duration.
            
        Returns:
            Dictionary with step statistics.
        """
        if self.agent and self.world:
            # Get sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Apply action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward and apply neuromodulation
            reward = 0.0
            if percept and percept.getInternal():
                reward = percept.getInternal()[0]
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(timestep)
        
        # Run brain step
        self.brain.step(step)
        
        # Update statistics
        self.episode_stats['steps'] += 1
        self.episode_stats['total_spikes'] += self.brain.getTotalSpikeCount()
        self.episode_stats['firing_neurons'] += self.brain.getFiringNeuronCount()
        
        if self.agent:
            self.episode_stats['curiosity_level'] = self.agent.getCuriosityLevel()
            self.episode_stats['novelty_level'] = self.agent.getNoveltyLevel()
            self.episode_stats['development_stage'] = float(self.agent.getDevelopmentalStage())
        
        if self.episode_stats['steps'] > 0:
            self.episode_stats['avg_firing_rate'] = (
                self.episode_stats['firing_neurons'] / self.episode_stats['steps']
            )
            self.episode_stats['total_spikes'] /= self.episode_stats['steps']
        
        return self.episode_stats.copy()
    
    def run_episode(self, num_steps: int = 100, timestep: float = 0.1) -> Dict[str, Any]:
        """Run a complete simulation episode.
        
        Args:
            num_steps: Number of simulation steps.
            timestep: Time step duration.
            
        Returns:
            Final episode statistics.
        """
        self.initialize()
        
        # Reset world if provided
        if self.world:
            self.world.reset()
            if self.agent:
                self.agent.initialize(self.world)
        
        for step in range(num_steps):
            self.run_step(step, timestep)
        
        return self.episode_stats.copy()
    
    def get_brain_info(self) -> Dict[str, Any]:
        """Get detailed information about the current brain state.
        
        Returns:
            Dictionary with brain statistics.
        """
        return {
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitatory_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'developmental_stage': self.brain.getDevelopmentalStage(),
            'config_summary': self.brain.getConfig().summary(),
        }
    
    def save_state(self, filepath: str) -> bool:
        """Save the current brain state to a file.
        
        Args:
            filepath: Path to save the checkpoint.
            
        Returns:
            True if successful, False otherwise.
        """
        return self.brain.save(filepath)
    
    def load_state(self, filepath: str) -> bool:
        """Load brain state from a file.
        
        Args:
            filepath: Path to the checkpoint file.
            
        Returns:
            True if successful, False otherwise.
        """
        return self.brain.load(filepath)

# Convenience functions for common use cases

def create_demo_config(neuron_count: int = 500, enable_memory: bool = True) -> Config:
    """Create a demo configuration optimized for testing and demonstration.
    
    Args:
        neuron_count: Number of neurons in the brain.
        enable_memory: Whether to enable memory systems.
        
    Returns:
        A configuration suitable for demos and testing.
    """
    config = create_default_config()
    
    # Basic brain parameters
    config.set("neuron_count", neuron_count)
    config.set("region_count", 1)
    config.set("connection_probability", 0.1)
    
    # Memory system parameters
    if enable_memory:
        config.set("memory.working_memory.capacity", 100.0)
        config.set("memory.episodic_memory.max_episodes", 20)
    
    # Neuromodulation parameters
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.sensitivity", 0.7)
    
    # Plasticity parameters
    config.set("plasticity.stdp.ltp_weight", 0.01)
    config.set("plasticity.stdp.ltd_weight", 0.012)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    
    return config

def create_demo_environment(width: int = 16, height: int = 16,
                           vision_width: int = 8, vision_height: int = 8) -> SimpleWorld:
    """Create a demo environment optimized for agent testing.
    
    Args:
        width: World width.
        height: World height.
        vision_width: Agent's vision width.
        vision_height: Agent's vision height.
        
    Returns:
        A SimpleWorld configured for demo use.
    """
    world = create_simple_world(width, height, vision_width, vision_height)
    world.setMaxEnergy(100.0)
    world.setEnergyDecayRate(0.95)
    return world

# Simulation benchmarks and utilities

class NLMBenchmark:
    """Performance benchmarking utilities for NLM.
    
    This class provides tools to measure and compare the performance
    of different NLM configurations and operations.
    """
    
    @staticmethod
    def benchmark_brain_creation(iterations: int = 10) -> Dict[str, float]:
        """Benchmark brain creation performance.
        
        Args:
            iterations: Number of creation iterations.
            
        Returns:
            Dictionary with timing statistics.
        """
        import time
        
        configs = [create_default_config() for _ in range(iterations)]
        times = []
        
        for config in configs:
            start = time.time()
            brain = create_brain(config)
            end = time.time()
            times.append(end - start)
        
        return {
            'mean_time': sum(times) / len(times),
            'min_time': min(times),
            'max_time': max(times),
            'total_time': sum(times),
            'iterations': iterations,
        }
    
    @staticmethod
    def benchmark_simulation_steps(config: Config, steps: int = 100,
                                  iterations: int = 5) -> Dict[str, float]:
        """Benchmark simulation performance.
        
        Args:
            config: Brain configuration.
            steps: Number of steps per simulation.
            iterations: Number of simulation runs.
            
        Returns:
            Dictionary with performance statistics.
        """
        import time
        
        times = []
        firing_rates = []
        
        for i in range(iterations):
            brain = create_brain(config)
            brain.initialize()
            
            start = time.time()
            for step in range(steps):
                brain.step(step)
            end = time.time()
            
            times.append(end - start)
            firing_rates.append(brain.getAverageFiringRate())
        
        return {
            'mean_time': sum(times) / len(times),
            'min_time': min(times),
            'max_time': max(times),
            'total_time': sum(times),
            'iterations': iterations,
            'steps_per_iteration': steps,
            'mean_firing_rate': sum(firing_rates) / len(firing_rates),
            'steps_per_second': (steps * iterations) / sum(times) if sum(times) > 0 else 0,
        }

# Module-level convenience functions

def get_system_info() -> Dict[str, Any]:
    """Get information about the NLM system configuration.
    
    Returns:
        Dictionary with system information.
    """
    return {
        'version': __version__,
        'author': __author__,
        'license': __license__,
        'description': __description__,
        'available_constants': get_all_constants(),
        'benchmarks': NLMBenchmark,
    }

def get_all_constants() -> Dict[str, Any]:
    """Get all available constants and enumerations.
    
    Returns:
        Dictionary containing all constants and enum values.
    """
    constants = {}
    
    # Neuron types
    constants['NeuronType'] = {
        'Excitatory': NeuronType.Excitatory,
        'Inhibitory': NeuronType.Inhibitory,
        'Modulatory': NeuronType.Modulatory,
        'Sensory': NeuronType.Sensory,
        'Motor': NeuronType.Motor,
        'Internal': NeuronType.Internal,
    }
    
    # Synapse types
    constants['SynapseType'] = {
        'Excitatory': SynapseType.Excitatory,
        'Inhibitory': SynapseType.Inhibitory,
        'Modulatory': SynapseType.Modulatory,
        'Electrical': SynapseType.Electrical,
        'GapJunction': SynapseType.GapJunction,
    }
    
    # Developmental stages
    constants['DevelopmentalStage'] = {
        'Initial': DevelopmentalStage.Initial,
        'CriticalPeriod': DevelopmentalStage.CriticalPeriod,
        'Maturation': DevelopmentalStage.Maturation,
        'Adult': DevelopmentalStage.Adult,
        'Aging': DevelopmentalStage.Aging,
    }
    
    # Firing states
    constants['FiringState'] = {
        'Resting': FiringState.Resting,
        'Active': FiringState.Active,
        'Refractory': FiringState.Refractory,
        'Inhibited': FiringState.Inhibited,
    }
    
    # Action types
    constants['ActionType'] = {
        'MoveForward': ActionType.MoveForward,
        'MoveBackward': ActionType.MoveBackward,
        'MoveLeft': ActionType.MoveLeft,
        'MoveRight': ActionType.MoveRight,
        'TurnLeft': ActionType.TurnLeft,
        'TurnRight': ActionType.TurnRight,
        'Look': ActionType.Look,
        'LookUp': ActionType.LookUp,
        'LookDown': ActionType.LookDown,
        'Interact': ActionType.Interact,
        'Eat': ActionType.Eat,
        'Drink': ActionType.Drink,
        'Rest': ActionType.Rest,
        'Wait': ActionType.Wait,
        'Custom': ActionType.Custom,
    }
    
    # Motor commands
    constants['MotorCommand'] = {
        'MoveForward': MotorCommand.MoveForward,
        'MoveBackward': MotorCommand.MoveBackward,
        'TurnLeft': MotorCommand.TurnLeft,
        'TurnRight': MotorCommand.TurnRight,
        'LookLeft': MotorCommand.LookLeft,
        'LookRight': MotorCommand.LookRight,
        'Interact': MotorCommand.Interact,
        'Wait': MotorCommand.Wait,
    }
    
    # World object types
    constants['WorldObjectType'] = {
        'Empty': WorldObjectType.Empty,
        'Resource': WorldObjectType.Resource,
        'Hazard': WorldObjectType.Hazard,
        'Wall': WorldObjectType.Wall,
        'Marker': WorldObjectType.Marker,
    }
    
    return constants

# Convenience aliases for common operations
def simple_demo(neuron_count: int = 200, steps: int = 50) -> Dict[str, Any]:
    """Simple brain demonstration.
    
    Args:
        neuron_count: Number of neurons.
        steps: Number of simulation steps.
        
    Returns:
        Simulation results.
    """
    config = create_demo_config(neuron_count)
    brain = create_brain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    return {
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'development_stage': brain.getDevelopmentalStage(),
    }

def environment_demo(width: int = 20, height: int = 20,
                    vision_width: int = 8, vision_height: int = 8,
                    num_steps: int = 100) -> Dict[str, Any]:
    """Environment interaction demonstration.
    
    Args:
        width: World width.
        height: World height.
        vision_width: Vision width.
        vision_height: Vision height.
        num_steps: Number of simulation steps.
        
    Returns:
        Simulation results.
    """
    # Create brain and world
    config = create_demo_config(300)
    brain = create_brain(config)
    brain.initialize()
    
    world = create_demo_environment(width, height, vision_width, vision_height)
    
    # Create agent
    agent = create_agent_brain(brain, world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward based on agent position
        reward = 0.0
        if agent.getBrain():
            agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
    
    return {
        'world_width': width,
        'world_height': height,
        'final_firing_rate': brain.getAverageFiringRate(),
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'development_stage': brain.getDevelopmentalStage(),
        'total_spikes': brain.getTotalSpikeCount(),
        'final_agent_x': agent.x,
        'final_agent_y': agent.y,
        'final_agent_orientation': agent.orientation,
    }

# Backward compatibility aliases
createDefaultConfig = create_default_config
createBrain = create_brain
createSimpleWorld = create_simple_world
createAgentBrain = create_agent_brain

# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"
__description__ = "High-level Python API for the NLM (Neural Learning Machine) experimental brain simulation"

# Define types for type hints
from typing import Callable, Optional

BrainInitializer = Callable[[], None]
SimulationStep = Callable[[int, float], Dict[str, Any]]

__all__ = [
    # Core classes
    'Config', 'Brain', 'AgentBrain', 'AgentBody', 'SimpleWorld',
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'WorldObject', 'ActionResult', 'SensoryPercept',
    
    # ID types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    
    # Enumerations
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Factory functions
    'create_default_config', 'create_brain', 'create_simple_world', 
    'create_agent_brain',
    
    # Convenience functions
    'create_demo_config', 'create_demo_environment',
    
    # High-level classes
    'NLMSimulation', 'NLMBenchmark',
    
    # Demos and examples
    'simple_demo', 'environment_demo',
    
    # Utilities
    'get_system_info', 'get_all_constants', 'print_brain_info',
    
    # Module metadata
    '__version__', '__author__', '__license__', '__description__',
    
    # Backward compatibility
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
]