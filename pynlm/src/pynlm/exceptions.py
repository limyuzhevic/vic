from typing import Any


class NLMError(Exception):
    """Base exception for all NLM errors."""
    pass


class NLMConfigurationError(NLMError):
    """Raised when there's an error in configuration."""
    pass


class NLMInitializationError(NLMError):
    """Raised when brain or simulation fails to initialize."""
    pass


class NLMInvalidStateError(NLMError):
    """Raised when an operation is invalid in the current state."""
    pass


class NLMSimulationError(NLMError):
    """Raised when simulation fails during execution."""
    pass


class NLMResourceError(NLMError):
    """Raised when there are resource allocation issues."""
    pass


class NLMVersionError(NLMError):
    """Raised when there's a version compatibility issue."""
    pass
