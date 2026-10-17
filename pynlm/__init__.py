# NLM Python Module
# ============
# Python bindings for the NLM (Neural Learning Machine) neural simulation framework
# This module provides a Pythonic interface to the C++ NLM implementation

import sys
import platform

# Check for pynlm module
try:
    from .pynlm import (
        createBrain,
        createDefaultConfig,
        createSimpleWorld,
        createAgentBrain,
        Config,
        Brain,
        SimpleWorld,
        AgentBrain,
        Vision,
        Audio,
        InternalSignals,
        SensoryInput,
        Action,
        WorldObject,
        AgentBody,
        ActionResult,
        SensoryPercept,
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        FiringState,
        ActionType,
        MotorCommand,
        WorldObjectType,
        INVALID_NEURON_ID,
        INVALID_SYNAPSE_ID,
        INVALID_REGION_ID,
        INVALID_POPULATION_ID
    )
except ImportError as e:
    # If the module isn't built yet, provide helpful error message
    def _import_error(*args, **kwargs):
        raise ImportError(
            "pynlm Python bindings are not installed or built. "
            "Please build the module with: pip install -e . "
            "or build with: python -m pip install scikit-build-core"
        ) from e
    
    # Create placeholder classes that raise ImportError when used
    class _ImportErrorModule:
        def __getattr__(self, name):
            raise ImportError(
                f"The pynlm Python bindings module is not available. "
                f"This module should have been built and installed with C++ bindings. "
                f"Error: {e}"
            )
    
    # Export all names as errors
    globals().update({name: _ImportErrorModule() for name in [
        'createBrain', 'createDefaultConfig', 'createSimpleWorld', 'createAgentBrain',
        'Config', 'Brain', 'SimpleWorld', 'AgentBrain',
        'Vision', 'Audio', 'InternalSignals', 'SensoryInput', 'Action',
        'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept',
        'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
        'ActionType', 'MotorCommand', 'WorldObjectType',
        'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
        'INVALID_POPULATION_ID'
    ]})

# Add version information
__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Python bindings for NLM (Neural Learning Machine) neural simulation framework"

# Provide Pythonic aliases for common usage
__all__ = [
    # Factory functions
    'createBrain', 'createDefaultConfig', 'createSimpleWorld', 'createAgentBrain',
    # Main classes
    'Config', 'Brain', 'SimpleWorld', 'AgentBrain',
    # Sensory and Motor classes
    'Vision', 'Audio', 'InternalSignals', 'SensoryInput', 'Action',
    'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept',
    # Enumerations
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    # Version info
    '__version__', '__author__', '__description__'
]

# Pythonic utility functions
def get_system_info():
    """Get information about the NLM system."""
    info = {
        'version': __version__,
        'platform': platform.system(),
        'architecture': platform.machine(),
        'python_version': platform.python_version(),
        'description': __description__
    }
    return info
def print_welcome():
    """Print a welcome message with basic usage information."""
    print("=" * 60)
    print(f"NLM Python Module v{__version__}")
    print(f"Python bindings for neural simulation framework")
    print("=" * 60)
    print()
    print("Quick Start Examples:")
    print("-" * 30)
    print("1. Create a simple brain:")
    print("   brain = createBrain(createDefaultConfig())")
    print()
    print("2. Initialize and run:")
    print("   brain.initialize()")
    print("   brain.step(0)")
    print()
    print("3. Get brain statistics:")
    print("   print(brain.getTotalNeuronCount())")
    print("   print(brain.getTotalSpikeCount())")
    print()
    print("For more examples, see the examples/ directory")
    print("=" * 60)
    return True
# Add module-level help and documentation
__doc__ = f"""
NLM Python Module v{__version__}
{'=' * 60}

{__description__}

This module provides Python bindings for the NLM (Neural Learning Machine)
framework, which is an experimental computational brain project.

Quick Reference:
---------------
Factory Functions:
    createBrain(config) -> Brain
    createDefaultConfig() -> Config
    createSimpleWorld() -> SimpleWorld
    createAgentBrain(brain) -> AgentBrain

Main Classes:
    Config      - Configuration for NLM system
    Brain       - Central neural simulation brain class
    SimpleWorld - Simple 2D world for NLM simulation
    AgentBrain  - Agent brain interface connecting NLM brain to world

For detailed documentation, see easy_usage.md and HOW_TO_USE.md

Build Instructions:
------------------
To build and install the Python bindings:
1. Run: pip install -e .
2. Or build with: python -m pip install scikit-build-core

For system information:
    import pynlm
    print(pynlm.get_system_info())
"""

# Auto-print welcome on import if in interactive mode
if __name__ != "__pycache__" and "pytest" not in sys.modules:
    try:
        if hasattr(sys, 'ps1') or sys.stdin is None or not sys.stdin.isatty():
            print_welcome()
    except (AttributeError, OSError):
        pass