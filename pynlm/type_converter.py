"""
Type conversion utilities for NLM Python bindings.

This module provides comprehensive type conversion utilities to bridge
C++ and Python types with Python-idiomatic interfaces.
"""

import pybind11
import numpy as np
from typing import Any, Dict, List, Optional, Union, Tuple
from enum import Enum

# Import existing exception classes
from .exceptions import (
    NLMException, ValidationError, ConfigurationError, BrainError,
    WorldError, SensoryError, AudioError, ActionError, AgentError
)
class TypeConverter:
    """
    Comprehensive type converter for NLM C++ and Python types.
    
    Provides automatic conversions between C++ and Python types
    with NumPy array support, enum conversions, and dictionary-based
    configuration handling.
    """
    
    @staticmethod
    def to_python(value: Any) -> Any:
        """
        Convert C++ value to Python type with automatic type detection.
        
        Args:
            value: C++ value to convert
            
        Returns:
            Converted Python value
            
        Raises:
            TypeError: If conversion is not supported
        """
        # Handle smart pointers
        if hasattr(value, '__class__'):
            # Check for common C++ types
            type_name = value.__class__.__name__
            
        # Handle numpy arrays
        if isinstance(value, np.ndarray):
            return value
            
        # Handle containers
        if isinstance(value, list):
            return [TypeConverter.to_python(v) for v in value]
            
        # Handle strings and basic types
        return value
    
    @staticmethod
    def from_python(value: Any, target_type: type) -> Any:
        """
        Convert Python value to C++ type.
        
        Args:
            value: Python value to convert
            target_type: Target C++ type
            
        Returns:
            Converted C++ value
            
        Raises:
            ValidationError: If conversion fails
        """
        type_name = target_type.__name__ if hasattr(target_type, '__name__') else str(target_type)
        
        # Handle basic numeric types
        if type_name in ['float', 'double', 'int', 'uint64_t']:
            if not isinstance(value, (int, float, str)):
                raise ValidationError(
                    f"Cannot convert {type(value)} to {target_type}",
                    field="value",
                    value=value,
                    expected=str(target_type)
                )
            return target_type(value)
            
        # Handle vector types
        if type_name == 'std::vector':
            if not isinstance(value, list):
                raise ValidationError(
                    f"Expected list for vector conversion, got {type(value)}",
                    field="value",
                    value=value,
                    expected="list"
                )
            return value
            
        # Handle string types
        if type_name == 'std::string':
            if isinstance(value, str):
                return value
            if value is None:
                return ""
            raise ValidationError(
                f"Expected string, got {type(value)}",
                field="value",
                value=value,
                expected="string"
            )
            
        # Handle enum types
        if isinstance(target_type, type) and issubclass(target_type, Enum):
            if isinstance(value, str):
                try:
                    return target_type(value)
                except ValueError:
                    raise ValidationError(
                        f"Invalid value '{value}' for enum {target_type.__name__}",
                        field="value",
                        value=value,
                        expected=list(t.value for t in target_type)
                    )
            if isinstance(value, int):
                try:
                    return target_type(value)
                except ValueError:
                    raise ValidationError(
                        f"Invalid value '{value}' for enum {target_type.__name__}",
                        field="value",
                        value=value,
                        expected=list(t.value for t in target_type)
                    )
                    
        # Handle None
        if value is None:
            if target_type is type(None):
                return value
            raise ValidationError(
                f"Expected {target_type}, got None",
                field="value",
                value=None,
                expected=str(target_type)
            )
            
        # Default fallback
        return value
class NumpyConverter:
    """
    Specialized converter for NumPy arrays with vision/audio data.
    
    Provides efficient conversion between C++ and NumPy arrays
    with validation for vision and audio data formats.
    """
    
    @staticmethod
    def to_numpy(data: Any, dtype: np.dtype = np.float32) -> np.ndarray:
        """
        Convert C++ vector<float> or similar to NumPy array.
        
        Args:
            data: Input data (vector, list, or array-like)
            dtype: NumPy data type
            
        Returns:
            NumPy array
        """
        if isinstance(data, np.ndarray):
            return data.astype(dtype) if data.dtype != dtype else data
            
        if isinstance(data, list):
            return np.array(data, dtype=dtype)
            
        # Handle C++ vector types (via pybind11 binding)
        if hasattr(data, '__iter__'):
            return np.array(list(data), dtype=dtype)
            
        raise ValidationError(
            f"Cannot convert {type(data)} to NumPy array",
            field="data",
            value=data,
            expected="list, vector, or NumPy array"
        )
    
    @staticmethod
    def from_numpy(arr: np.ndarray, target_shape: Optional[Tuple[int, ...]] = None) -> Any:
        """
        Convert NumPy array to C++ compatible format.
        
        Args:
            arr: NumPy array
            target_shape: Optional target shape for validation
            
        Returns:
            Converted data
        """
        if not isinstance(arr, np.ndarray):
            raise ValidationError(
                f"Expected NumPy array, got {type(arr)}",
                field="array",
                value=arr,
                expected="NumPy array"
            )
            
        if target_shape and arr.shape != target_shape:
            raise ValidationError(
                f"Array shape {arr.shape} does not match expected {target_shape}",
                field="array",
                value=arr.shape,
                expected=str(target_shape)
            )
            
        return arr.flatten().tolist()
