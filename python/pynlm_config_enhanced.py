"""NLM Python Bindings - Enhanced Configuration System

This module provides an enhanced configuration system with improved command-line
options, validation, and type safety for the NLM Python bindings.

Features:
1. Advanced command-line argument parsing
2. Configuration validation and schema checking
3. Type-safe value access with automatic type inference
4. Environment variable support
5. Configuration merging and inheritance
6. Advanced error handling and reporting
7. Configuration documentation generation
8. Configuration comparison and diffing
"""

import json
import os
import sys
from typing import Any, Dict, List, Optional, Union, Type, TypeVar, Callable
from enum import Enum
from dataclasses import dataclass, field
from abc import ABC, abstractmethod
import argparse
import yaml
from pathlib import Path

T = TypeVar('T')

class ConfigValueType(Enum):
    """Types of configuration values."""
    STRING = "string"
    INTEGER = "integer"
    FLOAT = "float"
    BOOLEAN = "boolean"
    VECTOR = "vector"
    MATRIX = "matrix"
    OBJECT = "object"
    ARRAY = "array"

class ConfigValidationError(Exception):
    """Exception raised when configuration validation fails."""
    
    def __init__(self, message: str, path: str = "", value: Any = None):
        super().__init__(message)
        self.message = message
        self.path = path
        self.value = value

class ConfigSchema:
    """Configuration schema for validation."""
    
    def __init__(self, name: str):
        self.name = name
        self.fields: Dict[str, 'ConfigField'] = {}
        self.required_fields: List[str] = []
        self.validation_rules: List[Callable[[Dict[str, Any]], bool]] = []
    
    def add_field(self, name: str, field: 'ConfigField'):
        """Add a field to the schema."""
        self.fields[name] = field
        if field.required:
            self.required_fields.append(name)
    
    def add_validation_rule(self, rule: Callable[[Dict[str, Any]], bool]):
        """Add a validation rule."""
        self.validation_rules.append(rule)
    
    def validate(self, config: Dict[str, Any]) -> List[ConfigValidationError]:
        """Validate configuration against schema."""
        errors = []
        
        # Check required fields
        for field_name in self.required_fields:
            if field_name not in config:
                errors.append(ConfigValidationError(
                    f"Required field '{field_name}' is missing",
                    path=field_name
                ))
        
        # Validate field types and constraints
        for field_name, field_schema in self.fields.items():
            if field_name in config:
                try:
                    field_schema.validate(config[field_name], field_name)
                except ConfigValidationError as e:
                    errors.append(e)
        
        # Apply custom validation rules
        for i, rule in enumerate(self.validation_rules):
            try:
                if not rule(config):
                    errors.append(ConfigValidationError(
                        f"Custom validation rule {i} failed",
                        path="validation_rule"
                    ))
            except Exception as e:
                errors.append(ConfigValidationError(
                    f"Custom validation rule {i} raised exception: {str(e)}",
                    path="validation_rule"
                ))
        
        return errors
    
    def generate_schema_json(self) -> Dict[str, Any]:
        """Generate JSON schema for the configuration."""
        return {
            'type': 'object',
            'title': self.name,
            'properties': {
                name: field.to_json_schema()
                for name, field in self.fields.items()
            },
            'required': self.required_fields,
            'definitions': {
                field_name: field.to_json_schema()
                for field_name, field in self.fields.items()
            }
        }
