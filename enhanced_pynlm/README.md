# Enhanced NLM Python API

This document provides comprehensive information about the enhanced NLM Python API, which extends the original pynlm bindings with improved interfaces, better error handling, and advanced features.

## Overview

The Enhanced NLM Python API builds upon the original pynlm bindings to provide a more Pythonic interface for neural simulation. It includes:

- **Context managers** for resource safety
- **Better error handling** with Python exceptions
- **Performance monitoring** tools
- **Neural recording** and playback capabilities
- **Configuration management** with validation
- **Advanced agent simulation** features
- **Analysis and visualization** tools

## Installation

### From Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Install the enhanced API
pip install -e enhanced_pynlm/
```

### Prerequisites

- Python 3.8 or higher
- numpy >= 1.20
- pip >= 21.0

## Quick Start

### Basic Usage

```python
import enhanced_pynlm

# Create an enhanced configuration
config = enhanced_pynlm.create_enhanced_config()
config.set("neuron_count", 1000)
config.set("development_enabled", True)

# Create an enhanced brain with context manager
with enhanced_pynlm.create_enhanced_brain(config) as brain:
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    # Get statistics
    print(f"Spike count: {brain.get_total_spike_count()}")
    print(f"Firing rate: {brain.get_average_firing_rate():.2f} Hz")
```

### Quick Simulation

```python
# Use the convenience function for quick simulation
brain = enhanced_pynlm.quick_simulation(neuron_count=500, steps=200)
print(f"Simulation completed: {brain.get_total_spike_count()} spikes")
```

### Agent Simulation

```python
# Run an agent simulation
agent, stats = enhanced_pynlm.agent_simulation(
    agent_config={"neuron_count": 800, "curiosity_enabled": True},
    max_steps=500
)

print(f"Episode completed: {stats['total_steps']} steps")
print(f"Total reward: {stats['total_reward']:.2f}")
```

## Key Features

### 1. Context Managers

The enhanced API provides context managers for automatic resource management:

```python
# Enhanced brain with context manager
with enhanced_pynlm.create_enhanced_brain(config) as brain:
    brain.initialize()
    # Your simulation code here
    # Brain automatically resets when context exits

# Enhanced session with monitoring
with enhanced_pynlm.create_enhanced_session(config, monitor) as (brain, monitor):
    # Your simulation with monitoring
    pass
```

### 2. Performance Monitoring

Monitor simulation performance in real-time:

```python
# Create and start monitoring
brain = enhanced_pynlm.create_enhanced_brain()
monitor = brain.start_monitoring()

# Run simulation with monitoring
for step in range(100):
    monitor.step(step)

# Stop monitoring and get statistics
stats = monitor.stop_monitoring()
print(f"Average step time: {stats['avg_step_time']*1000:.2f} ms")
print(f"Steps per second: {stats['steps_per_second']:.2f}")
```

### 3. Neural Recording and Playback

Record neural activity patterns for later analysis:

```python
# Start recording
brain = enhanced_pynlm.create_enhanced_brain()
recorder = brain.start_recording("my_experiment")

# Run simulation while recording
for i in range(50):
    recorder.record_step(i)

# Stop recording
recording = brain.stop_recording()

# Play back recording
playback_brain = recorder.play_recording("my_experiment")
```

### 4. Configuration Management

Manage configurations with validation and saving:

```python
from enhanced_pynlm import EnhancedConfig, ConfigManager

# Create and validate configuration
config = EnhancedConfig()
config.set("neuron_count", 1000)  # Auto-validated
config.set("random_seed", 42)

# Save configuration
config.save("my_config.json")

# Load configuration
config.load("my_config.json")

# Use configuration manager for experiments
config_manager = ConfigManager()
config_manager.add_config("efficient", {"neuron_count": 500})
config_manager.add_config("complex", {"neuron_count": 1000})
config_manager.set_current("efficient")
```

### 5. Advanced Agent Simulation

Create sophisticated agents with learning capabilities:

```python
# Create enhanced setup
brain = enhanced_pynlm.create_enhanced_brain()
world = enhanced_pynlm.create_enhanced_world()
agent = enhanced_pynlm.create_enhanced_agent(brain, world)

# Run episode with monitoring
def episode_callback(step, percept, action, result):
    print(f"Step {step}: {action} -> reward {result.reward}")

stats = agent.run_episode(max_steps=300, render_callback=episode_callback)

# Get detailed statistics
print(f"Success rate: {stats['success_rate']*100:.1f}%")
print(f"Action distribution: {stats['action_distribution']}")
```

### 6. Analysis and Visualization

Analyze brain data and compare experiments:

```python
from enhanced_pynlm import BrainAnalyzer

# Compare two brains
brain1 = enhanced_pynlm.create_enhanced_brain()
brain2 = enhanced_pynlm.create_enhanced_brain()

comparison = BrainAnalyzer.compare_brains(brain1, brain2, 'spiking')
print(f"Similarity: {comparison['similarity']:.2f}")
print(f"Better brain: {comparison['better_brain']}")

# Analyze firing patterns
spike_times = [0.1, 0.3, 0.5, 0.7, 1.0]  # Example spike times
analysis = BrainAnalyzer.analyze_firing_patterns(spike_times, 2.0)
print(f"Pattern: {analysis['pattern']}")
print(f"Firing rate: {analysis['avg_firing_rate']:.2f} Hz")
```

## Error Handling

The enhanced API provides better error handling with specific exception types:

```python
import enhanced_pynlm

