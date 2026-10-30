"""
Configuration management for NLM Python bindings.

This module provides configuration classes and utilities for configuring
the brain, world, and other NLM components.
"""

import json
from typing import Any, Dict, List, Optional
from enum import Enum


class ConfigSource(Enum):
    """Source of configuration values."""
    DEFAULT = "default"
    USER = "user"
    CONFIG_FILE = "config_file"
    COMMAND_LINE = "command_line"
    RUNTIME = "runtime"


class ConfigKey:
    """Keys for common configuration values."""
    
    # Brain configuration
    BRAIN_NEURON_COUNT = "brain.neuron_count"
    BRAIN_REGION_COUNT = "brain.region_count"
    BRAIN_SYNAPSE_DENSITY = "brain.synapse_density"
    BRAIN_CONNECTION_PROBABILITY = "brain.connection_probability"
    BRAIN_INITIAL_WEIGHT_MEAN = "brain.initial_weight_mean"
    BRAIN_INITIAL_WEIGHT_STD = "brain.initial_weight_std"
    BRAIN_V_THRESH = "brain.v_thresh"
    BRAIN_V_REST = "brain.v_rest"
    BRAIN_V_RESET = "brain.v_reset"
    BRAIN_TAU_MEM = "brain.tau_mem"
    BRAIN_TAU_REF = "brain.tau_ref"
    BRAIN_SPIKE_DELAY = "brain.spike_delay"
    BRAIN_SYNAPSE_DELAY = "brain.synapse_delay"
    BRAIN_MAX_FIRE_RATE = "brain.max_fire_rate"
    
    # Plasticity configuration
    PLASTICITY_LEARNING_RATE = "plasticity.learning_rate"
    PLASTICITY_STDP_ENABLE = "plasticity.stdp.enable"
    PLASTICITY_STDP_LEARNING_RATE = "plasticity.stdp.learning_rate"
    PLASTICITY_STDP_TAU_PLUS = "plasticity.stdp.tau_plus"
    PLASTICITY_STDP_TAU_MINUS = "plasticity.stdp.tau_minus"
    PLASTICITY_STDP_TRACE_DECAY = "plasticity.stdp.trace_decay"
    PLASTICITY_HEBBIAN_ENABLE = "plasticity.hebbian.enable"
    PLASTICITY_HEBBIAN_LEARNING_RATE = "plasticity.hebbian.learning_rate"
    PLASTICITY_STRUCTURAL_ENABLE = "plasticity.structural.enable"
    PLASTICITY_STRUCTURAL_SYNAPTOGENESIS_RATE = "plasticity.structural.synaptogenesis_rate"
    PLASTICITY_STRUCTURAL_PRUNING_RATE = "plasticity.structural.pruning_rate"
    
    # Neuromodulation configuration
    NEUROMOD_DOPAMINE_SCALE = "neuromod.dopamine.scale"
    NEUROMOD_DOPAMINE_LEARNING_RATE = "neuromod.dopamine.learning_rate"
    NEUROMOD_DOPAMINE_BASELINE = "neuromod.dopamine.baseline"
    NEUROMOD_CURIOUSITY_ENABLE = "neuromod.curiosity.enable"
    NEUROMOD_CURIOUSITY_THRESHOLD = "neuromod.curiosity.threshold"
    NEUROMOD_NOVELTY_ENABLE = "neuromod.novelty.enable"
    NEUROMOD_NOVELTY_SENSITIVITY = "neuromod.novelty.sensitivity"
    
    # Development configuration
    DEVELOPMENT_SYNAPTOGENESIS_RATE = "development.synaptogenesis_rate"
    DEVELOPMENT_PRUNING_RATE = "development.pruning_rate"
    DEVELOPMENT_MATURATION_RATE = "development.maturation_rate"
    DEVELOPMENT_CRITICAL_PERIOD_START = "development.critical_period_start"
    DEVELOPMENT_CRITICAL_PERIOD_END = "development.critical_period_end"
    
    # Memory configuration
    MEMORY_WORKING_CAPACITY = "memory.working.capacity"
    MEMORY_WORKING_DECAY = "memory.working.decay"
    MEMORY_EPISODIC_MAX_EPISODES = "memory.episodic.max_episodes"
    MEMORY_EPISODIC_COMPRESSION = "memory.episodic.compression"
    MEMORY_SEMANTIC_MAX_NODES = "memory.semantic.max_nodes"
    MEMORY_PROCEDURAL_MAX_PATTERNS = "memory.procedural.max_patterns"
    
    # Prediction configuration
    PREDICTION_ENABLE = "prediction.enable"
    PREDICTION_HORIZON = "prediction.horizon"
    PREDICTION_ACCURACY_THRESHOLD = "prediction.accuracy_threshold"
    PREDICTION_ERROR_DECAY = "prediction.error_decay"
    
    # Cognition configuration
    COGNITION_PLANNER_ENABLED = "cognition.planner.enabled"
    COGNITION_PLANNER_SEARCH_DEPTH = "cognition.planner.search_depth"
    COGNITION_CONCEPT_FORMATION_ENABLED = "cognition.concept_formation.enabled"
    COGNITION_CONCEPT_FORMATION_THRESHOLD = "cognition.concept_formation.threshold"
    COGNITION_ATTENTION_ENABLED = "cognition.attention.enabled"
    COGNITION_ATTENTION_SELECTIVITY = "cognition.attention.selectivity"
    
    # Environment configuration
    ENVIRONMENT_WIDTH = "environment.width"
    ENVIRONMENT_HEIGHT = "environment.height"
    ENVIRONMENT_TERRAIN_TYPE = "environment.terrain_type"
    ENVIRONMENT_OBSTACLE_DENSITY = "environment.obstacle_density"
    
    # Simulation configuration
    SIMULATION_TIMESTEP = "simulation.timestep"
    SIMULATION_TIME_LIMIT = "simulation.time_limit"
    SIMULATION_MAX_STEPS = "simulation.max_steps"
    SIMULATION_RANDOM_SEED = "simulation.random_seed"
    
    # Logging configuration
    LOG_LEVEL = "log.level"
    LOG_TO_FILE = "log.to_file"
    LOG_FILENAME = "log.filename"
    LOG_MAX_SIZE = "log.max_size"
    LOG_BACKUP_COUNT = "log.backup_count"
    
    # Visualization configuration
    VISUALIZATION_ENABLED = "visualization.enabled"
    VISUALIZATION_UPDATE_RATE = "visualization.update_rate"
    VISUALIZATION_WIDTH = "visualization.width"
    VISUALIZATION_HEIGHT = "visualization.height"
    VISUALIZATION_SCALE = "visualization.scale"


