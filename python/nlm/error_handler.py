#!/usr/bin/env python3
"""
Enhanced Error Handling and Type Safety for NLM Python Bindings

This module provides a comprehensive error handling and type safety framework
for the Enhanced NLM Python bindings. It includes:

1. Custom Exception Hierarchy
2. Type Safety Features
3. Error Handling Strategies
4. Validation Framework
5. Defensive Programming
6. Python Integration

This is a unified implementation that can be integrated into all NLM modules.
"""

import sys
import os
import json
import time
import logging
import threading
import itertools
import math
import csv
import warnings
import inspect
import traceback
import functools
import copy
from pathlib import Path
from datetime import datetime
from typing import (
    List, Dict, Tuple, Optional, Any, Union, Callable, Iterator, Generator,
    Type, get_type_hints, get_origin, get_args, TypeVar, Generic, Set,
    cast, overload, Protocol
)
from abc import ABC, abstractmethod
from enum import Enum
from dataclasses import dataclass, field, is_dataclass
from collections.abc import Sequence, Mapping, Iterable
from contextlib import contextmanager

# Type variables for generic typing
T = TypeVar('T')
K = TypeVar('K')
V = TypeVar('V')

# =============================================================================
# CUSTOM EXCEPTION HIERARCHY
# =============================================================================

class NLMException(Exception):
    """Base exception for all NLM errors.
    
    This is the base exception class for all custom NLM exceptions.
    It provides detailed error information including error codes,
    contextual details, and timestamps for debugging.
    """
    
    def __init__(self, message: str, error_code: Optional[str] = None,
                 details: Optional[Dict[str, Any]] = None):
        self.message = message
        self.error_code = error_code
        self.details = details or {}
        self.timestamp = datetime.now()
        super().__init__(message)
    
    def __str__(self) -> str:
        base_msg = self.message
        if self.error_code:
            base_msg = f"[{self.error_code}] {base_msg}"
        if self.details:
            details_str = ", ".join(f"{k}={v}" for k, v in self.details.items())
            base_msg = f"{base_msg} (details: {details_str})"
        return base_msg
    
    def __repr__(self) -> str:
        return f"{self.__class__.__name__}('{self.message}')"

class NLMConfigError(NLMException):
    """Configuration-related errors."""
    pass

class NLMBrainError(NLMException):
    """Brain initialization/processing errors."""
    pass

class NLMWorldError(NLMException):
    """World simulation errors."""
    pass

class NLMMemoryError(NLMException):
    """Memory management errors."""
    pass

class NLMPrecisionError(NLMException):
    """Numerical precision errors."""
    pass

class NLMFileError(NLMException):
    """File I/O errors."""
    pass

class NLMValidationError(NLMException):
    """Input validation errors."""
    pass

class NLMResourceError(NLMException):
    """Resource management errors."""
    pass

class NLMSimulationError(NLMException):
    """Simulation execution errors."""
    pass

# =============================================================================
# TYPE SAFETY AND VALIDATION
# =============================================================================

