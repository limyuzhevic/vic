# NLM Python Module
# Improved Python bindings for the NLM (Neural Learning Machine) framework

"""
NLM (Neural Learning Machine) Python Interface
================================================

An experimental artificial developmental brain system with Python bindings.

This module provides a Pythonic interface to the NLM C++ neural simulation framework.
It offers improved usability, better error handling, and more advanced features
compared to the original bindings.

Key Features:
- Context managers for proper resource management
- Property-style access for better usability
- Advanced memory management
- Enhanced error handling
- Pythonic iterators and collections
- Improved documentation and examples

Usage:
    import pynlm
    
    # Create a simple brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run a simulation
    with pynlm.simulation_session(brain) as session:
        for step in range(100):
            session.brain.step(step)
    
    print(f"Simulation complete: {brain.total_neurons} neurons, {brain.total_synapses} synapses")
"""

import sys
import os
from pathlib import Path
from typing import Optional, List, Dict, Any, ContextManager
import threading

# Import C++ bindings
sys.path.insert(0, str(Path(__file__).parent / 'build'))
try:
    import pynlm as _pynlm_module
    pynlm_available = True
except ImportError:
    pynlm_available = False
    _pynlm_module = None

# Import advanced features
from .advanced_features import (
    SimulationSession,
    MemoryManager,
    BrainProperty,
    AgentSession,
    WorldManager,
    ErrorTypes,
    ValidationError,
    ConfigurationError
)

class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Raised when there's an error in configuration."""
    pass

class ValidationError(NLMError):
    """Raised when validation fails."""
    pass

class SimulationError(NLMError):
    """Raised during simulation errors."""
    pass

class ResourceError(NLMError):
    """Raised when resource management fails."""
    pass

# Version information
__version__ = "2.0.0"
__author__ = "NLM Project"
__license__ = "MIT"
__description__ = "Python bindings for NLM (Neural Learning Machine)"

# Global error handling
import traceback as _traceback

def _set_error_handler():
    """Set up custom error handling for the module."""
    def _handle_exception(exc_type, exc_value, exc_traceback):
        # Print custom error message
        print(f"[NLM Error] {exc_type.__name__}: {exc_value}")
        print(f"[NLM Traceback] {_traceback.format_exception(exc_type, exc_value, exc_traceback)[-1]}")
    
    # Set up exception hook
    import sys
    old_excepthook = sys.excepthook
    sys.excepthook = _handle_exception

# Set up error handling
_set_error_handler()

