"""
NLM World simulation.

This module provides the SimpleWorld class for simulating 2D environments
that agents interact with. The world represents the external environment
in which the NLM agent operates, providing sensory input and receiving
motor output from the agent.
"""

from .world import SimpleWorld, WorldObject, ActionResult

__all__ = ["SimpleWorld", "WorldObject", "ActionResult"]