try:
    with enhanced_pynlm.create_enhanced_brain() as brain:
        brain.step(0)
except enhanced_pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except enhanced_pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except enhanced_pynlm.ResourceError as e:
    print(f"Resource error: {e}")
```

## Experiments and Analysis

### Running Experiments

```python
from enhanced_pynlm import ExperimentManager, create_enhanced_config

# Create experiment manager
experiment_manager = ExperimentManager()

# Add configurations
experiment_manager.add_config("config1", {"neuron_count": 500})
experiment_manager.add_config("config2", {"neuron_count": 1000})

# Run experiments
experiment_manager.set_current("config1")

def experiment_func(brain, iteration):
    for step in range(100):
        brain.step(step)
    return {
        'spike_count': brain.get_total_spike_count(),
        'firing_rate': brain.get_average_firing_rate()
    }

results = experiment_manager.run_experiments(
    experiment_func, iterations=5
)

# Analyze results
total_spikes = [r['spike_count'] for r in results]
print(f"Spike count range: {min(total_spikes)} - {max(total_spikes)}")
```

### Parameter Sweeps

```python
# Run parameter sweep
param_configs = {
    'neuron_count': [100, 500, 1000],
    'connection_probability': [0.1, 0.2, 0.3],
    'development_enabled': [True, False]
}

results = experiment_manager.run_parameter_sweep(
    param_configs, experiment_func
)

# Find optimal configuration
optimal = experiment_manager.find_best_configuration(results)
print(f"Optimal neurons: {optimal['neuron_count']}")
print(f"Optimal probability: {optimal['connection_probability']}")
```

## Advanced Features

### Custom Monitoring

```python
class CustomMonitor(enhanced_pynlm.BrainMonitor):
    def __init__(self, brain):
        super().__init__(brain)
        self.custom_data = []
    
    def add_custom_metric(self, name, value):
        self.custom_data.append({'name': name, 'value': value, 'timestamp': time.time()})
    
    def step(self, step_num):
        super().step(step_num)
        # Add custom monitoring logic here
        self.add_custom_metric('spike_rate', self.brain.get_average_firing_rate())
```

### Neural Recording Export

```python
# Export recording to file
recorder = enhanced_pynlm.NeuralRecorder(brain)
recorder.start_recording("experiment")

for i in range(100):
    recorder.record_step(i)

recording = recorder.stop_recording()
recorder.save_recording("experiment", "experiment.json")

# Import recording from file
new_recorder = enhanced_pynlm.NeuralRecorder()
new_recorder.import_from_file("experiment.json")
```

## Backward Compatibility

The enhanced API maintains backward compatibility with the original pynlm API:

```python
# Original API still works
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)

# Enhanced API provides additional features
import enhanced_pynlm

enhanced_brain = enhanced_pynlm.create_enhanced_brain()
```

You can use both APIs in the same project:

```python
import pynlm
import enhanced_pynlm

# Use both APIs
original_brain = pynlm.createBrain(pynlm.createDefaultConfig())
enhanced_brain = enhanced_pynlm.create_enhanced_brain()
```

## Examples

See the `examples/` directory for comprehensive examples:

- `examples/basic_usage.py` - Basic usage examples
- `examples/advanced_api_usage.py` - Advanced API features
- `examples/experiment_management.py` - Experiment management
- `examples/performance_monitoring.py` - Performance monitoring
- `examples/neural_recording.py` - Neural recording and playback

## API Reference

### Core Classes

- `EnhancedConfig` - Enhanced configuration with validation
- `EnhancedBrain` - Enhanced brain with context managers
- `EnhancedWorld` - Enhanced world with better interfaces
- `EnhancedAgent` - Enhanced agent with advanced features

### Analysis Tools

- `BrainAnalyzer` - Brain comparison and analysis
- `BrainMonitor` - Performance monitoring
- `NeuralRecorder` - Neural activity recording
- `ConfigManager` - Configuration management

### Error Classes

- `SimulationError` - Simulation errors
- `ConfigurationError` - Configuration errors
- `ResourceError` - Resource management errors

### Global Functions

- `create_enhanced_config()` - Create enhanced configuration
- `create_enhanced_brain()` - Create enhanced brain
- `create_enhanced_world()` - Create enhanced world
- `create_enhanced_agent()` - Create enhanced agent
- `quick_simulation()` - Quick simulation convenience function
- `agent_simulation()` - Quick agent simulation convenience function

## Development

### Contributing

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Write tests
5. Submit a pull request

### Testing

```bash
# Run tests
pytest tests/

# Run enhanced API tests
pytest enhanced_pynlm/tests/

# Run examples
python examples/basic_usage.py
python examples/advanced_api_usage.py
```

### Building from Source

```bash
# Build the package
pip install -e .

# Build enhanced API
pip install -e enhanced_pynlm/

# Build all
pip install -e . enhanced_pynlm/
```

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgements

The Enhanced NLM Python API is based on the original NLM framework and provides enhanced interfaces for better usability and advanced features.

For more information about the NLM project, visit:
- [NLM Project](https://github.com/nlm-project/nlm)
- [NLM Documentation](https://nlm.readthedocs.io)

## Support

For support, please create an issue in the GitHub repository or contact the development team.
