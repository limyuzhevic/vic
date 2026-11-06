#!/usr/bin/env python3

"""
Configuration validation and error handling for NLM (Neural Learning Machine)

This module provides comprehensive configuration validation and error handling
capabilities for NLM systems, including:
- Configuration schema validation
- Type checking and coercion
- Value range validation
- Cross-parameter validation
- Error recovery and reporting
- Validation statistics and metrics

The configuration validation system provides:
1. Comprehensive schema definitions for all configuration parameters
2. Automatic type checking and coercion
3. Value range and constraint validation
4. Cross-parameter validation rules
5. Detailed error reporting and recovery
6. Validation statistics and performance metrics
7. Integration with existing error handling frameworks
"""

import json
import re
import os
from typing import Dict, List, Any, Optional, Union, Callable, Set
from dataclasses import dataclass, field
from enum import Enum
from abc import ABC, abstractmethod
import logging

class ValidationError(Exception):
    """Base exception for configuration validation errors"""
    
    def __init__(self, message: str, parameter: str = "", value: Any = None, 
                 suggestion: Optional[str] = None):
        super().__init__(message)
        self.message = message
        self.parameter = parameter
        self.value = value
        self.suggestion = suggestion
    
    def __str__(self):
        if self.parameter and self.value is not None:
            return f"{self.message} (parameter: '{self.parameter}', value: {self.value})"
        elif self.parameter:
            return f"{self.message} (parameter: '{self.parameter}')"
        return self.message

class ValidationWarning(Warning):
    """Warning for configuration validation issues that don't cause failures"""
    pass

class ValidationResult:
    """Result of configuration validation"""
    
    def __init__(self):
        self.is_valid = True
        self.errors: List[ValidationError] = []
        self.warnings: List[ValidationWarning] = []
        self.processed_values: Dict[str, Any] = {}
        self.metadata: Dict[str, Any] = {}
    
    def add_error(self, error: ValidationError):
        """Add a validation error"""
        self.errors.append(error)
        self.is_valid = False
    
    def add_warning(self, warning: ValidationWarning):
        """Add a validation warning"""
        self.warnings.append(warning)
    
    def add_error_with_context(self, message: str, parameter: str = "", 
                              value: Any = None, suggestion: Optional[str] = None):
        """Add a validation error with context"""
        error = ValidationError(message, parameter, value, suggestion)
        self.add_error(error)
    
    def add_warning_with_context(self, message: str, parameter: str = "", 
                                value: Any = None, suggestion: Optional[str] = None):
        """Add a validation warning with context"""
        warning = ValidationWarning(f"{message} (parameter: '{parameter}', value: {value})")
        if suggestion:
            warning.args = (f"{message} (parameter: '{parameter}', value: {value}). Suggestion: {suggestion}",)
        self.add_warning(warning)
    
    def get_summary(self) -> Dict[str, Any]:
        """Get validation summary"""
        return {
            "is_valid": self.is_valid,
            "error_count": len(self.errors),
            "warning_count": len(self.warnings),
            "processed_parameter_count": len(self.processed_values),
            "has_critical_errors": any(
                "required" in str(error).lower() or "invalid" in str(error).lower()
                for error in self.errors
            )
        }
    
    def merge(self, other: 'ValidationResult'):
        """Merge another validation result"""
        self.is_valid = self.is_valid and other.is_valid
        self.errors.extend(other.errors)
        self.warnings.extend(other.warnings)
        self.processed_values.update(other.processed_values)
        self.metadata.update(other.metadata)

class ValidationRule(ABC):
    """Abstract base class for validation rules"""
    
    def __init__(self, parameter_name: str, description: str = ""):
        self.parameter_name = parameter_name
        self.description = description
    
    @abstractmethod
    def validate(self, value: Any, context: Dict[str, Any]) -> Optional[ValidationError]:
        """Validate a value, return error if invalid, None if valid"""
        pass

