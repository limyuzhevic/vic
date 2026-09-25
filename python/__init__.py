"""
Python interface for NLM (Neural Learning Machine).

This module provides the Python interface for NLM (Neural Learning Machine),
powering advanced artificial brain capabilities through comprehensive
Python bindings and high-level APIs.

Key features:
- Python bindings for NLM C++ core
- High-level convenience APIs
- Component registry system
- Plugin system
- Discovery APIs
- Registration APIs
- Experiment framework
- Metrics collection
- Exception handling
- Visualization

The Python interface is part of NLM Phase 6 "FINAL INTEGRATION",
powering advanced artificial brain capabilities through comprehensive
Python bindings and high-level APIs.

Module structure:
- Core: Basic NLM classes and types
- Registry: Component registry system
- Plugin: Plugin system
- Discovery: Component discovery APIs
- Registration: Component registration APIs
- Experiment: Enhanced experiment framework
- Metrics: Metrics collection system
- Exceptions: Custom exception classes

Integration with:
- C++ core (via pybind11)
- Component Registry (component management)
- Plugin System (dynamic component loading)
- Brain (neural coordination)
- AgentBrain (agent interface)
- World (environmental interaction)
- Memory systems (working, episodic, semantic, procedural)
- Prediction systems (forward models)
- Cognitive systems (planning, attention, concept formation)
- Neuromodulation systems (dopamine, serotonin, norepinephrine, acetylcholine)
- Development systems (maturation, learning)
- Performance infrastructure (memory pools, event queues)

See README.md for detailed documentation.
"""

from .core import (
    # Core types and classes
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    SensoryPercept,
    # Neuron types
    NeuronType,
    FiringState,
    # Action types
    ActionType,
    # Developmental stages
    DevelopmentalStage,
    # Memory types
    MemoryType,
    # World object types
    WorldObjectType,
    # Other core types
    SimulationStep,
    Timestamp,
    TimestepDuration,
    MembranePotential,
    RegionId,
    NeuronId,
    SynapseId,
    PopulationId,
)

from .registry import (
    # Component registry
    ComponentRegistry,
    get_registry,
    get_factory,
    register_component,
    get_component,
    search_components,
)

from .plugin import (
    # Plugin system
    Plugin,
    PluginManager,
    get_plugin_manager,
    load_plugin,
    unload_plugin,
    is_plugin_loaded,
    get_loaded_plugins,
    reload_plugin,
)

from .discovery import (
    # Discovery API
    DiscoveryAPI,
    get_discovery_api,
    register_component,
    unregister_component,
    find_by_type,
    find_by_category,
    search_by_name,
    get_all_components,
)

from .registration import (
    # Registration API
    RegistrationAPI,
    get_registration_api,
    register_component,
    unregister_component,
    update_component,
    get_component,
    has_component,
    get_all_components,
)

from .experiment import (
    # Experiment framework
    Experiment,
    ExperimentRunner,
    ExperimentState,
    ExperimentMetadata,
    ExperimentResult,
    ExperimentConfiguration,
)

from .metrics import (
    # Metrics collection
    MetricsCollector,
    Metric,
    MetricStatistics,
    MetricType,
)

from .exceptions import (
    # Exception classes
    NLMError,
    NLMInitializationError,
    NLMInvalidStateError,
    NLMSimulationError,
    NLMResourceError,
    NLMConfigurationError,
    NLMSerializationError,
    NLMServerError,
)

# High-level convenience functions
def create_simple_experiment():
    """Create a simple experiment configuration."""
    from .experiment import ExperimentConfiguration
    
    config = ExperimentConfiguration()
    config.max_steps = 1000
    config.time_step = 0.1
    config.num_agents = 1
    
    return config

def create_training_experiment():
    """Create a training experiment configuration."""
    from .experiment import ExperimentConfiguration
    
    config = ExperimentConfiguration()
    config.max_steps = 5000
    config.time_step = 0.1
    config.num_agents = 1
    config.enable_reward_modulation = True
    config.enable_structural_plasticity = True
    config.enable_development = True
    config.enable_curiosity = True
    
    return config

def create_memory_experiment():
    """Create a memory experiment configuration."""
    from .experiment import ExperimentConfiguration
    
    config = ExperimentConfiguration()
    config.max_steps = 2000
    config.time_step = 0.1
    config.num_agents = 1
    config.enable_reward_modulation = True
    config.enable_structural_plasticity = True
    config.enable_development = True
    
    return config

def create_exploration_experiment():
    """Create an exploration experiment configuration."""
    from .experiment import ExperimentConfiguration
    
    config = ExperimentConfiguration()
    config.max_steps = 3000
    config.time_step = 0.1
    config.num_agents = 1
    config.enable_curiosity = True
    config.enable_reward_modulation = True
    
    return config

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Python package metadata
__all__ = [
    # Core types and classes
    "Config",
    "Brain",
    "AgentBrain",
    "SimpleWorld",
    "SensoryPercept",
    "NeuronType",
    "FiringState",
    "ActionType",
    "DevelopmentalStage",
    "MemoryType",
    "WorldObjectType",
    "SimulationStep",
    "Timestamp",
    "TimestepDuration",
    "MembranePotential",
    "RegionId",
    "NeuronId",
    "SynapseId",
    "PopulationId",
    
    # Registry
    "ComponentRegistry",
    "get_registry",
    "get_factory",
    "register_component",
    "get_component",
    "search_components",
    
    # Plugin system
    "Plugin",
    "PluginManager",
    "get_plugin_manager",
    "load_plugin",
    "unload_plugin",
    "is_plugin_loaded",
    "get_loaded_plugins",
    "reload_plugin",
    
    # Discovery API
    "DiscoveryAPI",
    "get_discovery_api",
    "register_component",
    "unregister_component",
    "find_by_type",
    "find_by_category",
    "search_by_name",
    "get_all_components",
    
    # Registration API
    "RegistrationAPI",
    "get_registration_api",
    "register_component",
    "unregister_component",
    "update_component",
    "get_component",
    "has_component",
    "get_all_components",
    
    # Experiment framework
    "Experiment",
    "ExperimentRunner",
    "ExperimentState",
    "ExperimentMetadata",
    "ExperimentResult",
    "ExperimentConfiguration",
    
    # Metrics collection
    "MetricsCollector",
    "Metric",
    "MetricStatistics",
    "MetricType",
    
    # Exception classes
    "NLMError",
    "NLMInitializationError",
    "NLMInvalidStateError",
    "NLMSimulationError",
    "NLMResourceError",
    "NLMConfigurationError",
    "NLMSerializationError",
    "NLMServerError",
    
    # High-level convenience functions
    "create_simple_experiment",
    "create_training_experiment",
    "create_memory_experiment",
    "create_exploration_experiment",
    
    # Package metadata
    "__version__",
    "__author__",
    "__description__",
]