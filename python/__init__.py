"""
Enhanced NLM Python Bindings

This module provides improved Python bindings for the NLM (Neural Learning Machine) C++ neural simulation framework.

The bindings have been enhanced to be more maintainable, feature-rich, and user-friendly:

1. **Modular Structure**: Split into multiple binding files for better organization
   - brain_bindings.cpp: Brain and neural types
   - config_bindings.cpp: Configuration management
   - agent_bindings.cpp: Agent behavior and control
   - world_bindings.cpp: World simulation

2. **Enhanced Documentation**: Comprehensive docstrings with examples

3. **Improved Error Handling**: Custom exceptions for better error messages

4. **Convenience Methods**: Factory functions and helper methods

5. **Type Safety**: Validation utilities and type checking

6. **Advanced Features**: Batch operations, NumPy integration, visualization tools

Usage Examples:
"""

from .nlm.core.binding_helpers import (
    NLMError,
    ConfigError,
    BrainError,
    WorldError,
    TypeValidator,
    BatchOperations,
    VisualizationUtils,
    factory,
)

# Export core classes for backward compatibility
from .nlm.core.binding_helpers import NeuronId, SynapseId, RegionId, PopulationId
from .nlm.core.binding_helpers import NeuronType, SynapseType, DevelopmentalStage, FiringState
from .nlm.core.binding_helpers import ActionType, WorldObjectType

# Export main classes
from .nlm.core.binding_helpers import Config, SensoryInput, Vision, Audio, InternalSignals
from .nlm.core.binding_helpers import Action, WorldObject, AgentBody, ActionResult
from .nlm.core.binding_helpers import SensoryPercept, SimpleWorld, Brain, AgentBrain

# Export factory functions and utilities
from .nlm.core.binding_helpers import createDefaultConfig
from .nlm.core.binding_helpers import createBrain
from .nlm.core.binding_helpers import createSimpleWorld
from .nlm.core.binding_helpers import createAgentBrain
from .nlm.core.binding_helpers import batchInjectCurrent
from .nlm.core.binding_helpers import batchProduceAction
from .nlm.core.binding_helpers import getConnectivityMatrix
from .nlm.core.binding_helpers import getActivitySummary
from .nlm.core.binding_helpers import to_numpy_array
from .nlm.core.binding_helpers import to_numpy_matrix
from .nlm.core.binding_helpers import visionToNumpy

# Export validation utilities
from .nlm.core.binding_helpers import validateNeuronId
from .nlm.core.binding_helpers import validateRange
from .nlm.core.binding_helpers import validateNonNegative
from .nlm.core.binding_helpers import validatePositive

# Export constants and sentinels
from .nlm.core.binding_helpers import INVALID_NEURON_ID
from .nlm.core.binding_helpers import INVALID_SYNAPSE_ID
from .nlm.core.binding_helpers import INVALID_REGION_ID
from .nlm.core.binding_helpers import INVALID_POPULATION_ID

__all__ = [
    # Types
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    "NeuronType", "SynapseType", "DevelopmentalStage", "FiringState",
    "ActionType", "WorldObjectType",
    
    # Classes
    "Config", "SensoryInput", "Vision", "Audio", "InternalSignals",
    "Action", "WorldObject", "AgentBody", "ActionResult",
    "SensoryPercept", "SimpleWorld", "Brain", "AgentBrain",
    
    # Exceptions
    "NLMError", "ConfigError", "BrainError", "WorldError",
    
    # Factory functions
    "createDefaultConfig", "createBrain", "createSimpleWorld", "createAgentBrain",
    
    # Advanced APIs
    "batchInjectCurrent", "batchProduceAction",
    "getConnectivityMatrix", "getActivitySummary",
    "to_numpy_array", "to_numpy_matrix", "visionToNumpy",
    
    # Validation utilities
    "validateNeuronId", "validateRange", "validateNonNegative", "validatePositive",
    
    # Constants
    "INVALID_NEURON_ID", "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID", "INVALID_POPULATION_ID",
    
    # Utilities
    "TypeValidator", "BatchOperations", "VisualizationUtils", "factory",
]

# Quick Start Example

"""
# Basic usage example:

import pynlm as nlm

# Create a brain with default configuration
config = nlm.createDefaultConfig()
brain = nlm.Brain(config)
brain.initialize()

# Create a world
world = nlm.SimpleWorld(100, 100, 10, 10)
world.setAgentStart(50, 50)

# Create agent brain interface
agent_brain = nlm.AgentBrain(brain)
agent_brain.initialize(world)

# Run simulation
for step in range(100):
    percept = world.getSensoryPercept()
    agent_brain.processSensoryInput(percept)
    action = agent_brain.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    world.update(0.01)  # 10ms timestep

# Get brain statistics
activity = nlm.getActivitySummary(brain)
print(f"Active neurons: {activity['active_neurons']}")
print(f"Firing neurons: {activity['firing_neurons']}")
"""

"""
# Advanced usage example with NumPy integration:

import pynlm as nlm
import numpy as np

# Create vision input
vision = nlm.Vision(640, 480, 3)
data = np.random.rand(640 * 480 * 3).astype(np.float32)
vision.setData(data.tolist())

# Convert to numpy array for processing
numpy_array = nlm.visionToNumpy(vision)
processed_data = numpy_array * 2.0  # Example processing

# Create configuration with validation
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 200)
config.set("brain.synaptic_weight", 0.15)

# Validate configuration
if nlm.validateConfig(config):
    brain = nlm.Brain(config)
    brain.initialize()
    print("Configuration validated successfully")
"""