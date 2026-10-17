# NLM Python Bindings - Advanced Features

This document describes the advanced Python features and utilities available in the NLM Python bindings.

## Overview

The NLM Python bindings provide a comprehensive interface for working with the NLM (Neural Learning Machine) framework from Python. This document covers the advanced features, utilities, and patterns for using NLM in Python.

## What This Document Covers

1. **Performance Monitoring** - Track and analyze simulation performance
2. **Simulation Utilities** - Convenience functions for common simulation patterns
3. **Visualization Utilities** - Basic plotting and visualization of results
4. **Experiment Utilities** - Helper functions for conducting experiments
5. **Context Management** - Resource management and cleanup
6. **Convenience Functions** - Quick access to common patterns
7. **Best Practices** - Guidelines for effective NLM usage

## 1. Performance Monitoring

The `PerformanceMonitor` class provides comprehensive performance tracking for NLM simulations.

### Key Features

- **Operation Timing**: Time specific operations with decorators
- **Memory Monitoring**: Track memory usage over time
- **Custom Metrics**: Add custom metrics to track
- **Statistical Analysis**: Get timing statistics (min, max, mean, total)
- **Visualization**: Plot timing and memory usage
- **Report Generation**: Generate detailed performance reports

### Usage Examples

```python
from nlm_utilities import PerformanceMonitor

# Create a performance monitor
monitor = PerformanceMonitor("MySimulation")

# Time an operation with decorator
@monitor.time_operation("brain_step")
def brain_step(step):
    brain.step(step)

# Run simulation with monitoring
for step in range(1000):
    brain_step(step)
    monitor.sample_memory("brain")

# Generate and display report
monitor.print_report()

# Plot results
monitor.plot_timing("brain_step", "timing_plot.png")
monitor.plot_memory_usage("memory_plot.png")
```

### Key Methods

- `time_operation(operation_name)`: Decorator to time operations
- `sample_memory(component_name)`: Sample memory usage
- `add_metric(name, value, description)`: Add custom metrics
- `get_timing_stats(operation_name)`: Get statistics for timed operations
- `plot_timing(operation_name, save_path)`: Plot timing data
- `plot_memory_usage(save_path)`: Plot memory usage
- `print_report()`: Print detailed performance report

## 2. Simulation Utilities

The `SimulationUtilities` class provides convenience functions for common simulation patterns.

### Key Features

- **Experiment Creation**: Create standard experiment setups
- **Simulation Runner**: Run simulations with optional monitoring
- **Benchmark Suite**: Run benchmarks for different configurations
- **Configuration Management**: Create and manage configurations

### Usage Examples

```python
from nlm_utilities import SimulationUtilities, PerformanceMonitor

# Create a standard experiment
brain, world, agent = SimulationUtilities.create_reproduction_experiment("standard")

# Run simulation with monitoring
monitor = PerformanceMonitor("MySimulation")
SimulationUtilities.run_learning_simulation(
    brain, world, agent, 1000, monitor
)

# Run benchmark suite
results = SimulationUtilities.run_benchmark_suite(monitor)
```

### Key Methods

- `create_reproduction_experiment(config_name)`: Create experiment setup
- `run_learning_simulation(brain, world, agent, num_steps, monitor, callback)`: Run simulation
- `create_benchmark_suite()`: Create benchmark suite
- `run_benchmark_suite(monitor)`: Run benchmarks

## 3. Visualization Utilities

The `VisualizationUtilities` class provides basic plotting and visualization capabilities.

### Key Features

- **Brain Activity**: Plot brain activity statistics
- **Agent Metrics**: Plot agent neuromodulation metrics
- **Training Curves**: Plot training curves from simulation data
- **Matplotlib Integration**: Uses matplotlib for plotting (install with `pip install matplotlib`)

### Usage Examples

