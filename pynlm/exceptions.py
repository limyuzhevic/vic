"""
Custom exceptions for NLM Python bindings.
Provides a comprehensive exception hierarchy for different error types.
"""

import sys
from typing import Optional, Dict, Any
class NLMError(Exception):
    """Base exception for all NLM errors."""
    
    def __init__(self, message: str, error_code: str = "NLM_ERROR"):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.timestamp = self._get_timestamp()
    
    def _get_timestamp(self) -> str:
        import datetime
        return datetime.datetime.now().isoformat()
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert error to dictionary for serialization."""
        return {
            "error_type": self.__class__.__name__,
            "message": self.message,
            "error_code": self.error_code,
            "timestamp": self.timestamp
        }
    
    def __str__(self) -> str:
        return f"[{self.error_code}] {self.message}"
class ConfigurationError(NLMError):
    """Exception raised for configuration errors."""
    
    def __init__(self, message: str, field: Optional[str] = None):
        super().__init__(message, "CONFIG_ERROR")
        self.field = field
    
    def to_dict(self) -> Dict[str, Any]:
        data = super().to_dict()
        data["field"] = self.field
        return data
class SimulationError(NLMError):
    """Exception raised for simulation errors."""
    
    def __init__(self, message: str, simulation_step: Optional[int] = None):
        super().__init__(message, "SIMULATION_ERROR")
        self.simulation_step = simulation_step
    
    def to_dict(self) -> Dict[str, Any]:
        data = super().to_dict()
        data["simulation_step"] = self.simulation_step
        return data
class MemoryError(NLMError):
    """Exception raised for memory-related errors."""
    
    def __init__(self, message: str, memory_address: Optional[str] = None):
        super().__init__(message, "MEMORY_ERROR")
        self.memory_address = memory_address
    
    def to_dict(self) -> Dict[str, Any]:
        data = super().to_dict()
        data["memory_address"] = self.memory_address
        return data
class ValidationError(NLMError):
    """Exception raised for validation errors."""
    
    def __init__(self, message: str, validation_type: Optional[str] = None, invalid_value: Optional[Any] = None):
        super().__init__(message, "VALIDATION_ERROR")
        self.validation_type = validation_type
        self.invalid_value = invalid_value
    
    def to_dict(self) -> Dict[str, Any]:
        data = super().to_dict()
        data["validation_type"] = self.validation_type
        data["invalid_value"] = self.invalid_value
        return data
class AgentError(NLMError):
    """Exception raised for agent-related errors."""
    
    def __init__(self, message: str, agent_action: Optional[str] = None):
        super().__init__(message, "AGENT_ERROR")
        self.agent_action = agent_action
    
    def to_dict(self) -> Dict[str, Any]:
        data = super().to_dict()
        data["agent_action"] = self.agent_action
        return data
class WorldError(NLMError):
    """Exception raised for world-related errors."""
    
    def __init__(self, message: str, world_object: Optional[str] = None):
        super().__init__(message, "WORLD_ERROR")
        self.world_object = world_object
    
    def to_dict(self) -> Dict[str, Any]:
        data = super().to_dict()
        data["world_object"] = self.world_object
        return data
# Global error registry for tracking error occurrences
class ErrorRegistry:
    """Registry for tracking error occurrences and statistics."""
    
    def __init__(self):
        self.errors: List[NLMError] = []
        self.error_counts: Dict[str, int] = {}
    
    def record_error(self, error: NLMError) -> None:
        """Record an error occurrence."""
        self.errors.append(error)
        self.error_counts[error.error_code] = self.error_counts.get(error.error_code, 0) + 1
    
    def get_error_count(self, error_code: str) -> int:
        """Get count of errors with specific error code."""
        return self.error_counts.get(error_code, 0)
    
    def get_errors_by_type(self, error_type: str) -> List[NLMError]:
        """Get all errors of a specific type."""
        return [e for e in self.errors if isinstance(e, error_type)]
    
    def get_summary(self) -> Dict[str, Any]:
        """Get error registry summary."""
        return {
            "total_errors": len(self.errors),
            "error_types": self.error_counts,
            "most_common_error": max(self.error_counts.items(), key=lambda x: x[1], default=("none", 0))[0]
        }
    
    def clear(self) -> None:
        """Clear all recorded errors."""
        self.errors.clear()
        self.error_counts.clear()

# Global error registry instance
error_registry = ErrorRegistry()

# Global exception handler
def handle_exception(exc_type, exc_value, exc_traceback):
    """Global exception handler for NLM errors."""
    if issubclass(exc_type, NLMError):
        error = exc_value
        error_registry.record_error(error)
        
        # Log error with detailed information
        import traceback
        print(f"NLM Error: {error}")
        print(f"Error Type: {error.error_code}")
        print(f"Timestamp: {error.timestamp}")
        print("Stack Trace:")
        print(traceback.format_tb(exc_traceback))
        print("-" * 50)

# Install global exception handler
import sys
if not sys.platform.startswith("win"):
    import signal
    
    def signal_handler(signum, frame):
        handle_exception(SimulationError, SimulationError(f"Program terminated by signal {signum}"), None)
        sys.exit(1)
    
    signal.signal(signal.SIGTERM, signal_handler)
    signal.signal(signal.SIGINT, signal_handler)
# Function to convert Python exception to NLM exception
def convert_to_nlm_error(exc: Exception) -> NLMError:
    """Convert Python exception to NLM exception if possible."""
    if isinstance(exc, NLMError):
        return exc
    
    # Convert common Python exceptions to NLM exceptions
    if isinstance(exc, ValueError):
        return ConfigurationError(str(exc))
    elif isinstance(exc, TypeError):
        return ConfigurationError(f"Type error: {str(exc)}")
    elif isinstance(exc, AttributeError):
        return SimulationError(f"Missing attribute: {str(exc)}")
    elif isinstance(exc, IndexError):
        return MemoryError(f"Index error: {str(exc)}")
    elif isinstance(exc, KeyError):
        return ConfigurationError(f"Missing key: {str(exc)}")
    
    # Default to generic NLM error
    return NLMError(f"Unexpected error: {str(exc)}")

# Context manager for error handling
def error_handling_context():
    """Context manager for automatic error handling."""
    class ErrorHandlingContext:
        def __enter__(self):
            return self
        
        def __exit__(self, exc_type, exc_value, exc_traceback):
            if exc_type is not None:
                try:
                    nlm_error = convert_to_nlm_error(exc_value)
                    error_registry.record_error(nlm_error)
                    
                    # Re-raise as NLM error if it's not already
                    if not isinstance(exc_value, NLMError):
                        raise nlm_error
                except Exception:
                    # If conversion fails, raise original exception
                    pass
            
            return False  # Don't suppress exceptions
    
    return ErrorHandlingContext()
# Utility function to get error statistics
def get_error_statistics() -> Dict[str, Any]:
    """Get statistics about recorded errors."""
    return error_registry.get_summary()

# Function to clear error history
def clear_error_history() -> None:
    """Clear error history."""
    error_registry.clear()

__all__ = [
    "NLMError",
    "ConfigurationError",
    "SimulationError",
    "MemoryError",
    "ValidationError",
    "AgentError",
    "WorldError",
    "ErrorRegistry",
    "error_registry",
    "handle_exception",
    "convert_to_nlm_error",
    "error_handling_context",
    "get_error_statistics",
    "clear_error_history"
]
