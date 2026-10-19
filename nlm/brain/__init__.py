"""
NLM Brain Module

This module contains the core neural network components: neurons, synapses, populations,
regions, and the brain itself. These are the fundamental building blocks of NLM.
"""

from .neuron import Neuron
from .synapse import Synapse
from .neural_population import NeuralPopulation
from .neural_region import NeuralRegion
from .brain import Brain

__all__ = ['Neuron', 'Synapse', 'NeuralPopulation', 'NeuralRegion', 'Brain']
