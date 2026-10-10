# NLM Python Bindings

Enhanced Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework.

## Overview

This module provides a comprehensive, user-friendly interface to the NLM C++ neural simulation framework. It includes:

- **Enhanced error handling** with custom exception types
- **Factory methods** and convenience functions for quick setup
- **Configuration presets** and environment templates
- **Performance monitoring** and simulation control
- **Advanced utility functions** for data processing and visualization
- **Comprehensive documentation** and type safety
- **Type hints** and parameter validation

## Installation

```bash
pip install pynlm
```

## Quick Start

```python
import nlm

# Create a default configuration
config = nlm.create_default_config()

# Create a brain
brain = nlm.create_brain(config)

# Create a world
world = nlm.create_simple_world()
world.configure(20, 20, 64, 64)
world.setAgentStart(10, 10)

# Create agent brain
agent_brain = nlm.create_agent_brain(brain)

# Run simulation with monitoring
controller = nlm.create_simulation_controller()
monitor = nlm.create_performance_monitor()

for step in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with agent brain
    agent_brain.processSensoryInput(percept)
    action = agent_brain.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, step)
    world.update(0.001)
    
    # Record performance
    neurons_active = brain.getActiveNeuronCount()
    monitor.recordStep(neurons_active, 0, 0.001)

# Get performance report
print(monitor.getPerformanceReport())
```

## Core Classes

### Config
```python
config = nlm.Config()
config.set('brain.simulated_neurons', 1000)
config.set('world.width', 20)

# Save to file
config.saveToFile('config.json')

# Load from file
config2 = nlm.Config()
config2.loadFromFile('config.json')
```

### Brain
```python
brain = nlm.Brain(config)
brain.initialize(world)

# Get statistics
stats = brain.getStatistics()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Active neurons: {stats['active_neurons']}")
```

### AgentBrain
```python
agent_brain = nlm.AgentBrain(brain)
agent_brain.initialize(brain, world)

# Process sensory input
agent_brain.processSensoryInput(percept)
action = agent_brain.decodeMotorCommand()
```

### SimpleWorld
```python
world = nlm.create_simple_world()
world.configure(20, 20, 64, 64)
world.setAgentStart(10, 10)

# Get agent state
body = world.getAgentBody()
print(f"Position: ({body.x}, {body.y})")
print(f"Energy: {body.energy}")
```

### Sensory Inputs
```python
# Vision
vision = nlm.Vision(64, 64, 3)
vision.setData(image_data)

# Audio
audio = nlm.Audio(16000, 512)
audio.setData(audio_samples)

# Internal signals
internal = nlm.InternalSignals()
internal.addSignal(0.5)  # Heart rate
internal.addSignal(0.3)  # Hunger level
```

### Action
```python
# Create actions with factory methods
move_action = nlm.Action.createMoveAction('MoveForward', [1.0, 0.0, 0.0])
look_action = nlm.Action.createLookAction('LookLeft')
rest_action = nlm.Action.createRestAction()

# Custom action
custom_action = nlm.Action.createCustomAction('eat', [0.8, 0.2])
```

## Configuration Presets

### Default Configuration
```python
config = nlm.create_default_config()
# Includes all standard settings
```

### Minimal Configuration
```python
config = nlm.create_minimal_config()
# Essential settings only
```

### High Performance Configuration
```python
config = nlm.create_high_performance_config()
# Optimized for large simulations
```

### World Presets
```python
# Grid world for navigation
config = nlm.create_grid_world_config()

# Maze world for problem-solving
config = nlm.create_maze_world_config()
```

## Enhanced Features

### Performance Monitoring
```python
monitor = nlm.create_performance_monitor()

# Record simulation metrics
monitor.recordStep(neurons_active, spikes_generated, time_delta)

# Get performance report
print(monitor.getPerformanceReport())
```

### Simulation Control
```python
controller = nlm.create_simulation_controller()

# Pause/resume simulation
controller.pause()
controller.resume()

# Reset simulation
controller.reset()

# Check status
print(controller.getStatus())
```

### Batch Processing
```python
batch = nlm.create_batch_processor("simulation_batch")
batch.addOperation(lambda: agent_brain.reset())
batch.addOperation(lambda: world.reset())
batch.execute()  # Execute all operations
```

### Experiment Management
```python
experiment_manager = nlm.create_experiment_manager()
# Add experiments
# Run experiments
# Track results
```

## Utility Functions

### Data Conversion
```python
# Vector to JSON
vector = [0.1, 0.5, 0.9]
json_str = nlm.vector_to_json(vector)

# JSON to vector
restored_vector = nlm.json_to_vector(json_str)

# Object to JSON
obj = nlm.Vision(64, 64)
json_str = nlm.object_to_json(obj)

# JSON to object
restored_obj = nlm.json_to_object(json_str)
```

### Visualization
```python
# Generate neuron activity plot
plot = nlm.generate_neuron_activity_plot(brain)
print(plot)

# Generate connection density plot
density_plot = nlm.generate_connection_density_plot(brain)
print(density_plot)
```

## Exception Types

```python
try:
    brain = nlm.Brain(config)
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except nlm.ValidationError as e:
    print(f"Validation error: {e}")
```

## Performance Monitoring

