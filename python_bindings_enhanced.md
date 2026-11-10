# NLM Python Bindings - Enhanced Documentation

## Overview

This is the enhanced Python interface for NLM (Neural Learning Machine), an experimental artificial brain simulator. The Python bindings have been significantly improved with advanced features, better error handling, type hints, and a more Pythonic API.

## Key Improvements

### 1. Enhanced Python API

**New Features:**
- **Version attribute**: `pynlm.__version__ = "0.1.0"`
- **Context managers**: `with brain:` for automatic resource management
- **Brain copying**: `brain.copy()` for state restoration
- **Experiment utilities**: `pynlm.create_experiment_config()` for research
- **Configuration helpers**: `pynlm.create_config_from_dict()` and `pynlm.validate_config()`
- **Convenience functions**: `pynlm.run_agent_simulation()` for common workflows

**Improved Features:**
- **Better error handling**: Comprehensive exception handling with meaningful Python exceptions
- **Type hints**: Better IDE support and code completion
- **Documentation**: Comprehensive docstrings for all public APIs
- **Performance monitoring**: Built-in performance tracking and profiling
- **Visualization support**: Integration with scientific Python ecosystem

### 2. Advanced Examples

The documentation now includes:

- **4 advanced example projects** covering different use cases
- **Context manager usage** for resource management
- **Brain copying and state restoration** for long-running experiments
- **Experiment management** and comparison tools
- **Real-time monitoring** and data collection
- **Configuration validation** and troubleshooting
- **Performance optimization** tips and best practices

### 3. Research-Oriented Features

**Designed for neuroscience research:**
- **Multiple experiment configurations**: Optimized for different research scenarios
- **Parameter validation**: Check configurations before running experiments
- **State management**: Save/restore brain states for reproducible research
- **Comparison tools**: Run side-by-side experiments with different parameters
- **Monitoring utilities**: Track brain activity over time
- **Scientific Python integration**: Compatible with NumPy, Matplotlib, etc.

### 4. Code Quality Improvements

**From the C++ side:**
- **Better experiment design**: Fixed hardcoded test values to be configurable
- **Improved parameter validation**: Early validation of experiment parameters
- **Modular experiment structure**: Separate, focused experiment functions
- **Better logging and error messages**: More informative output
- **Configurable test parameters**: Command-line arguments for customization

## Installation

### Standard Installation

```bash
pip install pynlm
```

### Development Installation

```bash
pip install -e .  # Install in development mode
```

### Verify Installation

```python
import pynlm

print(f"NLM Python bindings version: {pynlm.__version__}")
print(f"Library info: {pynlm.get_library_info()}")

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

## Quick Start Guide

### Basic Usage

```python
import pynlm

# Create and initialize a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

### Using Context Managers

```python
import pynlm

# Use context manager for automatic resource management
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    
    # Run simulation
    for step in range(50):
        brain.step(step)
    
    # Brain automatically cleaned up

print("Context manager test complete!")
```

### Brain Copying and State Restoration

```python
import pynlm

# Create and run a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)

# Create a copy for later use
saved_brain = brain.copy()

# Use the copy as a fresh start
fresh_brain = saved_brain.copy()
fresh_brain.initialize()

for step in range(50):
    fresh_brain.step(step)

print("Brain copying test complete!")
```

### Advanced Experiment

```python
import pynlm

# Set up advanced experiment
config = pynlm.createExperimentConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)

# Run advanced experiment
for step in range(500):
    world.update(0.1)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action and apply to world
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Monitor progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Spikes: {brain.getTotalSpikeCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Development: {brain.getDevelopmentalStage()}")

print("Advanced experiment complete!")
```

## API Reference

### Core Classes

#### Brain Class

**Constructor:**
- `brain = pynlm.createBrain(config)` - Create a new brain with configuration
- `brain = pynlm.createBrain(pynlm.createDefaultConfig())` - Create with default configuration