# Core class wrappers for better Python integration
class _ConfigWrapper:
    """Wrapper for Config class with improved Python interface."""
    
    def __init__(self, config_obj=None):
        self._config = config_obj if config_obj is not None else _create_default_config()
    
    def __getattr__(self, name):
        if name == '_config':
            raise AttributeError(f"'{self.__class__.__name__}' object has no attribute '{name}'")
        
        # Map Pythonic names to C++ methods
        if hasattr(self._config, name):
            attr = getattr(self._config, name)
            
            # Wrap callable methods
            if callable(attr):
                def wrapper(*args, **kwargs):
                    try:
                        result = attr(*args, **kwargs)
                        # Convert result to Python types
                        return self._convert_result(result)
                    except Exception as e:
                        raise NLMError(f"Config error: {e}") from e
                
                return wrapper
            
            return attr
        
        raise AttributeError(f"'{self.__class__.__name__}' object has no attribute '{name}'")
    
    def __setattr__(self, name, value):
        if name in ['_config']:
            super().__setattr__(name, value)
            return
        
        if hasattr(self._config, name):
            setter = getattr(self._config, f'set_{name}')
            setter(value)
        else:
            raise AttributeError(f"'{self.__class__.__name__}' object has no attribute '{name}' to set")
    
    def _convert_result(self, result):
        """Convert C++ result to Python types."""
        if result is None:
            return None
        
        # Handle shared_ptr return types
        if hasattr(result, 'get'):
            # This is likely a smart pointer
            return result
        
        # Handle basic types
        if isinstance(result, (int, float, str, bool)):
            return result
        
        # Handle collections
        if hasattr(result, '__iter__'):
            return list(result)
        
        return result
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value with default."""
        return self._config.get_or(key, default)
    
    def set(self, key: str, value: Any) -> None:
        """Set a configuration value."""
        self._config.set(key, value)
    
    def has(self, key: str) -> bool:
        """Check if configuration has a key."""
        return self._config.has(key)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        keys = self._config.getKeys()
        result = {}
        for key in keys:
            result[key] = self.get(key)
        return result

class _BrainWrapper:
    """Wrapper for Brain class with improved Python interface."""
    
    def __init__(self, brain_obj=None):
        self._brain = brain_obj
    
    def __getattr__(self, name):
        if name == '_brain':
            raise AttributeError(f"'{self.__class__.__name__}' object has no attribute '{name}'")
        
        if hasattr(self._brain, name):
            attr = getattr(self._brain, name)
            
            if callable(attr):
                def wrapper(*args, **kwargs):
                    try:
                        result = attr(*args, **kwargs)
                        return self._convert_result(result)
                    except Exception as e:
                        raise NLMError(f"Brain error: {e}") from e
                
                return wrapper
            
            return attr
        
        raise AttributeError(f"'{self.__class__.__name__}' object has no attribute '{name}'")
    
    def _convert_result(self, result):
        """Convert C++ result to Python types."""
        if result is None:
            return None
        
        # Handle unique_ptr return types
        if hasattr(result, 'get'):
            return result
        
        # Handle basic types
        if isinstance(result, (int, float, str, bool)):
            return result
        
        # Handle collections
        if hasattr(result, '__iter__'):
            return list(result)
        
        return result
    
    @property
    def total_neurons(self) -> int:
        """Total number of neurons in the brain."""
        return self._brain.getTotalNeuronCount()
    
    @property
    def total_synapses(self) -> int:
        """Total number of synapses in the brain."""
        return self._brain.getTotalSynapseCount()
    
    @property
    def firing_neurons(self) -> int:
        """Number of firing neurons in current step."""
        return self._brain.getFiringNeuronCount()
    
    @property
    def average_firing_rate(self) -> float:
        """Average firing rate across all neurons."""
        return self._brain.getAverageFiringRate()
    
    @property
    def e_i_ratio(self) -> float:
        """Excitation/Inhibition ratio."""
        return self._brain.getExcitationInhibitionRatio()
    
    @property
    def developmental_stage(self) -> Any:
        """Current developmental stage."""
        return self._brain.getDevelopmentalStage()
    
    @property
    def developmental_age(self) -> float:
        """Current developmental age (simulated time)."""
        # This is a simulation age, not actual age
        return self._brain.getConfig().getOr('developmental_age', 0.0)
    
    @property
    def plasticity_modifier(self) -> float:
        """Current plasticity modifier."""
        return self._brain.getConfig().getOr('plasticity_modifier', 1.0)
    
    @property
    def neuromodulation_level(self) -> float:
        """Current neuromodulation level."""
        return self._brain.getNeuromodulationLevel()
    
    @property
    def curiosity_level(self) -> float:
        """Current curiosity level."""
        return self._brain.getCuriosityLevel()
    
    @property
    def novelty_level(self) -> float:
        """Current novelty level."""
        return self._brain.getNoveltyLevel()
    
    @property
    def prediction_error(self) -> float:
        """Current prediction error."""
        return self._brain.getPredictionError()
    
    def get_region_ids(self) -> List[Any]:
        """Get all region IDs."""
        return self._brain.getRegionIds()
    
    def get_regions(self) -> List[Any]:
        """Get all regions."""
        return list(self._brain.getRegions())
    
    def get_working_memory(self) -> Any:
        """Get working memory system."""
        return self._brain.getWorkingMemory()
    
    def get_episodic_memory(self) -> Any:
        """Get episodic memory system."""
        return self._brain.getEpisodicMemory()
    
    def get_associative_memory(self) -> Any:
        """Get associative memory system."""
        return self._brain.getAssociativeMemory()

class _WorldWrapper:
    """Wrapper for World class with improved Python interface."""
    
    def __init__(self, world_obj=None):
        self._world = world_obj
    
    def __getattr__(self, name):
        if name == '_world':
            raise AttributeError(f"'{self.__class__.__name__}' object has no attribute '{name}'")
        
        if hasattr(self._world, name):
            attr = getattr(self._world, name)
            
            if callable(attr):
                def wrapper(*args, **kwargs):
                    try:
                        result = attr(*args, **kwargs)
                        return self._convert_result(result)
                    except Exception as e:
                        raise NLMError(f"World error: {e}") from e
                
                return wrapper
            
            return attr
        
        raise AttributeError(f"'{self.__class__.__name__}' object has no attribute '{name}'")
    
    def _convert_result(self, result):
        """Convert C++ result to Python types."""
        if result is None:
            return None
        
        # Handle basic types
        if isinstance(result, (int, float, str, bool)):
            return result
        
        # Handle collections
        if hasattr(result, '__iter__'):
            return list(result)
        
        return result
    
    @property
    def width(self) -> int:
        """World width."""
        return self._world.getWidth()
    
    @property
    def height(self) -> int:
        """World height."""
        return self._world.getHeight()
    
    @property
    def simulation_time(self) -> float:
        """Current simulation time."""
        return self._world.getSimulationTime()
    
    @property
    def max_energy(self) -> float:
        """Maximum energy."""
        return self._world.getMaxEnergy()
    
    def set_agent_start(self, x: float, y: float) -> None:
        """Set agent start position."""
        self._world.setAgentStart(x, y)
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid in world."""
        return self._world.isValidPosition(x, y)

