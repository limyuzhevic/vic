"""
Core NLM module - Re-export of C++ bindings

This module provides the complete API from the C++ bindings
via pybind11. All classes and functions are exposed through
the pynlm extension module.

This file serves as the public interface for the NLM Python bindings.
"""

# Import all C++ exposed classes and functions from the pynlm module
# These are exposed by pybind11 in python/bindings.cpp

# Try to import from the native module first
try:
    from .pynlm_native import (
        # Core types and IDs
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        
        # Enumerations
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        FiringState,
        ActionType,
        MotorCommand,
        WorldObjectType,
        
        # Core classes
        Config,
        Vision,
        Audio,
        InternalSignals,
        SensoryPercept,
        Action,
        ActionResult,
        WorldObject,
        AgentBody,
        SimpleWorld,
        Brain,
        AgentBrain,
        
        # Factory functions
        createDefaultConfig,
        createBrain,
        createSimpleWorld,
        createAgentBrain,
        
        # Constants
        INVALID_NEURON_ID,
        INVALID_SYNAPSE_ID,
        INVALID_REGION_ID,
        INVALID_POPULATION_ID,
    )
    
    # Module is successfully imported
    _NATIVE_MODULE_AVAILABLE = True
    
except ImportError as e:
    # Native module not available
    _NATIVE_MODULE_AVAILABLE = False
    
    # Create placeholder classes that raise informative errors
    class _PlaceholderError(Exception):
        def __init__(self, name):
            super().__init__(
                f"NLM native module not available. "
                f"Please build and install the package. "
                f"Run 'pip install -e .' or 'pip install .' from the project root. "
                f"Original error: {e}"
            )
    
    # Create placeholder classes for all expected exports
    class NeuronId:
        def __init__(self, *args, **kwargs):
            raise _PlaceholderError("NeuronId")
    
    class SynapseId:
        def __init__(self, *args, **kwargs):
            raise _PlaceholderError("SynapseId")
    
    class RegionId:
        def __init__(self, *args, **kwargs):
            raise _PlaceholderError("RegionId")
    
    class PopulationId:
        def __init__(self, *args, **kwargs):
            raise _PlaceholderError("PopulationId")
    
    # Placeholder enumerations
    class NeuronType:
        Excitatory = None
        Inhibitory = None
        Modulatory = None
        Sensory = None
        Motor = None
        Internal = None
    
    class SynapseType:
        Excitatory = None
        Inhibitory = None
        Modulatory = None
        Electrical = None
        GapJunction = None
    
    class DevelopmentalStage:
        Initial = None
        CriticalPeriod = None
        Maturation = None
        Adult = None
        Aging = None
    
    class FiringState:
        Resting = None
        Active = None
        Refractory = None
        Inhibited = None
    
    class ActionType:
        MoveForward = None
        MoveBackward = None
        MoveLeft = None
        MoveRight = None
        TurnLeft = None
        TurnRight = None
        Look = None
        LookUp = None
        LookDown = None
        Interact = None
        Eat = None
        Drink = None
        Rest = None
        Wait = None
        Custom = None
    
    class MotorCommand:
        MoveForward = None
        MoveBackward = None
        TurnLeft = None
        TurnRight = None
        LookLeft = None
        LookRight = None
        Interact = None
        Wait = None
    
    class WorldObjectType:
        Empty = None
        Resource = None
        Hazard = None
        Wall = None
        Marker = None
    
    # Placeholder core classes
    class Config:
        def __init__(self, *args, **kwargs):
            raise _PlaceholderError("Config")
        
        def loadFromFile(self, *args, **kwargs):
            raise _PlaceholderError("Config.loadFromFile")
        
        def loadFromArgs(self, *args, **kwargs):
            raise _PlaceholderError("Config.loadFromArgs")
        
        def saveToFile(self, *args, **kwargs):
            raise _PlaceholderError("Config.saveToFile")
        
        def has(self, *args, **kwargs):
            raise _PlaceholderError("Config.has")
        
        def getKeys(self, *args, **kwargs):
            raise _PlaceholderError("Config.getKeys")
        
        def clear(self, *args, **kwargs):
            raise _PlaceholderError("Config.clear")
        
        def summary(self, *args, **kwargs):
            raise _PlaceholderError("Config.summary")
    
    class Vision:
        def __init__(self, *args, **kwargs):
            raise _PlaceholderError("Vision")
    
    class Audio:
        def __init__(self, *args, **kwargs):
            raise _PlaceholderError("Audio")
    
    class InternalSignals:
        def __init__(self, *args, **kwargs):
            raise _PlaceholderError("InternalSignals")
    
    class SensoryPercept:
        def __init__(self, *args, **kwargs):
            raise _PlaceholderError("SensoryPercept")
    
    class Action:
        def __init__(self, *args, **kwargs):
            raise _PlaceholderError("Action")
    
    class ActionResult:
        def __init__(self, *args, **kwargs):
            raise _PlaceholderError("ActionResult")
    
    class WorldObject:
        def __init__(self, *args, **kwargs):
            raise _PlaceholderError("WorldObject")
    
    class AgentBody:
        def __init__(self, *args, **kwargs):
            raise _PlaceholderError("AgentBody")
    
    class SimpleWorld:
        def __init__(self, *args, **kwargs):
            raise _PlaceholderError("SimpleWorld")
    
    class Brain:
        def __init__(self, *args, **kwargs):
            raise _PlaceholderError("Brain")
    
    class AgentBrain:
        def __init__(self, *args, **kwargs):
            raise _PlaceholderError("AgentBrain")
    
    # Placeholder factory functions
    def createDefaultConfig(*args, **kwargs):
        raise _PlaceholderError("createDefaultConfig")
    
    def createBrain(*args, **kwargs):
        raise _PlaceholderError("createBrain")
    
    def createSimpleWorld(*args, **kwargs):
        raise _PlaceholderError("createSimpleWorld")
    
    def createAgentBrain(*args, **kwargs):
        raise _PlaceholderError("createAgentBrain")
    
    # Placeholder constants
    INVALID_NEURON_ID = None
    INVALID_SYNAPSE_ID = None
    INVALID_REGION_ID = None
    INVALID_POPULATION_ID = None

# Import utilities
from .utils import (
    NLMError,
    ConfigurationError,
    SimulationError,
    BrainError,
    load_config_from_dict,
    create_config_from_brain_params,
    setup_plasticity_config,
    setup_neuromodulation_config,
    create_vision_from_array,
    create_audio_from_samples,
    create_agent_episode,
    train_agent,
    save_brain_state,
    load_brain_state_with_metadata,
)

# Re-export everything
__all__ = [
    # Core types and IDs
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    
    # Enumerations
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Core classes
    'Config', 'Vision', 'Audio', 'InternalSignals', 'SensoryPercept',
    'Action', 'ActionResult', 'WorldObject', 'AgentBody',
    'SimpleWorld', 'Brain', 'AgentBrain',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Utilities
    'NLMError', 'ConfigurationError', 'SimulationError', 'BrainError',
    'load_config_from_dict', 'create_config_from_brain_params',
    'setup_plasticity_config', 'setup_neuromodulation_config',
    'create_vision_from_array', 'create_audio_from_samples',
    'create_agent_episode', 'train_agent',
    'save_brain_state', 'load_brain_state_with_metadata',
]
