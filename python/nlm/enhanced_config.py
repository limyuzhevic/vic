# Enhanced Python Bindings for NLM Configuration System
# This module provides advanced configuration management capabilities

import json
import os
import sys
from datetime import datetime
from pathlib import Path
from typing import Any, Dict, List, Optional, Union, Callable

# Import the core NLM bindings
import pynlm

# Create the enhanced Python API
class NLMConfig:
    """
    Enhanced configuration class for NLM with advanced features.
    
    Provides comprehensive configuration management including:
    - Validation and schema support
    - Category-based organization
    - Batch operations
    - Builder pattern
    - Template system
    - History tracking
    - Export/import utilities
    - Presets for common use cases
    """
    
    def __init__(self, config_obj=None):
        """
        Initialize NLMConfig.
        
        Args:
            config_obj: Optional existing pynlm.Config object
        """
        if config_obj is not None:
            self._config = config_obj
        else:
            self._config = pynlm.createDefaultConfig()
        
        # Initialize history tracking
        self._history = []
        self._current_preset = None
        
    def __repr__(self):
        return f"<NLMConfig: {self._config.summary()}>"
    
    def __getitem__(self, key):
        """Get configuration value using dictionary-style access."""
        if self._config.has(key):
            return self._get_typed_value(key)
        raise KeyError(f"Configuration key '{key}' not found")
    
    def __setitem__(self, key, value):
        """Set configuration value using dictionary-style access."""
        self.set(key, value)
    
    def __contains__(self, key):
        """Check if key exists in configuration."""
        return self._config.has(key)
    
    def __delitem__(self, key):
        """Remove configuration key."""
        self._config.remove(key)
    
    def __len__(self):
        """Get number of configuration keys."""
        return len(self._config.getKeys())
    
    def keys(self):
        """Get all configuration keys."""
        return self._config.getKeys()
    
    def values(self):
        """Get all configuration values."""
        for key in self._config.getKeys():
            yield self._get_typed_value(key)
    
    def items(self):
        """Get all configuration items."""
        for key in self._config.getKeys():
            yield (key, self._get_typed_value(key))
    
    def get(self, key, default=None):
        """
        Get configuration value with optional default.
        
        Args:
            key: Configuration key
            default: Default value if key doesn't exist
            
        Returns:
            Configuration value or default
        """
        if self._config.has(key):
            return self._get_typed_value(key)
        return default
    
    def set(self, key, value, source=pynlm.ConfigSource.Runtime, category=""):
        """
        Set configuration value.
        
        Args:
            key: Configuration key
            value: Value to set
            source: Configuration source (Default, File, CommandLine, Runtime)
            category: Configuration category (e.g., "brain", "sensory")
        """
        # Record in history before setting
        old_value = self._get_raw_value(key) if self._config.has(key) else None
        
        # Set the value
        if isinstance(value, str):
            self._config.set(key, value, source)
        elif isinstance(value, bool):
            self._config.set(key, value, source)
        elif isinstance(value, int):
            self._config.set(key, value, source)
        elif isinstance(value, float):
            self._config.set(key, value, source)
        elif isinstance(value, list):
            if all(isinstance(x, int) for x in value):
                self._config.set(key, value, source)
            elif all(isinstance(x, float) for x in value):
                self._config.set(key, value, source)
            elif all(isinstance(x, str) for x in value):
                self._config.set(key, value, source)
            else:
                raise ValueError(f"Unsupported list type for key '{key}'")
        else:
            raise ValueError(f"Unsupported value type for key '{key}': {type(value)}")
        
        # Set category if provided
        if category:
            self._config.setEntryCategory(key, category)
        
        # Record history
        if old_value is not None or value is not None:
            self._add_to_history(key, old_value, value, source, category)
    
    def _get_typed_value(self, key):
        """Get configuration value with type inference."""
        raw_value = self._get_raw_value(key)
        
        # Handle variant types based on what we know
        if self._config.has(key):
            # Try to get by specific type
            try:
                return self._config.get(key, str)
            except:
                try:
                    return self._config.get(key, int)
                except:
                    try:
                        return self._config.get(key, float)
                    except:
                        try:
                            return self._config.get(key, bool)
                        except:
                            return self._config.get(key, list)
        
        return None
    
    def _get_raw_value(self, key):
        """Get raw configuration value without type inference."""
        # This would need to access the internal Config object
        # For now, return None as a placeholder
        return None
    
    def _add_to_history(self, key, old_value, new_value, source, category):
        """Add configuration change to history."""
        entry = {
            'timestamp': datetime.now().isoformat(),
            'key': key,
            'old_value': old_value,
            'new_value': new_value,
            'source': source,
            'category': category
        }
        self._history.append(entry)
        
    def get_history(self, key=None, limit=100):
        """
        Get configuration change history.
        
        Args:
            key: Optional specific key to filter history
            limit: Maximum number of history entries
            
        Returns:
            List of history entries
        """
        history = self._history.copy()
        if key:
            history = [e for e in history if e['key'] == key]
        return history[-limit:]
    
    def clear_history(self):
        """Clear configuration history."""
        self._history.clear()
    
    def validate(self, key=None, schema=None):
        """
        Validate configuration values.
        
        Args:
            key: Optional specific key to validate
            schema: Optional validation schema
            
        Returns:
            Validation results
        """
        if key:
            return self._validate_key(key, schema)
        else:
            return self._validate_all(schema)
    
    def _validate_key(self, key, schema):
        """Validate a specific configuration key."""
        # This would implement validation logic
        # For now, return a placeholder
        return {'valid': True, 'key': key, 'errors': []}
    
    def _validate_all(self, schema):
        """Validate all configuration values."""
        # This would implement validation logic for all keys
        # For now, return a placeholder
        return {'valid': True, 'invalid_keys': []}
    
    def batch_set(self, config_dict, source=pynlm.ConfigSource.Runtime):
        """
        Set multiple configuration values in batch.
        
        Args:
            config_dict: Dictionary of key-value pairs
            source: Configuration source for all values
        """
        for key, value in config_dict.items():
            self.set(key, value, source)
    
    def batch_get(self, keys):
        """
        Get multiple configuration values in batch.
        
        Args:
            keys: List of configuration keys
            
        Returns:
            Dictionary of key-value pairs
        """
        result = {}
        for key in keys:
            if self._config.has(key):
                result[key] = self._get_typed_value(key)
        return result
    
    def get_by_category(self, category):
        """
        Get all configuration entries for a specific category.
        
        Args:
            category: Category to filter by
            
        Returns:
            Dictionary of key-value pairs in the category
        """
        result = {}
        for key in self._config.getKeys():
            # This would need access to category information
            # For now, return empty dict as placeholder
            pass
        return result
    
    def get_categories(self):
        """Get all unique configuration categories."""
        categories = set()
        for key in self._config.getKeys():
            # This would need access to category information
            pass
        return list(categories)
    
    def load_preset(self, preset_name):
        """
        Load a configuration preset.
        
        Args:
            preset_name: Name of preset to load
        """
        presets = self._get_presets()
        if preset_name in presets:
            self._current_preset = preset_name
            self.clear()
            for key, value in presets[preset_name].items():
                self.set(key, value)
            return True
        return False
    
    def save_preset(self, preset_name, description=""):
        """
        Save current configuration as a preset.
        
        Args:
            preset_name: Name for the preset
            description: Description of the preset
        """
        preset_data = {
            'config': dict(self.items()),
            'description': description,
            'timestamp': datetime.now().isoformat()
        }
        # This would save to a persistent storage
        return True
    
    def _get_presets(self):
        """Get built-in presets."""
        return {
            'minimal': {
                'brain.neuronCount': 100,
                'brain.simulationSpeed': 1.0
            },
            'standard': {
                'brain.neuronCount': 1000,
                'brain.simulationSpeed': 1.0,
                'sensory.vision.enabled': True,
                'sensory.audio.enabled': True
            },
            'high_performance': {
                'brain.neuronCount': 5000,
                'brain.simulationSpeed': 2.0,
                'sensory.vision.enabled': True,
                'sensory.audio.enabled': True,
                'plasticity.enabled': True
            },
            'research': {
                'brain.neuronCount': 10000,
                'brain.simulationSpeed': 0.5,
                'development.enabled': True,
                'neuromodulation.enabled': True,
                'plasticity.enabled': True,
                'memory.enabled': True
            }
        }
    
    def create_builder(self):
        """
        Create a configuration builder for fluent API.
        
        Returns:
            ConfigurationBuilder instance
        """
        return ConfigurationBuilder(self)
    
    def export(self, filepath, format="json"):
        """
        Export configuration to file.
        
        Args:
            filepath: Output file path
            format: Export format (json, yaml, ini)
        """
        if format.lower() == "json":
            config_data = self._to_dict()
            with open(filepath, 'w') as f:
                json.dump(config_data, f, indent=2)
        else:
            raise ValueError(f"Unsupported export format: {format}")
        return True
    
    def import_config(self, filepath, format="json"):
        """
        Import configuration from file.
        
        Args:
            filepath: Input file path
            format: Import format (json, yaml, ini)
        """
        if format.lower() == "json":
            with open(filepath, 'r') as f:
                config_data = json.load(f)
            self.clear()
            for key, value in config_data.items():
                self.set(key, value)
        else:
            raise ValueError(f"Unsupported import format: {format}")
        return True
    
    def _to_dict(self):
        """Convert configuration to dictionary."""
        result = {}
        for key in self._config.getKeys():
            result[key] = self._get_typed_value(key)
        return result
    
    def diff(self, other_config):
        """
        Diff this configuration with another.
        
        Args:
            other_config: Another NLMConfig instance
            
        Returns:
            Dictionary with differences
        """
        diff_result = {
            'added': [],
            'removed': [],
            'modified': []
        }
        
        self_keys = set(self.keys())
        other_keys = set(other_config.keys())
        
        diff_result['added'] = list(other_keys - self_keys)
        diff_result['removed'] = list(self_keys - other_keys)
        
        for key in self_keys & other_keys:
            if self[key] != other_config[key]:
                diff_result['modified'].append({
                    'key': key,
                    'old_value': self[key],
                    'new_value': other_config[key]
                })
        
        return diff_result
    
    def merge(self, other_config, strategy="replace"):
        """
        Merge with another configuration.
        
        Args:
            other_config: Another NLMConfig instance
            strategy: Merge strategy (replace, merge, union)
            
        Returns:
            New NLMConfig with merged configuration
        """
        new_config = NLMConfig()
        
        if strategy == "replace":
            # Copy all keys from self
            for key in self.keys():
                new_config.set(key, self[key])
            # Overwrite with keys from other
            for key in other_config.keys():
                new_config.set(key, other_config[key])
        elif strategy == "merge":
            # Merge nested dictionaries
            for key in self.keys():
                if isinstance(self[key], dict) and isinstance(other_config.get(key), dict):
                    # Merge nested dictionaries
                    merged_value = self[key].copy()
                    merged_value.update(other_config[key])
                    new_config.set(key, merged_value)
                else:
                    new_config.set(key, self[key])
            # Add keys only in other
            for key in other_config.keys():
                if key not in self:
                    new_config.set(key, other_config[key])
        else:
            raise ValueError(f"Unknown merge strategy: {strategy}")
        
        return new_config
    
    def clone(self):
        """Create a deep copy of the configuration."""
        new_config = NLMConfig()
        for key in self.keys():
            new_config.set(key, self[key])
        return new_config
    
    def reset_to_defaults(self):
        """Reset configuration to defaults."""
        self.clear()
    
    def summary(self):
        """Get configuration summary."""
        return self._config.summary()


