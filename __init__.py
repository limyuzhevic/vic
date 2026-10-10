"""
NLM - High-Level API Module

This module provides simplified, high-level APIs for common NLM use cases.
It abstracts away the complexity of the underlying brain simulation framework
while maintaining full access to advanced features when needed.
"""

from .pynlm_api import (
    NLMAPI,
    SimulationResult,
    TrainingResult,
    SimulationMode,
    create_simple_neural_simulation,
    create_agent_training_simulation,
    create_experiment_simulation,
    run_legacy_simulation,
    run_legacy_agent_simulation,
)

# Re-export the original pynlm module for backward compatibility
import pynlm
from pynlm import (
    # Original pynlm exports (from bindings.cpp)
    Config,
    createDefaultConfig,
    createBrain,
    Brain,
    AgentBrain,
    createAgentBrain,
    SimpleWorld,
    createSimpleWorld,
    Vision,
    Audio,
    InternalSignals,
    SensoryInput,
    Action,
    ActionType,
    MotorCommand,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    WorldObjectType,
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
)

__all__ = [
    # High-level API
    'NLMAPI',
    'SimulationResult',
    'TrainingResult',
    'SimulationMode',
    'create_simple_neural_simulation',
    'create_agent_training_simulation',
    'create_experiment_simulation',
    'run_legacy_simulation',
    'run_legacy_agent_simulation',
    
    # Original pynlm APIs (for backward compatibility)
    'Config',
    'createDefaultConfig',
    'createBrain',
    'Brain',
    'AgentBrain',
    'createAgentBrain',
    'SimpleWorld',
    'createSimpleWorld',
    'Vision',
    'Audio',
    'InternalSignals',
    'SensoryInput',
    'Action',
    'ActionType',
    'MotorCommand',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    'SensoryPercept',
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'WorldObjectType',
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
]

# Version information
__version__ = "1.0.0"
__author__ = "NLM Team"
__description__ = "High-level API abstractions for NLM neural simulation"

# Module metadata
__license__ = "MIT"
__copyright__ = "Copyright (c) 2024 NLM Project"
