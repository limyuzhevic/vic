# NLM Enhanced Python API - Comprehensive Examples

This document provides comprehensive examples for using the enhanced NLM Python API. It covers both beginner and advanced usage patterns.

## Quick Start

The enhanced NLM API includes context managers, batch operations, performance monitoring, and advanced configuration tools.

### Example 1: Basic Usage with Context Manager

```python
import pynlm

# Basic usage with automatic resource management
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    for i in range(100):
        brain.step(i)
    
    print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

### Example 2: Agent with World Integration

```python
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 4)

with pynlm.createBrain(config) as brain:
    brain.initialize()
    
    with pynlm.createSimpleWorld() as world:
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run episode
        episode_spikes = agent.run_episode(world, 500)
        print(f"Episode complete: {episode_spikes} spikes")
        
        # Get agent metrics
        metrics = agent.get_agent_metrics()
        print(f"Metrics: {metrics}")
```

## Context Managers

The enhanced API provides automatic resource management through context managers:

### Context Manager Examples

```python
import pynlm

# Brain context manager - automatic initialization and cleanup
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    # Brain automatically cleaned up when exiting context
    print("Brain initialized and will be cleaned up")

# World context manager - automatic world management
with pynlm.createSimpleWorld() as world:
    world.configure(10, 10, 5, 5)
    world.reset()
    print("World configured and will be cleaned up")

# Agent context manager - enhanced agent management
with pynlm.createAgentBrain(brain) as agent:
    agent.initialize(world)
    # Agent automatically reset when exiting context
    print("Agent initialized and will be cleaned up")
```

## Performance Monitoring

The enhanced API includes comprehensive performance monitoring tools:

### Performance Monitoring Examples

```python
import pynlm

# Create performance monitor
monitor = pynlm.createPerformanceMonitor()

# Monitor brain operations
with monitor.measure("brain_init"):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())

with monitor.measure("brain_initialize"):
    brain.initialize()

with monitor.measure("simulation"):
    for i in range(1000):
        brain.step(i)

# Report performance
print(monitor.report())

# Get specific metrics
print(f"Brain init time: {monitor.get_elapsed_time('brain_init'):.3f}s")
print(f"Simulation average step time: {monitor.get_average_time('simulation'):.6f}s")
print(f"Simulation steps: {monitor.get_call_count('simulation')}")
```

## Batch Operations

The enhanced API provides efficient batch operations for improved performance:

### Batch Operation Examples

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Batch neuron current injection
neuron_ids = [pynlm.NeuronId(i) for i in range(100)]
currents = [10.0] * 100  # Inject 10.0 current to all neurons
brain.injectCurrentBatch(neuron_ids, currents)

# Batch simulation steps
efficient_steps = 10000
brain.step_batch(efficient_steps, 0.001)

# Batch region operations
region_ids = brain.getRegionIds()
for region_id in region_ids:
    region = brain.getRegion(region_id)
    neurons = region.getAllNeurons()
    print(f"Region {region_id}: {len(neurons)} neurons")
```

## Configuration Validation

The enhanced API includes comprehensive configuration validation:

### Configuration Validation Examples

```python
import pynlm

# Create and validate configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 4)
config.set("connection_probability", 0.05)

# Validate configuration
if config.isValid():
    print("Configuration is valid")
else:
    errors = config.getValidationErrors()
    print(f"Configuration errors: {errors}")

# Get recommended configurations
recommended = config.getRecommendedConfig()
print(recommended)

# Create test configurations
small_config = pynlm.createTestConfig("small")
medium_config = pynlm.createTestConfig("medium")
large_config = pynlm.createTestConfig("large")
```

## Advanced Features

### Advanced Brain Operations

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get comprehensive brain statistics
stats = brain.getStatistics()
print(f"Brain Statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")

# Check brain health
if brain.isHealthy():
    print("Brain is in a healthy state")