class TypeValidator:
    """Comprehensive type validation utilities."""
    
    @staticmethod
    def check_type(value: Any, expected_type: Type[T], param_name: str) -> T:
        """Check if value is of expected type."""
        if not isinstance(value, expected_type):
            actual_type = type(value).__name__
            expected_type_name = expected_type.__name__
            raise NLMValidationError(
                f"Parameter '{param_name}' must be {expected_type_name}, "
                f"got {actual_type}",
                error_code="INVALID_TYPE",
                details={
                    "parameter": param_name,
                    "expected_type": expected_type_name,
                    "actual_type": actual_type,
                    "value": str(value)[:100]
                }
            )
        return value
    
    @staticmethod
    def check_optional_type(value: Optional[T], expected_type: Type[T],
                           param_name: str) -> Optional[T]:
        """Check if optional value is of expected type (or None)."""
        if value is not None:
            TypeValidator.check_type(value, expected_type, param_name)
        return value
    
    @staticmethod
    def check_list_type(value: List[Any], element_type: Type[T],
                       param_name: str) -> List[T]:
        """Check if list contains elements of expected type."""
        if not isinstance(value, list):
            raise NLMValidationError(
                f"Parameter '{param_name}' must be a list, "
                f"got {type(value).__name__}",
                error_code="INVALID_TYPE",
                details={
                    "parameter": param_name,
                    "expected_type": "list",
                    "actual_type": type(value).__name__
                }
            )
        
        for i, element in enumerate(value):
            if not isinstance(element, element_type):
                raise NLMValidationError(
                    f"List element {i} in parameter '{param_name}' must be "
                    f"{element_type.__name__}, got {type(element).__name__}",
                    error_code="INVALID_ELEMENT_TYPE",
                    details={
                        "parameter": param_name,
                        "index": i,
                        "expected_type": element_type.__name__,
                        "actual_type": type(element).__name__
                    }
                )
        
        return value
    
    @staticmethod
    def check_dict_type(value: Dict[K, V], key_type: Type[K],
                       value_type: Type[V], param_name: str) -> Dict[K, V]:
        """Check if dict has keys and values of expected types."""
        if not isinstance(value, dict):
            raise NLMValidationError(
                f"Parameter '{param_name}' must be a dict, "
                f"got {type(value).__name__}",
                error_code="INVALID_TYPE",
                details={
                    "parameter": param_name,
                    "expected_type": "dict",
                    "actual_type": type(value).__name__
                }
            )
        
        for key, val in value.items():
            if not isinstance(key, key_type):
                raise NLMValidationError(
                    f"Dict key '{key}' in parameter '{param_name}' must be "
                    f"{key_type.__name__}, got {type(key).__name__}",
                    error_code="INVALID_KEY_TYPE",
                    details={
                        "parameter": param_name,
                        "key": str(key),
                        "expected_type": key_type.__name__,
                        "actual_type": type(key).__name__
                    }
                )
            
            if not isinstance(val, value_type):
                raise NLMValidationError(
                    f"Dict value for key '{key}' in parameter '{param_name}' must be "
                    f"{value_type.__name__}, got {type(val).__name__}",
                    error_code="INVALID_VALUE_TYPE",
                    details={
                        "parameter": param_name,
                        "key": str(key),
                        "expected_type": value_type.__name__,
                        "actual_type": type(val).__name__
                    }
                )
        
        return value

class ValueValidator:
    """Value validation utilities."""
    
    @staticmethod
    def check_range(value: Union[int, float], min_val: Union[int, float],
                   max_val: Union[int, float], param_name: str) -> Union[int, float]:
        """Check if value is within specified range."""
        if value < min_val or value > max_val:
            raise NLMValidationError(
                f"Parameter '{param_name}' must be between {min_val} and {max_val}, "
                f"got {value}",
                error_code="OUT_OF_RANGE",
                details={
                    "parameter": param_name,
                    "value": value,
                    "min": min_val,
                    "max": max_val
                }
            )
        return value
    
    @staticmethod
    def check_positive(value: Union[int, float], param_name: str) -> Union[int, float]:
        """Check if value is positive."""
        if value <= 0:
            raise NLMValidationError(
                f"Parameter '{param_name}' must be positive, got {value}",
                error_code="NON_POSITIVE_VALUE",
                details={
                    "parameter": param_name,
                    "value": value,
                    "min": 0
                }
            )
        return value
    
    @staticmethod
    def check_non_negative(value: Union[int, float], param_name: str) -> Union[int, float]:
        """Check if value is non-negative."""
        if value < 0:
            raise NLMValidationError(
                f"Parameter '{param_name}' must be non-negative, got {value}",
                error_code="NEGATIVE_VALUE",
                details={
                    "parameter": param_name,
                    "value": value
                }
            )
        return value
    
    @staticmethod
    def check_string_not_empty(value: str, param_name: str) -> str:
        """Check if string is not empty."""
        if not value:
            raise NLMValidationError(
                f"Parameter '{param_name}' must not be empty",
                error_code="EMPTY_STRING",
                details={
                    "parameter": param_name
                }
            )
        return value
    
    @staticmethod
    def check_string_length(value: str, min_len: int, max_len: int,
                           param_name: str) -> str:
        """Check if string length is within range."""
        if len(value) < min_len or len(value) > max_len:
            raise NLMValidationError(
                f"Parameter '{param_name}' must be between {min_len} and {max_len} characters, "
                f"got {len(value)} characters",
                error_code="STRING_LENGTH",
                details={
                    "parameter": param_name,
                    "value": value[:50] + "..." if len(value) > 50 else value,
                    "length": len(value),
                    "min": min_len,
                    "max": max_len
                }
            )
        return value
    
    @staticmethod
    def check_array_not_empty(value: List[T], param_name: str) -> List[T]:
        """Check if list is not empty."""
        if not value:
            raise NLMValidationError(
                f"Parameter '{param_name}' must not be empty",
                error_code="EMPTY_ARRAY",
                details={
                    "parameter": param_name
                }
            )
        return value
    
    @staticmethod
    def check_dict_not_empty(value: Dict[K, V], param_name: str) -> Dict[K, V]:
        """Check if dict is not empty."""
        if not value:
            raise NLMValidationError(
                f"Parameter '{param_name}' must not be empty",
                error_code="EMPTY_DICT",
                details={
                    "parameter": param_name
                }
            )
        return value

