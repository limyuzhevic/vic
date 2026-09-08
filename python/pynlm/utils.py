"""
Utility functions and helpers for NLM Python bindings.

This module provides utility functions and helpers for common tasks
in NLM, including validation, timing, and string formatting.
"""

import time
from typing import Dict, Any, Optional, Callable, TypeVar, Type
import re
from contextlib import contextmanager

from .exceptions import (
    ConfigurationError,
    ResourceError,
)

T = TypeVar("T")

# Type aliases
ConfigType = Dict[str, Any]
@contextmanager
def create_named_timer(name: str) -> Callable[[], float]:
    """
    Create a named timer context manager.
    
    Args:
        name: Name for the timer
        
    Yields:
        Function to get elapsed time
    """
    start_time = time.time()
    
    def elapsed() -> float:
        return time.time() - start_time
    
    try:
        yield elapsed
    finally:
        pass


def validate_config(config: ConfigType, schema: Optional[Dict[str, Any]] = None) -> bool:
    """
    Validate configuration dictionary.
    
    Args:
        config: Configuration dictionary to validate
        schema: Optional schema for validation
        
    Returns:
        True if configuration is valid, False otherwise
        
    Raises:
        ConfigurationError: If configuration is invalid
    """
    if not isinstance(config, dict):
        raise ConfigurationError("Configuration must be a dictionary")

    # Basic validation
    if "brain" not in config:
        raise ConfigurationError("Configuration must contain 'brain' section")

    brain_config = config.get("brain", {})
    if not isinstance(brain_config, dict):
        raise ConfigurationError("Brain configuration must be a dictionary")

    # Check for required brain parameters
    required_params = ["neuron_count", "v_thresh", "v_rest"]
    for param in required_params:
        if param not in brain_config:
            raise ConfigurationError(f"Brain configuration missing required parameter: {param}")

    return True


def create_neuron_id(region_id: int, neuron_index: int) -> str:
    """
    Create a neuron ID string.
    
    Args:
        region_id: Region identifier
        neuron_index: Neuron index within region
        
    Returns:
        Neuron ID string
    """
    return f"{region_id:04d}_{neuron_index:06d}"


def parse_neuron_id(neuron_id: str) -> Tuple[int, int]:
    """
    Parse neuron ID string.
    
    Args:
        neuron_id: Neuron ID string
        
    Returns:
        Tuple of (region_id, neuron_index)
    """
    match = re.match(r"(\d{4})_(\d{6})", neuron_id)
    if not match:
        raise ValueError(f"Invalid neuron ID format: {neuron_id}")

    region_id = int(match.group(1))
    neuron_index = int(match.group(2))
    
    return region_id, neuron_index


def format_neuron_id(region_id: int, neuron_index: int) -> str:
    """
    Format neuron ID string.
n    
    Args:
        region_id: Region identifier
        neuron_index: Neuron index within region
        
    Returns:
        Neuron ID string
    """
    return f"{region_id:04d}_{neuron_index:06d}"


def parse_synaptic_weight(weight: Any) -> float:
    """
    Parse synaptic weight value.
    
    Args:
        weight: Weight value (string or number)
        
    Returns:
        Parsed weight value as float
    """
    if isinstance(weight, str):
        try:
            return float(weight)
        except ValueError:
            raise ValueError(f"Invalid weight value: {weight}")
    elif isinstance(weight, (int, float)):
        return float(weight)
    else:
        raise ValueError(f"Invalid weight type: {type(weight)}")


def get_simulation_time() -> float:
    """
    Get current simulation time.
    
    Returns:
        Current simulation time in seconds
    """
    return time.time()


def safe_divide(numerator: float, denominator: float, default: float = 0.0) -> float:
    """
    Safely divide two numbers.
    
    Args:
        numerator: Numerator
        denominator: Denominator
        default: Default value if division by zero
        
    Returns:
        Division result or default if denominator is zero
    """
    if denominator == 0:
        return default
    return numerator / denominator


def clamp(value: float, min_value: float, max_value: float) -> float:
    """
    Clamp value between minimum and maximum.
    
    Args:
        value: Value to clamp
        min_value: Minimum value
        max_value: Maximum value
        
    Returns:
        Clamped value
    """
    return max(min_value, min(value, max_value))


def linear_interpolation(t: float, start: float, end: float) -> float:
    """
    Linearly interpolate between two values.
    
    Args:
        t: Interpolation parameter (0.0 to 1.0)
        start: Start value
        end: End value
        
    Returns:
        Interpolated value
    """
    return start + t * (end - start)


def gaussian_kernel(x: float, sigma: float = 1.0) -> float:
    """
    Compute Gaussian kernel value.
    
    Args:
        x: Input value
        sigma: Standard deviation
        
    Returns:
        Gaussian kernel value
    """
    return math.exp(-(x ** 2) / (2 * sigma ** 2)) / (sigma * math.sqrt(2 * math.pi))


# Performance monitoring utilities
_performance_data = {}

@contextmanager
def monitor_performance(operation_name: str):
    """
    Monitor performance of an operation.
    
    Args:
        operation_name: Name of the operation to monitor
        
    Yields:
        None
    """
    start_time = time.time()
    
    try:
        yield
    finally:
        elapsed = time.time() - start_time
        _performance_data[operation_name] = _performance_data.get(operation_name, 0) + elapsed


def get_performance_stats() -> Dict[str, float]:
    """
    Get performance statistics.
    
    Returns:
        Dictionary with performance statistics
    """
    return _performance_data.copy()


def reset_performance_stats() -> None:
    """Reset performance statistics."""
    _performance_data.clear()