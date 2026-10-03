# Advanced NLM Python Module Guide

This comprehensive guide provides detailed information about the enhanced NLM Python module, including advanced features, debugging tools, batch processing capabilities, and experiment management.

## Overview

The Advanced NLM module extends the basic Python bindings with:

- **High-level abstractions** for easier usage
- **Advanced configuration management** with validation and templates
- **Comprehensive debugging and monitoring** tools
- **Batch processing and parallel execution** utilities
- **Experiment tracking and comparison** tools
- **Enhanced error handling and reporting**

## Quick Start

### Basic Usage

```python
import pynlm
import pynlm_advanced as advanced

# Create a simple agent
agent = advanced.NLMAgent()
agent.initialize(world_width=20, world_height=20)

# Run a simulation
result = agent.run_simulation(100, dt=0.1)
print(f"Simulation: {result.total_steps} steps, {result.total_spikes} spikes")

# Get brain statistics
stats = agent.get_brain_stats()
print(f"Neurons: {stats['neuron_count']}, Regions: {stats['region_count']}")
```

## 1. High-Level Agent Interface

The `NLMAgent` class provides an intuitive interface for working with NLM brains.

### Features

- **Method chaining** for fluent API
- **Configuration management** through dictionary-based settings
- **Built-in simulation running** with progress callbacks
- **Comprehensive result reporting** through dataclasses
- **Learning management** with reward modulation support

### Key Methods

```python
# Create agent with configuration
agent = NLMAgent({
    'neuron_count': 500,
    'region_count': 3,
    'enable_reward_modulation': True
})

# Initialize with world settings
agent.initialize(world_width=30, world_height=30, 
                vision_width=8, vision_height=8)

# Configure using method chaining
agent.configure({
    'enable_curiosity': True,
    'enable_development': True
}).initialize()

# Run simulation with callback

def step_callback(step_info):
    print(f"Step {step_info['step']}: {step_info['spike_count']} spikes")

result = agent.run_simulation(100, callback=step_callback)
print(f"Final firing rate: {result.firing_rate:.3f}")
```

## 2. Advanced Configuration Management

The `AdvancedConfig` class provides powerful configuration features.

### Features

- **Configuration validation** with type checking
- **Configuration templates** and inheritance
- **Configuration history tracking**
- **File-based configuration management**
- **Validation rules engine**

### Key Features

```python
from pynlm_advanced import AdvancedConfig

# Create advanced config
config = AdvancedConfig({
    'neuron_count': 500,
    'region_count': 2
})

# Load from file
config.load_from_file('my_config.json')

# Validate configuration
try:
    config.set('neuron_count', 1000)
    print("Configuration updated successfully")
except ValueError as e:
    print(f"Invalid configuration: {e}")

# Merge configurations
config2 = AdvancedConfig({'neuron_count': 200})
merged = config.merge(config2, strategy='deep')

# Apply to brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
config.apply_to_brain(brain)

# Get configuration summary
summary = config.get_summary()
print(f"Configuration keys: {summary['config_keys']}")
```

## 3. Comprehensive Debugging Tools

The `AdvancedDebugger` class provides real-time monitoring and debugging capabilities.

### Features

- **Real-time monitoring** with configurable intervals
- **Performance profiling** with trend analysis
- **Error tracking and reporting**
- **Breakpoint system** for conditional debugging
- **Memory usage tracking**
- **Performance issue detection**

### Key Features

```python
from pynlm_advanced import AdvancedDebugger, ErrorType

# Create debugger for a brain
debugger = AdvancedDebugger(brain, log_file='nlm_debug.log')

# Add performance breakpoints
debugger.add_breakpoint(
    condition=lambda sample: sample['performance_metrics']['max_spike_rate'] > 100,
    action=lambda sample: print("WARNING: High spike rate detected!"),
    label="High spike rate alert"
)

# Start monitoring
debugger.start_monitoring(interval=0.1)  # 100ms intervals

# Run simulation while monitoring
for i in range(50):
    brain.step(i)

# Get comprehensive debug information
debug_info = debugger.get_debug_info()
print(f"Samples collected: {debug_info['performance_samples']}")
print(f"Errors encountered: {debug_info['error_count']}")

# Stop monitoring
debugger.stop_monitoring()

# Access error history
errors = debugger.error_handler.get_errors_by_type(ErrorType.PERFORMANCE_ERROR)
for error in errors:
    print(f"Performance error: {error['message']}")
```

## 4. Memory Management Tools

The `MemoryManager` class provides advanced memory management capabilities.

