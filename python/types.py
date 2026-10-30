"""
Python type definitions module for NLM

This module provides Python-friendly type definitions and aliases
for NLM types, with additional type checking and validation
utilities.
"""

from typing import Dict, List, Optional, Union, Any, Callable, Tuple, Set
from enum import Enum

# Import from C++ bindings if available
try:
    from .bindings import (
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        SimulationStep,
        Timestamp,
        TimestepDuration,
        NeuronIndex,
        SynapseIndex,
        PopulationIndex,
        RegionIndex,
        SynapticWeight,
        MembranePotential,
        FiringRate,
        Delay,
        ConfigValue,
    )
    
    from .enums import (
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        FiringState,
        ActionType,
        MotorCommand,
        WorldObjectType,
    )
except ImportError:
    # Create stub types if C++ bindings not available
    NeuronId = SynapseId = RegionId = PopulationId = SimulationStep = Timestamp = TimestepDuration = NeuronIndex = SynapseIndex = PopulationIndex = RegionIndex = 0
    SynapticWeight = MembranePotential = FiringRate = Delay = 0.0
    
    NeuronType = SynapseType = DevelopmentalStage = FiringState = ActionType = MotorCommand = WorldObjectType = Enum('StubEnum', ['Unknown'])
    
    ConfigValue = Union[int, float, bool, str, List, Dict]

# Type definitions with enhanced Pythonic features
class TypeValidator:
    """Mixin class to add type validation and checking"""
    
    @classmethod
    def validate_type(cls, value: Any, type_hint: type, name: str = "") -> Any:
        """
        Validate a value against a type hint.
        
        Args:
            value: Value to validate
            type_hint: Expected type
            name: Parameter name for error messages
            
        Returns:
            Validated value
            
        Raises:
            TypeError: If value doesn't match expected type
        """
        if not isinstance(value, type_hint):
            type_name = type_hint.__name__
            raise TypeError(f"Invalid type for {name}: expected {type_name}, got {type(value).__name__}")
        return value
    
    @classmethod
    def validate_range(cls, value: float, min_val: float, max_val: float, 
                      name: str = "") -> float:
        """
        Validate a value is within specified range.
        
        Args:
            value: Value to validate
            min_val: Minimum allowed value
            max_val: Maximum allowed value
            name: Parameter name for error messages
            
        Returns:
            Validated value
            
        Raises:
            ValueError: If value is outside range
        """
        if not (min_val <= value <= max_val):
            raise ValueError(f"Value for {name} must be between {min_val} and {max_val}, got {value}")
        return value
    
    @classmethod
    def validate_non_negative(cls, value: float, name: str = "") -> float:
        """Validate a value is non-negative."""
        if value < 0:
            raise ValueError(f"Value for {name} must be non-negative, got {value}")
        return value
    
    @classmethod
    def validate_positive(cls, value: float, name: str = "") -> float:
        """Validate a value is positive."""
        if value <= 0:
            raise ValueError(f"Value for {name} must be positive, got {value}")
        return value

# Enhanced type definitions
class EnhancedNeuronId:
    """Enhanced neuron identifier with additional methods"""
    
    def __init__(self, value: int = 0):
        TypeValidator.validate_type(value, int, "value")
        TypeValidator.validate_non_negative(value, "value")
        self.value = value
    
    def index(self) -> int:
        """Get neuron index"""
        return self.value
    
    def __eq__(self, other: Any) -> bool:
        if isinstance(other, EnhancedNeuronId):
            return self.value == other.value
        return False
    
    def __hash__(self) -> int:
        return hash(self.value)
    
    def __repr__(self) -> str:
        return f"<NeuronId: {self.value}>"
    
    def __str__(self) -> str:
        return f"N{self.value}"

class EnhancedSynapseId:
    """Enhanced synapse identifier with additional methods"""
    
    def __init__(self, value: int = 0):
        TypeValidator.validate_type(value, int, "value")
        TypeValidator.validate_non_negative(value, "value")
        self.value = value
    
    def index(self) -> int:
        """Get synapse index"""
        return self.value
    
    def __eq__(self, other: Any) -> bool:
        if isinstance(other, EnhancedSynapseId):
            return self.value == other.value
        return False
    
    def __hash__(self) -> int:
        return hash(self.value)
    
    def __repr__(self) -> str:
        return f"<SynapseId: {self.value}>"
    
    def __str__(self) -> str:
        return f"S{self.value}"

