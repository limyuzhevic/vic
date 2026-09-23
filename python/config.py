#!/usr/bin/env python3
"""
Configuration management for NLM Python bindings.

This module provides a Python interface to the NLM configuration system,
enabling users to create, modify, and save configuration settings for
neural simulations.
"""

import json
import yaml
from typing import Any, Dict, List, Optional, Union
from pathlib import Path

# Import underlying C++ bindings
try:
    import pynlm
    has_pynlm = True
except ImportError:
    has_pynlm = False

class Config:
    """
    NLM Configuration manager.
    
    This class provides a unified interface for managing NLM configuration
    across multiple formats (JSON, YAML, key=value) with validation and
    sensible defaults for different simulation scenarios.
    
    Examples:
        >>> config = Config()
        >>> config.set("neuron_count", 1000)
        >>> config.set("random_seed", 42)
        >>> config.set("simulation_timestep", 0.001)
        >>> 
        >>> config.load_from_file("experiment.json")
        >>> config.save_to_file("experiment.yaml")
    """
    
    def __init__(self):
        """Create a new configuration with sensible defaults."""
        if has_pynlm:
            self._c_config = pynlm.Config()
            self._setup_defaults()
        else:
            raise ImportError(
                "pynlm module not available. "
                "Please ensure NLM Python bindings are properly installed."
            )
    
    def _setup_defaults(self):
        """Set up default values for NLM Phase 6 simulations."""
        # Core simulation parameters
        self._c_config.set("random_seed", 42, pynlm.ConfigSource.Runtime)
        self._c_config.set("simulation_timestep", 0.001, pynlm.ConfigSource.Runtime)
        self._c_config.set("neuron_count", 1000, pynlm.ConfigSource.Runtime)
        self._c_config.set("region_count", 1, pynlm.ConfigSource.Runtime)
        self._c_config.set("connection_probability", 0.1, pynlm.ConfigSource.Runtime)
        
        # Memory system parameters
        self._c_config.set("working_memory_capacity", 100, pynlm.ConfigSource.Runtime)
        self._c_config.set("episodic_memory_max_episodes", 1000, pynlm.ConfigSource.Runtime)
        self._c_config.set("replay_interval", 100, pynlm.ConfigSource.Runtime)
        self._c_config.set("consolidation_interval", 1000, pynlm.ConfigSource.Runtime)
        
        # Neuromodulation parameters
        self._c_config.set("novelty_threshold", 0.5, pynlm.ConfigSource.Runtime)
        self._c_config.set("curiosity_rate", 0.01, pynlm.ConfigSource.Runtime)
        
        # Plasticity parameters
        self._c_config.set("stdp_ltp_weight", 0.01, pynlm.ConfigSource.Runtime)
        self._c_config.set("stdp_ltd_weight", 0.012, pynlm.ConfigSource.Runtime)
        self._c_config.set("stdp_tau", 20.0, pynlm.ConfigSource.Runtime)
        
        # Environment parameters
        self._c_config.set("world_width", 100.0, pynlm.ConfigSource.Runtime)
        self._c_config.set("world_height", 100.0, pynlm.ConfigSource.Runtime)
    
    def load_from_file(self, filepath: str) -> bool:
        """
        Load configuration from a file.
        
        Automatically detects file format based on extension:
        - .json: JSON format with schema validation
        - .yaml/.yml: YAML format
        - .cfg/.txt: Simple key=value format
        
        Args:
            filepath: Path to the configuration file
            
        Returns:
            True if loading succeeded, False otherwise
        """
        return self._c_config.loadFromFile(filepath)
    
    def save_to_file(self, filepath: str) -> bool:
        """
        Save configuration to a file.
        
        Automatically selects file format based on extension:
        - .json: Human-readable JSON format
        - .yaml/.yml: Human-readable YAML format
        - .cfg/.txt: Simple key=value format with comments
        
        Args:
            filepath: Path where to save the configuration
            
        Returns:
            True if saving succeeded, False otherwise
        """
        return self._c_config.saveToFile(filepath)
    
    def has(self, key: str) -> bool:
        """Check if a configuration key exists."""
        return self._c_config.has(key)
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get a configuration value with type conversion.
        
        Args:
            key: Configuration key to retrieve
            default: Default value if key doesn't exist
            
        Returns:
            Configuration value or default if key doesn't exist
        """
        if not self.has(key):
            return default
            
        if has_pynlm:
            return self._c_config.get(key)
        
        return default
    
    def get_int(self, key: str, default: int = 0) -> int:
        """Get an integer configuration value."""
        value = self.get(key)
        if isinstance(value, int):
            return value
        return default
    
    def get_float(self, key: str, default: float = 0.0) -> float:
        """Get a float configuration value."""
        value = self.get(key)
        if isinstance(value, (int, float)):
            return float(value)
        return default
    
    def get_bool(self, key: str, default: bool = False) -> bool:
        """Get a boolean configuration value."""
        value = self.get(key)
        if isinstance(value, bool):
            return value
        return default
    
    def get_str(self, key: str, default: str = "") -> str:
        """Get a string configuration value."""
        value = self.get(key)
        if isinstance(value, str):
            return value
        return default
    
    def set(self, key: str, value: Any, source: str = "runtime") -> None:
        """
        Set a configuration value.
        
        Args:
            key: Configuration key
            value: Configuration value
            source: Source of the configuration
        """
        if has_pynlm:
            # Convert Python types to C++ ConfigValue
            if isinstance(value, bool):
                c_source = self._parse_source(source)
                self._c_config.set(key, value, c_source)
            elif isinstance(value, int):
                c_source = self._parse_source(source)
                self._c_config.set(key, value, c_source)
            elif isinstance(value, float):
                c_source = self._parse_source(source)
                self._c_config.set(key, value, c_source)
            elif isinstance(value, str):
                c_source = self._parse_source(source)
                self._c_config.set(key, value, c_source)
            else:
                raise ValueError(f"Unsupported configuration value type: {type(value)}")
    
    def _parse_source(self, source_str: str):
        """Convert string source to ConfigSource enum."""
        if not has_pynlm:
            return None
            
        source_map = {
            "default": pynlm.ConfigSource.Default,
            "file": pynlm.ConfigSource.File,
            "command_line": pynlm.ConfigSource.CommandLine,
            "runtime": pynlm.ConfigSource.Runtime,
        }
        
        return source_map.get(source_str.lower(), pynlm.ConfigSource.Runtime)
    
    def remove(self, key: str) -> None:
        """Remove a configuration key."""
        if has_pynlm:
            self._c_config.remove(key)
    
    def clear(self) -> None:
        """Clear all configuration entries."""
        if has_pynlm:
            self._c_config.clear()
            self._setup_defaults()
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys."""
        if has_pynlm:
            return self._c_config.getKeys()
        return []
    
    def get_summary(self) -> str:
        """Get a human-readable summary of the configuration."""
        if has_pynlm:
            return self._c_config.summary()
        return "Configuration unavailable"
    
    def validate(self) -> List[str]:
        """
        Validate configuration values.
        
        Returns:
            List of validation error messages (empty if valid)
        """
        errors = []
        
        # Validate neuron count
        neuron_count = self.get_int("neuron_count")
        if neuron_count <= 0:
            errors.append("neuron_count must be positive")
        
        # Validate simulation timestep
        timestep = self.get_float("simulation_timestep", 0.001)
        if timestep <= 0 or timestep > 1.0:
            errors.append("simulation_timestep must be between 0 and 1.0 seconds")
        
        return errors
    
    def clone(self) -> 'Config':
        """
        Create a copy of this configuration.
        
        Returns:
            New Config object with the same values
        """
        new_config = Config()
        for key in self.get_keys():
            new_config.set(key, self.get(key))
        return new_config
    
    def __repr__(self) -> str:
        """String representation of the configuration."""
        return f"<Config: {len(self.get_keys())} entries>"
    
    def __str__(self) -> str:
        """String representation of the configuration."""
        return self.get_summary()
def create_config_from_dict(config_dict: Dict[str, Any]) -> Config:
    """
    Create a Config object from a dictionary.
    
    Args:
        config_dict: Dictionary with configuration values
        
    Returns:
        Config object populated from the dictionary
    """
    config = Config()
    for key, value in config_dict.items():
        config.set(key, value)
    return config
def create_config_from_json(json_file: str) -> Config:
    """
    Create a Config object by loading JSON data.
    
    Args:
        json_file: Path to JSON configuration file
        
    Returns:
        Config object loaded from JSON
        
    Raises:
        FileNotFoundError: If the file doesn't exist
    """
    if not Path(json_file).exists():
        raise FileNotFoundError(f"Configuration file not found: {json_file}")
    
    with open(json_file, 'r') as f:
        config_dict = json.load(f)
    
    return create_config_from_dict(config_dict)
class _Internal:
    """Internal class to avoid circular imports."""
    pass