else:
    print("Brain has issues")

# Warm up the brain (run idle steps to stabilize)
brain.warmup(100)

# Get brain state as dictionary
state = brain.getBrainState()
print(f"Brain state: {state}")
```

### Advanced Agent Operations

```python
import pynlm

# Create agent with enhanced capabilities
agent = pynlm.createAgentBrain(brain)

# Run complex episode with multiple steps
with pynlm.createSimpleWorld() as world:
    world.configure(15, 15, 8, 8)
    world.reset()
    agent.initialize(world)
    
    # Run extended episode
    spikes = agent.run_episode(world, 2000)
    print(f"Extended episode: {spikes} spikes")
    
    # Get comprehensive agent metrics
    metrics = agent.getAgentMetrics()
    print(f"Agent Metrics:")
    for key, value in metrics.items():
        print(f"  {key}: {value}")
```

## Error Handling

The enhanced API provides comprehensive error handling with custom exceptions:

### Error Handling Examples

```python
import pynlm

try:
    # Create brain with invalid configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", -100)  # Invalid!
    
    if not config.isValid():
        errors = config.getValidationErrors()
        raise pynlm.NLMConfigError(f"Invalid configuration: {errors}")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
except pynlm.NLMConfigError as e:
    print(f"Configuration error: {e}")
except pynlm.NLMBrainError as e:
    print(f"Brain error: {e}")
except pynlm.NLMWorldError as e:
    print(f"World error: {e}")
