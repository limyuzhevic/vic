"""
Core NLM functionality for Python bindings
"""

import numpy as np
import time
import threading
from typing import Dict, List, Any, Optional, Callable
from dataclasses import dataclass, field
from enum import Enum
import warnings

# Import the enhanced Python API from the main module
try:
    from ..enhanced_api import (
        NLM as EnhancedNLM,
        NMConfiguration,
        ExperimentResult,
        PerformanceMonitor,
        NLMError,
        ConfigurationError,
        PerformanceError,
        ExperimentError,
        create_default_nlm,
        run_multiple_experiments,
        get_performance_summary,
    )
    USE_ENHANCED_API = True
except ImportError:
    USE_ENHANCED_API = False
    warnings.warn("Enhanced API not available, falling back to basic functionality")

# Re-export the enhanced API classes
if USE_ENHANCED_API:
    NLM = EnhancedNLM
    # Create convenience aliases
    create_default_config = NMConfiguration
    experiment_result = ExperimentResult
    performance_monitor = PerformanceMonitor

# Define basic types for backward compatibility
class NeuronType(Enum):
    Excitatory = 0
    Inhibitory = 1
    Sensory = 2
    Motor = 3
    Modulatory = 4
    Internal = 5

class DevelopmentalStage(Enum):
    Initial = 0
    CriticalPeriod = 1
    Maturation = 2
    Adult = 3
    Aging = 4

class ActionType(Enum):
    MoveForward = 0
    MoveBackward = 1
    TurnLeft = 2
    TurnRight = 3
    Interact = 4
    LookLeft = 5
    LookRight = 6
    Wait = 7

class WorldObjectType(Enum):
    Empty = 0
    Resource = 1
    Hazard = 2
    Wall = 3
    Marker = 4

# Simulation types
SimulationStep = int
TimestepDuration = float

# Mock C++ extension interface
class MockBrain:
    """Mock brain implementation for testing"""
    
    def __init__(self, config):
        self.config = config
        self._neurons = config.get('neuron_count', 1000)
        self._spikes = 0
        self._steps = 0
    
    def initialize(self):
        return True
    
    def step(self, step):
        self._steps = step
        self._spikes += self._neurons // 100  # Mock spikes
        return True
    
    def get_total_spike_count(self):
        return self._spikes
    
    def get_firing_neuron_count(self):
        return min(self._neurons, self._spikes // 10)
    
    def get_average_firing_rate(self):
        if self._steps > 0:
            return (self._spikes / self._steps) * 1000.0  # Convert to Hz
        return 0.0
    
    def get_excitation_inhibition_ratio(self):
        return 1.0  # Mock value
    
    def get_developmental_stage(self):
        return DevelopmentalStage.Maturation
    
    def get_developmental_age(self):
        return self._steps
    
    def get_prediction_error(self):
        return 0.5  # Mock value
    
    def get_working_memory(self):
        return None
    
    def get_episodic_memory(self):
        return None
    
    def get_dopamine(self):
        return None
    
    def get_curiosity(self):
        return None

# Mock Config class
class MockConfig:
    """Mock configuration implementation"""
    
    def __init__(self, config_dict=None):
        self._data = config_dict or {}
    
    def set(self, key, value, source=None):
        self._data[key] = value
    
    def get(self, key, default=None):
        return self._data.get(key, default)
    
    def getOr(self, key, default):
        return self._data.get(key, default)
    
    def has(self, key):
        return key in self._data

# Mock functions to simulate C++ extensions
def mock_createBrain(config):
    return MockBrain(config)

def mock_createDefaultConfig():
    return MockConfig({
        'neuron_count': 1000,
        'region_count': 2,
        'connection_probability': 0.1,
        'random_seed': 42,
        'simulation_timestep': 0.001,
    })

# Try to import real C++ extensions if available
try:
    from .nlm_core import createBrain as real_createBrain
    from .nlm_core import createDefaultConfig as real_createDefaultConfig
    
    createBrain = real_createBrain
    createDefaultConfig = real_createDefaultConfig
except ImportError:
    # Use mock implementations
    createBrain = mock_createBrain
    createDefaultConfig = mock_createDefaultConfig

# Create enhanced NLM wrapper
if USE_ENHANCED_API:
    NLM = EnhancedNLM
else:
    class NLM:
        """Basic NLM wrapper when enhanced API is not available"""
        
        def __init__(self, config=None):
            if config is None:
                self.config = createDefaultConfig()
            elif isinstance(config, dict):
                self.config = MockConfig(config)
            else:
                self.config = config
        
        def create_brain(self):
            return createBrain(self.config._data)
        
        def run_experiment(self, name, steps):
            brain = self.create_brain()
            brain.initialize()
            
            for step in range(steps):
                brain.step(step)
            
            # Return mock result
            result = {
                'name': name,
                'success': True,
                'duration': steps * 0.001,
                'neurons_per_second': self.config._data.get('neuron_count', 1000) / (steps * 0.001),
                'spikes_per_second': brain.get_total_spike_count() / (steps * 0.001),
                'avg_firing_rate_hz': brain.get_average_firing_rate(),
                'task_performance': 0.8,
                'learning_accuracy': 0.7,
                'total_spikes': brain.get_total_spike_count(),
                'total_steps': steps,
                'final_spike_rate': brain.get_total_spike_count() / steps,
                'final_firing_rate': brain.get_firing_neuron_count(),
            }
            
            return result
        
        def run_batch(self, experiments):
            results = []
            for exp in experiments:
                result = self.run_experiment(**exp)
                results.append(result)
            return results

# Re-export commonly used types
__all__ = [
    'NLM',
    'MockBrain',
    'MockConfig',
    'NeuronType',
    'DevelopmentalStage',
    'ActionType',
    'WorldObjectType',
    'SimulationStep',
    'TimestepDuration',
    'createBrain',
    'createDefaultConfig',
]

__version__ = "0.1.0"
