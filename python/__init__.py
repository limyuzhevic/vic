"""
Enhanced NLM (Neural Learning Machine) Python Package
===================================================

This package provides a comprehensive Python interface for the NLM C++ neural
simulation framework. It includes enhanced bindings with Pythonic interfaces,
convenience methods, error handling, and integration with scientific Python
tools.
"""

import sys
from typing import Any, Dict, List, Optional, Union, Tuple, Iterator

# Version information
__version__ = "1.0.0-enhanced"
__author__ = "NLM Development Team"
__license__ = "MIT"
__copyright__ = "Copyright 2024 NLM Project"

# Import core modules
from .pynlm import (
    # Core classes
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    Vision,
    Audio,
    InternalSignals,
    SensoryPercept,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    
    # Identifiers and enumerations
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    
    # Exception types
    NLMException,
    ConfigurationError,
    SimulationError,
    MemoryError,
    
    # Constants
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
)

# Import convenience factory methods
from .pynlm import (
    create_config,
    create_brain,
    create_simple_world,
    create_agent_brain,
    DEFAULT_CONFIG,
)

# Import utility functions
from .pynlm import (
    safe_execute,
    generate_id,
    validate_input,
    to_upper,
    to_lower,
    trim,
)

# Import NumPy integration helpers (if available)
_numpy_available = False
if _numpy_available:
    from .pynlm import from_numpy, to_numpy, create_matrix, array_operations

# Import visualization helpers (if available)
_matplotlib_available = False
if _matplotlib_available:
    from .pynlm import plot_neural_activity, plot_weight_distribution

# Import machine learning helpers (if available)
_sklearn_available = False
if _sklearn_available:
    from .pynlm import prepare_ml_features, train_neural_model

# Package metadata
__all__ = [
    # Core classes
    "Config",
    "Brain",
    "AgentBrain",
    "SimpleWorld",
    "Vision",
    "Audio",
    "InternalSignals",
    "SensoryPercept",
    "Action",
    "WorldObject",
    "AgentBody",
    "ActionResult",
    
    # Identifiers and enumerations
    "NeuronId",
    "SynapseId",
    "RegionId",
    "PopulationId",
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    "FiringState",
    "ActionType",
    "MotorCommand",
    "WorldObjectType",
    
    # Exception types
    "NLMException",
    "ConfigurationError",
    "SimulationError",
    "MemoryError",
    
    # Factory functions
    "create_config",
    "create_brain",
    "create_simple_world",
    "create_agent_brain",
    
    # Constants
    "DEFAULT_CONFIG",
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    
    # Utility functions
    "safe_execute",
    "generate_id",
    "validate_input",
    "to_upper",
    "to_lower",
    "trim",
    
    # Optional dependencies
    "from_numpy",
    "to_numpy",
    "create_matrix",
    "array_operations",
    "plot_neural_activity",
    "plot_weight_distribution",
    "prepare_ml_features",
    "train_neural_model",
]

# Package-level configuration
_package_config = {
    "version": __version__,
    "author": __author__,
    "license": __license__,
    "numpy_available": _numpy_available,
    "matplotlib_available": _matplotlib_available,
    "sklearn_available": _sklearn_available,
}

# Context manager for resource management
class NLMSession:
    """
    Context manager for managing NLM simulation sessions.
    
    Ensures proper cleanup of resources and consistent error handling.
    
    Example:
        with NLMSession() as session:
            brain = session.create_brain(config)
            brain.initialize()
            brain.step(100)
            spikes = brain.get_spike_array()
    """
    
    def __init__(self):
        self.session_id = generate_id()
        self.brains = []
        self.worlds = []
        
    def __enter__(self):
        print(f"Starting NLM session: {self.session_id}")
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.cleanup()
        if exc_type is not None:
            print(f"Session {self.session_id} terminated with exception: {exc_val}")
        else:
            print(f"Session {self.session_id} completed successfully")
            
    def create_brain(self, config=None):
        """Create a brain within the current session."""
        if config is None:
            config = create_config()
        brain = create_brain(config)
        self.brains.append(brain)
        return brain
        
    def create_world(self, **kwargs):
        """Create a world within the current session."""
        from .pynlm import create_simple_world
        world = create_simple_world()
        if "width" in kwargs:
            world.configure(kwargs["width"], kwargs.get("height", 100),
                           kwargs.get("vision_width", 84), kwargs.get("vision_height", 84))
        self.worlds.append(world)
        return world
        
    def cleanup(self):
        """Clean up all resources in the session."""
        for brain in self.brains:
            try:
                brain.reset()
            except:
                pass
        for world in self.worlds:
            try:
                world.reset()
            except:
                pass
        self.brains.clear()
        self.worlds.clear()
        
    def __repr__(self):
        return f"NLMSession(id='{self.session_id}', brains={len(self.brains)}, worlds={len(self.worlds)})"

