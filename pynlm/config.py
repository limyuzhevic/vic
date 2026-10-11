"""
Enhanced configuration schema system for NLM Python bindings

This module provides a robust configuration validation and schema system
with support for JSON/YAML schema definitions, environment variables, 
and type conversion.
"""

import json
import os
import re
from typing import Any, Dict, List, Optional, Union, Callable, Type
from enum import Enum
try:
    import yaml
    HAS_YAML = True
except ImportError:
    HAS_YAML = False

class ConfigSchema:
    """Configuration schema definition."""
    
    def __init__(self, name: str, data_type: Type, required: bool = False, 
                 default: Any = None, validator: Optional[Callable[[Any], bool]] = None, 
                 description: str = ""):
        """Initialize schema entry.
        
        Args:
            name: Name of the configuration key
            data_type: Expected Python type (int, float, str, bool, list, dict)
            required: Whether this key is required
            default: Default value if not provided
            validator: Optional custom validation function
            description: Human-readable description
        """
        self.name = name
        self.data_type = data_type
        self.required = required
        self.default = default
        self.validator = validator
        self.description = description
        
    def validate(self, value: Any) -> bool:
        """Validate value against schema.
        
        Args:
            value: Value to validate
            
        Returns:
            True if valid, False otherwise
        """
        if value is None:
            return not self.required
            
        if not isinstance(value, self.data_type):
            # Try type conversion
            try:
                if self.data_type == int and isinstance(value, float):
                    value = int(value)
                elif self.data_type == float and isinstance(value, int):
                    value = float(value)
                elif self.data_type == str:
                    value = str(value)
                elif self.data_type == bool and isinstance(value, str):
                    value_lower = value.lower()
                    value = value_lower in ('true', '1', 'yes', 'on')
                elif self.data_type == list and isinstance(value, str):
                    # Try to parse JSON list string
                    if value.startswith('[') and value.endswith(']'):
                        value = json.loads(value)
                elif self.data_type == dict and isinstance(value, str):
                    # Try to parse JSON dict string
                    if value.startswith('{') and value.endswith('}'):
                        value = json.loads(value)
                else:
                    return False
            except (ValueError, json.JSONDecodeError):
                return False
        
        if self.validator:
            return self.validator(value)
        
        return True

class ConfigValidator:
    """Validates configurations against schemas."""
    
    def __init__(self, schemas: List[ConfigSchema]):
        """Initialize with schemas.
        
        Args:
            schemas: List of ConfigSchema objects
        """
        self.schemas = {schema.name: schema for schema in schemas}
    
    def validate_key(self, key: str, value: Any) -> tuple[bool, Optional[str]]:
        """Validate configuration key.
        
        Args:
            key: Configuration key name
            value: Configuration value
            
        Returns:
            Tuple of (is_valid, error_message)
        """
        if key not in self.schemas:
            return False, f"Unknown configuration key: {key}"
            
        schema = self.schemas[key]
        if not schema.validate(value):
            return False, f"Invalid value for '{key}': expected {schema.data_type.__name__}, got {type(value).__name__}"
        
        return True, None
    
    def validate_all(self, config: Dict[str, Any]) -> tuple[bool, List[str]]:
        """Validate all configuration entries.
        
        Args:
            config: Configuration dictionary
            
        Returns:
            Tuple of (is_valid, list_of_error_messages)
        """
        errors = []
        
        for key, value in config.items():
            is_valid, error_msg = self.validate_key(key, value)
            if not is_valid:
                errors.append(error_msg)
        
        # Check for required fields
        for schema in self.schemas.values():
            if schema.required and schema.name not in config:
                errors.append(f"Required configuration key '{schema.name}' is missing")
        
        return len(errors) == 0, errors
    
    def get_default_config(self) -> Dict[str, Any]:
        """Get configuration with all defaults applied.
        
        Returns:
            Configuration dictionary with defaults
        """
        result = {}
        for schema in self.schemas.values():
            if schema.default is not None:
                result[schema.name] = schema.default
        return result