# Type aliases with validation
ValidNeuronType = NeuronType  # Already validated by enum
ValidSynapseType = SynapseType
ValidDevelopmentalStage = DevelopmentalStage
ValidFiringState = FiringState
ValidActionType = ActionType
ValidMotorCommand = MotorCommand
ValidWorldObjectType = WorldObjectType

# Configuration value types
ValidConfigInt = Union[int, int64_t]
ValidConfigFloat = Union[float, double]
ValidConfigBool = bool
ValidConfigString = str
ValidConfigListInt = List[int]
ValidConfigListFloat = List[float]
ValidConfigListString = List[str]
ValidConfigAny = Union[ValidConfigInt, ValidConfigFloat, ValidConfigBool, 
                     ValidConfigString, ValidConfigListInt, ValidConfigListFloat, 
                     ValidConfigListString]

# Type checking functions
TypeCheckResult = Dict[str, Any]

def check_type(value: Any, expected_type: type) -> bool:
    """Check if value is of expected type"""
    return isinstance(value, expected_type)

def check_range(value: float, min_val: float = float('-inf'), 
               max_val: float = float('inf')) -> bool:
    """Check if value is within range"""
    return min_val <= value <= max_val

def check_config_value(key: str, value: Any) -> bool:
    """
    Check if configuration value is valid.
    
    Args:
        key: Configuration key
        value: Value to check
        
    Returns:
        True if value is valid, False otherwise
    """
    # Basic validation - could be extended with config-specific rules
    if value is None:
        return False
    
    valid_types = (int, float, bool, str, list, dict)
    if not isinstance(value, valid_types):
        return False
    
    # Additional type-specific validation could go here
    if isinstance(value, str) and len(value) == 0:
        return False
    
    if isinstance(value, list) and len(value) == 0:
        return False
    
    return True

# Collection types
Vector2D = Tuple[float, float]
Vector3D = Tuple[float, float, float]
Matrix2D = List[List[float]]
Matrix3D = List[List[List[float]]]

# Function type definitions
NeuronFunction = Callable[[NeuronId], float]
SynapseFunction = Callable[[SynapseId], float]
ProcessingFunction = Callable[[Any], Any]
ErrorHandler = Callable[[Exception], None]

# Context type definitions
Context = Dict[str, Any]
Environment = Dict[str, Any]
SimulationState = Dict[str, Any]

# Performance-related types
PerformanceMetrics = Dict[str, float]
CheckpointInfo = Dict[str, Any]
ValidationResult = Dict[str, Any]

# Export all type definitions
__all__ = [
    # Type classes
    'TypeValidator',
    'EnhancedNeuronId',
    'EnhancedSynapseId',
    
    # Basic type aliases
    'ValidNeuronType',
    'ValidSynapseType',
    'ValidDevelopmentalStage',
    'ValidFiringState',
    'ValidActionType',
    'ValidMotorCommand',
    'ValidWorldObjectType',
    
    # Configuration type aliases
    'ValidConfigInt',
    'ValidConfigFloat',
    'ValidConfigBool',
    'ValidConfigString',
    'ValidConfigListInt',
    'ValidConfigListFloat',
    'ValidConfigListString',
    'ValidConfigAny',
    
    # Collection types
    'Vector2D',
    'Vector3D',
    'Matrix2D',
    'Matrix3D',
    
    # Function type definitions
    'NeuronFunction',
    'SynapseFunction',
    'ProcessingFunction',
    'ErrorHandler',
    
    # Context and state types
    'Context',
    'Environment',
    'SimulationState',
    
    # Result types
    'PerformanceMetrics',
    'CheckpointInfo',
    'ValidationResult',
    
    # Type checking functions
    'check_type',
    'check_range',
    'check_config_value',
]

# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python type definitions for NLM neural simulation"

print(f"NLM type definitions module loaded (version {__version__})")