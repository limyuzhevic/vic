"""
Agent brain interface module for NLM Python bindings.

Provides the AgentBrain class that connects the neural brain to the simulation
world, handling sensory processing, motor decoding, and neuromodulation.
"""

import pybind11
from typing import Optional

# Import the C++ extensions
import pynlm
from pynlm import AgentBrain, Brain, createAgentBrain, SimpleWorld

__all__ = ['AgentBrain', 'createAgentBrain']
