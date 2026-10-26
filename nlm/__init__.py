# Enhanced NLM Python Interface

"""
Enhanced Python interface for NLM (Neural Learning Machine) neural simulation framework.

This module provides a comprehensive Python API for simulating neural networks
with biologically realistic properties, including multiple plasticity mechanisms,
memory systems, neuromodulation, and developmental processes.

The enhanced interface provides two main ways to use NLM:
1. Simple/Classic API (backward compatible)
2. Enhanced/High-level API (new features and utilities)
"""

__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"

import warnings
from typing import Optional, List, Tuple, Dict, Any, Union, Iterator, ContextManager
from contextlib import contextmanager
import numpy as np
import sys

# Import core NLM classes (delegates to C++ extensions when available)
from .core import (
    # Core classes
    Config, Brain, AgentBrain, SimpleWorld,
    SensoryInput, Vision, Audio, InternalSignals,
    Action, WorldObject, AgentBody, ActionResult,
    SensoryPercept,
    
    # Type definitions
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage,
    FiringState, ActionType, MotorCommand,
    WorldObjectType,
    
    # Constants
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID,
    INVALID_REGION_ID, INVALID_POPULATION_ID,
    
    # Factory functions
    create_default_config, create_brain,
    
    # Extensions availability
    C_EXTENSIONS_AVAILABLE
)

# Enhanced exception hierarchy
class NLMError(Exception):
    """Base exception for all NLM-related errors."""
    pass

class ConfigurationError(NLMError):
    """Raised when configuration is invalid or missing."""
    pass

class InitializationError(NLMError):
    """Raised when NLM components fail to initialize."""
    pass

class SimulationError(NLMError):
    """Raised when simulation encounters an error."""
    pass
# Utility functions for validation and error handling
def validate_positive(value: float, param_name: str) -> float:
    """
    Validate that a numeric value is positive.
    
    Args:
        value: The value to validate
        param_name: Name of the parameter for error message
        
    Returns:
        The validated value
        
    Raises:
        ValueError: If value is not positive
    """
    if value < 0:
        raise ValueError(f"{param_name} must be positive, got {value}")
    return value

def validate_range(value: float, min_val: float, max_val: float, param_name: str) -> float:
    """
    Validate that a value is within a specified range.
    
    Args:
        value: The value to validate
        min_val: Minimum allowed value
        max_val: Maximum allowed value
        param_name: Name of the parameter for error message
        
    Returns:
        The validated value
        
    Raises:
        ValueError: If value is outside the specified range
    """
    if value < min_val or value > max_val:
        raise ValueError(f"{param_name} must be between {min_val} and {max_val}, got {value}")
    return value
# Context managers for resource management
@contextmanager
def brain_context(config: Optional[Config] = None):
    """
    Context manager for automatic brain lifecycle management.
    
    Example:
        with brain_context() as brain:
            for step in range(1000):
                brain.step(step)
            print(f"Simulation completed: {brain.getTotalSpikeCount()} spikes")
    
    Args:
        config: Optional configuration (uses default if not provided)
        
    Yields:
        Initialized Brain object
    """
    brain = None
    try:
        brain = create_brain(config)
        yield brain
    finally:
        if brain:
            brain.reset()
@contextmanager
def simulation_episode(brain: Brain, world: SimpleWorld, agent: AgentBrain,
                         num_steps: int) -> Iterator[Dict[str, Any]]:
    """
    Context manager for running simulation episodes with automatic cleanup.
    
    Args:
        brain: Neural brain
        world: Simulation environment  
        agent: Agent interface
        num_steps: Number of simulation steps to run
        
    Yields:
        Episode statistics dictionary (updated during simulation)
        
    Example:
        brain, world, agent = create_educational_simulation()
        
        with simulation_episode(brain, world, agent, 1000) as stats:
            # Simulation runs with automatic cleanup
            # stats is available for monitoring
            pass
    """
    episode_stats = None
    try:
        episode_stats = run_episode(brain, world, agent, num_steps)
        yield episode_stats
    finally:
        # Automatic cleanup
        agent.reset()
        world.reset()
