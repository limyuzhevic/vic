# NLM Python Utilities
# Additional Python utilities for working with the NLM (Neural Learning Machine)

"""
This module provides additional Python utilities for working with NLM simulations.
It includes functions for configuration building, simulation running, visualization helpers,
and monitoring tools that enhance the basic Python bindings.
"""

import json
import time
import threading
import numpy as np
from typing import Dict, List, Optional, Any, Callable, Union, Iterator
from dataclasses import dataclass, field
from abc import ABC, abstractmethod
import sys

# Import the main NLM bindings
import nlm_py

__all__ = [
    'SimulationRunner',
    'ConfigBuilder',
    'VisualizationHelper',
    'MetricsCollector',
    'SimulationScheduler',
    'PluginLoader',
    'PerformanceProfiler',
    'EventSystem',
    'StateManager',
    'SimulationExporter',
    'ConfigValidator',
    'FeatureDetector',
    'ValidationError',
    'SimulationError',
    'VisualizationError',
    'PerformanceMetrics',
    'VisualizationConfig',
    'AdvancedConfig',
    'SimulationResult',
    'Event',
    'Metric',
    'PlotConfig'
]

@dataclass
class ValidationError:
    """Error from configuration validation."""
    field: str
    message: str
    value: Any = None
    
    def __str__(self) -> str:
        return f"{self.field}: {self.message}"

@dataclass
class SimulationError(Exception):
    """Error during simulation execution."""
    step: Optional[int] = None
    message: str = ""
    
    def __str__(self) -> str:
        if self.step is not None:
            return f"SimulationError at step {self.step}: {self.message}"
        return f"SimulationError: {self.message}"

@dataclass
class VisualizationError(Exception):
    """Error during visualization."""
    message: str = ""
    
    def __str__(self) -> str:
        return f"VisualizationError: {self.message}"

@dataclass
class PerformanceMetrics:
    """Performance metrics for simulation runs."""
    fps: float = 0.0
    simulation_time: float = 0.0
    memory_usage: float = 0.0
    cpu_utilization: float = 0.0
    spike_count: int = 0
    neuron_count: int = 0
    synapse_count: int = 0
    average_firing_rate: float = 0.0
    active_neurons: int = 0
    firing_neurons: int = 0
    energy_usage: float = 0.0
    steps_completed: int = 0
    metrics_collected: Dict[str, float] = field(default_factory=dict)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert metrics to dictionary."""
        return {
            'fps': self.fps,
            'simulation_time': self.simulation_time,
            'memory_usage': self.memory_usage,
            'cpu_utilization': self.cpu_utilization,
            'spike_count': self.spike_count,
            'neuron_count': self.neuron_count,
            'synapse_count': self.synapse_count,
            'average_firing_rate': self.average_firing_rate,
            'active_neurons': self.active_neurons,
            'firing_neurons': self.firing_neurons,
            'energy_usage': self.energy_usage,
            'steps_completed': self.steps_completed,
            **self.metrics_collected
        }
    
    def to_csv_line(self) -> str:
        """Convert metrics to CSV line."""
        values = [
            self.fps, self.simulation_time, self.memory_usage, self.cpu_utilization,
            self.spike_count, self.neuron_count, self.synapse_count,
            self.average_firing_rate, self.active_neurons, self.firing_neurons,
            self.energy_usage, self.steps_completed
        ]
        return ','.join(str(v) for v in values)

@dataclass
class VisualizationConfig:
    """Configuration for visualization settings."""
    enabled: bool = True
    enable_3d: bool = True
    window_width: int = 1920
    window_height: int = 1080
    show_neurons: bool = True
    show_synapses: bool = True
    show_world: bool = True
    neuron_size: float = 0.1
    synapse_alpha: float = 0.6
    refresh_rate: float = 30.0
    background_color: str = "black"
    neuron_color: str = "red"
    synapse_color: str = "blue"
    colormap: str = "viridis"
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        return {
            'enabled': self.enabled,
            'enable_3d': self.enable_3d,
            'window_width': self.window_width,
            'window_height': self.window_height,
            'show_neurons': self.show_neurons,
            'show_synapses': self.show_synapses,
            'show_world': self.show_world,
            'neuron_size': self.neuron_size,
            'synapse_alpha': self.synapse_alpha,
            'refresh_rate': self.refresh_rate,
            'background_color': self.background_color,
            'neuron_color': self.neuron_color,
            'synapse_color': self.synapse_color,
            'colormap': self.colormap
        }

@dataclass
class AdvancedConfig:
    """Advanced configuration for NLM simulations."""
    # Simulation parameters
    max_steps: int = 10000
    timestep: float = 0.1
    temporal_decay: float = 0.95
    spatial_decay: float = 0.8
    global_connectivity: float = 0.1
    
    # Plasticity parameters
    enable_stdp: bool = True
    enable_hebbian: bool = True
    enable_structural: bool = True
    stdp_learning_rate: float = 0.01
    hebbian_learning_rate: float = 0.005
    
    # Neuromodulation parameters
    enable_reward_modulation: bool = True
    enable_curiosity: bool = True
    enable_prediction_error: bool = True
    neuromodulation_strength: float = 1.0
    
    # Development parameters
    enable_development: bool = True
    development_rate: float = 0.001
    
    # Memory parameters
    enable_working_memory: bool = True
    enable_episodic_memory: bool = True
    memory_capacity: int = 1000
    
    # Performance parameters
    enable_performance_monitoring: bool = True
    enable_profiling: bool = False
    profile_interval: float = 1.0
    
    # Visualization parameters
    enable_visualization: bool = False
    visualization_config: VisualizationConfig = field(default_factory=VisualizationConfig)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        return {
            'max_steps': self.max_steps,
            'timestep': self.timestep,
            'temporal_decay': self.temporal_decay,
            'spatial_decay': self.spatial_decay,
            'global_connectivity': self.global_connectivity,
            'enable_stdp': self.enable_stdp,
            'enable_hebbian': self.enable_hebbian,
            'enable_structural': self.enable_structural,
            'stdp_learning_rate': self.stdp_learning_rate,
            'hebbian_learning_rate': self.hebbian_learning_rate,
            'enable_reward_modulation': self.enable_reward_modulation,
            'enable_curiosity': self.enable_curiosity,
            'enable_prediction_error': self.enable_prediction_error,
            'neuromodulation_strength': self.neuromodulation_strength,
            'enable_development': self.enable_development,
            'development_rate': self.development_rate,
            'enable_working_memory': self.enable_working_memory,
            'enable_episodic_memory': self.enable_episodic_memory,
            'memory_capacity': self.memory_capacity,
            'enable_performance_monitoring': self.enable_performance_monitoring,
            'enable_profiling': self.enable_profiling,
            'profile_interval': self.profile_interval,
            'enable_visualization': self.enable_visualization,
            'visualization_config': self.visualization_config.to_dict()
        }
    
    @classmethod
    def from_dict(cls, config_dict: Dict[str, Any]) -> 'AdvancedConfig':
        """Create AdvancedConfig from dictionary."""
        config = cls()
        for key, value in config_dict.items():
            if hasattr(config, key):
                if key == 'visualization_config' and isinstance(value, dict):
                    config.visualization_config = VisualizationConfig()
                    for vkey, vvalue in value.items():
                        if hasattr(config.visualization_config, vkey):
                            setattr(config.visualization_config, vkey, vvalue)
                else:
                    setattr(config, key, value)
        return config

@dataclass
class SimulationResult:
    """Result from a simulation run."""
    success: bool = False
    steps_completed: int = 0
    total_time: float = 0.0
    metrics: PerformanceMetrics = field(default_factory=PerformanceMetrics)
    final_state: Optional[Any] = None
    error: Optional[Exception] = None
    plugin_results: Dict[str, Any] = field(default_factory=dict)
    
    def is_successful(self) -> bool:
        """Check if simulation was successful."""
        return self.success and self.error is None

@dataclass
class Event:
    """Event in the event system."""
    name: str
    data: Dict[str, Any] = field(default_factory=dict)
    timestamp: float = 0.0
    source: Optional[str] = None
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert event to dictionary."""
        return {
            'name': self.name,
            'data': self.data,
            'timestamp': self.timestamp,
            'source': self.source
        }

