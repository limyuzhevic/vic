"""
NLM Agent brain interface.

This module provides the AgentBrain class that connects NLM brain to world,
handling sensory transduction and motor decoding. The AgentBrain acts as the
bridge between the neural processing of the Brain and the external world,
converting sensory inputs to neural signals and neural activity to actions.
"""

from .agent import AgentBrain

__all__ = ["AgentBrain"]
