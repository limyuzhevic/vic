from __future__ import annotations

"""
NLM (Neural Learning Machine) Python Package
===========================================

This package provides Python bindings for the NLM (Neural Learning Machine) C++ neural simulation framework.

Key Features:
- Advanced Python API for neural simulation
- High-level abstractions for common simulation patterns
- Comprehensive utilities for analysis and debugging
- Advanced configuration management
- Experimental workflow support

Quick Start:
```python
import nlm

# Create a simulation
config = nlm.create_default_config()
sim = nlm.NeuralSimulation(config)

# Run simulation
simulation_data = sim.run_episode(100)

# Analyze results
analyzer = nlm.BrainAnalyzer(sim.brain)
analysis = analyzer.analyze_current_state()
```

Advanced Usage:
```python
# Create advanced configuration
config = nlm.AdvancedConfig()
config.update_config(
    neuron_count=5000,
    region_count=5,
    plasticity_learning_rate=0.008,
    dopamine_baseline=0.2,
    visualization_enabled=True
)

# Run complex experiment
experiment = nlm.BrainExperiment("Memory Integration Test", config)
experiment_results = experiment.run_experiment(episodes_per_simulation=50)
```

For more advanced usage, see the documentation and examples in the package.
"""

# Import core functionality from the C++ extension
from .core import (
    # Basic types
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    ActionResult,
    
    # Core classes
    Config,
    Brain,
    SimpleWorld,
    AgentBrain,
    Vision,
    Audio,
    InternalSignals,
    Action,
    AgentBody,
    
    # Factory functions
    create_default_config,
    create_brain,
    create_simple_world,
    create_agent_brain,
    
    # Constants
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
)

# Import advanced API
from .advanced_api import (
    # Advanced configuration
    AdvancedConfig,
    SimulationPresets,
    
    # Advanced data structures
    SensoryData,
    MotorData,
    WorldState,
    AgentState,
    
    # Advanced simulation classes
    NeuralSimulation,
    BrainExperiment,
    
    # Advanced utilities
    run_simulation,
    save_brain_state,
    load_brain_state,
    benchmark_simulation,
    visualize_brain_state,
    analyze_brain_dynamics,
    
    # Advanced helper functions
    create_exploratory_simulation,
    create_dev_stage_simulation,
    create_plasticity_experiment,
    create_memory_test,
    
    # Analysis classes
    CognitiveTest,
    MemoryTest,
    PlasticityTest,
    BrainAnalyzer,
    DynamicsProfiler,
    ConnectivityInspector,
)

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Advanced Python bindings for NLM neural simulation framework"

# Package metadata
__all__ = [
    # Core components
    "NeuronId",
    "SynapseId",
    "RegionId",
    "PopulationId",
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    "FiringState",
    "ActionType",
    "MotorCommand",
    "WorldObjectType",
    "ActionResult",
    "Config",
    "Brain",
    "SimpleWorld",
    "AgentBrain",
    "Vision",
    "Audio",
    "InternalSignals",
    "Action",
    "AgentBody",
    "create_default_config",
    "create_brain",
    "create_simple_world",
    "create_agent_brain",
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    
    # Advanced components
    "AdvancedConfig",
    "SimulationPresets",
    "SensoryData",
    "MotorData",
    "WorldState",
    "AgentState",
    "NeuralSimulation",
    "BrainExperiment",
    "run_simulation",
    "save_brain_state",
    "load_brain_state",
    "benchmark_simulation",
    "visualize_brain_state",
    "analyze_brain_dynamics",
    "create_exploratory_simulation",
    "create_dev_stage_simulation",
    "create_plasticity_experiment",
    "create_memory_test",
    "CognitiveTest",
    "MemoryTest",
    "PlasticityTest",
    "BrainAnalyzer",
    "DynamicsProfiler",
    "ConnectivityInspector",
    
    # Package info
    "__version__",
    "__author__",
    "__description__",
]

# Usage examples and helper functions
def quick_start_example() -> str:
    """Get a quick start example for NLM usage."""
    return """
Quick Start Example:
------------------

import nlm

# Create a basic simulation
config = nlm.create_default_config()
sim = nlm.NeuralSimulation(config)

# Run simulation
simulation_data = sim.run_episode(100)

# Access brain state
brain_state = sim.brain.getTotalNeuronCount()
active_neurons = sim.brain.getActiveNeuronCount()

# Analyze performance
stats = sim.get_performance_stats()
print(f"Steps per second: {stats['steps_per_second']:.2f}")
print(f"Average reward: {stats['average_reward_per_step']:.4f}")

# Save simulation state
sim.save("simulation_result.json")

# Advanced example with custom configuration
config = nlm.AdvancedConfig()
config.update_config(
    neuron_count=5000,
    region_count=5,
    plasticity_learning_rate=0.008,
    dopamine_baseline=0.2
)

experiment = nlm.BrainExperiment("Advanced Test", config)
results = experiment.run_experiment(episodes_per_simulation=50)
"""
def get_examples() -> list[str]:
    """Get list of available usage examples."""
    return [
        "quick_start",
        "advanced_configuration",
        "memory_integration",
        "plasticity_experiments",
        "cognitive_tests",
        "performance_benchmarking",
        "visualization_and_analysis",
    ]

# Error types for user convenience
class NLMRuntimeError(Exception):
    """NLM runtime error."""
    pass

class NLMInvalidArgumentError(Exception):
    """NLM invalid argument error."""
    pass

class NLMOutOfRangeError(Exception):
    """NLM out of range error."""
    pass