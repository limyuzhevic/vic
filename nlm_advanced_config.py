# NLM Advanced Configuration Tool

An enhanced command-line interface and Python library for managing NLM simulations with advanced features for power users.

## Overview

The NLM Advanced Configuration Tool provides comprehensive configuration management, performance monitoring, and experiment tracking capabilities for NLM (Neural Learning Machine) simulations.

## Features

### 🚀 Advanced Configuration Management
- **Smart Configuration Validation**: Automatic validation with helpful error messages
- **Multiple Configuration Sources**: Load from JSON files, command line, or defaults
- **Configuration Templates**: Pre-defined configurations for different use cases
- **Configuration Optimization**: Automatic optimization based on system resources

### 📊 Performance Monitoring
- **Real-time Metrics**: Track simulation performance, memory usage, and execution times
- **Detailed Statistics**: Step-by-step profiling with statistical analysis
- **Performance Comparison**: Compare results across different configurations
- **Resource Usage Monitoring**: Track CPU, memory, and I/O usage

### 🧪 Advanced Experiment Management
- **Experiment Tracking**: Save and load complete experiment configurations and results
- **Batch Experiments**: Run multiple experiments with parameter variations
- **Experiment Comparison**: Compare results across different experimental conditions
- **Reproducible Research**: Full experiment reproducibility with timestamped results

### ⚡ Enhanced Python API
- **Pythonic Interface**: Intuitive Python API for configuration and control
- **Performance Profiling**: Built-in performance monitoring and analysis
- **Data Export**: Export results in multiple formats (JSON, CSV, etc.)
- **Integration Ready**: Easy integration with existing Python data science tools

### 🛠️ Command-Line Interface
- **Unified CLI**: Single command-line interface for all operations
- **Tab Completion**: Built-in shell tab completion for easier usage
- **Help System**: Comprehensive help with examples and usage tips
- **Logging**: Detailed logging with configurable levels

## Installation

### Prerequisites
```bash
python3 >= 3.8
numpy >= 1.20
psutil >= 5.8.0  (for system monitoring)
```

### Installation
```bash
pip install pynlm[nlm-advanced]

# Or install from local source
cd /path/to/nlm-advanced
cpip install -e .
```

## Quick Start

### Basic Configuration
```bash
# Start interactive configuration
nlm-advanced config

# Create brain with custom configuration
nlm-advanced config --set neuron_count 2000 --set connection_probability 0.2
nlm-advanced create

# Run simulation
nlm-advanced run --steps 5000

# Save experiment
nlm-advanced experiment save my_experiment_1

# Generate performance report
nlm-advanced report
```

### Python API
```python
from nlm_advanced_config import NLMAdvancedManager

# Initialize manager
manager = NLMAdvancedManager()

# Create brain
manager.create_brain()

# Run simulation with monitoring
results = manager.run_simulation(steps=2000)

# Save experiment
manager.save_experiment("my_experiment")

# Generate detailed report
report = manager.generate_report()
print(report)
```

## Configuration Management

### Default Configuration
The tool provides sensible defaults optimized for most use cases:

```json
{
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "random_seed": 42,
    "enable_checkpointing": false,
    "enable_development": true,
    "enable_curiosity": true,
    "enable_reward_modulation": true,
    "log_level": "INFO"
}
```

### Configuration Files
Create custom configuration files (`config.json`):

```json
{
    "neuron_count": 5000,
    "connection_probability": 0.15,
    "enable_structural_plasticity": true,
    "enable_reward_modulation": true,
    "log_level": "DEBUG"
}
```

Load configuration:
```bash
nlm-advanced config --file config.json
```

### Configuration Commands

#### Get Configuration
```bash
# Get specific value
nlm-advanced config --get neuron_count

# Get all configuration
nlm-advanced config --get-all
```

#### Set Configuration
```bash
# Set individual values
nlm-advanced config --set neuron_count 2000
nlm-advanced config --set connection_probability 0.2

# Set multiple values
nlm-advanced config --set neuron_count 3000 --set log_level DEBUG
```

#### Save Configuration
```bash
# Save current configuration
nlm-advanced config --save my_config.json

# Save with optimized settings
nlm-advanced optimize
nlm-advanced config --save optimized_config.json
```

