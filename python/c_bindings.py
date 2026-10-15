"""
C++ bindings for NLM Python package.

This module provides the underlying C++ bindings that power the enhanced Python API.
It maintains backward compatibility with the original bindings while enabling
the enhanced features in the Python wrapper.
"""

import sys
import os

# Try to import the compiled extension
try:
    # Add the Python directory to the path
    python_dir = os.path.dirname(os.path.abspath(__file__))
    sys.path.insert(0, python_dir)
    
    # Try to import the compiled module
    import pynlm as c_bindings
    
    # Re-export everything
    from pynlm import (
        # Core types
        NeuronId, SynapseId, RegionId, PopulationId,
        NeuronType, SynapseType, DevelopmentalStage, FiringState,
        ActionType, MotorCommand, WorldObjectType,
        
        # Classes
        Config, Vision, Audio, InternalSignals, Action, WorldObject,
        AgentBody, ActionResult, SensoryPercept,
        SimpleWorld, Brain, AgentBrain,
        
        # Helper functions
        createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
        
        # C++ specific attributes
        INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID
    )
    
    _CPP_BINDINGS_AVAILABLE = True
    
except ImportError:
    # If C++ bindings are not available, use mock implementations
    _CPP_BINDINGS_AVAILABLE = False
    
    warnings.warn("C++ bindings not available. Using mock implementations. "
                 "This is expected for development/testing.", ImportWarning)
    
    # Import mock implementations from the Python wrapper
    from nlm import (
        # Core types
        NeuronId, SynapseId, RegionId, PopulationId,
        NeuronType, SynapseType, DevelopmentalStage, FiringState,
        ActionType, MotorCommand, WorldObjectType,
        
        # Classes
        Config, Vision, Audio, InternalSignals, Action, WorldObject,
        AgentBody, ActionResult, SensoryPercept,
        SimpleWorld, Brain, AgentBrain,
        
        # Helper functions
        createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain
    )
    
    # Define mock constants
    class MockIDs:
        INVALID_NEURON_ID = NeuronId()
        INVALID_SYNAPSE_ID = SynapseId()
        INVALID_REGION_ID = RegionId()
        INVALID_POPULATION_ID = PopulationId()
    
    # Add mock constants to the module
    for key, value in MockIDs.__dict__.items():
        if not key.startswith('_'):
            globals()[key] = value

# Re-export everything
__all__ = [
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Classes
    'Config', 'Vision', 'Audio', 'InternalSignals', 'Action', 'WorldObject',
    'AgentBody', 'ActionResult', 'SensoryPercept',
    'SimpleWorld', 'Brain', 'AgentBrain',
    
    # Helper functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    
    # C++ specific attributes (only if available)
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID'
]

# Module metadata
__version__ = "1.0.0"
__author__ = "NLM Development Team"
__description__ = "Enhanced Python bindings for NLM neural simulation framework"
__license__ = "MIT"

# Check if C++ bindings are available
if _CPP_BINDINGS_AVAILABLE:
    print(f"C++ bindings loaded successfully for {__version__}")
else:
    print(f"Using mock Python implementations for {__version__}")
    print("C++ bindings are not available. Compile with: pip install -e .")

# Platform detection
import platform
if platform.system() == "Linux":
    __platform__ = "Linux"
elif platform.system() == "Windows":
    __platform__ = "Windows"
elif platform.system() == "Darwin":
    __platform__ = "macOS"
else:
    __platform__ = "Unknown"

# Export platform info
__platform__ = __platform__

# Set up environment variables
os.environ['NLM_PLATFORM'] = __platform__

# Check Python version
import sys
if sys.version_info < (3, 8):
    import warnings
    warnings.warn("NLM requires Python 3.8 or higher", DeprecationWarning)

# Check NumPy version
import numpy as np
if np.__version__ < "1.20.0":
    import warnings
    warnings.warn("NLM recommends NumPy 1.20.0 or higher", DeprecationWarning)

# Check Matplotlib version
import matplotlib
if matplotlib.__version__ < "3.0.0":
    import warnings
    warnings.warn("NLM recommends Matplotlib 3.0.0 or higher", DeprecationWarning)

print(f"NLM Python bindings initialized")
print(f"Platform: {__platform__}")
print(f"Python: {sys.version}")
print(f"NumPy: {np.__version__}")
print(f"Matplotlib: {matplotlib.__version__}")

if _CPP_BINDINGS_AVAILABLE:
    print(f"C++ bindings: Available")
else:
    print(f"C++ bindings: Not available (using mock implementations)")
