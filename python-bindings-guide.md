# NLM Python Bindings - User Guide

This guide provides comprehensive documentation for using the NLM (Neural Learning Machine) Python bindings.

## Overview

The NLM Python bindings provide access to a sophisticated artificial neural system that implements:

- **Spiking neural computation** with Leaky Integrate-and-Fire (LIF) neurons
- **Plasticity mechanisms** including STDP, Hebbian learning, and structural plasticity
- **Memory systems** for working memory, episodic memory, and associative memory
- **Prediction systems** for sensory prediction and error computation
- **Neuromodulation** with dopamine, curiosity, novelty, and prediction error signals
- **Development stages** affecting neural plasticity and behavior

## Quick Start

### Installation

```bash
# The bindings are built as part of the NLM project
# After building the project, the Python module will be available
pip install ./pynlm  # or use the package manager of your choice
```

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain
brain.initialize()

# Create a simple world
world = pynlm.createSimpleWorld()
world.configure(800, 600, 160, 120)  # width, height, vision_width, vision_height

# Create an agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("NLM simulation started!")
```

## Configuration System

### Creating Configurations

```python
import pynlm

# Default configuration
config = pynlm.createDefaultConfig()

# Configuration with custom values
config = pynlm.create_config_with_values({
    'random_seed': 42,
    'simulation_timestep': 0.001,
    'neuron_count': 200,
    'region_count': 2,
    'connection_probability': 0.15
})

# Validate configuration
warnings = pynlm.validate_configuration(config)
if warnings:
    print("Configuration warnings:")
    for warning in warnings:
        print(f"  - {warning}")
```

### Getting Configuration Values

```python
import pynlm

config = pynlm.create_config_with_values({
    'random_seed': 42,
    'simulation_timestep': 0.001,
    'neuron_count': 500,
    'max_energy': 100.0
})

# Safe value retrieval with defaults
seed = config.get_int('random_seed', 0)
timestep = config.get_float('simulation_timestep', 0.001)
neurons = config.get_int('neuron_count', 100)
max_energy = config.get_float('max_energy', 0.0)
energy_enabled = config.get_bool('enable_energy_decay', False)
```

## Brain Simulation

### Basic Simulation Loop

```python
import pynlm
import time

# Create brain with template
brain = pynlm.create_brain_from_template("simple")

print("Starting simulation...")
print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")

# Run simulation for specified steps
steps_to_run = 1000
timestep = 0.001

for step in range(steps_to_run):
    # Create and inject sensory input
    vision = pynlm.Vision(80, 60, 3)
    # In real usage, you'd set actual vision data here
    # vision.set_data(some_data)
    
    brain.receiveSensoryInput(vision)
    
    # Perform simulation step
    brain.step(step, step * timestep)
    
    # Log progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: Spikes={brain.getTotalSpikeCount()}, "
              f"Firing={brain.getFiringNeuronCount()}/{brain.getTotalNeuronCount()}")

print("Simulation completed!")
```

### Advanced Simulation with World Integration

```python
import pynlm

# Create complete simulation setup
brain, world = pynlm.setup_demo_simulation()

# Create agent
agent = pynlm.create_agent_simulation(brain, world, "explorer")

print("Demo simulation setup complete!")
print(f"World size: {world.getWidth()}x{world.getHeight()}")
print(f"Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")

# Get neural statistics
stats = pynlm.compute_neural_stats(brain)
print("Initial statistics:", stats)
```

## Sensory and Motor Systems

### Vision Processing

```python
import pynlm
import random

# Create vision sensor
vision = pynlm.Vision(160, 120, 3)  # width, height, channels

# Generate random vision data (simulating a visual scene)
vision_data = []
for _ in range(160 * 120 * 3):
    vision_data.append(random.uniform(0, 1))

vision.setData(vision_data)

