# Enhanced pynlm Python Bindings

This document provides comprehensive information about the enhanced Python bindings for NLM (Neural Learning Machine).

## Overview

The enhanced pynlm bindings provide a comprehensive Python interface for NLM neural simulations, featuring:

- **High-level wrapper classes** with intuitive APIs
- **Configuration builders** for easy parameter management
- **Performance monitoring** tools
- **Visualization utilities**
- **Error handling and validation**
- **Utility functions** for common tasks

## Key Improvements

### 1. Enhanced Brain Simulator (`BrainSimulator`)

The `BrainSimulator` class provides a high-level interface for brain simulations:

```python
from pynlm import BrainSimulator

# Create simulator
simulator = BrainSimulator()

# Run simulation with method chaining
simulator.step(0).step(1).step(2)

# Get statistics
stats = simulator.get_current_stats()
print(f"Neurons: {stats['total_neurons']}")
print(f"Firing rate: {simulator.get_average_firing_rate():.2f} Hz")
```

**Features:**
- Method chaining for fluent interface
- Automatic statistics collection
- Progress tracking
- Error handling and recovery
- State saving and loading

### 2. Agent Simulator (`AgentSimulator`)

The `AgentSimulator` class manages complete agent-environment interactions:

```python
from pynlm import AgentSimulator

# Create agent simulator
simulator = AgentSimulator(world_width=20, world_height=20)
simulator.initialize()

# Run episode
episode_data = simulator.run_episode(100)

# Get statistics
stats = simulator.get_episode_stats()
```

**Features:**
- Complete agent lifecycle management
- Sensory processing pipeline
- Action execution and reward collection
- Episode statistics and analysis
- Configurable world and agent parameters

### 3. Configuration Builder (`NLMConfigBuilder`)

The `NLMConfigBuilder` provides a fluent interface for creating NLM configurations:

```python
from pynlm import NLMConfigBuilder

# Create custom configuration
config = (NLMConfigBuilder()
         .apply_preset('standard')
         .set_neural_parameters(neuron_count=2000, synapse_density=0.15)
         .set_plasticity(stdp_enabled=True, hebbian_enabled=True)
         .set_neuromodulation(dopamine_scale=1.5)
         .set_development(developmental_enabled=True)
         .get_config())
```

**Features:**
- Preset configurations ('minimal', 'standard', 'advanced', 'research')
- Fluent method chaining
- Parameter validation
- Configuration export/import
- Parameter information and descriptions

### 4. Performance Monitor (`NLMPerformanceMonitor`)

The `NLMPerformanceMonitor` tracks and analyzes simulation performance:

```python
from pynlm import NLMPerformanceMonitor

# Create monitor
monitor = NLMPerformanceMonitor()
monitor.start_monitoring()

# Run simulation with monitoring
for step in range(1000):
    brain.step(step)
    monitor.end_step('simulation_step')

# Get performance summary
summary = monitor.get_summary()
print(f"Average step time: {summary['average_step_time']*1000:.1f}ms")
print(f"Steps per second: {summary['steps_per_second']:.1f}")
```

**Features:**
- Step-by-step performance tracking
- Event counting and analysis
- Summary statistics
- Memory and resource monitoring

### 5. Utility Functions (`NLMUtils`)

The `NLMUtils` class provides common utility functions:

```python
from pynlm import NLMUtils

# Generate random configuration
random_config = NLMUtils.generate_random_config(
    neuron_range=(500, 2000),
    synapse_range=(0.05, 0.3)
)

# Create simulation grid
agents = NLMUtils.create_simulation_grid(world_size=20, num_agents=5)

# Run benchmark
benchmark = NLMUtils.benchmark_simulation(num_steps=1000)
print(f"Benchmark: {benchmark['steps_per_second']:.1f} steps/sec")
```

**Features:**
- Random configuration generation
- Simulation grid creation
- Performance benchmarking
- Common utility functions

### 6. Enhanced Visualization (`NLMVisualizer`)

The `NLMVisualizer` class provides comprehensive visualization capabilities:

```python
from pynlm import NLMVisualizer

# Plot brain activity
fig, plt = NLMVisualizer.plot_brain_activity(stats_history)
if fig:
    plt.savefig('brain_activity.png', dpi=150, bbox_inches='tight')
    plt.close()

# Plot action distribution
fig, plt = NLMVisualizer.plot_action_distribution(episode_data)
if fig:
    plt.savefig('action_distribution.png', dpi=150, bbox_inches='tight')
    plt.close()
```

**Features:**
- Multiple plot types
- Comprehensive metric visualization
- Easy export to image files
- Graceful handling of missing dependencies

## Error Handling

The enhanced bindings provide comprehensive error handling:

```python
import pynlm

try:
    simulator = BrainSimulator()
    simulator.step(0)  # Will raise error if not initialized
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")
```

**Exception Types:**
- `NLMError`: Base exception for all NLM errors
- `ValidationError`: For configuration validation errors
- `SimulationError`: For simulation-related errors
- `ConfigError`: For configuration loading/saving errors

## Configuration Presets

The configuration builder includes several preset configurations:

### Minimal
```python
config = NLMConfigBuilder().apply_preset('minimal').get_config()
# Small brain for testing/debugging
```

### Standard
```python
config = NLMConfigBuilder().apply_preset('standard').get_config()
# Balanced configuration for most use cases
```

