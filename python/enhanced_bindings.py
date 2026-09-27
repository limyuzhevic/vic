"""Enhanced Python bindings for NLM with improved API.

This module provides enhanced bindings that make the NLM framework more accessible
and Pythonic, addressing common issues found in the original bindings.
"""

import numpy as np
import json
import os
from typing import Dict, List, Optional, Union, Any
from contextlib import contextmanager

# Import the basic bindings
from .bindings import (
    # Core types
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

# Custom exception classes for better error handling
class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Error in configuration."""
    pass

class SimulationError(NLMError):
    """Error during simulation."""
    pass

class SerializationError(NLMError):
    """Error during file I/O."""
    pass


class EnhancedConfig:
    """Enhanced Config class with better Pythonic interface."""
    
    def __init__(self, config=None):
        """Initialize with optional existing config."""
        if config is None:
            self._config = Config()
        else:
            self._config = config
    
    def set(self, key: str, value: Any, source="runtime"):
        """Set a configuration value.
        
        Args:
            key: Configuration key (e.g., "brain.neuron_count")
            value: Value to set
            source: Source of configuration (default: "runtime")
        """
        self._config.set(key, value, source)
    
    def get(self, key: str) -> Optional[Any]:
        """Get a configuration value.
        
        Args:
            key: Configuration key
            
        Returns:
            Value or None if key doesn't exist
        """
        return self._config.get(key)
    
    def has(self, key: str) -> bool:
        """Check if configuration key exists.
        
        Args:
            key: Configuration key
            
        Returns:
            True if key exists, False otherwise
        """
        return self._config.has(key)
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys.
        
        Returns:
            List of configuration keys
        """
        return self._config.get_keys()
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value with default.
        
        Args:
            key: Configuration key
            default: Default value if key doesn't exist
            
        Returns:
            Value or default
        """
        return self._config.get_or(key, default)
    
    def clear(self):
        """Clear all configuration entries."""
        self._config.clear()
    
    def save_to_file(self, filepath: str):
        """Save configuration to JSON file.
        
        Args:
            filepath: Path to save file
        """
        self._config.saveToFile(filepath)
    
    def load_from_file(self, filepath: str):
        """Load configuration from JSON file.
        
        Args:
            filepath: Path to load file
        """
        self._config.loadFromFile(filepath)
    
    def summary(self) -> str:
        """Get configuration summary.
        
        Returns:
            Configuration summary string
        """
        return self._config.summary()
    
    def __repr__(self) -> str:
        return f"EnhancedConfig({self.summary()})"
    
    def __str__(self) -> str:
        return self.summary()


class EnhancedBrain:
    """Enhanced Brain class with Pythonic properties and better error handling."""
    
    def __init__(self, config: EnhancedConfig):
        """Initialize brain with enhanced config.
        
        Args:
            config: EnhancedConfig object
        """
        self._config = config
        self._brain = Brain(config._config)
        self._initialized = False
    
    def initialize(self) -> bool:
        """Initialize the brain.
        
        Returns:
            True if successful, False otherwise
        """
        try:
            self._initialized = self._brain.initialize()
            return self._initialized
        except Exception as e:
            raise SimulationError(f"Failed to initialize brain: {e}")
    
    def step(self, step_number: int, time: Optional[float] = None):
        """Perform a simulation step.
        
        Args:
            step_number: Step number for simulation
            time: Optional timestamp
        """
        if not self._initialized:
            raise SimulationError("Brain must be initialized before stepping")
        
        try:
            if time is not None:
                self._brain.step(step_number, time)
            else:
                self._brain.step(step_number)
        except Exception as e:
            raise SimulationError(f"Failed to step brain: {e}")
    
    # Properties for Pythonic access
    @property
    def total_neuron_count(self) -> int:
        """Get total neuron count."""
        return self._brain.getTotalNeuronCount()
    
    @property
    def total_synapse_count(self) -> int:
        """Get total synapse count."""
        return self._brain.getTotalSynapseCount()
    
    @property
    def firing_neuron_count(self) -> int:
        """Get count of currently firing neurons."""
        return self._brain.getFiringNeuronCount()
    
    @property
    def average_firing_rate(self) -> float:
        """Get average firing rate."""
        return self._brain.getAverageFiringRate()
    
    @property
    def total_spike_count(self) -> int:
        """Get total spike count."""
        return self._brain.getTotalSpikeCount()
    
    @property
    def excitation_inhibition_ratio(self) -> float:
        """Get E/I balance ratio."""
        return self._brain.getExcitationInhibitionRatio()
    
    @property
    def developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        return self._brain.getDevelopmentalStage()
    
    @property
    def config(self) -> EnhancedConfig:
        """Get configuration."""
        return self._config
    
    @property
    def region_count(self) -> int:
        """Get number of regions."""
        return self._brain.getRegionCount()
    
    # Save/load methods
    def save(self, filepath: str) -> bool:
        """Save brain state to file.
        
        Args:
            filepath: Path to save file
            
        Returns:
            True if successful, False otherwise
        """
        try:
            result = self._brain.save(filepath)
            if not result:
                raise SerializationError(f"Failed to save brain to {filepath}")
            return True
        except Exception as e:
            raise SerializationError(f"Error saving brain: {e}")
    
    def load(self, filepath: str) -> bool:
        """Load brain state from file.
        
        Args:
            filepath: Path to load file
            
        Returns:
            True if successful, False otherwise
        """
        try:
            result = self._brain.load(filepath)
            if not result:
                raise SerializationError(f"Failed to load brain from {filepath}")
            self._initialized = True
            return True
        except Exception as e:
            raise SerializationError(f"Error loading brain: {e}")
    
    # Additional methods
    def reset(self):
        """Reset brain state."""
        self._brain.reset()
        self._initialized = False
    
    def get_region(self, region_id: RegionId) -> Any:
        """Get region by ID.
        
        Args:
            region_id: Region identifier
            
        Returns:
            Region object
        """
        return self._brain.getRegion(region_id)
    
    def get_region_ids(self) -> List[RegionId]:
        """Get all region IDs.
        
        Returns:
            List of region IDs
        """
        return self._brain.getRegionIds()
    
    def add_region(self, name: str = "") -> RegionId:
        """Add a new region.
        
        Args:
            name: Optional region name
            
        Returns:
            Region ID
        """
        return self._brain.addRegion(name)
    
    def produce_action(self) -> Action:
        """Produce motor action based on neural activity.
        
        Returns:
            Action object
        """
        return self._brain.produceAction()
    
    def receive_sensory_input(self, input_data):
        """Receive sensory input.
        
        Args:
            input_data: Sensory input object
        """
        self._brain.receiveSensoryInput(input_data)
    
    def inject_current(self, neuron_id: NeuronId, current: float):
        """Inject current into specific neuron.
        
        Args:
            neuron_id: Neuron identifier
            current: Current to inject
        """
        self._brain.injectCurrent(neuron_id, current)
    
    def inject_current_to_neurons(self, neuron_type: NeuronType, current: float):
        """Inject current into all neurons of specific type.
        
        Args:
            neuron_type: Neuron type
            current: Current to inject
        """
        self._brain.injectCurrentToNeurons(neuron_type, current)
    
    def log_status(self):
        """Log brain status."""
        self._brain.logStatus()
    
    def __repr__(self) -> str:
        return f"EnhancedBrain(neurons={self.total_neuron_count}, firing={self.firing_neuron_count}, stage={self.developmental_stage})"
    
    def __enter__(self):
        if not self._initialized:
            self.initialize()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.reset()


class EnhancedAgentBrain:
    """Enhanced AgentBrain class with Pythonic interface."""
    
    def __init__(self, brain: EnhancedBrain):
        """Initialize with enhanced brain.
        
        Args:
            brain: EnhancedBrain object
        """
        self._brain = brain._brain
        self._enhanced_brain = brain
    
    def initialize(self, world: EnhancedSimpleWorld):
        """Initialize with world.
        
        Args:
            world: EnhancedSimpleWorld object
        """
        self._brain.initialize(world._world)
    
    # Properties
    @property
    def curiosity_level(self) -> float:
        """Get curiosity level."""
        return self._brain.getCuriosityLevel()
    
    @property
    def novelty_level(self) -> float:
        """Get novelty level."""
        return self._brain.getNoveltyLevel()
    
    @property
    def prediction_error(self) -> float:
        """Get prediction error."""
        return self._brain.getPredictionError()
    
    @property
    def neuromodulation_level(self) -> float:
        """Get neuromodulation level."""
        return self._brain.getNeuromodulationLevel()
    
    @property
    def developmental_stage(self) -> DevelopmentalStage:
        """Get developmental stage."""
        return self._brain.getDevelopmentalStage()
    
    # Subsystem enable/disable methods
    def enable_reward_modulation(self, enable: bool):
        """Enable/disable reward modulation.
        
        Args:
            enable: True to enable, False to disable
        """
        self._brain.enableRewardModulation(enable)
    
    def enable_structural_plasticity(self, enable: bool):
        """Enable/disable structural plasticity.
        
        Args:
            enable: True to enable, False to disable
        """
        self._brain.enableStructuralPlasticity(enable)
    
    def enable_development(self, enable: bool):
        """Enable/disable development.
        
        Args:
            enable: True to enable, False to disable
        """
        self._brain.enableDevelopment(enable)
    
    def enable_curiosity(self, enable: bool):
        """Enable/disable curiosity.
        
        Args:
            enable: True to enable, False to disable
        """
        self._brain.enableCuriosity(enable)
    
    # Convenience methods
    def process_sensory_input(self, percept: SensoryPercept):
        """Process sensory input.
        
        Args:
            percept: Sensory percept object
        """
        self._brain.processSensoryInput(percept)
    
    def decode_motor_command(self) -> Action:
        """Decode motor command from brain activity.
        
        Returns:
            Action object
        """
        return self._brain.decodeMotorCommand()
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float):
        """Apply reward modulation.
        
        Args:
            reward: Actual reward
            predicted_reward: Predicted reward
        """
        self._brain.applyRewardModulation(reward, predicted_reward)
    
    def update_development(self, timestep: float):
        """Update development system.
        
        Args:
            timestep: Time step duration
        """
        self._brain.updateDevelopment(timestep)
    
    def get_brain(self) -> EnhancedBrain:
        """Get the underlying brain.
        
        Returns:
            EnhancedBrain object
        """
        return self._enhanced_brain
    
    def reset(self):
        """Reset agent for new episode."""
        self._brain.reset()


class EnhancedSimpleWorld:
    """Enhanced SimpleWorld class with Pythonic interface."""
    
    def __init__(self):
        """Initialize world."""
        self._world = SimpleWorld()
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int):
        """Configure world dimensions.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision width
            vision_height: Vision height
        """
        self._world.configure(width, height, vision_width, vision_height)
    
    def reset(self):
        """Reset world to initial state."""
        self._world.reset()
    
    def set_agent_start(self, x: float, y: float):
        """Set agent starting position.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        self._world.setAgentStart(x, y)
    
    def update(self, timestep: float):
        """Update world by timestep.
        
        Args:
            timestep: Time step duration
        """
        self._world.update(timestep)
    
    def apply_motor_command(self, cmd: Action, current_time: float):
        """Apply motor command to world.
        
        Args:
            cmd: Motor command
            current_time: Current simulation time
        """
        self._world.applyMotorCommand(cmd, current_time)
    
    def get_sensory_percept(self) -> SensoryPercept:
        """Get sensory percept.
        
        Returns:
            SensoryPercept object
        """
        return self._world.getSensoryPercept()
    
    def get_agent_body(self) -> AgentBody:
        """Get agent body state.
        
        Returns:
            AgentBody object
        """
        return self._world.getAgentBody()
    
    # Properties
    @property
    def width(self) -> int:
        """Get world width."""
        return self._world.getWidth()
    
    @property
    def height(self) -> int:
        """Get world height."""
        return self._world.getHeight()
    
    @property
    def max_energy(self) -> float:
        """Get maximum energy."""
        return self._world.getMaxEnergy()
    
    @property
    def simulation_time(self) -> float:
        """Get simulation time."""
        return self._world.getSimulationTime()
    
    # Configuration methods
    def set_max_energy(self, energy: float):
        """Set maximum energy.
        
        Args:
            energy: Maximum energy value
        """
        self._world.setMaxEnergy(energy)
    
    def set_energy_decay_rate(self, rate: float):
        """Set energy decay rate.
        
        Args:
            rate: Decay rate
        """
        self._world.setEnergyDecayRate(rate)
    
    def set_random_seed(self, seed: int):
        """Set random seed.
        
        Args:
            seed: Random seed
        """
        self._world.setRandomSeed(seed)
    
    def add_object(self, obj: WorldObject):
        """Add world object.
        
        Args:
            obj: World object
        """
        self._world.addObject(obj)
    
    def remove_object(self, x: float, y: float):
        """Remove object at position.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        self._world.removeObject(x, y)
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            True if valid, False otherwise
        """
        return self._world.isValidPosition(x, y)


# Create enhanced factory functions
def create_enhanced_brain(config: Optional[EnhancedConfig] = None) -> EnhancedBrain:
    """Create enhanced brain with optional config.
    
    Args:
        config: EnhancedConfig object. If None, creates default.
        
    Returns:
        EnhancedBrain object
    """
    if config is None:
        config = EnhancedConfig()
    return EnhancedBrain(config)


def create_enhanced_world() -> EnhancedSimpleWorld:
    """Create enhanced world.
    
    Returns:
        EnhancedSimpleWorld object
    """
    return EnhancedSimpleWorld()


def create_enhanced_agent_brain(brain: EnhancedBrain) -> EnhancedAgentBrain:
    """Create enhanced agent brain.
    
    Args:
        brain: EnhancedBrain object
        
    Returns:
        EnhancedAgentBrain object
    """
    return EnhancedAgentBrain(brain)


# Numpy integration utilities
def create_vision_from_numpy(array: np.ndarray, normalize: bool = True) -> Vision:
    """Create Vision object from numpy array.
    
    Args:
        array: 2D numpy array representing vision data
        normalize: Whether to normalize to [0,1] range
        
    Returns:
        Vision object
    """
    if len(array.shape) == 2:
        # Add channels dimension
        array = np.expand_dims(array, axis=-1)
    
    if normalize:
        array = array / 255.0
    
    vision = Vision(array.shape[0], array.shape[1], array.shape[2])
    vision.setData(array)
    return vision


def create_audio_from_numpy(array: np.ndarray, sample_rate: int = 44100) -> Audio:
    """Create Audio object from numpy array.
    
    Args:
        array: 1D numpy array representing audio samples
        sample_rate: Audio sample rate
        
    Returns:
        Audio object
    """
    audio = Audio(sample_rate, len(array))
    audio.setData(array)
    audio.setSampleRate(sample_rate)
    return audio


def array_to_internal_signals(values: List[float]) -> InternalSignals:
    """Create InternalSignals from list of values.
    
    Args:
        values: List of signal values
        
    Returns:
        InternalSignals object
    """
    signals = InternalSignals()
    for value in values:
        signals.addSignal(value)
    return signals


# Simulation helpers
def run_simulation(
    brain: EnhancedBrain,
    world: EnhancedSimpleWorld,
    agent: EnhancedAgentBrain,
    num_steps: int,
    timestep: float = 0.1
) -> Dict[str, Any]:
    """Run complete simulation.
    
    Args:
        brain: EnhancedBrain object
        world: EnhancedSimpleWorld object
        agent: EnhancedAgentBrain object
        num_steps: Number of steps to run
        timestep: Time step duration
        
    Returns:
        Dictionary of statistics
    """
    agent.initialize(world)
    
    stats = {
        'steps': num_steps,
        'final_neurons': 0,
        'total_spikes': 0,
        'average_firing_rate': 0.0,
        'curiosity': 0.0,
        'novelty': 0.0,
        'prediction_error': 0.0,
    }
    
    for step in range(num_steps):
        world.update(timestep)
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        brain.step(step)
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.simulation_time)
        
        # Log stats periodically
        if step % 100 == 0:
            stats['final_neurons'] = brain.firing_neuron_count
            stats['total_spikes'] = brain.total_spike_count
            stats['average_firing_rate'] = brain.average_firing_rate
            stats['curiosity'] = agent.curiosity_level
            stats['novelty'] = agent.novelty_level
            stats['prediction_error'] = agent.prediction_error
    
    return stats


