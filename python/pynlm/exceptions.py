"""
Custom exceptions for NLM Python bindings.

This module provides custom exception classes for NLM-specific error
conditions and error handling.
"""

from typing import Any
class NLMError(Exception):
    """
    Base exception class for all NLM errors.
    
    This is the base exception class for all NLM-specific errors.
    All custom NLM exceptions should inherit from this class.
    """

    def __init__(self, message: str, details: Any = None):
        """
        Initialize NLMError.
        
        Args:
            message: Error message
            details: Optional error details
        """
        super().__init__(message)
        self.message = message
        self.details = details

    def __str__(self) -> str:
        """String representation."""
        if self.details:
            return f"{self.message} (details: {self.details})"
        return self.message
class ConfigurationError(NLMError):
    """
    Exception raised for configuration errors.
    
    This exception is raised when there are errors in configuration,
    such as invalid parameters or missing required settings.
    """

    pass
class InitializationError(NLMError):
    """
    Exception raised for initialization errors.
    
    This exception is raised when there are errors during initialization
    of components or modules.
    """

    pass
class SimulationError(NLMError):
    """
    Exception raised for simulation errors.
    
    This exception is raised when there are errors during simulation,
    such as invalid simulation steps or runtime errors.
    """

    pass
class ResourceError(NLMError):
    """
    Exception raised for resource errors.
    
    This exception is raised when there are errors related to resource
    management, such as memory allocation failures or file I/O errors.
    """

    pass