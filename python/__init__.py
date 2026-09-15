"""
NLM (Neural Learning Machine) Python Bindings

This module provides Python bindings for the NLM C++ neural simulation framework.
It exposes the core brain simulation functionality including neurons, synapses,
memory systems, and agent interfaces.

The NLM brain is an experimental artificial developmental brain that learns
from experience through neural dynamics, plasticity, and neuromodulation.

Example Usage:
    >>> import pynlm
    >>> config = pynlm.createDefaultConfig()
    >>> brain = pynlm.createBrain(config)
    >>> brain.initialize()
    >>> brain.step(0)
    >>> print(f"Neurons: {brain.getTotalNeuronCount()}")
"""

__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"
__copyright__ = "Copyright (c) 2026 NLM Project"

# The C++ extension module will be imported directly
# This __init__.py provides Python-level convenience functions and metadata

import sys

# Version and metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"
__copyright__ = "Copyright (c) 2026 NLM Project"

# Re-export everything from the C++ extension module
# The actual bindings are defined in the compiled C++ extension
from pynlm.bindings import *

# Import all public classes from the C++ module
__all__ = [
    # Configuration and Setup
    'Config',
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    
    # Brain Components
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand',
    'WorldObjectType', 'MotorCommand',
    
    # Sensory and Perception
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'SensoryPercept',
    
    # Motor and Action
    'Action', 'WorldObject',
    'AgentBody', 'ActionResult',
    
    # Agents and Interfaces
    'AgentBrain', 'AgentBody',
    
    # World Simulation
    'SimpleWorld',
    
    # Constants and Types
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID', 'INVALID_POPULATION_ID'
]

# Module-level functions for convenience
def get_version():
    """Get the version of the pynlm module."""
    return __version__

def get_info():
    """Get information about the NLM Python bindings."""
    return {
        'name': 'pynlm',
        'version': __version__,
        'description': 'Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework',
        'author': __author__,
        'license': __license__,
        'website': 'https://github.com/nlm-project/nlm',
        'documentation': 'https://nlm.readthedocs.io'
    }