The performance monitor tracks:

- **Step timing**: Average and last step time
- **Neuron activity**: Average active neurons per step
- **Spike generation**: Average spikes per step
- **Simulation speed**: Steps per second
- **Total statistics**: All metrics over entire simulation

```python
monitor = nlm.create_performance_monitor()
# Record steps
monitor.recordStep(1000, 5000, 0.001)
print(monitor.getPerformanceReport())
```

## Type Safety

All parameters are validated:

- **Range validation**: Numeric values must be within specified ranges
- **String validation**: Non-empty strings for required parameters
- **Type validation**: Correct types for all parameters
- **Data validation**: Valid sensory data sizes and formats

## Performance Optimization

### Batch Operations
```python
batch = nlm.create_batch_processor()
# Multiple operations in single batch
batch.addOperation(operation1)
batch.addOperation(operation2)
batch.execute()
```

### Simulation Control
```python
controller = nlm.create_simulation_controller()
# Pause simulation for analysis
controller.pause()
# Analyze state
# Resume
controller.resume()
```

## Serialization

### Save/Load Brain State
```python
# Save brain state
brain.save('brain_state.json')

# Load brain state
brain2 = nlm.Brain(config)
brain2.load('brain_state.json')
```

### Save/Load Configuration
```python
config.saveToFile('config.json')

config2 = nlm.Config()
config2.loadFromFile('config.json')
```

## Example Applications

### Basic Agent Simulation
```python
import nlm

# Create and configure systems
config = nlm.create_default_config()
world = nlm.create_simple_world()
world.configure(20, 20, 64, 64)
world.setAgentStart(10, 10)

brain = nlm.create_brain(config)
brain.initialize(world)

agent_brain = nlm.create_agent_brain(brain)

# Run simulation with monitoring
controller = nlm.create_simulation_controller()
monitor = nlm.create_performance_monitor()

for step in range(100):
    # Process sensory input
    percept = world.getSensoryPercept()
    agent_brain.processSensoryInput(percept)
    
    # Generate action
    action = agent_brain.decodeMotorCommand()
    world.applyMotorCommand(action, step)
    
    # Update world
    world.update(0.001)
    
    # Record metrics
    neurons_active = brain.getActiveNeuronCount()
    monitor.recordStep(neurons_active, 0, 0.001)

print(monitor.getPerformanceReport())
```

### Advanced Usage with Experiments
```python
import nlm

# Create experiment manager
experiment_manager = nlm.create_experiment_manager()

# Run multiple experiments
for config_preset in [
    nlm.create_default_config(),
    nlm.create_minimal_config(),
    nlm.create_high_performance_config()
]:
    world = nlm.create_simple_world()
    world.configure(20, 20, 64, 64)
    world.setAgentStart(10, 10)
    
    brain = nlm.create_brain(config_preset)
    brain.initialize(world)
    
    agent_brain = nlm.create_agent_brain(brain)
    
    # Run experiment
    experiment_manager.run_experiment(
        "test_experiment",
        brain,
        world,
        50.0  # duration
    )
```

## Performance Tips

### For Large Simulations

1. **Use performance configurations**: `create_high_performance_config()`
2. **Enable batch processing**: `create_batch_processor()` for multiple operations
3. **Monitor performance**: Use `PerformanceMonitor` to track bottlenecks
4. **Control simulation**: Use `SimulationController` to pause/resume for analysis

### For Development

1. **Use minimal configurations**: `create_minimal_config()` for faster iteration
2. **Validate inputs**: All parameters are validated with clear error messages
3. **Use factory functions**: Quick setup with common patterns
4. **Leverage visualization**: `generate_neuron_activity_plot()` and `generate_connection_density_plot()`

## Migration Guide

### From Basic Bindings (v0.1.0)

**New features added:**

1. **Enhanced error handling**: Custom exception types with detailed messages
2. **Factory functions**: `create_default_config()`, `create_brain()`, etc.
3. **Performance monitoring**: `PerformanceMonitor`, `SimulationController`
4. **Batch processing**: `BatchProcessor` for multiple operations
5. **Configuration presets**: `create_grid_world_config()`, etc.
6. **Type safety**: Parameter validation and type checking
7. **Serialization utilities**: JSON conversion functions
8. **Visualization helpers**: Plot generation functions

**Usage examples:**

```python
# Old way (basic)
config = nlm.Config()

# New way (with factory functions)
config = nlm.create_default_config()

# Old way
brain = nlm.Brain(config)

# New way
brain = nlm.create_brain(config)
```

## API Reference

For detailed API documentation, see the docstrings in the source code or use Python's help() function.

```python
help(nlm.Brain)
help(nlm.create_simulation_controller)
help(nlm.vector_to_json)
```

## Troubleshooting

### Common Issues

1. **Import errors**: Ensure all dependencies are installed
2. **Type errors**: Check parameter types and use factory functions
3. **Performance issues**: Use performance configurations and monitoring
4. **Validation errors**: Check error messages for parameter validation failures

### Getting Help

- **Documentation**: See docstrings and examples in this file
- **Help function**: Use Python's help() for API documentation
- **Source code**: All classes are in the Python module directory

## License

MIT License. See LICENSE file for details.

## Author

NLM Team