class TypeValidationRule(ValidationRule):
    """Rule for validating parameter types"""
    
    def __init__(self, parameter_name: str, expected_type: type, 
                 description: str = "", coerce: bool = False):
        super().__init__(parameter_name, description)
        self.expected_type = expected_type
        self.coerce = coerce
    
    def validate(self, value: Any, context: Dict[str, Any]) -> Optional[ValidationError]:
        if value is None:
            return ValidationError(f"Parameter '{self.parameter_name}' cannot be None", self.parameter_name, value)
        
        if not isinstance(value, self.expected_type):
            if self.coerce:
                try:
                    coerced_value = self.expected_type(value)
                    context[self.parameter_name] = coerced_value
                    return None
                except (ValueError, TypeError):
                    return ValidationError(
                        f"Parameter '{self.parameter_name}' must be of type {self.expected_type.__name__}, "
                        f"got {type(value).__name__} and coercion failed",
                        self.parameter_name,
                        value
                    )
            else:
                return ValidationError(
                    f"Parameter '{self.parameter_name}' must be of type {self.expected_type.__name__}, "
                    f"got {type(value).__name__}",
                    self.parameter_name,
                    value
                )
        return None

class RangeValidationRule(ValidationRule):
    """Rule for validating parameter value ranges"""
    
    def __init__(self, parameter_name: str, min_value: Any = None, max_value: Any = None,
                 inclusive: bool = True, description: str = ""):
        super().__init__(parameter_name, description)
        self.min_value = min_value
        self.max_value = max_value
        self.inclusive = inclusive
    
    def validate(self, value: Any, context: Dict[str, Any]) -> Optional[ValidationError]:
        if self.min_value is not None:
            if self.inclusive and value < self.min_value:
                return ValidationError(
                    f"Parameter '{self.parameter_name}' must be >= {self.min_value}, got {value}",
                    self.parameter_name,
                    value,
                    f"Increase to at least {self.min_value}"
                )
            elif not self.inclusive and value <= self.min_value:
                return ValidationError(
                    f"Parameter '{self.parameter_name}' must be > {self.min_value}, got {value}",
                    self.parameter_name,
                    value,
                    f"Increase to greater than {self.min_value}"
                )
        
        if self.max_value is not None:
            if self.inclusive and value > self.max_value:
                return ValidationError(
                    f"Parameter '{self.parameter_name}' must be <= {self.max_value}, got {value}",
                    self.parameter_name,
                    value,
                    f"Decrease to at most {self.max_value}"
                )
            elif not self.inclusive and value >= self.max_value:
                return ValidationError(
                    f"Parameter '{self.parameter_name}' must be < {self.max_value}, got {value}",
                    self.parameter_name,
                    value,
                    f"Decrease to less than {self.max_value}"
                )
        return None

class ChoiceValidationRule(ValidationRule):
    """Rule for validating that a parameter value is one of allowed choices"""
    
    def __init__(self, parameter_name: str, allowed_choices: List[Any],
                 description: str = "", case_sensitive: bool = True):
        super().__init__(parameter_name, description)
        self.allowed_choices = allowed_choices
        self.case_sensitive = case_sensitive
    
    def validate(self, value: Any, context: Dict[str, Any]) -> Optional[ValidationError]:
        if not self.case_sensitive and isinstance(value, str):
            # Check case-insensitive match
            matches = [choice for choice in self.allowed_choices 
                      if isinstance(choice, str) and choice.lower() == value.lower()]
            if matches:
                # Use the original case from allowed choices
                context[self.parameter_name] = matches[0]
                return None
        elif value not in self.allowed_choices:
            choices_str = ", ".join(
                [str(choice) for choice in self.allowed_choices]
            )
            return ValidationError(
                f"Parameter '{self.parameter_name}' must be one of: {choices_str}, got '{value}'",
                self.parameter_name,
                value,
                f"Use one of: {choices_str}"
            )
        return None

class PatternValidationRule(ValidationRule):
    """Rule for validating that a string parameter matches a pattern"""
    
    def __init__(self, parameter_name: str, pattern: str, flags: int = 0,
                 description: str = ""):
        super().__init__(parameter_name, description)
        self.pattern = re.compile(pattern, flags)
    
    def validate(self, value: Any, context: Dict[str, Any]) -> Optional[ValidationError]:
        if not isinstance(value, str):
            return ValidationError(
                f"Parameter '{self.parameter_name}' must be a string, got {type(value).__name__}",
                self.parameter_name,
                value
            )
        
        if not self.pattern.match(value):
            return ValidationError(
                f"Parameter '{self.parameter_name}' must match pattern '{self.pattern.pattern}', got '{value}'",
                self.parameter_name,
                value,
                f"Use a string matching pattern: {self.pattern.pattern}"
            )
        return None

