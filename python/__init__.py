"""
NLM - Neural Learning Machine Python Package

This package provides Python bindings for the NLM (Neural Learning Machine) C++ neural simulation framework.
It offers a Pythonic interface to all the NLM functionality while maintaining full compatibility
with the underlying C++ implementation.

The package includes:
- Enhanced Brain and World classes with context managers
- Custom exceptions for better error handling
- Helper functions and utility classes
- Configuration and settings management
- Sensor data processing helpers
- Factory methods for common use cases
"""

__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"

# Import the main classes and functions from the compiled extensions
from .core import (
    # Core exceptions
    NLMError,
    ConfigError,
    BrainError,
    WorldError,
    SensorError,
    AgentError,

    # Enhanced configuration
    Config,
    create_default_config,

    # Enhanced brain classes
    Brain,
    create_brain,

    # Enhanced world classes
    World,
    SimpleWorld,
    create_simple_world,

    # Agent classes
    AgentBrain,
    create_agent_brain,

    # Sensor classes
    Vision,
    Audio,
    InternalSignals,
    SensoryInput,

    # Action classes
    Action,
    create_action,

    # World object classes
    WorldObject,

    # Helper classes
    WorldHelper,
    SensorHelper,

    # Utility functions
    run_simulation,
    normalize_sensory_data,
    create_config_from_dict,

    # Constants and enumerations
    NeuronType,
    ActionType,
    WorldObjectType,
    DevelopmentalStage,
    FiringState,
    SynapseType,
    MotorCommand,
)

# Convenience exports
__all__ = [
    # Core
    'NLMError', 'ConfigError', 'BrainError', 'WorldError', 'SensorError', 'AgentError',
    # Classes
    'Config', 'Brain', 'World', 'SimpleWorld', 'AgentBrain',
    'Vision', 'Audio', 'InternalSignals', 'SensoryInput',
    'Action', 'WorldObject',
    # Helpers
    'WorldHelper', 'SensorHelper',
    # Functions
    'create_default_config', 'create_brain', 'create_simple_world', 'create_agent_brain',
    'create_action', 'run_simulation', 'normalize_sensory_data', 'create_config_from_dict',
    # Constants
    'NeuronType', 'ActionType', 'WorldObjectType', 'DevelopmentalStage',
    'FiringState', 'SynapseType', 'MotorCommand',
    # Package info
    '__version__', '__author__', '__license__',
]

# Version check
def _check_version():
    """Check if the installed version matches the expected version."""
    try:
        # Try to import from compiled extension
        from .core import __version__ as compiled_version
        if compiled_version != __version__:
            import warnings
            warnings.warn(
                f"Version mismatch: package expects {__version__} but compiled extension is {compiled_version}",
                ImportWarning
            )
    except ImportError:
        pass

_check_version()

# Provide helpful error messages
class ImportErrorHandler:
    """Handler for import errors with helpful suggestions."""

    @staticmethod
    def handle_import_error():
        import sys
        print("Error: Could not import pynlm")
        print("\nTo install pynlm:")
        print("1. Build the C++ extensions:")
        print("   cd /path/to/nlm")
        print("   pip install .")
        print("2. Or install from pip:")
        print("   pip install pynlm")
        print("\nFor build issues, see README.md for detailed instructions.")
        sys.exit(1)

# Try to handle common import errors
try:
    # Test import
    from .core import Config
except ImportError:
    ImportErrorHandler.handle_import_error()