print(f"Vision sensor created: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
```

### Audio Processing

```python
import pynlm
import random

# Create audio sensor
audio = pynlm.Audio(sample_rate=44100, num_samples=22050)  # 0.5 seconds at 44.1kHz

# Generate random audio data
audio_data = [random.uniform(-1, 1) for _ in range(22050)]
audio.setData(audio_data)

print(f"Audio sensor created: {audio.getSampleRate()}Hz, {audio.getNumSamples()} samples")
print(f"Duration: {audio.getDuration():.2f} seconds")
```

### Motor Actions

```python
import pynlm

# Create different types of actions
actions = []

# Movement actions
actions.append(pynlm.Action(pynlm.ActionType.MoveForward))
actions.append(pynlm.Action(pynlm.ActionType.TurnLeft, [0.5]))
actions.append(pynlm.Action(pynlm.ActionType.TurnRight, [-0.3]))

# Interaction actions
actions.append(pynlm.Action(pynlm.ActionType.Interact))
actions.append(pynlm.Action(pynlm.ActionType.Eat, [0.8]))

# Wait action
actions.append(pynlm.Action(pynlm.ActionType.Wait))

print("Available actions:")
for i, action in enumerate(actions):
    print(f"  {i}: {action.getName()} - Type: {action.getType()}")
```

## Advanced Features

### Debug Monitoring

```python
import pynlm

# Create a debug monitor
brain = pynlm.create_brain_from_template("simple")
debug_monitor = pynlm.create_debug_monitor(brain)

print("Debug monitor created for neural activity tracking")

# Run simulation with debug monitoring
for step in range(100):
    # Inject some current to generate activity
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
    
    brain.step(step, step * 0.001)
    debug_monitor.log_step()

# Get comprehensive summary
summary = debug_monitor.get_summary()
print("\nDebug Summary:")
print(f"Steps simulated: {summary['step_count']}")
print(f"Total neurons: {summary['total_neurons']}")
print(f"Total synapses: {summary['total_synapses']}")
print(f"Average firing rate: {summary['avg_firing_rate']:.4f}")
```

### Visualization

```python
import pynlm

# Create brain and world
brain, world = pynlm.setup_demo_simulation()
visualizer = pynlm.create_visualizer(brain)

# Print network summary
visualizer.print_network_summary()
visualizer.print_plasticity_summary()
visualizer.print_development_summary()
visualizer.print_neuromodulation_summary()
visualizer.print_memory_summary()

# Export visualization data
visualizer.export_visualization_data()
```

### Performance Profiling

```python
import pynlm

# Create profiler
profiler = pynlm.create_profiler()

# Perform simulation with profiling
brain = pynlm.create_brain_from_template("simple")

profiler.log_event("brain_initialized")

for step in range(50):
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0)
    brain.step(step, step * 0.001)
    
    if step % 10 == 0:
        profiler.log_event(f"step_completed_{step}")
        profiler.increment_step()

# Generate performance report
report = profiler.get_profile_report()
print("\nPerformance Report:")
print(f"Total events: {report['total_events']}")
print(f"Total steps: {report['total_steps']}")
print(f"Total time: {report['total_time_seconds']:.2f} seconds")

# Generate detailed analysis
profiler.generate_performance_analysis()
```

### State Management

```python
import pynlm
import json
import os

# Create state saver for checkpointing
state_saver = pynlm.create_state_saver("brain_checkpoint.json")

# Create brain and run some simulation
brain = pynlm.create_brain_from_template("dense")
world = pynlm.createSimpleWorld()

# Simulate some steps
for step in range(200):
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 8.0)
    brain.step(step, step * 0.001)
    
    # Auto-save every 50 steps (if enabled)
    if state_saver.should_auto_save():
        state = pynlm.compute_neural_stats(brain)
        state_saver.save_state(state)

# Manual save
current_state = pynlm.compute_neural_stats(brain)
state_saver.save_state(current_state)

# Later, load the saved state
loaded_state = state_saver.load_state()
print("Loaded state with keys:", list(loaded_state.keys()))
```

### Error Handling

```python
import pynlm

# Safe function execution with error handling
def safe_simulation(brain, steps, timestep):
    try:
        for step in range(steps):
            brain.step(step, step * timestep)
            print(f"Step {step} completed")
    except pynlm.NLMError as e:
        print(f"NLM error occurred: {e}")
    except Exception as e:
        print(f"Unexpected error: {e}")
        return False
    
    return True

# Use safe execution
brain = pynlm.create_brain_from_template("simple")
success = pynlm.safe_execute(safe_simulation, brain, 100, 0.001)
print(f"Simulation {'succeeded' if success else 'failed'}")
```

## Neural Network Analysis

### Network Statistics

```python
import pynlm

brain = pynlm.create_brain_from_template("sparse")

# Get comprehensive neural statistics
stats = pynlm.compute_neural_stats(brain)

print("=== Neural Network Statistics ===")
for key, value in stats.items():
    if isinstance(value, float):
        print(f"{key}: {value:.4f}")
    else:
        print(f"{key}: {value}")