class _AgentWrapper:
    """Wrapper for Agent class with improved Python interface."""
    
    def __init__(self, agent_obj=None):
        self._agent = agent_obj
    
    def __getattr__(self, name):
        if name == '_agent':
            raise AttributeError(f"'{self.__class__.__name__}' object has no attribute '{name}'")
        
        if hasattr(self._agent, name):
            attr = getattr(self._agent, name)
            
            if callable(attr):
                def wrapper(*args, **kwargs):
                    try:
                        result = attr(*args, **kwargs)
                        return self._convert_result(result)
                    except Exception as e:
                        raise NLMError(f"Agent error: {e}") from e
                
                return wrapper
            
            return attr
        
        raise AttributeError(f"'{self.__class__.__name__}' object has no attribute '{name}'")
    
    def _convert_result(self, result):
        """Convert C++ result to Python types."""
        if result is None:
            return None
        
        # Handle basic types
        if isinstance(result, (int, float, str, bool)):
            return result
        
        return result
    
    @property
    def brain(self) -> _BrainWrapper:
        """Get brain object."""
        return _BrainWrapper(self._agent.getBrain())
    
    @property
    def world(self) -> _WorldWrapper:
        """Get world object."""
        return _WorldWrapper(self._agent.getWorld())
    
    @property
    def developmental_stage(self) -> Any:
        """Current developmental stage."""
        return self._agent.getDevelopmentalStage()

# Create default instances for convenience
_default_config = None
_default_brain = None
_default_world = None
_default_agent = None

def create_default_config() -> _ConfigWrapper:
    """Create a default configuration."""
    global _default_config
    if _default_config is None:
        _default_config = _ConfigWrapper()
    return _default_config

def createBrain(config_obj=None):
    """Create a brain with given configuration."""
    global _default_brain
    if config_obj is None:
        config_obj = create_default_config()
    
    # Create brain using C++ bindings
    brain_obj = _pynlm_module.createBrain(config_obj._config)
    _default_brain = _BrainWrapper(brain_obj)
    return _default_brain

def createSimpleWorld():
    """Create a simple world."""
    global _default_world
    if _default_world is None:
        world_obj = _pynlm_module.createSimpleWorld()
        _default_world = _WorldWrapper(world_obj)
    return _default_world

