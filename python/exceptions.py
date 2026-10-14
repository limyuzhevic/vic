"""
Exception hierarchy for NLM Python bindings.
Provides comprehensive error handling with Pythonic exception types.
"""

from typing import Any, Optional, Dict, List
import traceback

__all__ = [
    'NLMBaseException',
    'NLMError',
    'NLMConfigurationError',
    'NLMValidationError',
    'NLMMemoryError',
    'NLMIndexError',
    'NLMTypeError',
    'NLMRuntimeError',
    'NLMNotImplementedError',
    'NLMValueError',
    'NLMKeyError',
    'NLMFileError',
    'NLMResourceError',
    'NLMPerformanceError',
    'create_exception_from_cpp',
]

class NLMBaseException(Exception):
    """
    Base exception class for all NLM errors.
    
    Provides common functionality for all NLM exceptions.
    """
    
    def __init__(self, message: str, original_exception: Optional[Exception] = None,
                 error_code: Optional[str] = None, context: Optional[Dict[str, Any]] = None):
        """
        Initialize base NLM exception.
        
        Args:
            message: Error message.
            original_exception: Original exception that caused this error.
            error_code: Error code for programmatic error handling.
            context: Additional context information about the error.
        """
        super().__init__(message)
        self.message = message
        self.original_exception = original_exception
        self.error_code = error_code
        self.context = context or {}
        self.stack_trace = traceback.format_stack()
    
    def __str__(self) -> str:
        """Get string representation of the exception."""
        if self.error_code:
            return f"[{self.error_code}] {self.message}"
        return self.message
    
    def get_context(self) -> Dict[str, Any]:
        """Get context information about the error."""
        return self.context.copy()
    
    def add_context(self, **kwargs) -> None:
        """Add context information to the exception."""
        self.context.update(kwargs)
    
    def has_original_exception(self) -> bool:
        """Check if there is an original exception."""
        return self.original_exception is not None

class NLMError(NLMBaseException):
    """
    Base exception for general NLM errors.
    
    Use this for general errors that don't fit other specific exception types.
    """
    pass

class NLMConfigurationError(NLMBaseException):
    """
    Exception raised for configuration-related errors.
    
    Used when configuration files are invalid, missing, or corrupted.
    """
    
    def __init__(self, message: str, filepath: Optional[str] = None,
                 original_exception: Optional[Exception] = None):
        super().__init__(message, original_exception, "CONFIG_ERROR")
        self.filepath = filepath

class NLMValidationError(NLMBaseException):
    """
    Exception raised for validation errors.
    
    Used when input parameters or data validation fails.
    """
    
    def __init__(self, message: str, field: Optional[str] = None,
                 value: Optional[Any] = None, constraints: Optional[Dict[str, Any]] = None,
                 original_exception: Optional[Exception] = None):
        super().__init__(message, original_exception, "VALIDATION_ERROR")
        self.field = field
        self.value = value
        self.constraints = constraints or {}
    
    def get_validation_details(self) -> Dict[str, Any]:
        """Get detailed validation error information."""
        details = {
            'message': self.message,
            'field': self.field,
            'value': self.value,
            'constraints': self.constraints,
        }
        if self.has_original_exception():
            details['original_error'] = str(self.original_exception)
        return details

class NLMMemoryError(NLMBaseException):
    """
    Exception raised for memory-related errors.
    
    Used when memory allocation fails or memory pools are exhausted.
    """
    
    def __init__(self, message: str, memory_type: Optional[str] = None,
                 available_memory: Optional[int] = None,
                 original_exception: Optional[Exception] = None):
        super().__init__(message, original_exception, "MEMORY_ERROR")
        self.memory_type = memory_type
        self.available_memory = available_memory

class NLMIndexError(NLMBaseException):
    """
    Exception raised for index-related errors.
    
    Used when accessing arrays, vectors, or data structures with invalid indices.
    """
    
    def __init__(self, message: str, index: Optional[int] = None,
                 max_index: Optional[int] = None, structure: Optional[str] = None,
                 original_exception: Optional[Exception] = None):
        super().__init__(message, original_exception, "INDEX_ERROR")
        self.index = index
        self.max_index = max_index
        self.structure = structure

class NLMTypeError(NLMBaseException):
    """
    Exception raised for type-related errors.
    
    Used when type conversion or type checking fails.
    """
    
    def __init__(self, message: str, expected_type: Optional[str] = None,
                 actual_type: Optional[str] = None, value: Optional[Any] = None,
                 original_exception: Optional[Exception] = None):
        super().__init__(message, original_exception, "TYPE_ERROR")
        self.expected_type = expected_type
        self.actual_type = actual_type
        self.value = value

class NLMRuntimeError(NLMBaseException):
    """
    Exception raised for runtime errors.
    
    Used for general runtime errors during simulation or processing.
    """
    
    def __init__(self, message: str, operation: Optional[str] = None,
                 simulation_step: Optional[int] = None,
                 original_exception: Optional[Exception] = None):
        super().__init__(message, original_exception, "RUNTIME_ERROR")
        self.operation = operation
        self.simulation_step = simulation_step

