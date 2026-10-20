# NLM Python Bindings - Enhanced API

This document provides information about the enhanced Python bindings for the NLM (Neural Learning Machine) framework.

## Overview

The enhanced Python bindings provide a comprehensive API for NLM neural simulation with:

- **High-level convenience functions** for common tasks
- **Visualization support** for neural data analysis
- **Advanced machine learning features** for model training and optimization
- **Performance monitoring tools** for system analysis
- **Development and debugging utilities** for tool development
- **Improved API usability** with context managers and property access

## Quick Start

### Prerequisites

```bash
# Install required dependencies
pip install numpy scipy matplotlib seaborn scikit-learn networkx psutil

# Install the NLM Python bindings (from source)
pip install -e .
```

### Basic Usage

```python
import pynlm

# Create a brain with preset configuration
brain = pynlm.create_brain_with_preset("phase3")

# Initialize the brain
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
print(f"Total neurons: {brain.total_neurons}")
print(f"Firing neurons: {brain.firing_neurons}")
print(f"Developmental stage: {brain.developmental_stage}")
```

### Advanced Usage

```python
import pynlm

# Create a complete agent simulation
brain = pynlm.create_brain_with_preset("phase6")
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run training
for episode in range(100):
    world.reset()
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
```

## Available Modules

### Core Module (`pynlm`)

- **High-level functions**:
  - `create_brain_with_preset(preset)` - Create brain with predefined configurations
  - `run_learning_experiment(brain, world, duration, learning_rate)` - Run learning experiments
  - `train_agent(brain, world, episodes, max_steps_per_episode, exploration_rate)` - Train agent
  - `benchmark_performance(brain, duration, metrics)` - Run performance benchmarks

- **Visualization functions**:
  - `plot_brain_network(brain)` - Plot neural connectivity
  - `plot_activity_raster(brain)` - Plot spike rasters
  - `plot_weight_matrix(brain)` - Plot synaptic weights
  - `create_video(brain, output_path, duration, fps)` - Create animation

- **Machine learning features**:
  - `train_predictive_model(brain, features, targets, model_type)` - Train prediction models
  - `optimize_weights(brain, method, learning_rate, iterations)` - Optimize synaptic weights
  - `run_reinforcement_learning(brain, world, episodes, gamma, alpha)` - Run RL training
  - `cross_validate(brain, features, targets, cv_folds)` - Cross-validation

- **Performance monitoring**:
  - `monitor_performance(brain, duration, sampling_rate)` - Monitor system performance
  - `measure_compatibility(system1_type, system2_type)` - Measure compatibility
  - `profile_activity(brain, duration, profile_type)` - Profile neural activity

- **Development tools**:
  - `export_for_debugging(brain, filepath)` - Export brain state for debugging
  - `import_from_checkpoint(filepath)` - Import saved brain states
  - `validate_configuration(config_str)` - Validate configuration files
  - `generate_config_template()` - Generate config templates

### High-Level API (`python.high_level_api`)

This module provides a user-friendly interface for common NLM tasks:

```python
from python.high_level_api import BrainSimulator, create_brain_with_preset, run_learning_experiment

# Use BrainSimulator class
simulator = BrainSimulator("phase3")
results = simulator.run_learning_experiment(world, duration=1000)

# Use convenience functions
brain = create_brain_with_preset("phase4")
results = run_learning_experiment(brain, world, duration=500)
```

### Utilities (`python.utils`)

This module provides utility functions for data processing, validation, and management:

```python
from python.utils import (
    DataProcessor, TimeSeriesGenerator, ConfigurationValidator,
    ProgressTracker, CheckpointManager, BatchProcessor,
    get_memory_usage, get_cpu_usage, time_operation, NLMContextManager
)

# Data processing
normalized = DataProcessor.normalize_data(data)
statistics = DataProcessor.compute_statistics(data)

# Configuration validation
is_valid, errors = ConfigurationValidator.validate_config(config_dict)

# Progress tracking
tracker = ProgressTracker(total_steps=1000, update_interval=100)

# Checkpoint management
checkpoint_manager = CheckpointManager("/path/to/checkpoints")
checkpoint_manager.save_checkpoint("brain_state.json", brain_data)
```

