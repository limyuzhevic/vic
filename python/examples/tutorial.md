# NLM Python Tutorial
# Comprehensive guide to using the enhanced NLM Python bindings

This tutorial provides comprehensive guidance on using the enhanced Python bindings
for the NLM (Neural Learning Machine) project. It covers installation, basic usage,
advanced features, and best practices.

## Table of Contents

1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Quick Start](#quick-start)
4. [Basic Concepts](#basic-concepts)
5. [Core API](#core-api)
6. Advanced Features
   - [Performance Monitoring](#performance-monitoring)
   - [Visualization](#visualization)
   - [Plugin System](#plugin-system)
   - [Configuration Management](#configuration-management)
   - [Error Handling](#error-handling)
7. [Working Examples](#working-examples)
8. [Best Practices](#best-practices)
9. [Troubleshooting](#troubleshooting)
10. [Reference Guide](#reference-guide)

## Introduction

The NLM (Neural Learning Machine) is an experimental computational brain project
that investigates neural systems that learn from experience. The enhanced Python
bindings provide comprehensive access to NLM's neural simulation capabilities
from Python, enabling researchers and developers to:

- Create and simulate neural networks
- Monitor performance and collect statistics
- Visualize neural activity
- Extend functionality through plugins
- Analyze neural dynamics and behavior

## Installation

### Prerequisites

The enhanced Python bindings require:

- Python 3.8 or higher
- NumPy (optional, for array operations)
- Matplotlib (optional, for basic plotting)
- PyOpenGL (optional, for 3D visualization)

### Installation via pip

```bash
pip install pynlm
```

### Building from Source

If you need to build the bindings from source:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Build Python extensions
scikit-build-core setup.py build_ext --inplace
```

### Development Installation

For development and testing:

```bash
# Install with development dependencies
pip install pynlm[dev]

# Or install in development mode
pip install -e .
```

## Quick Start

This section provides a quick introduction to using the enhanced Python bindings.

### Creating a Basic Simulation

```python
import sys
sys.path.insert(0, '/workspace/.../python')

import nlm_py
import time

# Create configuration
config = nlm_py.Config()
config.set("simulation.max_steps", 1000)

# Create brain and world
brain = nlm_py.createBrain(config)
world = nlm_py.createSimpleWorld()

# Initialize components
brain.initialize()
world.configure(width=100.0, height=100.0, visionWidth=20, visionHeight=20)
world.setAgentStart(50.0, 50.0)

# Run simulation
for step in range(1000):
    brain.step(step)
    world.update(step * 0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    brain.receiveSensoryInput(percept)
    
    # Get action
    action = brain.produceAction()
    world.applyMotorCommand(action, step * 0.1)

print("Simulation completed successfully!")
```

### Using the Enhanced API

```python
from nlm_py import createBrain, createSimpleWorld
from nlm_utils import SimulationRunner, ConfigBuilder

# Create enhanced configuration
config = ConfigBuilder.create_research_config()

# Create simulation runner
runner = SimulationRunner(config)
brain = nlm_py.createBrain(config)
world = nlm_py.createSimpleWorld()

# Run simulation with monitoring
result = runner.run_simulation(brain, world)

if result.is_successful():
    print(f"Simulation completed: {result.steps_completed} steps")
    print(f"Performance metrics: {result.metrics.to_dict()}")
```

## Basic Concepts

### Core Components

#### Brain
The central neural simulation system. It integrates multiple subsystems:

- **Neurons**: Computational units that process and transmit signals
- **Synapses**: Connections between neurons that transmit signals
- **Plasticity**: Learning mechanisms (STDP, Hebbian, structural)
- **Development**: Developmental processes and maturation
- **Neuromodulation**: Regulatory signals (dopamine, curiosity, etc.)
- **Memory**: Working memory, episodic memory, and associative memory

#### World
The environment in which the neural system operates:

- **SimpleWorld**: Basic 2D world for development
- **Objects**: Resources, hazards, walls, markers
- **Agent**: Moving entity with sensory capabilities
- **Sensory Input**: Vision, audio, and internal signals

#### Configuration
System configuration and parameters:

- **Basic Configuration**: Simple key-value pairs
- **Advanced Configuration**: Comprehensive settings for research
- **Validation**: Configuration validation and error handling
- **File I/O**: JSON/YAML configuration support

### Key Types

#### Neuron Identification
```python
# Neuron IDs
from nlm_py import NeuronId, SynapseId, RegionId, PopulationId

# Create neuron IDs
neuron_id = NeuronId(42)
print(f"Neuron ID: {neuron_id}")  # <NeuronId: 42>

# Get neuron index
index = neuron_id.index()
```

#### Neuron Types
```python
from nlm_py import NeuronType

# Different neuron types
print(f"Excitatory: {NeuronType.Excitatory}")
print(f"Inhibitory: {NeuronType.Inhibitory}")
print(f"Sensory: {NeuronType.Sensory}")
print(f"Motor: {NeuronType.Motor}")
print(f"Internal: {NeuronType.Internal}")
```

#### Synapse Types
```python
from nlm_py import SynapseType

# Different synapse types
print(f"Excitatory: {SynapseType.Excitatory}")
print(f"Inhibitory: {SynapseType.Inhibitory}")
print(f"Modulatory: {SynapseType.Modulatory}")
print(f"Electrical: {SynapseType.Electrical}")
```

## Core API

### Brain Operations

#### Creating and Initializing a Brain
```python
from nlm_py import createBrain, Config, AdvancedBrain

# Create basic brain
config = Config()
brain = createBrain(config)

# Initialize brain
brain.initialize()

# Create advanced brain with enhanced interface
advanced_config = ConfigBuilder.create_research_config()
advanced_brain = AdvancedBrain(brain, advanced_config)
advanced_brain.initialize()
```

#### Simulation Control
```python
# Run simulation steps
for step in range(100):
    brain.step(step)
    brain.step(step, time=step * 0.1)  # With timestamp

# Get brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()} Hz")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")

# Save and load brain state
brain.save("brain_state.nlm")
# ... later
brain.load("brain_state.nlm")
```

#### Neural Network Operations
```python
# Get neuron and synapse information
from nlm_py import NeuronId, SynapseId

# Neuron operations
neuron_id = NeuronId(42)
if brain.has_neuron(neuron_id):
    neuron_info = brain.get_neuron_info(neuron_id)
    print(f"Neuron type: {neuron_info.type}")
    print(f"Neuron position: {neuron_info.position}")

# Synapse operations
synapse_id = SynapseId(100)
if brain.has_synapse(synapse_id):
    synapse_info = brain.get_synapse_info(synapse_id)
    print(f"Synapse type: {synapse_info.type}")
    print(f"Source: {synapse_info.source_id}")
    print(f"Target: {synapse_info.target_id}")
    print(f"Weight: {synapse_info.weight}")
```

### World Operations

#### Creating and Configuring World
```python
from nlm_py import createSimpleWorld, WorldObject, WorldObjectType

# Create world
world = createSimpleWorld()
world.configure(width=100.0, height=100.0, visionWidth=20, visionHeight=20)

# Set agent position
world.setAgentStart(50.0, 50.0)

# Add objects
world.addObject(WorldObject(10.0, 10.0, WorldObjectType.Resource, 1.0))
world.addObject(WorldObject(80.0, 80.0, WorldObjectType.Hazard, 10.0))

# Get world state
agent = world.getAgentBody()
percept = world.getSensoryPercept()
```

#### World Updates and Actions
```python
# Update world
world.update(timestep=0.1)

# Apply actions
from nlm_py import ActionType, MotorCommand

# Apply motor command
world.applyMotorCommand(MotorCommand.MoveForward, 0.1)

# Check position validity
if world.isValidPosition(50.0, 50.0):
    world.removeObject(50.0, 50.0)
```

### Configuration Management

#### Basic Configuration
```python
from nlm_py import Config

# Create configuration
config = Config()

# Set configuration values
config.set("simulation.max_steps", 1000)
config.set("simulation.temporal_decay", 0.95)
config.set("brain.global_connectivity", 0.1)

# Check configuration keys
print(config.has("simulation.max_steps"))  # True

# Get configuration keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Get configuration value
max_steps = config.get("simulation.max_steps")
print(f"Max steps: {max_steps}")

# Load/save configuration
config.saveToFile("config.json")
config.loadFromFile("config.json")
```

#### Advanced Configuration
```python
from nlm_utils import AdvancedConfig, ConfigBuilder

# Create advanced configuration
config = ConfigBuilder.create_research_config()

# Modify configuration
config.max_steps = 5000
config.temporal_decay = 0.99
config.stdp_learning_rate = 0.002

# Export configuration to dictionary
config_dict = config.to_dict()

# Create configuration from dictionary
config_from_dict = AdvancedConfig.from_dict(config_dict)
```

### Performance Monitoring

#### Performance Metrics
```python
from nlm_py import createPerformanceMonitor
from nlm_utils import PerformanceMetrics

# Create performance monitor
monitor = createPerformanceMonitor(brain)

# Get performance metrics
metrics = monitor.getMetrics()
print(f"FPS: {metrics.fps}")
print(f"Simulation time: {metrics.simulation_time} seconds")
print(f"Memory usage: {metrics.memory_usage} MB")
print(f"CPU utilization: {metrics.cpu_utilization}%")
print(f"Spike count: {metrics.spike_count}")
print(f"Average firing rate: {metrics.average_firing_rate} Hz")

# Export metrics
monitor.exportToCSV("performance.csv")
monitor.exportToJSON("performance.json")
```

#### Performance Profiler
```python
from nlm_utils import PerformanceProfiler

# Create profiler
profiler = PerformanceProfiler("simulation")

# Start profiling
profiler.start()

# Run simulation with profiling
for step in range(100):
    brain.step(step)
    profiler.record_frame()

# Stop profiling
profiler.stop()

# Get profiling results
profiler.printResults()
profiler.exportToFile("profile_results.json")
```

### Visualization

#### Visualization Configuration
```python
from nlm_utils import VisualizationConfig

# Create visualization configuration
viz_config = VisualizationConfig()
viz_config.enabled = True
viz_config.enable_3d = True
viz_config.window_width = 1920
viz_config.window_height = 1080
viz_config.show_neurons = True
viz_config.show_synapses = True
viz_config.show_world = True
viz_config.refresh_rate = 60.0
```

#### Visualization Manager
```python
from nlm_py import createVisualizationManager

# Create visualization manager
viz = createVisualizationManager(brain, world)
viz.configure(viz_config)

# Start visualization
viz.start()

# Update visualization during simulation
for step in range(100):
    brain.step(step)
    viz.update(step)

# Stop visualization
viz.stop()

# Get status
print(viz.getStatus())
```

### Plugin System

#### Plugin Manager
```python
from nlm_py import createPluginManager

# Create plugin manager
plugin_manager = createPluginManager(brain)

# Discover plugins
plugins = plugin_manager.discover_plugins()
print(f"Available plugins: {plugins}")

# Load and enable plugins
plugin_manager.load_plugin("visualization_plugin")
plugin_manager.enable_plugin("visualization_plugin")
plugin_manager.configure_plugin("visualization_plugin", {
    'enabled': True,
    'refresh_rate': 30.0,
    'show_neurons': True
})

# Get plugin state
state = plugin_manager.get_plugin_state("visualization_plugin")
print(f"Plugin state: {state}")

# Get enhanced statistics
stats = plugin_manager.get_enhanced_statistics()
print(f"Enhanced statistics: {stats}")
```

### Error Handling

#### Custom Exceptions
```python
from nlm_py import NLMError, ConfigurationError, SimulationError

try:
    # Some operation that might fail
    result = brain.step(step)
except ConfigurationError as e:
    print(f"Configuration error: {e}")
except SimulationError as e:
    print(f"Simulation error: {e}")
except NLMError as e:
    print(f"NLM error: {e}")
```

#### Graceful Degradation
```python
from nlm_utils import FeatureDetector

feature_detector = FeatureDetector()

if feature_detector.is_feature_available("3d_visualization"):
    # Use 3D visualization if available
    from nlm_py import VisualizationManager
    viz = VisualizationManager(brain)
    viz.enable_3d(True)
else:
    print("3D visualization not available, using 2D fallback")
```

## Advanced Features

### Performance Monitoring

#### Real-time Monitoring
```python
# Create comprehensive monitoring system
from nlm_py import PerformanceMonitor
from nlm_utils import MetricsCollector, PerformanceProfiler

# Create monitoring components
performance_monitor = PerformanceMonitor(brain)
metrics_collector = MetricsCollector()
performance_profiler = PerformanceProfiler("simulation")

# Start monitoring
performance_profiler.start()

# Run simulation with monitoring
for step in range(1000):
    brain.step(step)
    performance_monitor.record_frame()
    metrics_collector.collect_metrics(brain, step)
    
    # Log progress
    if step % 100 == 0:
        perf_metrics = performance_monitor.getMetrics()
        print(f"Step {step}: FPS={perf_metrics.fps:.1f}, "
              f"Memory={perf_metrics.memory_usage:.1f}MB, "
              f"Firing rate={perf_metrics.average_firing_rate:.2f}Hz")

# Stop monitoring
performance_profiler.stop()

# Export monitoring data
performance_monitor.exportToCSV("monitoring_results.csv")
metrics_collector.export_to_csv("collected_metrics.csv")
performance_profiler.exportToFile("profiling_results.json")
```

#### Performance Analysis
```python
# Analyze performance metrics
def analyze_performance(metrics):
    """Analyze performance metrics and provide insights."""
    analysis = {}
    
    # FPS analysis
    if metrics.fps > 30:
        analysis['frame_rate'] = 'Good'
    elif metrics.fps > 10:
        analysis['frame_rate'] = 'Acceptable'
    else:
        analysis['frame_rate'] = 'Poor'
    
    # Memory analysis
    if metrics.memory_usage < 100:
        analysis['memory_usage'] = 'Low'
    elif metrics.memory_usage < 500:
        analysis['memory_usage'] = 'Moderate'
    else:
        analysis['memory_usage'] = 'High'
    
    # Performance analysis
    analysis['cpu_efficiency'] = metrics.cpu_utilization / 100.0
    analysis['spike_efficiency'] = metrics.spike_count / max(1, metrics.steps_completed)
    
    return analysis

# Analyze metrics
perf_metrics = performance_monitor.getMetrics()
analysis = analyze_performance(perf_metrics)
print(f"Performance analysis: {analysis}")
```

### Visualization

#### 3D Visualization
```python
# Create 3D visualization
from nlm_py import createVisualizationManager
from nlm_utils import VisualizationConfig

# Configure 3D visualization
viz_config = VisualizationConfig()
viz_config.enabled = True
viz_config.enable_3d = True
viz_config.window_width = 1920
viz_config.window_height = 1080
viz_config.neuron_size = 0.1
viz_config.synapse_alpha = 0.6

# Create and start visualization
viz = createVisualizationManager(brain, world)
viz.configure(viz_config)
viz.start()

# Run simulation with 3D visualization
for step in range(500):
    brain.step(step)
    world.update(step * 0.1)
    viz.update(step)

viz.stop()
```

#### Data Visualization
```python
# Create data visualization
from nlm_utils import VisualizationHelper, PlotConfig

# Prepare data for plotting
metrics = metrics_collector.get_all_metrics()
plot_data = VisualizationHelper.prepare_data_for_plotting(
    metrics, 
    x_key='step', 
    y_key='average_firing_rate'
)

# Create plot configuration
plot_config = VisualizationHelper.create_plot_config(
    title='Neural Activity Over Time',
    x_label='Simulation Step',
    y_label='Average Firing Rate (Hz)'
)

# Save plot data
VisualizationHelper.save_plot_data(plot_data, "neural_activity_plot.json", plot_config)
```

### Plugin System

#### Plugin Development
```python
# Create custom plugin
class CustomPlugin:
    def __init__(self):
        self.name = "custom_plugin"
        self.version = "1.0.0"
        self.enabled = False
        self.config = {}
    
    def load(self, brain):
        """Load plugin with brain reference."""
        self.brain = brain
        self.enabled = True
        return True
    
    def unload(self):
        """Unload plugin."""
        self.brain = None
        self.enabled = False
        return True
    
    def get_state(self):
        """Get plugin state."""
        return {
            'name': self.name,
            'version': self.version,
            'enabled': self.enabled,
            'config': self.config
        }

# Use custom plugin
plugin = CustomPlugin()
plugin.load(brain)
print(f"Plugin loaded: {plugin.get_state()}")
plugin.unload()
```

#### Plugin Management
```python
# Comprehensive plugin management
from nlm_py import createPluginManager

# Create plugin manager
plugin_manager = createPluginManager(brain)

# Complete plugin lifecycle
plugins_to_manage = ['visualization', 'statistics', 'novelty', 'reward']

# Load all plugins
for plugin_name in plugins_to_manage:
    plugin_manager.load_plugin(plugin_name)

# Configure plugins
for plugin_name in plugins_to_manage:
    plugin_manager.configure_plugin(plugin_name, {
        'enabled': True,
        'refresh_rate': 30.0,
        'output_format': 'json'
    })

# Enable all plugins
for plugin_name in plugins_to_manage:
    plugin_manager.enable_plugin(plugin_name)

# Monitor plugin performance
while plugin_manager.are_any_plugins_enabled():
    # Check plugin results
    results = plugin_manager.get_all_results()
    for plugin_name, result in results.items():
        print(f"{plugin_name} results: {result}")
    
    # Run simulation step
    brain.step(step)
    step += 1

# Cleanup
for plugin_name in plugins_to_manage:
    plugin_manager.disable_plugin(plugin_name)
    plugin_manager.unload_plugin(plugin_name)

print("Plugin management completed")
```

### Configuration Management

#### Configuration Building
```python
# Advanced configuration building
from nlm_utils import ConfigBuilder, ConfigValidator

# Create different configuration presets
configs = {
    'basic': ConfigBuilder.create_default_config(),
    'research': ConfigBuilder.create_research_config(),
    'development': ConfigBuilder.create_development_config(),
    'production': ConfigBuilder.create_research_config()  # Optimized for production
}

# Optimize production configuration
production_config = configs['production']
production_config.max_steps = 10000
production_config.enable_profiling = True
production_config.profile_interval = 1.0
production_config.enable_visualization = False

# Validate configurations
for name, config in configs.items():
    errors = ConfigValidator.validate_advanced_config(config)
    if errors:
        print(f"Configuration '{name}' has errors:")
        for error in errors:
            print(f"  - {error}")
    else:
        print(f"Configuration '{name}' is valid")
```

#### Configuration Validation
```python
# Custom configuration validation
class CustomConfigValidator:
    @staticmethod
    def validate_for_simulation(config, brain, world):
        """Validate configuration for specific simulation scenario."""
        errors = []
        
        # Check configuration compatibility
        if config.max_steps < 10:
            errors.append("Max steps too low for meaningful simulation")
        
        if config.timestep <= 0:
            errors.append("Timestep must be positive")
        
        if brain and hasattr(brain, 'getTotalNeuronCount'):
            neuron_count = brain.getTotalNeuronCount()
            if neuron_count < 10:
                errors.append("Too few neurons for simulation")
        
        if world and hasattr(world, 'getWidth'):
            if world.getWidth() <= 0 or world.getHeight() <= 0:
                errors.append("World dimensions must be positive")
        
        # Check for conflicting settings
        if config.enable_stdp and config.enable_hebbian:
            # Check if both plasticity mechanisms are compatible
            if config.stdp_learning_rate + config.hebbian_learning_rate > 0.1:
                errors.append("Combined learning rates too high for stability")
        
        return errors

# Use custom validator
validator = CustomConfigValidator()
errors = validator.validate_for_simulation(config, brain, world)
if errors:
    print("Validation failed:")
    for error in errors:
        print(f"  - {error}")
else:
    print("Configuration is valid for simulation")
```

### Event System

#### Event Handling
```python
# Event-based programming
from nlm_utils import EventSystem

# Create event system
event_system = EventSystem()

# Define event handlers
def on_simulation_started(data):
    print(f"Simulation started with {data.get('steps', 'unknown')} steps")

def on_simulation_step(data):
    step = data['step']
    brain = data['brain']
    print(f"Step {step}: Firing rate = {brain.getAverageFiringRate():.2f} Hz")

def on_simulation_completed(data):
    result = data['result']
    print(f"Simulation completed: {result.success}, {result.steps_completed} steps")

# Register event handlers
event_system.on('simulation_started', on_simulation_started)
event_system.on('simulation_step', on_simulation_step)
event_system.on('simulation_completed', on_simulation_completed)

# Emit events
event_system.emit('simulation_started', {
    'brain': brain,
    'world': world,
    'steps': config.max_steps
})

# Run simulation with event system
for step in range(config.max_steps):
    event_system.emit('simulation_step', {
        'step': step,
        'brain': brain,
        'world': world
    })
    
    brain.step(step)
    
    if step == config.max_steps - 1:
        event_system.emit('simulation_completed', {
            'result': SimulationResult(success=True, steps_completed=step + 1),
            'brain': brain,
            'world': world
        })

# Cleanup
event_system.clear()
```

### Statistics Collection

#### Metrics Collection
```python
# Comprehensive statistics collection
from nlm_utils import MetricsCollector

# Create metrics collector
metrics = MetricsCollector()

# Collect various metrics during simulation
for step in range(100):
    brain.step(step)
    metrics.collect_metrics(brain, step)
    
    # Access collected metrics
    current_metrics = metrics.get_latest_metrics()
    print(f"Step {step}: {current_metrics}")

# Analyze metrics
all_metrics = metrics.get_all_metrics()

# Filter metrics by name
firing_rate_metrics = metrics.get_metrics_by_name('firing_rate')

# Export metrics
metrics.export_to_csv("simulation_metrics.csv")
metrics.export_to_json("simulation_metrics.json")
```

#### Statistical Analysis
```python
# Statistical analysis functions
def calculate_statistics(metrics):
    """Calculate statistical measures for metrics."""
    values = [metric.value for metric in metrics]
    
    if not values:
        return None
    
    return {
        'mean': sum(values) / len(values),
        'median': sorted(values)[len(values) // 2],
        'std': (sum((x - sum(values) / len(values)) ** 2 for x in values) / len(values)) ** 0.5,
        'min': min(values),
        'max': max(values),
        'range': max(values) - min(values)
    }

# Analyze firing rate statistics
firing_rate_stats = calculate_statistics(firing_rate_metrics)
print(f"Firing rate statistics: {firing_rate_stats}")
```

## Working Examples

### Example 1: Basic Neural Network
Run the basic simulation example:

```bash
python python/examples/basic_simulation.py
```

### Example 2: Advanced Simulation with Monitoring
Run the advanced simulation example:

```bash
python python/examples/advanced_simulation.py
```

### Example 3: Plugin-Based Simulation
Run the plugin-based simulation example:

```bash
python python/examples/plugin_based_simulation.py
```

### Example 4: Configuration Builder
Run the configuration builder example:

```bash
python python/examples/config_builder.py
```

## Best Practices

### Code Organization

1. **Use Configuration Files**: Create separate configuration files for different scenarios:

```python
# config_research.json
{
    "max_steps": 10000,
    "timestep": 0.1,
    "enable_stdp": true,
    "enable_hebbian": true,
    "enable_visualization": false
}

# config_development.json
{
    "max_steps": 1000,
    "timestep": 0.5,
    "enable_stdp": true,
    "enable_hebbian": true,
    "enable_visualization": true
}
```

2. **Implement Error Handling**: Always implement comprehensive error handling:

```python
from nlm_py import NLMError

def safe_simulation(config, brain, world):
    try:
        # Simulation code
        brain.step(step)
        return True
    except NLMError as e:
        print(f"Simulation error: {e}")
        return False
    except Exception as e:
        print(f"Unexpected error: {e}")
        return False
```

3. **Use Context Managers**: Use context managers for resource cleanup:

```python
from nlm_utils import SimulationRunner

# Use simulation runner with context manager
with SimulationRunner(config) as runner:
    result = runner.run_simulation(brain, world)
    if result.is_successful():
        print("Simulation completed successfully")
```

### Performance Optimization

1. **Batch Processing**: Process multiple steps in batch when possible:

```python
# Batch processing for performance
steps = range(100, 1100, 10)  # Process every 10th step
brain.batch_steps(steps)
```

2. **Memory Management**: Clean up resources regularly:

```python
# Use context managers for automatic cleanup
with createBrain(config) as brain:
    # Simulation code
    brain.initialize()
    for step in range(100):
        brain.step(step)
    # Brain is automatically cleaned up
```

3. **Profiler Integration**: Use profiler to identify performance bottlenecks:

```python
from nlm_utils import PerformanceProfiler

# Profile simulation
profiler = PerformanceProfiler("simulation")
profiler.start()

for step in range(100):
    brain.step(step)
    profiler.record_frame()

profiler.stop()
profiler.printResults()
```

### Testing and Validation

1. **Unit Testing**: Test individual components:

```python
import unittest
from nlm_utils import ConfigValidator, MetricsCollector

class TestNLMComponents(unittest.TestCase):
    def test_config_validation(self):
        config = ConfigBuilder.create_default_config()
        errors = ConfigValidator.validate_advanced_config(config)
        self.assertEqual(len(errors), 0)
    
    def test_metrics_collection(self):
        metrics = MetricsCollector()
        self.assertEqual(len(metrics.get_all_metrics()), 0)
        metrics.clear()
        self.assertEqual(len(metrics.get_all_metrics()), 0)

if __name__ == '__main__':
    unittest.main()
```

2. **Integration Testing**: Test complete workflows:

```python
import sys
sys.path.insert(0, '/workspace/.../python')

import nlm_py
from nlm_utils import SimulationRunner, ConfigBuilder

def test_complete_workflow():
    """Test complete NLM workflow."""
    # Create configuration
    config = ConfigBuilder.create_default_config()
    config.max_steps = 10
    
    # Create brain and world
    brain = nlm_py.createBrain(config)
    world = nlm_py.createSimpleWorld()
    
    # Initialize
    brain.initialize()
    world.configure(width=50.0, height=50.0, visionWidth=10, visionHeight=10)
    
    # Run simulation
    runner = SimulationRunner(config)
    result = runner.run_simulation(brain, world)
    
    # Verify results
    assert result.is_successful()
    assert result.steps_completed == 10
    assert brain.getTotalNeuronCount() > 0
    
    print("Complete workflow test passed")

if __name__ == "__main__":
    test_complete_workflow()
```

### Documentation and Examples

1. **Add Documentation**: Document your code and examples:

```python
"""
Example: Custom NLM Simulation

This example demonstrates how to create a custom NLM simulation with
enhanced features and monitoring.

Features:
- Advanced configuration
- Performance monitoring
- Statistics collection
- Error handling
"""

def custom_simulation():
    # Simulation code here
    pass
```

2. **Maintain Examples**: Keep examples up-to-date and functional:

```bash
# Run examples to ensure they work
python python/examples/basic_simulation.py
python python/examples/advanced_simulation.py
python python/examples/plugin_based_simulation.py
python python/examples/config_builder.py
```

## Troubleshooting

### Common Issues and Solutions

1. **Import Errors**
   - **Problem**: "ModuleNotFoundError: No module named 'nlm_py'"
   - **Solution**: Ensure the Python bindings are built and installed correctly:
   ```bash
   # Build the C++ bindings
   mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   
   # Install Python bindings
   pip install pynlm
   ```

2. **Performance Issues**
   - **Problem**: Simulation is slow or unresponsive
   - **Solutions**:
     - Reduce simulation steps
     - Disable visualization for performance
     - Increase timestep
     - Monitor memory usage

3. **Memory Issues**
   - **Problem**: Memory usage is high
   - **Solutions**:
     - Use context managers for cleanup
     - Monitor memory usage regularly
     - Consider batch processing

4. **Visualization Issues**
   - **Problem**: 3D visualization not working
   - **Solutions**:
     - Check OpenGL support
     - Disable 3D visualization
     - Use 2D visualization instead

5. **Plugin Issues**
   - **Problem**: Plugins not loading or failing
   - **Solutions**:
     - Check plugin dependencies
     - Verify plugin configuration
     - Use fallback mechanisms for missing plugins

### Debugging Tools

1. **Performance Profiler**
   ```python
   from nlm_utils import PerformanceProfiler
   
   # Profile simulation
   profiler = PerformanceProfiler("debug_simulation")
   profiler.start()
   
   # Run problematic code
   for step in range(100):
       brain.step(step)
       profiler.record_frame()
   
   profiler.stop()
   profiler.printResults()
   ```

2. **Error Logging**
   ```python
   import logging
   
   # Configure logging
   logging.basicConfig(level=logging.DEBUG)
   logger = logging.getLogger(__name__)
   
   try:
       # Problematic code
       brain.step(step)
   except Exception as e:
       logger.error(f"Error at step {step}: {e}")
       raise
   ```

3. **State Management**
   ```python
   from nlm_utils import StateManager
   
   # Create state manager
   state_manager = StateManager(brain, world)
   
   # Create snapshot
   snapshot_name = state_manager.create_snapshot("debug_state")
   
   # Later, restore from snapshot
   state_manager.restore_snapshot(snapshot_name)
   ```

## Reference Guide

### Class Reference

#### Core Classes
- **Brain**: Main neural simulation brain class
- **Config**: Configuration management
- **SimpleWorld**: 2D world simulation
- **AgentBrain**: Agent brain interface
- **SensoryInput**: Base class for sensory input
- **Action**: Motor action representation

#### Advanced Classes
- **AdvancedBrain**: Enhanced brain interface with advanced features
- **PerformanceMonitor**: Real-time performance monitoring
- **VisualizationManager**: 3D visualization interface
- **PluginManager**: Plugin system management
- **StatisticsCollector**: Statistics and metrics collection

#### Utility Classes
- **ConfigBuilder**: Configuration building utilities
- **FeatureDetector**: Feature detection system
- **MetricsCollector**: Metrics collection and analysis
- **PerformanceProfiler**: Performance profiling utilities
- **EventSystem**: Event-based programming system
- **StateManager**: State management and checkpointing
- **SimulationExporter**: Export functionality
- **ConfigValidator**: Configuration validation

### Function Reference

#### Core Functions
- **createBrain(config)**: Create brain with configuration
- **createSimpleWorld()**: Create simple world
- **createDefaultConfig()**: Create default configuration
- **createPerformanceMonitor(brain)**: Create performance monitor

#### Utility Functions
- **SimulationRunner.run_simulation()**: Run simulation with monitoring
- **VisualizationHelper.create_plot_config()**: Create plot configuration
- **ConfigBuilder.create_config_from_preset()**: Create configuration from preset
- **FeatureDetector.is_feature_available()**: Check feature availability

### Type References

#### Basic Types
- **NeuronId**: Unique identifier for neurons
- **SynapseId**: Unique identifier for synapses
- **RegionId**: Unique identifier for regions
- **PopulationId**: Unique identifier for populations

#### Enumerations
- **NeuronType**: Different neuron types
- **SynapseType**: Different synapse types
- **DevelopmentalStage**: Developmental stages
- **ActionType**: Action types
- **WorldObjectType**: World object types

#### Data Structures
- **PerformanceMetrics**: Performance monitoring metrics
- **VisualizationConfig**: Visualization configuration
- **AdvancedConfig**: Advanced configuration
- **SimulationResult**: Simulation results
- **Event**: Event data structure
- **Metric**: Metric data structure

### Configuration Options

#### Advanced Configuration Options
```python
# Simulation parameters
max_steps: 10000
  Maximum number of simulation steps

timestep: 0.1
  Simulation timestep

temporal_decay: 0.95
  Temporal decay factor

spatial_decay: 0.8
  Spatial decay factor

global_connectivity: 0.1
  Global connectivity probability

# Plasticity parameters
enable_stdp: true
  Enable STDP plasticity

enable_hebbian: true
  Enable Hebbian plasticity

enable_structural: true
  Enable structural plasticity

stdp_learning_rate: 0.01
  STDP learning rate

hebbian_learning_rate: 0.005
  Hebbian learning rate

# Neuromodulation parameters
enable_reward_modulation: true
  Enable reward modulation

enable_curiosity: true
  Enable curiosity modulation

enable_prediction_error: true
  Enable prediction error

neuromodulation_strength: 1.0
  Neuromodulation strength

# Development parameters
enable_development: true
  Enable developmental processes

development_rate: 0.001
  Development rate

# Memory parameters
enable_working_memory: true
  Enable working memory

enable_episodic_memory: true
  Enable episodic memory

memory_capacity: 1000
  Memory capacity

# Performance parameters
enable_performance_monitoring: true
  Enable performance monitoring

enable_profiling: false
  Enable profiling

profile_interval: 1.0
  Profiling interval

# Visualization parameters
enable_visualization: false
  Enable visualization
```

## Acknowledgments

This enhanced Python bindings implementation builds upon the foundation laid by:

- The original NLM C++ neural simulation framework
- The pybind11 library for C++/Python integration
- The scikit-build-core build system
- The Config implementation improvements
- The existing NLM architecture and design patterns

Special thanks to all contributors and researchers who have developed the NLM project.

## License

This enhanced Python bindings package is licensed under the MIT License.

## Version Information

- Python bindings version: 0.1.0
- C++ core version: 0.1.0
- Compatible with: NLM 0.1.0

## Changelog

### Version 0.1.0
- Initial release with enhanced Python bindings
- Added comprehensive error handling
- Added performance monitoring capabilities
- Added visualization integration
- Added plugin system support
- Added advanced configuration management
- Added statistics collection
- Added event system
- Added state management
- Added export functionality
- Added utility functions and helpers
- Added comprehensive documentation
- Added working examples
- Added testing utilities

### Future Enhancements

Planned enhancements for future versions:

- Enhanced neural plasticity modeling
- Multi-agent simulation support
- Advanced learning algorithms
- Integration with deep learning frameworks
- Real-time data streaming support
- Cloud computing integration
- Mobile platform support
- VR/AR integration
- Distributed computing support

## Getting Help

### Documentation

- **Python API Documentation**: Check docstrings in the code
- **Examples**: See the `examples/` directory for working examples
- **Tutorials**: Refer to this tutorial for step-by-step guidance

### Community Support

- **GitHub Issues**: Report bugs and feature requests
- **Discussion Forum**: Join the NLM community discussion
- **Documentation**: Check the main NLM documentation

### Technical Support

- **Bug Reports**: File issues on GitHub
- **Feature Requests**: Submit feature requests
- **Code Contributions**: Contribute to the project
- **Code Reviews**: Participate in code reviews

## Conclusion

The enhanced Python bindings provide comprehensive access to the NLM neural
simulation framework with advanced features for research, development, and
production use. The bindings include:

- **Core functionality**: Basic neural simulation capabilities
- **Performance monitoring**: Real-time performance metrics and analysis
- **Visualization**: 3D visualization and data plotting
- **Plugin system**: Extensible plugin architecture
- **Configuration management**: Advanced configuration and validation
- **Error handling**: Comprehensive error handling and graceful degradation
- **Statistics collection**: Metrics collection and analysis
- **Event system**: Event-based programming
- **State management**: State snapshots and checkpointing
- **Export functionality**: Data export in multiple formats
- **Utility functions**: Helper functions for common operations
- **Comprehensive documentation**: Complete documentation and examples

The enhanced Python bindings are designed to be flexible, extensible, and
user-friendly, supporting both basic usage and advanced research scenarios.
They provide a solid foundation for building complex neural simulations
and exploring the capabilities of the NLM project.

---

For more information and updates, visit the NLM project documentation.
