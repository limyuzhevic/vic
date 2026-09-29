"""
NLM Python Package - Core Modules

This package provides Python bindings for the NLM (Neural Learning Machine) 
computational brain simulator. It offers a high-level Python interface to the
underlying C++ implementation while maintaining full performance.

Key Features:
- Easy-to-use Python API for neural network simulation
- Event-driven spiking neural networks with LIF neurons
- Integrated memory, prediction, and neuromodulation systems
- World-based simulation environment for agent behavior
- Advanced plasticity rules (STDP, Hebbian, structural)
- Developmental processes and learning capabilities
"""

from .config import Config
from .brain import Brain
from .agent import AgentBrain
from .world import SimpleWorld
from .sensory import SensoryInput, Vision, Audio, InternalSignals
from .motor import Action, MotorCommand
from .exceptions import NLMError, ConfigurationError, SimulationError
from .cli import main as cli_main
from .benchmark import run_benchmark_suite, benchmark_configuration

__version__ = "0.1.0"

__all__ = [
    # Core classes
    "Config",
    "Brain", 
    "AgentBrain",
    "SimpleWorld",
    "Vision",
    "Audio",
    "InternalSignals",
    "Action",
    "MotorCommand",
    # Exception classes
    "NLMError",
    "ConfigurationError",
    "SimulationError",
    # CLI and benchmarking
    "cli_main",
    "run_benchmark_suite",
    "benchmark_configuration",
    # Factory functions (defined in __init__.py)
    "createDefaultConfig",
    "createBrain",
    "createSimpleWorld",
    "createAgentBrain",
]

# Package metadata
__author__ = "NLM Research Team"
__description__ = "Neural Learning Machine - Computational Brain Simulator"
__license__ = "MIT"
__repository__ = "https://github.com/nlm/nlm"
