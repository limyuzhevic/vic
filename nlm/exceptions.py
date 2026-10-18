"""
Custom exception classes for NLM Python bindings.

Provides Pythonic error handling with custom exception classes for different
error scenarios, making it easier to catch and handle specific errors.
"""

__all__ = [
    "NLMError",
    "ConfigurationError", 
    "SimulationError",
    "LearningError",
    "VisualizationError",
]

# Base NLM exception class
class NLMError(Exception):
    """Base exception for all NLM errors.
    
    This is the base class for all NLM-specific exceptions, allowing users
    to catch any NLM-related error using a single except clause.
    
    Args:
        message: Error message describing the issue.
        error_code: Optional error code for programmatic error handling.
        details: Optional additional error details for debugging.
    """
    
    def __init__(self, message: str, error_code: Optional[str] = None, 
                 details: Optional[dict] = None):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.details = details or {}
    
    def __str__(self) -> str:
        """String representation with error code if available."""
        if self.error_code:
            return f"[{self.error_code}] {self.message}"
        return self.message
    
    def __repr__(self) -> str:
        """Detailed representation for debugging."""
        return (f"{self.__class__.__name__}(message={self.message!r}, "
                f"error_code={self.error_code!r}, details={self.details!r})")

# Configuration-specific errors
class ConfigurationError(NLMError):
    """Error related to configuration operations.
    
    Raised when there are issues with loading, saving, or validating
    configuration data.
    
    Args:
        message: Error message describing the configuration issue.
        filepath: Optional path to the configuration file.
    """
    
    def __init__(self, message: str, filepath: Optional[str] = None):
        super().__init__(message, error_code="CONFIG_ERROR")
        self.filepath = filepath
    
    def __str__(self) -> str:
        if self.filepath:
            return f"[{self.error_code}] {self.message} (file: {self.filepath})"
        return f"[{self.error_code}] {self.message}"

# Simulation-specific errors
class SimulationError(NLMError):
    """Error during brain simulation.
    
    Raised when there are issues during brain simulation, such as
    invalid parameters, simulation failures, or step execution errors.
    
    Args:
        message: Error message describing the simulation issue.
        step: Optional simulation step where the error occurred.
    """
    
    def __init__(self, message: str, step: Optional[int] = None):
        super().__init__(message, error_code="SIMULATION_ERROR")
        self.step = step
    
    def __str__(self) -> str:
        if self.step is not None:
            return f"[{self.error_code}] {self.message} (step: {self.step})"
        return f"[{self.error_code}] {self.message}"

# Learning-specific errors
class LearningError(NLMError):
    """Error during learning operations.
    
    Raised when there are issues with learning algorithms, training,
    or reinforcement learning operations.
    
    Args:
        message: Error message describing the learning issue.
        algorithm: Optional learning algorithm that failed.
    """
    
    def __init__(self, message: str, algorithm: Optional[str] = None):
        super().__init__(message, error_code="LEARNING_ERROR")
        self.algorithm = algorithm
    
    def __str__(self) -> str:
        if self.algorithm:
            return f"[{self.error_code}] {self.message} (algorithm: {self.algorithm})"
        return f"[{self.error_code}] {self.message}"

# Visualization-specific errors
class VisualizationError(NLMError):
    """Error during visualization operations.
    
    Raised when there are issues with visualization, such as
    unsupported formats, rendering errors, or backend issues.
    
    Args:
        message: Error message describing the visualization issue.
        backend: Optional visualization backend that failed.
        format: Optional export format that caused the error.
    """
    
    def __init__(self, message: str, backend: Optional[str] = None, 
                 format: Optional[str] = None):
        super().__init__(message, error_code="VISUALIZATION_ERROR")
        self.backend = backend
        self.format = format
    
    def __str__(self) -> str:
        parts = [f"[{self.error_code}] {self.message}"]
        if self.backend:
            parts.append(f"backend: {self.backend}")
        if self.format:
            parts.append(f"format: {self.format}")
        return " ".join(parts)

# Helper functions for error handling
def catch_nlm_error(func):
    """
    Decorator to catch NLM-specific errors and re-raise them with better context.
    
    Args:
        func: Function to wrap.
    
    Returns:
        Wrapped function with enhanced error handling.
    """
    def wrapper(*args, **kwargs):
        try:
            return func(*args, **kwargs)
        except NLMError:
            raise
        except Exception as e:
            # Convert non-NLM errors to NLM errors
            if isinstance(e, (ValueError, TypeError, RuntimeError)):
                raise NLMError(f"Operation failed: {str(e)}", error_code="OPERATION_ERROR")
            raise
    return wrapper

def validate_parameter(value: Any, param_name: str, allowed_types: type) -> None:
    """
    Validate a parameter and raise NLMError if invalid.
    
    Args:
        value: Value to validate.
        param_name: Name of the parameter for error messages.
        allowed_types: Expected type(s) for the parameter.
    
    Raises:
        NLMError: If the value is not of the expected type.
    """
    if not isinstance(value, allowed_types):
        raise ConfigurationError(
            f"Invalid parameter '{param_name}': expected {allowed_types}, got {type(value)}",
            error_code="PARAM_VALIDATION_ERROR"
        )