class CrossValidationRule(ValidationRule):
    """Rule for validating that a parameter value satisfies constraints relative to other parameters"""
    
    def __init__(self, parameter_name: str, dependencies: List[str], validation_func: Callable,
                 description: str = ""):
        super().__init__(parameter_name, description)
        self.dependencies = dependencies
        self.validation_func = validation_func
    
    def validate(self, value: Any, context: Dict[str, Any]) -> Optional[ValidationError]:
        # Check that all dependencies are available
        missing_deps = [dep for dep in self.dependencies if dep not in context]
        if missing_deps:
            return ValidationError(
                f"Parameter '{self.parameter_name}' validation requires "
                f"parameters {missing_deps}, which are not present",
                self.parameter_name,
                value
            )
        
        try:
            return self.validation_func(value, {k: context[k] for k in self.dependencies})
        except Exception as e:
            return ValidationError(
                f"Cross-validation failed for parameter '{self.parameter_name}': {str(e)}",
                self.parameter_name,
                value,
                "Check parameter interdependencies"
            )

class ValidationSchema:
    """Schema definition for configuration validation"""
    
    def __init__(self, name: str = "default"):
        self.name = name
        self.rules: List[ValidationRule] = []
        self.parameter_metadata: Dict[str, Dict[str, Any]] = {}
        self.validation_order: List[str] = []
    
    def add_rule(self, rule: ValidationRule):
        """Add a validation rule"""
        self.rules.append(rule)
        if rule.parameter_name not in self.validation_order:
            self.validation_order.append(rule.parameter_name)
    
    def add_parameter(self, name: str, **metadata):
        """Add parameter metadata"""
        self.parameter_metadata[name] = metadata
        if name not in self.validation_order:
            self.validation_order.append(name)
    
    def get_rule(self, parameter_name: str) -> Optional[ValidationRule]:
        """Get validation rule for a parameter"""
        for rule in self.rules:
            if rule.parameter_name == parameter_name:
                return rule
        return None

