"""
Type definitions for NLM Python bindings.

This module provides type definitions, constants, and enumerations
for the NLM Python API, ensuring type safety and consistency.
"""

import enum
import math
from typing import Dict, Any, Optional, Union
from enum import Enum as PythonEnum


# Type aliases for common types
NeuronId = int
SynapseId = int
RegionId = int
PopulationId = int
SimulationStep = int
Timestamp = float
MembranePotential = float
FiringRate = float
Energy = float
Position = float
ActionResult = Dict[str, Any]
WorldObjectId = str


class NeuronType(PythonEnum):
    """Neuron type enumeration."""
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    SENSORY = "sensory"
    MOTOR = "motor"
    INTERNAL = "internal"


class SynapseType(PythonEnum):
    """Synapse type enumeration."""
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    ELECTRICAL = "electrical"
    GAP_JUNCTION = "gap_junction"


class DevelopmentalStage(PythonEnum):
    """Developmental stage enumeration."""
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"


class FiringState(PythonEnum):
    """Neuron firing state enumeration."""
    RESTING = "resting"
    ACTIVE = "active"
    REFRACTORY = "refractory"
    INHIBITED = "inhibited"


class ActionType(PythonEnum):
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
class MotorCommand(PythonEnum):
    """Motor command enumeration."""
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK_LEFT = "look_left"
    LOOK_RIGHT = "look_right"
    INTERACT = "interact"
    WAIT = "wait"
class WorldObjectType(PythonEnum):
    """World object type enumeration."""
    EMPTY = "empty"
    RESOURCE = "resource"
    HAZARD = "hazard"
    WALL = "wall"
    MARKER = "marker"
class ExplorationStrategy(PythonEnum):
    """Exploration strategy enumeration."""
    NONE = "none"
    EPSILON_GREEDY = "epsilon_greedy"
    UCB = "ucb"
    BOLTZMANN = "boltzmann"
    NOISE = "noise"
    CURIOUSITY = "curiosity"
class BehaviorType(PythonEnum):
    """Behavior type enumeration."""
    EXPLORATION = "exploration"
    EXPLOITATION = "exploitation"
    BALANCED = "balanced"
    RANDOM = "random"


# Constants for NLM
PI = 3.141592653589793
E = 2.718281828459045

DEFAULT_NEURON_COUNT = 500
DEFAULT_REGION_COUNT = 1
DEFAULT_CONNECTION_PROBABILITY = 0.15
DEFAULT_SIMULATION_TIMESTEP = 0.001
DEFAULT_RANDOM_SEED = 42

DEFAULT_WORLD_WIDTH = 100
DEFAULT_WORLD_HEIGHT = 100
DEFAULT_VISION_WIDTH = 16
DEFAULT_VISION_HEIGHT = 16

MAX_ENERGY = 100.0
MIN_ENERGY = 0.0

# Type validation constants
MIN_NEURON_COUNT = 10
MAX_NEURON_COUNT = 100000
MIN_REGION_COUNT = 1
MAX_REGION_COUNT = 100
MIN_CONNECTION_PROBABILITY = 0.0
MAX_CONNECTION_PROBABILITY = 1.0
MIN_SIMULATION_TIMESTEP = 0.0
MAX_SIMULATION_TIMESTEP = 1.0

# Default configuration keys
CONFIG_NEURON_COUNT = "neuron_count"
CONFIG_REGION_COUNT = "region_count"
CONFIG_CONNECTION_PROBABILITY = "connection_probability"
CONFIG_SIMULATION_TIMESTEP = "simulation_timestep"
CONFIG_RANDOM_SEED = "random_seed"
CONFIG_ENABLE_DEVELOPMENT = "enable_development"
CONFIG_ENABLE_CURIOUSITY = "enable_curiosity"
CONFIG_ENABLE_REWARD_MODULATION = "enable_reward_modulation"
CONFIG_ENABLE_STRUCTURAL_PLASTICITY = "enable_structural_plasticity"

# Object type constants
OBJECT_RESOURCE_VALUE = 1.0
OBJECT_HAZARD_RADIUS = 0.3
OBJECT_WALL_RADIUS = 0.4
OBJECT_MARKER_RADIUS = 0.2

# Action effect constants
ACTION_REWARD_SUCCESS = 1.0
ACTION_REWARD_FAILURE = 0.0
ACTION_REWARD_REST = 0.5
ACTION_REWARD_EAT = 2.0

# Validation ranges
VALID_REWARD_RANGES = (-10.0, 10.0)
VALID_CURiosityRANGES = (0.0, 1.0)
VALID_NOVELTY_RANGES = (0.0, 1.0)
VALID_PREDICTION_ERROR_RANGES = (-1.0, 1.0)

