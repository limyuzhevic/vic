"""Main Python module exposing NLM C++ bindings."""

# Import from the bindings module
from .bindings import (
    # Config class
    Config,
    createDefaultConfig,
    
    # Brain class
    Brain,
    createBrain,
    
    # SimpleWorld class
    SimpleWorld,
    createSimpleWorld,
    
    # AgentBrain class
    AgentBrain,
    createAgentBrain,
    
    # Data classes
    Action,
    ActionType,
    WorldObject,
    WorldObjectType,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    SensoryPercept,
    AgentBody,
    ActionResult,
    
    # ID classes
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    
    # Enumerations
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    MotorCommand,
)

# Re-export all symbols for easy access
__all__ = [
    # Config
    "Config",
    "createDefaultConfig",
    
    # Brain
    "Brain",
    "createBrain",
    
    # World
    "SimpleWorld",
    "createSimpleWorld",
    
    # Agent
    "AgentBrain",
    "createAgentBrain",
    
    # Data
    "Action",
    "ActionType",
    "WorldObject",
    "WorldObjectType",
    "SensoryInput",
    "Vision",
    "Audio",
    "InternalSignals",
    "SensoryPercept",
    "AgentBody",
    "ActionResult",
    
    # IDs
    "NeuronId",
    "SynapseId",
    "RegionId",
    "PopulationId",
    
    # Enums
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    "FiringState",
    "MotorCommand",
]
