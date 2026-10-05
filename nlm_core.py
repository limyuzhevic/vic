#!/usr/bin/env python3
"""
NLM Core - Shared functionality for NLM Python API

This module contains all the core functionality shared across different
API modules, including base classes, utility functions, configuration
management, and common operations.
"""

import typing as t
from typing import Any, Callable, Dict, List, Optional, Union, Tuple, Type
import json
import logging
import sys
import os
from pathlib import Path
from dataclasses import dataclass, field
from enum import Enum
import numpy as np

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

@dataclass
class NeuronId:
    """Unique identifier for a neuron."""
    value: int = 0
    
    def index(self) -> int:
        """Get the index part of the ID."""
        return self.value
    
    def __eq__(self, other: Any) -> bool:
        if isinstance(other, NeuronId):
            return self.value == other.value
        return False
    
    def __ne__(self, other: Any) -> bool:
        return not self.__eq__(other)
    
    def __hash__(self) -> int:
        return hash(self.value)
    
    def __repr__(self) -> str:
        return f"<NeuronId: {self.value}>"

@dataclass
class SynapseId:
    """Unique identifier for a synapse."""
    value: int = 0
    
    def index(self) -> int:
        """Get the index part of the ID."""
        return self.value
    
    def __eq__(self, other: Any) -> bool:
        if isinstance(other, SynapseId):
            return self.value == other.value
        return False
    
    def __ne__(self, other: Any) -> bool:
        return not self.__eq__(other)
    
    def __hash__(self) -> int:
        return hash(self.value)
    
    def __repr__(self) -> str:
        return f"<SynapseId: {self.value}>"

@dataclass
class RegionId:
    """Unique identifier for a brain region."""
    value: int = 0
    
    def index(self) -> int:
        """Get the index part of the ID."""
        return self.value
    
    def __eq__(self, other: Any) -> bool:
        if isinstance(other, RegionId):
            return self.value == other.value
        return False
    
    def __ne__(self, other: Any) -> bool:
        return not self.__eq__(other)
    
    def __hash__(self) -> int:
        return hash(self.value)
    
    def __repr__(self) -> str:
        return f"<RegionId: {self.value}>"

@dataclass
class PopulationId:
    """Unique identifier for a neuron population."""
    value: int = 0
    
    def __eq__(self, other: Any) -> bool:
        if isinstance(other, PopulationId):
            return self.value == other.value
        return False
    
    def __ne__(self, other: Any) -> bool:
        return not self.__eq__(other)
    
    def __hash__(self) -> int:
        return hash(self.value)

class NeuronType(Enum):
    """Neuron type enumeration."""
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    SENSORY = "sensory"
    MOTOR = "motor"
    INTERNAL = "internal"

class SynapseType(Enum):
    """Synapse type enumeration."""
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    ELECTRICAL = "electrical"
    GAP_JUNCTION = "gap_junction"

class DevelopmentalStage(Enum):
    """Developmental stage enumeration."""
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"

class FiringState(Enum):
    """Neuron firing state enumeration."""
    RESTING = "resting"
    ACTIVE = "active"
    REFRACTORY = "refractory"
    INHIBITED = "inhibited"

class ActionType(Enum):
    """Action type enumeration."""
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    MOVE_LEFT = "move_left"
    MOVE_RIGHT = "move_right"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK = "look"
    LOOK_UP = "look_up"
    LOOK_DOWN = "look_down"
    INTERACT = "interact"
    EAT = "eat"
    DRINK = "drink"
    REST = "rest"
    WAIT = "wait"
    CUSTOM = "custom"

class MotorCommand(Enum):
    """Low-level motor command enumeration."""
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK_LEFT = "look_left"
    LOOK_RIGHT = "look_right"
    INTERACT = "interact"
    WAIT = "wait"

class WorldObjectType(Enum):
    """World object type enumeration."""
    EMPTY = "empty"
    RESOURCE = "resource"
    HAZARD = "hazard"
    WALL = "wall"
    MARKER = "marker"

# Invalid ID constants
INVALID_NEURON_ID = NeuronId(-1)
INVALID_SYNAPSE_ID = SynapseId(-1)
INVALID_REGION_ID = RegionId(-1)
INVALID_POPULATION_ID = PopulationId(-1)

# Type aliases
ConfigValue = Union[int, float, str, bool, List[Any], Dict[str, Any]]
ConfigDict = Dict[str, ConfigValue]
NeuronParameters = Dict[str, Any]

# Global constants
PI = 3.14159265359
E = 2.71828182846
DEFAULT_NEURON_COUNT = 1000
DEFAULT_SYNAPSE_DENSITY = 0.1
MAX_REPRESENTATION_DEPTH = 10

# Exception classes
class ConfigurationError(Exception):
    """Exception raised for configuration-related errors."""
    pass

class SimulationError(Exception):
    """Exception raised for simulation-related errors."""
    pass

class ValidationError(Exception):
    """Exception raised for validation errors."""
    pass

class ResourceError(Exception):
    """Exception raised for resource-related errors."""
    pass

class NetworkError(Exception):
    """Exception raised for network-related errors."""
    pass

class IntegrationError(Exception):
    """Exception raised for integration-related errors."""
    pass

# Utility functions
def formatNumber(number: float, precision: int = 2) -> str:
    """Format a number with specified precision.
    
    Args:
        number: Number to format
        precision: Number of decimal places
        
    Returns:
        Formatted number string
    """
    format_str = f"{{:.{precision}f}}"
    return format_str.format(number)

