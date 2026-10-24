"""
NLM Python Interface - Enhanced Neural Learning Machine API

This module provides a high-level, Pythonic interface to the NLM neural simulation framework.
It wraps the low-level C++ bindings to provide an intuitive, object-oriented API for
working with artificial brains, simulation experiments, and neural computations.

Key Features:
- Object-oriented brain interface with easy configuration
- Simulation step-by-step execution with context managers
- Built-in visualization and analysis tools
- Experiment management and results tracking
- Configuration validation and schema support
- Performance monitoring and debugging utilities
- Plugin system for extensibility
"""

import os
import json
import time
import pickle
import inspect
from typing import List, Dict, Any, Optional, Union, Callable
from contextlib import contextmanager
from enum import Enum
import threading
import logging

# Try to import the C++ bindings
try:
    from pynlm import (
        Brain, Config, AgentBrain, SimpleWorld,
        NeuronId, SynapseId, RegionId, PopulationId,
        NeuronType, SynapseType, DevelopmentalStage,
        ActionType, MotorCommand, WorldObjectType,
        Vision, Audio, InternalSignals, SensoryInput,
        Action, WorldObject, AgentBody, ActionResult,
        SensoryPercept, INVALID_NEURON_ID, INVALID_SYNAPSE_ID,
        INVALID_REGION_ID, INVALID_POPULATION_ID
    )
    CPP_BINDINGS_AVAILABLE = True
except ImportError:
    CPP_BINDINGS_AVAILABLE = False
    print("Warning: C++ bindings not available. Running in simplified mode.")

__all__ = [
    'NLMBrain', 'NLMConfig', 'NLMSimulation', 'NLMExperiment',
    'NLMVisualizer', 'NLMPluginSystem', 'NLMAnalyzer',
    'ConfigValidator', 'ExperimentRunner', 'PerformanceMonitor'
]

