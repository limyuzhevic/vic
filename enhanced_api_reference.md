# NLM Enhanced Python API Reference
# Comprehensive API Documentation for Advanced NLM Usage

This document provides a complete reference for the enhanced Python API of NLM (Neural Learning Machine). 
It covers both the original pynlm API and the new enhanced functionality added in this release.

## Table of Contents

1. [Introduction](#introduction)
2. [Basic API Overview](#basic-api-overview)
3. [Enhanced API - Advanced Functionality](#enhanced-api-advanced-functionality)
4. [Error Handling and Validation](#error-handling-and-validation)
5. [Configuration Management](#configuration-management)
6. [Experiment Management](#experiment-management)
7. [Batch Processing and Performance](#batch-processing-and-performance)
8. [Visualization and Analytics](#visualization-and-analytics)
9. [Migration Guide](#migration-guide)
10. [Example Applications](#example-applications)

## Introduction

The NLM Enhanced Python API extends the original pynlm library with advanced functionality for:

- **Advanced Configuration Management**: Preset configurations, parameter sweeps, and export/import capabilities
- **Experiment Management**: Comprehensive experiment tracking, templates, and result analysis
- **Batch Processing**: Parallel simulation execution and performance optimization
- **Enhanced Error Handling**: Robust error handling with detailed diagnostics
- **Advanced Visualization**: Network plots, time series dashboards, and performance analytics
- **Comprehensive Validation**: Input validation and error recovery mechanisms

## Basic API Overview

The basic API maintains backward compatibility with the original pynlm interface:

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize a brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Enhanced API - Advanced Functionality

### 1. Advanced Configuration Management

#### NLMSimulationConfig Class

```python
@dataclass
class NLMSimulationConfig:
    """Enhanced configuration for NLM simulations"""
    brain_neuron_count: int = 1000
    brain_synapse_density: float = 0.1
    brain_connection_probability: float = 0.05
    brain_initial_weight_mean: float = 0.5
    brain_initial_weight_std: float = 0.1
    brain_v_thresh: float = -50.0
    brain_v_rest: float = -70.0
    brain_v_reset: float = -75.0
    brain_tau_mem: float = 20.0
    brain_tau_ref: float = 2.0
    world_width: int = 20
    world_height: int = 20
    world_vision_width: int = 8
    world_vision_height: int = 8
    world_max_energy: float = 100.0
    world_energy_decay_rate: float = 0.01
    learning_stdp_enable: bool = True
    learning_stdp_learning_rate: float = 0.001
    learning_stdp_tau_plus: float = 20.0
    learning_stdp_tau_minus: float = 20.0
    learning_hebbian_enable: bool = True
    learning_structural_enable: bool = True
    neuromodulation_dopamine_scale: float = 1.0
    neuromodulation_curiosity_enable: bool = True
    neuromodulation_novelty_enable: bool = True
```

#### Configuration Presets

```python
# Create configuration from preset
from enhanced_pynlm import NLMSimulationConfig

# Minimal configuration
minimal_config = NLMSimulationConfig(
    brain_neuron_count=100,
    brain_synapse_density=0.01,
    learning_stdp_enable=False,
    neuromodulation_curiosity_enable=False
)

# Standard configuration
standard_config = NLMSimulationConfig()  # Default

# Advanced configuration
advanced_config = NLMSimulationConfig(
    brain_neuron_count=5000,
    brain_synapse_density=0.2,
    learning_stdp_enable=True,
    learning_hebbian_enable=True,
    learning_structural_enable=True,
    neuromodulation_curiosity_enable=True,
    neuromodulation_novelty_enable=True
)
```

### 2. Enhanced Error Handling and Validation

#### Exception Hierarchy

```python
class NLMError(Exception):
    """Base exception for NLM errors"""

class ConfigurationError(NLMError):
    """Configuration related error"""

class SimulationError(NLMError):
    """Simulation related error"""

class ValidationError(NLMError):
    """Validation related error"""

class PerformanceError(NLMError):
    """Performance related error"""
```

#### Validation Function

```python
from enhanced_pynlm import validate_config

# Validate configuration
errors = validate_config(config)
if errors:
    print("Configuration errors:", errors)
    raise ConfigurationError("Invalid configuration")
```

#### Error Handler

```python
from enhanced_pynlm import ErrorHandler, get_error_handler

error_handler = get_error_handler()

# Handle errors gracefully
try:
    brain.step(step)
except NLMError as e:
    if error_handler.handle_error(e):
        print("Error handled gracefully")
    else:
        print("Error will propagate")
```

### 3. Advanced Brain Interface

#### AdvancedBrain Class

```python
from enhanced_pynlm import AdvancedBrain, create_advanced_brain

# Create advanced brain with enhanced features
config = NLMSimulationConfig()
brain = create_advanced_brain(config)

# Run batch simulation with data collection
results = brain.run_batch_simulation(1000, step_size=10)

# Get performance metrics
metrics = brain.get_performance_metrics()
print(f"Performance metrics: {metrics}")

# Export simulation data
brain.export_simulation_data("simulation_results.json")
```

### 4. Experiment Management

#### NLMExperimentManager Class

```python
from enhanced_pynlm import create_experiment, create_experiment_template, run_parameter_sweep

# Create standard experiment
experiment = create_experiment(
    name="standard_test",
    config=standard_config,
    num_steps=1000,
    experiment_type="standard"
)

# Create experiment template
template_config = NLMSimulationConfig(brain_neuron_count=2000)
create_experiment_template("high_performance", template_config, "High-performance simulation")

# Run comparative experiment
comparative_results = create_experiment(
    name="comparative_test",
    config=standard_config,
    num_steps=500,
    experiment_type="comparative"
)

# Run parameter sweep
parameter_ranges = {
    "brain_neuron_count": [100, 500, 1000, 2000],
    "brain_synapse_density": [0.05, 0.1, 0.2]
}

sweep_results = run_parameter_sweep(
    base_config=standard_config,
    parameter_ranges=parameter_ranges,
    num_steps=300
)
```

### 5. Batch Processing and Performance

#### BatchProcessor Class

```python
from enhanced_pynlm import BatchProcessor

# Process multiple simulations in parallel
configs = [
    NLMSimulationConfig(brain_neuron_count=500),
    NLMSimulationConfig(brain_neuron_count=1000),
    NLMSimulationConfig(brain_neuron_count=2000)
]

results = BatchProcessor.process_simulations_in_parallel(
    sim_configs=configs,
    num_steps=500,
    num_workers=3
)
```

#### Performance Monitoring

```python
from enhanced_pynlm import PerformanceMonitor, get_performance_monitor

performance_monitor = get_performance_monitor()

# Record performance metrics
performance_monitor.record_metrics("simulation_1", {
    'processing_time': 0.1,
    'firing_rate': 12.5,
    'memory_usage': 50.0
})

# Get performance comparison
comparison = performance_monitor.get_comparison_report(["simulation_1", "simulation_2"])
print(f"Performance comparison: {comparison}")
```

### 6. Advanced Visualization

#### NLMVisualizer Class

```python
from enhanced_pynlm import create_brain_network_plot, create_firing_pattern_plot, create_time_series_dashboard

# Create network visualization
brain = create_advanced_brain(config)
network_plot = create_brain_network_plot(brain, "network_visualization.png")

# Create firing pattern plot
firing_plot = create_firing_pattern_plot(brain, 1000, "firing_patterns.png")

# Create time series dashboard
dashboard = create_time_series_dashboard(brain, 500, "dashboard.png")

# Create performance visualization
performance_viz = create_performance_visualization(brain, 200, "performance.png")
```

#### Visualization Functions

Available visualization functions:

- `create_brain_network_plot()` - Network graph of brain regions and neurons
- `create_firing_pattern_plot()` - Firing rate over time
- `create_development_animation()` - Animation showing brain development
- `create_time_series_dashboard()` - Comprehensive dashboard with multiple metrics
- `create_performance_visualization()` - Performance analysis and statistics
- `create_correlation_matrix()` - Correlation between different metrics

### 7. Configuration Management

#### ConfigManager Class

```python
from enhanced_pynlm import get_config_manager, save_config_to_name, load_config_from_name

config_manager = get_config_manager()

# Save configuration
save_config_to_name(standard_config, "my_configuration")

# Load configuration
loaded_config = load_config_from_name("my_configuration")

# List all saved configurations
configs = config_manager.list_configs()

# Get configuration suggestions
suggestions = config_manager.get_config_suggestions(standard_config)
```

## Migration Guide

### From Basic pynlm to Enhanced API

The enhanced API maintains full backward compatibility. To migrate:

1. **Import the enhanced module**
   ```python
   import enhanced_pynlm as epynlm
   ```

2. **Use existing functions with enhanced features**
   ```python
   # Original pynlm usage still works
   import pynlm
   
   # Enhanced API for advanced features
   from enhanced_pynlm import (
       create_advanced_brain,
       validate_config,
       create_experiment,
       BatchProcessor,
       get_visualizer
   )
   ```

3. **Add enhanced features as needed**
   ```python
   # For advanced usage
   brain = create_advanced_brain(config)
   results = brain.run_batch_simulation(1000)
   ```

### Upgrading from pynlm 0.1.0

No breaking changes were introduced. All existing code will continue to work without modification.

## Example Applications

### 1. Standard Neural Simulation

```python
import enhanced_pynlm as epynlm

# Basic simulation
config = epynlm.createDefaultConfig()
brain = epynlm.createBrain(config)
brain.initialize()

for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: Firing rate = {brain.getAverageFiringRate():.2f}")
```

### 2. Advanced Research Simulation

```python
import enhanced_pynlm as epynlm
import numpy as np

# Create research configuration
config = epynlm.NLMSimulationConfig(
    brain_neuron_count=5000,
    brain_synapse_density=0.3,
    brain_initial_weight_mean=0.6,
    brain_initial_weight_std=0.15,
    learning_stdp_enable=True,
    learning_hebbian_enable=True,
    learning_structural_enable=True,
    neuromodulation_curiosity_enable=True,
    neuromodulation_novelty_enable=True
)

# Validate configuration
errors = epynlm.validate_config(config)
if errors:
    raise ValueError(f"Invalid configuration: {errors}")

# Create advanced brain
brain = epynlm.create_advanced_brain(config)

# Run comparative experiment
experiment = epynlm.create_experiment(
    name="research_simulation",
    config=config,
    num_steps=2000,
    experiment_type="comparative"
)

# Visualize results
epynlm.create_time_series_dashboard(brain, 1000, "research_dashboard.png")
```

### 3. Batch Processing for Parameter Sweeps

```python
import enhanced_pynlm as epynlm

# Set up parameter sweep
base_config = epynlm.NLMSimulationConfig(
    brain_neuron_count=1000,
    brain_synapse_density=0.1
)

parameter_ranges = {
    "brain_neuron_count": [500, 1000, 2000, 5000],
    "brain_synapse_density": [0.05, 0.1, 0.2],
    "brain_v_thresh": [-55.0, -50.0, -45.0]
}

# Run parameter sweep
sweep_results = epynlm.run_parameter_sweep(
    base_config=base_config,
    parameter_ranges=parameter_ranges,
    num_steps=500
)

# Analyze results
best_config = sweep_results['best_configuration']
print(f"Best configuration: {best_config}")
```

### 4. Long-term Development Study

```python
import enhanced_pynlm as epynlm

# Create developmental experiment config
config = epynlm.NLMSimulationConfig(
    brain_neuron_count=2000,
    brain_synapse_density=0.15,
    neuromodulation_curiosity_enable=True,
    neuromodulation_novelty_enable=True
)

# Run developmental experiment
development_experiment = epynlm.create_experiment(
    name="developmental_study",
    config=config,
    num_steps=5000,
    experiment_type="developmental"
)

# Analyze developmental stages
developmental_data = development_experiment['results']['developmental_data']
final_stage = development_experiment['results']['final_stage']
stage_transitions = development_experiment['results']['stage_transitions']

print(f"Final developmental stage: {final_stage}")
print(f"Stage transitions: {stage_transitions}")

# Visualize development
epynlm.create_development_animation(
    brain=epynlm.create_advanced_brain(config),
    steps=500,
    fps=10,
    output_file="development_animation.gif"
)
```

## API Reference Summary

### Core Classes

- **NLMSimulationConfig**: Enhanced configuration class
- **AdvancedBrain**: Enhanced brain interface with batch processing
- **NLMSimulationManager**: Simulation management and batch operations
- **NLMExperimentManager**: Comprehensive experiment management
- **NLMVisualizer**: Advanced visualization tools
- **ConfigManager**: Configuration management utilities
- **PerformanceMonitor**: Performance monitoring and comparison
- **ErrorHandler**: Comprehensive error handling

### Factory Functions

- **create_advanced_brain()**: Create brain with enhanced features
- **create_experiment()**: Create and run experiments
- **run_parameter_sweep()**: Run parameter sweep experiments
- **get_visualizer()**: Get visualizer instance
- **get_config_manager()**: Get config manager instance
- **get_experiment_manager()**: Get experiment manager instance
- **get_performance_monitor()**: Get performance monitor instance

### Visualization Functions

- **create_brain_network_plot()**: Brain network visualization
- **create_firing_pattern_plot()**: Firing pattern over time
- **create_development_animation()**: Development animation
- **create_time_series_dashboard()**: Comprehensive dashboard
- **create_performance_visualization()**: Performance analysis
- **create_correlation_matrix()**: Correlation between metrics

### Configuration Functions

- **validate_config()**: Validate configuration
- **save_config_to_file()**: Save configuration to file
- **load_config_from_file()**: Load configuration from file

## Installation and Setup

### Prerequisites

```bash
pip install matplotlib numpy seaborn networkx scipy
```

### Installation

```bash
# Install the enhanced pynlm module
pip install -e .  # From the project root

# Or install specific components
pip install enhanced_pynlm
```

### Usage Examples

See the `example_applications.md` file for complete usage examples and tutorials.

## Acknowledgments

This enhanced API builds upon the original NLM framework and extends it with advanced functionality for:

- Research simulation and parameter optimization
- Advanced visualization and analytics
- Comprehensive experiment management
- Robust error handling and validation
- Performance optimization and batch processing

All enhancements maintain backward compatibility with the original pynlm API while providing powerful new capabilities for advanced users and researchers.
