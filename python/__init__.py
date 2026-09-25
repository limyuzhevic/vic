# NLM - Enhanced Python Package for Neural Learning Machine

This is the main package module that exposes the enhanced NLM API and utilities.

The NLM package provides:
- Core NLM functionality via pynlm bindings (enhanced)
- High-level simulation utilities (from helpers)
- Command-line interface
- Configuration presets
- Monitoring and analysis tools
- Enhanced error handling

Enhanced features:
- Comprehensive NLM C++ bindings with custom exceptions
- Neuromodulation systems (dopamine, curiosity, novelty, prediction error)
- Memory systems (working memory, episodic memory, semantic memory)
- Performance optimization (memory pools, performance monitoring, profiling)
- Development systems (developmental stages, synaptic plasticity, structural plasticity)
- Enhanced configuration with parameter validation and type-safe operations
- Simulation helpers and convenience functions
- Serialization utilities with validation
- Statistical analysis tools
- Debugging and monitoring capabilities
- Comprehensive docstrings and usage examples
"""

import sys

# Core imports - these are the enhanced pynlm bindings
import pynlm

# Import helpers from local module
sys.path.insert(0, str(sys.path[0] + '/python'))

# Simulation classes
from .simulation import *

# Configuration utilities
from .config import *

# Monitoring and analysis
from .monitoring import *

# High-level simulation helpers
from .helpers import *

# CLI module (import but don't expose directly at package level)
try:
    from .nlm_cli import main as cli_main
except ImportError:
    # CLI may not be available in all installations
    pass

# Package metadata
__version__ = "1.0.0"
__author__ = "NLM Authors"
__description__ = "Enhanced Python utilities for NLM neural simulations"
__license__ = "MIT"

# Public API - what users will access
__all__ = [
    # Core NLM API (from enhanced pynlm bindings)
    "createDefaultConfig",
    "createBrain", 
    "createSimpleWorld",
    "createAgentBrain",
    # Enhanced configuration methods
    "createStandardConfig",
    "configureBrainFromPython",
    "exportBrainConfig",
    "importBrainConfig",
    "validateConfiguration",
    # Enhanced simulation methods
    "runCompleteSimulation",
    "runEpisode",
    "runSteps",
    "benchmarkPerformance",
    # Serialization methods
    "saveBrainState",
    "saveBrainStateWithValidation",
    "loadBrainState",
    "loadBrainStateWithValidation",
    # Type conversion helpers
    "neuronIdToIndex",
    "indexToNeuronId",
    "collectBrainStatistics",
    "collectAgentStatistics",
    # Debugging and monitoring
    "logBrainStatus",
    "logAgentStatus",
    # Utility functions
    "validateSimulation",
    "checkCompletionConditions",
    "calculateConvergenceScore",
    "getMemoryUsage",
    "getNeuromodulationLevels",
    # Exception types
    "NLMError",
    "ConfigError", 
    "BrainError",
    "WorldError",
    "NeuromodulationError",
    "MemoryError",
    "DevelopmentError",
    # Classes that users will use
    "Config",
    "Brain",
    "AgentBrain",
    "SimpleWorld",
    "SensoryInput",
    "Vision",
    "Audio",
    "InternalSignals",
    "Action",
    "WorldObject",
    "AgentBody",
    "ActionResult",
    "SensoryPercept",
    "SimulationStats",
    "SimulationStatus",
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
    # Neuromodulation classes
    "Dopamine",
    "Curiosity", 
    "Novelty",
    "PredictionError",
    # Memory system classes
    "WorkingMemory",
    "EpisodicMemory",
    "SemanticMemory",
    # Performance classes
    "MemoryPool",
    "PerformanceMonitor",
    "Profiler",
    # Development system classes
    "DevelopmentalSystem",
    "SynapticPlasticity",
    "StructuralPlasticity",
    # Convenience aliases for common usage
    "DEFAULT_CONFIG",
    "DEFAULT_BRAIN",
    "DEFAULT_WORLD",
    "DEFAULT_AGENT",
    # Status and logging constants
    "SIMULATION_STATUS",
    "STATISTICS_KEYS",
]

# Package initialization and setup
def setup_enhanced_package():
    """Set up the enhanced NLM package with all available functionality."""
    import logging
    
    # Configure logging
    logging.basicConfig(level=logging.INFO)
    logger = logging.getLogger(__name__)
    
    logger.info("NLM Enhanced Package initialized")
    logger.info(f"Version: {__version__}")
    
    # Check available modules
    available_modules = []
    
    # Check if core NLM module is available
    try:
        import pynlm
        available_modules.append("pynlm (core)")
    except ImportError:
        logger.warning("Core NLM module not available")
    
    # Check if helpers module is available
    try:
        from .helpers import *
        available_modules.append("helpers (simulation utilities)")
    except ImportError:
        logger.warning("Helpers module not available")
    
    logger.info(f"Available modules: {', '.join(available_modules)}")
    
    return True

# Package setup when imported
if __name__ != "__main__":
    setup_enhanced_package()

# Quick start guide for users
__quick_start__ = """
# Quick Start Guide for Enhanced NLM Python Package

## Basic Usage

```python
import nlm

# Create a standard configuration
config = nlm.createStandardConfig(neuron_count=1000, learning_rate=0.01)

# Create brain and world
brain = nlm.createBrain(config)
world = nlm.createWorldWithDefaults()
agent = nlm.createAgentBrain(brain)

# Run a complete simulation
results = nlm.runCompleteSimulation(brain, world, agent, steps=1000)

print(f"Simulation completed: {results['stats'].steps_completed} steps")
print(f"Final spike count: {results['stats'].total_spikes}")
```

## Advanced Usage

```python
import nlm

# Create configuration from Python dictionary
config_dict = {
    'neuron_count': 500,
    'learning_rate': 0.05,
    'synapse_density': 0.2
}
brain = nlm.createBrainWithDefaults()
nlm.configureBrainFromPython(brain, config_dict)

# Run episode with monitoring
agent = nlm.createAgentWithDefaults()
episode_results = nlm.runEpisode(agent, max_steps=200)

# Get detailed statistics
brain_stats = nlm.collectBrainStatistics(brain)
agent_stats = nlm.collectAgentStatistics(agent)

print(f"Brain stats: {brain_stats}")
print(f"Agent stats: {agent_stats}")
```

## Error Handling

```python
import nlm

try:
    config = nlm.createStandardConfig(neuron_count=100)
    brain = nlm.createBrain(config)
    world = nlm.createWorldWithDefaults()
    
    # Validate configuration
    nlm.validateConfiguration(brain, world, agent)
    
    # Run simulation
    results = nlm.runCompleteSimulation(brain, world, agent, steps=500)
    
except nlm.ConfigError as e:
    print(f"Configuration error: {e}")
except nlm.BrainError as e:
    print(f"Brain error: {e}")
except nlm.NLMError as e:
    print(f"NLM error: {e}")
```

## Available Features

- ✅ Enhanced NLM C++ bindings with custom exceptions
- ✅ Neuromodulation systems (dopamine, curiosity, novelty, prediction error)
- ✅ Memory systems (working memory, episodic memory, semantic memory)
- ✅ Performance optimization (memory pools, performance monitoring)
- ✅ Development systems (developmental stages, synaptic plasticity)
- ✅ Enhanced configuration with parameter validation
- ✅ Simulation helpers and convenience functions
- ✅ Serialization utilities with validation
- ✅ Statistical analysis tools
- ✅ Debugging and monitoring capabilities
- ✅ Comprehensive documentation and examples
"""