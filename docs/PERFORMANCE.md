# NLM Performance and Debugging Tools - Integration Guide

This document provides integration guidance for the new Performance and Debugging Tools added to the NLM (Neural Learning Machine) framework.

## Overview

The Performance and Debugging Tools provide comprehensive monitoring, profiling, and analysis capabilities for NLM brain simulations. These tools help users:

- Monitor simulation performance (FPS/TPS, memory usage)
- Profile execution time of different components
- Debug simulation issues and errors
- Export performance data for analysis
- Optimize brain configurations

## Key Features

### 1. Performance Monitoring

#### Real-time Metrics
- **FPS (Frames Per Second)**: Monitoring simulation display/refresh rate
- **TPS (Ticks Per Second)**: Monitoring simulation update rate
- **Memory Usage**: Real-time memory consumption tracking
- **CPU Usage**: System resource utilization monitoring

#### Components Monitored
- Brain neural computation
- Agent sensory processing
- Motor command execution
- World simulation updates
- Development system updates

### 2. Timer and Profiler

#### High-resolution Timing
- Custom timer creation for specific operations
- Nested timer support for detailed profiling
- Automatic timing of simulation steps
- Component-specific profiling

#### Common Timers
- `brain_step`: Neural computation time
- `sensory_processing`: Input processing time
- `motor_decoding`: Action selection time
- `world_update`: Environment simulation time

### 3. Configuration Management

#### Enhanced Configuration
- Environment variable support
- Configuration validation
- Type-safe parameter setting
- Automatic configuration saving/loading

#### Configuration Categories
- **Brain Parameters**: Neuron count, synapse density, plasticity rules
- **Neuromodulation**: Dopamine, curiosity, novelty settings
- **Development**: Stage progression, plasticity modifiers
- **Performance**: Monitoring intervals, update rates

### 4. Error Handling and Validation

#### Robust Error Checking
- Input validation for all operations
- Configuration parameter validation
- Brain state consistency checking
- Graceful error recovery

#### Error Types Handled
- Configuration errors (invalid parameters)
- Runtime errors (memory issues, null pointers)
- Simulation errors (invalid steps, bad states)
- Resource errors (memory allocation failures)

### 5. Data Export and Analysis

#### Multiple Export Formats
- JSON export for data analysis
- Text export for human-readable reports
- Performance reports with detailed metrics
- Configuration snapshots

#### Export Contents
- Simulation metrics and statistics
- Configuration parameters
- Timer and profiler data
- System information
- Performance trends

## Integration Examples

### Basic Usage

```python
import pynlm

# Create brain with configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent and world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Performance Monitoring Example

```python
import pynlm
import time

# Create brain with monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with performance tracking
start_time = time.time()
for step in range(50):
    brain.step(step)
    # Additional processing...

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Average TPS: {(end_time - start_time) / 50:.2f}")
```

### Configuration Management Example

```python
import pynlm

# Create and customize configuration
config = pynlm.createDefaultConfig()

# Set various parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

# Save configuration
config.saveToFile("my_config.cfg")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.cfg")
```

## Performance Optimization Tips

### 1. Configuration Optimization

#### Recommended Settings for Different Use Cases

**Research/Machine Learning:**
```json
{
    "brain.neuron_count": 5000,
    "brain.synapse_density": 0.2,
    "plasticity.stdp.enable": true,
    "plasticity.hebbian.enable": true,
    "neuromod.curiosity.enable": true,
    "neuromod.novelty.enable": true,
    "development.enabled": true
}
```

**Real-time Applications:**
```json
{
    "brain.neuron_count": 1000,
    "brain.synapse_density": 0.1,
    "plasticity.stdp.enable": true,
    "plasticity.hebbian.enable": false,
    "neuromod.curiosity.enable": false,
    "performance.monitoring": true,
    "performance.update_interval": 0.1
}
```

**Educational/Demonstration:**
```json
{
    "brain.neuron_count": 500,
    "brain.synapse_density": 0.05,
    "plasticity.stdp.enable": true,
    "plasticity.hebbian.enable": true,
    "neuromod.curiosity.enable": true,
    "neuromod.novelty.enable": true,
    "development.enabled": true,
    "debug.show_warnings": true
}
```

### 2. Monitoring Best Practices

#### Enable Performance Monitoring Early
```python
# Enable comprehensive monitoring
performance_monitor.startMonitoring()
performance_monitor.setUpdateInterval(0.5)  # 2 FPS reporting
performance_monitor.startTimer("brain_computation")
```

#### Profile Different Components
```python
# Time different components
performance_monitor.startTimer("sensory_processing")
agent.processSensoryInput(percept)
performance_monitor.stopTimer("sensory_processing")