class ParameterValidator:
    """Parameter validation framework."""
    
    def __init__(self):
        self.validation_rules: Dict[str, Dict[str, Any]] = {}
    
    def add_rule(self, param_name: str, rule_type: str,
                 rule_params: Optional[Dict[str, Any]] = None,
                 message: Optional[str] = None) -> 'ParameterValidator':
        """Add a validation rule."""
        if param_name not in self.validation_rules:
            self.validation_rules[param_name] = {}
        
        self.validation_rules[param_name][rule_type] = {
            'params': rule_params or {},
            'message': message
        }
        return self
    
    def validate_all(self, **kwargs) -> Dict[str, Any]:
        """Validate all parameters based on rules."""
        validated_params = {}
        
        for param_name, rules in self.validation_rules.items():
            if param_name not in kwargs:
                raise NLMValidationError(
                    f"Missing required parameter: {param_name}",
                    error_code="MISSING_PARAMETER",
                    details={
                        "parameter": param_name
                    }
                )
            
            value = kwargs[param_name]
            
            for rule_type, rule_config in rules.items():
                params = rule_config['params']
                message = rule_config['message']
                
                self._apply_rule(param_name, value, rule_type, params, message)
            
            validated_params[param_name] = value
        
        return validated_params
    
    def _apply_rule(self, param_name: str, value: Any, rule_type: str,
                    params: Dict[str, Any], custom_message: Optional[str]) -> None:
        """Apply a validation rule."""
        if rule_type == "type":
            expected_type = params.get('type')
            if expected_type:
                if value is not None and not isinstance(value, expected_type):
                    raise NLMValidationError(
                        custom_message or 
                        f"Parameter '{param_name}' must be of type {expected_type.__name__}",
                        error_code="INVALID_TYPE",
                        details={
                            "parameter": param_name,
                            "expected_type": expected_type.__name__,
                            "actual_type": type(value).__name__
                        }
                    )
        
        elif rule_type == "range":
            min_val = params.get('min')
            max_val = params.get('max')
            if value is not None:
                if min_val is not None and value < min_val:
                    raise NLMValidationError(
                        custom_message or 
                        f"Parameter '{param_name}' must be >= {min_val}, got {value}",
                        error_code="OUT_OF_RANGE",
                        details={
                            "parameter": param_name,
                            "value": value,
                            "min": min_val
                        }
                    )
                
                if max_val is not None and value > max_val:
                    raise NLMValidationError(
                        custom_message or 
                        f"Parameter '{param_name}' must be <= {max_val}, got {value}",
                        error_code="OUT_OF_RANGE",
                        details={
                            "parameter": param_name,
                            "value": value,
                            "max": max_val
                        }
                    )
        
        elif rule_type == "not_empty":
            if custom_message:
                if isinstance(value, str) and not value:
                    raise NLMValidationError(
                        custom_message,
                        error_code="EMPTY_VALUE"
                    )
                elif isinstance(value, (list, dict)) and not value:
                    raise NLMValidationError(
                        custom_message,
                        error_code="EMPTY_COLLECTION"
                    )
    
    def clear_rules(self) -> 'ParameterValidator':
        """Clear all validation rules."""
        self.validation_rules.clear()
        return self

# =============================================================================
# ERROR HANDLING STRATEGIES
# =============================================================================

