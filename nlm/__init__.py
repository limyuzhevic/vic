"""
NLM - Neural Learning Machine Python Bindings

A comprehensive Python API for the Neural Learning Machine (NLM) brain simulator,
providing a Pythonic interface to advanced neural computation and learning systems.
"""

__version__ = "0.1.0"

from .core import Brain, Config, SimpleWorld, AgentBrain
from .agent import AdvancedActions, ActionSequence, ActionTemplate
from .visualization import AdvancedVisualization, VisualizationConfig
from .configuration import AdvancedConfig, DynamicConfigManager
from .learning import AdvancedLearning, LearningConfig
from .experiments import ExperimentManager

__all__ = [
    # Core components
    "Brain", "Config", "SimpleWorld", "AgentBrain",
    # Agent operations
    "AdvancedActions", "ActionSequence", "ActionTemplate",
    # Visualization
    "AdvancedVisualization", "VisualizationConfig",
    # Configuration
    "AdvancedConfig", "DynamicConfigManager",
    # Learning
    "AdvancedLearning", "LearningConfig",
    # Experiments
    "ExperimentManager",
    # Version
    "__version__",
]

# Convenience factory functions
def create_default_config():
    """Create a default configuration."""
    return Config()

def create_brain(config=None):
    """Create a new brain with configuration."""
    if config is None:
        config = create_default_config()
    return Brain(config)

def create_simple_world():
    """Create a new simple world."""
    return SimpleWorld()

def create_agent_brain(brain):
    """Create a new agent brain interface."""
    return AgentBrain(brain)

# Pythonic context managers
class NLMContext:
    """Context manager for NLM components that require cleanup."""
    
    def __init__(self, component, *args, **kwargs):
        self.component = component
        self.args = args
        self.kwargs = kwargs
        self._instance = None
    
    def __enter__(self):
        if self.component == "brain":
            self._instance = create_brain(*self.args, **self.kwargs)
        elif self.component == "world":
            self._instance = create_simple_world(*self.args, **self.kwargs)
        elif self.component == "agent":
            self._instance = create_agent_brain(*self.args, **self.kwargs)
        else:
            raise ValueError(f"Unknown component: {self.component}")
        return self._instance
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if hasattr(self._instance, 'reset'):
            self._instance.reset()
        self._instance = None

# Custom exceptions for Pythonic error handling
class NLMError(Exception):
    """Base exception for all NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Error related to configuration."""
    pass

class SimulationError(NLMError):
    """Error during simulation."""
    pass

class LearningError(NLMError):
    """Error during learning."""
    pass

class VisualizationError(NLMError):
    """Error during visualization."""
    pass

# Type hints for better IDE support
from typing import (
    Any, Dict, List, Optional, Tuple, Union, Iterator, ContextManager,
    Callable, TypeVar, Generic, Type
)

T = TypeVar('T')

# Protocol definitions for better type checking
from typing_extensions import Protocol

class NLMComponent(Protocol):
    """Protocol for all NLM components."""
    
    def initialize(self) -> bool:
        ...
    
    def reset(self) -> None:
        ...

class Configurable(Protocol):
    """Protocol for configurable components."""
    
    def load_from_file(self, filepath: str) -> None:
        ...
    
    def save_to_file(self, filepath: str) -> None:
        ...
    
    def get_config(self, key: str) -> Any:
        ...
    
    def set_config(self, key: str, value: Any) -> None:
        ...
