# Enhanced Visualization Module - User Guide

## Quick Start Guide

This document provides a comprehensive guide to using the enhanced visualization module for the NLM framework.

## Table of Contents

1. [Introduction](#introduction)
2. [Getting Started](#getting-started)
3. [Real-time Visualization](#real-time-visualization)
4. [Advanced Analytics](#advanced-analytics)
5. [Debugging Tools](#debugging-tools)
6. [Interactive Visualization](#interactive-visualization)
7. [Export Capabilities](#export-capabilities)
8. [Comparison Tools](#comparison-tools)
9. [Educational Tools](#educational-tools)
10. [API Reference](#api-reference)
11. [Examples](#examples)
12. [Troubleshooting](#troubleshooting)

## Introduction

The enhanced visualization module provides comprehensive visualization and debugging capabilities for the NLM framework. It includes real-time monitoring, advanced analytics, debugging tools, interactive visualization, export capabilities, comparison tools, and educational resources.

## Getting Started

### Prerequisites

- Python 3.8 or higher
- NLM framework installed
- Required Python packages: `matplotlib`, `plotly`, `pandas`, `numpy`

### Installation

```bash
pip install matplotlib plotly pandas numpy
```

### Basic Usage

```python
from python import (
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    create_advanced_agent,
)
from python.visualization import (
    real_time_monitor,
    analyze_brain_statistics,
    enable_debug_logging,
    export_to_csv,
    export_to_json,
)

# Create NLM components
config = createDefaultConfig()
brain = createBrain(config)
world = createSimpleWorld(100, 100, 20, 20)
agent = create_advanced_agent(brain, world)

# Enable debug logging
enable_debug_logging(brain, level='info', file='debug.log')

# Run simulation (simplified example)
for step in range(100):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step)

# Analyze results
stats = analyze_brain_statistics(brain)
print(f"Analysis results: {stats}")

# Export data
export_to_csv(brain, "simulation_results.csv", format='spike_data')
export_to_json(brain, "simulation_results.json", include_metadata=True)
```

## Real-time Visualization

### real_time_monitor()

Real-time brain monitoring with live updates.

```python
from python.visualization import real_time_monitor

# Start real-time monitoring
monitor_result = real_time_monitor(brain, world, agent, update_interval=0.1)
print(f"Monitoring started: {monitor_result}")
```

### RealTimeVisualizer Class

```python
from python.visualization import RealTimeVisualizer

# Create visualizer
visualizer = RealTimeVisualizer(brain, world, agent)

# Start monitoring
visualizer.start_monitoring(update_interval=0.1)

# Get current state
state = visualizer.get_current_state()
print(f"Current brain state: {state}")

# Export data
export_result = visualizer.export_realtime_data("realtime_data.json", "json")
print(f"Exported: {export_result}")

# Stop monitoring
visualizer.stop_monitoring()
```

## Advanced Analytics

### analyze_brain_statistics()

Compute comprehensive statistical metrics from brain activity.

```python
from python.visualization import analyze_brain_statistics

# Get statistics
stats = analyze_brain_statistics(brain, metrics=['firing_rate', 'spike_count', 'memory_usage'])
print(f"Brain statistics: {stats}")
```

### AnalyticsEngine Class

```python
from python.visualization import AnalyticsEngine

# Create analytics engine
analytics = AnalyticsEngine(brain)

# Profile performance
performance = analytics.profile_performance(num_steps=1000)
print(f"Performance profile: {performance}")

# Monitor resources
resources = analytics.monitor_resource_usage(interval=5.0)
print(f"Resource usage: {resources}")
```

## Debugging Tools

### enable_debug_logging()

Enable detailed logging with configurable levels.

```python
from python.visualization import enable_debug_logging

# Enable debug logging
logger_info = enable_debug_logging(brain, level='info', file='debug.log')
print(f"Debug logging enabled: {logger_info}")
```

### DebugLogger Class

```python
from python.visualization import DebugLogger, LogLevel

# Create debug logger
logger = DebugLogger(brain, LogLevel.INFO, "debug.log")

# Log component health
logger.log_component_health('brain', {
    'neuron_count': brain.getTotalNeuronCount(),
    'firing_rate': brain.getAverageFiringRate(),
})

# Log error
logger.log_error('spike_generation', 'Failed to generate spike at neuron 42', component='brain')

# Get error report
error_report = logger.get_error_report()
print(f"Error report: {error_report}")
```

### track_errors() and monitor_component_health()

```python
from python.visualization import track_errors, monitor_component_health

# Track errors with callback
def error_callback(error_info):
    print(f"Error detected: {error_info}")

error_tracker = track_errors(brain, error_callback)

# Monitor component health
health_info = monitor_component_health(brain)
print(f"Component health: {health_info}")
```

## Interactive Visualization

### create_matplotlib_plots()

Create static plots using matplotlib.

```python
from python.visualization import create_matplotlib_plots

# Create matplotlib plots
plots = create_matplotlib_plots(brain, world, agent, output_file="brain_plots.png")
print(f"Plots created: {plots}")
```

### create_interactive_plots()

Create interactive plots with plotly.

```python
from python.visualization import create_interactive_plots

# Create interactive plots
interactive_plots = create_interactive_plots(brain, world, agent)
print(f"Interactive plots created: {interactive_plots}")
```

### create_jupyter_notebook()

Create Jupyter notebook with visualization.

```python
from python.visualization import create_jupyter_notebook

# Create Jupyter notebook
notebook_info = create_jupyter_notebook(brain, world, agent, output_file="nlm_visualization.ipynb")
print(f"Notebook created: {notebook_info}")
```

### InteractivePlotting Class

```python
from python.visualization import InteractivePlotting

# Create interactive plotting instance
plotting = InteractivePlotting(brain, world, agent)

# Various plotting methods
matplotlib_plots = plotting.create_matplotlib_plots()
interactive_plots = plotting.create_interactive_plots()
notebook = plotting.create_jupyter_notebook()
```

## Export Capabilities

### export_to_csv(), export_to_json(), export_to_image()

Export data in various formats.

```python
from python.visualization import export_to_csv, export_to_json, export_to_image

# Export to CSV
export_to_csv(brain, "brain_data.csv", format='neuron_data')

# Export to JSON
export_to_json(brain, "brain_data.json", include_metadata=True)

# Export to image
export_to_image(brain, "brain_viz.png", format='png', dpi=150)
```

### ExportManager Class

```python
from python.visualization import ExportManager

# Create export manager
exporter = ExportManager(brain)

# Export in different formats
csv_result = exporter.export_to_csv("data.csv", format='spike_data')
json_result = exporter.export_to_json("data.json", include_metadata=True)
image_result = exporter.export_to_image("visualization.png", format='png', dpi=300)
```

## Comparison Tools

### compare_simulations(), compare_performance(), compare_configurations()

Compare multiple simulations or configurations.

```python
from python.visualization import compare_simulations, compare_performance, compare_configurations

# Compare simulations
simulations = [
    {'id': 'sim1', 'brain': {...}, 'performance': {...}},
    {'id': 'sim2', 'brain': {...}, 'performance': {...}},
]
comparison = compare_simulations(simulations)
print(f"Simulation comparison: {comparison}")

# Compare performance
configs = [
    {'id': 'config1', 'parameters': {...}},
    {'id': 'config2', 'parameters': {...}},
]
performance_comparison = compare_performance(configs, duration=60.0)
print(f"Performance comparison: {performance_comparison}")

# Compare configurations
config1 = {'id': 'config1', 'parameters': {...}}
config2 = {'id': 'config2', 'parameters': {...}}
config_comparison = compare_configurations(config1, config2)
print(f"Configuration comparison: {config_comparison}")
```

### ComparisonAnalyzer Class

```python
from python.visualization import ComparisonAnalyzer

# Create comparison analyzer
analyzer = ComparisonAnalyzer()

# Perform various comparisons
comparison_results = analyzer.compare_simulations(simulations)
performance_results = analyzer.compare_performance(configs)
config_results = analyzer.compare_configurations(config1, config2)
```

## Educational Tools

### create_tutorial(), generate_example(), create_learning_mode()

Create educational content and learning environments.

```python
from python.visualization import create_tutorial, generate_example, create_learning_mode

# Create tutorial
tutorial = create_tutorial(brain, topic='basic_usage')
print(f"Tutorial created: {tutorial}")

# Generate example
example = generate_example(config_type='complete_example')
print(f"Example generated: {example}")

# Create learning mode
learning_mode = create_learning_mode(brain, world, agent)
print(f"Learning mode created: {learning_mode}")
```

### EducationalToolkit Class

```python
from python.visualization import EducationalToolkit

# Create educational toolkit
toolkit = EducationalToolkit()

# Educational methods
tutorial = toolkit.create_tutorial(brain, topic='advanced_analytics')
example = toolkit.generate_example('basic_example')
learning_env = toolkit.create_learning_mode(brain, world, agent)
```

## Global Visualization Instances

### initialize_visualization() and shutdown_visualization()

Initialize and manage global visualization instances.

```python
from python.visualization import (
    initialize_visualization,
    shutdown_visualization,
    get_global_visualizer,
    get_global_analytics,
    get_global_debug_logger,
    get_global_exporter,
    get_global_comparison_analyzer,
    get_global_educational_toolkit,
)

# Initialize global instances
init_result = initialize_visualization(brain, world, agent)
print(f"Initialization result: {init_result}")

# Get global instances
visualizer = get_global_visualizer()
analytics = get_global_analytics()
logger = get_global_debug_logger()
exporter = get_global_exporter()
analyzer = get_global_comparison_analyzer()
toolkit = get_global_educational_toolkit()

# Shutdown visualiz
# Shutdown global instances
shutdown_result = shutdown_visualization()
print(f"Shutdown result: {shutdown_result}")
```

## Feature Availability

### is_visualization_feature_available()

Check if a visualization feature is available.

```python
from python.visualization import is_visualization_feature_available

# Check features
real_time_available = is_visualization_feature_available('real_time_visualization')
analytics_available = is_visualization_feature_available('advanced_analytics')
debug_available = is_visualization_feature_available('debugging_tools')
```

### get_visualization_capabilities()

Get all available visualization capabilities.

```python
from python.visualization import get_visualization_capabilities

# Get capabilities
capabilities = get_visualization_capabilities()
for feature, info in capabilities.items():
    print(f"{feature}: {info['description']}")
```

## Examples

### Example 1: Complete Simulation with Analytics

```python
"""
Complete NLM simulation with full analytics and visualization.
"""

from python import (
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    create_advanced_agent,
)
from python.visualization import (
    AnalyticsEngine,
    RealTimeVisualizer,
    ExportManager,
    create_matplotlib_plots,
    analyze_brain_statistics,
    enable_debug_logging,
    initialize_visualization,
)

# Setup
config = createDefaultConfig()
brain = createBrain(config)
world = createSimpleWorld(200, 200, 30, 30)
agent = create_advanced_agent(brain, world, {'enable_curiosity': True})

# Initialize global visualization
initialize_visualization(brain, world, agent)

# Run simulation
for step in range(500):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step)

# Analytics and visualization
analytics = AnalyticsEngine(brain)
stats = analytics.analyze_brain_statistics()

visualizer = RealTimeVisualizer(brain, world, agent)
visualizer.start_monitoring()
import time
time.sleep(2)  # Monitor for 2 seconds
visualizer.stop_monitoring()

# Create plots
plots = create_matplotlib_plots(brain, world, agent, output_file="simulation_plots.png")

# Export data
exporter = ExportManager(brain)
exporter.export_to_csv("simulation_data.csv", format='spike_data')
exporter.export_to_json("simulation_data.json")

print("Simulation completed with full analytics!")
```

### Example 2: Real-time Monitoring with Debug Logging

```python
"""
Real-time monitoring with debug logging and error tracking.
"""

from python import createDefaultConfig, createBrain, createSimpleWorld
from python.visualization import (
    enable_debug_logging,
    real_time_monitor,
    track_errors,
    monitor_component_health,
)

# Setup
config = createDefaultConfig()
brain = createBrain(config)
world = createSimpleWorld(100, 100, 20, 20)

# Enable debug logging
enable_debug_logging(brain, level='info', file='debug.log')

# Track errors
error_tracker = track_errors(brain, lambda error: print(f"ERROR: {error}"))

# Start real-time monitoring
monitor_result = real_time_monitor(brain, world, None, update_interval=0.2)
print(f"Real-time monitoring: {monitor_result}")

# Monitor component health
health_info = monitor_component_health(brain)
print(f"Component health: {health_info}")
```

### Example 3: Analytics and Export Pipeline

```python
"""
Analytics pipeline with comprehensive data export.
"""

from python import createDefaultConfig, createBrain
from python.visualization import (
    AnalyticsEngine,
    ExportManager,
    compare_simulations,
    analyze_brain_statistics,
    profile_brain_performance,
)

# Setup
config = createDefaultConfig()
brain = createBrain(config)

# Analytics engine
analytics = AnalyticsEngine(brain)

# Analyze brain activity
brain_stats = analytics.analyze_brain_statistics()
print(f"Brain statistics: {brain_stats}")

# Performance profiling
performance = analytics.profile_performance(num_steps=1000)
print(f"Performance profile: {performance}")

# Export data
exporter = ExportManager(brain)

# Export to multiple formats
csv_result = exporter.export_to_csv("brain_data.csv", format='neuron_data')
json_result = exporter.export_to_json("brain_data.json", include_metadata=True)
image_result = exporter.export_to_image("brain_viz.png", format='png', dpi=150)

print("Analytics and export pipeline completed!")
```

## Troubleshooting

### Common Issues

1. **Import errors**
   ```python
   # Ensure all required packages are installed
   pip install matplotlib plotly pandas numpy
   ```

2. **Missing dependencies**
   ```python
   # The enhanced visualization module requires:
   - matplotlib (for static plots)
   - plotly (for interactive plots)
   - pandas (for data export)
   - numpy (for numerical operations)
   ```

3. **Real-time monitoring issues**
   ```python
   # Check update interval
   visualizer.start_monitoring(update_interval=0.1)  # Too small, try 0.1 or higher
   visualizer.start_monitoring(update_interval=1.0)   # Too large, try 0.5 or smaller
   ```

4. **Export file permissions**
   ```python
   # Ensure write permissions to output directory
   import os
   os.makedirs("output_directory", exist_ok=True)
   ```

5. **Memory usage during large simulations**
   ```python
   # Monitor memory usage
   analytics = AnalyticsEngine(brain)
   resources = analytics.monitor_resource_usage(interval=5.0)
   
   # If memory is high, consider:
   # - Reducing data collection frequency
   # - Using compression for exports
   # - Implementing data sampling
   ```

### Error Handling

```python
import traceback

def safe_function():
    try:
        # Your code here
        pass
    except ImportError as e:
        print(f"Missing dependency: {e}")
        print("Please install required packages with: pip install matplotlib plotly pandas numpy")
    except Exception as e:
        print(f"Unexpected error: {e}")
        traceback.print_exc()
```

### Debugging Tips

1. **Check visualization initialization:**
   ```python
   from python.visualization import initialize_visualization
   initialize_visualization(brain, world, agent)
   ```

2. **Enable debug logging:**
   ```python
   from python.visualization import enable_debug_logging
   enable_debug_logging(brain, level='debug', file='debug.log')
   ```

3. **Monitor component health:**
   ```python
   from python.visualization import monitor_component_health
   health = monitor_component_health(brain)
   print(f"Health status: {health}")
   ```

4. **Export data for analysis:**
   ```python
   from python.visualization import export_to_csv, export_to_json
   export_to_csv(brain, "debug_data.csv")
   export_to_json(brain, "debug_data.json")
   ```

## API Reference

### Constants

- `LogLevel`: Enum for logging levels (DEBUG, INFO, WARNING, ERROR, CRITICAL)
- `VisualizationStatus`: Enum for visualization tool status (INACTIVE, ACTIVE, RUNNING, PAUSED, STOPPED)

### Classes

- `AnalyticsEngine`: Advanced analytics engine for brain activity analysis
- `DebugLogger`: Debug logging with detailed output and error tracking
- `RealTimeVisualizer`: Real-time brain activity monitoring and visualization
- `InteractivePlotting`: Interactive plotting capabilities
- `ExportManager`: Export capabilities for visualization data
- `ComparisonAnalyzer`: Compare multiple simulations or configurations
- `EducationalToolkit`: Educational tools for learning and training

### Functions

All functions are listed in the documentation above by category.

### Global Functions

- `is_visualization_feature_available(feature)`: Check if a visualization feature is available
- `get_visualization_capabilities()`: Get all available visualization capabilities
- `initialize_visualization(brain, world, agent)`: Initialize global visualization instances
- `shutdown_visualization()`: Shutdown global visualization instances
- `get_global_visualizer()`: Get the global real-time visualizer instance
- `get_global_analytics()`: Get the global analytics engine instance
- `get_global_debug_logger()`: Get the global debug logger instance
- `get_global_exporter()`: Get the global export manager instance
- `get_global_comparison_analyzer()`: Get the global comparison analyzer instance
- `get_global_educational_toolkit()`: Get the global educational toolkit instance

## Version Information

- **Module Version**: 2.0.0 (Enhanced)
- **Python Version**: 3.8+
- **Framework**: NLM (Neural Learning Machine)
- **Last Updated**: 2026-10-20

## Support

For questions, issues, or feature requests:

1. Check the documentation in `visualization.py`
2. Review examples in the `examples/` directory
3. Use debug logging for troubleshooting
4. Report issues at: https://github.com/Kilo-Org/kilocode/issues

## Acknowledgements

This enhanced visualization module was developed as part of the NLM framework enhancement project. It provides comprehensive visualization and debugging capabilities for neural simulation research and analysis.

Special thanks to the NLM development team for their support and guidance throughout this project.
