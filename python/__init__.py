# NLM Python Module
# Package for Neural Learning Machine Python bindings

__version__ = "0.1.0"
__all__ = [
    # Core types and IDs
    "NeuronId",
    "SynapseId", 
    "RegionId",
    "PopulationId",
    
    # Enumerations
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    "FiringState",
    "ActionType",
    "MotorCommand",
    "WorldObjectType",
    
    # Core classes
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
    
    # Factory functions
    "createDefaultConfig",
    "createBrain",
    "createSimpleWorld",
    "createAgentBrain",
    
    # Constants
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID", 
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
]

# Import all exposed classes from the bindings
from pynlm_bindings import (
    # Core types and IDs
    NeuronId, SynapseId, RegionId, PopulationId,
    
    # Enumerations
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    
    # Core classes
    Config, SensoryInput, Vision, Audio, InternalSignals,
    Action, WorldObject, AgentBody, ActionResult, SensoryPercept,
    SimpleWorld, Brain, AgentBrain,
    
    # Factory functions
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    
    # Constants
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID,
    INVALID_REGION_ID, INVALID_POPULATION_ID,
)

# Re-export for backward compatibility
brain = type('brain', (), {})()