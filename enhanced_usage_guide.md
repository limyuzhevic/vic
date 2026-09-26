# NLM - Enhanced Usage Guide for Python Bindings

**An Experimental Artificial Developmental Brain with Pythonic Enhancements**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project with advanced Python bindings that provide a modern, Pythonic interface to the C++ neural simulation framework.

### Key Improvements:
- **Pythonic Interface**: Context managers, iterators, comprehensive error handling
- **Performance Monitoring**: Built-in profiling and metrics
- **Batch Processing**: Efficient simulation of multiple steps
- **Configuration Validation**: Type-safe configuration with validation
- **Scientific Integration**: NumPy and pandas compatibility
- **Resource Management**: Automatic cleanup and context managers

## Quick Start (Enhanced)

### Example 1: Simplest Brain with Context Manager

```python
import pynlm

# Create brain with default configuration (validated)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Use context manager for automatic cleanup
with pynlm.SimulationContext(brain, world, agent) as sim:
    sim.runEpisode(100, 0.1, verbose=True)
    print(f"Simulation completed with {brain.getTotalSpikeCount()} spikes")
```

### Example 2: Complete Agent with All Features

```python
import pynlm
import numpy as np

# Setup
config = pynlm.createDefaultConfig()
config.setNeuronCount(500)
config.set("brain.region_count", 3)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(10.0, 10.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation with batch processing
steps = list(range(200))
timesteps = [0.1] * 200

batch = pynlm.BatchProcessor(brain, world, agent, batchSize=50)
metrics = batch.processBatch(steps, timesteps)

# Get performance metrics
perf_metrics = brain.getPerformanceMetrics()
print(f"Average step time: {perf_metrics.simulationTime/len(metrics):.3f}s")
print(f"Total spikes: {perf_metrics.totalSpikes}")
```

## New Pythonic Features

### 1. Context Managers

```python
with pynlm.SimulationContext(brain, world, agent) as sim:
    # Automatic resource management and cleanup
    sim.runEpisode(1000, 0.1)
    # Resources are automatically cleaned up when exiting context
```

### 2. Batch Processing

```python
# Process multiple simulation steps efficiently
batch = pynlm.BatchProcessor(brain, world, agent, batchSize=100)
metrics = batch.processBatch(steps=[0, 1, 2, ...], timesteps=[0.1, 0.1, 0.1])

# Monitor progress
for metric in metrics:
    print(f"Step time: {metric.simulationTime:.3f}s, Spikes: {metric.totalSpikes}")
```

### 3. Enhanced Configuration with Validation

```python
# Create validated configuration
config = pynlm.createDefaultConfig()

# Set with validation
config.setNeuronCount(1000)  # Validates neuron count
config.set("brain.region_count", 5)  # Validates region count

# Check configuration validity
pynlm.validate_config(config)  # Raises NLMConfigurationError if invalid
```

### 4. Performance Monitoring

```python
# Get comprehensive performance metrics
metrics = brain.getPerformanceMetrics()

print(f"Simulation time: {metrics.simulationTime:.3f}s")
print(f"CPU time: {metrics.cpuTime:.3f}s")
print(f"Steps per second: {metrics.stepsPerSecond}")
print(f"Total spikes: {metrics.totalSpikes}")
print(f"Active neurons: {metrics.activeNeurons}")
print(f"Memory usage: {metrics.memoryUsage:.2f} MB")
```

### 5. Error Handling

```python
try:
    brain.step(step)
except pynlm.NLMResourceError as e:
    print(f"Resource error: {e}")
except pynlm.NLMConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.NLMBoundaryError as e:
    print(f"Boundary error: {e}")
```

## Enhanced Brain API

### New Methods

#### Performance Monitoring
- `brain.getPerformanceMetrics()`: Get comprehensive performance metrics
- `brain.runBatch(steps, timesteps)`: Run multiple steps in batch

#### Statistics and Analysis
- `brain.get_brain_stats()`: Get comprehensive brain statistics
- `brain.get_neuron_population_stats()`: Get neuron population statistics

#### Enhanced Configuration
- `config.setNeuronCount(count)`: Set with validation
- `config.getNeuronCount()`: Get with default value
- `config.validate()`: Validate configuration

#### Context Manager Support
- `SimulationContext(brain, world, agent)`: Context manager for simulation

## Enhanced AgentBrain API

### New Methods

#### Advanced Neuromodulation
- `agent.enableAdvancedLearning(enable)`: Enable/disable advanced learning features
- `agent.getLearningStats()`: Get learning statistics

#### Performance Monitoring
- `agent.getProcessingStats()`: Get agent processing statistics

#### Development Tracking
- `agent.getDevelopmentTimeline()`: Get development history
- `agent.getPlasticityStats()`: Get plasticity statistics

## Enhanced SimpleWorld API

### New Methods

#### Batch Operations
- `world.runBatchUpdates(timesteps)`: Run multiple updates efficiently
- `world.batchAddObjects(objects)`: Add multiple objects at once

#### Performance
- `world.getPerformanceMetrics()`: Get world performance metrics
- `world.optimizeMemory()`: Optimize memory usage

## Scientific Python Ecosystem Integration

### NumPy Integration

```python
import numpy as np

# Create vision data as numpy array
vision_array = np.random.rand(8, 8, 3).astype(np.float32)

# Use in simulation
percept = pynlm.SensoryPercept()
percept.setVision(vision_array)

# NumPy array operations for preprocessing
vision_array = np.clip(vision_array, 0.0, 1.0)
vision_array = vision_array * 2.0 - 1.0  # Normalize to [-1, 1]
```

