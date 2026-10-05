"""
NLM brain module __init__.py

This module provides the Python package structure for the brain component of the NLM neural simulation framework.
"""

# Import core classes from brain submodules
from .brain import Brain
from .neuron import Neuron
from .synapse import Synapse
from .neural_population import NeuralPopulation
from .neural_region import NeuralRegion
from .python_utils import (
    PythonSessionLogger,
    PythonTimer,
    numpy_utils,
    numpy_export,
    numpy_visualization,
)

# Export all public classes and utilities
__all__ = [
    'Brain',
    'Neuron',
    'Synapse',
    'NeuralPopulation',
    'NeuralRegion',
    'PythonSessionLogger',
    'PythonTimer',
    'numpy_utils',
    'numpy_export',
    'numpy_visualization',
]