# Configuration builder for fluent configuration API
class ConfigBuilder:
    """
    Builder class for creating NLM configurations with validation.
    
    Provides a fluent API for creating complex configurations with parameter
    validation and helpful error messages.
    """
    
    def __init__(self):
        """Initialize empty configuration builder."""
        self._config_data = {}
        self._config_file_path = None
        self._validate = True
    
    def set_neuron_count(self, count: int) -> 'ConfigBuilder':
        """
        Set number of neurons (default: 1000).
        
        Args:
            count: Number of neurons
            
        Returns:
            Self for method chaining
        """
        if self._validate:
            count = validate_positive(count, "neuron_count")
        self._config_data['neuron_count'] = count
        return self
    
    def set_region_count(self, count: int) -> 'ConfigBuilder':
        """
        Set number of brain regions (default: 1).
        
        Args:
            count: Number of regions
            
        Returns:
            Self for method chaining
        """
        if self._validate:
            count = validate_positive(count, "region_count")
        self._config_data['region_count'] = count
        return self
    
    def set_connection_probability(self, prob: float) -> 'ConfigBuilder':
        """
        Set synaptic connection probability (default: 0.1, range: 0.0-1.0).
        
        Args:
            prob: Connection probability
            
        Returns:
            Self for method chaining
        """
        if self._validate:
            prob = validate_range(prob, 0.0, 1.0, "connection_probability")
        self._config_data['connection_probability'] = prob
        return self
    
    def set_simulation_timestep(self, dt: float) -> 'ConfigBuilder':
        """
        Set simulation timestep in milliseconds (default: 0.001).
        
        Args:
            dt: Simulation timestep
            
        Returns:
            Self for method chaining
        """
        if self._validate:
            dt = validate_positive(dt, "simulation_timestep")
        self._config_data['simulation_timestep'] = dt
        return self
    
    def set_random_seed(self, seed: int) -> 'ConfigBuilder':
        """
        Set random seed for reproducible simulations (default: 42).
        
        Args:
            seed: Random seed
            
        Returns:
            Self for method chaining
        """
        if self._validate:
            seed = validate_positive(seed, "random_seed")
        self._config_data['random_seed'] = seed
        return self
    
    def enable_stdp(self, enabled: bool = True, ltp_weight: float = 0.01,
                    ltd_weight: float = 0.012, tau: float = 20.0) -> 'ConfigBuilder':
        """
        Enable Spike-Timing-Dependent Plasticity (default: True).
        
        Args:
            enabled: Whether STDP is enabled
            ltp_weight: Long-term potentiation weight
            ltd_weight: Long-term depression weight
            tau: STDP time constant
            
        Returns:
            Self for method chaining
        """
        if self._validate:
            ltp_weight = validate_positive(ltp_weight, "stdp_ltp_weight")
            ltd_weight = validate_positive(ltd_weight, "stdp_ltd_weight")
            tau = validate_positive(tau, "stdp_tau")
        
        self._config_data['plasticity.stdp.enable'] = enabled
        self._config_data['stdp.ltp_weight'] = ltp_weight
        self._config_data['stdp.ltd_weight'] = ltd_weight
        self._config_data['stdp.tau'] = tau
        return self
    
    def enable_hebbian(self, enabled: bool = True) -> 'ConfigBuilder':
        """
        Enable Hebbian learning (default: True).
        
        Args:
            enabled: Whether Hebbian learning is enabled
            
        Returns:
            Self for method chaining
        """
        self._config_data['plasticity.hebbian.enable'] = enabled
        return self
    
    def enable_structural_plasticity(self, enabled: bool = True) -> 'ConfigBuilder':
        """
        Enable structural plasticity (default: True).
        
        Args:
            enabled: Whether structural plasticity is enabled
            
        Returns:
            Self for method chaining
        """
        self._config_data['plasticity.structural.enable'] = enabled
        return self
    
    def enable_curiosity(self, enabled: bool = True) -> 'ConfigBuilder':
        """
        Enable curiosity-driven exploration (default: True).
        
        Args:
            enabled: Whether curiosity is enabled
            
        Returns:
            Self for method chaining
        """
        self._config_data['neuromod.curiosity.enable'] = enabled
        return self
    
    def enable_novelty(self, enabled: bool = True) -> 'ConfigBuilder':
        """
        Enable novelty detection (default: True).
        
        Args:
            enabled: Whether novelty detection is enabled
            
        Returns:
            Self for method chaining
        """
        self._config_data['neuromod.novelty.enable'] = enabled
        return self
    
    def enable_reward_modulation(self, enabled: bool = True) -> 'ConfigBuilder':
        """
        Enable reward modulation (default: True).
        
        Args:
            enabled: Whether reward modulation is enabled
            
        Returns:
            Self for method chaining
        """
        self._config_data['neuromod.dopamine.enable'] = enabled
        return self
    
    def set_replay_interval(self, steps: int) -> 'ConfigBuilder':
        """
        Set memory replay interval in steps (default: 100).
        
        Args:
            steps: Replay interval
            
        Returns:
            Self for method chaining
        """
        if self._validate:
            steps = validate_positive(steps, "replay_interval")
        self._config_data['replay_interval'] = steps
        return self
    
    def set_development_interval(self, steps: int) -> 'ConfigBuilder':
        """
        Set development update interval in steps (default: 1000).
        
        Args:
            steps: Development interval
            
        Returns:
            Self for method chaining
        """
        if self._validate:
            steps = validate_positive(steps, "development_interval")
        self._config_data['development_interval'] = steps
        return self
    
    def set_checkpoint_dir(self, path: str) -> 'ConfigBuilder':
        """
        Set checkpoint directory path (default: './checkpoints').
        
        Args:
            path: Checkpoint directory path
            
        Returns:
            Self for method chaining
        """
        self._config_data['checkpoint_dir'] = path
        return self
    
    def set_max_checkpoints(self, count: int) -> 'ConfigBuilder':
        """
        Set maximum number of checkpoints to keep (default: 10).
        
        Args:
            count: Maximum number of checkpoints
            
        Returns:
            Self for method chaining
        """
        if self._validate:
            count = validate_positive(count, "max_checkpoints")
        self._config_data['max_checkpoints'] = count
        return self
    
    def with_simulation_mode(self, mode: str = 'continuous') -> 'ConfigBuilder':
        """
        Set simulation mode (continuous, episodic, or interactive).
        
        Args:
            mode: Simulation mode
            
        Returns:
            Self for method chaining
            
        Raises:
            ValueError: If mode is not one of the valid options
        """
        if self._validate and mode not in ['continuous', 'episodic', 'interactive']:
            raise ValueError("mode must be 'continuous', 'episodic', or 'interactive'")
        self._config_data['simulation_mode'] = mode
        return self
    
    def disable_validation(self) -> 'ConfigBuilder':
        """
        Disable parameter validation (faster but less safe).
        
        Returns:
            Self for method chaining
        """
        self._validate = False
        return self
    
    def build(self) -> 'Config':
        """
        Build and return the configuration object.
        
        Returns:
            Configured Config object
        """
        config = create_default_config()
        
        # Apply all settings
        for key, value in self._config_data.items():
            config.set(key, value)
        
        # Apply file path if specified
        if self._config_file_path:
            config.loadFromFile(self._config_file_path)
        
        return config
    
    def load_from_file(self, filepath: str) -> 'ConfigBuilder':
        """
        Load configuration from file and apply to this builder.
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            Self for method chaining
        """
        self._config_file_path = filepath
        return self
    
    def merge_with(self, other_config: 'Config') -> 'ConfigBuilder':
        """
        Merge with existing configuration (other_config overrides this builder's settings).
        
        Args:
            other_config: Existing configuration to merge with
            
        Returns:
            Self for method chaining
        """
        if hasattr(other_config, 'getKeys'):
            for key in other_config.getKeys():
                if key not in self._config_data:
                    self._config_data[key] = other_config.get(key)
        return self
