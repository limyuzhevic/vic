"""
Enhanced types and utilities for NLM Python interface.

This module provides enhanced data types, configuration classes, and utility
functions with comprehensive type hints and documentation.
"""

from dataclasses import dataclass, field
from typing import Optional, List, Dict, Any, Union
from enum import Enum

from pynlm import (
    NeuronType, DevelopmentalStage, WorldObjectType,
    ActionType, MotorCommand, SynapseType
)

__all__ = [
    'LearningMetrics', 'SimulationConfig', 'PerformanceProfiler',
    'SimulationMonitor', 'EnvironmentBuilder'
]


@dataclass
class LearningMetrics:
    """Comprehensive metrics for tracking brain learning and performance."""
    
    # Basic statistics
    total_steps: int = 0
    total_neurons: int = 0
    total_synapses: int = 0
    firing_rate: float = 0.0
    e_i_ratio: float = 0.0
    
    # Learning indicators
    synaptic_change_magnitude: float = 0.0
    plasticity_events: int = 0
    exploration_rate: float = 0.0
    prediction_accuracy: float = 0.0
    
    # Performance metrics
    simulation_time: float = 0.0
    steps_per_second: float = 0.0
    memory_usage: float = 0.0
    
    # Neuromodulation levels
    dopamine_level: float = 0.0
    curiosity_level: float = 0.0
    novelty_level: float = 0.0
    prediction_error: float = 0.0
    
    # Developmental stage
    developmental_stage: DevelopmentalStage = DevelopmentalStage.Initial
    developmental_age: float = 0.0
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert metrics to dictionary for serialization."""
        return {
            "total_steps": self.total_steps,
            "total_neurons": self.total_neurons,
            "total_synapses": self.total_synapses,
            "firing_rate": self.firing_rate,
            "e_i_ratio": self.e_i_ratio,
            "synaptic_change_magnitude": self.synaptic_change_magnitude,
            "plasticity_events": self.plasticity_events,
            "exploration_rate": self.exploration_rate,
            "prediction_accuracy": self.prediction_accuracy,
            "simulation_time": self.simulation_time,
            "steps_per_second": self.steps_per_second,
            "memory_usage": self.memory_usage,
            "dopamine_level": self.dopamine_level,
            "curiosity_level": self.curiosity_level,
            "novelty_level": self.novelty_level,
            "prediction_error": self.prediction_error,
            "developmental_stage": str(self.developmental_stage),
            "developmental_age": self.developmental_age
        }
    
    def is_learning(self, threshold: float = 0.01) -> bool:
        """Check if significant learning has occurred."""
        return (self.synaptic_change_magnitude > threshold or 
                self.plasticity_events > 0)
    
    def get_performance_grade(self) -> str:
        """Get a performance grade based on metrics."""
        if self.steps_per_second > 10000 and self.firing_rate > 0.1:
            return "Excellent"
        elif self.steps_per_second > 5000 and self.firing_rate > 0.05:
            return "Good"
        elif self.steps_per_second > 1000:
            return "Fair"
        else:
            return "Needs Improvement"


@dataclass
class SimulationConfig:
    """Advanced simulation configuration with validation."""
    
    # Core simulation parameters
    neuron_count: int = 1000
    region_count: int = 1
    simulation_timestep: float = 0.001
    total_steps: int = 1000
    connection_probability: float = 0.1
    
    # Learning parameters
    learning_rate: float = 0.001
    stdp_ltp_weight: float = 0.02
    stdp_ltd_weight: float = 0.015
    stdp_tau: float = 20.0
    
    # Plasticity settings
    enable_hebbian: bool = True
    enable_stdp: bool = True
    enable_structural_plasticity: bool = True
    synaptogenesis_rate: float = 0.0001
    pruning_rate: float = 0.00001
    
    # Neuromodulation
    enable_dopamine: bool = True
    enable_curiosity: bool = True
    enable_novelty: bool = True
    dopamine_scale: float = 1.0
    curiosity_threshold: float = 0.1
    novelty_threshold: float = 0.05
    
    # Development
    enable_development: bool = True
    critical_period_duration: float = 100.0
    maturation_rate: float = 0.01
    
    # Monitoring
    monitor_performance: bool = True
    monitor_learning: bool = True
    monitor_neuromodulation: bool = True
    snapshot_interval: int = 100
    
    # Environment
    world_width: int = 20
    world_height: int = 20
    vision_width: int = 8
    vision_height: int = 8
    max_energy: float = 100.0
    energy_decay_rate: float = 0.01
    
    # Validation constraints
    min_neuron_count: int = 10
    max_neuron_count: int = 10000
    min_connection_probability: float = 0.001
    max_connection_probability: float = 0.5
    
    def validate(self) -> List[str]:
        """Validate configuration and return list of errors."""
        errors = []
        
        # Validate neuron count
        if self.neuron_count < self.min_neuron_count:
            errors.append(f"neuron_count ({self.neuron_count}) < minimum ({self.min_neuron_count})")
        if self.neuron_count > self.max_neuron_count:
            errors.append(f"neuron_count ({self.neuron_count}) > maximum ({self.max_neuron_count})")
        
        # Validate connection probability
        if self.connection_probability < self.min_connection_probability:
            errors.append(f"connection_probability ({self.connection_probability}) < minimum ({self.min_connection_probability})")
        if self.connection_probability > self.max_connection_probability:
            errors.append(f"connection_probability ({self.connection_probability}) > maximum ({self.max_connection_probability})")
        
        # Validate timesteps
        if self.simulation_timestep <= 0:
            errors.append(f"simulation_timestep ({self.simulation_timestep}) must be positive")
        
        # Validate rates
        if self.synaptogenesis_rate < 0 or self.synaptogenesis_rate > 1:
            errors.append(f"synaptogenesis_rate ({self.synaptogenesis_rate}) must be between 0 and 1")
        
        if self.pruning_rate < 0 or self.pruning_rate > 1:
            errors.append(f"pruning_rate ({self.pruning_rate}) must be between 0 and 1")
        
        return errors
    
    def apply_to_config(self, config: Any) -> None:
        """Apply simulation configuration to NLM config object."""
        # This method would apply the configuration to the underlying C++ Config object
        # For now, it's a placeholder
        pass


class PerformanceProfiler:
    """Performance profiling and benchmarking for NLM simulations."""
    
    def __init__(self):
        self.samples: List[Dict[str, Any]] = []
        self.start_time: Optional[float] = None
        self.step_times: List[float] = []
        self.memory_usage: List[float] = []
    
    def start_profiling(self) -> None:
        """Start performance profiling."""
        import time
        self.start_time = time.time()
        self.step_times.clear()
        self.memory_usage.clear()
        self.samples.clear()
    
    def step(self, brain: Any) -> Dict[str, Any]:
        """Profile a single simulation step."""
        import time
        if self.start_time is None:
            return {}
        
        step_start = time.time()
        
        # Collect metrics
        metrics = {
            "step_time": time.time() - step_start,
            "neuron_count": brain.get_total_neuron_count(),
            "synapse_count": brain.get_total_synapse_count(),
            "firing_rate": brain.get_average_firing_rate(),
            "e_i_ratio": brain.get_excitation_inhibition_ratio(),
            "total_spikes": brain.get_total_spike_count(),
        }
        
        self.step_times.append(metrics["step_time"])
        self.samples.append(metrics)
        
        return metrics
    
    def stop_profiling(self) -> Dict[str, Any]:
        """Stop profiling and return summary statistics."""
        import time
        if self.start_time is None:
            return {}
        
        total_time = time.time() - self.start_time
        
        summary = {
            "total_time": total_time,
            "steps_profiled": len(self.samples),
            "avg_step_time": sum(self.step_times) / len(self.step_times) if self.step_times else 0,
            "min_step_time": min(self.step_times) if self.step_times else 0,
            "max_step_time": max(self.step_times) if self.step_times else 0,
            "std_step_time": (sum((x - sum(self.step_times)/len(self.step_times))**2 
                                  for x in self.step_times) / len(self.step_times))**0.5 
                              if len(self.step_times) > 1 else 0,
            "steps_per_second": len(self.samples) / total_time if total_time > 0 else 0,
        }
        
        self.start_time = None
        return summary
    
    def get_efficiency_rating(self) -> str:
        """Get efficiency rating based on performance."""
        if not self.samples:
            return "No data"
        
        avg_step_time = sum(self.step_times) / len(self.step_times)
        steps_per_sec = 1.0 / avg_step_time if avg_step_time > 0 else 0
        
        if steps_per_sec > 10000:
            return "Excellent"
        elif steps_per_sec > 5000:
            return "Good"
        elif steps_per_sec > 1000:
            return "Fair"
        else:
            return "Needs Improvement"


class SimulationMonitor:
    """Real-time monitoring and visualization of NLM simulations."""
    
    def __init__(self, brain: Any, world: Optional[Any] = None):
        self.brain = brain
        self.world = world
        self.history: List[Dict[str, Any]] = []
        self.enabled = True
    
    def step(self) -> Dict[str, Any]:
        """Monitor a simulation step and collect metrics."""
        if not self.enabled:
            return {}
        
        metrics = {
            "step": len(self.history),
            "neuron_count": self.brain.get_total_neuron_count(),
            "synapse_count": self.brain.get_total_synapse_count(),
            "firing_rate": self.brain.get_average_firing_rate(),
            "e_i_ratio": self.brain.get_excitation_inhibition_ratio(),
            "total_spikes": self.brain.get_total_spike_count(),
            "developmental_stage": str(self.brain.get_developmental_stage()),
        }
        
        if self.world:
            percept = self.world.get_sensory_percept()
            metrics.update({
                "vision_active": percept.get_vision() is not None,
            })
        
        self.history.append(metrics)
        return metrics
    
    def get_summary(self) -> Dict[str, Any]:
        """Get summary statistics of monitored simulation."""
        if not self.history:
            return {}
        
        metrics = [h for h in self.history if h]
        
        return {
            "total_steps": len(metrics),
            "avg_firing_rate": sum(m["firing_rate"] for m in metrics) / len(metrics),
            "avg_e_i_ratio": sum(m["e_i_ratio"] for m in metrics) / len(metrics),
            "total_spikes": sum(m["total_spikes"] for m in metrics),
            "development_progress": len(set(m["developmental_stage"] for m in metrics)),
            "final_stage": metrics[-1]["developmental_stage"] if metrics else "Unknown"
        }
    
    def save_history(self, filepath: str) -> None:
        """Save monitoring history to JSON file."""
        import json
        with open(filepath, 'w') as f:
            json.dump(self.history, f, indent=2)
    
    def load_history(self, filepath: str) -> None:
        """Load monitoring history from JSON file."""
        import json
        with open(filepath, 'r') as f:
            self.history = json.load(f)


class EnvironmentBuilder:
    """Builder class for creating complex simulation environments."""
    
    def __init__(self):
        self.config = {}
        self.objects = []
        self.agent_start = (0.0, 0.0)
    
    def set_dimensions(self, width: int, height: int) -> 'EnvironmentBuilder':
        """Set world dimensions."""
        self.config['width'] = width
        self.config['height'] = height
        return self
    
    def set_vision(self, vision_width: int, vision_height: int) -> 'EnvironmentBuilder':
        """Set vision sensor dimensions."""
        self.config['vision_width'] = vision_width
        self.config['vision_height'] = vision_height
        return self
    
    def add_resource(self, x: float, y: float, value: float = 1.0) -> 'EnvironmentBuilder':
        """Add a resource object to the environment."""
        self.objects.append({
            'type': 'resource',
            'x': x,
            'y': y,
            'value': value,
            'radius': 0.5
        })
        return self
    
    def add_hazard(self, x: float, y: float, damage: float = 0.5) -> 'EnvironmentBuilder':
        """Add a hazard object to the environment."""
        self.objects.append({
            'type': 'hazard',
            'x': x,
            'y': y,
            'value': -damage,
            'radius': 0.3
        })
        return self
    
    def add_wall(self, x: float, y: float) -> 'EnvironmentBuilder':
        """Add a wall object to the environment."""
        self.objects.append({
            'type': 'wall',
            'x': x,
            'y': y,
            'radius': 0.2
        })
        return self
    
    def set_agent_start(self, x: float, y: float) -> 'EnvironmentBuilder':
        """Set agent starting position."""
        self.agent_start = (x, y)
        return self
    
    def build_world(self) -> Any:
        """Build and return the configured world."""
        # This would create the world with the configured objects
        # For now, it's a placeholder
        world = SimpleWorld()
        # Configure world based on self.config
        return world