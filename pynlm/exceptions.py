"""
Exception classes for NLM Python API
"""

import time

class NLMError(Exception):
    """Base exception for NLM-related errors"""
    pass

class ConfigurationError(NLMError):
    """Error in configuration"""
    pass

class BrainError(NLMError):
    """Error in brain simulation"""
    pass

class WorldError(NLMError):
    """Error in world simulation"""
    pass

class SensoryError(NLMError):
    """Error in sensory processing"""
    pass

class MotorError(NLMError):
    """Error in motor control"""
    pass

class DevelopmentError(NLMError):
    """Error in developmental system"""
    pass

class MemoryError(NLMError):
    """Error in memory systems"""
    pass

class PredictionError(NLMError):
    """Error in prediction systems"""
    pass

class NeuromodulationError(NLMError):
    """Error in neuromodulation systems"""
    pass

# Runtime error alias for compatibility
RuntimeError = NLMError
