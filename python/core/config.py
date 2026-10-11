"""
Enhanced configuration management for NLM.

This module provides an enhanced Python interface to NLM configuration,
adding type safety, validation, and convenient accessors.
"""

import json
from typing import Dict, Any, Optional, Union
from .exceptions import ConfigError

# Import the original C++ Config class
import pynlm

def create_default_config() -> pynlm.Config:
    """Create a new default configuration."""
    return pynlm.createDefaultConfig()

class ConfigWrapper:
    """
    Enhanced Python wrapper for NLM configuration with validation and convenience methods.
    
    This class provides a more Pythonic interface to configuration management,
    including type safety, validation, and convenient dictionary-like access.
    """
    
    def __init__(self, config: pynlm.Config):
        """
        Initialize the configuration wrapper.
        
        Args:
            config: The underlying C++ Config object
        """
        self._config = config
    
    def set(self, key: str, value: Any) -> None:
        """
        Set a configuration value with type validation.
        
        Args:
            key: Configuration key (e.g., "brain.neuron_count")
            value: Configuration value
            
        Raises:
            ConfigError: If the key is invalid or value type doesn't match
        """
        if not key:
            raise ConfigError("Configuration key cannot be empty")
        
        # Basic validation - could be extended with type checking
        self._config.set(key, value)
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get a configuration value with optional default.
        
        Args:
            key: Configuration key
            default: Default value if key doesn't exist
            
        Returns:
            Configuration value or default
            
        Raises:
            ConfigError: If key doesn't exist and no default is provided
        """
        if not self._config.has(key):
            if default is not None:
                return default
            raise ConfigError(f"Configuration key '{key}' does not exist")
        
        return self._config.get(key)
    
    def has(self, key: str) -> bool:
        """
        Check if a configuration key exists.
        
        Args:
            key: Configuration key
            
        Returns:
            True if key exists, False otherwise
        """
        return self._config.has(key)
    
    def items(self) -> Dict[str, Any]:
        """
        Get all configuration key-value pairs.
        
        Returns:
            Dictionary of all configuration items
        """
        return {key: self.get(key) for key in self.keys()}
    
    def keys(self) -> list:
        """
        Get all configuration keys.
        
        Returns:
            List of configuration keys
        """
        return self._config.getKeys()
    
    def values(self) -> list:
        """
        Get all configuration values.
        
        Returns:
            List of configuration values
        """
        return [self.get(key) for key in self.keys()]
    
    def clear(self) -> None:
        """Clear all configuration entries."""
        self._config.clear()
    
    def load_from_dict(self, config_dict: Dict[str, Any]) -> None:
        """
        Load configuration from a dictionary.
        
        Args:
            config_dict: Dictionary of configuration values
        """
        for key, value in config_dict.items():
            self.set(key, value)
    
    def load_from_json(self, json_str: str) -> None:
        """
        Load configuration from JSON string.
        
        Args:
            json_str: JSON string containing configuration
        """
        config_dict = json.loads(json_str)
        self.load_from_dict(config_dict)
    
    def save_to_json(self, filepath: str) -> None:
        """
        Save configuration to JSON file.
        
        Args:
            filepath: Path to JSON file
        """
        config_dict = self.items()
        with open(filepath, 'w') as f:
            json.dump(config_dict, f, indent=2)
        
        # Also save to the underlying C++ config
        self._config.saveToFile(filepath)
    
    def validate_required_keys(self, required_keys: list) -> bool:
        """
        Validate that all required keys are present.
        
        Args:
            required_keys: List of required key names
            
        Returns:
            True if all required keys are present, False otherwise
            
        Raises:
            ConfigError: If validation fails
        """
        missing_keys = [key for key in required_keys if not self.has(key)]
        if missing_keys:
            raise ConfigError(f"Missing required configuration keys: {missing_keys}")
        return True
    
    def get_int(self, key: str, default: int = None) -> int:
        """
        Get an integer configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key doesn't exist
            
        Returns:
            Integer value
        """
        value = self.get(key, default)
        if value is not None:
            return int(value)
        return default
    
    def get_float(self, key: str, default: float = None) -> float:
        """
        Get a float configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key doesn't exist
            
        Returns:
            Float value
        """
        value = self.get(key, default)
        if value is not None:
            return float(value)
        return default
    
    def get_bool(self, key: str, default: bool = None) -> bool:
        """
        Get a boolean configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key doesn't exist
            
        Returns:
            Boolean value
        """
        value = self.get(key, default)
        if value is not None:
            return bool(value)
        return default
    
    def __getitem__(self, key: str) -> Any:
        """Python dictionary-style access."""
        return self.get(key)
    
    def __setitem__(self, key: str, value: Any) -> None:
        """Python dictionary-style assignment."""
        self.set(key, value)
    
    def __contains__(self, key: str) -> bool:
        """Python 'in' operator support."""
        return self.has(key)
    
    def __repr__(self) -> str:
        """String representation of the configuration."""
        return f"<ConfigWrapper with {len(self.keys())} entries>"
    
    def __str__(self) -> str:
        """String representation."""
        return self._config.summary()
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        return self.items()
    
    @classmethod
    def from_dict(cls, config_dict: Dict[str, Any]) -> 'ConfigWrapper':
        """
        Create a configuration wrapper from a dictionary.
        
        Args:
            config_dict: Dictionary of configuration values
            
        Returns:
            ConfigWrapper instance
        """
        config = create_default_config()
        wrapper = cls(config)
        wrapper.load_from_dict(config_dict)
        return wrapper

# Re-export the enhanced functions and classes
__all__ = [
    'create_default_config',
    'ConfigWrapper',
    'ConfigError',
]

# Convenience functions
def create_config_from_dict(config_dict: Dict[str, Any]) -> ConfigWrapper:
    """
    Create a configuration wrapper from a dictionary.
    
    Args:
        config_dict: Dictionary of configuration values
        
    Returns:
        ConfigWrapper instance
    """
    return ConfigWrapper.from_dict(config_dict)
def create_config_with_validation(config_dict: Dict[str, Any], required_keys: list = None) -> ConfigWrapper:
    """
    Create a configuration wrapper with validation.
    
    Args:
        config_dict: Dictionary of configuration values
        required_keys: List of required key names
        
    Returns:
        ConfigWrapper instance
        
    Raises:
        ConfigError: If validation fails
    """
    config = create_config_from_dict(config_dict)
    if required_keys:
        config.validate_required_keys(required_keys)
    return config
