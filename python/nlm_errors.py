"""
NLM Python Bindings - Comprehensive Error Handling and Validation

This module provides robust error handling for the NLM (Neural Learning Machine)
Python bindings, including custom exception classes, validation functions,
and error recovery mechanisms.
"""

import sys
import traceback
import json
import numpy as np
from typing import Any, Dict, List, Optional, Union, Type, TypeVar, cast
from enum import Enum

# Version
__version__ = "0.1.0"

# Custom exception hierarchy for NLM errors

class NLMError(Exception):
    """
    Base exception class for all NLM errors.
    
    This is the root exception for all NLM-specific errors, providing
    a consistent error hierarchy for error handling.
    """
    
    def __init__(self, message: str, error_code: str = None, 
                 details: Optional[Dict[str, Any]] = None):
        """
        Initialize NLMError with enhanced error information.
        
        Args:
            message: Human-readable error message
            error_code: Unique error code for programmatic handling
            details: Additional context about the error
        """
        super().__init__(message)
        self.message = message
        self.error_code = error_code or self.__class__.__name__
        self.details = details or {}
        self.stack_trace = traceback.format_stack()
        self.timestamp = self._get_timestamp()
    
    def _get_timestamp(self) -> str:
        """Get current timestamp for error tracking."""
        import datetime
        return datetime.datetime.now().isoformat()
    
    def __str__(self) -> str:
        """Enhanced error message with context."""
        base_msg = self.message
        if self.error_code:
            base_msg = f"[{self.error_code}] {base_msg}"
        
        if self.details:
            base_msg += f"\nDetails: {json.dumps(self.details, indent=2)}"
        
        return base_msg

class ConfigurationError(NLMError):
    """
    Exception raised for configuration-related errors.
    
    This includes file loading issues, invalid JSON/YAML, missing required keys,
    and parameter validation failures.
    """
    
    def __init__(self, message: str, filepath: Optional[str] = None,
                 missing_keys: Optional[List[str]] = None,
                 invalid_values: Optional[Dict[str, Any]] = None):
        details = {}
        if filepath:
            details['filepath'] = filepath
        if missing_keys:
            details['missing_keys'] = missing_keys
        if invalid_values:
            details['invalid_values'] = invalid_values
        
        super().__init__(message, "CONFIG_ERROR", details)

class MemoryError(NLMError):
    """
    Exception raised for memory system errors.
    
    This includes memory allocation failures, memory bounds violations,
    and memory-related constraints.
    """
    
    def __init__(self, message: str, neuron_id: Optional[str] = None,
                 memory_type: Optional[str] = None,
                 allocated_size: Optional[int] = None,
                 max_capacity: Optional[int] = None):
        details = {}
        if neuron_id:
            details['neuron_id'] = neuron_id
        if memory_type:
            details['memory_type'] = memory_type
        if allocated_size is not None:
            details['allocated_size'] = allocated_size
        if max_capacity is not None:
            details['max_capacity'] = max_capacity
        
        super().__init__(message, "MEMORY_ERROR", details)

class PlasticityError(NLMError):
    """
    Exception raised for plasticity rule errors.
    
    This includes invalid plasticity parameters, rule application failures,
    and plasticity constraint violations.
    """
    
    def __init__(self, message: str, plasticity_rule: Optional[str] = None,
                 neuron_type: Optional[str] = None,
                 parameters: Optional[Dict[str, float]] = None):
        details = {}
        if plasticity_rule:
            details['plasticity_rule'] = plasticity_rule
        if neuron_type:
            details['neuron_type'] = neuron_type
        if parameters:
            details['parameters'] = parameters
        
        super().__init__(message, "PLASTICITY_ERROR", details)

class SimulationError(NLMError):
    """
    Exception raised for runtime simulation errors.
    
    This includes simulation step failures, timing issues, and
    runtime constraint violations during neural simulation.
    """
    
    def __init__(self, message: str, step_number: Optional[int] = None,
                 component: Optional[str] = None,
                 simulation_time: Optional[float] = None):
        details = {}
        if step_number is not None:
            details['step_number'] = step_number
        if component:
            details['component'] = component
        if simulation_time is not None:
            details['simulation_time'] = simulation_time
        
        super().__init__(message, "SIMULATION_ERROR", details)

