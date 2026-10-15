# NLM Python Module Definition
# This module provides the enhanced Python interface for NLM bindings

from . import bindings
from .pynlm import *

# Create a more intuitive import structure
# Users can now import everything they need directly from pynlm

# Re-export all important classes and functions for direct access
__all__ = [
    # Core simulation classes
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
    'Vision', 'Audio', 'InternalSignals',
    
    # Agent and world classes
    'Action', 'WorldObject', 'AgentBody', 'ActionResult',
    'SensoryPercept',
    
    # Type definitions
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'SimulationStep', 'Timestamp', 'MembranePotential', 'Delay',
    
    # Enumerations
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Factory and utility functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld',
    'createAgentBrain', 'createBrainWithDefaultConfig',
    'createCompleteAgentWorld', 'runSimulation', 'runLearningExperiment',
    
    # Helper functions
    'createNeuronId', 'createSynapseId', 'createRegionId',
    'createPopulationId', 'getVersion', 'getInfo',
    
    # Convenience functions
    'pynlm_version',
]

# Add module metadata
__version__ = "1.0.0-enhanced"
__description__ = "Enhanced Python interface for NLM neural simulation framework"

# Module-level documentation
__doc__ = """
NLM Python Interface - Enhanced API
=====================================

This module provides a clean, Pythonic interface to the NLM neural simulation framework.

Key Features:
• Pythonic properties and context managers
• Type-safe configuration
• High-level simulation control
• Agent-environment integration
• Comprehensive error handling

Usage:
    import pynlm
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    
    # Initialize and run
    brain.initialize()
    pynlm.runSimulation(brain, 100)
"""

# Create convenience aliases
import sys
if sys.version_info >= (3, 8):
    from typing import TypedDict
else:
    from typing_extensions import TypedDict

# Optional type definitions for better IDE support
class SimulationConfig(TypedDict):
    """Configuration for simulation runs."""
    steps: int
    timestep: float
    callback: callable

class ActionParams(TypedDict):
    """Action parameters."""
    value: float
    metadata: dict

# Export convenience aliases
brain_config = createDefaultConfig
simulate = runSimulation
learn = runLearningExperiment

# Make commonly used types available at module level
NeuronId.__doc__ = "Unique identifier for a neuron"
SynapseId.__doc__ = "Unique identifier for a synapse"
RegionId.__doc__ = "Unique identifier for a brain region"
PopulationId.__doc__ = "Unique identifier for a neuron population"
