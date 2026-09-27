from dataclasses import dataclass, field
"""
NLM (Neural Learning Machine) Python Bindings
===========================================

A comprehensive Python interface to the NLM C++ neural simulation framework,
providing access to biologically plausible neural computation, learning,
and cognitive systems.

This module implements a full-featured neural simulation environment with:

**Core Components:**
- **Brain**: Central neural simulation coordinator managing all subsystems
- **Neuron**: Leaky Integrate-and-Fire (LIF) neuron model with adaptation
- **MemorySystem**: Working, episodic, and associative memory integration
- **PlasticityRule**: STDP, Hebbian, and structural plasticity mechanisms
- **Neuromodulator**: Dopamine, curiosity, novelty, and prediction error
- **CognitiveSystem**: Neural planning, concept formation, and attention
- **PredictionSystem**: Forward models and prediction error computation
- **DevelopmentSystem**: Developmental stages and critical periods
- **Config**: Configuration management with schema validation
- **Experiment**: Experiment management and parameter sweeps
- **Visualizer**: Comprehensive visualization of neural activity

**Key Features:**
- Manager-based architecture with dependency injection
- Biologically plausible learning algorithms
- Multi-scale neural dynamics
- Integration with experimental protocols
- Comprehensive visualization capabilities
- State persistence and checkpointing
- Extensible plugin architecture

**Installation:**
    pip install pynlm
    pip install -e .[all]  # For all dependencies including visualization

**Quick Start:**
    >>> import pynlm
    >>> 
    >>> # Create configuration
    >>> config = pynlm.Config.create_default()
    >>> 
    >>> # Create and initialize brain
    >>> brain = pynlm.create_brain(config)
    >>> brain.initialize()
    >>> 
    >>> # Run simulation
    >>> for step in range(1000):
    ...     brain.step(step)
    ...     if step % 100 == 0:
    ...         print(brain.log_status())
    >>> 
    >>> # Save results
    >>> brain.save("brain_checkpoint.json")

**Documentation:**
    - API Reference: docs/API_REFERENCE.md
    - User Guide: docs/USER_GUIDE.md
    - Examples: examples/

Attributes:
    __version__ (str): Package version number
    __author__ (str): Package author
    __email__ (str): Contact email
    __license__ (str): Software license
    API_VERSION (str): NLM API version
    NUMPY_AVAILABLE (bool): Whether NumPy is available
    MATPLOTLIB_AVAILABLE (bool): Whether Matplotlib is available

Functions:
    create_brain(config): Factory function to create a Brain instance
    create_default_config(): Factory function to create a default Config

See Also:
    The individual module documentation for detailed class and method references
"""

__version__ = "0.1.0"
__author__ = "NLM Project"
__email__ = "nlm-project@example.com"
__license__ = "MIT"

# Core exports
from .brain import Brain
from .neurons import Neuron, NeuronType, FiringState, NeuronState
from .memory import MemorySystem
from .plasticity import PlasticityRule
from .neuromodulation import Neuromodulator
from .cognition import CognitiveSystem
from .prediction import PredictionSystem
from .development import DevelopmentSystem
from .config import Config
from .experiment import Experiment
from .visualizer import Visualizer

# Factory functions
from .brain import create_brain, create_default_config

# Version info
from . import __version__

# API version
API_VERSION = "1.0"

# Check for dependencies
try:
    import numpy as np
    NUMPY_AVAILABLE = True
except ImportError:
    NUMPY_AVAILABLE = False

try:
    import matplotlib
    MATPLOTLIB_AVAILABLE = True
except ImportError:
    MATPLOTLIB_AVAILABLE = False

__all__ = [
    # Core classes
    "Brain",
    "Neuron", "NeuronType", "FiringState", "NeuronState",
    "MemorySystem", "PlasticityRule", "Neuromodulator",
    "CognitiveSystem", "PredictionSystem", "DevelopmentSystem",
    "Config", "Experiment", "Visualizer",
    # Factory functions
    "create_brain", "create_default_config",
    # Constants and utilities
    "API_VERSION",
    "NUMPY_AVAILABLE", "MATPLOTLIB_AVAILABLE",
    "__version__"
]

__doc__ = __doc__
