# NLM Python API - Main entry point

"""
NLM (Neural Learning Machine) Python API
==========================================

A high-level Python interface to the NLM neural simulation framework.
Provides simplified classes and convenience functions for common use cases.

This module contains high-level classes for working with NLM brains,
experiments, and visualization, along with convenience functions for
common operations.
"""

"""
NLM Python API - Main entry point
================================

A high-level Python interface to the NLM neural simulation framework.
Provides simplified classes and convenience functions for common use cases.

This module contains high-level classes for working with NLM brains,
experiments, and visualization, along with convenience functions for
common operations.
"""

import warnings
from typing import Any, Dict, List, Optional, Union, Callable
from contextlib import contextmanager

# Try to import low-level bindings
try:
    import pynlm
    HAS_PYNLM = True
except ImportError:
    HAS_PYNLM = False
    warnings.warn(
        "pynlm (NLM C++ bindings) not found. "
        "Please build the C++ extensions first."
    )

# Import high-level classes
from .processors import NLMBrain, NLMExperiment, NLMVisualizer, ExperimentResults

# Import convenience functions
from .nlm_api import (
    create_simple_brain,
    create_standard_experiment,
    quick_test,
    train_agent,
    evaluate_performance,
    optimize_hyperparameters,
    configure as _configure
)

# Version information
__version__ = "1.0.0"
__author__ = "NLM Development Team"

# Global configuration
_config = {
    'default_neuron_count': 1000,
    'default_complexity': 0.5,
    'default_world_size': (10, 10),
    'default_steps': 1000,
    'enable_warnings': True,
}


def configure(**kwargs):
    """
    Configure global NLM Python API settings.
    
    Args:
        **kwargs: Configuration options
            - default_neuron_count: Default neuron count for brains
            - default_complexity: Default brain complexity (0.0-1.0)
            - default_world_size: Default world dimensions
            - default_steps: Default simulation steps
            - enable_warnings: Whether to show warnings
    """
    global _config
    _config.update(kwargs)


def get_config(key: str) -> Any:
    """
    Get a configuration value.
    
    Args:
        key: Configuration key
        
    Returns:
        Configuration value
    """
    return _config.get(key)


@contextmanager
def timing_context(description: str):
    """
    Context manager for timing operations.
    
    Args:
        description: Description of what is being timed
        
    Example:
        ```python
        with timing_context("Training simulation"):
            brain = create_simple_brain(100, 0.7)
        ```
    """
    import time
    start = time.time()
    try:
        yield
    finally:
        duration = time.time() - start
        if _config.get('enable_warnings', True):
            print(f"{description} took {duration:.2f} seconds")


class NLMError(Exception):
    """
    Base exception class for NLM Python API errors.
    """
    pass


class ValidationError(NLMError):
    """
    Exception raised for input validation errors.
    """
    pass


class ConfigurationError(NLMError):
    """
    Exception raised for configuration errors.
    """
    pass


# Set up exception wrapping for pynlm if available
if HAS_PYNLM:
    try:
        # Wrap C++ exceptions in Python ones
        pynlm.RuntimeError = NLMError
    except (AttributeError, TypeError):
        pass  # C++ exceptions may already be wrapped


# Export for easy imports
from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from .processors import NLMBrain, NLMExperiment, NLMVisualizer
    from .nlm_api import (
        create_simple_brain,
        create_standard_experiment,
        quick_test,
        train_agent,
        evaluate_performance,
        optimize_hyperparameters
    )

__doc__ += """

Example Usage:
-------------

Create a simple brain:
    brain = create_simple_brain(neuron_count=1000, complexity=0.7)

Run a quick test:
    quick_test(mode='connectivity', steps=500)

Create an experiment:
    experiment = NLMExperiment.create_standard(world_size=(20, 20))

Train an agent:
    train_agent(world, brain, episodes=100)

See the individual class and function documentation for more details.
"""

# Re-export everything for backward compatibility
__all__ = [
    # High-level classes
    'NLMBrain',
    'NLMExperiment',
    'NLMVisualizer',
    'ExperimentResults',
    # Convenience functions
    'create_simple_brain',
    'create_standard_experiment',
    'quick_test',
    'train_agent',
    'evaluate_performance',
    'optimize_hyperparameters',
    # Configuration
    'configure',
    'get_config',
    'timing_context',
    # Exceptions
    'NLMError',
    'ValidationError',
    'ConfigurationError',
    # Low-level access (if available)
    'pynlm' if HAS_PYNLM else None,  # Provide access even if import failed
]