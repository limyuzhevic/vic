"""
Configuration management for NLM Python bindings.

This module provides enhanced configuration functionality for the NLM brain
simulation framework, with improved Pythonic interfaces and validation.
"""

import json
import os
import sys
from typing import Dict, Any, Optional, Union
from pathlib import Path

import pybind11
from pybind11 import kwargs_only

from ..bindings import Config, createDefaultConfig
from .exceptions import ConfigurationError, ValidationError

class NLMConfig:
    """
    Enhanced configuration manager for NLM simulations.
    
    Provides Pythonic interface to NLM configuration with validation,
    type conversion, and default value management.
    """
    
    def __init__(self, config_file: Optional[str] = None):
        """
        Initialize configuration.
        
        Args:
            config_file: Optional path to configuration file
        """
        self._config = createDefaultConfig()
        
        if config_file and os.path.exists(config_file):
            self.load_from_file(config_file)
    
    def load_from_file(self, filepath: str) -> bool:
        """
        Load configuration from JSON file.
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            True if successful, False otherwise
            
        Raises:
            ConfigurationError: If file cannot be loaded
        """
        try:
            self._config.loadFromFile(filepath)
            return True
        except Exception as e:
            raise ConfigurationError(f"Failed to load config from {filepath}: {e}")
    
    def load_from_dict(self, config_dict: Dict[str, Any]) -> None:
        """
        Load configuration from Python dictionary.
        
        Args:
            config_dict: Dictionary with configuration values
        """
        for key, value in config_dict.items():
            self.set(key, value)
    
    @kwargs_only
    def set(self, key: str, value: Any, source: Optional[str] = None) -> None:
        """
        Set configuration value.
        
        Args:
            key: Configuration key
            value: Configuration value
            source: Optional source description
        """
        # Type conversion based on key
        if key.endswith('_count') or key == 'random_seed':
            if not isinstance(value, int):
                value = int(value)
        elif key.endswith('_rate') or key in ['connection_probability', 'stdp_ltp_weight', 
                                            'stdp_ltd_weight', 'synaptogenesis_rate', 'pruning_rate']:
            if not isinstance(value, float):
                value = float(value)
        elif key.endswith('_path') or key.endswith('_file'):
            if not isinstance(value, str):
                value = str(value)
        
        self._config.set(key, value)
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key not found
            
        Returns:
            Configuration value or default
        """
        if self._config.has(key):
            return self._config.get(key)
        return default
    
    def get_int(self, key: str, default: int = 0) -> int:
        """Get integer configuration value."""
        value = self.get(key)
        return int(value) if value is not None else default
    
    def get_float(self, key: str, default: float = 0.0) -> float:
        """Get float configuration value."""
        value = self.get(key)
        return float(value) if value is not None else default
    
    def get_str(self, key: str, default: str = "") -> str:
        """Get string configuration value."""
        value = self.get(key)
        return str(value) if value is not None else default
    
    def has(self, key: str) -> bool:
        """Check if configuration key exists."""
        return self._config.has(key)
    
    def get_keys(self) -> list:
        """Get all configuration keys."""
        return self._config.getKeys()
    
    def clear(self) -> None:
        """Clear all configuration entries."""
        self._config.clear()
    
    def summary(self) -> str:
        """Get configuration summary."""
        return self._config.summary()
    
    def to_dict(self) -> Dict[str, Any]:
        """
        Convert configuration to dictionary.
        
        Returns:
            Dictionary with all configuration values
        """
        result = {}
        for key in self.get_keys():
            result[key] = self.get(key)
        return result
    
    def save_to_file(self, filepath: str) -> bool:
        """
        Save configuration to JSON file.
        
        Args:
            filepath: Path to save configuration
            
        Returns:
            True if successful, False otherwise
            
        Raises:
            ConfigurationError: If file cannot be saved
        """
        try:
            self._config.saveToFile(filepath)
            return True
        except Exception as e:
            raise ConfigurationError(f"Failed to save config to {filepath}: {e}")
    
    def __repr__(self) -> str:
        return f"NLMConfig({len(self.get_keys())} settings)"
    
    def __str__(self) -> str:
        return self.summary()


def create_config(config_file: Optional[str] = None) -> NLMConfig:
    """
    Create NLM configuration object.
    
    Args:
        config_file: Optional path to configuration file
        
    Returns:
        NLMConfig object
    """
    return NLMConfig(config_file)


# Convenience functions for common configurations
def create_simulation_config(
    neuron_count: int = 500,
    region_count: int = 1,
    connection_probability: float = 0.15,
    simulation_timestep: float = 0.001,
    **kwargs
) -> NLMConfig:
    """
    Create simulation configuration with sensible defaults.
    
    Args:
        neuron_count: Number of neurons
        region_count: Number of brain regions
        connection_probability: Probability of synaptic connections
        simulation_timestep: Simulation time step
        **kwargs: Additional configuration options
        
    Returns:
        NLMConfig object with simulation settings
    """
    config = NLMConfig()
    config.set("neuron_count", neuron_count)
    config.set("region_count", region_count)
    config.set("connection_probability", connection_probability)
    config.set("simulation_timestep", simulation_timestep)
    
    for key, value in kwargs.items():
        config.set(key, value)
    
    return config


def create_development_config(
    enable_development: bool = True,
    enable_curiosity: bool = True,
    enable_reward_modulation: bool = True,
    enable_structural_plasticity: bool = True,
    **kwargs
) -> NLMConfig:
    """
    Create development configuration for learning and adaptation.
    
    Args:
        enable_development: Enable developmental stages
        enable_curiosity: Enable curiosity-driven exploration
        enable_reward_modulation: Enable reward-based learning
        enable_structural_plasticity: Enable structural changes
        **kwargs: Additional configuration options
        
    Returns:
        NLMConfig object with development settings
    """
    config = NLMConfig()
    config.set("enable_development", enable_development)
    config.set("enable_curiosity", enable_curiosity)
    config.set("enable_reward_modulation", enable_reward_modulation)
    config.set("enable_structural_plasticity", enable_structural_plasticity)
    
    for key, value in kwargs.items():
        config.set(key, value)
    
    return config


def validate_config(config: NLMConfig) -> bool:
    """
    Validate configuration for compatibility.
    
    Args:
        config: NLMConfig object to validate
        
    Returns:
        True if configuration is valid, False otherwise
        
    Raises:
        ValidationError: If configuration is invalid
    """
    try:
        # Check required settings
        if not config.has("neuron_count"):
            raise ValidationError("Configuration missing 'neuron_count'")
        
        if not config.has("region_count"):
            raise ValidationError("Configuration missing 'region_count'")
        
        # Validate ranges
        neuron_count = config.get_int("neuron_count")
        if neuron_count < 10 or neuron_count > 100000:
            raise ValidationError(f"Invalid neuron_count: {neuron_count}. Must be between 10 and 100000")
        
        region_count = config.get_int("region_count")
        if region_count < 1 or region_count > 100:
            raise ValidationError(f"Invalid region_count: {region_count}. Must be between 1 and 100")
        
        connection_prob = config.get_float("connection_probability", 0.15)
        if connection_prob < 0.0 or connection_prob > 1.0:
            raise ValidationError(f"Invalid connection_probability: {connection_prob}. Must be between 0.0 and 1.0")
        
        timestep = config.get_float("simulation_timestep", 0.001)
        if timestep <= 0.0 or timestep > 1.0:
            raise ValidationError(f"Invalid simulation_timestep: {timestep}. Must be between 0.0 and 1.0")
        
        return True
        
    except ValidationError:
        raise
    except Exception as e:
        raise ValidationError(f"Configuration validation failed: {e}")