## Configuration Presets

The enhanced bindings provide predefined configuration presets:

- **phase2**: Basic neural system with core plasticity
- **phase3**: Full integration with memory and prediction
- **phase4**: Advanced cognition with concept formation
- **phase5**: Performance optimizations and continual learning
- **phase6**: Complete brain integration with visualization

```python
# Available presets
presets = ["phase2", "phase3", "phase4", "phase5", "phase6"]

for preset in presets:
    brain = pynlm.create_brain_with_preset(preset)
    print(f"Created brain with {preset} configuration")
```

## Visualization Examples

### Neural Network Plot

```python
import matplotlib.pyplot as plt

fig = pynlm.plot_brain_network(brain)
plt.show()
fig.savefig("brain_network.png")
```

### Activity Raster Plot

```python
fig = pynlm.plot_activity_raster(brain)
plt.show()
fig.savefig("activity_raster.png")
```

### Weight Matrix

```python
fig = pynlm.plot_weight_matrix(brain)
plt.show()
fig.savefig("weight_matrix.png")
```

### Video Animation

```python
animation = pynlm.create_video(brain, "brain_activity.mp4", duration=10, fps=30)
```

## Machine Learning Examples

### Predictive Model Training

```python
from python.utils import TimeSeriesGenerator

# Generate training data
features = TimeSeriesGenerator.generate_lif_activity(100, 1000, firing_rate=10.0)
targets = [sum(row) for row in features]

# Train model
model_results = pynlm.train_predictive_model(brain, features, targets, "random_forest")
print(f"Model R² score: {model_results['r2_score']}")
```

### Reinforcement Learning

```python
results = pynlm.run_reinforcement_learning(brain, world, episodes=1000, gamma=0.99, alpha=0.01)

print(f"Final reward: {results['final_reward']}")
print(f"Average reward per episode: {sum(results['episode_rewards']) / len(results['episode_rewards'])}")
```

### Cross-Validation

```python
# Prepare data
X = [[1, 2], [3, 4], [5, 6], [7, 8], [9, 10], [11, 12]]
y = [2, 4, 6, 8, 10, 12]

# Cross-validation
cv_results = pynlm.cross_validate(None, X, y, cv_folds=3)
print(f"CV mean score: {cv_results['mean_score']}")
print(f"CV std score: {cv_results['std_score']}")
```

## Performance Monitoring

```python
# Monitor system performance
monitoring_data = pynlm.monitor_performance(brain, duration=1000, sampling_rate=100)

# Get memory and CPU usage
memory_usage = pynlm.get_memory_usage()
cpu_usage = pynlm.get_cpu_usage()

print(f"Memory usage: {memory_usage:.2f} MB")
print(f"CPU usage: {cpu_usage:.2f}%")
```

## Development Tools

### Export for Debugging

```python
# Export brain state for debugging
export_path = pynlm.export_for_debugging(brain, "/tmp/brain_debug.json")
print(f"Brain state exported to: {export_path}")
```

### Import from Checkpoint

```python
# Import saved brain state
brain_state = pynlm.import_from_checkpoint("/path/to/checkpoint.json")
```

### Configuration Validation

```python
# Validate configuration
config_str = '{"brain": {"neuron_count": 1000}, "plasticity": {"stdp": {"enable": true}}}'
validation = pynlm.validate_configuration(config_str)

print(f"Is valid: {validation['is_valid']}")
if not validation['is_valid']:
    print(f"Errors: {validation['errors']}")
```

### Generate Config Template

```python
# Generate default configuration template
template = pynlm.generate_config_template()
print(template)

# Save to file
with open("config_template.json", "w") as f:
    f.write(template)
```

## Property Access

The enhanced bindings provide convenient property access for better data inspection:

```python
# Direct property access (read-only)
print(f"Total neurons: {brain.total_neurons}")
print(f"Total synapses: {brain.total_synapses}")
print(f"Active neurons: {brain.active_neurons}")
print(f"Firing neurons: {brain.firing_neurons}")
print(f"Average firing rate: {brain.average_firing_rate}")
print(f"Developmental stage: {brain.developmental_stage}")

# Configuration access
config = brain.config
print(f"Config keys: {config.getKeys()}")
```