class DevelopmentError(NLMError):
    """
    Exception raised for development stage errors.
    
    This includes developmental stage violations, maturation failures,
    and developmental timing issues.
    """
    
    def __init__(self, message: str, developmental_stage: Optional[str] = None,
                 current_stage: Optional[str] = None,
                 requirements: Optional[Dict[str, Any]] = None):
        details = {}
        if developmental_stage:
            details['developmental_stage'] = developmental_stage
        if current_stage:
            details['current_stage'] = current_stage
        if requirements:
            details['requirements'] = requirements
        
        super().__init__(message, "DEVELOPMENT_ERROR", details)

class NeuromodulationError(NLMError):
    """
    Exception raised for neuromodulation issues.
    
    This includes neuromodulator application failures, signal violations,
    and neuromodulatory constraint violations.
    """
    
    def __init__(self, message: str, neuromodulator: Optional[str] = None,
                 target_neuron: Optional[str] = None,
                 signal_strength: Optional[float] = None):
        details = {}
        if neuromodulator:
            details['neuromodulator'] = neuromodulator
        if target_neuron:
            details['target_neuron'] = target_neuron
        if signal_strength is not None:
            details['signal_strength'] = signal_strength
        
        super().__init__(message, "NEUROMODULATION_ERROR", details)


# Validation Functions

