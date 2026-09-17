"""
NLM (熙然) - Enhanced Python API for Neural Learning Machine

This package provides a Pythonic interface to the NLM neural simulation framework.
It includes context managers, batch processing, performance monitoring,
and enhanced error handling while maintaining full backward compatibility.
"""

import sys
from typing import Optional, List, Dict, Any

# Import core modules
from .exceptions import (
    NLMError,
    ConfigurationError,
    SimulationError,
    MemoryError,
    ValidationError,
    AgentError,
    WorldError
)

from .core import (
    createBrain,
    createDefaultConfig,
    Brain,
    SimulationStage,
    NeuronMetrics
)

from .agent import (
    createAgentBrain,
    AgentBrain,
    LearningFeature
)

from .sensory import (
    createSimpleWorld,
    createSimplePercept,
    Vision,
    Audio,
    InternalSignals,
    Proprioception,
    SensoryInput
)

from .config import (
    EnhancedConfig,
    ConfigEntry,
    ConfigScope
)

from .simulation import (
    Simulation,
    SimulationState,
    SimulationUtils,
    SimulationBenchmark,
    SimulationMonitor
)

from .profiler import (
    PerformanceProfiler,
    PerformanceMetrics,
    TimeProfiler,
    MemoryProfiler,
    ProfileSimulation,
    PerformanceMonitor
)

from .examples import (
    ExampleRunner,
    ExampleSuite,
    get_example_by_name
)

# Re-export core bindings from C++ for backward compatibility
from ._bindings import (
    # Enhanced APIs
    BrainContextManager,
    AgentBrainContextManager,
    SimpleWorldContextManager,
    run_batch,
    run_episode,
    configure_agent,
    configure_world,
    # Context managers
    # Performance monitoring
    PerformanceProfiler,
    PerformanceStats,
    # Configuration
    EnhancedConfig,
    # Factory functions
    createBrain,
    createDefaultConfig,
    createSimpleWorld,
    createAgentBrain,
    # Learning feature control
    LearningFeatureControl,
    # Advanced operations
    AdvancedBrainOps,
    AdvancedWorldOps,
    AdvancedAgentOps,
    # Statistics
    SimulationStats,
    # Constants
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID
)

# Version information
__version__ = "2.0.0"
__author__ = "NLM Project"
__license__ = "MIT"
__description__ = "Enhanced Python bindings for NLM neural simulation framework"

# Package metadata
__all__ = [
    # Core classes
    "Brain",
    "AgentBrain",
    "SimpleWorld",
    "Config",
    "EnhancedConfig",
    "Action",
    "Vision",
    "Audio",
    "InternalSignals",
    "SensoryInput",
    "SensoryPercept",
    "AgentBody",
    "WorldObject",
    "ActionResult",
    "SimulationStage",
    "NeuronMetrics",
    
    # Exceptions
    "NLMError",
    "ConfigurationError",
    "SimulationError",
    "MemoryError",
    "ValidationError",
    "AgentError",
    "WorldError",
    
    # Factory functions
    "createBrain",
    "createDefaultConfig",
    "createSimpleWorld",
    "createAgentBrain",
    "createSimplePercept",
    
    # Context managers
    "BrainContextManager",
    "AgentBrainContextManager",
    "SimpleWorldContextManager",
    "Simulation",
    
    # Batch processing
    "run_batch",
    "run_episode",
    
    # Configuration and setup
    "configure_agent",
    "configure_world",
    "EnhancedConfig",
    
    # Performance monitoring
    "PerformanceProfiler",
    "PerformanceMetrics",
    "SimulationStats",
    
    # Learning feature control
    "LearningFeatureControl",
    
    # Advanced operations
    "AdvancedBrainOps",
    "AdvancedWorldOps",
    "AdvancedAgentOps",
    
    # Example systems
    "ExampleRunner",
    "ExampleSuite",
    "get_example_by_name",
    
    # Statistics and profiling
    "TimeProfiler",
    "MemoryProfiler",
    "ProfileSimulation",
    "PerformanceMonitor",
    "SimulationBenchmark",
    "SimulationMonitor",
    
    # Enumerations
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    "FiringState",
    "ActionType",
    "MotorCommand",
    "WorldObjectType",
    
    # Constants
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    
    # Metadata
    "__version__",
    "__author__",
    "__license__",
    "__description__",
]

# Helper functions for common operations
def quick_simulation(steps: int = 100) -> Dict[str, Any]:
    """Run a quick simulation with default settings.
    
    Args:
        steps: Number of simulation steps to run
        
    Returns:
        Dictionary with simulation statistics
    """
    config = createDefaultConfig()
    brain = createBrain(config)
    
    with BrainContextManager(brain) as managed_brain:
        stats = run_batch(managed_brain.getBrain(), steps)
        return stats.__dict__
def run_agent_episode(world: SimpleWorld, steps: int = 100) -> Dict[str, Any]:
    """Run an agent episode with the given world.
    
    Args:
        world: The world to simulate in
        steps: Number of steps to run
        
    Returns:
        Dictionary with episode statistics
    """
    config = createDefaultConfig()
    brain = createBrain(config)
    agent = createAgentBrain(brain)
    
    with AgentBrainContextManager(agent) as managed_agent:
        stats = run_episode(managed_agent.getAgent(), world, steps)
        return stats.__dict__
def create_brain_with_config(neuron_count: int = 1000, **kwargs) -> Brain:
    """Create a brain with custom configuration.
    
    Args:
        neuron_count: Number of neurons in the brain
        **kwargs: Additional configuration parameters
        
    Returns:
        Configured brain instance
    """
    config = EnhancedConfig({"brain.neuron_count": neuron_count})
    for key, value in kwargs.items():
        config.set(key, value)
    
    return createBrain(config)
def enable_agent_learning(agent: AgentBrain, features: List[str] = None) -> None:
    """Enable learning features for an agent.
    
    Args:
        agent: Agent to configure
        features: List of features to enable (default: all)
    """
    if features is None:
        agent.enableLearning()
    else:
        agent.enableLearning(features)
def print_welcome_message():
    """Print a welcome message for NLM users."""
    print("=" * 60)
    print(f"NLM (熙然) Enhanced Python API v{__version__}")
    print(f"Author: {__author__}")
    print(f"License: {__license__}")
    print()
    print("Enhanced features:")
    print("- Context managers for automatic resource management")
    print("- Batch processing with performance monitoring")
    print("- Episode simulation with detailed statistics")
    print("- Enhanced error handling and validation")
    print("- Pythonic API design with type hints")
    print("- 100% backward compatibility with existing code")
    print("=" * 60)
    print()

# Print welcome message when package is imported
if sys.version_info >= (3, 8):
    print_welcome_message()