def formatTime(time_seconds: float, unit: str = "s") -> str:
    """Format time with appropriate units.
    
    Args:
        time_seconds: Time in seconds
        unit: Unit for display
        
    Returns:
        Formatted time string
    """
    if time_seconds >= 60.0:
        minutes = time_seconds / 60.0
        if minutes >= 60.0:
            hours = minutes / 60.0
            return f"{formatNumber(hours, 1)} h"
        else:
            return f"{formatNumber(minutes, 1)} min"
    else:
        return f"{formatNumber(time_seconds, 3)} {unit}"

def formatBytes(bytes_count: int, unit: str = "B") -> str:
    """Format bytes with appropriate units.
    
    Args:
        bytes_count: Number of bytes
        unit: Unit for display
        
    Returns:
        Formatted bytes string
    """
    if bytes_count >= 1024 * 1024:
        mb = bytes_count / (1024 * 1024)
        return f"{formatNumber(mb, 2)} MB"
    elif bytes_count >= 1024:
        kb = bytes_count / 1024
        return f"{formatNumber(kb, 2)} KB"
    else:
        return f"{bytes_count} {unit}"

def calculateStatistics(data: List[float]) -> Dict[str, float]:
    """Calculate basic statistics for a list of numbers.
    
    Args:
        data: List of numbers
        
    Returns:
        Dictionary with statistics
    """
    if not data:
        return {"count": 0, "mean": 0.0, "std": 0.0, "min": 0.0, "max": 0.0}
    
    import statistics
    
    return {
        "count": len(data),
        "mean": statistics.mean(data),
        "std": statistics.stdev(data) if len(data) > 1 else 0.0,
        "min": min(data),
        "max": max(data),
        "median": statistics.median(data),
        "percentile_25": statistics.quantiles(data, n=4)[0],
        "percentile_75": statistics.quantiles(data, n=4)[2],
    }

def normalizeValue(value: float, min_value: float, max_value: float) -> float:
    """Normalize a value to [0, 1] range.
    
    Args:
        value: Value to normalize
        min_value: Minimum value
        max_value: Maximum value
        
    Returns:
        Normalized value
    """
    if max_value == min_value:
        return 0.5
    
    return (value - min_value) / (max_value - min_value)

def clampValue(value: float, min_value: float, max_value: float) -> float:
    """Clamp a value between min and max.
    
    Args:
        value: Value to clamp
        min_value: Minimum value
        max_value: Maximum value
        
    Returns:
        Clamped value
    """
    return max(min_value, min(max_value, value))

# Configuration validation functions
def validateNeuronCount(count: int) -> bool:
    """Validate neuron count.
    
    Args:
        count: Neuron count to validate
        
    Returns:
        True if valid, False otherwise
    """
    return count > 0 and count <= 100000

def validateSynapseDensity(density: float) -> bool:
    """Validate synapse density.
    
    Args:
        density: Synapse density to validate
        
    Returns:
        True if valid, False otherwise
    """
    return 0.0 <= density <= 1.0

def validateConfigParams(params: Dict[str, Any]) -> List[str]:
    """Validate configuration parameters.
    
    Args:
        params: Configuration parameters to validate
        
    Returns:
        List of validation errors
    """
    errors = []
    
    if "neuron_count" in params:
        count = params["neuron_count"]
        if not isinstance(count, int) or not validateNeuronCount(count):
            errors.append(f"Invalid neuron_count: {count}. Must be between 1 and 100000.")
    
    if "synapse_density" in params:
        density = params["synapse_density"]
        if not isinstance(density, (int, float)) or not validateSynapseDensity(density):
            errors.append(f"Invalid synapse_density: {density}. Must be between 0.0 and 1.0.")
    
    if "learning_rate" in params:
        rate = params["learning_rate"]
        if not isinstance(rate, (int, float)) or rate < 0.0 or rate > 1.0:
            errors.append(f"Invalid learning_rate: {rate}. Must be between 0.0 and 1.0.")
    
    return errors

