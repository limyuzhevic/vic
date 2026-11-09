#!/usr/bin/env python3
"""
NLM Enhanced Python Bindings - Usage Guide

This document provides comprehensive documentation for the enhanced Python bindings
added to the NLM (Neural Learning Machine) framework.

## Overview

The enhanced Python bindings provide a powerful, user-friendly API for working with
the NLM neural simulation framework. This guide covers all new functionality including
batch operations, monitoring, error handling, and convenience methods.

## Quick Start

### Basic Usage
```python
import nlm

# Create a brain with default configuration
brain = nlm.createQuickBrain(neuron_count=500, connection_prob=0.1)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
stats = nlm.brain.getNetworkStats()
print(f"Total neurons: {stats.total_neurons}")
print(f"Average firing rate: {stats.average_firing_rate}")

# Save brain state
brain.save("/path/to/brain_checkpoint.bin")
```

### Advanced Configuration
```python
import nlm

# Create configuration
config = nlm.createDefaultConfig()

# Set nested configuration
config.setNested("system.brain.neuron_count", 200)
config.setNested("system.network.connection_probability", 0.5)
config.setNested("system.learning.rate", 0.01)

# Update from dictionary
config_dict = {
    "system.training.epochs": 1000,
    "system.optimization.learning_rate": 0.001
}
config.updateFromDict(config_dict)

# Get nested values
neuron_count = config.getNested("system.brain.neuron_count")
print(f"Neuron count: {neuron_count}")
```

## New Python API Features

### 1. Batch Operations

The `brain` namespace provides batch operations for working with multiple brains simultaneously.

#### `brain.initializeMultiple(configs)`
Initialize multiple brains from configuration objects.

```python
configs = [
    nlm.createDefaultConfig(),
    nlm.createDefaultConfig(),
    nlm.createDefaultConfig()
]

# Configure each brain differently
configs[0].set("neuron_count", 100, nlm.ConfigSource.Default)
configs[1].set("neuron_count", 200, nlm.ConfigSource.Default)
configs[2].set("neuron_count", 300, nlm.ConfigSource.Default)

brains = nlm.brain.initializeMultiple(configs)
```

#### `brain.stepMultiple(brains, steps)`
Run multiple brains in parallel.

```python
steps = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
nlm.brain.stepMultiple(brains, steps)
```

#### `brain.resetMultiple(brains)`
Reset multiple brains to their initial state.
```python
nlm.brain.resetMultiple(brains)
```

#### `brain.saveAll(brains, directory)`
Save multiple brains to checkpoint files.
```python
with tempfile.TemporaryDirectory() as temp_dir:
    filepaths = nlm.brain.saveAll(brains, temp_dir)
    for filepath in filepaths:
        print(f"Saved: {filepath}")
```

### 2. Network Statistics and Monitoring

#### `brain.getNetworkStats()`
Get comprehensive network statistics.

```python
stats = nlm.brain.getNetworkStats(brain)

print(f"Total neurons: {stats.total_neurons}")
print(f"Total synapses: {stats.total_synapses}")
print(f"Active neurons: {stats.active_neurons}")
print(f"Firing neurons: {stats.firing_neurons}")
print(f"Average firing rate: {stats.average_firing_rate}")
print(f"Excitatory/Inhibitory ratio: {stats.excitation_inhibition_ratio}")
print(f"Total spikes: {stats.total_spike_count}")

# Get neuron type distribution
for neuron_type, count in stats.neuron_type_counts.items():
    print(f"{neuron_type}: {count}")
```

#### `brain.getLearningProgress(baseline)`
Track learning progress over time.

```python
brain = nlm.createQuickBrain()
brain.initialize()

# Get baseline measurements
baseline_spike_count = brain.getTotalSpikeCount()
baseline_firing_rate = brain.getAverageFiringRate()

# Run simulation
for step in range(100):
    brain.step(step)

# Get learning progress
progress = nlm.brain.getLearningProgress(brain, baseline_spike_count, baseline_firing_rate)

# Analyze learning
for point in progress:
    print(f"Step {point.step}: Learning rate = {point.learning_rate:.4f}, "
          f"Stability = {point.stability_score:.4f}")
```

#### `brain.findActivePaths(neuron_ids)`
Find active neural pathways.

```python
# Get neuron IDs for monitoring
neuron_ids = [nlm.NeuronId(i) for i in range(10, 20)]

# Track active paths over time
steps = list(range(100))
active_paths = nlm.brain.findActivePaths(brain, neuron_ids, steps)

active_count = sum(1 for path in active_paths if path.is_active)
print(f"Neurons that were active: {active_count}/{len(neuron_ids)}")
```

### 3. World and Environment Creation

#### `nlm.createWorldWithObjects(width, height, objects)`
Create world with predefined objects.

```python
from nlm import WorldObject, WorldObjectType

# Create world objects
objects = [
    WorldObject(10, 10, WorldObjectType.Resource, 100.0),
    WorldObject(50, 50, WorldObjectType.Hazard, 50.0),
    WorldObject(90, 90, WorldObjectType.Wall, 10.0),
    WorldObject(20, 80, WorldObjectType.Marker, 1.0)
]

# Create world with objects
world = nlm.createWorldWithObjects(100, 100, objects)

# Add more objects dynamically
world.addObjectAt(30, 30, WorldObjectType.Resource, 50.0)
```

### 4. Convenience Methods for Common Operations

#### `brain.trainOnPattern(pattern, duration)`
Train brain on specific input pattern.

```python
brain = nlm.createQuickBrain()
brain.initialize()

# Create training pattern
pattern = [0.1, 0.2, 0.3, 0.4, 0.5] * 20  # 100 elements

# Train brain
brain.trainOnPattern(pattern, duration=100)

print("Training completed")
```

#### `brain.evaluateTestSet(test_patterns)`
Evaluate brain performance on test patterns.

```python
test_patterns = [
    [0.1, 0.2, 0.3, 0.4, 0.5] * 20,
    [0.9, 0.8, 0.7, 0.6, 0.5] * 20,
    [0.5, 0.5, 0.5, 0.5, 0.5] * 20
]

accuracy, accuracies = brain.evaluateTestSet(test_patterns)
print(f"Average accuracy: {accuracy:.4f}")
for i, acc in enumerate(accuracies):
    print(f"Pattern {i+1} accuracy: {acc:.4f}")
```

#### `brain.generateBehavior(world, steps)`
Generate behavior in a world environment.

```python
world = nlm.createSimpleWorld()
world.configure(100, 100, 84, 84)

# Generate behavior
actions = brain.generateBehavior(world, steps=50)

for i, action in enumerate(actions):
    print(f"Step {i}: Action = {action.getName()}, "
          f"Type = {action.getType()}")
```

#### `brain.getActionDistribution(action_type)`
Get distribution of action outputs.

```python
# Get action distribution
distribution = brain.getActionDistribution(nlm.ActionType.MoveForward, steps=1000)

print("Action distribution for MoveForward:")
for action_type, prob in distribution.items():
    if prob > 0.01:  # Only show significant probabilities
        print(f"  {action_type}: {prob:.4f}")
```

### 5. Agent Creation and Neuromodulation

#### `nlm.createAgentWithAllSystems(brain, world)`
Create agent with all neuromodulation systems.

```python
brain = nlm.createQuickBrain()
world = nlm.createWorldWithObjects(100, 100, [])

# Create agent with all systems
agent = nlm.createAgentWithAllSystems(brain, world)

# Enable all neuromodulation systems
agent.enableAllSystems()

# Process sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Get neuromodulation levels
print(f"Reward modulation level: {agent.getNeuromodulationLevel()}")
print(f"Curiosity level: {agent.getCuriosityLevel()}")
print(f"Novelty level: {agent.getNoveltyLevel()}")
```

### 6. Progress Tracking

```python
import time

# Track progress with callbacks
progress_updates = []

def progress_callback(progress, message):
    progress_updates.append((progress, message))
    print(f"Progress: {progress:.1f}% - {message}")

# Create progress tracker
tracker = nlm.createProgressTracker(1000, "Running simulation", progress_callback)

# Simulate work
for i in range(1000):
    tracker.update(1.0)
    time.sleep(0.001)  # Simulate work

print(f"Final progress: {tracker.getProgress():.1f}%")
```

### 7. Error Handling

The enhanced bindings provide custom exception types for better error handling:

```python
import nlm

try:
    # Invalid configuration
    config = nlm.createDefaultConfig()
    config.setNested("invalid.key", None)
except nlm.InvalidArgumentError as e:
    print(f"Invalid argument: {e}")

try:
    # Initialization error
    brain = None
    nlm.validateBrain(brain, "test_operation")
except nlm.InitializationError as e:
    print(f"Initialization error: {e}")
```

## Type Hints and Documentation

All new functions include comprehensive docstrings with examples and type hints:

```python
# This will show help in IDEs
help(nlm.brain.initializeMultiple)
help(nlm.brain.getNetworkStats)
help(nlm.brain.trainOnPattern)
```

## Testing

Run the test suite to verify all functionality:

```bash
python3 -m pytest tests/test_enhanced_bindings.py -v
```

Individual test functions:
- `test_basic_functionality`: Test basic API functionality
- `test_advanced_config`: Test advanced configuration methods
- `test_batch_operations`: Test batch operations
- `test_network_statistics`: Test network statistics
- `test_learning_progress`: Test learning progress tracking
- `test_active_paths`: Test neural pathway analysis
- `test_convenience_methods`: Test convenience methods
- `test_progress_tracking`: Test progress tracking
- `test_error_handling`: Test error handling
- `test_world_creation`: Test world creation utilities
- `test_agent_creation`: Test agent creation
- `test_brain_creation`: Test brain creation
- `test_connectivity_matrix`: Test connectivity matrix
- `test_integration`: Test full integration scenario
```

## Performance Considerations

1. **Batch Operations**: Use batch methods for multiple brains to improve efficiency
2. **Memory Management**: The bindings handle reference counting automatically
3. **Progress Tracking**: Use progress callbacks for long-running operations
4. **Configuration**: Use nested keys for organized configuration management

## Migration Guide

### From Old to New API

**Old:**
```python
brain = nlm.createBrain(config)
brain.initialize()

# Manual step
brain.step(step)

# Manual statistics
synapse_count = brain.getTotalSynapseCount()
```

**New:**
```python
brain = nlm.createBrain(config)
brain.initialize()

# Same manual step (backward compatible)
brain.step(step)

# Using new convenience methods
stats = nlm.brain.getNetworkStats()
print(f"Synapses: {stats.total_synapses}")

# Or using convenience methods
brain.stepMultiple([brain], [step])
```

## License

This enhanced Python bindings are provided under the MIT License.

## Dependencies

- Python 3.8+
- NLM C++ framework
- pybind11
- NumPy (optional, for advanced operations)

## Build Instructions

To build these enhanced bindings:

1. Ensure you have CMake 3.16+, GCC/Clang, and Python development headers
2. Build the NLM C++ framework
3. Build the Python bindings using the provided CMakeLists.txt

The enhanced bindings automatically extend the existing API without breaking backward compatibility.