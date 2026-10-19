"""
NLM Environment Module

This module contains environment and observation components. These define the
virtual environment in which the neural network operates.
"""

from .environment import Environment
from .observation import Observation

__all__ = ['Environment', 'Observation']
