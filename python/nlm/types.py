"""
Type definitions and constants for NLM.

Type definitions and constants used across NLM modules.
"""

from enum import Enum
from typing import TypedDict, Tuple, Optional
import time
# Neuron types
class NeuronType(Enum):
    """Neuron type enumeration."""
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    SENSORY = "sensory"
    MOTOR = "motor"
    INTERNAL = "internal"
# Synapse types
class SynapseType(Enum):
    """Synapse type enumeration."""
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    ELECTRICAL = "electrical"
    GAP_JUNCTION = "gap_junction"
# Developmental stages
class DevelopmentalStage(Enum):
    """Developmental stage enumeration."""
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"
# Firing states
class FiringState(Enum):
    """Neuron firing state enumeration."""
    RESTING = "resting"
    ACTIVE = "active"
    REFRACTORY = "refractory"
    INHIBITED = "inhibited"
# Action types
class ActionType(Enum):
    """Action type enumeration."""
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    MOVE_LEFT = "move_left"
    MOVE_RIGHT = "move_right"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK = "look"
    LOOK_UP = "look_up"
    LOOK_DOWN = "look_down"
    INTERACT = "interact"
    EAT = "eat"
    DRINK = "drink"
    REST = "rest"
    WAIT = "wait"
    CUSTOM = "custom"
# Motor commands
class MotorCommand(Enum):
    """Motor command enumeration."""
    MOVE_FORWARD = 0
    MOVE_BACKWARD = 1
    TURN_LEFT = 2
    TURN_RIGHT = 3
    LOOK_LEFT = 4
    LOOK_RIGHT = 5
    INTERACT = 6
    WAIT = 7
# World object types
class WorldObjectType(Enum):
    """World object type enumeration."""
    EMPTY = "empty"
    RESOURCE = "resource"
    HAZARD = "hazard"
    WALL = "wall"
    MARKER = "marker"
# Configuration keys
class ConfigKey(Enum):
    """Configuration key enumeration."""
    NEURON_COUNT = "brain.neuron_count"
    SYNAPSE_DENSITY = "brain.synapse_density"
    CONNECTION_PROBABILITY = "brain.connection_probability"
    INITIAL_WEIGHT_MEAN = "brain.initial_weight_mean"
    INITIAL_WEIGHT_STD = "brain.initial_weight_std"
    V_THRESH = "brain.v_thresh"
    V_REST = "brain.v_rest"
    V_RESET = "brain.v_reset"
    TAU_MEM = "brain.tau_mem"
    TAU_REF = "brain.tau_ref"
    STDP_ENABLE = "plasticity.stdp.enable"
    STDP_LEARNING_RATE = "plasticity.stdp.learning_rate"
    STDP_TAU_PLUS = "plasticity.stdp.tau_plus"
    STDP_TAU_MINUS = "plasticity.stdp.tau_minus"
    HEBBIAN_ENABLE = "plasticity.hebbian.enable"
    STRUCTURAL_PLASTICITY_ENABLE = "plasticity.structural.enable"
    DOPAMINE_SCALE = "neuromod.dopamine.scale"
    CURIOUSY_ENABLE = "neuromod.curiosity.enable"
    NOVELTY_ENABLE = "neuromod.novelty.enable"
# Data structures
class SensoryInputData(TypedDict):
    """Sensory input data structure."""
    vision: List[List[float]]
    touch: List[float]
    internal: List[float]
    proprioception: List[float]
    audio: List[float]
class ActionResultData(TypedDict):
    """Action result data structure."""
    reward: float
    success: bool
    message: str
    position: Optional[Tuple[float, float]]
    orientation: Optional[float]
# Type aliases
NeuronId = int
SynapseId = int
RegionId = int
PopulationId = int
Timestamp = float
SimulationStep = int
MembranePotential = float
FiringRate = float
# Constants
DEFAULT_TIMESTEP = 0.001
DEFAULT_VISION_WIDTH = 16
DEFAULT_VISION_HEIGHT = 16
DEFAULT_TOUCH_SENSORS = 8
DEFAULT_INTERNAL_SENSORS = 4
DEFAULT_PROPSEPTATION_SENSORS = 6
DEFAULT_MOTOR_ACTIONS = 6
DEFAULT_MAX_ENERGY = 100.0
DEFAULT_ENERGY_DECAY_RATE = 0.01
DEFAULT_RESOURCE_ENERGY_GAIN = 10.0
# Version info
NLM_VERSION = "0.1.0"
NLM_DESCRIPTION = "Neural Learning Machine Python Interface"
NLM_AUTHOR = "NLM Project"
NLM_LICENSE = "MIT"
# Mathematical constants
PI = 3.14159265359
E = 2.71828182846
PHI = 1.61803398875