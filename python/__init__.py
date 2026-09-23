# NLM Python Package
"""
Python bindings for NLM (Neural Learning Machine) - Phase 6 Integration

This package provides Python interfaces to the NLM brain simulation framework.
All systems are fully integrated including memory, neuromodulation, prediction,
and cognition.

Main Components:
- Brain: The neural network with all integrated subsystems
- AgentBrain: Interface for sensory-motor interaction
- SimpleWorld: Virtual environment for agent interaction
- Various neural dynamics and learning mechanisms

Examples:
    >>> import pynlm
    >>> config = pynlm.createDefaultConfig()
    >>> brain = pynlm.createBrain(config)
    >>> brain.initialize()
    >>> brain.step(0)
    >>> brain.getTotalNeuronCount()
    1000
"""

from .pynlm import (
    # Core classes
    Brain, Config, AgentBrain, SimpleWorld, WorldObject,
    SensoryPercept, MotorCommand, NeuronType, DevelopmentalStage,
    ActionType, WorldObjectType,
    
    # Factory functions
    createBrain, createDefaultConfig, createAgentBrain, 
    createSimpleWorld,
    
    # Utility functions
    get_version, print_banner,
    
    # Integration tests
    run_phase6_integration_test,
    
    # Checkpoint operations
    save_brain_checkpoint, load_brain_checkpoint,
    
    # Advanced creation functions
    create_advanced_brain, create_complex_world,
    
    # Advanced simulations
    run_advanced_simulation,
)

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine) Phase 6 Integration"
__all__ = [
    # Core classes
    'Brain', 'Config', 'AgentBrain', 'SimpleWorld', 'WorldObject',
    'SensoryPercept', 'MotorCommand', 'NeuronType', 'DevelopmentalStage',
    'ActionType', 'WorldObjectType',
    
    # Factory functions
    'createBrain', 'createDefaultConfig', 'createAgentBrain',
    'createSimpleWorld',
    
    # Utility functions
    'get_version', 'print_banner',
    
    # Integration tests
    'run_phase6_integration_test',
    
    # Checkpoint operations
    'save_brain_checkpoint', 'load_brain_checkpoint',
    
    # Advanced creation functions
    'create_advanced_brain', 'create_complex_world',
    
    # Advanced simulations
    'run_advanced_simulation',
]

# Convenience aliases for common usage
__all__.extend([
    'brain', 'config', 'agent', 'world', 'object'
])

# Runtime validation
import sys
if sys.version_info < (3, 8):
    raise RuntimeError("pynlm requires Python 3.8 or higher")