## Context Managers

The enhanced bindings provide context managers for automatic resource cleanup:

```python
# Use context manager for automatic cleanup
with pynlm.BrainCleanupContext(brain) as ctx:
    # Perform operations with brain
    for step in range(100):
        ctx.step(step)
    
    # Resources are automatically cleaned up when exiting context
```

## Error Handling

The enhanced bindings provide improved error handling:

```python
try:
    brain = pynlm.create_brain_with_preset("invalid_preset")
except pynlm.ValueError as e:
    print(f"Invalid preset: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Measurement

Measure execution time of operations:

```python
# Time an operation
import time

start_time = time.time()
brain.step(100)
end_time = time.time()

print(f"Step 100 took {end_time - start_time:.4f} seconds")

# Use built-in timing function
execution_time = pynlm.time_operation(lambda: brain.step(100))
print(f"Operation took {execution_time} milliseconds")
```

## Backward Compatibility

The enhanced bindings maintain backward compatibility with the original API:

```python
# Original API (still supported)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# New enhanced API
brain = pynlm.create_brain_with_preset("phase3")
```

## Installation and Building

### From Source

```bash
# Clone the repository
# Clone the repository
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install the Python package
pip install -e .
```

### Using pip

```bash
# Install the package (will build from source)
pip install .

# Install in development mode (for editing)
pip install -e .
```

### Testing

```bash
# Run the test suite
pytest tests/

# Run the comprehensive test script
python test_nlm_python_api.py
```

## Requirements

### Python Version

- Python 3.8 or higher

### Required Packages

- `numpy>=1.20`
- `scipy>=1.7.0`
- `matplotlib>=3.5.0`
- `seaborn>=0.11.0`
- `scikit-learn>=1.0`
- `networkx>=2.6`
- `psutil>=5.8`

### Optional Packages

- `jupyter>=1.0` (for interactive examples)
- `ipython>=7.0` (for interactive shell)

## Troubleshooting

### Common Issues

#### Import Error: No module named 'pynlm'

```bash
# Install or reinstall the package
pip install --force-reinstall .
```

#### Build Errors

```bash
# Ensure all dependencies are installed
pip install numpy scipy matplotlib seaborn scikit-learn networkx psutil

# Check that CMake and a C++ compiler are available
g++ --version
cmake --version
```

#### Missing Modules

```bash
# Install optional dependencies
pip install jupyter ipython
```

#### Visualization Issues

```python
# Set appropriate backend for your environment
import matplotlib
matplotlib.use('Agg')  # For non-interactive environments
```

## Examples and Demos

The repository includes example scripts and demos. Check the `examples/` directory for additional usage patterns.

## Further Reading

- [NLM Documentation](https://nlm.readthedocs.io) - Comprehensive documentation
- [NLM Architecture](docs/ARCHITECTURE.md) - System architecture overview
- [NLM Science](docs/SCIENCE.md) - Scientific background
- [NLM Roadmap](docs/ROADMAP.md) - Development roadmap

## License

MIT License. See LICENSE file for details.

## Version

This enhanced version of the Python bindings is version 0.1.0.

## Acknowledgments

This enhanced API builds upon the original NLM Python bindings and adds comprehensive functionality for:

- High-level convenience functions
- Visualization support
- Advanced machine learning features
- Performance monitoring
- Development tools
- Improved API usability

## Changes from Original

Version 0.1.0 introduces the following enhancements:

1. **New high-level convenience functions** for common tasks
2. **Visualization support** using matplotlib and networkx
3. **Machine learning features** using scikit-learn
4. **Performance monitoring** tools
5. **Development utilities** for debugging and testing
6. **Improved error handling** and type conversions
7. **Context managers** for resource cleanup
8. **Property access** for better data inspection
9. **Iterator support** for collections
10. **Backward compatibility** with original API

All changes maintain backward compatibility while adding new functionality.
