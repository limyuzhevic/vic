#!/usr/bin/env python3

"""
NLM Brain - Neural Network Brain Simulation

This module provides the Brain class for neural network simulation,
including neuron management, synaptic connections, and simulation execution.
"""

from typing import Dict, List, Optional, Any, Union, Callable
import time
import json
import pickle
import numpy as np
from enum import Enum

from nlm.core.types import NeuronId, SynapseId, RegionId, PopulationId, NeuronType, SynapseType
from nlm.core.config import Config
from nlm.core.sensory import Vision, Audio, InternalSignals
from nlm.core.action import Action, MotorCommand
from nlm.serialization.state import StateManager
from nlm.monitoring.real_time import RealTimeMonitor

class NLMError(Exception):
    """Base exception for NLM operations."""
    
    def __init__(self, message: str, error_code: str = "NLM_ERROR", details: Optional[Dict] = None):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.details = details or {}
        self.timestamp = time.time()
    
    def __str__(self):
        return f"[{self.error_code}] {self.message}"

class ConfigError(NLMError):
    """Configuration error."""
    
    def __init__(self, message: str, config_path: Optional[str] = None, field: Optional[str] = None):
        super().__init__(message, "CONFIG_ERROR", {
            "config_path": config_path,
            "field": field,
            "timestamp": time.time()
        })

class BenchmarkError(NLMError):
    """Benchmark error."""
    
    def __init__(self, message: str, benchmark_type: Optional[str] = None):
        super().__init__(message, "BENCHMARK_ERROR", {
            "benchmark_type": benchmark_type,
            "timestamp": time.time()
        })

class SerializationError(NLMError):
    """Serialization error."""
    
    def __init__(self, message: str, filepath: Optional[str] = None, format: Optional[str] = None):
        super().__init__(message, "SERIALIZATION_ERROR", {
            "filepath": filepath,
            "format": format,
            "timestamp": time.time()
        })

class VisualizationError(NLMError):
    """Visualization error."""
    
    def __init__(self, message: str, visualization_type: Optional[str] = None):
        super().__init__(message, "VISUALIZATION_ERROR", {
            "visualization_type": visualization_type,
            "timestamp": time.time()
        })

class MonitoringError(NLMError):
    """Monitoring error."""
    
    def __init__(self, message: str, monitoring_type: Optional[str] = None):
        super().__init__(message, "MONITORING_ERROR", {
            "monitoring_type": monitoring_type,
            "timestamp": time.time()
        })

class NLMContextManager:
    """Context manager for NLM resources."""
    
    def __init__(self, auto_save: bool = True, auto_profiling: bool = False):
        self.auto_save = auto_save
        self.auto_profiling = auto_profiling
        self.session_id = id(self)
        self.start_time = None
        self.brain = None
        self.experiment = None
        self.monitor = None
    
    def __enter__(self):
        self.start_time = time.time()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        duration = time.time() - self.start_time if self.start_time else 0
        
        if exc_type:
            print(f"NLM Session {self.session_id} ended with error: {exc_val}")
        else:
            print(f"NLM Session {self.session_id} completed successfully in {duration:.2f}s")
        
        if self.auto_save and self.brain:
            self.save_session()
    
    def create_brain(self, config: Config) -> 'Brain':
        """Create a brain for this session."""
        self.brain = Brain(config)
        self.brain.initialize()
        return self.brain
    
    def create_monitor(self) -> RealTimeMonitor:
        """Create a monitor for this session."""
        self.monitor = RealTimeMonitor()
        self.monitor.start()
        return self.monitor
    
    def save_session(self, prefix: str = "session") -> tuple:
        """Save current session state."""
        timestamp = time.time()
        brain_file = f"{prefix}_{self.session_id}_brain.pkl"
        
        if self.brain:
            save_brain(self.brain, brain_file)
        
        return brain_file
    
    def load_session(self, brain_file: str) -> 'Brain':
        """Load session state."""
        if brain_file:
            self.brain = load_brain(brain_file)
        
        return self.brain

# Global convenience functions
def run_performance_benchmark(brain: 'Brain', steps: int = 1000, 
                           neuron_count: Optional[int] = None) -> Dict:
    """
    Run a performance benchmark on the brain.
    
    Args:
        brain: The brain to benchmark
        steps: Number of simulation steps to run
        neuron_count: Target neuron count (will create new brain if different)
    
    Returns:
        Dictionary containing benchmark metrics
    """
    if neuron_count and brain.config.get("brain.neuron_count", 0) != neuron_count:
        brain = create_brain_with_config(neuron_count)
    
    start_time = time.time()
    metrics = {
        "start_time": start_time,
        "steps": steps,
        "neurons_per_second": 0,
        "memory_usage_mb": 0,
        "avg_firing_rate": 0,
        "active_neurons": 0
    }
    
    for step in range(steps):
        brain.step(step)
        if step > 0:
            elapsed = time.time() - start_time
            metrics["neurons_per_second"] = brain.config.get("brain.neuron_count", 0) / elapsed
    
    metrics["end_time"] = time.time()
    metrics["total_time"] = metrics["end_time"] - metrics["start_time"]
    
    return metrics

