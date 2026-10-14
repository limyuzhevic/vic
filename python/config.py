# Enhanced configuration management

import os
import json
import sys
from typing import Dict, Any, Optional, List, Union, overload
from enum import Enum

from .exceptions import NLMErrors, ConfigurationError

class ConfigSource(Enum):
    """Configuration source."""
    DEFAULT = 0
    FILE = 1
    COMMAND_LINE = 2
    RUNTIME = 3

class Config:
    """Enhanced configuration management with Pythonic API."""
    
    def __init__(self, initial_config: Optional[Dict[str, Any]] = None):
        self._config: Dict[str, Any] = {}
        self._sources: Dict[str, ConfigSource] = {}
        self._descriptions: Dict[str, str] = {}
        
        # Load default configuration
        self._load_defaults()
        
        # Apply initial configuration if provided
        if initial_config:
            self.update(initial_config)
            
    def _load_defaults(self):
        """Load default configuration values."""
        defaults = {
            'neuron_count': 1000,
            'region_count': 1,
            'connection_probability': 0.1,
            'simulation_timestep': 0.001,
            'random_seed': 42,
            'stdp_ltp_weight': 0.01,
            'stdp_ltd_weight': 0.012,
            'stdp_tau': 20.0,
            'synaptogenesis_rate': 0.0001,
            'pruning_rate': 0.00001,
            'enable_reward_modulation': True,
            'enable_development': True,
            'enable_curiosity': True,
            'enable_structural_plasticity': True,
            'enable_prediction': True,
            'enable_cognition': True,
            'enable_memory': True,
            'checkpoint_dir': './checkpoints',
            'checkpoint_interval': 1000,
            'max_checkpoints': 10,
            'compression_level': 'balanced',
            'world_width': 16.0,
            'world_height': 16.0,
            'vision_width': 8,
            'vision_height': 8,
            'max_energy': 100.0,
            'energy_decay_rate': 0.01,
            'resource_energy_gain': 10.0,
        }
        
        for key, value in defaults.items():
            self._config[key] = value
            self._sources[key] = ConfigSource.DEFAULT
            self._descriptions[key] = f"Default {key.replace('_', ' ')}"
            
    def update(self, config: Dict[str, Any], source: ConfigSource = ConfigSource.RUNTIME):
        """Update configuration with dictionary."""
        for key, value in config.items():
            self.set(key, value, source)
            
    def set(self, key: str, value: Any, source: ConfigSource = ConfigSource.RUNTIME,
            description: Optional[str] = None):
        """Set a configuration value."""
        # Validate key
        if not isinstance(key, str) or not key.strip():
            raise ConfigurationError(f"Invalid configuration key: {key}")
            
        # Validate value based on key
        self._validate_value(key, value)
        
        # Store configuration
        self._config[key] = value
        self._sources[key] = source
        
        if description:
            self._descriptions[key] = description
            
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value."""
        return self._config.get(key, default)
        
    @overload
    def get_or(self, key: str, default: Any) -> Any: ...
        
    @overload
    def get_or(self, key: str, default: None = None) -> Any: ...
        
    def get_or(self, key: str, default: Any = None) -> Any:
        """Get configuration value with default."""
        return self._config.get(key, default)
        
    def has(self, key: str) -> bool:
        """Check if key exists in configuration."""
        return key in self._config
        
    def remove(self, key: str):
        """Remove a configuration key."""
        if key in self._config:
            del self._config[key]
            if key in self._sources:
                del self._sources[key]
            if key in self._descriptions:
                del self._descriptions[key]
                
    def get_keys(self) -> List[str]:
        """Get all configuration keys."""
        return list(self._config.keys())
        
    def clear(self):
        """Clear all configuration."""
        self._config.clear()
        self._sources.clear()
        self._descriptions.clear()
        self._load_defaults()
        
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        return self._config.copy()
        
    def from_dict(self, config_dict: Dict[str, Any]):
        """Load configuration from dictionary."""
        self.update(config_dict)
        
    def save_to_file(self, filepath: str):
        """Save configuration to JSON file."""
        try:
            with open(filepath, 'w') as f:
                json.dump(self._config, f, indent=2)
                
            # Ensure directory exists
            os.makedirs(os.path.dirname(filepath), exist_ok=True)
            
        except Exception as e:
            raise ConfigurationError(f"Failed to save configuration to {filepath}: {e}")
            
    def load_from_file(self, filepath: str) -> bool:
        """Load configuration from JSON file."""
        try:
            if not os.path.exists(filepath):
                return False
                
            with open(filepath, 'r') as f:
                config_data = json.load(f)
                
            # Validate configuration
            for key, value in config_data.items():
                self._validate_value(key, value)
                
            self.update(config_data, ConfigSource.FILE)
            return True
            
        except Exception as e:
            raise ConfigurationError(f"Failed to load configuration from {filepath}: {e}")
            
    def load_from_args(self, args: List[str]):
        """Load configuration from command line arguments."""
        i = 0
        while i < len(args):
            arg = args[i]
            if arg.startswith('--'):
                key = arg[2:]  # Remove -- prefix
                if i + 1 < len(args) and not args[i + 1].startswith('--'):
                    # Next argument is the value
                    value_str = args[i + 1]
                    i += 1
                    
                    # Try to parse value as different types
                    value = self._parse_value(value_str)
                    self.set(key, value, ConfigSource.COMMAND_LINE)
                else:
                    # Flag argument
                    self.set(key, True, ConfigSource.COMMAND_LINE)
            i += 1
            
    def _parse_value(self, value_str: str) -> Any:
        """Parse string value to appropriate type."""
        # Try to parse as boolean
        if value_str.lower() in ('true', 'false'):
            return value_str.lower() == 'true'
            
        # Try to parse as integer
        if value_str.isdigit() or (value_str.startswith('-') and value_str[1:].isdigit()):
            return int(value_str)
            
        # Try to parse as float
        try:
            return float(value_str)
        except ValueError:
            pass
            
        # Return as string
        return value_str
        
    def _validate_value(self, key: str, value: Any):
        """Validate configuration value."""
        # Basic validation for common keys
        if key in ('neuron_count', 'region_count', 'connection_probability',
                  'random_seed', 'stdp_ltp_weight', 'stdp_ltd_weight',
                  'stdp_tau', 'synaptogenesis_rate', 'pruning_rate'):
            if not isinstance(value, (int, float)):
                raise ConfigurationError(f"Expected numeric value for {key}, got {type(value)}")
            if value < 0:
                raise ConfigurationError(f"Value for {key} must be non-negative")
                
        elif key in ('enable_reward_modulation', 'enable_development',
                    'enable_curiosity', 'enable_structural_plasticity',
                    'enable_prediction', 'enable_cognition', 'enable_memory'):
            if not isinstance(value, bool):
                raise ConfigurationError(f"Expected boolean value for {key}, got {type(value)}")
                
        elif key in ('simulation_timestep', 'world_width', 'world_height'):
            if not isinstance(value, (int, float)):
                raise ConfigurationError(f"Expected numeric value for {key}, got {type(value)}")
            if value <= 0:
                raise ConfigurationError(f"Value for {key} must be positive")
                
        elif key in ('checkpoint_dir', 'compression_level'):
            if not isinstance(value, str):
                raise ConfigurationError(f"Expected string value for {key}, got {type(value)}")
                
    def summary(self) -> str:
        """Get configuration summary."""
        lines = ["Configuration Summary:"]
        lines.append("=" * 50)
        
        for key in sorted(self._config.keys()):
            value = self._config[key]
            source = self._sources.get(key, ConfigSource.DEFAULT)
            description = self._descriptions.get(key, "")
            
            lines.append(f"{key}: {value}")
            lines.append(f"  Source: {source.name}")
            if description:
                lines.append(f"  Description: {description}")
            lines.append("")
            
        return "\n".join(lines)
        
    def __repr__(self) -> str:
        return f"Config({len(self._config)} settings)"
        
    def __getitem__(self, key: str) -> Any:
        """Dictionary-style access."""
        return self.get(key)
        
    def __setitem__(self, key: str, value: Any):
        """Dictionary-style assignment."""
        self.set(key, value)
        
    def __contains__(self, key: str) -> bool:
        """Dictionary-style membership test."""
        return self.has(key)
        
    def __len__(self) -> int:
        """Get number of configuration keys."""
        return len(self._config)
        
    def __iter__(self):
        """Iterate over configuration keys."""
        return iter(self._config.keys())
# Convenience functions

def create_default_config() -> Config:
    """Create a default configuration."""
    return Config()
        

def merge_configs(*configs: Config) -> Config:
    """Merge multiple configurations."""
    merged = Config()
    for config in configs:
        merged.update(config.to_dict())
    return merged
        

def validate_config(config: Config) -> List[str]:
    """Validate configuration and return list of errors."""
    errors = []
    
    # Check for required keys
    required_keys = ['neuron_count', 'simulation_timestep']
    for key in required_keys:
        if not config.has(key):
            errors.append(f"Missing required configuration key: {key}")
            
    # Check for valid values
    if config.has('neuron_count') and config['neuron_count'] <= 0:
        errors.append("neuron_count must be positive")
        
    if config.has('simulation_timestep') and config['simulation_timestep'] <= 0:
        errors.append("simulation_timestep must be positive")
        
    if config.has('world_width') and config['world_width'] <= 0:
        errors.append("world_width must be positive")
        
    if config.has('world_height') and config['world_height'] <= 0:
        errors.append("world_height must be positive")
        
    return errors