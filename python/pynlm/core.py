"""
Core Python module for NLM (Neural Learning Machine)

This module provides the fundamental classes and utilities for working with
the NLM C++ neural simulation framework through Python bindings.
"""

import sys
from typing import Dict, Any, Optional, List, Union, Iterator
from contextlib import contextmanager

# Import C++ bindings
try:
    from pynlm.core import (
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        Config,
        createDefaultConfig,
        createBrain,
        createSimpleWorld,
        createAgentBrain
    )
    _CPP_BINDINGS_AVAILABLE = True
except ImportError:
    _CPP_BINDINGS_AVAILABLE = False

# Version information
__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Type aliases for better code readability
NeuronType = Any  # Will be properly defined when bindings are available
SynapseType = Any
ActionType = Any
MotorCommand = Any
WorldObjectType = Any
DevelopmentalStage = Any
FiringState = Any

class NLMError(Exception):
    """Base exception class for all NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Exception raised for configuration-related errors."""
    pass

class InitializationError(NLMError):
    """Exception raised for initialization-related errors."""
    pass

class SimulationError(NLMError):
    """Exception raised for simulation-related errors."""
    pass

def validate_condition(condition: bool, message: str, error_type: type = ConfigurationError):
    """
    Validate a condition and raise appropriate exception if false.
    
    Args:
        condition: Condition to validate
        message: Error message if condition is false
        error_type: Exception type to raise
        
    Raises:
        error_type: If condition is false
    """
    if not condition:
        raise error_type(message)

class Property:
    """Utility class for creating Python properties with validation."""
    
    @staticmethod
    def readonly(value):
        """Create a readonly property."""
        return property(lambda self: value)

class ResourceManager:
    """Context manager for NLM resources."""
    
    def __init__(self, resource_name: str, resource_id: Any):
        self.resource_name = resource_name
        self.resource_id = resource_id
        self.resource = None
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        # Cleanup logic would go here
        pass

def get_nlm_version() -> str:
    """Get NLM version information."""
    return __version__

def get_available_systems() -> Dict[str, Any]:
    """
    Get information about available NLM systems.
    
    Returns:
        Dictionary with system information
    """
    if not _CPP_BINDINGS_AVAILABLE:
        return {"status": "unavailable", "message": "C++ bindings not available"}
    
    systems = {
        "status": "available",
        "memory": {"working": True, "episodic": True, "associative": True},
        "neuromodulation": {"dopamine": True, "curiosity": True, "novelty": True},
        "prediction": True,
        "cognition": {"planning": True, "concept_formation": True, "attention": True},
        "development": True,
        "plasticity": {"stdp": True, "hebbian": True, "structural": True},
        "sensory": {"vision": True, "audio": True, "internal": True},
        "motor": True,
        "environment": True,
        "agent": True
    }
    
    return systems

def validate_configuration(config_dict: Dict[str, Any]) -> Dict[str, Any]:
    """
    Validate and normalize configuration dictionary.
    
    Args:
        config_dict: Configuration dictionary to validate
        
    Returns:
        Validated and normalized configuration
        
    Raises:
        ConfigurationError: If configuration is invalid
    """
    # Default values
    defaults = {
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.15,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001,
        "simulation_timestep": 0.001,
        "random_seed": 42
    }
    
    # Validate and merge with defaults
    validated_config = defaults.copy()
    validated_config.update(config_dict)
    
    # Additional validation
    if validated_config["neuron_count"] <= 0:
        raise ConfigurationError("neuron_count must be positive")
    
    if not 0.0 <= validated_config["connection_probability"] <= 1.0:
        raise ConfigurationError("connection_probability must be between 0 and 1")
    
    return validated_config

class Logger:
    """NLM logger with different levels."""
    
    class Level:
        DEBUG = 0
        INFO = 1
        WARNING = 2
        ERROR = 3
        CRITICAL = 4
    
    def __init__(self, level: int = Level.INFO):
        self.level = level
        self.handlers = []
    
    def add_handler(self, handler, level: int = None):
        """Add a log handler."""
        if level is not None:
            handler.setLevel(level)
        self.handlers.append(handler)
    
    def debug(self, message: str):
        """Log debug message."""
        if self.level <= self.Level.DEBUG:
            for handler in self.handlers:
                print(f"DEBUG: {message}")
    
    def info(self, message: str):
        """Log info message."""
        if self.level <= self.Level.INFO:
            for handler in self.handlers:
                print(f"INFO: {message}")
    
    def warning(self, message: str):
        """Log warning message."""
        if self.level <= self.Level.WARNING:
            for handler in self.handlers:
                print(f"WARNING: {message}")
    
    def error(self, message: str):
        """Log error message."""
        if self.level <= self.Level.ERROR:
            for handler in self.handlers:
                print(f"ERROR: {message}")

