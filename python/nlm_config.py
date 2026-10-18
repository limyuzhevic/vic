# Advanced Configuration Management System for NLM
# Sophisticated configuration system for experienced users

import json
import yaml
import toml
import os
import sys
import time
import hashlib
import threading
from typing import Dict, List, Any, Optional, Union, Callable, Tuple
from dataclasses import dataclass, field
from enum import Enum
import copy
import argparse
import configparser
from pathlib import Path
from collections import defaultdict

class ConfigSource(Enum):
    """Source of configuration parameters."""
    
    DEFAULT = "default"
    CONFIG_FILE = "config_file"
    COMMAND_LINE = "command_line"
    PROGRAMMATIC = "programmtic"
    ENVIRONMENT = "environment"
    DYNAMIC = "dynamic"
class ConfigScope(Enum):
    """Scope of configuration parameters."""
    
    GLOBAL = "global"
    SESSION = "session"
    LOCAL = "local"
    TRANSIENT = "transient"
    PLUGIN = "plugin"
class ConfigValidationError(Exception):
    """Configuration validation error."""
    
    def __init__(self, message: str, parameter: str = "", value: Any = None, 
                 expected_type: str = "", suggestions: List[str] = None):
        super().__init__(message)
        self.message = message
        self.parameter = parameter
        self.value = value
        self.expected_type = expected_type
        self.suggestions = suggestions or []
        
    def to_dict(self) -> Dict[str, Any]:
        """Convert error to dictionary."""
        return {
            'message': self.message,
            'parameter': self.parameter,
            'value': self.value,
            'expected_type': self.expected_type,
            'suggestions': self.suggestions,
            'type': self.__class__.__name__
        }
