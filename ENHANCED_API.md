# Enhanced Python API for NLM (Neural Learning Machine)

This document provides comprehensive documentation for the enhanced Python API
that provides access to the NLM (Neural Learning Machine) neural simulation framework.

## Overview

The enhanced Python API provides a user-friendly interface to the NLM framework,
combining the power of C++ neural simulation with the convenience of Python.
It includes advanced features such as:

- Enhanced configuration management with validation
- Comprehensive performance monitoring
- Batch experiment execution
- Configuration templates
- Real-time progress tracking
- Data analysis and visualization tools

## Installation

### Prerequisites

```bash
python3 >= 3.8
numpy >= 1.20 (optional, for advanced features)
```

### Installation Methods

#### Method 1: From PyPI (if available)

```bash
pip install pynlm
```

#### Method 2: From Source

```bash
# Clone the repository
https://github.com/nlm-project/nlm.git

cd nlm

# Install with pip (recommended)
pip install -e .

# Or install dependencies manually
pip install numpy
```

### Building from Source

The NLM Python bindings are built using scikit-build-core, which combines
C++ compilation with Python packaging.

```bash
# Ensure you have the C++ development dependencies
# For Ubuntu/Debian:
sudo apt-get update
sudo apt-get install build-essential cmake python3-dev

# For macOS:
# Install Xcode Command Line Tools
# Install Homebrew if not present
brew install cmake python3

# Install Python build dependencies
pip install scikit-build-core pybind11 numpy

# Build and install
cd /path/to/nlm
pip install .
```

## Quick Start

### Basic Usage

```python
from pynlm import create_default_nlm, run_multiple_experiments

# Create NLM with default configuration
nlm = create_default_nlm()

# Run a simple experiment
result = nlm.run_experiment("simple_test", steps=1000)

print(f"Experiment completed: {result['success']}")
print(f"Performance: {result['task_performance']:.3f}")
```

### Using Configuration Templates

```python
from pynlm.utils import get_config_template, create_default_nlm

# Get predefined configuration templates
templates = ['small', 'medium', 'large', 'research', 'production']

for template_name in templates:
    config = get_config_template(template_name)
    print(f"Template: {template_name}")
    print(f"  Neurons: {config['neuron_count']:,}")
    print(f"  Regions: {config['region_count']}")
    
    # Create NLM with template
    nlm = create_default_nlm(config)
```

### Batch Experiment Execution

```python
from pynlm.utils import run_multiple_experiments, save_results, get_performance_summary

# Define multiple experiments
experiments = [
    {'name': 'small_scale', 'steps': 1000, 'neurons': 100},
    {'name': 'medium_scale', 'steps': 2000, 'neurons': 1000},
    {'name': 'large_scale', 'steps': 5000, 'neurons': 10000},
]

# Progress callback
def progress(current, total, name):
    print(f"  Processing: {current+1}/{total} - {name}")

# Run batch experiments
results = run_multiple_experiments(experiments, on_progress=progress)

# Get performance summary
summary = get_performance_summary(results)
print(f"Average performance: {summary['average_performance']:.3f}")

# Save results
save_results(results, 'experiment_results.json')
```

### Configuration Validation

```python
from pynlm.utils import validate_config

# Valid configuration
valid_config = {
    'neuron_count': 1000,
    'region_count': 2,
    'simulation_timestep': 0.001,
    'connection_probability': 0.1,
}

errors = validate_config(valid_config)
if errors:
    print("Configuration errors:", errors)
else:
    print("Configuration is valid!")
```

### Advanced Features

```python
from pynlm import NLM
from pynlm.utils import save_results

# Create NLM with advanced configuration
advanced_config = {
    'neuron_count': 50000,
    'region_count': 10,
    'enable_SIMD': True,
    'enable_multithreading': True,
    'enable_sparse_connectivity': True,
    'enable_event_driven': True,
    'plasticity_learning_rate': 0.001,
}

nlm = NLM(advanced_config)
result = nlm.run_experiment("advanced_test", steps=10000)

print(f"Advanced experiment results:")
print(f"  Success: {result['success']}")
print(f"  Performance: {result['task_performance']:.3f}")
print(f"  Learning accuracy: {result['learning_accuracy']:.3f}")

# Save detailed results
save_results([result], 'advanced_results.json')
```

## API Reference

### Core Classes

#### NLM
The main NLM class for neural simulation experiments.

