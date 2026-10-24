"""
NLM - Neural Learning Machine Python Package

This package provides a Python interface to the NLM neural simulation framework.
It offers both direct bindings to the C++ API and high-level wrapper classes
for easier use by researchers and developers.
"""

__version__ = "0.1.0"

# Core API imports from pybind11 bindings
from .core import Brain, Config, AgentBrain, SimpleWorld
from .sensory import Vision, Audio, InternalSignals, SensoryPercept
from .motor import Action, MotorCommand
from .agent import AgentBody, SensoryPercept as AgentSensoryPercept
from .world import WorldObject
from .types import *

# High-level wrapper classes
from .wrapper import NLMSimulation, NLMExperiment, NLMPlotting, NLMDeveloper

# Convenience functions
from .factory import create_default_config, create_brain, create_world, create_agent_brain

# Constants and utilities
from .constants import (
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
    NEURON_TYPES,
    ACTION_TYPES,
    MOTOR_COMMANDS,
    WORLD_OBJECT_TYPES,
    DEVELOPER_STAGES,
    FIRING_STATES,
    PLASTICITY_FLAGS,
    SYNAPSE_TYPES,
)

# For backward compatibility
from .core import createBrain as create_brain_legacy
from .core import createConfig as create_config_legacy
from .core import createSimpleWorld as create_world_legacy
from .core import createAgentBrain as create_agent_brain_legacy

__all__ = [
    # Core API
    "Brain",
    "Config",
    "AgentBrain",
    "SimpleWorld",
    "Vision",
    "Audio",
    "InternalSignals",
    "SensoryPercept",
    "Action",
    "MotorCommand",
    "AgentBody",
    "WorldObject",
    "NeuronId",
    "SynapseId",
    "RegionId",
    "PopulationId",
    
    # Wrappers
    "NLMSimulation",
    "NLMExperiment",
    "NLMPlotting",
    "NLMDeveloper",
    
    # Factory functions
    "create_default_config",
    "create_brain",
    "create_world",
    "create_agent_brain",
    
    # Constants
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    "NEURON_TYPES",
    "ACTION_TYPES",
    "MOTOR_COMMANDS",
    "WORLD_OBJECT_TYPES",
    "DEVELOPER_STAGES",
    "FIRING_STATES",
    "PLASTICITY_FLAGS",
    "SYNAPSE_TYPES",
    
    # Legacy (deprecated)
    "createBrain",
    "createConfig",
    "createSimpleWorld",
    "createAgentBrain",
    "__version__",
]
