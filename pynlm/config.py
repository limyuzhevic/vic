"""
Configuration management for NLM Python bindings

This module provides comprehensive configuration management for NLM simulations,
including type-safe configuration access, configuration files, and validation.
"""

import json
import os
from typing import Any, Dict, Optional, Union, List
from dataclasses import dataclass, field
import copy

@dataclass
class ConfigEntry:
    """Configuration entry with metadata"""
    key: str
    value: Any
    section: str = ""
    description: str = ""
    type: str = "string"
    validation: Optional[Dict[str, Any]] = None
    default: Any = None

@dataclass
class ConfigSection:
    """Configuration section"""
    name: str
    entries: List[ConfigEntry] = field(default_factory=list)
    description: str = ""

class NLMConfig:
    """
    Enhanced configuration management for NLM simulations
    
    Provides type-safe configuration access, validation, and file I/O
    with support for nested sections and parameter validation.
    """
    
    def __init__(self, config_dict: Optional[Dict[str, Any]] = None):
        """
        Initialize configuration with optional dictionary
        
        Args:
            config_dict: Optional dictionary with initial configuration
        """
        self._sections: Dict[str, ConfigSection] = {}
        self._flat_config: Dict[str, Any] = {}
        self._validation_rules: Dict[str, Dict[str, Any]] = {}
        
        # Apply defaults
        self._apply_defaults()
        
        # Load provided configuration
        if config_dict:
            self.load_from_dict(config_dict)
    
    def _apply_defaults(self) -> None:
        """Apply default configuration values"""
        default_sections = {
            "brain": ConfigSection(
                "brain",
                description="Brain simulation parameters",
                entries=[
                    ConfigEntry("neuron_count", 1000, "brain", "Total number of neurons", "int"),
                    ConfigEntry("region_count", 2, "brain", "Number of brain regions", "int"),
                    ConfigEntry("connection_probability", 0.1, "brain", "Connection probability between neurons", "float"),
                    ConfigEntry("initial_weight_mean", 0.5, "brain", "Mean initial synaptic weight", "float"),
                    ConfigEntry("initial_weight_std", 0.1, "brain", "Standard deviation of initial weights", "float"),
                    ConfigEntry("v_thresh", -50.0, "brain", "Neuron firing threshold (mV)", "float"),
                    ConfigEntry("v_rest", -70.0, "brain", "Resting membrane potential (mV)", "float"),
                    ConfigEntry("v_reset", -75.0, "brain", "Reset potential after spike (mV)", "float"),
                    ConfigEntry("tau_mem", 20.0, "brain", "Membrane time constant (ms)", "float"),
                    ConfigEntry("tau_ref", 2.0, "brain", "Refractory period (ms)", "int"),
                ]
            ),
            "plasticity": ConfigSection(
                "plasticity",
                description="Plasticity mechanism parameters",
                entries=[
                    ConfigEntry("stdp_enable", True, "plasticity", "Enable Spike-Timing-Dependent Plasticity", "bool"),
                    ConfigEntry("stdp_learning_rate", 0.001, "plasticity", "STDP learning rate", "float"),
                    ConfigEntry("stdp_tau_plus", 20.0, "plasticity", "STDP LTP time constant (ms)", "float"),
                    ConfigEntry("stdp_tau_minus", 20.0, "plasticity", "STDP LTD time constant (ms)", "float"),
                    ConfigEntry("hebbian_enable", True, "plasticity", "Enable Hebbian learning", "bool"),
                    ConfigEntry("structural_enable", True, "plasticity", "Enable structural plasticity", "bool"),
                    ConfigEntry("synaptogenesis_rate", 0.0001, "plasticity", "Rate of new synapse formation", "float"),
                    ConfigEntry("pruning_rate", 0.00001, "plasticity", "Rate of synapse elimination", "float"),
                ]
            ),
            "neuromod": ConfigSection(
                "neuromod",
                description="Neuromodulation system parameters",
                entries=[
                    ConfigEntry("dopamine_scale", 1.0, "neuromod", "Dopamine modulation scale", "float"),
                    ConfigEntry("curiosity_enable", True, "neuromod", "Enable curiosity-driven exploration", "bool"),
                    ConfigEntry("novelty_enable", True, "neuromod", "Enable novelty detection", "bool"),
                    ConfigEntry("prediction_error_enable", True, "neuromod", "Enable prediction error signaling", "bool"),
                ]
            ),
            "simulation": ConfigSection(
                "simulation",
                description="Simulation parameters",
                entries=[
                    ConfigEntry("timestep", 0.001, "simulation", "Simulation timestep (s)", "float"),
                    ConfigEntry("max_steps", 10000, "simulation", "Maximum number of simulation steps", "int"),
                    ConfigEntry("time_limit", 0.0, "simulation", "Simulation time limit (s)", "float"),
                    ConfigEntry("checkpoint_interval", 100, "simulation", "Checkpoint save interval (steps)", "int"),
                ]
            ),
            "development": ConfigSection(
                "development",
                description="Developmental system parameters",
                entries=[
                    ConfigEntry("synaptogenesis_rate", 0.0001, "development", "Rate of synapse formation during development", "float"),
                    ConfigEntry("pruning_rate", 0.00001, "development", "Rate of synapse elimination during development", "float"),
                    ConfigEntry("plasticity_decay_rate", 0.01, "development", "Rate of plasticity decay over time", "float"),
                    ConfigEntry("critical_period_length", 10.0, "development", "Length of critical period (days)", "float"),
                ]
            ),
            "environment": ConfigSection(
                "environment",
                description="Environment parameters",
                entries=[
                    ConfigEntry("world_width", 20, "environment", "World width (units)", "int"),
                    ConfigEntry("world_height", 20, "environment", "World height (units)", "int"),
                    ConfigEntry("vision_width", 8, "environment", "Vision width (sensors)", "int"),
                    ConfigEntry("vision_height", 8, "environment", "Vision height (sensors)", "int"),
                    ConfigEntry("reward_function", "distance", "environment", "Reward function type", "string"),
                ]
            ),
            "logging": ConfigSection(
                "logging",
                description="Logging parameters",
                entries=[
                    ConfigEntry("log_level", "INFO", "logging", "Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)", "string"),
                    ConfigEntry("log_to_file", False, "logging", "Write logs to file", "bool"),
                    ConfigEntry("log_filename", "nlm.log", "logging", "Log file name", "string"),
                    ConfigEntry("log_format", "%(asctime)s - %(name)s - %(levelname)s - %(message)s", "logging", "Log format string", "string"),
                ]
            ),
        }
        
        # Add sections to configuration
        for section_name, section in default_sections.items():
            self._sections[section_name] = section
            for entry in section.entries:
                self._flat_config[f"{section_name}.{entry.key}"] = entry.default
    
    def load_from_dict(self, config_dict: Dict[str, Any]) -> None:
        """
        Load configuration from dictionary
        
        Args:
            config_dict: Dictionary with configuration values
        """
        for key, value in config_dict.items():
            self.set(key, value)
    
    def set(self, key_path: str, value: Any) -> None:
        """
        Set configuration value using dot notation
        
        Args:
            key_path: Configuration key in dot notation (e.g., "brain.neuron_count")
            value: Configuration value
        """
        # Handle special keys
        if key_path == "":
            return
        
        # Split key path
        keys = key_path.split('.')
        section_name = keys[0]
        key_name = '.'.join(keys[1:]) if len(keys) > 1 else keys[0]
        
        # Get or create section
        if section_name not in self._sections:
            self._sections[section_name] = ConfigSection(section_name)
        
        section = self._sections[section_name]
        
        # Check if entry exists
        existing_entry = None
        for entry in section.entries:
            if entry.key == key_name:
                existing_entry = entry
                break
        
        if existing_entry:
            # Update existing entry
            existing_entry.value = value
        else:
            # Create new entry
            entry_type = self._infer_type(value)
            new_entry = ConfigEntry(key_name, value, section_name, "", entry_type)
            section.entries.append(new_entry)
        
        # Update flat configuration
        self._flat_config[key_path] = value
        
        # Validate if validation rules exist
        self._validate_key(key_path, value)
    
    def get(self, key_path: str, default: Any = None) -> Any:
        """
        Get configuration value using dot notation
        
        Args:
            key_path: Configuration key in dot notation
            default: Default value if key not found
            
        Returns:
            Configuration value or default
        """
        return self._flat_config.get(key_path, default)
    
    def get_typed(self, key_path: str, value_type: type, default: Any = None) -> Any:
        """
        Get configuration value with type conversion
        
        Args:
            key_path: Configuration key in dot notation
            value_type: Desired type (int, float, str, bool)
            default: Default value if key not found
            
        Returns:
            Configuration value with type conversion or default
        """
        value = self.get(key_path, default)
        if value is None:
            return default
        
        try:
            return value_type(value)
        except (ValueError, TypeError):
            return default
    
    def get_int(self, key_path: str, default: int = 0) -> int:
        """Get integer configuration value"""
        return self.get_typed(key_path, int, default)
    
    def get_float(self, key_path: str, default: float = 0.0) -> float:
        """Get float configuration value"""
        return self.get_typed(key_path, float, default)
    
    def get_str(self, key_path: str, default: str = "") -> str:
        """Get string configuration value"""
        return self.get_typed(key_path, str, default)
    
    def get_bool(self, key_path: str, default: bool = False) -> bool:
        """Get boolean configuration value"""
        # Handle common boolean representations
        value = self.get(key_path, default)
        if isinstance(value, bool):
            return value
        elif isinstance(value, str):
            return value.lower() in ('true', 'yes', '1', 'on')
        elif isinstance(value, int):
            return bool(value)
        return default
    
    def has(self, key_path: str) -> bool:
        """
        Check if configuration key exists
        
        Args:
            key_path: Configuration key in dot notation
            
        Returns:
            True if key exists, False otherwise
        """
        return key_path in self._flat_config
    
    def remove(self, key_path: str) -> None:
        """
        Remove configuration key
        
        Args:
            key_path: Configuration key in dot notation
        """
        if key_path in self._flat_config:
            del self._flat_config[key_path]
            
            # Remove from sections
            keys = key_path.split('.')
            section_name = keys[0]
            key_name = '.'.join(keys[1:]) if len(keys) > 1 else keys[0]
            
            if section_name in self._sections:
                section = self._sections[section_name]
                section.entries = [e for e in section.entries if e.key != key_name]
    
    def get_keys(self, prefix: str = "") -> List[str]:
        """
        Get all configuration keys with optional prefix
        
        Args:
            prefix: Optional prefix to filter keys
            
        Returns:
            List of configuration keys
        """
        if prefix:
            return [k for k in self._flat_config.keys() if k.startswith(prefix)]
        return list(self._flat_config.keys())
    
    def get_sections(self) -> List[str]:
        """
        Get all section names
        
        Returns:
            List of section names
        """
        return list(self._sections.keys())
    
    def get_section_entries(self, section_name: str) -> List[ConfigEntry]:
        """
        Get all entries in a section
        
        Args:
            section_name: Section name
            
        Returns:
            List of configuration entries
        """
        if section_name in self._sections:
            return self._sections[section_name].entries
        return []
    
    def to_dict(self) -> Dict[str, Any]:
        """
        Convert configuration to dictionary
        
        Returns:
            Dictionary representation of configuration
        """
        return copy.deepcopy(self._flat_config)
    
    def save(self, filepath: str) -> bool:
        """
        Save configuration to JSON file
        
        Args:
            filepath: Path to save configuration file
            
        Returns:
            True if successful, False otherwise
        """
        try:
            with open(filepath, 'w') as f:
                json.dump(self._flat_config, f, indent=2, default=str)
            return True
        except Exception as e:
            print(f"Error saving configuration: {e}")
            return False
    
    @classmethod
    def load(cls, filepath: str) -> 'NLMConfig':
        """
        Load configuration from JSON file
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            NLMConfig instance
        """
        try:
            with open(filepath, 'r') as f:
                config_dict = json.load(f)
            return cls(config_dict)
        except Exception as e:
            print(f"Error loading configuration: {e}")
            return cls()
    
    def reset(self) -> None:
        """Reset configuration to defaults"""
        self._flat_config.clear()
        self._sections.clear()
        self._apply_defaults()
    
    def merge(self, other_config: 'NLMConfig') -> None:
        """
        Merge another configuration into this one
        
        Args:
            other_config: NLMConfig instance to merge
        """
        for key, value in other_config._flat_config.items():
            self.set(key, value)
    
    def validate(self) -> List[str]:
        """
        Validate configuration
        
        Returns:
            List of error messages, empty if valid
        """
        errors = []
        
        # Validate each key
        for key_path, value in self._flat_config.items():
            key_errors = self._validate_key(key_path, value)
            errors.extend(key_errors)
        
        return errors
    
    def _infer_type(self, value: Any) -> str:
        """
        Infer type from value
        
        Args:
            value: Value to infer type from
            
        Returns:
            Type string (int, float, str, bool, list, dict)
        """
        if isinstance(value, int):
            return "int"
        elif isinstance(value, float):
            return "float"
        elif isinstance(value, str):
            # Check for boolean-like strings
            lower_val = value.lower()
            if lower_val in ('true', 'false', 'yes', 'no', '1', '0', 'on', 'off'):
                return "bool"
            return "str"
        elif isinstance(value, bool):
            return "bool"
        elif isinstance(value, list):
            return "list"
        elif isinstance(value, dict):
            return "dict"
        else:
            return "unknown"
    
    def _validate_key(self, key_path: str, value: Any) -> List[str]:
        """
        Validate a single configuration key
        
        Args:
            key_path: Configuration key
            value: Configuration value
            
        Returns:
            List of error messages
        """
        errors = []
        
        # Check validation rules
        if key_path in self._validation_rules:
            rules = self._validation_rules[key_path]
            
            # Check range validation
            if 'range' in rules:
                range_min, range_max = rules['range']
                if not (range_min <= value <= range_max):
                    errors.append(f"Value {value} for {key_path} is outside valid range [{range_min}, {range_max}]")
            
            # Check enum validation
            if 'enum' in rules:
                if value not in rules['enum']:
                    errors.append(f"Value {value} for {key_path} is not in allowed enum {rules['enum']}")
            
            # Check regex validation
            if 'regex' in rules:
                import re
                if not re.match(rules['regex'], str(value)):
                    errors.append(f"Value {value} for {key_path} does not match regex pattern {rules['regex']}")
        
        return errors
    
    def add_validation_rule(self, key_path: str, rule_type: str, rule_value: Any) -> None:
        """
        Add validation rule for configuration key
        
        Args:
            key_path: Configuration key
            rule_type: Type of rule ('range', 'enum', 'regex')
            rule_value: Rule value
        """
        if key_path not in self._validation_rules:
            self._validation_rules[key_path] = {}
        
        self._validation_rules[key_path][rule_type] = rule_value
    
    def get_description(self, key_path: str) -> str:
        """
        Get description for configuration key
        
        Args:
            key_path: Configuration key
            
        Returns:
            Description string or empty string if not found
        """
        keys = key_path.split('.')
        section_name = keys[0]
        key_name = '.'.join(keys[1:]) if len(keys) > 1 else keys[0]
        
        if section_name in self._sections:
            section = self._sections[section_name]
            for entry in section.entries:
                if entry.key == key_name:
                    return entry.description
        
        return ""
    
    def __str__(self) -> str:
        """String representation of configuration"""
        lines = ["Configuration Summary:"]
        lines.append(f"  Sections: {len(self._sections)}")
        lines.append(f"  Entries: {len(self._flat_config)}")
        
        for section_name, section in self._sections.items():
            lines.append(f"\n{section_name.upper()}:")
            for entry in section.entries:
                lines.append(f"  {entry.key} ({entry.type}): {entry.value}")
        
        return "\n".join(lines)
    
    def __repr__(self) -> str:
        """Detailed string representation"""
        return f"NLMConfig(sections={len(self._sections)}, entries={len(self._flat_config)})"