**Methods:**

- `__init__(config=None)`: Initialize NLM with configuration
- `create_brain()`: Create brain with current configuration
- `run_experiment(name, steps, neurons=None, create_brain_func=None, run_simulation_func=None, collect_metrics_func=None)`: Run experiment
- `run_batch(experiments, on_progress=None)`: Run multiple experiments
- `get_experiment_summary()`: Get summary of all experiments
- `save_results(filepath)`: Save all experiment results
- `load_results(filepath)`: Load experiment results

**Parameters:**

- `config`: Configuration dictionary (optional)
- `name`: Experiment name
- `steps`: Number of simulation steps
- `neurons`: Number of neurons (overrides config if provided)
- `on_progress`: Progress callback function

**Attributes:**

- `config`: Configuration object
- `performance_monitor`: Performance monitoring object
- `experiment_results`: List of experiment results

#### NMConfiguration
Enhanced configuration class with validation.

**Methods:**

- `__init__(**kwargs)`: Initialize configuration
- `to_dict()`: Convert configuration to dictionary
- `from_dict(dict)`: Create configuration from dictionary
- `load_from_file(filepath)`: Load configuration from file
- `save_to_file(filepath)`: Save configuration to file
- `get_default()`: Get default configuration
- `get_templates()`: Get predefined configuration templates
- `_validate()`: Validate configuration values

**Template Names:**

- `small`: Small-scale experiments (100 neurons)
- `medium`: Medium-scale experiments (1,000 neurons)
- `large`: Large-scale experiments (10,000 neurons)
- `research`: Research configuration (optimized for research)
- `production`: Production configuration (optimized for deployment)

#### ExperimentResult
Experiment results with comprehensive metrics.

**Attributes:**

- `experiment_name`: Name of the experiment
- `timestamp`: Experiment start time
- `duration`: Experiment duration
- `success`: Experiment success status
- `neurons_per_second`: Neurons processed per second
- `synapses_per_second`: Synapses processed per second
- `spikes_per_second`: Spikes per second
- `real_time_factor`: Real-time factor (simulated_time / wall_clock_time)
- `avg_step_time_ms`: Average step time
- `min_step_time_ms`: Minimum step time
- `max_step_time_ms`: Maximum step time
- `step_time_stddev`: Standard deviation of step times
- `neuron_memory_mb`: Neuron memory usage
- `synapse_memory_mb`: Synapse memory usage
- `event_queue_memory_mb`: Event queue memory usage
- `total_memory_mb`: Total memory usage
- `peak_memory_mb`: Peak memory usage
- `active_neurons`: Average active neurons
- `firing_neurons`: Average firing neurons
- `avg_firing_rate_hz`: Average firing rate
- `spike_density`: Spike density
- `synaptic_weight_mean`: Mean synaptic weight
- `synaptic_weight_stddev`: Standard deviation of synaptic weights
- `excitatory_ratio`: Excitatory/inhibitory ratio
- `connection_density`: Connection density
- `cpu_utilization_percent`: CPU utilization
- `gpu_utilization_percent`: GPU utilization
- `thread_count`: Number of threads
- `task_performance`: Task performance
- `learning_accuracy`: Learning accuracy
- `memory_retention`: Memory retention
- `total_spikes`: Total spikes
- `total_steps`: Total steps
- `final_spike_rate`: Final spike rate
- `final_firing_rate`: Final firing rate
- `metadata`: Additional metadata

**Methods:**

- `to_dict()`: Convert result to dictionary
- `save_to_file(filepath)`: Save result to file
- `load_from_file(filepath)`: Load result from file
- `print_summary()`: Print experiment summary

#### PerformanceMonitor
Real-time performance monitoring.

**Methods:**

- `__init__()`: Initialize performance monitor
- `start()`: Start performance monitoring
- `stop()`: Stop performance monitoring
- `record_step(step_time_ms, spikes, active_neurons)`: Record simulation step
- `get_current_metrics()`: Get current performance metrics
- `get_snapshot()`: Get performance metrics snapshot
- `is_stable(window_size=100, threshold=0.1)`: Check if performance is stable
- `reset()`: Reset performance monitor

### Utility Functions

#### Core Functions

