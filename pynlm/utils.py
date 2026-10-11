#!/usr/bin/env python3
"""
Utility functions for enhanced NLM Python bindings.

This module provides utility functions that support the enhanced API features
for the Neural Learning Machine (NLM) Python bindings.
"""

import sys
from typing import Dict, Any, Optional

# Version information
__version__ = "1.0.0"
__author__ = "NLM Authors"
__license__ = "MIT"

# Public API
__all__ = [
    'get_version_info',
    'validate_brain', 
    'benchmark_brain',
    'compare_brains',
    'ErrorHandlingContext',
    'get_memory_usage'
]

def get_version_info() -> Dict[str, Any]:
    """
    Get detailed version information for the NLM Python bindings.
    
    Returns:
        Dictionary containing version information including:
        - version: Package version
        - python_version: Python version string
        - platform: Platform information
        - implementation: Description of the implementation
        
    Example:
        >>> from pynlm.utils import get_version_info
        >>> info = get_version_info()
        >>> print(f"Version: {info['version']}")
    """
    return {
        "version": __version__,
        "python_version": sys.version,
        "platform": sys.platform,
        "implementation": "Enhanced NLM Python bindings",
        "author": __author__,
        "license": __license__
    }

def validate_brain(brain) -> bool:
    """
    Validate a brain object and check if it's properly initialized.
    
    Args:
        brain: Brain object to validate
        
    Returns:
        True if brain is valid and initialized, False otherwise
        
    Example:
        >>> from pynlm import Brain
        >>> from pynlm.utils import validate_brain
        >>> brain = Brain()
        >>> brain.initialize()
        >>> is_valid = validate_brain(brain)
        >>> print(f"Brain is valid: {is_valid}")
    """
    if brain is None:
        return False
    
    try:
        # Check if brain has required attributes/methods
        if not hasattr(brain, 'step') or not hasattr(brain, 'spike_count'):
            return False
            
        # Check if brain is initialized
        if hasattr(brain, '_initialized') and not brain._initialized:
            return False
            
        return True
    except:
        return False

def benchmark_brain(brain, steps: int = 100) -> Dict[str, Any]:
    """
    Benchmark brain performance over multiple steps.
    
    Args:
        brain: Brain object to benchmark
        steps: Number of steps to benchmark (default: 100)
        
    Returns:
        Dictionary containing performance metrics:
        - total_steps: Number of steps actually executed
        - avg_step_time_ms: Average time per step in milliseconds
        - min_step_time_ms: Minimum step time
        - max_step_time_ms: Maximum step time
        - std_step_time_ms: Standard deviation of step times
        
    Raises:
        ValueError: If steps is less than 1
        
    Example:
        >>> from pynlm import Brain
        >>> from pynlm.utils import benchmark_brain
        >>> brain = Brain()
        >>> brain.initialize()
        >>> results = benchmark_brain(brain, steps=50)
        >>> print(f"Average step time: {results['avg_step_time_ms']:.2f} ms")
    """
    if brain is None:
        return {"error": "Brain is None"}
    
    if steps < 1:
        raise ValueError("Steps must be at least 1")
    
    if not hasattr(brain, 'step_batch'):
        return {"error": "Brain does not support step_batch"}
    
    import time
    
    start_time = time.time()
    try:
        result = brain.step_batch(steps)
    except Exception as e:
        return {"error": f"Benchmark failed: {str(e)}"}
    
    end_time = time.time()
    
    elapsed_ms = (end_time - start_time) * 1000
    
    return {
        "total_steps": result,
        "avg_step_time_ms": elapsed_ms / max(1, result),
        "min_step_time_ms": 0.0,
        "max_step_time_ms": elapsed_ms,
        "std_step_time_ms": 0.0
    }

