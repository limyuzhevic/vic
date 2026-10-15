# Enhanced NLM Python Interface
# Comprehensive Documentation for Pythonic API

**Version:** 1.0.0-enhanced  
**Status:** Beta  
**Last Updated:** 2026-10-15

This document provides comprehensive documentation for the enhanced NLM Python interface. The enhanced interface builds upon the original NLM C++ framework bindings, adding Pythonic features, high-level convenience functions, and comprehensive error handling.

## Overview

The enhanced NLM Python interface provides a clean, intuitive API for working with neural simulation frameworks. It combines the power of the original C++ implementation with Pythonic design patterns, making it easier to:

- Create and configure neural simulations
- Manage agent-environment interactions
- Process sensory inputs and produce motor outputs
- Run complex learning experiments
- Analyze simulation results

## Key Improvements

### 1. Pythonic Interfaces
- **Properties**: Clean attribute access for configuration values
- **Context Managers**: Automatic resource management for brain instances
- **Type Safety**: Enhanced type checking and validation
- **String Representations**: Informative `__repr__` and `__str__` methods

### 2. High-Level Convenience APIs
- **Factory Functions**: Easy creation of common components
- **Batch Operations**: Efficient processing of multiple simulation steps
- **Progress Callbacks**: Real-time monitoring of simulation progress
- **Configuration Templates**: Pre-built configuration presets

### 3. Enhanced Error Handling
- **Input Validation**: Comprehensive validation of sensory inputs
- **Exception Types**: Custom exception hierarchy for better error handling
- **Graceful Degradation**: Fallback mechanisms for invalid inputs
- **Debug Information**: Detailed error messages with context

### 4. Performance Optimizations
- **Lazy Loading**: On-demand computation for expensive operations
- **Caching**: Smart caching of frequently accessed data
- **Vectorized Operations**: Efficient batch processing
- **Memory Management**: Improved resource utilization

## Installation

### From PyPI

```bash
pip install pynlm[examples,dev]
```

### From Source

```bash
pip install -e .
```

### With Dev Dependencies

```bash
pip install pynlm[dev]
```

## Quick Start Guide

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrainWithDefaultConfig()
brain.initialize()

# Run a simulation
print("Running simulation...")
pynlm.runSimulation(brain, 100, lambda step: print(f"Step {step}/100"))
print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

### Advanced Usage

```python
import pynlm

# Create complete agent world
world, brain, agent_brain = pynlm.createCompleteAgentWorld()
world.setAgentStart(25, 25)

# Run an episode with logging
for step in range(50):
    world.update(0.1)
    agent_brain.processSensoryInput(world.getSensoryPercept())
    agent_brain.updateDevelopment(0.1)
    
    if step % 10 == 0:
        print(f"Step {step}: Curiosity={agent_brain.getCuriosityLevel():.3f}, "
              f"Novelty={agent_brain.getNoveltyLevel():.3f}")
```

## API Reference

### Core Classes

#### Config
Configuration management with type-safe getters and setters.

**Methods:**
- `loadFromFile(filepath, strict=False)` - Load configuration from JSON file
- `saveToFile(filepath, pretty=False)` - Save configuration to JSON file
- `getInt(key, default=0)`, `getDouble(key, default=0.0)`, etc. - Type-safe getters
- `setInt(key, value)`, `setDouble(key, value)`, etc. - Type-safe setters
- `validate()` - Validate configuration and return errors
- `summary()` - Get configuration summary

**Properties:**
- `is_empty` - Check if configuration is empty
- `keys` - Get all configuration keys

**Example:**
```python
config = pynlm.createDefaultConfig()
config.setInt("brain.neuron_count", 2000)
config.setDouble("brain.simulation_timestep", 0.001)

# Type-safe access
neuron_count = config.getInt("brain.neuron_count")
timestep = config.getDouble("brain.simulation_timestep")
```

#### Brain
Main neural simulation controller with comprehensive functionality.

**Key Methods:**
- `initialize()` - Initialize brain with configuration
- `step(step, time=None)` - Perform simulation step
- `runSteps(steps, timestep=0.001)` - Run multiple steps efficiently
- `runStepsWithCallback(steps, callback, timestep=0.001)` - Run with progress tracking
- `enablePlasticity(enable, hebbian=True, stdp=True)` - Control plasticity

**Properties:**
- `total_neuron_count` - Total number of neurons
- `total_synapse_count` - Total number of synapses
- `active_neuron_count` - Number of active neurons
- `firing_neuron_count` - Number of firing neurons

