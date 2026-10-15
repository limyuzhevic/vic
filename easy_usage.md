# NLM Python Bindings - Enhanced API Documentation

## Overview

This document describes the enhanced Python API for NLM (Neural Learning Machine) bindings. The enhanced edition introduces significant improvements over the original API, including:

- **Context managers** for automatic resource management
- **Batch operations** for improved performance
- **Configuration builder pattern** for easier setup
- **Performance profiling** and monitoring
- **Enhanced error handling** with custom Python exceptions
- **Convenience methods** and Pythonic API design
- **JSON configuration support** with proper parsers

## Quick Start

### Enhanced Basic Usage

```python
import pynlm

# Method 1: Using context manager (recommended)
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    brain.run_steps(100)  # Run multiple steps efficiently
    print(f"Brain stats: {brain.get_brain_stats()}")

# Method 2: Using configuration builder
brain = pynlm.createBrain(
    pynlm.createConfigBuilder()
    .setBrainConfig(neuron_count=1000, region_count=3)
    .setLearningConfig(learning_rate=0.01, stdp_rate=0.001)
    .setNeuromodulationConfig(curiosity=True, novelty=True)
    .build()
)

brain.initialize()
brain.warm_up(50)

# Method 3: Complete agent simulation with enhanced API
brain, world, agent = setup_complete_simulation()

# Run simulation with enhanced API
with agent:
    agent.initialize(world)
    pynlm.run_agent_simulation(brain, world, agent, 1000)
    
    # Monitor performance
    profiler = brain.create_profiler()
    # ... simulation code ...
    # profiler.record_step() would be called internally
    stats = profiler.collect()
    print(profiler.report())
```

## Enhanced Configuration API

### Configuration Builder Pattern

The configuration builder provides a fluent interface for creating complex configurations:

```python
import pynlm

# Basic configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 3)

# Using builder pattern (recommended)
config = (pynlm.createConfigBuilder()
          .setBrainConfig(neuron_count=1000, region_count=3)
          .setLearningConfig(learning_rate=0.01)
          .setNeuromodulationConfig(curiosity=True, novelty=True)
          .build())

# JSON configuration support
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")  # Now supports proper JSON
config.saveToFile("backup_config.json")  # Saves actual values
```

### Configuration Access Methods

```python
config = pynlm.createDefaultConfig()

# Enhanced getters with type safety
neuron_count = config.getIntOr("brain.neuron_count", 1000)
learning_rate = config.getDoubleOr("learning.rate", 0.001)
curiosity_enabled = config.getBoolOr("neuromod.curiosity.enable", True)

# Convert to dictionary
config_dict = config.to_dict()

# Schema validation
schema = {"brain.neuron_count": "int", "learning.rate": "float"}
if config.validate(schema):
    print("Configuration is valid")
```

## Enhanced Brain API

### Context Manager

```python
import pynlm

# Context manager ensures proper initialization/cleanup
brain = pynlm.createBrain(config)

# Old way:
brain.initialize()
for step in range(100):
    brain.step(step)
brain.reset()

# New way with context manager:
with pynlm.createBrain(config) as brain:
    for step in range(100):
        brain.step(step)  # No need to call initialize/reset manually

# Context manager returns the brain for method chaining
with pynlm.createBrain(config) as brain:
    brain.warm_up(50)  # Convenience method
    brain.run_steps(100)  # Batch operation
```

### Batch Operations

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Run multiple steps efficiently
brain.run_steps(1000)  # Equivalent to: for i in range(1000): brain.step(i)

# Run complete simulation with world and agent
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
pynlm.run_agent_simulation(brain, world, agent, 1000)
```

### Convenience Methods

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Reset and initialize in one call
brain.reset_and_initialize()

# Warm up the brain (common pattern)
brain.warm_up(50)  # Reset, initialize, then run 50 steps

# Get comprehensive statistics
stats = brain.get_brain_stats()
print(f"Neurons: {stats['total_neurons']}")
print(f"Firing rate: {stats['average_firing_rate']:.2f} Hz")

# Configure brain in one method call
brain.configure_neurons(neuron_count=1000, region_count=3)
brain.configure_learning(learning_rate=0.01, stdp_rate=0.001)
brain.configure_neuromodulation(curiosity=True, novelty=True)
```

### Performance Monitoring

