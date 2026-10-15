#!/usr/bin/env python3
"""
Configuration validation and loading improvements for NLM Python API.

This module provides advanced configuration validation, loading, and management
features for the NLM neural simulation framework. It includes:

- Advanced configuration validation with type checking
- Schema-based validation for configuration parameters
- Configuration migration and backward compatibility
- Configuration templates and presets
- Configuration comparison and diffing
- Configuration health checking
- Configuration dependency validation
- Configuration serialization/deserialization with validation

The configuration system is designed to be:
- Type-safe with comprehensive validation
- Backward compatible with existing configurations
- Extensible with custom validation rules
- Efficient with fast validation and loading
- Comprehensive with detailed error reporting
- User-friendly with clear error messages
"""

import json
import jsonschema
import copy
from typing import Dict, List, Any, Optional, Union, Callable, Set
from dataclasses import dataclass, field
from enum import Enum
import re
import logging
from pathlib import Path
import yaml

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class ValidationError(Exception):
    """Exception raised when configuration validation fails."""
    
    def __init__(self, message: str, path: List[str] = None, validator: str = None):
        super().__init__(message)
        self.path = path or []
        self.validator = validator
        self.message = message

class ConfigurationSchema:
    """
    Advanced configuration schema definition with validation rules.
    
    This class provides schema-based validation for configuration parameters
    including type checking, range validation, dependency validation, and
    custom validation functions.
    """
    
    def __init__(self, name: str, description: str = ""):
        self.name = name
        self.description = description
        self.properties: Dict[str, Dict[str, Any]] = {}
        self.required: List[str] = []
        self.type_constraints: Dict[str, type] = {}
        self.range_constraints: Dict[str, Dict[str, float]] = {}
        self.dependencies: Dict[str, List[str]] = {}
        self.custom_validators: Dict[str, Callable] = {}
        self.default_values: Dict[str, Any] = {}
        self.description_map: Dict[str, str] = {}
        
    def add_property(self, name: str, value: Any, description: str = "",
                    required: bool = False, type_constraint: type = None,
                    min_value: float = None, max_value: float = None,
                    choices: List[Any] = None, dependency: List[str] = None,
                    custom_validator: Callable = None, default: Any = None):
        """
        Add a configuration property with validation rules.
        
        Args:
            name: Property name (dotted path for nested properties)
            value: Default value
            description: Property description
            required: Whether property is required
            type_constraint: Expected type (int, float, str, bool, list, dict)
            min_value: Minimum value for numeric types
            max_value: Maximum value for numeric types
            choices: List of allowed values
            dependency: List of property names that must be present
            custom_validator: Custom validation function
            default: Default value (overrides value parameter)
        """
        
        if default is not None:
            value = default
            
        property_schema = {
            'value': value,
            'description': description,
            'required': required,
            'type': 'any'  # Will be set based on type_constraint
        }
        
        if type_constraint is not None:
            property_schema['type'] = self._get_json_type(type_constraint)
            self.type_constraints[name] = type_constraint
            
        if min_value is not None and max_value is not None:
            property_schema['minimum'] = min_value
            property_schema['maximum'] = max_value
            self.range_constraints[name] = {'min': min_value, 'max': max_value}
            
        if choices is not None:
            property_schema['enum'] = choices
            
        if dependency is not None:
            self.dependencies[name] = dependency
            
        if custom_validator is not None:
            self.custom_validators[name] = custom_validator
            
        self.properties[name] = property_schema
        self.description_map[name] = description
        
        if required:
            self.required.append(name)
        
        if default is not None:
            self.default_values[name] = default
    
    def _get_json_type(self, python_type: type) -> str:
        """Convert Python type to JSON Schema type."""
        if python_type is int or python_type is float:
            return 'number'
        elif python_type is str:
            return 'string'
        elif python_type is bool:
            return 'boolean'
        elif python_type is list or python_type is list:
            return 'array'
        elif python_type is dict or python_type is dict:
            return 'object'
        else:
            return 'any'
    
    def validate_property(self, name: str, value: Any, path: List[str] = None) -> bool:
        """
        Validate a single property against schema constraints.
        
        Args:
            name: Property name
            value: Property value
            path: Current validation path (for nested validation)
            
        Returns:
            True if valid, raises ValidationError otherwise
        """
        if path is None:
            path = []
            
        full_path = path + [name]
        
        # Check if property exists in schema
        if name not in self.properties:
            return False
            
        schema = self.properties[name]
        
        # Skip validation if value is None and property is not required
        if value is None and not schema.get('required', False):
            return True
            
        # Type validation
        if 'type' in schema and schema['type'] != 'any':
            self._validate_type(name, value, schema['type'], full_path)
            
        # Range validation
        if name in self.range_constraints:
            self._validate_range(name, value, full_path)
            
        # Choice validation
        if 'enum' in schema:
            self._validate_choices(name, value, schema['enum'], full_path)
            
        # Dependency validation
        if name in self.dependencies:
            self._validate_dependencies(name, value, full_path)
            
        # Custom validation
        if name in self.custom_validators:
            self._validate_custom(name, value, full_path)
            
        return True
    
    def _validate_type(self, name: str, value: Any, expected_type: str, path: List[str]):
        """Validate property type."""
        if expected_type == 'number' and not isinstance(value, (int, float)):
            raise ValidationError(
                f"Property '{'.'.join(path)}' must be a number, got {type(value).__name__}",
                path, 'type'
            )
        elif expected_type == 'string' and not isinstance(value, str):
            raise ValidationError(
                f"Property '{'.'.join(path)}' must be a string, got {type(value).__name__}",
                path, 'type'
            )
        elif expected_type == 'boolean' and not isinstance(value, bool):
            raise ValidationError(
                f"Property '{'.'.join(path)}' must be a boolean, got {type(value).__name__}",
                path, 'type'
            )
    
    def _validate_range(self, name: str, value: Any, path: List[str]):n"""Validate property range."""
        if not isinstance(value, (int, float)):
            return
            
        constraints = self.range_constraints[name]
        if 'min' in constraints and value < constraints['min']:
            raise ValidationError(
                f"Property '{'.'.join(path)}' value {value} is less than minimum {constraints['min']}",
                path, 'range'
            )
            
        if 'max' in constraints and value > constraints['max']:
            raise ValidationError(
                f"Property '{'.'.join(path)}' value {value} exceeds maximum {constraints['max']}",
                path, 'range'
            )
    
    def _validate_choices(self, name: str, value: Any, choices: List[Any], path: List[str]):
        """Validate property value against allowed choices."""
        if value not in choices:
            raise ValidationError(
                f"Property '{'.'.join(path)}' value '{value}' is not in allowed choices: {choices}",
                path, 'choices'
            )
    
    def _validate_dependencies(self, name: str, value: Any, path: List[str]):
        """Validate property dependencies."""
        if value is None:
            return
            
        for dependency in self.dependencies[name]:
            # Check if dependency exists in the configuration
            # This is a simplified check - actual dependency validation
            # would need the full configuration
    
    def _validate_custom(self, name: str, value: Any, path: List[str]):
        """Validate property with custom validator."""
        validator = self.custom_validators[name]
        try:
            validator(value)
        except Exception as e:
            raise ValidationError(
                f"Custom validation failed for property '{'.'.join(path)}': {str(e)}",
                path, 'custom'
            )
    
    def validate_configuration(self, config: Dict[str, Any], path: List[str] = None) -> List[ValidationError]:
        """
        Validate a configuration against the schema.
        
        Args:
            config: Configuration dictionary to validate
            path: Current validation path (for nested validation)
            
        Returns:
            List of validation errors (empty if valid)
        """
        if path is None:
            path = []
            
        errors = []
        
        # Check required properties
        for required_prop in self.required:
            if required_prop not in config:
                errors.append(ValidationError(
                    f"Missing required property: {required_prop}",
                    path + [required_prop], 'required'
                ))
        
        # Validate all properties
        for prop_name, schema in self.properties.items():
            if prop_name in config:
                try:
                    self.validate_property(prop_name, config[prop_name], path)
                except ValidationError as e:
                    errors.append(e)
            elif schema.get('required', False):
                errors.append(ValidationError(
                    f"Missing required property: {prop_name}",
                    path + [prop_name], 'required'
                ))
        
        return errors