## Experiment Management

### Saving Experiments
```bash
# Save current experiment
nlm-advanced experiment save experiment_1

# Save with custom name
nlm-advanced experiment save "my_simulation_2024"
```

### Loading Experiments
```bash
# Load saved experiment
nlm-advanced experiment load my_simulation_2024

# List available experiments
nlm-advanced experiment list
```

### Experiment Comparison
```python
from nlm_advanced_config import NLMAdvancedManager

manager = NLMAdvancedManager()

# Load multiple experiments
manager.experiment_load("experiment_1")
manager.experiment_load("experiment_2")

# Compare results
experiment_1 = manager.experiments[0]
experiment_2 = manager.experiments[1]

print(f"Experiment 1 neurons: {experiment_1['config']['neuron_count']}")
print(f"Experiment 2 neurons: {experiment_2['config']['neuron_count']}")
print(f"Experiment 1 firing rate: {experiment_1['performance']['avg_firing_rate']}")
print(f"Experiment 2 firing rate: {experiment_2['performance']['avg_firing_rate']}")
```

## Performance Monitoring

### Real-time Monitoring
```bash
# Start monitoring before simulation
nlm-advanced monitor --start

# Run simulation (metrics collected automatically)
nlm-advanced run --steps 1000

# Show current statistics
nlm-advanced monitor --stats
```

### Performance Reports
```bash
# Generate detailed performance report
nlm-advanced report --full

# View key performance metrics
nlm-advanced report
```

### Performance Analysis
Python API for detailed performance analysis:

```python
from nlm_advanced_config import NLMAdvancedManager

manager = NLMAdvancedManager()
manager.create_brain()
manager.run_simulation(2000)

# Get performance statistics
stats = manager.monitor.get_performance_stats()
print(f"Average Step Time: {stats['avg_step_time']*1000:.2f}ms")
print(f"Min Step Time: {stats['min_step_time']*1000:.2f}ms")
print(f"Max Step Time: {stats['max_step_time']*1000:.2f}ms")
print(f"Performance Standard Deviation: {stats['step_time_std']*1000:.4f}ms")

# Generate performance report
report = manager.monitor.generate_report()
print(report)
```

## Advanced Configuration Options

### Configuration Optimization
```bash
# Automatically optimize configuration for current system
nlm-advanced optimize

# Optimize with specific resource constraints
nlm-advanced optimize --cpu-limit 4 --memory-limit 4
```

### Performance Tuning
```python
from nlm_advanced_config import NLMAdvancedManager

manager = NLMAdvancedManager()

# Optimize based on system resources
manager.optimize_configuration()

# Get optimized configuration
print(f"Optimized neurons: {manager.config.get('neuron_count')}")
print(f"Optimized connections: {manager.config.get('connection_probability')}")
```

### Batch Processing
```python
from nlm_advanced_config import NLMAdvancedManager

manager = NLMAdvancedManager()

# Run parameter sweep
parameters = {
    "neuron_count": [500, 1000, 2000],
    "connection_probability": [0.05, 0.1, 0.15],
    "enable_structural_plasticity": [True, False]
}

for neurons in parameters["neuron_count"]:
    for connectivity in parameters["connection_probability"]:
        for plasticity in parameters["enable_structural_plasticity"]:
            # Set configuration
            manager.config.set("neuron_count", neurons)
            manager.config.set("connection_probability", connectivity)
            manager.config.set("enable_structural_plasticity", plasticity)
            
            # Create and run brain
            manager.create_brain()
            results = manager.run_simulation(500)
            
            # Save results
            exp_name = f"sweep_n{neurons}_c{connectivity}_p{plasticity}"
            manager.save_experiment(exp_name)
```

## System Integration

### Integration with Existing NLM Code
```python
# Import the enhanced configuration system
from nlm_advanced_config import NLMConfig, PerformanceMonitor

# Create enhanced configuration
config = NLMConfig()
config.set("neuron_count", 2000)
config.set("enable_reward_modulation", True)

# Create performance monitor
monitor = PerformanceMonitor()

# Use with existing NLM code
# brain = NLM.Brain(config)
# brain.initialize()

# Monitor simulation
monitor.start_simulation()
for step in range(1000):
    monitor.start_step()
    # brain.step(step)
    monitor.end_step()
monitor.end_simulation()

# Get results
stats = monitor.get_performance_stats()
print(f"Simulation completed in {stats['wall_clock_time']:.2f}s")
print(f"Average step time: {stats['avg_step_time']*1000:.2f}ms")
```

