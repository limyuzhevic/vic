"""
Python interface for NLM (Neural Learning Machine)

This module provides high-level Python APIs for common neural simulation tasks.
It simplifies the low-level C++ interface for easier use by researchers and developers.

Key Features:
- Simplified brain creation and configuration
- Pre-built simulation scenarios
- High-level memory management
- Built-in learning algorithms
- Visualization integration
"""

from . import brain
from . import world
from . import agent
from . import experiments
from . import utils
from . import config

__version__ = "0.1.0"
__all__ = ['brain', 'world', 'agent', 'experiments', 'utils', 'config']