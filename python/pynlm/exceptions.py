"""
Custom exception hierarchy for pynlm.

Provides specific exception types for different categories of errors
that may occur during NLM (Neural Learning Machine) operations.
"""

from typing import Any, Dict, Optional


class NLMError(Exception):
    """Base exception for all NLM-related errors.
    
    Attributes:
        message: Human-readable error message
        code: Error code for programmatic handling
        details: Optional dictionary with additional error context
    """
    
    def __init__(self, message: str, code: Optional[str] = None, 
                 details: Optional[Dict[str, Any]] = None):
        super().__init__(message)
        self.message = message
        self.code = code
        self.details = details or {}
    
    def __str__(self) -> str:
        if self.code:
            return f"[{self.code}] {self.message}"
        return self.message


class ConfigurationError(NLMError):
    """Exception raised for configuration-related errors."""
    
    def __init__(self, message: str, key: Optional[str] = None, 
                 value: Optional[Any] = None):
        super().__init__(message, code="CONFIG_ERROR")
        self.key = key
        self.value = value


class SimulationError(NLMError):
    """Exception raised for simulation-related errors."""
    
    def __init__(self, message: str, step: Optional[int] = None, 
                 component: Optional[str] = None):
        super().__init__(message, code="SIMULATION_ERROR")
        self.step = step
        self.component = component


class MemoryError(NLMError):
    """Exception raised for memory-related errors."""
    
    def __init__(self, message: str, memory_usage: Optional[float] = None):
        super().__init__(message, code="MEMORY_ERROR")
        self.memory_usage = memory_usage


class ValidationError(NLMError):
    """Exception raised for validation-related errors."""
    
    def __init__(self, message: str, field: Optional[str] = None, 
                 value: Optional[Any] = None, constraints: Optional[Dict[str, Any]] = None):
        super().__init__(message, code="VALIDATION_ERROR")
        self.field = field
        self.value = value
        self.constraints = constraints


class AgentError(NLMError):
    """Exception raised for agent-related errors."""
    
    def __init__(self, message: str, agent_id: Optional[str] = None, 
                 action: Optional[str] = None):
        super().__init__(message, code="AGENT_ERROR")
        self.agent_id = agent_id
        self.action = action


class WorldError(NLMError):
    """Exception raised for world-related errors."""
    
    def __init__(self, message: str, world_id: Optional[str] = None, 
                 position: Optional[tuple] = None):
        super().__init__(message, code="WORLD_ERROR")
        self.world_id = world_id
        self.position = position


# Convenience functions for raising specific exceptions
def raise_config_error(message: str, key: Optional[str] = None, 
                     value: Optional[Any] = None) -> None:
    """Raise a ConfigurationError with the given details."""
    raise ConfigurationError(message, key, value)


def raise_simulation_error(message: str, step: Optional[int] = None, 
                         component: Optional[str] = None) -> None:
    """Raise a SimulationError with the given details."""
    raise SimulationError(message, step, component)


def raise_validation_error(message: str, field: Optional[str] = None,
                         value: Optional[Any] = None,
                         constraints: Optional[Dict[str, Any]] = None) -> None:
    """Raise a ValidationError with the given details."""
    raise ValidationError(message, field, value, constraints)


def raise_memory_error(message: str, memory_usage: Optional[float] = None) -> None:
    """Raise a MemoryError with the given details."""
    raise MemoryError(message, memory_usage)


def raise_agent_error(message: str, agent_id: Optional[str] = None,
                    action: Optional[str] = None) -> None:
    """Raise an AgentError with the given details."""
    raise AgentError(message, agent_id, action)


def raise_world_error(message: str, world_id: Optional[str] = None,
                    position: Optional[tuple] = None) -> None:
    """Raise a WorldError with the given details."""
    raise WorldError(message, world_id, position)
