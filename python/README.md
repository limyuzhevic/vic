# NLM Python Bindings

This directory contains the Python bindings for the NLM (Neural Learning Machine) neural simulation framework.

## Overview

The Python bindings provide access to the complete NLM neural simulation capabilities through a high-level, Pythonic API. This allows researchers and developers to:

- Create and simulate neural networks using the brain-inspired architecture
- Configure and run experiments with flexible Python control
- Access brain state, metrics, and diagnostic information
- Save and load brain states for persistence
- Integrate NLM with Python data science and machine learning tools

## Architecture

The bindings expose the core NLM systems through a clean, Python interface:

### Core Systems
- **Brain**: The main neural substrate with neurons, synapses, and all integrated systems
- **Memory**: Working memory, episodic memory, and associative memory systems
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error systems
- **Prediction**: Internal models and predictive coding
- **Cognition**: Planning, concept formation, and attention
- **Development**: Synaptogenesis, pruning, and maturation

### Key Features
- **Flexible Configuration**: Python dicts, objects, or JSON for configuration
- **Easy Experiment Control**: Run simulations with precise control over parameters
- **Rich Diagnostics**: Access to neural activity, synaptic weights, and system metrics
- **Persistence**: Save and load brain states for long-running experiments
- **Extensibility**: Hook into neural processes for custom interactions

## Quick Start

```python
import pynlm

# Create a brain with custom configuration
brain = pynlm.Brain(neuron_count=1000, regions=1)

# Initialize the brain
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step, step * 0.001)

# Access metrics
print(f"Average firing rate: {brain.getAverageFiringRate()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Save brain state
brain.save("my_brain.bin")

# Load brain state
brain2 = pynlm.Brain()
brain2.load("my_brain.bin")
```

## Available Modules

### Core Module (`pynlm.core`)
- `Brain`: Main neural simulation class
- `Config`: Configuration management
- `SimulationClock`: Time tracking
- `Logger`: Logging system

### Simulation Module (`pynlm.simulation`)
- `Experiment`: High-level experiment runner
- `Metrics`: Performance and system metrics

### System Modules (`pynlm.*`)
- `pynlm.memory`: Working memory, episodic memory, associative memory
- `pynlm.neuromodulation`: Dopamine, curiosity, reward processing
- `pynlm.prediction`: Predictive coding and forward models
- `pynlm.cognition`: Planning, concept formation, attention
- `pynlm.development`: Synaptogenesis, pruning, neural maturation
- `pynlm.environment`: World models and sensory processing
- `pynlm.motor`: Action selection and motor control

## Configuration

### Python Dictionary Configuration
```python
config = {
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.15,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "enable_development": True,
}
```

### Programmatic Configuration
```python
from pynlm import Config

config = Config()
config.set("neuron_count", 1000)
config.set("connection_probability", 0.15)
config.set("stdp_ltp_weight", 0.01)
```

### JSON Configuration
```python
import json

with open("config.json", "r") as f:
    config_dict = json.load(f)

brain = pynlm.Brain(**config_dict)
```

## Experiments

### Basic Experiments
```python
from pynlm import Experiment

# Create a simple learning experiment
experiment = Experiment(brain)
result = experiment.run(steps=1000)

print(f"Total reward: {result.total_reward}")
print(f"Learning achieved: {result.learning_occurred}")
```

### Phase 6 Integration
```python
from pynlm import Phase6Integration

# Run the full integration test
integration = Phase6Integration()
result = integration.run(
    neuron_count=500,
    max_steps=2000,
    enable_checkpointing=True,
    enable_replay=True
)

print(f"Integration status: {result.integration_status}")
print(f"Memory working memory integrated: {result.memory_working_memory_integrated}")
```

## Advanced Usage

### Custom Events and Hooks
```python
brain = pynlm.Brain()
brain.set_spike_callback(my_spike_handler)
brain.set_reward_callback(my_reward_handler)
brain.set_development_callback(my_development_handler)
```

### Real-time Monitoring
```python
import time
import threading

brain = pynlm.Brain()

# Monitor brain activity in a separate thread
monitor = pynlm.Monitor(brain)
monitor.start_monitoring(interval=0.1)

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.001)
    time.sleep(0.001)

monitor.stop_monitoring()
```

### Visualization
```python
from pynlm.visualization import NetworkVisualizer

visualizer = NetworkVisualizer(brain)
visualizer.plot_activity_history()
visualizer.plot_synapse_weights()
visualizer.plot_memory_traces()
```

## Performance Considerations

1. **Memory Management**: Use `brain.reset()` to free memory between experiments
2. **Parallel Processing**: Use `pynlm.parallel` for batch simulations
3. **Large Scale**: For >10,000 neurons, consider using `pynlm.hpc` for distributed computing
4. **Streaming**: Use `pynlm.streaming` for real-time data streams

## Error Handling

The Python bindings include comprehensive error handling:

```python
try:
    brain = pynlm.Brain()
    brain.initialize()
except pynlm.NLMPythonError as e:
    print(f"NLM error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Extending NLM with Python

You can extend NLM functionality through Python:

### Custom Plasticity Rules
```python
from pynlm import PlasticityRule

class CustomPlasticity(PlasticityRule):
    def apply(self, pre_spike, post_spike, synapse):
        # Custom plasticity rule
        delta = 0.001 * (pre_spike - post_spike)
        synapse.set_weight(synapse.get_weight() + delta)

brain.set_plasticity_rule(CustomPlasticity())
```

### Custom Neuromodulation
```python
from pynlm import Neuromodulator

class CustomNeuromodulator(Neuromodulator):
    def update(self, reward, novelty, time_step):
        # Custom neuromodulation logic
        return reward * 0.5 + novelty * 0.5

brain.set_neuromodulator(CustomNeuromodulator())
```

## API Reference

See the API documentation for complete reference of all available classes, methods, and functions.

## Testing and Validation

The Python bindings include comprehensive tests:

```bash
pytest pynlm/tests/
```

For performance benchmarks:

```bash
python -m pynlm.benchmark
```

## Troubleshooting

Common issues and solutions:

1. **Configuration Errors**: Use `pynlm.Config.validate()` to check configuration
2. **Memory Issues**: Use `brain.reset()` between large experiments
3. **Performance Issues**: Profile with `cProfile` or `memory_profiler`
4. **Python Binding Errors**: Check that NLM was compiled with Python bindings enabled

## Migration Guide

### From NLM v0.5 to v0.6 Python API

- `Brain.create()` → `Brain()` (constructor simplified)
- `Experiment.run_experiment()` → `Experiment.run()`
- `MetricCollector` → `Metrics`
- `CheckpointManager` → `brain.save()`/`brain.load()`

## Future Enhancements

Planned improvements:

1. **GPU Acceleration**: Native GPU support for neural simulation
2. **Real-time Web Interface**: Web-based visualization and control
3. **Cloud Integration**: AWS/GCP integration for distributed simulations
4. **ML Model Export**: Export trained models to ONNX/TensorFlow/PyTorch
5. **Simulation Frameworks**: Integration with OpenAI Gym, PyBullet, etc.

## Support

For questions and issues:
- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Documentation: https://nlm.readthedocs.io
- Slack Channel: #nlm-python-bindings

---
*This documentation is generated automatically from the NLM source code.*
