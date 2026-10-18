"""
NLM Python bindings for neural simulation.

This module provides Python interfaces to the NLM (Neural Learning Machine) framework,
a computational model of artificial brains with neurons, synapses, and learning mechanisms.

Key components:
- Brain: The central neural simulation engine
- World: The environment containing the brain
- Agent: Interface between brain and world
- Config: Configuration management for NLM systems

Examples:
    import pynlm
    
    # Create a simple brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation
    for i in range(100):
        brain.step(i)
"""

# Import main components
from pynlm_core import (
    Brain,
    Config,
    SimpleWorld,
    AgentBrain,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    NeuronType,
    SynapseType,
    ActionType,
    MotorCommand,
    WorldObjectType,
    DevelopmentalStage,
    FiringState,
)

# Import factory functions
from pynlm_core import (
    createBrain,
    createDefaultConfig,
    createSimpleWorld,
    createAgentBrain,
)

# Import special values
from pynlm_core import (
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
)

# Create builder functions
from pynlm_core import BrainBuilder

# Create convenience functions
from pynlm_core import run_simulation, quick_brain

# Create error types
from pynlm_core import (
    NLMError,
    ConfigurationError,
    SimulationError,
    MemoryError,
)

__version__ = "0.1.0"
__author__ = "NLM Authors"

# Package metadata
__all__ = [
    # Core classes
    'Brain',
    'Config',
    'SimpleWorld',
    'AgentBrain',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    'SensoryPercept',
    
    # Enums
    'NeuronType',
    'SynapseType',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    'DevelopmentalStage',
    'FiringState',
    
    # Factory functions
    'createBrain',
    'createDefaultConfig',
    'createSimpleWorld',
    'createAgentBrain',
    
    # Special values
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Helper classes
    'BrainBuilder',
    
    # Convenience functions
    'run_simulation',
    'quick_brain',
    
    # Error types
    'NLMError',
    'ConfigurationError',
    'SimulationError',
    'MemoryError',
    
    # Module info
    '__version__',
    '__author__',
]