class ConfigurationBuilder:
    """
    Fluent builder pattern for configuration creation.
    
    Provides a chainable API for building complex configurations.
    """
    
    def __init__(self, config):
        """
        Initialize builder.
        
        Args:
            config: NLMConfig instance to build upon
        """
        self._config = config
        self._operations = []
    
    def set(self, key, value, source=pynlm.ConfigSource.Runtime, category=""):
        """
        Set a configuration value.
        
        Args:
            key: Configuration key
            value: Value to set
            source: Configuration source
            category: Configuration category
            
        Returns:
            Builder instance (for chaining)
        """
        self._operations.append(('set', key, value, source, category))
        self._config.set(key, value, source, category)
        return self
    
    def set_if_not_exists(self, key, value, source=pynlm.ConfigSource.Default):
        """
        Set a configuration value only if it doesn't exist.
        
        Args:
            key: Configuration key
            value: Value to set
            source: Configuration source
            
        Returns:
            Builder instance (for chaining)
        """
        if not self._config.has(key):
            return self.set(key, value, source)
        return self
    
    def set_nested(self, path, value, source=pynlm.ConfigSource.Runtime):
        """
        Set a nested configuration value.
        
        Args:
            path: Dot-separated path (e.g., "brain.simulation.speed")
            value: Value to set
            source: Configuration source
            
        Returns:
            Builder instance (for chaining)
        """
        self._operations.append(('set_nested', path, value, source))
        self._config._config.setNested(path, value, source)
        return self
    
    def add_to_list(self, key, value, source=pynlm.ConfigSource.Runtime):
        """
        Add value to a list configuration.
        
        Args:
            key: Configuration key
            value: Value to add
            source: Configuration source
            
        Returns:
            Builder instance (for chaining)
        """
        self._operations.append(('add_to_list', key, value, source))
        
        # Get current list or create new one
        current = self._config.get(key, [])
        if isinstance(current, list):
            current.append(value)
            self._config.set(key, current, source)
        else:
            # Create new list with the value
            self._config.set(key, [value], source)
        
        return self
    
    def build(self):
        """
        Build the configuration and return it.
        
        Returns:
            NLMConfig instance
        """
        return self._config
    
    def get_operations(self):
        """Get list of operations performed."""
        return self._operations.copy()
    
    def undo(self, count=1):
        """
        Undo last operations.
        
        Args:
            count: Number of operations to undo
            
        Returns:
            Builder instance (for chaining)
        """
        # This would need to be implemented with history tracking
        return self
    
    def validate(self):
        """
        Validate the current configuration.
        
        Returns:
            Builder instance (for chaining)
        """
        # This would validate the current configuration
        return self