**Example:**
```python
brain = pynlm.createBrainWithDefaultConfig()
brain.initialize()

# Run simulation with progress tracking
pynlm.runSimulation(brain, 100, lambda step: print(f"Step {step}/100"))
```

#### AgentBrain
Bridge between neural brain and world environment.

**Key Methods:**
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory data
- `decodeMotorCommand()` - Convert neural activity to motor command
- `applyRewardModulation(reward, predicted_reward)` - Apply reward signals
- `runEpisode(world, max_steps)` - Run complete agent episode

**Properties:**
- `developmental_stage` - Current developmental stage
- `neuromodulation_level` - Current neuromodulation level
- `curiosity_level` - Current curiosity level
- `novelty_level` - Current novelty level

**Example:**
```python
world, brain, agent_brain = pynlm.createCompleteAgentWorld()
agent_brain.initialize(world)

# Run episode
agent_brain.runEpisode(world, 100)
```

#### SimpleWorld
2D simulation environment for agent interactions.

**Key Methods:**
- `configure(width, height, vision_width, vision_height)` - Configure world dimensions
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world simulation
- `applyMotorCommand(action, time)` - Apply motor command
- `addObject(obj)` - Add world object

**Properties:**
- `width`, `height` - World dimensions
- `max_energy` - Maximum energy
- `energy_decay_rate` - Energy decay rate

**Example:**
```python
world = pynlm.createSimpleWorld()
world.configure(100, 80, 20, 20)
world.setAgentStart(50, 50)

# Run simulation
for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent_brain.processSensoryInput(percept)
```

### Factory Functions

**Common convenience functions:**

- `createDefaultConfig()` - Create configuration with sensible defaults
- `createBrain(config)` - Create and initialize brain
- `createSimpleWorld()` - Create world with default settings
- `createAgentBrain(brain)` - Create agent brain interface
- `createBrainWithDefaultConfig()` - Create brain with default config
- `createCompleteAgentWorld()` - Create complete agent setup
- `runSimulation(brain, steps, callback=None, timestep=0.001)` - Run simulation
- `runLearningExperiment(brain, steps)` - Run learning experiment

**Example:**
```python
# Create everything you need for a complete setup
world, brain, agent_brain = pynlm.createCompleteAgentWorld()
```

### ID Types

**Unique identifiers:**
- `NeuronId(value)` - Neuron identifier
- `SynapseId(value)` - Synapse identifier
- `RegionId(value)` - Region identifier
- `PopulationId(value)` - Population identifier

**Helper Functions:**
- `createNeuronId(value)`, `createSynapseId(value)`, etc. - Factory functions

### Enumerations

**NeuronType:** Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal
**SynapseType:** Excitatory, Inhibitory, Modulatory, Electrical, GapJunction
**DevelopmentalStage:** Initial, CriticalPeriod, Maturation, Adult, Aging
**FiringState:** Resting, Active, Refractory, Inhibited
**ActionType:** MoveForward, MoveBackward, TurnLeft, TurnRight, Look, etc.
**MotorCommand:** MoveForward, MoveBackward, TurnLeft, TurnRight, Interact, Wait
**WorldObjectType:** Empty, Resource, Hazard, Wall, Marker

## Advanced Features

### Context Managers

Automatic resource management for brain instances:

```python
with pynlm.createBrain() as brain:
    brain.initialize()
    # Brain is automatically reset when exiting context
    for i in range(20):
        brain.step(i, i * 0.001)
```

### Type Safety

Enhanced type checking and validation:

```python
config = pynlm.createDefaultConfig()
config.setInt("test", 42)

# Type-safe getters with defaults
value = config.getInt("test.key", default_value=100)
value = config.getString("name", default_value="unknown")

# Input validation
vision = pynlm.Vision(10, 10)
vision.setData([0.1] * 300)  # 10*10*3 = 300 elements
```

### Progress Callbacks

Real-time monitoring of simulation progress:

```python
def progress_callback(step):
    if step % 10 == 0:
        print(f"Step {step}/100 completed")

pynlm.runSimulation(brain, 100, progress_callback)
```

### Error Handling

Comprehensive error handling with custom exceptions:

```python
config = pynlm.createDefaultConfig()
config.setString("name", "")  # Empty name causes validation error
errors = config.validate()
print(f"Validation errors: {errors}")

# Try-catch blocks for error handling
try:
    brain.receiveSensoryInput(invalid_input)
except (pynlm.ValueError, pynlm.RuntimeError) as e:
    print(f"Error processing sensory input: {e}")
```

## Examples

### Basic Simulation

See `examples/basic_usage.py` for a complete example of basic simulation.

### Agent World