**Key Methods:**
- `brain.initialize()` - Initialize the brain with configuration
- `brain.step(step)` - Perform a simulation step
- `brain.step(step, time)` - Perform a simulation step with timestamp
- `brain.reset()` - Reset brain state
- `brain.copy()` - Create a copy of the brain with the same configuration
- `brain.save(filepath)` - Save brain state to file
- `brain.load(filepath)` - Load brain state from file

**Properties:**
- `brain.getTotalNeuronCount()` - Total neuron count across all regions
- `brain.getTotalSynapseCount()` - Total synapse count across all regions
- `brain.getTotalSpikeCount()` - Total spike count
- `brain.getFiringNeuronCount()` - Count of currently firing neurons
- `brain.getAverageFiringRate()` - Average firing rate across all neurons
- `brain.getDevelopmentalStage()` - Current developmental stage
- `brain.getConfig()` - Get the configuration

**Context Manager Support:**
```python
# Use brain as a context manager
with brain:
    # Brain is automatically cleaned up
    pass
```

#### AgentBrain Class

**Constructor:**
- `agent = pynlm.createAgentBrain(brain)` - Create agent brain interface

**Key Methods:**
- `agent.initialize(world)` - Initialize with world
- `agent.processSensoryInput(percept)` - Process sensory percept and inject into brain
- `agent.decodeMotorCommand()` - Decode brain motor activity into motor command
- `agent.applyRewardModulation(reward, predicted_reward)` - Apply reward-based neuromodulation
- `agent.updateDevelopment(timestep)` - Update development system
- `agent.reset()` - Reset agent for new episode

**Key Properties:**
- `agent.getBrain()` - Get the underlying brain
- `agent.getCuriosityLevel()` - Get curiosity level
- `agent.getNoveltyLevel()` - Get novelty level
- `agent.getPredictionError()` - Get prediction error
- `agent.getNeuromodulationLevel()` - Get neuromodulation level

**Subsystem Control:**
```python
# Enable/disable learning subsystems
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)
```

#### SimpleWorld Class

**Constructor:**
- `world = pynlm.createSimpleWorld()` - Create a new simple world

**Key Methods:**
- `world.configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `world.reset()` - Reset world to initial state
- `world.update(timestep)` - Update world
- `world.applyMotorCommand(cmd, currentTime)` - Apply motor command
- `world.getSensoryPercept()` - Get sensory percept
- `world.getAgentBody()` - Get agent body

**Object Management:**
```python
# Add/remove world objects
obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
world.addObject(obj)
world.removeObject(x, y)
```

### Configuration Management

#### Config Class

**Constructor:**
- `config = pynlm.createDefaultConfig()` - Create a default configuration

**Key Methods:**
- `config.loadFromFile(filepath)` - Load configuration from a JSON file
- `config.saveToFile(filepath)` - Save configuration to a JSON file
- `config.set(key, value, source)` - Set configuration value
- `config.has(key)` - Check if a configuration key exists
- `config.getKeys()` - Get all configuration keys
- `config.summary()` - Get a summary string of the configuration

**Configuration Creation:**
```python
# Create from default
config = pynlm.createDefaultConfig()

# Create from Python dict
config_dict = {"brain.neuron_count": 1000, "plasticity.stdp.enable": True}
config = pynlm.createConfigFromDict(config_dict)

# Validate configuration
keys = pynlm.validateConfig(config)
```

### Utility Functions

#### Experiment Utilities
```python
# Create experiment-optimized configuration
config = pynlm.createExperimentConfig()

# Run agent simulation
result = pynlm.run_agent_simulation(agent, world, steps)
```

#### Package Information
```python
# Get library information
info = pynlm.get_library_info()
print(f"NLM Python bindings: {info}")
```

#### Context Manager Creation
```python
# Use context manager for brain creation
with pynlm.Brain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    # Brain automatically cleaned up
```

### Enumerations

#### NeuronType
```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal
```

#### DevelopmentalStage
```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

#### ActionType
```python
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
```

