# Brain module for NLM Python package

# Import brain classes
from .neuron import Neuron
from .synapse import Synapse
from .neural_population import NeuralPopulation
from .neural_region import NeuralRegion
from .brain import Brain

# Re-export commonly used classes
__all__ = [
    'Neuron',
    'Synapse',
    'NeuralPopulation',
    'NeuralRegion',
    'Brain'
]