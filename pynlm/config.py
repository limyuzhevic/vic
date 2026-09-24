# Configuration management for NLM

"""
Configuration management for NLM (Neural Learning Machine).

This module provides classes and functions for managing brain configuration
parameters, including loading from files, saving to files, and runtime modifications.
"""

from ._cpp import Config, ConfigEntry, ConfigSource, ConfigValue
from enum import Enum
import json
import os
from typing import Any, Dict, Optional, Union, List

__all__ = [
    'Config', 'ConfigEntry', 'ConfigSource', 'ConfigValue',
    'createDefaultConfig', 'createMinimalConfig',
    'loadConfigFromFile', 'saveConfigToFile',
    'validateConfig', 'mergeConfig',
    'CONFIG_SCHEMA', 'DEFAULT_CONFIG'
]


# Default configuration values
DEFAULT_CONFIG = {
    'neuron_count': 100,
    'region_count': 2,
    'connection_probability': 0.1,
    'simulation_timestep': 0.001,
    'random_seed': 42,
    'plasticity_learning_rate': 0.01,
    'stdp_ltp_weight': 0.01,
    'stdp_ltd_weight': 0.012,
    'stdp_time_constant': 20.0,
    'development_synaptogenesis_rate': 0.001,
    'development_pruning_rate': 0.0001,
    'dopamine_baseline': 0.0,
    'reward_discount_factor': 0.99,
    'environment_name': 'GridWorld',
    'environment_width': 10,
    'environment_height': 10,
    'log_level': 'INFO',
    'log_to_file': False,
    'log_filename': 'nlm.log',
    'max_simulation_steps': 10000,
    'simulation_time_limit': 0.0,
    'visualization_enabled': False,
    'visualization_update_rate': 30.0,
    # Phase 2+ extensions
    'synaptogenesis_rate': 0.0001,
    'pruning_rate': 0.00001,
    'long_term_potentiation': True,
    'long_term_depression': True,
    'meta_plasticity': False,
    'dopamine_modulation_strength': 1.0,
    'curiosity_modulation_strength': 0.5,
    'novelty_modulation_strength': 0.3,
    'development_stage_duration': 100.0,
    'critical_period_enabled': True,
    'maturation_rate': 0.001,
    'working_memory_capacity': 100,
    'episodic_memory_capacity': 1000,
    'associative_memory_threshold': 0.5,
    'planning_horizon': 10,
    'attention_span': 50,
    'novelty_threshold': 0.1,
    'environment_complexity': 1.0,
    'world_obj_randomization': True,
    'terrain_modulation': False,
}


# Configuration schema for validation
CONFIG_SCHEMA = {
    'types': {
        'neuron_count': (int, lambda x: x > 0),
        'region_count': (int, lambda x: x > 0),
        'connection_probability': (float, lambda x: 0.0 <= x <= 1.0),
        'simulation_timestep': (float, lambda x: x > 0.0),
        'random_seed': (int, lambda x: True),
        'plasticity_learning_rate': (float, lambda x: x >= 0.0),
        'stdp_ltp_weight': (float, lambda x: x >= 0.0),
        'stdp_ltd_weight': (float, lambda x: x >= 0.0),
        'stdp_time_constant': (float, lambda x: x > 0.0),
        'development_synaptogenesis_rate': (float, lambda x: x >= 0.0),
        'development_pruning_rate': (float, lambda x: x >= 0.0),
        'dopamine_baseline': (float, lambda x: x >= 0.0),
        'reward_discount_factor': (float, lambda x: 0.0 <= x <= 1.0),
        'environment_width': (int, lambda x: x > 0),
        'environment_height': (int, lambda x: x > 0),
        'max_simulation_steps': (int, lambda x: x > 0),
        'simulation_time_limit': (float, lambda x: x >= 0.0),
        'visualization_update_rate': (float, lambda x: x > 0.0),
        'synaptogenesis_rate': (float, lambda x: x >= 0.0),
        'pruning_rate': (float, lambda x: x >= 0.0),
        'dopamine_modulation_strength': (float, lambda x: x >= 0.0),
        'curiosity_modulation_strength': (float, lambda x: x >= 0.0),
        'novelty_modulation_strength': (float, lambda x: x >= 0.0),
        'development_stage_duration': (float, lambda x: x > 0.0),
        'maturation_rate': (float, lambda x: x >= 0.0),
        'working_memory_capacity': (int, lambda x: x > 0),
        'episodic_memory_capacity': (int, lambda x: x > 0),
        'associative_memory_threshold': (float, lambda x: 0.0 <= x <= 1.0),
        'planning_horizon': (int, lambda x: x > 0),
        'attention_span': (int, lambda x: x > 0),
        'novelty_threshold': (float, lambda x: x >= 0.0),
        'environment_complexity': (float, lambda x: x >= 0.0),
    },
    'required': [],  # No required fields - allows for minimal config
    'deprecated': [
        'development_synaptogenesis_rate',  # Use synaptogenesis_rate
        'development_pruning_rate',        # Use pruning_rate
    ]
}


