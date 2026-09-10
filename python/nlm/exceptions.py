"""
NLM exceptions and error handling.

Custom exceptions for NLM Python API.
"""
class NLMRuntimeError(Exception):
    """Base runtime error for NLM operations."""
    
    def __init__(self, message: str, details: Optional[Dict[str, Any]] = None):
        """Initialize NLM runtime error.
        
        Args:
            message: Error message
            details: Optional error details
        """
        super().__init__(message)
        self.message = message
        self.details = details or {}
        self.timestamp = time.time()
    
    def __str__(self) -> str:
        return f"NLMRuntimeError: {self.message}"
class NLMConfigurationError(NLMRuntimeError):
    """Error in NLM configuration."""
    
    def __init__(self, message: str, details: Optional[Dict[str, Any]] = None):
        super().__init__(message, details)
        self.error_type = "configuration"
class NLMValidationError(NLMRuntimeError):
    """Error in NLM validation."""
    
    def __init__(self, message: str, details: Optional[Dict[str, Any]] = None):
        super().__init__(message, details)
        self.error_type = "validation"
class NLMMemoryError(NLMRuntimeError):
    """Error in NLM memory management."""
    
    def __init__(self, message: str, details: Optional[Dict[str, Any]] = None):
        super().__init__(message, details)
        self.error_type = "memory"
class NLMCompatibilityError(NLMRuntimeError):
    """Error in NLM compatibility."""
    
    def __init__(self, message: str, details: Optional[Dict[str, Any]] = None):
        super().__init__(message, details)
        self.error_type = "compatibility"
class NLMNotImplementedError(NLMRuntimeError):
    """Error for not implemented features."""
    
    def __init__(self, feature: str, message: Optional[str] = None):
        msg = message or f"Feature '{feature}' not implemented"
        super().__init__(msg)
        self.feature = feature
        self.error_type = "not_implemented"