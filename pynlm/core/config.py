"""Configuration management for NLM."""

import json
import os
import time
from typing import Any, Dict, Optional, Union, List
from enum import Enum

from ..exceptions import ConfigurationError


class ConfigSource(Enum):
    """Configuration source enumeration."""
    DEFAULT = 0
    FILE = 1
    COMMAND_LINE = 2
    PROGRAM = 3


class ConfigEntry:
    """Single configuration entry."""
    
    def __init__(self, key: str, value: Any, source: ConfigSource, description: str = ""):
        self.key = key
        self.value = value
        self.source = source
        self.description = description
        self._timestamp = time.time()
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary for serialization."""
        return {
            'key': self.key,
            'value': self.value,
            'source': self.source.value,
            'description': self.description,
            'timestamp': self._timestamp
        }


class Config:
    """Configuration manager with JSON/YAML support."""
    
    def __init__(self):
        self._entries: Dict[str, ConfigEntry] = {}
        self._schema: Dict[str, Dict[str, Any]] = {}
        self._validators: Dict[str, callable] = {}
        
    def load_from_json(self, filepath: str, source: ConfigSource = ConfigSource.FILE):
        """
        Load configuration from JSON file.
        
        Args:
            filepath: Path to JSON file
            source: Configuration source
            
        Raises:
            ConfigurationError: If file cannot be loaded or parsed
        """
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            
            # Validate schema if defined
            self._validate_schema(data)
            
            # Apply configuration
            self._apply_dict(data, source)
            
        except FileNotFoundError:
            raise ConfigurationError(f"Configuration file not found: {filepath}")
        except json.JSONDecodeError as e:
            raise ConfigurationError(f"Invalid JSON in configuration file: {e}")
        except Exception as e:
            raise ConfigurationError(f"Error loading configuration: {e}")
    
    def load_from_yaml(self, filepath: str, source: ConfigSource = ConfigSource.FILE):
        """
        Load configuration from YAML file.
        
        Args:
            filepath: Path to YAML file
            source: Configuration source
            
        Raises:
            ConfigurationError: If file cannot be loaded or parsed
        """
        try:
            import yaml
            
            with open(filepath, 'r') as f:
                data = yaml.safe_load(f)
            
            # Validate schema if defined
            self._validate_schema(data)
            
            # Apply configuration
            self._apply_dict(data, source)
            
        except ImportError:
            raise ConfigurationError("PyYAML not installed. Install with: pip install pyyaml")
        except FileNotFoundError:
            raise ConfigurationError(f"Configuration file not found: {filepath}")
        except yaml.YAMLError as e:
            raise ConfigurationError(f"Invalid YAML in configuration file: {e}")
        except Exception as e:
            raise ConfigurationError(f"Error loading configuration: {e}")
    
    def load_from_file(self, filepath: str, source: ConfigSource = ConfigSource.FILE):
        """
        Load configuration from file (auto-detects format).
        
        Args:
            filepath: Path to configuration file
            source: Configuration source
            
        Raises:
            ConfigurationError: If file cannot be loaded
        """
        if filepath.endswith('.json'):
            self.load_from_json(filepath, source)
        elif filepath.endswith('.yaml') or filepath.endswith('.yml'):
            self.load_from_yaml(filepath, source)
        elif filepath.endswith('.ini') or filepath.endswith('.cfg'):
            self._load_from_ini(filepath, source)
        else:
            # Try to detect format
            try:
                self.load_from_json(filepath, source)
            except ConfigurationError:
                try:
                    self.load_from_yaml(filepath, source)
                except ConfigurationError:
                    self._load_from_ini(filepath, source)
    
    def _load_from_ini(self, filepath: str, source: ConfigSource):
        """Load configuration from INI file."""
        try:
            import configparser
            
            config = configparser.ConfigParser()
            config.read(filepath)
            
            for section in config.sections():
                for key, value in config.items(section):
                    full_key = f"{section}.{key}" if section else key
                    self.set(full_key, value, source)
                    
        except ImportError:
            raise ConfigurationError("configparser not available")
        except Exception as e:
            raise ConfigurationError(f"Error loading INI configuration: {e}")
    
    def save_to_json(self, filepath: str):
        """
        Save configuration to JSON file.
        
        Args:
            filepath: Path to save JSON file
            
        Raises:
            ConfigurationError: If file cannot be saved
        """
        try:
            data = self.to_dict()
            
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
                
        except Exception as e:
            raise ConfigurationError(f"Error saving configuration: {e}")
    
    def save_to_yaml(self, filepath: str):
        """
        Save configuration to YAML file.
        
        Args:
            filepath: Path to save YAML file
            
        Raises:
            ConfigurationError: If file cannot be saved
        """
        try:
            import yaml
            
            data = self._to_dict()
            
            with open(filepath, 'w') as f:
                yaml.dump(data, f, default_flow_style=False)
                
        except ImportError:
            raise ConfigurationError("PyYAML not installed")
        except Exception as e:
            raise ConfigurationError(f"Error saving configuration: {e}")
    
    def save_to_file(self, filepath: str):
        """
        Save configuration to file (auto-detects format).
        
        Args:
            filepath: Path to save configuration file
        """
        if filepath.endswith('.json'):
            self.save_to_json(filepath)
        elif filepath.endswith('.yaml') or filepath.endswith('.yml'):
            self.save_to_yaml(filepath)
        else:
            # Default to JSON
            self.save_to_json(filepath)
            
    # CamelCase aliases for test compatibility
    def saveToFile(self, filepath: str):
        """Save configuration to file (camelCase alias)."""
        self.save_to_file(filepath)
        
    def loadFromFile(self, filepath: str, source: ConfigSource = ConfigSource.FILE):
        """Load configuration from file (camelCase alias)."""
        self.load_from_file(filepath, source)
    
    def set(self, key: str, value: Any, source: ConfigSource = ConfigSource.DEFAULT,
            description: str = ""):
        """
        Set configuration value.
        
        Args:
            key: Configuration key (dot notation for nested)
            value: Value to set
            source: Configuration source
            description: Description for this configuration
        """
        # Validate value if validator exists
        if key in self._validators:
            validator = self._validators[key]
            try:
                validator(value)
            except ValueError as e:
                raise ConfigurationError(f"Invalid value for {key}: {e}")
        
        self._entries[key] = ConfigEntry(key, value, source, description)
    
    def set_from_dict(self, data: Dict[str, Any], source: ConfigSource = ConfigSource.PROGRAM,
                     description: str = ""):
        """
        Set multiple configuration values from dictionary.
        
        Args:
            data: Dictionary of configuration values
            source: Configuration source
            description: Description for these configuration values
        """
        for key, value in data.items():
            self.set(key, value, source, description)
    
    def get(self, key: str) -> Optional[Any]:
        """
        Get configuration value.
        
        Args:
            key: Configuration key
            
        Returns:
            Configuration value or None if not found
        """
        entry = self._entries.get(key)
        return entry.value if entry else None
    
    def get_or(self, key: str, default_value: Any = None) -> Any:
        """
        Get configuration value with default.
        
        Args:
            key: Configuration key
            default_value: Default value if key not found
            
        Returns:
            Configuration value or default
        """
        value = self.get(key)
        return value if value is not None else default_value
    
    def get_int(self, key: str) -> Optional[int]:
        """Get configuration value as integer."""
        value = self.get(key)
        return int(value) if value is not None else None
    
    def get_float(self, key: str) -> Optional[float]:
        """Get configuration value as float."""
        value = self.get(key)
        return float(value) if value is not None else None
    
    def get_bool(self, key: str) -> Optional[bool]:
        """Get configuration value as boolean."""
        value = self.get(key)
        return bool(value) if value is not None else None
    
    def get_string(self, key: str) -> Optional[str]:
        """Get configuration value as string."""
        value = self.get(key)
        return str(value) if value is not None else None
    
    def has(self, key: str) -> bool:
        """
        Check if configuration has key.
        
        Args:
            key: Configuration key
            
        Returns:
            True if key exists, False otherwise
        """
        return key in self._entries
    
    def remove(self, key: str):
        """Remove configuration entry."""
        self._entries.pop(key, None)
    
    def clear(self):
        """Clear all configuration entries."""
        self._entries.clear()
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys."""
        return list(self._entries.keys())
        
    # CamelCase aliases for test compatibility
    def getKeys(self) -> List[str]:
        """Get all configuration keys (camelCase alias)."""
        return self.get_keys()
        """
        Get all configuration entries for specific source.
        
        Args:
            source: Configuration source
            
        Returns:
            Dictionary of configuration values
        """
        return {key: entry.value for key, entry in self._entries.items()
                if entry.source == source}
    
    def get_source(self, key: str) -> Optional[ConfigSource]:
        """
        Get configuration source for key.
        
        Args:
            key: Configuration key
            
        Returns:
            Configuration source or None
        """
        entry = self._entries.get(key)
        return entry.source if entry else None
    
    def override_from_dict(self, data: Dict[str, Any], source: ConfigSource = ConfigSource.COMMAND_LINE):
        """
        Override configuration from dictionary.
        
        Args:
            data: Dictionary of overrides
            source: Configuration source for overrides
        """
        for key, value in data.items():
            self.set(key, value, source)
    
    def merge_from_dict(self, data: Dict[str, Any], source: ConfigSource = ConfigSource.FILE):
        """
        Merge configuration from dictionary (only sets missing keys).
        
        Args:
            data: Dictionary of configuration values
            source: Configuration source
        """
        for key, value in data.items():
            if not self.has(key):
                self.set(key, value, source)
    
    def summary(self) -> str:
        """Get configuration summary."""
        lines = [f"Configuration ({len(self._entries)} entries):"]
        
        for entry in sorted(self._entries.values(), key=lambda e: e.key):
            lines.append(f"  {entry.key} = [{entry.value}] ({entry.source.name})")
        
        return "\n".join(lines)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        return {key: entry.value for key, entry in self._entries.items()}
    
    def _apply_dict(self, data: Dict[str, Any], source: ConfigSource):
        """Apply dictionary to configuration."""
        # Handle nested structure
        def _flatten_dict(d: Dict[str, Any], parent_key: str = "") -> Dict[str, Any]:
            items = []
            for k, v in d.items():
                new_key = f"{parent_key}.{k}" if parent_key else k
                if isinstance(v, dict):
                    items.extend(_flatten_dict(v, new_key).items())
                else:
                    items.append((new_key, v))
            return dict(items)
        
        flattened = _flatten_dict(data)
        for key, value in flattened.items():
            self.set(key, value, source)
    
    def _validate_schema(self, data: Dict[str, Any]):
        """Validate configuration against schema if defined."""
        if not self._schema:
            return
        
        def _validate_dict(d: Dict[str, Any], schema: Dict[str, Any], path: str = ""):
            for key, expected_type in schema.items():
                full_key = f"{path}.{key}" if path else key
                if key in d:
                    value = d[key]
                    if isinstance(expected_type, type):
                        if not isinstance(value, expected_type):
                            raise ConfigurationError(
                                f"Invalid type for {full_key}: expected {expected_type}, got {type(value)}"
                            )
                    elif isinstance(expected_type, dict):
                        if isinstance(value, dict):
                            _validate_dict(value, expected_type, full_key)
                        else:
                            raise ConfigurationError(
                                f"Invalid type for {full_key}: expected dict, got {type(value)}"
                            )
                    elif isinstance(expected_type, list):
                        if not isinstance(value, list):
                            raise ConfigurationError(
                                f"Invalid type for {full_key}: expected list, got {type(value)}"
                            )
                    # Add more type checks as needed
        
        _validate_dict(data, self._schema)
    
    def add_validator(self, key: str, validator: callable, description: str = ""):
        """
        Add validator for configuration key.
        
        Args:
            key: Configuration key
            validator: Validation function
            description: Description of validation
        """
        self._validators[key] = validator
    
    def add_schema(self, schema: Dict[str, Any], description: str = ""):
        """
        Add schema definition for configuration.
        
        Args:
            schema: Schema dictionary
            description: Description of schema
        """
        self._schema.update(schema)
    
    def validate(self) -> List[str]:
        """
        Validate configuration against all validators.
        
        Returns:
            List of validation errors (empty if valid)
        """
        errors = []
        
        for key, entry in self._entries.items():
            if key in self._validators:
                validator = self._validators[key]
                try:
                    validator(entry.value)
                except ValueError as e:
                    errors.append(f"{key}: {e}")
        
        return errors
    
    def copy(self) -> 'Config':
        """Create a copy of configuration."""
        new_config = Config()
        new_config._entries = self._entries.copy()
        new_config._schema = self._schema.copy()
        new_config._validators = self._validators.copy()
        return new_config
    
    def merge(self, other: 'Config', override: bool = True):
        """
        Merge another configuration into this one.
        
        Args:
            other: Configuration to merge
            override: Whether to override existing keys
        """
        for key, entry in other._entries.items():
            if override or not self.has(key):
                self._entries[key] = entry


