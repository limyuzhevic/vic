# NLM Enhanced Python API Documentation

## Overview

The enhanced Python bindings for NLM (Neural Learning Machine) provide a more Pythonic interface to the C++ neural simulation framework. This documentation covers the new features, classes, and patterns available in the enhanced API.

## New High-Level Convenience Classes

### BrainBuilder

The `BrainBuilder` class provides high-level utilities for creating brain configurations and agent setups.

#### Basic Usage

```python
import pynlm

# Create a default brain
brain = pynlm.BrainBuilder().create_brain()
brain.initialize()

# Configure brain with specific parameters
config = pynlm.BrainBuilder().create_config()
config.set("neuron_count", 2000)
config.set("connection_probability", 0.15)
brain = pynlm.BrainBuilder().create_brain(config)
```

#### Creating Complete Agent Setups

```python
# Create a complete agent setup (brain, agent, world)
brain, agent, world = pynlm.BrainBuilder().create_agent_setup(
    neurons=1000,
    width=30,
    height=30
)

# Configure the world
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.setAgentStart(15.0, 15.0)

# Initialize agent
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

### BrainRunner

The `BrainRunner` class provides utilities for running simulations with progress tracking and batch operations.

#### Basic Usage

```python
runner = pynlm.BrainRunner()

# Run simulation with progress callback
def progress_callback(step, firing_rate):
    print(f"Step {step}: Firing rate = {firing_rate:.2f}")

runner.run_simulation(brain, 1000, progress_callback)

# Run batch simulation with statistics
stats = runner.run_batch(brain, 5000)
print(f"Generated {stats.total_spikes} spikes in {stats.steps} steps")
```

## Enhanced Config API

The configuration system now provides Pythonic property access and validation.

#### Property Access

```python
config = pynlm.Config()