class ErrorHandler:
    """Comprehensive error handling strategies."""
    
    def __init__(self, log_level: str = "INFO", enable_recovery: bool = True):
        self.log_level = log_level
        self.enable_recovery = enable_recovery
        self.error_count = 0
        self.last_error: Optional[NLMException] = None
        self.logger = self._setup_logger()
        self.recovery_handlers: Dict[Type[NLMException], Callable] = {}
    
    def _setup_logger(self) -> logging.Logger:
        """Setup error logging."""
        logger = logging.getLogger("nlm.error_handler")
        logger.setLevel(getattr(logging, self.log_level.upper()))
        
        if not logger.handlers:
            handler = logging.StreamHandler(sys.stderr)
            formatter = logging.Formatter(
                '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
            )
            handler.setFormatter(formatter)
            logger.addHandler(handler)
        
        return logger
    
    def handle_error(self, error: Exception, context: Optional[Dict[str, Any]] = None) -> Any:
        """Handle an error with appropriate strategy."""
        self.error_count += 1
        self.last_error = error if isinstance(error, NLMException) else None
        
        # Log the error
        if isinstance(error, NLMException):
            self.logger.error(str(error))
        else:
            self.logger.error(f"Unexpected error: {error}", exc_info=True)
        
        # Check for recovery handlers
        if self.enable_recovery and self.last_error:
            for error_type, handler in self.recovery_handlers.items():
                if isinstance(self.last_error, error_type):
                    try:
                        return handler(self.last_error, context)
                    except Exception as recovery_error:
                        self.logger.error(f"Recovery handler failed: {recovery_error}")
        
        # Re-raise the error if no recovery is possible
        raise
    
    def add_recovery_handler(self, error_type: Type[NLMException], 
                           handler: Callable) -> 'ErrorHandler':
        """Add a recovery handler for a specific error type."""
        self.recovery_handlers[error_type] = handler
        return self
    
    def get_error_summary(self) -> Dict[str, Any]:
        """Get summary of errors."""
        return {
            'total_errors': self.error_count,
            'last_error': str(self.last_error) if self.last_error else None,
            'last_error_type': self.last_error.__class__.__name__ if self.last_error else None
        }
    
    def reset(self) -> 'ErrorHandler':
        """Reset error counter and last error."""
        self.error_count = 0
        self.last_error = None
        return self

@contextmanager
def error_context(handler: ErrorHandler, context: Optional[Dict[str, Any]] = None):
    """Context manager for handling errors in a specific context."""
    try:
        yield
    except Exception as e:
        handler.handle_error(e, context)

# =============================================================================
# DEFENSIVE PROGRAMMING UTILITIES
# =============================================================================

class DefensiveProgramming:
    """Defensive programming utilities."""
    
    @staticmethod
    def null_check(value: Any, param_name: str, allow_none: bool = False) -> Any:
        """Check for null values."""
        if value is None:
            if allow_none:
                return value
            raise NLMValidationError(
                f"Parameter '{param_name}' cannot be None",
                error_code="NULL_VALUE",
                details={
                    "parameter": param_name
                }
            )
        return value
    
    @staticmethod
    def resource_ownership_check(resource_id: str, 
                               owned_resources: Set[str], 
                               param_name: str = "resource") -> bool:
        """Check if resource is owned."""
        if resource_id not in owned_resources:
            raise NLMResourceError(
                f"Resource '{resource_id}' is not owned. Owned resources: {owned_resources}",
                error_code="RESOURCE_NOT_OWNED",
                details={
                    "resource_id": resource_id,
                    "owned_resources": list(owned_resources)
                }
            )
        return True
    
    @staticmethod
    def thread_safety_check(lock: Optional[threading.Lock], 
                           operation: str = "operation") -> bool:
        """Check thread safety for an operation."""
        if lock is None:
            warnings.warn(f"Operation '{operation}' is not thread-safe (no lock provided)",
                         RuntimeWarning)
        return True
    
    @staticmethod
    def memory_allocation_check(size: int, max_size: int, 
                              operation: str = "allocation") -> bool:
        """Check if memory allocation is within limits."""
        if size > max_size:
            raise NLMMemoryError(
                f"Memory allocation {size} bytes exceeds limit {max_size} bytes for '{operation}'",
                error_code="MEMORY_LIMIT_EXCEEDED",
                details={
                    "operation": operation,
                    "allocated_size": size,
                    "max_size": max_size
                }
            )
        return True
    
    @staticmethod
    def file_existence_check(filepath: str, 
                           operation: str = "file operation",
                           create_if_missing: bool = False) -> bool:
        """Check if file exists."""
        path = Path(filepath)
        if not path.exists():
            if create_if_missing:
                path.parent.mkdir(parents=True, exist_ok=True)
                return True
            raise NLMFileError(
                f"File '{filepath}' does not exist for '{operation}'",
                error_code="FILE_NOT_FOUND",
                details={
                    "filepath": filepath,
                    "operation": operation
                }
            )
        return True
    
    @staticmethod
    def resource_availability_check(resource_name: str, 
                                  available_resources: Set[str],
                                  operation: str = "resource access") -> bool:
        """Check if resource is available."""
        if resource_name not in available_resources:
            raise NLMResourceError(
                f"Resource '{resource_name}' is not available for '{operation}'. "
                f"Available resources: {available_resources}",
                error_code="RESOURCE_NOT_AVAILABLE",
                details={
                    "resource_name": resource_name,
                    "available_resources": list(available_resources),
                    "operation": operation
                }
            )
        return True
    
    @staticmethod
    def graceful_degradation(operation: Callable, fallback: Callable,
                           error_types: Tuple[Type[Exception], ...] = (Exception,)) -> Any:
        """Perform operation with graceful degradation."""
        try:
            return operation()
        except error_types as e:
            warnings.warn(f"Operation failed, using fallback: {e}", RuntimeWarning)
            return fallback()