# Global logger
_default_logger = Logger()

def get_logger(name: str = "nlm") -> Logger:
    """Get logger instance."""
    return _default_logger

def setup_nlm_logging(level: str = "INFO"):
    """
    Setup NLM logging system.
    
    Args:
        level: Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
    """
    level_map = {
        "DEBUG": Logger.Level.DEBUG,
        "INFO": Logger.Level.INFO,
        "WARNING": Logger.Level.WARNING,
        "ERROR": Logger.Level.ERROR,
        "CRITICAL": Logger.Level.CRITICAL
    }
    
    if level.upper() in level_map:
        _default_logger.level = level_map[level.upper()]

# Educational utilities
class Tutorial:
    """Educational tutorial system for NLM."""
    
    @staticmethod
    def basic_neural_network():
        """Basic neural network tutorial."""
        tutorial_content = """
# Basic Neural Network Tutorial

This tutorial demonstrates how to create and run a basic neural network
using the NLM Python bindings.

## Step 1: Import and Setup
```python
from pynlm import createDefaultConfig, createBrain

config = createDefaultConfig()
config.set("neuron_count", 100)
config.set("region_count", 1)
```

## Step 2: Create Brain
```python
brain = createBrain(config)
brain.initialize()
```

## Step 3: Run Simulation
```python
for step in range(100):
    brain.step(step, step * 0.001)
```

## Step 4: Get Results
```python
spike_count = brain.getTotalSpikeCount()
firing_rate = brain.getAverageFiringRate()
print(f"Total spikes: {spike_count}")
print(f"Average firing rate: {firing_rate:.2f} Hz")
```

## Step 5: Produce Action
```python
action = brain.produceAction()
print(f"Action type: {action.getType()}")
```

This tutorial introduces the basic workflow of creating and running
an NLM neural network.
"""
        return tutorial_content
    
    @staticmethod
    def phase6_integration():
        """Phase 6 integration tutorial."""
        tutorial_content = """
# Phase 6 Integration Tutorial

This tutorial demonstrates how all brain systems work together in Phase 6
of the NLM project.

## What is Phase 6?
Phase 6 represents the final integration of all brain systems into a coherent
artificial brain capable of memory, prediction, cognition, and development.

## Key Features

### Memory Integration
- Working memory with persistent activity
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations

### Neuromodulation
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing

### Prediction Systems
- Forward models integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

### Cognitive Processing
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics

### Development
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

## Integration Benefits

1. **Coherent Behavior**: All systems work together seamlessly
2. **Adaptive Learning**: Multiple mechanisms drive learning
3. **Persistent Memory**: Experiences are stored and replayed
4. **Goal-Directed Behavior**: Planning and attention guide actions
5. **Developmental Trajectory**: System changes over time

## How to Use Phase 6

```python
from pynlm.experiments import Phase6IntegrationDemo

# Create demo with custom settings
demo = Phase6IntegrationDemo()
results = demo.run({
    "neuronCount": 500,
    "maxSteps": 2000,
    "enableCheckpointing": True,
    "enableReplay": True,
    "enableDevelopment": True
})

# Check integration status
print(f"Working Memory Integrated: {results.memoryWorkingMemoryIntegrated}")
print(f"Episodic Memory Integrated: {results.memoryEpisodicMemoryIntegrated}")
print(f"Neuromodulation Integrated: {results.neuromodulationIntegrated}")
```

## Advanced Features

### Checkpoint System
Save and restore brain states for long-term experiments.

### Memory Replay
Replay experiences for memory consolidation.

### Development
Study how the brain changes over time.

### Performance Monitoring
Track simulation performance and metrics.

This tutorial shows how Phase 6 represents the culmination of NLM development,
demonstrating how all brain systems work together to create a functioning
artificial brain.
"""
        return tutorial_content

