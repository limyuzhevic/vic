# NLM Python Integration Guide

## Overview

This guide documents the integration of NLM (Neural Learning Machine) with Python, showcasing the improvements made to the Python bindings to enhance usability, performance, and developer experience.

## Summary of Improvements

The NLM Python bindings have been completely redesigned to follow modern Python best practices while maintaining full compatibility with the underlying C++ implementation.

### Key Improvements

1. **Pythonic API Design**: Context managers, property access, dictionary-like interfaces
2. **Enhanced Documentation**: Comprehensive API reference and examples
3. **Performance Monitoring**: Built-in profiling and optimization tools
4. **Advanced Usage Patterns**: Factory functions, helper methods, and convenience constructors
5. **Type Safety**: Comprehensive type hints and IDE support
6. **Error Handling**: Pythonic exception translation and validation

## Migration Path

### For Beginners

If you're new to NLM, start with `easy_usage.md` for a gentle introduction, then progress to this guide for advanced features.

### For Intermediate Users

If you're familiar with the basic API, explore the `advanced_usage.py` examples and `API_REFERENCE.md` for detailed documentation.

### For Advanced Users

Leverage the `performance.py` module for optimization, create custom agents, and integrate NLM into your existing Python workflows.

## Getting Started

### Installation

```bash
# Standard installation
pip install pynlm

# Development installation (with all optional dependencies)
pip install -e .[dev]
```

### Quick Verification

```python
import pynlm

# Verify installation
print(f"NLM Python Bindings v{pynlm.get_version()}")

# Create and run a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(10):
    brain.step(i)

print(f"✓ Basic functionality working: {brain.getTotalSpikeCount()} spikes")
```

## API Evolution

### Before (Legacy API)

The original API was C++-centric with direct object access:

```python
config = pynlm.createDefaultConfig()
neuron_id = neuron.getId().value  # Direct C++ method call
config_value = config.get("key")  # Manual handling
```

### After (Pythonic API)

The new API follows Python conventions:

```python
config = pynlm.createDefaultConfig()
neuron_id = neuron.id  # Property access
config_value = config["key"]  # Dictionary-like access
```

## New API Features

### 1. Context Managers

```python
# Safe, controlled brain operations
with brain.create_context() as ctx:
    ctx.step(step)
    ctx.processSensoryInput(percept)
    action = ctx.produceAction()
    
    # Automatic cleanup and error handling
    snapshot = ctx.getSnapshot()
```

### 2. Factory Functions

```python
# Easy brain creation
brain = pynlm.createBrain("simple")
brain = pynlm.createBrain("medium")
brain = pynlm.createBrain("complex")

# Parameter-based creation
brain = pynlm.createBrain(1000, 5)  # neurons, regions
```

### 3. Enhanced Configuration

```python
config = pynlm.createDefaultConfig()

# Dictionary-like interface
config["brain.neuron_count"] = 1000
config["learning.rate"] = 0.001

# Get with default
neuron_count = config.getOr("brain.neuron_count", 1000)

# Configuration helper
pynlm.configureBrain(config, 500, 10, connectionProbability=0.15)
```

### 4. Learning Agents

```python
# Create agent with learning capabilities
agent = pynlm.createLearningAgent(
    brain,
    enableRewardModulation=True,
    enableCuriosity=True,
    enableDevelopment=True
)

# Get comprehensive learning status
status = agent.getLearningStatus()
print(f"Learning: {status['reward_modulation']}, {status['curiosity_level']:.2f}")
```

### 5. Data Processing

```python
# Merge sensory data
percept1.merge(percept2)

# Serialize to dictionary
data = percept1.to_dict()

# Create brain snapshot
with brain.create_context() as ctx:
    snapshot = ctx.getSnapshot()
    print(snapshot.to_string())
```

## World Integration

### Complete Agent Simulation

```python
# Setup simulation
brain = pynlm.createBrain("medium")
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

agent = pynlm.createLearningAgent(brain)
agent.initialize(world)
world.setAgentStart(10.0, 10.0)

# Run episode
for step in range(100):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
```

### World Object Management

```python
world = pynlm.createSimpleWorld()
world.configure(30, 30, 12, 12)
world.reset()

# Add objects
world.addResource(5.0, 5.0, 10.0)  # Food
world.addHazard(25.0, 25.0, 2.0)    # Danger
world.addWall(15.0, 15.0, 1.5)      # Obstacle
```

## Performance Monitoring

### Basic Profiling

```python
from pynlm.performance import profile_brain_simulation

# Profile a complete simulation
results = profile_brain_simulation(
    brain_factory=lambda: pynlm.createBrain("medium"),
    world_factory=lambda: pynlm.createSimpleWorld(),
    steps=100
)

print(f"Steps per second: {results['summary']['steps_per_second']:.2f}")
print(f"Recommendations: {len(results['recommendations'])}")
```

### Benchmark Suite

