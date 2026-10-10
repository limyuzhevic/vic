"""NLM Exception Classes"""

class NLMError(Exception):
    """Base exception for all NLM errors"""
    pass

class ConfigurationError(NLMError):
    """Exception raised for configuration errors"""
    pass

class SimulationError(NLMError):
    """Exception raised for simulation errors"""
    pass

class ValidationError(NLMError):
    """Exception raised for validation errors"""
    pass