class NLMValidator:
    """
    Comprehensive validation system for NLM parameters and operations.
    
    This class provides validation methods for all NLM operations,
    including type checking, bounds validation, and cross-component validation.
    """
    
    # Validation constants
    MAX_NEURONS = 1000000  # 1 million neurons max
    MAX_SYNAPSES = 10000000  # 10 million synapses max
    MAX_REGIONS = 1000
    MAX_POPULATIONS = 1000
    
    MIN_NEURON_COUNT = 1
    MIN_REGION_COUNT = 1
    MIN_TIME_STEP = 0.0
    MAX_TIME_STEP = 1.0
    
    NEURON_ID_MIN = 0
    NEURON_ID_MAX = 2**64 - 1
    
    @classmethod
    def validate_config(cls, config: Dict[str, Any]) -> None:
        """
        Validate NLM configuration dictionary.
        
        Args:
            config: Configuration dictionary to validate
            
        Raises:
            ConfigurationError: If configuration is invalid
        """
        required_keys = {
            'neuron_count': (int, lambda x: x >= cls.MIN_NEURON_COUNT and x <= cls.MAX_NEURONS),
            'region_count': (int, lambda x: x >= cls.MIN_REGION_COUNT and x <= cls.MAX_REGIONS),
            'connection_probability': (float, lambda x: 0.0 <= x <= 1.0),
            'time_step': (float, lambda x: cls.MIN_TIME_STEP <= x <= cls.MAX_TIME_STEP),
        }
        
        missing_keys = []
        for key, (expected_type, validator) in required_keys.items():
            if key not in config:
                missing_keys.append(key)
            elif not isinstance(config[key], expected_type):
                raise ConfigurationError(
                    f"Configuration key '{key}' must be of type {expected_type}",
                    invalid_values={key: {'expected': str(expected_type), 'got': str(type(config[key]))}}
                )
            elif not validator(config[key]):
                raise ConfigurationError(
                    f"Configuration value for '{key}' {config[key]} is out of valid range",
                    invalid_values={key: config[key]}
                )
        
        if missing_keys:
            raise ConfigurationError(
                f"Missing required configuration keys: {', '.join(missing_keys)}",
                missing_keys=missing_keys
            )
    
    @classmethod
    def validate_neuron_id(cls, neuron_id: Union[int, str]) -> None:
        """
        Validate neuron ID.
        
        Args:
            neuron_id: Neuron ID to validate
            
        Raises:
            MemoryError: If neuron ID is invalid
        """
        try:
            if isinstance(neuron_id, str):
                neuron_id = int(neuron_id)
            
            if neuron_id < cls.NEURON_ID_MIN or neuron_id > cls.NEURON_ID_MAX:
                raise MemoryError(
                    f"Neuron ID {neuron_id} is out of valid range [{cls.NEURON_ID_MIN}, {cls.NEURON_ID_MAX}]"
                )
        except ValueError:
            raise MemoryError(f"Invalid neuron ID format: {neuron_id}")
    
    @classmethod
    def validate_memory_bounds(cls, current_usage: int, max_capacity: int, 
                              component_name: str = "memory") -> None:
        """
        Validate memory usage against capacity.
        
        Args:
            current_usage: Current memory usage
            max_capacity: Maximum allowed capacity
            component_name: Name of the memory component
            
        Raises:
            MemoryError: If memory bounds are exceeded
        """
        if current_usage > max_capacity:
            raise MemoryError(
                f"Memory usage ({current_usage}) exceeds capacity ({max_capacity}) in {component_name}",
                allocated_size=current_usage,
                max_capacity=max_capacity,
                memory_type=component_name
            )
    
    @classmethod
    def validate_simulation_step(cls, step: int, max_steps: int = 1000000) -> None:
        """
        Validate simulation step number.
        
        Args:
            step: Simulation step number
            max_steps: Maximum allowed steps
            
        Raises:
            SimulationError: If step is invalid
        """
        if step < 0:
            raise SimulationError(f"Simulation step {step} cannot be negative")
        if step > max_steps:
            raise SimulationError(
                f"Simulation step {step} exceeds maximum ({max_steps})"
            )
    
    @classmethod
    def validate_plasticity_parameters(cls, rule_name: str, 
                                       parameters: Dict[str, float]) -> None:
        """
        Validate plasticity rule parameters.
        
        Args:
            rule_name: Name of the plasticity rule
            parameters: Parameters to validate
            
        Raises:
            PlasticityError: If parameters are invalid
        """
        valid_rules = {'stdp', 'hebbian', 'structural'}
        
        if rule_name not in valid_rules:
            raise PlasticityError(
                f"Unknown plasticity rule: {rule_name}. Valid rules: {', '.join(valid_rules)}"
            )
        
        # Rule-specific validation
        if rule_name == 'stdp':
            if 'learning_rate' not in parameters:
                raise PlasticityError("STDP rule requires 'learning_rate' parameter")
            if parameters['learning_rate'] < 0 or parameters['learning_rate'] > 1:
                raise PlasticityError("STDP learning_rate must be between 0 and 1")
        
        elif rule_name == 'hebbian':
            if 'connection_weight' not in parameters:
                raise PlasticityError("Hebbian rule requires 'connection_weight' parameter")
            if parameters['connection_weight'] < 0:
                raise PlasticityError("Hebbian connection_weight must be non-negative")
    
    @classmethod
    def validate_developmental_stage(cls, current_stage: str, 
                                    required_stage: Optional[str] = None) -> None:
        """
        Validate developmental stage.
        
        Args:
            current_stage: Current developmental stage
            required_stage: Required developmental stage
            
        Raises:
            DevelopmentError: If stage validation fails
        """
        valid_stages = {
            'initial', 'critical_period', 'maturation', 'adult', 'aging'
        }
        
        if current_stage not in valid_stages:
            raise DevelopmentError(
                f"Invalid developmental stage: {current_stage}"
            )
        
        if required_stage and current_stage != required_stage:
            raise DevelopmentError(
                f"Required developmental stage '{required_stage}' not met. "
                f"Current stage: {current_stage}",
                current_stage=current_stage,
                developmental_stage=required_stage
            )
    
    @classmethod
    def validate_neuromodulation(cls, neuromodulator: str, 
                                 target_neuron: str, signal_strength: float) -> None:
        """
        Validate neuromodulation signal.
        
        Args:
            neuromodulator: Neuromodulator name
            target_neuron: Target neuron identifier
            signal_strength: Signal strength
            
        Raises:
            NeuromodulationError: If validation fails
        """
        valid_neuromodulators = {'dopamine', 'serotonin', 'acetylcholine', 'noradrenaline'}
        
        if neuromodulator not in valid_neuromodulators:
            raise NeuromodulationError(
                f"Unknown neuromodulator: {neuromodulator}"
            )
        
        if signal_strength < 0:
            raise NeuromodulationError(
                f"Neuromodulation signal strength {signal_strength} cannot be negative"
            )


