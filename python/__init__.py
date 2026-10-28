# NLM Python Package
#
# High-level Python interface for the NLM (Neural Learning Machine) framework.
# This module provides a user-friendly API for creating, training, and interacting with
# artificial neural brains using the NLM C++ simulation framework.

from .nlm import (
    NLMConfiguration,
    NLMBrain,
    NLMWorld,
    NLMEnvironment,
    create_simple_brain_demo,
    create_reproduction_demo,
    main
)

__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Python bindings for NLM (Neural Learning Machine) neural simulation framework"

__all__ = [
    'NLMConfiguration',
    'NLMBrain', 
    'NLMWorld',
    'NLMEnvironment',
    'create_simple_brain_demo',
    'create_reproduction_demo',
    'main'
]
