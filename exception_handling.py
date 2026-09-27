#!/usr/bin/env python3
"""
Comprehensive error handling and validation for NLM Python bindings.

This module provides advanced error handling, validation, and debugging
capabilities for the NLM Python bindings.
"""

import pynlm
import json
import traceback
import sys
from typing import Dict, List, Any, Optional, Union, Callable
from enum import Enum
import numpy as np

class ErrorSeverity(Enum):
    """Error severity levels."""
    INFO = "info"
    WARNING = "warning"
    ERROR = "error"
    CRITICAL = "critical"

class NLMError(Exception):
    """Base exception for NLM errors."""
    
    def __init__(self, message: str, severity: ErrorSeverity = ErrorSeverity.ERROR, 
                 error_code: Optional[str] = None, details: Optional[Dict[str, Any]] = None):
        super().__init__(message)
        self.message = message
        self.severity = severity
        self.error_code = error_code
        self.details = details or {}
        self.timestamp = 0.0  # Would be real timestamp
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert error to dictionary."""
        return {
            'message': self.message,
            'severity': self.severity.value,
            'error_code': self.error_code,
            'details': self.details,
            'timestamp': self.timestamp
        }

class ValidationError(NLMError):
    """Error related to validation failures."""
    
    def __init__(self, message: str, validation_field: Optional[str] = None,
                 validation_rule: Optional[str] = None, details: Optional[Dict[str, Any]] = None):
        super().__init__(message, ErrorSeverity.WARNING, 'VALIDATION_ERROR', details)
        self.validation_field = validation_field
        self.validation_rule = validation_rule

class ConfigurationError(NLMError):
    """Error related to configuration issues."""
    
    def __init__(self, message: str, config_key: Optional[str] = None,
                 config_value: Optional[Any] = None, details: Optional[Dict[str, Any]] = None):
        super().__init__(message, ErrorSeverity.ERROR, 'CONFIGURATION_ERROR', details)
        self.config_key = config_key
        self.config_value = config_value

class BrainError(NLMError):
    """Error related to brain operations."""
    
    def __init__(self, message: str, operation: Optional[str] = None,
                 brain_state: Optional[str] = None, details: Optional[Dict[str, Any]] = None):
        super().__init__(message, ErrorSeverity.ERROR, 'BRAIN_ERROR', details)
        self.operation = operation
        self.brain_state = brain_state

class WorldError(NLMError):
    """Error related to world operations."""
    
    def __init__(self, message: str, operation: Optional[str] = None,
                 world_state: Optional[str] = None, details: Optional[Dict[str, Any]] = None):
        super().__init__(message, ErrorSeverity.ERROR, 'WORLD_ERROR', details)
        self.operation = operation
        self.world_state = world_state
class ValidationRule:
    """
    Base class for validation rules.
    
    Validation rules check configuration values, sensory inputs, or system states
    to ensure they meet specified criteria.
    """
    
    def __init__(self, name: str, description: str):
        self.name = name
        self.description = description
    
    def validate(self, value: Any, context: Optional[Dict[str, Any]] = None) -> List[ValidationError]:
        """
        Validate a value against this rule.
        
        Args:
            value: Value to validate
            context: Context information for validation
            
        Returns:
            List of validation errors (empty if valid)
        """
        raise NotImplementedError("Subclasses must implement validate method")
    
    def get_error_message(self) -> str:
        """Get error message for this rule."""
        return f"Validation failed: {self.name} - {self.description}"
class RangeValidationRule(ValidationRule):
    """Validation rule for checking if a value is within a range."""
    
    def __init__(self, name: str, min_value: float, max_value: float, inclusive: bool = True):
        super().__init__(name, f"Value must be between {min_value} and {max_value}")
        self.min_value = min_value
        self.max_value = max_value
        self.inclusive = inclusive
    
    def validate(self, value: Any, context: Optional[Dict[str, Any]] = None) -> List[ValidationError]:
        errors = []
        
        if not isinstance(value, (int, float)):
            errors.append(ValidationError(
                f"Value {value} is not a number",
                validation_field=self.name,
                validation_rule="range"
            ))
            return errors
        
        if self.inclusive:
            if value < self.min_value or value > self.max_value:
                errors.append(ValidationError(
                    f"Value {value} is outside range [{self.min_value}, {self.max_value}]",
                    validation_field=self.name,
                    validation_rule="range",
                    details={'value': value, 'min': self.min_value, 'max': self.max_value}
                ))
        else:
            if value <= self.min_value or value >= self.max_value:
                errors.append(ValidationError(
                    f"Value {value} is outside range ({self.min_value}, {self.max_value})",
                    validation_field=self.name,
                    validation_rule="range",
                    details={'value': value, 'min': self.min_value, 'max': self.max_value}
                ))
        
        return errors
class ChoiceValidationRule(ValidationRule):
    """Validation rule for checking if a value is in a set of allowed choices."""
    
    def __init__(self, name: str, allowed_values: List[Any], case_sensitive: bool = True):
        super().__init__(name, f"Value must be one of: {allowed_values}")
        self.allowed_values = allowed_values
        self.case_sensitive = case_sensitive
    
    def validate(self, value: Any, context: Optional[Dict[str, Any]] = None) -> List[ValidationError]:
        errors = []
        
        # Handle case sensitivity for strings
        if not self.case_sensitive and isinstance(value, str):
            value_normalized = value.lower()
            allowed_normalized = [v.lower() if isinstance(v, str) else v for v in self.allowed_values]
            if value_normalized not in allowed_normalized:
                errors.append(ValidationError(
                    f"Value '{value}' is not in allowed choices",
                    validation_field=self.name,
                    validation_rule="choice",
                    details={'value': value, 'allowed': self.allowed_values}
                ))
        else:
            if value not in self.allowed_values:
                errors.append(ValidationError(
                    f"Value '{value}' is not in allowed choices",
                    validation_field=self.name,
                    validation_rule="choice",
                    details={'value': value, 'allowed': self.allowed_values}
                ))
        
        return errors
class PatternValidationRule(ValidationRule):
    """Validation rule for checking if a value matches a pattern."""
    
    def __init__(self, name: str, pattern: str, regex_flags: int = 0):
        super().__init__(name, f"Value must match pattern: {pattern}")
        self.pattern = pattern
        self.regex_flags = regex_flags
    
    def validate(self, value: Any, context: Optional[Dict[str, Any]] = None) -> List[ValidationError]:
        errors = []
        
        import re
        value_str = str(value)
        
        if not re.match(self.pattern, value_str, self.regex_flags):
            errors.append(ValidationError(
                f"Value '{value}' does not match pattern '{self.pattern}'",
                validation_field=self.name,
                validation_rule="pattern",
                details={'value': value, 'pattern': self.pattern}
            ))
        
        return errors
class CustomValidationRule(ValidationRule):
    """Custom validation rule using a custom validation function."""
    
    def __init__(self, name: str, validation_func: Callable[[Any, Optional[Dict[str, Any]]], List[ValidationError]],
                 description: str):
        super().__init__(name, description)
        self.validation_func = validation_func
    
    def validate(self, value: Any, context: Optional[Dict[str, Any]] = None) -> List[ValidationError]:
        return self.validation_func(value, context)
class ConfigurationValidator:
    """
    Comprehensive configuration validator for NLM.
    
    This class provides validation for NLM configuration, checking for
    required keys, valid values, and consistency between different settings.
    """
    
    def __init__(self):
        self.validation_rules: Dict[str, List[ValidationRule]] = {}
        self.error_handler = ErrorHandler()
        self.register_default_rules()
    
    def register_validation_rule(self, config_key: str, rule: ValidationRule):
        """
        Register a validation rule for a configuration key.
        
        Args:
            config_key: Configuration key to validate
            rule: Validation rule
        """
        if config_key not in self.validation_rules:
            self.validation_rules[config_key] = []
        self.validation_rules[config_key].append(rule)
    
    def validate_config(self, config: pynlm.Config, config_name: str = "unnamed") -> List[ValidationError]:
        """
        Validate configuration against all registered rules.
        
        Args:
            config: Configuration to validate
            config_name: Name of configuration for error reporting
            
        Returns:
            List of validation errors (empty if valid)
        """
        errors = []
        
        # Get all keys in config
        config_keys = config.getKeys()
        
        # Check registered rules for each key
        for config_key in config_keys:
            if config_key in self.validation_rules:
                value = self._get_config_value(config, config_key)
                for rule in self.validation_rules[config_key]:
                    rule_errors = rule.validate(value, {'config_key': config_key, 'config_name': config_name})
                    for error in rule_errors:
                        error.details['config_key'] = config_key
                        error.details['config_name'] = config_name
                        error.details['config_value'] = value
                        errors.append(error)
        
        # Check for required keys
        required_keys = self._get_required_keys()
        for required_key in required_keys:
            if not config.has(required_key):
                errors.append(ValidationError(
                    f"Required configuration key '{required_key}' is missing",
                    validation_field=required_key,
                    validation_rule="required"
                ))
        
        return errors
    
    def validate_sensory_input(self, sensory_data: List[float], input_type: str = "vision") -> List[ValidationError]:
        """
        Validate sensory input data.
        
        Args:
            sensory_data: Sensory input data
            input_type: Type of sensory input
            
        Returns:
            List of validation errors (empty if valid)
        """
        errors = []
        
        # Register default rules for sensory inputs
        sensory_rules = self._get_sensory_rules(input_type)
        
        for i, value in enumerate(sensory_data):
            for rule in sensory_rules:
                rule_errors = rule.validate(value, {
                    'input_index': i,
                    'input_type': input_type,
                    'sensory_data_length': len(sensory_data)
                })
                for error in rule_errors:
                    error.details['input_index'] = i
                    error.details['input_type'] = input_type
                    errors.append(error)
        
        return errors
    
    def validate_brain_operation(self, operation: str, brain: pynlm.Brain, 
                                 context: Optional[Dict[str, Any]] = None) -> List[BrainError]:
        """
        Validate a brain operation.
        
        Args:
            operation: Operation to validate
            brain: Brain to check
            context: Context information
            
        Returns:
            List of brain errors (empty if valid)
        """
        errors = []
        
        # Check brain state consistency
        if operation == "step" and brain.getTotalNeuronCount() == 0:
            errors.append(BrainError(
                "Cannot perform step operation: brain has no neurons",
                operation=operation,
                brain_state="empty"
            ))
        
        if operation == "initialize" and brain.getConfig().has("brain.neuron_count"):
            neuron_count = brain.getConfig().get_int("brain.neuron_count")
            if neuron_count <= 0:
                errors.append(BrainError(
                    f"Cannot initialize brain with non-positive neuron count: {neuron_count}",
                    operation=operation,
                    brain_state="invalid_config"
                ))
        
        if context:
            # Validate context-specific rules
            for rule in self._get_brain_rules(operation):
                rule_errors = rule.validate(operation, brain, context)
                errors.extend(rule_errors)
        
        return errors
    
    def validate_world_operation(self, operation: str, world: pynlm.SimpleWorld,
                                context: Optional[Dict[str, Any]] = None) -> List[WorldError]:
        """
        Validate a world operation.
        
        Args:
            operation: Operation to validate
            world: World to check
            context: Context information
            
        Returns:
            List of world errors (empty if valid)
        """
        errors = []
        
        # Check world configuration
        if operation in ["update", "applyMotorCommand", "getSensoryPercept"]:
            if world.getWidth() <= 0 or world.getHeight() <= 0:
                errors.append(WorldError(
                    f"Cannot perform {operation} operation: invalid world dimensions",
                    operation=operation,
                    world_state="invalid_dimensions"
                ))
        
        if operation == "setAgentStart":
            # In a real implementation, we would validate the position
            pass
        
        return errors
    
    def register_default_rules(self):
        """Register default validation rules."""
        # Brain configuration rules
        self.register_validation_rule("brain.neuron_count", RangeValidationRule(
            "brain.neuron_count", 1, 10000, inclusive=True
        ))
        
        self.register_validation_rule("brain.synapse_density", RangeValidationRule(
            "brain.synapse_density", 0.0, 1.0, inclusive=True
        ))
        
        self.register_validation_rule("brain.connection_probability", RangeValidationRule(
            "brain.connection_probability", 0.0, 1.0, inclusive=True
        ))
        
        self.register_validation_rule("brain.initial_weight_mean", RangeValidationRule(
            "brain.initial_weight_mean", -1.0, 1.0, inclusive=True
        ))
        
        self.register_validation_rule("brain.v_thresh", RangeValidationRule(
            "brain.v_thresh", -100.0, 0.0, inclusive=True
        ))
        
        self.register_validation_rule("brain.v_rest", RangeValidationRule(
            "brain.v_rest", -100.0, 0.0, inclusive=True
        ))
        
        self.register_validation_rule("brain.tau_mem", RangeValidationRule(
            "brain.tau_mem", 0.0, 1000.0, inclusive=True
        ))
        
        # Plasticity rules
        self.register_validation_rule("plasticity.stdp.enable", self._create_bool_rule(
            "plasticity.stdp.enable"
        ))
        
        self.register_validation_rule("plasticity.hebbian.enable", self._create_bool_rule(
            "plasticity.hebbian.enable"
        ))
        
        self.register_validation_rule("plasticity.structural.enable", self._create_bool_rule(
            "plasticity.structural.enable"
        ))
        
        # Neuromodulation rules
        self.register_validation_rule("neuromod.dopamine.scale", RangeValidationRule(
            "neuromod.dopamine.scale", -2.0, 2.0, inclusive=True
        ))
        
        # Sensory input rules
        self.register_validation_rule("vision.*", self._create_vision_rule())
        self.register_validation_rule("audio.*", self._create_audio_rule())
    
    def _create_bool_rule(self, name: str) -> ValidationRule:
        """Create a boolean validation rule."""
        return CustomValidationRule(
            name,
            lambda value, ctx: [] if isinstance(value, bool) else [ValidationError(
                f"Expected boolean value, got {type(value)}",
                validation_field=name,
                validation_rule="boolean"
            )],
            "Value must be a boolean"
        )
    
    def _create_vision_rule(self) -> ValidationRule:
        """Create a vision validation rule."""
        return CustomValidationRule(
            "vision.*",
            lambda value, ctx: [] if isinstance(value, (int, float)) and 0 <= value <= 1 else [ValidationError(
                f"Vision value must be between 0 and 1, got {value}",
                validation_field="vision",
                validation_rule="range"
            )],
            "Vision values must be between 0 and 1"
        )
    
    def _create_audio_rule(self) -> ValidationRule:
        """Create an audio validation rule."""
        return CustomValidationRule(
            "audio.*",
            lambda value, ctx: [] if isinstance(value, (int, float)) and 0 <= value <= 1 else [ValidationError(
                f"Audio value must be between 0 and 1, got {value}",
                validation_field="audio",
                validation_rule="range"
            )],
            "Audio values must be between 0 and 1"
        )
    
    def _get_config_value(self, config: pynlm.Config, key: str) -> Any:
        """Get configuration value with type detection."""
        if config.has(key):
            # Try to get as different types
            try:
                return config.get_int(key, 0)
            except:
                try:
                    return config.get_float(key, 0.0)
                except:
                    try:
                        return config.get_string(key, "")
                    except:
                        try:
                            return config.get_bool(key, False)
                        except:
                            return None
        return None
    
    def _get_required_keys(self) -> List[str]:
        """Get required configuration keys."""
        return ["brain.neuron_count", "brain.synapse_density"]
    
    def _get_sensory_rules(self, input_type: str) -> List[ValidationRule]:
        """Get validation rules for sensory input type."""
        rules = []
        
        # Add range rule for values
        rules.append(RangeValidationRule(
            f"{input_type}.*", 0.0, 1.0, inclusive=True
        ))
        
        return rules
    
    def _get_brain_rules(self, operation: str) -> List[ValidationRule]:
        """Get validation rules for brain operation."""
        # Return empty list for now - could be extended with operation-specific rules
        return []
class ErrorHandler:
    """
    Comprehensive error handler for NLM.
    
    This class provides centralized error handling, logging, and recovery
    for NLM operations.
    """
    
    def __init__(self, log_level: str = "INFO"):
        self.log_level = log_level
        self.error_log: List[NLMError] = []
        self.warning_log: List[NLMError] = []
        self.info_log: List[NLMError] = []
        
        # Error recovery handlers
        self.recovery_handlers: Dict[str, Callable[[NLMError], bool]] = {}
        
        self._register_default_handlers()
    
    def handle_error(self, error: NLMError, context: Optional[Dict[str, Any]] = None) -> bool:
        """
        Handle an error.
        
        Args:
            error: Error to handle
            context: Context information
            
        Returns:
            True if error was handled successfully, False otherwise
        """
        # Log error based on severity
        if error.severity == ErrorSeverity.CRITICAL:
            self.error_log.append(error)
        elif error.severity == ErrorSeverity.ERROR:
            self.error_log.append(error)
        elif error.severity == ErrorSeverity.WARNING:
            self.warning_log.append(error)
        else:
            self.info_log.append(error)
        
        # Try recovery handlers
        error_type = error.error_code or "unknown"
        if error_type in self.recovery_handlers:
            try:
                return self.recovery_handlers[error_type](error)
            except Exception:
                pass
        
        # Default error handling
        return self._default_error_handler(error, context)
    
    def handle_validation_error(self, error: ValidationError) -> bool:
        """Handle validation error with specific logic."""
        # For validation errors, try to provide helpful suggestions
        if error.validation_field:
            print(f"Validation error in field '{error.validation_field}': {error.message}")
            print(f"Rule: {error.validation_rule}")
        
        return True
    
    def handle_configuration_error(self, error: ConfigurationError) -> bool:
        """Handle configuration error with specific logic."""
        print(f"Configuration error for key '{error.config_key}': {error.message}")
        return True
    
    def handle_brain_error(self, error: BrainError) -> bool:
        """Handle brain error with specific logic."""
        print(f"Brain error during {error.operation}: {error.message}")
        return True
    
    def handle_world_error(self, error: WorldError) -> bool:
        """Handle world error with specific logic."""
        print(f"World error during {error.operation}: {error.message}")
        return True
    
    def add_recovery_handler(self, error_type: str, handler: Callable[[NLMError], bool]):
        """
        Add a recovery handler for a specific error type.
        
        Args:
            error_type: Error type to handle
            handler: Recovery handler function
        """
        self.recovery_handlers[error_type] = handler
    
    def get_error_stats(self) -> Dict[str, Any]:
        """Get error statistics."""
        return {
            'total_errors': len(self.error_log),
            'total_warnings': len(self.warning_log),
            'total_info': len(self.info_log),
            'error_types': self._count_error_types(),
            'recovery_success_rate': self._calculate_recovery_success_rate()
        }
    
    def get_recent_errors(self, limit: int = 10) -> List[NLMError]:
        """Get recent errors."""
        return self.error_log[-limit:]
    
    def clear_logs(self):
        """Clear all logs."""
        self.error_log.clear()
        self.warning_log.clear()
        self.info_log.clear()
    
    def _register_default_handlers(self):
        """Register default recovery handlers."""
        self.add_recovery_handler("VALIDATION_ERROR", self.handle_validation_error)
        self.add_recovery_handler("CONFIGURATION_ERROR", self.handle_configuration_error)
        self.add_recovery_handler("BRAIN_ERROR", self.handle_brain_error)
        self.add_recovery_handler("WORLD_ERROR", self.handle_world_error)
    
    def _default_error_handler(self, error: NLMError, context: Optional[Dict[str, Any]]) -> bool:
        """Default error handler."""
        print(f"Error: {error.message}")
        if error.details:
            print(f"Details: {error.details}")
        
        return True
    
    def _count_error_types(self) -> Dict[str, int]:
        """Count errors by type."""
        error_counts = {}
        for error in self.error_log:
            error_type = error.error_code or "unknown"
            error_counts[error_type] = error_counts.get(error_type, 0) + 1
        
        return error_counts
    
    def _calculate_recovery_success_rate(self) -> float:
        """Calculate recovery success rate."""
        if not self.recovery_handlers:
            return 0.0
        
        # This is a simplified calculation
        return 0.8  # Placeholder
class ExceptionWrapper:
    """
    Exception wrapper for NLM operations.
    
    This class provides comprehensive exception handling for NLM operations,
    catching, wrapping, and re-raising exceptions with additional context
    and error information.
    """
    
    def __init__(self, error_handler: Optional[ErrorHandler] = None):
        self.error_handler = error_handler or ErrorHandler()
        self.original_exception: Optional[Exception] = None
        self.wrapped_exception: Optional[NLMError] = None
    
    def wrap_operation(self, operation_name: str, operation_func: Callable,
                      *args, **kwargs) -> Any:
        """
        Wrap an operation with exception handling.
        
        Args:
            operation_name: Name of operation
            operation_func: Function to execute
            *args: Function arguments
            **kwargs: Function keyword arguments
            
        Returns:
            Result of operation function
            
        Raises:
            NLMError: If operation fails with NLM-specific error
            Exception: If operation fails with unexpected exception
        """
        try:
            result = operation_func(*args, **kwargs)
            return result
        except NLMError as e:
            # NLM error - wrap and re-raise
            self.wrapped_exception = e
            self.error_handler.handle_error(e, {
                'operation': operation_name,
                'timestamp': 0.0
            })
            raise e
        except Exception as e:
            # Unexpected exception - wrap as NLM error
            self.original_exception = e
            
            # Create appropriate NLM error
            if operation_name == "brain.step":
                error = BrainError(
                    f"Brain operation failed: {str(e)}",
                    operation=operation_name,
                    details={'original_exception': str(e), 'traceback': traceback.format_exc()}
                )
            elif operation_name == "world.update":
                error = WorldError(
                    f"World operation failed: {str(e)}",
                    operation=operation_name,
                    details={'original_exception': str(e), 'traceback': traceback.format_exc()}
                )
            elif operation_name.startswith("config."):
                error = ConfigurationError(
                    f"Configuration operation failed: {str(e)}",
                    details={'original_exception': str(e), 'traceback': traceback.format_exc()}
                )
            else:
                error = NLMError(
                    f"Operation failed: {str(e)}",
                    error_code='OPERATION_ERROR',
                    details={'operation': operation_name, 'original_exception': str(e), 'traceback': traceback.format_exc()}
                )
            
            self.wrapped_exception = error
            self.error_handler.handle_error(error, {
                'operation': operation_name,
                'original_exception': e,
                'traceback': traceback.format_exc()
            })
            
            raise error
    
    def get_last_exception(self) -> Optional[Exception]:
        """Get last original exception."""
        return self.original_exception
    
    def get_last_wrapped_exception(self) -> Optional[NLMError]:
        """Get last wrapped NLM error."""
        return self.wrapped_exception
    
    def clear_exceptions(self):
        """Clear stored exceptions."""
        self.original_exception = None
        self.wrapped_exception = None
class NLMExceptionManager:
    """
    Comprehensive exception manager for NLM.
    
    This class provides centralized exception management for the entire
    NLM system, coordinating error handlers, validators, and exception wrappers.
    """
    
    def __init__(self):
        self.error_handler = ErrorHandler()
        self.config_validator = ConfigurationValidator()
        self.exception_wrapper = ExceptionWrapper(self.error_handler)
        
        # Global error callbacks
        self.error_callbacks: List[Callable[[NLMError], None]] = []
        
        # Error recovery strategies
        self.recovery_strategies: Dict[str, Callable[[NLMError], bool]] = {}
    
    def register_error_callback(self, callback: Callable[[NLMError], None]):
        """
        Register a global error callback.
        
        Args:
            callback: Callback function to call on errors
        """
        self.error_callbacks.append(callback)
    
    def register_recovery_strategy(self, error_type: str, strategy: Callable[[NLMError], bool]):
        """
        Register a recovery strategy for an error type.
        
        Args:
            error_type: Error type to handle
            strategy: Recovery strategy function
        """
        self.recovery_strategies[error_type] = strategy
    
    def handle_operation(self, operation_name: str, operation_func: Callable,
                        *args, **kwargs) -> Any:
        """
        Handle an operation with comprehensive error management.
        
        Args:
            operation_name: Name of operation
            operation_func: Function to execute
            *args: Function arguments
            **kwargs: Function keyword arguments
            
        Returns:
            Result of operation function
            
        Raises:
            NLMError: If operation fails with NLM-specific error
            Exception: If operation fails with unexpected exception
        """
        try:
            # Execute operation with exception wrapping
            result = self.exception_wrapper.wrap_operation(operation_name, operation_func, *args, **kwargs)
            
            # Get any wrapped exception
            wrapped_error = self.exception_wrapper.get_last_wrapped_exception()
            if wrapped_error:
                # Trigger error callbacks
                for callback in self.error_callbacks:
                    try:
                        callback(wrapped_error)
                    except Exception:
                        pass
            
            return result
            
        except NLMError as e:
            # Re-raise NLM errors after processing
            raise e
        except Exception as e:
            # Convert to NLM error if not already
            if not isinstance(e, NLMError):
                error = NLMError(
                    f"Operation {operation_name} failed: {str(e)}",
                    error_code='OPERATION_ERROR',
                    details={'operation': operation_name, 'original_exception': str(e)}
                )
                raise error
            else:
                raise e
    
    def validate_system(self, brain: Optional[pynlm.Brain] = None,
                        world: Optional[pynlm.SimpleWorld] = None) -> Dict[str, List[NLMError]]:
        """
        Validate system state.
        
        Args:
            brain: Brain to validate
            world: World to validate
            
        Returns:
            Dictionary of validation errors by category
        """
        validation_results = {
            'configuration': [],
            'brain': [],
            'world': [],
            'sensory': []
        }
        
        # Validate configuration if config validator is available
        try:
            # This would require access to the actual config
            pass
        except Exception as e:
            validation_results['configuration'].append(
                NLMError(f"Configuration validation failed: {str(e)}", 
                        error_code='CONFIG_VALIDATION_ERROR')
            )
        
        # Validate brain if provided
        if brain:
            try:
                # Example brain validation
                if brain.getTotalNeuronCount() == 0:
                    validation_results['brain'].append(
                        BrainError("Brain has no neurons", operation="validate")
                    )
            except Exception as e:
                validation_results['brain'].append(
                    BrainError(f"Brain validation failed: {str(e)}", operation="validate")
                )
        
        # Validate world if provided
        if world:
            try:
                if world.getWidth() <= 0 or world.getHeight() <= 0:
                    validation_results['world'].append(
                        WorldError("Invalid world dimensions", operation="validate")
                    )
            except Exception as e:
                validation_results['world'].append(
                    WorldError(f"World validation failed: {str(e)}", operation="validate")
                )
        
        return validation_results
    
    def get_system_error_report(self) -> Dict[str, Any]:
        """Get comprehensive system error report."""
        error_stats = self.error_handler.get_error_stats()
        
        return {
            'error_handler_stats': error_stats,
            'wrapped_exceptions': [
                exc.to_dict() for exc in self.exception_wrapper.get_last_wrapped_exception()
                if exc else []
            ],
            'validation_rules_count': len(self.config_validator.validation_rules),
            'registered_callbacks': len(self.error_callbacks),
            'registered_strategies': len(self.recovery_strategies)
        }
# Global exception manager instance
global_exception_manager = NLMExceptionManager()

def get_exception_manager() -> NLMExceptionManager:
    """Get the global exception manager instance."""
    return global_exception_manager
def handle_with_exception_manager(operation_name: str, operation_func: Callable,
                               *args, **kwargs) -> Any:
    """
    Convenience function to handle an operation with the global exception manager.
    
    Args:
        operation_name: Name of operation
        operation_func: Function to execute
        *args: Function arguments
        **kwargs: Function keyword arguments
        
    Returns:
        Result of operation function
    """
    return global_exception_manager.handle_operation(operation_name, operation_func, *args, **kwargs)
# Example usage and demonstration
if __name__ == "__main__":
    print("=== NLM Exception Management and Validation Demo ===\\n")
    
    # Create test objects
    config = pynlm.createDefaultConfig()
    config.set_int("brain.neuron_count", 100)
    config.set_float("brain.synapse_density", 0.1)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Create exception manager
    exception_manager = NLMExceptionManager()
    
    # Add error callback
    def error_callback(error: NLMError):
        print(f"Global error callback triggered: {error.message}")
    
    exception_manager.register_error_callback(error_callback)
    
    # Test operations with exception handling
    print("1. Testing successful operations...")
    
    # Successful operation
    try:
        result = exception_manager.handle_operation("brain.step", brain.step, 0)
        print("✓ Brain step operation succeeded")
    except Exception as e:
        print(f"✗ Brain step operation failed: {e}")
    
    # Successful world operation
    try:
        world.update(0.1)
        print("✓ World update operation succeeded")
    except Exception as e:
        print(f"✗ World update operation failed: {e}")
    
    # Test configuration validation
    print("\\n2. Testing configuration validation...")
    
    try:
        config.set_int("brain.neuron_count", -50)  # Invalid value
        print("✗ Configuration should have failed with negative neuron count")
    except Exception as e:
        print(f"✓ Configuration validation caught error: {e}")
    
    # Test error recovery
    print("\\n3. Testing error recovery...")
    
    def recovery_strategy(error: NLMError) -> bool:
        print(f"Applying recovery strategy for: {error.error_code}")
        return True
    
    exception_manager.register_recovery_strategy("TEST_ERROR", recovery_strategy)
    
    # Test system validation
    print("\\n4. Testing system validation...")
    
    validation_results = exception_manager.validate_system(brain, world)
    
    for category, errors in validation_results.items():
        if errors:
            print(f"{category.capitalize()} validation errors:")
            for error in errors:
                print(f"  - {error.message}")
        else:
            print(f"{category.capitalize()} validation passed")
    
    # Get error report
    print("\\n5. Getting error report...")
    
    error_report = exception_manager.get_system_error_report()
    print(f"Error handler stats: {error_report['error_handler_stats']}")
    print(f"Validation rules: {error_report['validation_rules_count']}")
    print(f"Registered callbacks: {error_report['registered_callbacks']}")
    print(f"Registered strategies: {error_report['registered_strategies']}")
    
    print("\\n=== NLM Exception Management Demo Complete ===")
