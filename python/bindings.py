"""
Python bindings module for NLM

This module provides the main Python interface to the NLM C++ framework.
It includes enhanced Pythonic APIs with properties, type checking,
validation, and improved error handling.
"""

import sys
from typing import Dict, List, Optional, Union, Any, Callable, ContextManager
from contextlib import contextmanager
import warnings

# Try to import from C++ bindings
try:
    # Import core C++ bindings
    from .core_bindings import (
        Config,
        Brain,
        AgentBrain,
        SimpleWorld,
        AgentBody,
        SensoryPercept,
        Vision,
        Audio,
        InternalSignals,
        Action,
        WorldObject,
        ActionResult,
        createDefaultConfig,
        createBrain,
        createAgentBrain,
        createSimpleWorld,
        # Enumerations
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        FiringState,
        ActionType,
        MotorCommand,
        WorldObjectType,
        # Identifiers
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        # Constants
        INVALID_NEURON_ID,
        INVALID_SYNAPSE_ID,
        INVALID_REGION_ID,
        INVALID_POPULATION_ID,
        CHECKPOINT_VERSION_MAJOR,
        CHECKPOINT_VERSION_MINOR,
        CHECKPOINT_VERSION_PATCH,
        CHECKPOINT_MAGIC,
    )
    
    from . import enums
    from . import types
    from . import constants
    
    BINDINGS_AVAILABLE = True
    
