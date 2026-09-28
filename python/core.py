"""
NLM Python Bindings - Core Module

This module contains the core C++ bindings that expose the NLM functionality to Python.
In a real implementation, these would be implemented with pybind11 to create actual
C++ extensions.
"""

from typing import Optional, List, Dict, Any
import numpy as np

# Re-export all the core types and classes
from .core_types import (
    Config,
    Brain,
    RegionId,
    NeuronId,
    NeuronType,
    FiringState,
    DevelopmentalStage,
    ActionType,
    MembranePotential,
    SimulationStep,
    Timestamp,
    Delay,
    TimestepDuration,
)

from .memory import (
    NeuralWorkingMemory,
    NeuralEpisodicMemory,
    NeuralAssociativeMemory,
)

from .neuromodulation import (
    Dopamine,
    Curiosity,
    Novelty,
    PredictionError,
)

from .prediction import PredictionSystem
from .cognition import (
    NeuralPlanner,
    ConceptFormation,
    AttentionalSelection,
)

from .development import DevelopmentSystem
from .plasticity import (
    STDP,
    Hebbian,
    StructuralPlasticity,
)

from .dynamics import SpikeSystem

from .environment import SimpleWorld, SensoryInput, MotorCommand
from .experiments import Phase6IntegratedExperiment, Phase6Config, Phase6IntegrationResult

# Module exports
__all__ = [
    # Core classes
    'Config',
    'Brain',
    'RegionId',
    'NeuronId', 
    'NeuronType',
    'FiringState',
    'DevelopmentalStage',
    'ActionType',
    'MembranePotential',
    'SimulationStep',
    'Timestamp',
    'Delay',
    'TimestepDuration',
    
    # Memory systems
    'NeuralWorkingMemory',
    'NeuralEpisodicMemory',
    'NeuralAssociativeMemory',
    
    # Neuromodulation
    'Dopamine',
    'Curiosity',
    'Novelty',
    'PredictionError',
    
    # Other systems
    'PredictionSystem',
    'NeuralPlanner',
    'ConceptFormation',
    'AttentionalSelection',
    'DevelopmentSystem',
    'STDP',
    'Hebbian',
    'StructuralPlasticity',
    'SpikeSystem',
    
    # Environment and experiments
    'SimpleWorld',
    'SensoryInput',
    'MotorCommand',
    'Phase6IntegratedExperiment',
    'Phase6Config',
    'Phase6IntegrationResult',
]

# Placeholder for the actual C++ extension that would be imported
# In a real implementation, this would be: from . import _pynlm_core
try:
    from . import _pynlm_core
    # Re-export all symbols from the C++ extension
    for name in dir(_pynlm_core):
        if not name.startswith('_'):
            globals()[name] = getattr(_pynlm_core, name)
except ImportError:
    # The C++ extension is not available, use placeholder implementations
    pass

# Type definitions for better type hints
ActionDict = Dict[str, Any]
MetricsDict = Dict[str, float]
Observation = np.ndarray
Command = np.ndarray

# Utility functions for converting between C++ and Python types
def to_python_neuron_type(cpp_type: int) -> NeuronType:
    """Convert C++ neuron type to Python enum."""
    return NeuronType(cpp_type)

def to_cpp_neuron_type(python_type: NeuronType) -> int:
    """Convert Python neuron type to C++ enum."""
    return int(python_type)

def to_python_firing_state(cpp_state: int) -> FiringState:
    """Convert C++ firing state to Python enum."""
    return FiringState(cpp_state)

def to_cpp_firing_state(python_state: FiringState) -> int:
    """Convert Python firing state to C++ enum."""
    return int(python_state)

def to_python_developmental_stage(cpp_stage: int) -> DevelopmentalStage:
    """Convert C++ developmental stage to Python enum."""
    return DevelopmentalStage(cpp_stage)

def to_cpp_developmental_stage(python_stage: DevelopmentalStage) -> int:
    """Convert Python developmental stage to C++ enum."""
    return int(python_stage)

# Memory convenience functions
def create_memory_system(brain: Brain, system_type: str) -> Optional[Any]:
    """Create a specific memory system for a brain."""
    if system_type == "working":
        return brain.get_working_memory()
    elif system_type == "episodic":
        return brain.get_episodic_memory()
    elif system_type == "associative":
        return brain.get_associative_memory()
    else:
        raise ValueError(f"Unknown memory system type: {system_type}")

# Neuromodulation convenience functions
def get_neuromodulation_levels(brain: Brain) -> Dict[str, float]:
    """Get current levels of all neuromodulation systems."""
    levels = {}
    
    dopamine = brain.get_dopamine()
    if dopamine:
        levels['dopamine'] = dopamine.get_level()
    
    curiosity = brain.get_curiosity()
    if curiosity:
        levels['curiosity'] = curiosity.get_level()
    
    novelty = brain.get_novelty()
    if novelty:
        levels['novelty'] = novelty.get_level()
    
    prediction_error = brain.get_prediction_error_signal()
    if prediction_error:
        levels['prediction_error'] = prediction_error.get_level()
    
    return levels

# Action convenience functions
action_type_names = {
    ActionType.Wait: "wait",
    ActionType.MoveForward: "move_forward",
    ActionType.TurnLeft: "turn_left",
    ActionType.TurnRight: "turn_right",
}


def action_to_dict(action: Any) -> Dict[str, Any]:
    """Convert an action to a dictionary representation."""
    if action is None:
        return None
    return {
        'type': action_type_names.get(action.get_type(), 'unknown'),
        'name': action.get_type().name,
    }


def dict_to_action(action_dict: Dict[str, Any]) -> Optional[ActionType]:
    """Convert a dictionary representation to an action type."""
    if action_dict is None:
        return None
    
    action_name = action_dict.get('name', '')
    for action_type in ActionType:
        if action_type.name == action_name:
            return action_type
    
    raise ValueError(f"Unknown action name: {action_name}")
