# Python Binding Improvements - Enhanced API

This document describes the enhanced Python API for NLM, providing better usability, more features, and improved error handling.

## Introduction

The enhanced Python bindings provide a more Pythonic interface to the NLM C++ neural simulation framework, with better error handling, more utilities, and improved documentation.

## Key Improvements

### 1. Enhanced Context Managers

```python
# Traditional way
brain = pynlm.createBrain(config)
brain.initialize()
# ... use brain ...
brain.reset()

# New way with context manager
with pynlm.createBrain(config) as brain:
    # Brain is automatically initialized and cleaned up
    brain.step(0)
    # Automatic cleanup on exit

# With resource management
with pynlm.ResourceManager() as rm:
    rm.create_brain(config)
    rm.initialize()
    # Resources automatically released
```

### 2. Improved Error Handling and Validation

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(100)
except pynlm.NLMRuntimeError as e:
    print(f"Brain error: {e}")
    # Handle specific error types
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
    # Fix configuration and retry
except pynlm.ResourceError as e:
    print(f"Resource error: {e}")
    # Adjust resources and retry

# Validation
try:
    pynlm.validate_config(config)
    print("Configuration is valid")
except pynlm.ValidationError as e:
    print(f"Invalid configuration: {e}")
    # Fix configuration
```

### 3. Enhanced Configuration API

```python
# Factory functions for common configurations
def create_research_config(neuron_count=5000, learning_rate=0.01):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("plasticity.stdp.learning_rate", learning_rate)
    config.set("plasticity.hebbian.learning_rate", learning_rate)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    return config

def create_production_config():
    config = pynlm.createDefaultConfig()
    # Optimized for performance
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.learning_rate", 0.005)
    config.set("plasticity.hebbian.learning_rate", 0.005)
    config.set("performance.checkpoint_interval", 1000)
    return config

# Configuration comparison and diff
def compare_configs(config1, config2):
    diff = pynlm.diff_configs(config1, config2)
    for key, (val1, val2) in diff.items():
        if val1 != val2:
            print(f"Difference: {key} = {val1} vs {val2}")
```

### 4. Advanced Brain Operations

```python
# Batch operations for efficiency
brain = pynlm.createBrain(config)
brain.initialize()

# Run multiple steps efficiently
steps = [i for i in range(1000)]
brain.run_steps(steps)

# Parallel sensory processing
sensory_inputs = [world.getSensoryPercept() for _ in range(100)]
brain.process_sensory_batch(sensory_inputs)

# Memory management
brain.clear_working_memory()
brain.clear_episodic_memory()
brain.compress_memory()
```

### 5. Enhanced Statistics and Monitoring

```python
# Comprehensive statistics
stats = brain.get_comprehensive_stats()
print(f"Neurons: {stats.total_neurons}")
print(f"Synapses: {stats.total_synapses}")
print(f"Firing rate: {stats.firing_rate:.2f} Hz")
print(f"Learning progress: {stats.learning_progress:.2%}")
print(f"Memory usage: {stats.memory_usage:.1f} MB")

# Activity monitoring
activity_monitor = pynlm.ActivityMonitor(brain)
activity_monitor.start_monitoring(duration=1000)
activity_monitor.export_data("activity_data.csv")
```

### 6. Advanced Agent Features

```python
# Enhanced agent with learning management
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Learning control
agent.enable_learning(True)
agent.set_learning_rate(0.01)
agent.set_curriculum([100, 500, 1000])  # Steps per curriculum stage

# Curiosity and exploration
agent.enable_curiosity(True)
agent.set_curiosity_threshold(0.5)
agent.enable_structural_plasticity(True)

# Performance monitoring
performance = agent.get_performance_stats()
print(f"Steps per second: {performance.steps_per_second:.1f}")
print(f"Reward per step: {performance.reward_per_step:.3f}")
print(f"Exploration rate: {performance.exploration_rate:.2%}")
```

### 7. Enhanced Visualization and Export

```python
# Network visualization
network_viz = pynlm.NetworkVisualizer(brain)
network_viz.generate_png("network.png")
network_viz.generate_html("network.html")
network_viz.generate_json("network_data.json")

# Activity plotting
activity_plot = pynlm.ActivityPlotter(brain)
activity_plot.plot_spike_trains("spike_trains.png", neurons=[0, 1, 2])
activity_plot.plot_firing_rates("firing_rates.png", window=100)
activity_plot.plot_weight_changes("weight_changes.png", synapses=[0, 1])

