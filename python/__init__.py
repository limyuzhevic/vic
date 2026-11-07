# NLM - Enhanced Python Bindings
# This file contains the improved Python bindings for the NLM (Neural Learning Machine) framework.
# It provides a modern, Pythonic interface to the C++ neural simulation capabilities.

import numpy as np
import json
import time
import logging
from typing import Dict, List, Any, Optional, Union, Callable
from dataclasses import dataclass
from contextlib import contextmanager

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Import the C++ bindings
import pynlm

# Type aliases for better readability
NeuronID = pynlm.NeuronId
SynapseID = pynlm.SynapseId
RegionID = pynlm.RegionId
PopulationID = pynlm.PopulationId
NeuronType = pynlm.NeuronType
ActionType = pynlm.ActionType
WorldObjectType = pynlm.WorldObjectType
DevelopmentalStage = pynlm.DevelopmentalStage
FiringState = pynlm.FiringState
ActionResult = pynlm.ActionResult
NeuralRegion = pynlm.NeuralRegion
Brain = pynlm.Brain
Config = pynlm.Config
Vision = pynlm.Vision
Audio = pynlm.Audio
InternalSignals = pynlm.InternalSignals
SensoryPercept = pynlm.SensoryPercept
SimpleWorld = pynlm.SimpleWorld
AgentBrain = pynlm.AgentBrain
Dopamine = pynlm.Dopamine
Curiosity = pynlm.Curiosity
Novelty = pynlm.Novelty
PredictionError = pynlm.PredictionError

# Global error class
class NLMError(Exception):
    """Base exception for NLM errors"""
    pass

class ConfigError(NLMError):
    """Configuration-related errors"""
    pass

class BrainError(NLMError):
    """Brain-related errors"""
    pass

class SimulationError(NLMError):
    """Simulation-related errors"""
    pass

@dataclass
class SimulationStep:
    """Container for simulation step information"""
    step: int
    time: float
    reward: float = 0.0
    neuromodulation: float = 0.0
    curiosity: float = 0.0
    energy: float = 0.0
    action: str = ""
    spike_count: int = 0
    firing_rate: float = 0.0

@dataclass
class EpisodeStats:
    """Container for episode statistics"""
    episode: int
    steps: int
    total_reward: float
    final_energy: float
    final_novelty: float
    final_curiosity: float
    prediction_error: float
    developmental_age: float
    developmental_stage: str
    total_synapses: int
    average_weight: float
    max_weight: float
    total_spikes: int
    average_firing_rate: float
    action_counts: Dict[str, int]

class NLMConfig:
    """
    Enhanced configuration management for NLM with Pythonic interface
    
    This class provides a more Pythonic interface to the underlying Config class,
    with additional convenience methods and validation.
    """
    
    def __init__(self, config_file: Optional[str] = None):
        """Initialize configuration"""
        self._config = pynlm.createDefaultConfig()
        
        if config_file:
            if not self._config.loadFromFile(config_file):
                raise ConfigError(f"Failed to load configuration from {config_file}")
        
        # Set defaults if not already set
        defaults = {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 500,
            "region_count": 1,
            "connection_probability": 0.15,
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015,
            "stdp_tau": 20.0,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001,
        }
        
        for key, value in defaults.items():
            if not self._config.has(key):
                self._config.set(key, value)
    
    def __getitem__(self, key: str) -> Any:
        """Get configuration value using [] operator"""
        return self._config[key]
    
    def __setitem__(self, key: str, value: Any) -> None:
        """Set configuration value using [] operator"""
        self._config.set(key, value)
    
    def __repr__(self) -> str:
        """String representation of configuration"""
        return f"<NLMConfig: {self._config.getSummary()}>"
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value with default"""
        try:
            return self._config[key]
        except:
            return default
    
    def has(self, key: str) -> bool:
        """Check if configuration key exists"""
        return self._config.has(key)
    
    def keys(self) -> List[str]:
        """Get all configuration keys"""
        return self._config.getKeys()
    
    def update(self, other: Dict[str, Any]) -> None:
        """Update configuration from dictionary"""
        for key, value in other.items():
            self._config.set(key, value)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary"""
        result = {}
        for key in self.keys():
            result[key] = self.get(key)
        return result
    
    def save(self, filepath: str) -> bool:
        """Save configuration to file"""
        return self._config.saveToFile(filepath)
    
    def validate(self) -> bool:
        """Validate configuration (placeholder for advanced validation)"""
        # Basic validation
        required_keys = ["random_seed", "simulation_timestep", "neuron_count"]
        for key in required_keys:
            if not self.has(key):
                logger.warning(f"Configuration key '{key}' is missing")
        return True