### Advanced
```python
config = NLMConfigBuilder().apply_preset('advanced').get_config()
# Complex configuration with all features enabled
```

### Research
```python
config = NLMConfigBuilder().apply_preset('research').get_config()
# High-performance configuration for research
```

## Performance Optimization

The enhanced bindings include performance optimization features:

### Parallel Processing
```python
import concurrent.futures
import pynlm

# Create multiple brains
brains = [pynlm.createBrain(pynlm.createDefaultConfig()) for _ in range(10)]
for brain in brains:
    brain.initialize()

# Run simulations in parallel
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
    futures = []
    for i, brain in enumerate(brains):
        future = executor.submit(run_simulation, brain, 100)
        futures.append(future)
    
    for future in concurrent.futures.as_completed(futures):
        result = future.result()
```

### Memory-Efficient Simulation
```python
import gc
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate with checkpointing
for step in range(10000):
    brain.step(step)
    
    if step % 1000 == 0:
        brain.save(f"checkpoint_{step}.pkl")
        gc.collect()  # Force garbage collection
```

### Batch Processing
```python
import pynlm

# Create multiple brains
brains = []
for i in range(10):
    config = NLMUtils.generate_random_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    brains.append(brain)

# Run batch simulation
results = []
for i, brain in enumerate(brains):
    for step in range(100):
        brain.step(step)
    
    results.append({
        'brain_id': i,
        'neurons': brain.getTotalNeuronCount(),
        'performance': brain.getAverageFiringRate()
    })
```

## Visualization Examples

### Brain Activity Plot
```python
import matplotlib.pyplot as plt
from pynlm import NLMVisualizer

# Generate sample data
stats_history = [
    {'step': i, 'total_neurons': 1000, 'firing_neurons': 50 + i}
    for i in range(100)
]

# Create plot
fig, plt = NLMVisualizer.plot_brain_activity(stats_history)
if fig:
    plt.savefig('brain_activity.png', dpi=150, bbox_inches='tight')
    plt.close()
```

### Action Distribution Plot
```python
import matplotlib.pyplot as plt
from pynlm import NLMVisualizer

# Generate sample action data
episode_data = [
    {'action_type': 'MoveForward', 'step': i}
    for i in range(50)
]

# Create plot
fig, plt = NLMVisualizer.plot_action_distribution(episode_data)
if fig:
    plt.savefig('action_distribution.png', dpi=150, bbox_inches='tight')
    plt.close()
```

## Testing

The enhanced bindings include comprehensive tests:

```bash
# Run all tests
pytest tests/test_enhanced_bindings.py -v

# Run specific test classes
pytest tests/test_enhanced_bindings.py::TestBrainSimulator -v
pytest tests/test_enhanced_bindings.py::TestAgentSimulator -v

# Run with coverage
pytest tests/ --cov=pynlm --cov-report=html
```

## Example Scripts

### Basic Examples
- `examples/basic.py`: Basic NLM simulation example
- `examples/agent.py`: Agent-based simulation example
- `examples/visualization.py`: Visualization and performance examples

### Advanced Examples
- `examples/demo_enhanced.py`: Comprehensive demonstration of enhanced features

### Documentation
- `pynlm.md`: Comprehensive documentation for the enhanced bindings

## Migration Guide

### From Old API to Enhanced API

**Old API:**
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

**New API:**
```python
import pynlm
from pynlm import BrainSimulator

# Create brain using enhanced simulator
simulator = BrainSimulator()

# Run simulation with method chaining
simulator.run(100)

# Get statistics
for step_stats in simulator.get_stats_history():
    print(f"Step {step_stats['step']}: {step_stats['firing_neurons']} firing neurons")

# Get average firing rate
print(f"Average firing rate: {simulator.get_average_firing_rate():.2f} Hz")
```

## Installation and Setup

### Dependencies

The enhanced bindings require:
- Python 3.8+
- pybind11
- scikit-build-core
- pytest (for testing)
- numpy
- matplotlib (for visualization, optional)

### Installation

```bash
# Install dependencies
pip install pybind11 scikit-build-core pytest numpy

# Install enhanced bindings
pip install -e .

# Or install from source
pip install build
python -m build
pip install dist/*.whl
```

## Troubleshooting

### Common Issues

**Issue: Matplotlib not available**
```python
# Installation
pip install matplotlib

# Visualization will work automatically
```

**Issue: Performance monitoring overhead**
```python
# Disable monitoring for production
# Or sample less frequently
monitor = NLMPerformanceMonitor()
for step in range(1000):
    brain.step(step)
    if step % 10 == 0:  # Sample every 10 steps
        monitor.end_step('simulation_step')
```

**Issue: Memory usage**
```python
import gc

# Force garbage collection periodically
for step in range(10000):
    brain.step(step)
    if step % 1000 == 0:
        gc.collect()
```

**Issue: Configuration validation**
```python
# Use the configuration builder for validation
from pynlm import NLMConfigBuilder

builder = NLMConfigBuilder()
builder.set_parameter('brain.neuron_count', 1000)
builder.set_parameter('brain.synapse_density', 0.1)

# Validate (would be caught by C++ at runtime)
errors = builder.validate()
if errors:
    print(f"Validation errors: {errors}")
```

### Getting Help

1. Check the documentation in `docs/`
2. Visit the NLM GitHub repository
3. Check the issue tracker for known problems
4. Join the NLM community for support

## License

MIT License

## Copyright

Copyright © 2026 NLM Authors
