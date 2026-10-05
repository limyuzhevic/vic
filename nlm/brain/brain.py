"""
NLM brain.py module for C++ brain integration.

This module provides the main Brain class interface for the NLM neural simulation framework.
"""

from .brain import Brain
from .neuron import Neuron
from .synapse import Synapse
from .neural_population import NeuralPopulation
from .neural_region import NeuralRegion

__all__ = [
    'Brain',
    'Neuron',
    'Synapse',
    'NeuralPopulation',
    'NeuralRegion',
]
