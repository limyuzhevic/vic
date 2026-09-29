"""
Python-idiomatic interfaces for NLM classes.

This module provides enhanced Python interfaces for NLM classes including
property-style access, context manager support, iterable support, and
enhanced operator overloading.
"""

import pybind11
import numpy as np
from typing import Any, Dict, List, Optional, Union, Tuple, Iterator
from enum import Enum

# Import existing classes and converters
from .type_converter import TypeConverter, NumpyConverter, EnumConverter
from .exceptions import ValidationError
class PythonicConfig:
    """
    Pythonic interface for Config class with enhanced accessors.
    
    Provides property-style access, dictionary-like interface,
    and context manager support for configuration operations.
    """
    
    def __init__(self, config_obj):
        """
        Initialize with C++ Config object.
        
        Args:
            config_obj: C++ Config object
        """
        self._config = config_obj
        self._lazy_validation = False
    
    @property
    def config(self):
        """Get the underlying C++ config object"""
        return self._config
    
    # Property-style access for commonly used attributes
    @property
    def keys(self) -> List[str]:
        """Get all configuration keys (property-style access)"""
        return self._config.getKeys() if hasattr(self._config, 'getKeys') else list()
    
    @property
    def values(self) -> Dict[str, Any]:
        """Get all configuration values as dictionary (property-style access)"""
        result = {}
        for key in self.keys:
            try:
                result[key] = self._config.get(key) if hasattr(self._config, 'get') else getattr(self._config, key)
            except Exception:
                result[key] = None
        return result
    
    @property
    def has(self) -> callable:
        """Return a function to check if a key exists"""
        return lambda key: self._config.has(key) if hasattr(self._config, 'has') else False
    
    # Dictionary-like interface
    def __getitem__(self, key):
        """Get configuration value using dictionary-style access"""
        if key in self.keys:
            return self._config.get(key) if hasattr(self._config, 'get') else getattr(self._config, key)
        raise KeyError(f"Configuration key '{key}' not found")
    
    def __setitem__(self, key, value):
        """Set configuration value using dictionary-style access"""
        if hasattr(self._config, 'set'):
            self._config.set(key, value)
        else:
            setattr(self._config, key, value)
    
    def __delitem__(self, key):
        """Delete configuration value using dictionary-style access"""
        if hasattr(self._config, 'remove'):
            self._config.remove(key)
        elif hasattr(self._config, key):
            delattr(self._config, key)
        else:
            raise KeyError(f"Configuration key '{key}' not found")
    
    def __contains__(self, key):
        """Check if key exists using 'in' operator"""
        return key in self.keys
    
    def __len__(self):
        """Get number of configuration entries"""
        return len(self.keys)
    
    def __iter__(self):
        """Iterate over configuration keys"""
        return iter(self.keys)
    
    def __repr__(self):
        """String representation of config"""
        return f"<PythonicConfig: {self._config.summary() if hasattr(self._config, 'summary') else ''}>"
    
    def __str__(self):
        """String representation"""
        return repr(self)
    
    # Enhanced methods
    def get(self, key, default=None):
        """Get value with default (mimics dict.get)"""
        try:
            return self[key]
        except KeyError:
            return default
    
    def update(self, other_dict: Dict[str, Any]) -> None:
        """Update config from dictionary"""
        for key, value in other_dict.items():
            self[key] = value
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to Python dictionary"""
        return self.values
    
    @classmethod
    def from_dict(cls, config_dict: Dict[str, Any]):
        """Create from dictionary (class method)"""
        from .type_converter import ConfigConverter
        # This would need to create a C++ Config object
        pass
    
    def save_to_dict(self, filepath: str) -> None:
        """Save configuration to dictionary (placeholder)"""
        import json
        with open(filepath, 'w') as f:
            json.dump(self.to_dict(), f, indent=2)
    
    def load_from_dict(self, filepath: str) -> None:
        """Load configuration from dictionary (placeholder)"""
        import json
        with open(filepath, 'r') as f:
            config_dict = json.load(f)
        self.update(config_dict)
    
    # Context manager support
    def __enter__(self):
        """Enter context manager"""
        if hasattr(self._config, 'lock') or hasattr(self._config, 'acquire'):
            if hasattr(self._config, 'lock'):
                self._config.lock()
            else:
                self._config.acquire()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit context manager"""
        if hasattr(self._config, 'unlock') or hasattr(self._config, 'release'):
            if hasattr(self._config, 'unlock'):
                self._config.unlock()
            else:
                self._config.release()
    
    # Enhanced operator overloading
    def __add__(self, other):
        """Merge with another config (placeholder)"""
        if isinstance(other, PythonicConfig):
            merged = PythonicConfig(self._config)
            merged.update(other.to_dict())
            return merged
        raise TypeError(f"Cannot add PythonicConfig with {type(other)}")
    
    def __iadd__(self, other):
        """In-place addition (merge)"""
        if isinstance(other, PythonicConfig):
            self.update(other.to_dict())
            return self
        raise TypeError(f"Cannot add PythonicConfig with {type(other)}")
    
    def __eq__(self, other):
        """Equality comparison"""
        if isinstance(other, PythonicConfig):
            return self.to_dict() == other.to_dict()
        return False
    
    def __ne__(self, other):
        """Inequality comparison"""
        return not self.__eq__(other)