```python
import pynlm

brain = pynlm.createBrain(config)
brain.initialize()

# Create performance profiler
profiler = brain.create_profiler()

# Run simulation (in a real implementation, you'd call profiler.record_step())
for step in range(1000):
    brain.step(step)
    # profiler.record_step()  # Would be called internally

# Get performance statistics
stats = profiler.collect()
print(f"Performance Report:")
print(f"  Steps: {stats.steps_completed}")
print(f"  Average time per step: {stats.total_time_ms / stats.steps_completed:.2f} ms")
print(f"  Total spikes: {stats.spikes_generated}")

# Get formatted report
print(profiler.report())
```

## Enhanced AgentBrain API

### Context Manager

```python
import pynlm

brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()

# Old way:
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
for step in range(100):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
agent.reset()

# New way with context manager:
agent = pynlm.createAgentBrain(brain)
with agent:  # Context manager for agent
    agent.initialize(world)
    for step in range(100):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    # No need to call agent.reset() manually
```

### Convenience Methods

```python
agent = pynlm.createAgentBrain(brain)

# Get comprehensive agent statistics
agent_stats = agent.get_agent_stats()
print(f"Agent statistics: {agent_stats}")

# Check subsystem status
print(f"Reward modulation: {agent.isRewardModulationEnabled()}")
print(f"Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
print(f"Development: {agent.isDevelopmentEnabled()}")
print(f"Curiosity: {agent.isCuriosityEnabled()}")
```

## Enhanced World API

### Convenience Methods

```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Reset and configure in one call
world.reset_and_configure(20, 20, 8, 8)

# Create episode function
episode = world.create_episode(timestep=0.1)
for step in range(100):
    episode(step)  # Run one step of the episode

# Get world statistics
world_stats = world.get_world_stats()
print(f"World statistics: {world_stats}")
```

## Error Handling

### Custom Python Exceptions

The enhanced API introduces custom Python exceptions for better error handling:

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.BrainError as e:
    print(f"Brain error: {e}")
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
except pynlm.WorldError as e:
    print(f"World error: {e}")
```

### Error Handling in Enhanced Methods

```python
brain = pynlm.createBrain(config)
try:
    with pynlm.createBrain(config) as brain:  # Context manager
        brain.initialize()
        # ... simulation code ...
except Exception as e:
    print(f"Simulation failed: {e}")
    # Context manager ensures cleanup even if an exception occurs
```

## Advanced Features

### Complete Simulation Setup

```python
def setup_complete_simulation(config_file=None):
    """Set up a complete NLM simulation with all components."""
    import pynlm
    
    # Create configuration
    if config_file:
        config = pynlm.createDefaultConfig()
        config.loadFromFile(config_file)
    else:
        config = pynlm.createConfigBuilder() \
            .setBrainConfig(neuron_count=500, region_count=2) \
            .setLearningConfig(learning_rate=0.01) \
            .setNeuromodulationConfig(curiosity=True, novelty=True) \
            .build()
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    return brain, world, agent

# Use the simulation
brain, world, agent = setup_complete_simulation()

# Run enhanced simulation
with agent:
    agent.initialize(world)
    for step in range(500):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### Performance Comparison

| Feature | Original API | Enhanced API | Benefits |
|---------|-------------|-------------|----------|
| Initialization | Manual `brain.initialize()` | Context manager | Automatic, error handling |
| Cleanup | Manual `brain.reset()` | Context manager | Automatic, guaranteed |
| Batch operations | Manual loops | `brain.run_steps()` | Better performance |
| Configuration | Manual `config.set()` | Builder pattern | More readable, less error-prone |
| Error handling | Basic exceptions | Custom exceptions | Better debugging |
| Statistics | Individual calls | `get_brain_stats()` | Easier data access |
| Performance monitoring | Not available | Built-in profiler | Performance analysis |

## Migration Guide

### From Original to Enhanced API

#### Configuration

```python
# Original
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.01)
config.set("neuromod.curiosity.enable", True)

# Enhanced
config = pynlm.createConfigBuilder() \
    .setBrainConfig(neuron_count=1000) \
    .setLearningConfig(learning_rate=0.01) \
    .setNeuromodulationConfig(curiosity=True) \
    .build()
```

#### Brain Usage

```python
# Original
brain = pynlm.createBrain(config)
brain.initialize()
for step in range(100):
    brain.step(step)
brain.reset()

# Enhanced (with context manager)
with pynlm.createBrain(config) as brain:
    brain.run_steps(100)  # More efficient

# Enhanced (without context manager)
brain = pynlm.createBrain(config)
brain.initialize()
brain.run_steps(100)  # More efficient
brain.reset()
```

