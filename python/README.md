# Enhanced Python Bindings for NLM (Neural Learning Machine)

This document provides comprehensive documentation for the enhanced Python bindings of the NLM (Neural Learning Machine) project. These bindings extend the basic Python API with advanced features for neural simulation, visualization, performance monitoring, and plugin management.

## Overview

The enhanced Python bindings provide access to the full NLM neural simulation framework from Python, enabling researchers and developers to:

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

## Basic Usage

### Creating a Brain Simulation

```python
from nlm_py import Brain, Config, createBrain

# Create default configuration
config = Config()

# Create brain with configuration
brain = Brain(config)

# Initialize the brain
brain.initialize()

# Run simulation steps
for step in range(1000):
    brain.step(step)
```

### Working with the World

```python
from nlm_py import SimpleWorld, createSimpleWorld, ActionType

# Create a world
world = SimpleWorld()
world.configure(width=100.0, height=100.0, visionWidth=20, visionHeight=20)

# Set agent starting position
world.setAgentStart(50.0, 50.0)

# Update world each timestep
for timestep in range(100):
    world.update(timestep * 0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Get agent state
    body = world.getAgentBody()
    
    # Apply actions based on neural output
    # ... (action generation logic)
```

### Advanced Configuration

```python
from nlm_py import Config

# Create configuration
config = Config()

# Set basic parameters
config.set("simulation.max_steps", 1000)
config.set("simulation.temporal_decay", 0.95)
config.set("brain.global_connectivity", 0.1)

# Add sensory parameters
config.set("sensory.vision.enabled", True)
config.set("sensory.vision.noise_level", 0.01)

# Save configuration
config.save_to_file("simulation_config.json")
```

## Advanced Features

### Performance Monitoring

```python
from nlm_py import PerformanceMonitor
from nlm_py import createPerformanceMonitor

# Create performance monitor
monitor = createPerformanceMonitor(brain)

# Record performance metrics
metrics = monitor.getMetrics()
print(f"FPS: {metrics.fps}")
print(f"Memory usage: {metrics.memory_usage} MB")
print(f"CPU utilization: {metrics.cpu_utilization}%")

# Export performance data
monitor.exportToCSV("performance.csv")
```

### Visualization Integration

```python
from nlm_py import VisualizationManager
from nlm_py import createVisualizationManager

# Create visualization manager
viz = createVisualizationManager(brain, world)
viz.configure(
    window_width=1920,
    window_height=1080,
    enable_3d=True,
    show_neurons=True,
    show_synapses=True,
    show_world=True
)

# Start real-time visualization
viz.start()

# Update visualization during simulation
for step in range(1000):
    brain.step(step)
    viz.update(step)
```

### Plugin System

```python
from nlm_py import PluginManager
from nlm_py import createPluginManager

# Create plugin manager
plugin_manager = createPluginManager(brain)

# Discover available plugins
plugins = plugin_manager.discover_plugins()
for plugin in plugins:
    print(f"Found plugin: {plugin.name} v{plugin.version}")

# Load and enable a plugin
plugin_manager.load_plugin("novelty_plugin")
plugin_manager.enable_plugin("novelty_plugin")

# Get plugin state
state = plugin_manager.get_plugin_state("novelty_plugin")
print(f"Plugin active: {state.active}")
print(f"Plugin configuration: {state.config}")
```

### Statistics and Analytics

```python
from nlm_py import StatisticsCollector
from nlm_py import createStatisticsCollector

# Create statistics collector
stats = createStatisticsCollector(brain)

# Collect various statistics
stats.add_metric("firing_rate", brain.getAverageFiringRate())
stats.add_metric("active_neurons", brain.getActiveNeuronCount())
stats.add_metric("spike_count", brain.getTotalSpikeCount())
stats.add_metric("excitatory_inhibition_ratio", brain.getExcitationInhibitionRatio())

# Generate reports
report = stats.generate_report()
print(report)

# Export statistics
stats.export_to_json("simulation_stats.json")
stats.export_to_csv("simulation_stats.csv")
```

## Neural Network Operations

### Neuron Management

```python
from nlm_py import NeuronId, NeuronType
from nlm_py import createNeuronId

# Create neuron IDs
neuron_id = createNeuronId(42)
print(f"Neuron ID: {neuron_id}")
print(f"Neuron index: {neuron_id.index()}")

# Get neuron information from brain
# (Assuming brain has methods for neuron access)
if brain.has_neuron(neuron_id):
    neuron_info = brain.get_neuron_info(neuron_id)
    print(f"Neuron type: {neuron_info.type}")
    print(f"Neuron position: {neuron_info.position}")
    print(f"Neuron firing rate: {neuron_info.firing_rate}")
```

### Synapse Operations

```python
from nlm_py import SynapseId, SynapseType

# Create synapse IDs
synapse_id = createSynapseId(100)

# Get synapse information
# (Assuming brain has methods for synapse access)
if brain.has_synapse(synapse_id):
    synapse_info = brain.get_synapse_info(synapse_id)
    print(f"Synapse type: {synapse_info.type}")
    print(f"Source neuron: {synapse_info.source_id}")
    print(f"Target neuron: {synapse_info.target_id}")
    print(f"Synaptic weight: {synapse_info.weight}")
```