class NLMBrain:
    """
    Enhanced brain interface for NLM with Pythonic API
    
    This class provides a more Pythonic interface to the underlying Brain class,
    with context manager support, convenience methods, and better error handling.
    """
    
    def __init__(self, config: Optional[NLMConfig] = None):
        """Initialize brain with configuration"""
        if config is None:
            config = NLMConfig()
        
        self._brain = pynlm.createBrain(config._config)
        self._config = config
        self._initialized = False
        self._step_count = 0
    
    def __enter__(self) -> 'NLMBrain':
        """Context manager entry"""
        self.initialize()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        """Context manager exit"""
        self.reset()
    
    def initialize(self) -> bool:
        """Initialize the brain"""
        if self._initialized:
            return True
        
        try:
            self._initialized = self._brain.initialize()
            if not self._initialized:
                raise BrainError("Failed to initialize brain")
            logger.info("Brain initialized successfully")
            return True
        except Exception as e:
            raise BrainError(f"Brain initialization failed: {str(e)}")
    
    def reset(self) -> None:
        """Reset brain state"""
        if self._initialized:
            self._brain.reset()
            self._initialized = False
            self._step_count = 0
            logger.info("Brain reset")
    
    def step(self, current_step: int, current_time: Optional[float] = None) -> bool:
        """Perform a simulation step"""
        if not self._initialized:
            raise BrainError("Brain not initialized. Call initialize() or use with context manager.")
        
        try:
            if current_time is None:
                current_time = current_step * self._config.get("simulation_timestep", 0.001)
            
            self._brain.step(current_step, current_time)
            self._step_count = current_step
            return True
        except Exception as e:echa raise SimulationError(f"Simulation step failed: {str(e)}")
    
    def inject_current(self, neuron_id: NeuronID, current: float) -> None:
        """Inject current into a specific neuron"""
        if not self._initialized:
            raise BrainError("Brain not initialized")
        
        try:
            self._brain.injectCurrent(neuron_id, current)
        except Exception as e:
            raise BrainError(f"Failed to inject current: {str(e)}")
    
    def inject_current_to_neurons(self, neuron_type: NeuronType, current: float) -> None:
        """Inject current into all neurons of a specific type"""
        if not self._initialized:
            raise BrainError("Brain not initialized")
        
        try:
شروع self._brain.injectCurrentToNeurons(neuron_type, current)
        except Exception as e:
            raise BrainError(f"Failed to inject current to neurons: {str(e)}")
    
    def receive_sensory_input(self, input_data: SensoryPercept) -> None:
        """Receive sensory input from environment"""
        if not self._initialized:
            raise BrainError("Brain not initialized")
        
        try:
            self._brain.receiveSensoryInput(input_data)
        except Exception as e:
            raise BrainError(f"Failed to receive sensory input: {str(e)}")
    
    def produce_action(self) -> ActionResult:
        """Produce motor/action output based on neural activity"""
        if not self._initialized:
            raise BrainError("Brain not initialized")
        
        try:
            action = self._brain.produceAction()
            if action is None:
                return ActionResult()
            return action
        except Exception as e:
            raise BrainError(f"Failed to produce action: {str(e)}")
    
    def get_total_spike_count(self) -> int:
        """Get total spike count"""
        if not self._initialized:
            raise BrainError("Brain not initialized")
        
        try:
            return self._brain.getTotalSpikeCount()
        except Exception as e:
            raise BrainError(f"Failed to get spike count: {str(e)}")
    
    def get_active_neuron_count(self) -> int:
        """Get count of active neurons"""
        if not self._initialized:
            raise BrainError("Brain not initialized")
        
        try:
            return self._brain.getActiveNeuronCount()
        except Exception as e:
            raise BrainError(f"Failed to get active neuron count: {str(e)}")
    
    def get_firing_neuron_count(self) -> int:
        """Get count of currently firing neurons"""
        if not self._initialized:
            raise BrainError("Brain not initialized")
        
        try:
            return self._brain.getFiringNeuronCount()
        except Exception as e:
            raise BrainError(f"Failed to get firing neuron count: {str(e)}")
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate across all neurons"""
        if not self._initialized:
            raise BrainError("Brain not initialized")
        
        try:
            return self._brain.getAverageFiringRate()
        except Exception as e:
            raise BrainError(f"Failed to get average firing rate: {str(e)}")
    
    def get_total_neuron_count(self) -> int:
        """Get total neuron count across all regions"""
        if not self._initialized:
            raise BrainError("Brain not initialized")
        
        try:
            return self._brain.getTotalNeuronCount()
        except Exception as e:
            raise BrainError(f"Failed to get total neuron count: {str(e)}")
    
    def get_total_synapse_count(self) -> int:
        """Get total synapse count across all regions"""
        if not self._initialized:
            raise BrainError("Brain not initialized")
        
        try:
            return self._brain.getTotalSynapseCount()
        except Exception as e:
            raise BrainError(f"Failed to get total synapse count: {str(e)}")
    
    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition balance ratio"""
        if not self._initialized:
            raise BrainError("Brain not initialized")
        
        try:
            return self._brain.getExcitationInhibitionRatio()
        except Exception as e:
            raise BrainError(f"Failed to get excitation/inhibition ratio: {str(e)}")
    
    def save(self, filepath: str) -> bool:
        """Save brain state to file"""
        if not self._initialized:
            raise BrainError("Brain not initialized")
        
        try:
            return self._brain.save(filepath)
        except Exception as e:
            raise BrainError(f"Failed to save brain: {str(e)}")
    
    def load(self, filepath: str) -> bool:
        """Load brain state from file"""
        try:
            return self._brain.load(filepath)
        except Exception as e:
            raise BrainError(f"Failed to load brain: {str(e)}")
    
    def add_region(self, name: str = "") -> RegionID:
        """Add a new neural region"""
        if not self._initialized:
            raise BrainError("Brain not initialized")
        
        try:
            return self._brain.addRegion(name)
        except Exception as e:
            raise BrainError(f"Failed to add region: {str(e)}")
    
    def get_region(self, region_id: RegionID):
        """Get region by ID"""
        if not self._initialized:
            raise BrainError("Brain not initialized")
        
        try:
            return self._brain.getRegion(region_id)
        except Exception as e:
            raise BrainError(f"Failed to get region: {str(e)}")
    
    def get_region_count(self) -> int:
        """Get the number of regions"""
        if not self._initialized:
            raise BrainError("Brain not initialized")
        
        try:
            return self._brain.getRegionCount()
        except Exception as e:
            raise BrainError(f"Failed to get region count: {str(e)}")
    
    def get_region_ids(self) -> List[RegionID]:
        """Get all region IDs"""
        if not self._initialized:
            raise BrainError("Brain not initialized")
        
        try:
            return self._brain.getRegionIds()
        except Exception as e:
            raise BrainError(f"Failed to get region IDs: {str(e)}")
    
    def get_total_spike_count(self) -> int:
        """Get total spike count"""
        if not self._initialized:
            raise BrainError("Brain not initialized")
        
        try:
            return self._brain.getTotalSpikeCount()
        except Exception as e:
            raise BrainError(f"Failed to get spike count: {str(e)}")
    
    def log_status(self) -> None:
        """Log brain status"""
        if not self._initialized:
            raise BrainError("Brain not initialized")
        
        try:
            self._brain.logStatus()
        except Exception as e:
            raise BrainError(f"Failed to log brain status: {str(e)}")
    
    def get_config(self) -> NLMConfig:
        """Get the configuration used by this brain"""
        return self._config

