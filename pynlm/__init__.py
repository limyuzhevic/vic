# NLM Python Package
# Improved Python API for the Neural Learning Machine (NLM) brain simulator

"""
NLM (Neural Learning Machine) is an experimental computational brain project.
This package provides a Python interface to the NLM C++ neural simulation framework.

The NLM brain begins in a primitive developmental state and acquires increasingly 
complex abilities through interaction with an environment.
"""

__version__ = "0.1.0"

from .brain import Brain
from .config import Config
from .world import SimpleWorld
from .agent import AgentBrain
from .sensory import Vision, Audio, InternalSignals, SensoryInput
from .motor import Action
from .agent_body import AgentBody
from .action_result import ActionResult
from .sensory_percept import SensoryPercept
from .types import NeuronId, SynapseId, RegionId, PopulationId, NeuronType, SynapseType
from .development import DevelopmentalStage
from .cognition import ActionType, MotorCommand
from .world_objects import WorldObject
from .neuromodulation import Dopamine, Curiosity, Novelty, PredictionError

__all__ = [
    # Core components
    "Brain",
    "Config",
    "SimpleWorld",
    "AgentBrain",
    
    # Sensory systems
    "Vision",
    "Audio", 
    "InternalSignals",
    "SensoryInput",
    "SensoryPercept",
    
    # Motor system
    "Action",
    "ActionType",
    "MotorCommand",
    
    # Supporting components
    "AgentBody",
    "ActionResult",
    "WorldObject",
    
    # Types and enums
    "NeuronId",
    "SynapseId",
    "RegionId",
    "PopulationId",
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    
    # Neuromodulation
    "Dopamine",
    "Curiosity",
    "Novelty",
    "PredictionError",
    
    # Constants
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID", 
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    "INVALID_NEURON_ID",
]