# Export utilities
exporter = pynlm.DataExporter(brain)
exporter.export_neuron_states("neurons.csv")
exporter.export_synapse_states("synapses.csv")
exporter.export_activity_data("activity.npy")
```

### 8. Resource Management and Cleanup

```python
# Resource manager for better cleanup
with pynlm.ResourceManager() as rm:
    # Multiple brains in same context
    brains = []
    for i in range(3):
        brain_config = pynlm.createDefaultConfig()
        brain_config.set("brain.neuron_count", 1000)
        brain = rm.create_brain(brain_config)
        brain.initialize()
        brains.append(brain)
    
    # Use brains...
    
# All brains automatically cleaned up

# Memory management
brain = pynlm.createBrain(config)
brain.initialize()

# Explicit memory management
brain.gc_collect()  # Garbage collection
brain.compress_memory()  # Memory compression
brain.clear_cache()  # Clear internal caches
```

### 9. Advanced Error Recovery

```python
# Error recovery and retry logic
def robust_brain_operation(brain, operation, max_retries=3):
    for attempt in range(max_retries):
        try:
            return operation(brain)
        except pynlm.ResourceError:
            if attempt == max_retries - 1:
                raise
            # Wait and retry
            time.sleep(0.1 * (attempt + 1))
        except pynlm.ConfigurationError:
            # Fix configuration and retry
            fix_configuration(brain)
        except pynlm.RuntimeError:
            # Reset brain and retry
            brain.reset()
            brain.initialize()

# Example usage
result = robust_brain_operation(brain, lambda b: b.run_simulation(1000))
```

### 10. Performance Monitoring and Benchmarking

```python
# Performance benchmarking
benchmarker = pynlm.BenchmarkManager(brain)

# Benchmark different operations
benchmarker.benchmark_step_time(1000)  # Time 1000 steps
benchmarker.benchmark_memory_usage()  # Measure memory usage
benchmarker.benchmark_plasticity()   # Benchmark plasticity operations

# Performance analysis
stats = benchmarker.get_performance_stats()
print(f"Step time: {stats.step_time_avg:.3f} ± {stats.step_time_std:.3f} ms")
print(f"Memory efficiency: {stats.memory_efficiency:.2%}")
print(f"Plasticity throughput: {stats.plasticity_throughput:.0f} ops/sec")
```

## Migration Guide

### From Old to New API

#### Configuration Management
```python
# Old way
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)

# New way
config = pynlm.create_config(neurons=1000, learning_rate=0.01)
```

#### Error Handling
```python
# Old way
try:
    brain.initialize()
except:
    print("Error")

# New way
try:
    brain.initialize()
except pynlm.NLMRuntimeError as e:
    print(f"Brain error: {e}")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
```

#### Resource Management
```python
# Old way
brain = pynlm.createBrain(config)
brain.initialize()
# ... use brain ...
brain.reset()

# New way
with pynlm.createBrain(config) as brain:
    # Brain automatically managed
    # ... use brain ...
# Brain automatically cleaned up
```

## Performance Considerations

### Memory Efficiency
- Use context managers for automatic cleanup
- Explicit memory management with `compress_memory()` and `gc_collect()`
- Batch operations for better memory locality

### Computation Efficiency
- Use batch operations for multiple sensory inputs
- Pre-allocate memory for large simulations
- Use efficient data structures for large networks

### I/O Efficiency
- Batch export operations
- Use appropriate export formats (JSON for small data, CSV for structured data, numpy for numerical data)

## Future Enhancements

### Planned Features
1. **Neural Network Architecture DSL**: Define brain architectures using Python
2. **Transfer Learning**: Load and adapt pre-trained brains
3. **Multi-Agent Coordination**: Synchronize multiple brains
4. **Distributed Computing**: Support for distributed simulations
5. **Advanced Visualization**: Interactive neural network visualization
6. **Real-time Monitoring**: Web-based monitoring dashboard

## Conclusion

The enhanced Python bindings provide a more powerful, user-friendly interface to the NLM framework, with better error handling, more features, and improved usability. The new API maintains backward compatibility while providing significant enhancements for advanced users and researchers.

Key benefits:
- Better error handling and validation
- Enhanced resource management
- More comprehensive statistics and monitoring
- Advanced visualization and export capabilities
- Improved performance optimization tools
- Better documentation and examples

These improvements make NLM more accessible to researchers and developers while providing the power and flexibility needed for advanced neural simulation research.