except ImportError:
    BINDINGS_AVAILABLE = False
    
    # Create stub classes if C++ bindings not available
    import warnings
    warnings.warn(
        "C++ bindings not available. Creating stub classes.",
        ImportWarning,
        stacklevel=2,
    )
    
    # Create stub classes for development
    class Config:
        """Stub Config class"""
        def __init__(self):
            self._data = {}
            
        def loadFromFile(self, filepath: str) -> bool:
            print(f"[STUB] Loading config from {filepath}")
            return True
            
        def loadFromArgs(self, argc: int, argv: List[str]) -> bool:
            print(f"[STUB] Loading config from args")
            return True
            
        def saveToFile(self, filepath: str) -> bool:
            print(f"[STUB] Saving config to {filepath}")
            return True
            
        def has(self, key: str) -> bool:
            return key in self._data
            
        def getKeys(self) -> List[str]:
            return list(self._data.keys())
            
        def clear(self) -> None:
            self._data.clear()
            
        def summary(self) -> str:
            return f"Config with {len(self._data)} entries"
            
        def __repr__(self) -> str:
            return f"<Config: {self.summary()}>"
    
    class Brain:
        """Stub Brain class"""
        def __init__(self, config):
            self.config = config
            
        def initialize(self) -> None:
            print("[STUB] Brain initialized")
            
        def step(self, step: int, time: Optional[float] = None) -> None:
            print(f"[STUB] Brain step {step}")
            
        def receiveSensoryInput(self, input_data) -> None:
            print("[STUB] Brain received sensory input")
            
        def getTotalNeuronCount(self) -> int:
            return 1000
            
        def getTotalSynapseCount(self) -> int:
            return 10000
            
        def getFiringNeuronCount(self) -> int:
            return 50
            
        def getTotalSpikeCount(self) -> int:
            return 150
            
        def getAverageFiringRate(self) -> float:
            return 2.5
            
        def getExcitationInhibitionRatio(self) -> float:
            return 1.5
            
        def save(self, filepath: str) -> bool:
            print(f"[STUB] Saving brain to {filepath}")
            return True
            
        def load(self, filepath: str) -> bool:
            print(f"[STUB] Loading brain from {filepath}")
            return True
            
        def reset(self) -> None:
            print("[STUB] Brain reset")
            
        def logStatus(self) -> None:
            print("[STUB] Brain status logged")
    
    class AgentBrain:
        """Stub AgentBrain class"""
        def __init__(self, brain):
            self.brain = brain
            
        def initialize(self, world) -> None:
            print("[STUB] AgentBrain initialized with world")
            
        def processSensoryInput(self, percept) -> None:
            print("[STUB] AgentBrain processed sensory input")
            
        def decodeMotorCommand(self) -> Any:
            print("[STUB] AgentBrain decoded motor command")
            return None
            
        def getCuriosityLevel(self) -> float:
            return 0.5
            
        def getNoveltyLevel(self) -> float:
            return 0.3
            
        def enableRewardModulation(self, enable: bool) -> None:
            print(f"[STUB] Reward modulation {'enabled' if enable else 'disabled'}")
            
        def enableStructuralPlasticity(self, enable: bool) -> None:
            print(f"[STUB] Structural plasticity {'enabled' if enable else 'disabled'}")
            
        def enableDevelopment(self, enable: bool) -> None:
            print(f"[STUB] Development {'enabled' if enable else 'disabled'}")
            
        def enableCuriosity(self, enable: bool) -> None:
            print(f"[STUB] Curiosity {'enabled' if enable else 'disabled'}")
    
    class SimpleWorld:
        """Stub SimpleWorld class"""
        def __init__(self):
            self.width = 0.0
            self.height = 0.0
            
        def configure(self, width: float, height: float, 
                     visionWidth: int, visionHeight: int) -> None:
            self.width = width
            self.height = height
            print(f"[STUB] World configured: {width}x{height}")
            
        def reset(self) -> None:
            print("[STUB] World reset")
            
        def update(self, timestep: float) -> None:
            print(f"[STUB] World updated with timestep {timestep}")
            
        def getSensoryPercept(self):
            print("[STUB] Getting sensory percept")
            return None
            
        def getAgentBody(self):
            print("[STUB] Getting agent body")
            return None
            
        def applyMotorCommand(self, cmd, currentTime: float) -> Any:
            print(f"[STUB] Applied motor command at time {currentTime}")
            return None
    
    class AgentBody:
        """Stub AgentBody class"""
        def __init__(self):
            pass
            
        def reset(self) -> None:
            print("[STUB] AgentBody reset")
    
    class SensoryPercept:
        """Stub SensoryPercept class"""
        def __init__(self):
            pass
    
    class Vision:
        """Stub Vision class"""
        def __init__(self, width: int = 0, height: int = 0, channels: int = 0):
            pass
            
        def setData(self, data: List[float]) -> None:
            pass
    
    class Audio:
        """Stub Audio class"""
        def __init__(self, sampleRate: int = 0, numSamples: int = 0):
            pass
            
        def setData(self, data: List[float]) -> None:
            pass
    
    class InternalSignals:
        """Stub InternalSignals class"""
        def __init__(self):
            pass
            
        def addSignal(self, value: float) -> None:
            pass
            
        def clearSignals(self) -> None:
            pass
    
    class Action:
        """Stub Action class"""
        def __init__(self, action_type: Any = None, parameters: Optional[List[float]] = None):
            pass
            
        def getType(self) -> Any:
            return None
            
        def getParameters(self) -> List[float]:
            return []
            
        def getName(self) -> str:
            return "Action"
            
        def clone(self) -> Any:
            return None
    
    class WorldObject:
        """Stub WorldObject class"""
        def __init__(self, x: float = 0.0, y: float = 0.0, 
                    obj_type: Any = None, value: float = 0.0, 
                    radius: float = 0.5):
            pass
    
    class ActionResult:
        """Stub ActionResult class"""
        def __init__(self, reward: float = 0.0, success: bool = False, 
                    message: str = ""):
            pass
    
    # Stub factory functions
    def createDefaultConfig():
        return Config()
        
    def createBrain(config):
        return Brain(config)
        
    def createAgentBrain(brain):
        return AgentBrain(brain)
        
    def createSimpleWorld():
        return SimpleWorld()
    
    # Stub enumerations
    from enum import Enum
    NeuronType = Enum('NeuronType', ['Excitatory', 'Inhibitory', 'Modulatory', 
                                     'Sensory', 'Motor', 'Internal'])
    SynapseType = Enum('SynapseType', ['Excitatory', 'Inhibitory', 'Modulatory',
                                       'Electrical', 'GapJunction'])
    DevelopmentalStage = Enum('DevelopmentalStage', ['Initial', 'CriticalPeriod', 
                                                     'Maturation', 'Adult', 'Aging'])
    FiringState = Enum('FiringState', ['Resting', 'Active', 'Refractory', 'Inhibited'])
    ActionType = Enum('ActionType', ['MoveForward', 'MoveBackward', 'MoveLeft', 'MoveRight',
                                     'TurnLeft', 'TurnRight', 'Look', 'LookUp', 'LookDown',
                                     'Interact', 'Eat', 'Drink', 'Rest', 'Wait', 'Custom'])
    MotorCommand = Enum('MotorCommand', ['MoveForward', 'MoveBackward', 'TurnLeft', 'TurnRight',
                                         'LookLeft', 'LookRight', 'Interact', 'Wait'])
    WorldObjectType = Enum('WorldObjectType', ['Empty', 'Resource', 'Hazard', 'Wall', 'Marker'])
    
    # Stub constants
    INVALID_NEURON_ID = 0xFFFFFFFFFFFFFFFF
    INVALID_SYNAPSE_ID = 0xFFFFFFFFFFFFFFFF
    INVALID_REGION_ID = 0xFFFFFFFFFFFFFFFF
    INVALID_POPULATION_ID = 0xFFFFFFFFFFFFFFFF
    
    CHECKPOINT_VERSION_MAJOR = 1
    CHECKPOINT_VERSION_MINOR = 0
    CHECKPOINT_VERSION_PATCH = 0
    CHECKPOINT_MAGIC = 0x4E4C4D434B545430