class Config:
    """Enhanced configuration system with schema validation."""
    
    def __init__(self, schema_file: Optional[str] = None, schemas: Optional[List[ConfigSchema]] = None):
        """Initialize with optional schema file.
        
        Args:
            schema_file: Path to JSON schema file
            schemas: List of ConfigSchema objects
        """
        self._config: Dict[str, Any] = {}
        self._schemas: List[ConfigSchema] = schemas or []
        self._validator = ConfigValidator(self._schemas)
        
        if schema_file:
            self.load_schema_from_file(schema_file)
    
    def load_schema_from_file(self, filepath: str, format: Optional[str] = None):
        """Load configuration schema from file (JSON or YAML).
        
        Args:
            filepath: Path to schema file
            format: File format ('json', 'yaml', or auto-detected)
        """
        if format is None:
            if filepath.endswith('.json'):
                format = 'json'
            elif filepath.endswith(('.yaml', '.yml')):
                format = 'yaml'
            else:
                raise ValueError(f"Unsupported file format: {filepath}")
        
        with open(filepath, 'r') as f:
            if format == 'json':
                schema_data = json.load(f)
            elif format == 'yaml':
                if not HAS_YAML:
                    raise ImportError("PyYAML is required to load YAML files. Install with: pip install PyYAML")
                schema_data = yaml.safe_load(f)
            else:
                raise ValueError(f"Unsupported format: {format}")
        
        self._schemas = []
        for name, spec in schema_data.get('schema', {}).items():
            schema = ConfigSchema(
                name=name,
                data_type=self._get_python_type(spec.get('type', 'string')),
                required=spec.get('required', False),
                default=spec.get('default'),
                description=spec.get('description', ''),
                validator=self._create_validator(spec)
            )
            self._schemas.append(schema)
        
        self._validator = ConfigValidator(self._schemas)
    
    def _get_python_type(self, type_str: str) -> Type:
        """Convert JSON/YAML type string to Python type.
        
        Args:
            type_str: Type string (string, number, integer, boolean, array, object)
            
        Returns:
            Python type class
        """
        type_map = {
            'string': str,
            'number': (int, float),
            'integer': int,
            'boolean': bool,
            'array': list,
            'object': dict,
        }
        return type_map.get(type_str, str)
    
    def _create_validator(self, spec: Dict) -> Optional[Callable[[Any], bool]]:
        """Create validator function from schema specification.
        
        Args:
            spec: Schema specification
            
        Returns:
            Validator function or None
        """
        if 'validation' in spec:
            validation = spec['validation']
            if 'min' in validation and 'max' in validation:
                min_val = validation['min']
                max_val = validation['max']
                if isinstance(min_val, int) and isinstance(max_val, int):
                    return lambda x: min_val <= x <= max_val
            
            if 'pattern' in validation:
                pattern = validation['pattern']
                return lambda x: bool(re.match(pattern, x))
        
        return None
    
    def load_from_file(self, filepath: str, format: Optional[str] = None):
        """Load configuration from file (JSON, YAML, or key=value).
        
        Args:
            filepath: Path to configuration file
            format: File format ('json', 'yaml', or 'env' for environment variables)
        """
        if format is None:
            if filepath.endswith('.json'):
                format = 'json'
            elif filepath.endswith(('.yaml', '.yml')):
                format = 'yaml'
            else:
                raise ValueError(f"Unsupported file format: {filepath}")
        
        with open(filepath, 'r') as f:
            if format == 'json':
                self._config.update(json.load(f))
            elif format == 'yaml':
                self._config.update(yaml.safe_load(f))
        
        self.validate()
    
    def load_from_env(self, prefix: str = 'NLM_'):
        """Load configuration from environment variables.
        
        Args:
            prefix: Prefix for environment variables (default: 'NLM_')
        """
        for key, value in os.environ.items():
            if key.startswith(prefix):
                config_key = key[len(prefix):].lower().replace('-', '_')
                self._set_config_value(config_key, self._convert_env_value(value))
        
        self.validate()
    
    def _convert_env_value(self, value: str) -> Any:
        """Convert environment variable string to appropriate type.
        
        Args:
            value: String value from environment variable
            
        Returns:
            Converted value (int, float, bool, or string)
        """
        # Try to parse as int
        if value.isdigit():
            return int(value)
        
        # Try to parse as float
        try:
            float_val = float(value)
            if '.' in value or 'e' in value.lower():
                return float_val
        except ValueError:
            pass
        
        # Try to parse as bool
        value_lower = value.lower()
        if value_lower in ('true', '1', 'yes', 'on'):
            return True
        elif value_lower in ('false', '0', 'no', 'off'):
            return False
        
        return value
    
    def save_to_file(self, filepath: str, format: Optional[str] = None):
        """Save configuration to file in specified format.
        
        Args:
            filepath: Path to save configuration
            format: File format ('json', 'yaml', or auto-detected)
        """
        if format is None:
            if filepath.endswith('.json'):
                format = 'json'
            elif filepath.endswith(('.yaml', '.yml')):
                format = 'yaml'
            else:
                format = 'json'  # default to JSON
        
        with open(filepath, 'w') as f:
            if format == 'json':
                json.dump(self._config, f, indent=2)
            elif format == 'yaml':
                yaml.dump(self._config, f, default_flow_style=False)
    
    def validate(self, key: Optional[str] = None):
        """Validate configuration against schema.
        
        Args:
            key: Specific key to validate (optional, validates all if None)
            
        Raises:
            ValueError: If validation fails
        """
        if key:
            if key not in self._config:
                return
            
            is_valid, error_msg = self._validator.validate_key(key, self._config[key])
            if not is_valid:
                raise ValueError(error_msg)
        else:
            is_valid, errors = self._validator.validate_all(self._config)
            if not is_valid:
                raise ValueError("Configuration validation failed: " + "; ".join(errors))
    
    def get_with_validation(self, key: str, default: Any = None) -> Any:
        """Get configuration value with validation.
        
        Args:
            key: Configuration key
            default: Default value if key doesn't exist
            
        Returns:
            Configuration value
        """
        if key in self._config:
            return self._config[key]
        elif default is not None:
            return default
        
        # Try to get default from schema
        for schema in self._schemas:
            if schema.name == key and schema.default is not None:
                return schema.default
        
        raise KeyError(f"Configuration key '{key}' not found")
    
    def set(self, key: str, value: Any):
        """Set configuration value with validation.
        
        Args:
            key: Configuration key
            value: Configuration value
        """
        self._set_config_value(key, value)
        self.validate(key)
    
    def _set_config_value(self, key: str, value: Any):
        """Set configuration value without validation.
        
        Args:
            key: Configuration key
            value: Configuration value
        """
        self._config[key] = value
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key doesn't exist
            
        Returns:
            Configuration value
        """
        if key in self._config:
            return self._config[key]
        return default
    
    def has(self, key: str) -> bool:
        """Check if configuration key exists.
        
        Args:
            key: Configuration key
            
        Returns:
            True if key exists, False otherwise
        """
        return key in self._config
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys.
        
        Returns:
            List of configuration keys
        """
        return list(self._config.keys())
    
    def clear(self):
        """Clear all configuration entries."""
        self._config.clear()
    
    def summary(self) -> str:
        """Get a summary string of the configuration.
        
        Returns:
            Summary string
        """
        return f"Config({len(self._config)} items)"
    
    def __repr__(self) -> str:
        """Get string representation of configuration.
        
        Returns:
            String representation
        """
        return f"Config(keys={self.get_keys()})"

