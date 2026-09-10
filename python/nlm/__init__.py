"""
NLM (Neural Learning Machine) - Python Interface

A Python wrapper for the NLM C++ neural simulation framework.
Provides high-level access to brain simulation, learning, and agent systems.
"""

from .config import Config
from .brain import Brain, BrainSimulator
from .world import World, SimpleWorld, WorldObject
from .agent import Agent, AgentBrain
from .simulation import Simulation, run_episode, run_learning_session
from .exceptions import NLMRuntimeError, NLMConfigurationError, NLMValidationError
from .utils import LearningMetrics, EpisodeStats, SimulationLogger
from .types import *

__version__ = "0.1.0"
__author__ = "NLM Project"
__all__ = [
    'Config', 'Brain', 'BrainSimulator',
    'World', 'SimpleWorld', 'WorldObject',
    'Agent', 'AgentBrain',
    'Simulation', 'run_episode', 'run_learning_session',
    'NLMRuntimeError', 'NLMConfigurationError', 'NLMValidationError',
    'LearningMetrics', 'EpisodeStats', 'SimulationLogger',
    # Types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType',
    'ConfigKey', 'SensoryInputType', 'ActionResultType'
]

# Set up logging
import logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)