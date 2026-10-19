"""
NLM World Module

This module contains world components including world objects and simple world
implementation. These define the virtual environment for simulation.
"""

from .world_object import WorldObject
from .simple_world import SimpleWorld

__all__ = ['WorldObject', 'SimpleWorld']
