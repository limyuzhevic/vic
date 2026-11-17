# Enhanced NLM Python Bindings - Usage Guide

This document provides comprehensive information about the enhanced Python bindings for the NLM (Neural Learning Machine) framework.

## Overview

The enhanced NLM Python bindings provide:

### Key Improvements

1. **Better Error Handling**
   - Custom exception classes: `NLMException`, `ConfigurationError`, `BrainInitializationError`, `SimulationError`
   - More descriptive error messages
   - Exception chaining and detailed debugging information

2. **Enhanced Convenience Methods**
   - Batch operations for improved performance
   - Validation methods for objects and inputs
   - Pythonic APIs with method chaining where appropriate
   - Helper functions for common operations

3. **Performance Monitoring**
   - Built-in performance statistics tracking
   - Memory usage monitoring
   - Step-by-step performance metrics
   - Profiling utilities

4. **Enhanced Type Safety**
   - Better type validation
   - Null pointer checking
   - Input validation and sanitization
   - Safe casting with proper error handling

5. **Improved Documentation**
   - Enhanced docstrings with examples
   - Better parameter descriptions
   - Return type documentation
   - Usage examples for advanced features

## Installation

### Prerequisites

```bash
pip install pybind11
# Ensure NLM C++ libraries are built and installed
```

### Building Enhanced Bindings

```bash
# Build with enhanced bindings (default)
mkdir build
cd build
cmake -DENHANCED_BINDINGS=ON ..
make -j4

# Build with original bindings (for comparison)
mkdir build_original
cd build_original
cmake -DENHANCED_BINDINGS=OFF ..
make -j4
```

## Quick Start Examples

### Basic Usage (Same as Original)

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create a brain
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run a simulation step
brain.step(0)