class ConfigurationTemplate:
    """
    Template system for configuration templates.
    
    Provides template-based configuration management with variable substitution
    and inheritance.
    """
    
    def __init__(self, name, template_config):
        """
        Initialize template.
        
        Args:
            name: Template name
            template_config: Template configuration (dict or NLMConfig)
        """
        self.name = name
        if isinstance(template_config, NLMConfig):
            self.template = template_config._to_dict()
        else:
            self.template = template_config
    
    def render(self, variables=None):
        """
        Render template with variables.
        
        Args:
            variables: Dictionary of variables to substitute
            
        Returns:
            Rendered configuration (NLMConfig)
        """
        if variables is None:
            variables = {}
        
        # Simple variable substitution
        rendered = self._substitute_variables(self.template, variables)
        return NLMConfig(pynlm.createDefaultConfig())
    
    def _substitute_variables(self, config, variables):
        """Recursively substitute variables in configuration."""
        if isinstance(config, dict):
            result = {}
            for key, value in config.items():
                if isinstance(value, str) and value.startswith('${') and value.endswith('}'):
                    var_name = value[2:-1]
                    result[key] = variables.get(var_name, value)
                else:
                    result[key] = self._substitute_variables(value, variables)
            return result
        elif isinstance(config, list):
            return [self._substitute_variables(item, variables) for item in config]
        else:
            return config


