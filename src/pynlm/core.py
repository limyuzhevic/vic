# NLM Core Python Module
# Basic types and classes for the NLM system

import ctypes
import array
from typing import List, Optional, Dict, Any, Union

# Basic numeric types
Float = float
Int = int
Bool = bool
String = str

# Forward declarations to avoid circular imports
class Config:
    def __init__(self):
        self._handle = None
    
    @staticmethod
    def create_default():
        """Create a default configuration"""
        config = Config()
        config._handle = _nlm_create_default_config()
        return config
    
    def load_from_file(self, filepath):
        return _nlm_config_load_from_file(self._handle, filepath)
    
    def load_from_args(self, argc, argv):
        return _nlm_config_load_from_args(self._handle, argc, argv)
    
    def save_to_file(self, filepath):
        return _nlm_config_save_to_file(self._handle, filepath)
    
    def has(self, key):
        return _nlm_config_has(self._handle, key)
    
    def get_keys(self):
        return _nlm_config_get_keys(self._handle)
    
    def set(self, key, value, source=0):
        if isinstance(value, bool):
            return _nlm_config_set_bool(self._handle, key, value, source)
        elif isinstance(value, int):
            return _nlm_config_set_int(self._handle, key, value, source)
        elif isinstance(value, float):
            return _nlm_config_set_float(self._handle, key, value, source)
        elif isinstance(value, str):
            return _nlm_config_set_string(self._handle, key, value, source)
        else:
            raise TypeError(f"Unsupported type for key '{key}': {type(value)}")
    
    def get(self, key, default=None):
        if default is None:
            return _nlm_config_get(self._handle, key)
        else:
            return _nlm_config_get_or(self._handle, key, default)
    
    def summary(self):
        return _nlm_config_summary(self._handle)
    
    def __del__(self):
        if self._handle:
            _nlm_config_destroy(self._handle)

# Basic type definitions
class NeuronId:
    def __init__(self, value=0):
        self.value = value
    
    def __eq__(self, other):
        return isinstance(other, NeuronId) and self.value == other.value
    
    def __hash__(self):
        return hash(self.value)
    
    def __repr__(self):
        return f"NeuronId({self.value})"

class SynapseId:
    def __init__(self, value=0):
        self.value = value
    
    def __eq__(self, other):
        return isinstance(other, SynapseId) and self.value == other.value
    
    def __hash__(self):
        return hash(self.value)

class RegionId:
    def __init__(self, value=0):
        self.value = value
    
    def __eq__(self, other):
        return isinstance(other, RegionId) and self.value == other.value
    
    def __hash__(self):
        return hash(self.value)

class PopulationId:
    def __init__(self, value=0):
        self.value = value
    
    def __eq__(self, other):
        return isinstance(other, PopulationId) and self.value == other.value
    
    def __hash__(self):
        return hash(self.value)

# Enums
class NeuronType:
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Sensory = 3
    Motor = 4
    Internal = 5

class SynapseType:
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Electrical = 3
    GapJunction = 4

class DevelopmentalStage:
    Initial = 0
    CriticalPeriod = 1
    Maturation = 2
    Adult = 3
    Aging = 4

class FiringState:
    Resting = 0
    Active = 1
    Refractory = 2
    Inhibited = 3

class ActionType:
    MoveForward = 0
    MoveBackward = 1
    MoveLeft = 2
    MoveRight = 3
    TurnLeft = 4
    TurnRight = 5
    Look = 6
    LookUp = 7
    LookDown = 8
    Interact = 9
    Eat = 10
    Drink = 11
    Rest = 12
    Wait = 13
    Custom = 14

class MotorCommand:
    MoveForward = 0
    MoveBackward = 1
    TurnLeft = 2
    TurnRight = 3
    LookLeft = 4
    LookRight = 5
    Interact = 6
    Wait = 7

class WorldObjectType:
    Empty = 0
    Resource = 1
    Hazard = 2
    Wall = 3
    Marker = 4

# External bindings (placeholder implementations)
def _nlm_create_default_config():
    """Create a default config instance"""
    return Config._create_handle()

def _nlm_config_destroy(handle):
    """Destroy config instance"""
    pass

def _nlm_config_load_from_file(handle, filepath):
    """Load config from file"""
    return True

def _nlm_config_load_from_args(handle, argc, argv):
    """Load config from args"""
    return True

def _nlm_config_save_to_file(handle, filepath):
    """Save config to file"""
    return True

def _nlm_config_has(handle, key):
    """Check if config has key"""
    return False

def _nlm_config_get_keys(handle):
    """Get all config keys"""
    return []

def _nlm_config_set_bool(handle, key, value, source):
    """Set boolean config value"""
    return True

def _nlm_config_set_int(handle, key, value, source):
    """Set int config value"""
    return True

def _nlm_config_set_float(handle, key, value, source):
    """Set float config value"""
    return True

def _nlm_config_set_string(handle, key, value, source):
    """Set string config value"""
    return True

def _nlm_config_get(handle, key):
    """Get config value"""
    return None

def _nlm_config_get_or(handle, key, default):
    """Get config value or default"""
    return default

def _nlm_config_summary(handle):
    """Get config summary"""
    return ""

# Helper function to create config handle
Config._create_handle = lambda: object()

# Submodule exports
__all__ = [
    'Config',
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 
    'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType',
    '_nlm_create_default_config', '_nlm_config_destroy',
    '_nlm_config_load_from_file', '_nlm_config_load_from_args',
    '_nlm_config_save_to_file', '_nlm_config_has', '_nlm_config_get_keys',
    '_nlm_config_set_bool', '_nlm_config_set_int', '_nlm_config_set_float',
    '_nlm_config_set_string', '_nlm_config_get', '_nlm_config_get_or',
    '_nlm_config_summary'
]
