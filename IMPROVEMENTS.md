# NLM Python Bindings - Improvements

## Overview

This document describes the improvements made to the NLM (Neural Learning Machine) Python bindings, making the API more Pythonic, robust, and feature-rich while maintaining backward compatibility.

## Key Improvements

### 1. Enhanced Configuration API

The configuration system now supports method chaining and dictionary-based operations:

```python
import pynlm as nlm

# Method chaining for fluent configuration
config = nlm.createConfig() \
    .withNeuronCount(1000) \
    .withLearningRate(0.001) \
    .withDevelopment(True) \
    .withCuriosity(True) \
    .withRewardModulation(True)

# Dictionary operations
config = nlm.createConfig()
config_dict = config.to_dict()  # Convert to Python dict
config2 = nlm.createConfig()
config2.from_dict({"brain.neuron_count": 2000})  # Load from dict

# Simulation configuration factory
sim_config = nlm.createConfig().createSimulationConfig(
    neuronCount=500,
    regionCount=2,
    learningRate="0.001",
    enableDevelopment=True
)
```

### 2. Context Managers

Automatic resource management with context managers:

```python
import pynlm as nlm

# Create components
config = nlm.createConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Use context manager for automatic cleanup
with nlm.brain_context(brain) as brain_ctx:
    brain_ctx.brain.step(0)
    # Cleanup happens automatically
```

### 3. Simulation Utilities

High-level simulation functions for common scenarios:

```python
import pynlm as nlm

# Create a foraging environment
config, world, agent = nlm.SimulationUtils.createForagingEnvironment(
    worldWidth=20,
    worldHeight=20,
    visionWidth=8,
    visionHeight=8,
    numFoodResources=10
)

# Run a complete episode
brain, agent = nlm.SimulationUtils.runEpisode(
    config, world, numSteps=100, enableLearning=True
)

# Measure performance metrics
perf_metrics = nlm.SimulationUtils.measurePerformance(
    brain, world, agent, numSteps=50
)
print(f"Average firing rate: {perf_metrics.averageFiringRate:.2f} Hz")
print(f"Curiosity level: {perf_metrics.curiosityLevel:.3f}")
```

### 4. Enhanced Vision Processing

Improved vision sensor with analysis capabilities:

```python
import pynlm as nlm

# Create vision sensor
vision = nlm.Vision(8, 8, 3)

# Set vision data (e.g., from camera input)
data = [0.1, 0.5, 0.9] * (8 * 8 * 3)
vision.setData(data)

# Convert to Python-friendly format for visualization
matrix = vision.to_python_matrix()
# matrix is now a list of lists suitable for matplotlib

# Detect objects in vision (simple edge detection)
objects = vision.detectObjects()
print(f"Detected edges: {objects['edges_count']}")
```

### 5. Improved Error Handling

Better exception types with clear error messages:

```python
import pynlm as nlm

try:
    config = nlm.createConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    brain.step(0)
    
    # Your code here
    
except nlm.ConfigError as e:
    print(f"Configuration error: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except nlm.NLMError as e:
    print(f"General NLM error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### 6. Performance Monitoring

Built-in performance tracking utilities:

```python
import pynlm as nlm

