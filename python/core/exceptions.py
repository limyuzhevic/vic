"""
Custom exception classes for NLM Python package.

These provide more specific error handling than the basic C++ exceptions,
making it easier to catch and handle specific error conditions in Python.
"""

class NLMError(Exception):
    """Base exception for all NLM errors."""
    pass

class ConfigError(NLMError):
    """Exception raised for configuration-related errors."""
    pass

class BrainError(NLMError):
    """Exception raised for brain-related errors."""
    pass

class WorldError(NLMError):
    """Exception raised for world-related errors."""
    pass

class SensorError(NLMError):
    """Exception raised for sensor-related errors."""
    pass

class AgentError(NLMError):
    """Exception raised for agent-related errors."""
    pass

class ValidationError(NLMError):
    """Exception raised for validation errors."""
    pass

class MemoryError(NLMError):
    """Exception raised for memory-related errors."""
    pass

class SimulationError(NLMError):
    """Exception raised for simulation errors."""
    pass

class DevelopmentalError(NLMError):
    """Exception raised for developmental stage errors."""
    pass

# Re-export exceptions for backward compatibility
__all__ = [
    'NLMError', 'ConfigError', 'BrainError', 'WorldError',
    'SensorError', 'AgentError', 'ValidationError', 'MemoryError',
    'SimulationError', 'DevelopmentalError'
]