def createAgentBrain(brain_obj=None):
    """Create an agent brain interface."""
    if brain_obj is None:
        brain_obj = _default_brain._brain if _default_brain else None
    
    if brain_obj is None:
        raise NLMError("Brain object required for agent creation")
    
    agent_obj = _pynlm_module.createAgentBrain(brain_obj)
    return _AgentWrapper(agent_obj)

# Context manager for simulation sessions
class SimulationSession:
    """Context manager for simulation sessions."""
    
    def __init__(self, brain=None, world=None, agent=None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.original_state = None
    
    def __enter__(self):
        self.original_state = self._capture_state()
        
        if self.brain:
            self.brain.initialize()
        
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self._restore_state()
        
        if self.brain:
            self.brain.reset()
        
        return False
    
    def _capture_state(self):
        """Capture initial state for restoration."""
        state = {}
        if self.brain:
            state['brain_neuron_count'] = self.brain.total_neurons
            state['brain_synapse_count'] = self.brain.total_synapses
        return state
    
    def _restore_state(self):
        """Restore initial state."""
        if self.original_state:
            pass  # In a real implementation, this would restore state
    
    def run(self, num_steps: int, timestep: float = 0.1) -> Dict[str, Any]:
        """Run simulation for given number of steps."""
        if not self.brain:
            raise NLMError("Brain required for simulation")
        
        stats = {
            'steps_completed': 0,
            'total_spikes': 0,
            'firing_neurons': 0,
            'neurons': 0,
            'synapses': 0
        }
        
        for step in range(num_steps):
            try:
                if self.world:
                    self.world.update(timestep)
                    if self.agent:
                        percept = self.world.getSensoryPercept()
                        self.agent.processSensoryInput(percept)
                        action = self.agent.decodeMotorCommand()
                        self.world.applyMotorCommand(action, self.world.simulation_time)
                
                self.brain.step(step)
                
                stats['steps_completed'] = step + 1
                stats['total_spikes'] = self.brain.getTotalSpikeCount()
                stats['firing_neurons'] = self.brain.firing_neurons
                stats['neurons'] = self.brain.total_neurons
                stats['synapses'] = self.brain.total_synapses
                
            except Exception as e:
                raise SimulationError(f"Simulation failed at step {step}: {e}") from e
        
        return stats

# Memory management helper
class MemoryManager:
    """Helper for memory management operations."""
    
    def __init__(self, brain=None):
        self.brain = brain
    
    def optimize_working_memory(self, target_size: int = 1000) -> None:
        """Optimize working memory capacity."""
        if not self.brain:
            raise NLMError("Brain required for memory management")
        
        working_memory = self.brain.get_working_memory()
        if working_memory:
            working_memory.setCapacity(target_size)
    
    def consolidate_memories(self, importance_threshold: float = 0.5) -> None:
        """Consolidate episodic memories."""
        if not self.brain:
            raise NLMError("Brain required for memory management")
        
        episodic_memory = self.brain.get_episodic_memory()
        if episodic_memory:
            episodic_memory.consolidate(importance_threshold)
    
    def get_memory_stats(self) -> Dict[str, Any]:
        """Get memory system statistics."""
        if not self.brain:
            raise NLMError("Brain required for memory management")
        
        stats = {}
        
        working_memory = self.brain.get_working_memory()
        if working_memory:
            stats['working_memory'] = {
                'active_traces': working_memory.getActiveTraces(),
                'capacity': working_memory.getCapacity()
            }
        
        episodic_memory = self.brain.get_episodic_memory()
        if episodic_memory:
            stats['episodic_memory'] = {
                'episodes': episodic_memory.getEpisodeCount(),
                'max_episodes': episodic_memory.getMaxEpisodes()
            }
        
        associative_memory = self.brain.get_associative_memory()
        if associative_memory:
            stats['associative_memory'] = {
                'pattern_count': associative_memory.getPatternCount()
            }
        
        return stats

# Agent session helper
class AgentSession:
    """Helper for agent session management."""
    
    def __init__(self, brain=None, world=None):
        self.brain = brain or (_default_brain._brain if _default_brain else None)
        self.world = world or (_default_world._world if _default_world else None)
        self.agent = None
    
    def __enter__(self):
        if not self.brain:
            raise NLMError("Brain required for agent session")
        
        self.agent = createAgentBrain(self.brain)
        
        if self.world:
            self.agent.initialize(self.world)
        
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.agent:
            self.agent.reset()
        
        return False
    
    def run_episode(self, num_steps: int, timestep: float = 0.1) -> Dict[str, Any]:
        """Run an agent episode."""
        if not self.brain or not self.world or not self.agent:
            raise NLMError("Brain, world, and agent required for episode")
        
        stats = {
            'steps_completed': 0,
            'total_spikes': 0,
            'firing_neurons': 0,
            'curiosity': 0,
            'novelty': 0,
            'prediction_error': 0
        }
        
        for step in range(num_steps):
            try:
                self.world.update(timestep)
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.simulation_time)
                
                self.brain.step(step)
                
                stats['steps_completed'] = step + 1
                stats['total_spikes'] = self.brain.getTotalSpikeCount()
                stats['firing_neurons'] = self.brain.firing_neurons
                stats['curiosity'] = self.agent.getCuriosityLevel()
                stats['novelty'] = self.agent.getNoveltyLevel()
                stats['prediction_error'] = self.agent.getPredictionError()
                
            except Exception as e:
                raise SimulationError(f"Episode failed at step {step}: {e}") from e
        
        return stats