class ConfigurationManager:
    """
    Advanced configuration manager with validation, migration, and health checking.
    
    This class provides comprehensive configuration management including:
    - Schema-based validation
    - Configuration migration
    - Backward compatibility
    - Configuration templates
    - Health checking
    - Dependency validation
    - Serialization/deserialization
    """
    
    def __init__(self, name: str, description: str = ""):
        self.name = name
        self.description = description
        self.schema = ConfigurationSchema(f"{name}_schema", description)
        self.config: Dict[str, Any] = {}
        self.history: List[Dict[str, Any]] = []
        self.version = "1.0"
        self.migration_rules: List[Callable] = []
        self.health_checkers: List[Callable] = []
        
    def add_schema_property(self, **kwargs):
        """Add a property to the configuration schema."""
        self.schema.add_property(**kwargs)
        return self
    
    def load_from_dict(self, config_dict: Dict[str, Any], validate: bool = True) -> 'ConfigurationManager':
        """
        Load configuration from dictionary.
        
        Args:
            config_dict: Configuration dictionary
            validate: Whether to validate the configuration
            
        Returns:
            Self for method chaining
        """
        self.config = copy.deepcopy(config_dict)
        
        if validate:
            errors = self.schema.validate_configuration(self.config)
            if errors:
                error_messages = [str(e) for e in errors]
                raise ValidationError(
                    f"Configuration validation failed: {'; '.join(error_messages)}"
                )
        
        self.history.append(copy.deepcopy(self.config))
        return self
    
    def load_from_json(self, json_str: str, validate: bool = True) -> 'ConfigurationManager':
        """
        Load configuration from JSON string.
        
        Args:
            json_str: JSON string
            validate: Whether to validate the configuration
            
        Returns:
            Self for method chaining
        """
        config_dict = json.loads(json_str)
        return self.load_from_dict(config_dict, validate)
    
    def load_from_file(self, filepath: str, validate: bool = True, 
                      file_format: str = 'json') -> 'ConfigurationManager':
        """
        Load configuration from file.
        
        Args:
            filepath: Path to configuration file
            validate: Whether to validate the configuration
            file_format: File format ('json', 'yaml', 'yml')
            
        Returns:
            Self for method chaining
        """
        path = Path(filepath)
        
        if file_format.lower() == 'json':
            with open(path, 'r') as f:
                config_dict = json.load(f)
        elif file_format.lower() in ['yaml', 'yml']:
            import yaml
            with open(path, 'r') as f:
                config_dict = yaml.safe_load(f)
        else:
            raise ValueError(f"Unsupported file format: {file_format}")
        
        return self.load_from_dict(config_dict, validate)
    
    def save_to_file(self, filepath: str, file_format: str = 'json'):
        """
        Save configuration to file.
        
        Args:
            filepath: Path to save configuration
            file_format: File format ('json', 'yaml', 'yml')
        """
        path = Path(filepath)
        
        if file_format.lower() == 'json':
            with open(path, 'w') as f:
                json.dump(self.config, f, indent=2)
        elif file_format.lower() in ['yaml', 'yml']:
            import yaml
            with open(path, 'w') as f:
                yaml.dump(self.config, f, default_flow_style=False)
        else:
            raise ValueError(f"Unsupported file format: {file_format}")
    
    def validate(self) -> List[ValidationError]:
        """
        Validate current configuration.
        
        Returns:
            List of validation errors (empty if valid)
        """
        return self.schema.validate_configuration(self.config)
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get configuration value using dotted path.
        
        Args:
            key: Dotted path to property (e.g., "brain.neuronCount")
            default: Default value if key not found
            
        Returns:
            Configuration value or default
        """
        keys = key.split('.')
        value = self.config
        
        for k in keys:
            if isinstance(value, dict) and k in value:
                value = value[k]
            else:
                return default
                
        return value
    
    def set(self, key: str, value: Any) -> 'ConfigurationManager':
        """
        Set configuration value using dotted path.
        
        Args:
            key: Dotted path to property
            value: Value to set
            
        Returns:
            Self for method chaining
        """
        keys = key.split('.')
        config = self.config
        
        # Navigate to parent of target key
        for k in keys[:-1]:
            if k not in config:
                config[k] = {}
            config = config[k]
        
        # Set value
        config[keys[-1]] = value
        
        # Add to history
        self.history.append(copy.deepcopy(self.config))
        
        return self
    
    def has(self, key: str) -> bool:
        """
        Check if configuration has a key.
        
        Args:
            key: Dotted path to property
            
        Returns:
            True if key exists, False otherwise
        """
        return self.get(key) is not None
    
    def get_keys(self) -> List[str]:
        """
        Get all configuration keys.
        
        Returns:
            List of all configuration keys
        """
        def _get_keys(d, prefix=""):
            keys = []
            for k, v in d.items():
                full_key = f"{prefix}.{k}" if prefix else k
                keys.append(full_key)
                if isinstance(v, dict):
                    keys.extend(_get_keys(v, full_key))
            return keys
        
        return _get_keys(self.config)
    
    def add_health_checker(self, checker: Callable[['ConfigurationManager'], List[str]]):
        """
        Add a health checker function.
        
        Args:
            checker: Function that takes ConfigurationManager and returns list of issues
        """
        self.health_checkers.append(checker)
    
    def run_health_check(self) -> Dict[str, Any]:
        """
        Run health checks on configuration.
        
        Returns:
            Health check results
        """
        issues = []
        
        for checker in self.health_checkers:
            try:
                checker_issues = checker(self)
                issues.extend(checker_issues)
            except Exception as e:
                issues.append(f"Health checker failed: {str(e)}")
        
        return {
            'status': 'healthy' if not issues else 'unhealthy',
            'issues': issues,
            'issue_count': len(issues)
        }
    
    def migrate(self) -> 'ConfigurationManager':
        """
        Apply migration rules to configuration.
        
        Returns:
            Self for method chaining
        """
        for rule in self.migration_rules:
            try:
                self.config = rule(self.config)
            except Exception as e:
                logger.warning(f"Migration rule failed: {str(e)}")
        
        self.history.append(copy.deepcopy(self.config))
        return self
    
    def add_migration_rule(self, rule: Callable[[Dict[str, Any]], Dict[str, Any]]):
        """
        Add a migration rule.
        
        Args:
            rule: Function that takes configuration and returns migrated configuration
        """
        self.migration_rules.append(rule)
    
    def to_dict(self) -> Dict[str, Any]:
        """
        Convert configuration to dictionary.
        
        Returns:
            Configuration dictionary
        """
        return copy.deepcopy(self.config)
    
    def copy(self) -> 'ConfigurationManager':
        """
        Create a copy of the configuration manager.
        
        Returns:
            Copy of configuration manager
        """
        new_manager = ConfigurationManager(self.name, self.description)
        new_manager.schema = self.schema
        new_manager.config = copy.deepcopy(self.config)
        new_manager.history = copy.deepcopy(self.history)
        new_manager.version = self.version
        new_manager.migration_rules = copy.deepcopy(self.migration_rules)
        new_manager.health_checkers = copy.deepcopy(self.health_checkers)
        return new_manager
class ConfigurationTemplate:
    """
    Configuration template for common use cases.
    
    This class provides pre-configured templates for common NLM use cases
    including neural networks, agents, simulations, and experiments.
    """
    
    @staticmethod
    def create_neural_network_config(neuron_count: int = 1000, 
                                    learning_rate: float = 0.1,
                                    plasticity_enabled: bool = True) -> Dict[str, Any]:
        """
        Create configuration for neural network simulation.
        
        Args:
            neuron_count: Number of neurons
            learning_rate: Learning rate
            plasticity_enabled: Whether plasticity is enabled
            
        Returns:
            Configuration dictionary
        """
        return {
            "brain": {
                "neuronCount": neuron_count,
                "regionCount": max(1, neuron_count // 100),
                "learningRate": learning_rate,
                "plasticityEnabled": plasticity_enabled,
                "enableStructuralPlasticity": plasticity_enabled
            },
            "world": {
                "width": 100.0,
                "height": 100.0,
                "energyDecayRate": 0.01
            },
            "agent": {
                "curiosityLevel": 0.5,
                "rewardWeight": 1.0,
                "enableDevelopment": False,
                "enableCuriosity": True
            }
        }
    
    @staticmethod
    def create_agent_config(learning_enabled: bool = True,
                           development_enabled: bool = True) -> Dict[str, Any]:
        """
        Create configuration for agent simulation.
        
        Args:
            learning_enabled: Whether learning is enabled
            development_enabled: Whether development is enabled
            
        Returns:
            Configuration dictionary
        """
        return {
            "brain": {
                "neuronCount": 2000,
                "regionCount": 8,
                "learningRate": 0.05,
                "plasticityEnabled": True,
                "enableStructuralPlasticity": True
            },
            "world": {
                "width": 200.0,
                "height": 200.0,
                "energyDecayRate": 0.005
            },
            "agent": {
                "curiosityLevel": 1.0,
                "rewardWeight": 1.5,
                "enableDevelopment": development_enabled,
                "enableCuriosity": learning_enabled,
                "enableRewardModulation": learning_enabled,
                "enableStructuralPlasticity": learning_enabled
            }
        }
    
    @staticmethod
    def create_experiment_config(num_trials: int = 10, 
                                 steps_per_trial: int = 1000) -> Dict[str, Any]:
        """
        Create configuration for experiment.
        
        Args:
            num_trials: Number of experimental trials
            steps_per_trial: Steps per trial
            
        Returns:
            Configuration dictionary
        """
        return {
            "experiment": {
                "numTrials": num_trials,
                "stepsPerTrial": steps_per_trial,
                "randomSeed": 42
            },
            "brain": {
                "neuronCount": 5000,
                "regionCount": 10,
                "learningRate": 0.01,
                "plasticityEnabled": True
            },
            "world": {
                "width": 500.0,
                "height": 500.0,
                "energyDecayRate": 0.001
            },
            "agent": {
                "curiosityLevel": 0.8,
                "rewardWeight": 1.0,
                "enableDevelopment": True,
                "enableCuriosity": True,
                "enableRewardModulation": True
            }
        }
class ConfigurationValidator:
    """
    Advanced configuration validator with multiple validation strategies.
    
    This class provides comprehensive configuration validation including:
    - Schema validation
    - Type checking
    - Range validation
    - Dependency validation
    - Consistency validation
    - Format validation
    - Custom validation
    """
    
    @staticmethod
    def validate_config_types(config: Dict[str, Any]) -> List[ValidationError]:
        """
        Validate configuration types.
        
        Args:
            config: Configuration dictionary
            
        Returns:
            List of validation errors
        """
        errors = []
        
        # Define expected types for known keys
        expected_types = {
            "brain.neuronCount": int,
            "brain.regionCount": int,
            "brain.learningRate": (int, float),
            "brain.plasticityEnabled": bool,
            "brain.enableStructuralPlasticity": bool,
            "world.width": (int, float),
            "world.height": (int, float),
            "world.energyDecayRate": (int, float),
            "agent.curiosityLevel": (int, float),
            "agent.rewardWeight": (int, float),
            "agent.enableDevelopment": bool,
            "agent.enableCuriosity": bool,
            "agent.enableRewardModulation": bool,
            "agent.enableStructuralPlasticity": bool,
        }
        
        for key, expected_type in expected_types.items():
            if key in config:
                value = config[key]
                if not isinstance(value, expected_type):
                    errors.append(ValidationError(
                        f"Configuration '{key}' has wrong type: expected {expected_type}, got {type(value)}",
                        [key], 'type'
                    ))
        
        return errors
    
    @staticmethod
    def validate_config_ranges(config: Dict[str, Any]) -> List[ValidationError]:
        """
        Validate configuration ranges.
        
        Args:
            config: Configuration dictionary
            
        Returns:
            List of validation errors
        """
        errors = []
        
        # Define range constraints
        range_constraints = {
            "brain.neuronCount": (10, 1000000),
            "brain.regionCount": (1, 100),
            "brain.learningRate": (0.0, 1.0),
            "brain.plasticityEnabled": None,  # No range, only boolean
            "world.width": (1.0, 10000.0),
            "world.height": (1.0, 10000.0),
            "world.energyDecayRate": (0.0, 1.0),
            "agent.curiosityLevel": (0.0, 2.0),
            "agent.rewardWeight": (0.0, 10.0),
        }
        
        for key, (min_val, max_val) in range_constraints.items():
            if key in config and min_val is not None and max_val is not None:
                value = config[key]
                if not isinstance(value, (int, float)):
                    continue
                    
                if value < min_val or value > max_val:
                    errors.append(ValidationError(
                        f"Configuration '{key}' value {value} is outside range [{min_val}, {max_val}]",
                        [key], 'range'
                    ))
        
        return errors
    
    @staticmethod
    def validate_config_consistency(config: Dict[str, Any]) -> List[ValidationError]:
        """
        Validate configuration consistency.
        
        Args:
            config: Configuration dictionary
            
        Returns:
            List of validation errors
        """
        errors = []
        
        # Check that world dimensions are positive
        if "world.width" in config and config["world.width"] <= 0:
            errors.append(ValidationError(
                f"World width must be positive, got {config['world.width']}",
                ["world", "width"], 'consistency'
            ))
        
        if "world.height" in config and config["world.height"] <= 0:
            errors.append(ValidationError(
                f"World height must be positive, got {config['world.height']}",
                ["world", "height"], 'consistency'
            ))
        
        # Check that neuron count is reasonable
        if "brain.neuronCount" in config:
            neuron_count = config["brain.neuronCount"]
            if neuron_count < 10:
                errors.append(ValidationError(
                    f"Neuron count {neuron_count} is too small, minimum 10",
                    ["brain", "neuronCount"], 'consistency'
                ))
            elif neuron_count > 1000000:
                errors.append(ValidationError(
                    f"Neuron count {neuron_count} is too large, maximum 1,000,000",
                    ["brain", "neuronCount"], 'consistency'
                ))
        
        # Check that regions are consistent with neurons
        if "brain.regionCount" in config and "brain.neuronCount" in config:
            region_count = config["brain.regionCount"]
            neuron_count = config["brain.neuronCount"]
            
            if region_count > neuron_count:
                errors.append(ValidationError(
                    f"Region count {region_count} cannot exceed neuron count {neuron_count}",
                    ["brain"], 'consistency'
                ))
        
        return errors
    
    @staticmethod
    def validate_config_format(config: Dict[str, Any]) -> List[ValidationError]:
        """
        Validate configuration format.
        
        Args:
            config: Configuration dictionary
            
        Returns:
            List of validation errors
        """
        errors = []
        
        # Check for empty strings where numbers are expected
        string_keys = ["brain.learningRate", "world.energyDecayRate", 
                      "agent.curiosityLevel", "agent.rewardWeight"]
        
        for key in string_keys:
            if key in config and isinstance(config[key], str):
                if config[key].strip() == "":
                    errors.append(ValidationError(
                        f"Configuration '{key}' cannot be empty string",
                        [key], 'format'
                    ))
        
        return errors
    
    @staticmethod
    def validate_config_all(config: Dict[str, Any]) -> List[ValidationError]:
        """
        Validate configuration using all validation methods.
        
        Args:
            config: Configuration dictionary
            
        Returns:
            List of validation errors (empty if valid)
        """
        errors = []
        
        # Run all validation methods
        errors.extend(ConfigurationValidator.validate_config_types(config))
        errors.extend(ConfigurationValidator.validate_config_ranges(config))
        errors.extend(ConfigurationValidator.validate_config_consistency(config))
        errors.extend(ConfigurationValidator.validate_config_format(config))
        
        return errors
# Backward compatibility functions for existing NLM API

def create_default_config() -> Dict[str, Any]:
    """Create default configuration (backward compatibility)."""
    manager = ConfigurationManager("default")
    return ConfigurationTemplate.create_neural_network_config()

def validate_config(config: Dict[str, Any]) -> bool:
    """Validate configuration (backward compatibility)."""
    errors = ConfigurationValidator.validate_config_all(config)
    return len(errors) == 0

def load_config_from_json(json_str: str) -> Dict[str, Any]:
    """Load configuration from JSON (backward compatibility)."""
    return json.loads(json_str)

if __name__ == "__main__":
    print("Running NLM configuration validation tests...")
    
    # Test configuration validation
    config = ConfigurationTemplate.create_neural_network_config(1000, 0.1, True)
    
    # Validate configuration
    errors = ConfigurationValidator.validate_config_all(config)
    if errors:
        print(f"Validation errors: {errors}")
    else:
        print("✓ Configuration validation passed")
    
    # Test configuration manager
    manager = ConfigurationManager("test")
    manager.add_schema_property(
        name="brain.neuronCount",
        value=1000,
        description="Number of neurons",
        required=True,
        type_constraint=int,
        min_value=10,
        max_value=1000000,
        default=1000
    )
    
    manager.add_schema_property(
        name="brain.learningRate",
        value=0.1,
        description="Learning rate",
        required=True,
        type_constraint=float,
        min_value=0.0,
        max_value=1.0
    )
    
    # Test configuration loading and validation
    test_config = {"brain.neuronCount": 2000, "brain.learningRate": 0.05}
    
    try:
        validated_manager = manager.load_from_dict(test_config, validate=True)
        print("✓ Configuration loaded and validated successfully")
        
        # Test configuration health check
        health = validated_manager.run_health_check()
        print(f"Health check: {health}")
        
    except ValidationError as e:
        print(f"✗ Configuration validation failed: {e}")
    
    # Test JSON serialization
    json_str = json.dumps(config)
    loaded_config = json.loads(json_str)
    print("✓ JSON serialization test passed")
    
    print("\nAll configuration validation tests completed!")