### Features

- **State snapshots** with selective data capture
- **Memory compression** to reduce storage usage
- **Snapshot restoration** with validation
- **Memory usage tracking**
- **Selective snapshot management**

### Key Features

```python
from pynlm_advanced import MemoryManager

# Create memory manager
memory_manager = MemoryManager(brain)

# Take different types of snapshots
basic_snapshot = memory_manager.take_snapshot("basic")
with_weights = memory_manager.take_snapshot("with_weights", include_weights=True)
with_neuron_states = memory_manager.take_snapshot("full", 
                                                 include_weights=True, 
                                                 include_neuron_states=True)

# Restore snapshot
memory_manager.restore_snapshot(1)  # Restore second snapshot

# Get memory usage statistics
usage_stats = memory_manager.get_memory_usage()
print(f"Compression ratio: {usage_stats['compression_ratio']:.2%}")
print(f"Estimated savings: {usage_stats['estimated_memory_savings']:.1f}%")

# Compress snapshots
compressed = memory_manager.compress_snapshots()
print(f"Original: {len(memory_manager.memory_snapshots)}, "
      f"Compressed: {len(compressed)}")
```

## 5. Experiment Management

The `ExperimentRunner` class provides comprehensive experiment management.

### Features

- **Parameter sweep experiments** for systematic testing
- **Multiple experiment types** (standard, sweeps, batches)
- **Progress tracking and callbacks**
- **Result aggregation and comparison**
- **Experiment history tracking**
- **Comprehensive reporting**

### Key Features

```python
from pynlm_advanced import ExperimentRunner

# Create experiment runner
runner = ExperimentRunner(base_config={'neuron_count': 500})

# Define agent factory
def agent_factory(config):
    return NLMAgent(config, headless=True)

# Create standard experiment
runner.create_experiment(
    name="Baseline Experiment",
    config={'region_count': 2},
    agent_factory=agent_factory,
    simulation_steps=100
)

# Create parameter sweep
runner.create_parameter_sweep(
    param_name='neuron_count',
    param_values=[100, 200, 500, 1000],
    simulation_steps=50
)

# Run experiments with progress tracking
def progress_callback(progress, result):
    print(f"Progress: {progress:.1f}%, "
          f"Spikes: {result.get('spikes', 0)}")

results = runner.run_parameter_sweep(progress_callback)

# Compare results
summary = runner.get_experiment_summary()
print(f"Success rate: {summary['success_rate']*100:.1f}%")
print(f"Average duration: {summary['average_duration']:.2f}s")

# Run individual experiments
for exp in runner.experiments:
    result = runner.run_experiment(exp)
    print(f"Completed: {exp['name']}, "
          f"Duration: {result['duration']:.2f}s")
```

## 6. Batch Processing Utilities

The `BatchProcessor` class provides parallel processing capabilities.

### Features

- **Parallel experiment execution**
- **Thread pool management**
- **Progress tracking across batches**
- **Error handling and recovery**
- **Resource management**
- **Scalable batch processing**

### Key Features

```python
from pynlm_advanced import BatchProcessor

# Create batch processor with 4 workers
processor = BatchProcessor(max_workers=4)

# Create multiple experiments
experiments = []
for i in range(10):
    experiments.append({
        'name': f"Batch Exp {i+1}",
        'config': {'neuron_count': 100 + i*50},
        'agent_factory': agent_factory,
        'simulation_steps': 30
    })

# Run batch with progress tracking
results = processor.run_batch_experiments(
    experiments,
    progress_callback=lambda p, info: print(f"Batch progress: {p:.1f}%")
)

# Get batch status
status = processor.get_batch_status()
print(f"Running: {status['running_jobs']}, "
      f"Completed: {status['completed_jobs']}")

# Shutdown processor
processor.shutdown()
```

## 7. Error Handling and Reporting

The error handling system provides comprehensive error tracking.

### Key Features

```python
from pynlm_advanced import NLMErrors, ErrorType

# Access global error handler
errors = pynlm_advanced.get_errors()

# Log different types of errors
errors.add_error(
    ErrorType.VALIDATION_ERROR,
    "Invalid neuron count: must be between 1 and 10000",
    {'value': 0, 'min': 1, 'max': 10000},
    recoverable=False
)

errors.add_error(
    ErrorType.PERFORMANCE_ERROR,
    "High spike rate variability detected",
    {'avg_rate': 50.0, 'std_rate': 150.0},
    recoverable=True
)

# Get error summary
summary = errors.get_summary()
print(f"Total errors: {summary['total_errors']}")
print(f"Error types: {summary['error_types']}")
print(f"Recoverable errors: {summary['recoverable_count']}")

# Get errors by type
performance_errors = errors.get_errors_by_type(ErrorType.PERFORMANCE_ERROR)
for error in performance_errors:
    print(f"Performance issue: {error['message']}")
```

