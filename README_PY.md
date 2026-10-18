# NLM Python Bindings - Quick Start Guide

## Overview

Welcome to the enhanced NLM (Neural Learning Machine) Python bindings! This guide provides a quick introduction to the new features and improvements available in the current version.

## What's New

The NLM Python bindings have been completely enhanced with:

- **Comprehensive API**: Complete bindings for all NLM components
- **Convenience Functions**: High-level utilities for common operations
- **Enhanced Error Handling**: Custom exceptions and validation
- **Advanced Configuration**: Flexible configuration with sensible defaults
- **Rich Examples**: Extensive example scripts demonstrating all features

## Quick Installation

The NLM Python bindings are part of the main NLM project. If you have the NLM source code, the Python bindings are already included.

For users who want to work with the Python bindings directly:

```bash
# Clone the NLM repository
cd /path/to/nlm

# Navigate to python directory
cd python

# The bindings are already included in the project
# No separate installation required for development
```

## Basic Usage Examples

### Example 1: Simple Brain Simulation

```python
import pynlm

# Create brain with sensible defaults
config = pynlm.createConfigWithDefaults()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
brain.runSimulation(100, log_interval=20)

print(f"Simulation completed. Total spikes: {brain.getTotalSpikeCount()}")
```

### Example 2: Advanced Configuration

```python
import pynlm

# Create brain with custom parameters
config = pynlm.createConfigAdvanced(
    neuron_count=1500,
    connection_probability=0.05,
    enable_stdp=True,
    enable_hebbian=True,
    enable_structural=True
)

brain = pynlm.createBrain(config)
brain.initialize()
brain.runSimulation(500, log_interval=50)
```

### Example 3: Complete Agent-World Simulation

```python
import pynlm

# Setup complete system
config = pynlm.createConfigWithDefaults()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all advanced features
agent.enableAllFeatures()

# Run simulation
for step in range(400):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Key Features

### 1. Enhanced Configuration

#### `createConfigWithDefaults()`
Creates a configuration with sensible defaults for most use cases.

```python
config = pynlm.createConfigWithDefaults()
```

#### `createConfigAdvanced()`
Creates a configuration with advanced control over all parameters.

```python
config = pynlm.createConfigAdvanced(
    neuron_count=2000,
    connection_probability=0.05,
    enable_stdp=True,
    enable_hebbian=True,
    enable_structural=True,
    enable_curiosity=True
)
```

### 2. High-Level Simulation Control

#### `runSimulation(steps, log_interval)`
Runs a simulation with built-in logging.

```python
brain.runSimulation(1000, log_interval=100)
```

#### `getBrainStatistics(brain)`
Returns comprehensive brain statistics as a dictionary.

```python
stats = pynlm.getBrainStatistics(brain)
print(f"Neurons: {stats['total_neurons']}")
print(f"E/I Ratio: {stats['e_i_ratio']:.2f}")
```

### 3. Memory System Analysis

#### `getMemoryStats(brain)`
Returns memory system statistics.

```python
memory_stats = pynlm.getMemoryStats(brain)
print(f"Working memory traces: {memory_stats['working_memory_active_traces']}")
print(f"Episodic memory episodes: {memory_stats['episodic_memory_episodes']}")
```

### 4. Development Tracking

#### `getDevelopmentStats(brain, agent_brain)`
Returns development system statistics.

```python
dev_stats = pynlm.getDevelopmentStats(brain, agent)
print(f"Development stage: {dev_stats['stage']}")
print(f"Curiosity level: {dev_stats['curiosity_level']:.3f}")
```

### 5. Spike Pattern Analysis

#### `analyzeSpikePattern(brain, window_size)`
Analyzes spike patterns over a specified time window.

```python
pattern = pynlm.analyzeSpikePattern(brain, window_size=50)
print(f"Spike pattern (first 10 points): {pattern[:10]}")
```

## Error Handling

The enhanced Python bindings include comprehensive error handling:

```python
import pynlm

try:
    config = pynlm.createConfigWithDefaults()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Brain created successfully")
    
    # Test error handling
    if not brain:
        raise pynlm.NLMError("Brain object is null")
    
    print("✓ Error handling test passed")
    
except pynlm.NLMError as e:
    print(f"✓ Caught NLMError: {e}")
except Exception as e:
    print(f"✗ Unexpected error: {e}")
