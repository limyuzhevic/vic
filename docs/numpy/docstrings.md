# NLM Python API - Enhanced Documentation

# Overview

The NLM (Neural Learning Machine) Python API provides advanced bindings for the Neural Learning Machine framework, enabling researchers and developers to create, simulate, and analyze neural systems using Python.

## Key Features

- Advanced configuration management with property access
- Performance monitoring and profiling tools
- Brain analysis and visualization capabilities
- Simulation checkpoint/restore with progress tracking
- Plugin integration for Python plugins
- Advanced learning algorithms with hyperparameter tuning
- Brain-to-brain comparison and transfer learning

# Installation

```bash
pip install nlm
# Or install from source
pip install -e .
```

# Quick Start

## Basic Usage

```python
import pynlm

# Create a brain with configuration
config = pynlm.Config()
brain = pynlm.Brain(config)
brain.initialize()

# Run simulation
for step in range(100):
    s = pynlm.SimulationStep(step)
    brain.step(s)
```

## Advanced Configuration

```python
from nlm_advanced import AdvancedConfig

# Create advanced config with schema
config = AdvancedConfig()
config.define_schema('learning_rate', 'float', default=0.01, min_value=0.0, max_value=1.0)
config.define_schema('neuron_count', 'int', default=100, min_value=10, max_value=10000)

# Set values
config.set_value('learning_rate', 0.05)
config.set_value('neuron_count', 500)

# Get values
learning_rate = config.get_value('learning_rate')
neuron_count = config.get_value('neuron_count', 100)
```

# Advanced Configuration Management

## Schema Validation

The `AdvancedConfig` class provides comprehensive schema validation:

```python
class AdvancedConfig:
    def define_schema(self, key, type_, default=None, min_value=None, max_value=None, 
                     allowed_values=None, required=True):
        # Define a configuration parameter schema
        pass
    
    def validate_all(self):
        # Validate all configuration values
        return is_valid, errors
    
    def save_to_file(self, filepath, include_schema=True):
        # Save configuration to file
        pass
```

## Environment Configuration

```python
config = AdvancedConfig()
config.load_from_env('NLM_')  # Load from environment variables
config.load_from_file('config.json')  # Load from JSON file
config.load_from_dict({'param': 'value'})  # Load from dictionary
```

# Performance Monitoring

## Profiling Tools

```python
from nlm_profiling import SimulationProfiler, benchmark_brain_performance

# Create profiler
profiler = SimulationProfiler(brain)

# Start profiling
profiler.start_profiling()

# Run simulation
for step in range(100):
    s = pynlm.SimulationStep(step)
    profiler.record_step(step, time.time() - step_start)
    brain.step(s)

# Stop profiling
profiler.stop_profiling()

# Get performance report
report = profiler.generate_report()
```

## Benchmarking

```python
from nlm_profiling import benchmark_brain_performance

# Benchmark configuration
results = benchmark_brain_performance(brain, config_data, steps=100)

# Get step statistics
step_stats = profiler.get_step_statistics()
print(f"Average step time: {step_stats.mean:.4f}s")
print(f"95th percentile: {step_stats.p95:.4f}s")
```

# Brain Analysis

## Structural Analysis

```python
from nlm_analysis import analyze_brain

# Perform comprehensive analysis
analysis_results = analyze_brain(brain)

# Get structure analysis
structure = analysis_results['structure']
print(f"Region count: {structure['region_count']}")
print(f"Total neurons: {structure['total_neurons']}")

# Get functional analysis
functional = analysis_results['functional']
print(f"Average firing rate: {functional['neuron_properties']['average_firing_rate']}")

# Get connectivity analysis
connectivity = analysis_results['connectivity']
print(f"Clustering coefficient: {connectivity['graph_structure']['graph_metrics']['clustering_coefficient']}")
```

## Activity Pattern Analysis

```python
from nlm_analysis import BrainAnalyzer

analyzer = BrainAnalyzer(brain)
activity_patterns = analyzer.analyze_activity_patterns(steps=1000)

# Get pattern statistics
stats = activity_patterns['statistical_analysis']
print(f"Activity variance: {stats['activity_variance']:.4f}")

# Detect activity regimes
regimes = activity_patterns['regime_detection']
print(f"Detected regimes: {len(regimes)}")
```

# Simulation Control

## SimulationRunner

