"""
Python type definitions and utilities for NLM Python bindings
"""

from typing import Dict, List, Any, Optional, Union, Tuple
from dataclasses import dataclass, field
from enum import Enum
import numpy as np

# Core type aliases
NeuronId = Any
SynapseId = Any
RegionId = Any
PopulationId = Any
ConfigValue = Any
ConfigSource = Any

class NeuronType(Enum):
    """Neuron types for different functional roles"""
    EXCITATORY = 0
    INHIBITORY = 1
    MODULATORY = 2
    SENSORY = 3
    MOTOR = 4
    INTERNAL = 5

class SynapseType(Enum):
    """Synapse connection types"""
    EXCITATORY = 0
    INHIBITORY = 1
    MODULATORY = 2
    ELECTRICAL = 3
    GAP_JUNCTION = 4

class DevelopmentalStage(Enum):
    """Brain development stages"""
    INITIAL = 0
    CRITICAL_PERIOD = 1
    MATURATION = 2
    ADULT = 3
    AGING = 4

class FiringState(Enum):
    """Neuron firing states"""
    RESTING = 0
    ACTIVE = 1
    REFRACTORY = 2
    INHIBITED = 3

class ActionType(Enum):
    """Motor action types"""
    MOVE_FORWARD = 0
    MOVE_BACKWARD = 1
    MOVE_LEFT = 2
    MOVE_RIGHT = 3
    TURN_LEFT = 4
    TURN_RIGHT = 5
    LOOK = 6
    LOOK_UP = 7
    LOOK_DOWN = 8
    INTERACT = 9
    EAT = 10
    DRINK = 11
    REST = 12
    WAIT = 13
    CUSTOM = 14

class MotorCommand(Enum):
    """Low-level motor commands"""
    MOVE_FORWARD = 0
    MOVE_BACKWARD = 1
    TURN_LEFT = 2
    TURN_RIGHT = 3
    LOOK_LEFT = 4
    LOOK_RIGHT = 5
    INTERACT = 6
    WAIT = 7

class WorldObjectType(Enum):
    """Types of objects in the world"""
    EMPTY = 0
    RESOURCE = 1
    HAZARD = 2
    WALL = 3
    MARKER = 4

@dataclass
class NeuroDebugConfig:
    """Debug configuration for neural simulations"""
    enable_neuron_logging: bool = False
    enable_synapse_logging: bool = False
    enable_memory_logging: bool = False
    enable_plasticity_logging: bool = False
    log_file: Optional[str] = None
    log_level: str = "INFO"
    output_interval: int = 100

@dataclass
class ResearchExperimentConfig:
    """Configuration for research experiments"""
    experiment_name: str = "experiment"
    description: str = ""
    use_preset: Optional[str] = None
    parameter_ranges: Dict[str, List[float]] = field(default_factory=dict)
    seed: Optional[int] = None
    num_seeds: int = 1
    max_steps: int = 1000
    checkpoint_interval: int = 100
    save_results: bool = True
    generate_plots: bool = True
    enable_multiprocessing: bool = False
    parallel_workers: int = 1

@dataclass
class ParameterSweepConfig:
    """Configuration for parameter sweeps"""
    parameters: Dict[str, List[float]]
    grid_search: bool = False
    random_search: bool = False
    bayesian_optimization: bool = False
    genetic_algorithm: bool = False
    num_iterations: int = 100
    objective_function: Optional[callable] = None
    optimization_metric: str = "performance"
    parallel_execution: bool = True
    timeout_per_iteration: Optional[float] = None

@dataclass
class StatisticalResults:
    """Statistical analysis results"""
    mean: float = 0.0
    std: float = 0.0
    min: float = 0.0
    max: float = 0.0
    median: float = 0.0
    variance: float = 0.0
    percentiles: Dict[float, float] = field(default_factory=dict)
    samples: int = 0

@dataclass
class ComparisonResults:
    """Results from experiment comparison"""
    experiment1_name: str
    experiment2_name: str
    metric_name: str
    difference: float
    p_value: float
    statistical_significance: bool
    effect_size: float

@dataclass
class VisualizationConfig:
    """Configuration for visualization"""
    plot_type: str = "line"
    figure_size: Tuple[int, int] = (12, 8)
    style: str = "seaborn-v0_8"
    save_plots: bool = True
    output_format: str = "png"
    dpi: int = 150
    color_scheme: str = "default"
    enable_interactivity: bool = False

@dataclass
class PerformanceMetrics:
    """Performance monitoring metrics"""
    simulation_time: float = 0.0
    memory_usage: float = 0.0
    cpu_usage: float = 0.0
    spikes_per_second: float = 0.0
    neuron_count: int = 0
    synapse_count: int = 0
    total_steps: int = 0
    initialization_time: float = 0.0
    peak_memory: float = 0.0

@dataclass
class ExperimentResult:
    """Result from a single experiment run"""
    experiment_name: str
    seed: int
    steps_completed: int
    final_spike_count: int
    average_firing_rate: float
    performance_metrics: PerformanceMetrics
    statistical_results: Optional[StatisticalResults] = None
    custom_metrics: Dict[str, float] = field(default_factory=dict)
    success: bool = False
    error_message: Optional[str] = None
    timestamp: float = 0.0

# Type aliases for better readability
t = Any  # Python generic type placeholder

# Research workflow type definitions
ExperimentConfig = ResearchExperimentConfig
SweepConfig = ParameterSweepConfig
VisualizationCfg = VisualizationConfig
Metrics = PerformanceMetrics
Results = ExperimentResult
Stat = StatisticalResults

__all__ = [
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId', 'ConfigValue', 'ConfigSource',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    'NeuroDebugConfig', 'ResearchExperimentConfig', 'ParameterSweepConfig',
    'StatisticalResults', 'ComparisonResults', 'VisualizationConfig',
    'PerformanceMetrics', 'ExperimentResult',
    't', 'ExperimentConfig', 'SweepConfig', 'VisualizationCfg', 'Metrics', 'Results', 'Stat'
]