# =============================================================================
# VALIDATION FRAMEWORK
# =============================================================================

class ValidationFramework:
    """Comprehensive validation framework."""
    
    def __init__(self, strict_mode: bool = False):
        self.strict_mode = strict_mode
        self.validators = {}
        self.validated_modules = []
    
    def add_module_validator(self, module_name: str, 
                           validator_func: Callable[[Any], bool],
                           error_class: Type[NLMException] = NLMValidationError) -> 'ValidationFramework':
        """Add validator for a specific module."""
        self.validators[module_name] = {
            'validator': validator_func,
            'error_class': error_class
        }
        return self
    
    def validate_module(self, module_name: str, data: Any) -> bool:
        """Validate module data."""
        if module_name not in self.validators:
            if self.strict_mode:
                raise NLMValidationError(
                    f"No validator registered for module: {module_name}",
                    error_code="MODULE_NOT_VALIDATED"
                )
            return False
        
        validator_info = self.validators[module_name]
        is_valid = validator_info['validator'](data)
        
        if not is_valid and self.strict_mode:
            raise validator_info['error_class'](
                f"Module '{module_name}' validation failed",
                error_code="MODULE_VALIDATION_FAILED"
            )
        
        self.validated_modules.append(module_name)
        return is_valid
    
    def get_validation_report(self) -> Dict[str, Any]:
        """Get validation report."""
        return {
            'strict_mode': self.strict_mode,
            'validated_modules': self.validated_modules,
            'total_validated': len(self.validated_modules),
            'modules_with_validators': list(self.validators.keys())
        }

# =============================================================================
# TYPE SAFETY VALIDATION
# =============================================================================

class TypeSafetyChecker:
    """Type safety validation."""
    
    @staticmethod
    def check_type_hints(func: Callable) -> Dict[str, Any]:
        """Check if function has type hints."""
        hints = get_type_hints(func)
        result = {
            'has_type_hints': bool(hints),
            'parameters': {},
            'return_type': hints.get('return')
        }
        
        sig = inspect.signature(func)
        for param_name, param in sig.parameters.items():
            if param_name in hints:
                param_type = hints[param_name]
                result['parameters'][param_name] = {
                    'type': param_type,
                    'has_annotation': True,
                    'is_optional': get_origin(param.annotation) is Union
                }
            else:
                result['parameters'][param_name] = {
                    'type': None,
                    'has_annotation': False,
                    'is_optional': False
                }
        
        return result
    
    @staticmethod
    def check_instance_type(obj: Any, expected_type: Type[T],
                           context: Optional[str] = None) -> T:
        """Check if object is instance of expected type."""
        if not isinstance(obj, expected_type):
            type_name = expected_type.__name__
            actual_type = type(obj).__name__
            raise NLMValidationError(
                f"Expected {type_name}, got {actual_type}",
                error_code="TYPE_MISMATCH",
                details={
                    'expected_type': type_name,
                    'actual_type': actual_type,
                    'context': context
                }
            )
        return obj
    
    @staticmethod
    def check_generic_type(obj: Any, expected_generic: Any) -> bool:
        """Check if object matches expected generic type."""
        origin = get_origin(expected_generic)
        args = get_args(expected_generic)
        
        if origin is None:
            return isinstance(obj, expected_generic)
        
        if not isinstance(obj, origin):
            return False
        
        # For simple generic types, check the type arguments
        if origin is list and args:
            element_type = args[0]
            return all(isinstance(item, element_type) for item in obj)
        
        elif origin is dict and len(args) == 2:
            key_type, value_type = args
            return all(isinstance(k, key_type) and isinstance(v, value_type) 
                      for k, v in obj.items())
        
        return True