class PythonicSensoryInput:
    """
    Pythonic interface for SensoryInput class.
    
    Provides property-style access, NumPy integration,
    and enhanced data handling.
    """
    
    def __init__(self, sensory_obj):
        """
        Initialize with C++ SensoryInput object.
        
        Args:
            sensory_obj: C++ SensoryInput object
        """
        self._sensory = sensory_obj
    
    @property
    def sensory(self):
        """Get the underlying C++ sensory object"""
        return self._sensory
    
    # Property-style access
    @property
    def type(self) -> str:
        """Get sensory type (property-style)"""
        return self._sensory.getType() if hasattr(self._sensory, 'getType') else 'Unknown'
    
    @property
    def dimensions(self) -> int:
        """Get dimensionality (property-style)"""
        return self._sensory.getDimensions() if hasattr(self._sensory, 'getDimensions') else 0
    
    @property
    def timestamp(self) -> float:
        """Get timestamp (property-style)"""
        return self._sensory.getTimestamp() if hasattr(self._sensory, 'getTimestamp') else 0.0
    
    @timestamp.setter
    def timestamp(self, value: float):
        """Set timestamp (property-style)"""
        if hasattr(self._sensory, 'setTimestamp'):
            self._sensory.setTimestamp(value)
    
    # NumPy integration for data
    @property
    def data(self) -> np.ndarray:
        """Get data as NumPy array"""
        raw_data = self._sensory.getData() if hasattr(self._sensory, 'getData') else []
        return NumpyConverter.to_numpy(raw_data)
    
    @data.setter
    def data(self, value: Union[List[float], np.ndarray]):
        """Set data from NumPy array or list"""
        if isinstance(value, np.ndarray):
            data_list = value.flatten().tolist()
        else:
            data_list = value
        
        if hasattr(self._sensory, 'setData'):
            self._sensory.setData(data_list)
    
    # Context manager support
    def __enter__(self):
        """Enter context manager"""
        if hasattr(self._sensory, 'lock') or hasattr(self._sensory, 'acquire'):
            if hasattr(self._sensory, 'lock'):
                self._sensory.lock()
            else:
                self._sensory.acquire()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit context manager"""
        if hasattr(self._sensory, 'unlock') or hasattr(self._sensory, 'release'):
            if hasattr(self._sensory, 'unlock'):
                self._sensory.unlock()
            else:
                self._sensory.release()
    
    # Enhanced data access
    def get_data(self, as_numpy: bool = True) -> Union[List[float], np.ndarray]:
        """
        Get data with option for NumPy array.
        
        Args:
            as_numpy: If True, return NumPy array; otherwise return list
            
        Returns:
            Data as NumPy array or list
        """
        raw_data = self._sensory.getData() if hasattr(self._sensory, 'getData') else []
        
        if as_numpy:
            return NumpyConverter.to_numpy(raw_data)
        else:
            return raw_data
    
    def set_data(self, data: Union[List[float], np.ndarray]) -> None:
        """
        Set data from list or NumPy array.
        
        Args:
            data: Data to set (list or NumPy array)
        """
        if isinstance(data, np.ndarray):
            data_list = data.flatten().tolist()
        else:
            data_list = data
        
        if hasattr(self._sensory, 'setData'):
            self._sensory.setData(data_list)
    
    def __repr__(self):
        """String representation"""
        return f"<PythonicSensoryInput type='{self.type}' dimensions={self.dimensions}>"
class PythonicAction:
    """
    Pythonic interface for Action class.
    
    Provides property-style access, enum conversions,
    and enhanced operator support.
    """
    
    def __init__(self, action_obj):
        """
        Initialize with C++ Action object.
        
        Args:
            action_obj: C++ Action object
        """
        self._action = action_obj
    
    @property
    def action(self):
        """Get the underlying C++ action object"""
        return self._action
    
    # Property-style access
    @property
    def action_type(self) -> Any:
        """Get action type (property-style)"""
        return self._action.getType() if hasattr(self._action, 'getType') else None
    
    @action_type.setter
    def action_type(self, value: Any):
        """Set action type (property-style)"""
        if hasattr(self._action, 'setType'):
            self._action.setType(value)
    
    @property
    def parameters(self) -> List[float]:
        """Get parameters (property-style)"""
        return self._action.getParameters() if hasattr(self._action, 'getParameters') else []
    
    @parameters.setter
    def parameters(self, value: List[float]):
        """Set parameters (property-style)"""
        if hasattr(self._action, 'setParameters'):
            self._action.setParameters(value)
    
    @property
    def name(self) -> str:
        """Get action name (property-style)"""
        return self._action.getName() if hasattr(self._action, 'getName') else "Unknown"
    
    # Enum conversions
    @property
    def type_string(self) -> str:
        """Get type as string"""
        return EnumConverter.to_string(self.action_type) if self.action_type else ""
    
    # Enhanced methods
    def get_name(self) -> str:
        """Get action name (alternative method)"""
        return self.name
    
    def clone(self) -> 'PythonicAction':
        """Create a copy of this action"""
        if hasattr(self._action, 'clone'):
            cloned_obj = self._action.clone()
            return PythonicAction(cloned_obj)
        raise RuntimeError("Action object does not support cloning")
    
    def update(self, **kwargs) -> None:
        """Update action properties using keyword arguments"""
        for key, value in kwargs.items():
            if key == 'type' or key == 'action_type':
                self.action_type = value
            elif key == 'parameters':
                self.parameters = value
            elif key == 'name':
                # Name is read-only
                pass
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary representation"""
        return {
            'type': self.type_string,
            'parameters': self.parameters,
            'name': self.name
        }
    
    @classmethod
    def from_dict(cls, action_dict: Dict[str, Any]) -> 'PythonicAction':
        """Create from dictionary"""
        # This would need to create a C++ Action object
        pass
    
    # Context manager support
    def __enter__(self):
        """Enter context manager"""
        if hasattr(self._action, 'lock') or hasattr(self._action, 'acquire'):
            if hasattr(self._action, 'lock'):
                self._action.lock()
            else:
                self._action.acquire()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit context manager"""
        if hasattr(self._action, 'unlock') or hasattr(self._action, 'release'):
            if hasattr(self._action, 'unlock'):
                self._action.unlock()
            else:
                self._action.release()
    
    # Enhanced operator support
    def __eq__(self, other):
        """Equality comparison"""
        if isinstance(other, PythonicAction):
            return self.to_dict() == other.to_dict()
        return False
    
    def __ne__(self, other):
        """Inequality comparison"""
        return not self.__eq__(other)
    
    def __repr__(self):
        """String representation"""
        return f"<PythonicAction type='{self.type_string}' params={self.parameters}>"
class PythonicBrain:
    """
    Pythonic interface for Brain class.
    
    Provides property-style access, enhanced statistics,
    and context manager support.
    """
    
    def __init__(self, brain_obj):
        """
        Initialize with C++ Brain object.
        
        Args:
            brain_obj: C++ Brain object
        """
        self._brain = brain_obj
    
    @property
    def brain(self):
        """Get the underlying C++ brain object"""
        return self._brain
    
    # Property-style access for statistics
    @property
    def total_neurons(self) -> int:
        """Get total neuron count (property-style)"""
        return self._brain.getTotalNeuronCount() if hasattr(self._brain, 'getTotalNeuronCount') else 0
    
    @property
    def total_synapses(self) -> int:
        """Get total synapse count (property-style)"""
        return self._brain.getTotalSynapseCount() if hasattr(self._brain, 'getTotalSynapseCount') else 0
    
    @property
    def active_neurons(self) -> int:
        """Get active neuron count (property-style)"""
        return self._brain.getActiveNeuronCount() if hasattr(self._brain, 'getActiveNeuronCount') else 0
    
    @property
    def firing_neurons(self) -> int:
        """Get firing neuron count (property-style)"""
        return self._brain.getFiringNeuronCount() if hasattr(self._brain, 'getFiringNeuronCount') else 0
    
    @property
    def average_firing_rate(self) -> float:
        """Get average firing rate (property-style)"""
        return self._brain.getAverageFiringRate() if hasattr(self._brain, 'getAverageFiringRate') else 0.0
    
    @property
    def excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition ratio (property-style)"""
        return self._brain.getExcitationInhibitionRatio() if hasattr(self._brain, 'getExcitationInhibitionRatio') else 0.0
    
    @property
    def total_spikes(self) -> int:
        """Get total spike count (property-style)"""
        return self._brain.getTotalSpikeCount() if hasattr(self._brain, 'getTotalSpikeCount') else 0
    
    @property
    def developmental_stage(self) -> Any:
        """Get developmental stage (property-style)"""
        return self._brain.getDevelopmentalStage() if hasattr(self._brain, 'getDevelopmentalStage') else None
    
    @developmental_stage.setter
    def developmental_stage(self, value: Any):
        """Set developmental stage (property-style)"""
        if hasattr(self._brain, 'setDevelopmentalStage'):
            self._brain.setDevelopmentalStage(value)
    
    # Context manager support
    def __enter__(self):
        """Enter context manager"""
        if hasattr(self._brain, 'lock') or hasattr(self._brain, 'acquire'):
            if hasattr(self._brain, 'lock'):
                self._brain.lock()
            else:
                self._brain.acquire()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit context manager"""
        if hasattr(self._brain, 'unlock') or hasattr(self._brain, 'release'):
            if hasattr(self._brain, 'unlock'):
                self._brain.unlock()
            else:
                self._brain.release()
    
    # Enhanced statistics methods
    def get_statistics(self) -> Dict[str, Any]:
        """Get all statistics as dictionary"""
        return {
            'total_neurons': self.total_neurons,
            'total_synapses': self.total_synapses,
            'active_neurons': self.active_neurons,
            'firing_neurons': self.firing_neurons,
            'average_firing_rate': self.average_firing_rate,
            'excitation_inhibition_ratio': self.excitation_inhibition_ratio,
            'total_spikes': self.total_spikes,
            'developmental_stage': self.developmental_stage
        }
    
    def update_development(self, timestep: float) -> None:
        """Update development system"""
        if hasattr(self._brain, 'updateDevelopment'):
            self._brain.updateDevelopment(timestep)
    
    def log_status(self) -> None:
        """Log brain status"""
        if hasattr(self._brain, 'logStatus'):
            self._brain.logStatus()
    
    def __repr__(self):
        """String representation"""
        return f"<PythonicBrain neurons={self.total_neurons} synapses={self.total_synapses}>"
# Factory functions for Pythonic interfaces
def create_pythonic_config(config_obj) -> PythonicConfig:
    """Create PythonicConfig from C++ Config object"""
    return PythonicConfig(config_obj)
def create_pythonic_sensory(sensory_obj) -> PythonicSensoryInput:
    """Create PythonicSensoryInput from C++ SensoryInput object"""
    return PythonicSensoryInput(sensory_obj)
def create_pythonic_action(action_obj) -> PythonicAction:
    """Create PythonicAction from C++ Action object"""
    return PythonicAction(action_obj)
def create_pythonic_brain(brain_obj) -> PythonicBrain:
    """Create PythonicBrain from C++ Brain object"""
    return PythonicBrain(brain_obj)
# Pythonic helper functions
def get_as_numpy(data: Any, dtype: np.dtype = np.float32) -> np.ndarray:
    """Convert data to NumPy array with validation"""
    return NumpyConverter.to_numpy(data, dtype)
def set_from_numpy(target: Any, numpy_array: np.ndarray) -> None:
    """Set target from NumPy array"""
    if hasattr(target, 'setData'):
        target.setData(numpy_array.flatten().tolist())
def to_string(enum_value: Any) -> str:
    """Convert enum to string"""
    return EnumConverter.to_string(enum_value)
def from_string(value: str, enum_type: type) -> Any:
    """Convert string to enum"""
    return EnumConverter.from_string(value, enum_type)
# Register Pythonic interfaces with type converter
def _register_pythonic_interfaces() -> None:
    """Register Pythonic interfaces with type converter"""
    from .type_converter import register_converter
    
    # Register custom converters for Pythonic objects
    register_converter('PythonicConfig', create_pythonic_config)
    register_converter('PythonicSensoryInput', create_pythonic_sensory)
    register_converter('PythonicAction', create_pythonic_action)
    register_converter('PythonicBrain', create_pythonic_brain)
# Initialize on import
_register_pythonic_interfaces()
# Export all public interfaces
__all__ = [
    'PythonicConfig',
    'PythonicSensoryInput',
    'PythonicAction',
    'PythonicBrain',
    'create_pythonic_config',
    'create_pythonic_sensory',
    'create_pythonic_action',
    'create_pythonic_brain',
    'get_as_numpy',
    'set_from_numpy',
    'to_string',
    'from_string'
]