class ConfigManager:
    """
    Advanced configuration manager for NLM with validation and merging capabilities.

    This class provides advanced configuration management features including
    validation, merging, loading/saving, and version control.
    """

    def __init__(self):
        """Initialize configuration manager."""
        self.config = Config()
        self._history = []
        self._version = 1

    def load_from_file(self, filepath: str, validate: bool = True) -> bool:
        """
        Load configuration from a file.

        Args:
            filepath: Path to the configuration file.
            validate: Whether to validate the configuration.

        Returns:
            True if loading succeeded, False otherwise.
        """
        try:
            if filepath.endswith('.json'):
                return self._load_from_json(filepath, validate)
            elif filepath.endswith('.cfg') or filepath.endswith('.txt'):
                return self._load_from_text(filepath, validate)
            else:
                # Try to detect format
                return self._load_from_json(filepath, validate)
        except Exception as e:
            print(f"Error loading configuration from {filepath}: {e}")
            return False

    def _load_from_json(self, filepath: str, validate: bool) -> bool:
        """Load configuration from JSON file."""
        with open(filepath, 'r') as f:
            data = json.load(f)

        # Apply configuration
        for key, value in data.items():
            self.config.set(key, value, ConfigSource::File)

        if validate:
            return self._validate()

        self._add_to_history('load_from_json', filepath, data)
        return True

    def _load_from_text(self, filepath: str, validate: bool) -> bool:
        """Load configuration from text file (INI-like format)."""
        with open(filepath, 'r') as f:
            lines = f.readlines()

        config_data = {}
        for line in lines:
            line = line.strip()
            if line and not line.startswith('#') and '=' in line:
                key, value = line.split('=', 1)
                key = key.strip()
                value = value.strip()

                # Try to convert value to appropriate type
                try:
                    if '.' in value:
                        value = float(value)
                    else:
                        value = int(value)
                    if value == 'true':
                        value = True
                    elif value == 'false':
                        value = False
                except ValueError:
                    pass

                config_data[key] = value

        # Apply configuration
        for key, value in config_data.items():
            if isinstance(value, (int, float, bool, str)):
                self.config.set(key, value, ConfigSource::File)

        if validate:
            return self._validate()

        self._add_to_history('load_from_text', filepath, config_data)
        return True

    def save_to_file(self, filepath: str) -> bool:
        """
        Save configuration to a file.

        Args:
            filepath: Path where to save the configuration file.

        Returns:
            True if saving succeeded, False otherwise.
        """
        try:
            if filepath.endswith('.json'):
                return self._save_to_json(filepath)
            else:
                # Default to JSON format
                return self._save_to_json(filepath + '.json')
        except Exception as e:
            print(f"Error saving configuration to {filepath}: {e}")
            return False

    def _save_to_json(self, filepath: str) -> bool:
        """Save configuration to JSON file."""
        data = {}

        # Get all configuration keys
        keys = self.config.getKeys()
        for key in keys:
            value = self.config.get(key)
            if value is not None:
                data[key] = value

        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)

        self._add_to_history('save_to_json', filepath, data)
        return True

    def _validate(self) -> bool:
        """Validate the current configuration."""
        errors = []

        # Check schema
        for key, (expected_type, validator) in CONFIG_SCHEMA['types'].items():
            if self.config.has(key):
                value = self.config.get(key)
                if value is not None:
                    if not isinstance(value, expected_type):
                        errors.append(f"Key '{key}' expects type {expected_type}, got {type(value)}")
                    elif not validator(value):
                        errors.append(f"Key '{key}' value {value} is invalid")

        # Check deprecated keys
        for key in CONFIG_SCHEMA['deprecated']:
            if self.config.has(key):
                print(f"Warning: Configuration key '{key}' is deprecated")

        if errors:
            print("Configuration validation errors:")
            for error in errors:
                print(f"  {error}")
            return False

        return True

    def _add_to_history(self, action: str, details: str, data: Dict):
        """Add configuration change to history."""
        change = {
            'action': action,
            'details': details,
            'data': data,
            'timestamp': time.time(),
            'version': self._version
        }
        self._history.append(change)
        self._version += 1

    def get_history(self) -> List[Dict]:
        """Get configuration change history."""
        return self._history.copy()

    def get_version(self) -> int:
        """Get current configuration version."""
        return self._version

    def merge_config(self, other_config: 'ConfigManager') -> bool:
        """
        Merge another configuration into this one.

        Args:
            other_config: Another ConfigManager instance to merge.

        Returns:
            True if merge succeeded, False otherwise.
        """
        try:
            for key in other_config.config.getKeys():
                value = other_config.config.get(key)
                if value is not None:
                    self.config.set(key, value, ConfigSource::Runtime)

            return True
        except Exception as e:
            print(f"Error merging configuration: {e}")
            return False