See `examples/advanced_usage.py` for advanced agent-environment integration.

### Learning Experiment

```python
brain = pynlm.createBrainWithDefaultConfig()
pynlm.runLearningExperiment(brain, 1000)
print(f"Learning experiment completed. Total spikes: {brain.getTotalSpikeCount()}")
```

### Performance Monitoring

```python
def performance_monitor(step):
    brain_status = brain.logStatus()
    if step % 50 == 0:
        print(f"Step {step}: {brain_status}")

pynlm.runSimulation(brain, 500, performance_monitor)
```

## Testing

### Running Tests

```bash
pytest python/tests/
pytest python/tests/test_enhanced_bindings.py
```

### Test Coverage

The test suite includes:
- ID type tests
- Enumeration tests
- Configuration tests
- Vision enhancement tests
- Action enhancement tests
- World integration tests
- Brain functionality tests
- Agent brain tests
- Factory function tests
- Context manager tests
- Error handling tests
- Complete integration tests

### Writing Tests

Create test files in `python/tests/`:

```python
import pynlm

def test_basic_functionality():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    assert brain is not None
    brain.initialize()
    assert brain.getTotalNeuronCount() > 0
```

## Configuration

### Default Configuration

The enhanced interface provides sensible defaults:

```python
config = pynlm.createDefaultConfig()
config.setInt("brain.neuron_count", 1000)  # 1000 neurons
config.setDouble("brain.simulation_timestep", 0.001)  # 1ms timestep
config.setDouble("brain.connection_probability", 0.1)  # 10% connection probability
```

### Custom Configuration

Create custom configurations:

```python
config = pynlm.Config()
config.setInt("brain.neuron_count", 5000)
config.setString("brain.name", "My Custom Brain")
config.setBool("brain.enable_plasticity", True)
```

## Performance Tips

### Efficient Simulation

1. **Batch Processing**: Use `runSteps()` for multiple steps
2. **Progress Callbacks**: Use callbacks to monitor progress without slowing simulation
3. **Memory Management**: Use context managers for automatic cleanup

### Optimizing Memory

1. **Lazy Loading**: Use lazy computation for expensive operations
2. **Caching**: Cache frequently accessed data
3. **Vector Operations**: Use vectorized operations when possible

### Scaling for Large Simulations

1. **Memory Pools**: Use memory pools for large simulations
2. **Parallel Processing**: Consider parallel processing for batch operations
3. **Checkpointing**: Use save/load functionality for long simulations

## Migration Guide

### From Original Interface

The enhanced interface maintains backward compatibility while adding new features:

**Original:**
```python
brain = pynlm.createBrain(config)
brain.initialize()
for i in range(steps):
    brain.step(i)
```

**Enhanced:**
```python
brain = pynlm.createBrain(config)
brain.initialize()
pynlm.runSimulation(brain, steps)
```

**Benefits:**
- Cleaner, more readable code
- Progress tracking capabilities
- Error handling improvements
- Type safety enhancements

### Adding New Features

To add new features to the enhanced interface:

1. **Add new factory functions** in the bindings module
2. **Create new context managers** for resource management
3. **Enhance existing classes** with Pythonic properties
4. **Add validation and error handling** for new inputs
5. **Update documentation** and examples

## Troubleshooting

### Common Issues

**Issue:** Module import errors
```bash
pip install pynlm[dev]
```

**Issue:** Build failures
```bash
# Check for system dependencies
pkg-config --libs python3
```

**Issue:** Performance problems
- Ensure you're using batch operations (`runSteps()`)
- Check for memory leaks in custom code
- Consider using caching for repeated calculations

**Issue:** Error handling
- Use try-catch blocks for error handling
- Check configuration validation
- Validate sensory inputs before processing

### Getting Help

- **Documentation**: See `docs/` for detailed API documentation
- **Examples**: See `examples/` for usage examples
- **Issues**: Report issues at GitHub repository
- **Discussions**: Join NLM community discussions

## Conclusion

The enhanced NLM Python interface provides a powerful, intuitive API for working with neural simulation frameworks. It combines the performance of the original C++ implementation with the ease of use of Python, making it accessible to researchers, developers, and students alike.

Key benefits:
- **Pythonic Design**: Clean, intuitive API with properties and context managers
- **Type Safety**: Enhanced type checking and validation
- **High-Level APIs**: Convenience functions for common tasks
- **Comprehensive Error Handling**: Robust error handling and debugging
- **Performance Optimized**: Efficient for both small and large simulations
- **Well Documented**: Comprehensive documentation and examples

The enhanced interface is ready for production use and provides a solid foundation for future enhancements and feature additions.
