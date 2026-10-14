# Python package for NLM neural simulation framework
"""
NLM (熙然) is an experimental artificial developmental brain project.
This package provides Python bindings for the C++ NLM neural simulation framework.

Key Features:
- High-performance neural simulation with event-driven architecture
- Comprehensive brain systems integration (memory, neuromodulation, prediction, cognition)
- Developmental neuroscience modeling
- Advanced plasticity rules (STDP, Hebbian, structural)
- Real-world sensory-motor integration
- Python-friendly API with type hints and comprehensive documentation

Usage Examples:
    >>> import nlm
    >>> config = nlm.createDefaultConfig()
    >>> brain = nlm.createBrain(config)
    >>> brain.initialize()
    >>> # Simulation loop...
"""

# Core types
from .bindings import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage,
    FiringState, ActionType, MotorCommand, WorldObjectType
)

# Main classes
from .bindings import (
    Config, Brain, SimpleWorld, AgentBrain,
    Vision, Audio, InternalSignals, Action,
    WorldObject, AgentBody, ActionResult,
    SensoryPercept, SensoryInput
)

# Factory functions
from .bindings import (
    createDefaultConfig, createBrain, createSimpleWorld,
    createAgentBrain
)

# Constants
from .bindings import (
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID,
    INVALID_POPULATION_ID
)

# Advanced submodules
from . import config
from . import brain
from . import world
from . import agent
from . import sensory
from . import motor
from . import memory
from . import cli

# Convenience imports for exceptions
from .exceptions import (
    NLMBaseException, NLMError, NLMConfigurationError,
    NLMValidationError, NLMMemoryError, NLMIndexError,
    NLMTypeError, NLMRuntimeError, NLMNotImplementedError,
    NLMValueError, NLMKeyError, NLMFileError,
    NLMResourceError, NLMPerformanceError,
    create_exception_from_cpp,
    raise_config_error, raise_validation_error,
    raise_memory_error, raise_index_error,
    raise_type_error, raise_runtime_error,
)
from . import cli

# Convenience imports for common exceptions
from .exceptions import (
    NLMBaseException, NLMError, NLMConfigurationError,
    NLMValidationError, NLMMemoryError, NLMIndexError,
    NLMTypeError, NLMRuntimeError, NLMNotImplementedError,
    NLMValueError, NLMKeyError, NLMFileError,
    NLMResourceError, NLMPerformanceError,
    create_exception_from_cpp,
    raise_config_error, raise_validation_error,
    raise_memory_error, raise_index_error,
    raise_type_error, raise_runtime_error,
)

__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"

# Public API exports
__all__ = [
    # Core identifiers
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    
    # Enums and types
    'NeuronType', 'SynapseType', 'ActionType', 'MotorCommand',
    'WorldObjectType', 'DevelopmentalStage', 'FiringState',
    
    # Main classes
    'Config', 'Brain', 'SimpleWorld', 'AgentBrain',
    'Vision', 'Audio', 'InternalSignals', 'Action',
    'WorldObject', 'AgentBody', 'ActionResult',
    'SensoryPercept', 'SensoryInput',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld',
    'createAgentBrain',
    
    # Advanced modules
    'config', 'brain', 'world', 'agent', 'sensory',
    'motor', 'memory', 'cognition',
    
    # Performance settings
    'NLM_PERFORMANCE_MODE', 'NLM_PARALLEL_PROCESSING', 'NLM_MEMORY_POOLING',
    
    # Default parameters
    'DEFAULT_SIMULATION_DT', 'DEFAULT_MAX_STEP_COUNT',
    'DEFAULT_CHECKPOINT_INTERVAL',
    
    # Error handling
    'DEFAULT_ERROR_RECOVERY', 'DEFAULT_VALIDATION_STRICT',
    
    # Logging
    'DEFAULT_LOG_LEVEL', 'DEFAULT_LOG_FILE',
    
    # Exceptions
    'NLMBaseException', 'NLMError', 'NLMConfigurationError',
    'NLMValidationError', 'NLMMemoryError', 'NLMIndexError',
    'NLMTypeError', 'NLMRuntimeError', 'NLMNotImplementedError',
    'NLMValueError', 'NLMKeyError', 'NLMFileError',
    'NLMResourceError', 'NLMPerformanceError',
    'create_exception_from_cpp',
    
# Validation
    'InputValidator', 'ErrorRecovery', 'validate_brain_config',
    'validate_sensory_data', 'validate_motor_action',
    'validate_world_parameters', 'validate_simulation_parameters',
    'get_validation_errors', 'try_with_recovery',

    # CLI
    'NLMCLI', 'main', 'create_cli', 'run_command',]

import os

# Configure performance settings
NLM_PERFORMANCE_MODE = os.getenv('NLM_PERFORMANCE_MODE', 'auto').lower() in ('1', 'true', 'yes', 'on')
NLM_PARALLEL_PROCESSING = os.getenv('NLM_PARALLEL_PROCESSING', 'auto').lower() in ('1', 'true', 'yes', 'on')
NLM_MEMORY_POOLING = os.getenv('NLM_MEMORY_POOLING', 'auto').lower() in ('1', 'true', 'yes', 'on')

# Default simulation parameters
DEFAULT_SIMULATION_DT = 0.01
DEFAULT_MAX_STEP_COUNT = 10000
DEFAULT_CHECKPOINT_INTERVAL = 1000

# Error handling configuration
DEFAULT_ERROR_RECOVERY = True
DEFAULT_VALIDATION_STRICT = False

# Logging configuration
DEFAULT_LOG_LEVEL = os.getenv('NLM_LOG_LEVEL', 'INFO')
DEFAULT_LOG_FILE = os.getenv('NLM_LOG_FILE', None)