@dataclass
class Metric:
    """Metric data point."""
    name: str
    value: float
    timestamp: float = 0.0
    metadata: Dict[str, Any] = field(default_factory=dict)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert metric to dictionary."""
        return {
            'name': self.name,
            'value': self.value,
            'timestamp': self.timestamp,
            'metadata': self.metadata
        }

@dataclass
class PlotConfig:
    """Configuration for plotting."""
    title: str = ""
    x_label: str = "X"
    y_label: str = "Y"
    figure_size: tuple = (10, 6)
    style: str = "seaborn-v0_8"
    colors: List[str] = field(default_factory=lambda: ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728'])
    linewidth: float = 2.0
    marker_size: float = 6.0
    show_grid: bool = True
    save_format: str = "png"
    save_dpi: int = 300

class ConfigValidator:
    """Validates NLM configurations."""
    
    @staticmethod
    def validate_config(config: Dict[str, Any]) -> List[ValidationError]:
        """Validate configuration dictionary."""
        errors = []
        
        # Required fields
        required_fields = ['max_steps', 'timestep', 'temporal_decay']
        for field in required_fields:
            if field not in config:
                errors.append(ValidationError(field, "Required field is missing"))
        
        # Validate value ranges
        if 'max_steps' in config and config['max_steps'] <= 0:
            errors.append(ValidationError('max_steps', "Must be positive"))
        
        if 'timestep' in config and config['timestep'] <= 0:
            errors.append(ValidationError('timestep', "Must be positive"))
        
        if 'temporal_decay' in config and (config['temporal_decay'] < 0 or config['temporal_decay'] > 1):
            errors.append(ValidationError('temporal_decay', "Must be between 0 and 1"))
        
        if 'spatial_decay' in config and config['spatial_decay'] < 0:
            errors.append(ValidationError('spatial_decay', "Must be non-negative"))
        
        return errors
    
    @staticmethod
    def validate_advanced_config(config: AdvancedConfig) -> List[ValidationError]:
        """Validate AdvancedConfig object."""
        errors = []
        
        # Convert to dict for validation
        config_dict = config.to_dict()
        return ConfigValidator.validate_config(config_dict)

class FeatureDetector:
    """Detect available features in the NLM system."""
    
    @staticmethod
    def detect_features() -> Dict[str, bool]:
        """Detect available features."""
        features = {}
        
        # Core features
        features['json_config'] = True
        features['performance_monitoring'] = True
        features['threading'] = True
        features['memory_profiling'] = True
        features['plugin_system'] = True
        features['multi_environment'] = True
        features['advanced_visualization'] = True
        features['event_system'] = True
        features['state_management'] = True
        
        # Optional features (would check actual availability)
        features['gpu_acceleration'] = False
        features['distributed_computing'] = False
        features['real_time_streaming'] = False
        features['virtual_reality'] = False
        features['augmented_reality'] = False
        
        return features
    
    @staticmethod
    def is_feature_available(feature_name: str) -> bool:
        """Check if a specific feature is available."""
        features = FeatureDetector.detect_features()
        return features.get(feature_name, False)

class ConfigBuilder:
    """Helper class for building NLM configurations."""
    
    @staticmethod
    def create_default_config() -> AdvancedConfig:
        """Create a default advanced configuration."""
        return AdvancedConfig()
    
    @staticmethod
    def create_research_config() -> AdvancedConfig:
        """Create a configuration optimized for research."""
        config = AdvancedConfig()
        config.max_steps = 50000
        config.temporal_decay = 0.99
        config.spatial_decay = 0.9
        config.global_connectivity = 0.05
        config.enable_stdp = True
        config.enable_hebbian = True
        config.enable_structural = True
        config.stdp_learning_rate = 0.005
        config.hebbian_learning_rate = 0.002
        config.enable_reward_modulation = True
        config.enable_curiosity = True
        config.enable_prediction_error = True
        config.enable_development = True
        config.development_rate = 0.0005
        config.enable_working_memory = True
        config.enable_episodic_memory = True
        config.memory_capacity = 5000
        config.enable_performance_monitoring = True
        config.enable_profiling = True
        config.profile_interval = 0.5
        config.enable_visualization = False
        return config
    
    @staticmethod
    def create_development_config() -> AdvancedConfig:
        """Create a configuration optimized for development/testing."""
        config = AdvancedConfig()
        config.max_steps = 1000
        config.temporal_decay = 0.9
        config.spatial_decay = 0.7
        config.global_connectivity = 0.2
        config.enable_stdp = True
        config.enable_hebbian = True
        config.enable_structural = False
        config.stdp_learning_rate = 0.01
        config.hebbian_learning_rate = 0.005
        config.enable_reward_modulation = False
        config.enable_curiosity = True
        config.enable_prediction_error = False
        config.enable_development = False
        config.enable_working_memory = False
        config.enable_episodic_memory = False
        config.memory_capacity = 100
        config.enable_performance_monitoring = False
        config.enable_profiling = False
        config.enable_visualization = False
        return config
    
    @staticmethod
    def create_config_from_preset(preset: str) -> AdvancedConfig:
        """Create configuration from preset name."""
        presets = {
            'default': ConfigBuilder.create_default_config,
            'research': ConfigBuilder.create_research_config,
            'development': ConfigBuilder.create_development_config,
        }
        
        if preset not in presets:
            raise ValidationError('preset', f"Unknown preset: {preset}")
        
        return presets[preset]()

class SimulationRunner:
    """Manages and runs NLM simulations."""
    
    def __init__(self, config: Optional[AdvancedConfig] = None):
        self.config = config or ConfigBuilder.create_default_config()
        self.event_system = EventSystem()
        self.state_manager = StateManager()
        self.metrics_collector = MetricsCollector()
        self.simulation_scheduler = SimulationScheduler()
        self.plugin_loader = PluginLoader()
        self.performance_profiler = PerformanceProfiler()
        
        # Connect events
        self.event_system.on('simulation_started', self._on_simulation_started)
        self.event_system.on('simulation_step', self._on_simulation_step)
        self.event_system.on('simulation_completed', self._on_simulation_completed)
        self.event_system.on('simulation_error', self._on_simulation_error)
    
    def run_simulation(self, brain, world, steps: Optional[int] = None) -> SimulationResult:
        """Run a simulation with the given brain and world."""
        result = SimulationResult()
        
        try:
            # Validate configuration
            errors = ConfigValidator.validate_advanced_config(self.config)
            if errors:
                raise SimulationError(f"Configuration validation failed: {[str(e) for e in errors]}")
            
            # Load plugins
            self.plugin_loader.load_all_plugins(brain)
            
            # Enable plugins based on configuration
            for plugin_name, plugin_config in self.config.plugin_configs.items():
                if plugin_config.get('enabled', False):
                    self.plugin_loader.enable_plugin(plugin_name)
            
            # Initialize brain if needed
            if not brain.is_initialized():
                brain.initialize()
            
            # Start performance monitoring
            if self.config.enable_performance_monitoring:
                self.performance_profiler.start()
            
            # Record start time
            start_time = time.time()
            
            # Emit simulation started event
            self.event_system.emit('simulation_started', {
                'brain': brain,
                'world': world,
                'steps': steps or self.config.max_steps
            })
            
            # Run simulation steps
            steps_to_run = steps or self.config.max_steps
            for step in range(steps_to_run):
                try:
                    # Emit simulation step event
                    self.event_system.emit('simulation_step', {
                        'step': step,
                        'brain': brain,
                        'world': world
                    })
                    
                    # Step the brain
                    brain.step(step)
                    
                    # Update world if available
                    if world:
                        world.update(step * self.config.timestep)
                    
                    # Collect metrics
                    self.metrics_collector.collect_metrics(brain, step)
                    
                    # Check for errors
                    if brain.getTotalSpikeCount() > 1000000:  # Arbitrary large spike threshold
                        raise SimulationError(f"High spike count at step {step}", step)
                    
                except Exception as e:
                    # Emit error event
                    self.event_system.emit('simulation_error', {
                        'error': e,
                        'step': step,
                        'brain': brain,
                        'world': world
                    })
                    
                    result.error = e
                    result.success = False
                    break
            
            # Record end time
            end_time = time.time()
            result.total_time = end_time - start_time
            result.steps_completed = steps_to_run
            result.success = True
            
            # Get final metrics
            if self.config.enable_performance_monitoring:
                result.metrics = self.performance_profiler.get_metrics()
            
            # Get plugin results
            result.plugin_results = self.plugin_loader.get_all_results()
            
            # Emit simulation completed event
            self.event_system.emit('simulation_completed', {
                'result': result,
                'brain': brain,
                'world': world
            })
            
        except Exception as e:
            result.error = e
            result.success = False
            self.event_system.emit('simulation_error', {
                'error': e,
                'brain': brain,
                'world': world
            })
        
        return result
    
    def _on_simulation_started(self, event: Event) -> None:
        """Handle simulation started event."""
        pass
    
    def _on_simulation_step(self, event: Event) -> None:
        """Handle simulation step event."""
        pass
    
    def _on_simulation_completed(self, event: Event) -> None:
        """Handle simulation completed event."""
        pass
    
    def _on_simulation_error(self, event: Event) -> None:
        """Handle simulation error event."""
        pass

class MetricsCollector:
    """Collects and manages metrics during simulation."""
    
    def __init__(self):
        self.metrics: List[Metric] = []
        self.current_values: Dict[str, float] = {}
        self.lock = threading.Lock()
    
    def collect_metrics(self, brain, step: int) -> None:
        """Collect metrics from brain at current step."""
        with self.lock:
            # Basic metrics
            self.current_values['step'] = float(step)
            self.current_values['total_spikes'] = float(brain.get_total_spike_count())
            self.current_values['total_neurons'] = float(brain.get_total_neuron_count())
            self.current_values['total_synapses'] = float(brain.get_total_synapse_count())
            self.current_values['average_firing_rate'] = brain.get_average_firing_rate()
            self.current_values['active_neurons'] = float(brain.get_active_neuron_count())
            self.current_values['firing_neurons'] = float(brain.get_firing_neuron_count())
            self.current_values['excitation_inhibition_ratio'] = brain.get_excitation_inhibition_ratio()
            
            # Create metric data point
            metric = Metric(
                name=f'step_{step}',
                value=self.current_values['average_firing_rate'],
                timestamp=time.time(),
                metadata=self.current_values.copy()
            )
            
            self.metrics.append(metric)
    
    def get_metric(self, name: str) -> Optional[Metric]:
        """Get metric by name."""
        for metric in self.metrics:
            if metric.name == name:
                return metric
        return None
    
    def get_metrics_by_name(self, name: str) -> List[Metric]:
        """Get all metrics with given name."""
        return [m for m in self.metrics if m.name == name]
    
    def get_latest_metrics(self) -> Dict[str, float]:
        """Get latest metric values."""
        with self.lock:
            return self.current_values.copy()
    
    def get_all_metrics(self) -> List[Metric]:
        """Get all collected metrics."""
        return self.metrics.copy()
    
    def clear(self) -> None:
        """Clear all metrics."""
        with self.lock:
            self.metrics.clear()
            self.current_values.clear()
    
    def export_to_csv(self, filename: str) -> bool:
        """Export metrics to CSV file."""
        try:
            with open(filename, 'w') as f:
                # Write header
                headers = ['step', 'timestamp', 'name', 'value']
                if self.metrics:
                    headers.extend(self.metrics[0].metadata.keys())
                f.write(','.join(headers) + '\n')
                
                # Write data
                for metric in self.metrics:
                    line = [str(metric.name), str(metric.value), str(metric.timestamp)]
                    for header in headers[3:]:  # Metadata columns
                        if header in metric.metadata:
                            line.append(str(metric.metadata[header]))
                        else:
                            line.append('')
                    f.write(','.join(line) + '\n')
            
            return True
        except Exception:
            return False
    
    def export_to_json(self, filename: str) -> bool:
        """Export metrics to JSON file."""
        try:
            data = {
                'metrics': [metric.to_dict() for metric in self.metrics],
                'latest_values': self.get_latest_metrics()
            }
            
            with open(filename, 'w') as f:
                json.dump(data, f, indent=2)
            
            return True
        except Exception:
            return False

class VisualizationHelper:
    """Helper class for visualization operations."""
    
    @staticmethod
    def create_plot_config(title: str = "", x_label: str = "X", y_label: str = "Y") -> PlotConfig:
        """Create a basic plot configuration."""
        return PlotConfig(title=title, x_label=x_label, y_label=y_label)
    
    @staticmethod
    def create_colormap(name: str, colors: Optional[List[str]] = None) -> List[str]:
        """Create colormap from color names."""
        if colors is None:
            colors = []
        
        # Map common colormap names to colors
        colormap_map = {
            'viridis': ['#440154', '#3b528b', '#30678d', '#2c819b', '#2bb27c', '#48c16f', '#71bd64', '#9bba5a', '#c5b65a', '#eecd5b'],
            'plasma': ['#0d0887', '#4906a5', '#6a13cd', '#8632e7', '#9e4ff3', '#b96afc', '#d386fa', '#e5a3f8', '#f3c1f5', '#fde0ef'],
            'inferno': ['#000004', '#1b0c41', '#4a0c6b', '#7d0e82', '#ac1c7f', '#d62a78', '#fa5069', '#fd8168', '#feb975', '#fee793'],
            'magma': ['#000004', '#1a0733', '#3e134f', '#61266a', '#842178', '#a52a80', '#c75382', '#e67b84', '#fca588', '#ffc98a'],
            'cividis': ['#002654', '#214977', '#3f6c96', '#5c8fba', '#7bb1d7', '#99d4ef', '#b8f7f8', '#ddf8f7', '#fcf7e6', '#f6f2c5']
        }
        
        if name.lower() in colormap_map:
            return colormap_map[name.lower()]
        
        return colors if colors else ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728']
    
    @staticmethod
    def prepare_data_for_plotting(metrics: List[Metric], x_key: str = 'step', y_key: str = 'value') -> Dict[str, Any]:
        """Prepare metric data for plotting."""
        if not metrics:
            return {'x': [], 'y': [], 'metadata': []}
        
        x_values = []
        y_values = []
        metadata = []
        
        for metric in metrics:
            if x_key in metric.metadata:
                x_values.append(metric.metadata[x_key])
            else:
                x_values.append(metric.timestamp)
            
            if y_key in metric.metadata:
                y_values.append(metric.metadata[y_key])
            else:
                y_values.append(metric.value)
            
            metadata.append(metric.metadata)
        
        return {
            'x': x_values,
            'y': y_values,
            'metadata': metadata
        }
    
    @staticmethod
    def save_plot_data(data: Dict[str, Any], filename: str, config: Optional[PlotConfig] = None) -> bool:
        """Save plot data to file."""
        try:
            if config is None:
                config = PlotConfig()
            
            # Create plot data structure
            plot_data = {
                'title': config.title,
                'x_label': config.x_label,
                'y_label': config.y_label,
                'data': {
                    'x': data['x'],
                    'y': data['y'],
                    'metadata': data['metadata']
                },
                'config': config.to_dict()
            }
            
            # Save to JSON
            with open(filename, 'w') as f:
                json.dump(plot_data, f, indent=2)
            
            return True
        except Exception:
            return False

class PluginLoader:
    """Loads and manages plugins for NLM."""
    
    def __init__(self):
        self.plugins: Dict[str, Any] = {}
        self.loaded_plugins: List[str] = []
        self.enabled_plugins: List[str] = []
        self.plugin_configs: Dict[str, Dict[str, Any]] = {}
        self.plugin_results: Dict[str, Any] = {}
        self.lock = threading.Lock()
    
    def discover_plugins(self) -> List[str]:
        """Discover available plugins."""
        # Placeholder for plugin discovery
        # In a real implementation, this would scan the filesystem
        # for plugin directories and load metadata
        return [
            'visualization',
            'statistics', 
            'novelty',
            'reward',
            'curiosity',
            'prediction',
            'memory',
            'development',
            'plasticity',
            'neuromodulation'
        ]
    
    def load_plugin(self, plugin_name: str) -> bool:
        """Load a specific plugin."""
        with self.lock:
            if plugin_name in self.loaded_plugins:
                return True
            
            # Placeholder for plugin loading
            # In a real implementation, this would:
            # 1. Find the plugin implementation
            # 2. Load the plugin module
            # 3. Initialize the plugin
            # 4. Register plugin with NLM
            
            self.loaded_plugins.append(plugin_name)
            self.plugins[plugin_name] = {'status': 'loaded'}
            
            return True
    
    def unload_plugin(self, plugin_name: str) -> bool:
        """Unload a plugin."""
        with self.lock:
            if plugin_name not in self.loaded_plugins:
                return False
            
            # Placeholder for plugin unloading
            # Clean up plugin resources
            
            self.loaded_plugins.remove(plugin_name)
            
            if plugin_name in self.plugins:
                del self.plugins[plugin_name]
            
            if plugin_name in self.plugin_results:
                del self.plugin_results[plugin_name]
            
            return True
    
    def enable_plugin(self, plugin_name: str) -> bool:
        """Enable a plugin."""
        with self.lock:
            if plugin_name not in self.loaded_plugins:
                return False
            
            self.enabled_plugins.append(plugin_name)
            self.plugins[plugin_name]['status'] = 'enabled'
            
            return True
    
    def disable_plugin(self, plugin_name: str) -> bool:
        """Disable a plugin."""
        with self.lock:
            if plugin_name not in self.enabled_plugins:
                return False
            
            self.enabled_plugins.remove(plugin_name)
            self.plugins[plugin_name]['status'] = 'disabled'
            
            return True
    
    def configure_plugin(self, plugin_name: str, config: Dict[str, Any]) -> bool:
        """Configure a plugin."""
        with self.lock:
            if plugin_name not in self.loaded_plugins:
                return False
            
            self.plugin_configs[plugin_name] = config
            self.plugins[plugin_name]['config'] = config
            
            return True
    
    def get_plugin_state(self, plugin_name: str) -> Dict[str, Any]:
        """Get state of a plugin."""
        with self.lock:
            state = {
                'name': plugin_name,
                'loaded': plugin_name in self.loaded_plugins,
                'enabled': plugin_name in self.enabled_plugins,
                'config': self.plugin_configs.get(plugin_name, {}),
                'results': self.plugin_results.get(plugin_name, {})
            }
            
            if plugin_name in self.plugins:
                state.update(self.plugins[plugin_name])
            
            return state
    
    def load_all_plugins(self, brain) -> None:
        """Load all available plugins."""
        plugins = self.discover_plugins()
        
        for plugin_name in plugins:
            self.load_plugin(plugin_name)
    
    def enable_all_plugins(self) -> None:
        """Enable all loaded plugins."""
        for plugin_name in self.loaded_plugins:
            self.enable_plugin(plugin_name)
    
    def get_all_results(self) -> Dict[str, Any]:
        """Get results from all plugins."""
        return self.plugin_results.copy()
    
    def clear_all_plugins(self) -> None:
        """Clear all plugins."""
        with self.lock:
            for plugin_name in self.loaded_plugins:
                self.unload_plugin(plugin_name)

class PerformanceProfiler:
    """Profiler for performance monitoring."""
    
    def __init__(self):
        self.events: List[Dict[str, Any]] = []
        self.metrics: PerformanceMetrics = PerformanceMetrics()
        self.is_running = False
        self.start_time = 0.0
        self.last_frame_time = 0.0
        self.lock = threading.Lock()
    
    def start(self) -> None:
        """Start profiling."""
        with self.lock:
            self.is_running = True
            self.start_time = time.time()
            self.last_frame_time = self.start_time
            self.events.clear()
            self.metrics = PerformanceMetrics()
    
    def stop(self) -> None:
        """Stop profiling."""
        with self.lock:
            self.is_running = False
    
    def record_frame(self) -> None:
        """Record a frame for FPS calculation."""
        with self.lock:
            if not self.is_running:
                return
            
            current_time = time.time()
            
            # Calculate FPS
            if self.last_frame_time > 0:
                frame_time = current_time - self.last_frame_time
                if frame_time > 0:
                    self.metrics.fps = 1.0 / frame_time
            
            self.last_frame_time = current_time
            
            # Record event
            self.events.append({
                'timestamp': current_time,
                'type': 'frame',
                'fps': self.metrics.fps
            })
    
    def record_metric(self, name: str, value: float) -> None:
        """Record a custom metric."""
        with self.lock:
            self.metrics.metrics_collected[name] = value
            
            self.events.append({
                'timestamp': time.time(),
                'type': 'metric',
                'name': name,
                'value': value
            })
    
    def get_metrics(self) -> PerformanceMetrics:
        """Get current metrics."""
        with self.lock:
            return self.metrics
    
    def get_events(self) -> List[Dict[str, Any]]:
        """Get profiling events."""
        return self.events.copy()
    
    def export_to_csv(self, filename: str) -> bool:
        """Export events to CSV file."""
        try:
            with open(filename, 'w') as f:
                f.write('timestamp,type,name,value,fps\n')
                
                for event in self.events:
                    f.write(f"{event['timestamp']},{event['type']},")
                    f.write(f"{event.get('name', '')},{event.get('value', 0)},{event.get('fps', 0)}\n")
            
            return True
        except Exception:
            return False
    
    def export_to_json(self, filename: str) -> bool:
        """Export events to JSON file."""
        try:
            data = {
                'events': self.events,
                'metrics': self.metrics.to_dict()
            }
            
            with open(filename, 'w') as f:
                json.dump(data, f, indent=2)
            
            return True
        except Exception:
            return False

class SimulationScheduler:
    """Scheduler for running multiple simulations."""
    
    def __init__(self):
        self.scheduled_simulations: List[Dict[str, Any]] = []
        self.running_simulations: List[Dict[str, Any]] = []
        self.completed_simulations: List[Dict[str, Any]] = []
        self.lock = threading.Lock()
    
    def schedule_simulation(self, brain, world, config: AdvancedConfig, 
                          name: str = "", steps: Optional[int] = None) -> str:
        """Schedule a simulation to run."""
        with self.lock:
            simulation_id = f"sim_{len(self.scheduled_simulations)}"
            
            simulation = {
                'id': simulation_id,
                'name': name,
                'brain': brain,
                'world': world,
                'config': config,
                'steps': steps or config.max_steps,
                'status': 'scheduled',
                'created_at': time.time(),
                'results': None
            }
            
            self.scheduled_simulations.append(simulation)
            
            return simulation_id
    
    def run_scheduled_simulations(self) -> List[SimulationResult]:
        """Run all scheduled simulations."""
        results = []
        
        with self.lock:
            for simulation in self.scheduled_simulations:
                if simulation['status'] == 'scheduled':
                    simulation['status'] = 'running'
                    self.running_simulations.append(simulation)
        
        # Run simulations (would typically use threading/async)
        for simulation in self.running_simulations[:]:
            try:
                runner = SimulationRunner(simulation['config'])
                result = runner.run_simulation(simulation['brain'], simulation['world'])
                
                simulation['results'] = result
                simulation['status'] = 'completed'
                
                self.completed_simulations.append(simulation)
                results.append(result)
                
            except Exception as e:
                simulation['status'] = 'error'
                simulation['error'] = str(e)
                
                self.completed_simulations.append(simulation)
                results.append(SimulationResult(success=False, error=e))
        
        # Clear running simulations
        with self.lock:
            self.running_simulations.clear()
        
        return results
    
    def get_simulation_status(self, simulation_id: str) -> Optional[Dict[str, Any]]:
        """Get status of a specific simulation."""
        with self.lock:
            # Check scheduled simulations
            for simulation in self.scheduled_simulations:
                if simulation['id'] == simulation_id:
                    return simulation
            
            # Check running simulations
            for simulation in self.running_simulations:
                if simulation['id'] == simulation_id:
                    return simulation
            
            # Check completed simulations
            for simulation in self.completed_simulations:
                if simulation['id'] == simulation_id:
                    return simulation
            
            return None

class EventSystem:
    """Event system for NLM communication."""
    
    def __init__(self):
        self.listeners: Dict[str, List[Callable]] = {}
        self.lock = threading.Lock()
    
    def on(self, event_name: str, callback: Callable) -> None:
        """Register event listener."""
        with self.lock:
            if event_name not in self.listeners:
                self.listeners[event_name] = []
            
            self.listeners[event_name].append(callback)
    
    def off(self, event_name: str, callback: Callable) -> None:
        """Remove event listener."""
        with self.lock:
            if event_name in self.listeners:
                if callback in self.listeners[event_name]:
                    self.listeners[event_name].remove(callback)
    
    def emit(self, event_name: str, data: Dict[str, Any] = None) -> None:
        """Emit event."""
        with self.lock:
            if event_name in self.listeners:
                for callback in self.listeners[event_name]:
                    try:
                        callback(data or {})
                    except Exception:
                        # Ignore callback errors to prevent event system crashes
                        pass
    
    def clear(self, event_name: str = None) -> None:
        """Clear all listeners."""
        with self.lock:
            if event_name:
                if event_name in self.listeners:
                    del self.listeners[event_name]
            else:
                self.listeners.clear()

class StateManager:
    """Manages state snapshots and checkpoints for simulations."""
    
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.snapshots: Dict[str, Dict[str, Any]] = {}
        self.lock = threading.Lock()
    
    def create_snapshot(self, name: str = "") -> str:
        """Create a state snapshot."""
        with self.lock:
            if not name:
                name = f"snapshot_{len(self.snapshots)}"
            
            snapshot = {
                'brain_state': self._get_brain_state(),
                'world_state': self._get_world_state() if self.world else None,
                'timestamp': time.time(),
                'name': name
            }
            
            self.snapshots[name] = snapshot
            
            return name
    
    def restore_snapshot(self, name: str) -> bool:
        """Restore state from snapshot."""
        with self.lock:
            if name not in self.snapshots:
                return False
            
            snapshot = self.snapshots[name]
            
            self._restore_brain_state(snapshot['brain_state'])
            
            if self.world and snapshot['world_state']:
                self._restore_world_state(snapshot['world_state'])
            
            return True
    
    def get_snapshot_names(self) -> List[str]:
        """Get all snapshot names."""
        with self.lock:
            return list(self.snapshots.keys())
    
    def delete_snapshot(self, name: str) -> bool:
        """Delete a snapshot."""
        with self.lock:
            if name in self.snapshots:
                del self.snapshots[name]
                return True
            return False
    
    def _get_brain_state(self) -> Dict[str, Any]:
        """Get brain state (implementation depends on brain API)."""
        # Placeholder for brain state extraction
        return {
            'neurons': [],
            'synapses': [],
            'regions': [],
            'populations': []
        }
    
    def _restore_brain_state(self, state: Dict[str, Any]) -> None:
        """Restore brain state (implementation depends on brain API)."""
        # Placeholder for brain state restoration
        pass
    
    def _get_world_state(self) -> Optional[Dict[str, Any]]:
        """Get world state (implementation depends on world API)."""
        if not self.world:
            return None
        
        # Placeholder for world state extraction
        return {
            'objects': [],
            'agent': {},
            'simulation_time': self.world.get_simulation_time()
        }
    
    def _restore_world_state(self, state: Dict[str, Any]) -> None:
        """Restore world state (implementation depends on world API)."""
        # Placeholder for world state restoration
        pass

class SimulationExporter:
    """Exports simulation results and data."""
    
    @staticmethod
    def export_to_json(brain, world, filename: str) -> bool:
        """Export simulation state to JSON file."""
        try:
            data = {
                'timestamp': time.time(),
                'brain': {
                    'neurons': [],
                    'synapses': [],
                    'regions': [],
                    'populations': []
                },
                'world': None,
                'metrics': {
                    'total_spikes': brain.get_total_spike_count(),
                    'total_neurons': brain.get_total_neuron_count(),
                    'total_synapses': brain.get_total_synapse_count(),
                    'average_firing_rate': brain.get_average_firing_rate(),
                    'active_neurons': brain.get_active_neuron_count(),
                    'firing_neurons': brain.get_firing_neuron_count(),
                    'excitation_inhibition_ratio': brain.get_excitation_inhibition_ratio()
                }
            }
            
            if world:
                data['world'] = {
                    'width': world.get_width(),
                    'height': world.get_height(),
                    'simulation_time': world.get_simulation_time(),
                    'max_energy': world.get_max_energy(),
                    'energy_decay_rate': world.get_energy_decay_rate()
                }
            
            with open(filename, 'w') as f:
                json.dump(data, f, indent=2)
            
            return True
        except Exception:
            return False
    
    @staticmethod
    def export_to_csv(brain, world, filename: str, metrics_only: bool = False) -> bool:
        """Export simulation data to CSV file."""
        try:
            with open(filename, 'w') as f:
                if metrics_only:
                    # Only export metrics
                    f.write('metric,value\n')
                    f.write(f'total_spikes,{brain.get_total_spike_count()}\n')
                    f.write(f'total_neurons,{brain.get_total_neuron_count()}\n')
                    f.write(f'total_synapses,{brain.get_total_synapse_count()}\n')
                    f.write(f'average_firing_rate,{brain.get_average_firing_rate()}\n')
                    f.write(f'active_neurons,{brain.get_active_neuron_count()}\n')
                    f.write(f'firing_neurons,{brain.get_firing_neuron_count()}\n')
                    f.write(f'excitation_inhibition_ratio,{brain.get_excitation_inhibition_ratio()}\n')
                else:
                    # Export all simulation data
                    f.write('type,name,value\n')
                    f.write(f'simulation,timestamp,{time.time()}\n')
                    f.write(f'brain,total_spikes,{brain.get_total_spike_count()}\n')
                    f.write(f'brain,total_neurons,{brain.get_total_neuron_count()}\n')
                    f.write(f'brain,total_synapses,{brain.get_total_synapse_count()}\n')
                    f.write(f'brain,average_firing_rate,{brain.get_average_firing_rate()}\n')
                    
                    if world:
                        f.write(f'world,width,{world.get_width()}\n')
                        f.write(f'world,height,{world.get_height()}\n')
                        f.write(f'world,simulation_time,{world.get_simulation_time()}\n')
                        f.write(f'world,max_energy,{world.get_max_energy()}\n')
                        f.write(f'world,energy_decay_rate,{world.get_energy_decay_rate()}\n')
            
            return True
        except Exception:
            return False

# Global instance for easy access
_feature_detector = FeatureDetector()
_config_builder = ConfigBuilder()
_event_system = EventSystem()

def get_feature_detector() -> FeatureDetector:
    """Get global feature detector instance."""
    return _feature_detector

def get_config_builder() -> ConfigBuilder:
    """Get global config builder instance."""
    return _config_builder

def get_event_system() -> EventSystem:
    """Get global event system instance."""
    return _event_system

# Example usage functions
def run_example_simulation(config: Optional[AdvancedConfig] = None) -> SimulationResult:
    """Run an example simulation."""
    config = config or ConfigBuilder.create_default_config()
    
    # Create brain and world
    brain = nlm_py.createBrain(config)
    world = nlm_py.createSimpleWorld()
    
    # Run simulation
    runner = SimulationRunner(config)
    return runner.run_simulation(brain, world)

def create_example_config() -> AdvancedConfig:
    """Create an example configuration."""
    config = ConfigBuilder.create_default_config()
    config.max_steps = 100
    config.enable_visualization = False
    return config

# Compatibility functions for easy migration from basic bindings
def create_advanced_brain(config: Optional[AdvancedConfig] = None) -> nlm_py.AdvancedBrain:
    """Create advanced brain with enhanced interface."""
    config = config or ConfigBuilder.create_default_config()
    
    # Create basic brain
    basic_brain = nlm_py.createBrain(config)
    
    # Convert to advanced brain
    advanced_brain = nlm_py.createAdvancedBrain(basic_brain, config)
    
    return advanced_brain

def create_performance_monitor(brain) -> nlm_py.PerformanceMonitor:
    """Create performance monitor for brain."""
    return nlm_py.PerformanceMonitor(brain)

def create_plugin_manager(brain) -> nlm_py.PluginManager:
    """Create plugin manager for brain."""
    return nlm_py.PluginManager(brain)

# Example usage patterns
if __name__ == "__main__":
    # Example: Basic usage
    print("NLM Python Utilities - Example Usage")
    print("=" * 50)
    
    # Create configuration
    config = ConfigBuilder.create_default_config()
    
    # Create brain and world
    brain = nlm_py.createBrain(config)
    world = nlm_py.createSimpleWorld()
    
    print(f"Configuration created:")
    print(f"  Max steps: {config.max_steps}")
    print(f"  Timestep: {config.timestep}")
    print(f"  STDP enabled: {config.enable_stdp}")
    
    # Create metrics collector
    metrics = MetricsCollector()
    
    # Run example simulation
    print("\nRunning example simulation...")
    result = run_example_simulation()
    
    print(f"Simulation result:")
    print(f"  Success: {result.success}")
    print(f"  Steps completed: {result.steps_completed}")
    print(f"  Total time: {result.total_time:.2f}s")
    
    # Export results
    if result.success:
        print("\nExporting results...")
        SimulationExporter.export_to_json(brain, world, "simulation_output.json")
        SimulationExporter.export_to_csv(brain, world, "simulation_output.csv")
        print("Results exported successfully")