```

## Available Example Scripts

### 1. `examples/main.py`
Complete example suite demonstrating all features:
- Basic usage and simulation
- Advanced configuration
- Complete agent-world simulation
- Memory system analysis
- Development tracking
- Error handling
- Performance benchmarking

### 2. `examples/example_improved_bindings.py`
Demonstrates new Python bindings features:
- Enhanced API usage
- Convenience functions
- Error handling examples

### 3. `examples/example_advanced_features.py`
Advanced feature demonstrations:
- Development and adaptation systems
- Plasticity mechanisms
- Complex agent behavior
- Memory integration and replay
- Performance scaling
- Error scenarios

## Error Codes

The enhanced Python bindings provide specific error codes:

### NLMError
General NLM-specific errors:
- `Invalid configuration parameters`
- `Missing required files`
- `Initialization failures`

### Configuration Errors
Errors related to configuration management:
- `Invalid parameter values`
- `Missing required configuration keys`
- `File I/O errors`

### Runtime Errors
Errors during simulation execution:
- `Null pointer exceptions`
- `Memory allocation failures`
- `Simulation step errors`

## Troubleshooting

### Common Issues and Solutions

#### "Brain not initialized"

**Cause**: `brain.initialize()` not called before simulation

**Solution**:
```python
config = pynlm.createConfigWithDefaults()
brain = pynlm.createBrain(config)
brain.initialize()  # Always call initialize() first
brain.runSimulation(100)
```

#### "Configuration file not found"

**Cause**: `config_file` parameter points to non-existent file

**Solution**:
```python
# Use default configuration
config = pynlm.createConfigWithDefaults()

# Or handle file not found gracefully
try:
    config = pynlm.createConfigAdvanced(config_file="custom.cfg")
except pynlm.NLMError:
    config = pynlm.createConfigWithDefaults()
    print("Warning: Using default configuration")
```

#### "Simulation step error"

**Cause**: Invalid simulation parameters

**Solution**:
```python
brain.runSimulation(1000, log_interval=100)  # Use valid parameters
```

## Best Practices

### 1. Always Initialize

```python
# Always call initialize() after creating a brain
config = pynlm.createConfigWithDefaults()
brain = pynlm.createBrain(config)
brain.initialize()  # Required!
```

### 2. Use Appropriate Log Intervals

```python
# Use appropriate log intervals for different simulation sizes
brain.runSimulation(1000, log_interval=100)  # Every 100 steps
brain.runSimulation(10000, log_interval=1000)  # Every 1000 steps
```

### 3. Handle Errors Gracefully

```python
try:
    brain.runSimulation(500)
except pynlm.NLMError as e:
    print(f"Simulation failed: {e}")
    # Recovery or fallback logic
    brain = pynlm.createConfigWithDefaults()
    brain = pynlm.createBrain(config)
    brain.initialize()
```

### 4. Monitor Performance

```python
import time

start_time = time.time()
brain.runSimulation(1000)
end_time = time.time()

print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Steps per second: {1000 / (end_time - start_time):.1f}")
```

## Further Reading

### Documentation

- **HOW_TO_USE.md**: Complete user guide with examples
- **easy_usage.md**: Beginner-friendly introduction
- **IMPROVEMENTS_SUMMARY.md**: Complete summary of improvements
- **CODE_STRUCTURE_IMPROVEMENTS.md**: Code structure and organization details
- **FINAL_PROJECT_SUMMARY.md**: Comprehensive project overview

### API Reference

The Python bindings provide complete API documentation for:

- **Configuration Classes**: `Config`, `ConfigManager`
- **Brain Classes**: `Brain`, `AgentBrain`
- **World Classes**: `SimpleWorld`, `WorldObject`
- **Sensory Classes**: `Vision`, `Audio`, `InternalSignals`
- **Action Classes**: `Action`, `WorldObject`
- **Utility Functions**: `createConfigWithDefaults`, `createConfigAdvanced`, etc.

### Examples

All examples are located in the `examples/` directory:

- `main.py`: Complete example suite
- `example_improved_bindings.py`: Enhanced bindings examples
- `example_advanced_features.py`: Advanced feature demonstrations

## Support

### For Issues and Questions

- **GitHub Issues**: Report bugs and request features
- **Documentation**: Check the latest documentation for answers
- **Examples**: Use the provided examples as reference

### For Advanced Support

- **Code Examples**: Study the provided example scripts
- **Documentation**: Consult the comprehensive documentation
- **Community**: Engage with the NLM community for support

## Conclusion

The enhanced NLM Python bindings provide a comprehensive, user-friendly interface to the NLM neural simulation framework. Whether you're a beginner or an advanced user, you'll find powerful tools and extensive documentation to help you create sophisticated neural simulations.

Get started today and explore the full capabilities of the NLM Python bindings!

---

**Version**: 1.0.0 (Enhanced)
**Author**: NLM Project Team
**License**: MIT
**Documentation**: Complete online documentation available
**Examples**: Comprehensive example suite included

Happy simulating!