# Performance monitoring utilities
class PerformanceMonitor:
    """Performance monitoring utilities."""
    
    def __init__(self):
        self.metrics = {}
        self.start_time = None
    
    def start(self):
        """Start performance monitoring."""
        import time
        self.start_time = time.time()
        self.metrics = {"steps": 0, "start_time": self.start_time}
    
    def step_completed(self, step: int, spikes: int, firing_rate: float):
        """Record completion of a simulation step."""
        import time
        self.metrics["steps"] = step
        self.metrics["spikes"] = spikes
        self.metrics["firing_rate"] = firing_rate
        self.metrics["current_time"] = time.time()
    
    def get_performance_report(self) -> Dict[str, Any]:
        """Get performance report."""
        if not self.metrics or "start_time" not in self.metrics:
            return {"status": "not_started"}
        
        import time
        total_time = time.time() - self.metrics["start_time"]
        steps = self.metrics.get("steps", 0)
        
        report = {
            "total_steps": steps,
            "total_time": total_time,
            "steps_per_second": steps / total_time if total_time > 0 else 0,
            "spikes_per_second": self.metrics.get("spikes", 0) / total_time if total_time > 0 else 0,
            "avg_firing_rate": self.metrics.get("firing_rate", 0)
        }
        
        return report

# Batch processing utilities
class BatchProcessor:
    """Batch processing utilities for NLM simulations."""
    
    def __init__(self, batch_size: int = 10):
        self.batch_size = batch_size
        self.current_batch = []
        self.completed_batches = []
    
    def add_result(self, result: Dict[str, Any]):
        """Add result to current batch."""
        self.current_batch.append(result)
        
        if len(self.current_batch) >= self.batch_size:
            self.flush_batch()
    
    def flush_batch(self):
        """Flush current batch to completed batches."""
        if self.current_batch:
            self.completed_batches.append(self.current_batch.copy())
            self.current_batch = []
    
    def get_results(self) -> List[Dict[str, Any]]:
        """Get all results."""
        self.flush_batch()
        return self.completed_batches
    
    def clear(self):
        """Clear all results."""
        self.current_batch = []
        self.completed_batches = []

# Configuration presets
PRESETS = {
    "quick_test": {
        "neuron_count": 100,
        "region_count": 1,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001,
        "random_seed": 42
    },
    "learning_experiment": {
        "neuron_count": 500,
        "region_count": 2,
        "connection_probability": 0.15,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_tau": 20.0,
        "simulation_timestep": 0.001,
        "random_seed": 123
    },
    "production": {
        "neuron_count": 1000,
        "region_count": 4,
        "connection_probability": 0.2,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "stdp_tau": 15.0,
        "synaptogenesis_rate": 0.0002,
        "pruning_rate": 0.00002,
        "simulation_timestep": 0.001,
        "random_seed": 42
    }
}

# Educational mode
class EducationalMode:
    """Educational mode with guided exploration."""
    
    def __init__(self):
        self.current_lesson = None
        self.completed_lessons = []
    
    def start_lesson(self, lesson_name: str):
        """Start an educational lesson."""
        self.current_lesson = lesson_name
        print(f"Starting lesson: {lesson_name}")
        # Implementation would include lesson content and steps
    
    def complete_lesson(self):
        """Mark current lesson as completed."""
        if self.current_lesson:
            self.completed_lessons.append(self.current_lesson)
            print(f"Completed lesson: {self.current_lesson}")
            self.current_lesson = None

# Export main symbols
__all__ = [
    # Exception classes
    'NLMError',
    'ConfigurationError',
    'InitializationError',
    'SimulationError',
    
    # Utility functions
    'validate_condition',
    'get_nlm_version',
    'get_available_systems',
    'validate_configuration',
    'get_logger',
    'setup_nlm_logging',
    'Property',
    'ResourceManager',
    
    # Tutorials
    'Tutorial',
    
    # Performance monitoring
    'PerformanceMonitor',
    
    # Batch processing
    'BatchProcessor',
    
    # Configuration presets
    'PRESETS',
    
    # Educational mode
    'EducationalMode',
    
    # Version info
    '__version__',
    '__author__',
    '__description__'
]

# For backward compatibility
if _CPP_BINDINGS_AVAILABLE:
    # Export C++ bindings for backward compatibility
    sys.modules['nlm'] = sys.modules['pynlm']