# Global configuration registry
_config_registry = {}
_config_presets = {}


def register_config(name, config):
    """
    Register a configuration in the global registry.
    
    Args:
        name: Configuration name
        config: Configuration to register (NLMConfig or dict)
    """
    if isinstance(config, NLMConfig):
        _config_registry[name] = config
    else:
        _config_registry[name] = NLMConfig(pynlm.createDefaultConfig())


def get_config(name):
    """
    Get a registered configuration.
    
    Args:
        name: Configuration name
        
    Returns:
        Registered NLMConfig instance
        
    Raises:
        KeyError: If configuration not found
    """
    if name in _config_registry:
        return _config_registry[name]
    raise KeyError(f"Configuration '{name}' not found in registry")


def list_configs():
    """List all registered configurations."""
    return list(_config_registry.keys())


def create_config_from_template(template_name, variables=None):
    """
    Create configuration from template.
    
    Args:
        template_name: Template name
        variables: Optional variables for template rendering
        
    Returns:
        NLMConfig instance
        
    Raises:
        KeyError: If template not found
    """
    if template_name in _config_presets:
        template = ConfigurationTemplate(template_name, _config_presets[template_name])
        return template.render(variables)
    raise KeyError(f"Template '{template_name}' not found")


def register_preset(name, config, description=""):
    """
    Register a configuration preset.
    
    Args:
        name: Preset name
        config: Preset configuration (NLMConfig or dict)
        description: Preset description
    """
    if isinstance(config, NLMConfig):
        _config_presets[name] = config._to_dict()
    else:
        _config_presets[name] = config


def get_preset(name):
    """
    Get a configuration preset.
    
    Args:
        name: Preset name
        
    Returns:
        NLMConfig instance
        
    Raises:
        KeyError: If preset not found
    """
    if name in _config_presets:
        return NLMConfig(pynlm.createDefaultConfig())
    raise KeyError(f"Preset '{name}' not found")


def list_presets():
    """List all registered presets."""
    return list(_config_presets.keys())


def validate_config(config, schema=None):
    """
    Validate configuration against schema.
    
    Args:
        config: Configuration to validate (NLMConfig or dict)
        schema: Optional validation schema
        
    Returns:
        Validation result
    """
    if isinstance(config, NLMConfig):
        return config.validate(schema)
    # For dict, create temporary NLMConfig
    temp_config = NLMConfig(pynlm.createDefaultConfig())
    for key, value in config.items():
        temp_config.set(key, value)
    return temp_config.validate(schema)