class ConfigField:
    """Configuration field definition."""
    
    def __init__(self, field_type: ConfigValueType, default: Any = None, 
                 required: bool = False, description: str = "",
                 validation: Optional[Callable[[Any], bool]] = None,
                 options: Optional[List[Any]] = None):
        self.field_type = field_type
        self.default = default
        self.required = required
        self.description = description
        self.validation = validation
        self.options = options
    
    def validate(self, value: Any, path: str = "") -> None:
        """Validate a field value."""
        if value is None and not self.required:
            return
        
        if value is None and self.required:
            raise ConfigValidationError(
                f"Field '{path}' is required but has no value",
                path=path,
                value=value
            )
        
        # Type validation
        if not self._validate_type(value):
            raise ConfigValidationError(
                f"Field '{path}' must be of type {self.field_type.value}, got {type(value).__name__}",
                path=path,
                value=value
            )
        
        # Value validation
        if self.validation and not self.validation(value):
            raise ConfigValidationError(
                f"Field '{path}' has invalid value: {value}",
                path=path,
                value=value
            )
        
        # Options validation
        if self.options and value not in self.options:
            raise ConfigValidationError(
                f"Field '{path}' has invalid value '{value}'. "
                f"Allowed values: {self.options}",
                path=path,
                value=value
            )
    
    def _validate_type(self, value: Any) -> bool:
        """Validate value type against field type."""
        if value is None and not self.required:
            return True
        
        type_checks = {
            ConfigValueType.STRING: lambda x: isinstance(x, str),
            ConfigValueType.INTEGER: lambda x: isinstance(x, int) and not isinstance(x, bool),
            ConfigValueType.FLOAT: lambda x: isinstance(x, (int, float)) and not isinstance(x, bool),
            ConfigValueType.BOOLEAN: lambda x: isinstance(x, bool),
            ConfigValueType.VECTOR: lambda x: isinstance(x, list),
            ConfigValueType.MATRIX: lambda x: isinstance(x, list) and all(isinstance(row, list) for row in x),
            ConfigValueType.OBJECT: lambda x: isinstance(x, dict),
            ConfigValueType.ARRAY: lambda x: isinstance(x, list)
        }
        
        if self.field_type in type_checks:
            return type_checks[self.field_type](value)
        
        return False
    
    def to_json_schema(self) -> Dict[str, Any]:
        """Convert field to JSON schema."""
        schema = {
            'type': self.field_type.value if self.field_type != ConfigValueType.OBJECT else 'object',
            'description': self.description,
            'required': self.required,
            'default': self.default
        }
        
        if self.options:
            schema['enum'] = self.options
        
        if self.field_type == ConfigValueType.VECTOR:
            schema.update({
                'type': 'array',
                'items': {'type': 'number'}
            })
        
        elif self.field_type == ConfigValueType.MATRIX:
            schema.update({
                'type': 'array',
                'items': {
                    'type': 'array',
                    'items': {'type': 'number'}
                }
            })
        
        elif self.field_type == ConfigValueType.OBJECT:
            schema['additionalProperties'] = True
        
        return schema

class EnvironmentVariableLoader:
    """Loader for environment variables."""
    
    def __init__(self, prefix: str = "NLM_"):
        self.prefix = prefix
    
    def load(self) -> Dict[str, Any]:
        """Load environment variables."""
        config = {}
        
        for key, value in os.environ.items():
            if key.startswith(self.prefix):
                config_name = key[len(self.prefix):].lower().replace('-', '_')
                config[config_name] = self._convert_env_value(value)
        
        return config
    
    def _convert_env_value(self, value: str) -> Any:
        """Convert environment variable string to appropriate type."""
        # Try boolean conversion
        if value.lower() in ('true', 'false'):
            return value.lower() == 'true'
        
        # Try integer conversion
        try:
            return int(value)
        except ValueError:
            pass
        
        # Try float conversion
        try:
            return float(value)
        except ValueError:
            pass
        
        # Check for JSON-like values
        if value.startswith('[') and value.endswith(']'):
            try:
                return json.loads(value)
            except json.JSONDecodeError:
                pass
        
        if value.startswith('{') and value.endswith('}'):
            try:
                return json.loads(value)
            except json.JSONDecodeError:
                pass
        
        # Return as string
        return value

