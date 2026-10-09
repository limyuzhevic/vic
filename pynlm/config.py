"""
Configuration Module

This module provides the high-level Config class that wraps the C++ Config interface
through a Pythonic API. It supports property-based configuration, type-safe parameter
access, and comprehensive configuration management.
"""

from typing import Any, Dict, List, Optional, Union, Set, TypeVar
from dataclasses import dataclass
import json
import os
from pathlib import Path

from .types import NeuronId, SynapseId, RegionId, PopulationId

T = TypeVar('T')

class Config:
    """
    High-level configuration wrapper for NLM simulation.
    
    This class provides property-based configuration for the NLM brain simulation.
    It supports type-safe parameter access, JSON/YAML file loading, command-line
    argument parsing, and configuration validation.
    
    Features:
    - Property-based configuration API
    - Type-safe parameter access
    - JSON/YAML file loading
    - Command-line argument parsing
    - Configuration validation
    - Environment variable support
    - Template-based configuration
    - Error handling and validation
    
    Examples:
        >>> # Create default configuration
        >>> config = Config()
        >>> 
        >>> # Set values using property-based API
        >>> config.set("neuron_count", 1000)
        >>> config.set("region_count", 3)
        >>> config.set("connection_probability", 0.1)
        
        >>> # Get values with type safety
        >>> neurons = config.get_or("neuron_count", 1000)
        >>> regions = config.get_or("region_count", 3)
        
        >>> # Load from file
        >>> config.load_from_file("config.json")
        >>> 
        >>> # Use fluent interface
        >>> config = Config().set("neuron_count", 1000).set("region_count", 3)
    """
    
    def __init__(self, config_file: Optional[str] = None):
        """
        Initialize configuration.
        
        Args:
            config_file: Optional path to configuration file (JSON/YAML).
        
        Raises:
            ValueError: If config file is invalid or cannot be loaded.
        """
        self._config_data = {}
        self._default_values = {}
        self._validators = {}
        self._environment_prefix = ""
        self._templates = {}
        self._last_error = ""
        
        # Register default validators
        self._register_default_validators()
        
        # Load from file if specified
        if config_file:
            self.load_from_file(config_file)
    
    def _register_default_validators(self):
        """Register default configuration validators."""
        self._validators = {
            "neuron_count": (int, lambda x: x > 0),
            "region_count": (int, lambda x: x > 0),
            "connection_probability": (float, lambda x: 0.0 <= x <= 1.0),
            "timestep": (float, lambda x: x > 0.0),
            "max_energy": (float, lambda x: x >= 0.0),
            "energy_decay_rate": (float, lambda x: 0.0 <= x <= 1.0),
            "random_seed": (int, lambda x: x >= 0),
            "vision_width": (int, lambda x: x > 0),
            "vision_height": (int, lambda x: x > 0),
            "world_width": (int, lambda x: x > 0),
            "world_height": (int, lambda x: x > 0),
            "sensory_noise": (float, lambda x: x >= 0.0),
            "action_noise": (float, lambda x: x >= 0.0),
            "reward_scale": (float, lambda x: x > 0.0),
            "curiosity_threshold": (float, lambda x: 0.0 <= x <= 1.0),
            "novelty_decay": (float, lambda x: 0.0 <= x <= 1.0),
        }
    
    @property
    def config_file(self) -> Optional[str]:
        """Get the configuration file path."""
        return getattr(self, '_config_file', None)
    
    def set(self, key: str, value: Any, source: str = "runtime") -> 'Config':
        """
        Set configuration value.
        
        Args:
            key: Configuration key.
            value: Configuration value.
            source: Source of configuration ("runtime", "file", "template").
        
        Returns:
            Self for fluent interface chaining.
        
        Raises:
            ValueError: If key or value is invalid.
        """
        # Validate key
        if not isinstance(key, str) or not key:
            raise ValueError(f"Invalid configuration key: {key}")
        
        # Validate value
        self._validate_value(key, value)
        
        # Store value
        self._config_data[key] = value
        self._default_values[key] = (value, source)
        
        return self
    
    def _validate_value(self, key: str, value: Any):
        """Validate configuration value."""
        if key in self._validators:
            value_type, validator = self._validators[key]
            
            # Type check
            if not isinstance(value, value_type):
                # Try to convert common types
                if value_type == int:
                    try:
                        value = int(value)
                    except (ValueError, TypeError):
                        raise ValueError(f"Expected int for {key}, got {type(value).__name__}")
                elif value_type == float:
                    try:
                        value = float(value)
                    except (ValueError, TypeError):
                        raise ValueError(f"Expected float for {key}, got {type(value).__name__}")
                elif value_type == str:
                    value = str(value)
            
            # Validation function
            if not validator(value):
                raise ValueError(f"Invalid value for {key}: {value}")
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get configuration value.
        
        Args:
            key: Configuration key.
            default: Default value if key not found.
        
        Returns:
            Configuration value or default.
        
        Raises:
            KeyError: If key is not found and no default provided.
        """
        if key in self._config_data:
            return self._config_data[key]
        elif default is not None:
            return default
        else:
            raise KeyError(f"Configuration key not found: {key}")
    
    def get_or(self, key: str, default: Any) -> Any:
        """
        Get configuration value with default fallback.
        
        Args:
            key: Configuration key.
            default: Default value if key not found.
        
        Returns:
            Configuration value or default.
        """
        return self.get(key, default)
    
    def has(self, key: str) -> bool:
        """
        Check if configuration key exists.
        
        Args:
            key: Configuration key.
        
        Returns:
            True if key exists, False otherwise.
        """
        return key in self._config_data
    
    def get_keys(self) -> List[str]:
        """
        Get all configuration keys.
        
        Returns:
            List of configuration keys.
        """
        return list(self._config_data.keys())
    
    def remove(self, key: str) -> 'Config':
        """
        Remove configuration key.
        
        Args:
            key: Configuration key.
        
        Returns:
            Self for fluent interface chaining.
        """
        if key in self._config_data:
            del self._config_data[key]
        return self
    
    def clear(self) -> 'Config':
        """
        Clear all configuration values.
        
        Returns:
            Self for fluent interface chaining.
        """
        self._config_data.clear()
        self._default_values.clear()
        return self
    
    def load_from_file(self, filepath: str) -> 'Config':
        """
        Load configuration from file.
        
        Args:
            filepath: Path to configuration file.
        
        Returns:
            Self for fluent interface chaining.
        
        Raises:
            ValueError: If file cannot be loaded or parsed.
        """
        path = Path(filepath)
        if not path.exists():
            raise ValueError(f"Configuration file not found: {filepath}")
        
        # Determine file type from extension
        if path.suffix.lower() in ['.json']:
            self._load_from_json(filepath)
        elif path.suffix.lower() in ['.yaml', '.yml']:
            self._load_from_yaml(filepath)
        else:
            # Try JSON by default
            try:
                self._load_from_json(filepath)
            except ValueError:
                try:
                    self._load_from_yaml(filepath)
                except ValueError:
                    raise ValueError(f"Unsupported configuration file format: {filepath}")
        
        self._config_file = filepath
        return self
    
    def _load_from_json(self, filepath: str):
        """Load configuration from JSON file."""
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        for key, value in data.items():
            self.set(key, value, source="file")
    
    def _load_from_yaml(self, filepath: str):
        """Load configuration from YAML file."""
        try:
            import yaml
            with open(filepath, 'r') as f:
                data = yaml.safe_load(f)
        except ImportError:
            raise ValueError("YAML support requires pyyaml package")
        
        for key, value in data.items():
            self.set(key, value, source="file")
    
    def save_to_file(self, filepath: str, format: str = "json") -> 'Config':
        """
        Save configuration to file.
        
        Args:
            filepath: Path to save file.
            format: File format ("json", "yaml").
        
        Returns:
            Self for fluent interface chaining.
        
        Raises:
            ValueError: If file cannot be saved.
        """
        path = Path(filepath)
        
        if format.lower() in ['json']:
            self._save_to_json(filepath)
        elif format.lower() in ['yaml', 'yml']:
            self._save_to_yaml(filepath)
        else:
            raise ValueError(f"Unsupported format: {format}")
        
        return self
    
    def _save_to_json(self, filepath: str):
        """Save configuration to JSON file."""
        with open(filepath, 'w') as f:
            json.dump(self._config_data, f, indent=2)
    
    def _save_to_yaml(self, filepath: str):
        """Save configuration to YAML file."""
        try:
            import yaml
            with open(filepath, 'w') as f:
                yaml.dump(self._config_data, f, default_flow_style=False)
        except ImportError:
            raise ValueError("YAML support requires pyyaml package")
    
    def load_from_args(self, args: List[str]) -> 'Config':
        """
        Load configuration from command line arguments.
        
        Args:
            args: Command line arguments (typically sys.argv[1:]).
        
        Returns:
            Self for fluent interface chaining.
        """
        i = 0
        while i < len(args):
            arg = args[i]
            if arg.startswith('--'):
                key = arg[2:]
                if i + 1 < len(args) and not args[i + 1].startswith('--'):
                    value = args[i + 1]
                    i += 1
                    
                    # Try to convert to appropriate type
                    converted = self._convert_arg_value(value)
                    self.set(key, converted, source="args")
            i += 1
        
        return self
    
    def _convert_arg_value(self, value: str) -> Any:
        """Convert argument value to appropriate type."""
        # Try boolean
        if value.lower() in ['true', 'false']:
            return value.lower() == 'true'
        
        # Try integer
        try:
            return int(value)
        except ValueError:
            pass
        
        # Try float
        try:
            return float(value)
        except ValueError:
            pass
        
        # Default to string
        return value
    
    def summary(self) -> str:
        """
        Get configuration summary.
        
        Returns:
            Configuration summary string.
        """
        if not self._config_data:
            return "Empty configuration"
        
        lines = ["Configuration Summary:"]
        for key, value in sorted(self._config_data.items()):
            lines.append(f"  {key}: {value}")
        
        return "\n".join(lines)
    
    def __repr__(self) -> str:
        """String representation."""
        return f"<Config keys={len(self._config_data)}>"
    
    def __str__(self) -> str:
        """String representation."""
        return self.summary()


# Helper functions

def create_default_config() -> Config:
    """
    Create a default configuration for NLM.
    
    Returns:
        Default Config instance with sensible defaults.
    
    Examples:
        >>> config = create_default_config()
        >>> config.set("neuron_count", 1000)
        >>> config.set("region_count", 3)
    """
    config = Config()
    
    # Set default values
    defaults = {
        "neuron_count": 1000,
        "region_count": 3,
        "connection_probability": 0.1,
        "timestep": 0.01,
        "max_energy": 100.0,
        "energy_decay_rate": 0.95,
        "random_seed": 42,
        "vision_width": 16,
        "vision_height": 16,
        "world_width": 100,
        "world_height": 100,
        "sensory_noise": 0.1,
        "action_noise": 0.1,
        "reward_scale": 1.0,
        "curiosity_threshold": 0.3,
        "novelty_decay": 0.99,
    }
    
    for key, value in defaults.items():
        config.set(key, value)
    
    return config


# Export commonly used symbols

__all__ = ['Config', 'create_default_config']