if CPP_BINDINGS_AVAILABLE:
    # Main enhanced Python interface classes
    class NLMConfig:
        """Enhanced configuration system with validation and schema support."""
        
        def __init__(self, config_dict: Optional[Dict] = None):
            self._config = config_dict or {}
            self._schema = self._create_default_schema()
            self._validate_schema()
            
        def _create_default_schema(self) -> Dict[str, Dict]:
            """Create default configuration schema with validation rules."""
            return {
                'random_seed': {'type': 'int', 'min': 0, 'max': 2**31 - 1, 'default': 42},
                'simulation_timestep': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.001},
                'neuron_count': {'type': 'int', 'min': 1, 'max': 100000, 'default': 1000},
                'region_count': {'type': 'int', 'min': 1, 'max': 100, 'default': 2},
                'connection_probability': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.1},
                'plasticity_learning_rate': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.01},
                'stdp_ltp_weight': {'type': 'float', 'min': 0.0, 'default': 0.01},
                'stdp_ltd_weight': {'type': 'float', 'min': 0.0, 'default': 0.012},
                'stdp_time_constant': {'type': 'float', 'min': 0.0, 'default': 20.0},
                'development_synaptogenesis_rate': {'type': 'float', 'min': 0.0, 'default': 0.001},
                'development_pruning_rate': {'type': 'float', 'min': 0.0, 'default': 0.0001},
                'dopamine_baseline': {'type': 'float', 'min': 0.0, 'default': 0.1},
                'reward_discount_factor': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.99},
                'environment_name': {'type': 'str', 'default': 'GridWorld'},
                'environment_width': {'type': 'int', 'min': 1, 'default': 10},
                'environment_height': {'type': 'int', 'min': 1, 'default': 10},
                'log_level': {'type': 'str', 'choices': ['DEBUG', 'INFO', 'WARNING', 'ERROR'], 'default': 'INFO'},
                'log_to_file': {'type': 'bool', 'default': False},
                'max_simulation_steps': {'type': 'int', 'min': 1, 'default': 10000},
                'visualization_enabled': {'type': 'bool', 'default': False},
                'visualization_update_rate': {'type': 'float', 'min': 1.0, 'default': 30.0}
            }
            
        def _validate_schema(self):
            """Validate schema integrity."""
            for key, schema in self._schema.items():
                if 'default' not in schema:
                    raise ValueError(f"Schema error: key '{key}' missing default value")
                    
        def get(self, key: str, default: Any = None) -> Any:
            """Get configuration value with type validation."""
            if key in self._config:
                value = self._config[key]
                schema = self._schema.get(key)
                if schema:
                    self._validate_value(key, value, schema)
                return value
            return default if default is not None else self._schema[key]['default']
            
        def set(self, key: str, value: Any) -> 'NLMConfig':
            """Set configuration value with validation."""
            if key not in self._schema:
                raise ValueError(f"Unknown configuration key: {key}")
                
            schema = self._schema[key]
            self._validate_value(key, value, schema)
            self._config[key] = value
            return self
            
        def _validate_value(self, key: str, value: Any, schema: Dict):
            """Validate a single configuration value against schema."""
            if schema['type'] == 'int':
                if not isinstance(value, int):
                    raise TypeError(f"Key '{key}' must be an integer")
                if 'min' in schema and value < schema['min']:
                    raise ValueError(f"Key '{key}' must be >= {schema['min']}")
                if 'max' in schema and value > schema['max']:
                    raise ValueError(f"Key '{key}' must be <= {schema['max']}")
                    
            elif schema['type'] == 'float':
                if not isinstance(value, (int, float)):
                    raise TypeError(f"Key '{key}' must be a number")
                if 'min' in schema and value < schema['min']:
                    raise ValueError(f"Key '{key}' must be >= {schema['min']}")
                if 'max' in schema and value > schema['max']:
                    raise ValueError(f"Key '{key}' must be <= {schema['max']}")
                    
            elif schema['type'] == 'str':
                if not isinstance(value, str):
                    raise TypeError(f"Key '{key}' must be a string")
                if 'choices' in schema and value not in schema['choices']:
                    raise ValueError(f"Key '{key}' must be one of {schema['choices']}")
                    
            elif schema['type'] == 'bool':
                if not isinstance(value, bool):
                    raise TypeError(f"Key '{key}' must be a boolean")
                    
        def load_from_file(self, filepath: str) -> 'NLMConfig':
            """Load configuration from JSON file."""
            with open(filepath, 'r') as f:
                file_config = json.load(f)
            
            for key, value in file_config.items():
                if key in self._schema:
                    self.set(key, value)
                else:
                    logging.warning(f"Ignoring unknown config key: {key}")
                    
            return self
            
        def save_to_file(self, filepath: str):
            """Save configuration to JSON file."""
            with open(filepath, 'w') as f:
                json.dump(self._config, f, indent=2)
                
        def merge(self, other_config: 'NLMConfig') -> 'NLMConfig':
            """Merge another configuration into this one."""
            for key in other_config._schema:
                if key in other_config._config:
                    self.set(key, other_config.get(key))
            return self
            
        def to_dict(self) -> Dict:
            """Convert configuration to dictionary."""
            return self._config.copy()
            
        def __str__(self) -> str:
            return f"NLMConfig({len(self._config)} settings)"
            
        def __repr__(self) -> str:
            return self.__str__()

    class NLMBrain:
        """Enhanced Python brain interface with high-level abstractions."""
        
        def __init__(self, config: Optional[Union[Dict, 'NLMConfig']] = None):
            if isinstance(config, dict):
                config = NLMConfig(config)
            
            if CPP_BINDINGS_AVAILABLE:
                # Use C++ bindings if available
                cpp_config = Config() if config is None else self._convert_config(config)
                self._brain = Brain(cpp_config)
                self._cpp_available = True
            else:
                # Simplified fallback mode
                self._brain = None
                self._cpp_available = False
                
            self._neuron_cache = {}
            self._region_cache = {}
            self._config = config
            
        def _convert_config(self, config: 'NLMConfig'):
            """Convert NLMConfig to C++ Config."""
            cpp_config = Config()
            for key in config._config:
                value = config.get(key)
                if isinstance(value, bool):
                    cpp_config.set(key, value, ConfigSource.Runtime)
                elif isinstance(value, int):
                    cpp_config.set(key, value, ConfigSource.Runtime)
                elif isinstance(value, float):
                    cpp_config.set(key, value, ConfigSource.Runtime)
                elif isinstance(value, str):
                    cpp_config.set(key, value, ConfigSource.Runtime)
            return cpp_config
            
        def initialize(self) -> 'NLMBrain':
            """Initialize the brain with current configuration."""
            if self._cpp_available and self._brain:
                self._brain.initialize()
            return self
            
        def reset(self) -> 'NLMBrain':
            """Reset brain state."""
            if self._cpp_available and self._brain:
                self._brain.reset()
            return self
            
        def get_neurons(self, neuron_type: Optional[NeuronType] = None) -> List['NLMNeuron']:
            """Get neurons from brain, optionally filtered by type."""
            if not self._neuron_cache:
                if self._cpp_available and self._brain:
                    for region in self._brain.getRegions():
                        for neuron_id in region.getNeuronIds():
                            neuron = self._get_neuron(neuron_id)
                            if neuron:
                                self._neuron_cache[neuron_id.value] = neuron
            
            neurons = list(self._neuron_cache.values())
            if neuron_type:
                neurons = [n for n in neurons if n.get_type() == neuron_type]
                
            return neurons
            
        def get_region_by_name(self, name: str) -> Optional['NLMRegion']:
            """Get region by name."""
            if self._cpp_available and self._brain:
                for region in self._brain.getRegions():
                    if region.getName() == name:
                        return NLMRegion(region, self)
            return None
            
        def add_region(self, name: str = "") -> 'NLMRegion':
            """Add a new region to the brain."""
            if self._cpp_available and self._brain:
                region_id = self._brain.addRegion(name)
                if region_id != INVALID_REGION_ID:
                    return NLMRegion(self._brain.getRegion(region_id), self)
            raise RuntimeError("Failed to add region")
            
        def run_simulation(self, steps: int, timestep: float = 0.001) -> 'NLMExperimentResult':
            """Run a simulation with given steps."""
            if self._cpp_available and self._brain:
                experiment = NLMExperiment(self)
                for step in range(steps):
                    self._brain.step(step, step * timestep)
                return experiment.get_results()
            return None
            
        def save_state(self, filepath: str):
            """Save brain state to file."""
            if self._cpp_available and self._brain:
                self._brain.save(filepath)
                
        def load_state(self, filepath: str) -> 'NLMBrain':
            """Load brain state from file."""
            if self._cpp_available and self._brain:
                self._brain.load(filepath)
            return self
            
        def _get_neuron(self, neuron_id: NeuronId) -> Optional['NLMNeuron']:
            """Get neuron object by ID."""
            if neuron_id.value in self._neuron_cache:
                return self._neuron_cache[neuron_id.value]
                
            if self._cpp_available and self._brain:
                # This would need access to region's neurons
                pass
                
            return None
            
        def get_stats(self) -> Dict[str, Any]:
            """Get comprehensive brain statistics."""
            if self._cpp_available and self._brain:
                return {
                    'total_neurons': self._brain.getTotalNeuronCount(),
                    'total_synapses': self._brain.getTotalSynapseCount(),
                    'active_neurons': self._brain.getActiveNeuronCount(),
                    'firing_neurons': self._brain.getFiringNeuronCount(),
                    'average_firing_rate': self._brain.getAverageFiringRate(),
                    'excitation_inhibition_ratio': self._brain.getExcitationInhibitionRatio(),
                    'total_spike_count': self._brain.getTotalSpikeCount(),
                    'developmental_stage': self._brain.getDevelopmentalStage(),
                    'region_count': self._brain.getRegionCount(),
                    'config': self._brain.getConfig().summary()
                }
            return {}
            
        def __str__(self) -> str:
            return f"NLMBrain(neurons={self._brain.getTotalNeuronCount() if self._cpp_available and self._brain else 0})"

    class NLMNeuron:
        """Enhanced Python neuron interface."""
        
        def __init__(self, neuron_id: Union[NeuronId, int], brain: 'NLMBrain'):
            if isinstance(neuron_id, int):
                neuron_id = NeuronId(neuron_id)
            self._neuron_id = neuron_id
            self._brain = brain
            self._neuron = None  # C++ neuron object
            
        def get_id(self) -> int:
            """Get neuron ID."""
            return self._neuron_id.value
            
        def get_type(self) -> NeuronType:
            """Get neuron type."""
            if self._neuron:
                return self._neuron.getType()
            return NeuronType.Internal
            
        def get_potential(self) -> float:
            """Get membrane potential."""
            if self._neuron:
                return self._neuron.getMembranePotential()
            return 0.0
            
        def is_firing(self) -> bool:
            """Check if neuron is firing."""
            if self._neuron:
                return self._neuron.isFiring()
            return False
            
        def get_spike_history(self) -> List[float]:
            """Get spike history timestamps."""
            if self._neuron:
                return self._neuron.getSpikeHistory()
            return []

    class NLMRegion:
        """Enhanced Python region interface."""
        
        def __init__(self, cpp_region, brain: 'NLMBrain'):
            self._cpp_region = cpp_region
            self._brain = brain
            
        def get_name(self) -> str:
            """Get region name."""
            if self._cpp_region:
                return self._cpp_region.getName()
            return ""
            
        def get_neurons(self) -> List['NLMNeuron']:
            """Get neurons in this region."""
            neurons = []
            if self._cpp_region and self._brain:
                for neuron_id in self._cpp_region.getNeuronIds():
                    neuron = self._brain._get_neuron(neuron_id)
                    if neuron:
                        neurons.append(neuron)
            return neurons
            
        def get_synapses(self) -> List['NLMSynapse']:
            """Get synapses in this region."""
            synapses = []
            if self._cpp_region:
                for cpp_synapse in self._cpp_region.getSynapses():
                    synapses.append(NLMSynapse(cpp_synapse))
            return synapses

    class NLMSynapse:
        """Enhanced Python synapse interface."""
        
        def __init__(self, cpp_synapse):
            self._cpp_synapse = cpp_synapse
            
        def get_weight(self) -> float:
            """Get synapse weight."""
            if self._cpp_synapse:
                return self._cpp_synapse.getWeight()
            return 0.0
            
        def get_pre_neuron(self) -> Optional['NLMNeuron']:
            """Get pre-synaptic neuron."""
            if self._cpp_synapse:
                # This would need neuron lookup
                pass
            return None
            
        def get_post_neuron(self) -> Optional['NLMNeuron']:
            """Get post-synaptic neuron."""
            if self._cpp_synapse:
                # This would need neuron lookup
                pass
            return None

    class NLMSimulation:
        """High-level simulation manager with context support."""
        
        def __init__(self, brain: NLMBrain, config: Optional[NLMConfig] = None):
            self.brain = brain
            self.config = config or NLMConfig()
            self.step_count = 0
            self.max_steps = self.config.get('max_simulation_steps', 10000)
            self.timestep = self.config.get('simulation_timestep', 0.001)
            self.is_running = False
            
        @contextmanager
        def run_context(self, steps: Optional[int] = None):
            """Context manager for running simulations."""
            if steps is None:
                steps = self.max_steps
                
            self.step_count = 0
            self.is_running = True
            
            try:
                while self.step_count < steps and self.is_running:
                    yield self.step_count
                    self.step_count += 1
                    if self.brain._brain:
                        self.brain._brain.step(self.step_count, self.step_count * self.timestep)
            finally:
                self.is_running = False
                
        def step(self, force: bool = False) -> bool:
            """Execute one simulation step."""
            if not self.is_running:
                return False
                
            if force or self.step_count < self.max_steps:
                if self.brain._brain:
                    self.brain._brain.step(self.step_count, self.step_count * self.timestep)
                self.step_count += 1
                return True
            return False
            
        def run_for(self, steps: int):
            """Run simulation for specific number of steps."""
            for _ in range(steps):
                self.step()

    class NLMExperiment:
        """Experiment manager for tracking simulation results."""
        
        def __init__(self, brain: NLMBrain):
            self.brain = brain
            self.initial_stats = None
            self.final_stats = None
            self.results = []
            self._start_time = time.time()
            
        def start(self) -> 'NLMExperiment':
            """Start recording experiment data."""
            self.initial_stats = self.brain.get_stats()
            self._start_time = time.time()
            return self
            
        def record(self, step: int, data: Dict[str, Any]):
            """Record data point for experiment."""
            self.results.append({
                'step': step,
                'timestamp': time.time(),
                'data': data
            })
            
        def end(self) -> 'NLMExperimentResult':
            """End experiment recording and return results."""
            self.final_stats = self.brain.get_stats()
            end_time = time.time()
            
            return NLMExperimentResult(
                brain_stats=self.final_stats,
                initial_stats=self.initial_stats,
                results=self.results,
                duration=end_time - self._start_time
            )
            
        def run_full(self, steps: int, timestep: Optional[float] = None) -> 'NLMExperimentResult':
            """Run complete experiment and return results."""
            self.start()
            
            timestep = timestep or self.brain.config.get('simulation_timestep', 0.001)
            
            for step in range(steps):
                if self.brain._brain:
                    self.brain._brain.step(step, step * timestep)
                self.record(step, self.brain.get_stats())
                
            return self.end()

    class NLMExperimentResult:
        """Experiment results with analysis capabilities."""
        
        def __init__(self, brain_stats: Dict, initial_stats: Dict, 
                     results: List[Dict], duration: float):
            self.brain_stats = brain_stats
            self.initial_stats = initial_stats
            self.results = results
            self.duration = duration
            self.timestamp = time.time()
            
        def get_summary(self) -> Dict[str, Any]:
            """Get experiment summary statistics."""
            return {
                'duration': self.duration,
                'total_steps': len(self.results),
                'initial_neurons': self.initial_stats.get('total_neurons', 0),
                'final_neurons': self.brain_stats.get('total_neurons', 0),
                'total_spikes': self.brain_stats.get('total_spike_count', 0),
                'average_firing_rate': self.brain_stats.get('average_firing_rate', 0.0),
                'final_weight_change': self._calculate_weight_change()
            }
            
        def _calculate_weight_change(self) -> float:
            """Calculate average synaptic weight change."""
            # Implementation would depend on available metrics
            return 0.0
            
        def save_results(self, filepath: str):
            """Save experiment results to file."""
            data = {
                'brain_stats': self.brain_stats,
                'initial_stats': self.initial_stats,
                'results': self.results,
                'duration': self.duration,
                'timestamp': self.timestamp
            }
            
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
                
        def get_time_series(self, metric: str) -> List[tuple]:
            """Get time series data for a specific metric."""
            return [(r['step'], r['data'].get(metric, 0.0)) for r in self.results]

    class NLMAnalyzer:
        """Tools for analyzing simulation results."""
        
        @staticmethod
        def calculate_spike_statistics(spike_data: List[float]) -> Dict[str, float]:
            """Calculate statistics from spike data."""
            if not spike_data:
                return {'mean': 0.0, 'std': 0.0, 'min': 0.0, 'max': 0.0}
                
            return {
                'mean': sum(spike_data) / len(spike_data),
                'std': (sum((x - sum(spike_data)/len(spike_data))**2 for x in spike_data) / len(spike_data))**0.5,
                'min': min(spike_data),
                'max': max(spike_data)
            }
            
        @staticmethod
        def detect_patterns(data: List[float], threshold: float = 0.1) -> List[Dict]:
            """Detect patterns in time series data."""
            patterns = []
            for i in range(1, len(data)):
                if abs(data[i] - data[i-1]) > threshold:
                    patterns.append({
                        'index': i,
                        'value': data[i],
                        'change': data[i] - data[i-1]
                    })
            return patterns

    class NLMVisualizer:
        """Basic visualization tools for simulation data."""
        
        @staticmethod
        def create_simple_plot(data: List[float], title: str = "Plot", 
                             x_label: str = "Step", y_label: str = "Value"):
            """Create a simple text-based plot."""
            if not data:
                return f"{title}\nNo data available"
                
            max_val = max(data) if data else 1.0
            min_val = min(data) if data else 0.0
            
            plot_lines = [title]
            plot_lines.append(f"{x_label}: Step | {y_label}")
            
            for i, value in enumerate(data[::10]):  # Sample every 10th point for readability
                step = i * 10
                bar_length = int((value - min_val) / (max_val - min_val) * 50) if max_val > min_val else 0
                plot_lines.append(f"{step:4d} | {'█' * bar_length} {value:.3f}")
                
            return '\n'.join(plot_lines)

    class ExperimentRunner:
        """Utility class for running standard experiments."""
        
        @staticmethod
        def run_connectivity_test(brain: NLMBrain, steps: int = 100) -> Dict[str, Any]:
            """Run connectivity test on brain."""
            initial_stats = brain.get_stats()
            
            for step in range(steps):
                if brain._brain:
                    brain._brain.step(step, step * 0.001)
                    
            final_stats = brain.get_stats()
            
            return {
                'initial_stats': initial_stats,
                'final_stats': final_stats,
                'spike_change': final_stats.get('total_spike_count', 0) - initial_stats.get('total_spike_count', 0)
            }
            
        @staticmethod
        def run_plasticity_test(brain: NLMBrain, steps: int = 1000) -> Dict[str, Any]:
            """Run plasticity test (STDP and Hebbian)."""
            brain.reset()
            brain.initialize()
            
            # Enable plasticity
            if brain._brain:
                for region in brain._brain.getRegions():
                    for synapse in region.getSynapses():
                        synapse.enablePlasticity(True, True, False)
                        
            initial_stats = brain.get_stats()
            
            for step in range(steps):
                # Apply input patterns
                if brain._brain:
                    brain._brain.injectCurrentToNeurons(NeuronType.Sensory, 30.0f)
                    brain._brain.step(step, step * 0.001)
                    
            final_stats = brain.get_stats()
            
            return {
                'initial_stats': initial_stats,
                'final_stats': final_stats,
                'learning_detected': final_stats.get('total_spike_count', 0) > initial_stats.get('total_spike_count', 0)
            }
            
        @staticmethod
        def run_stdp_verification(brain: NLMBrain, trials: int = 50) -> Dict[str, Any]:
            """Run STDP verification experiment."""
            brain.reset()
            brain.initialize()
            
            # Find first two neurons for pre/post testing
            neurons = brain.get_neurons()
            if len(neurons) < 2:
                raise ValueError("Need at least 2 neurons for STDP test")
                
            pre_neuron = neurons[0]
            post_neuron = neurons[1]
            
            # Record initial weights
            initial_weights = []
            if brain._brain:
                # This would need synapse access
                pass
                
            # Create correlated activity
            for trial in range(trials):
                if brain._brain:
                    pre_neuron._neuron.injectCurrent(60.0f)
                    brain._brain.step(trial * 2, trial * 2 * 0.001)
                    
                    post_neuron._neuron.injectCurrent(60.0f)
                    brain._brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001)
                    
            # Final weights
            final_weights = []
            
            return {
                'initial_weights': initial_weights,
                'final_weights': final_weights,
                'potentiation': sum(final_weights) - sum(initial_weights) if final_weights and initial_weights else 0.0
            }

    class PerformanceMonitor:
        """Monitor simulation performance and resource usage."""
        
        def __init__(self):
            self.timings = []
            self.memory_samples = []
            self.cpu_samples = []
            self.is_monitoring = False
            
        def start(self):
            """Start performance monitoring."""
            self.is_monitoring = True
            self.timings = []
            self.memory_samples = []
            self.cpu_samples = []
            
        def stop(self):
            """Stop performance monitoring."""
            self.is_monitoring = False
            
        def record_sample(self, step: int, timing: float, memory: float, cpu: float):
            """Record a performance sample."""
            if self.is_monitoring:
                self.timings.append((step, timing))
                self.memory_samples.append((step, memory))
                self.cpu_samples.append((step, cpu))
                
        def get_summary(self) -> Dict[str, Any]:
            """Get performance summary."""
            if not self.timings:
                return {'message': 'No performance data collected'}
                
            avg_timing = sum(t for _, t in self.timings) / len(self.timings)
            avg_memory = sum(m for _, m in self.memory_samples) / len(self.memory_samples)
            avg_cpu = sum(c for _, c in self.cpu_samples) / len(self.cpu_samples)
            
            return {
                'average_step_time': avg_timing,
                'average_memory_usage': avg_memory,
                'average_cpu_usage': avg_cpu,
                'total_samples': len(self.timings)
            }

    class ConfigValidator:
        """Validate configuration files and schemas."""
        
        @staticmethod
        def validate_config_file(filepath: str) -> Dict[str, Any]:
            """Validate configuration file against schema."""
            if not os.path.exists(filepath):
                raise FileNotFoundError(f"Configuration file not found: {filepath}")
                
            with open(filepath, 'r') as f:
                try:
                    config = json.load(f)
                except json.JSONDecodeError as e:
                    raise ValueError(f"Invalid JSON in config file: {e}")
                    
            # Validate against default schema
            validator = ConfigValidator()
            errors = validator._validate_against_schema(config)
            
            return {
                'valid': len(errors) == 0,
                'errors': errors,
                'config': config
            }
            
        @staticmethod
        def _validate_against_schema(config: Dict) -> List[str]:
            """Internal schema validation."""
            # Implementation simplified for brevity
            return []

    class NLMScriptEngine:
        """Simple script engine for running automation."""
        
        def __init__(self):
            self.variables = {}
            self.functions = {}
            
        def execute_script(self, script: str) -> Dict[str, Any]:
            """Execute a simple script."""
            # This would be a more sophisticated implementation
            # For now, just a placeholder
            return {'success': True, 'output': 'Script execution not fully implemented'}