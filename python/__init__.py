"""
NLM (Neural Learning Machine) Python Bindings
===========================================

A Python binding for the NLM C++ neural simulation framework.
Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.

This module provides both direct C++ bindings and enhanced convenience features
for both beginners and advanced users.
"""

import sys
from typing import Optional, Dict, Any

# Direct C++ bindings from pybind11 module
try:
    from .pynlm import (
        # Core classes
        Config, Brain, SimpleWorld, AgentBrain,
        Vision, Audio, InternalSignals, SensoryInput, Action,
        WorldObject, AgentBody, ActionResult, SensoryPercept,
        
        # Types
        NeuronId, SynapseId, RegionId, PopulationId,
        NeuronType, SynapseType, DevelopmentalStage, FiringState,
        ActionType, MotorCommand, WorldObjectType,
        
        # Factory functions
        createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
        
        # Constants
        INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID,
        INVALID_POPULATION_ID,
        
        # Exception
        RuntimeError,
    )
    
    # Import all from the underlying module for convenience
    from .pynlm import *  # noqa: F403, F405
    
    # Set __version__ if available
    __version__ = getattr(sys.modules['pynlm.pynlm'], '__version__', '0.1.0')
    
    # Try to import helpers module
    try:
        from . import helpers
        
        # Import key helper classes/functions
        from .helpers import (
            QuickStart, SimulationMonitor, ExperimentTemplate,
            BrainContext, AgentContext,
            run_simulation, explore_brain, benchmark,
            visualize_activity, plot_metrics, monitor_simulation,
            tutorial_examples, save_tutorial_examples,
        )
        
        # Add helper functions to module namespace
        __all__ = [
            # Core classes (from C++ bindings)
            'Config', 'Brain', 'SimpleWorld', 'AgentBrain',
            'Vision', 'Audio', 'InternalSignals', 'SensoryInput', 'Action',
            'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept',
            'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
            'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
            'ActionType', 'MotorCommand', 'WorldObjectType',
            'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
            'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
            'INVALID_POPULATION_ID',
            'RuntimeError',
            
            # Helper classes
            'QuickStart', 'SimulationMonitor', 'ExperimentTemplate',
            
            # Context managers
            'BrainContext', 'AgentContext',
            
            # Convenience functions
            'run_simulation', 'explore_brain', 'benchmark',
            'visualize_activity', 'plot_metrics', 'monitor_simulation',
            
            # Tutorial utilities
            'tutorial_examples', 'save_tutorial_examples',
            
            # Import alias for convenience
            'nlm',
        ]
        
    except ImportError:
        # Fallback if helpers module is not available
        __all__ = [
            'Config', 'Brain', 'SimpleWorld', 'AgentBrain',
            'Vision', 'Audio', 'InternalSignals', 'SensoryInput', 'Action',
            'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept',
            'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
            'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
            'ActionType', 'MotorCommand', 'WorldObjectType',
            'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
            'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
            'INVALID_POPULATION_ID',
            'RuntimeError',
        ]
        