# Error Recovery and State Management

class ErrorRecoveryManager:
    """
    Manages error recovery and graceful degradation for NLM operations.
    
    This class provides mechanisms for recovering from errors and maintaining
    system state integrity during error conditions.
    """
    
    def __init__(self):
        self.error_count = 0
        self.last_error = None
        self.recovery_actions = {}
        self.state_backup = {}
    
    def record_error(self, error: NLMError) -> None:
        """
        Record an error for recovery and monitoring.
        
        Args:
            error: The error that occurred
        """
        self.error_count += 1
        self.last_error = error
        
        # Log error details
        print(f"[NLM ERROR] {error}", file=sys.stderr)
        
        # Check if recovery action is defined
        if error.error_code in self.recovery_actions:
            self._execute_recovery_action(error)
    
    def backup_state(self, component: str, state: Dict[str, Any]) -> None:
        """
        Backup component state for recovery.
        
        Args:
            component: Component name
            state: State dictionary
        """
        self.state_backup[component] = {
            'state': state.copy(),
            'timestamp': self._get_timestamp()
        }
    
    def restore_state(self, component: str) -> Optional[Dict[str, Any]]:
        """
        Restore component state from backup.
        
        Args:
            component: Component name
            
        Returns:
            Restored state if available, None otherwise
        """
        if component in self.state_backup:
            backup = self.state_backup[component]
            # Remove from backup after restoration
            del self.state_backup[component]
            return backup['state']
        return None
    
    def add_recovery_action(self, error_code: str, action: callable) -> None:
        """
        Add a recovery action for a specific error code.
        
        Args:
            error_code: Error code to handle
            action: Recovery action function
        """
        self.recovery_actions[error_code] = action
    
    def _execute_recovery_action(self, error: NLMError) -> None:
        """
        Execute recovery action for the error.
        
        Args:
            error: The error that occurred
        """
        if error.error_code in self.recovery_actions:
            try:
                self.recovery_actions[error.error_code](error)
            except Exception as e:
                print(f"[RECOVERY ERROR] Failed to execute recovery action: {e}", file=sys.stderr)
    
    def _get_timestamp(self) -> str:
        """Get current timestamp."""
        import datetime
        return datetime.datetime.now().isoformat()


# Context Manager for Safe Operations

class SafeNLMOperation:
    """
    Context manager for safe NLM operations with automatic error handling.
    
    This class provides a safe execution context for NLM operations,
    with automatic error recovery and state management.
    """
    
    def __init__(self, operation_name: str, recovery_manager: Optional[ErrorRecoveryManager] = None):
        """
        Initialize safe operation context.
        
        Args:
            operation_name: Name of the operation
            recovery_manager: Error recovery manager
        """
        self.operation_name = operation_name
        self.recovery_manager = recovery_manager or ErrorRecoveryManager()
        self.original_error_count = self.recovery_manager.error_count
    
    def __enter__(self):
        """Enter safe operation context."""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """
        Exit safe operation context and handle errors.
        
        Args:
            exc_type: Exception type
            exc_val: Exception value
            exc_tb: Exception traceback
            
        Returns:
            bool: True if exception was handled, False otherwise
        """
        if exc_type is not None:
            # Handle the exception
            if isinstance(exc_val, NLMError):
                nlm_error = exc_val
            else:
                # Convert to NLMError
                nlm_error = NLMError(
                    f"Unexpected error in {self.operation_name}: {str(exc_val)}",
                    error_code="UNKNOWN_ERROR"
                )
            
            # Record error
            self.recovery_manager.record_error(nlm_error)
            
            # Try recovery if needed
            if nlm_error.error_code in self.recovery_manager.recovery_actions:
                try:
                    self.recovery_manager._execute_recovery_action(nlm_error)
                except Exception as recovery_exc:
                    print(f"[RECOVERY FAILED] {recovery_exc}", file=sys.stderr)
                    return False
            
            return True  # Exception handled
        
        return False  # No exception


