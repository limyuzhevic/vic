# NLM Python Bindings Documentation

## Overview

This document provides comprehensive documentation for the enhanced Python bindings of the NLM (Neural Learning Machine) project. These bindings extend the basic Python API with advanced features for neural simulation, visualization, performance monitoring, and plugin management.

## Introduction

The NLM Python bindings provide access to the full NLM neural simulation framework from Python, enabling researchers and developers to:

- Create and simulate neural networks
- Monitor performance and statistics
- Visualize neural activity in 3D
- Integrate plugins for extended functionality
- Develop custom simulation scenarios
- Analyze neural dynamics and behavior

## Installation

### Prerequisites

```bash
pip install pynlm
```

The Python bindings require:
- Python 3.8 or higher
- NumPy (optional, for array operations)
- Matplotlib (optional, for basic plotting)
- PyOpenGL (optional, for 3D visualization)

### Building from Source

If you need to build the bindings from source, first compile the C++ libraries:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

Then build the Python extensions:

```bash
scikit-build-core setup.py build_ext --inplace
```

## API Reference

### Core Classes

#### Brain Class

The `Brain` class represents the main neural network brain with advanced simulation capabilities.

```python
class Brain:
    """Neural network brain with advanced simulation capabilities."""
    
    def __init__(self, config: Optional[AdvancedConfig] = None):
        """Initialize the brain with configuration."""
        pass
    
    def initialize(self) -> bool:
        """Initialize the brain.
        
        Returns:
            bool: True if initialization succeeded, False otherwise.
            
        Raises:
            ConfigurationError: If configuration is invalid.
            SimulationError: If initialization fails.
        """
        pass
    
    def step(self, step: int) -> None:
        """Step the brain simulation.
        
        Args:
            step: Step number to simulate.
            
        Raises:
            SimulationError: If simulation fails.
        """
        pass
    
    def step_with_time(self, step: int, time: float) -> None:
        """Step the brain simulation with time.
        
        Args:
            step: Step number to simulate.
            time: Simulation time in seconds.
        """
        pass
    
    def getAverageFiringRate(self) -> float:
        """Get the average firing rate of the brain.
        
        Returns:
            float: Average firing rate in Hz.
        """
        pass
    
    def getActiveNeuronCount(self) -> int:
        """Get the number of active neurons.
        
        Returns:
            int: Number of active neurons.
        """
        pass
    
    def getFiringNeuronCount(self) -> int:
        """Get the number of firing neurons.
        
        Returns:
            int: Number of firing neurons.
        """
        pass
    
    def getTotalSpikeCount(self) -> int:
        """Get the total number of spikes.
        
        Returns:
            int: Total spike count.
        """
        pass
    
    def getTotalNeuronCount(self) -> int:
        """Get the total number of neurons.
        
        Returns:
            int: Total neuron count.
        """
        pass
    
    def getTotalSynapseCount(self) -> int:
        """Get the total number of synapses.
        
        Returns:
            int: Total synapse count.
        """
        pass
    
    def getExcitationInhibitionRatio(self) -> float:
        """Get the excitation/inhibition ratio.
        
        Returns:
            float: Excitation/inhibition ratio.
        """
        pass
    
    def getStatus(self) -> str:
        """Get the brain status string.
        
        Returns:
            str: Brain status information.
        """
        pass
    
    def reset(self) -> None:
        """Reset the brain to its initial state."""
        pass
    
    def save(self, filename: str) -> None:
        """Save the brain state to file.
        
        Args:
            filename: File to save brain state to.
        """
        pass
    
    def load(self, filename: str) -> None:
        """Load the brain state from file.
        
        Args:
            filename: File to load brain state from.
        """
        pass
    
    def isInitialized(self) -> bool:
        """Check if the brain is initialized.
        
        Returns:
            bool: True if brain is initialized, False otherwise.
        """
        pass
    
    def getAdvancedBrain(self) -> 'AdvancedBrain':
        """Get advanced brain wrapper.
        
        Returns:
            AdvancedBrain: Advanced brain wrapper.
        """
        pass
```

#### AdvancedBrain Class

The `AdvancedBrain` class provides enhanced brain interface with additional features.

