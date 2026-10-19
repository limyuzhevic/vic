# NLM Python Package

__version__ = "0.1.0"

from .pynlm import (
    # Core classes
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    
    # Enumerations
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    
    # Identifiers
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    
    # Factory functions
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
)

# Export package information
__all__ = [
    "__version__",
    "Config",
    "Brain", 
    "AgentBrain",
    "SimpleWorld",
    "SensoryInput",
    "Vision",
    "Audio",
    "InternalSignals",
    "Action",
    "WorldObject",
    "AgentBody",
    "ActionResult",
    "SensoryPercept",
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    "FiringState",
    "ActionType",
    "MotorCommand",
    "WorldObjectType",
    "NeuronId",
    "SynapseId",
    "RegionId",
    "PopulationId",
    "createDefaultConfig",
    "createBrain",
    "createSimpleWorld",
    "createAgentBrain",
]