# =============================================================================
# DEPRECATION AND WARNING SYSTEM
# =============================================================================

class DeprecationManager:
    """Deprecation and warning management."""
    
    def __init__(self):
        self.deprecations: Dict[str, Dict[str, Any]] = {}
        self.warnings: List[Dict[str, Any]] = []
    
    def deprecate(self, feature: str, message: str, 
                  version: str, alternative: Optional[str] = None) -> None:
        """Mark a feature as deprecated."""
        self.deprecations[feature] = {
            'message': message,
            'version': version,
            'alternative': alternative,
            'deprecated_at': datetime.now()
        }
        
        warning_msg = f"DeprecationWarning: {feature} is deprecated in {version}. {message}"
        if alternative:
            warning_msg += f" Use {alternative} instead."
        
        warnings.warn(warning_msg, DeprecationWarning, stacklevel=2)
    
    def warn(self, category: str, message: str, level: str = "WARNING") -> None:
        """Issue a warning."""
        self.warnings.append({
            'category': category,
            'message': message,
            'level': level,
            'timestamp': datetime.now()
        })
        
        if level == "WARNING":
            warnings.warn(message, RuntimeWarning, stacklevel=2)
        elif level == "ERROR":
            warnings.warn(message, RuntimeWarning, stacklevel=2)
    
    def get_deprecation_report(self) -> Dict[str, Any]:
        """Get deprecation report."""
        return {
            'deprecations': self.deprecations,
            'warnings': self.warnings,
            'total_deprecations': len(self.deprecations),
            'total_warnings': len(self.warnings)
        }

# =============================================================================
# MAIN VALIDATOR CLASS
# =============================================================================

class NLMValidator:
    """Main validator class that combines all validation features."""
    
    def __init__(self, strict_mode: bool = False, log_level: str = "INFO"):
        self.strict_mode = strict_mode
        self.log_level = log_level
        self.error_handler = ErrorHandler(log_level)
        self.type_checker = TypeSafetyChecker()
        self.value_validator = ValueValidator()
        self.param_validator = ParameterValidator()
        self.defensive = DefensiveProgramming()
        self.validation_framework = ValidationFramework(strict_mode)
        self.deprecation_manager = DeprecationManager()
    
    def validate_parameter(self, value: Any, expected_type: Type[T],
                          param_name: str, **kwargs) -> T:
        """Validate a single parameter."""
        try:
            # Type validation
            validated_value = self.type_checker.check_instance_type(
                value, expected_type, f"validate_parameter:{param_name}"
            )
            
            # Range validation if specified
            if "min" in kwargs:
                validated_value = self.value_validator.check_range(
                    validated_value, kwargs["min"], kwargs.get("max", float('inf')),
                    param_name
                )
            
            if "positive" in kwargs and kwargs["positive"]:
                validated_value = self.value_validator.check_positive(
                    validated_value, param_name
                )
            
            if "not_empty" in kwargs and kwargs["not_empty"]:
                if isinstance(validated_value, str):
                    validated_value = self.value_validator.check_string_not_empty(
                        validated_value, param_name
                    )
                elif isinstance(validated_value, (list, dict)):
                    validated_value = self.value_validator.check_array_not_empty(
                        validated_value, param_name
                    )
            
            return validated_value
            
        except Exception as e:
            self.error_handler.handle_error(e, {"parameter": param_name, "value": value})
            raise
    
    def validate_function_call(self, func: Callable, *args, **kwargs) -> Any:
        """Validate function call with type checking."""
        # Get function signature
        sig = inspect.signature(func)
        param_names = list(sig.parameters.keys())
        
        # Check if all parameters are provided
        missing_params = [name for name in param_names if name not in kwargs]
        if missing_params:
            raise NLMValidationError(
                f"Missing required parameters: {missing_params}",
                error_code="MISSING_PARAMETERS"
            )
        
        # Validate parameter types
        for param_name, value in kwargs.items():
            if param_name in param_names:
                param = sig.parameters[param_name]
                if param.annotation != inspect.Parameter.empty:
                    try:
                        self.validate_parameter(value, param.annotation, param_name)
                    except Exception as e:
                        raise NLMValidationError(
                            f"Invalid parameter '{param_name}' for function '{func.__name__}': {e}",
                            error_code="PARAMETER_VALIDATION_FAILED"
                        ) from e
        
        return func(*args, **kwargs)
    
    def check_module_state(self, module_name: str, state: Any) -> bool:
        """Check module state."""
        return self.validation_framework.validate_module(module_name, state)
    
    def get_validation_summary(self) -> Dict[str, Any]:
        """Get validation summary."""
        return {
            'error_handler': self.error_handler.get_error_summary(),
            'validation_framework': self.validation_framework.get_validation_report(),
            'deprecation_manager': self.deprecation_manager.get_deprecation_report()
        }

