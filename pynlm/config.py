#!/usr/bin/env python3
"""
NLM Configuration Module

This module provides comprehensive configuration management for NLM simulations,
including validation, serialization, and parameter management.
"""

import json
import os
from pathlib import Path
from typing import Dict, Any, Optional, List, Union
from dataclasses import dataclass, field, asdict
from enum import Enum
import logging

from pynlm import _TypeValidator

logger = logging.getLogger(__name__)


class ConfigSource(Enum):
    """Source of configuration parameters."""
    DEFAULT = "default"
    FILE = "file"
    ARGS = "args"
    RUNTIME = "runtime"


@dataclass
class NLMConfig:
    """Comprehensive configuration for NLM simulations."""
    
    # Core simulation parameters
    neuron_count: int = 1000
    region_count: int = 1
    connection_probability: float = 0.1
    simulation_timestep: float = 0.001
    random_seed: Optional[int] = None
    
    # Neural parameters
    v_thresh: float = -50.0      # Neuron threshold potential (mV)
    v_rest: float = -70.0        # Resting potential (mV)
    v_reset: float = -75.0       # Reset potential after spike (mV)
    tau_mem: float = 20.0        # Membrane time constant (ms)
    tau_ref: float = 2.0         # Refractory period (ms)
    
    # Synaptic parameters
    initial_weight_mean: float = 0.5
    initial_weight_std: float = 0.1
    
    # Plasticity parameters
    stdp_enable: bool = True
    stdp_learning_rate: float = 0.001
    stdp_tau_plus: float = 20.0
    stdp_tau_minus: float = 20.0
    hebbian_enable: bool = True
    hebbian_learning_rate: float = 0.001
    structural_enable: bool = True
    synaptogenesis_rate: float = 0.0001
    pruning_rate: float = 0.00001
    
    # Neuromodulation parameters
    neuromod_dopamine_scale: float = 1.0
    neuromod_curiosity_enable: bool = True
    neuromod_novelty_enable: bool = True
    neuromod_prediction_error_scale: float = 1.0
    
    # Memory parameters
    memory_working_capacity: int = 100
    memory_episodic_capacity: int = 1000
    memory_associative_similarity_threshold: float = 0.7
    
    # Prediction parameters
    prediction_enable: bool = True
    prediction_horizon: float = 0.1
    prediction_confidence_threshold: float = 0.5
    
    # Development parameters
    development_enable: bool = True
    developmental_stage_duration: float = 100.0
    critical_period_multiplier: float = 2.0
    
    # Environment parameters
    world_width: int = 20
    world_height: int = 20
    world_vision_width: int = 8
    world_vision_height: int = 8
    world_energy_decay: float = 0.01
    world_max_energy: float = 100.0
    
    # Agent parameters
    agent_energy_cost_move: float = 1.0
    agent_energy_cost_turn: float = 0.5
    agent_energy_gain_resource: float = 5.0
    agent_health_decay_rate: float = 0.001
    
    # Performance parameters
    performance_checkpoint_interval: int = 100
    performance_max_memory_mb: int = 512
    performance_parallel_enabled: bool = False
    
    def __post_init__(self) -> None:
        """Validate configuration after initialization."""
        self.validate()
    
    def validate(self) -> None:
        """Validate all configuration parameters."""
        _TypeValidator.validate_positive(self.neuron_count, "neuron_count")
        _TypeValidator.validate_range(self.neuron_count, 10, 100000, "neuron_count")
        
        _TypeValidator.validate_positive(self.region_count, "region_count")
        _TypeValidator.validate_range(self.region_count, 1, 100, "region_count")
        
        _TypeValidator.validate_range(self.connection_probability, 0.0, 1.0, "connection_probability")
        
        _TypeValidator.validate_range(self.simulation_timestep, 0.0001, 0.1, "simulation_timestep")
        
        if self.random_seed is not None:
            _TypeValidator.validate_positive(self.random_seed, "random_seed")
        
        _TypeValidator.validate_range(self.v_thresh, -100.0, 0.0, "v_thresh")
        _TypeValidator.validate_range(self.v_rest, -100.0, 0.0, "v_rest")
        _TypeValidator.validate_range(self.v_reset, -100.0, 0.0, "v_reset")
        _TypeValidator.validate_range(self.tau_mem, 1.0, 100.0, "tau_mem")
        _TypeValidator.validate_range(self.tau_ref, 0.1, 50.0, "tau_ref")
        
        _TypeValidator.validate_range(self.initial_weight_mean, 0.0, 1.0, "initial_weight_mean")
        _TypeValidator.validate_range(self.initial_weight_std, 0.0, 1.0, "initial_weight_std")
        
        self._validate_plasticity_params()
        self._validate_neuromodulation_params()
        self._validate_memory_params()
        self._validate_prediction_params()
        self._validate_development_params()
        self._validate_environment_params()
        self._validate_agent_params()
        self._validate_performance_params()
    
    def _validate_plasticity_params(self) -> None:
        """Validate plasticity-related parameters."""
        _TypeValidator.validate_range(self.stdp_learning_rate, 0.0, 1.0, "stdp_learning_rate")
        _TypeValidator.validate_range(self.stdp_tau_plus, 1.0, 100.0, "stdp_tau_plus")
        _TypeValidator.validate_range(self.stdp_tau_minus, 1.0, 100.0, "stdp_tau_minus")
        _TypeValidator.validate_range(self.hebbian_learning_rate, 0.0, 1.0, "hebbian_learning_rate")
        _TypeValidator.validate_range(self.synaptogenesis_rate, 0.0, 0.01, "synaptogenesis_rate")
        _TypeValidator.validate_range(self.pruning_rate, 0.0, 0.01, "pruning_rate")
    
    def _validate_neuromodulation_params(self) -> None:
        """Validate neuromodulation-related parameters."""
        _TypeValidator.validate_range(self.neuromod_dopamine_scale, 0.0, 10.0, "neuromod_dopamine_scale")
        _TypeValidator.validate_range(self.neuromod_prediction_error_scale, 0.0, 10.0, "neuromod_prediction_error_scale")
    
    def _validate_memory_params(self) -> None:
        """Validate memory-related parameters."""
        _TypeValidator.validate_positive(self.memory_working_capacity, "memory_working_capacity")
        _TypeValidator.validate_positive(self.memory_episodic_capacity, "memory_episodic_capacity")
        _TypeValidator.validate_range(self.memory_associative_similarity_threshold, 0.0, 1.0, "memory_associative_similarity_threshold")
    
    def _validate_prediction_params(self) -> None:
        """Validate prediction-related parameters."""
        _TypeValidator.validate_range(self.prediction_horizon, 0.001, 1.0, "prediction_horizon")
        _TypeValidator.validate_range(self.prediction_confidence_threshold, 0.0, 1.0, "prediction_confidence_threshold")
    
    def _validate_development_params(self) -> None:
        """Validate development-related parameters."""
        _TypeValidator.validate_range(self.developmental_stage_duration, 10.0, 10000.0, "developmental_stage_duration")
        _TypeValidator.validate_range(self.critical_period_multiplier, 1.0, 10.0, "critical_period_multiplier")
    
    def _validate_environment_params(self) -> None:
        """Validate environment-related parameters."""
        _TypeValidator.validate_positive(self.world_width, "world_width")
        _TypeValidator.validate_positive(self.world_height, "world_height")
        _TypeValidator.validate_positive(self.world_vision_width, "world_vision_width")
        _TypeValidator.validate_positive(self.world_vision_height, "world_vision_height")
        _TypeValidator.validate_range(self.world_energy_decay, 0.0, 1.0, "world_energy_decay")
        _TypeValidator.validate_positive(self.world_max_energy, "world_max_energy")
    
    def _validate_agent_params(self) -> None:
        """Validate agent-related parameters."""
        _TypeValidator.validate_range(self.agent_energy_cost_move, 0.0, 10.0, "agent_energy_cost_move")
        _TypeValidator.validate_range(self.agent_energy_cost_turn, 0.0, 10.0, "agent_energy_cost_turn")
        _TypeValidator.validate_range(self.agent_energy_gain_resource, 0.0, 10.0, "agent_energy_gain_resource")
        _TypeValidator.validate_range(self.agent_health_decay_rate, 0.0, 1.0, "agent_health_decay_rate")
    
    def _validate_performance_params(self) -> None:
        """Validate performance-related parameters."""
        _TypeValidator.validate_positive(self.performance_checkpoint_interval, "performance_checkpoint_interval")
        _TypeValidator.validate_positive(self.performance_max_memory_mb, "performance_max_memory_mb")
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        return asdict(self)
    
    def to_json(self, filepath: Optional[str] = None) -> Optional[str]:
        """
        Serialize configuration to JSON.
        
        Args:
            filepath: Optional path to save configuration to file
            
        Returns:
            JSON string if filepath not provided, otherwise None
        """
        data = self.to_dict()
        
        if filepath:
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
            logger.info(f"Configuration saved to {filepath}")
            return None
        else:
            return json.dumps(data, indent=2)
    
    @classmethod
    def from_json(cls, json_str: str) -> "NLMConfig":
        """
        Create configuration from JSON string.
        
        Args:
            json_str: JSON string containing configuration
            
        Returns:
            NLMConfig instance
        """
        data = json.loads(json_str)
        return cls(**data)
    
    @classmethod
    def from_file(cls, filepath: str) -> "NLMConfig":
        """
        Load configuration from JSON file.
        
        Args:
            filepath: Path to JSON configuration file
            
        Returns:
            NLMConfig instance
        """
        with open(filepath, 'r') as f:
            data = json.load(f)
        return cls(**data)
    
    def merge(self, other: "NLMConfig") -> "NLMConfig":
        """
        Merge another configuration into this one.
        
        Args:
            other: Configuration to merge (overwrites current values)
            
        Returns:
            New merged configuration
        """
        other_dict = other.to_dict()
        merged_dict = self.to_dict()
        
        # Merge dictionaries (other takes precedence)
        merged_dict.update(other_dict)
        
        return NLMConfig(**merged_dict)
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get configuration value by key with fallback.
        
        Args:
            key: Configuration key (dot notation supported)
            default: Default value if key not found
            
        Returns:
            Configuration value or default
        """
        keys = key.split('.')
        value = self
        
        for k in keys:
            if hasattr(value, k):
                value = getattr(value, k)
            else:
                return default
        
        return value


class ConfigManager:
    """Configuration manager for NLM simulations."""
    
    def __init__(self, config: Optional[NLMConfig] = None):
        """
        Initialize configuration manager.
        
        Args:
            config: Initial configuration
        """
        self._config = config or NLMConfig()
        self._validation_enabled = True
        self._history: List[NLMConfig] = []
        self._callbacks: List[callable] = []
    
    def set_config(self, config: NLMConfig) -> None:
        """
        Set configuration.
        
        Args:
            config: New configuration
        """
        if self._validation_enabled:
            config.validate()
        
        self._config = config
        self._history.append(config)
        
        # Notify callbacks
        for callback in self._callbacks:
            callback(config)
    
    def update_config(self, **kwargs) -> None:
        """
        Update configuration with new parameters.
        
        Args:
            **kwargs: Configuration parameters to update
        """
        current_dict = self._config.to_dict()
        current_dict.update(kwargs)
        
        new_config = NLMConfig(**current_dict)
        self.set_config(new_config)
    
    def get_config(self) -> NLMConfig:
        """Get current configuration."""
        return self._config
    
    def save_config(self, filepath: str) -> None:
        """
        Save configuration to file.
        
        Args:
            filepath: Path to save configuration
        """
        self._config.to_json(filepath)
        logger.info(f"Configuration saved to {filepath}")
    
    def load_config(self, filepath: str) -> None:
        """
        Load configuration from file.
        
        Args:
            filepath: Path to load configuration from
        """
        config = NLMConfig.from_file(filepath)
        self.set_config(config)
    
    def validate(self) -> bool:
        """
        Validate current configuration.
        
        Returns:
            True if configuration is valid, False otherwise
        """
        try:
            self._config.validate()
            return True
        except Exception as e:
            logger.error(f"Configuration validation failed: {e}")
            return False
    
    def get_validation_errors(self) -> List[str]:
        """
        Get configuration validation errors.
        
        Returns:
            List of error messages
        """
        errors = []
        
        # Check all numeric parameters
        if self._config.neuron_count < 1:
            errors.append("neuron_count must be positive")
        
        if not (0.0 <= self._config.connection_probability <= 1.0):
            errors.append("connection_probability must be between 0.0 and 1.0")
        
        if not (0.0001 <= self._config.simulation_timestep <= 0.1):
            errors.append("simulation_timestep must be between 0.0001 and 0.1")
        
        return errors
    
    def add_validation_callback(self, callback: callable) -> None:
        """
        Add callback for configuration changes.
        
        Args:
            callback: Function to call when configuration changes
        """
        self._callbacks.append(callback)
    
    def get_config_summary(self) -> Dict[str, Any]:
        """
        Get summary of current configuration.
        
        Returns:
            Dictionary with configuration summary
        """
        return {
            "neuron_count": self._config.neuron_count,
            "region_count": self._config.region_count,
            "connection_probability": self._config.connection_probability,
            "simulation_timestep": self._config.simulation_timestep,
            "random_seed": self._config.random_seed,
            "plasticity_enabled": self._config.stdp_enable or self._config.hebbian_enable or self._config.structural_enable,
            "neuromodulation_enabled": self._config.neuromod_curiosity_enable or self._config.neuromod_novelty_enable,
            "memory_capacity": self._config.memory_working_capacity + self._config.memory_episodic_capacity,
            "prediction_enabled": self._config.prediction_enable,
            "development_enabled": self._config.development_enable,
        }
    
    def __repr__(self) -> str:
        return f"ConfigManager({len(self._history)} history entries)"


# Global configuration instance
_global_config: Optional[ConfigManager] = None


def get_config() -> ConfigManager:
    """Get global configuration manager."""
    global _global_config
    if _global_config is None:
        _global_config = ConfigManager()
    return _global_config


def set_config(config: NLMConfig) -> None:
    """Set global configuration."""
    global _global_config
    _global_config = ConfigManager(config)