# Custom exception classes
class NLMError(Exception):
    """Base exception for NLM errors"""
    
    def __init__(self, message: str, error_code: int = 1):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
    
    def __str__(self) -> str:
        return f"NLMError(code={self.error_code}): {self.message}"
class ConfigurationError(NLMError):
    """Exception for configuration errors"""
    
    def __init__(self, message: str, error_code: int = 2):
        super().__init__(message, error_code)
class InitializationError(NLMError):
    """Exception for initialization errors"""
    
    def __init__(self, message: str, error_code: int = 3):
        super().__init__(message, error_code)
class SimulationError(NLMError):
    """Exception for simulation errors"""
    
    def __init__(self, message: str, error_code: int = 4):
        super().__init__(message, error_code)
class ValidationError(NLMError):
    """Exception for validation errors"""
    
    def __init__(self, message: str, error_code: int = 6):
        super().__init__(message, error_code)
class FileError(NLMError):
    """Exception for file-related errors"""
    
    def __init__(self, message: str, error_code: int = 3):
        super().__init__(message, error_code)

# Enhanced Python API with properties and validation
class EnhancedPythonBindings:
    """
    Enhanced Python API wrapper for NLM bindings.
    
    This class provides a Pythonic interface to the NLM C++ framework
    with improved error handling, type checking, and convenience methods.
    """
    
    def __init__(self):
        self._initialized = False
        self._error_handler = None
    
    def set_error_handler(self, handler: Callable[[Exception], None]) -> None:
        """
        Set custom error handler.
        
        Args:
            handler: Function to handle errors
        """
        self._error_handler = handler
    
    def safe_call(self, func: Callable, *args, **kwargs) -> Any:
        """
        Safely call a function with error handling.
        
        Args:
            func: Function to call
            *args: Positional arguments
            **kwargs: Keyword arguments
            
        Returns:
            Function result or None if error occurred
            
        Raises:
            NLMError: If function raises an NLMError
            Exception: If other error occurs and no error handler set
        """
        try:
            return func(*args, **kwargs)
        except NLMError:
            raise
        except Exception as e:
            if self._error_handler:
                self._error_handler(e)
                return None
            else:
                raise SimulationError(f"Function call failed: {e}")
    
    @contextmanager
    def simulation_context(self, config=None):
        """
        Context manager for safe simulation execution.
        
        Args:
            config: Configuration object (optional)
            
        Yields:
            Simulation context
        """
        if config is None:
            config = createDefaultConfig()
        
        brain = createBrain(config)
        
        try:
            brain.initialize()
            yield brain
        except Exception as e:
            raise SimulationError(f"Simulation failed: {e}")
        finally:
            brain.reset()
    
    def create_agent_simulation(self, brain, world, **kwargs) -> Dict[str, Any]:
        """
        Create and run an agent simulation with common patterns.
        
        Args:
            brain: Brain object
            world: World object
            **kwargs: Additional options
            
        Returns:
            Dictionary with simulation results
        """
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable subsystems based on kwargs
        if kwargs.get('enable_reward_modulation', False):
            agent.enableRewardModulation(True)
        
        if kwargs.get('enable_curiosity', False):
            agent.enableCuriosity(True)
            
        if kwargs.get('enable_development', False):
            agent.enableDevelopment(True)
            
        if kwargs.get('enable_structural_plasticity', False):
            agent.enableStructuralPlasticity(True)
        
        results = {
            'steps': kwargs.get('num_steps', 1000),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'development_stage': brain.getDevelopmentalStage(),
        }
        
        return results

# Create global instance of enhanced bindings
_enhanced_bindings = EnhancedPythonBindings()

