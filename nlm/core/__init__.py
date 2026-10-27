"""
NLM Core Module

This module contains the core classes and functionality of NLM:
- Brain: The main neural simulation engine
- Config: Configuration management system
- Other core components
"""

# Import core classes from their respective modules
from .brain import Brain
from .config import Config
from .types import Types

__all__ = ['Brain', 'Config', 'Types']