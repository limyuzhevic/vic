# NLM - Neural Learning Machine
"""
NLM (熙然) - An Experimental Artificial Developmental Brain

This is a Python interface to the NLM C++ neural simulation framework.
It provides classes for brain simulation, neural networks, and cognitive modeling.

Key Features:
- Brain simulation with spiking neurons
- Multiple memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, reward)
- Developmental stages and plasticity
- Integration with world simulation
- Checkpoint save/load capability
"""

from .core import Config
from .brain import Brain
from .agent import AgentBrain
from .world import SimpleWorld
from .sensory import Vision, Audio, InternalSignals
from .motor import Action

__version__ = "0.1.0"
__author__ = "NLM Authors"
__all__ = [
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
    'Vision', 'Audio', 'InternalSignals', 'Action',
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain'
]

# Factory functions
def createDefaultConfig():
    """Create a default configuration for NLM."""
    return Config()

def createBrain(config):
    """Create a new brain with the given configuration."""
    return Brain(config)

def createSimpleWorld():
    """Create a simple 2D world for simulation."""
    return SimpleWorld()

def createAgentBrain(brain):
    """Create an agent brain interface connecting brain to world."""
    return AgentBrain(brain)