## 8. Configuration Templates

The configuration system supports templates and inheritance.

### Key Features

```python
# Create configuration templates
base_config = AdvancedConfig({
    'neuron_count': 500,
    'region_count': 2,
    'enable_reward_modulation': True,
    'enable_curiosity': True
})

# Create specialized configurations
training_config = base_config.merge({
    'neuron_count': 1000,
    'plasticity_learning_rate': 0.05
}, strategy='override')

debug_config = base_config.merge({
    'neuron_count': 100,
    'enable_structural_plasticity': True
}, strategy='deep')

# Validate each configuration
try:
    training_config.set('neuron_count', 2000)  # Should fail
except ValueError as e:
    print(f"Validation error: {e}")
```

## 9. Integration with Existing API

The advanced module integrates seamlessly with the existing NLM API.

### Key Features

```python
# Use existing pynlm functionality
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Use advanced features
agent = NLMAgent({'neuron_count': 200})
agent.initialize()

# Access underlying components
config = brain.getConfig()
debugger = AdvancedDebugger(brain)
memory_manager = MemoryManager(brain)
```

## 10. Performance Monitoring

The system provides comprehensive performance monitoring.

### Key Features

```python
# Create monitoring system
debugger = AdvancedDebugger(brain)

# Configure monitoring intervals
debugger.start_monitoring(interval=0.5)  # Monitor every 500ms

# Run long-running simulation
for step in range(1000):
    brain.step(step)
    time.sleep(0.001)

# Analyze performance
performance_data = debugger.get_debug_info()['performance_summary']
print(f"Average spike rate: {performance_data.get('avg_firing_rate', 0):.3f}")
print(f"Spike rate variability: {performance_data.get('std_firing_rate', 0):.3f}")
print(f"Performance trend: {performance_data.get('trend', 'stable')}")

# Stop monitoring
debugger.stop_monitoring()
```

## 11. Example: Complete Workflow

This example demonstrates a complete workflow using all advanced features.

```python
import pynlm
import pynlm_advanced as advanced

# Global error handler
errors = advanced.get_errors()

# Initialize experiment runner
runner = advanced.ExperimentRunner(base_config={
    'neuron_count': 500,
    'enable_reward_modulation': True,
    'enable_curiosity': True
})

# Define agent factory
def create_agent(config):
    return advanced.NLMAgent(config, headless=True)

# Create multiple experiments
experiments = [
    {
        'name': 'Baseline - Small Brain',
        'config': {'neuron_count': 200, 'region_count': 1},
        'agent_factory': create_agent,
        'simulation_steps': 50
    },
    {
        'name': 'Medium Brain with Learning',
        'config': {'neuron_count': 500, 'region_count': 2},
        'agent_factory': create_agent,
        'simulation_steps': 100
    },
    {
        'name': 'Large Brain with Full Learning',
        'config': {'neuron_count': 1000, 'region_count': 3},
        'agent_factory': create_agent,
        'simulation_steps': 150
    }
]

# Create parameter sweep
runner.create_parameter_sweep(
    param_name='plasticity_learning_rate',
    param_values=[0.01, 0.05, 0.1],
    simulation_steps=75
)

# Run all experiments with comprehensive monitoring
def progress_tracker(progress, result):
    if progress % 25 == 0:
        print(f"Progress: {progress:.1f}%")

print("Starting comprehensive experiment suite...")
all_results = []

# Run standard experiments
for exp in experiments:
    result = runner.run_experiment(exp, progress_callback=progress_tracker)
    all_results.append(result)

# Run parameter sweep
sweep_results = runner.run_parameter_sweep(progress_callback=progress_tracker)
all_results.extend(sweep_results)

# Analyze results
summary = runner.get_experiment_summary()
print(f"\nExperiment Summary:")
print(f"  Total experiments: {summary['total_experiments']}")
print(f"  Completed: {summary['completed_experiments']}")
print(f"  Success rate: {summary['success_rate']*100:.1f}%")
print(f"  Average duration: {summary['average_duration']:.2f}s")
print(f"  Average spikes: {summary['average_spikes']:.1f}")
print(f"  Average firing rate: {summary['average_firing_rate']:.3f}")

# Show errors if any
if errors.error_count > 0:
    print(f"\nErrors encountered: {errors.error_count}")
    for error in errors.get_recoverable_errors():
        print(f"  - {error['message']}")

# Save experiment results
import json
with open('experiment_results.json', 'w') as f:
    json.dump({
        'summary': summary,
        'results': all_results
    }, f, indent=2)

print(f"\nResults saved to 'experiment_results.json'")
```