# =============================================================================
# DECORATORS
# =============================================================================

def validate_types(expected_types: Dict[str, Type[T]]) -> Callable:
    """Decorator to validate function parameter types."""
    def decorator(func: Callable) -> Callable:
        @functools.wraps(func)
        def wrapper(*args, **kwargs) -> Any:
            # Get function signature
            sig = inspect.signature(func)
            param_names = list(sig.parameters.keys())
            
            # Validate each parameter
            for i, (param_name, param) in enumerate(sig.parameters.items()):
                if i < len(args):
                    value = args[i]
                else:
                    value = kwargs.get(param_name, None)
                
                if param_name in expected_types and value is not None:
                    expected_type = expected_types[param_name]
                    if not isinstance(value, expected_type):
                        raise NLMValidationError(
                            f"Parameter '{param_name}' must be {expected_type.__name__}, "
                            f"got {type(value).__name__}",
                            error_code="INVALID_TYPE",
                            details={
                                "parameter": param_name,
                                "expected_type": expected_type.__name__,
                                "actual_type": type(value).__name__
                            }
                        )
            
            return func(*args, **kwargs)
        
        return wrapper
    return decorator

def validate_range(min_val: Union[int, float] = None, 
                  max_val: Union[int, float] = None,
                  param_name: str = None) -> Callable:
    """Decorator to validate parameter range."""
    def decorator(func: Callable) -> Callable:
        @functools.wraps(func)
        def wrapper(*args, **kwargs) -> Any:
            # Get function signature
            sig = inspect.signature(func)
            
            # Check if we need to validate
            if param_name:
                if param_name in kwargs:
                    value = kwargs[param_name]
                else:
                    # Find positional parameter
                    param_names = list(sig.parameters.keys())
                    if param_name in param_names:
                        param_index = param_names.index(param_name)
                        if param_index < len(args):
                            value = args[param_index]
                        else:
                            return func(*args, **kwargs)
                    else:
                        return func(*args, **kwargs)
                
                # Validate range
                if min_val is not None and value < min_val:
                    raise NLMValidationError(
                        f"Parameter '{param_name}' must be >= {min_val}, got {value}",
                        error_code="OUT_OF_RANGE"
                    )
                
                if max_val is not None and value > max_val:
                    raise NLMValidationError(
                        f"Parameter '{param_name}' must be <= {max_val}, got {value}",
                        error_code="OUT_OF_RANGE"
                    )
            
            return func(*args, **kwargs)
        
        return wrapper
    return decorator

def safe_operation(error_handler: ErrorHandler, 
                  fallback: Callable = None,
                  error_types: Tuple[Type[Exception], ...] = (Exception,)) -> Callable:
    """Decorator to ensure safe operation with error handling."""
    def decorator(func: Callable) -> Callable:
        @functools.wraps(func)
        def wrapper(*args, **kwargs) -> Any:
            try:
                return func(*args, **kwargs)
            except error_types as e:
                error_handler.handle_error(e, {
                    "function": func.__name__,
                    "args": args,
                    "kwargs": kwargs
                })
                
                if fallback:
                    return fallback(*args, **kwargs)
                
                # Re-raise if no fallback
                raise
        
        return wrapper
    return decorator

# =============================================================================
# GLOBAL INSTANCES
# ==============================================================================

# Global validator instance
_validator_instance: Optional[NLMValidator] = None

def get_validator() -> NLMValidator:
    """Get global validator instance."""
    global _validator_instance
    if _validator_instance is None:
        _validator_instance = NLMValidator()
    return _validator_instance

def set_validator(validator: NLMValidator) -> None:
    """Set global validator instance."""
    global _validator_instance
    _validator_instance = validator

# =============================================================================
# BACKWARD COMPATIBILITY FUNCTIONS
# ==============================================================================

