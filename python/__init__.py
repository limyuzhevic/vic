# NLM Python Package - Main Entry Point

"""
NLM (Neural Learning Machine) - A Python interface for the NLM computational brain

This package provides a user-friendly Python API for working with the NLM framework,
which implements an experimental artificial developmental brain with neurons, synapses,
memory, neuromodulation, and learning capabilities.

The package includes:
- High-level brain management and configuration
- Environment and world simulation
- Agent interfaces for perception and action
- Learning and development utilities
- Visualization and analysis tools
"""

import sys
from typing import Dict, List, Optional, Union, Any
from pathlib import Path

# Import C++ bindings
import pynlm

class Config:
    """
    Configuration manager for NLM brain simulation
    
    This class provides a Python-friendly interface for managing
    NLM brain configuration parameters, including preset configurations
    and validation.
    """
    
    def __init__(self):
        """Create a new configuration"""
        self._cpp_config = pynlm.createDefaultConfig()
    
    def loadFromFile(self, filepath: Union[str, Path]) -> bool:
        """Load configuration from a file"""
        return self._cpp_config.loadFromFile(str(filepath))
    
    def saveToFile(self, filepath: Union[str, Path]) -> bool:
        """Save configuration to a file"""
        return self._cpp_config.saveToFile(str(filepath))
    
    def has(self, key: str) -> bool:
        """Check if a configuration key exists"""
        return self._cpp_config.has(key)
    
    def getKeys(self) -> List[str]:
        """Get all configuration keys"""
        return self._cpp_config.getKeys()
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get a configuration value with type conversion
        
        Supports common type conversions:
        - int for integer values
        - float for float values  
        - str for string values
        """
        if not self.has(key):
            return default
        
        # Try to determine the type and get appropriate value
        if default is None:
            return default
        
        # Use C++ config's getOr which has type-safe access
        if isinstance(default, int):
            return self._cpp_config.getOr(int, key, default)
        elif isinstance(default, float):
            return self._cpp_config.getOr(float, key, default)
        elif isinstance(default, str):
            return self._cpp_config.getOr(str, key, default)
        elif isinstance(default, bool):
            # bool is tricky in C++ config, use int as bool
            return bool(self._cpp_config.getOr(int, key, int(default)))
        else:
            return default
    
    def set(self, key: str, value: Any) -> None:
        """
        Set a configuration value
        
        Automatically converts Python types to appropriate C++ types
        """
        # Use appropriate config.set method based on type
        if isinstance(value, int):
            self._cpp_config.setInt(key, value)
        elif isinstance(value, float):
            self._cpp_config.setFloat(key, value)
        elif isinstance(value, str):
            self._cpp_config.setString(key, value)
        elif isinstance(value, bool):
            self._cpp_config.setBool(key, value)
        else:
            raise ValueError(f"Unsupported configuration type: {type(value)}")
    
    def getOr(self, key: str, default: Any = None) -> Any:
        """Get value with default, alias for get()"""
        return self.get(key, default)
    
    def summary(self) -> str:
        """Get configuration summary"""
        return self._cpp_config.summary()
    
    def __repr__(self) -> str:
        return f"Config({len(self.getKeys())} keys)"
    
    def __str__(self) -> str:
        return self.summary()


def createDefaultConfig() -> Config:
    """Create a default configuration"""
    return Config()


class Brain:
    """
    Main brain simulation class
    
    Represents a spiking neural network with neurons, synapses, memory systems,
    neuromodulation, and learning capabilities.
    """
    
    def __init__(self, config: Optional[Config] = None):
        """Create a new brain with the given configuration"""
        if config is None:
            config = createDefaultConfig()
        
        # Create C++ brain
        self._cpp_brain = pynlm.createBrain(config._cpp_config)
        
        # Store reference to config
        self._config = config
    
    def initialize(self) -> None:
        """Initialize the brain with configuration"""
        return self._cpp_brain.initialize()
    
    def step(self, currentStep: int, currentTime: Optional[float] = None) -> None:
        """
        Perform a simulation step
        
        Args:
            currentStep: Current simulation step number
            currentTime: Current simulation time (if None, calculated from timestep)
        """
        if currentTime is not None:
            return self._cpp_brain.step(currentStep, currentTime)
        else:
            return self._cpp_brain.step(currentStep)
    
    def receiveSensoryInput(self, input: Any) -> None:
        """Inject sensory input into the brain"""
        return self._cpp_brain.receiveSensoryInput(input)
    
    def injectCurrent(self, neuron: Any, current: float) -> None:
        """Inject current into a specific neuron"""
        return self._cpp_brain.injectCurrent(neuron, current)
    
    def injectCurrentToNeurons(self, neuronType: Any, current: float) -> None:
        """Inject current into all neurons of a specific type"""
        return self._cpp_brain.injectCurrentToNeurons(neuronType, current)
    
    def produceAction(self) -> Any:
        """Produce motor action based on neural activity"""
        return self._cpp_brain.produceAction()
    
    def reset(self) -> None:
        """Reset brain state"""
        return self._cpp_brain.reset()
    
    def save(self, filepath: Union[str, Path]) -> None:
        """Save brain state to file"""
        return self._cpp_brain.save(str(filepath))
    
    def load(self, filepath: Union[str, Path]) -> None:
        """Load brain state from file"""
        return self._cpp_brain.load(str(filepath))
    
    # Statistics methods
    def getTotalNeuronCount(self) -> int:
        """Get total neuron count across all regions"""
        return self._cpp_brain.getTotalNeuronCount()
    
    def getTotalSynapseCount(self) -> int:
        """Get total synapse count across all regions"""
        return self._cpp_brain.getTotalSynapseCount()
    
    def getActiveNeuronCount(self) -> int:
        """Get count of active neurons"""
        return self._cpp_brain.getActiveNeuronCount()
    
    def getFiringNeuronCount(self) -> int:
        """Get count of currently firing neurons"""
        return self._cpp_brain.getFiringNeuronCount()
    
    def getAverageFiringRate(self) -> float:
        """Get average firing rate across all neurons"""
        return self._cpp_brain.getAverageFiringRate()
    
    def getExcitationInhibitionRatio(self) -> float:
        """Get excitation/inhibition balance ratio"""
        return self._cpp_brain.getExcitationInhibitionRatio()
    
    def getTotalSpikeCount(self) -> int:
        """Get total spike count"""
        return self._cpp_brain.getTotalSpikeCount()
    
    def getDevelopmentalStage(self) -> Any:
        """Get current developmental stage"""
        return self._cpp_brain.getDevelopmentalStage()
    
    def setDevelopmentalStage(self, stage: Any) -> None:
        """Set developmental stage"""
        return self._cpp_brain.setDevelopmentalStage(stage)
    
    def getConfig(self) -> Config:
        """Get the configuration"""
        return self._config
    
    def logStatus(self) -> None:
        """Log brain status"""
        return self._cpp_brain.logStatus()
    
    def __repr__(self) -> str:
        return f"Brain({self.getTotalNeuronCount()} neurons, {self.getTotalSynapseCount()} synapses)"


def createBrain(config: Optional[Config] = None) -> Brain:
    """Create a new brain with the given configuration"""
    return Brain(config)


def getVersion() -> str:
    """Get NLM version"""
    return pynlm.__version__


def getInfo() -> Dict[str, str]:
    """Get NLM package information"""
    return {
        "version": getVersion(),
        "author": "NLM Research Team",
        "license": "MIT",
        "description": "Neural Learning Machine - Computational Brain Simulator"
    }


# Re-export commonly used types and classes
__all__ = [
    "Config", "createDefaultConfig", "createBrain", "Brain",
    "getVersion", "getInfo"
]

# Version
__version__ = "6.0.0"
__author__ = "NLM Research Team"
__license__ = "MIT"

print(f"NLM Python Package v{__version__} loaded successfully")
print("Type 'help(pynlm)' for detailed documentation")