```python
class AdvancedBrain:
    """Advanced brain wrapper with enhanced interface."""
    
    def __init__(self, brain: Brain, config: AdvancedConfig):
        """Initialize advanced brain wrapper."""
        pass
    
    def initialize(self) -> bool:
        """Initialize advanced brain wrapper."""
        pass
    
    def step(self, step: int) -> None:
        """Step the advanced brain wrapper."""
        pass
    
    def step_with_time(self, step: int, time: float) -> None:
        """Step the advanced brain wrapper with time."""
        pass
    
    def getCoreBrain(self) -> Brain:
        """Get core brain.
        
        Returns:
            Brain: Core brain.
        """
        pass
    
    def getPerformanceMonitor(self) -> PerformanceMonitor:
        """Get performance monitor.
        
        Returns:
            PerformanceMonitor: Performance monitor.
        """
        pass
    
    def getStatisticsCollector(self) -> StatisticsCollector:
        """Get statistics collector.
        
        Returns:
            StatisticsCollector: Statistics collector.
        """
        pass
    
    def getPerformanceMetrics(self) -> PerformanceMetrics:
        """Get performance metrics.
        
        Returns:
            PerformanceMetrics: Performance metrics.
        """
        pass
    
    def getBrainStatus(self) -> str:
        """Get brain status.
        
        Returns:
            str: Brain status.
        """
        pass
    
    def reset(self) -> None:
        """Reset advanced brain wrapper."""
        pass
    
    def save(self, filename: str) -> None:
        """Save advanced brain to file.
        
        Args:
            filename: File to save advanced brain to.
        """
        pass
    
    def load(self, filename: str) -> None:
        """Load advanced brain from file.
        
        Args:
            filename: File to load advanced brain from.
        """
        pass
```

#### PerformanceMonitor Class

The `PerformanceMonitor` class provides real-time performance monitoring.

```python
class PerformanceMonitor:
    """Performance monitoring system."""
    
    def __init__(self, brain: Brain):
        """Initialize performance monitor."""
        pass
    
    def start(self) -> None:
        """Start performance monitoring."""
        pass
    
    def stop(self) -> None:
        """Stop performance monitoring."""
        pass
    
    def update(self) -> None:
        """Update performance metrics."""
        pass
    
    def getMetrics(self) -> PerformanceMetrics:
        """Get performance metrics.
        
        Returns:
            PerformanceMetrics: Performance metrics.
        """
        pass
    
    def reset(self) -> None:
        """Reset performance metrics."""
        pass
    
    def exportToCSV(self, filename: str) -> bool:
        """Export performance metrics to CSV.
        
        Args:
            filename: File to export performance metrics to.
            
        Returns:
            bool: True if export succeeded, False otherwise.
        """
        pass
    
    def exportToJSON(self, filename: str) -> bool:
        """Export performance metrics to JSON.
        
        Args:
            filename: File to export performance metrics to.
            
        Returns:
            bool: True if export succeeded, False otherwise.
        """
        pass
```

#### StatisticsCollector Class

The `StatisticsCollector` class provides statistics collection and analysis.

```python
class StatisticsCollector:
    """Statistics collection and analysis."""
    
    def __init__(self, brain: Brain):
        """Initialize statistics collector."""
        pass
    
    def add_metric(self, name: str, value: float) -> None:
        """Add metric to collection.
        
        Args:
            name: Metric name.
            value: Metric value.
        """
        pass
    
    def get_metric(self, name: str) -> float:
        """Get metric by name.
        
        Args:
            name: Metric name.
            
        Returns:
            float: Metric value.
            
        Raises:
            NLMError: If metric not found.
        """
        pass
    
    def get_metric_names(self) -> List[str]:
        """Get list of metric names.
        
        Returns:
            List[str]: List of metric names.
        """
        pass
    
    def generate_report(self) -> str:
        """Generate statistics report.
        
        Returns:
            str: Statistics report.
        """
        pass
    
    def export_to_json(self, filename: str) -> bool:
        """Export statistics to JSON.
        
        Args:
            filename: File to export statistics to.
            
        Returns:
            bool: True if export succeeded, False otherwise.
        """
        pass
    
    def export_to_csv(self, filename: str) -> bool:
        """Export statistics to CSV.
        
        Args:
            filename: File to export statistics to.
            
        Returns:
            bool: True if export succeeded, False otherwise.
        """
        pass
    
    def clear(self) -> None:
        """Clear all statistics."""
        pass
```

#### VisualizationManager Class

The `VisualizationManager` class provides visualization and rendering.

```python
class VisualizationManager:
    """Visualization and rendering system."""
    
    def __init__(self, brain: Brain, world=None):
        """Initialize visualization manager."""
        pass
    
    def configure(self, config: VisualizationConfig) -> None:
        """Configure visualization manager.
        
        Args:
            config: Visualization configuration.
        """
        pass
    
    def start(self) -> None:
        """Start visualization."""
        pass
    
    def stop(self) -> None:
        """Stop visualization."""
        pass
    
    def update(self, step: int) -> None:
        """Update visualization.
        
        Args:
            step: Step number.
        """
        pass
    
    def initialize(self) -> None:
        """Initialize visualization."""
        pass
    
    def isRunning(self) -> bool:
        """Check if visualization is running.
        
        Returns:
            bool: True if visualization is running, False otherwise.
        """
        pass
    
    def getStatus(self) -> str:
        """Get visualization status.
        
        Returns:
            str: Visualization status.
        """
        pass
```

#### PluginManager Class

The `PluginManager` class provides plugin management and loading.