```python
from nlm_utilities import VisualizationUtilities

# Plot brain activity
VisualizationUtilities.plot_brain_activity(brain, "brain_activity.png")

# Plot agent metrics
VisualizationUtilities.plot_agent_metrics(agent, "agent_metrics.png")

# Create training curves
metrics = {
    'firing_rate': [0.1, 0.15, 0.2, 0.25, 0.3],
    'spike_count': [10, 25, 50, 75, 100],
    'memory_usage': [50, 60, 70, 80, 90]
}
VisualizationUtilities.create_training_curves_plot(metrics, "training.png")
```

### Key Methods

- `plot_brain_activity(brain, save_path)`: Plot brain activity statistics
- `plot_agent_metrics(agent, save_path)`: Plot agent metrics
- `create_training_curves_plot(data, save_path)`: Create training curves

## 4. Experiment Utilities

The `ExperimentUtilities` class provides helper functions for conducting experiments.

### Key Features

- **Experiment Creation**: Create experiment configurations
- **Experiment Running**: Run complete experiments
- **Result Saving**: Save experiment results to files
- **Result Comparison**: Compare multiple experiment results

### Usage Examples

```python
from nlm_utilities import ExperimentUtilities

# Run a complete experiment
results = ExperimentUtilities.run_experiment(
    "learning_experiment",
    config_name="standard",
    steps=5000,
    variations={"neuronCount": 1500},
    save_results=True
)

# Compare multiple experiments
comparison = ExperimentUtilities.compare_experiments([results1, results2, results3])
```

### Key Methods

- `create_experiment_config(base_config_name, variations)`: Create experiment configuration
- `run_experiment(experiment_name, config_name, steps, variations, monitor, save_results)`: Run experiment
- `save_experiment_results(results, experiment_name, directory)`: Save results
- `compare_experiments(experiment_results)`: Compare experiment results

## 5. Context Management

The `NLMContextManager` class provides context management for NLM resources.

### Key Features

- **Resource Management**: Automatic setup and cleanup of NLM components
- **Context Safety**: Ensure proper cleanup even if exceptions occur
- **Statistics Access**: Get statistics from within context
- **Simulation Integration**: Run simulations within context

### Usage Examples

```python
from nlm_utilities import NLMContextManager

# Use context manager
with NLMContextManager("standard") as context:
    # Components are automatically created
    brain = context.brain
    world = context.world
    agent = context.agent
    monitor = context.monitor
    
    # Run simulation within context
    context.run_simulation(1000)
    
    # Get statistics
    stats = context.get_stats()
    print(f"Neurons: {stats['brain_stats']['neurons']}")
```

### Key Methods

- `__enter__()`: Enter context and create components
- `__exit__()`: Exit context and clean up
- `run_simulation(steps, callback)`: Run simulation within context
- `get_stats()`: Get current statistics

## 6. Convenience Functions

The module provides several convenience functions for common patterns:

### Key Functions

- `create_experiment_brain(config_name)`: Create a configured brain
- `quick_test(config_name, steps)`: Run a quick test
- `benchmark_configurations()`: Run benchmarks for all configurations

### Usage Examples

```python
from nlm_utilities import create_experiment_brain, quick_test, benchmark_configurations

# Create a configured brain
brain = create_experiment_brain("standard")

# Run a quick test
brain = quick_test("fast", 500)

# Run benchmarks
results = benchmark_configurations()
```

## 7. Example Usage

### Simple Simulation Example

```python
"""Example: Simple simulation with basic features"""

from nlm_utilities import example_simple_simulation, example_learning_simulation, example_benchmark

# Run examples
example_simple_simulation()
example_learning_simulation()
example_benchmark()
```

### Custom Experiment Example

```python
"""Example: Custom experiment with variations"""

from nlm_utilities import ExperimentUtilities, PerformanceMonitor

# Create performance monitor
monitor = PerformanceMonitor("CustomExperiment")

# Run custom experiment with variations
results = ExperimentUtilities.run_experiment(
    "curiosity_experiment",
    config_name="standard",
    steps=3000,
    variations={
        "neuronCount": 1200,
        "regionCount": 3
    },
    monitor=monitor,
    save_results=True
)

# Display results
print(f"Experiment completed in {results['duration']:.2f} seconds")
print(f"Total spikes: {results['total_spikes']}")
print(f"Average firing rate: {results['avg_firing_rate']:.2f} Hz")
```

