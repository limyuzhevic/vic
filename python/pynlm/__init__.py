"""
pynlm - Neural Learning Machine Python bindings

Python bindings for NLM (Neural Learning Machine), a brain-inspired spiking neural
network simulator written in C++20. This module provides a Pythonic interface
to the core neural network simulation functionality.
"""

__version__ = "0.1.0"

from .core import Brain, createBrain, createDefaultConfig
from .brain import BrainInterface
from .world import SimpleWorld, WorldObject, WorldObjectType
from .agent import AgentBrain
from .sensory import SensoryProcessor
from .motor import MotorController
from .neuromodulation import NeuromodulationSystem
from .exceptions import (
    NLMError,
    ConfigurationError,
    InitializationError,
    SimulationError,
    ResourceError,
)
from .utils import (
    validate_config,
    create_named_timer,
    format_neuron_id,
    parse_synaptic_weight,
)

__all__ = [
    "Brain",
    "createBrain",
    "createDefaultConfig",
    "BrainInterface",
    "SimpleWorld",
    "WorldObject",
    "WorldObjectType",
    "AgentBrain",
    "SensoryProcessor",
    "MotorController",
    "NeuromodulationSystem",
    "NLMError",
    "ConfigurationError",
    "InitializationError",
    "SimulationError",
    "ResourceError",
    "validate_config",
    "create_named_timer",
    "format_neuron_id",
    "parse_synaptic_weight",
    "__version__",
]

import logging

# Configure default logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)