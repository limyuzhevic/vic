# Advanced NLM Python API Documentation

This document covers the advanced Python API extensions for NLM, designed for expert users who need more control, performance monitoring, and batch processing capabilities.

## Overview

The advanced Python API extends the basic NLM bindings with:

1. **Batch Processing**: Execute multiple simulation steps efficiently
2. **Performance Monitoring**: Real-time metrics collection and profiling
3. **Advanced Configuration Management**: Optimize configurations for specific use cases
4. **Batch Checkpoint Operations**: Multiple save/load operations
5. **Memory Profiling**: Analyze memory usage patterns
6. **Enhanced Agent Creation**: Create agents with pre-configured subsystems
7. **Advanced Simulation Utilities**: Complex experiment setup and episode management

## Module: pynlm_advanced

The advanced API is available through the `pynlm_advanced` module, which provides enhanced bindings for expert users.

### Installation

```bash
pip install pynlm_advanced
```

## Core Classes and Functions

### 1. AdvancedBrainOperations

#### Batch Processing Functions

**`batch_step(brain, steps, log_interval=100)`**
- Executes multiple simulation steps efficiently
- Provides periodic logging for monitoring progress
- Optimized for large-scale simulations

**`batch_step_with_time(brain, steps, timestep, log_interval=100)`**
- Executes multiple steps with explicit time tracking
- Useful for time-synchronized simulations
- More detailed logging with timestamps

**`batch_process_sequence(brain, inputs, expected_actions=[])`**
- Process a sequence of sensory inputs and actions
- Ensures synchronized execution of inputs and outputs
- Can include validation for expected actions

### 2. PerformanceMonitor

The `PerformanceMonitor` class provides comprehensive performance tracking.

#### Usage Example

```python
import pynlm_advanced

# Start monitoring
pynlm_advanced.start_performance_monitoring()

# Run simulation
for step in range(1000):
    brain.step(step)
    pynlm_advanced.record_step(brain)

# Stop monitoring and get metrics
metrics = pynlm_advanced.stop_performance_monitoring()

# Log detailed metrics
pynlm_advanced.log_performance_metrics(metrics, "My Simulation")

print(f"Simulation completed in {metrics.total_simulation_time:.2f}s")
print(f"Average firing rate: {metrics.average_firing_rate:.2f} Hz")
print(f"Total spikes generated: {metrics.total_spikes_generated}")
```

#### Performance Metrics

- `total_simulation_time`: Total time elapsed in seconds
- `total_steps_executed`: Number of simulation steps processed
- `total_spikes_generated`: Total spikes across all neurons
- `average_firing_rate`: Average firing rate in Hz
- `memory_usage`: Memory consumption in MB
- `cpu_usage`: CPU utilization percentage
- `checkpoint_count`: Number of checkpoints created

### 3. AdvancedConfig

#### Optimization Functions

**`optimize_for_performance(config)`**
- Optimizes configuration for maximum simulation performance
- Increases neuron count, connection density, and learning rates
- Enables all neuromodulation systems

**`optimize_for_learning(config)`**
- Optimizes configuration for maximum learning capability
- Higher plasticity parameters and neuromodulation strength
- More neurons and connections for complex learning

**`validate_safety_constraints(config)`**
- Validates configuration against predefined safety limits
- Automatically adjusts values that exceed safe thresholds
- Logs warnings for adjustments made

**`get_optimization_recommendations(config)`**
- Returns a string with optimization suggestions
- Analyzes current configuration against best practices
- Suggests specific parameter adjustments

#### Usage Example

```python
import pynlm
import pynlm_advanced

# Create base configuration
config = pynlm.createDefaultConfig()

# Optimize for performance
perf_config = pynlm_advanced.optimize_for_performance(config)

# Validate safety constraints
safe_config = pynlm_advanced.validate_safety_constraints(perf_config)

# Get recommendations
recommendations = pynlm_advanced.get_optimization_recommendations(safe_config)
print(recommendations)
```

### 4. BatchCheckpointManager

#### Batch Operations

**`batch_save_checkpoints(brain, filepaths)`**
- Save the brain state to multiple files simultaneously
- Returns success status for each operation
- Useful for creating checkpoint sequences

**`batch_load_checkpoints(brain, filepaths)`**
- Load brain states from multiple files
- Can restore different simulation states
- Returns success status for each operation