# Factory functions for creating NLM components
def create_brain(config: Optional['Config'] = None) -> 'Brain':
    """
    Create a new NLM brain with optional configuration.
    
    Args:
        config: NLM configuration. If None, uses default configuration.
    
    Returns:
        Initialized Brain object.
    
    Raises:
        ConfigurationError: If configuration is invalid
        InitializationError: If brain fails to initialize
    """
    if config is None:
        config = create_default_config()
    
    try:
        brain = Brain(config)
        if not brain.initialize():
            raise InitializationError("Brain failed to initialize")
        return brain
    except Exception as e:
        if isinstance(e, (ConfigurationError, InitializationError)):
            raise
        raise InitializationError(f"Failed to create brain: {e}")
def create_agent(brain: 'Brain', world: 'SimpleWorld') -> 'AgentBrain':
    """
    Create an agent brain interface that connects brain to world.
    
    Args:
        brain: The neural brain to control
        world: The environment to interact with
    
    Returns:
        Initialized AgentBrain object.
    """
    agent = AgentBrain(brain)
    agent.initialize(world)
    return agent
def create_simulation_environment(width: float = 20.0, height: float = 20.0,
                                  vision_width: int = 16, vision_height: int = 16) -> 'SimpleWorld':
    """
    Create a simple 2D simulation environment.
    
    Args:
        width: World width in meters (default: 20.0)
        height: World height in meters (default: 20.0)
        vision_width: Agent vision width in pixels (default: 16)
        vision_height: Agent vision height in pixels (default: 16)
    
    Returns:
        Configured SimpleWorld object.
    """
    world = SimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    return world