```python
from pynlm.performance import BenchmarkSuite

suite = BenchmarkSuite()
suite.run_all_benchmarks()

# Save results for analysis
suite.save_results("benchmark_results.json")
```

## Advanced Patterns

### Custom Agent Subclasses

```python
class AdvancedAgent:
    def __init__(self, brain, world):
        self.agent = pynlm.createLearningAgent(
            brain,
            enableRewardModulation=True,
            enableCuriosity=True,
            enableDevelopment=True
        )
        self.agent.initialize(world)
        self.brain = brain
        self.world = world
    
    def step(self, step):
        # Custom agent logic
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        self.brain.step(step)
        
        action = self.agent.decodeMotorCommand()
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Custom learning
        reward = self.calculate_reward()
        self.agent.applyRewardModulation(reward, 0.0)
        
        return action
    
    def calculate_reward(self):
        # Custom reward calculation
        body = self.world.getAgentBody()
        return body.energy / 100.0
```

### Data Analysis

```python
# Analyze simulation results
brain = pynlm.createBrain("medium")
brain.initialize()

# Run simulation
for i in range(1000):
    brain.step(i)

# Analyze results
stats = brain.formatStats()
print(stats)

# Get brain snapshot
with brain.create_context() as ctx:
    snapshot = ctx.getSnapshot()
    print(f"Average firing rate: {snapshot.avg_firing_rate:.2f} Hz")
```

## Error Handling

### Configuration Validation

```python
try:
    config = pynlm.createDefaultConfig()
    pynlm.configureBrain(config, 500, 5)  # Valid configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
except ValueError as e:
    print(f"Configuration error: {e}")
except RuntimeError as e:
    print(f"Runtime error: {e}")
```

### Context Manager Safety

```python
# Context manager ensures proper cleanup
with brain.create_context() as ctx:
    try:
        ctx.step(0)
        action = ctx.produceAction()
    except Exception as e:
        print(f"Error in context: {e}")
        # Context manager still cleans up
```

## Troubleshooting

### Common Issues and Solutions

**Issue: Module not found**
```bash
pip install --force-reinstall .
```

**Issue: Brain initialization fails**
```python
# Check configuration
config = pynlm.createDefaultConfig()
print(f"Default config: {config.summary()}")

# Try with specific parameters
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
config.set("brain.region_count", 2)
brain = pynlm.createBrain(config)
brain.initialize()
```

**Issue: Performance is slow**
```python
# Use performance module
from pynlm.performance import quick_benchmark
results = quick_benchmark()
print(f"Performance: {results}")

# Consider simpler brain configuration
brain = pynlm.createBrain("simple")
```

**Issue: Complex API overwhelming**
```python
# Start with simple examples
from pynlm import easy_usage
# Or use basic examples from easy_usage.md
```

## Development Resources

### For Developers

- **Source Code**: `python/bindings.cpp` - Main Python bindings
- **API Reference**: `python/API_REFERENCE.md` - Complete API documentation
- **Examples**: `examples/advanced_usage.py` - Advanced usage patterns
- **Performance Tools**: `python/performance.py` - Optimization and profiling
- **Package Structure**: `python/__init__.py` - Module organization

### For Research

- **Phase 6 Integration**: All NLM Phase 6 systems are accessible
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Prediction**: Neural prediction and error signals
- **Development**: Developmental stages and plasticity

## Future Enhancements

### Planned Features

1. **Async Support**: Non-blocking simulation for parallel processing
2. **GPU Acceleration**: CUDA support for neural computations
3. **Distributed Computing**: Multi-agent simulations
4. **Visualization Tools**: Interactive brain visualization
5. **Model Zoo**: Pre-trained brain models
6. **Export/Import**: Model serialization formats

### Community Contributions

- **Extensions**: Custom agent behaviors and world models
- **Documentation**: Example additions and API improvements
- **Tests**: Binding-specific test coverage
- **Performance**: Optimization and benchmarking

## Support

### Getting Help

- **Documentation**: Check the `docs/` directory for comprehensive guides
- **Examples**: Run the provided examples to understand usage patterns
- **Community**: Join NLM community for support and discussions

### Reporting Issues

- **Bug Reports**: Submit issues with reproduction steps
- **Feature Requests**: Suggest new API features and improvements
- **Performance Issues**: Use performance module to identify bottlenecks

## Conclusion

The NLM Python bindings now provide a comprehensive, Pythonic interface to the neural simulation framework, making it accessible to beginners while powerful enough for advanced research applications. The improvements focus on:

1. **Usability**: Intuitive API patterns and comprehensive documentation
2. **Performance**: Built-in monitoring and optimization tools
3. **Flexibility**: Multiple creation methods and configuration options
4. **Reliability**: Robust error handling and validation
5. **Extensibility**: Easy to extend with custom behaviors

The NLM Python integration is now ready for production use, research applications, and educational purposes.
