# Enhanced NLM Python Module

This module provides advanced functionality for the NLM (Neural Learning Machine) neural simulation framework. It extends the original pynlm library with enhanced features for research, simulation, and analysis.

## Overview

The Enhanced NLM Python Module adds powerful new capabilities to the existing NLM framework:

- **Advanced Configuration Management**: Preset configurations, parameter sweeps, and export/import capabilities
- **Experiment Management**: Comprehensive experiment tracking, templates, and result analysis
- **Batch Processing**: Parallel simulation execution and performance optimization
- **Enhanced Error Handling**: Robust error handling with detailed diagnostics
- **Advanced Visualization**: Network plots, time series dashboards, and performance analytics
- **Comprehensive Validation**: Input validation and error recovery mechanisms

## Features

### 1. Advanced Configuration Management

- **NLMSimulationConfig**: Enhanced configuration class with extensive customization options
- **Configuration Presets**: Built-in presets for common use cases (minimal, standard, advanced, research)
- **Configuration Validation**: Comprehensive validation with detailed error messages
- **Configuration Management**: Save, load, and manage multiple configurations

### 2. Experiment Management

- **NLMExperimentManager**: Comprehensive experiment tracking and management
- **Experiment Templates**: Reusable experiment templates for consistent studies
- **Parameter Sweeps**: Automated parameter optimization and exploration
- **Comparative Experiments**: Compare different configurations systematically
- **Developmental Studies**: Track brain development over extended periods
- **Longitudinal Studies**: Analyze multiple independent simulations over time

### 3. Batch Processing and Performance

- **AdvancedBrain**: Enhanced brain interface with batch processing capabilities
- **BatchProcessor**: Process multiple simulations in parallel
- **Performance Monitoring**: Track and compare simulation performance
- **Performance Metrics**: Detailed statistics and analysis
- **Throughput Optimization**: Enhanced simulation performance

### 4. Advanced Visualization

- **NLMVisualizer**: Comprehensive visualization tools
- **Network Plots**: Brain region and neuron connectivity visualization
- **Time Series Dashboards**: Multi-panel visualizations of simulation metrics
- **Development Animations**: Animated visualizations of brain development
- **Performance Analytics**: Performance metrics and statistical analysis
- **Correlation Analysis**: Relationships between different metrics

### 5. Enhanced Error Handling

- **Exception Hierarchy**: Comprehensive exception classes for different error types
- **Error Handler**: Graceful error handling with detailed logging
- **Validation**: Input validation with descriptive error messages
- **Error Recovery**: Robust error recovery mechanisms

## Installation

### Prerequisites

```bash
pip install matplotlib numpy seaborn networkx scipy
```

### Installation

```bash
# Install from source
pip install -e .

# Or install the enhanced module directly
pip install enhanced_pynlm
```

## Usage Examples

### 1. Basic Usage

```python
import enhanced_pynlm as epynlm

# Create default configuration
config = epynlm.createDefaultConfig()

# Create and initialize brain
brain = epynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
```

### 2. Advanced Configuration

```python
# Create advanced configuration
config = epynlm.NLMSimulationConfig(
    brain_neuron_count=5000,
    brain_synapse_density=0.2,
    learning_stdp_enable=True,
    neuromodulation_curiosity_enable=True,
    neuromodulation_novelty_enable=True
)

# Validate configuration
errors = epynlm.validate_config(config)
if errors:
    raise ValueError(f"Invalid configuration: {errors}")
```

### 3. Advanced Brain Simulation

```python
# Create advanced brain with enhanced features
brain = epynlm.create_advanced_brain(config)

# Run batch simulation with data collection
results = brain.run_batch_simulation(1000, step_size=10)

# Get performance metrics
metrics = brain.get_performance_metrics()
```

### 4. Experiment Management

```python
# Create standard experiment
experiment = epynlm.create_experiment(
    name="standard_test",
    config=config,
    num_steps=1000,
    experiment_type="standard"
)

# Run comparative experiment
comparative_results = epynlm.create_experiment(
    name="comparative_test",
    config=config,
    num_steps=500,
    experiment_type="comparative"
)
```

### 5. Batch Processing

```python
# Set up parameter sweep
base_config = epynlm.NLMSimulationConfig(
    brain_neuron_count=1000,
    brain_synapse_density=0.1
)

parameter_ranges = {
    "brain_neuron_count": [500, 1000, 2000],
    "brain_synapse_density": [0.05, 0.1, 0.2]
}

# Run parameter sweep
sweep_results = epynlm.run_parameter_sweep(
    base_config=base_config,
    parameter_ranges=parameter_ranges,
    num_steps=300
)
```

### 6. Visualization

```python
# Create network visualization
network_plot = epynlm.create_brain_network_plot(brain, "network.png")

# Create time series dashboard
dashboard = epynlm.create_time_series_dashboard(brain, 500, "dashboard.png")

# Create development animation
animation = epynlm.create_development_animation(brain, 500, fps=10, output_file="development.gif")
```

## API Reference

For comprehensive API documentation, refer to `enhanced_api_reference.md` in this directory.

## Files Included

- `enhanced_pynlm.py`: Main enhanced Python module
- `enhanced_api_reference.md`: Complete API documentation
- `example_applications.md`: Usage examples and tutorials
- `pyproject.toml`: Project configuration
- `README.md`: Basic usage instructions

## Backward Compatibility

The enhanced module maintains full backward compatibility with the original pynlm library. All existing code will continue to work without modification. New features are available through the enhanced API while the original pynlm functions remain unchanged.

## Contributing

Contributions to the enhanced NLM module are welcome. Please follow the existing code style and add comprehensive documentation for new features.

## License

This enhanced module is licensed under the MIT License, compatible with the original NLM framework.