# Error messages
ERROR_MESSAGES = {
    'brain_not_initialized': 'Brain must be initialized before use',
    'brain_already_initialized': 'Brain is already initialized',
    'invalid_configuration': 'Invalid configuration provided',
    'missing_configuration': 'Required configuration missing',
    'simulation_not_running': 'Simulation is not running',
    'simulation_already_running': 'Simulation is already running',
    'memory_allocation_failed': 'Memory allocation failed',
    'file_not_found': 'File not found: {file_path}',
    'invalid_parameter': 'Invalid parameter: {parameter}',
    'feature_not_available': 'Feature not available: {feature}',
    'dependency_missing': 'Required dependency missing: {dependency}',
    'version_mismatch': 'Version mismatch between C++ and Python bindings',
    'initialization_failed': 'Initialization failed: {reason}',
}


def validate_range(value: float, min_val: float, max_val: float, param_name: str) -> None:
    """
    Validate value is within range.
    
    Args:
        value: Value to validate
        min_val: Minimum allowed value
        max_val: Maximum allowed value
        param_name: Parameter name for error message
        
    Raises:
        ValueError: If value is out of range
    """
    if not min_val <= value <= max_val:
        raise ValueError(
            f"Parameter '{param_name}' must be between {min_val} and {max_val}, "
            f"got {value}"
        )


def validate_positive(value: float, param_name: str) -> None:
    """
    Validate value is positive.
    
    Args:
        value: Value to validate
        param_name: Parameter name for error message
        
    Raises:
        ValueError: If value is not positive
    """
    if value <= 0:
        raise ValueError(
            f"Parameter '{param_name}' must be positive, got {value}"
        )


def validate_non_negative(value: float, param_name: str) -> None:
    """
    Validate value is non-negative.
    
    Args:
        value: Value to validate
        param_name: Parameter name for error message
        
    Raises:
        ValueError: If value is negative
    """
    if value < 0:
        raise ValueError(
            f"Parameter '{param_name}' must be non-negative, got {value}"
        )


def clamp(value: float, min_val: float, max_val: float) -> float:
    """
    Clamp value to range.
    
    Args:
        value: Value to clamp
        min_val: Minimum value
        max_val: Maximum value
        
    Returns:
        Clamped value
    """
    return max(min_val, min(max_val, value))


def lerp(start: float, end: float, t: float) -> float:
    """
    Linear interpolation between two values.
    
    Args:
        start: Start value
        end: End value
        t: Interpolation factor (0-1)
        
    Returns:
        Interpolated value
    """
    return start + (end - start) * t


def normalize(value: float, min_val: float, max_val: float) -> float:
    """
    Normalize value to range [0, 1].
    
    Args:
        value: Value to normalize
        min_val: Minimum value in original range
        max_val: Maximum value in original range
        
    Returns:
        Normalized value (0-1)
    """
    if max_val == min_val:
        return 0.5
    return (value - min_val) / (max_val - min_val)


def distance(x1: float, y1: float, x2: float, y2: float) -> float:
    """
    Calculate Euclidean distance between two points.
    
    Args:
        x1, y1: First point coordinates
        x2, y2: Second point coordinates
        
    Returns:
        Distance between points
    """
    return ((x2 - x1) ** 2 + (y2 - y1) ** 2) ** 0.5


def angle_to_target(x1: float, y1: float, x2: float, y2: float) -> float:
    """
    Calculate angle from point 1 to point 2.
    
    Args:
        x1, y1: Source point coordinates
        x2, y2: Target point coordinates
        
    Returns:
        Angle in radians (-pi to pi)
    """
    dx = x2 - x1
    dy = y2 - y1
    return math.atan2(dy, dx)


# Pre-computed action names and descriptions for efficiency
ACTION_NAMES = {
    action_type.value: action_type.name.replace('_', ' ').title()
    for action_type in ActionType
}

ACTION_DESCRIPTIONS = {
    ActionType.MOVE_FORWARD: "Move forward one unit",
    ActionType.MOVE_BACKWARD: "Move backward one unit",
    ActionType.MOVE_LEFT: "Move left one unit",
    ActionType.MOVE_RIGHT: "Move right one unit",
    ActionType.TURN_LEFT: "Turn left 90 degrees",
    ActionType.TURN_RIGHT: "Turn right 90 degrees",
    ActionType.LOOK: "Look forward",
    ActionType.LOOK_UP: "Look up",
    ActionType.LOOK_DOWN: "Look down",
    ActionType.INTERACT: "Interact with object",
    ActionType.EAT: "Eat food",
    ActionType.DRINK: "Drink water",
    ActionType.REST: "Rest and recover",
    ActionType.WAIT: "Wait for next turn",
    ActionType.CUSTOM: "Custom action",
}

# Helper functions for common operations
def get_action_names() -> Dict[str, str]:
    """Get action names mapping."""
    return ACTION_NAMES.copy()
