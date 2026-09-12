import pynlm

"""
Pattern Examples for NLM Python API

This module contains common simulation patterns that demonstrate
how to use the improved NLM Python API effectively.

Each pattern represents a different approach to using NLM for
neural simulation and experimentation.
"""

# Basic simulation loop pattern
from .basic_simulation import BasicSimulation
from .training_pattern import TrainingPattern
from .developmental_pattern import DevelopmentalPattern
from .multi_agent_pattern import MultiAgentPattern
from .experimental_pattern import ExperimentalPattern

__all__ = [
    'BasicSimulation',
    'TrainingPattern', 
    'DevelopmentalPattern',
    'MultiAgentPattern',
    'ExperimentalPattern'
]
