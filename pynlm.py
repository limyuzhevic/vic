"""
pynlm - Main Python module entry point

This module provides the main Python API for the NLM (Neural Learning Machine) framework.
It re-exports all core functionality from C++ bindings and provides enhanced Python wrappers.
"""

import sys
from .config import Config, ConfigSchema, ConfigValidator

# Simple version without circular imports
__version__ = "0.1.0"
__author__ = "NLM Authors"
__email__ = "contact@nlm-project.org"
__license__ = "MIT"
__copyright__ = "Copyright (c) 2026 NLM Authors"

# Re-export key components
from .config import (
    Config,
    ConfigSchema,
    ConfigValidator,
    create_default_config,
    load_config_from_file,
    load_config_from_env,
)

# Create simple factory functions for backward compatibility
createDefaultConfig = create_default_config

def createBrain(config):
    """Create a brain with configuration (placeholder for C++ bindings)."""
    print("Warning: C++ Brain bindings not available in this environment.")
    return None

def createSimpleWorld():
    """Create a simple world (placeholder for C++ bindings)."""
    print("Warning: C++ SimpleWorld bindings not available in this environment.")
    return None

def createAgentBrain(brain):
    """Create an agent brain interface (placeholder for C++ bindings)."""
    print("Warning: C++ AgentBrain bindings not available in this environment.")
    return None

# Make convenience functions available
createBrain = create_brain = createBrain
createSimpleWorld = create_world = createSimpleWorld
createAgentBrain = create_agent = createAgentBrain

# Export types from individual modules
from .brain import (
    Brain as _Brain,
    ProgressCallback,
)
from .world import (
    SimpleWorld as _SimpleWorld,
)
from .agent import (
    AgentBrain as _AgentBrain,
)
from .logger import (
    LogLevel,
    StructuredLogger,
    get_logger,
    add_to_global_stats,
    get_global_stats,
    close_all_loggers,
)

# Constants from C++ bindings (simplified)
INVALID_NEURON_ID = None
INVALID_SYNAPSE_ID = None
INVALID_REGION_ID = None
INVALID_POPULATION_ID = None

# Type aliases
SimulationStep = int
Timestamp = float
TimestepDuration = float
NeuronIndex = int
SynapseIndex = int
PopulationIndex = int
RegionIndex = int
SynapticWeight = float
MembranePotential = float
FiringRate = float
Delay = int

# Basic C++ types (simplified)
class NeuronId:
    def __init__(self, value=0):
        self.value = value

class SynapseId:
    def __init__(self, value=0):
        self.value = value

class RegionId:
    def __init__(self, value=0):
        self.value = value

class PopulationId:
    def __init__(self, value=0):
        self.value = value

class NeuronType:
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Sensory = 3
    Motor = 4
    Internal = 5

class SynapseType:
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Electrical = 3
    GapJunction = 4

class DevelopmentalStage:
    Initial = 0
    CriticalPeriod = 1
    Maturation = 2
    Adult = 3
    Aging = 4

class FiringState:
    Resting = 0
    Active = 1
    Refractory = 2
    Inhibited = 3

class ActionType:
    MoveForward = 0
    MoveBackward = 1
    MoveLeft = 2
    MoveRight = 3
    TurnLeft = 4
    TurnRight = 5
    Look = 6
    LookUp = 7
    LookDown = 8
    Interact = 9
    Eat = 10
    Drink = 11
    Rest = 12
    Wait = 13
    Custom = 14

class MotorCommand:
    MoveForward = 0
    MoveBackward = 1
    TurnLeft = 2
    TurnRight = 3
    LookLeft = 4
    LookRight = 5
    Interact = 6
    Wait = 7

class WorldObjectType:
    Empty = 0
    Resource = 1
    Hazard = 2
    Wall = 3
    Marker = 4

# Export types
__all__ = [
    # Configuration
    "Config",
    "ConfigSchema",
    "ConfigValidator",
    "create_default_config",
    "load_config_from_file",
    "load_config_from_env",
    
    # Brain wrapper
    "Brain",
    "ProgressCallback",
    
    # World wrapper
    "SimpleWorld",
    
    # Agent wrapper
    "AgentBrain",
    
    # Logger
    "LogLevel",
    "StructuredLogger",
    "get_logger",
    "add_to_global_stats",
    "get_global_stats",
    "close_all_loggers",
    
    # Convenience functions
    "createDefaultConfig",
    "createBrain",
    "createSimpleWorld",
    "createAgentBrain",
    "create_config",
    "create_brain",
    "create_world",
    "create_agent",
    
    # Constants
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    
    # Type aliases
    "SimulationStep",
    "Timestamp",
    "TimestepDuration",
    "NeuronIndex",
    "SynapseIndex",
    "PopulationIndex",
    "RegionIndex",
    "SynapticWeight",
    "MembranePotential",
    "FiringRate",
    "Delay",
    
    # C++ types
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
    
    # Module info
    "__version__",
    "__author__",
    "__email__",
    "__license__",
    "__copyright__",
]