class NLMWorld:
    """
    Enhanced world interface for NLM simulation
    
    This class provides a Pythonic interface for the world simulation,
    including sensory perception and agent body management.
    """
    
    def __init__(self, width: int = 100, height: int = 100, 
                 vision_width: int = 10, vision_height: int = 10):
        """Initialize world"""
        self._world = pynlm.createSimpleWorld()
        self._world.configure(width, height, vision_width, vision_height)
        self._last_percept = None
    
    def reset(self) -> None:
        """Reset world state"""
        self._world.reset()
        self._last_percept = None
    
    def set_agent_start(self, x: float, y: float) -> None:
        """Set agent starting position"""
        self._world.setAgentStart(x, y)
    
    def update(self, timestep: float) -> None:
        """Update world state"""
        self._world.update(timestep)
        self._last_percept = self._world.getSensoryPercept()
    
    def get_sensory_percept(self) -> SensoryPercept:
        """Get sensory percept from world"""
        if self._last_percept is None:
            self._last_percept = self._world.getSensoryPercept()
        return self._last_percept
    
    def get_agent_body(self):
        """Get agent body from world"""
        return self._world.getAgentBody()
    
    def add_object(self, obj: pynlm.WorldObject) -> None:
        """Add a world object"""
        self._world.addObject(obj)
    
    def remove_object(self, x: float, y: float) -> None:
        """Remove object at position"""
        self._world.removeObject(x, y)
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid"""
        return self._world.isValidPosition(x, y)
    
    def get_width(self) -> int:
        """Get world width"""
        return self._world.getWidth()
    
    def get_height(self) -> int:
        """Get world height"""
        return self._world.getHeight()
    
    def get_max_energy(self) -> float:
        """Get maximum energy"""
        return self._world.getMaxEnergy()
    
    def set_max_energy(self, energy: float) -> None:
        """Set maximum energy"""
        self._world.setMaxEnergy(energy)
    
    def get_energy_decay_rate(self) -> float:
        """Get energy decay rate"""
        return self._world.getEnergyDecayRate()
    
    def set_energy_decay_rate(self, rate: float) -> None:
        """Set energy decay rate"""
        self._world.setEnergyDecayRate(rate)
    
    def get_simulation_time(self) -> float:
        """Get simulation time"""
        return self._world.getSimulationTime()
    
    def set_random_seed(self, seed: int) -> None:
        """Set random seed"""
        self._world.setRandomSeed(seed)
    
    def get_random_seed(self) -> int:
        """Get random seed"""
        return self._world.getRandomSeed()
    
    def __repr__(self) -> str:
        """String representation of world"""
        return f"<NLMWorld: {self._world.getWidth()}x{self._world.getHeight()}>"