class Config:
    """
    Configuration manager for NLM components.
    
    This class provides a flexible way to configure the brain, world,
    and other NLM components through key-value pairs.
    """
    
    def __init__(self):
        """Initialize an empty configuration."""
        self._values: Dict[str, Any] = {}
        self._defaults: Dict[str, Any] = {}
        self._sources: Dict[str, ConfigSource] = {}
    
    def set(self, key: str, value: Any, source: ConfigSource = ConfigSource.USER):
        """
        Set a configuration value.
        
        Args:
            key (str): Configuration key (e.g., "brain.neuron_count")
            value: Configuration value
            source (ConfigSource): Source of the configuration
        """
        self._values[key] = value
        self._sources[key] = source
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get a configuration value.
        
        Args:
            key (str): Configuration key
            default: Default value if key not found
            
        Returns:
            Configuration value or default
        """
        return self._values.get(key, default)
    
    def getOr(self, key: str, default: Any) -> Any:
        """
        Get a configuration value, or return default if not found.
        
        This is a convenience method for type-specific defaults.
        
        Args:
            key (str): Configuration key
            default: Default value
            
        Returns:
            Configuration value or default
        """
        return self._values.get(key, default)
    
    def getInt(self, key: str, default: int = 0) -> int:
        """Get an integer configuration value."""
        value = self._values.get(key, default)
        try:
            return int(value)
        except (ValueError, TypeError):
            return default
    
    def getFloat(self, key: str, default: float = 0.0) -> float:
        """Get a float configuration value."""
        value = self._values.get(key, default)
        try:
            return float(value)
        except (ValueError, TypeError):
            return default
    
    def getString(self, key: str, default: str = "") -> str:
        """Get a string configuration value."""
        value = self._values.get(key, default)
        return str(value)
    
    def getBool(self, key: str, default: bool = False) -> bool:
        """Get a boolean configuration value."""
        value = self._values.get(key, default)
        if isinstance(value, bool):
            return value
        elif isinstance(value, str):
            return value.lower() in ['true', 'yes', '1', 'on']
        elif isinstance(value, int):
            return value != 0
        return default
    
    def has(self, key: str) -> bool:
        """
        Check if configuration has the given key.
        
        Args:
            key (str): Configuration key
            
        Returns:
            True if key exists, False otherwise
        """
        return key in self._values
    
    def getKeys(self) -> List[str]:
        """
        Get all configuration keys.
        
        Returns:
            List of configuration keys
        """
        return list(self._values.keys())
    
    def getSource(self, key: str) -> Optional[ConfigSource]:
        """
        Get the source of a configuration value.
        
        Args:
            key (str): Configuration key
            
        Returns:
            Configuration source or None if not found
        """
        return self._sources.get(key)
    
    def getAll(self) -> Dict[str, Any]:
        """
        Get all configuration values.
        
        Returns:
            Dictionary of all configuration values
        """
        return self._values.copy()
    
    def clear(self):
        """Clear all configuration values."""
        self._values.clear()
        self._sources.clear()
    
    def loadFromFile(self, filename: str) -> bool:
        """
        Load configuration from a file.
        
        Supports JSON, YAML, and TOML formats (if available).
        
        Args:
            filename (str): Path to configuration file
            
        Returns:
            True if successful, False otherwise
        """
        try:
            # Try JSON first
            with open(filename, 'r') as f:
                data = json.load(f)
                for key, value in data.items():
                    self.set(key, value, ConfigSource.CONFIG_FILE)
            return True
        except (FileNotFoundError, json.JSONDecodeError):
            return False
    
    def saveToFile(self, filename: str) -> bool:
        """
        Save configuration to a file.
        
        Supports JSON format.
        
        Args:
            filename (str): Path to save configuration file
            
        Returns:
            True if successful, False otherwise
        """
        try:
            with open(filename, 'w') as f:
                json.dump(self._values, f, indent=2)
            return True
        except Exception:
            return False
    
    def merge(self, other: 'Config', override: bool = True):
        """
        Merge another configuration into this one.
        
        Args:
            other (Config): Configuration to merge
            override (bool): Whether to override existing values
        """
        for key, value in other._values.items():
            if override or key not in self._values:
                self.set(key, value, other._sources.get(key, ConfigSource.RUNTIME))
    
    def remove(self, key: str):
        """
        Remove a configuration key.
        
        Args:
            key (str): Configuration key to remove
        """
        self._values.pop(key, None)
        self._sources.pop(key, None)
    
    def copy(self) -> 'Config':
        """
        Create a copy of this configuration.
        
        Returns:
            Copy of this configuration
        """
        new_config = Config()
        new_config._values = self._values.copy()
        new_config._sources = self._sources.copy()
        return new_config
    
    def __contains__(self, key: str) -> bool:
        """Check if configuration contains a key."""
        return key in self._values
    
    def __getitem__(self, key: str) -> Any:
        """Get configuration value using dictionary syntax."""
        return self.get(key)
    
    def __setitem__(self, key: str, value: Any):
        """Set configuration value using dictionary syntax."""
        self.set(key, value)
    
    def __repr__(self) -> str:
        """String representation of configuration."""
        return f"Config({len(self._values)} values)"
    
    def __len__(self) -> int:
        """Number of configuration values."""
        return len(self._values)


# Convenience functions for common configurations

def createDefaultConfig() -> Config:
    """
    Create a default configuration for a brain.
    
    Returns:
        Default configuration
    """
    config = Config()
    
    # Brain settings
    config.set(ConfigKey.BRAIN_NEURON_COUNT, 1000)
    config.set(ConfigKey.BRAIN_REGION_COUNT, 1)
    config.set(ConfigKey.BRAIN_SYNAPSE_DENSITY, 0.1)
    config.set(ConfigKey.BRAIN_CONNECTION_PROBABILITY, 0.1)
    config.set(ConfigKey.BRAIN_INITIAL_WEIGHT_MEAN, 0.5)
    config.set(ConfigKey.BRAIN_INITIAL_WEIGHT_STD, 0.1)
    config.set(ConfigKey.BRAIN_V_THRESH, -50.0)
    config.set(ConfigKey.BRAIN_V_REST, -70.0)
    config.set(ConfigKey.BRAIN_V_RESET, -75.0)
    config.set(ConfigKey.BRAIN_TAU_MEM, 20.0)
    config.set(ConfigKey.BRAIN_TAU_REF, 2.0)
    
    # Plasticity settings
    config.set(ConfigKey.PLASTICITY_LEARNING_RATE, 0.01)
    config.set(ConfigKey.PLASTICITY_STDP_ENABLE, True)
    config.set(ConfigKey.PLASTICITY_STDP_LEARNING_RATE, 0.001)
    config.set(ConfigKey.PLASTICITY_STDP_TAU_PLUS, 20.0)
    config.set(ConfigKey.PLASTICITY_STDP_TAU_MINUS, 20.0)
    config.set(ConfigKey.PLASTICITY_HEBBIAN_ENABLE, True)
    config.set(ConfigKey.PLASTICITY_HEBBIAN_LEARNING_RATE, 0.01)
    config.set(ConfigKey.PLASTICITY_STRUCTURAL_ENABLE, True)
    config.set(ConfigKey.PLASTICITY_STRUCTURAL_SYNAPTOGENESIS_RATE, 0.001)
    config.set(ConfigKey.PLASTICITY_STRUCTURAL_PRUNING_RATE, 0.0001)
    
    # Neuromodulation settings
    config.set(ConfigKey.NEUROMOD_DOPAMINE_SCALE, 1.0)
    config.set(ConfigKey.NEUROMOD_DOPAMINE_LEARNING_RATE, 0.01)
    config.set(ConfigKey.NEUROMOD_DOPAMINE_BASELINE, 0.0)
    config.set(ConfigKey.NEUROMOD_CURIOUSITY_ENABLE, True)
    config.set(ConfigKey.NEUROMOD_CURIOUSITY_THRESHOLD, 0.5)
    config.set(ConfigKey.NEUROMOD_NOVELTY_ENABLE, True)
    config.set(ConfigKey.NEUROMOD_NOVELTY_SENSITIVITY, 0.5)
    
    # Development settings
    config.set(ConfigKey.DEVELOPMENT_SYNAPTOGENESIS_RATE, 0.001)
    config.set(ConfigKey.DEVELOPMENT_PRUNING_RATE, 0.0001)
    config.set(ConfigKey.DEVELOPMENT_MATURATION_RATE, 0.1)
    
    # Memory settings
    config.set(ConfigKey.MEMORY_WORKING_CAPACITY, 100)
    config.set(ConfigKey.MEMORY_WORKING_DECAY, 0.9)
    config.set(ConfigKey.MEMORY_EPISODIC_MAX_EPISODES, 100)
    config.set(ConfigKey.MEMORY_SEMANTIC_MAX_NODES, 50)
    
    # Prediction settings
    config.set(ConfigKey.PREDICTION_ENABLE, True)
    config.set(ConfigKey.PREDICTION_HORIZON, 5)
    config.set(ConfigKey.PREDICTION_ACCURACY_THRESHOLD, 0.7)
    
    # Simulation settings
    config.set(ConfigKey.SIMULATION_TIMESTEP, 0.001)
    config.set(ConfigKey.SIMULATION_MAX_STEPS, 10000)
    config.set(ConfigKey.SIMULATION_RANDOM_SEED, 42)
    
    # Logging settings
    config.set(ConfigKey.LOG_LEVEL, "INFO")
    config.set(ConfigKey.LOG_TO_FILE, False)
    config.set(ConfigKey.LOG_FILENAME, "nlm.log")
    
    return config


def createPhase2Config() -> Config:
    """
    Create Phase 2 configuration (real neural computation).
    
    Returns:
        Phase 2 configuration
    """
    config = createDefaultConfig()
    config.set(ConfigKey.BRAIN_NEURON_COUNT, 500)  # Smaller for faster tests
    config.set(ConfigKey.BRAIN_REGION_COUNT, 1)
    config.set(ConfigKey.PLASTICITY_STDP_LEARNING_RATE, 0.02)
    config.set(ConfigKey.PLASTICITY_STDP_TAU_PLUS, 20.0)
    config.set(ConfigKey.PLASTICITY_STDP_TAU_MINUS, 15.0)
    config.set(ConfigKey.PLASTICITY_STRUCTURAL_SYNAPTOGENESIS_RATE, 0.0001)
    config.set(ConfigKey.PLASTICITY_STRUCTURAL_PRUNING_RATE, 0.00001)
    return config


def createPhase3Config() -> Config:
    """
    Create Phase 3 configuration (world interaction).
    
    Returns:
        Phase 3 configuration
    """
    config = createDefaultConfig()
    config.set(ConfigKey.BRAIN_NEURON_COUNT, 500)
    config.set(ConfigKey.BRAIN_REGION_COUNT, 2)
    config.set(ConfigKey.BRAIN_CONNECTION_PROBABILITY, 0.1)
    config.set(ConfigKey.PLASTICITY_STDP_LEARNING_RATE, 0.01)
    config.set(ConfigKey.PLASTICITY_STDP_TAU_PLUS, 20.0)
    config.set(ConfigKey.PLASTICITY_STDP_TAU_MINUS, 20.0)
    config.set(ConfigKey.ENVIRONMENT_WIDTH, 20)
    config.set(ConfigKey.ENVIRONMENT_HEIGHT, 20)
    config.set(ConfigKey.VISUALIZATION_WIDTH, 16)
    config.set(ConfigKey.VISUALIZATION_HEIGHT, 16)
    return config


def createPhase6Config() -> Config:
    """
    Create Phase 6 configuration (integration).
    
    Returns:
        Phase 6 configuration
    """
    config = createDefaultConfig()
    config.set(ConfigKey.BRAIN_NEURON_COUNT, 500)
    config.set(ConfigKey.BRAIN_REGION_COUNT, 1)
    config.set(ConfigKey.BRAIN_CONNECTION_PROBABILITY, 0.1)
    config.set(ConfigKey.SIMULATION_MAX_STEPS, 2000)
    config.set(ConfigKey.PLASTICITY_STRUCTURAL_SYNAPTOGENESIS_RATE, 0.0001)
    config.set(ConfigKey.PLASTICITY_STRUCTURAL_PRUNING_RATE, 0.00001)
    config.set(ConfigKey.DEVELOPMENT_SYNAPTOGENESIS_RATE, 0.0001)
    config.set(ConfigKey.DEVELOPMENT_PRUNING_RATE, 0.00001)
    return config


__all__ = [
    'Config',
    'ConfigKey',
    'ConfigSource',
    'createDefaultConfig',
    'createPhase2Config',
    'createPhase3Config',
    'createPhase6Config'
]
