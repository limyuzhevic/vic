"""
Exception classes for NLM Python bindings.

Provides custom exception classes for better error handling and debugging
in NLM simulations.
"""

__all__ = ['NLMError', 'ConfigurationError', 'SimulationError', 'ValidationError']


class NLMError(Exception):
    """Base exception class for all NLM-related errors."""
    pass


class ConfigurationError(NLMError):
    """Exception raised for configuration-related errors."""
    pass


class SimulationError(NLMError):
    """Exception raised for simulation-related errors."""
    pass


class ValidationError(NLMError):
    """Exception raised for validation errors."""
    pass
