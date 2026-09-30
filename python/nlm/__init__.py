"""
NLM (Neural Learning Machine) - Python API

This is the main entry point for the NLM Python API, providing comprehensive
access to neural simulation, experiment management, and performance monitoring
with enhanced Pythonic interfaces.

The API includes:
- Core simulation components (Brain, World, Agent, Memory)
- High-level abstractions (BrainSimulator, WorldSimulator, AgentSimulator, MemoryManager)
- Performance monitoring and profiling
- Experiment management and analysis
- Advanced features and custom environments
"""

import os
import sys
from typing import Dict, Any, Optional

# Version information
__version__ = "2.0.0"
__author__ = "NLM Development Team"
__email__ = "dev@nlm.ai"

# Core configuration and types
from .core.high_level_api import (
    # High-level simulation abstractions
    BrainSimulator,
    WorldSimulator,
    AgentSimulator,
    MemoryManager,
    SimulationStatus,
    SimulationMetrics as HighLevelSimulationMetrics
)

# For backward compatibility, also import core C++ types if available
try:
    from .pynlm import (
        # Core classes
        Config,
        Brain,
        SimpleWorld,
        Action,
        SensoryPercept,
        Vision,
        Audio,
        InternalSignals,
        ActionType,
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        NeuronType,
        DevelopmentalStage,
        FiringState,
        SimulationStep,
        Timestamp,
        MembranePotential,
        Delay,
        SignalStrength,
        PlasticityFlags,
        DetailedSpikeEvent,
        DelayedSpikeEvent,
        NeuronState,
        SynapseState,
        SpikeEvent,
        InterRegionConnection
    )
except ImportError:
    # Define placeholder classes if C++ bindings are not available
    Config = Brain = SimpleWorld = Action = SensoryPercept = Vision = Audio = InternalSignals = None
    ActionType = NeuronId = SynapseId = RegionId = PopulationId = None
    NeuronType = DevelopmentalStage = FiringState = SimulationStep = Timestamp = None
    MembranePotential = Delay = SignalStrength = PlasticityFlags = None
    DetailedSpikeEvent = DelayedSpikeEvent = NeuronState = SynapseState = SpikeEvent = InterRegionConnection = None

# Import existing modules
from .profiling import (
    # Performance monitoring
    MemoryMonitor,
    CPUMonitor,
    SimulationProfiler,
    PerformanceProfiler,
    MemoryMetrics,
    CPUMetrics,
    SimulationMetrics,
    BenchmarkResult,
    PerformanceLevel
)

from .experiments import (
    # Experiment management
    Experiment,
    ExperimentConfig,
    ExperimentResult,
    ExperimentStatus,
    ExperimentType,
    ExperimentSuite
)

from .nlm.advanced import (
    # Advanced features
    AdvancedExperiment,
    CustomEnvironment
)

# Import high-level abstractions
from .core.high_level_api import (
    # High-level simulation abstractions
    BrainSimulator,
    WorldSimulator,
    AgentSimulator,
    MemoryManager,
    SimulationStatus,
    SimulationMetrics as HighLevelSimulationMetrics
)