#### Agent Usage

```python
# Original
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
for step in range(100):
    # ... simulation code ...
agent.reset()

# Enhanced (with context manager)
agent = pynlm.createAgentBrain(brain)
with agent:
    agent.initialize(world)
    for step in range(100):
        # ... simulation code ...
    # agent.reset() called automatically
```

## Best Practices

### 1. Use Context Managers

Always use context managers for Brain and AgentBrain:

```python
with pynlm.createBrain(config) as brain:
    # Safe initialization and cleanup
    brain.initialize()
    # ... use brain ...
    # No need for manual reset()
```

### 2. Use Builder Pattern for Configuration

Use the configuration builder for complex configurations:

```python
config = (pynlm.createConfigBuilder()
          .setBrainConfig(neuron_count=1000, region_count=3)
          .setLearningConfig(learning_rate=0.01)
          .setNeuromodulationConfig(curiosity=True)
          .build())
```

### 3. Use Batch Operations

Use batch operations for better performance:

```python
brain.run_steps(1000)  # More efficient than manual loop
pynlm.run_agent_simulation(brain, world, agent, 1000)
```

### 4. Handle Errors Gracefully

Always handle custom exceptions:

```python
try:
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        # ... simulation code ...
except pynlm.BrainError as e:
    print(f"Brain initialization failed: {e}")
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
```

### 5. Monitor Performance

Use the built-in profiler for performance analysis:

```python
brain = pynlm.createBrain(config)
profiler = brain.create_profiler()

# ... run simulation ...

stats = profiler.collect()
print(profiler.report())
```

## Version Information

- **Enhanced API Version**: 2.0.0
- **Python Bindings Version**: pynlm 2.0.0-enhanced
- **C++ Core**: NLM Phase 6.0

## Compatibility

### Backward Compatibility

The enhanced API maintains backward compatibility with the original API:

- All original methods remain available
- Original examples continue to work with minor modifications
- New features are additive, not breaking changes

### New Requirements

The enhanced API requires Python 3.8+ (same as original).

## Troubleshooting

### Common Issues and Solutions

#### "My brain isn't doing anything"

**Problem**: Brain not initialized or running

**Solution**:

```python
# Ensure proper initialization
with pynlm.createBrain(config) as brain:
    brain.initialize()
    brain.run_steps(10)  # Run at least a few steps
```

#### "The agent isn't moving"

**Problem**: Agent not properly initialized

**Solution**:

```python
agent = pynlm.createAgentBrain(brain)
with agent:  # Use context manager
    agent.initialize(world)
    # ... simulation code ...
```

#### "Everything is 0"

**Problem**: Brain needs time to "warm up"

**Solution**:

```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.warm_up(100)  # Let brain run a few steps before using
```

#### "Performance issues"

**Problem**: Simulation is slow

**Solution**:

```python
# Use batch operations for better performance
brain.run_steps(1000)  # More efficient than manual loop

# Monitor performance
profiler = brain.create_profiler()
# ... run simulation ...
print(profiler.report())
```

## Further Reading

### Related Documents

- `HOW_TO_USE.md` - Detailed usage instructions for the original API
- `docs/ARCHITECTURE.md` - System architecture overview
- `docs/SCIENCE.md` - Scientific background
- `docs/ROADMAP.md` - Future development plans

### Learning Resources

1. **Tutorials**: Start with the examples in `easy_usage.md`
2. **Advanced Usage**: Refer to `HOW_TO_USE.md` for advanced patterns
3. **Architecture**: Read `docs/ARCHITECTURE.md` for understanding system design
4. **Science**: Consult `docs/SCIENCE.md` for scientific background

## Support and Contributions

### Community Support

