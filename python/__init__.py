"""NLM - Neural Learning Machine Python Bindings"""

__version__ = "0.1.0"

# Import all major components for easy usage
from pynlm.bindings import (
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
)

# Create submodules for better organization
from pynlm import brain
from pynlm import world
from pynlm import agent
from pynlm import sensory
from pynlm import motor
from pynlm import memory
from pynlm import neuromodulation
from pynlm import prediction
from pynlm import cognition
from pynlm import development
from pynlm import plasticity
from pynlm import dynamics

# Configure logging
import logging
logging.basicConfig(level=logging.WARNING)

__all__ = [
    # Core constructors
    'createDefaultConfig',
    'createBrain', 
    'createSimpleWorld',
    'createAgentBrain',
    # Constants
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    # Submodules
    'brain', 'world', 'agent', 'sensory', 'motor',
    'memory', 'neuromodulation', 'prediction', 'cognition',
    'development', 'plasticity', 'dynamics',
]