# World manager helper
class WorldManager:
    """Helper for world management."""
    
    def __init__(self, world=None):
        self.world = world or (_default_world._world if _default_world else None)
    
    def configure(self, width: int, height: int, vision_width: int = 8, vision_height: int = 8) -> None:
        """Configure world dimensions."""
        if not self.world:
            raise NLMError("World required for configuration")
        
        self.world.configure(width, height, vision_width, vision_height)
    
    def add_resource(self, x: float, y: float, value: float = 1.0, radius: float = 0.5) -> None:
        """Add a resource object to the world."""
        if not self.world:
            raise NLMError("World required for object manipulation")
        
        from pynlm import WorldObject, WorldObjectType
        resource = WorldObject(x, y, WorldObjectType.Resource, value, radius)
        self.world.addObject(resource)
    
    def add_hazard(self, x: float, y: float, value: float = -1.0, radius: float = 0.5) -> None:
        """Add a hazard object to the world."""
        if not self.world:
            raise NLMError("World required for object manipulation")
        
        from pynlm import WorldObject, WorldObjectType
        hazard = WorldObject(x, y, WorldObjectType.Hazard, value, radius)
        self.world.addObject(hazard)

# Experimental features
class ExperimentalFeatures:
    """Access to experimental features and APIs."""
    
    @staticmethod
    def get_phase6_demo():
        """Get Phase 6 experimental demo."""
        # This would import and return Phase 6 demo if available
        pass
    
    @staticmethod
    def get_metrics_calculator():
        """Get metrics calculator."""
        # This would return metrics calculator if available
        pass
    
    @staticmethod
    def run_experiment(experiment_name: str, **kwargs) -> Dict[str, Any]:
        """Run an experiment."""
        # This would run experiments if available
        pass

# Public API exports
__all__ = [
    # Core functions
    'create_default_config',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    
    # Classes
    'SimulationSession',
    'MemoryManager',
    'AgentSession',
    'WorldManager',
    'ExperimentalFeatures',
    
    # Exceptions
    'NLMError',
    'ConfigurationError',
    'ValidationError',
    'SimulationError',
    'ResourceError',
    
    # Constants
    '__version__',
    '__author__',
    '__license__',
    '__description__',
]

# Create convenience instances
_default_config = None
_default_brain = None
_default_world = None
_default_agent = None