def createDefaultConfig() -> Config:
    """
    Create a default configuration with sensible defaults.

    Returns:
        Config: A configuration instance with default values.
    """
    config = Config()

    # Apply all default values
    for key, value in DEFAULT_CONFIG.items():
        config.set(key, value, ConfigSource::Default)

    return config


def createMinimalConfig() -> Config:
    """
    Create a minimal configuration with only essential settings.

    Returns:
        Config: A minimal configuration instance.
    """
    config = Config()

    # Only essential settings
    essential_keys = ['neuron_count', 'region_count', 'simulation_timestep', 'random_seed']
    for key in essential_keys:
        if key in DEFAULT_CONFIG:
            config.set(key, DEFAULT_CONFIG[key], ConfigSource::Default)

    return config


def loadConfigFromFile(filepath: str, validate: bool = True) -> Optional[Config]:
    """
    Load configuration from a file.

    Args:
        filepath: Path to the configuration file.
        validate: Whether to validate the configuration.

    Returns:
        Loaded configuration or None if loading failed.
    """
    config = Config()

    try:
        if filepath.endswith('.json'):
            with open(filepath, 'r') as f:
                data = json.load(f)
            for key, value in data.items():
                config.set(key, value, ConfigSource::File)
        else:
            # Try to load as text config
            with open(filepath, 'r') as f:
                lines = f.readlines()
            for line in lines:
                line = line.strip()
                if line and not line.startswith('#') and '=' in line:
                    key, value = line.split('=', 1)
                    key = key.strip()
                    value = value.strip()

                    # Try to infer type
                    try:
                        if value.lower() == 'true':
                            value = True
                        elif value.lower() == 'false':
                            value = False
                        elif '.' in value:
                            value = float(value)
                        else:
                            value = int(value)
                    except ValueError:
                        pass

                    config.set(key, value, ConfigSource::File)

        if validate:
            # Create temporary manager for validation
            temp_manager = ConfigManager()
            temp_manager.config = config
            if not temp_manager._validate():
                return None

        return config

    except Exception as e:
        print(f"Error loading configuration from {filepath}: {e}")
        return None


def saveConfigToFile(config: Config, filepath: str) -> bool:
    """
    Save configuration to a file.

    Args:
        config: Configuration to save.
        filepath: Path where to save the configuration file.

    Returns:
        True if saving succeeded, False otherwise.
    """
    try:
        data = {}
        for key in config.getKeys():
            value = config.get(key)
            if value is not None:
                data[key] = value

        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)

        return True
    except Exception as e:
        print(f"Error saving configuration to {filepath}: {e}")
        return False


def validateConfig(config: Config) -> List[str]:
    """
    Validate a configuration.

    Args:
        config: Configuration to validate.

    Returns:
        List of error messages. Empty list if configuration is valid.
    """
    errors = []

    # Create temporary manager for validation
    temp_manager = ConfigManager()
    temp_manager.config = config

    if not temp_manager._validate():
        # Get errors from validation (this would need to be enhanced)
        errors.append("Configuration validation failed")

    return errors


def mergeConfig(base_config: Config, override_config: Config) -> Config:
    """
    Merge two configurations.

    Args:
        base_config: Base configuration.
        override_config: Configuration to merge on top of base.

    Returns:
        Merged configuration.
    """
    result = Config()

    # Copy base configuration
    for key in base_config.getKeys():
        value = base_config.get(key)
        if value is not None:
            result.set(key, value, ConfigSource::File)

    # Override with other configuration
    for key in override_config.getKeys():
        value = override_config.get(key)
        if value is not None:
            result.set(key, value, ConfigSource::Runtime)

    return result


# Convenience type checking functions
def isInt(value: Any) -> bool:
    """Check if value is an integer."""
    return isinstance(value, int) and not isinstance(value, bool)


def isFloat(value: Any) -> bool:
    """Check if value is a float."""
    return isinstance(value, float)


def isString(value: Any) -> bool:
    """Check if value is a string."""
    return isinstance(value, str)


def isBool(value: Any) -> bool:
    """Check if value is a boolean."""
    return isinstance(value, bool)


def isVector(value: Any) -> bool:
    """Check if value is a vector (list)."""
    return isinstance(value, list)