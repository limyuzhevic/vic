"""
NLM (熙然) - Neural Learning Machine
An experimental artificial developmental brain simulator

This module provides Python bindings to the NLM C++ neural network simulator.
"""

import numpy as np
from typing import List, Optional, Dict, Any, Union
import json
import warnings

"""
NLM (熙然) - Neural Learning Machine Python Bindings

Python interface to the NLM (Neural Learning Machine) neural network simulator.

This module provides a Python API to the NLM C++ neural network simulator,
enabling Python developers to create and run neural network simulations
without needing to write C++ code.
"""

__version__ = "0.1.0"

# Try to import the core Python implementation
try:
    from .core import (
        Config,
        Brain,
        AgentBrain,
        SimpleWorld,
        WorldObject,
        NeuronType,
        DevelopmentalStage,
        ActionType,
        WorldObjectType,
        ConfigError,
        NeuralError,
        WorldError,
        SimulationError,
        create_default_config,
        create_brain,
        create_agent_brain,
        create_simple_world,
        run_minimal_simulation,
        run_agent_simulation,
        analyze_brain_stats,
        benchmark_performance,
        save_experiment_results,
        load_experiment_results,
    )
    _PYTHON_IMPLEMENTATION_AVAILABLE = True
except ImportError as e:
    _PYTHON_IMPLEMENTATION_AVAILABLE = False
    import warnings
    warnings.warn(
        f"NLM Python implementation not fully available: {e}",
        ImportWarning
    )

# Try to import C++ bindings if available
try:
    # This will attempt to import the C++ bindings module
    import pynlm as _pynlm
    _C_BINDINGS_AVAILABLE = True
    _c_version = getattr(_pynlm, '__version__', 'unknown')
    
    # If Python implementation is available, we can use it
    # The C++ bindings will be used when explicitly requested
    if _PYTHON_IMPLEMENTATION_AVAILABLE:
        # We'll use the Python implementation by default,
        # but allow access to C++ bindings for performance-critical parts
        _C_BINDINGS = _pynlm
    else:
        # Fall back to C++ bindings
        _C_BINDINGS = _pynlm
        
except ImportError:
    _C_BINDINGS_AVAILABLE = False
    _C_BINDINGS = None
    import warnings
    warnings.warn(
        "NLM C++ bindings not available. "
        "Please build the project with: pip install -e .",
        ImportWarning
    )

# Re-export key classes and functions for ease of use
if _PYTHON_IMPLEMENTATION_AVAILABLE:
    __all__ = [
        # Core classes
        'Config',
        'Brain',
        'AgentBrain', 
        'SimpleWorld',
        'WorldObject',
        
        # Enums
        'NeuronType',
        'DevelopmentalStage',
        'ActionType',
        'WorldObjectType',
        
        # Exceptions
        'ConfigError',
        'NeuralError',
        'WorldError',
        'SimulationError',
        
        # Factory functions
        'create_default_config',
        'create_brain',
        'create_agent_brain',
        'create_simple_world',
        
        # Convenience functions
        'run_minimal_simulation',
        'run_agent_simulation',
        'analyze_brain_stats',
        'benchmark_performance',
        'save_experiment_results',
        'load_experiment_results',
        
        # Advanced modules
        'extensions',
        
        # Version
        '__version__'
    ]
else:
    # Minimal export if only C++ bindings are available
    __all__ = [
        '__version__',
        '_C_BINDINGS_AVAILABLE',
        '_C_BINDINGS'
    ]

# Additional convenience functions that work with both implementations
def is_available() -> bool:
    """Check if NLM is available."""
    return _PYTHON_IMPLEMENTATION_AVAILABLE or _C_BINDINGS_AVAILABLE


def get_implementation_type() -> str:
    """Get the current implementation type being used."""
    if _PYTHON_IMPLEMENTATION_AVAILABLE:
        return "python"
    elif _C_BINDINGS_AVAILABLE:
        return "cpp"
    else:
        return "none"


def print_info() -> None:
    """Print NLM version and implementation information."""
    print(f"NLM (熙然) - Neural Learning Machine")
    print(f"Version: {__version__}")
    print(f"Implementation: {get_implementation_type()}")
    print(f"Available: {is_available()}")
    
    if _C_BINDINGS_AVAILABLE:
        print(f"C++ Bindings Version: {_c_version}")
    
    print("\nQuick Start Examples:")
    print("1. Create a brain:")
    print("   from pynlm import create_brain")
    print("   brain = create_brain()")
    print("   brain.initialize()")
    print("")
    print("2. Run simulation:")
    print("   for step in range(1000):")
    print("       brain.step(step)")
    print("")
    print("3. Create agent with world:")
    print("   from pynlm import create_simple_world")
    print("   world = create_simple_world()")
    print("   world.configure(width=20, height=20)")
    print("   agent = create_agent_brain(brain)")
    print("   agent.initialize(world)")


# Initialize and configure
if _PYTHON_IMPLEMENTATION_AVAILABLE:
    # Set up the default configuration
    pass

# Provide access to advanced extensions
if _PYTHON_IMPLEMENTATION_AVAILABLE:
    try:
        from . import extensions
        __all__.append('extensions')
    except ImportError:
        pass

# Make the modules accessible
try:
    from .core import *
    from .extensions import *
except ImportError:
    pass