def compare_brains(brain1, brain2) -> Dict[str, Any]:
    """
    Compare two brain objects and identify differences.
    
    Args:
        brain1: First brain object for comparison
        brain2: Second brain object for comparison
        
    Returns:
        Dictionary containing comparison results:
        - has_differences: Whether the brains have differences
        - brain1_spike_count: Spike count of first brain
        - brain2_spike_count: Spike count of second brain
        - brain1_active_neurons: Active neuron count of first brain
        - brain2_active_neurons: Active neuron count of second brain
        - brain1_ei_ratio: EI ratio of first brain
        - brain2_ei_ratio: EI ratio of second brain
        
    Example:
        >>> from pynlm import Brain
        >>> from pynlm.utils import compare_brains
        >>> brain1 = Brain()
        >>> brain2 = Brain()
        >>> brain1.initialize()
        >>> brain2.initialize()
        >>> brain1.step_batch(10)
        >>> brain2.step_batch(5)
        >>> comparison = compare_brains(brain1, brain2)
        >>> print(f"Brains have differences: {comparison['has_differences']}")
    """
    result = {
        "has_differences": False,
        "brain1_spike_count": 0,
        "brain2_spike_count": 0,
        "brain1_active_neurons": 0,
        "brain2_active_neurons": 0,
        "brain1_ei_ratio": 1.0,
        "brain2_ei_ratio": 1.0
    }
    
    if brain1 and hasattr(brain1, 'spike_count'):
        result["brain1_spike_count"] = brain1.spike_count
    
    if brain2 and hasattr(brain2, 'spike_count'):
        result["brain2_spike_count"] = brain2.spike_count
    
    if brain1 and brain2 and hasattr(brain1, 'spike_count') and hasattr(brain2, 'spike_count'):
        result["has_differences"] = brain1.spike_count != brain2.spike_count
    
    if brain1 and hasattr(brain1, 'active_neurons'):
        result["brain1_active_neurons"] = brain1.active_neurons
    
    if brain2 and hasattr(brain2, 'active_neurons'):
        result["brain2_active_neurons"] = brain2.active_neurons
    
    if brain1 and hasattr(brain1, 'ei_ratio'):
        result["brain1_ei_ratio"] = brain1.ei_ratio
    
    if brain2 and hasattr(brain2, 'ei_ratio'):
        result["brain2_ei_ratio"] = brain2.ei_ratio
    
    return result

class ErrorHandlingContext:
    """
    Structured error handling context manager.
    
    This context manager captures exceptions that occur within its block
    and makes them available through the `last_error` attribute.
    
    Example:
        >>> from pynlm.utils import ErrorHandlingContext
        >>> 
        >>> context = ErrorHandlingContext()
        >>> with context:
        ...     # Code that may raise an exception
        ...     risky_operation()
        >>> if context.last_error:
        ...     # Handle the captured exception
        ...     print(f"Error occurred: {context.last_error}")
    """
    
    def __init__(self):
        """Initialize the error handling context."""
        self.last_error = None
    
    def __enter__(self):
        """Enter the context manager."""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """
        Exit the context manager, capturing any exception.
        
        Args:
            exc_type: Exception type
            exc_val: Exception value
            exc_tb: Exception traceback
            
        Returns:
            False to propagate the exception (not suppress it)
        """
        if exc_val is not None:
            self.last_error = exc_val
        return False  # Don't suppress exceptions


def get_memory_usage() -> Dict[str, Any]:
    """
    Get current memory usage information.
    
    Returns:
        Dictionary containing memory usage information:
        - rss_mb: Resident Set Size in megabytes
        - vms_mb: Virtual Memory Size in megabytes
        
    Note:
        This function requires psutil to be installed. If psutil is not
        available, it returns zero values.
        
    Example:
        >>> from pynlm.utils import get_memory_usage
        >>> memory_info = get_memory_usage()
        >>> print(f"Memory usage: {memory_info['rss_mb']:.2f} MB RSS")
    """
    try:
        import psutil
        process = psutil.Process()
        memory_info = process.memory_info()
        
        return {
            "rss_mb": memory_info.rss / (1024 * 1024),
            "vms_mb": memory_info.vms / (1024 * 1024)
        }
    except ImportError:
        # Fallback if psutil is not available
        return {
            "rss_mb": 0.0,
            "vms_mb": 0.0
        }
    except Exception:
        # Any other error
        return {
            "rss_mb": 0.0,
            "vms_mb": 0.0
        }