# Custom exception classes for NLM

class NLMErrors(Exception):
    """Base exception class for all NLM errors."""
    pass
class InitializationError(NLMErrors):
    """Exception raised when initialization fails."""
    pass
class ConfigurationError(NLMErrors):
    """Exception raised when configuration is invalid."""
    pass
class SimulationError(NLMErrors):
    """Exception raised during simulation."""
    pass
class MemoryError(NLMErrors):
    """Exception raised when memory operations fail."""
    pass
class PlasticityError(NLMErrors):
    """Exception raised during plasticity operations."""
    pass
class DevelopmentError(NLMErrors):
    """Exception raised during development operations."""
    pass
class NeuromodulationError(NLMErrors):
    """Exception raised during neuromodulation operations."""
    pass
class WorldError(NLMErrors):
    """Exception raised when world operations fail."""
    pass
class AgentError(NLMErrors):
    """Exception raised when agent operations fail."""
    pass
class CheckpointError(NLMErrors):
    """Exception raised during checkpoint operations."""
    pass
class ExperimentError(NLMErrors):
    """Exception raised during experiment operations."""
    pass
class PerformanceError(NLMErrors):
    """Exception raised during performance operations."""
    pass