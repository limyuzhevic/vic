# NLM (Neural Learning Machine) Python Package
"""
NLM (熙然) - Python bindings for the Neural Learning Machine simulation framework.

This package provides Python interfaces to the complete NLM brain simulation system,
including neural computation, neuromodulation, learning, and world interaction.

For detailed documentation, visit: https://github.com/yourusername/nlm
"""

__version__ = "2.6.0"
__author__ = "NLM Development Team"
__email__ = "contact@nlm.org"

# Import main classes for convenience
from .brain import Brain
from .agent import AgentBrain
from .world import SimpleWorld
from .sensory import Vision, Audio, InternalSignals, SensoryInput
from .motor import Action
from .config import Config

# Core types
from .types import (
    NeuronId, SynapseId, RegionId, PopulationId, 
    NeuronType, SynapseType, DevelopmentalStage,
    FiringState, ActionType, MotorCommand, WorldObjectType
)

# Type aliases
from .types import IdType, ValueType, SimulationStep, Timestamp

# Constants
from .types import (
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID, 
    INVALID_REGION_ID, INVALID_POPULATION_ID
)

# Utility functions for convenience
from .config import (
    createDefaultConfig, loadConfigFromFile, 
    createConfigFromDict, ConfigSource
)

# Try to import from C++ bindings (when available)
try:
    from ._pynlm import createBrain, createSimpleWorld, createAgentBrain
except ImportError:
    # Create stub functions for testing/development
    def createBrain(*args, **kwargs):
        raise NotImplementedError("C++ bindings not available")
    
    def createSimpleWorld(*args, **kwargs):
        raise NotImplementedError("C++ bindings not available")
    
    def createAgentBrain(*args, **kwargs):
        raise NotImplementedError("C++ bindings not available")

__all__ = [
    # Version info
    "__version__",
    "__author__",
    "__email__",
    
    # Main classes
    "Brain",
    "AgentBrain", 
    "SimpleWorld",
    "Vision",
    "Audio",
    "InternalSignals",
    "SensoryInput",
    "Action",
    "Config",
    
    # Neural components
    "Neuron",
    "Synapse",
    "NeuralRegion",
    
    # Type definitions
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
    
    # Type aliases
    "IdType",
    "ValueType",
    "SimulationStep",
    "Timestamp",
    
    # Constants
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    
    # Utility functions
    "createDefaultConfig",
    "loadConfigFromFile",
    "createConfigFromDict",
    "ConfigSource",
    
    # Re-exported functions
    "createBrain",
    "createSimpleWorld",
    "createAgentBrain",
]

# Package metadata
if __name__ == "__main__":
    import json
    import os
    
    # Write package metadata
    metadata = {
        "name": "pynlm",
        "version": __version__,
        "description": "Python bindings for Neural Learning Machine",
        "author": __author__,
        "email": __email__,
        "license": "MIT",
        "homepage": "https://github.com/yourusername/nlm",
        "platforms": ["Linux", "macOS", "Windows"],
        "python_requires": ">=3.8",
        "install_requires": [
            "pybind11>=2.10.0",
            "numpy>=1.21.0",
        ],
    }
    
    with open(os.path.join(os.path.dirname(__file__), "pyproject.toml"), "w") as f:
        f.write(json.dumps(metadata, indent=2))
    
    print("Package metadata generated.")