class NLMNotImplementedError(NLMBaseException):
    """
    Exception raised when functionality is not implemented.
    
    Used when trying to use features that haven't been implemented yet.
    """
    
    def __init__(self, message: str, feature: Optional[str] = None,
                 original_exception: Optional[Exception] = None):
        super().__init__(message, original_exception, "NOT_IMPLEMENTED")
        self.feature = feature

class NLMValueError(NLMBaseException):
    """
    Exception raised for value-related errors.
    
    Used when values are out of valid range or invalid.
    """
    
    def __init__(self, message: str, parameter: Optional[str] = None,
                 value: Optional[Any] = None, valid_range: Optional[tuple] = None,
                 original_exception: Optional[Exception] = None):
        super().__init__(message, original_exception, "VALUE_ERROR")
        self.parameter = parameter
        self.value = value
        self.valid_range = valid_range

class NLMKeyError(NLMBaseException):
    """
    Exception raised for key-related errors.
    
    Used when dictionary keys or configuration keys are not found.
    """
    
    def __init__(self, message: str, key: Optional[str] = None,
                 available_keys: Optional[List[str]] = None,
                 original_exception: Optional[Exception] = None):
        super().__init__(message, original_exception, "KEY_ERROR")
        self.key = key
        self.available_keys = available_keys or []

class NLMFileError(NLMBaseException):
    """
    Exception raised for file-related errors.
    
    Used when file operations fail (read, write, parse, etc.).
    """
    
    def __init__(self, message: str, filepath: Optional[str] = None,
                 operation: Optional[str] = None,
                 original_exception: Optional[Exception] = None):
        super().__init__(message, original_exception, "FILE_ERROR")
        self.filepath = filepath
        self.operation = operation

class NLMResourceError(NLMBaseException):
    """
    Exception raised for resource-related errors.
    
    Used when resources (CPU, memory, GPU) are not available or exhausted.
    """
    
    def __init__(self, message: str, resource_type: Optional[str] = None,
                 available: Optional[int] = None, required: Optional[int] = None,
                 original_exception: Optional[Exception] = None):
        super().__init__(message, original_exception, "RESOURCE_ERROR")
        self.resource_type = resource_type
        self.available = available
        self.required = required

class NLMPerformanceError(NLMBaseException):
    """
    Exception raised for performance-related errors.
    
    Used when performance degrades or exceeds thresholds.
    """
    
    def __init__(self, message: str, metric: Optional[str] = None,
                 value: Optional[float] = None, threshold: Optional[float] = None,
                 original_exception: Optional[Exception] = None):
        super().__init__(message, original_exception, "PERFORMANCE_ERROR")
        self.metric = metric
        self.value = value
        self.threshold = threshold

def create_exception_from_cpp(cpp_exception: Exception, context: Optional[Dict[str, Any]] = None) -> NLMBaseException:
    """
    Convert C++ exceptions to appropriate Python NLM exceptions.
    
    Args:
        cpp_exception: The original C++ exception.
        context: Additional context information.
        
    Returns:
        Appropriate Python exception.
    """
    error_message = str(cpp_exception)
    
    # Map common C++ exception types to Python NLM exceptions
    if "runtime_error" in str(type(cpp_exception)):
        return NLMRuntimeError(error_message, original_exception=cpp_exception, context=context)
    elif "invalid_argument" in str(type(cpp_exception)):
        return NLMValidationError(error_message, original_exception=cpp_exception, context=context)
    elif "out_of_range" in str(type(cpp_exception)):
        return NLMIndexError(error_message, original_exception=cpp_exception, context=context)
    elif "logic_error" in str(type(cpp_exception)):
        return NLMError(error_message, original_exception=cpp_exception, context=context)
    elif "bad_alloc" in str(type(cpp_exception)):
        return NLMMemoryError(error_message, original_exception=cpp_exception, context=context)
    elif "length_error" in str(type(cpp_exception)):
        return NLMMemoryError(error_message, memory_type="length", original_exception=cpp_exception, context=context)
    elif "out_of_memory" in str(error_message).lower():
        return NLMMemoryError(error_message, original_exception=cpp_exception, context=context)
    
    # Default to generic NLM error
    return NLMError(error_message, original_exception=cpp_exception, context=context)

# Convenience functions for common error cases
def raise_config_error(message: str, filepath: Optional[str] = None,
                      original_exception: Optional[Exception] = None) -> None:
    """Raise a configuration error."""
    raise NLMConfigurationError(message, filepath, original_exception)

def raise_validation_error(message: str, field: Optional[str] = None,
                         value: Optional[Any] = None,
                         original_exception: Optional[Exception] = None) -> None:
    """Raise a validation error."""
    raise NLMValidationError(message, field, value, original_exception)

def raise_memory_error(message: str, original_exception: Optional[Exception] = None) -> None:
    """Raise a memory error."""
    raise NLMMemoryError(message, original_exception)

def raise_index_error(message: str, index: Optional[int] = None,
                     original_exception: Optional[Exception] = None) -> None:
    """Raise an index error."""
    raise NLMIndexError(message, index, original_exception)

def raise_type_error(message: str, original_exception: Optional[Exception] = None) -> None:
    """Raise a type error."""
    raise NLMTypeError(message, original_exception)

def raise_runtime_error(message: str, original_exception: Optional[Exception] = None) -> None:
    """Raise a runtime error."""
    raise NLMRuntimeError(message, original_exception)