```python
from nlm_advanced import SimulationRunner

# Create simulation runner
runner = SimulationRunner(brain)

# Run episode
episode_results = runner.run_episode(max_steps=1000)

# Create checkpoint
runner.create_checkpoint('episode_1', 'checkpoint.pkl')

# Restore from checkpoint
runner.restore_checkpoint('episode_1')

# Get statistics
stats = runner.get_statistics()
print(f"Total steps: {stats['total_steps']}")
print(f"Average step time: {stats['average_step_time']:.4f}s")
```

## Progress Tracking

```python
def progress_callback(progress, result):
    if progress % 0.1 == 0:  # Every 10%
        print(f"Progress: {progress*100:.1f}%")

runner.set_progress_callback(progress_callback)
```

# Plugin Integration

## Plugin Development

```python
from nlm_plugin import Plugin

class MyPlugin(Plugin):
    def __init__(self):
        super().__init__("my_plugin")
        self.version = "1.0.0"
        self.description = "My custom NLM plugin"
        self.dependencies = ["numpy"]
        self.supported_brain_types = ["neural", "cognitive"]
    
    def _on_enable(self):
        print("Plugin enabled")
    
    def _on_disable(self):
        print("Plugin disabled")
    
    def execute(self, brain, *args, **kwargs):
        # Plugin functionality
        return {"result": "success"}
```

## Plugin Management

```python
from nlm_plugin import PluginManager

# Create plugin manager
plugin_manager = PluginManager()

# Load plugin
plugin_manager.load_plugin('visualization', VisualizationPlugin)

# Enable plugin
plugin_manager.enable_plugin('visualization')

# Activate plugin
plugin_manager.activate_plugin('visualization')

# Execute plugin
results = plugin_manager.execute_plugin('visualization', brain)
```

# Advanced Learning

## Hyperparameter Tuning

```python
from examples.advanced_learning import hyperparameter_tuning

# Configuration template
config_template = {
    'learning_rate': 0.01,
    'neuron_count': 100,
    'plasticity_enabled': True
}

# Parameter grid
param_grid = {
    'learning_rate': [0.001, 0.01, 0.1],
    'plasticity_enabled': [True, False],
    'neuron_count': [50, 100, 200]
}

# Perform tuning
results = hyperparameter_tuning(config_template, param_grid)
print(f"Best configuration: {results['best_config']}")
print(f"Best performance: {results['best_score']:.4f}")
```

# Transfer Learning

## Brain Transfer

```python
from examples.brain_transfer import create_source_brain, create_target_brain, transfer_knowledge

# Create brains
source_brain = create_source_brain()
target_brain = create_target_brain()

# Simulate source to collect knowledge
source_knowledge = simulate_source_brain(source_brain, 200)

# Transfer knowledge
transferred_brain = transfer_knowledge(source_knowledge, target_brain, transfer_ratio=0.7)

# Evaluate transfer
evaluation = evaluate_transfer_learning(source_knowledge, target_results)
print(f"Transfer effectiveness: {evaluation['transfer_effectiveness_score']:.4f}")
```

# Performance Tips

## Optimization Guidelines

1. **Configuration Validation**: Always use `AdvancedConfig` for complex configurations to ensure type safety.

2. **Memory Management**: Use `SimulationRunner` with checkpointing for long-running simulations.

3. **Performance Monitoring**: Enable `SimulationProfiler` to identify bottlenecks early.

4. **Plugin Architecture**: Use plugins for extending functionality without modifying core code.

5. **Hyperparameter Tuning**: Use systematic approaches like grid search or Bayesian optimization for configuration optimization.

## Best Practices

- Always validate configuration before use
- Use checkpoint/restore for long simulations
- Profile performance regularly
- Document plugin interfaces
- Use type hints for better code maintainability
- Implement proper error handling and logging

# API Reference

## Core Classes

### AdvancedConfig
- `__init__(config_file=None)`
- `load_from_file(filepath)`
- `load_from_dict(config_dict)`
- `load_from_env(prefix='NLM_')`
- `define_schema(key, type_, default=None, ...)`
- `set_value(key, value)`
- `get_value(key, default=None, as_type=None)`
- `has(key)`
- `add_validation_callback(callback)`
- `validate_all()`
- `save_to_file(filepath, include_schema=True)`

### PerformanceProfiler
- `__init__(brain=None)`
- `start_profiling()`
- `stop_profiling()`
- `record_step(step_num, step_time, additional_metrics=None)`
- `benchmark_configuration(config_data, steps=100, warmup_steps=10)`
- `compare_performance(profilers, labels)`
- `generate_report()`
- `export_data(filepath, format='json')`

