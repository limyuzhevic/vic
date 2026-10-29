"""
NLM Python Bindings - Neural Learning Machine

A Python interface for the NLM (Neural Learning Machine) C++ neural simulation framework.
This package provides Python bindings for creating artificial neural systems that learn
and develop through interaction with an environment.

Key Features:
- Real spiking neural networks (LIF neurons)
- Experience-driven synaptic plasticity (STDP, Hebbian)
- Neuromodulation and reward learning
- Memory systems (working, episodic, associative)
- Sensorimotor integration and control
- Developmental stages and plasticity regulation
- Checkpoint saving and replay

Usage Examples:
    >>> import pynlm
    >>> config = pynlm.createDefaultConfig()
    >>> brain = pynlm.createBrain(config)
    >>> brain.initialize()
    >>> for i in range(100):
    ...     brain.step(i)

    >>> # With agent-world interaction
    >>> world = pynlm.createSimpleWorld()
    >>> agent = pynlm.createAgentBrain(brain)
    >>> agent.initialize(world)

For command-line interface:
    python -m pynlm help
    python -m pynlm demo --episodes 10 --steps 200

For comprehensive documentation, visit: https://nlm.readthedocs.io
"""

__version__ = "0.1.0"
__author__ = "NLM Project Contributors"
__license__ = "MIT"

from .pynlm import (
    # Core neural system
    Brain, Config, SimpleWorld,
    
    # Agent and brain interface
    AgentBrain, SensoryPercept, Action, ActionResult,
    
    # Factory functions
    createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain,
    
    # Enumerations
    NeuronType, SynapseType, DevelopmentalStage, ActionType, MotorCommand,
    
    # Core types
    NeuronId, SynapseId, RegionId, SimulationStep,
    
    # Constants and parameters
    NLM_PI, NLM_E, NLM_DEFAULT_NEURON_COUNT, NLM_DEFAULT_REGION_COUNT,
)

# Make all exported symbols available at package level
__all__ = [
    # Core neural system
    'Brain', 'Config', 'SimpleWorld',
    
    # Agent and brain interface
    'AgentBrain', 'SensoryPercept', 'Action', 'ActionResult',
    
    # Factory functions
    'createBrain', 'createDefaultConfig', 'createSimpleWorld', 'createAgentBrain',
    
    # Enumerations
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'ActionType', 'MotorCommand',
    
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'SimulationStep',
    
    # Constants and parameters
    'NLM_PI', 'NLM_E', 'NLM_DEFAULT_NEURON_COUNT', 'NLM_DEFAULT_REGION_COUNT',
]

# Provide a helpful error message if the underlying C++ library is not available
def __getattr__(name):
    if name in __all__:
        try:
            from .pynlm import __getattr__ as _getattr
            return _getattr(name)
        except (ImportError, AttributeError):
            raise ImportError(
                f"Cannot import '{name}' from pynlm. "
                "The underlying C++ extension may not be built. "
                "Please install the pynlm package properly."
            )
    raise AttributeError(f"module '{__name__}' has no attribute '{name}'")