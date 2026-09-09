"""
NLM Python Exceptions

This module contains exception classes for the NLM Python bindings.
"""

from typing import Optional, Dict, Any

class NLMError(Exception):
    """Base exception for all NLM errors."""
    
    def __init__(self, message: str, error_code: Optional[str] = None, 
                 details: Optional[Dict[str, Any]] = None):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.details = details or {}
    
    def __str__(self) -> str:
        if self.error_code:
            return f"[{self.error_code}] {self.message}"
        return self.message
class ConfigError(NLMError):
    """Error in configuration."""
    
    def __init__(self, message: str, key: Optional[str] = None):
        super().__init__(message, error_code="CONFIG_ERROR", 
                        details={"key": key})
class BrainError(NLMError):
    """Error in brain operations."""
    
    def __init__(self, message: str, brain_state: Optional[str] = None):
        super().__init__(message, error_code="BRAIN_ERROR",
                        details={"brain_state": brain_state})
class NeuronError(NLMError):
    """Error in neuron operations."""
    
    def __init__(self, message: str, neuron_id: Optional[int] = None):
        super().__init__(message, error_code="NEURON_ERROR",
                        details={"neuron_id": neuron_id})
class SynapseError(NLMError):
    """Error in synapse operations."""
    
    def __init__(self, message: str, synapse_id: Optional[int] = None):
        super().__init__(message, error_code="SYNAPSE_ERROR",
                        details={"synapse_id": synapse_id})
class SimulationError(NLMError):
    """Error during simulation."""
    
    def __init__(self, message: str, step: Optional[int] = None):
        super().__init__(message, error_code="SIMULATION_ERROR",
                        details={"step": step})
class MemoryError(NLMError):
    """Error in memory operations."""
    
    def __init__(self, message: str, memory_type: Optional[str] = None):
        super().__init__(message, error_code="MEMORY_ERROR",
                        details={"memory_type": memory_type})
class PredictionError(NLMError):
    """Error in prediction operations."""
    
    def __init__(self, message: str, prediction_type: Optional[str] = None):
        super().__init__(message, error_code="PREDICTION_ERROR",
                        details={"prediction_type": prediction_type})
class NeuromodulationError(NLMError):
    """Error in neuromodulation operations."""
    
    def __init__(self, message: str, neuromodulator: Optional[str] = None):
        super().__init__(message, error_code="NEUROMODULATION_ERROR",
                        details={"neuromodulator": neuromodulator})


# Export public interface
__all__ = [
    'NLMError',
    'ConfigError',
    'BrainError',
    'NeuronError',
    'SynapseError',
    'SimulationError',
    'MemoryError',
    'PredictionError',
    'NeuromodulationError',
]
