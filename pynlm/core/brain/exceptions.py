"""Exception definitions for NLM brain simulation."""

from ...exceptions import NLMException, SimulationError, ConfigurationError, IntegrationError


class BrainError(NLMException):
    """Brain-specific errors."""
    
    def __init__(self, message: str):
        super().__init__(message, error_code=9000)


class NeuronError(NLMException):
    """Neuron-specific errors."""
    
    def __init__(self, message: str):
        super().__init__(message, error_code=9100)


class SynapseError(NLMException):
    """Synapse-specific errors."""
    
    def __init__(self, message: str):
        super().__init__(message, error_code=9200)


class MemoryError(NLMException):
    """Memory-related errors."""
    
    def __init__(self, message: str):
        super().__init__(message, error_code=9300)


class NeuromodulationError(NLMException):
    """Neuromodulation-specific errors."""
    
    def __init__(self, message: str):
        super().__init__(message, error_code=9400)


class DevelopmentalError(NLMException):
    """Development-specific errors."""
    
    def __init__(self, message: str):
        super().__init__(message, error_code=9500)


class PredictionError(NLMException):
    """Prediction-specific errors."""
    
    def __init__(self, message: str):
        super().__init__(message, error_code=9600)


class PlasticityError(NLMException):
    """Plasticity-specific errors."""
    
    def __init__(self, message: str):
        super().__init__(message, error_code=9700)