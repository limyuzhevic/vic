"""
Custom exceptions for NLM Python bindings
"""

class NLMError(Exception):
    """Base exception for all NLM errors"""
    pass

class NLMConfigurationError(NLMError):
    """Exception for configuration-related errors"""
    pass

class NLMBrainError(NLMError):
    """Exception for brain-related errors"""
    pass

class NLMSimulationError(NLMError):
    """Exception for simulation-related errors"""
    pass

class NLMFileError(NLMError):
    """Exception for file I/O errors"""
    pass

class NLMMemoryError(NLMError):
    """Exception for memory-related errors"""
    pass

class NLMNeuromodulationError(NLMError):
    """Exception for neuromodulation-related errors"""
    pass

class NLMDevelopmentError(NLMError):
    """Exception for development-related errors"""
    pass