```

### Data Processing Utilities

```python
import pynlm

# Example data processing
neurons_data = pynlm.serialize_neuron(some_neuron)
print("Serialized neuron data:", neurons_data)

# Convert data for JSON serialization
json_data = pynlm.convert_to_json({
    'neurons': [neurons_data],
    'metadata': {'simulation_id': pynlm.generate_unique_id()}
})

print("JSON-serializable data created")
```

## Best Practices

### Configuration Management

1. **Use templates for common configurations**:
   ```python
   brain = pynlm.create_brain_from_template("simple")  # Fast for testing
   brain = pynlm.create_brain_from_template("dense")   # For complex simulations
   ```

2. **Always validate configurations**:
   ```python
   warnings = pynlm.validate_configuration(config)
   for warning in warnings:
       print(f"WARNING: {warning}")
   ```

3. **Use safe value retrieval**:
   ```python
   value = config.get_int('key', default_value)
   value = config.get_float('key', default_value)
   value = config.get_string('key', default_value)
   value = config.get_bool('key', default_value)
   ```

### Simulation Management

1. **Implement progress tracking**:
   ```python
   progress_tracker = pynlm.progress_tracker("Simulation", total_steps)
   for step in range(total_steps):
       progress_tracker(step)
       # Your simulation code here
   ```

2. **Use batch operations** for multiple brains:
   ```python
   brains = [brain1, brain2, brain3]
   pynlm.batch_synchronize(brains, timestep)
   ```

3. **Monitor performance**:
   ```python
   profiler = pynlm.create_profiler()
   # Your simulation code
   report = profiler.get_profile_report()
   ```

4. **Implement checkpointing**:
   ```python
   state_saver = pynlm.create_state_saver("checkpoint.json")
   state_saver.set_auto_save(True, interval=60.0)  # Auto-save every 60 seconds
   ```

### Memory and Performance

1. **Use efficient data structures**:
   ```python
   # Process large datasets efficiently
   weights = pynlm.compute_weighted_average(values, weights)
   ```

2. **Implement round-robin data distribution**:
   ```python
   grouped_data = pynlm.round_robin_distribution(items, num_groups)
   ```

3. **Use safe division to avoid division by zero**:
   ```python
   result = pynlm.safe_divide(numerator, denominator, default_value=0.0)
   ```

## Examples Directory

The `examples/` directory contains comprehensive examples covering:

1. **Basic Usage**: Getting started with the NLM bindings
2. **Configuration Management**: Creating and managing brain configurations
3. **Simulation Control**: Running and monitoring neural simulations
4. **Sensory Processing**: Working with vision and audio sensors
5. **Motor Control**: Creating and managing actions
6. **Advanced Features**: Using debug monitors, visualizers, and profilers
7. **Data Processing**: Serialization, filtering, and analysis
8. **Error Handling**: Robust error handling and recovery
9. **Performance Optimization**: Profiling and benchmarking

To run examples:
```bash
python examples/basic_usage.py
python examples/configuration_management.py
python examples/advanced_features.py
```

## API Reference

For detailed API documentation, see:
- `pynlm.Brain` - Main neural simulation class
- `pynlm.Config` - Configuration management
- `pynlm.Vision` - Visual sensor
- `pynlm.Audio` - Audio sensor
- `pynlm.Action` - Motor action
- `pynlm.SimpleWorld` - Simulation environment
- `pynlm.AgentBrain` - Agent interface

## Troubleshooting

### Common Issues

1. **Configuration errors**: Always validate configurations before use
2. **Memory issues**: Monitor performance and implement checkpointing
3. **Timing issues**: Use consistent timesteps throughout simulation
4. **Type errors**: Use safe type conversion functions

### Debugging Tips

1. Use debug monitors to track neural activity
2. Enable verbose logging for detailed information
3. Use visualization tools to understand network structure
4. Profile performance to identify bottlenecks

### Getting Help

- Check the examples directory for common use cases
- Use debug monitors for real-time monitoring
- Profile performance to identify issues
- Consult the API documentation for detailed information

## Version Information

This documentation covers the NLM Python bindings with advanced features including:
- Comprehensive error handling and exception support
- Convenience factory methods and builder patterns
- Pythonic API design with idiomatic patterns
- Utility functions for data conversion and serialization
- Debugging and visualization tools
- Advanced simulation controls and experiment management

The bindings provide access to a sophisticated artificial neural system suitable for research, education, and development of brain-inspired systems.