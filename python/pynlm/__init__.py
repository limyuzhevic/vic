"""
NLM Python Bindings - Core Module

This is the core module for the NLM Python bindings package. It contains:

- Core bindings functionality (compiled C++ extensions)
- Convenience functions and utilities
- High-level API for common operations
- Example code and demonstrations
"""

from .bindings import *
from .utils import *

__all__ = [
    # Identity classes
    "NeuronId",
    "SynapseId",
    "RegionId",
    "PopulationId",
    
    # Enums
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    "FiringState",
    "ActionType",
    "MotorCommand",
    "WorldObjectType",
    
    # Main classes
    "Config",
    "SensoryInput",
    "Vision",
    "Audio",
    "InternalSignals",
    "Action",
    "WorldObject",
    "AgentBody",
    "ActionResult",
    "SensoryPercept",
    "SimpleWorld",
    "Brain",
    "AgentBrain",
    
    # Utility functions
    "createDefaultConfig",
    "createBrain",
    "createSimpleWorld",
    "createAgentBrain",
    "create_simulation_config",
    "create_test_environment",
    "create_training_environment",
    
    # Global constants
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
]

# Version
__version__ = "0.1.0"

# Package metadata
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"
__author__ = "NLM Authors"
__license__ = "MIT"