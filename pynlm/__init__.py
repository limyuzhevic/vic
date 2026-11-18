"""Python bindings for NLM (Neural Learning Machine) neural simulation framework.

This package provides a well-structured Python API for the NLM C++ neural simulation
framework, with backward compatibility and improved maintainability.
"""

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine) neural simulation framework"

# Re-export the main API for backward compatibility
from .brain import Brain
from .world import SimpleWorld
from .agent import AgentBrain
from .config import Config
from .core.types import (
    NeuronId, SynapseId, RegionId, PopulationId,
    SimulationStep, Timestamp, TimestepDuration,
    NeuronIndex, SynapseIndex, PopulationIndex, RegionIndex,
    SynapticWeight, MembranePotential, FiringRate, Delay,
    SpikeEvent, DelayedSpikeEvent,
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID,
)
from .core.enums import (
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType, ConfigSource,
)
from .sensory import Vision, Audio, InternalSignals, SensoryInput
from .motor import Action, WorldObject, AgentBody, ActionResult, SensoryPercept
from .utils import (
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    run_simulation, run_agent_simulation, benchmark_brain,
    ConfigurationError, SimulationError, VisualizationError,
    create_preset_config, create_advanced_features_demo,
    export_to_json, import_from_json,
    save_results, load_results,
    validate_configuration, get_default_configuration_template,
    calculate_neural_density, calculate_connectivity, analyze_simulation_results,
)

# Error classes for backward compatibility
class PynlmException(Exception):
    """Base exception for pynlm errors."""
    pass

class ConfigurationError(PynlmException):
    """Error in configuration."""
    pass

class SimulationError(PynlmException):
    """Error in simulation."""
    pass

class VisualizationError(PynlmException):
    """Error in visualization."""
    pass

# Convenience factory functions (maintained for backward compatibility)
def createDefaultConfig():
    """Create a default configuration."""
    return Config()


def createBrain(config=None):
    """Create a new brain with configuration."""
    if config is None:
        config = createDefaultConfig()
    return Brain(config)


def createSimpleWorld():
    """Create a new simple world."""
    return SimpleWorld()


def createAgentBrain(brain):
    """Create a new agent brain interface."""
    return AgentBrain(brain)


# Module metadata
__all__ = [
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'SimulationStep', 'Timestamp', 'TimestepDuration',
    'NeuronIndex', 'SynapseIndex', 'PopulationIndex', 'RegionIndex',
    'SynapticWeight', 'MembranePotential', 'FiringRate', 'Delay',
    'SpikeEvent', 'DelayedSpikeEvent',
    # Enumerations
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType', 'ConfigSource',
    # Main classes
    'Brain', 'SimpleWorld', 'AgentBrain', 'Config',
    # Sensory input
    'Vision', 'Audio', 'InternalSignals', 'SensoryInput',
    # Motor and actions
    'Action', 'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept',
    # Utilities and advanced features
    'VisualizationUtils',
    # Convenience functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    'run_simulation', 'run_agent_simulation', 'benchmark_brain',
    # Exception classes
    'PynlmException', 'ConfigurationError', 'SimulationError', 'VisualizationError',
    # File operations
    'save_results', 'load_results', 'export_to_json', 'import_from_json',
    # Configuration utilities
    'validate_configuration', 'get_default_configuration_template', 'create_preset_config',
    # Analysis functions
    'calculate_neural_density', 'calculate_connectivity', 'analyze_simulation_results',
    # Advanced features
    'create_advanced_features_demo',
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    # Version info
    '__version__', '__author__', '__description__',
]

# Additional imports for specific components
from .core import types, enums