"""
Python wrapper for C++ exceptions and type converters.
This module provides the bridge between C++ exceptions and Python exceptions,
plus comprehensive type conversion utilities for NLM bindings.
"""

import pybind11

# Import the C++ exception header
from pynlm.exceptions import (
    NLMException, FileIOException, ValidationError, ConfigurationError,
    BrainError, WorldError, SensoryError, AudioError, ActionError, AgentError,
    ErrorCodes
)

# Import type converters
from .type_converter import (
    TypeConverter, NumpyConverter, EnumConverter, ConfigConverter,
    register_converter, get_converter
)

# Import Pythonic interfaces
from .pythonic_interfaces import (
    PythonicConfig, PythonicSensoryInput, PythonicAction, PythonicBrain,
    create_pythonic_config, create_pythonic_sensory, create_pythonic_action,
    create_pythonic_brain, get_as_numpy, set_from_numpy, to_string, from_string
)

# Re-export everything for convenience
__all__ = [
    'NLMException',
    'FileIOException', 
    'ValidationError',
    'ConfigurationError',
    'BrainError',
    'WorldError',
    'SensoryError',
    'AudioError',
    'ActionError',
    'AgentError',
    'ErrorCodes',
    'TypeConverter',
    'NumpyConverter',
    'EnumConverter',
    'ConfigConverter',
    'register_converter',
    'get_converter',
    'PythonicConfig',
    'PythonicSensoryInput',
    'PythonicAction',
    'PythonicBrain',
    'create_pythonic_config',
    'create_pythonic_sensory',
    'create_pythonic_action',
    'create_pythonic_brain',
    'get_as_numpy',
    'set_from_numpy',
    'to_string',
    'from_string'
]

# Create module-level function to get all exception classes
def get_exception_classes():
    """Get all registered exception classes"""
    return {
        'NLMException': NLMException,
        'FileIOException': FileIOException,
        'ValidationError': ValidationError,
        'ConfigurationError': ConfigurationError,
        'BrainError': BrainError,
        'WorldError': WorldError,
        'SensoryError': SensoryError,
        'AudioError': AudioError,
        'ActionError': ActionError,
        'AgentError': AgentError,
    }