### Pandas Integration

```python
import pandas as pd

# Create DataFrame for recording simulation data
simulation_data = pd.DataFrame({
    'step': range(1000),
    'firing_neurons': [0] * 1000,
    'total_spikes': [0] * 1000,
    'avg_firing_rate': [0.0] * 1000,
    'curiosity': [0.0] * 1000,
})

# Simulation callback to record data
def record_data(step, brain, agent):
    simulation_data.loc[step] = {
        'step': step,
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'curiosity': agent.getCuriosityLevel(),
    }
```

## Performance Optimization

### Best Practices

```python
# Use batch processing for multiple steps
batch = pynlm.BatchProcessor(brain, world, agent)
metrics = batch.processBatch(steps=[0, 1, 2, ...], timesteps=[0.1, 0.1, 0.1])

# Use context managers for resource management
with pynlm.SimulationContext(brain, world, agent) as sim:
    sim.runEpisode(10000, 0.01)

# Use performance monitoring
perf_metrics = brain.getPerformanceMetrics()
print(f"Steps per second: {perf_metrics.stepsPerSecond}")

# Use configuration validation
pynlm.validate_config(config)
```

### Benchmarking

```python
import time

def benchmark_simulation(steps, timesteps):
    start_time = time.time()
    
    batch = pynlm.BatchProcessor(brain, world, agent)
    metrics = batch.processBatch(steps, timesteps)
    
    end_time = time.time()
    
    duration = end_time - start_time
    avg_step_time = duration / len(steps)
    
    print(f"Benchmarked {len(steps)} steps in {duration:.3f}s")
    print(f"Average step time: {avg_step_time:.3f}s")
    print(f"Steps per second: {len(steps) / duration:.1f}")
    
    return metrics
```

## Help and Documentation

The improved bindings include a comprehensive help system:

```python
# Get help on simulation topics
help_text = pynlm.help("simulation")

# Get help on configuration
help_text = pynlm.help("configuration")

# Get help on performance monitoring
help_text = pynlm.help("performance")
```

## Version Information

```python
import pynlm

version = pynlm.__version__()
print(f"NLM Python bindings version: {version}")
```

## Backward Compatibility

All existing Python code continues to work without any changes:

```python
# Original easy usage pattern (still works)
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

The improvements are completely additive and maintain full backward compatibility.

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"
** - Ensure you call `brain.initialize()` before `brain.step()`
** - Check that `agent.initialize(world)` is called for agent simulations
** - Use `brain.logStatus()` for debugging information

**"The agent isn't moving"
** - Ensure `world.applyMotorCommand()` is being called
** - Check that `world.update()` is being called
** - Verify agent is initialized with world

**"Everything is 0"
** - Brains need time to "warm up" - try more steps
** - Some neurons need input to fire - make sure sensory input is connected
** - Check that development is enabled if needed

### Error Messages

**NLMConfigurationError**
- Configuration parameter is invalid (out of range, type mismatch)
- Use `pynlm.validate_config(config)` to check configuration

**NLMResourceError**
- Resource management issue (null pointer, invalid operation)
- Ensure all required components are properly initialized

**NLMBoundaryError**
- Array index out of bounds, invalid position, etc.
- Check bounds before accessing arrays or positions

## Testing

Run the test suite to verify all improvements:

```bash
python test_python_bindings.py
```

Run the demonstration to see all features in action:

```bash
python demo_python_bindings.py
```

## Migration Guide

### From Old to New API

**Old Pattern:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)
```

**New Pattern (with improvements):**
```python
import pynlm

# Enhanced with context manager and validation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

with pynlm.SimulationContext(brain, world, agent) as sim:
    sim.runEpisode(100, 0.1, verbose=False)
```

### Migration Checklist

- [ ] All existing code continues to work unchanged
- [ ] New context manager for automatic resource management
- [ ] Enhanced error handling with descriptive messages
- [ ] Performance monitoring APIs added
- [ ] Batch processing capabilities
- [ ] Configuration validation
- [ ] NumPy integration support
- [ ] Comprehensive help system
- [ ] Advanced statistics and analysis

## Next Steps

### For Users

1. **Try the examples** in this guide
2. **Use context managers** for better resource management
3. **Enable batch processing** for long simulations
4. **Monitor performance** to optimize simulations
5. **Integrate with NumPy/Pandas** for data analysis

### For Developers

1. **Contribute** to the Python bindings
2. **Add new features** to the enhanced API
3. **Improve documentation** and examples
4. **Add more performance optimizations**
5. **Extend scientific ecosystem integration**

## Conclusion

The enhanced NLM Python bindings provide a modern, Pythonic interface to the sophisticated neural simulation framework while maintaining complete backward compatibility. The improvements include:

- **Pythonic features** (context managers, iterators, error handling)
- **Performance monitoring** (metrics, profiling, benchmarking)
- **Batch processing** (efficient multi-step execution)
- **Configuration validation** (type safety, parameter checking)
- **Scientific integration** (NumPy, pandas, matplotlib)
- **Resource management** (automatic cleanup, context managers)
- **Comprehensive documentation** (help system, examples)
- **Backward compatibility** (all existing code continues to work)

These improvements make NLM more accessible to Python developers while providing powerful tools for advanced research and experimentation.