```python
class PluginManager:
    """Plugin management and loading system."""
    
    def __init__(self, brain: Brain):
        """Initialize plugin manager."""
        pass
    
    def discover_plugins(self) -> List[str]:
        """Discover available plugins.
        
        Returns:
            List[str]: List of plugin names.
        """
        pass
    
    def load_plugin(self, plugin_name: str) -> bool:
        """Load a plugin.
        
        Args:
            plugin_name: Name of plugin to load.
            
        Returns:
            bool: True if plugin loaded successfully, False otherwise.
        """
        pass
    
    def enable_plugin(self, plugin_name: str) -> bool:
        """Enable a plugin.
        
        Args:
            plugin_name: Name of plugin to enable.
            
        Returns:
            bool: True if plugin enabled successfully, False otherwise.
        """
        pass
    
    def disable_plugin(self, plugin_name: str) -> bool:
        """Disable a plugin.
        
        Args:
            plugin_name: Name of plugin to disable.
            
        Returns:
            bool: True if plugin disabled successfully, False otherwise.
        """
        pass
    
    def configure_plugin(self, plugin_name: str, config: Dict[str, str]) -> bool:
        """Configure a plugin.
        
        Args:
            plugin_name: Name of plugin to configure.
            config: Plugin configuration.
            
        Returns:
            bool: True if plugin configured successfully, False otherwise.
        """
        pass
    
    def get_plugin_state(self, plugin_name: str) -> Dict[str, str]:
        """Get plugin state.
        
        Args:
            plugin_name: Name of plugin.
            
        Returns:
            Dict[str, str]: Plugin state.
            
        Raises:
            NLMError: If plugin not found.
        """
        pass
    
    def get_loaded_plugins(self) -> List[str]:
        """Get list of loaded plugins.
        
        Returns:
            List[str]: List of loaded plugin names.
        """
        pass
    
    def get_enabled_plugins(self) -> List[str]:
        """Get list of enabled plugins.
        
        Returns:
            List[str]: List of enabled plugin names.
        """
        pass
    
    def get_enhanced_statistics(self) -> Dict[str, str]:
        """Get enhanced statistics.
        
        Returns:
            Dict[str, str]: Enhanced statistics.
        """
        pass
    
    def disable_all_plugins(self) -> None:
        """Disable all plugins."""
        pass
```

#### Config Class

The `Config` class provides configuration management.

```python
class Config:
    """Configuration management system."""
    
    def __init__(self):
        """Initialize configuration."""
        pass
    
    def set(self, key: str, value: Any) -> None:
        """Set configuration value.
        
        Args:
            key: Configuration key.
            value: Configuration value.
        """
        pass
    
    def get(self, key: str, default_value: Any = None) -> Any:
        """Get configuration value.
        
        Args:
            key: Configuration key.
            default_value: Default value if key not found.
            
        Returns:
            Any: Configuration value.
        """
        pass
    
    def has(self, key: str) -> bool:
        """Check if configuration has key.
        
        Args:
            key: Configuration key.
            
        Returns:
            bool: True if configuration has key, False otherwise.
        """
        pass
    
    def remove(self, key: str) -> None:
        """Remove configuration key.
        
        Args:
            key: Configuration key to remove.
        """
        pass
    
    def save_to_file(self, filename: str) -> bool:
        """Save configuration to file.
        
        Args:
            filename: File to save configuration to.
            
        Returns:
            bool: True if save succeeded, False otherwise.
        """
        pass
    
    def load_from_file(self, filename: str) -> bool:
        """Load configuration from file.
        
        Args:
            filename: File to load configuration from.
            
        Returns:
            bool: True if load succeeded, False otherwise.
        """
        pass
    
    def clear(self) -> None:
        """Clear all configuration."""
        pass
    
    def summary(self) -> str:
        """Get configuration summary.
        
        Returns:
            str: Configuration summary.
        """
        pass
```

#### SimpleWorld Class

The `SimpleWorld` class provides world simulation.

```python
class SimpleWorld:
    """World simulation system."""
    
    def __init__(self):
        """Initialize world."""
        pass
    
    def configure(self, width: float, height: float, 
                  vision_width: int, vision_height: int) -> None:
        """Configure world.
        
        Args:
            width: World width.
            height: World height.
            vision_width: Vision width.
            vision_height: Vision height.
        """
        pass
    
    def setAgentStart(self, x: float, y: float) -> None:
        """Set agent starting position.
        
        Args:
            x: X coordinate.
            y: Y coordinate.
        """
        pass
    
    def update(self, time: float) -> None:
        """Update world.
        
        Args:
            time: Simulation time.
        """
        pass
    
    def getSensoryPercept(self) -> Any:
        """Get sensory perception.
        
        Returns:
            Any: Sensory perception.
        """
        pass
    
    def getAgentBody(self) -> Any:
        """Get agent body.
        
        Returns:
            Any: Agent body.
        """
        pass
    
    def applyMotorCommand(self, action: Any) -> None:
        """Apply motor command.
        
        Args:
            action: Motor action.
        """
        pass
    
    def get_width(self) -> float:
        """Get world width.
        
        Returns:
            float: World width.
        """
        pass
    
    def get_height(self) -> float:
        """Get world height.
        
        Returns:
            float: World height.
        """
        pass
    
    def get_simulation_time(self) -> float:
        """Get simulation time.
        
        Returns:
            float: Simulation time.
        """
        pass
```