# Pythonic property access
config.set("neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Get values with defaults
neuron_count = config.get_int("neuron_count", 1000)
conn_prob = config.get_float("connection_probability", 0.1)

# Check if key exists
if config.has("brain.neuron_count"):
    keys = config.get_keys()
```

#### Validation

```python
from pynlm import validate_brain_config

# Validate configuration
try:
    validate_brain_config(config)
    print("Configuration is valid!")
except RuntimeError as e:
    print(f"Invalid configuration: {e}")
```

## Enhanced Brain Class

The `Brain` class now includes batch operations and enhanced methods.

#### Batch Operations

```python
# Run multiple steps efficiently
brain.run_batch(1000, timestep=0.001)

# Run batch with statistics
spike_count, firing_neurons, firing_rate = brain.run_batch_with_stats(500)

# Batch current injection
neurons = [neuron_1, neuron_2, neuron_3, ...]
brain.inject_current_batch(neurons, 10.0)
```

#### Enhanced Methods

```python
# Check brain status
brain.log_status()

# Get configuration as dictionary
summary = pynlm.get_simulation_summary(brain)
print(f"Neurons: {summary['neurons']}")
print(f"Total spikes: {summary['total_spikes']}")
```

## Enhanced AgentBrain Class

The `AgentBrain` class includes convenience methods and improved subsystem control.

#### Subsystem Control

```python
# Enable/disable subsystems using string names
agent.enable_subsystem("reward_modulation", True)
agent.enable_subsystem("curiosity", True)
agent.enable_subsystem("development", True)
agent.enable_subsystem("structural_plasticity", False)

# Check subsystem status
if agent.is_subsystem_enabled("curiosity"):
    print("Curiosity is enabled")
```

#### Processing Pipeline

```python
def run_agent_episode(brain, agent, world, steps):
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.get_sensory_percept()
        
        # Process sensory input
        agent.process_sensory_input(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        cmd = agent.decode_motor_command()
        
        # Apply to world
        world.apply_action(cmd, world.get_simulation_time())
        
        # Apply reward modulation if enabled
        if agent.is_subsystem_enabled("reward_modulation"):
            reward = percept.get_internal()[0] if percept.get_internal() else 0.0
            agent.apply_reward_modulation(reward, 0.0)
        
        # Update development
        agent.update_development(0.1)
    
    # Return statistics
    return {
        "total_spikes": brain.get_total_spike_count(),
        "curiosity": agent.get_curiosity_level(),
        "novelty": agent.get_novelty_level(),
        "development_stage": brain.get_developmental_stage()
    }

# Run episode
stats = run_agent_episode(brain, agent, world, 1000)
```

## World Extensions

The `SimpleWorld` class includes convenience methods for common operations.

#### World Management

```python
world = pynlm.SimpleWorld()
world.configure(20, 20, 8, 8)

# Add resources
def add_resource(world, x, y, value=1.0):
    world.add_resource(x, y, value)

# Check valid positions
if world.is_valid_position(5.0, 10.0):
    print("Position is valid")
```

## Helper Functions

### Factory Functions

```python
# Create common setups
brain = pynlm.create_default_brain()
world = pynlm.create_simple_world()
agent = pynlm.create_agent_brain(brain)

# Create complete agent setup
brain, agent, world = pynlm.create_agent_world_setup(
    neurons=2000,
    width=40,
    height=40
)
```

### Example Functions

```python
# Simple brain example
spikes, firing_rate = pynlm.example_simple_brain()
print(f"Brain generated {spikes} spikes with {firing_rate:.2f} avg firing rate")

# Complete agent simulation
spikes, curiosity, novelty = pynlm.example_agent_simulation()
print(f"Simulation: {spikes} spikes, curiosity={curiosity:.2f}, novelty={novelty:.2f}")
```

## Performance Monitoring

```python
# Monitor simulation performance
performance = pynlm.performance_monitor(brain, 1000)
fps = performance[0]  # Frames per second
total_time = performance[1]  # Total time in ms
total_spikes = performance[2]  # Total spikes generated

print(f"Simulation: {fps:.1f} FPS, {total_time} ms for {total_spikes} spikes")
```

## Safe Operations

```python
# Safe stepping with error recovery
try:
    pynlm.safe_step(brain, 100, timestep=0.001)
    print("Step completed successfully")
except Exception as e:
    print(f"Step failed, brain reset: {e}")
```

## Backward Compatibility

The enhanced API maintains full backward compatibility with the original API.

```python
# Original API still works
import pynlm

# Original usage patterns still work
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()
for i in range(100):
    brain.step(i)

# Enhanced API provides additional features
def create_enhanced_setup():
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    agent = pynlm.create_agent_brain(brain)
    world = pynlm.create_simple_world()
    
    # Enhanced features
    agent.enable_subsystem("curiosity", True)
    stats = pynlm.get_simulation_summary(brain)
    
    return brain, agent, world, stats
```

## Error Handling

The enhanced API includes comprehensive error handling and recovery mechanisms.

```python
# Configuration validation
config = pynlm.Config()
try:
    validate_brain_config(config)
    brain = pynlm.Brain(config)
except RuntimeError as e:
    print(f"Configuration error: {e}")

# Safe operations with recovery
try:
    pynlm.safe_step(brain, step)
except Exception as e:
    print(f"Step failed, brain state reset")
    # Continue with recovery
```

## Type Hints and Documentation

The enhanced bindings include comprehensive type hints and documentation for better IDE support.

```python
# Type hints are automatically provided
brain: pynlm.Brain = pynlm.BrainBuilder().create_brain()
agent: pynlm.AgentBrain = pynlm.create_agent_brain(brain)

# Method documentation is available via help()
help(pynlm.BrainBuilder.create_brain)
help(pynlm.BrainRunner.run_batch)
```

## Migration Guide

### From v0.1.0 to v2.0.0

1. **New Convenience Classes**: Replace manual setup with `BrainBuilder.create_agent_setup()`
2. **Enhanced Config API**: Use property access instead of direct method calls
3. **Batch Operations**: Use `brain.run_batch()` instead of manual loops
4. **Subsystem Control**: Use string-based subsystem control instead of individual enable methods
5. **Error Handling**: Use `validate_brain_config()` and `safe_step()` for better error recovery

### Example Migration

```python
# Old way
config = pynlm.create_default_config()
config.set("neuron_count", 1000)
brain = pynlm.create_brain(config)
brain.initialize()
for i in range(100):
    brain.step(i)

# New way
brain = pynlm.BrainBuilder().create_brain()
brain.run_batch(100)

# Or with full control
brain, agent, world = pynlm.BrainBuilder().create_agent_setup(neurons=1000)
stats = pynlm.get_simulation_summary(brain)
```

## Best Practices

1. **Use Factory Functions**: Prefer `BrainBuilder.create_agent_setup()` for common setups
2. **Leverage Batch Operations**: Use `run_batch()` for efficient simulation
3. **Monitor Performance**: Use `performance_monitor()` for optimization
4. **Validate Configurations**: Use `validate_brain_config()` early in development
5. **Enable Learning**: Enable curiosity, reward modulation, and development for realistic behavior
6. **Handle Errors**: Use `safe_step()` for robust simulation loops
7. **Track Progress**: Use progress callbacks in `BrainRunner.run_simulation()`

## Troubleshooting

### Common Issues and Solutions

1. **Brain Not Initialized**
   ```python
   # Always initialize after creation
   brain.initialize()
   ```

2. **Invalid Configuration**
   ```python
   # Use validation before creating brain
   try:
       validate_brain_config(config)
       brain = pynlm.Brain(config)
   except RuntimeError as e:
       print(f"Fix config: {e}")
   ```

3. **Simulation Performance**
   ```python
   # Monitor and optimize
   performance = pynlm.performance_monitor(brain, 1000)
   ```

4. **Subsystem Not Working**
   ```python
   # Check if enabled
   if agent.is_subsystem_enabled("curiosity"):
       # Subsystem should be working
   ```

## Conclusion

The enhanced NLM Python API provides a more Pythonic, user-friendly interface to the neural simulation framework while maintaining full backward compatibility. The new features include:

- High-level convenience classes for quick setup
- Enhanced configuration with property access and validation
- Batch operations for improved performance
- Comprehensive error handling and recovery
- Performance monitoring and optimization tools
- Extensive examples and documentation
- Full backward compatibility with existing code

These enhancements make it easier to use NLM for both research and development while maintaining the power and flexibility of the underlying C++ implementation.