- **GitHub**: Report issues and contribute at [github.com/nlm-project/nlm](https://github.com/nlm-project/nlm)
- **Discussions**: Join the community discussion
- **Issues**: Submit bug reports and feature requests

### Development Guidelines

When contributing to the enhanced API:

1. **Follow the existing code style**
2. **Maintain backward compatibility**
3. **Add comprehensive documentation**
4. **Include examples and test cases**
5. **Write clear commit messages**

### Development Workflow

1. **Fork the repository**
2. **Create a feature branch**
3. **Implement changes**
4. **Add tests**
5. **Update documentation**
6. **Create pull request**

## Conclusion

The enhanced NLM Python API provides a more powerful, intuitive, and Pythonic interface for neural simulation. The new features:

- **Simplify common patterns** with context managers and builder pattern
- **Improve performance** with batch operations and optimized calls
- **Enhance debugging** with custom exceptions and profiling
- **Increase productivity** with convenience methods and comprehensive statistics
- **Maintain compatibility** with existing code while adding new capabilities

The enhanced API makes it easier than ever to:

1. **Create and configure neural networks**
2. **Run complex simulations**
3. **Monitor performance and statistics**
4. **Debug and troubleshoot issues**
5. **Integrate NLM into larger applications**

Get started today and experience the power of NLM's enhanced Python API!

---

*This documentation is for NLM Python Bindings v2.0.0-enhanced. For the original API documentation, refer to `HOW_TO_USE.md`.*

## Appendices

### Appendix A: Complete Code Examples

#### Example 1: Silent Brain (Just Neurons)

```python
import pynlm

# Using context manager
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    for i in range(10):
        brain.step(i)

print("Silent brain test done!")
```

#### Example 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)

# Watch the world for 30 steps
with agent:
    agent.initialize(world)
    for i in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)

print("Watched world for 30 steps")
print(f"Firing rate: {brain.getAverageFiringRate()}")
```

#### Example 3: Complete Agent with Enhanced API

```python
import pynlm

# Setup
config = pynlm.createConfigBuilder() \
    .setBrainConfig(neuron_count=500, region_count=2) \
    .setLearningConfig(learning_rate=0.01) \
    .setNeuromodulationConfig(curiosity=True, novelty=True) \
    .build()

brain = pynlm.createBrain(config)
brain.initialize()
brain.warm_up(50)

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent with enhanced API
with agent:
    agent.initialize(world)
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
```

#### Example 4: Performance Monitoring

```python
import pynlm

# Setup simulation
brain = pynlm.createBrain(pynlm.createConfigBuilder()
    .setBrainConfig(neuron_count=1000, region_count=3)
    .setLearningConfig(learning_rate=0.01)
    .build())

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Create profiler
profiler = brain.create_profiler()

# Run simulation
with agent:
    agent.initialize(world)
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Profiler would record step internally
        # profiler.record_step()

# Collect and report performance
stats = profiler.collect()
print("Performance Analysis:")
print(profiler.report())
```

### Appendix B: Configuration Examples

#### JSON Configuration

```json
{
    "brain.neuron_count": 1000,
    "brain.region_count": 3,
    "brain.connection_probability": 0.1,
    "plasticity.stdp.learning_rate": 0.01,
    "plasticity.stdp.tau_plus": 20.0,
    "plasticity.stdp.tau_minus": 20.0,
    "neuromod.curiosity.enable": true,
    "neuromod.novelty.enable": true
}
```

#### Configuration Builder Usage

```python
import pynlm

# Load from JSON file
config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.json")

# Create config from builder
config = (pynlm.createConfigBuilder()
          .setBrainConfig(neuron_count=1000, region_count=3)
          .setLearningConfig(learning_rate=0.01, stdp_rate=0.001)
          .setNeuromodulationConfig(curiosity=True, novelty=True)
          .build())

# Use the config
brain = pynlm.createBrain(config)
```

### Appendix C: Error Handling Examples

#### Handling Brain Errors

```python
import pynlm

brain = pynlm.createBrain(config)

try:
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        # ... simulation code ...
except pynlm.BrainError as e:
    print(f"Brain operation failed: {e}")
    # Handle brain-specific errors
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
    # Handle configuration errors
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
    # Handle other NLM errors
except Exception as e:
    print(f"Unexpected error: {e}")
    # Handle unexpected errors
```

#### Debug Information

```python
import pynlm

brain = pynlm.createBrain(config)

try:
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        # ... simulation code ...
except pynlm.BrainError as e:
    print("Brain error details:")
    print(f"  Error message: {e}")
    print(f"  Brain state: total_neurons={brain.getTotalNeuronCount()}")
    print(f"  Brain state: total_spikes={brain.getTotalSpikeCount()}")
    print(f"  Brain state: firing_neurons={brain.getFiringNeuronCount()}")
except Exception as e:
    print(f"Error: {e}")