- `create_default_config()`: Create default configuration
- `get_config_template(name)`: Get predefined configuration template
- `validate_config(config)`: Validate configuration dictionary
- `create_default_nlm(config=None)`: Create NLM with default configuration
- `run_multiple_experiments(experiments, config=None, on_progress=None)`: Run multiple experiments
- `get_performance_summary(experiment_results)`: Get performance summary
- `save_results(results, filepath)`: Save experiment results
- `load_results(filepath)`: Load experiment results
- `print_experiment_summary(summary)`: Print experiment summary

### Constants

#### TEMPLATES
Dictionary of predefined configuration templates:

- `'small'`: Small-scale configuration
- `'medium'`: Medium-scale configuration
- `'large'`: Large-scale configuration
- `'research'`: Research configuration
- `'production'`: Production configuration

## Advanced Features

### Performance Monitoring

The enhanced API includes comprehensive performance monitoring capabilities:

- **Real-time monitoring**: Track simulation performance in real-time
- **Step timing**: Measure and analyze step execution times
- **Memory tracking**: Monitor memory usage and allocation
- **Hardware utilization**: Track CPU and GPU usage
- **Stability analysis**: Determine if performance is stable

### Configuration Validation

The configuration system includes validation for:

- **Type validation**: Ensure configuration values are of correct type
- **Range validation**: Check that values are within valid ranges
- **Dependency validation**: Validate configuration dependencies
- **Format validation**: Ensure configuration follows expected format

### Batch Processing

Run multiple experiments with:

- **Progress tracking**: Monitor experiment progress
- **Error handling**: Handle experiment failures gracefully
- **Result aggregation**: Aggregate results across experiments
- **Progress callbacks**: Custom progress tracking

### Data Analysis

Analyze experiment results with:

- **Statistical analysis**: Calculate mean, std, min, max
- **Performance comparison**: Compare different experiment configurations
- **Trend analysis**: Identify performance trends over time
- **Best/worst performer identification**: Find top and bottom performers

## Examples

See the `examples.py` file for comprehensive examples demonstrating all features.

## File Structure

```
workspace/
├── pynlm/                    # Python package root
│   ├── __init__.py          # Package initialization
│   ├── core.py              # Core classes and functionality
│   └── utils.py             # Utility functions
├── examples.py              # Usage examples
├── enhanced_api.py          # Enhanced API implementation
├── pyproject.toml          # Python project configuration
└── README.md               # This documentation
```

## Compatibility

### Python Version

- Python 3.8 and above
- numpy 1.20 and above (optional, for advanced features)

### Dependencies

- scikit-build-core (for building C++ extensions)
- pybind11 (for Python bindings)
- numpy (optional, for numerical operations)

### Platforms

- Linux (x86_64, ARM64)
- macOS (x86_64)
- Windows (x86_64)

## Troubleshooting

### Common Issues

1. **Module not found**

   ```python
   # Error: No module named 'pynlm'
   ```

   **Solution:** Install the package:
   ```bash
   pip install -e .
   ```

2. **Missing dependencies**

   ```python
   # Error: ModuleNotFoundError: No module named 'numpy'
   ```

   **Solution:** Install numpy:
   ```bash
   pip install numpy
   ```

3. **Build errors**

   ```bash
   # Error: Can't compile extension modules
   ```

   **Solution:** Ensure C++ development tools are installed:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install build-essential cmake python3-dev
   
   # macOS
   xcode-select --install
   ```

4. **Import errors**

   ```python
   # Error: ImportError: cannot import name 'NLM' from 'pynlm'
   ```

   **Solution:** Check that the package is properly installed:
   ```python
   import pynlm
   print(dir(pynlm))
   ```

### Support

For issues and support, please:

1. Check the documentation
2. Review the examples
3. Check the issue tracker
4. Submit a bug report if needed

## Future Enhancements

The enhanced Python API is continuously being improved. Planned features include:

- **Visualization tools**: Real-time plotting and visualization
- **Machine learning integration**: Integration with ML frameworks
- **Distributed computing**: Support for distributed experiments
- **Advanced optimization**: Automatic optimization of experiment parameters
- **Cloud integration**: Deployment to cloud platforms

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Acknowledgments

- Thanks to all contributors and maintainers
- Thanks to the scientific community for inspiring research
- Thanks to users for testing and providing feedback

## Contact

For questions, issues, or suggestions, please contact:

- GitHub: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Issues: https://github.com/nlm-project/nlm/issues

---

This documentation is continuously updated to reflect the latest features and improvements.
Please check the repository for the most recent version.