class ConfigValidator:
    """
    Comprehensive configuration validator for NLM systems.
    
    Provides validation of configuration parameters against schemas,
    with detailed error reporting and recovery capabilities.
    """
    
    # Default configuration schemas
    DEFAULT_SCHEMAS = {
        "brain": {
            "neuron_count": {"type": "int", "min": 1, "max": 1000000, "required": True},
            "region_count": {"type": "int", "min": 1, "max": 100, "required": True},
            "connection_probability": {"type": "float", "min": 0.0, "max": 1.0, "required": True},
            "random_seed": {"type": "int", "min": 0, "max": 2**31 - 1, "required": False},
            "simulation_timestep": {"type": "float", "min": 0.0001, "max": 1.0, "required": True},
        },
        "plasticity": {
            "stdp_enable": {"type": "bool", "required": False},
            "stdp_ltp_weight": {"type": "float", "min": 0.0, "max": 1.0, "required": False},
            "stdp_ltd_weight": {"type": "float", "min": 0.0, "max": 1.0, "required": False},
            "stdp_tau": {"type": "float", "min": 1.0, "max": 100.0, "required": False},
            "hebbian_enable": {"type": "bool", "required": False},
            "hebbian_learning_rate": {"type": "float", "min": 0.0, "max": 1.0, "required": False},
            "structural_enable": {"type": "bool", "required": False},
            "synaptogenesis_rate": {"type": "float", "min": 0.0, "max": 0.1, "required": False},
            "pruning_rate": {"type": "float", "min": 0.0, "max": 0.01, "required": False},
        },
        "neuromodulation": {
            "dopamine_baseline": {"type": "float", "min": -1.0, "max": 1.0, "required": False},
            "curiosity_enable": {"type": "bool", "required": False},
            "novelty_enable": {"type": "bool", "required": False},
            "curiosity_scale": {"type": "float", "min": 0.0, "max": 10.0, "required": False},
            "novelty_scale": {"type": "float", "min": 0.0, "max": 10.0, "required": False},
        },
        "development": {
            "development_enabled": {"type": "bool", "required": False},
            "critical_period_end": {"type": "float", "min": 0.0, "max": 1000.0, "required": False},
            "maturation_end": {"type": "float", "min": 0.0, "max": 1000.0, "required": False},
        },
        "reward": {
            "reward_discount_factor": {"type": "float", "min": 0.0, "max": 1.0, "required": False},
            "reward_scale": {"type": "float", "min": 0.0, "max": 100.0, "required": False},
        },
        "world": {
            "environment_name": {"type": "string", "required": False},
            "width": {"type": "int", "min": 1, "max": 100, "required": False},
            "height": {"type": "int", "min": 1, "max": 100, "required": False},
            "vision_width": {"type": "int", "min": 1, "max": 50, "required": False},
            "vision_height": {"type": "int", "min": 1, "max": 50, "required": False},
        },
        "logging": {
            "log_level": {"type": "string", "choices": ["trace", "debug", "info", "warn", "error", "critical"], "required": False},
            "log_to_file": {"type": "bool", "required": False},
            "log_filename": {"type": "string", "required": False},
            "log_level": {"type": "int", "min": 0, "max":  i},
        }
    }
    
    def __init__(self, schema_name: str = "default"):
        self.schema_name = schema_name
        self.schema = ValidationSchema(schema_name)
        self._setup_default_schema()
        self.logger = logging.getLogger(__name__)
    
    def _setup_default_schema(self):
        """Set up the default validation schema"""
        for category, params in self.DEFAULT_SCHEMAS.items():
            for param_name, metadata in params.items():
                param_type = metadata.get("type")
                required = metadata.get("required", False)
                
                if param_type == "int":
                    rule = TypeValidationRule(
                        f"{category}.{param_name}",
                        int,
                        f"{category}.{param_name} must be an integer"
                    )
                    if "min" in metadata:
                        rule = RangeValidationRule(
                            f"{category}.{param_name}",
                            metadata["min"],
                            metadata.get("max")
                        )
                elif param_type == "float":\n                    rule = TypeValidationRule(
                        f"{category}.{param_name}",
                        float,
                        f"{category}.{param_name} must be a float"
                    )
                    if "min" in metadata:
                        rule = RangeValidationRule(
                            f"{category}.{param_name}",
                            metadata["min"],
                            metadata.get("max")
                        )
                elif param_type == "bool":
                    rule = TypeValidationRule(
                        f"{category}.{param_name}",
                        bool,
                        f"{category}.{param_name} must be a boolean"
                    )
                elif param_type == "string":
                    rule = TypeValidationRule(
                        f"{category}.{param_name}",
                        str,
                        f"{category}.{param_name} must be a string"
                    )
                    if "choices" in metadata:
                        rule = ChoiceValidationRule(
                            f"{category}.{param_name}",
                            metadata["choices"],
                            f"{category}.{param_name} must be one of: {metadata['choices']}"
                        )
                
                self.schema.add_rule(rule)
                self.schema.add_parameter(f"{category}.{param_name}", **metadata)
                
                # Add cross-validation rules
                self._add_cross_validation_rules()
    
    def _add_cross_validation_rules(self):
        """Add cross-validation rules for parameter interdependencies"""
        
        def validate_region_neuron_ratio(value: Any, deps: Dict[str, Any]) -> Optional[ValidationError]:
            neuron_count = deps.get("brain.neuron_count", 0)
            region_count = deps.get("brain.region_count", 1)
            
            if neuron_count > 0 and region_count > 0:
                neurons_per_region = neuron_count / region_count
                if neurons_per_region < 10:
                    return ValidationError(
                        f"Neuron count ({neuron_count}) is too low for region count ({region_count}), "
                        f"need at least {region_count * 10} neurons",
                        "brain.region_count",
                        region_count,
                        f"Reduce region count or increase neuron count"
                    )
            return None
        
        # Add cross-validation rule
        self.schema.add_rule(
            CrossValidationRule(
                "brain.region_count",
                ["brain.neuron_count"],
                validate_region_neuron_ratio,
                "Validate neuron count to region count ratio"
            )
        )
        
        def validate_plasticity_weights(value: Any, deps: Dict[str, Any]) -> Optional[ValidationError]:
            ltp = deps.get("plasticity.stdp_ltp_weight", 0.01)
            ltd = deps.get("plasticity.stdp_ltd_weight", 0.012)
            
            if ltp >= ltd:
                return ValidationError(
                    f"STDP LTP weight ({ltp}) must be less than LTD weight ({ltd}) "
                    "for proper learning dynamics",
                    "plasticity.stdp_ltd_weight",
                    ltd,
                    "Set LTP weight lower than LTD weight"
                )
            return None
        
        self.schema.add_rule(
            CrossValidationRule(
                "plasticity.stdp_ltd_weight",
                ["plasticity.stdp_ltp_weight"],
                validate_plasticity_weights,
                "Validate STDP LTP/LTD weight relationship"
            )
        )
    
    def validate(self, config_data: Dict[str, Any]) -> ValidationResult:
        """
        Validate configuration data against the schema.
        
        Args:
            config_data: Dictionary containing configuration parameters
            
        Returns:
            ValidationResult object with validation results
        """
        result = ValidationResult()
        
        # Process parameters in order
        for param_name in self.schema.validation_order:
            if param_name not in config_data:
                # Check if parameter is required
                if self._is_parameter_required(param_name):
                    error = ValidationError(
                        f"Required parameter '{param_name}' is missing",
                        param_name,
                        None,
                        f"Add '{param_name}' to the configuration"
                    )
                    result.add_error(error)
                
                continue
            
            value = config_data[param_name]
            result.processed_values[param_name] = value
            
            # Validate with schema rule if available
            rule = self.schema.get_rule(param_name)
            if rule:
                error = rule.validate(value, result.processed_values)
                if error:
                    result.add_error(error)
                    continue
            
            # Apply additional validation based on parameter metadata
            metadata = self.schema.parameter_metadata.get(param_name, {})
            error = self._validate_with_metadata(value, param_name, metadata, result.processed_values)
            if error:
                result.add_error(error)
                continue
        
        # Post-processing validation (cross-parameter checks)
        self._post_validate_parameters(result)
        
        return result
    
    def _is_parameter_required(self, param_name: str) -> bool:
        """Check if a parameter is required based on schema"""
        # Check all schemas for this parameter
        for category, params in self.DEFAULT_SCHEMAS.items():
            if param_name in params and params[param_name].get("required", False):
                return True
        return False
    
    def _validate_with_metadata(self, value: Any, param_name: str, 
                               metadata: Dict[str, Any], context: Dict[str, Any]) -> Optional[ValidationError]:
        """Validate value using parameter metadata"""
        # Additional validation based on metadata
        if "validation_func" in metadata:
            try:
                validation_func = metadata["validation_func"]
                error = validation_func(value, context)
                if error:
                    return error
            except Exception as e:
                return ValidationError(
                    f"Custom validation failed for '{param_name}': {str(e)}",
                    param_name,
                    value
                )
        
        return None
    
    def _post_validate_parameters(self, result: ValidationResult):
        """Perform post-validation checks on validated parameters"""
        
        # Check consistency between categories
        if "brain.neuron_count" in result.processed_values and "brain.region_count" in result.processed_values:
            neurons_per_region = result.processed_values["brain.neuron_count"] / result.processed_values["brain.region_count"]
            if neurons_per_region < 5:
                warning = ValidationWarning(
                    f"Low neuron density: {neurons_per_region:.1f} neurons per region may limit expressivity"
                )
                result.add_warning(warning)
        
        # Check plasticity system balance
        if ("plasticity.stdp_ltp_weight" in result.processed_values and 
            "plasticity.stdp_ltd_weight" in result.processed_values):
            ltp = result.processed_values["plasticity.stdp_ltp_weight"]
            ltd = result.processed_values["plasticity.stdp_ltd_weight"]
            if ltp > ltd * 1.1:  # Allow some tolerance for small differences
                warning = ValidationWarning(
                    f"LTP weight ({ltp}) is significantly higher than LTD weight ({ltd}) "
                    "which may create biased learning"
                )
                result.add_warning(warning)
    
    def create_default_config(self) -> Dict[str, Any]:
        """Create a default configuration based on schema"""
        config = {}
        
        for param_name in self.schema.validation_order:
            metadata = self.schema.parameter_metadata.get(param_name, {})
            
            # Generate default value based on metadata
            if "default" in metadata:
                config[param_name] = metadata["default"]
            elif metadata.get("type") == "int":
                config[param_name] = metadata.get("min", 0)
            elif metadata.get("type") == "float":
                config[param_name] = metadata.get("min", 0.0)
            elif metadata.get("type") == "bool":
                config[param_name] = False
            elif metadata.get("type") == "string":
                config[param_name] = ""
        
        return config

