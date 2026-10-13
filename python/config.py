"""
NLM Configuration Module

This module provides configuration management functionality for the NLM (Neural Learning Machine) framework.
It allows users to create, modify, and save configuration objects programmatically.

Usage:
    from nlm_config import Config, ConfigSource
    
    # Create a configuration
    config = Config()
    config.set("neuron_count", 1000)
    config.set("environment_name", "GridWorld")
    
    # Load from file
    config.loadFromFile("configs/default.cfg")
    
    # Get values
    neurons = config.getOr("neuron_count", 100)
    """

import json
import os
from typing import Optional, Union, Dict, Any, List
from enum import Enum

class ConfigSource(Enum):
    """Configuration source enumeration"""
    DEFAULT = 0
    FILE = 1
    COMMAND_LINE = 2
    RUNTIME = 3

class NLMConfigError(Exception):
    """Exception raised for configuration errors"""
    pass

class ConfigEntry:
    """Configuration entry with key, value, source, and description"""
    def __init__(self, key: str, value: Any, source: ConfigSource = ConfigSource.DEFAULT, description: str = ""):
        self.key = key
        self.value = value
        self.source = source
        self.description = description
    
    def __repr__(self):
        return f"ConfigEntry(key='{self.key}', value={self.value}, source={self.source.name})"

class Config:
    """
    NLM Configuration Manager
    
    Provides a Python interface to the NLM configuration system.
    """
    
    def __init__(self):
        self._entries: Dict[str, ConfigEntry] = {}
        self._load_defaults()
    
    def _load_defaults(self):
        """Load default configuration values"""
        default_values = {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 1000,
            "region_count": 1,
            "connection_probability": 0.1,
            "plasticity_learning_rate": 0.01,
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012,
            "stdp_tau": 20.0,
            "development_synaptogenesis_rate": 0.0001,
            "development_pruning_rate": 0.00001,
            "dopamine_baseline": 0.0,
            "reward_discount_factor": 0.99,
            "environment_name": "GridWorld",
            "environment_width": 10,
            "environment_height": 10,
            "log_level": "INFO",
            "log_to_file": False,
            "log_filename": "nlm.log",
            "max_simulation_steps": 10000,
            "simulation_time_limit": 0.0,
            "visualization_enabled": False,
            "visualization_update_rate": 30.0,
        }
        
        for key, value in default_values.items():
            self._entries[key] = ConfigEntry(key, value, ConfigSource.DEFAULT, f"Default value for {key}")
    
    def set(self, key: str, value: Any, source: ConfigSource = ConfigSource.RUNTIME, description: str = ""):
        """
        Set a configuration value.
        
        Args:
            key: Configuration key
            value: Configuration value
            source: Configuration source (default: RUNTIME)
            description: Description of the configuration parameter
        """
        if not key or not isinstance(key, str):
            raise NLMConfigError(f"Invalid key: '{key}'. Key must be a non-empty string.")
        
        self._entries[key] = ConfigEntry(key, value, source, description)
    
    def get(self, key: str) -> Optional[Any]:
        """
        Get a configuration value.
        
        Args:
            key: Configuration key
            
        Returns:
            Configuration value or None if key doesn't exist
        """
        entry = self._entries.get(key)
        return entry.value if entry else None
    
    def get_or(self, key: str, default_value: Any) -> Any:
        """
        Get a configuration value with a default fallback.
        
        Args:
            key: Configuration key
            default_value: Default value if key doesn't exist
            
        Returns:
            Configuration value or default value
        """
        return self.get(key) if self.has(key) else default_value
    
    def has(self, key: str) -> bool:
        """
        Check if a configuration key exists.
        
        Args:
            key: Configuration key
            
        Returns:
            True if key exists, False otherwise
        """
        return key in self._entries
    
    def remove(self, key: str):
        """
        Remove a configuration key.
        
        Args:
            key: Configuration key to remove
        """
        if key in self._entries:
            del self._entries[key]
    
    def load_from_file(self, filepath: str) -> bool:
        """
        Load configuration from a file.
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            True if successful, False otherwise
        """
        try:
            if not os.path.exists(filepath):
                print(f"Warning: Configuration file not found: {filepath}")
                return False
            
            # TODO: Implement actual file loading logic
            # For now, just a placeholder
            return True
            
        except Exception as e:
            print(f"Error loading configuration from file {filepath}: {e}")
            return False
    
    def save_to_file(self, filepath: str) -> bool:
        """
        Save configuration to a file.
        
        Args:
            filepath: Path to save configuration to
            
        Returns:
            True if successful, False otherwise
        """
        try:
            # Create directory if it doesn't exist
            os.makedirs(os.path.dirname(filepath), exist_ok=True)
            
            # TODO: Implement actual file saving logic
            # For now, just a placeholder
            return True
            
        except Exception as e:
            print(f"Error saving configuration to file {filepath}: {e}")
            return False
    
    def get_keys(self) -> List[str]:
        """
        Get all configuration keys.
        
        Returns:
            List of all configuration keys
        """
        return list(self._entries.keys())
    
    def clear(self):
        """Clear all configuration entries."""
        self._entries.clear()
        self._load_defaults()
    
    def summary(self) -> str:
        """
        Get a summary of the configuration.
        
        Returns:
            Configuration summary string
        """
        lines = [f"Configuration ({len(self._entries)} entries):"]
        for entry in sorted(self._entries.values(), key=lambda x: x.key):
            lines.append(f"  {entry.key} = {entry.value} ({entry.source.name})")
        return "\n".join(lines)
    
    def __repr__(self):
        return f"Config(entries={len(self._entries)})"

# Global configuration instance
_global_config = Config()

def get_global_config() -> Config:
    """Get the global configuration instance."""
    return _global_config

def set_config_value(key: str, value: Any, source: ConfigSource = ConfigSource.RUNTIME):
    """Set a value in the global configuration."""
    _global_config.set(key, value, source)

def get_config_value(key: str, default_value: Any = None):
    """Get a value from the global configuration."""
    return _global_config.get_or(key, default_value)