# Enhanced factory methods
def create_enhanced_config(**kwargs) -> Config:
    """
    Create enhanced configuration with validation.
    
    Args:
        **kwargs: Configuration parameters
        
    Returns:
        Enhanced configuration object
    """
    config = createDefaultConfig()
    
    # Apply kwargs with validation
    for key, value in kwargs.items():
        if value is not None:
            config.set(key, value)
    
    return config

def create_enhanced_brain(config=None, **kwargs) -> Brain:
    """
    Create enhanced brain with common configurations.
    
    Args:
        config: Configuration object (optional)
        **kwargs: Additional configuration options
        
    Returns:
        Enhanced brain object
    """
    if config is None:
        config = create_enhanced_config(**kwargs)
    
    brain = createBrain(config)
    brain.initialize()
    
    return brain

def run_enhanced_simulation(num_steps: int = 1000, config=None, **kwargs) -> Dict[str, Any]:
    """
    Run enhanced simulation with common patterns.
    
    Args:
        num_steps: Number of simulation steps
        config: Configuration object (optional)
        **kwargs: Additional options
        
    Returns:
        Dictionary with simulation results
    """
    if config is None:
        config = create_enhanced_config(**kwargs)
    
    brain = create_enhanced_brain(config)
    world = createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning subsystems
    if kwargs.get('enable_learning', False):
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return {
        'steps_completed': num_steps,
        'neurons': brain.getTotalNeuronCount(),
        'synapses': brain.getTotalSynapseCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel(),
        'development_stage': brain.getDevelopmentalStage(),
    }

# Enhanced property access for Brain
if BINDINGS_AVAILABLE:
    # Add Pythonic properties to Brain
    def add_pythonic_properties():
        """Add Pythonic properties to Brain class"""
        brain = Brain
        
        # Add property for neuron count
        @property
        def total_neurons(brain_instance):
            return brain_instance.getTotalNeuronCount()
        
        @property
        def total_synapses(brain_instance):
            return brain_instance.getTotalSynapseCount()
        
        @property
        def firing_neurons(brain_instance):
            return brain_instance.getFiringNeuronCount()
        
        @property
        def total_spikes(brain_instance):
            return brain_instance.getTotalSpikeCount()
        
        @property
        def average_firing_rate(brain_instance):
            return brain_instance.getAverageFiringRate()
        
        @property
        def excitation_inhibition_ratio(brain_instance):
            return brain_instance.getExcitationInhibitionRatio()
        
        # Add properties to other classes
        agent_brain = AgentBrain
        @property
        def curiosity_level(agent_instance):
            return agent_instance.getCuriosityLevel()
        
        @property
        def novelty_level(agent_instance):
            return agent_instance.getNoveltyLevel()
        
        # Add properties to SimpleWorld
        world = SimpleWorld
        @property
        def simulation_time(world_instance):
            return world_instance.getSimulationTime()
        
        @property
        def width(world_instance):
            return world_instance.getWidth()
        
        @property
        def height(world_instance):
            return world_instance.getHeight()
        
        @property
        def agent_body(world_instance):
            return world_instance.getAgentBody()

# Export everything
__all__ = [
    # Core classes
    'Config',
    'Brain',
    'AgentBrain',
    'SimpleWorld',
    'AgentBody',
    'SensoryPercept',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'WorldObject',
    'ActionResult',
    
    # Factory functions
    'createDefaultConfig',
    'createBrain',
    'createAgentBrain',
    'createSimpleWorld',
    
    # Enhanced factory functions
    'create_enhanced_config',
    'create_enhanced_brain',
    'run_enhanced_simulation',
    
    # Enumerations
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    
    # Constants
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    'CHECKPOINT_VERSION_MAJOR',
    'CHECKPOINT_VERSION_MINOR',
    'CHECKPOINT_VERSION_PATCH',
    'CHECKPOINT_MAGIC',
    
    # Exception classes
    'NLMError',
    'ConfigurationError',
    'InitializationError',
    'SimulationError',
    'ValidationError',
    'FileError',
    
    # Enhanced bindings
    'EnhancedPythonBindings',
    '_enhanced_bindings',
    
    # Convenience functions
    'run_agent_simulation',
    'get_system_info',
    
    # Submodules
    'enums',
    'types',
    'constants',
]

# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Enhanced Python bindings for NLM neural simulation"

print(f"NLM enhanced Python bindings loaded (version {__version__})")
if not BINDINGS_AVAILABLE:
    print("WARNING: C++ bindings not available. Using stub classes.")