### SimulationRunner
- `__init__(brain)`
- `run_episode(max_steps, timestep=None, time=None)`
- `run_batch(episodes, parallel=True, max_workers=4)`
- `create_checkpoint(name, filepath=None)`
- `restore_checkpoint(name)`
- `save_checkpoint_to_file(name, filepath)`
- `load_checkpoint_from_file(filepath)`
- `set_progress_callback(callback)`
- `get_statistics()`

### BrainAnalyzer
- `__init__(brain)`
- `analyze_structure()`
- `analyze_functional()`
- `analyze_activity_patterns(steps=1000, window_size=100)`
- `analyze_connectivity()`
- `generate_visualization_data(analysis_type='structure', output_format='json', **kwargs)`
- `compare_brains(other_brain)`
- `detect_anomalies()`
- `get_insights()`
- `export_results(filepath, format='json', analysis_types=None)`

### PluginManager
- `__init__(plugin_directories=None)`
- `add_plugin_directory(directory)`
- `discover_plugins()`
- `load_plugin(plugin_name, force=False)`
- `unload_plugin(plugin_name)`
- `enable_plugin(plugin_name)`
- `disable_plugin(plugin_name)`
- `activate_plugin(plugin_name)`
- `deactivate_plugin(plugin_name)`
- `configure_plugin(plugin_name, config)`
- `get_plugin(plugin_name)`
- `list_plugins()`
- `execute_plugin_method(plugin_name, method, *args, **kwargs)`
- `get_plugin_info(plugin_name)`

## Module Functions

### High-level API Functions
- `advanced_learning_algorithms(config)`
- `brain_to_brain_comparison(brain1, brain2)`
- `transfer_learning(source_brain, target_brain, transfer_ratio=0.5)`
- `benchmark_brain_performance(brain, config_data, steps=100, warmup_steps=10)`
- `profile_simulation(brain, steps, warmup_steps=10)`
- `compare_implementations(brain_configs, steps=100)`
- `analyze_brain(brain, analysis_types=None)`
- `generate_visualization_data(brain, analysis_type='structure', output_format='json', **kwargs)`
- `compare_brain_analyses(brain1, brain2)`

# File Structure

```
project_root/
├── python_bindings/
│   ├── pynlm_advanced.py          # Advanced Python API
│   ├── pynlm_profiling.py         # Performance profiling tools
│   ├── pynlm_analysis.py         # Brain analysis tools
│   └── pynlm_plugin.py           # Python plugin integration
├── examples/
│   ├── advanced_learning.py      # Advanced learning example
│   └── brain_transfer.py         # Transfer learning example
├── docs/
│   ├── numpy/
│   │   └── docstrings.md         # Enhanced documentation
│   └── jupyter/
│       └── AdvancedExamples.ipynb # Jupyter notebook examples
└── CMakeLists.txt                # CMake updates for Python bindings
```

# Troubleshooting

## Common Issues

### Module Not Found

If you encounter `ModuleNotFoundError` for `pynlm`:
1. Ensure NLM is properly installed: `pip install nlm`
2. Check that you're in the correct directory
3. Verify that `pynlm` is in your Python path

### Configuration Validation Errors

If configuration validation fails:
1. Check that your configuration file has the correct format
2. Ensure that all required parameters are present
3. Verify that parameter types match the schema definitions

### Performance Issues

If performance is slow:
1. Consider using `SimulationProfiler` to identify bottlenecks
2. Enable checkpointing for long simulations
3. Use parallel processing where applicable
4. Check for memory leaks using memory profiling tools

### Plugin Loading Errors

If plugins fail to load:
1. Ensure all dependencies are installed
2. Check that plugin classes follow the Plugin interface
3. Verify that plugin names are unique
4. Check logging output for specific error messages

# Future Enhancements

## Planned Features

1. **Async Simulation Support**: Add full async simulation capabilities
2. **GPU Acceleration**: Integrate with GPU computing for faster simulations
3. **Distributed Computing**: Support for multi-node simulation runs
4. **Advanced Visualization**: Interactive web-based visualization tools
5. **Machine Learning Integration**: Integration with scikit-learn and TensorFlow/PyTorch
6. **Real-time Monitoring**: Web-based real-time monitoring dashboards
7. **Cloud Integration**: Support for cloud-based simulation resources
8. **Automated Testing**: Comprehensive test suite for NLM APIs

# License

This software is licensed under the MIT License. See LICENSE file for details.