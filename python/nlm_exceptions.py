"""Custom exception hierarchy for NLM Python bindings"""

import typing
from enum import Enum
from typing import Optional, Dict, Any


class NLMPythonError(Exception):
    """Base exception for all NLM Python errors."""
    
    def __init__(self, message: str, context: Optional[Dict[str, Any]] = None):
        super().__init__(message)
        self.message = message
        self.context = context or {}
        self.timestamp = None
        
    def __str__(self) -> str:
        if self.context:
            context_str = ", ".join(f"{k}={v}" for k, v in self.context.items())
            return f"{self.message} (Context: {context_str})"
        return self.message
    
    def add_context(self, key: str, value: Any) -> None:
        """Add context information to this exception."""
        self.context[key] = value


class ConfigurationError(NLMPythonError):
    """Exception raised for configuration-related errors."""
    
    def __init__(self, message: str, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, context or {"error_type": "configuration"})


class ConfigValidationError(ConfigurationError):
    """Exception raised for configuration validation errors."""
    
    def __init__(self, message: str, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, {**(context or {}), "error_type": "validation"})


class ConfigFileError(ConfigurationError):
    """Exception raised for configuration file I/O errors."""
    
    def __init__(self, message: str, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, {**(context or {}), "error_type": "file_io"})


class BrainError(NLMPythonError):
    """Exception raised for brain-related errors."""
    
    def __init__(self, message: str, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, context or {"error_type": "brain"})


class BrainInitializationError(BrainError):
    """Exception raised for brain initialization errors."""
    
    def __init__(self, message: str, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, {**(context or {}), "error_type": "initialization"})


class BrainValidationError(BrainError):
    """Exception raised for brain validation errors."""
    
    def __init__(self, message: str, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, {**(context or {}), "error_type": "validation"})


class MemorySystemError(NLMPythonError):
    """Exception raised for memory system errors."""
    
    def __init__(self, message: str, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, context or {"error_type": "memory_system"})


class PredictionSystemError(NLMPythonError):
    """Exception raised for prediction system errors."""
    
    def __init__(self, message: str, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, context or {"error_type": "prediction_system"})


class CognitionSystemError(NLMPythonError):
    """Exception raised for cognition system errors."""
    
    def __init__(self, message: str, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, context or {"error_type": "cognition_system"})


class NeuromodulationSystemError(NLMPythonError):
    """Exception raised for neuromodulation system errors."""
    
    def __init__(self, message: str, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, context or {"error_type": "neuromodulation_system"})


class ResourceError(NLMPythonError):
    """Exception raised for resource-related errors."""
    
    def __init__(self, message: str, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, context or {"error_type": "resource"})


class MemoryAllocationError(ResourceError):
    """Exception raised for memory allocation errors."""
    
    def __init__(self, message: str, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, {**(context or {}), "error_type": "memory_allocation"})


class FileHandleError(ResourceError):
    """Exception raised for file handle errors."""
    
    def __init__(self, message: str, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, {**(context or {}), "error_type": "file_handle"})


# Exception type registry for quick lookup
EXCEPTION_TYPES = {
    "NLMPythonError": NLMPythonError,
    "ConfigurationError": ConfigurationError,
    "ConfigValidationError": ConfigValidationError,
    "ConfigFileError": ConfigFileError,
    "BrainError": BrainError,
    "BrainInitializationError": BrainInitializationError,
    "BrainValidationError": BrainValidationError,
    "MemorySystemError": MemorySystemError,
    "PredictionSystemError": PredictionSystemError,
    "CognitionSystemError": CognitionSystemError,
    "NeuromodulationSystemError": NeuromodulationSystemError,
    "ResourceError": ResourceError,
    "MemoryAllocationError": MemoryAllocationError,
    "FileHandleError": FileHandleError,
}


def get_exception_class(exception_type: str) -> type:
    """Get exception class by name."""
    return EXCEPTION_TYPES.get(exception_type, NLMPythonError)


def translate_cpp_exception_to_python(cpp_exception: Exception) -> NLMPythonError:
    """Translate C++ exception to appropriate Python exception."""
    import traceback
    
    # Map C++ exception types to Python exception types
    if isinstance(cpp_exception, (ValueError, TypeError)):
        return ConfigValidationError(str(cpp_exception))
    elif isinstance(cpp_exception, (std::bad_alloc, MemoryError)):
        return MemoryAllocationError(str(cpp_exception))
    elif isinstance(cpp_exception, (IOError, OSError)):
        return ConfigFileError(str(cpp_exception))
    elif isinstance(cpp_exception, RuntimeError):
        return NLMPythonError(str(cpp_exception))
    else:
        # Default to generic NLM error
        return NLMPythonError(f"C++ exception: {str(cpp_exception)}")


# Global exception registry for statistics
exception_stats = {
    "total_caught": 0,
    "type_counts": {},
    "last_exception": None,
    "exception_history": [],
}


def record_exception(exception: NLMPythonError) -> None:
    """Record exception for statistics and debugging."""
    exception_stats["total_caught"] += 1
    exc_type = type(exception).__name__
    exception_stats["type_counts"][exc_type] = exception_stats["type_counts"].get(exc_type, 0) + 1
    exception_stats["last_exception"] = exception
    exception_stats["exception_history"].append({
        "type": exc_type,
        "message": str(exception),
        "timestamp": exception.timestamp,
        "context": exception.context
    })
    
    # Keep only last 100 exceptions in history
    if len(exception_stats["exception_history"]) > 100:
        exception_stats["exception_history"] = exception_stats["exception_history"][-100:]


def get_exception_statistics() -> Dict[str, Any]:
    """Get exception statistics for monitoring."""
    return {
        "total_caught": exception_stats["total_caught"],
        "type_counts": exception_stats["type_counts"].copy(),
        "last_exception": {
            "type": type(exception_stats["last_exception"]).__name__,
            "message": str(exception_stats["last_exception"]),
            "timestamp": exception_stats["last_exception"].timestamp
        } if exception_stats["last_exception"] else None,
        "exception_history": exception_stats["exception_history"].copy()
    }


# Context manager for exception handling
class NLMTryCatch:
    """Context manager for NLM exception handling."""
    
    def __init__(self, exception_types: typing.Optional[typing.Union[type, typing.Tuple[type, ...]]] = None):
        self.exception_types = exception_types
        self.caught_exception = None
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if exc_type is not None:
            if self.exception_types is None or issubclass(exc_type, self.exception_types):
                # Translate and record exception
                if isinstance(exc_val, NLMPythonError):
                    python_exception = exc_val
                else:
                    python_exception = translate_cpp_exception_to_python(exc_val)
                
                record_exception(python_exception)
                self.caught_exception = python_exception
                return True  # Suppress the original exception
        return False


def nlm_try(*exception_types):
    """Decorator for NLM exception handling."""
    def decorator(func):
        def wrapper(*args, **kwargs):
            try:
                return func(*args, **kwargs)
            except Exception as e:
                if exception_types and not any(isinstance(e, exc_type) for exc_type in exception_types):
                    raise
                # Translate C++ exceptions
                if isinstance(e, NLMPythonError):
                    python_exception = e
                else:
                    python_exception = translate_cpp_exception_to_python(e)
                record_exception(python_exception)
                raise python_exception
        return wrapper
    return decorator