# Global instances

_recovery_manager = ErrorRecoveryManager()


def get_recovery_manager() -> ErrorRecoveryManager:
    """Get the global error recovery manager instance."""
    return _recovery_manager


def with_safe_operation(operation_name: str):
    """
    Decorator for safe NLM operations.
    
    Args:
        operation_name: Name of the operation
        
    Returns:
        Decorated function
    """
    def decorator(func):
        def wrapper(*args, **kwargs):
            with SafeNLMOperation(operation_name, _recovery_manager):
                return func(*args, **kwargs)
        return wrapper
    return decorator


# Error handling utilities

exception_registry = {
    'CONFIG_ERROR': ConfigurationError,
    'MEMORY_ERROR': MemoryError,
    'PLASTICITY_ERROR': PlasticityError,
    'SIMULATION_ERROR': SimulationError,
    'DEVELOPMENT_ERROR': DevelopmentError,
    'NEUROMODULATION_ERROR': NeuromodulationError,
    'UNKNOWN_ERROR': NLMError
}


def raise_nlm_error(error_code: str, message: str, **kwargs) -> None:
    """
    Raise an NLM error with the specified code and message.
    
    Args:
        error_code: Error code (must be registered)
        message: Error message
        **kwargs: Additional error details
        
    Raises:
        NLMError: The appropriate error type
    """
    if error_code not in exception_registry:
        error_code = 'UNKNOWN_ERROR'
    
    error_class = exception_registry[error_code]
    raise error_class(message, **kwargs)


def handle_nlm_error(func):
    """
    Decorator to handle NLM errors in functions.
    
    Args:
        func: Function to decorate
        
    Returns:
        Decorated function
    """
    def wrapper(*args, **kwargs):
        try:
            return func(*args, **kwargs)
        except NLMError:
            raise
        except Exception as e:
            # Wrap unexpected exceptions
            raise_nlm_error(
                'UNKNOWN_ERROR',
                f"Unexpected error in {func.__name__}: {str(e)}",
                details={'original_exception': str(e)}
            )
    return wrapper


# Error information and debugging utilities

_error_info_cache = {}


def get_error_info(error_code: str) -> Dict[str, Any]:
    """
    Get information about a specific error code.
    
    Args:
        error_code: Error code to look up
        
    Returns:
        Error information dictionary
    """
    if error_code not in _error_info_cache:
        if error_code in exception_registry:
            error_class = exception_registry[error_code]
            _error_info_cache[error_code] = {
                'class': error_class,
                'description': error_class.__doc__ or "",
                'recovery_actions': _recovery_manager.recovery_actions.get(error_code, None)
            }
        else:
            _error_info_cache[error_code] = {
                'class': None,
                'description': 'Unknown error code',
                'recovery_actions': None
            }
    
    return _error_info_cache[error_code]


def get_all_error_codes() -> List[str]:
    """
    Get all registered error codes.
    
    Returns:
        List of error codes
    """
    return list(exception_registry.keys())


def clear_error_info_cache() -> None:
    """Clear error information cache."""
    global _error_info_cache
    _error_info_cache.clear()


# Backward compatibility wrapper

def safe_execute(func, *args, **kwargs):
    """
    Legacy function for safe execution with error handling.
    
    Args:
        func: Function to execute
        *args: Function arguments
        **kwargs: Function keyword arguments
        
    Returns:
        Function result
        
    Raises:
        NLMError: If function execution fails
    """
    try:
        return func(*args, **kwargs)
    except NLMError:
        raise
    except Exception as e:
        raise_nlm_error(
            'UNKNOWN_ERROR',
            f"Legacy safe execution failed: {str(e)}",
            details={'function': func.__name__}
        )