### Web Dashboard Integration
```python
# For web-based monitoring and visualization
from nlm_advanced_config import NLMAdvancedManager
import json

manager = NLMAdvancedManager()

# Run simulation
manager.create_brain()
manager.run_simulation(1000)

# Export data for web dashboard
experiment_data = {
    "config": manager.config.get_summary(),
    "performance": manager.monitor.get_performance_stats(),
    "timestamp": time.time()
}

# Save for web dashboard
with open("dashboard_data.json", "w") as f:
    json.dump(experiment_data, f, indent=2)

print("Data saved for web dashboard")
```

## Advanced Usage Examples

### 1. Distributed Simulation Setup
```python
from nlm_advanced_config import NLMAdvancedManager
import multiprocessing

# Setup distributed simulation
manager = NLMAdvancedManager()

# Configure for distributed execution
manager.config.set("neuron_count", 10000)
manager.config.set("enable_structural_plasticity", True)

# Create multiple brains for different workers
brains = []
for i in range(multiprocessing.cpu_count()):
    brain_manager = NLMAdvancedManager()
    brain_manager.config = manager.config.copy()
    brain_manager.config.set("random_seed", 42 + i)
    brain_manager.create_brain()
    brains.append(brain_manager)

# Run distributed simulation
results = []
for i, brain_manager in enumerate(brains):
    print(f"Worker {i} running simulation...")
    result = brain_manager.run_simulation(500)
    results.append(result)

# Combine results
print(f"Total simulations: {len(results)}")
print(f"Average performance: {sum(r.get('avg_firing_rate', 0) for r in results) / len(results)}")
```

### 2. Machine Learning Integration
```python
from nlm_advanced_config import NLMAdvancedManager
import numpy as np
from sklearn.model_selection import ParameterGrid

# Hyperparameter optimization for NLM
manager = NLMAdvancedManager()

# Define hyperparameter grid
param_grid = ParameterGrid({
    'neuron_count': [500, 1000, 2000],
    'connection_probability': [0.05, 0.1, 0.15],
    'enable_reward_modulation': [True, False],
    'enable_structural_plasticity': [True, False]
})

# Run hyperparameter optimization
results = []
for params in param_grid:
    # Set parameters
    for key, value in params.items():
        manager.config.set(key, value)
    
    # Create and run simulation
    manager.create_brain()
    simulation_results = manager.run_simulation(200)
    
    # Record results
    experiment = {
        'params': params,
        'results': simulation_results,
        'performance': manager.monitor.get_performance_stats()
    }
    results.append(experiment)
    
    # Save experiment
    exp_name = f"hpo_{len(results)}"
    manager.save_experiment(exp_name)

# Find best parameters
best_result = max(results, key=lambda x: x['performance'].get('avg_firing_rate', 0))
print(f"Best parameters: {best_result['params']}")
print(f"Best performance: {best_result['performance'].get('avg_firing_rate', 0)}")
```

### 3. Automated Experiment Pipeline
```python
from nlm_advanced_config import NLMAdvancedManager
import glob
import json

# Automated experiment pipeline
manager = NLMAdvancedManager()

# Load existing configurations
config_files = glob.glob("configs/*.json")
for config_file in config_files:
    # Create experiment for each configuration
    exp_name = Path(config_file).stem
    
    # Load configuration
    manager.config = NLMConfig(config_file)
    
    # Create and run simulation
    manager.create_brain()
    results = manager.run_simulation(manager.config.get("max_steps", 1000))
    
    # Save experiment
    manager.save_experiment(f"{exp_name}_v1")
    
    print(f"Completed experiment: {exp_name}")

# Generate summary report
print("\n" + "="*50)
print("EXPERIMENT PIPELINE SUMMARY")
print("="*50)

for experiment in manager.experiments:
    print(f"\nExperiment: {experiment['name']}")
    print(f"  Neurons: {experiment['config']['neuron_count']}")
    print(f"  Connectivity: {experiment['config']['connection_probability']}")
    print(f"  Duration: {experiment['performance']['wall_clock_time']:.2f}s")
    print(f"  Avg Firing Rate: {experiment['performance'].get('avg_firing_rate', 0):.4f}")
```

