"""
NLM - Neural Learning Machine

An experimental artificial developmental brain with Python bindings.

This package provides Python access to the NLM C++ neural simulation framework.
"""

__version__ = "0.1.0"
__author__ = "NLM Authors"
__all__ = [
    'createBrain',
    'createDefaultConfig',
    'createSimpleWorld',
    'createAgentBrain',
    'Brain',
    'Config',
    'SimpleWorld',
    'AgentBrain',
    'Vision',
    'Audio',
    'Action',
    'SensoryPercept',
    'ActionType',
    'MotorCommand',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'WorldObjectType',
]

from .core import *
from .config import *
from .simulation import *
from .vision import *
from .agent import *
from .world import *
from .motor import *
from .sensory import *
from .environment import *
from .types import *

import logging

# Configure logging for Python users
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)