#### Vision Class

The `Vision` class provides visual perception.

```python
class Vision:
    """Visual perception system."""
    
    def __init__(self):
        """Initialize vision."""
        pass
    
    def setFOV(self, fov: float) -> None:
        """Set field of view.
        
        Args:
            fov: Field of view in degrees.
        """
        pass
    
    def setNoiseLevel(self, noise_level: float) -> None:
        """Set noise level.
        
        Args:
            noise_level: Noise level.
        """
        pass
    
    def setResolution(self, width: int, height: int) -> None:
        """Set resolution.
        
        Args:
            width: Image width.
            height: Image height.
        """
        pass
    
    def setColorMode(self, mode: str) -> None:
        """Set color mode.
        
        Args:
            mode: Color mode ("grayscale", "color", etc.).
        """
        pass
    
    def processInput(self, input_data: Any) -> Any:
        """Process visual input.
        
        Args:
            input_data: Visual input data.
            
        Returns:
            Any: Processed visual data.
        """
        pass
```

#### Audio Class

The `Audio` class provides auditory perception.

```python
class Audio:
    """Auditory perception system."""
    
    def __init__(self):
        """Initialize audio."""
        pass
    
    def setSamplingRate(self, rate: int) -> None:
        """Set sampling rate.
        
        Args:
            rate: Sampling rate.
        """
        pass
    
    def setNoiseLevel(self, noise_level: float) -> None:
        """Set noise level.
        
        Args:
            noise_level: Noise level.
        """
        pass
    
    def setDirectionality(self, enabled: bool) -> None:
        """Set directionality.
        
        Args:
            enabled: Whether directionality is enabled.
        """
        pass
    
    def processInput(self, input_data: Any) -> Any:
        """Process audio input.
        
        Args:
            input_data: Audio input data.
            
        Returns:
            Any: Processed audio data.
        """
        pass
```

### Configuration Classes

#### AdvancedConfig Class

The `AdvancedConfig` class provides advanced configuration options.

```python
class AdvancedConfig:
    """Advanced configuration options."""
    
    def __init__(self):
        """Initialize advanced configuration."""
        # Simulation parameters
        self.max_steps = 10000
        self.timestep = 0.1
        self.temporal_decay = 0.95
        self.spatial_decay = 0.8
        self.global_connectivity = 0.1
        
        # Plasticity parameters
        self.enable_stdp = True
        self.enable_hebbian = True
        self.enable_structural = True
        self.stdp_learning_rate = 0.01
        self.hebbian_learning_rate = 0.005
        
        # Neuromodulation parameters
        self.enable_reward_modulation = True
        self.enable_curiosity = True
        self.enable_prediction_error = True
        self.neuromodulation_strength = 1.0
        
        # Development parameters
        self.enable_development = True
        self.development_rate = 0.001
        
        # Memory parameters
        self.enable_working_memory = True
        self.enable_episodic_memory = True
        self.memory_capacity = 1000
        
        # Performance parameters
        self.enable_performance_monitoring = True
        self.enable_profiling = False
        self.profile_interval = 1.0
        
        # Visualization parameters
        self.enable_visualization = False
        self.visualization_config = VisualizationConfig()
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary.
        
        Returns:
            Dict[str, Any]: Configuration dictionary.
        """
        pass
    
    @classmethod
    def from_dict(cls, config_dict: Dict[str, Any]) -> 'AdvancedConfig':
        """Create AdvancedConfig from dictionary.
        
        Args:
            config_dict: Configuration dictionary.
            
        Returns:
            AdvancedConfig: Advanced configuration.
        """
        pass
```

#### VisualizationConfig Class

The `VisualizationConfig` class provides visualization configuration.

```python
class VisualizationConfig:
    """Visualization configuration."""
    
    def __init__(self):
        """Initialize visualization configuration."""
        self.enabled = True
        self.enable_3d = True
        self.window_width = 1920
        self.window_height = 1080
        self.show_neurons = True
        self.show_synapses = True
        self.show_world = True
        self.neuron_size = 0.1
        self.synapse_alpha = 0.6
        self.refresh_rate = 30.0
        self.background_color = "black"
        self.neuron_color = "red"
        self.synapse_color = "blue"
        self.colormap = "viridis"
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary.
        
        Returns:
            Dict[str, Any]: Configuration dictionary.
        """
        pass
```

#### PerformanceMetrics Class

The `PerformanceMetrics` class provides performance metrics.

```python
class PerformanceMetrics:
    """Performance metrics for simulation runs."""
    
    def __init__(self):
        """Initialize performance metrics."""
        self.fps = 0.0
        self.simulation_time = 0.0
        self.memory_usage = 0.0
        self.cpu_utilization = 0.0
        self.spike_count = 0
        self.neuron_count = 0
        self.synapse_count = 0
        self.average_firing_rate = 0.0
        self.active_neurons = 0
        self.firing_neurons = 0
        self.energy_usage = 0.0
        self.steps_completed = 0
        self.metrics_collected = {}
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert metrics to dictionary.
        
        Returns:
            Dict[str, Any]: Metrics dictionary.
        """
        pass
```

