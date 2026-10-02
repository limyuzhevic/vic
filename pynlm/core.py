"""
Core neural simulation components for NLM Python bindings

This module provides the main neural simulation components including
brain management, configuration, and basic world simulation.
"""

import pybind11
import pybind11.stl
from pybind11 import literal
from .types import *
import json
import yaml
import logging
from datetime import datetime
from typing import Dict, List, Any, Optional, Callable
import numpy as np

# Import the NLM C++ bindings
import pynlm.core_ext as core_ext

# Re-export key C++ classes for convenience
NeuronId = core_ext.NeuronId
SynapseId = core_ext.SynapseId
RegionId = core_ext.RegionId
PopulationId = core_ext.PopulationId
NeuronType = core_ext.NeuronType
SynapseType = core_ext.SynapseType
DevelopmentalStage = core_ext.DevelopmentalStage
FiringState = core_ext.FiringState
ActionType = core_ext.ActionType
MotorCommand = core_ext.MotorCommand
WorldObjectType = core_ext.WorldObjectType

class Config:
    """
    Enhanced configuration system with Pythonic interface.

    This class provides a comprehensive configuration management system
    for NLM simulations, supporting JSON, YAML, and command-line formats
    with validation and preset management.
    """

    def __init__(self, config=None):
        """
        Initialize configuration.

        Args:
            config: Optional existing Config object or dictionary
        """
        if config is None:
            self._config = core_ext.create_default_config()
        elif isinstance(config, Config):
            self._config = config._config
        else:
            self._config = core_ext.create_default_config()
            if isinstance(config, dict):
                self.load_from_dict(config)

    def load_from_file(self, filepath: str) -> bool:
        """
        Load configuration from a file.

        Automatically detects format based on file extension
        (.json, .yaml, .yml, .cfg, .txt).

        Args:
            filepath: Path to configuration file

        Returns:
            bool: True if successful, False otherwise
        """
        return self._config.loadFromFile(filepath)

    def load_from_dict(self, config_dict: Dict[str, Any]) -> bool:
        """
        Load configuration from a Python dictionary.

        Args:
            config_dict: Dictionary containing configuration parameters

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            # Convert dict to JSON string
            json_str = json.dumps(config_dict)
            return self._config.loadFromJSON(json_str)
        except Exception as e:
            logging.error(f"Error loading from dict: {e}")
            return False

    def load_from_yaml(self, filepath: str) -> bool:
        """
        Load configuration from a YAML file.

        Args:
            filepath: Path to YAML configuration file

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            with open(filepath, 'r') as f:
                yaml_data = yaml.safe_load(f)
            return self.load_from_dict(yaml_data)
        except Exception as e:
            logging.error(f"Error loading from YAML: {e}")
            return False

    def load_from_args(self, args: List[str]) -> bool:
        """
        Load configuration from command-line arguments.

        Supports both --key=value and -k value formats.

        Args:
            args: List of command-line arguments

        Returns:
            bool: True if successful, False otherwise
        """
        return self._config.loadFromArgs(len(args), args)

    def save_to_file(self, filepath: str, format: Optional[str] = None) -> bool:
        """
        Save configuration to a file.

        Args:
            filepath: Path to save configuration
            format: Optional format (json, yaml, cfg). If None, detected from extension

        Returns:
            bool: True if successful, False otherwise
        """
        return self._config.saveToFile(filepath)

    def save_to_json(self, filepath: str) -> bool:
        """
        Save configuration to a JSON file.

        Args:
            filepath: Path to JSON configuration file

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            json_data = self._config.toJSON()
            with open(filepath, 'w') as f:
                json.dump(json_data, f, indent=2)
            return True
        except Exception as e:
            logging.error(f"Error saving to JSON: {e}")
            return False

    def save_to_yaml(self, filepath: str) -> bool:
        """
        Save configuration to a YAML file.

        Args:
            filepath: Path to YAML configuration file

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            yaml_data = self._config.toYAML()
            with open(filepath, 'w') as f:
                yaml.dump(yaml_data, f, default_flow_style=False)
            return True
        except Exception as e:
            logging.error(f"Error saving to YAML: {e}")
            return False

    def get(self, key: str, default: Any = None) -> Any:
        """
        Get a configuration value.

        Args:
            key: Configuration key
            default: Default value if key not found

        Returns:
            Configuration value or default
        """
        value = self._config.get(key)
        return value if value is not None else default

    def get_int(self, key: str, default: int = 0) -> int:
        """Get integer configuration value."""
        value = self.get(key)
        return int(value) if value is not None else default

    def get_float(self, key: str, default: float = 0.0) -> float:
        """Get float configuration value."""
        value = self.get(key)
        return float(value) if value is not None else default

    def get_string(self, key: str, default: str = "") -> str:
        """Get string configuration value."""
        value = self.get(key)
        return str(value) if value is not None else default

    def get_bool(self, key: str, default: bool = False) -> bool:
        """Get boolean configuration value."""
        value = self.get(key)
        return bool(value) if value is not None else default

    def set(self, key: str, value: Any) -> None:
        """
        Set a configuration value.

        Args:
            key: Configuration key
            value: Configuration value
        """
        if isinstance(value, int):
            self._config.set(key, value)
        elif isinstance(value, float):
            self._config.set(key, value)
        elif isinstance(value, str):
            self._config.set(key, value)
        elif isinstance(value, bool):
            self._config.set(key, value)
        else:
            self._config.set(key, str(value))

    def has(self, key: str) -> bool:
        """Check if configuration contains a key."""
        return self._config.has(key)

    def remove(self, key: str) -> None:
        """Remove a configuration key."""
        self._config.remove(key)

    def keys(self) -> List[str]:
        """Get all configuration keys."""
        return self._config.getKeys()

    def clear(self) -> None:
        """Clear all configuration."""
        self._config.clear()

    def validate(self) -> List[str]:
        """
        Validate configuration.

        Returns:
            List[str]: List of validation errors, empty if valid
        """
        return self._config.validate()

    def summary(self) -> str:
        """Get configuration summary."""
        return self._config.summary()

    def __repr__(self) -> str:
        return f"<Config: {self.summary()}>"

    def __getitem__(self, key: str) -> Any:
        value = self.get(key)
        if value is None:
            raise KeyError(f"Key '{key}' not found in configuration")
        return value

    def __setitem__(self, key: str, value: Any) -> None:
        self.set(key, value)

    def __contains__(self, key: str) -> bool:
        return self.has(key)

    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        return self._config.toDict()

    def clone(self) -> 'Config':
        """Create a copy of the configuration."""
        return Config(self._config.clone())

    def update(self, other: Dict[str, Any] or 'Config') -> None:
        """Update configuration from another config or dictionary."""
        if isinstance(other, Config):
            for key in other.keys():
                self.set(key, other.get(key))
        elif isinstance(other, dict):
            for key, value in other.items():
                self.set(key, value)