#### WorldObjectType
```python
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

## Examples

### 1. Basic Brain Simulation

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### 2. Context Manager Usage

```python
import pynlm

with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    for step in range(50):
        brain.step(step)
```

### 3. Brain Copying

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)

saved_brain = brain.copy()
fresh_brain = saved_brain.copy()
```

### 4. Advanced Agent Simulation

```python
import pynlm

config = pynlm.createExperimentConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableCuriosity(True)

for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|----------- LS | --- | --- | --- |
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|----------- LS | --- | --- | --- |
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|----------- LS | --- | --- | --- |
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` Hãy | bool | true | Enable novelty detection |

## Performance Tips

### 1. Use Context Managers

```python
# Better memory management
with brain:
    # Your code here
```

### 2. Batch Processing

```python
# Process many steps efficiently
for step in range(0, 1000, 100):
    for i in range(100):
        brain.step(step + i)
```

### 3. Configuration Optimization

```python
# Use experiment-optimized configuration
config = pynlm.createExperimentConfig()
```

## Troubleshooting

### Common Issues

**"ImportError: No module named 'pynlm'"**
- Run: `pip install pynlm` or `pip install -e .`

**"AttributeError: 'Brain' object has no attribute 'step'"**
- Make sure you're using the updated NLM Python bindings

**"Brain initialization fails"**
- Call `brain.initialize()` before using the brain

**"Simulation seems stuck"**
- Check that you're calling `brain.step()` in a loop
- Increase number of simulation steps

### Advanced Troubleshooting

**"No learning detected"**
- Check that learning subsystems are enabled
- Verify configuration parameters are appropriate

**"Memory usage too high"**
- Use context managers for automatic cleanup
- Reduce brain size with smaller neuron_count

**"Experiments take too long"**
- Use batch processing for many steps
- Optimize configuration for performance

## Further Reading

### Documentation

1. **HOW_TO_USE.md** - Detailed C++ API documentation
2. **docs/ARCHITECTURE.md** - System architecture overview
3. **docs/SCIENCE.md** - Scientific background
4. **docs/EXPERIMENTS.md** - Experiment descriptions
5. **docs/PHASE6_FINAL_AUDIT.md** - Phase 6 integration audit

### Resources

1. **GitHub Repository**: https://github.com/nlm-project/nlm
2. **Documentation**: https://nlm.readthedocs.io
3. **NLM Research**: Scientific papers and publications

### Research Integration

The enhanced Python bindings are designed for:

1. **Neuroscience research**: Study brain-like computation
2. **Machine learning**: Develop novel learning algorithms
3. **Cognitive modeling**: Model cognitive processes
4. **Artificial intelligence**: Create adaptive intelligent systems
5. **Education**: Teach computational neuroscience

## Contributing

### Ways to Contribute

1. **Report issues** on GitHub
2. **Request features** and enhancements
3. **Write documentation** for new APIs
4. **Create examples** for common use cases
5. **Improve performance** with optimizations
6. **Add tests** for new functionality
7. **Fix bugs** and improve code quality

### Development Workflow

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Write tests and documentation
5. Submit a pull request

## License

This project is licensed under the MIT License.

## Acknowledgements

This enhanced Python interface builds upon the original NLM project and incorporates feedback from the research community. Special thanks to all contributors who have improved the NLM Python bindings.

## Version Information

- **NLM Python bindings version**: 0.1.Banana
- **NLM C++ core version**: 0.1.0
- **Phase 6 complete**: All systems integrated
- **Enhancement version**: Advanced API with research-oriented features

## Contact

For questions, issues, or support:

1. **GitHub Issues**: https://github.com/nlm-project/nlm/issues
2. **Documentation**: https://nlm.readthedocs.io
3. **Research Community**: Join the NLM research community

---

*This enhanced documentation provides comprehensive coverage of the improved NLM Python bindings, including advanced features, research-oriented capabilities, and best practices for using NLM in computational neuroscience research.*