# Helper classes and functions
class SimpleBrainModel:
    """Simple brain model for demonstration and testing."""
    
    def __init__(self, neuron_count: int = DEFAULT_NEURON_COUNT):
        self.neuron_count = neuron_count
        self.synapses = []
        self.spikes = 0
        self.total_steps = 0
        
    def step(self, step_num: int) -> None:
        """Run one simulation step."""
        self.total_steps += 1
        # Simulate some neural activity
        import random
        spike_probability = 0.01 + (step_num * 0.0001)
        if random.random() < spike_probability:
            self.spikes += 1
    
    def getTotalNeuronCount(self) -> int:
        """Get total neuron count."""
        return self.neuron_count
    
    def getTotalSynapseCount(self) -> int:
        """Get total synapse count."""
        return len(self.synapses)
    
    def getTotalSpikeCount(self) -> int:
        """Get total spike count."""
        return self.spikes
    
    def getFiringNeuronCount(self) -> int:
        """Get firing neuron count."""
        return min(self.neuron_count // 10, 100)
    
    def getAverageFiringRate(self) -> float:
        """Get average firing rate."""
        if self.total_steps == 0:
            return 0.0
        return self.spikes / self.total_steps * 1000.0  # Hz

# Factory functions
class _ConfigFactory:
    """Factory class for creating NLM objects."""
    
    @staticmethod
    def createConfig(preset: str = "default") -> 'Config':
        """Create a configuration with the specified preset."""
        from .nlm_core import Config
        return Config(preset)
    
    @staticmethod
    def createBrain(config: 'Config') -> 'Brain':
        """Create a brain with the specified configuration."""
        from .nlm_core import Brain
        return Brain(config)
    
    @staticmethod
    def createSimpleWorld() -> 'SimpleWorld':
        """Create a simple world."""
        from .nlm_core import SimpleWorld
        return SimpleWorld()
    
    @staticmethod
    def createAgentBrain(brain: 'Brain') -> 'AgentBrain':
        """Create an agent brain interface."""
        from .nlm_core import AgentBrain
        return AgentBrain(brain)

# Create factory instances for easy access
_config_factory = _ConfigFactory()

# Convenience functions
createConfig = _config_factory.createConfig
createBrain = _config_factory.createBrain
createSimpleWorld = _config_factory.createSimpleWorld
createAgentBrain = _config_factory.createAgentBrain

# Configuration preset functions
def getDefaultConfig() -> ConfigDict:
    """Get default configuration."""
    return {
        "neuron_count": DEFAULT_NEURON_COUNT,
        "synapse_density": DEFAULT_SYNAPSE_DENSITY,
        "learning_rate": 0.01,
        "plasticity": True,
        "development": True,
        "curiosity": True,
        "reward_modulation": True,
        "structural_plasticity": True,
        "simulation_timestep": 0.001,
        "max_steps": 1000,
    }

def getDevelopmentConfig() -> ConfigDict:
    """Get development configuration."""
    config = getDefaultConfig()
    config.update({
        "neuron_count": 5000,
        "learning_rate": 0.001,
        "plasticity": True,
        "development": True,
        "curiosity": True,
        "reward_modulation": True,
        "structural_plasticity": True,
    })
    return config

def getSimulationConfig() -> ConfigDict:
    """Get simulation configuration."""
    config = getDefaultConfig()
    config.update({
        "neuron_count": DEFAULT_NEURON_COUNT,
        "learning_rate": 0.01,
        "plasticity": True,
        "development": False,
        "curiosity": False,
        "reward_modulation": True,
        "structural_plasticity": False,
        "simulation_timestep": 0.01,
    })
    return config

def getResearchConfig() -> ConfigDict:
    """Get research configuration."""
    config = getDefaultConfig()
    config.update({
        "neuron_count": 10000,
        "learning_rate": 0.001,
        "plasticity": True,
        "development": True,
        "curiosity": True,
        "reward_modulation": True,
        "structural_plasticity": True,
        "simulation_timestep": 0.0001,
    })
    return config

def getProductionConfig() -> ConfigDict:
    """Get production configuration."""
    config = getSimulationConfig()
    config.update({
        "neuron_count": 1000,
        "learning_rate": 0.1,
        "plasticity": True,
        "development": False,
        "curiosity": False,
        "reward_modulation": True,
        "structural_plasticity": False,
        "simulation_timestep": 0.001,
        "max_steps": 10000,
    })
    return config

# Base classes for future expansion
class BaseObject:
    """Base class for all NLM objects."""
    
    def __init__(self):
        self.id = id(self)
        self.logger = logging.getLogger(self.__class__.__name__)
    
    def log(self, level: str, message: str) -> None:
        """Log a message."""
        getattr(self.logger, level)(message)
    
    def log_info(self, message: str) -> None:
        """Log an info message."""
        self.log("info", message)
    
    def log_warning(self, message: str) -> None:
        """Log a warning message."""
        self.log("warning", message)
    
    def log_error(self, message: str) -> None:
        """Log an error message."""
        self.log("error", message)
    
    def log_debug(self, message: str) -> None:
        """Log a debug message."""
        self.log("debug", message)

class Parameterizable(BaseObject):
    """Mixin class for objects that can be configured."""
    
    def __init__(self):
        super().__init__()
        self.parameters: Dict[str, Any] = {}
    
    def setParameter(self, key: str, value: Any) -> None:
        """Set a parameter."""
        self.parameters[key] = value
        self.log_info(f"Set parameter '{key}' to '{value}'")
    
    def getParameter(self, key: str, default: Any = None) -> Any:
        """Get a parameter."""
        return self.parameters.get(key, default)
    
    def hasParameter(self, key: str) -> bool:
        """Check if a parameter exists."""
        return key in self.parameters
    
    def removeParameter(self, key: str) -> bool:
        """Remove a parameter."""
        if key in self.parameters:
            del self.parameters[key]
            return True
        return False
    
    def clearParameters(self) -> None:
        """Clear all parameters."""
        self.parameters.clear()
        self.log_info("Cleared all parameters")

# Main brain interface (placeholder for now)
class Brain(BaseObject, Parameterizable):
    """Main brain interface for NLM simulations."""
    
    def __init__(self, config: Optional[ConfigDict] = None):
        super().__init__()
        self.config = config or getDefaultConfig()
        self.model = SimpleBrainModel(self.config.get("neuron_count", DEFAULT_NEURON_COUNT))
        
        self.log_info(f"Initialized brain with {self.config.get('neuron_count', DEFAULT_NEURON_COUNT)} neurons")
    
    def initialize(self) -> None:
        """Initialize the brain."""
        self.log_info("Brain initialized")
    
    def step(self, step_num: int, time: Optional[float] = None) -> None:
        """Run one simulation step."""
        if time is None:
            time = step_num * self.config.get("simulation_timestep", 0.001)
        
        self.model.step(step_num)
        self.log_debug(f"Completed step {step_num} at time {time}")
    
    def reset(self) -> None:
        """Reset the brain."""
        self.model = SimpleBrainModel(self.config.get("neuron_count", DEFAULT_NEURON_COUNT))
        self.log_info("Brain reset")
    
    # Statistics methods
    def getTotalNeuronCount(self) -> int:
        """Get total neuron count."""
        return self.model.getTotalNeuronCount()
    
    def getTotalSynapseCount(self) -> int:
        """Get total synapse count."""
        return self.model.getTotalSynapseCount()
    
    def getTotalSpikeCount(self) -> int:
        """Get total spike count."""
        return self.model.getTotalSpikeCount()
    
    def getFiringNeuronCount(self) -> int:
        """Get firing neuron count."""
        return self.model.getFiringNeuronCount()
    
    def getAverageFiringRate(self) -> float:
        """Get average firing rate."""
        return self.model.getAverageFiringRate()
    
    def getExcitationInhibitionRatio(self) -> float:
        """Get excitation/inhibition ratio."""
        return 1.0  # Placeholder
    
    def save(self, filepath: str) -> None:
        """Save brain state to file."""
        import pickle
        
        with open(filepath, 'wb') as f:
            pickle.dump({
                'config': self.config,
                'model': self.model.__dict__
            }, f)
        
        self.log_info(f"Saved brain state to {filepath}")
    
    def load(self, filepath: str) -> None:
        """Load brain state from file."""
        import pickle
        
        with open(filepath, 'rb') as f:
            data = pickle.load(f)
        
        self.config = data['config']
        self.model.__dict__.update(data['model'])
        
        self.log_info(f"Loaded brain state from {filepath}")
    
    def addRegion(self, name: str = "") -> RegionId:
        """Add a new neural region."""
        # Placeholder implementation
        region_id = RegionId(len(self.config.get("regions", [])))
        self.log_info(f"Added region '{name}' with ID {region_id.value}")
        return region_id
    
    def getRegion(self, region_id: RegionId) -> Optional['Region']:
        """Get a region by ID."""
        # Placeholder implementation
        return None
    
    def getRegionCount(self) -> int:
        """Get number of regions."""
        return 0  # Placeholder
    
    def getRegionIds(self) -> List[RegionId]:
        """Get all region IDs."""
        return []  # Placeholder
    
    def getRegions(self) -> List['Region']:
        """Get all regions."""
        return []  # Placeholder
    
    def getTotalNeuronCount(self) -> int:
        """Get total neuron count."""
        return self.model.getTotalNeuronCount()
    
    def getTotalSynapseCount(self) -> int:
        """Get total synapse count."""
        return self.model.getTotalSynapseCount()
    
    def getActiveNeuronCount(self) -> int:
        """Get count of active neurons."""
        return self.model.getFiringNeuronCount()
    
    def getFiringNeuronCount(self) -> int:
        """Get count of currently firing neurons."""
        return self.model.getFiringNeuronCount()
    
    def getDevelopmentalStage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        return DevelopmentalStage.ADULT  # Placeholder
    
    def setDevelopmentalStage(self, stage: DevelopmentalStage) -> None:
        """Set developmental stage."""
        self.log_info(f"Set developmental stage to {stage}")
    
    def getConfig(self) -> 'Config':
        """Get the configuration."""
        return self.config
    
    def logStatus(self) -> None:
        """Log brain status."""
        self.log_info(f"Brain status: {self.getTotalNeuronCount()} neurons, {self.getTotalSynapseCount()} synapses")
        self.log_info(f"Spike count: {self.getTotalSpikeCount()}, Firing rate: {formatNumber(self.getAverageFiringRate(), 2)} Hz")

class Region(BaseObject):
    """Neural region."""
    
    def __init__(self, region_id: RegionId, name: str = ""):
        super().__init__()
        self.id = region_id
        self.name = name
        self.neurons: List['Neuron'] = []
        self.synapses: List['Synapse'] = []
    
    def addNeuron(self, neuron: 'Neuron') -> None:
        """Add a neuron to the region."""
        self.neurons.append(neuron)
    
    def addSynapse(self, synapse: 'Synapse') -> None:
        """Add a synapse to the region."""
        self.synapses.append(synapse)
    
    def getId(self) -> RegionId:
        """Get region ID."""
        return self.id
    
    def getName(self) -> str:
        """Get region name."""
        return self.name
    
    def getNeurons(self) -> List['Neuron']:
        """Get all neurons in the region."""
        return self.neurons
    
    def getSynapses(self) -> List['Synapse']:
        """Get all synapses in the region."""
        return self.synapses

class Neuron(BaseObject):
    """Neuron class."""
    
    def __init__(self, neuron_id: NeuronId, neuron_type: NeuronType = NeuronType.EXCITATORY):
        super().__init__()
        self.id = neuron_id
        self.type = neuron_type
        self.potential = -70.0
        self.threshold = -55.0
        self.spikes = 0
    
    def getId(self) -> NeuronId:
        """Get neuron ID."""
        return self.id
    
    def getType(self) -> NeuronType:
        """Get neuron type."""
        return self.type
    
    def getMembranePotential(self) -> float:
        """Get membrane potential."""
        return self.potential
    
    def setMembranePotential(self, potential: float) -> None:
        """Set membrane potential."""
        self.potential = potential
    
    def isFiring(self) -> bool:
        """Check if neuron is firing."""
        return self.potential >= self.threshold
    
    def step(self, time: float) -> bool:
        """Run one simulation step. Returns True if neuron fired."""
        self.potential += 0.1  # Simple integration
        if self.potential >= self.threshold:
            self.potential = -70.0
            self.spikes += 1
            return True
        return False

class Synapse(BaseObject):
    """Synapse class."""
    
    def __init__(self, synapse_id: SynapseId, source: Neuron, target: Neuron):
        super().__init__()
        self.id = synapse_id
        self.source = source
        self.target = target
        self.weight = 1.0
    
    def getId(self) -> SynapseId:
        """Get synapse ID."""
        return self.id
    
    def getWeight(self) -> float:
        """Get synapse weight."""
        return self.weight
    
    def setWeight(self, weight: float) -> None:
        """Set synapse weight."""
        self.weight = weight

class Config:
    """Configuration class."""
    
    def __init__(self, preset: str = "default"):
        super().__init__()
        self.preset = preset
        self.parameters = self._get_preset(preset)
        self.logger.info(f"Created config with preset '{preset}'")
    
    def _get_preset(self, preset: str) -> ConfigDict:
        """Get preset configuration."""
        presets = {
            "default": getDefaultConfig(),
            "development": getDevelopmentConfig(),
            "simulation": getSimulationConfig(),
            "research": getResearchConfig(),
            "production": getProductionConfig(),
        }
        
        if preset not in presets:
            self.logger.warning(f"Unknown preset '{preset}', using default")
            return getDefaultConfig()
        
        return presets[preset]
    
    def has(self, key: str) -> bool:
        """Check if a configuration key exists."""
        return key in self.parameters
    
    def getKeys(self) -> List[str]:
        """Get all configuration keys."""
        return list(self.parameters.keys())
    
    def set(self, key: str, value: Any) -> None:
        """Set a configuration value."""
        self.parameters[key] = value
        self.logger.info(f"Set config '{key}' to '{value}'")
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value."""
        return self.parameters.get(key, default)
    
    def getOr(self, key: str, default_value: Any) -> Any:
        """Get a configuration value with default."""
        return self.parameters.get(key, default_value)
    
    def summary(self) -> str:
        """Get configuration summary."""
        return f"Config({len(self.parameters)} parameters)"
    
    def __repr__(self) -> str:
        return self.summary()

class SimpleWorld(BaseObject):
    """Simple world simulation."""
    
    def __init__(self):
        super().__init__()
        self.width = 10
        self.height = 10
        self.vision_width = 8
        self.vision_height = 8
        self.sim_time = 0.0
        self.agent_x = 5.0
        self.agent_y = 5.0
        self.agent_orientation = 0.0
        self.agent_energy = 100.0
        self.agent_health = 100.0
        self.agent_age = 0.0
        self.is_moving = False
        self.is_turning = False
        self.last_action_time = 0.0
        
    def configure(self, width: int = 10, height: int = 10, vision_width: int = 8, vision_height: int = 8) -> None:
        """Configure the world."""
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        self.logger.info(f"Configured world: {width}x{height}, vision: {vision_width}x{vision_height}")
    
    def reset(self) -> None:
        """Reset the world."""
        self.sim_time = 0.0
        self.agent_x = self.width / 2.0
        self.agent_y = self.height / 2.0
        self.agent_orientation = 0.0
        self.agent_energy = 100.0
        self.agent_health = 100.0
        self.agent_age = 0.0
        self.is_moving = False
        self.is_turning = False
        self.last_action_time = 0.0
        self.logger.info("World reset")
    
    def setAgentStart(self, x: float, y: float) -> None:
        """Set agent starting position."""
        self.agent_x = x
        self.agent_y = y
        self.logger.info(f"Set agent start position to ({x}, {y})")
    
    def update(self, timestep: float) -> None:
        """Update world state."""
        self.sim_time += timestep
        self.agent_age += timestep
        self.agent_energy *= 0.99  # Energy decay
        if self.agent_energy < 10.0:
            self.agent_energy = 0.0
        
        self.logger.debug(f"World updated: time={formatNumber(self.sim_time, 3)}s, age={formatNumber(self.agent_age, 1)}s")
    
    def applyMotorCommand(self, command: MotorCommand, current_time: float) -> ActionResult:
        """Apply a motor command."""
        result = ActionResult(0.0, False, "")
        
        # Simple movement commands
        if command == MotorCommand.MOVE_FORWARD:
            dx = np.cos(self.agent_orientation)
            dy = np.sin(self.agent_orientation)
            self.agent_x += dx * 0.1
            self.agent_y += dy * 0.1
            self.agent_energy -= 5.0
            self.is_moving = True
            result.reward = 0.1
            result.success = True
            result.message = "Moved forward"
        
        elif command == MotorCommand.TURN_LEFT:
            self.agent_orientation -= np.pi / 4.0
            self.is_turning = True
            self.agent_energy -= 2.0
            result.reward = 0.05
            result.success = True
            result.message = "Turned left"
        
        elif command == MotorCommand.TURN_RIGHT:
            self.agent_orientation += np.pi / 4.0
            self.is_turning = True
            self.agent_energy -= 2.0
            result.reward = 0.05
            result.success = True
            result.message = "Turned right"
        
        elif command == MotorCommand.INTERACT:
            self.agent_energy -= 10.0
            result.reward = 1.0
            result.success = True
            result.message = "Interacted with environment"
        
        elif command == MotorCommand.WAIT:
            result.reward = 0.0
            result.success = True
            result.message = "Waited"
        
        self.last_action_time = current_time
        self.logger.info(f"Applied command {command}: {result.message}")
        return result
    
    def getSensoryPercept(self) -> 'SensoryPercept':
        """Get sensory percept from world."""
        return SensoryPercept(self)
    
    def getAgentBody(self) -> 'AgentBody':
        """Get agent body state."""
        return AgentBody(self)
    
    # World properties
    def getWidth(self) -> int:
        """Get world width."""
        return self.width
    
    def getHeight(self) -> int:
        """Get world height."""
        return self.height
    
    def getMaxEnergy(self) -> float:
        """Get maximum energy."""
        return 100.0
    
    def setMaxEnergy(self, energy: float) -> None:
        """Set maximum energy."""
        pass  # Placeholder
    
    def getEnergyDecayRate(self) -> float:
        """Get energy decay rate."""
        return 0.01
    
    def setEnergyDecayRate(self, rate: float) -> None:
        """Set energy decay rate."""
        pass  # Placeholder
    
    def getSimulationTime(self) -> float:
        """Get simulation time."""
        return self.sim_time
    
    def setRandomSeed(self, seed: int) -> None:
        """Set random seed."""
        import random
        random.seed(seed)
    
    def getRandomSeed(self) -> int:
        """Get random seed."""
        import random
        return int(random.random() * 1000000)
    
    def addObject(self, obj: 'WorldObject') -> None:
        """Add a world object."""
        # Placeholder
        pass
    
    def removeObject(self, x: float, y: float) -> None:
        """Remove a world object."""
        # Placeholder
        pass
    
    def isValidPosition(self, x: float, y: float) -> bool:
        """Check if position is valid."""
        return 0 <= x <= self.width and 0 <= y <= self.height

class SensoryPercept(BaseObject):
    """Sensory percept data."""
    
    def __init__(self, world: Optional[SimpleWorld] = None):
        super().__init__()
        self.world = world
        self.vision = Vision(0, 0, 0) if world else Vision()
        self.touch = []
        self.internal = []
        self.proprioception = []
        self.audio = Audio() if world else Audio()
    
    def getVision(self) -> Vision:
        """Get vision data."""
        return self.vision
    
    def setVision(self, vision: Vision) -> None:
        """Set vision data."""
        self.vision = vision
    
    def getVisionWidth(self) -> int:
        """Get vision width."""
        return self.vision.getWidth() if self.vision else 0
    
    def getVisionHeight(self) -> int:
        """Get vision height."""
        return self.vision.getHeight() if self.vision else 0
    
    def getTouch(self) -> List[float]:
        """Get touch data."""
        return self.touch
    
    def setTouch(self, touch: List[float]) -> None:
        """Set touch data."""
        self.touch = touch
    
    def getInternal(self) -> List[float]:
        """Get internal data."""
        return self.internal
    
    def setInternal(self, internal: List[float]) -> None:
        """Set internal data."""
        self.internal = internal
    
    def getProprioception(self) -> List[float]:
        """Get proprioception data."""
        return self.proprioception
    
    def setProprioception(self, proprioception: List[float]) -> None:
        """Set proprioception data."""
        self.proprioception = proprioception
    
    def getAudio(self) -> Audio:
        """Get audio data."""
        return self.audio
    
    def setAudio(self, audio: Audio) -> None:
        """Set audio data."""
        self.audio = audio
    
    def getAllSignals(self) -> Dict[str, Any]:
        """Get all sensory signals."""
        return {
            "vision": self.vision,
            "touch": self.touch,
            "internal": self.internal,
            "proprioception": self.proprioception,
            "audio": self.audio,
        }
    
    def getTimestamp(self) -> float:
        """Get timestamp."""
        return self.world.getSimulationTime() if self.world else 0.0
    
    def setTimestamp(self, timestamp: float) -> None:
        """Set timestamp."""
        pass  # Placeholder

class Vision(BaseObject):
    """Vision sensory input."""
    
    def __init__(self, width: int = 0, height: int = 0, channels: int = 3):
        super().__init__()
        self.width = width
        self.height = height
        self.channels = channels
        self.data: List[float] = []
    
    def setData(self, data: List[float]) -> None:
        """Set vision data."""
        self.data = data
        expected_size = self.width * self.height * self.channels
        if len(data) != expected_size:
            self.log_warning(f"Vision data size mismatch: expected {expected_size}, got {len(data)}")
    
    def getWidth(self) -> int:
        """Get vision width."""
        return self.width
    
    def getHeight(self) -> int:
        """Get vision height."""
        return self.height
    
    def getChannels(self) -> int:
        """Get vision channels."""
        return self.channels

class Audio(BaseObject):
    """Audio sensory input."""
    
    def __init__(self, sample_rate: int = 0, num_samples: int = 0):
        super().__init__()
        self.sample_rate = sample_rate
        self.num_samples = num_samples
        self.data: List[float] = []
    
    def setData(self, data: List[float]) -> None:
        """Set audio data."""
        self.data = data
        if len(data) != self.num_samples:
            self.log_warning(f"Audio data size mismatch: expected {self.num_samples}, got {len(data)}")
    
    def setSampleRate(self, sample_rate: int) -> None:
        """Set sample rate."""
        self.sample_rate = sample_rate
    
    def getSampleRate(self) -> int:
        """Get sample rate."""
        return self.sample_rate
    
    def getNumSamples(self) -> int:
        """Get number of samples."""
        return self.num_samples

class InternalSignals(BaseObject):
    """Internal signals sensory input."""
    
    def __init__(self):
        super().__init__()
        self.signals: List[float] = []
    
    def addSignal(self, value: float) -> None:
        """Add a signal."""
        self.signals.append(value)
    
    def clearSignals(self) -> None:
        """Clear all signals."""
        self.signals.clear()

class Action(BaseObject):
    """Action representation for motor output."""
    
    def __init__(self, action_type: ActionType = ActionType.WAIT, parameters: Optional[List[float]] = None):
        super().__init__()
        self.type = action_type
        self.parameters = parameters or []
    
    def getType(self) -> ActionType:
        """Get action type."""
        return self.type
    
    def setType(self, action_type: ActionType) -> None:
        """Set action type."""
        self.type = action_type
    
    def getParameters(self) -> List[float]:
        """Get action parameters."""
        return self.parameters
    
    def setParameters(self, parameters: List[float]) -> None:
        """Set action parameters."""
        self.parameters = parameters
    
    def getName(self) -> str:
        """Get action name."""
        return self.type.value
    
    def clone(self) -> 'Action':
        """Clone this action."""
        return Action(self.type, self.parameters.copy())

class WorldObject(BaseObject):
    """World object representation."""
    
    def __init__(self, x: float = 0.0, y: float = 0.0, obj_type: WorldObjectType = WorldObjectType.EMPTY, value: float = 0.0, radius: float = 0.5):
        super().__init__()
        self.x = x
        self.y = y
        self.type = obj_type
        self.value = value
        self.radius = radius
        self.active = True
    
    # Properties
    @property
    def x(self) -> float:
        """Get x coordinate."""
        return self._x
    
    @x.setter
    def x(self, value: float) -> None:
        """Set x coordinate."""
        self._x = value
    
    @property
    def y(self) -> float:
        """Get y coordinate."""
        return self._y
    
    @y.setter
    def y(self, value: float) -> None:
        """Set y coordinate."""
        self._y = value
    
    @property
    def type(self) -> WorldObjectType:
        """Get object type."""
        return self._type
    
    @type.setter
    def type(self, value: WorldObjectType) -> None:
        """Set object type."""
        self._type = value
    
    @property
    def value(self) -> float:
        """Get object value."""
        return self._value
    
    @value.setter
    def value(self, value: float) -> None:
        """Set object value."""
        self._value = value
    
    @property
    def radius(self) -> float:
        """Get object radius."""
        return self._radius
    
    @radius.setter
    def radius(self, value: float) -> None:
        """Set object radius."""
        self._radius = value
    
    @property
    def active(self) -> bool:
        """Get object active status."""
        return self._active
    
    @active.setter
    def active(self, value: bool) -> None:
        """Set object active status."""
        self._active = value

class AgentBody(BaseObject):
    """Agent body state."""
    
    def __init__(self):
        super().__init__()
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocity_x = 0.0
        self.velocity_y = 0.0
        self.angular_velocity = 0.0
        self.energy = 100.0
        self.health = 100.0
        self.age = 0.0
        self.is_moving = False
        self.is_turning = False
        self.last_action_time = 0.0
    
    def reset(self) -> None:
        """Reset agent body."""
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocity_x = 0.0
        self.velocity_y = 0.0
        self.angular_velocity = 0.0
        self.energy = 100.0
        self.health = 100.0
        self.age = 0.0
        self.is_moving = False
        self.is_turning = False
        self.last_action_time = 0.0

class ActionResult(BaseObject):
    """Action result from world."""
    
    def __init__(self, reward: float = 0.0, success: bool = False, message: str = ""):
        super().__init__()
        self.reward = reward
        self.success = success
        self.message = message

class AgentBrain(BaseObject):
    """Agent brain interface connecting NLM brain to world."""
    
    def __init__(self, brain: Brain):
        super().__init__()
        self.brain = brain
        self.world: Optional[SimpleWorld] = None
        self.logger.info("Created AgentBrain interface")
    
    def initialize(self, world: SimpleWorld) -> None:
        """Initialize with world."""
        self.world = world
        self.logger.info("AgentBrain initialized with world")
    
    def getSensoryInputSize(self) -> int:
        """Get expected sensory input size."""
        if not self.world:
            return 0
        return (self.world.getVisionWidth() * self.world.getVisionHeight() +
                8 +  # touch
                4 +  # internal
                6)   # proprioception
    
    def getMotorOutputSize(self) -> int:
        """Get expected motor output size."""
        return 6  # One motor neuron per action
    
    def processSensoryInput(self, percept: SensoryPercept) -> None:
        """Process sensory percept and inject into brain."""
        if not self.brain:
            return
        
        # Inject vision data
        vision = percept.getVision()
        for i in range(min(vision.getWidth() * vision.getHeight(), self.brain.getTotalNeuronCount() // 4)):
            self.brain.step(i)
        
        self.logger.debug("Processed sensory input")
    
    def decodeMotorCommand(self) -> MotorCommand:
        """Decode brain motor activity into motor command."""
        # Simple decoding based on brain activity
        return MotorCommand.WAIT
    
    def applyRewardModulation(self, reward: float, predicted_reward: float) -> None:
        """Apply reward-based neuromodulation."""
        # Placeholder for reward modulation
        self.logger.debug(f"Applied reward modulation: {reward}, predicted: {predicted_reward}")
    
    def updateDevelopment(self, timestep: float) -> None:
        """Update development system."""
        # Placeholder for development update
        self.logger.debug(f"Updated development: timestep {timestep}")
    
    def getDevelopmentalStage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        return self.brain.getDevelopmentalStage()
    
    def getNeuromodulationLevel(self) -> float:
        """Get current neuromodulation level."""
        return 0.5  # Placeholder
    
    def getCuriosityLevel(self) -> float:
        """Get curiosity level."""
        return 0.5  # Placeholder
    
    def getNoveltyLevel(self) -> float:
        """Get novelty level."""
        return 0.5  # Placeholder
    
    def getPredictionError(self) -> float:
        """Get prediction error."""
        return 0.0  # Placeholder
    
    def reset(self) -> None:
        """Reset agent for new episode."""
        self.brain.reset()
        self.logger.info("AgentBrain reset")
    
    def getBrain(self) -> Brain:
        """Get the underlying brain."""
        return self.brain

# Config factory functions
def createConfig(preset: str = "default") -> Config:
    """Create a configuration with the specified preset."""
    return Config(preset)

def createBrain(config: Config) -> Brain:
    """Create a brain with the specified configuration."""
    return Brain(config.getParameters())

def createSimpleWorld() -> SimpleWorld:
    """Create a simple world."""
    return SimpleWorld()

def createAgentBrain(brain: Brain) -> AgentBrain:
    """Create an agent brain interface."""
    return AgentBrain(brain)

def createVision(width: int = 0, height: int = 0, channels: int = 3) -> Vision:
    """Create a vision sensory input."""
    return Vision(width, height, channels)

def createAudio(sample_rate: int = 0, num_samples: int = 0) -> Audio:
    """Create an audio sensory input."""
    return Audio(sample_rate, num_samples)

def createInternalSignals() -> InternalSignals:
    """Create internal signals sensory input."""
    return InternalSignals()

def createAction(action_type: ActionType = ActionType.WAIT, parameters: Optional[List[float]] = None) -> Action:
    """Create an action representation."""
    return Action(action_type, parameters)

def createWorldObject(x: float = 0.0, y: float = 0.0, obj_type: WorldObjectType = WorldObjectType.EMPTY, value: float = 0.0, radius: float = 0.5) -> WorldObject:
    """Create a world object."""
    return WorldObject(x, y, obj_type, value, radius)

def createAgentBody() -> AgentBody:
    """Create an agent body state."""
    return AgentBody()

def createActionResult(reward: float = 0.0, success: bool = False, message: str = "") -> ActionResult:
    """Create an action result."""
    return ActionResult(reward, success, message)

def createSensoryPercept(world: Optional[SimpleWorld] = None) -> SensoryPercept:
    """Create a sensory percept."""
    return SensoryPercept(world)

# Configuration preset functions
def getDefaultConfig() -> ConfigDict:
    """Get default configuration."""
    return {
        "neuron_count": DEFAULT_NEURON_COUNT,
        "synapse_density": DEFAULT_SYNAPSE_DENSITY,
        "learning_rate": 0.01,
        "plasticity": True,
        "development": True,
        "curiosity": True,
        "reward_modulation": True,
        "structural_plasticity": True,
        "simulation_timestep": 0.001,
        "max_steps": 1000,
    }

def getDevelopmentConfig() -> ConfigDict:
    """Get development configuration."""
    config = getDefaultConfig()
    config.update({
        "neuron_count": 5000,
        "learning_rate": 0.001,
        "plasticity": True,
        "development": True,
        "curiosity": True,
        "reward_modulation": True,
        "structural_plasticity": True,
    })
    return config

def getSimulationConfig() -> ConfigDict:
    """Get simulation configuration."""
    config = getDefaultConfig()
    config.update({
        "neuron_count": DEFAULT_NEURON_COUNT,
        "learning_rate": 0.01,
        "plasticity": True,
        "development": False,
        "curiosity": False,
        "reward_modulation": True,
        "structural_plasticity": False,
        "simulation_timestep": 0.01,
    })
    return config

def getResearchConfig() -> ConfigDict:
    """Get research configuration."""
    config = getDefaultConfig()
    config.update({
        "neuron_count": 10000,
        "learning_rate": 0.001,
        "plasticity": True,
        "development": True,
        "curiosity": True,
        "reward_modulation": True,
        "structural_plasticity": True,
        "simulation_timestep": 0.0001,
    })
    return config

def getProductionConfig() -> ConfigDict:
    """Get production configuration."""
    config = getSimulationConfig()
    config.update({
        "neuron_count": 1000,
        "learning_rate": 0.1,
        "plasticity": True,
        "development": False,
        "curiosity": False,
        "reward_modulation": True,
        "structural_plasticity": False,
        "simulation_timestep": 0.001,
        "max_steps": 10000,
    })
    return config
"""

This is the core implementation of the NLM Python API. It includes:

1. **Data classes and enumerations**: NeuronId, SynapseId, NeuronType, etc.
2. **Exception classes**: ConfigurationError, SimulationError, ValidationError, etc.
3. **Utility functions**: formatNumber, formatTime, formatBytes, calculateStatistics, etc.
4. **Main classes**: Brain, Config, SimpleWorld, Vision, Audio, Action, etc.
5. **Factory functions**: createConfig, createBrain, createSimpleWorld, etc.
6. **Configuration presets**: getDefaultConfig, getDevelopmentConfig, etc.
7. **Base classes**: BaseObject, Parameterizable

This provides the foundation for the NLM Python API with:
- Complete type annotations
- Comprehensive documentation
- Error handling
- Configuration management
- Simulation capabilities
"""

# Fix for __all__ error - ensure all names are defined before use
__all__ = [
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    'ConfigurationError', 'SimulationError', 'ValidationError', 'ResourceError', 'NetworkError', 'IntegrationError',
    'formatNumber', 'formatTime', 'formatBytes', 'calculateStatistics', 'normalizeValue', 'clampValue',
    'validateNeuronCount', 'validateSynapseDensity', 'validateConfigParams',
    'SimpleBrainModel',
    '_ConfigFactory',
    'createConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    'getDefaultConfig', 'getDevelopmentConfig', 'getSimulationConfig', 'getResearchConfig', 'getProductionConfig',
    'BaseObject', 'Parameterizable',
    'Brain', 'Region', 'Neuron', 'Synapse', 'Config', 'SimpleWorld',
    'SensoryPercept', 'Vision', 'Audio', 'InternalSignals', 'Action', 'WorldObject', 'AgentBody', 'ActionResult', 'AgentBrain',
    'PI', 'E', 'DEFAULT_NEURON_COUNT', 'DEFAULT_SYNAPSE_DENSITY', 'MAX_REPRESENTATION_DEPTH',
]

# Set up module logger
logger = logging.getLogger(__name__)