# Improved NLM Python Bindings

This document describes the improved NLM (Neural Learning Machine) Python bindings, which provide a more Pythonic and user-friendly interface to the underlying C++ neural simulation framework.

## Overview

The improved NLM Python bindings address several key issues with the original bindings:

1. **Pythonic API**: Functions and classes follow Python conventions
2. **Convenience Functions**: Pre-built functions for common operations
3. **Enhanced Configuration**: Better configuration management and presets
4. **Advanced Features**: Tools for training, optimization, and analysis
5. **Performance Monitoring**: Built-in performance tracking and analysis
6. **Error Handling**: Comprehensive error handling and validation
7. **Documentation**: Extensive documentation and examples

## Key Improvements

### 1. Core API Enhancements

#### Convenience Functions
- `create_simulation_config()`: Create configurations with sensible defaults
- `run_brain_simulation()`: Run simulations with monitoring
- `create_agent_with_world()`: Set up complete agent-world systems
- `SimulationRunner`: High-level simulation runner class

#### Configuration Presets
- `ConfigurationPresets.create_simple_brain()`: Minimal brain setup
- `ConfigurationPresets.create_advanced_brain()`: Full-featured brain
- `ConfigurationPresets.create_minimal_brain()`: Testing configuration

#### World Presets
- `WorldPresets.create_simple_world()`: Basic 2D world
- `WorldPresets.create_large_world()`: Exploration world

### 2. Advanced Features

#### Training Tools
- `AgentTrainer`: Reinforcement learning for agents
- `BrainOptimizer`: Evolutionary algorithm for brain optimization
- `CuriosityDrivenTrainer`: Curiosity-driven exploration training

#### Analysis Tools
- `PerformanceMonitor`: Track simulation performance
- `BrainAnalyzer`: Analyze neural activity patterns
- `benchmark_simulation()`: Performance benchmarking
- `analyze_simulation_performance()`: Comprehensive analysis

#### Batch Processing
- `BatchProcessor`: Process multiple simulations in batch
- `batch_simulate()`: Run multiple simulations efficiently

### 3. Improved Error Handling

#### Custom Exceptions
- `NLMError`: Base exception for all NLM errors
- `SimulationError`: Simulation-specific errors
- `ConfigurationError`: Configuration-specific errors

#### Validation Utilities
- `validate_config()`: Validate configuration parameters
- `validate_world()`: Validate world configuration

## Usage Examples

### Basic Usage

```python
import pynlm

# Create a simple brain
config = pynlm.create_simulation_config(neuron_count=100)
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
stats = pynlm.run_brain_simulation(brain, num_steps=100)
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
```

### Complete Agent Setup

```python
import pynlm

# Create complete agent setup using presets
config = pynlm.ConfigurationPresets.create_advanced_brain(200)
world = pynlm.WorldPresets.create_large_world(15, 15)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning features
agent.enable_all_features()  # New convenience method

# Run episode with simulation runner
runner = pynlm.SimulationRunner(config, {'width': 15, 'height': 15})
runner.initialize()
runner.enable_all_features()

stats = runner.run_episode(500)
print(f"Episode reward: {stats['total_rewards']}")
```

### Advanced Training

```python
import pynlm
from pynlm.advanced_features import AgentTrainer, CuriosityDrivenTrainer

# Setup simulation
config = pynlm.ConfigurationPresets.create_advanced_brain(500)
world = pynlm.WorldPresets.create_large_world(20, 20)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Train with curiosity-driven learning
trainer = CuriosityDrivenTrainer(brain, agent, world)
stats = trainer.train_with_curiosity(1000)

print(f"Training stats: {trainer.get_curiosity_stats()}")
```

### Performance Monitoring

```python
import pynlm
from pynlm.utilities import PerformanceMonitor

# Setup simulation
config = pynlm.create_simulation_config(100)
brain = pynlm.createBrain(config)
brain.initialize()

# Start monitoring
monitor = PerformanceMonitor()
monitor.start_monitoring()

# Run simulation with monitoring
for step in range(100):
    brain.step(step)
    firing_rate = brain.getAverageFiringRate()
    spike_count = brain.getTotalSpikeCount()
    monitor.record_step(firing_rate, spike_count)

# Get statistics
stats = monitor.get_statistics()
print(f"Average step time: {stats['average_step_time']:.4f} seconds")
```

### Batch Processing

```python
import pynlm
from pynlm.utilities import batch_simulate

# Create multiple configurations
configs = [
    pynlm.ConfigurationPresets.create_simple_brain(50),
    pynlm.ConfigurationPresets.create_advanced_brain(200),
    pynlm.ConfigurationPresets.create_minimal_brain(20)
]

# Run batch simulation
results = batch_simulate(configs, num_steps=100)

for result in results:
    print(f"Config {result['config_index']}: "
          f"Spikes={result['total_spikes']}, "
          f"Firing rate={result['final_firing_rate']:.2f} Hz")
```

### Configuration Validation

```python
import pynlm

# Create configuration
config = pynlm.create_simulation_config(1000)

# Validate configuration
errors = pynlm.validate_config(config)
if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
else:
    print("Configuration is valid!")
```

### API Information