def create_educational_simulation() -> Tuple['Brain', 'SimpleWorld', 'AgentBrain']:
    """
    Create a complete educational simulation with a learning agent.
    
    Returns:
        Tuple of (brain, world, agent) ready for simulation.
    
    Example:
        brain, world, agent = create_educational_simulation()
        
        for step in range(500):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        print(f"Final state: {agent.getNeuromodulationLevel():.2f} neuromodulation,")
        print(f"  {brain.getTotalSpikeCount()} total spikes,")
        print(f"  curiosity: {agent.getCuriosityLevel():.2f}")
    """
    # Create default configuration for educational simulation
    config = create_default_config()
    
    # Create brain with moderate complexity
    config.set('neuron_count', 500)
    config.set('region_count', 2)
    config.set('connection_probability', 0.15)
    
    # Create environment
    world = create_simulation_environment(vision_width=8, vision_height=8)
    
    # Create brain and agent
    brain = create_brain(config)
    agent = create_agent(brain, world)
    
    # Enable key subsystems for learning
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    
    return brain, world, agent
def run_episode(brain: 'Brain', world: 'SimpleWorld', agent: 'AgentBrain',
                num_steps: int, verbose: bool = False) -> Dict[str, Any]:
    """
    Run a complete simulation episode.
    
    Args:
        brain: Neural brain
        world: Simulation environment
        agent: Agent interface
        num_steps: Number of simulation steps to run
        verbose: If True, print progress information
    
    Returns:
        Dictionary containing episode statistics and final state.
    
    Raises:
        SimulationError: If simulation fails
    """
    if num_steps < 0:
        raise ValueError(f"num_steps must be non-negative, got {num_steps}")
    
    try:
        # Reset world to initial state
        world.reset()
        agent.reset()
        
        # Run simulation loop
        episode_stats = {
            'steps_completed': 0,
            'total_spikes': 0,
            'final_neuromodulation': 0.0,
            'final_curiosity': 0.0,
            'final_novelty': 0.0,
            'final_prediction_error': 0.0,
            'action_counts': {},
            'performance_metrics': {}
        }
        
        action_counts = {}
        
        for step in range(num_steps):
            # Update world environment
            world.update(0.1)
            
            # Get sensory input from world
            percept = world.getSensoryPercept()
            
            # Process sensory input in agent/brain
            agent.processSensoryInput(percept)
            brain.step(step)
            
            # Decode motor command from brain activity
            action = agent.decodeMotorCommand()
            
            # Apply motor command to world
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation if enabled
            reward = result.reward
            predicted_reward = 0.0  # Could be implemented with prediction system
            agent.applyRewardModulation(reward, predicted_reward)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Track action counts
            action_name = action.name if hasattr(action, 'name') else str(action)
            action_counts[action_name] = action_counts.get(action_name, 0) + 1
            
            # Update episode statistics
            episode_stats['total_spikes'] += brain.getFiringNeuronCount()
            episode_stats['final_neuromodulation'] = agent.getNeuromodulationLevel()
            episode_stats['final_curiosity'] = agent.getCuriosityLevel()
            episode_stats['final_novelty'] = agent.getNoveltyLevel()
            episode_stats['final_prediction_error'] = agent.getPredictionError()
            
            if verbose and step % 100 == 0:
                print(f"Step {step}: "
                      f"Spikes: {brain.getFiringNeuronCount()}, "
                      f"Actions: {action_name}, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}")
        
        episode_stats['steps_completed'] = num_steps
        episode_stats['action_counts'] = action_counts
        episode_stats['final_spikes_per_step'] = episode_stats['total_spikes'] / num_steps if num_steps > 0 else 0.0
        episode_stats['simulation_efficiency'] = episode_stats['final_neuromodulation'] * episode_stats['final_curiosity']
        
        return episode_stats
        
    except Exception as e:
        if isinstance(e, SimulationError):
            raise
        raise SimulationError(f"Episode simulation failed at step {step}: {e}")