class EnumConverter:
    """
    Converter for C++ enums to/from Python enums and strings.
    
    Provides bidirectional conversion between C++ enum types
    and Python enums or string representations.
    """
    
    @staticmethod
    def to_string(enum_value: Any) -> str:
        """
        Convert enum value to human-readable string.
        
        Args:
            enum_value: Enum value
            
        Returns:
            String representation
        """
        if isinstance(enum_value, Enum):
            return enum_value.value
        return str(enum_value)
    
    @staticmethod
    def from_string(value: str, enum_type: type) -> Any:
        """
        Convert string to enum value.
        
        Args:
            value: String value
            enum_type: Target enum type
            
        Returns:
            Enum value
            
        Raises:
            ValidationError: If string doesn't match any enum value
        """
        if not isinstance(value, str):
            raise ValidationError(
                f"Expected string for enum conversion, got {type(value)}",
                field="value",
                value=value,
                expected="string"
            )
            
        try:
            return enum_type(value)
        except ValueError:
            available = [e.value for e in enum_type]
            raise ValidationError(
                f"Invalid value '{value}' for enum {enum_type.__name__}",
                field="value",
                value=value,
                expected=available
            )
class ConfigConverter:
    """
    Converter for dictionary-based configuration.
    
    Provides automatic conversion between Python dictionaries
    and C++ Config objects with validation.
    """
    
    @staticmethod
    def to_config_dict(config_obj: Any) -> Dict[str, Any]:
        """
        Convert C++ Config object to Python dictionary.
        
        Args:
            config_obj: C++ Config object
            
        Returns:
            Python dictionary representation
        """
        result = {}
        
        # Try to get keys (may need different method based on actual Config implementation)
        if hasattr(config_obj, 'getKeys'):
            keys = config_obj.getKeys()
        elif hasattr(config_obj, 'keys'):
            keys = config_obj.keys()
        else:
            # Fallback: try to access attributes
            for attr_name in dir(config_obj):
                if not attr_name.startswith('_'):
                    attr_value = getattr(config_obj, attr_name)
                    if not callable(attr_value):
                        result[attr_name] = attr_value
            return result
            
        for key in keys:
            try:
                value = config_obj.get(key) if hasattr(config_obj, 'get') else getattr(config_obj, key)
                result[key] = TypeConverter.to_python(value)
            except Exception:
                result[key] = None
                
        return result
    
    @staticmethod
    def from_config_dict(config_dict: Dict[str, Any], config_obj: Any) -> None:
        """
        Update C++ Config object from Python dictionary.
        
        Args:
            config_dict: Python dictionary with configuration
            config_obj: C++ Config object to update
        """
        for key, value in config_dict.items():
            # Try to set value on config object
            if hasattr(config_obj, f'set_{key}'):
                getattr(config_obj, f'set_{key}')(value)
            elif hasattr(config_obj, key):
                setattr(config_obj, key, value)
            else:
                raise ValidationError(
                    f"Config key '{key}' not found",
                    field=key,
                    value=value,
                    expected="valid config key"
                )
# Global converter registry
_CONVERTERS = {
    'float': TypeConverter.to_python,
    'std::vector<float>': NumpyConverter.to_numpy,
    'std::string': TypeConverter.to_python,
    'NeuronType': EnumConverter.to_string,
    'ActionType': EnumConverter.to_string,
}
# Register converters for specific types
def register_converter(type_name: str, converter: callable) -> None:
    """
    Register a custom converter for a specific type.
    
    Args:
        type_name: Name of the type to convert
        converter: Converter function
    """
    _CONVERTERS[type_name] = converter
def get_converter(type_name: str) -> Optional[callable]:
    """
    Get converter for a specific type.
    
    Args:
        type_name: Name of the type
        
    Returns:
        Converter function or None if not found
    """
    return _CONVERTERS.get(type_name)
# Export all converters
__all__ = [
    'TypeConverter',
    'NumpyConverter',
    'EnumConverter',
    'ConfigConverter',
    'register_converter',
    'get_converter'
]