def save_experiment_state(experiment: Dict, filepath: str) -> None:
    """
    Save experiment state to file.
    
    Args:
        experiment: Dictionary containing experiment data
        filepath: Path to save the experiment state
    """
    try:
        with open(filepath, 'w') as f:
            json.dump(experiment, f, indent=2, default=str)
    except Exception as e:
        raise SerializationError(f"Failed to save experiment state: {e}", filepath)

def load_experiment_state(filepath: str) -> Dict:
    """
    Load experiment state from file.
    
    Args:
        filepath: Path to load the experiment state from
    
    Returns:
        Dictionary containing experiment data
    """
    try:
        with open(filepath, 'r') as f:
            return json.load(f)
    except Exception as e:
        raise SerializationError(f"Failed to load experiment state: {e}", filepath)

def create_brain_snapshot(brain: 'Brain', filepath: str, format: str = "json") -> None:
    """
    Create a snapshot of the brain's current state.
    
    Args:
        brain: The brain to snapshot
        filepath: Path to save the snapshot
        format: Format of the snapshot (json, binary, etc.)
    """
    try:
        if format == "json":
            snapshot = brain.get_state()
            with open(filepath, 'w') as f:
                json.dump(snapshot, f, indent=2, default=str)
        else:
            with open(filepath, 'wb') as f:
                pickle.dump(brain.get_state(), f)
    except Exception as e:
        raise SerializationError(f"Failed to create brain snapshot: {e}", filepath, format)

def analyze_brain_dynamics(brain: 'Brain', window: int = 100) -> Dict:
    """
    Analyze brain dynamics over a time window.
    
    Args:
        brain: The brain to analyze
        window: Time window for analysis
    
    Returns:
        Dictionary containing analysis results
    """
    metrics = {
        "firing_rate": brain.compute_metrics()['avg_firing_rate'],
        "active_neurons": brain.compute_metrics()['active_neurons'],
        "network_activity": 0.0,
        "stability": 0.0
    }
    
    return metrics

def get_standard_benchmarks() -> Dict:
    """Get standard benchmark configurations for different scales."""
    return {
        "small": {"neurons": 1000, "steps": 1000},
        "medium": {"neurons": 10000, "steps": 5000},
        "large": {"neurons": 100000, "steps": 10000},
        "huge": {"neurons": 1000000, "steps": 50000},
    }

def get_visualization_presets() -> Dict:
    """Get common visualization presets."""
    return {
        "network": "neural_network",
        "activity": "firing_patterns",
        "spikes": "spike_raster",
        "weights": "synaptic_weights",
        "trajectories": "neuron_trajectories",
        "memory": "memory_system",
        "development": "developmental_stages",
    }

# Factory functions
def create_brain_with_config(neuron_count: int = 10000, 
                           connection_probability: float = 0.1) -> 'Brain':
    """Create a brain with specified configuration."""
    config = Config()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.connection_probability", connection_probability)
    
    brain = Brain(config)
    brain.initialize()
    return brain

def create_session(auto_save: bool = True, auto_profiling: bool = False) -> NLMContextManager:
    """Create a new NLM session."""
    return NLMContextManager(auto_save, auto_profiling)

# Quick access helpers
brain = type('BrainProxy', (), {})()
brain.create = create_brain_with_config
brain.save = save_brain
brain.load = load_brain
brain.benchmark = run_performance_benchmark

config = type('ConfigProxy', (), {})()
config.create = Config
config.load = lambda f: Config().load_from_file(f)
config.save = lambda c, f: c.save_to_file(f)

__all__ = [
    "Brain",
    "NLMError",
    "ConfigError", 
    "BenchmarkError",
    "SerializationError",
    "VisualizationError",
    "MonitoringError",
    "NLMContextManager",
    "run_performance_benchmark",
    "save_experiment_state",
    "load_experiment_state", 
    "create_brain_snapshot",
    "analyze_brain_dynamics",
    "get_standard_benchmarks",
    "get_visualization_presets",
    "create_brain_with_config",
    "create_session",
    "brain",
    "config",
    "__version__"
]