print("Total neurons:", brain.getTotalNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
```

### Enhanced Usage

```python
import pynlm

# Create and configure brain with validation
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Brain created and initialized successfully")
except pynlm.BrainInitializationError as e:
    print(f"✗ Brain initialization failed: {e}")

# Create a simple world for training
world = pynlm.setupWorldForTraining(width=30, height=30, visionWidth=24, visionHeight=24)

# Enhanced action creation
action = pynlm.Action.create(pynlm.ActionType.MoveForward)
action.setParameters([0.5, 0.3, 0.1])  # Optional parameters

print(f"Action type: {action.getType()}")
print(f"Action valid: {action.isValid()}")

# Enhanced sensory input processing
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Batch operations for efficiency
sensory_data = [
    (pynlm.NeuronId(1), 10.0),
    (pynlm.NeuronId(2), 15.0),
    (pynlm.NeuronId(3), 8.0)
]

brain.batchInjectCurrent(sensory_data)

# Performance monitoring
stats = brain.getPerformanceStats()
print(f"Performance stats: {stats}")
print(f"Average step time: {stats.getAverageStepTime():.4f}s")
print(f"Total simulation time: {stats.getTotalTimeSeconds():.2f}s")
```

### Advanced Usage

```python
import pynlm
import time

# Create training setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.setupWorldForTraining(width=40, height=40)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run training simulation
def train_agent(steps=1000):
    start_time = time.time()
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log progress
        if step % 100 == 0:
            stats = brain.getPerformanceStats()
            print(f"Step {step}: Firing rate={stats.avgFiringRate:.2f}, "
                  f"Spikes={stats.totalSpikes}, "
                  f"Time={stats.getTotalTimeSeconds():.2f}s")
    
    end_time = time.time()
    print(f"✓ Training completed in {end_time - start_time:.2f}s")
    return brain.getPerformanceStats()

# Run training
stats = train_agent(500)

# Validate training results
print(f"\nTraining Results:")
print(f"Total steps: {stats.totalSteps}")
print(f"Final firing rate: {stats.avgFiringRate:.2f}")
print(f"Total spikes: {stats.totalSpikes}")
print(f"Active neurons: {stats.activeNeurons}")
```

### Error Handling Examples

```python
import pynlm

# Example 1: Configuration error handling

try:
    config = pynlm.createDefaultConfig()
    # Try to load a non-existent configuration file
    config.loadFromFile("nonexistent_config.json")
except pynlm.ConfigurationError as e:
    print(f"Configuration error caught: {e}")
    # Handle the error gracefully
    config = pynlm.createDefaultConfig()
except Exception as e:
    print(f"Unexpected error: {e}")

# Example 2: Brain initialization error handling

try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.BrainInitializationError as e:
    print(f"Brain initialization failed: {e}")
    # Try with different configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Recovered with smaller brain")
except Exception as e:
    print(f"Unexpected error: {e}")

# Example 3: Simulation error handling

try:
    brain.step(-1)  # Invalid step
except pynlm.SimulationError as e:
    print(f"Simulation error caught: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Monitoring

### Built-in Performance Statistics

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get performance statistics
stats = brain.getPerformanceStats()

print("Performance Statistics:")
print(f"  Total steps: {stats.totalSteps}")
print(f"  Average step time: {stats.getAverageStepTime():.4f}s")
print(f"  Total simulation time: {stats.getTotalTimeSeconds():.2f}s")
print(f"  Total spikes: {stats.totalSpikes}")
print(f"  Average firing rate: {stats.avgFiringRate:.2f} Hz")
print(f"  Active neurons: {stats.activeNeurons}")
```

### Memory Usage Monitoring

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get memory usage (approximate)
memory_kb = brain.getMemoryUsage()
print(f"Approximate memory usage: {memory_kb} KB")
print(f"Memory usage per neuron: {memory_kb / brain.getTotalNeuronCount():.2f} KB")
```

## Advanced Features

### Batch Operations

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create batch of current injections
sensory_injections = [
    (pynlm.NeuronId(1), 10.0),
    (pynlm.NeuronId(2), 15.0),
    (pynlm.NeuronId(3), 8.0),
    (pynlm.NeuronId(4), 12.0),
    (pynlm.NeuronId(5), 9.0)
]

# Apply batch injection efficiently
brain.batchInjectCurrent(sensory_injections)
print("✓ Batch current injection completed")
```

### Enhanced Action Creation

```python
import pynlm

# Create actions with parameters
action1 = pynlm.Action.create(pynlm.ActionType.MoveForward)
print(f"Action: {action1.getName()}, Type: {action1.getType()}")

action2 = pynlm.Action.create(pynlm.ActionType.Custom, [0.5, 0.3, 0.1])
print(f"Action: {action2.getName()}, Parameters: {action2.getParameters()}")
print(f"Action valid: {action2.isValid()}")
```

### Enhanced World Setup

```python
import pynlm

# Create world for specific training scenario
world = pynlm.setupWorldForTraining(width=50, height=50, visionWidth=32, visionHeight=32)

# Additional world configuration
world.setMaxEnergy(200.0)
world.setEnergyDecayRate(0.02)

# Set agent starting position
world.setAgentStart(25, 25)

print(f"World configured: {world.getWidth()}x{world.getHeight()}")
print(f"Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
```

## AgentBrain Enhancements

### AgentBrain Statistics

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.setupWorldForTraining()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Get agent statistics
stats = agent.getStats()
print("Agent Statistics:")
print(f"  Steps since turn: {stats.stepsSinceTurn}")
print(f"  Repeat count: {stats.repeatCount}")
print(f"  Curiosity level: {stats.curiosityLevel:.3f}")
print(f"  Novelty level: {stats.noveltyLevel:.3f}")
print(f"  Planner decisions: {stats.plannerDecisionCount}")
print(f"  Concept biases: {stats.conceptBiasCount}")
```

### AgentBrain Readiness Check

```python
import pynlm

agent = pynlm.createAgentBrain(None)  # This will fail
print(f"Agent ready: {agent.isReady()}")  # False

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.setupWorldForTraining()
agent.initialize(world)

print(f"Agent ready: {agent.isReady()}")  # True
```

## Migration Guide

### From Original to Enhanced Bindings

If you are migrating from the original bindings, the changes are largely backward compatible. Here's what changed:

#### API Changes (Backward Compatible)

1. **Same class names and methods** - All original classes and methods are preserved
2. **Enhanced constructor parameters** - New optional parameters with defaults
3. **Additional convenience methods** - New methods that don't affect existing code
4. **Better error handling** - More descriptive exceptions

#### New Features (Optional)

1. **`pynlm.setupWorldForTraining()`** - Helper for creating training worlds
2. **`brain.getPerformanceStats()`** - Performance monitoring utilities
3. **`brain.batchInjectCurrent()`** - Batch operations
4. **`agent.getStats()`** - Agent statistics
5. **`action.isValid()`** - Action validation
6. **Custom exception classes** - Better error handling

#### Migration Examples

**Original code:**

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)
```

**Enhanced code (same functionality, plus improvements):**

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)

# You can now use enhanced features if needed
brain.batchInjectCurrent([(pynlm.NeuronId(1), 10.0)])
```

## Troubleshooting

### Common Issues

1. **Import errors**
   ```python
   # Make sure pynlm is built with enhanced bindings
   # Check that the module is in your PYTHONPATH
   ```

2. **Configuration errors**
   ```python
   # Handle ConfigurationError exceptions
   try:
       config.loadFromFile("config.json")
   except pynlm.ConfigurationError as e:
       print(f"Configuration error: {e}")
   ```

3. **Brain initialization errors**
   ```python
   # Use try-catch for BrainInitializationError
   try:
       brain = pynlm.createBrain(config)
       brain.initialize()
   except pynlm.BrainInitializationError as e:
       print(f"Brain initialization failed: {e}")
   ```

4. **Performance monitoring**
   ```python
   # Use try-catch for performance monitoring
   try:
       stats = brain.getPerformanceStats()
   except Exception as e:
       print(f"Performance monitoring error: {e}")
   ```

### Building Issues

1. **Compilation errors**
   ```bash
   # Check that pybind11 is properly installed
   # Ensure all required NLM libraries are built
   ```

2. **Linking errors**
   ```bash
   # Make sure all dependencies are linked correctly
   # Check that NLM_CORE, NLM_AGENT, and NLM_WORLD are available
   ```

## Version Information

- **Enhanced Bindings Version**: 1.0.0
- **Original Bindings Version**: 0.1.0
- **Enhanced Features**: Added in version 1.0.0
- **Backward Compatibility**: Fully backward compatible

## Support

For issues or questions about the enhanced bindings:

1. **Documentation**: This file (`ENHANCED_USAGE.md`)
2. **GitHub Issues**: Report bugs or feature requests
3. **Examples**: Check the examples directory for more usage patterns
4. **Community**: Join the NLM community for support

## License

This enhanced bindings implementation is licensed under the same terms as the original NLM project (MIT License).

---

*Last Updated: $(date -u +%Y-%m-%d)*
*Version: Enhanced Bindings 1.0.0*