class NLM:
    """
    Main NLM interface class for high-level simulation control.
    
    This class provides a simplified, high-level API for common use cases
    while maintaining access to low-level NLM features.
    """
    
    def __init__(self, config: Optional['Config'] = None):
        """
        Initialize NLM with optional configuration.
        
        Args:
            config: NLM configuration. If None, uses default configuration.
        """
        self.config = config or create_default_config()
        self.brain = None
        self.agent = None
        self.world = None
        self._initialized = False
    
    def setup(self, world_width: float = 20.0, world_height: float = 20.0,
              enable_learning: bool = True) -> 'NLM':
        """
        Setup NLM components.
        
        Args:
            world_width: Simulation world width
            world_height: Simulation world height
            enable_learning: If True, enable learning subsystems
        
        Returns:
            Self for method chaining.
        """
        # Create brain
        self.brain = create_brain(self.config)
        
        # Create world
        self.world = create_simulation_environment(world_width, world_height)
        
        # Create agent
        self.agent = create_agent(self.brain, self.world)
        
        # Enable learning subsystems if requested
        if enable_learning:
            self.agent.enable_reward_modulation(True)
            self.agent.enable_curiosity(True)
            self.agent.enable_structural_plasticity(True)
            self.agent.enable_development(True)
        
        self._initialized = True
        return self
    
    def run(self, num_steps: int, verbose: bool = False) -> Dict[str, Any]:
        """
        Run simulation.
        
        Args:
            num_steps: Number of simulation steps to run
            verbose: If True, print progress information
        
        Returns:
            Simulation statistics.
        """
        if not self._initialized:
            raise InitializationError("NLM not initialized. Call setup() first.")
        
        return run_episode(self.brain, self.world, self.agent, num_steps, verbose)
    
    def reset(self):
        """Reset all NLM components to initial state."""
        if self.agent:
            self.agent.reset()
        if self.world:
            self.world.reset()
        if self.brain:
            self.brain.reset()
        self._initialized = False
    
    def __enter__(self) -> 'NLM':
        """Context manager entry."""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit - cleanup resources."""
        self.reset()
    
    def __repr__(self) -> str:
        if self._initialized:
            return (f"<NLM: brain={self.brain.getTotalNeuronCount()} neurons, "
                   f"world={self.world.getWidth():.1f}x{self.world.getHeight():.1f}, "
                   f"agent initialized>")
        return "<NLM: not initialized>"
# Pre-configured simulation modes

def create_research_config() -> 'Config':
    """Create configuration optimized for research simulations."""
    config = create_default_config()
    config.set('neuron_count', 2000)
    config.set('region_count', 3)
    config.set('connection_probability', 0.2)
    config.set('plasticity.stdp.enable', True)
    config.set('plasticity.hebbian.enable', True)
    config.set('plasticity.structural.enable', True)
    config.set('neuromod.curiosity.enable', True)
    config.set('neuromod.novelty.enable', True)
    config.set('neuromod.dopamine.enable', True)
    config.set('replay_interval', 50)
    config.set('development_interval', 500)
    return config
def create_educational_config() -> 'Config':
    """Create configuration optimized for educational demonstrations."""
    config = create_default_config()
    config.set('neuron_count', 500)
    config.set('region_count', 2)
    config.set('connection_probability', 0.15)
    config.set('plasticity.stdp.enable', True)
    config.set('plasticity.hebbian.enable', True)
    config.set('neuromod.curiosity.enable', True)
    config.set('neuromod.novelty.enable', True)
    config.set('replay_interval', 100)
    config.set('development_interval', 1000)
    return config
def create_simple_config() -> 'Config':
    """Create minimal configuration for quick demonstrations."""
    config = create_default_config()
    config.set('neuron_count', 200)
    config.set('region_count', 1)
    config.set('connection_probability', 0.1)
    config.set('plasticity.stdp.enable', False)  # Disable for speed
    config.set('neuromod.curiosity.enable', False)
    return config
# Performance optimization utilities
class PerformanceMonitor:
    """
    Utility class for monitoring NLM simulation performance.
    
    Tracks execution time, memory usage, and key metrics for optimization.
    """
    
    def __init__(self):
        self.reset()
    
    def reset(self):
        """Reset performance counters."""
        self.step_times = []
        self.memory_usage = []
        self.neuron_counts = []
        self.spike_counts = []
        self.plasticity_events = []
        self.start_time = None
    
    def start_step(self):
        """Mark start of simulation step."""
        import time
        self.start_time = time.time()
    
    def end_step(self, brain: 'Brain', world: Optional['SimpleWorld'] = None):
        """Mark end of simulation step and record metrics."""
        import time
        
        if self.start_time is None:
            return
        
        # Record step time
        step_time = time.time() - self.start_time
        self.step_times.append(step_time)
        
        # Record brain metrics
        self.neuron_counts.append(brain.getTotalNeuronCount())
        self.spike_counts.append(brain.getTotalSpikeCount())
        
        # Record world metrics if available
        if world:
            self.memory_usage.append(world.getSimulationTime())
        
        self.start_time = None
    
    def get_summary(self) -> Dict[str, Any]:
        """
        Get performance summary statistics.
        
        Returns:
            Dictionary containing performance metrics.
        """
        if not self.step_times:
            return {
                'steps_completed': 0,
                'avg_step_time_ms': 0.0,
                'min_step_time_ms': 0.0,
                'max_step_time_ms': 0.0,
                'std_step_time_ms': 0.0,
                'spikes_per_second': 0.0,
                'efficiency_score': 0.0
            }
        
        import statistics
        
        avg_step_time = statistics.mean(self.step_times)
        if len(self.step_times) > 1:
            step_time_std = statistics.stdev(self.step_times)
        else:
            step_time_std = 0.0
        
        min_step_time = min(self.step_times)
        max_step_time = max(self.step_times)
        
        # Calculate derived metrics
        total_time = sum(self.step_times)
        steps_per_second = len(self.step_times) / total_time if total_time > 0 else 0
        
        avg_spikes = statistics.mean(self.spike_counts) if self.spike_counts else 0
        
        # Efficiency score: spikes per second per neuron
        efficiency_score = (avg_spikes * steps_per_second / 
                           (statistics.mean(self.neuron_counts) if self.neuron_counts else 1))
        
        return {
            'steps_completed': len(self.step_times),
            'avg_step_time_ms': avg_step_time * 1000,
            'min_step_time_ms': min_step_time * 1000,
            'max_step_time_ms': max_step_time * 1000,
            'std_step_time_ms': step_time_std * 1000,
            'spikes_per_second': avg_spikes * steps_per_second,
            'efficiency_score': efficiency_score,
            'avg_neurons': statistics.mean(self.neuron_counts) if self.neuron_counts else 0
        }
def create_performance_monitor() -> PerformanceMonitor:
    """Create a performance monitor for NLM simulations."""
    return PerformanceMonitor()
# Example usage and demonstrations

EXAMPLES = {
    'simple_brain': """