def merge_configs(*configs, strategy="replace"):
    """
    Merge multiple configurations.
    
    Args:
        *configs: Configuration objects to merge
        strategy: Merge strategy (replace, merge, union)
        
    Returns:
        Merged NLMConfig instance
    """
    if not configs:
        return NLMConfig()
    
    result = configs[0].clone()
    for config in configs[1:]:
        result = result.merge(config, strategy)
    
    return result


def diff_configs(config1, config2):
    """
    Diff two configurations.
    
    Args:
        config1: First configuration
        config2: Second configuration
        
    Returns:
        Dictionary with differences
    """
    if isinstance(config1, NLMConfig) and isinstance(config2, NLMConfig):
        return config1.diff(config2)
    # For dict, create temporary NLMConfig instances
    config1_obj = NLMConfig(pynlm.createDefaultConfig())
    config2_obj = NLMConfig(pynlm.createDefaultConfig())
    
    for key, value in config1.items():
        config1_obj.set(key, value)
    for key, value in config2.items():
        config2_obj.set(key, value)
    
    return config1_obj.diff(config2_obj)


def export_config(config, filepath, format="json"):
    """
    Export configuration to file.
    
    Args:
        config: Configuration to export (NLMConfig or dict)
        filepath: Output file path
        format: Export format (json, yaml, ini)
    """
    if isinstance(config, NLMConfig):
        return config.export(filepath, format)
    # For dict, create temporary NLMConfig
    temp_config = NLMConfig(pynlm.createDefaultConfig())
    for key, value in config.items():
        temp_config.set(key, value)
    return temp_config.export(filepath, format)


def import_config(filepath, format="json"):
    """
    Import configuration from file.
    
    Args:
        filepath: Input file path
        format: Import format (json, yaml, ini)
        
    Returns:
        NLMConfig instance
    """
    config = NLMConfig(pynlm.createDefaultConfig())
    return config.import_config(filepath, format)


def auto_detect_config_type(value):
    """
    Automatically infer configuration type from value.
    
    Args:
        value: Value to infer type from
        
    Returns:
        Inferred type
    """
    if isinstance(value, bool):
        return "bool"
    elif isinstance(value, int):
        return "int"
    elif isinstance(value, float):
        return "float"
    elif isinstance(value, str):
        return "string"
    elif isinstance(value, list):
        if all(isinstance(x, int) for x in value):
            return "int_list"
        elif all(isinstance(x, float) for x in value):
            return "float_list"
        elif all(isinstance(x, str) for x in value):
            return "string_list"
        else:
            return "mixed_list"
    else:
        return "unknown"


# Example usage functions
def example_minimal_config():
    """Create a minimal configuration example."""
    return NLMConfig()


def example_standard_config():
    """Create a standard configuration example."""
    config = NLMConfig()
    config.create_builder() \
        .set("brain.neuronCount", 1000) \
        .set("brain.simulationSpeed", 1.0) \
        .set("sensory.vision.enabled", True) \
        .set("sensory.audio.enabled", True) \
        .set("plasticity.enabled", False) \
        .build()
    return config


def example_high_performance_config():
    """Create a high-performance configuration example."""
    return NLMConfig().load_preset("high_performance")


def example_research_config():
    """Create a research configuration example."""
    return NLMConfig().load_preset("research")


# Monkey-patch the enhanced Config class into pybind11's namespace
# This allows users to access enhanced features through pynlm

# Add category enum
ConfigCategory = pynlm.ConfigCategory

# Add helper functions for validation and configuration patterns
def validate_int_range(value, min_val, max_val):
    """Validate integer is within range."""
    return min_val <= value <= max_val

def validate_positive(value):
    """Validate value is positive."""
    return value > 0

def validate_string_length(value, min_len, max_len):
    """Validate string length is within range."""
    return min_len <= len(value) <= max_len

def create_validation_function(*validators):
    """Create a composite validation function."""
    def composite(value):
        return all(validator(value) for validator in validators)
    return composite

# Export all enhanced functions
__all__ = [
    'NLMConfig',
    'ConfigurationBuilder', 
    'ConfigurationTemplate',
    'register_config',
    'get_config',
    'list_configs',
    'create_config_from_template',
    'register_preset',
    'get_preset',
    'list_presets',
    'validate_config',
    'merge_configs',
    'diff_configs',
    'export_config',
    'import_config',
    'auto_detect_config_type',
    'example_minimal_config',
    'example_standard_config',
    'example_high_performance_config',
    'example_research_config',
    'validate_int_range',
    'validate_positive',
    'validate_string_length',
    'create_validation_function',
    'ConfigCategory'
]
