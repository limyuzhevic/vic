# __init__.py - Main pynlm module

from .bindings import (
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    Config,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    SimpleWorld,
    Brain,
    AgentBrain,
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
)

from .exceptions import NLMRuntimeError, NLMConfigError, NLMValidationError
from .utils import (
    createDefaultConfig,
    createSimulationConfig,
    createSmallBrainConfig,
    createLargeBrainConfig,
    createVisionFromArray,
    createActionFromDict,
    createSensoryPerceptFromVision,
    benchmarkBrain,
    runStandardExperiment,
    compareBrains,
    createRandomConfig,
    validateBrainConfiguration,
    runControlledSimulation,
)

from .examples import (
    run_simple_brain_example,
    run_agent_world_example,
    run_complete_agent_example,
)

__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Python bindings for NLM (Neural Learning Machine)"