### Performance Monitoring Example

```python
"""Example: Advanced performance monitoring"""

from nlm_utilities import PerformanceMonitor, NLMContextManager

# Method 1: Manual monitoring
monitor = PerformanceMonitor("ManualSimulation")

with NLMContextManager("standard") as context:
    # Run simulation with monitoring
    context.run_simulation(2000)
    
    # Get performance report
    monitor.print_report()

# Method 2: Context manager with monitoring
with NLMContextManager("standard") as context:
    context.run_simulation(1000)
    
    # Automatic performance tracking
    if context.monitor:
        context.monitor.print_report()
```

## 8. Installation and Requirements

### Basic Installation

```bash
# Install NLM Python bindings
pip install pynlm

# Install advanced utilities
pip install -e .  # Install in development mode
```

### Visualization Support

```bash
# Install matplotlib for plotting
pip install matplotlib
```

### Additional Monitoring Tools

```bash
# Install psutil for memory monitoring
pip install psutil
```

### Complete Development Installation

```bash
# Clone the repository
cd /path/to/nlm

# Install in development mode
pip install -e .

# Install optional dependencies
pip install matplotlib psutil
```

## 9. Best Practices

### Performance Optimization

1. **Use Performance Monitoring**: Always use `PerformanceMonitor` for production simulations
2. **Monitor Memory**: Sample memory usage regularly, especially for long simulations
3. **Profile Bottlenecks**: Use timing to identify performance bottlenecks
4. **Optimize Configurations**: Use appropriate configurations for your use case

### Experiment Design

1. **Document Configurations**: Always document your experiment configurations
2. **Save Results**: Use `save_results=True` for important experiments
3. **Use Benchmarks**: Compare your results against benchmarks
4. **Iterate Systematically**: Make one change at a time and measure the effect

### Visualization

1. **Save Plots**: Always save plots to files for documentation
2. **Use Appropriate Scales**: Choose appropriate axis scales for your data
3. **Include Legends**: Always include legends and labels
4. **Consistency**: Use consistent styling across multiple plots

### Error Handling

1. **Context Managers**: Use `NLMContextManager` for safe resource management
2. **Exception Handling**: Handle exceptions gracefully
3. **Resource Cleanup**: Always ensure resources are properly cleaned up
4. **Error Reporting**: Include detailed error messages in logs

## 10. Troubleshooting

### Common Issues

1. **Matplotlib Not Available**: Install matplotlib with `pip install matplotlib`
2. **Memory Monitoring Issues**: Install psutil with `pip install psutil`
3. **Import Errors**: Ensure all dependencies are installed
4. **Performance Issues**: Use slower configurations or reduce simulation steps

### Error Messages

- "matplotlib not available for plotting": Install matplotlib
- "psutil not available for memory monitoring": Install psutil
- "ModuleNotFoundError: No module named 'nlm_utilities'": Check installation

## 11. API Reference

### Classes

- `PerformanceMonitor`: Performance monitoring utilities
- `SimulationUtilities`: Simulation convenience functions
- `VisualizationUtilities`: Visualization utilities
- `ExperimentUtilities`: Experiment utilities
- `NLMContextManager`: Context manager for NLM resources

### Functions

- `create_experiment_brain(config_name)`: Create configured brain
- `quick_test(config_name, steps)`: Run quick test
- `benchmark_configurations()`: Run benchmark suite

## 12. Further Reading

- **NLM Documentation**: Refer to the main NLM documentation for detailed information
- **Python Bindings**: Refer to the Python bindings documentation for API details
- **Performance Tuning**: Refer to performance documentation for optimization tips
- **Experiment Guidelines**: Refer to experiment guidelines for best practices

## 13. License

This utility is distributed under the NLM license. See the main NLM documentation for license details.

---

*For questions, issues, or feature requests, please refer to the main NLM documentation or GitHub repository.*