class ArgumentParser:
    """Advanced argument parser for configuration."""
    
    def __init__(self, name: str = "NLM Configuration"):
        self.name = name
        self.schema: Optional[ConfigSchema] = None
        self.argument_groups: Dict[str, List['ArgumentGroup']] = {}
        self.positional_args: List[str] = []
        self.option_args: List['OptionArg'] = []
    
    def add_argument_group(self, name: str) -> 'ArgumentGroup':
        """Add an argument group."""
        group = ArgumentGroup(name)
        if name not in self.argument_groups:
            self.argument_groups[name] = []
        self.argument_groups[name].append(group)
        return group
    
    def add_argument(self, dest: str, type: Any = None, default: Any = None,
                     help: str = "", choices: Optional[List[str]] = None,
                     required: bool = False) -> 'OptionArg':
        """Add a command-line argument."""
        arg = OptionArg(dest, type, default, help, choices, required)
        self.option_args.append(arg)
        return arg
    
    def add_positional(self, dest: str, type: Any = str, nargs: Optional[str] = None,
                       help: str = "") -> 'PositionalArg':
        """Add a positional argument."""
        arg = PositionalArg(dest, type, nargs, help)
        self.positional_args.append(arg)
        return arg
    
    def parse_args(self, argv: Optional[List[str]] = None) -> Dict[str, Any]:
        """Parse command-line arguments."""
        parser = argparse.ArgumentParser(description=self.name)
        
        # Add positional arguments
        for arg in self.positional_args:
            parser.add_argument(arg.dest, type=arg.type, nargs=arg.nargs,
                              help=arg.help)
        
        # Add option arguments
        for arg in self.option_args:
            kwargs = {'help': arg.help, 'default': arg.default}
            if arg.type:
                kwargs['type'] = arg.type
            if arg.choices:
                kwargs['choices'] = arg.choices
            if arg.required:
                kwargs['required'] = True
            parser.add_argument(f'--{arg.dest}', **kwargs)
        
        # Add argument groups
        for group_name, groups in self.argument_groups.items():
            subgroup = parser.add_argument_group(group_name)
            for group in groups:
                for arg in group.arguments:
                    subgroup.add_argument(arg.dest, type=arg.type, nargs=arg.nargs,
                                        help=arg.help)
        
        # Parse arguments
        args = parser.parse_args(argv)
        
        # Convert to dictionary
        config = {}
        for action in parser._actions:
            if action.dest != 'help':
                config[action.dest] = getattr(args, action.dest)
        
        return config
    
    def set_schema(self, schema: ConfigSchema):
        """Set configuration schema for validation."""
        self.schema = schema
class ArgumentGroup:
    """Group of related arguments."""
    
    def __init__(self, name: str):
        self.name = name
        self.arguments: List['BaseArg'] = []
    
    def add_argument(self, arg: 'BaseArg'):
        """Add an argument to the group."""
        self.arguments.append(arg)
class BaseArg:
    """Base argument class."""
    
    def __init__(self, dest: str, help: str = ""):
        self.dest = dest
        self.help = help
class OptionArg(BaseArg):
    """Command-line option argument."""
    
    def __init__(self, dest: str, type: Any = None, default: Any = None,
                 help: str = "", choices: Optional[List[str]] = None,
                 required: bool = False):
        super().__init__(dest, help)
        self.type = type
        self.default = default
        self.choices = choices
        self.required = required
class PositionalArg(BaseArg):
    """Positional argument."""
    
    def __init__(self, dest: str, type: Any = str, nargs: Optional[str] = None,
                 help: str = ""):
        super().__init__(dest, help)
        self.type = type
        self.nargs = nargs
