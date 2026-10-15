# pynlm - High-performance Python bindings for NLM
#
# This is the main Python module that exposes NLM's C++ neural simulation
# framework through a clean, Pythonic API.
#

__version__ = "0.1.0"
__author__ = "NLM Project"

# Import core functionality from submodules
from .core import (
    Config,
    createDefaultConfig,
    createBrain,
    getVersion,
    getBuildInfo,
    validateConfig,
)

from .brain import (
    Brain,
)

from .agent import (
    AgentBrain,
    createAgentBrain,
)

from .world import (
    SimpleWorld,
    createSimpleWorld,
)

from .memory import (
    NeuralWorkingMemory,
    NeuralEpisodicMemory,
    NeuralAssociativeMemory,
    getWorkingMemory,
    getEpisodicMemory,
    getAssociativeMemory,
)

from .neuromodulation import (
    Dopamine,
    Curiosity,
    Novelty,
    PredictionError,
)

from .sensory import (
    Vision,
    Audio,
    SensoryInput,
    InternalSignals,
)

from .motor import (
    MotorSystem,
    Action,
)

from .prediction import (
    PredictionSystem,
    NeuralPrediction,
)

from .cognition import (
    NeuralPlanner,
    ConceptFormation,
    AttentionalSelection,
)

from .development import (
    DevelopmentSystem,
)

from .environment import (
    Environment,
    Observation,
    Action as EnvAction,
)

from .exceptions import (
    NLMError,
    ConfigError,
    BrainError,
    MemoryError,
    EnvironmentError,
)

# Re-export for convenience
__all__ = [
    # Core
    "Config",
    "createDefaultConfig",
    "createBrain",
    "Brain",
    
    # Agent System
    "AgentBrain",
    "createAgentBrain",
    "SensoryPercept",
    "MotorCommand",
    
    # World System
    "SimpleWorld",
    "createSimpleWorld",
    "Observation",
    "Action",
    
    # Memory Systems
    "NeuralWorkingMemory",
    "NeuralEpisodicMemory",
    "NeuralAssociativeMemory",
    "getWorkingMemory",
    "getEpisodicMemory",
    "getAssociativeMemory",
    
    # Neuromodulation
    "Dopamine",
    "Curiosity",
    "Novelty",
    "PredictionError",
    
    # Sensory Systems
    "Vision",
    "Audio",
    "SensoryInput",
    "InternalSignals",
    
    # Motor Systems
    "MotorSystem",
    "Action",
    
    # Prediction System
    "PredictionSystem",
    "NeuralPrediction",
    
    # Cognitive Systems
    "NeuralPlanner",
    "ConceptFormation",
    "AttentionalSelection",
    
    # Development System
    "DevelopmentSystem",
    
    # Environment
    "Environment",
    "Observation",
    
    # Error Handling
    "NLMError",
    "ConfigError",
    "BrainError",
    "MemoryError",
    "EnvironmentError",
    
    # Utilities
    "getVersion",
    "getBuildInfo",
    "validateConfig",
]

# Print startup message
import sys
print(f"NLM Python bindings v{__version__} loaded successfully.")
print(f"Run 'import pynlm; help(pynlm)' for comprehensive documentation.")