class ConfigurationManager:
    """
    Centralized configuration manager for NLM systems.
    
    Manages configuration validation, loading, saving, and provides
    a unified interface for accessing and modifying configuration.
    """
    
    def __init__(self, schema_name: str = "default", validate_on_load: bool = True):
        self.schemas = {
            "default": ConfigValidator("default")
        }
        self.current_schema_name = schema_name
        self.validate_on_load = validate_on_load
        self.config_cache: Dict[str, Any] = {}
        self.logger = logging.getLogger(__name__)
    
    def get_validator(self, schema_name: str = None) -> ConfigValidator:
        """Get validator for a specific schema"""
        name = schema_name or self.current_schema_name
        if name not in self.schemas:
            # Create new schema if not exists
            self.schemas[name] = ConfigValidator(name)
        return self.schemas[name]
    
    def load_config(self, filepath: str, schema_name: str = None) -> Dict[str, Any]:
        """
        Load configuration from a JSON file with validation.
        
        Args:
            filepath: Path to configuration file
            schema_name: Name of schema to use for validation
            
        Returns:
            Loaded configuration dictionary
            
        Raises:
            ValidationError: If configuration validation fails
            IOError: If file cannot be read
        """
        try:
            with open(filepath, 'r') as f:
                config_data = json.load(f)
            
            # Validate configuration
            validator = self.get_validator(schema_name)
            validation_result = validator.validate(config_data)
            
            if validation_result.has_critical_errors:
                for error in validation_result.errors:
                    self.logger.error(f"Configuration validation error: {error}")
            
            if self.validate_on_load and not validation_result.is_valid:
                errors = [str(e) for e in validation_result.errors]
                raise ValidationError(
                    f"Configuration validation failed with {len(errors)} errors",
                    "config_file",
                    filepath,
                    "Check configuration file format and values"
                )
            
            # Cache configuration
            self.config_cache[filepath] = config_data
            
            self.logger.info(f"Configuration loaded successfully from {filepath}")
            return config_data
            
        except (IOError, OSError) as e:
            raise IOError(f"Cannot read configuration file {filepath}: {str(e)}")
        except json.JSONDecodeError as e:
            raise ValidationError(
                f"Invalid JSON in configuration file: {str(e)}",
                "config_file",
                filepath,
                "Ensure the file is valid JSON"
            )
    
    def save_config(self, config_data: Dict[str, Any], filepath: str, 
                    schema_name: str = None, validate: bool = True):
        """
        Save configuration to a JSON file with validation.
        
        Args:
            config_data: Configuration dictionary to save
            filepath: Path to save configuration file
            schema_name: Name of schema to use for validation
            validate: Whether to validate configuration before saving
        """
        try:
            validator = self.get_validator(schema_name)
            
            if validate:
                validation_result = validator.validate(config_data)
                
                if not validation_result.is_valid:
                    errors = [str(e) for e in validation_result.errors]
                    raise ValidationError(
                        f"Configuration validation failed with {len(errors)} errors",
                        "config_save",
                        filepath,
                        "Fix validation errors before saving"
                    )
            
            # Write to file
            os.makedirs(os.path.dirname(os.path.abspath(filepath)), exist_ok=True)
            with open(filepath, 'w') as f:
                json.dump(config_data, f, indent=2)
            
            self.logger.info(f"Configuration saved successfully to {filepath}")
            
        except (IOError, OSError) as e:
            raise IOError(f"Cannot write configuration file {filepath}: {str(e)}")
    
    def get_config_with_validation(self, config_data: Dict[str, Any], 
                                  schema_name: str = None) -> ValidationResult:
        """
        Get validated configuration with validation results.
        
        Args:
            config_data: Configuration dictionary
            schema_name: Name of schema to use for validation
            
        Returns:
            ValidationResult object with validation results
        """
        validator = self.get_validator(schema_name)
        return validator.validate(config_data)
    
    def get_default_config(self, schema_name: str = None) -> Dict[str, Any]:
        """
        Get default configuration for a schema.
        
        Args:
            schema_name: Name of schema to use
            
        Returns:
            Default configuration dictionary
        """
        validator = self.get_validator(schema_name)
        return validator.create_default_config()
    
    def merge_configs(self, base_config: Dict[str, Any], 
                     override_config: Dict[str, Any], 
                     schema_name: str = None) -> Dict[str, Any]:
        """
        Merge two configurations with validation.
        
        Args:
            base_config: Base configuration
            override_config: Configuration to merge over base
            schema_name: Name of schema to use for validation
            
        Returns:
            Merged configuration
        """
        merged = base_config.copy()
        merged.update(override_config)
        
        validator = self.get_validator(schema_name)
        validation_result = validator.validate(merged)
        
        if not validation_result.is_valid:
            for error in validation_result.errors:
                self.logger.warning(f"Merged configuration issue: {error}")
        
        return merged
    
    def create_config_from_args(self, args_dict: Dict[str, Any], 
                               schema_name: str = None) -> Dict[str, Any]:
        """
        Create configuration from command-line arguments or dictionary.
        
        Args:
            args_dict: Dictionary of command-line arguments
            schema_name: Name of schema to use for validation
            
        Returns:
            Configuration dictionary
        """
        # Convert args_dict to config format
        config = {}
        
        # Handle nested parameters (e.g., "brain.neuron_count")
        for key, value in args_dict.items():
            if isinstance(value, dict):
                # Nested configuration
                for sub_key, sub_value in value.items():
                    config[f"{key}.{sub_key}"] = sub_value
            else:
                config[key] = value
        
        validator = self.get_validator(schema_name)
        validation_result = validator.validate(config)
        
        if not validation_result.is_valid:
            for error in validation_result.errors:
                self.logger.warning(f"Argument configuration issue: {error}")
        
        return config
    
    def validate_config_partial(self, config_data: Dict[str, Any], 
                               schema_name: str = None) -> ValidationResult:
        """
        Validate configuration even if it's not complete (warnings instead of errors).
        
        Args:
            config_data: Configuration dictionary
            schema_name: Name of schema to use for validation
            
        Returns:
            ValidationResult object
        """
        validator = self.get_validator(schema_name)
        result = validator.validate(config_data)
        
        # Convert errors to warnings for partial validation
        # (This is a simplified approach - specific validation logic can be added)
        
        return result

# Global configuration manager instance
_config_manager = ConfigurationManager()

# Convenience functions

def load_config(filepath: str, schema_name: str = None) -> Dict[str, Any]:
    """Load configuration from file with global configuration manager"""
    return _config_manager.load_config(filepath, schema_name)

def save_config(config_data: Dict[str, Any], filepath: str, 
               validate: bool = True) -> None:
    """Save configuration to file with global configuration manager"""
    return _config_manager.save_config(config_data, filepath, validate)

def get_default_config(schema_name: str = None) -> Dict[str, Any]:
    """Get default configuration with global configuration manager"""
    return _config_manager.get_default_config(schema_name)

def validate_config(config_data: Dict[str, Any], schema_name: str = None) -> ValidationResult:
    """Validate configuration with global configuration manager"""
    return _config_manager.get_config_with_validation(config_data, schema_name)

# Initialize logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)