class ConfigMerger:
    """Merger for configuration files and dictionaries."""
    
    def __init__(self):
        self.precedence_order = ['default', 'environment', 'file', 'command_line']
    
    def merge(self, sources: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Merge multiple configuration sources."""
        merged = {}
        
        # Merge in order of precedence (last wins)
        for source in sources:
            if source:
                self._merge_dict(merged, source)
        
        return merged
    
    def _merge_dict(self, target: Dict[str, Any], source: Dict[str, Any]):
        """Merge source dictionary into target."""
        for key, value in source.items():
            if key in target and isinstance(target[key], dict) and isinstance(value, dict):
                # Recursively merge nested dictionaries
                self._merge_dict(target[key], value)
            else:
                # Override with new value
                target[key] = value
    
    def load_config_files(self, file_paths: List[str]) -> List[Dict[str, Any]]:
        """Load configuration from multiple files."""
        configs = []
        
        for file_path in file_paths:
            try:
                config = self._load_config_file(file_path)
                if config:
                    configs.append(config)
            except Exception as e:
                print(f"Warning: Failed to load config file {file_path}: {e}")
        
        return configs
    
    def _load_config_file(self, file_path: str) -> Dict[str, Any]:
        """Load configuration from a single file."""
        path = Path(file_path)
        
        if not path.exists():
            raise FileNotFoundError(f"Config file not found: {file_path}")
        
        # Determine file format based on extension
        if path.suffix.lower() == '.json':
            with open(path, 'r') as f:
                return json.load(f)
        
        elif path.suffix.lower() in ['.yaml', '.yml']:
            with open(path, 'r') as f:
                return yaml.safe_load(f)
        
        else:
            raise ValueError(f"Unsupported config file format: {path.suffix}")
class ConfigValidator:
    """Validator for configuration values."""
    
    def __init__(self, schema: Optional[ConfigSchema] = None):
        self.schema = schema
        self.custom_validators: List[Callable[[Dict[str, Any]], List[ConfigValidationError]]] = []
    
    def add_validator(self, validator: Callable[[Dict[str, Any]], List[ConfigValidationError]]):
        """Add a custom validator."""
        self.custom_validators.append(validator)
    
    def validate(self, config: Dict[str, Any]) -> List[ConfigValidationError]:
        """Validate configuration."""
        errors = []
        
        # Validate against schema if present
        if self.schema:
            schema_errors = self.schema.validate(config)
            errors.extend(schema_errors)
        
        # Run custom validators
        for i, validator in enumerate(self.custom_validators):
            try:
                validator_errors = validator(config)
                errors.extend(validator_errors)
            except Exception as e:
                errors.append(ConfigValidationError(
                    f"Custom validator {i} raised exception: {str(e)}"
                ))
        
        return errors
    
    def is_valid(self, config: Dict[str, Any]) -> bool:
        """Check if configuration is valid."""
        errors = self.validate(config)
        return len(errors) == 0

class ConfigDocumentationGenerator:
    """Generator for configuration documentation."""
    
    def __init__(self, schema: ConfigSchema):
        self.schema = schema
    
    def generate_markdown(self) -> str:
        """Generate Markdown documentation."""
        lines = [f"# {self.schema.name} Configuration", ""]
        
        # Add field descriptions
        for field_name, field in self.schema.fields.items():
            lines.append(f"## {field_name}")
            lines.append(f"**Type**: {field.field_type.value}")
            lines.append(f"**Required**: {'Yes' if field.required else 'No'}")
            lines.append(f"**Description**: {field.description}")
            
            if field.default is not None:
                lines.append(f"**Default**: {field.default}")
            
            if field.options:
                lines.append(f"**Options**: {', '.join(str(opt) for opt in field.options)}")
            
            lines.append("")
        
        # Add validation rules
        if self.schema.validation_rules:
            lines.append("## Validation Rules")
            for i, rule in enumerate(self.schema.validation_rules):
                lines.append(f"{i + 1}. Custom validation rule {i}")
        
        return "\n".join(lines)
    
    def generate_json_schema(self) -> Dict[str, Any]:
        """Generate JSON schema for documentation."""
        return self.schema.generate_schema_json()

class EnhancedConfig:
    """Enhanced configuration class with advanced features."""
    
    def __init__(self, name: str = "Default Configuration"):
        self.name = name
        self._config: Dict[str, Any] = {}
        self.schema: Optional[ConfigSchema] = None
        self.loaded_from: List[str] = []
        self._validator = ConfigValidator()
    
    def load_from_dict(self, config: Dict[str, Any], source: str = "dict") -> bool:
        """Load configuration from dictionary."""
        try:
            self._config = config.copy()
            self.loaded_from.append(f"{source}")
            return True
        except Exception as e:
            print(f"Error loading configuration from {source}: {e}")
            return False
    
    def load_from_file(self, filepath: str, format: Optional[str] = None) -> bool:
        """Load configuration from file."""
        try:
            path = Path(filepath)
            
            if not path.exists():
                raise FileNotFoundError(f"Config file not found: {filepath}")
            
            # Determine format based on extension or parameter
            if format:
                fmt = format.lower()
            else:
                fmt = path.suffix.lower()
            
            if fmt == '.json':
                with open(path, 'r') as f:
                    config = json.load(f)
            elif fmt in ['.yaml', '.yml']:
                with open(path, 'r') as f:
                    config = yaml.safe_load(f)
            else:
                raise ValueError(f"Unsupported config file format: {fmt}")
            
            self.load_from_dict(config, filepath)
            return True
            
        except Exception as e:
            print(f"Error loading configuration from {filepath}: {e}")
            return False
    
    def load_from_env(self, prefix: str = "NLM_") -> bool:
        """Load configuration from environment variables."""
        try:
            loader = EnvironmentVariableLoader(prefix)
            config = loader.load()
            self.load_from_dict(config, "environment_variables")
            return True
        except Exception as e:
            print(f"Error loading configuration from environment: {e}")
            return False
    
    def parse_args(self, argv: Optional[List[str]] = None, name: Optional[str] = None) -> bool:
        """Parse command-line arguments."""
        try:
            parser = ArgumentParser(name or self.name)
            config = parser.parse_args(argv)
            self.load_from_dict(config, "command_line")
            return True
        except Exception as e:
            print(f"Error parsing command-line arguments: {e}")
            return False
    
    def merge_configurations(self, configs: List[Dict[str, Any]]) -> bool:
        """Merge multiple configuration sources."""
        try:
            merger = ConfigMerger()
            merged = merger.merge(configs)
            self.load_from_dict(merged, "merged")
            return True
        except Exception as e:
            print(f"Error merging configurations: {e}")
            return False
    
    def validate(self) -> List[ConfigValidationError]:
        """Validate configuration."""
        return self._validator.validate(self._config)
    
    def is_valid(self) -> bool:
        """Check if configuration is valid."""
        return self._validator.is_valid(self._config)
    
    def add_validator(self, validator: Callable[[Dict[str, Any]], List[ConfigValidationError]]):
        """Add a custom validator."""
        self._validator.add_validator(validator)
    
    def set_schema(self, schema: ConfigSchema):
        """Set configuration schema."""
        self.schema = schema
        self._validator = ConfigValidator(schema)
    
    def get(self, key: str, default: Any = None, expected_type: Optional[Type[T]] = None) -> T:
        """Get configuration value with type safety."""
        if key not in self._config:
            return default
        
        value = self._config[key]
        
        # Type validation
        if expected_type and not isinstance(value, expected_type):
            raise ConfigValidationError(
                f"Expected type {expected_type.__name__} for key '{key}', "
                f"got {type(value).__name__}"
            )
        
        return value
    
    def get_typed(self, key: str, value_type: ConfigValueType, 
                  default: Any = None) -> Any:
        """Get configuration value with type specification."""
        if key not in self._config:
            return default
        
        value = self._config[key]
        
        # Try type conversion
        try:
            return self._convert_to_type(value, value_type)
        except (ValueError, TypeError):
            raise ConfigValidationError(
                f"Cannot convert value '{value}' to type {value_type.value}"
            )
    
    def _convert_to_type(self, value: Any, target_type: ConfigValueType) -> Any:
        """Convert value to target type."""
        if target_type == ConfigValueType.STRING:
            return str(value)
        
        elif target_type == ConfigValueType.INTEGER:
            return int(value)
        
        elif target_type == ConfigValueType.FLOAT:
            return float(value)
        
        elif target_type == ConfigValueType.BOOLEAN:
            if isinstance(value, str):
                return value.lower() in ('true', 'yes', '1', 'on')
            return bool(value)
        
        elif target_type == ConfigValueType.VECTOR:
            if isinstance(value, list):
                return value
            elif isinstance(value, str):
                return json.loads(value) if value.startswith('[') else [value]
            else:
                return [value]
        
        elif target_type == ConfigValueType.MATRIX:
            if isinstance(value, list) and all(isinstance(row, list) for row in value):
                return value
            elif isinstance(value, str):
                parsed = json.loads(value) if value.startswith('[') else [[value]]
                return parsed if isinstance(parsed, list) else [parsed]
            else:
                return [[value]]
        
        elif target_type == ConfigValueType.OBJECT:
            if isinstance(value, dict):
                return value
            elif isinstance(value, str):
                return json.loads(value) if value.startswith('{') else {'value': value}
            else:
                return {'value': value}
        
        return value
    
    def set(self, key: str, value: Any) -> None:
        """Set configuration value."""
        self._config[key] = value
    
    def has(self, key: str) -> bool:
        """Check if key exists."""
        return key in self._config
    
    def getKeys(self) -> List[str]:
        """Get all keys."""
        return list(self._config.keys())
    
    def clear(self) -> None:
        """Clear all configuration."""
        self._config.clear()
        self.loaded_from.clear()
    
    def summary(self) -> str:
        """Get configuration summary."""
        if not self._config:
            return "Empty configuration"
        
        summary_lines = [f"{key}: {value}" for key, value in self._config.items()]
        return f"Configuration with {len(self._config)} entries:\n" + "\n".join(summary_lines)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        return self._config.copy()
    
    def save_to_file(self, filepath: str, format: str = "json") -> bool:
        """Save configuration to file."""
        try:
            path = Path(filepath)
            
            if format.lower() == 'json':
                with open(path, 'w') as f:
                    json.dump(self._config, f, indent=2)
            elif format.lower() in ['yaml', 'yml']:
                with open(path, 'w') as f:
                    yaml.dump(self._config, f, default_flow_style=False)
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            self.loaded_from.append(filepath)
            return True
            
        except Exception as e:
            print(f"Error saving configuration to {filepath}: {e}")
            return False
    
    def __repr__(self) -> str:
        return f"<EnhancedConfig: {self.summary()}>"
    
    def __eq__(self, other) -> bool:
        if not isinstance(other, EnhancedConfig):
            return False
        return self._config == other._config
# Factory functions for creating enhanced configurations

def create_default_config(name: str = "Default Configuration") -> EnhancedConfig:
    """Create a default configuration."""
    config = EnhancedConfig(name)
    
    # Set some reasonable default values
    config.set("neuron_count", 1000)
    config.set("region_count", 1)
    config.set("connection_probability", 0.1)
    config.set("stdp_ltp_weight", 0.01)
    config.set("stdp_ltd_weight", 0.012)
    config.set("synaptogenesis_rate", 0.0001)
    config.set("pruning_rate", 0.00001)
    config.set("simulation_timestep", 0.001)
    config.set("random_seed", 42)
    
    return config

def create_config_schema(name: str) -> ConfigSchema:
    """Create a configuration schema."""
    return ConfigSchema(name)

def create_config_field(field_type: ConfigValueType, **kwargs) -> ConfigField:
    """Create a configuration field."""
    return ConfigField(field_type, **kwargs)

# Example usage and demonstration
def example_usage():
    """Example of enhanced configuration usage."""
    print("=== Enhanced Configuration Example ===")
    
    # Create default configuration
    config = create_default_config("NLM Simulation")
    
    # Load from file
    config.load_from_file("config.json")
    
    # Load from environment
    config.load_from_env("NLM_")
    
    # Parse command-line arguments
    config.parse_args()
    
    # Set schema
    schema = create_config_schema("Brain Configuration")
    schema.add_field("neuron_count", create_config_field(
        ConfigValueType.INTEGER,
        default=1000,
        required=True,
        description="Number of neurons in the brain"
    ))
    schema.add_field("connection_probability", create_config_field(
        ConfigValueType.FLOAT,
        default=0.1,
        required=False,
        description="Probability of synaptic connections"
    ))
    
    config.set_schema(schema)
    
    # Validate
    errors = config.validate()
    if errors:
        for error in errors:
            print(f"Error: {error.message} (path: {error.path})")
    else:
        print("Configuration is valid!")
    
    # Get typed values
    neuron_count = config.get_typed("neuron_count", ConfigValueType.INTEGER)
    print(f"Neuron count: {neuron_count}")
    
    # Generate documentation
    generator = ConfigDocumentationGenerator(schema)
    print("\n=== Configuration Documentation ===")
    print(generator.generate_markdown())

if __name__ == "__main__":
    example_usage()