# Factory functions
def create_default_config() -> Config:
    """Create default configuration with all NLM settings."""
    config = Config()
    
    # Basic configuration
    config.set("brain.neuron_count", 1000, ConfigSource.DEFAULT, "Number of neurons")
    config.set("brain.region_count", 1, ConfigSource.DEFAULT, "Number of brain regions")
    config.set("brain.synapse_density", 0.1, ConfigSource.DEFAULT, "Synapse connectivity density")
    config.set("brain.connection_probability", 0.05, ConfigSource.DEFAULT, 
               "Probability of connection between neurons")
    
    # Neuron parameters
    config.set("brain.v_thresh", -50.0, ConfigSource.DEFAULT, "Neuron threshold potential")
    config.set("brain.v_rest", -70.0, ConfigSource.DEFAULT, "Neuron resting potential")
    config.set("brain.v_reset", -75.0, ConfigSource.DEFAULT, "Neuron reset potential")
    config.set("brain.tau_mem", 20.0, ConfigSource.DEFAULT, "Membrane time constant")
    config.set("brain.tau_ref", 2.0, ConfigSource.DEFAULT, "Refractory period")
    
    # STDP parameters
    config.set("plasticity.stdp.enable", True, ConfigSource.DEFAULT, "Enable STDP")
    config.set("plasticity.stdp.learning_rate", 0.001, ConfigSource.DEFAULT, "STDP learning rate")
    config.set("plasticity.stdp.tau_plus", 20.0, ConfigSource.DEFAULT, "STDP potentiation time constant")
    config.set("plasticity.stdp.tau_minus", 20.0, ConfigSource.DEFAULT, "STDP depression time constant")
    
    # Hebbian parameters
    config.set("plasticity.hebbian.enable", True, ConfigSource.DEFAULT, "Enable Hebbian learning")
    config.set("plasticity.hebbian.learning_rate", 0.01, ConfigSource.DEFAULT, "Hebbian learning rate")
    
    # Structural plasticity parameters
    config.set("plasticity.structural.enable", True, ConfigSource.DEFAULT, "Enable structural plasticity")
    config.set("plasticity.structural.synaptogenesis_rate", 0.0001, ConfigSource.DEFAULT,
               "Rate of new synapse formation")
    config.set("plasticity.structural.pruning_rate", 0.00001, ConfigSource.DEFAULT,
               "Rate of synapse elimination")
    
    # Neuromodulation parameters
    config.set("neuromod.dopamine.scale", 1.0, ConfigSource.DEFAULT, "Dopamine modulation scale")
    config.set("neuromod.curiosity.enable", True, ConfigSource.DEFAULT, "Enable curiosity-driven exploration")
    config.set("neuromod.novelty.enable", True, ConfigSource.DEFAULT, "Enable novelty detection")
    
    # Development parameters
    config.set("development.stages.critical_period.enabled", True, ConfigSource.DEFAULT,
               "Enable critical period plasticity")
    config.set("development.stages.maturation.rate", 0.5, ConfigSource.DEFAULT,
               "Rate of maturation")
    config.set("development.stages.aging.rate", 0.01, ConfigSource.DEFAULT,
               "Rate of aging effects")
    
    # Simulation parameters
    config.set("simulation.timestep", 0.001, ConfigSource.DEFAULT, "Simulation timestep (s)")
    config.set("simulation.max_steps", 10000, ConfigSource.DEFAULT, "Maximum simulation steps")
    config.set("simulation.random_seed", 42, ConfigSource.DEFAULT, "Random seed")
    
    # Memory system parameters
    config.set("memory.working_capacity", 100, ConfigSource.DEFAULT, "Working memory capacity")
    config.set("memory.episodic_capacity", 1000, ConfigSource.DEFAULT, "Episodic memory capacity")
    config.set("memory.associative_strength", 0.5, ConfigSource.DEFAULT, "Associative memory strength")
    
    # Prediction system parameters
    config.set("prediction.horizon", 10, ConfigSource.DEFAULT, "Prediction horizon steps")
    config.set("prediction.confidence_threshold", 0.7, ConfigSource.DEFAULT,
               "Confidence threshold for predictions")
    
    # Cognition parameters
    config.set("cognition.planning.horizon", 20, ConfigSource.DEFAULT, "Planning horizon")
    config.set("cognition.attention.gain", 1.0, ConfigSource.DEFAULT, "Attention gain")
    
    return config


def load_config_from_file(filepath: str, source: ConfigSource = ConfigSource.FILE) -> Config:
    """
    Load configuration from file with auto-detection.
    
    Args:
        filepath: Path to configuration file
        source: Configuration source
        
    Returns:
        Configuration object
    """
    config = Config()
    config.load_from_file(filepath, source)
    return config


def save_config_to_file(config: Config, filepath: str):
    """
    Save configuration to file.
    
    Args:
        config: Configuration to save
        filepath: Path to save file
    """
    config.save_to_file(filepath)


def override_config_from_dict(config: Config, overrides: Dict[str, Any],
                              source: ConfigSource = ConfigSource.COMMAND_LINE):
    """
    Override configuration from dictionary.
    
    Args:
        config: Configuration to override
        overrides: Dictionary of overrides
        source: Configuration source for overrides
    """
    config.override_from_dict(overrides, source)