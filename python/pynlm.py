"""
Bridge module for NLM Python bindings.

This module acts as an interface between the C++ NLM bindings and Python code.
The actual bindings are implemented in C++ and compiled to a shared library
(pynlm.cpython-*.so). This module provides the Python interface and includes
the enhanced functionality we've developed.
"""

import sys
from pathlib import Path

# Try to import the actual C++ bindings
try:
    # First, try to import from the compiled module
    import pynlm as _pynlm

    # Copy all public symbols from the C++ bindings
    from pynlm import (
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        FiringState,
        ActionType,
        MotorCommand,
        WorldObjectType,
        Config,
        SensoryInput,
        Vision,
        Audio,
        InternalSignals,
        Action,
        WorldObject,
        AgentBody,
        ActionResult,
        SensoryPercept,
        SimpleWorld,
        Brain,
        AgentBrain
    )

    # Create wrapper functions for better Python API
    def createDefaultConfig():
        """Create a default configuration."""
        return Config()

    def createBrain(config):
        """Create a new brain with configuration."""
        return Brain(config)

    def createSimpleWorld():
        """Create a new simple world."""
        return SimpleWorld()

    def createAgentBrain(brain):
        """Create a new agent brain interface."""
        return AgentBrain(brain)

    # Export all symbols
    __all__ = [
        # Core classes
        'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
        'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
        'ActionType', 'MotorCommand', 'WorldObjectType',
        'Config', 'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
        'Action', 'WorldObject', 'AgentBody', 'ActionResult',
        'SensoryPercept', 'SimpleWorld', 'Brain', 'AgentBrain',

        # Factory functions
        'createDefaultConfig',
        'createBrain',
        'createSimpleWorld',
        'createAgentBrain',

        # Advanced utilities (if available)
        'NLMExperimentLogger',
        'NLMBenchmark',
        'NLMVisualizer',
        'get_version',
        'is_available'
    ]

    # Import advanced utilities
    try:
        from .nlm_utils import (
            NLMExperimentLogger,
            NLMBenchmark,
            NLMVisualizer,
            get_version,
            is_available
        )
        __all__.extend(['NLMExperimentLogger', 'NLMBenchmark', 'NLMVisualizer', 'get_version', 'is_available'])
    except ImportError:
        pass

    # Set module attributes
    globals().update({
        'NeuronId': NeuronId,
        'SynapseId': SynapseId,
        'RegionId': RegionId,
        'PopulationId': PopulationId,
        'NeuronType': NeuronType,
        'SynapseType': SynapseType,
        'DevelopmentalStage': DevelopmentalStage,
        'FiringState': FiringState,
        'ActionType': ActionType,
        'MotorCommand': MotorCommand,
        'WorldObjectType': WorldObjectType,
        'Config': Config,
        'SensoryInput': SensoryInput,
        'Vision': Vision,
        'Audio': Audio,
        'InternalSignals': InternalSignals,
        'Action': Action,
        'WorldObject': WorldObject,
        'AgentBody': AgentBody,
        'ActionResult': ActionResult,
        'SensoryPercept': SensoryPercept,
        'SimpleWorld': SimpleWorld,
        'Brain': Brain,
        'AgentBrain': AgentBrain,
        'createDefaultConfig': createDefaultConfig,
        'createBrain': createBrain,
        'createSimpleWorld': createSimpleWorld,
        'createAgentBrain': createAgentBrain
    })

except ImportError:
    # Fallback if the C++ bindings are not available
    print("Warning: NLM C++ bindings not found. Some functionality will be limited.")

    # Create stub classes for backward compatibility
    class _StubClass:
        def __init__(self, *args, **kwargs):
            pass

        def __getattr__(self, name):
            return lambda *args, **kwargs: self

        def __call__(self, *args, **kwargs):
            return self

    # Create stub classes
    NeuronId = SynapseId = RegionId = PopulationId = _StubClass
    NeuronType = SynapseType = DevelopmentalStage = FiringState = _StubClass
    ActionType = MotorCommand = WorldObjectType = _StubClass
    Config = SensoryInput = Vision = Audio = InternalSignals = _StubClass
    Action = WorldObject = AgentBody = ActionResult = _StubClass
    SensoryPercept = SimpleWorld = Brain = AgentBrain = _StubClass

    # Create stub factory functions
    def createDefaultConfig():
        return Config()

    def createBrain(config):
        return Brain(config)

    def createSimpleWorld():
        return SimpleWorld()

    def createAgentBrain(brain):
        return AgentBrain(brain)

    # Add utility functions (these might not work without the actual bindings)
    def get_version():
        return "NLM Python Bindings v0.1.0 (Stub)"

    def is_available():
        return False

    # Define __all__ for stub module
    __all__ = [
        # Stub classes
        'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
        'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
        'ActionType', 'MotorCommand', 'WorldObjectType',
        'Config', 'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
        'Action', 'WorldObject', 'AgentBody', 'ActionResult',
        'SensoryPercept', 'SimpleWorld', 'Brain', 'AgentBrain',

        # Factory functions
        'createDefaultConfig',
        'createBrain',
        'createSimpleWorld',
        'createAgentBrain',

        # Utility functions
        'get_version',
        'is_available'
    ]

# Set module docstring and metadata
__doc__ = """
NLM (Neural Learning Machine) Python Bindings

A computational brain simulation framework with neural development,
learning, and memory systems.

This package provides Python bindings to the NLM C++ neural simulation
framework. The actual implementation is in C++ for performance.

Key Features:
------------
- Spiking Neural Networks (LIF neurons)
- Memory Systems (working, episodic, associative memory)
- Neuromodulation (dopamine, curiosity, novelty detection)
- Neural development and plasticity
- Prediction systems
- World interaction and control
- Multiple learning mechanisms (STDP, Hebbian, structural)
- Checkpoint save/load functionality

Usage:
------
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
```

For advanced features, utilities, and examples, visit the documentation.
"""

__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"

# Initialize logging
import sys
if not hasattr(sys, '_nlm_initialized'):
    print(f"NLM Python Bindings v{__version__} loaded")
    print("For documentation and examples, visit: https://nlm.readthedocs.io")
    sys._nlm_initialized = True