# Error classes
class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Exception raised for configuration-related errors."""
    pass

class SimulationError(NLMError):
    """Exception raised for simulation-related errors."""
    pass

class MemoryError(NLMError):
    """Exception raised for memory-related errors."""
    pass

class PerformanceError(NLMError):
    """Exception raised for performance monitoring errors."""
    pass


# Convenience factory functions
def create_brain(config_file: Optional[str] = None, 
               config_dict: Optional[Dict[str, Any]] = None) -> Brain:
    """
    Create a new brain with specified configuration.
    
    Args:
        config_file: Path to configuration file
        config_dict: Configuration dictionary
        
    Returns:
        Configured brain instance
        
    Example:
        >>> brain = create_brain(config_dict={"neuron_count": 1000})
    """
    from .core.high_level_api import _create_default_config
    config = _create_default_config(config_file, config_dict)
    return Brain(config)


def create_world(width: int = 100, height: int = 100) -> SimpleWorld:
    """
    Create a new world with specified dimensions.
    
    Args:
        width: World width
        height: World height
        
    Returns:
        Configured world instance
        
    Example:
        >>> world = create_world(width=200, height=150)
    """
    from .core.high_level_api import _create_default_config
    config = _create_default_config()
    world = SimpleWorld()
    world.configure(width, height, 10, 10)
    return world


def create_agent(brain: Brain, world: SimpleWorld) -> Any:
    """
    Create an agent with specified brain and world.
    
    Args:
        brain: Agent's brain
        world: Environment world
        
    Returns:
        Configured agent instance
        
    Example:
        >>> brain = create_brain()
        >>> world = create_world()
        >>> agent = create_agent(brain, world)
    """
    from .pynlm import AgentBrain
    return AgentBrain(brain)


def create_enhanced_brain(config_file: Optional[str] = None,
                           config_dict: Optional[Dict[str, Any]] = None) -> Brain:
    """
    Create an enhanced brain with Pythonic wrapper.
    
    Args:
        config_file: Path to configuration file
        config_dict: Configuration dictionary
        
    Returns:
        Enhanced brain instance
        
    Example:
        >>> brain = create_enhanced_brain(config_dict={"neuron_count": 2000})
    """
    from .core.high_level_api import BrainSimulator
    simulator = BrainSimulator(config_file, config_dict)
    return simulator.brain


def create_enhanced_agent(config_file: Optional[str] = None,
                          config_dict: Optional[Dict[str, Any]] = None) -> Any:
    """
    Create an enhanced agent with high-level interface.
    
    Args:
        config_file: Path to configuration file
        config_dict: Configuration dictionary
        
    Returns:
        Enhanced agent instance
        
    Example:
        >>> agent = create_enhanced_agent()
    """
    from .core.high_level_api import BrainSimulator
    simulator = BrainSimulator(config_file, config_dict)
    from .pynlm import AgentBrain
    return AgentBrain(simulator.brain)


def quick_start_simulation(num_steps: int = 100) -> Dict[str, Any]:
    """
    Quick start a complete simulation with default configuration.
    
    Args:
        num_steps: Number of simulation steps
        
    Returns:
        Simulation results
        
    Example:
        >>> results = quick_start_simulation(num_steps=100)
    """
    from .core.high_level_api import BrainSimulator
    simulator = BrainSimulator()
    return simulator.run_simulation(num_steps=num_steps)


# Simulation factory classes
class SimulationFactory:
    """Factory class for creating simulation components."""
    
    @staticmethod
    def create_brain_simulator(config_file: Optional[str] = None,
                               config_dict: Optional[Dict[str, Any]] = None) -> 'BrainSimulator':
        """Create a brain simulator."""
        from .core.high_level_api import BrainSimulator
        return BrainSimulator(config_file, config_dict)
    
    @staticmethod
    def create_world_simulator(width: int = 100, height: int = 100,
                               config_dict: Optional[Dict[str, Any]] = None) -> 'WorldSimulator':
        """Create a world simulator."""
        from .core.high_level_api import WorldSimulator
        return WorldSimulator(config_dict, width, height)
    
    @staticmethod
    def create_agent_simulator(brain: Brain, world: SimpleWorld,
                               config_dict: Optional[Dict[str, Any]] = None) -> 'AgentSimulator':
        """Create an agent simulator."""
        from .core.high_level_api import AgentSimulator
        return AgentSimulator(brain, world, config_dict)


# Utility functions

def load_config(config_file: str) -> Config:
    """
    Load configuration from file.
    
    Args:
        config_file: Path to configuration file
        
    Returns:
        Loaded configuration
        
    Example:
        >>> config = load_config("config.json")
    """
    config = Config()
    if not config.loadFromFile(config_file):
        raise ConfigurationError(f"Failed to load configuration from {config_file}")
    return config


def save_config(config: Config, config_file: str) -> bool:
    """
    Save configuration to file.
    
    Args:
        config: Configuration to save
        config_file: Path to save configuration
        
    Returns:
        True if successful, False otherwise
        
    Example:
        >>> save_config(config, "config.json")
    """
    return config.saveToFile(config_file)


def benchmark_simulation(brain: Brain, world: SimpleWorld, 
                         num_steps: int = 100, num_runs: int = 10) -> List['BenchmarkResult']:
    """
    Benchmark neural simulation performance.
    
    Args:
        brain: Brain to simulate
        world: World to simulate
        num_steps: Number of steps per simulation
        num_runs: Number of simulation runs
        
    Returns:
        List of benchmark results
        
    Example:
        >>> profiler = PerformanceProfiler()
        >>> results = benchmark_simulation(brain, world, num_steps=100)
    """
    from .profiling import PerformanceProfiler
    profiler = PerformanceProfiler()
    return profiler.benchmark_simulation(brain, world, num_steps, num_runs)


def run_experiment(experiment_config: Dict[str, Any]) -> 'ExperimentResult':
    """
    Run an experiment with specified configuration.
    
    Args:
        experiment_config: Experiment configuration
        
    Returns:
        Experiment results
        
    Example:
        >>> config = {"name": "test", "type": "basic", "parameters": {}}
        >>> results = run_experiment(config)
    """
    experiment = Experiment(ExperimentConfig(**experiment_config))
    return experiment.run()


def load_example_config() -> Dict[str, Any]:
    """
    Load example configuration.
    
    Returns:
        Example configuration dictionary
    """
    return {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.12,
        "random.seed": 42,
        "energy.decay_rate": 0.01,
        "world.width": 100,
        "world.height": 100,
        "world.vision_width": 10,
        "world.vision_height": 10,
        "world.max_energy": 100.0,
        "world.energy_decay_rate": 0.01,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001,
        "simulation_timestep": 0.001,
        "replay_interval": 100,
        "consolidation_interval": 1000
    }


# Module metadata
__all__ = [
    # Core classes
    'Config',
    'Brain', 
    'SimpleWorld',
    'Action',
    'SensoryPercept',
    'Vision',
    'Audio',
    'InternalSignals',
    'ActionType',
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    'NeuronType',
    'DevelopmentalStage',
    'FiringState',
    'SimulationStep',
    'Timestamp',
    'MembranePotential',
    'Delay',
    'SignalStrength',
    'PlasticityFlags',
    'DetailedSpikeEvent',
    'DelayedSpikeEvent',
    'NeuronState',
    'SynapseState',
    'SpikeEvent',
    'InterRegionConnection',
    
    # Performance monitoring
    'MemoryMonitor',
    'CPUMonitor',
    'SimulationProfiler',
    'PerformanceProfiler',
    'MemoryMetrics',
    'CPUMetrics',
    'SimulationMetrics',
    'BenchmarkResult',
    'PerformanceLevel',
    
    # Experiment management
    'Experiment',
    'ExperimentConfig',
    'ExperimentResult',
    'ExperimentStatus',
    'ExperimentType',
    'ExperimentSuite',
    
    # Advanced features
    'AdvancedExperiment',
    'CustomEnvironment',
    
    # High-level abstractions
    'BrainSimulator',
    'WorldSimulator',
    'AgentSimulator',
    'MemoryManager',
    'SimulationStatus',
    'HighLevelSimulationMetrics',
    
    # Error classes
    'NLMError',
    'ConfigurationError',
    'SimulationError',
    'MemoryError',
    'PerformanceError',
    
    # Factory functions
    'create_brain',
    'create_world',
    'create_agent',
    'create_enhanced_brain',
    'create_enhanced_agent',
    'quick_start_simulation',
    'SimulationFactory',
    
    # Utility functions
    'load_config',
    'save_config',
    'benchmark_simulation',
    'run_experiment',
    'load_example_config',
    
    # Module info
    '__version__',
    '__author__',
    '__email__'
]

# Initialize module
print(f"NLM Python API v{__version__} loaded successfully")
print(f"Author: {__author__}")
print(f"Available modules: core simulation, performance monitoring, experiment management, high-level abstractions")
print(f"Run 'help(nlm)' for more information")
