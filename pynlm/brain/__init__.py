"""
NLM Brain module for neural simulation

This module provides the Brain class which represents the core neural
simulation system in NLM. The Brain implements Leaky Integrate-and-Fire
(LIF) neuron dynamics with multiple plasticity mechanisms including
STDP, Hebbian learning, and reward-modulated learning.

The brain is the central component of the NLM system, responsible for:
- Neural computation and processing
- Memory storage and retrieval
- Plasticity and learning
- Action selection
- Integration with sensory and motor systems
"""

import sys
from typing import List, Dict, Any, Optional, Union, Callable
from .brain import Brain

__all__ = ['Brain']

# Factory functions for backward compatibility
def create_brain(config):
    """Create a new brain with the given configuration.
    
    Args:
        config: Configuration object for the brain
        
    Returns:
        Brain: New brain instance
    """
    return Brain(config)

__all__.append('create_brain')