### Exception Classes

#### NLMError Class

The `NLMError` class provides base NLM exception.

```python
class NLMError(Exception):
    """Base NLM exception."""
    
    def __init__(self, message: str):
        """Initialize NLM error.
        
        Args:
            message: Error message.
        """
        super().__init__(f"NLM Error: {message}")
```

#### ConfigurationError Class

The `ConfigurationError` class provides configuration exception.

```python
class ConfigurationError(NLMError):
    """Configuration exception."""
    
    def __init__(self, message: str):
        """Initialize configuration error.
        
        Args:
            message: Error message.
        """
        super().__init__(f"Configuration Error: {message}")
```

#### SimulationError Class

The `SimulationError` class provides simulation exception.

```python
class SimulationError(NLMError):
    """Simulation exception."""
    
    def __init__(self, message: str):
        """Initialize simulation error.
        
        Args:
            message: Error message.
        """
        super().__init__(f"Simulation Error: {message}")
```

#### VisualizationError Class

The `VisualizationError` class provides visualization exception.

```python
class VisualizationError(NLMError):
    """Visualization exception."""
    
    def __init__(self, message: str):
        """Initialize visualization error.
        
        Args:
            message: Error message.
        """
        super().__init__(f"Visualization Error: {message}")
```

### Data Classes

#### Event Class

The `Event` class provides event data.

```python
@dataclass
class Event:
    """Event in the event system."""
    
    name: str
    data: Dict[str, Any] = field(default_factory=dict)
    timestamp: float = 0.0
    source: Optional[str] = None
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert event to dictionary.
        
        Returns:
            Dict[str, Any]: Event dictionary.
        """
        pass
```

#### Metric Class

The `Metric` class provides metric data.

```python
@dataclass
class Metric:
    """Metric data point."""
    
    name: str
    value: float
    timestamp: float = 0.0
    metadata: Dict[str, Any] = field(default_factory=dict)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert metric to dictionary.
        
        Returns:
            Dict[str, Any]: Metric dictionary.
        """
        pass
```

#### PlotConfig Class

The `PlotConfig` class provides plot configuration.

```python
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
```

### Utility Classes

#### FeatureDetector Class

The `FeatureDetector` class provides feature detection.

```python
class FeatureDetector:
    """Detect available features in the NLM system."""
    
    def detect_features(self) -> Dict[str, bool]:
        """Detect available features.
        
        Returns:
            Dict[str, bool]: Feature detection results.
        """
        pass
    
    def is_feature_available(self, feature_name: str) -> bool:
        """Check if a specific feature is available.
        
        Args:
            feature_name: Feature name.
            
        Returns:
            bool: True if feature is available, False otherwise.
        """
        pass
```

#### Profiler Class

The `Profiler` class provides profiling utilities.

```python
class Profiler:
    """Profiler for performance monitoring."""
    
    def __init__(self, name: str):
        """Initialize profiler.
        
        Args:
            name: Profiler name.
        """
        pass
    
    def checkpoint(self, label: str) -> None:
        """Add a checkpoint.
        
        Args:
            label: Checkpoint label.
        """
        pass
    
    def stop(self) -> None:
        """Stop profiling."""
        pass
    
    def printResults(self) -> None:
        """Print profiling results."""
        pass
    
    def exportToFile(self, filename: str) -> bool:
        """Export profiling results to file.
        
        Args:
            filename: File to export results to.
            
        Returns:
            bool: True if export succeeded, False otherwise.
        """
        pass
```

### Context Managers

#### Brain Context Manager

The `Brain` class can be used as a context manager.

```python
def createBrain(config=None):
    """Create brain with context manager support."""
    # Context manager implementation
    pass
```

#### PerformanceMonitor Context Manager

The `PerformanceMonitor` class can be used as a context manager.

```python
def createPerformanceMonitor(brain):
    """Create performance monitor with context manager support."""
    # Context manager implementation
    pass
```

## Configuration Examples

### Basic Configuration

```python
from nlm_py import AdvancedConfig, VisualizationConfig

# Create basic configuration
config = AdvancedConfig()
config.max_steps = 1000
config.timestep = 0.1
config.temporal_decay = 0.95
config.spatial_decay = 0.8
config.global_connectivity = 0.1

print(f"Configuration:")
print(f"  Max steps: {config.max_steps}")
print(f"  Timestep: {config.timestep}")
print(f"  Temporal decay: {config.temporal_decay}")
print(f"  Spatial decay: {config.spatial_decay}")
print(f"  Global connectivity: {config.global_connectivity}")
```

### Research Configuration

