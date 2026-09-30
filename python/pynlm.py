"""
NLM (Neural Learning Machine) Python Bindings
================================================

A Python interface to the NLM (熙然) neural simulation framework.
This module provides access to the C++ NLM brain simulation system with
a Pythonic API that is both beginner-friendly and powerful for advanced users.

The NLM is an experimental artificial developmental brain that learns from
interaction with environments through neurons, synapses, and neuromodulation.

## Core Classes

The module provides access to the following main classes:

- `Config`: Brain configuration and parameters
- `Brain`: The neural simulation brain
- `AgentBrain`: Interface connecting brain to world
- `SimpleWorld`: 2D simulation world
- `SensoryPercept`: What the agent perceives
- `Action`: Motor output from the brain

## Basic Usage Example

```python
import pynlm

# Create a default brain configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Create an agent with world integration
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent.initialize(world)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Brain version: {pynlm.__version__}")
```

## Documentation

See the individual class and method docstrings for detailed information.
The module follows PEP 8 style guidelines and includes comprehensive
documentation with examples.

## Type Hints

This module uses Python type hints throughout for better developer
experience and IDE support. Optional types are clearly indicated with
`Optional[]` and collections use `List[]`, `Dict[]`, etc.

## Error Handling

The module provides comprehensive error handling with custom exceptions
for common failure cases:

- `PyNLMRuntimeError`: Runtime errors in the NLM system
- `PyNLMConfigurationError`: Configuration-related errors
- `PyNLMInitializationError`: Brain initialization errors

## Performance Considerations

For optimal performance in long-running simulations:

1. Reuse brain instances rather than creating new ones
2. Use `save()` and `load()` for checkpointing
3. Consider parallel processing with multiple brain instances
4. Monitor memory usage with the provided statistics methods

## License

MIT License. See LICENSE file for details.
"""

__version__ = "0.1.0"

# Custom exceptions
class PyNLMRuntimeError(RuntimeError):
    """Base exception for NLM runtime errors."""
    pass
class PyNLMConfigurationError(PyNLMRuntimeError):
    """Exception raised for configuration-related errors."""
    pass
class PyNLMInitializationError(PyNLMRuntimeError):
    """Exception raised for brain initialization errors."""
    pass
# Import the C++ bindings
from .pynlm_core import (
    # Core classes
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    SensoryPercept,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    Vision,
    Audio,
    InternalSignals,

    # Enums
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,

    # Factory functions
    createDefaultConfig,
    createBrain,
    createAgentBrain,
    createSimpleWorld,

    # Invalid IDs (for comparison)
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
)

# Pythonic aliases for common types
from typing import List, Dict, Optional, Union, Tuple, Any

# Re-export all public symbols
__all__ = [
    # Version
    '__version__',
    
    # Exceptions
    'PyNLMRuntimeError',
    'PyNLMConfigurationError',
    'PyNLMInitializationError',
    
    # Core classes
    'Config',
    'Brain',
    'AgentBrain',
    'SimpleWorld',
    'SensoryPercept',
    'Action',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    'Vision',
    'Audio',
    'InternalSignals',
    
    # Enums
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    
    # Factory functions
    'createDefaultConfig',
    'createBrain',
    'createAgentBrain',
    'createSimpleWorld',
    
    # Invalid IDs
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Type hints
    'List', 'Dict', 'Optional', 'Union', 'Tuple', 'Any',
]

# Initialize the module
def _init_module():
    """Initialize the Python module."""
    import sys
    import logging

    # Set up logging
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    )
    
    # Log module loading
    logger = logging.getLogger('pynlm')
    logger.info(f"Loading NLM Python bindings version {__version__}")
    
    # Verify that core C++ bindings are available
    try:
        # Test by creating a config object
        config = createDefaultConfig()
        logger.debug("Core C++ bindings successfully initialized")
    except Exception as e:
        logger.error(f"Failed to initialize C++ bindings: {e}")
        raise PyNLMRuntimeError(f"Failed to initialize NLM bindings: {e}")

# Auto-initialize the module
_init_module()
