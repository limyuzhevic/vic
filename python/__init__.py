# NLM Python Package

# This package provides Python bindings for the Neural Learning Machine (NLM) neural simulation framework.

from nlm.bindings import *

__all__ = [
    # Core types
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    "NeuronType", "SynapseType", "DevelopmentalStage", "FiringState",
    "ActionType", "MotorCommand", "WorldObjectType",
    # Classes
    "Config", "Vision", "Audio", "InternalSignals", "SensoryInput",
    "Action", "WorldObject", "AgentBody", "ActionResult",
    "SensoryPercept", "SimpleWorld", "Brain", "AgentBrain",
    # Constants
    "INVALID_NEURON_ID", "INVALID_SYNAPSE_ID", "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    # Factory functions
    "createDefaultConfig", "createBrain", "createSimpleWorld",
    "createAgentBrain",
]

# Version information
__version__ = "1.0.0"

# Module metadata
__author__ = "NLM Development Team"
__license__ = "MIT"
__copyright__ = "Copyright (c) 2024 NLM Project"

# Copyright notice
# This package is part of the NLM (Neural Learning Machine) project.
# All rights reserved.

# Usage example:
# import nlm
# 
# config = nlm.createDefaultConfig()
# brain = nlm.createBrain(config)
# world = nlm.createSimpleWorld()
# agent_brain = nlm.createAgentBrain(brain)
