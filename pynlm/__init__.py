# NLM Python Package - Main Exports

"""
NLM (Neural Learning Machine) Python Bindings

An experimental artificial developmental brain system with Python bindings
for neural simulation, learning, and cognitive development.

Key Components:
- Brain: Central neural simulation engine
- Config: Configuration system for brain parameters
- AgentBrain: Interface between brain and world
- SimpleWorld: 2D world simulation
- SensoryInput classes: Vision, Audio, Internal signals
- Action classes: Motor outputs and behaviors
- Utility functions: Common operations and helpers
"""

from .core import (
    Brain, Config, AgentBrain, SimpleWorld,
    NeuralRegion
)

from .types import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage,
    FiringState, ActionType, MotorCommand, WorldObjectType,
    SimulationStep, Timestamp, MembranePotential, Delay,
    PlasticityFlags, Reward
)

from .sensory import Vision, Audio, InternalSignals, SensoryPercept
from .motor import Action, ActionResult
from .agent import AgentBody
from .utils import (
    create_default_config, create_brain, create_simple_world,
    create_agent_brain, run_simulation_step,
    save_brain_checkpoint, load_brain_checkpoint,
    get_brain_statistics, reset_all_systems,
    SimulationLoop, ActionSequence,
    parse_action_string, format_action,
    wait_for_input, format_duration, benchmark_function
)

from .exceptions import RuntimeError, NLMError, ConfigurationError, BrainError, WorldError

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__email__ = "nlm@example.com"
__license__ = "MIT"

# Package metadata
__all__ = [
    # Core classes
    'Brain', 'Config', 'AgentBrain', 'SimpleWorld', 'NeuralRegion',
    # Type definitions
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType',
    'SimulationStep', 'Timestamp', 'MembranePotential', 'Delay',
    'PlasticityFlags', 'Reward',
    # Sensory and motor
    'Vision', 'Audio', 'InternalSignals', 'SensoryPercept',
    'Action', 'ActionResult',
    # Agent
    'AgentBody',
    # Utilities
    'create_default_config', 'create_brain', 'create_simple_world',
    'create_agent_brain', 'run_simulation_step',
    'save_brain_checkpoint', 'load_brain_checkpoint',
    'get_brain_statistics', 'reset_all_systems',
    'SimulationLoop', 'ActionSequence',
    'parse_action_string', 'format_action',
    'wait_for_input', 'format_duration', 'benchmark_function',
    # Exceptions
    'RuntimeError', 'NLMError', 'ConfigurationError', 'BrainError', 'WorldError',
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
    'INVALID_POPULATION_ID', 'DEFAULT_WORLD_WIDTH', 'DEFAULT_WORLD_HEIGHT',
    'DEFAULT_VISION_WIDTH', 'DEFAULT_VISION_HEIGHT',
    'DEFAULT_MAX_ENERGY', 'DEFAULT_ENERGY_DECAY_RATE', 'DEFAULT_RANDOM_SEED',
    # Metadata
    '__version__', '__author__', '__email__', '__license__'
]