## 12. Installation and Setup

### Prerequisites

```bash
pip install pynlm pynlm-advanced numpy
```

### Quick Installation

```bash
# Install from local directory
pip install -e .

# Or install specific module
pip install pynlm_advanced
```

### Development Installation

```bash
# Install in development mode
pip install -e .[dev]

# Install with all optional dependencies
pip install -e .[all]
```

## 13. Configuration File Examples

### JSON Configuration File

```json
{
    "neuron_count": 1000,
    "region_count": 3,
    "connection_probability": 0.15,
    "plasticity_learning_rate": 0.02,
    "enable_reward_modulation": true,
    "enable_curiosity": true,
    "enable_development": true,
    "enable_structural_plasticity": false,
    "simulation_timestep": 0.001,
    "max_simulation_steps": 10000
}
```

### YAML Configuration File

```yaml
neuron_count: 500
region_count: 2
connection_probability: 0.1
plasticity_learning_rate: 0.01
enable_reward_modulation: true
enable_curiosity: true
enable_development: true
enable_structural_plasticity: false
simulation_timestep: 0.001
max_simulation_steps: 10000
```

## 14. Troubleshooting

### Common Issues

#### 1. "ModuleNotFoundError: No module named 'pynlm_advanced'"

**Solution:** Install the advanced module:

```bash
pip install pynlm_advanced
```

#### 2. "AttributeError: 'Brain' object has no attribute 'set'"

**Solution:** Use the advanced configuration system:

```python
from pynlm_advanced import AdvancedConfig

config = AdvancedConfig({'neuron_count': 1000})
brain = pynlm.createBrain(pynlm.createDefaultConfig())
config.apply_to_brain(brain)
```

#### 3. Memory Usage Too High

**Solution:** Use memory compression:

```python
memory_manager = MemoryManager(brain)
memory_manager.compression_ratio = 0.5  # Increase compression
```

#### 4. Performance Too Slow

**Solution:** Use headless mode and optimize:

```python
agent = NLMAgent(config, headless=True)
```

### Debugging Tips

1. **Enable detailed logging:** Set `log_file` parameter in `AdvancedDebugger`
2. **Monitor performance:** Use `start_monitoring()` with short intervals
3. **Check errors:** Use `get_errors().get_summary()` for error tracking
4. **Validate configurations:** Always validate before use

## 15. Migration Guide

### From Basic to Advanced API

**Before:**

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)
stats = brain.getTotalNeuronCount()
```

**After:**

```python
import pynlm_advanced as advanced

agent = advanced.NLMAgent()
agent.initialize()
result = agent.run_simulation(1)
stats = agent.get_brain_stats()
```

### Key Changes

1. **Configuration:** Use `AdvancedConfig` for validation and management
2. **Agent Management:** Use `NLMAgent` for high-level interface
3. **Debugging:** Use `AdvancedDebugger` for monitoring and error tracking
4. **Memory Management:** Use `MemoryManager` for state snapshots
5. **Experimentation:** Use `ExperimentRunner` for systematic testing

## 16. Version Information

- **Module:** pynlm_advanced v2.0.0
- **Dependencies:** pynlm, numpy, Python 3.8+
- **License:** MIT
- **Documentation:** This guide

## 17. Acknowledgments

This module builds upon the foundation of the original NLM Python bindings and extends them with advanced functionality developed by the NLM Advanced Development Team.

### Dependencies

- **pynlm:** Core NLM Python bindings
- **numpy:** Numerical operations and array handling
- **threading:** Parallel processing utilities
- **concurrent.futures:** Thread pool management

### Special Thanks

- Core NLM development team for the foundational framework
- Python bindings developers for pybind11 integration
- Community contributors for feedback and testing

## 18. Future Enhancements

### Planned Features

1. **Real-time visualization integration**
2. **Machine learning model integration**
3. **Distributed computing support**
4. **Advanced neural architecture search**
5. **Automated experiment optimization**

### Community Contributions

We welcome contributions to this module! Please submit issues and pull requests to the GitHub repository.

---

*This guide is continuously updated. For the latest information, visit the NLM documentation website.*