# For backward compatibility with pynlm_enhanced imports
def create_default_validator() -> NLMValidator:
    """Create a default validator instance."""
    return NLMValidator()

def validate_parameter(value: Any, expected_type: Type[T],
                      param_name: str) -> T:
    """Legacy function for backward compatibility."""
    validator = get_validator()
    return validator.validate_parameter(value, expected_type, param_name)

def validate_function_call(func: Callable, *args, **kwargs) -> Any:
    """Legacy function for backward compatibility."""
    validator = get_validator()
    return validator.validate_function_call(func, *args, **kwargs)

# =============================================================================
# UTILITY FUNCTIONS
# ==============================================================================

def get_type_info(obj: Any) -> Dict[str, Any]:
    """Get type information about an object."""
    return {
        'type': type(obj).__name__,
        'module': type(obj).__module__,
        'is_instance': isinstance(obj, (list, dict, tuple, str, int, float, bool)),
        'has_attributes': hasattr(obj, '__dict__') or hasattr(obj, '__slots__')
    }

def get_validation_error_chain(error: Exception) -> List[str]:
    """Get validation error chain."""
    chain = []
    current = error
    
    while current:
        chain.append(str(current))
        current = getattr(current, '__cause__', None)
        if not current and hasattr(current, '__context__'):
            current = current.__context__
    
    return chain

def format_validation_report(results: Dict[str, Any]) -> str:
    """Format validation report as string."""
    lines = ["Validation Report"]
    lines.append("=" * 50)
    
    for section, data in results.items():
        lines.append(f"\n{section.replace('_', ' ').title()}:")
        if isinstance(data, dict):
            for key, value in data.items():
                if isinstance(value, dict):
                    lines.append(f"  {key}:")
                    for sub_key, sub_value in value.items():
                        lines.append(f"    {sub_key}: {sub_value}")
                else:
                    lines.append(f"  {key}: {value}")
        elif isinstance(data, (list, tuple)):
            for item in data:
                lines.append(f"  - {item}")
        else:
            lines.append(f"  {data}")
    
    return "\n".join(lines)

# =============================================================================
# MODULE INITIALIZATION
# ==============================================================================

# Initialize global validator
_validator_instance = NLMValidator()

# Log module initialization
logging.getLogger("nlm.error_handler").info(
    "Error handling and type safety module initialized successfully"
)

__all__ = [
    # Exception classes
    'NLMException', 'NLMConfigError', 'NLMBrainError', 'NLMWorldError',
    'NLMMemoryError', 'NLMPrecisionError', 'NLMFileError', 'NLMValidationError',
    'NLMResourceError', 'NLMSimulationError',
    
    # Validator classes
    'TypeValidator', 'ValueValidator', 'ParameterValidator', 'ErrorHandler',
    'DefensiveProgramming', 'ValidationFramework', 'TypeSafetyChecker',
    'DeprecationManager', 'NLMValidator',
    
    # Decorators
    'validate_types', 'validate_range', 'safe_operation',
    
    # Global instances and utilities
    'get_validator', 'set_validator', 'create_default_validator',
    'validate_parameter', 'validate_function_call', 'error_context',
    
    # Utility functions
    'get_type_info', 'get_validation_error_chain', 'format_validation_report',
    
    # Backward compatibility
    'NLMException', 'NLMConfigError', 'NLMSimulationError', 'NLMResourceError'
]

if __name__ == "__main__":
    # Test the error handling and type safety module
    print("Testing Error Handling and Type Safety Module")
    print("=" * 60)
    
    # Test exception hierarchy
    try:
        raise NLMConfigError("Test config error", error_code="TEST_001")
    except NLMException as e:
        print(f"✓ NLMException hierarchy test: {e}")
    
    # Test type validation
    validator = get_validator()
    try:
        result = validator.validate_parameter(42, int, "test_param")
        print(f"✓ Type validation test: {result}")
    except Exception as e:
        print(f"✗ Type validation test failed: {e}")
    
    # Test range validation
    try:
        result = validator.validate_parameter(50, int, "range_test", min=0, max=100)
        print(f"✓ Range validation test: {result}")
    except Exception as e:
        print(f"✗ Range validation test failed: {e}")
    
    # Test error handling
    try:
        raise NLMValidationError("Test validation error")
    except Exception as e:
        try:
            validator.error_handler.handle_error(e, {"test": "validation"})
        except:
            print(f"✓ Error handling test: Error was handled")
    
    print("\nAll tests completed!")