# Global error handler
def set_error_handler(handler):
    """
    Set a custom error handler for NLM exceptions.
    
    Args:
        handler: Function that takes (exception_type, exception_message)
    """
    global _error_handler
    _error_handler = handler

_error_handler = None

# Convenience shortcuts
brain_builder = None  # Will be set to ConfigBuilder when needed

def quick_brain(**kwargs):
    """
    Create a quick brain with common configurations.
    
    Example:
        config = quick_brain(neuron_count=1000, learning_rate=0.01)
        brain = create_brain(config)
    """
    config = create_config()
    for key, value in kwargs.items():
        config.set(key, value)
    return config

# Package-level utilities
class NLMTools:
    """Utility class for common NLM operations and data processing."""
    
    @staticmethod
    def normalize_spike_data(spike_array, min_val=0.0, max_val=1.0):
        """Normalize spike data to specified range."""
        if _numpy_available:
            import numpy as np
            arr = np.array(spike_array)
            normalized = (arr - arr.min()) / (arr.max() - arr.min()) * (max_val - min_val) + min_val
            return normalized.tolist()
        else:
            raise RuntimeError("NumPy is required for normalization")
    
    @staticmethod
    def compute_statistics(data):
        """Compute basic statistics for a dataset."""
        if _numpy_available:
            import numpy as np
            arr = np.array(data)
            return {
                "mean": float(np.mean(arr)),
                "std": float(np.std(arr)),
                "min": float(np.min(arr)),
                "max": float(np.max(arr)),
                "median": float(np.median(arr)),
            }
        else:
            raise RuntimeError("NumPy is required for statistics")
    
    @staticmethod
    def save_simulation_data(brain, filepath, format="json"):
        """Save simulation data to file."""
        if format.lower() == "json":
            import json
            data = {
                "spike_array": brain.get_spike_array(),
                "weight_matrix": brain.get_weight_matrix(),
                "config": brain.get_config().summary(),
                "neuron_count": brain.get_neuron_count(),
                "synapse_count": brain.get_synapse_count(),
            }
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
        elif format.lower() == "csv":
            import csv
            with open(filepath, 'w', newline='') as f:
                writer = csv.writer(f)
                writer.writerow(["neuron_id", "firing_rate"])
                spike_array = brain.get_spike_array()
                if spike_array.size() > 0:
                    data = spike_array.unchecked<2>()
                    for i in range(data.shape(0)):
                        writer.writerow([data(i, 0), data(i, 1)])
        else:
            raise ValueError(f"Unsupported format: {format}")
    
    @staticmethod
    def load_simulation_data(filepath, format="json"):
        """Load simulation data from file."""
        if format.lower() == "json":
            import json
            with open(filepath, 'r') as f:
                data = json.load(f)
            return data
        else:
            raise ValueError(f"Unsupported format: {format}")
    
    @staticmethod
    def benchmark_operation(operation, *args, **kwargs):
        """Benchmark an NLM operation and return timing information."""
        import time
        start_time = time.perf_counter()
        
        if callable(operation):
            result = operation(*args, **kwargs)
        else:
            raise ValueError("Operation must be callable")
        
        end_time = time.perf_counter()
        duration = end_time - start_time
        
        return {
            "result": result,
            "duration_seconds": duration,
            "duration_milliseconds": duration * 1000,
            "operations_per_second": 1.0 / duration if duration > 0 else float('inf'),
        }

# Make tools available at package level
tools = NLMTools()

# Environment check
def check_dependencies():
    """Check which optional dependencies are available."""
    dependencies = {
        "numpy": _numpy_available,
        "matplotlib": _matplotlib_available,
        "scikit_learn": _sklearn_available,
    }
    
    print("NLM Package Dependencies:")
    for dep, available in dependencies.items():
        status = "✓ Available" if available else "✗ Not available"
        print(f"  {dep}: {status}")
    
    return dependencies

# Initialize package
if __name__ == "__main__":
    print(f"Initializing NLM v{__version__}")
    check_dependencies()