# Create a simple brain and run simulation
brain = create_brain()
brain.initialize()

for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
""",
    
    'learning_agent': """
# Create a complete learning agent simulation
brain, world, agent = create_educational_simulation()

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
stats = runEpisode(brain, world, agent, 500, verbose=True)

print(f"Episode completed in {stats['stepsCompleted']} steps")
print(f"Total spikes: {stats['total_spikes']}")
print(f"Final neuromodulation level: {stats['final_neuromodulation']:.3f}")
print(f"Final curiosity level: {stats['final_curiosity']:.3f}")
print(f"Average spikes per step: {stats['final_spikes_per_step']:.1f}")
""",
    
    'high_level_api': """
# Use the high-level NLM API
nlm = NLM()
nlm.setup(world_width=30.0, world_height=30.0, enable_learning=True)

stats = nlm.run(1000, verbose=True)
print(f"Simulation completed: {stats['efficiency_score']:.4f} efficiency score")
""",
    
    'performance_monitor': """
# Monitor simulation performance
from nlm import create_performance_monitor

nlm = NLM()
nlm.setup(enable_learning=True)

monitor = create_performance_monitor()

for step in range(100):
    monitor.start_step()
    nlm.run(1)
    monitor.end_step(nlm.brain, nlm.world)

# Get performance summary
perf_stats = monitor.get_summary()
print(f"Performance summary: {perf_stats}")
""",
    
    'configuration_builder': """