def get_action_descriptions() -> Dict[str, str]:
    """Get action descriptions mapping."""
    return ACTION_DESCRIPTIONS.copy()


def is_valid_action_type(value: str) -> bool:
    """Check if action type string is valid."""
    return value in ACTION_NAMES
def parse_action_type(value: str) -> ActionType:
    """
    Parse action type string to enum.
    
    Args:
        value: Action type string
        
    Returns:
        ActionType enum
        
    Raises:
        ValueError: If action type is invalid
    """
    try:
        return ActionType(value)
    except ValueError:
        raise ValueError(f"Invalid action type: {value}. Valid types: {list(ActionType)}")


def get_config_defaults() -> Dict[str, Any]:
    """Get default configuration values."""
    return {
        'neuron_count': DEFAULT_NEURON_COUNT,
        'region_count': DEFAULT_REGION_COUNT,
        'connection_probability': DEFAULT_CONNECTION_PROBABILITY,
        'simulation_timestep': DEFAULT_SIMULATION_TIMESTEP,
        'random_seed': DEFAULT_RANDOM_SEED,
    }


def validate_config(config: Dict[str, Any]) -> List[str]:
    """
    Validate configuration dictionary.
    
    Args:
        config: Configuration dictionary
        
    Returns:
        List of validation errors (empty if valid)
    """
    errors = []
    
    # Check neuron count
    if 'neuron_count' in config:
        validate_positive(config['neuron_count'], 'neuron_count')
    
    # Check region count
    if 'region_count' in config:
        validate_positive(config['region_count'], 'region_count')
    
    # Check connection probability
    if 'connection_probability' in config:
        validate_range(config['connection_probability'], MIN_CONNECTION_PROBABILITY, MAX_CONNECTION_PROBABILITY, 'connection_probability')
    
    # Check simulation timestep
    if 'simulation_timestep' in config:
        validate_range(config['simulation_timestep'], MIN_SIMULATION_TIMESTEP, MAX_SIMULATION_TIMESTEP, 'simulation_timestep')
    
    return errors


# Export all main types and functions
__all__ = [
    # Type aliases
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'SimulationStep', 'Timestamp', 'MembranePotential',
    'FiringRate', 'Energy', 'Position', 'ActionResult',
    'WorldObjectId',
    
    # Enums
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType',
    'ExplorationStrategy', 'BehaviorType',
    
    # Constants
    'PI', 'E', 'DEFAULT_NEURON_COUNT', 'DEFAULT_REGION_COUNT',
    'DEFAULT_CONNECTION_PROBABILITY', 'DEFAULT_SIMULATION_TIMESTEP',
    'DEFAULT_RANDOM_SEED', 'DEFAULT_WORLD_WIDTH', 'DEFAULT_WORLD_HEIGHT',
    'DEFAULT_VISION_WIDTH', 'DEFAULT_VISION_HEIGHT', 'MAX_ENERGY',
    'MIN_ENERGY',
    
    # Validation ranges
    'MIN_NEURON_COUNT', 'MAX_NEURON_COUNT', 'MIN_REGION_COUNT',
    'MAX_REGION_COUNT', 'MIN_CONNECTION_PROBABILITY', 'MAX_CONNECTION_PROBABILITY',
    'MIN_SIMULATION_TIMESTEP', 'MAX_SIMULATION_TIMESTEP',
    
    # Configuration keys
    'CONFIG_NEURON_COUNT', 'CONFIG_REGION_COUNT',
    'CONFIG_CONNECTION_PROBABILITY', 'CONFIG_SIMULATION_TIMESTEP',
    'CONFIG_RANDOM_SEED', 'CONFIG_ENABLE_DEVELOPMENT',
    'CONFIG_ENABLE_CURIOUSITY', 'CONFIG_ENABLE_REWARD_MODULATION',
    'CONFIG_ENABLE_STRUCTURAL_PLASTICITY',
    
    # Object type constants
    'OBJECT_RESOURCE_VALUE', 'OBJECT_HAZARD_RADIUS',
    'OBJECT_WALL_RADIUS', 'OBJECT_MARKER_RADIUS',
    
    # Action effect constants
    'ACTION_REWARD_SUCCESS', 'ACTION_REWARD_FAILURE',
    'ACTION_REWARD_REST', 'ACTION_REWARD_EAT',
    
    # Validation ranges
    'VALID_REWARD_RANGES', 'VALID_CURiosityRANGES',
    'VALID_NOVELTY_RANGES', 'VALID_PREDICTION_ERROR_RANGES',
    
    # Error messages
    'ERROR_MESSAGES',
    
    # Helper functions
    'validate_range', 'validate_positive', 'validate_non_negative',
    'clamp', 'lerp', 'normalize', 'distance', 'angle_to_target',
    'get_action_names', 'get_action_descriptions',
    'is_valid_action_type', 'parse_action_type',
    'get_config_defaults', 'validate_config',
]