class Brain:
    """
    Enhanced neural simulation brain with Pythonic interface.

    This class provides access to the C++ Brain implementation with
    additional Python features for research workflows, debugging,
    and performance monitoring.
    """

    def __init__(self, config: Optional[Config] = None):
        """
        Initialize brain with configuration.

        Args:
            config: Configuration object (uses default if None)
        """
        if config is None:
            config = Config()
        self._brain = core_ext.create_brain(config._config)
        self._config = config
        self._debug_config = NeuroDebugConfig()
        self._performance_metrics = PerformanceMetrics()

    def initialize(self) -> bool:
        """
        Initialize the brain with configuration.

        Returns:
            bool: True if initialization successful, False otherwise
        """
        success = self._brain.initialize()
        if success:
            self._performance_metrics.initialization_time = 0.0  # Would need timing
        return success

    def step(self, steps: int = 1, **inject_currents) -> None:
        """
        Perform simulation steps.

        Args:
            steps: Number of simulation steps to perform
            **inject_currents: Currents to inject (neuron_id=amount)
        """
        start_time = datetime.now()
        
        # Inject currents if specified
        for neuron_id, current in inject_currents.items():
            if isinstance(neuron_id, NeuronId):
                self._brain.inject_current(neuron_id, current)
            else:
                # Assume integer neuron ID
                self._brain.inject_current(NeuronId(neuron_id), current)

        for step in range(steps):
            self._brain.step(step)

        # Update performance metrics
        end_time = datetime.now()
        duration = (end_time - start_time).total_seconds()
        self._performance_metrics.simulation_time += duration

    def reset(self) -> None:
        """Reset brain to initial state."""
        self._brain.reset()

    def save(self, filepath: str) -> bool:
        """
        Save brain state to file.

        Args:
            filepath: Path to save brain state

        Returns:
            bool: True if save successful, False otherwise
        """
        return self._brain.save(filepath)

    def load(self, filepath: str) -> bool:
        """
        Load brain state from file.

        Args:
            filepath: Path to load brain state from

        Returns:
            bool: True if load successful, False otherwise
        """
        return self._brain.load(filepath)

    def add_region(self, name: str = "") -> RegionId:
        """Add a new neural region."""
        return self._brain.add_region(name)

    def get_region(self, region_id: RegionId) -> Any:
        """Get region by ID."""
        return self._brain.get_region(region_id)

    def get_region_count(self) -> int:
        """Get number of regions."""
        return self._brain.get_region_count()

    def get_region_ids(self) -> List[RegionId]:
        """Get all region IDs."""
        return self._brain.get_region_ids()

    def get_regions(self) -> List[Any]:
        """Get all regions."""
        return self._brain.get_regions()

    def get_total_neuron_count(self) -> int:
        """Get total neuron count across all regions."""
        return self._brain.get_total_neuron_count()

    def get_total_synapse_count(self) -> int:
        """Get total synapse count across all regions."""
        return self._brain.get_total_synapse_count()

    def get_active_neuron_count(self) -> int:
        """Get count of active neurons."""
        return self._brain.get_active_neuron_count()

    def get_firing_neuron_count(self) -> int:
        """Get count of currently firing neurons."""
        return self._brain.get_firing_neuron_count()

    def get_average_firing_rate(self) -> float:
        """Get average firing rate across all neurons."""
        return self._brain.get_average_firing_rate()

    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition balance ratio."""
        return self._brain.get_excitation_inhibition_ratio()

    def get_total_spike_count(self) -> int:
        """Get total spike count."""
        return self._brain.get_total_spike_count()

    def get_pending_spike_event_count(self) -> int:
        """Get count of pending spike events."""
        return self._brain.get_pending_spike_event_count()

    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        return self._brain.get_developmental_stage()

    def set_developmental_stage(self, stage: DevelopmentalStage) -> None:
        """Set developmental stage."""
        self._brain.set_developmental_stage(stage)

    def get_config(self) -> Config:
        """Get the configuration."""
        return Config(self._brain.get_config())

    def log_status(self) -> None:
        """Log brain status."""
        self._brain.log_status()

    def get_spike_system(self) -> Any:
        """Get spike system reference."""
        return self._brain.get_spike_system()

    def get_stdp(self) -> Any:
        """Get STDP plasticity system reference."""
        return self._brain.get_stdp()

    def get_hebbian(self) -> Any:
        """Get Hebbian plasticity system reference."""
        return self._brain.get_hebbian()

    def get_structural_plasticity(self) -> Any:
        """Get structural plasticity system reference."""
        return self._brain.get_structural_plasticity()

    def produce_action(self) -> Any:
        """Produce motor action based on neural activity."""
        return self._brain.produce_action()

    def get_performance_metrics(self) -> PerformanceMetrics:
        """Get current performance metrics."""
        return self._performance_metrics

    def enable_debug(self, config: Optional[NeuroDebugConfig] = None) -> None:
        """
        Enable debug logging.

        Args:
            config: Debug configuration (uses default if None)
        """
        if config is not None:
            self._debug_config = config
        # Would set up debug logging here

    def inject_current(self, neuron_id: NeuronId, current: float) -> None:
        """Inject current into a specific neuron."""
        self._brain.inject_current(neuron_id, current)

    def inject_current_to_neurons(self, neuron_type: NeuronType, current: float) -> None:
        """Inject current into all neurons of a specific type."""
        self._brain.inject_current_to_neurons(neuron_type, current)

    def receive_sensory_input(self, input_data: List[float]) -> None:
        """
        Inject sensory input into the brain.

        Args:
            input_data: Sensory input values
        """
        # This would require a proper SensoryInput object
        # For now, just log
        logging.warning("Sensory input injection not fully implemented")

    def get_working_memory(self) -> Any:
        """Get working memory system reference."""
        return self._brain.get_working_memory()

    def get_episodic_memory(self) -> Any:
        """Get episodic memory system reference."""
        return self._brain.get_episodic_memory()

    def get_associative_memory(self) -> Any:
        """Get associative memory system reference."""
        return self._brain.get_associative_memory()

    def get_prediction_system(self) -> Any:
        """Get prediction system reference."""
        return self._brain.get_prediction_system()

    def get_planner(self) -> Any:
        """Get neural planner reference."""
        return self._brain.get_planner()

    def get_concept_formation(self) -> Any:
        """Get concept formation reference."""
        return self._brain.get_concept_formation()

    def get_attention(self) -> Any:
        """Get attention system reference."""
        return self._brain.get_attention()

    def get_development_system(self) -> Any:
        """Get development system reference."""
        return self._brain.get_development_system()

    def get_dopamine(self) -> Any:
        """Get dopamine neuromodulator reference."""
        return self._brain.get_dopamine()

    def get_curiosity(self) -> Any:
        """Get curiosity neuromodulator reference."""
        return self._brain.get_curiosity()

    def get_prediction_error(self) -> Any:
        """Get prediction error neuromodulator reference."""
        return self._brain.get_prediction_error_signal()

    def get_random_generator(self) -> Any:
        """Get random generator reference."""
        return self._brain.get_random_generator()

    def __repr__(self) -> str:
        return f"<Brain: {self.get_total_neuron_count()} neurons, {self.get_total_synapse_count()} synapses>"


class SimpleWorld:
    """
    Simple 2D world for NLM simulation with Python interface.

    This class provides a simulated environment where agents can interact
    with objects, receive sensory input, and produce motor actions.
    """

    def __init__(self):
        """Initialize simple world."""
        self._world = core_ext.create_simple_world()
        self._simulation_time = 0.0

    def configure(self, width: int, height: int, vision_width: int, vision_height: int) -> None:
        """
        Configure world dimensions and vision.

        Args:
            width: World width
            height: World height
            vision_width: Vision sensor width
            vision_height: Vision sensor height
        """
        self._world.configure(width, height, vision_width, vision_height)

    def reset(self) -> None:
        """Reset world to initial state."""
        self._world.reset()

    def set_agent_start(self, x: float, y: float) -> None:
        """
        Set agent starting position.

        Args:
            x: Starting x coordinate
            y: Starting y coordinate
        """
        self._world.set_agent_start(x, y)

    def update(self, timestep: float) -> None:
        """
        Update world state.

        Args:
            timestep: Time step for update
        """
        self._world.update(timestep)
        self._simulation_time += timestep

    def apply_motor_command(self, command: Any, current_time: float) -> Any:
        """
        Apply motor command to the world.

        Args:
            command: Motor command
            current_time: Current simulation time

        Returns:
            ActionResult: Result of action execution
        """
        return self._world.apply_motor_command(command, current_time)

    def get_sensory_percept(self) -> Any:
        """Get current sensory percept."""
        return self._world.get_sensory_percept()

    def get_agent_body(self) -> Any:
        """Get current agent body state."""
        return self._world.get_agent_body()

    def add_object(self, obj: Any) -> None:
        """
        Add object to the world.

        Args:
            obj: World object to add
        """
        self._world.add_object(obj)

    def remove_object(self, x: float, y: float) -> None:
        """
        Remove object at position.

        Args:
            x: X coordinate
            y: Y coordinate
        """
        self._world.remove_object(x, y)

    def is_valid_position(self, x: float, y: float) -> bool:
        """
        Check if position is valid.

        Args:
            x: X coordinate
            y: Y coordinate

        Returns:
            bool: True if position is valid
        """
        return self._world.is_valid_position(x, y)

    def get_width(self) -> int:
        """Get world width."""
        return self._world.get_width()

    def get_height(self) -> int:
        """Get world height."""
        return self._world.get_height()

    def get_max_energy(self) -> float:
        """Get maximum agent energy."""
        return self._world.get_max_energy()

    def set_max_energy(self, energy: float) -> None:
        """Set maximum agent energy."""
        self._world.set_max_energy(energy)

    def get_energy_decay_rate(self) -> float:
        """Get energy decay rate."""
        return self._world.get_energy_decay_rate()

    def set_energy_decay_rate(self, rate: float) -> None:
        """Set energy decay rate."""
        self._world.set_energy_decay_rate(rate)

    def get_simulation_time(self) -> float:
        """Get current simulation time."""
        return self._simulation_time

    def set_random_seed(self, seed: int) -> None:
        """Set random seed."""
        self._world.set_random_seed(seed)

    def get_random_seed(self) -> int:
        """Get random seed."""
        return self._world.get_random_seed()

    def __repr__(self) -> str:
        return f"<SimpleWorld: {self.get_width()}x{self.get_height()} at t={self.get_simulation_time()}>"


class AgentBrain:
    """
    Agent brain interface connecting NLM brain to world.

    This class provides the interface between the neural brain and
    the simulated world, handling sensory input and motor output.
    """

    def __init__(self, brain: Brain):
        """
        Initialize agent brain interface.

        Args:
            brain: Neural brain instance
        """
        self._agent_brain = core_ext.create_agent_brain(brain._brain)
        self._brain = brain
        self._reward_modulation_enabled = True
        self._structural_plasticity_enabled = True
        self._development_enabled = True
        self._curiosity_enabled = True

    def initialize(self, world: SimpleWorld) -> bool:
        """
        Initialize with world.

        Args:
            world: World instance

        Returns:
            bool: True if initialization successful
        """
        return self._agent_brain.initialize(world._world)

    def get_sensory_input_size(self) -> int:
        """Get expected sensory input size."""
        return self._agent_brain.get_sensory_input_size()

    def get_motor_output_size(self) -> int:
        """Get expected motor output size."""
        return self._agent_brain.get_motor_output_size()

    def process_sensory_input(self, percept: Any) -> None:
        """
        Process sensory percept and inject into brain.

        Args:
            percept: Sensory percept data
        """
        self._agent_brain.process_sensory_input(percept)

    def decode_motor_command(self) -> Any:
        """Decode brain motor activity into motor command."""
        return self._agent_brain.decode_motor_command()

    def apply_reward_modulation(self, reward: float, predicted_reward: float) -> None:
        """
        Apply reward-based neuromodulation.

        Args:
            reward: Actual reward received
            predicted_reward: Expected reward
        """
        self._agent_brain.apply_reward_modulation(reward, predicted_reward)

    def update_development(self, timestep: float) -> None:
        """
        Update development system.

        Args:
            timestep: Time step for development update
        """
        self._agent_brain.update_development(timestep)

    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        return self._agent_brain.get_developmental_stage()

    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level."""
        return self._agent_brain.get_neuromodulation_level()

    def get_curiosity_level(self) -> float:
        """Get curiosity level."""
        return self._agent_brain.get_curiosity_level()

    def get_novelty_level(self) -> float:
        """Get novelty level."""
        return self._agent_brain.get_novelty_level()

    def get_prediction_error(self) -> float:
        """Get prediction error."""
        return self._agent_brain.get_prediction_error()

    def reset(self) -> None:
        """Reset agent for new episode."""
        self._agent_brain.reset()

    def get_brain(self) -> Brain:
        """Get the underlying brain."""
        return self._brain

    def enable_reward_modulation(self, enable: bool) -> None:
        """Enable/disable reward modulation."""
        self._reward_modulation_enabled = enable
        self._agent_brain.enable_reward_modulation(enable)

    def enable_structural_plasticity(self, enable: bool) -> None:
        """Enable/disable structural plasticity."""
        self._structural_plasticity_enabled = enable
        self._agent_brain.enable_structural_plasticity(enable)

    def enable_development(self, enable: bool) -> None:
        """Enable/disable development."""
        self._development_enabled = enable
        self._agent_brain.enable_development(enable)

    def enable_curiosity(self, enable: bool) -> None:
        """Enable/disable curiosity."""
        self._curiosity_enabled = enable
        self._agent_brain.enable_curiosity(enable)

    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled."""
        return self._reward_modulation_enabled

    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled."""
        return self._structural_plasticity_enabled

    def is_development_enabled(self) -> bool:
        """Check if development is enabled."""
        return self._development_enabled

    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled."""
        return self._curiosity_enabled

    def __repr__(self) -> str:
        return f"<AgentBrain connected to {self._brain}>"


# Factory functions for common creation patterns

def create_default_config() -> Config:
    """Create a default configuration."""
    return Config()


def create_brain(config: Optional[Config] = None) -> Brain:
    """
    Create a new brain with configuration.

    Args:
        config: Configuration object (uses default if None)

    Returns:
        Brain: Neural brain instance
    """
    return Brain(config)


def create_simple_world() -> SimpleWorld:
    """Create a new simple world."""
    return SimpleWorld()


def create_agent_brain(brain: Brain) -> AgentBrain:
    """
    Create a new agent brain interface.

    Args:
        brain: Neural brain instance

    Returns:
        AgentBrain: Agent brain interface
    """
    return AgentBrain(brain)

# Global convenience functions

_config = None
_brain = None
_world = None
_agent_brain = None


def set_config(config: Config) -> None:
    """Set global configuration."""
    global _config
    _config = config


def get_config() -> Config:
    """Get global configuration."""
    return _config


def set_brain(brain: Brain) -> None:
    """Set global brain."""
    global _brain
    _brain = brain


def get_brain() -> Brain:
    """Get global brain."""
    return _brain


def set_world(world: SimpleWorld) -> None:
    """Set global world."""
    global _world
    _world = world


def get_world() -> SimpleWorld:
    """Get global world."""
    return _world


def set_agent_brain(agent: AgentBrain) -> None:
    """Set global agent brain."""
    global _agent_brain
    _agent_brain = agent


def get_agent_brain() -> AgentBrain:
    """Get global agent brain."""
    return _agent_brain

# Initialize logging
logging.basicConfig(level=logging.INFO)

# Export key classes and functions
__all__ = [
    'Config', 'Brain', 'SimpleWorld', 'AgentBrain',
    'create_default_config', 'create_brain', 'create_simple_world', 'create_agent_brain',
    'set_config', 'get_config', 'set_brain', 'get_brain',
    'set_world', 'get_world', 'set_agent_brain', 'get_agent_brain'
]