```python
from nlm_py import AdvancedConfig, VisualizationConfig
from nlm_py import ConfigBuilder

# Create research configuration
config = ConfigBuilder.create_research_config()

print(f"Research configuration:")
print(f"  Max steps: {config.max_steps}")
print(f"  STDP enabled: {config.enable_stdp}")
print(f"  Hebbian enabled: {config.enable_hebbian}")
print(f"  Structural enabled: {config.enable_structural}")
print(f"  Reward modulation enabled: {config.enable_reward_modulation}")
print(f"  Curiosity enabled: {config.enable_curiosity}")
```

### Development Configuration

```python
from nlm_py import AdvancedConfig, VisualizationConfig
from nlm_py import ConfigBuilder

# Create development configuration
config = ConfigBuilder.create_development_config()

print(f"Development configuration:")
print(f"  Max steps: {config.max_steps}")
print(f"  STDP enabled: {config.enable_stdp}")
print(f"  Hebbian enabled: {config.enable_hebbian}")
print(f"  Structural enabled: {config.enable_structural}")
print(f"  Reward modulation enabled: {config.enable_reward_modulation}")
print(f"  Curiosity enabled: {config.enable_curiosity}")
```

### Visualization Configuration

```python
from nlm_py import AdvancedConfig, VisualizationConfig

# Create visualization configuration
viz_config = VisualizationConfig()
viz_config.enabled = True
viz_config.enable_3d = True
viz_config.window_width = 1920
viz_config.window_height = 1080
viz_config.show_neurons = True
viz_config.show_synapses = True
viz_config.show_world = True
viz_config.neuron_size = 0.2
viz_config.synapse_alpha = 0.7
viz_config.refresh_rate = 60.0

# Create advanced configuration with visualization
config = AdvancedConfig()
config.enable_visualization = True
config.visualization_config = viz_config

print(f"Visualization configuration:")
print(f"  Enabled: {viz_config.enabled}")
print(f"  3D enabled: {viz_config.enable_3d}")
print(f"  Window size: {viz_config.window_width}x{viz_config.window_height}")
print(f"  Neuron size: {viz_config.neuron_size}")
print(f"  Synapse alpha: {viz_config.synapse_alpha}")
```

## Plugin Development

### Plugin Example: Novelty Plugin

```python
# python/plugins/novelty_plugin.py
class NoveltyPlugin:
    """Plugin for novelty detection and surprise calculation."""
    
    def __init__(self):
        self.threshold = 0.5
        self.sensitivity = 1.0
        self.observations = []
    
    def get_name(self):
        """Get plugin name."""
        return "NoveltyPlugin"
    
    def get_version(self):
        """Get plugin version."""
        return "1.0.0"
    
    def load(self, brain):
        """Load plugin with brain reference."""
        self.brain = brain
        return True
    
    def initialize(self, config):
        """Initialize plugin with configuration."""
        self.threshold = config.get("threshold", 0.5)
        self.sensitivity = config.get("sensitivity", 1.0)
        return True
    
    def step(self, step):
        """Process step with plugin logic."""
        # Calculate novelty
        novelty = self.calculate_novelty(step)
        self.observations.append((step, novelty))
        
        # Trigger if threshold exceeded
        if novelty > self.threshold:
            self.on_novelty_detected(novelty, step)
    
    def shutdown(self):
        """Shutdown plugin."""
        self.observations.clear()
        self.brain = None
    
    def get_statistics(self):
        """Get plugin statistics."""
        stats = {
            "name": self.get_name(),
            "version": self.get_version(),
            "observations_count": len(self.observations),
            "average_novelty": self.calculate_average_novelty(),
            "max_novelty": self.calculate_max_novelty(),
            "threshold": self.threshold,
            "sensitivity": self.sensitivity
        }
        return stats
    
    def calculate_novelty(self, step):
        """Calculate novelty for given step."""
        # Implementation-specific novelty calculation
        return 0.0
    
    def calculate_average_novelty(self):
        """Calculate average novelty."""
        if not self.observations:
            return 0.0
        total = sum(obs[1] for obs in self.observations)
        return total / len(self.observations)
    
    def calculate_max_novelty(self):
        """Calculate maximum novelty."""
        if not self.observations:
            return 0.0
        return max(obs[1] for obs in self.observations)
    
    def on_novelty_detected(self, novelty, step):
        """Handle novelty detection event."""
        # Implementation-specific handling
        pass
```

### Plugin Registration

```python
# python/plugin_manager.py
class PluginManager:
    """Plugin management and loading system."""
    
    def __init__(self, brain):
        self.brain = brain
        self.plugins = {}
    
    def register_plugin(self, plugin):
        """Register a plugin.
        
        Args:
            plugin: Plugin instance.
        """
        self.plugins[plugin.get_name()] = plugin
    
    def load_plugin(self, plugin_name, plugin_config=None):
        """Load a plugin by name.
        
        Args:
            plugin_name: Plugin name.
            plugin_config: Plugin configuration.
            
        Returns:
            bool: True if plugin loaded successfully, False otherwise.
        """
        if plugin_name == "novelty_plugin":
            from .plugins.novelty_plugin import NoveltyPlugin
            plugin = NoveltyPlugin()
            plugin.load(self.brain)
            if plugin_config:
                plugin.initialize(plugin_config)
            self.register_plugin(plugin)
            return True
        return False
```

