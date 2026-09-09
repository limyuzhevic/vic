"""
NLM Python Core Module

This module contains the basic types and utilities for the NLM Python bindings.
"""

# Version handling
import sys
import platform

# Platform information
__platform__ = platform.system()
__python_version__ = sys.version_info

# Constants
NLM_VERSION = "0.1.0"
DEFAULT_SIMULATION_DT = 0.01  # 10 ms timesteps
DEFAULT_SIMULATION_STEPS = 1000
MAX_NEURONS = 1000000
MAX_SYNAPSES = 10000000

# Error types
class NLMError(Exception):
    """Base exception for all NLM errors."""
    pass

class ConfigError(NLMError):
    """Error in configuration."""
    pass

class BrainError(NLMError):
    """Error in brain operations."""
    pass

class NeuronError(NLMError):
    """Error in neuron operations."""
    pass

class SynapseError(NLMError):
    """Error in synapse operations."""
    pass

# Type aliases for convenience
from typing import Optional, List, Dict, Any, Callable, Tuple, Union

NeuronIndex = int
PopulationIndex = int
RegionIndex = int
SynapseIndex = int

# Factory functions for convenience
def create_config():
    """Create a new configuration object."""
    from .brain import Config
    return Config()

# Platform-specific optimizations
def get_optimization_level():
    """Get recommended optimization level for the current platform."""
    if platform.system() == "Linux":
        return "simd"  # SIMD optimizations available on Linux
    elif platform.system() == "Darwin":
        return "avx2"  # macOS with AVX2
    else:
        return "basic"

# Performance monitoring
class PerformanceMonitor:
    """Monitor performance of NLM operations."""
    
    def __init__(self):
        self.operations = {}
        self.start_time = 0.0
        
    def start_operation(self, name: str):
        """Start timing an operation."""
        import time
        self.operations[name] = {"start": time.time()}
        
    def end_operation(self, name: str) -> float:
        """End timing an operation and return duration."""
        import time
        if name in self.operations:
            duration = time.time() - self.operations[name]["start"]
            del self.operations[name]
            return duration
        return 0.0
        
    def get_stats(self) -> Dict[str, float]:
        """Get performance statistics."""
        import time
        stats = {}
        for name, data in self.operations.items():
            duration = time.time() - data["start"]
            stats[name] = duration
        return stats

# Global performance monitor
_monitor = PerformanceMonitor()

def get_performance_monitor() -> PerformanceMonitor:
    """Get the global performance monitor."""
    return _monitor

# Memory pool for efficiency (placeholder)
_memory_pools = {}

def register_memory_pool(name: str, pool: Any):
    """Register a memory pool for reuse."""
    _memory_pools[name] = pool

def get_memory_pool(name: str) -> Optional[Any]:
    """Get a registered memory pool."""
    return _memory_pools.get(name)

# Utility functions
def format_memory_size(bytes_: int) -> str:
    """Format memory size in human-readable format."""
    for unit in ['B', 'KB', 'MB', 'GB', 'TB']:
        if bytes_ < 1024.0:
            return f"{bytes_:.2f} {unit}"
        bytes_ /= 1024.0
    return f"{bytes_:.2f} PB"

def clamp(value: float, min_val: float, max_val: float) -> float:
    """Clamp a value between min and max."""
    return max(min_val, min(max_val, value))

def lerp(start: float, end: float, t: float) -> float:
    """Linear interpolation between start and end."""
    return start + (end - start) * t

# Neuron type factories
NEURON_TYPES = {
    "excitatory": NeuronType.Excitatory,
    "inhibitory": NeuronType.Inhibitory,
    "modulatory": NeuronType.Modulatory,
    "sensory": NeuronType.Sensory,
    "motor": NeuronType.Motor,
    "internal": NeuronType.Internal,
}

def get_neuron_type(name: str) -> NeuronType:
    """Get neuron type from string."""
    name = name.lower()
    if name not in NEURON_TYPES:
        raise ValueError(f"Unknown neuron type: {name}")
    return NEURON_TYPES[name]

SYNAPSE_TYPES = {
    "excitatory": SynapseType.Excitatory,
    "inhibitory": SynapseType.Inhibitory,
    "modulatory": SynapseType.Modulatory,
    "electrical": SynapseType.Electrical,
    "gap_junction": SynapseType.GapJunction,
}

def get_synapse_type(name: str) -> SynapseType:
    """Get synapse type from string."""
    name = name.lower()
    if name not in SYNAPSE_TYPES:
        raise ValueError(f"Unknown synapse type: {name}")
    return SYNAPSE_TYPES[name]

# Export public interface
__all__ = [
    # Version and constants
    '__version__',
    '__platform__',
    '__python_version__',
    'NLM_VERSION',
    'DEFAULT_SIMULATION_DT',
    'DEFAULT_SIMULATION_STEPS',
    'MAX_NEURONS',
    'MAX_SYNAPSES',
    
    # Error types
    'NLMError',
    'ConfigError',
    'BrainError',
    'NeuronError',
    'SynapseError',
    
    # Type aliases
    'NeuronIndex',
    'PopulationIndex', 
    'RegionIndex',
    'SynapseIndex',
    
    # Factory functions
    'create_config',
    'get_neuron_type',
    'get_synapse_type',
    'get_optimization_level',
    
    # Performance monitoring
    'PerformanceMonitor',
    'get_performance_monitor',
    
    # Memory pools
    'register_memory_pool',
    'get_memory_pool',
    
    # Utility functions
    'format_memory_size',
    'clamp',
    'lerp',
    
    # Constants
    'NEURON_TYPES',
    'SYNAPSE_TYPES',
]