except ImportError as e:
    # If pynlm module is not available (e.g., not built yet)
    print(f"Warning: Could not import pynlm module: {e}", file=sys.stderr)
    print("Please build the Python bindings first using: pip install -e .", file=sys.stderr)
    
    # Create placeholder classes for import error handling
    class _ImportErrorPlaceholder:
        def __init__(self, name: str):
            self.name = name
            
        def __call__(self, *args, **kwargs):
            raise ImportError(
                f"pynlm module not available. Please build the Python bindings "
                f"first using: pip install -e .\n"
                f"Original error: {self.name}"
            )
        
        def __getattr__(self, name):
            raise ImportError(
                f"pynlm module not available. Please build the Python bindings "
                f"first using: pip install -e ."
            )
    
    # Create placeholder objects for all expected symbols
    Config = _ImportErrorPlaceholder("Config")
    Brain = _ImportErrorPlaceholder("Brain")
    SimpleWorld = _ImportErrorPlaceholder("SimpleWorld")
    AgentBrain = _ImportErrorPlaceholder("AgentBrain")
    Vision = _ImportErrorPlaceholder("Vision")
    Audio = _ImportErrorPlaceholder("Audio")
    InternalSignals = _ImportErrorPlaceholder("InternalSignals")
    SensoryInput = _ImportErrorPlaceholder("SensoryInput")
    Action = _ImportErrorPlaceholder("Action")
    WorldObject = _ImportErrorPlaceholder("WorldObject")
    AgentBody = _ImportErrorPlaceholder("AgentBody")
    ActionResult = _ImportErrorPlaceholder("ActionResult")
    SensoryPercept = _ImportErrorPlaceholder("SensoryPercept")
    NeuronId = _ImportErrorPlaceholder("NeuronId")
    SynapseId = _ImportErrorPlaceholder("SynapseId")
    RegionId = _ImportErrorPlaceholder("RegionId")
    PopulationId = _ImportErrorPlaceholder("PopulationId")
    NeuronType = _ImportErrorPlaceholder("NeuronType")
    SynapseType = _ImportErrorPlaceholder("SynapseType")
    DevelopmentalStage = _ImportErrorPlaceholder("DevelopmentalStage")
    FiringState = _ImportErrorPlaceholder("FiringState")
    ActionType = _ImportErrorPlaceholder("ActionType")
    MotorCommand = _ImportErrorPlaceholder("MotorCommand")
    WorldObjectType = _ImportErrorPlaceholder("WorldObjectType")
    
    def createDefaultConfig():  # noqa: F811
        raise ImportError(
            "pynlm module not available. Please build the Python bindings "
            "first using: pip install -e ."
        )
    
    def createBrain(config):  # noqa: F811
        raise ImportError(
            "pynlm module not available. Please build the Python bindings "
            "first using: pip install -e ."
        )
    
    def createSimpleWorld():  # noqa: F811
        raise ImportError(
            "pynlm module not available. Please build the Python bindings "
            "first using: pip install -e ."
        )
    
    def createAgentBrain(brain):  # noqa: F811
        raise ImportError(
            "pynlm module not available. Please build the Python bindings "
            "first using: pip install -e ."
        )
    
    INVALID_NEURON_ID = _ImportErrorPlaceholder("INVALID_NEURON_ID")
    INVALID_SYNAPSE_ID = _ImportErrorPlaceholder("INVALID_SYNAPSE_ID")
    INVALID_REGION_ID = _ImportErrorPlaceholder("INVALID_REGION_ID")
    INVALID_POPULATION_ID = _ImportErrorPlaceholder("INVALID_POPULATION_ID")
    
    RuntimeError = _ImportErrorPlaceholder("RuntimeError")
    
    # Set __version__
    __version__ = "0.1.0"
    
    # Create placeholder for helpers
    class _ImportErrorPlaceholderHelpers:
        def __getitem__(self, key):
            raise ImportError(
                "pynlm module not available. Please build the Python bindings "
                "first using: pip install -e ."
            )
        
        def __call__(self, *args, **kwargs):
            raise ImportError(
                "pynlm module not available. Please build the Python bindings "
                "first using: pip install -e ."
            )
    
    helpers = _ImportErrorPlaceholderHelpers()
    
    __all__ = [
        'Config', 'Brain', 'SimpleWorld', 'AgentBrain',
        'Vision', 'Audio', 'InternalSignals', 'SensoryInput', 'Action',
        'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept',
        'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
        'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
        'ActionType', 'MotorCommand', 'WorldObjectType',
        'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
        'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
        'INVALID_POPULATION_ID',
        'RuntimeError',
        'helpers',
    ]


# Import alias for convenience
import pynlm as nlm


# Create convenience module that re-exports all symbols
# This allows users to use either `pynlm.Brain` or `pynlm as nlm; nlm.Brain`
nlm = nlm


# Add common import patterns for better IDE support
if False:  # This block is never executed but helps with type checkers
    # These imports help IDEs understand what's available
    from .pynlm import (
        Config, Brain, SimpleWorld, AgentBrain,
        Vision, Audio, SensoryInput, Action,
        NeuronId, SynapseId, RegionId, PopulationId,
        NeuronType, SynapseType, DevelopmentalStage, FiringState,
        ActionType, MotorCommand, WorldObjectType,
        createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
        INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID,
        INVALID_POPULATION_ID,
        RuntimeError,
    )

    from .helpers import (
        QuickStart, SimulationMonitor, ExperimentTemplate,
        BrainContext, AgentContext,
        run_simulation, explore_brain, benchmark,
        visualize_activity, plot_metrics, monitor_simulation,
        tutorial_examples, save_tutorial_examples,
    )


__all__ = list(__all__)