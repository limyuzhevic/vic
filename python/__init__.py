"""
NLM (Neural Learning Machine) - Python Bindings
================================================

A comprehensive Python interface to the NLM C++ neural simulation framework.

This package provides access to:
- Brain simulation with real spiking neural computation
- Memory systems (working, episodic, associative)
- Neuromodulation (reward, curiosity, novelty)
- Cognitive functions (attention, planning, concept formation)
- Sensorimotor integration
- Development and plasticity
"""

from pynlm import (
    Config,
    createBrain,
    createDefaultConfig,
    createSimpleWorld,
    createAgentBrain,
)

__version__ = "0.1.0"
__all__ = [
    'Config',
    'createBrain',
    'createDefaultConfig',
    'createSimpleWorld',
    'createAgentBrain',
]