performance_monitor.startTimer("motor_decoding")
action = agent.decodeMotorCommand()
performance_monitor.stopTimer("motor_decoding")

performance_monitor.startTimer("world_update")
world.update(timestep)
performance_monitor.stopTimer("world_update")
```

#### Regular Performance Logging
```python
# Log performance data periodically
if step % 100 == 0:
    performance_monitor.logPerformanceData()
    performance_monitor.exportPerformanceData(f"performance_step_{step}.json")
```

### 3. Debugging Best Practices

#### Error Prevention
```python
# Validate configuration before use
config = pynlm.createDefaultConfig()
if not config.has("brain.neuron_count"):
    config.set("brain.neuron_count", 1000)

# Check brain state before operations
brain = pynlm.createBrain(config)
brain.initialize()
if brain.getTotalNeuronCount() == 0:
    print("Warning: Brain has no neurons")
```

#### Debug Information
```python
# Display system information
performance_monitor.logSystemInfo()

# Check for potential issues
system_info = performance_monitor.getSystemInfo()
if "Windows" in system_info:
    print("Running on Windows - check memory limits")
elif "Linux" in system_info:
    print("Running on Linux - optimize for large datasets")
```

## File Structure

### Core Files
- `src/performance/PerformanceMonitor.hpp`: Performance monitoring header
- `src/performance/PerformanceMonitor.cpp`: Performance monitoring implementation

### Examples
- `examples/performance_debugging.py`: Comprehensive examples
- `examples/test_performance_integration.py`: Integration tests
- `examples/quick_start.py`: Quick start guide

### Documentation
- `docs/PERFORMANCE.md`: Performance monitoring documentation
- `docs/ARCHITECTURE.md`: System architecture overview

## Build Instructions

### Build with Performance Monitoring

The PerformanceMonitor is automatically included in the build process. To enable performance monitoring:

```bash
# Configure with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Examples

To run the Python examples, ensure the NLM Python bindings are installed:

```bash
# Install Python bindings
pip install -e .

# Run examples
python examples/quick_start.py
python examples/performance_debugging.py
```

## Performance Metrics

### Typical Performance

| Metric | Typical Value | Target |
|--------|---------------|--------|
| FPS (display rate) | 30-60 | 60 |
| TPS (simulation rate) | 10-100 | 100 |
| Memory usage (1000 neurons) | 50-100 MB | 100 MB |
| CPU usage | 5-20% | 10% |

### Optimization Targets

1. **Memory Efficiency**: < 100MB for 1000 neurons
2. **Simulation Speed**: > 50 TPS for 1000 neurons
3. **Real-time Performance**: < 10ms per simulation step
4. **Scaling**: Linear scaling up to 10,000 neurons

## Troubleshooting

### Common Issues and Solutions

#### Issue: High Memory Usage
**Solution**: Reduce neuron count or synapse density
```python
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.05)
```

#### Issue: Low Performance
**Solution**: Disable unnecessary subsystems
```python
agent.enableCuriosity(False)
agent.enableDevelopment(False)
```

#### Issue: Simulation Errors
**Solution**: Add error handling and validation
```python
try:
    brain.step(step)
except Exception as e:
    print(f"Simulation error: {e}")
    performance_monitor.logPerformanceData()
```

#### Issue: Configuration Problems
**Solution**: Validate configuration before use
```python
if not config.has("brain.neuron_count"):
    print("Error: brain.neuron_count not set")
```

## Future Enhancements

### Planned Features

1. **Advanced Profiling**: SIMD-optimized performance counters
2. **Real-time Visualization**: Live performance graphs and charts
3. **Distributed Monitoring**: Multi-process performance monitoring
4. **AI-powered Optimization**: Machine learning-based performance tuning
5. **Cloud Integration**: Performance metrics to cloud monitoring services

### Research Directions

1. **Neuromorphic Performance**: Performance characteristics of brain-inspired computing
2. **Scale-up Optimization**: Efficient scaling to millions of neurons
3. **Energy Efficiency**: Power consumption and energy-aware scheduling
4. **Hardware Acceleration**: GPU/TPU acceleration for neural simulation

## Conclusion

The Performance and Debugging Tools significantly enhance the NLM framework by providing comprehensive monitoring, profiling, and optimization capabilities. These tools enable users to:

- Monitor and optimize simulation performance
- Profile different components for bottlenecks
- Debug and troubleshoot simulation issues
- Export data for analysis and reporting
- Optimize configurations for specific use cases

The tools are designed to be lightweight and non-intrusive, providing valuable insights without significantly impacting simulation performance.
