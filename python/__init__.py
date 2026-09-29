"""
Enhanced NLM Python Interface
=============================

A modern, type-safe Python interface for the NLM (Neural Learning Machine) framework
with comprehensive type hints, documentation, and advanced features.

This is the main entry point for NLM Python usage. Import this module to access
the full functionality of the NLM neural simulation framework.

Key Features:
- Comprehensive type hints for all public APIs
- Enhanced documentation with examples
- Advanced simulation controls
- Performance monitoring and profiling
- Configuration validation
- Real-time simulation monitoring
- Environment building utilities

Usage Examples:
    >>> import pynlm
    >>> # Create and configure brain
    >>> config = pynlm.create_default_config()
    >>> config.set("brain.neuron_count", 5000)
    >>> brain = pynlm.create_brain(config)
    >>> brain.initialize()
    >>> # Run simulation with monitoring
    >>> results = pynlm.run_advanced_simulation(brain, world, agent, config)
    >>> print(f"Performance: {results['efficiency_grade']}")

For comprehensive documentation, see the enhanced API guide in ENHANCED_API.md
"""

import sys
import os
from typing import Optional, List, Dict, Any, Union

# Version information
__version__ = "0.2.0"
__author__ = "NLM Project"
__license__ = "MIT"

# Add the current directory to Python path for local imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import core types and classes from C++ bindings
from pynlm import (
    # Core type definitions
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage,
    FiringState, ActionType, MotorCommand,
    WorldObjectType,
    
    # Core classes
    Config, Brain, AgentBrain, SimpleWorld,
    SensoryInput, Vision, Audio, InternalSignals,
    Action, WorldObject, AgentBody, ActionResult,
    SensoryPercept,
    
    # Constants
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID,
    INVALID_REGION_ID, INVALID_POPULATION_ID
)

# Import enhanced classes
from .enhanced_types import (
    LearningMetrics, SimulationConfig, PerformanceProfiler,
    SimulationMonitor, EnvironmentBuilder
)

# Import utility functions
from .utils import (
    run_advanced_simulation, run_learning_experiment,
    run_steady_state_test, benchmark_performance,
    validate_configuration, export_brain_state,
    import_brain_state, create_reproducible_environment
)

# Factory functions with enhanced type hints
def create_default_config() -> Config:
    """Create a default configuration optimized for advanced simulations.
    
    Returns:
        Config: A new configuration with all necessary parameters set.
        
    Example:
        >>> config = create_default_config()
        >>> config.set("brain.neuron_count", 5000)
        >>> config.set("plasticity.stdp.enable", True)
    """
    return Config()


def create_brain(config: Optional[Config] = None) -> Brain:
    """Create a new brain with the specified configuration.
    
    Args:
        config: Configuration to use. If None, a default config will be created.
        
    Returns:
        Brain: A new brain instance ready for initialization.
        
    Example:
        >>> config = create_default_config()
        >>> config.set("brain.neuron_count", 2000)
        >>> brain = create_brain(config)
        >>> brain.initialize()
    """
    if config is None:
        config = create_default_config()
    return Brain(config)


def create_agent(brain: Brain) -> AgentBrain:
    """Create an agent brain interface for world interaction.
    
    Args:
        brain: The brain instance to interface.
        
    Returns:
        AgentBrain: An agent interface that connects brain to world.
        
    Example:
        >>> brain = create_brain(config)
        >>> agent = create_agent(brain)
    """
    return AgentBrain(brain)


def create_world() -> SimpleWorld:
    """Create a new simple 2D world for simulation.
    
    Returns:
        SimpleWorld: A new world instance.
        
    Example:
        >>> world = create_world()
        >>> world.configure(20, 20, 8, 8)
    """
    return SimpleWorld()


def create_vision_sensor(width: int, height: int, channels: int = 3) -> Vision:
    """Create a vision sensory input.
    
    Args:
        width: Vision width in pixels.
        height: Vision height in pixels.
        channels: Number of color channels (default: 3 for RGB).
        
    Returns:
        Vision: A new vision sensor.
        
    Example:
        >>> vision = create_vision_sensor(8, 8, 3)
    """
    return Vision(width, height, channels)


def create_audio_sensor(sample_rate: int, num_samples: int) -> Audio:
    """Create an audio sensory input.
    
    Args:
        sample_rate: Audio sample rate in Hz.
        num_samples: Number of audio samples.
        
    Returns:
        Audio: A new audio sensor.
        
    Example:
        >>> audio = create_audio_sensor(44100, 1024)
    """
    return Audio(sample_rate, num_samples)


# Re-export all important classes and functions for convenience
__all__ = [
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand',
    'WorldObjectType',
    
    # Core classes
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'WorldObject', 'AgentBody', 'ActionResult',
    'SensoryPercept',
    
    # Enhanced classes
    'LearningMetrics', 'SimulationConfig', 'PerformanceProfiler',
    'SimulationMonitor', 'EnvironmentBuilder',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    
    # Factory functions
    'create_default_config', 'create_brain', 'create_agent',
    'create_world', 'create_vision_sensor', 'create_audio_sensor',
    
    # Simulation functions
    'run_advanced_simulation', 'run_learning_experiment',
    'run_steady_state_test', 'benchmark_performance',
    
    # Utility functions
    'validate_configuration', 'export_brain_state',
    'import_brain_state', 'create_reproducible_environment',
]


# Module metadata
_metadata = {
    'version': __version__,
    'author': __author__,
    'license': __license__,
    'description': 'Enhanced NLM Python interface with type hints and advanced features',
    'features': [
        'comprehensive_type_hints',
        'advanced_configuration',
        'performance_profiling',
        'real_time_monitoring',
        'environment_building',
        'benchmarking',
        'learning_metrics',
        'simulation_automation',
        'configuration_validation',
        'state_management'
    ]
}


def get_metadata() -> Dict[str, Any]:
    """Get module metadata.
    
    Returns:
        Dict[str, Any]: Module metadata including version, author, features, etc.
    """
    return _metadata.copy()


def print_version_info() -> None:
    """Print version information to stdout."""
    metadata = get_metadata()
    print(f"NLM Python Enhanced Interface v{metadata['version']}")
    print(f"Author: {metadata['author']}")
    print(f"License: {metadata['license']}")
    print(f"Description: {metadata['description']}")
    print("\nAvailable features:")
    for feature in metadata['features']:
        print(f"  • {feature}")


# Print version info when module is imported
if __name__ == "__main__":
    print_version_info()