# Create and run simulation
config = nlm.createConfig().createSimulationConfig(neuronCount=1000)
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation while monitoring performance
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Get performance metrics
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Development stage: {agent.getDevelopmentalStage()}")
```

## Installation

The improved bindings are available as part of the main `pynlm` package:

```bash
pip install pynlm
```

For development, build from source:

```bash
cd /path/to/nlm
pip install -e .
```

## Migration Guide

### Backward Compatibility

The improvements maintain full backward compatibility with existing code. All existing API methods continue to work exactly as before.

### New Features

If you're using the old API, you can now:

1. **Use method chaining** for more fluent configuration:
   ```python
   # Old way
   config = nlm.createConfig()
   config.set("brain.neuron_count", 1000)
   
   # New way
   config = nlm.createConfig().withNeuronCount(1000)
   ```

2. **Use context managers** for automatic cleanup:
   ```python
   # Old way (manual cleanup)
   brain.initialize()
   brain.step(0)
   # cleanup...
   
   # New way (automatic cleanup)
   with nlm.brain_context(brain) as brain_ctx:
       brain_ctx.brain.initialize()
       brain_ctx.brain.step(0)
   ```

3. **Use enhanced configuration**:
   ```python
   # Old way (verbose)
   config = nlm.createConfig()
   config.set("brain.neuron_count", 1000)
   config.set("plasticity.stdp.learning_rate", 0.001)
   
   # New way (fluent)
   config = nlm.createConfig().withNeuronCount(1000).withLearningRate(0.001)
   ```

## Performance Considerations

### Configuration

- **Method chaining** is as efficient as the old approach
- **Dictionary operations** create temporary objects but are convenient
- **Validation** adds minimal overhead

### Memory Management

- Context managers ensure proper cleanup even if exceptions occur
- Shared pointers prevent memory leaks
- Python garbage collection handles remaining cleanup

### Simulation Performance

- All new features maintain the same performance characteristics
- Context managers have minimal overhead when not used
- Simulation utilities are optimized for common scenarios

## Testing

Run the test suite to verify the improvements:

```bash
python -m pytest tests/ -v
```

Or run the demonstration script:

```bash
python demo_improved_bindings.py
```

## Example Scripts

Several example scripts are included:

1. **`demo_improved_bindings.py`** - Complete demonstration of new features
2. **`tests/test_pynlm.py`** - Comprehensive test suite
3. **`easy_usage.md`** - Beginner-friendly usage guide
4. **`HOW_TO_USE.md`** - Detailed technical documentation

## Advanced Features

### Custom Simulation Environments

Create custom environments using the simulation utilities:

```python
import pynlm as nlm

# Create a custom environment with obstacles and rewards
def create_maze_environment(width=20, height=20):
    config = nlm.createConfig().withNeuronCount(500)
    world = nlm.createSimpleWorld()
    world.configure(width, height, 6, 6)
    world.reset()
    
    # Add maze walls
    for x in range(width):
        world.addObject(nlm.WorldObject(x, 0, nlm.WorldObjectType.Wall))
        world.addObject(nlm.WorldObject(x, height-1, nlm.WorldObjectType.Wall))
    
    for y in range(height):
        world.addObject(nlm.WorldObject(0, y, nlm.WorldObjectType.Wall))
        world.addObject(nlm.WorldObject(width-1, y, nlm.WorldObjectType.Wall))
    
    # Add food and poison
    world.addObject(nlm.WorldObject(5, 5, nlm.WorldObjectType.Resource, 10.0))
    world.addObject(nlm.WorldObject(15, 15, nlm.WorldObjectType.Hazard, -5.0))
    
    agent = nlm.createAgentBrain(nlm.createBrain(config))
    agent.initialize(world)
    
    return config, world, agent
```

### Performance Profiling

Profile simulation performance:

```python
import pynlm as nlm
import time

config = nlm.createConfig().withNeuronCount(2000)
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(30, 30, 8, 8)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Profile simulation
start_time = time.time()

for step in range(100):
    world.update(0.05)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    agent.decodeMotorCommand()
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())

end_time = time.time()
print(f"Simulation took {end_time - start_time:.2f} seconds")
print(f"Performance: {100 / (end_time - start_time):.1f} steps/second")
```

## Future Enhancements

The following enhancements are planned:

1. **WebSocket API** for real-time visualization
2. **Cloud deployment** capabilities
3. **Model import/export** functionality
4. **Advanced visualization tools**
5. **Multi-agent coordination**
6. **Distributed simulation** support

## Contributing

Contributions to the improved Python bindings are welcome! Please submit pull requests to the `main` branch.

### Development Setup

1. Install dependencies:
   ```bash
   pip install pytest numpy
   ```

2. Build the project:
   ```bash
   pip install -e .
   ```

3. Run tests:
   ```bash
   python -m pytest tests/ -v
   ```

4. Run demonstrations:
   ```bash
   python demo_improved_bindings.py
   ```

## Support

For support with the improved bindings, please:

1. Check the documentation in `docs/`
2. Review the examples in `examples/`
3. Check the test suite in `tests/`
4. Visit the NLM project website at https://github.com/nlm-project/nlm

## License

The NLM Python bindings are licensed under the MIT License. See `LICENSE` for details.

---

**Last Updated:** 2026-10-03
**Version:** 0.1.0 (enhanced bindings)

These improvements make the NLM Python bindings more accessible to Python developers while providing advanced features for experienced users.