class ConfigParameter:
    """Configuration parameter with validation and metadata."""
    
    def __init__(self, name: str, default_value: Any, 
                 param_type: type = None, validation_func: Callable = None,
                 description: str = "", required: bool = False,
                 source: ConfigSource = ConfigSource.DEFAULT,
                 scope: ConfigScope = ConfigScope.GLOBAL,
                 min_value: Any = None, max_value: Any = None,
                 allowed_values: List[Any] = None,
                 sensitivity: float = 1.0,  # Parameter sensitivity for optimization
                 deprecated: bool = False):
        self.name = name
        self.default_value = default_value
        self.param_type = param_type or type(default_value)
        self.validation_func = validation_func
        self.description = description
        self.required = required
        self.source = source
        self.scope = scope
        self.min_value = min_value
        self.max_value = max_value
        self.allowed_values = allowed_values
        self.sensitivity = sensitivity
        self.deprecated = deprecated
        self.validation_errors: List[ConfigValidationError] = []
        self.last_validated = time.time()
        self.validation_cache = {}
        
        # Validate parameter
        self._validate(default_value)
    
    def _validate(self, value: Any) -> bool:
        """Validate parameter value."""
        self.validation_errors = []
        
        # Type validation
        if not isinstance(value, self.param_type) and not isinstance(value, type(None)):
            try:
                # Try type conversion
                converted_value = self.param_type(value)
                # Re-validate converted value
                return self._validate_with_rules(converted_value)
            except (ValueError, TypeError):
                self.validation_errors.append(
                    ConfigValidationError(
                        f"Parameter '{self.name}' must be of type {self.param_type.__name__}, "
                        f"got {type(value).__name__}",
                        self.name, value, self.param_type.__name__
                    )
                )
                return False
        
        # Additional validation rules
        return self._validate_with_rules(value)
    
    def _validate_with_rules(self, value: Any) -> bool:
        """Validate value against additional rules."""
        # Range validation
        if self.min_value is not None and value < self.min_value:
            self.validation_errors.append(
                ConfigValidationError(
                    f"Parameter '{self.name}' must be >= {self.min_value}",
                    self.name, value, f">= {self.min_value}"
                )
            )
            return False
        
        if self.max_value is not None and value > self.max_value:
            self.validation_errors.append(
                ConfigValidationError(
                    f"Parameter '{self.name}' must be <= {self.max_value}",
                    self.name, value, f"<= {self.max_value}"
                )
            )
            return False
        
        # Allowed values validation
        if self.allowed_values is not None and value not in self.allowed_values:
            self.validation_errors.append(
                ConfigValidationError(
                    f"Parameter '{self.name}' must be one of {self.allowed_values}",
                    self.name, value, f"one of {self.allowed_values}",
                    [f"Use one of: {', '.join(map(str, self.allowed_values))}"]
                )
            )
            return False
        
        # Custom validation function
        if self.validation_func and not self.validation_func(value):
            self.validation_errors.append(
                ConfigValidationError(
                    f"Parameter '{self.name}' failed custom validation",
                    self.name, value, "custom validation"
                )
            )
            return False
        
        return len(self.validation_errors) == 0
    
    def get_value(self, config: 'Configuration', scope: ConfigScope = ConfigScope.GLOBAL) -> Any:
        """Get parameter value from configuration."""
        cache_key = f"{self.name}_{scope.value}"
        
        # Check cache
        if cache_key in config._validation_cache:
            return config._validation_cache[cache_key]
        
        # Get value from config
        value = config.get(self.name, self.default_value, scope)
        
        # Validate if not already validated recently
        if time.time() - self.last_validated > 60.0:  # 1 minute cache
            self._validate(value)
            self.last_validated = time.time()
        
        # Cache the value
        config._validation_cache[cache_key] = value
        
        return value
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert parameter to dictionary."""
        return {
            'name': self.name,
            'default_value': self.default_value,
            'param_type': self.param_type.__name__,
            'description': self.description,
            'required': self.required,
            'source': self.source.value,
            'scope': self.scope.value,
            'min_value': self.min_value,
            'max_value': self.max_value,
            'allowed_values': self.allowed_values,
            'sensitivity': self.sensitivity,
            'deprecated': self.deprecated,
            'validation_errors': [err.to_dict() for err in self.validation_errors]
        }
class Configuration:
    """
    Advanced configuration management system for NLM.
    
    This system provides sophisticated configuration management with support for:
    - Multiple configuration sources (file, command line, programmatic, environment)
    - Parameter validation and error recovery
    - Configuration scopes (global, session, local)
    - Configuration templates and presets
    - Configuration optimization and sensitivity analysis
    - Configuration backup and restore
    - Dynamic configuration updates
    """
    
    def __init__(self, config_file: Optional[str] = None, 
                 config_template: Optional[Dict[str, Any]] = None,
                 auto_save: bool = True):
        self.config_file = config_file
        self.config_template = config_template or {}
        self.auto_save = auto_save
        
        # Configuration storage
        self._parameters: Dict[str, ConfigParameter] = {}
        self._values: Dict[str, Dict[str, Any]] = defaultdict(dict)
        self._sources: Dict[str, ConfigSource] = {}
        
        # Validation and caching
        self._validation_cache: Dict[str, Any] = {}
        self._validation_errors: Dict[str, List[ConfigValidationError]] = defaultdict(list)
        
        # Configuration history
        self._history: List[Dict[str, Any]] = []
        self._current_version = 0
        
        # Load configuration
        if config_file and os.path.exists(config_file):
            self.load_from_file(config_file)
        
        # Apply template
        if config_template:
            self.apply_template(config_template)
    
    def add_parameter(self, name: str, default_value: Any, 
                     param_type: type = None, **kwargs) -> None:
        """
        Add a configuration parameter.
        
        Args:
            name: Parameter name
            default_value: Default value
            param_type: Parameter type
            **kwargs: Additional parameter options
        """
        param = ConfigParameter(name, default_value, param_type, **kwargs)
        self._parameters[name] = param
    
    def set(self, name: str, value: Any, source: ConfigSource = ConfigSource.PROGRAMMATIC,
            scope: ConfigScope = ConfigScope.GLOBAL) -> bool:
        """
        Set a configuration parameter.
        
        Args:
            name: Parameter name
            value: Parameter value
            source: Configuration source
            scope: Configuration scope
            
        Returns:
            True if parameter was set successfully, False otherwise
        """
        if name not in self._parameters:
            # Try to create parameter from template
            if name in self.config_template:
                template_value = self.config_template[name]
                param_type = type(template_value)
                self.add_parameter(name, template_value, param_type, source=source, scope=scope)
            else:
                raise ConfigValidationError(
                    f"Unknown parameter: {name}",
                    name, value, "known parameter name",
                    [f"Available parameters: {list(self._parameters.keys())}"]
                )
        
        param = self._parameters[name]
        
        # Validate value
        if not param._validate(value):
            self._validation_errors[name] = param.validation_errors
            return False
        
        # Update configuration
        self._values[name][scope.value] = value
        self._sources[name] = source
        
        # Update history
        self._add_to_history(name, value, source, scope)
        
        # Save configuration if auto-save is enabled
        if self.auto_save:
            self.save_to_file(self.config_file)
        
        # Clear validation errors for this parameter
        if name in self._validation_errors:
            del self._validation_errors[name]
        
        return True
    
    def get(self, name: str, default: Any = None, 
            scope: ConfigScope = ConfigScope.GLOBAL) -> Any:
        """
        Get a configuration parameter value.
        
        Args:
            name: Parameter name
            default: Default value if parameter doesn't exist
            scope: Configuration scope
            
        Returns:
            Parameter value
        """
        if name in self._values:
            # Return value from the specified scope, or global if not available
            return self._values[name].get(scope.value, self._values[name].get('global', default))
        
        # Return default value
        if default is not None:
            return default
        
        # Check if parameter exists in template
        if name in self.config_template:
            return self.config_template[name]
        
        # Parameter doesn't exist
        return None
    
    def has(self, name: str) -> bool:
        """
        Check if parameter exists.
        
        Args:
            name: Parameter name
            
        Returns:
            True if parameter exists, False otherwise
        """
        return name in self._parameters or name in self.config_template
    
    def remove(self, name: str, scope: ConfigScope = ConfigScope.GLOBAL) -> bool:
        """
        Remove a configuration parameter.
        
        Args:
            name: Parameter name
            scope: Configuration scope
            
        Returns:
            True if parameter was removed, False otherwise
        """
        if name in self._values and scope.value in self._values[name]:
            del self._values[name][scope.value]
            return True
        
        return False
    
    def clear_scope(self, scope: ConfigScope) -> None:
        """
        Clear all parameters in a scope.
        
        Args:
            scope: Configuration scope to clear
        """
        scope_key = scope.value
        for name in list(self._values.keys()):
            if scope_key in self._values[name]:
                del self._values[name][scope_key]
    
    def validate_all(self) -> Dict[str, List[ConfigValidationError]]:
        """
        Validate all configuration parameters.
        
        Returns:
            Dictionary of validation errors by parameter name
        """
        errors = defaultdict(list)
        
        for name, param in self._parameters.items():
            # Get current value
            value = self.get(name)
            
            # Validate
            if value is not None:
                param._validate(value)
                errors[name].extend(param.validation_errors)
        
        self._validation_errors = dict(errors)
        
        return dict(errors)
    
    def get_validation_errors(self, name: str) -> List[ConfigValidationError]:
        """
        Get validation errors for a parameter.
        
        Args:
            name: Parameter name
            
        Returns:
            List of validation errors
        """
        return self._validation_errors.get(name, [])
    
    def get_all_validation_errors(self) -> Dict[str, List[ConfigValidationError]]:
        """
        Get all validation errors.
        
        Returns:
            Dictionary of validation errors by parameter name
        """
        return dict(self._validation_errors)
    
    def load_from_file(self, filepath: str) -> bool:
        """
        Load configuration from file.
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            True if configuration was loaded successfully, False otherwise
        """
        try:
            # Determine file format based on extension
            if filepath.endswith('.json'):
                return self._load_from_json(filepath)
            elif filepath.endswith('.yaml') or filepath.endswith('.yml'):
                return self._load_from_yaml(filepath)
            elif filepath.endswith('.toml'):
                return self._load_from_toml(filepath)
            elif filepath.endswith('.cfg') or filepath.endswith('.ini'):
                return self._load_from_config(filepath)
            else:
                raise ConfigValidationError(
                    f"Unsupported configuration file format: {filepath}",
                    "filepath", filepath, "supported formats: .json, .yaml, .yml, .toml, .cfg, .ini"
                )
        
        except Exception as e:
            print(f"Warning: Could not load configuration from {filepath}: {e}")
            return False
    
    def _load_from_json(self, filepath: str) -> bool:
        """Load configuration from JSON file."""
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        # Apply configuration
        self.apply_config_dict(data, ConfigSource.CONFIG_FILE)
        
        return True
    
    def _load_from_yaml(self, filepath: str) -> bool:
        """Load configuration from YAML file."""
        try:
            with open(filepath, 'r') as f:
                data = yaml.safe_load(f)
            
            # Apply configuration
            self.apply_config_dict(data, ConfigSource.CONFIG_FILE)
            
            return True
        except ImportError:
            raise ConfigValidationError(
                "YAML support requires PyYAML package",
                "filepath", filepath, "pip install PyYAML"
            )
    
    def _load_from_toml(self, filepath: str) -> bool:
        """Load configuration from TOML file."""
        try:
            with open(filepath, 'r') as f:
                data = toml.load(f)
            
            # Apply configuration
            self.apply_config_dict(data, ConfigSource.CONFIG_FILE)
            
            return True
        except ImportError:
            raise ConfigValidationError(
                "TOML support requires toml package",
                "filepath", filepath, "pip install toml"
            )
    
    def _load_from_config(self, filepath: str) -> bool:
        """Load configuration from INI file."""
        try:
            config = configparser.ConfigParser()
            config.read(filepath)
            
            # Convert to dictionary
            data = {}
            for section in config.sections():
                data[section] = dict(config[section])
            
            # Apply configuration
            self.apply_config_dict(data, ConfigSource.CONFIG_FILE)
            
            return True
        except Exception as e:
            raise ConfigValidationError(
                f"Error loading INI configuration file: {e}",
                "filepath", filepath, "check file format and syntax"
            )
    
    def save_to_file(self, filepath: str) -> bool:
        """
        Save configuration to file.
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            True if configuration was saved successfully, False otherwise
        """
        try:
            # Determine file format based on extension
            if filepath.endswith('.json'):
                return self._save_to_json(filepath)
            elif filepath.endswith('.yaml') or filepath.endswith('.yml'):
                return self._save_to_yaml(filepath)
            elif filepath.endswith('.toml'):
                return self._save_to_toml(filepath)
            elif filepath.endswith('.cfg') or filepath.endswith('.ini'):
                return self._save_to_config(filepath)
            else:
                raise ConfigValidationError(
                    f"Unsupported configuration file format: {filepath}",
                    "filepath", filepath, "supported formats: .json, .yaml, .yml, .toml, .cfg, .ini"
                )
        
        except Exception as e:
            print(f"Warning: Could not save configuration to {filepath}: {e}")
            return False
    
    def _save_to_json(self, filepath: str) -> bool:
        """Save configuration to JSON file."""
        data = self.to_dict()
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)
        
        return True
    
    def _save_to_yaml(self, filepath: str) -> bool:
        """Save configuration to YAML file."""
        try:
            import yaml
            
            data = self.to_dict()
            
            with open(filepath, 'w') as f:
                yaml.dump(data, f, default_flow_style=False)
            
            return True
        except ImportError:
            raise ConfigValidationError(
                "YAML support requires PyYAML package",
                "filepath", filepath, "pip install PyYAML"
            )
    
    def _save_to_toml(self, filepath: str) -> bool:
        """Save configuration to TOML file."""
        try:
            import toml
            
            data = self.to_dict()
            
            with open(filepath, 'w') as f:
                toml.dump(data, f)
            
            return True
        except ImportError:
            raise ConfigValidationError(
                "TOML support requires toml package",
                "filepath", filepath, "pip install toml"
            )
    
    def _save_to_config(self, filepath: str) -> bool:
        """Save configuration to INI file."""
        try:
            import configparser
            
            config = configparser.ConfigParser()
            
            # Add sections for each scope
            for scope in ConfigScope:
                section_name = f"{scope.value.upper()}_SECTION"
                config.add_section(section_name)
                
                # Add parameters in this scope
                for name, value in self._values.items():
                    if scope.value in value:
                        config.set(section_name, name, str(value[scope.value]))
            
            config.write(filepath)
            
            return True
        except Exception as e:
            raise ConfigValidationError(
                f"Error saving configuration to INI file: {e}",
                "filepath", filepath, "check file permissions and disk space"
            )
    
    def apply_config_dict(self, data: Dict[str, Any], source: ConfigSource) -> None:
        """
        Apply configuration dictionary.
        
        Args:
            data: Configuration dictionary
            source: Configuration source
        """
        if isinstance(data, dict):
            for name, value in data.items():
                if name in self._parameters:
                    self.set(name, value, source)
                else:
                    # Create new parameter from template
                    if name in self.config_template:
                        template_value = self.config_template[name]
                        param_type = type(template_value)
                        self.add_parameter(name, template_value, param_type, source=source, scope=ConfigScope.GLOBAL)
                        self.set(name, value, source)
    
    def apply_template(self, template: Dict[str, Any]) -> None:
        """
        Apply configuration template.
        
        Args:
            template: Configuration template
        """
        self.config_template = template
        
        # Apply template values
        for name, value in template.items():
            if name not in self._parameters:
                param_type = type(value)
                self.add_parameter(name, value, param_type, source=ConfigSource.DEFAULT, scope=ConfigScope.GLOBAL)
    
    def get_template(self) -> Dict[str, Any]:
        """
        Get configuration template.
        
        Returns:
            Configuration template
        """
        return copy.deepcopy(self.config_template)
    
    def set_template(self, template: Dict[str, Any]) -> None:
        """
        Set configuration template.
        
        Args:
            template: Configuration template
        """
        self.config_template = template
    
    def get_effective_config(self, scope: ConfigScope = ConfigScope.GLOBAL) -> Dict[str, Any]:
        """
        Get effective configuration for a scope.
        
        Args:
            scope: Configuration scope
            
        Returns:
            Effective configuration
        """
        effective_config = {}
        
        # Add values from specified scope
        for name, values in self._values.items():
            if scope.value in values:
                effective_config[name] = values[scope.value]
        
        # Add values from parent scopes
        parent_scopes = [ConfigScope.GLOBAL, ConfigScope.SESSION, ConfigScope.LOCAL]
        current_scope_index = parent_scopes.index(scope)
        
        for parent_scope in parent_scopes[current_scope_index + 1:]:
            for name, values in self._values.items():
                if parent_scope.value in values and name not in effective_config:
                    effective_config[name] = values[parent_scope.value]
        
        # Add values from template
        for name, value in self.config_template.items():
            if name not in effective_config:
                effective_config[name] = value
        
        return effective_config
    
    def merge_config(self, other_config: 'Configuration', 
                    merge_strategy: str = "override") -> None:
        """
        Merge another configuration into this one.
        
        Args:
            other_config: Configuration to merge
            merge_strategy: Merge strategy ("override", "merge", "append")
        """
        if merge_strategy == "override":
            # Override current values with other configuration
            for name, values in other_config._values.items():
                for scope, value in values.items():
                    self._values[name][scope] = value
                    self._sources[name] = other_config._sources.get(name, ConfigSource.CONFIG_FILE)
        
        elif merge_strategy == "merge":
            # Merge values, preferring current values for overlapping keys
            for name, values in other_config._values.items():
                if name not in self._values:
                    self._values[name] = {}
                
                for scope, value in values.items():
                    if scope not in self._values[name]:
                        self._values[name][scope] = value
        
        elif merge_strategy == "append":
            # Append values, preserving current values
            for name, values in other_config._values.items():
                if name not in self._values:
                    self._values[name] = {}
                
                for scope, value in values.items():
                    # Only append if scope doesn't already exist
                    if scope not in self._values[name]:
                        self._values[name][scope] = value
    
    def copy(self) -> 'Configuration':
        """
        Create a copy of the configuration.
        
        Returns:
            Copy of the configuration
        """
        new_config = Configuration(self.config_file, self.config_template, self.auto_save)
        
        # Copy parameters
        new_config._parameters = copy.deepcopy(self._parameters)
        
        # Copy values
        new_config._values = copy.deepcopy(self._values)
        
        # Copy sources
        new_config._sources = copy.deepcopy(self._sources)
        
        # Copy validation cache
        new_config._validation_cache = copy.deepcopy(self._validation_cache)
        
        # Copy validation errors
        new_config._validation_errors = copy.deepcopy(self._validation_errors)
        
        # Copy history
        new_config._history = copy.deepcopy(self._history)
        new_config._current_version = self._current_version
        
        return new_config
    
    def to_dict(self) -> Dict[str, Any]:
        """
        Convert configuration to dictionary.
        
        Returns:
            Configuration dictionary
        """
        config_dict = {
            'parameters': {},
            'values': {},
            'sources': dict(self._sources),
            'history': []
        }
        
        # Add parameters
        for name, param in self._parameters.items():
            config_dict['parameters'][name] = param.to_dict()
        
        # Add values
        for name, values in self._values.items():
            config_dict['values'][name] = dict(values)
        
        # Add history (last 100 entries)
        config_dict['history'] = self._history[-100:]
        
        return config_dict
    
    def from_dict(self, data: Dict[str, Any]) -> None:
        """
        Load configuration from dictionary.
        
        Args:
            data: Configuration dictionary
        """
        # Clear current configuration
        self._parameters.clear()
        self._values.clear()
        self._sources.clear()
        self._validation_cache.clear()
        self._validation_errors.clear()
        self._history.clear()
        
        # Restore parameters
        if 'parameters' in data:
            for name, param_data in data['parameters'].items():
                # Recreate parameter (simplified)
                self._parameters[name] = ConfigParameter(
                    param_data['name'],
                    param_data['default_value'],
                    param_data['param_type']
                )
        
        # Restore values
        if 'values' in data:
            self._values = {name: dict(values) for name, values in data['values'].items()}
        
        # Restore sources
        if 'sources' in data:
            self._sources = dict(data['sources'])
        
        # Restore history
        if 'history' in data:
            self._history = data['history']
            self._current_version = len(self._history)
    
    def _add_to_history(self, name: str, value: Any, source: ConfigSource, scope: ConfigScope) -> None:
        """
        Add configuration change to history.
        
        Args:
            name: Parameter name
            value: Parameter value
            source: Configuration source
            scope: Configuration scope
        """
        history_entry = {
            'timestamp': time.time(),
            'name': name,
            'value': value,
            'source': source.value,
            'scope': scope.value,
            'version': self._current_version + 1
        }
        
        self._history.append(history_entry)
        self._current_version += 1
    
    def get_history(self, name: Optional[str] = None, 
                   from_time: Optional[float] = None,
                   to_time: Optional[float] = None) -> List[Dict[str, Any]]:
        """
        Get configuration history.
        
        Args:
            name: Parameter name (optional)
            from_time: Start time (optional)
            to_time: End time (optional)
            
        Returns:
            List of history entries
        """
        history = self._history
        
        # Filter by parameter name
        if name:
            history = [entry for entry in history if entry['name'] == name]
        
        # Filter by time
        if from_time:
            history = [entry for entry in history if entry['timestamp'] >= from_time]
        
        if to_time:
            history = [entry for entry in history if entry['timestamp'] <= to_time]
        
        return history
    
    def get_configuration_diff(self, other_config: 'Configuration') -> Dict[str, Any]:
        """
        Get differences between this configuration and another.
        
        Args:
            other_config: Other configuration to compare
            
        Returns:
            Dictionary of differences
        """
        diff = {
            'added': [],
            'removed': [],
            'modified': [],
            'identical': True
        }
        
        # Check added parameters
        for name in other_config._parameters:
            if name not in self._parameters:
                diff['added'].append(name)
        
        # Check removed parameters
        for name in self._parameters:
            if name not in other_config._parameters:
                diff['removed'].append(name)
        
        # Check modified parameters
        for name in self._parameters:
            if name in other_config._parameters:
                # Compare values
                if (name in self._values and name in other_config._values and
                    self._values[name] != other_config._values[name]):
                    diff['modified'].append({
                        'name': name,
                        'old_value': self._values[name] if name in self._values else None,
                        'new_value': other_config._values[name] if name in other_config._values else None
                    })
        
        # Check if configurations are identical
        diff['identical'] = (
            self._parameters == other_config._parameters and
            self._values == other_config._values and
            self._sources == other_config._sources
        )
        
        return diff
    
    def backup(self, backup_path: Optional[str] = None) -> str:
        """
        Create a backup of the configuration.
        
        Args:
            backup_path: Path to backup file (optional)
            
        Returns:
            Path to backup file
        """
        if backup_path is None:
            timestamp = int(time.time())
            backup_path = f"nlm_config_backup_{timestamp}.json"
        
        # Save backup
        self.save_to_file(backup_path)
        
        return backup_path
    
    def restore(self, backup_path: str) -> bool:
        """
        Restore configuration from backup.
        
        Args:
            backup_path: Path to backup file
            
        Returns:
            True if configuration was restored successfully, False otherwise
        """
        return self.load_from_file(backup_path)
    
    def optimize(self, objective_function: Callable[[Dict[str, Any]], float],
                 constraints: Optional[List[Callable]] = None,
                 optimization_method: str = "grid_search") -> Dict[str, Any]:
        """
        Optimize configuration using an objective function.
        
        Args:
            objective_function: Function to optimize
            constraints: List of constraint functions
            optimization_method: Optimization method ("grid_search", "random_search")
            
        Returns:
            Optimization results
        """
        results = {
            'method': optimization_method,
            'best_score': float('inf'),
            'best_config': None,
            'search_space': [],
            'constraints_violated': 0,
            'total_evaluations': 0
        }
        
        if optimization_method == "grid_search":
            # Grid search optimization
            for param_name, param in self._parameters.items():
                if hasattr(param, 'allowed_values') and param.allowed_values:
                    results['search_space'].append((param_name, param.allowed_values))
            
            # Generate all combinations (simplified)
            # In practice, you'd use a more sophisticated optimization library
            param_combinations = []
            # This is a simplified version - you'd typically use itertools.product
            # or a dedicated optimization library
            
            for combination in param_combinations:
                # Create temporary configuration
                temp_config = self.copy()
                
                # Set values from combination
                for (param_name, value), (param_idx, _) in zip(combination, enumerate(results['search_space'])):
                    temp_config.set(param_name, value)
                
                # Check constraints
                constraint_violated = False
                if constraints:
                    for constraint in constraints:
                        if not constraint(temp_config.to_dict()):
                            constraint_violated = True
                            break
                
                if constraint_violated:
                    results['constraints_violated'] += 1
                    continue
                
                # Evaluate objective function
                config_dict = temp_config.to_dict()
                score = objective_function(config_dict)
                results['total_evaluations'] += 1
                
                # Update best result
                if score < results['best_score']:
                    results['best_score'] = score
                    results['best_config'] = config_dict
        
        return results
    
    def print_help(self) -> None:
        """Print configuration help information."""
        print("NLM Configuration Management")
        print("============================")
        print(f"\nConfiguration file: {self.config_file}")
        print(f"Auto-save: {self.auto_save}")
        print(f"Parameters: {len(self._parameters)}")
        print(f"Scopes: {[scope.value for scope in ConfigScope]}")
        print(f"Sources: {[source.value for source in ConfigSource]}")
        
        print("\nAvailable parameters:")
        for name, param in self._parameters.items():
            print(f"  {name}:")
            print(f"    Default: {param.default_value}")
            print(f"    Type: {param.param_type.__name__}")
            print(f"    Description: {param.description}")
            print(f"    Required: {param.required}")
            print(f"    Source: {param.source.value}")
            print(f"    Scope: {param.scope.value}")
            
            if param.min_value is not None or param.max_value is not None:
                range_str = ""
                if param.min_value is not None:
                    range_str += f" >= {param.min_value}"
                if param.max_value is not None:
                    range_str += f" <= {param.max_value}"
                print(f"    Range: {range_str}")
            
            if param.allowed_values:
                print(f"    Allowed values: {param.allowed_values}")
            
            if param.deprecated:
                print(f"    Status: DEPRECATED")
            
            if param.validation_errors:
                print(f"    Validation errors: {len(param.validation_errors)}")
        
        print("\nCurrent configuration values:")
        for name, values in self._values.items():
            for scope, value in values.items():
                print(f"  {name} ({scope}): {value}")
        
        # Print validation errors
        errors = self.get_all_validation_errors()
        if errors:
            print("\nValidation errors:")
            for name, param_errors in errors.items():
                print(f"  {name}:")
                for error in param_errors:
                    print(f"    - {error.message}")
# Global configuration instance
_global_configuration = None

def get_configuration(config_file: Optional[str] = None) -> Configuration:
    """
    Get or create the global configuration instance.
    
    Args:
        config_file: Configuration file path (optional)
        
    Returns:
        Global configuration instance
    """
    global _global_configuration
    if _global_configuration is None:
        _global_configuration = Configuration(config_file)
    elif config_file and _global_configuration.config_file != config_file:
        # Create new configuration if file differs
        _global_configuration = Configuration(config_file)
    
    return _global_configuration

def set_configuration(config: Configuration) -> None:
    """
    Set the global configuration instance.
    
    Args:
        config: Configuration instance
    """
    global _global_configuration
    _global_configuration = config

# Convenience functions

def create_default_config() -> Configuration:
    """Create a default configuration for NLM."""
    config = Configuration()
    
    # Add common NLM parameters
    config.add_parameter("neuron_count", 1000, int,
                         description="Number of neurons",
                         min_value=10, max_value=1000000,
                         allowed_values=None,
                         source=ConfigSource.DEFAULT,
                         scope=ConfigScope.GLOBAL)
    
    config.add_parameter("region_count", 2, int,
                         description="Number of brain regions",
                         min_value=1, max_value=100,
                         source=ConfigSource.DEFAULT,
                         scope=ConfigScope.GLOBAL)
    
    config.add_parameter("connection_probability", 0.1, float,
                         description="Probability of synaptic connections",
                         min_value=0.0, max_value=1.0,
                         source=ConfigSource.DEFAULT,
                         scope=ConfigScope.GLOBAL)
    
    config.add_parameter("simulation_timestep", 0.001, float,
                         description="Simulation time step in seconds",
                         min_value=0.0001, max_value=0.1,
                         source=ConfigSource.DEFAULT,
                         scope=ConfigScope.GLOBAL)
    
    config.add_parameter("plasticity_learning_rate", 0.01, float,
                         description="Plasticity learning rate",
                         min_value=0.0, max_value=1.0,
                         source=ConfigSource.DEFAULT,
                         scope=ConfigScope.GLOBAL)
    
    config.add_parameter("random_seed", 42, int,
                         description="Random seed for reproducible results",
                         source=ConfigSource.DEFAULT,
                         scope=ConfigScope.GLOBAL)
    
    return config

# Command-line interface for configuration management
class ConfigCLI:
    """
    Command-line interface for configuration management.
    
    Provides command-line tools for configuring NLM through command line arguments.
    """
    
    def __init__(self, config: Configuration):
        self.config = config
    
    def parse_args(self, args: List[str]) -> Dict[str, Any]:
        """
        Parse command-line arguments.
        
        Args:
            args: Command-line arguments
            
        Returns:
            Parsed arguments
        """
        parser = argparse.ArgumentParser(
            description="NLM Configuration Management",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Examples:
  nlm_config.py --set neuron_count 2000
  nlm_config.py --load config.json
  nlm_config.py --save config.json
  nlm_config.py --validate
  nlm_config.py --show-help
            """
        )
        
        # Global options
        parser.add_argument('--config-file', '-c', 
                           help='Configuration file path')
        parser.add_argument('--set', '-s', nargs=2, action='append',
                           help='Set configuration parameter (name value)')
        parser.add_argument('--get', '-g', 
                           help='Get configuration parameter value')
        parser.add_argument('--load', '-l', 
                           help='Load configuration from file')
        parser.add_argument('--save', '-S', 
                           help='Save configuration to file')
        parser.add_argument('--validate', action='store_true',
                           help='Validate configuration')
        parser.add_argument('--show-help', '-h', action='store_true',
                           help='Show help information')
        parser.add_argument('--show-errors', action='store_true',
                           help='Show validation errors')
        parser.add_argument('--list-params', action='store_true',
                           help='List all configuration parameters')
        parser.add_argument('--export', '-e', 
                           help='Export configuration to file (json, yaml, toml)')
        parser.add_argument('--import', '-i', dest='import_config',
                           help='Import configuration from file (json, yaml, toml)')
        parser.add_argument('--backup', action='store_true',
                           help='Create backup of current configuration')
        
        # Parse arguments
        parsed_args = parser.parse_args(args)
        
        # Handle additional arguments
        config_updates = {}
        if parsed_args.set:
            for name, value in parsed_args.set:
                # Convert value to appropriate type based on parameter type
                param_name = name
                if param_name in self.config._parameters:
                    param_type = self.config._parameters[param_name].param_type
                    try:
                        if param_type == bool:
                            value = value.lower() in ('true', '1', 'yes', 'on')
                        elif param_type == int:
                            value = int(value)
                        elif param_type == float:
                            value = float(value)
                        elif param_type == str:
                            value = value
                        else:
                            value = param_type(value)
                    except (ValueError, TypeError):
                        print(f"Warning: Could not convert value '{value}' to {param_type.__name__}")
                
                config_updates[name] = value
        
        # Apply updates
        for name, value in config_updates.items():
            self.config.set(name, value, ConfigSource.COMMAND_LINE)
        
        return parsed_args
    
    def execute(self, args: List[str]) -> bool:
        """
        Execute command-line interface.
        
        Args:
            args: Command-line arguments
            
        Returns:
            True if successful, False otherwise
        """
        try:
            parsed_args = self.parse_args(args)
            
            # Handle commands
            if parsed_args.show_help:
                self.config.print_help()
                return True
            
            if parsed_args.load:
                return self.config.load_from_file(parsed_args.load)
            
            if parsed_args.save:
                return self.config.save_to_file(parsed_args.save)
            
            if parsed_args.validate:
                errors = self.config.validate_all()
                if errors:
                    print("Configuration validation errors:")
                    for name, param_errors in errors.items():
                        print(f"  {name}:")
                        for error in param_errors:
                            print(f"    - {error.message}")
                    return False
                else:
                    print("Configuration validation passed.")
                    return True
            
            if parsed_args.show_errors:
                errors = self.config.get_all_validation_errors()
                if errors:
                    print("Validation errors:")
                    for name, param_errors in errors.items():
                        print(f"  {name}:")
                        for error in param_errors:
                            print(f"    - {error.message}")
                    return False
                else:
                    print("No validation errors.")
                    return True
            
            if parsed_args.list_params:
                self.config.print_help()
                return True
            
            if parsed_args.export:
                return self.config.save_to_file(parsed_args.export)
            
            if parsed_args.import_config:
                return self.config.load_from_file(parsed_args.import_config)
            
            if parsed_args.backup:
                backup_path = self.config.backup()
                print(f"Backup created: {backup_path}")
                return True
            
            # No command specified, show help
            if not any([parsed_args.load, parsed_args.save, parsed_args.validate, 
                       parsed_args.show_help, parsed_args.show_errors, 
                       parsed_args.list_params, parsed_args.export, 
                       parsed_args.import_config, parsed_args.backup]):
                print("No command specified. Use --show-help for help.")
                return False
            
            return True
            
        except Exception as e:
            print(f"Error: {e}")
            return False
# Convenience function for command-line usage
def main_cli(args: Optional[List[str]] = None) -> bool:
    """
    Command-line interface main function.
    
    Args:
        args: Command-line arguments (optional, defaults to sys.argv[1:])
        
    Returns:
        True if successful, False otherwise
    """
    if args is None:
        args = sys.argv[1:]
    
    # Get configuration
    config = get_configuration()
    
    # Create CLI
    cli = ConfigCLI(config)
    
    # Execute CLI
    return cli.execute(args)
# Export key classes and functions
__all__ = [
    'ConfigSource',
    'ConfigScope',
    'ConfigValidationError',
    'ConfigParameter',
    'Configuration',
    'get_configuration',
    'set_configuration',
    'create_default_config',
    'main_cli',
    'ConfigCLI'
]