**`create_checkpoint_names(prefix, start_index, count)`**
- Generate a series of checkpoint filenames
- Ensures unique, sequential names
- Useful for creating systematic checkpoint sequences

#### Usage Example

```python
import pynlm_advanced

# Create checkpoint names
checkpoint_names = pynlm_advanced.create_checkpoint_names(
    "simulation_checkpoints", 1000, 50
)
print(f"Created {len(checkpoint_names)} checkpoint names")

# Save multiple checkpoints
brain = pynlm.createBrain(config)
for i in range(50):
    brain.step(i)
success = pynlm_advanced.batch_save_checkpoints(
    brain, checkpoint_names
)
print(f"Batch save completed with {sum(success)}/{len(success)} successes")
```

### 5. MemoryProfiler

#### Memory Analysis

**`profile_memory_usage(brain)`**
- Analyzes memory usage of the brain and its components
- Breaks down usage by neurons, synapses, and overhead
- Calculates memory fragmentation ratio

#### Usage Example

```python
import pynlm_advanced

memory_stats = pynlm_advanced.profile_memory_usage(brain)
print(f"Total memory usage: {memory_stats.totalMemoryUsage} bytes")
print(f"Neurons: {memory_stats.allocatedNeurons}")
print(f"Synapses: {memory_stats.allocatedSynapses}")
print(f"Fragmentation ratio: {memory_stats.fragmentationRatio:.2%}")
```

### 6. AdvancedAgentFactory

#### Enhanced Agent Creation

**`create_advanced_agent(brain, world, enable_reward_modulation=True, 
                       enable_curiosity=True, enable_development=True,
                       enable_structural_plasticity=True)`**

Creates an advanced agent with pre-configured subsystems.

#### Usage Example

```python
import pynlm
import pynlm_advanced

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create advanced agent with all subsystems enabled
agent = pynlm_advanced.create_advanced_agent(
    brain, world,
    enable_reward_modulation=True,
    enable_curiosity=True,
    enable_development=True,
    enable_structural_plasticity=True
)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### 7. ExperimentEnvironmentFactory

#### Complex Environment Creation

**`create_experiment_environment(num_agents=5, world_size=20, type="simple")`**

Creates a multi-agent experimental environment with configurable parameters.

#### Usage Example

```python
import pynlm_advanced

# Create complex experiment environment
world, agents = pynlm_advanced.create_experiment_environment(
    num_agents=10,
    world_size=50,
    type="complex"
)

print(f"Created environment with {len(agents)} agents")
print(f"World size: {world.getWidth()}x{world.getHeight()}")

# Setup agents for competition or cooperation
for i, agent in enumerate(agents):
    agent.initialize(world)
    # Configure each agent differently for interesting experiments
    if i % 2 == 0:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
    else:
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
```

### 8. Episode Simulation Utility

#### Complete Episode Management

**`simulate_episode(brain, world, agent, max_steps=1000, verbose=False)`**

Runs a complete simulation episode with comprehensive logging and statistics.

#### Usage Example

```python
import pynlm
import pynlm_advanced

# Setup simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run episode with monitoring
brain, world, agent = pynlm_advanced.simulate_episode(
    brain, world, agent,
    max_steps=500,
    verbose=True
)

# Access updated brain and collect statistics
print(f"Episode completed with {brain.getTotalSpikeCount()} total spikes")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

## Advanced Configuration Examples

### High-Performance Configuration

```python
import pynlm
import pynlm_advanced

# Base configuration
config = pynlm.createDefaultConfig()

# Optimize for performance
perf_config = pynlm_advanced.optimize_for_performance(config)

# Create brain with optimized config
brain = pynlm.createBrain(perf_config)
brain.initialize()

# Run batch processing
pynlm_advanced.batch_step(brain, 10000, log_interval=1000)
```

### Learning-Optimized Configuration

```python
import pynlm
import pynlm_advanced

# Base configuration
config = pynlm.createDefaultConfig()

# Optimize for learning
learning_config = pynlm_advanced.optimize_for_learning(config)

# Create advanced agent with learning config
agent = pynlm_advanced.create_advanced_agent(
    pynlm.createBrain(learning_config),
    pynlm.createSimpleWorld(),
    enable_reward_modulation=True,
    enable_curiosity=True,
    enable_development=True,
    enable_structural_plasticity=True
)

# Run learning simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    agent.getBrain().step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Batch Processing with Checkpoints

```python
import pynlm
import pynlm_advanced