## Examples

### Basic Neural Network Simulation

```python
"""Example: Basic neural network simulation"""

import nlm_py
from nlm_py import Brain, Config, SimpleWorld, createBrain, createSimpleWorld

# Create configuration
config = Config()
config.set("simulation.max_steps", 1000)
config.set("brain.global_connectivity", 0.05)
config.set("plasticity.stdp.enabled", True)

# Create brain and world
brain = createBrain(config)
world = createSimpleWorld()
world.configure(100.0, 100.0, 20, 20)

# Initialize
brain.initialize()

# Run simulation
for step in range(1000):
    # Get sensory input
    percept = world.getSensoryPercept()
    brain.receiveSensoryInput(percept)
    
    # Step brain
    brain.step(step)
    
    # Apply action
    action = brain.produceAction()
    world.applyMotorCommand(action)
    
    # Update world
    world.update(step * 0.1)

print("Simulation completed")
```

### Simulation with Performance Monitoring

```python
"""Example: Simulation with performance monitoring"""

import nlm_py
from nlm_py import Brain, Config, SimpleWorld
from nlm_py import PerformanceMonitor, StatisticsCollector

# Create components
config = Config()
brain = Brain(config)
world = SimpleWorld()

# Create monitoring tools
monitor = PerformanceMonitor(brain)
stats = StatisticsCollector(brain)

# Initialize
brain.initialize()

# Start monitoring
monitor.start()

# Run simulation with monitoring
for step in range(1000):
    # Record statistics
    stats.add_metric("firing_rate", brain.getAverageFiringRate())
    stats.add_metric("active_neurons", brain.getActiveNeuronCount())
    
    # Step simulation
    brain.step(step)
    
    # Get performance metrics
    perf = monitor.getMetrics()
    
    # Periodic output
    if step % 100 == 0:
        print(f"Step {step}: FPS={perf.fps:.2f}, "
              f"Memory={perf.memory_usage:.2f}MB, "
              f"Active neurons={brain.getActiveNeuronCount()}")

# Stop monitoring
monitor.stop()

# Generate final report
stats.generate_report()
print("Simulation completed with monitoring")
```

### Simulation with Visualization

```python
"""Example: Simulation with 3D visualization"""

import nlm_py
from nlm_py import Brain, Config, SimpleWorld
from nlm_py import VisualizationManager, VisualizationConfig

# Create components
config = Config()
brain = Brain(config)
world = SimpleWorld()

# Initialize brain
brain.initialize()

# Create visualization configuration
viz_config = VisualizationConfig()
viz_config.enabled = True
viz_config.enable_3d = True
viz_config.window_width = 1920
viz_config.window_height = 1080
viz_config.show_neurons = True
viz_config.show_synapses = True
viz_config.show_world = True

# Create visualization manager
viz = VisualizationManager(brain, world)
viz.configure(viz_config)

# Start visualization
viz.start()

# Run simulation with visualization
for step in range(1000):
    brain.step(step)
    viz.update(step)

# Stop visualization
viz.stop()
print("Visualization simulation completed")
```

### Simulation with Plugins

```python
"""Example: Simulation with plugins"""

import nlm_py
from nlm_py import Brain, Config, SimpleWorld
from nlm_py import PluginManager

# Create brain
config = Config()
brain = Brain(config)

# Create plugin manager
plugin_manager = PluginManager(brain)

# Discover and load plugins
plugin_manager.discover_plugins()

# Load and enable a plugin
plugin_manager.load_plugin("novelty_plugin")
plugin_manager.enable_plugin("novelty_plugin")

# Configure plugin
plugin_manager.configure_plugin("novelty_plugin", {
    "threshold": 0.5,
    "sensitivity": 1.0
})

# Initialize and run
brain.initialize()

for step in range(1000):
    # Plugin-enhanced simulation step
    brain.step(step)
    
    # Get plugin-enhanced statistics
    enhanced_stats = plugin_manager.get_enhanced_statistics()
    
    # Check if plugins have generated insights
    if "novelty_insights" in enhanced_stats:
        handle_novelty_detection(enhanced_stats["novelty_insights"])

# Disable all plugins
plugin_manager.disable_all_plugins()
print("Plugin simulation completed")
```

## Troubleshooting

### Common Issues and Solutions

#### Import Errors
**Problem**: Cannot import nlm_py or its modules.

**Solution**: Ensure all dependencies are installed:

```bash
pip install numpy matplotlib pyopengl
```

#### Performance Issues
**Problem**: Simulation is slow or memory usage is high.

**Solutions**:
1. Use batch processing for large simulations
2. Monitor memory usage with PerformanceMonitor
3. Optimize configuration for your hardware
4. Use NumPy arrays for bulk operations

```python
# Use batch operations
brain.batch_steps(steps=range(100))

# Monitor memory usage
monitor = PerformanceMonitor(brain)
```

#### Visualization Not Working
**Problem**: 3D visualization is not displaying correctly.