except pynlm.NLMPerformanceError as e:
    print(f"Performance error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Neuroscience Analysis Tools

### Neuroscience Analysis Examples

```python
import pynlm

# Create a brain for analysis
brain = pynlm.createBrain(pynlm.createTestConfig("medium"))
brain.initialize()

# Run simulation
brain.step_batch(1000)

# Get spike statistics
spike_count = brain.getTotalSpikeCount()
firing_rate = brain.getAverageFiringRate()
print(f"Spike analysis: {spike_count} total spikes, {firing_rate:.2f} Hz average")

# Get connectivity analysis
neuron_count = brain.getTotalNeuronCount()
synapse_count = brain.getTotalSynapseCount()
connection_ratio = synapse_count / (neuron_count * (neuron_count - 1)) if neuron_count > 1 else 0
print(f"Connectivity: {synapse_count} synapses, {connection_ratio:.6f} connection ratio")

# Get regional analysis
region_ids = brain.getRegionIds()
for region_id in region_ids:
    region = brain.getRegion(region_id)
    neurons = region.getAllNeurons()
    active = sum(1 for neuron in neurons if neuron.isFiring())
    print(f"Region {region_id}: {len(neurons)} neurons, {active} active")
```

## Debugging and Testing Tools

### Debugging Examples

```python
import pynlm

# Debug brain initialization
def debug_brain_creation():
    config = pynlm.createDefaultConfig()
    print("Creating brain...")
    
    brain = pynlm.createBrain(config)
    print("Brain object created")
    
    print("Initializing brain...")
    if not brain.initialize():
        raise pynlm.NLMBrainError("Brain initialization failed")
    
    print("Brain initialized successfully")
    return brain

# Step-by-step debugging
def debug_simulation(brain, steps):
    print(f"Running simulation for {steps} steps...")
    
    for i in range(steps):
        step_start = time.time()
        brain.step(i)
        step_time = time.time() - step_start
        
        if i % 100 == 0:
            print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{step_time:.6f}s")
    
    print("Simulation complete")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")

# Usage
try:
    brain = debug_brain_creation()
    debug_simulation(brain, 500)
except Exception as e:
    print(f"Debug error: {e}")
```

## Performance Optimization

### Performance Optimization Examples

```python
import pynlm
import time

# Performance comparison: single steps vs batch steps
brain = pynlm.createBrain(pynlm.createTestConfig("large"))
brain.initialize()

# Test single steps performance
start_time = time.time()
for i in range(1000):
    brain.step(i)
single_step_time = time.time() - start_time
print(f"Single steps (1000 steps): {single_step_time:.3f}s")

# Reset brain
brain.reset()
brain.initialize()

# Test batch steps performance
start_time = time.time()
brain.step_batch(1000, 0.001)
batch_step_time = time.time() - start_time
print(f"Batch steps (1000 steps): {batch_step_time:.3f}s")
print(f"Speedup: {single_step_time/batch_step_time:.1f}x")

# Performance monitoring with batch operations
monitor = pynlm.createPerformanceMonitor()

with monitor.measure("batch_simulation"):
    brain.reset()
    brain.initialize()
    brain.step_batch(5000, 0.001)

print(monitor.report())
```

## Migration Guide

### From Basic to Enhanced API

If you're migrating from the basic NLM API to the enhanced API, follow these guidelines:

#### Basic to Enhanced Changes

1. **Context Managers**
   - Old: `brain = createBrain(config); brain.initialize(); ... brain.reset()`
   - New: `with createBrain(config) as brain: ...` (automatic management)

2. **Error Handling**
   - Old: Check return values manually
   - New: Use custom exception hierarchy (`NLMBrainError`, `NLMConfigError`, etc.)

3. **Batch Operations**
   - Old: `for i in range(steps): brain.step(i)`
   - New: `brain.step_batch(steps, timestep)`

4. **Configuration Validation**
   - Old: Assume configuration is valid
   - New: `if config.isValid(): ... errors = config.getValidationErrors()`

5. **Performance Monitoring**
   - Old: Manual timing
   - New: `with monitor.measure("operation"): ...`

## Best Practices

### Best Practices for Enhanced API Usage

1. **Always use context managers** for automatic resource management
2. **Validate configurations** before creating objects
3. **Use batch operations** for better performance
4. **Monitor performance** regularly
5. **Implement proper error handling** with try-catch blocks
6. **Use context-specific methods** like `brain.getStatistics()`
7. **Clean up resources** explicitly when not using context managers
8. **Validate inputs** before passing to API methods
9. **Use appropriate scale** configurations for your use case
10. **Implement logging** for debugging and monitoring

### Code Quality Guidelines

```python
# Good practices example
import pynlm

def run_simulation_with_best_practices(config_scale="medium", steps=1000):
    """Run simulation following best practices"""
    
    # 1. Use appropriate configuration
    config = pynlm.createTestConfig(config_scale)
    
    # 2. Validate configuration
    if not config.isValid():
        raise ValueError("Invalid configuration")
    
    # 3. Use context manager for automatic resource management
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        
        # 4. Use performance monitoring
        monitor = pynlm.createPerformanceMonitor()
        
        with monitor.measure("simulation"):
            # 5. Use batch operations for better performance
            brain.step_batch(steps, 0.001)
        
        # 6. Get statistics
        stats = brain.getStatistics()
        print(f"Simulation completed: {stats}")
        
        # 7. Performance report
        print(monitor.report())
        
        return brain.getTotalSpikeCount()

# Usage
spikes = run_simulation_with_best_practices("medium", 2000)
print(f"Simulation completed with {spikes} spikes")
```

## Summary

The enhanced NLM Python API provides:

1. **Automatic Resource Management** - Context managers for automatic cleanup
2. **Comprehensive Error Handling** - Custom exception hierarchy
3. **Batch Operations** - Efficient processing for better performance
4. **Performance Monitoring** - Built-in timing and profiling tools
5. **Configuration Validation** - Comprehensive validation with helpful errors
6. **Advanced Analytics** - Neuroscience-specific analysis tools
7. **Better Debugging** - Step-by-step debugging and monitoring
8. **Performance Optimization** - Efficient processing and monitoring

This comprehensive API makes it easier to use NLM for both beginners and advanced users while providing the tools needed for complex neuroscience simulations.