# Create brain and environment
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create checkpoint sequence
checkpoint_names = pynlm_advanced.create_checkpoint_names(
    "experiment_1", 0, 11
)

# Run simulation with periodic checkpoints
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Save checkpoint every 100 steps
    if step % 100 == 0 and step > 0:
        idx = step // 100
        if idx < len(checkpoint_names):
            brain.save(checkpoint_names[idx])

# Batch load checkpoints for analysis
pynlm_advanced.batch_load_checkpoints(brain, checkpoint_names[::2])
```

## Integration with Basic NLM API

The advanced API seamlessly integrates with the basic NLM Python bindings:

```python
import pynlm
import pynlm_advanced

# Use basic API for simple operations
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Use advanced API for complex operations
pynlm_advanced.batch_step(brain, 100)
pynlm_advanced.start_performance_monitoring()

# Continue with basic API
for step in range(100, 200):
    brain.step(step)
    pynlm_advanced.record_step(brain)

# Get performance metrics
metrics = pynlm_advanced.stop_performance_monitoring()
print(f"Performance: {metrics.average_firing_rate:.2f} Hz")
```

## Performance Considerations

### When to Use Advanced API

1. **Batch Processing**: Use `batch_step` for large numbers of simulation steps
2. **Performance Monitoring**: Use `PerformanceMonitor` for optimization analysis
3. **Memory Management**: Use `MemoryProfiler` for memory-constrained environments
4. **Complex Experiments**: Use `ExperimentEnvironmentFactory` for multi-agent studies
5. **Checkpoint Management**: Use `BatchCheckpointManager` for systematic state management

### Performance Tips

1. **Enable Performance Monitoring Early**: Start monitoring before simulation begins
2. **Use Batch Operations**: Avoid individual step calls in loops
3. **Monitor Memory Usage**: Profile memory during long simulations
4. **Optimize Configuration**: Use `optimize_for_performance` or `optimize_for_learning`
5. **Plan Checkpoint Strategy**: Decide on checkpoint frequency before simulation

## Error Handling and Best Practices

### Common Pitfalls

1. **Forgetting to Start Monitoring**: Always call `start_performance_monitoring()` before simulation
2. **Ignoring Return Values**: Check return values from batch operations for error handling
3. **Memory Leaks**: Profile memory usage during long simulations
4. **Configuration Validation**: Always validate configurations before use

### Best Practices

1. **Always Validate Configurations**: Use `validate_safety_constraints()`
2. **Use Batch Operations for Efficiency**: Prefer batch over individual operations
3. **Monitor Performance Regularly**: Use performance monitoring throughout simulation
4. **Plan Checkpoint Strategy**: Decide on checkpoint frequency and storage
5. **Profile Memory Usage**: Monitor memory during long-running simulations

## Migration Guide

### From Basic to Advanced API

1. **Keep Basic API for Simple Operations**: Use `batch_step` instead of manual loops
2. **Add Performance Monitoring**: Start with basic monitoring, add detailed profiling as needed
3. **Use Advanced Configurations**: Replace manual configuration with optimization functions
4. **Implement Batch Operations**: Use batch checkpoint and processing functions
5. **Add Memory Management**: Include memory profiling for production simulations

### Key Functions to Replace

- Manual loop `for step in range(1000): brain.step(step)` → `pynlm_advanced.batch_step(brain, 1000)`
- Manual performance tracking → `PerformanceMonitor`
- Manual configuration optimization → `AdvancedConfig`
- Manual checkpoint management → `BatchCheckpointManager`
- Memory profiling → `MemoryProfiler`

## Conclusion

The advanced NLM Python API provides powerful tools for expert users who need:

- **High-performance simulation** through batch processing
- **Comprehensive performance monitoring** and analysis
- **Advanced configuration management** for different use cases
- **Systematic checkpoint management** for reproducibility
- **Memory profiling** for optimization
- **Complex experiment setup** with multi-agent environments
- **Complete episode management** with detailed statistics

These tools make NLM suitable for production use, research experiments, and large-scale simulations while maintaining compatibility with the basic NLM API for simpler use cases.