## API Reference

### NLMConfig Class

#### Methods
- `__init__(config_file=None)`: Initialize configuration
- `load_from_file(filepath)`: Load configuration from JSON file
- `validate_config()`: Validate configuration values
- `get(key, default)`: Get configuration value
- `set(key, value)`: Set configuration value
- `save_to_file(filepath)`: Save configuration to JSON file
- `reset_to_defaults()`: Reset to default configuration
- `get_summary()`: Get configuration summary

#### Properties
- `config`: Current configuration dictionary
- `DEFAULT_CONFIG`: Default configuration template

### PerformanceMonitor Class

#### Methods
- `__init__()`: Initialize performance monitor
- `start_simulation()`: Start monitoring simulation
- `end_simulation()`: End simulation monitoring
- `start_step()`: Start monitoring step
- `end_step(spikes, neurons_created, synapses_created)`: End step monitoring
- `update_memory_usage(memory_mb)`: Update memory usage
- `get_performance_stats()`: Get performance statistics
- `generate_report()`: Generate performance report

#### Metrics Tracked
- `simulations`: Number of simulations run
- `total_steps`: Total simulation steps
- `total_spikes`: Total spikes generated
- `avg_firing_rate`: Average firing rate
- `wall_clock_time`: Total wall clock time
- `step_times`: List of step execution times
- `neuron_creation_times`: Neuron creation times
- `synapse_creation_times`: Synapse creation times

### NLMAdvancedManager Class

#### Methods
- `__init__()`: Initialize advanced manager
- `create_brain(config_file=None)`: Create and initialize brain
- `run_simulation(steps)`: Run simulation with monitoring
- `save_experiment(name)`: Save current experiment
- `load_experiment(name)`: Load saved experiment
- `list_experiments()`: List saved experiments
- `optimize_configuration()`: Optimize configuration for performance
- `generate_report()`: Generate comprehensive system report

## Troubleshooting

### Common Issues

#### Configuration Errors
```bash
# Error: Configuration file not found
nlm-advanced config --file non_existent.json

# Solution: Create the configuration file or omit --file option
nlm-advanced config
```

#### Missing Dependencies
```bash
# Error: psutil module not found
python3 -c "import psutil"

# Solution: Install missing dependencies
pip install psutil
```

#### Permission Errors
```bash
# Error: Permission denied when saving configuration
nlm-advanced config --save /root/config.json

# Solution: Run with appropriate permissions or save to user directory
nlm-advanced config --save ./config.json
```

### Debugging Tips

#### Enable Debug Logging
```bash
# Enable debug logging
nlm-advanced config --set log_level DEBUG

# Run simulation with debug output
nlm-advanced create
nlm-advanced run --steps 100
```

#### Check System Resources
```python
import psutil

# Check available system resources
cpu_count = psutil.cpu_count()
memory_gb = psutil.virtual_memory().total / (1024**3)

print(f"CPU Cores: {cpu_count}")
print(f"Memory (GB): {memory_gb}")
```

#### Performance Bottlenecks
```python
# Check for performance bottlenecks
from nlm_advanced_config import NLMAdvancedManager

manager = NLMAdvancedManager()
manager.create_brain()
manager.run_simulation(500)

# Analyze performance statistics
stats = manager.monitor.get_performance_stats()

if 'avg_step_time' in stats:
    print(f"Average step time: {stats['avg_step_time']*1000:.2f}ms")
    
    # Check for slow steps
    if stats['max_step_time'] > stats['avg_step_time'] * 2:
        print("Warning: High variance in step times detected")
```

## License

This software is licensed under the MIT License. See the LICENSE file for more details.

## Support

For support and issues, please visit the NLM repository or create an issue on GitHub.

## Acknowledgments

- Based on the NLM (熙然) neural learning machine framework
- Inspired by biological brain architectures
- Built with Python best practices and modern software engineering principles