```python
import pynlm

# Get API information
info = pynlm.get_api_info()
print(f"NLM Python API Version: {info['version']}")
print(f"Available features: {len(info['features'])}")
print(f"Available classes: {len(info['available_classes'])}")
print(f"Available functions: {len(info['available_functions'])}")
```

## Available Classes

### Core Classes
- `Config`: Configuration management
- `Brain`: Neural simulation brain
- `AgentBrain`: Interface between brain and world
- `SimpleWorld`: 2D simulation world

### Advanced Classes
- `SimulationRunner`: High-level simulation management
- `PerformanceMonitor`: Performance tracking
- `BrainAnalyzer`: Neural activity analysis
- `AgentTrainer`: Reinforcement learning trainer
- `BrainOptimizer`: Evolutionary optimizer
- `CuriosityDrivenTrainer`: Curiosity-driven learning
- `BatchProcessor`: Batch processing utilities

## Available Functions

### Core Functions
- `createDefaultConfig()`: Create default configuration
- `createBrain()`: Create neural brain
- `createSimpleWorld()`: Create simulation world
- `createAgentBrain()`: Create agent interface
- `create_simulation_config()`: Create simulation configuration
- `run_brain_simulation()`: Run brain simulation
- `create_agent_with_world()`: Create agent-world system
- `benchmark_brain()`: Benchmark brain performance
- `analyze_spike_patterns()`: Analyze spike patterns
- `optimize_configuration()`: Optimize configuration
- `benchmark_simulation()`: Benchmark simulation
- `validate_config()`: Validate configuration
- `validate_world()`: Validate world

### Utility Functions
- `get_api_info()`: Get API information
- `calculate_firing_rate()`: Calculate firing rate
- `calculate_activity_index()`: Calculate activity index

## Performance Features

### Monitoring
- Real-time performance tracking
- Step time measurement
- Firing rate monitoring
- Memory usage tracking

### Analysis
- Spike pattern analysis
- Firing rate statistics
- Temporal pattern detection
- Regional activity analysis

### Optimization
- Configuration optimization
- Parameter tuning
- Evolutionary algorithms
- Fitness evaluation

## Installation and Setup

### Prerequisites
- Python 3.8+
- NumPy (for utilities and advanced features)

### Installation
```bash
# Install NLM Python bindings
pip install pynlm

# For advanced features with NumPy
pip install 'numpy>=1.20'
```

### Development Setup
```bash
# Clone the repository
cd /path/to/nlm

# Build the C++ bindings
mkdir build
cd build
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j4

# Install Python module
pip install .

# For development with editable install
pip install -e .
```

## API Reference

### Error Classes

```python
# Base exception for all NLM errors
class NLMError(Exception):
    pass

# Simulation-specific errors
class SimulationError(NLMError):
    pass

# Configuration-specific errors
class ConfigurationError(NLMError):
    pass
```

### Configuration Validation

```python
# Validate a configuration
errors = pynlm.validate_config(config)
if not errors:
    print("Configuration is valid")
else:
    for error in errors:
        print(f"Invalid: {error}")
```

### Performance Monitoring

```python
# Create performance monitor
monitor = pynlm.PerformanceMonitor()

# Start monitoring
monitor.start_monitoring()

# Record step data
monitor.record_step(firing_rate, spike_count)

# Get statistics
stats = monitor.get_statistics()
print(f"Average step time: {stats['average_step_time']:.4f}s")
```

## Performance Tips

1. **Use Batch Processing**: For multiple simulations, use `batch_simulate()`
2. **Monitor Performance**: Use `PerformanceMonitor` for large-scale simulations
3. **Optimize Configurations**: Use `optimize_configuration()` for target metrics
4. **Use Presets**: Use predefined configurations for common use cases
5. **Enable Learning Sparingly**: Only enable learning when needed for performance

## Troubleshooting

### Common Issues

1. **ImportError: No module named 'pynlm'**
   - Ensure NLM is properly installed: `pip install .`
   - Check that dependencies are installed: `pip install pybind11 scikit-build-core numpy`

2. **Configuration errors**
   - Use `validate_config()` to check for issues
   - Try using predefined configurations

3. **Performance issues**
   - Reduce brain size for testing
   - Disable learning features
   - Use `PerformanceMonitor` to identify bottlenecks

4. **Memory issues**
   - Reduce neuron count
   - Use smaller worlds
   - Monitor memory usage with `PerformanceMonitor`

### Getting Help

- **Documentation**: See `docs/` directory for detailed documentation
- **Examples**: Check the `examples/` directory for more examples
- **Community**: Join the NLM community for support
- **Issues**: Report bugs at the NLM GitHub repository

## Version Information

- **API Version**: 0.1.0
- **Underlying Framework**: NLM 0.1.0
- **Build Date**: $(date)
- **License**: MIT

## Changelog

### Version 0.1.0
- Initial release with improved Python API
- Added convenience functions and presets
- Included performance monitoring tools
- Added advanced training features
- Comprehensive error handling and validation

### Future Improvements
- Machine learning integration
- Advanced visualization tools
- Web API support
- Cloud integration
- More sophisticated optimization algorithms

## Acknowledgments

This improved NLM Python bindings build upon the excellent work of the NLM team and the pybind11 project. Special thanks to all contributors who have improved the NLM framework.

## License

MIT License. See LICENSE file for details.
