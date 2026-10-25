"""Enhanced Pythonic interfaces for NLM (Neural Learning Machine)

This module provides Pythonic APIs for the NLM neural simulation framework,
including context managers, properties, batch operations, and advanced features
to make the framework more accessible and powerful for Python users.
"""

__version__ = "1.0.0-Pythonic-Enhanced"
__author__ = "NLM Team"
__description__ = "Python bindings for NLM (Neural Learning Machine) with enhanced Pythonic interfaces"

# Core enhanced classes
from .brain import Brain, Config, AgentBrain, SimpleWorld, createBrain, createDefaultConfig
from .sensory import Vision, Audio, InternalSignals, SensoryInput
from .motor import Action, MotorCommand, ActionResult, ActionType, ActionParameters
from .world import WorldObject, WorldObjectType, SimpleWorld as SimpleWorldImpl
from .agent import Agent, AgentState, create_agent

# Enums and types
from .enums import (
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    get_enum_value, get_enum_values, get_enum_members
)

# Advanced simulation features
from .simulation import (
    BatchSimulator, EpisodeRunner, EnvironmentInterface,
    SimulationMode, EpisodeStatus, EpisodeMetrics, CheckpointManager
)

# Monitoring and statistics
from .monitoring import (
    StatisticsCollector, PerformanceMonitor, NeuralAnalyzer,
    MonitoringLevel
)

# Utilities
from .utils import (
    create_config,
    validate_config,
    format_output,
    plot_neural_activity,
    analyze_simulation_results,
    ConfigurationValidator,
    OutputFormatter,
    VisualizationHelper,
    ResultAnalyzer
)

# Constants
from .constants import (
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID,
    DEFAULT_CONFIG, SIMULATION_TIMESTEP, DEFAULT_VISION_WIDTH, DEFAULT_VISION_HEIGHT,
    DEFAULT_VISION_CHANNELS, DEFAULT_AUDIO_SAMPLE_RATE, DEFAULT_AUDIO_DURATION,
    UnitTypes, Colors,
    create_test_config, validate_config as validate_config_utility,
    mV, ms, Hz, nA, is_near, clamp, euclidean_distance, manhattan_distance,
    normalize_angle, angle_difference
)

# Sensory utilities
from .sensory import (
    SensoryModality,
    create_vision,
    create_audio,
    create_proprioceptive,
    create_internal_signals
)

# Motor utilities
from .motor import (
    create_motor_command,
    create_action,
    create_action_result,
    get_action_type_names,
    get_movement_actions,
    get_interaction_actions
)

# World utilities
from .world import (
    create_simple_world,
    create_world_object,
    WorldObjectType as WorldObjectTypeAlias
)

# Agent utilities
from .agent import (
    create_agent,
    create_test_agent as create_agent_test
)

# Main simulation interface
from .simulation import EnvironmentInterface as EnvironmentInterfaceClass

__all__ = [
    # Core classes
    "Brain", "Config", "AgentBrain", "SimpleWorld", "createBrain", "createDefaultConfig",
    "Vision", "Audio", "InternalSignals", "SensoryInput",
    "Action", "MotorCommand", "ActionResult", "ActionType", "ActionParameters",
    "WorldObject", "WorldObjectType",
    "Agent", "AgentState",
    "create_agent",
    
    # Enums
    "NeuronType", "SynapseType", "DevelopmentalStage", "FiringState",
    "ActionType", "MotorCommand", "WorldObjectType",
    "get_enum_value", "get_enum_values", "get_enum_members",
    
    # Advanced features
    "BatchSimulator", "EpisodeRunner", "EnvironmentInterface",
    "SimulationMode", "EpisodeStatus", "EpisodeMetrics", "CheckpointManager",
    
    # Monitoring
    "StatisticsCollector", "PerformanceMonitor", "NeuralAnalyzer",
    "MonitoringLevel",
    
    # Utilities
    "create_config",
    "validate_config", "ConfigurationValidator",
    "format_output", "OutputFormatter",
    "plot_neural_activity", "VisualizationHelper",
    "analyze_simulation_results", "ResultAnalyzer",
    
    # Constants
    "INVALID_NEURON_ID", "INVALID_SYNAPSE_ID", "INVALID_REGION_ID", "INVALID_POPULATION_ID",
    "DEFAULT_CONFIG", "SIMULATION_TIMESTEP", "DEFAULT_VISION_WIDTH", "DEFAULT_VISION_HEIGHT",
    "DEFAULT_VISION_CHANNELS", "DEFAULT_AUDIO_SAMPLE_RATE", "DEFAULT_AUDIO_DURATION",
    "UnitTypes", "Colors",
    "create_test_config", "validate_config_utility",
    "mV", "ms", "Hz", "nA", "is_near", "clamp", "euclidean_distance", "manhattan_distance",
    "normalize_angle", "angle_difference",
    
    # Sensory utilities
    "SensoryModality",
    "create_vision",
    "create_audio",
    "create_proprioceptive",
    "create_internal_signals",
    
    # Motor utilities
    "create_motor_command",
    "create_action",
    "create_action_result",
    "get_action_type_names",
    "get_movement_actions",
    "get_interaction_actions",
    
    # World utilities
    "create_simple_world",
    "create_world_object",
    "WorldObjectTypeAlias",
    
    # Agent utilities
    "create_agent",
    "create_agent_test",
    
    # Main interface
    "EnvironmentInterface",
    
    # Module info
    "__version__", "__author__", "__description__"
]