```

### Appendix D: Performance Tips

#### 1. Use Batch Operations

```python
# Instead of: for i in range(1000): brain.step(i)
brain.run_steps(1000)
```

#### 2. Warm Up Before Use

```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.warm_up(100)  # Let neurons settle
```

#### 3. Monitor Performance

```python
brain = pynlm.createBrain(config)
profiler = brain.create_profiler()

# ... run simulation ...

stats = profiler.collect()
if stats.steps_completed > 0:
    avg_time = stats.total_time_ms / stats.steps_completed
    print(f"Average time per step: {avg_time:.2f} ms")
```

#### 4. Use Efficient Configuration

```python
# Use builder for efficient configuration
config = pynlm.createConfigBuilder() \
    .setBrainConfig(neuron_count=1000) \
    .setLearningConfig(learning_rate=0.01) \
    .build()
```

### Appendix E: API Reference

#### Main Classes

- **Brain**: Neural simulation brain with enhanced API
- **Config**: Configuration class with JSON/YAML support
- **ConfigBuilder**: Configuration builder pattern
- **AgentBrain**: Agent brain interface with context manager
- **SimpleWorld**: Simple 2D world for simulation
- **BrainProfiler**: Performance profiler for brain simulations

#### Key Methods

##### Brain Class

| Method | Description | Old Equivalent |
|--------|-------------|----------------|
| `__enter__` | Context manager entry | `brain.initialize()` |
| `__exit__` | Context manager exit | `brain.reset()` |
| `run_steps(n)` | Run n steps efficiently | `for i in range(n): brain.step(i)` |
| `warm_up(n)` | Reset, initialize, run n steps | Manual sequence |
| `reset_and_initialize()` | Reset and initialize in one call | Manual sequence |
| `get_brain_stats()` | Get comprehensive statistics | Multiple individual calls |
| `create_profiler()` | Create performance profiler | Manual timing setup |
| `configure_neurons(n, r)` | Configure brain with neurons/regions | Multiple `config.set()` calls |
| `configure_learning(r, p)` | Configure learning parameters | Multiple `config.set()` calls |

##### Config Class

| Method | Description | Old Equivalent |
|--------|-------------|----------------|
| `parseSimpleJSON(content)` | Parse JSON content | `loadFromFile()` (new implementation) |
| `to_dict()` | Convert config to dictionary | Manual extraction |
| `validate(schema)` | Validate configuration | Manual validation |

##### AgentBrain Class

| Method | Description | Old Equivalent |
|--------|-------------|----------------|
| `__enter__` | Context manager entry | `agent.initialize()` |
| `__exit__` | Context manager exit | `agent.reset()` |
| `get_agent_stats()` | Get comprehensive statistics | Multiple individual calls |

##### SimpleWorld Class

| Method | Description | Old Equivalent |
|--------|-------------|----------------|
| `reset_and_configure(w, h, vw, vh)` | Reset and configure in one call | Manual sequence |
| `create_episode(dt)` | Create episode function | Manual loop creation |

#### Global Functions

| Function | Description |
|----------|-------------|
| `pynlm.createDefaultConfig()` | Create default configuration |
| `pynlm.createConfigBuilder()` | Create configuration builder |
| `pynlm.createBrain(config)` | Create brain with configuration |
| `pynlm.createSimpleWorld()` | Create simple world |
| `pynlm.createAgentBrain(brain)` | Create agent brain interface |
| `pynlm.run_agent_simulation(brain, world, agent, steps, dt)` | Run complete simulation |

#### Custom Exceptions

| Exception | Description |
|-----------|-------------|
| `NLMError` | Base NLM exception |
| `ConfigError` | Configuration-specific exception |
| `BrainError` | Brain-specific exception |
| `WorldError` | World-specific exception |

## Conclusion

The enhanced NLM Python API represents a significant improvement over the original implementation, providing:

1. **Better usability** with context managers and builder pattern
2. **Improved performance** with batch operations and optimized calls
3. **Enhanced debugging** with custom exceptions and profiling
4. **Greater flexibility** with new convenience methods and Pythonic API
5. **Maintained compatibility** with existing code while adding new capabilities

The enhanced API makes it easier and more enjoyable to work with NLM for both beginners and advanced users. Whether you're running simple neural simulations or complex agent-based experiments, the enhanced API provides the tools you need to be productive and effective.

Start using the enhanced NLM Python API today and experience the difference!

---

*Version: 2.0.0-enhanced*
*Last Updated: October 15, 2026*
*Copyright: MIT License*