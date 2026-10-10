"""
NLM Python Bindings - Enhanced Python API

This is the main package entry point for the improved Python bindings of the NLM
neural simulation framework. The bindings provide a clean, Pythonic interface to
the underlying C++ implementation while maintaining full compatibility.

Key Improvements:
- Modular design with separate modules for different components
- Comprehensive type hints and documentation
- Convenience functions for common operations
- Robust error handling and validation
- Example scripts demonstrating usage
- Full test suite for verification

The package wraps the existing pybind11 C++ extension (`pynlm`) and provides
a more intuitive, Pythonic interface while preserving all original functionality.

Usage Examples:

Basic Usage:
    >>> from pynlm import Config, Brain, SimpleWorld
    >>> config = Config(neuron_count=100)
    >>> brain = Brain(config)
    >>> brain.initialize()
    >>> world = SimpleWorld()
    >>> world.configure(64, 64)
    >>> # ... run simulation

Advanced Usage:
    >>> from pynlm import create_default_config, create_brain
    >>> >>> config = create_default_config()
    >>> brain = create_brain(config)
    >>> action = create_movement_action("forward", 1.0)

The package includes the following main modules:

- core: Configuration management and basic types
- brain: Neural brain simulation and control
- agent: Agent brain interface for world interaction
- world: Environment and world state management
- sensory: Sensory processing (vision, audio, internal signals)
- motor: Action types and motor commands
- examples: Example scripts demonstrating usage
- tests: Test suite for verification

All modules follow Python best practices with:
- Type hints for better IDE support
- Comprehensive docstrings
- Error handling and validation
- Pythonic interfaces (property accessors, context managers)
- Convenience functions for common operations

The original C++ extension is imported as `_c_extension` for low-level access
if needed, but the Python API provides all necessary functionality for typical
use cases.

For more information, see the examples directory and test files.
"""

__version__ = "0.1.0"
__author__ = "NLM Authors"
__email__ = "nlm-authors@example.com"

# Import the C++ extension module
import pynlm as _c_extension

# Import all major components
from .core import Config
from .brain import Brain
from .agent import AgentBrain, AgentBody, SensoryPercept, Action, ActionResult
from .world import SimpleWorld
from .sensory import Vision, Audio, InternalSignals
from .motor import ActionType, MotorCommand

# Import convenience functions
from .core import create_default_config
from .brain import create_brain
from .world import create_simple_world
from .agent import create_agent_brain
from .motor import create_movement_action, create_rotation_action

__all__ = [
    'Config',
    'Brain',
    'AgentBrain',
    'AgentBody',
    'SensoryPercept',
    'Action',
    'ActionResult',
    'SimpleWorld',
    'Vision',
    'Audio',
    'InternalSignals',
    'ActionType',
    'MotorCommand',
    'create_default_config',
    'create_brain',
    'create_simple_world',
    'create_agent_brain',
    'create_movement_action',
    'create_rotation_action',
    '_c_extension',
]