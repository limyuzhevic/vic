# NLM Python Bindings - Quick Start Guide

## Overview

This guide introduces the enhanced NLM (Neural Learning Machine) Python bindings, showcasing the Pythonic interfaces and advanced features that make neural simulation more accessible, powerful, and enjoyable for Python developers.

## Key Improvements

### 1. **Pythonic Interface Design**
- Context managers for automatic resource management
- Property-based access for commonly used attributes
- Enhanced configuration validation
- Type-safe factory functions

### 2. **Advanced Simulation Features**
- Batch processing for efficient simulation
- Episode-based simulation with tracking
- Comprehensive monitoring and analysis tools
- Checkpoint management

### 3. **Developer Experience**
- Comprehensive documentation and examples
- Utility functions for common tasks
- Multiple output formats (JSON, CSV, reports)
- Visualization helpers

## Installation

The enhanced Python bindings are already installed in this environment. The package structure includes:

```
python/
├── __init__.py                    # Main package interface
├── brain.py                       # Enhanced Brain interface
├── agent.py                       # Agent implementation
├── sensory.py                     # Sensory input classes
├── motor.py                       # Motor action classes
├── world.py                       # World simulation
├── enums.py                       # Type definitions
├── simulation.py                  # Advanced simulation features
├── monitoring.py                  # Monitoring and analysis
├── utils.py                       # Utility functions
└── constants.py                   # Constants and helpers
```

## Basic Usage

### 1. Simple Brain Simulation

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### 2. Enhanced Configuration

```python
import pynlm
from pynlm.utils import create_config, validate_config

# Create custom configuration
config = create_config(neuron_count=500, synapse_density=0.15)

# Validate configuration
result = validate_config(config)
if result['valid']:
    print("Configuration is valid")
else:
    print(f"Errors: {result['errors']}")
```

### 3. Pythonic Property Access

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Use property-based access
print(f"Neurons: {brain.total_neurons}")
print(f"Active: {brain.active_neurons}")
print(f"Firing rate: {brain.average_firing_rate:.3f}")
print(f"E/I ratio: {brain.e_i_ratio:.3f}")
```

## Advanced Features

### 1. Context Manager

```python
import pynlm

# Use context manager for automatic cleanup
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    # Brain automatically cleaned up
    print("Brain created and used")
```

### 2. Batch Processing

```python
import pynlm
from pynlm.simulation import BatchSimulator

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

simulator = BatchSimulator(brain)
results = simulator.run_batch(1000, timestep=0.001, enable_monitoring=True)

print(f"Batch simulation completed:")
print(f"  Steps: {results['total_steps']}")
print(f"  Time: {results['total_time']:.3f}s")
print(f"  Steps/s: {results['steps_per_second']:.1f}")
```

### 3. Monitoring and Analysis

```python
import pynlm
from pynlm.monitoring import StatisticsCollector, PerformanceMonitor

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create monitoring tools
collector = StatisticsCollector(pynlm.MonitoringLevel.COMPREHENSIVE)
monitor = PerformanceMonitor()

# Run simulation with monitoring
for step in range(100):
    brain.step(step)
    metrics = brain.get_statistics()
    collector.record_step(step, metrics)

# Get reports
stats = collector.get_summary_statistics()
performance = monitor.get_performance_report()
print(f"Statistics: {stats}")
print(f"Performance: {performance}")
```

### 4. Episode-based Simulation

```python
import pynlm
from pynlm.simulation import EpisodeRunner

# Create complete agent system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create episode runner
runner = EpisodeRunner(brain, world, agent)

# Run episode
episode = runner.run_episode(max_steps=100, enable_learning=True)

print(f"Episode completed: {episode.steps} steps, {episode.total_spikes} spikes")
```

### 5. Utility Functions

```python
import pynlm
from pynlm.utils import create_config, analyze_results, OutputFormatter

# Create sample data
sample_data = [
    {'step': 0, 'total_spikes': 5, 'firing_rate': 0.5},
    {'step': 1, 'total_spikes': 8, 'firing_rate': 0.8},
    {'step': 2, 'total_spikes': 12, 'firing_rate': 1.2},
]

# Analyze results
analysis = analyze_results(sample_data, 'summary')
print(f"Analysis: {analysis}")

# Format output
formatter = OutputFormatter()
json_output = formatter.format_json(analysis)
print(f"JSON output: {json_output}")
```

## Enumeration Types

The enhanced bindings provide comprehensive type definitions:

```python
import pynlm

# Neuron types
print(f"Neuron types: {[t.value for t in pynlm.NeuronType]}")

# Action types
print(f"Action types: {[t.value for t in pynlm.ActionType]}")

# Developmental stages
print(f"Developmental stages: {[s.value for s in pynlm.DevelopmentalStage]}")

# World object types
print(f"World object types: {[t.value for t in pynlm.WorldObjectType]}")
```

## Configuration Options

The enhanced configuration system provides:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

# Validate and use
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.get('brain.neuron_count', 1000)}")
```

## Error Handling and Validation

The enhanced bindings include comprehensive error handling:

```python
import pynlm
from pynlm.utils import validate_config

# Test various configurations
configs = [
    {'brain': {'neuron_count': 100}},
    {'brain': {'neuron_count': 1000, 'synapse_density': 0.2}},
    {'brain': {'neuron_count': 10000, 'synapse_density': 0.5}},  # May cause warnings
]

for i, config in enumerate(configs):
    result = validate_config(config)
    print(f"Config {i+1}: {'Valid' if result['valid'] else 'Invalid'}")
    if result['warnings']:
        print(f"  Warnings: {result['warnings']}")
```

## Performance Monitoring

The enhanced bindings provide comprehensive performance monitoring:

```python
import pynlm
from pynlm.monitoring import PerformanceMonitor

# Create and start monitor
monitor = PerformanceMonitor()
monitor.start_monitoring()

# Run simulation (add your simulation code here)

# Get performance report
monitor.stop_monitoring()
performance = monitor.get_performance_report()

print(f"Performance metrics:")
print(f"  Steps: {performance.get('steps', 0)}")
print(f"  Total time: {performance.get('total_time', 0):.3f}s")
print(f"  Steps per second: {performance.get('steps_per_second', 0):.1f}")
```

## File Input/Output

The enhanced bindings support multiple file formats:

### JSON Export
```python
import pynlm
from pynlm.utils import analyze_results

# Analyze simulation results
results = analyze_results(your_data, 'detailed')

# Export to JSON
import json
json_output = json.dumps(results, indent=2, default=str)
with open('results.json', 'w') as f:
    f.write(json_output)
```

### CSV Export
```python
import pynlm
from pynlm.utils import OutputFormatter

# Format as CSV
formatter = OutputFormatter()
csv_output = formatter.format_csv(your_data)
with open('results.csv', 'w') as f:
    f.write(csv_output)
```

## Visualization

The enhanced bindings include visualization helpers:

```python
import pynlm
from pynlm.utils import VisualizationHelper

# Create visualization helper
viz = VisualizationHelper(style='scientific')

# Plot neural activity
fig = viz.plot_neural_activity(your_data)
viz.save_figure('neural_activity.png')

# Create performance metrics plot
fig = viz.plot_performance_metrics(performance_data)
viz.save_figure('performance.png')
```

## Best Practices

### 1. Use Context Managers
```python
# Always use context managers for resource management
with pynlm.createBrain(config) as brain:
    brain.initialize()
    # Your code here
# Brain automatically cleaned up
```

### 2. Validate Configurations
```python
# Always validate configurations before use
from pynlm.utils import validate_config

result = validate_config(config)
if not result['valid']:
    raise ValueError(f"Invalid configuration: {result['errors']}")
```

### 3. Use Property-Based Access
```python
# Use property-based access for commonly accessed attributes
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.total_neurons}")
print(f"Active: {brain.active_neurons}")
```

### 4. Leverage Batch Processing
```python
# Use batch processing for multiple simulation steps
from pynlm.simulation import BatchSimulator

simulator = BatchSimulator(brain)
results = simulator.run_batch(1000, timestep=0.001)
```

### 5. Monitor Performance
```python
# Always monitor performance for optimization
from pynlm.monitoring import PerformanceMonitor

monitor = PerformanceMonitor()
monitor.start_monitoring()

# Your simulation code here

monitor.stop_monitoring()
performance = monitor.get_performance_report()
```

## Troubleshooting

### Common Issues and Solutions

**Issue: Configuration validation errors**
```python
# Solution: Use the enhanced configuration system
from pynlm.utils import create_config, validate_config

config = create_config(neuron_count=1000)
result = validate_config(config)
if not result['valid']:
    print(f"Configuration errors: {result['errors']}")
```

**Issue: Performance monitoring not working**
```python
# Solution: Ensure monitoring tools are properly initialized
from pynlm.monitoring import PerformanceMonitor

monitor = PerformanceMonitor()
monitor.start_monitoring()
# Run your simulation
monitor.stop_monitoring()
performance = monitor.get_performance_report()
```

**Issue: Context manager not working**
```python
# Solution: Use the enhanced Brain class with context manager support
from pynlm.brain import Brain

with Brain(config) as brain:
    brain.initialize()
    # Your code here
```

**Issue: Missing Python modules**
```python
# Solution: Ensure all Python modules are properly installed
sys.path.insert(0, '/path/to/nlm/python')
import pynlm
```

## Migration Guide

### From Basic to Enhanced Bindings

**Basic Usage:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for step in range(100):
    brain.step(step)
```

**Enhanced Usage:**
```python
import pynlm

# Use enhanced interfaces
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    # Use property-based access
    print(f"Neurons: {brain.total_neurons}")
    # Use enhanced configuration
    config = pynlm.create_config(neuron_count=1000)
    # Use batch processing
    from pynlm.simulation import BatchSimulator
    simulator = BatchSimulator(brain)
    results = simulator.run_batch(1000)
```

## Conclusion

The enhanced NLM Python bindings provide a comprehensive, Pythonic interface for neural simulation that balances ease of use with advanced functionality. The improvements include:

- **Pythonic interfaces** with context managers and property-based access
- **Enhanced configuration validation** and type safety
- **Advanced simulation features** including batch processing and episode tracking
- **Comprehensive monitoring and analysis** tools
- **Utility functions** for common tasks
- **Robust error handling** and documentation

These improvements make NLM more accessible to Python developers while providing the power and flexibility needed for complex neural simulation research and applications.