"""
NLM Exception class
Custom exception hierarchy for NLM errors.
"""

class NLMException(Exception):
    """Base exception class for NLM errors."""
    
    def __init__(self, message: str, error_code: int = 1, details: dict = None):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.details = details or {}
    
    def __str__(self) -> str:
        if self.error_code != 1:
            return f"NLM Error {self.error_code}: {self.message}"
        return f"NLM Error: {self.message}"
    
    def __repr__(self) -> str:
        return f"NLMException(message={self.message!r}, error_code={self.error_code}, details={self.details})"


class BrainCreationError(NLMException):
    """Exception raised when brain creation fails."""
    
    def __init__(self, message: str = "Brain creation failed", details: dict = None):
        super().__init__(message, error_code=2, details=details)


class SimulationError(NLMException):
    """Exception raised when simulation fails."""
    
    def __init__(self, message: str = "Simulation failed", details: dict = None):
        super().__init__(message, error_code=3, details=details)


class ConfigurationError(NLMException):
    """Exception raised when configuration operations fail."""
    
    def __init__(self, message: str = "Configuration error", details: dict = None):
        super().__init__(message, error_code=4, details=details)


class WorldError(NLMException):
    """Exception raised when world operations fail."""
    
    def __init__(self, message: str = "World error", details: dict = None):
        super().__init__(message, error_code=5, details=details)


class AgentError(NLMException):
    """Exception raised when agent operations fail."""
    
    def __init__(self, message: str = "Agent error", details: dict = None):
        super().__init__(message, error_code=6, details=details)


class ValidationError(NLMException):
    """Exception raised when validation fails."""
    
    def __init__(self, message: str = "Validation error", details: dict = None):
        super().__init__(message, error_code=7, details=details)