# Context managers
@contextmanager
def brain_simulation(config: Optional[EnhancedConfig] = None, num_steps: int = 1000):
    """Context manager for brain simulation.
    
    Example:
        with brain_simulation(num_steps=500) as brain:
            for i in range(500):
                brain.step(i)
    """
    brain = create_enhanced_brain(config)
    brain.initialize()
    try:
        yield brain
    finally:
        brain.reset()


@contextmanager
def world_simulation(width: int = 20, height: int = 20, 
                      vision_width: int = 8, vision_height: int = 8):
    """Context manager for world simulation.
    
    Example:
        with world_simulation(width=10, height=10) as world:
            world.configure(10, 10, 8, 8)
            world.reset()
            # Run simulation...
    """
    world = create_enhanced_world()
    world.configure(width, height, vision_width, vision_height)
    world.reset()
    try:
        yield world
    finally:
        world.reset()


# Export all classes and functions
__all__ = [
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
    'EnhancedConfig',
    'Brain',
    'EnhancedBrain',
    'AgentBrain',
    'EnhancedAgentBrain',
    'SimpleWorld',
    'EnhancedSimpleWorld',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    'SensoryPercept',
    
    # Exception classes
    'NLMError',
    'ConfigurationError',
    'SimulationError',
    'SerializationError',
    
    # Factory functions
    'create_enhanced_brain',
    'create_enhanced_world',
    'create_enhanced_agent_brain',
    'create_brain',
    'create_default_config',
    'create_simple_world',
    'create_agent_brain',
    
    # Numpy integration
    'create_vision_from_numpy',
    'create_audio_from_numpy',
    'array_to_internal_signals',
    
    # Simulation helpers
    'run_simulation',
    
    # Context managers
    'brain_simulation',
    'world_simulation',
    
    # Batch utilities
    'BatchSimulator',
    'BrainContext',
    'WorldContext',
]