**Solutions**:
1. Check OpenGL support
2. Verify graphics drivers are updated
3. Try 2D fallback mode
4. Check system resources

```python
# Test OpenGL support
from nlm_py import test_opengl_support
if not test_opengl_support():
    print("OpenGL not available, falling back to 2D visualization")
    viz = VisualizationManager(brain)
    viz.enable_3d(False)
```

#### Configuration Errors
**Problem**: Configuration file cannot be loaded or parsed.

**Solutions**:
1. Check file format (JSON, YAML, or key=value)
2. Validate configuration syntax
3. Ensure required fields are present
4. Check file permissions

```python
from nlm_py import ConfigValidator

# Validate configuration
config = {"max_steps": 1000, "timestep": 0.1}
errors = ConfigValidator.validate_config(config)
if errors:
    for error in errors:
        print(f"Configuration error: {error}")
```

### Getting Help

- **Documentation**: Check the `nlm_utils.py` module for additional utilities
- **Examples**: Look at the `examples/` directory for complete working examples
- **Community**: Join the NLM discussion forum for help
- **Issues**: Report bugs in the GitHub repository

## Performance Guidelines

### Optimizing Simulation Performance

1. **Use NumPy arrays for bulk operations**:
   ```python
   import numpy as np
   
   # Use NumPy for large array operations
   input_data = np.random.randn(1000, 784)
   sensory_input = SensoryInput(input_data)
   ```

2. **Batch processing**:
   ```python
   # Process multiple simulation steps in batch
   brain.batch_steps(range(1000))
   ```

3. **Memory management**:
   ```python
   # Use context managers to ensure proper cleanup
   with createBrain(config) as brain:
       # Simulation code here
       pass
   ```

### Profiling and Benchmarking

```python
# Profile simulation performance
from nlm_py import Profiler

with Profiler("simulation") as profiler:
    for step in range(1000):
        brain.step(step)

# Print profiling results
profiler.print_results()
profiler.export_to_file("profile_results.json")
```

## Best Practices

### Code Structure

1. **Use configuration files**:
   ```python
   # Create reusable configuration profiles
   config1 = Config()
   config1.set("simulation.max_steps", 1000)
   
   config2 = Config()
   config2.set("simulation.max_steps", 5000)
   ```

2. **Leverage Python utilities**:
   ```python
   from nlm_utils import SimulationRunner, ConfigBuilder
   
   # Use utility functions for common operations
   runner = SimulationRunner(brain, world)
   results = runner.run_simulation(steps=1000)
   ```

3. **Implement error handling**:
   ```python
   try:
       # Simulation code
       pass
   except Exception as e:
       # Handle errors gracefully
       handle_simulation_error(e)
   ```

### Integration with Other Libraries

```python
# Integrate with matplotlib for plotting
import matplotlib.pyplot as plt
import nlm_py

# Create simulation with visualization
brain = nlm_py.Brain(config)
world = nlm_py.SimpleWorld()

# Run simulation
for step in range(100):
    brain.step(step)

# Plot results
steps = list(range(100))
firing_rates = [brain.get_average_firing_rate(step) for step in steps]
plt.plot(steps, firing_rates)
plt.xlabel("Step")
plt.ylabel("Average Firing Rate")
plt.title("Neural Activity Over Time")
plt.show()
```

## Changelog

### Version 0.1.0
- Initial release with enhanced Python bindings
- Added comprehensive error handling
- Added performance monitoring capabilities
- Added visualization integration
- Added plugin system support
- Added advanced simulation features
- Added comprehensive documentation

### Future Enhancements

The enhanced Python bindings will continue to be developed with:

- Enhanced neural plasticity modeling
- Multi-agent simulation support
- Advanced learning algorithms
- Integration with deep learning frameworks
- Real-time data streaming support
- Cloud computing integration
- Mobile platform support
- VR/AR integration

For more information and updates, visit the NLM project documentation.

## References

- [NLM C++ Documentation](https://github.com/your-org/nlm/tree/main/docs/api) - C++ API documentation
- [NLM Architecture Documentation](docs/ARCHITECTURE.md) - System architecture
- [NLM Science Documentation](docs/SCIENCE.md) - Scientific background
- [Phase 6 Audit](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 completion audit
- [NLM Performance Documentation](docs/PERFORMANCE.md) - Performance analysis
- [NLM Scaling Documentation](docs/SCALING.md) - Scalability analysis
- [Research Methods](docs/RESEARCH_METHODS.md) - Research methodology
- [Python Bindings Examples](examples/) - Python examples and tutorials

## Conclusion

The enhanced Python bindings provide comprehensive access to the NLM neural simulation framework. This documentation covers all aspects of working with the Python bindings, from basic usage to advanced features and plugin development.

The enhanced Python bindings make it easy to get started while providing advanced features for complex simulations. The C++ core ensures maximum performance while the Python interface provides accessibility and ease of use.

Start with the basic examples and gradually explore more advanced features as you become comfortable with the system. The documentation and examples will guide you through the full range of NLM's Python capabilities.

Happy simulating!
