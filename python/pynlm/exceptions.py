"""
Exception hierarchy for NLM Python bindings.

This module provides custom exception classes for NLM Python bindings,
enabling better error handling and debugging.
"""


class NLMError(Exception):
    """Base exception class for all NLM errors."""
    pass
class ConfigurationError(NLMError):
    """Exception raised for configuration-related errors."""
    pass
class ValidationError(NLMError):
    """Exception raised for validation-related errors."""
    pass
class SimulationError(NLMError):
    """Exception raised for simulation-related errors."""
    pass
class BrainError(NLMError):
    """Exception raised for brain-related errors."""
    pass
class MemoryError(NLMError):
    """Exception raised for memory-related errors."""
    pass
class IntegrationError(NLMError):
    """Exception raised for integration-related errors."""
    pass
class PythonBindingError(NLMError):
    """Exception raised for Python binding-related errors."""
    pass
class TypeError(NLMError):
    """Exception raised for type-related errors."""
    pass
class ValueError(NLMError):
    """Exception raised for value-related errors."""
    pass
class NotImplementedError(NLMError):
    """Exception raised when a feature is not yet implemented."""
    pass
class ResourceError(NLMError):
    """Exception raised for resource-related errors (memory, file, etc.)."""
    pass


# Error message templates for common scenarios
ERROR_MESSAGES = {
    'brain_not_initialized': 'Brain must be initialized before use',
    'brain_already_initialized': 'Brain is already initialized',
    'invalid_configuration': 'Invalid configuration provided',
    'missing_configuration': 'Required configuration missing',
    'simulation_not_running': 'Simulation is not running',
    'simulation_already_running': 'Simulation is already running',
    'memory_allocation_failed': 'Memory allocation failed',
    'file_not_found': 'File not found: {file_path}',
    'invalid_parameter': 'Invalid parameter: {parameter}',
    'feature_not_available': 'Feature not available: {feature}',
    'dependency_missing': 'Required dependency missing: {dependency}',
    'version_mismatch': 'Version mismatch between C++ and Python bindings',
    'initialization_failed': 'Initialization failed: {reason}',
}


def raise_error(error_type: str, **kwargs) -> None:
    """
    Raise appropriate NLM error based on error type and parameters.
    
    Args:
        error_type: Type of error (key from ERROR_MESSAGES)
        **kwargs: Additional parameters for error message
        
    Raises:
        NLMError: Appropriate error class
    """
    if error_type in ERROR_MESSAGES:
        message = ERROR_MESSAGES[error_type].format(**kwargs)
    else:
        message = f"Unknown error: {error_type}"
    
    # Determine appropriate error class
    error_class = NLMError
    
    if 'brain' in error_type.lower():
        error_class = BrainError
    elif 'configuration' in error_type.lower():
        error_class = ConfigurationError
    elif 'validation' in error_type.lower():
        error_class = ValidationError
    elif 'simulation' in error_type.lower():
        error_class = SimulationError
    elif 'memory' in error_type.lower():
        error_class = MemoryError
    elif 'integration' in error_type.lower():
        error_class = IntegrationError
    elif 'binding' in error_type.lower():
        error_class = PythonBindingError
    
    raise error_class(message)


def handle_nml_errors(func):
    """
    Decorator to handle NLM errors in Python functions.
    
    Args:
        func: Function to decorate
        
    Returns:
        Decorated function with error handling
    """
    def wrapper(*args, **kwargs):
        try:
            return func(*args, **kwargs)
        except NLMError:
            raise
        except Exception as e:
            # Convert unknown exceptions to NLMError
            raise NLMError(f"Unexpected error in {func.__name__}: {e}") from e
    
    return wrapper


def validate_parameter(value: Any, param_name: str, param_type: type, 
                      min_value: Any = None, max_value: Any = None) -> None:
    """
    Validate parameter value.
    
    Args:
        value: Parameter value to validate
        param_name: Name of parameter
        param_type: Expected type
        min_value: Minimum allowed value
        max_value: Maximum allowed value
        
    Raises:
        ValidationError: If parameter is invalid
    """
    if not isinstance(value, param_type):
        raise ValidationError(
            f"Parameter '{param_name}' must be {param_type.__name__}, "
            f"got {type(value).__name__}"
        )
    
    if min_value is not None and value < min_value:
        raise ValidationError(
            f"Parameter '{param_name}' must be >= {min_value}, "
            f"got {value}"
        )
    
    if max_value is not None and value > max_value:
        raise ValidationError(
            f"Parameter '{param_name}' must be <= {max_value}, "
            f"got {value}"
        )


def safe_call(c_func, *args, **kwargs):
    """
    Safely call C++ function and handle exceptions.
    
    Args:
        c_func: C++ function to call
        *args: Arguments for C++ function
        **kwargs: Keyword arguments for C++ function
        
    Returns:
        Result of C++ function call
        
    Raises:
        PythonBindingError: If C++ function call fails
    """
    try:
        return c_func(*args, **kwargs)
    except pybind11.error_already_set:
        # Extract error message from pybind11
        import pybind11
        error_msg = pybind11.get_exception_details()
        raise PythonBindingError(f"C++ function error: {error_msg}")
    except Exception as e:
        raise PythonBindingError(f"Unexpected error calling C++ function: {e}")