### Population Management

```python
from nlm_py import PopulationId

# Create population ID
population_id = createPopulationId(5)

# Get population statistics
population_stats = brain.get_population_stats(population_id)
print(f"Population size: {population_stats.size}")
print(f"Population type: {population_stats.type}")
print(f"Population firing rate: {population_stats.firing_rate}")
print(f"Average synaptic strength: {population_stats.avg_synaptic_strength}")
```

## Context Managers and Resource Management

```python
from nlm_py import Brain, Config

# Use context manager for automatic resource cleanup
with createBrain(Config()) as brain:
    brain.initialize()
    for step in range(100):
        brain.step(step)
    # Brain is automatically cleaned up
```

```python
from nlm_py import PerformanceMonitor

# Performance monitoring with context manager
with createPerformanceMonitor(brain) as monitor:
    for step in range(1000):
        brain.step(step)
    
    # Get final performance metrics
    metrics = monitor.getMetrics()
    print(f"Average FPS: {metrics.fps}")
```

## Type Hints and Documentation

The enhanced Python bindings include comprehensive type hints and documentation:

```python
import nlm_py
from nlm_py import Brain, Config, NeuronType

# Type hints in function signatures
brain: Brain = Brain(Config())

# Documentation accessible via __doc__
print(Brain.__doc__)
print(Config.__init__.__doc__)
```

## Error Handling

### Custom Exceptions

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

### Graceful Degradation

```python
from nlm_py import FeatureDetector

feature_detector = FeatureDetector()

if feature_detector.is_feature_available("visualization_3d"):
    # Use 3D visualization if available
    from nlm_py import VisualizationManager
    viz = VisualizationManager(brain)
    viz.enable_3d(True)
else:
    print("3D visualization not available, using 2D fallback")
    # Fallback to 2D visualization
    from nlm_py import VisualizationManager
    viz = VisualizationManager(brain)
    viz.enable_3d(False)
```

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
from nlm_py import Profiler

# Profile simulation performance
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

### Advanced Simulation with Monitoring

```python
"""Example: Advanced simulation with performance monitoring"""

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

# Generate final report
stats.generate_report()
print("Advanced simulation completed with monitoring")
```

### Visualization Example

```python
"""Example: Simulation with 3D visualization"""

import nlm_py
from nlm_py import Brain, Config, SimpleWorld
from nlm_py import VisualizationManager

# Create components
config = Config()
brain = Brain(config)
world = SimpleWorld()

# Initialize brain
brain.initialize()

# Create visualization
viz = VisualizationManager(brain, world)
viz.configure(
    window_width=1920,
    window_height=1080,
    enable_3d=True,
    show_neurons=True,
    show_synapses=True,
    show_world=True,
    neuron_size=0.1,
    synapse_alpha=0.6
)

# Start visualization
viz.start()

# Run simulation with real-time visualization
for step in range(1000):
    brain.step(step)
    viz.update(step)

# Stop visualization
viz.stop()
print("Visualization simulation completed")
```

### Plugin-Based Simulation

```python
"""Example: Plugin-based simulation with extended functionality"""

import nlm_py
from nlm_py import Brain, Config
from nlm_py import PluginManager, VisualizationPlugin, StatisticsPlugin

# Create brain
config = Config()
brain = Brain(config)

# Create plugin manager
plugin_manager = PluginManager(brain)

# Discover and load plugins
plugin_manager.discover_plugins()

# Enable specific plugins
plugin_manager.enable_plugin("visualization_plugin")
plugin_manager.enable_plugin("statistics_plugin")

# Configure plugins
plugin_manager.configure_plugin("visualization_plugin", {
    "enabled": True,
    "refresh_rate": 30,
    "show_neurons": True
})

plugin_manager.configure_plugin("statistics_plugin", {
    "enabled": True,
    "metrics": ["firing_rate", "spike_count", "energy"]
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

# Cleanup
plugin_manager.disable_all_plugins()
print("Plugin-based simulation completed")
```

## Troubleshooting

### Common Issues and Solutions

1. **Import errors**: Ensure all dependencies are installed
   ```bash
   pip install numpy matplotlib pyopengl
   ```

2. **Performance issues**: Check for memory leaks and optimize loops
   ```python
   # Use batch operations
   brain.batch_steps(steps=range(100))
   
   # Monitor memory usage
   monitor = PerformanceMonitor(brain)
   ```

3. **Visualization not working**: Check OpenGL support
   ```python
   # Test OpenGL context
   from nlm_py import test_opengl_support
   if not test_opengl_support():
       print("OpenGL not available, falling back to 2D visualization")
   ```

### Getting Help

- **Documentation**: Check the `nlm_utils.py` module for additional utilities
- **Examples**: Look at the `examples/` directory for complete working examples
- **Community**: Join the NLM discussion forum for help
- **Issues**: Report bugs in the GitHub repository

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
- Added advanced simulation features
- Added comprehensive documentation

## Future Enhancements

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
