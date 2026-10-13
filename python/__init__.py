# Enhanced NLM Python bindings module - Setup and utilities

"""
Enhanced NLM Python Module
==========================

This module provides an enhanced interface to the NLM (Neural Learning Machine) 
neural simulation framework with advanced functionality for research and development.

Key Features:
- Complete brain introspection and state inspection
- Enhanced memory system access
- Advanced neuromodulation control
- Batch operations for performance
- Configuration templates and presets
- Development and plasticity control
- Convenience methods for common workflows
- Comprehensive error handling
- Performance monitoring and benchmarking

Version: Enhanced v1.0 (Phase 6)
Python API: Compatible with Python 3.8+
C++ Backend: C++20 with pybind11
"""

# Module exports
from .bindings import *
from .utils import *
from .presets import *
from .benchmark import *

__all__ = [
    # Core classes
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'Config', 'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'WorldObject', 'AgentBody', 'SimpleWorld',
    'Brain', 'AgentBrain',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld',
    'createAgentBrain', 'createVision', 'createAudio',
    'createInternalSignals', 'createAction', 'createAgentBody',
    'createSensoryPercept',
    
    # Enhanced utility functions
    'benchmark', 'getDefaultConfig', 'getResearchConfig',
    'getMinimalConfig',
    
    # Constants
    'PRESETS', 'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    'version',
    
    # Version
    '__version__'
]

__version__ = "enhanced-1.0-phase6"

# Import version from bindings if available
try:
    from .bindings import version as _version
    __version__ = _version()
except ImportError:
    pass

# Module initialization
import sys
if sys.version_info >= (3, 8):
    import warnings
    warnings.warn(
        "Enhanced NLM bindings module loaded. "
        "This version includes advanced functionality beyond the basic bindings.",
        UserWarning,
        stacklevel=2
    )

# Performance and compatibility checks
import os
if os.environ.get('NLM_ENHANCED_BINDINGS', '').lower() == 'debug':
    import time
    def timed_call(func, *args, **kwargs):
        """Helper function to time calls for performance monitoring"""
        start = time.time()
        result = func(*args, **kwargs)
        end = time.time()
        return result, end - start
    
    globals()['timed_call'] = timed_call
else:
    def timed_call(func, *args, **kwargs):
        return func(*args, **kwargs), 0.0
    globals()['timed_call'] = timed_call

print(f"Enhanced NLM Python bindings loaded (version {__version__})")
print("For documentation, see: https://github.com/nlm-project/nlm/tree/Phase6/docs")