# Convenience functions

def create_default_config(schemas: Optional[List[ConfigSchema]] = None) -> 'Config':
    """Create a default configuration with optional schema.
    
    Args:
        schemas: Optional list of ConfigSchema objects
        
    Returns:
        Config instance
    """
    config = Config(schemas=schemas)
    # Add common default configuration
    config.set('brain.region_count', 1)
    config.set('brain.neurons_per_region', 100)
    config.set('brain.synapses_per_neuron', 50)
    config.set('simulation.max_steps', 1000)
    config.set('simulation.dt', 0.01)
    config.set('world.width', 100.0)
    config.set('world.height', 100.0)
    config.set('world.vision_width', 8)
    config.set('world.vision_height', 8)
    config.set('logging.level', 'info')
    config.set('logging.enable_file', False)
    config.set('learning.enable_reward_modulation', True)
    config.set('learning.enable_curiosity', True)
    config.set('learning.enable_structural_plasticity', False)
    config.set('learning.enable_development', False)
    
    return config

def load_config_from_file(filepath: str, schemas: Optional[List[ConfigSchema]] = None, 
                         format: Optional[str] = None) -> 'Config':
    """Load configuration from file.
    
    Args:
        filepath: Path to configuration file
        schemas: Optional schema definitions
        format: File format
        
    Returns:
        Config instance
    """
    config = Config(schemas=schemas)
    config.load_from_file(filepath, format)
    return config

def load_config_from_env(prefix: str = 'NLM_', schemas: Optional[List[ConfigSchema]] = None) -> 'Config':
    """Load configuration from environment variables.
    
    Args:
        prefix: Prefix for environment variables
        schemas: Optional schema definitions
        
    Returns:
        Config instance
    """
    config = Config(schemas=schemas)
    config.load_from_env(prefix)
    return config

# Export types for type checking
__all__ = [
    'ConfigSchema',
    'ConfigValidator',
    'Config',
    'create_default_config',
    'load_config_from_file',
    'load_config_from_env',
]