# Use configuration builder for custom setups
from nlm import ConfigBuilder

config = (ConfigBuilder()
         .set_neuron_count(1000)
         .set_region_count(3)
         .set_connection_probability(0.2)
         .enable_stdp(True)
         .enable_curiosity(True)
         .enable_structural_plasticity(True)
         .set_simulation_timestep(0.001)
         .set_random_seed(42)
         .build())

brain = create_brain(config)
"""
}

# Public API exports
__all__ = [
    # Core classes
    'NLM', 'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'WorldObject', 'AgentBody', 'ActionResult',
    'SensoryPercept',
    
    # Exceptions
    'NLMError', 'ConfigurationError', 'InitializationError', 'SimulationError',
    
    # Factory functions
    'create_brain', 'create_agent', 'create_simulation_environment',
    'create_educational_simulation', 'create_default_config',
    
    # Context managers
    'brain_context', 'simulation_episode', 'NLM',
    
    # Configuration builders
    'ConfigBuilder', 'create_research_config', 'create_educational_config', 'create_simple_config',
    
    # Performance utilities
    'PerformanceMonitor', 'create_performance_monitor',
    
    # Example usage
    'EXAMPLES',
    
    # Version and metadata
    '__version__', '__author__', '__license__'
]

# Module initialization
if C_EXTENSIONS_AVAILABLE:
    print(f"NLM Python interface loaded successfully (v{__version__})")
    print(f"C++ extensions available: {C_EXTENSIONS_AVAILABLE}")
else:
    print(f"Warning: NLM Python interface loaded with stub implementation (v{__version__})")
    print("For full functionality, please build NLM with C++ extensions")

# Print welcome message
print("\n" + "="*60)
print("NLM (熙然) - Neural Learning Machine")
print("An experimental artificial developmental brain")
print("="*60)
print(f"\nVersion: {__version__}")
print(f"Author: {__author__}")
print(f"License: {__license__}")
print(f"\nAvailable features:")
print("  - Real spiking neural network simulation")
print("  - Multiple plasticity mechanisms (STDP, Hebbian, structural)")
print("  - Memory systems (working, episodic, associative)")
print("  - Neuromodulation (dopamine, curiosity, prediction error)")
print("  - Developmental processes")
print("  - Perception and action loops")
print("\nFor examples, see EXAMPLES dictionary in this module")
print("For help, see the documentation or examples directory")