# NLM Python API Documentation

## Overview

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python API. NLM is an experimental artificial developmental brain project that implements real spiking neural computation with biological inspiration.

## Installation

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install the Python bindings
pip install .

# Or install in development mode (recommended for development)
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

## Basic Usage Examples

### 1. Creating a Simple Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### 2. Running a Basic Simulation

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)
    
print("Simulation complete!")
```

### 3. Using the Agent-Brain Interface

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    motor_cmd = agent.decodeMotorCommand()
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Get reward and apply neuromodulation
    reward = 0.0  # In real usage, compute reward from world
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
```

## Classes and Methods

### Config Class

The `Config` class manages NLM system parameters.

#### Basic Methods

```python
config = pynlm.createDefaultConfig()

# Check if a parameter exists
if config.has("brain.neuron_count"):
    print("Neuron count is configured")

# Get all configuration keys
keys = config.getKeys()

# Modify configuration
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# Load/Save configuration
config.loadFromFile("config.json")
config.saveToFile("my_config.json")

# Get summary
print(config.summary())
```

#### Convenience Methods

```python
# Create validated configuration
config = pynlm.createDefaultConfig()

# Validate configuration
pynlm.validate_config(config)

# Safe configuration with defaults
config = pynlm.create_config_with_validation()
```

### Brain Class

The `Brain` class represents the central neural simulation brain.

#### Core Methods

```python
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run simulation steps
brain.step(current_step)
brain.step(current_step, current_time)

# Input/Output
brain.receiveSensoryInput(sensory_input)
action = brain.produceAction()

# State management
brain.reset()
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")

# Statistics
brain.getTotalNeuronCount()
brain.getTotalSynapseCount()
brain.getActiveNeuronCount()
brain.getFiringNeuronCount()
brain.getAverageFiringRate()
brain.getExcitationInhibitionRatio()

# Development
brain.getDevelopmentalStage()
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)

# Integration access
brain.getWorkingMemory()
brain.getEpisodicMemory()
brain.getPredictionSystem()
brain.getPlanner()
```

#### Advanced Methods

```python
# Multiple sensory inputs
brain.injectCurrent(neuron_id, current_value)
brain.injectCurrentToNeurons(neuron_type, current_value)

# Region management
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)

# Status and logging
brain.logStatus()
```

### AgentBrain Class

The `AgentBrain` class connects the NLM brain to the world interface.

#### Core Methods

```python
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
```

#### Convenience Methods

```python
# Easy sensory input processing
agent.processVisionInput(vision_data)
agent.processTouchInput(touch_data)
agent.processInternalInput(internal_data)

# Get statistics
stats = agent.getStats()
print(f"Curiosity: {stats['curiosity']}")
print(f"Novelty: {stats['novelty']}")
print(f"Dopamine: {stats['dopamine']}")

# Simple action selection with curiosity
motor_cmd, curiosity = agent.getActionWithThreshold(0.7)

# Run simulation step
agent.stepWithInput(percept, world, reward)
```

#### Subsystem Control

```python
# Enable/disable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check subsystem status
if agent.isRewardModulationEnabled():
    print("Reward modulation is active")
```

### SimpleWorld Class

The `SimpleWorld` class provides a 2D simulation environment.

#### Basic Usage

```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Update simulation
world.update(timestep=0.1)

# Get state
percept = world.getSensoryPercept()
body = world.getAgentBody()

# Agent interaction
world.applyMotorCommand(motor_cmd, current_time)
```

#### World Objects

```python
# Add world objects
obj = pynlm.WorldObject(x=5.0, y=5.0, type=pynlm.WorldObjectType.Resource, value=1.0)
world.addObject(obj)

# Remove objects
world.removeObject(x, y)

# Check valid positions
if world.isValidPosition(x, y):
    print("Position is valid")
```

## Enumerations

### NeuronType

```python
import pynlm

# Neuron types
neuron_types = [
    pynlm.NeuronType.Excitatory,
    pynlm.NeuronType.Inhibitory,
    pynlm.NeuronType.Modulatory,
    pynlm.NeuronType.Sensory,
    pynlm.NeuronType.Motor,
    pynlm.NeuronType.Internal
]
```

### DevelopmentalStage

```python
# Developmental stages
stages = [
    pynlm.DevelopmentalStage.Initial,
    pynlm.DevelopmentalStage.CriticalPeriod,
    pynlm.DevelopmentalStage.Maturation,
    pynlm.DevelopmentalStage.Adult,
    pynlm.DevelopmentalStage.Aging
]
```

### ActionType

```python
# Action types
actions = [
    pynlm.ActionType.MoveForward,
    pynlm.ActionType.MoveBackward,
    pynlm.ActionType.TurnLeft,
    pynlm.ActionType.TurnRight,
    pynlm.ActionType.Interact,
    pynlm.ActionType.Eat,
    pynlm.ActionType.Drink,
    pynlm.ActionType.Rest,
    pynlm.ActionType.Wait
]
```

### MotorCommand

```python
# Motor commands
commands = [
    pynlm.MotorCommand.MoveForward,
    pynlm.MotorCommand.MoveBackward,
    pynlm.MotorCommand.TurnLeft,
    pynlm.MotorCommand.TurnRight,
    pynlm.MotorCommand.Interact,
    pynlm.MotorCommand.Wait
]
```

### WorldObjectType

```python
# World object types
object_types = [
    pynlm.WorldObjectType.Empty,
    pynlm.WorldObjectType.Resource,
    pynlm.WorldObjectType.Hazard,
    pynlm.WorldObjectType.Wall,
    pynlm.WorldObjectType.Marker
]
```

## Configuration Parameters

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Error Handling

NLM provides comprehensive error handling with specific exception types:

### Exception Types

1. **NLMException**: Base exception for all NLM errors
2. **ConfigurationException**: Errors related to configuration
3. **BrainException**: Errors related to brain operations
4. **MemoryException**: Errors related to memory systems
5. **PlasticityException**: Errors related to plasticity mechanisms
6. **NeuromodulationException**: Errors related to neuromodulation

### Error Handling Best Practices

```python
import pynlm

try:
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Validate configuration
    pynlm.validate_config(config)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
except pynlm.ConfigurationException as e:
    print(f"Configuration error: {e}")
    
except pynlm.BrainException as e:
    print(f"Brain error: {e}")
    
except pynlm.NLMException as e:
    print(f"NLM error: {e}")
    
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Error Reporting Functions

```python
# Report an error (will raise exception)
pynlm.report_error("Configuration missing required parameter")

# Log a warning (continues execution)
pynlm.warn("This is a warning message")

# Check if error occurred
if pynlm.check_error():
    print(f"Last error: {pynlm.get_last_error()}")
```

## Convenience Functions

### Simulation Helpers

```python
# Run a complete simulation with error checking
result = pynlm.run_simulation_with_checks(
    brain=brain,
    agent=agent,
    world=world,
    steps=1000
)

# Create validated configuration
config = pynlm.create_config_with_validation()
```

## Performance Tips

### Large-Scale Simulations

For simulations with many neurons, consider these performance optimizations:

1. **Increase neuron count gradually**: Start with 100 neurons and scale up
2. **Use structural plasticity**: Enable for better long-term stability
3. **Monitor memory usage**: Use `brain.logStatus()` to track memory
4. **Checkpoint periodically**: Save progress to resume later

### Memory Management

```python
# Check memory usage
brain = pynlm.createBrain(config)
brain.initialize()

# Monitor during simulation
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Synapses: {brain.getTotalSynapseCount()}")
        # Working memory statistics
        wm = brain.getWorkingMemory()
        if wm:
            print(f"  Working memory traces: {wm.getActiveTraces()}")
```

## Advanced Usage

### Custom Configuration

```python
import json

# Create custom configuration
config = pynlm.createDefaultConfig()

# Modify parameters
custom_params = {
    "brain.neuron_count": 5000,
    "brain.synapse_density": 0.15,
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": True,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True
}

for key, value in custom_params.items():
    config.set(key, value)

# Validate custom configuration
pynlm.validate_config(config)

# Save custom configuration
config.saveToFile("custom_config.json")
```

### Multi-Agent System

```python
# Multiple agents sharing the same brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create multiple agents
agent1 = pynlm.createAgentBrain(brain)
agent2 = pynlm.createAgentBrain(brain)

# Each agent can have different settings
agent1.enableCuriosity(True)
agent2.enableCuriosity(False)

# Different worlds for each agent
world1 = pynlm.createSimpleWorld()
world2 = pynlm.createSimpleWorld()

agent1.initialize(world1)
agent2.initialize(world2)
```

## Troubleshooting

### Common Issues and Solutions

1. **ImportError: No module named 'pynlm'**
   - Solution: Ensure the module is properly installed (`pip install -e .`)

2. **RuntimeError: Failed to initialize brain**
   - Solution: Check configuration parameters and ensure they are valid

3. **Segmentation fault**
   - Solution: Use smaller neuron counts for debugging, increase simulation timestep

4. **Memory usage too high**
   - Solution: Reduce neuron count, enable memory pools, use checkpointing

### Getting Help

For detailed help and examples:

1. **Documentation**: Check the NLM documentation in `docs/`
2. **Examples**: Look in the `examples/` directory
3. **GitHub**: Report issues at the NLM GitHub repository
4. **Community**: Join the NLM discussion forum

## Migration Guide

### From C++ API to Python API

The Python API provides a simplified interface to the C++ NLM system:

**C++ Code (from main.cpp):**
```cpp
auto config = std::make_shared<Config>();
auto brain = std::make_shared<Brain>(config);
if (!brain->initialize()) {
    return 1;
}
```

**Python Equivalent:**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

### From Raw Python to Convenience API

**Raw Python:**
```python
percept = SensoryPercept()
percept.setVision(vision_data)
brain.receiveSensoryInput(percept)
```

**Convenience API:**
```python
agent.processVisionInput(vision_data)
```

## Future Enhancements

The NLM Python API is actively developed. Future enhancements may include:

1. **Real-time visualization**: Live brain state visualization
2. **GPU acceleration**: GPU-accelerated neural computation
3. **Distributed simulation**: Multi-machine distributed simulation
4. **Advanced agent behaviors**: More sophisticated agent control
5. **Cloud integration**: Integration with cloud computing platforms
6. **Web API**: REST API for web applications
7. **Mobile support**: Support for mobile devices

## Conclusion

The NLM Python API provides a powerful, user-friendly interface to the experimental artificial brain project. Whether you're a beginner looking to understand spiking neural networks or an expert researcher needing advanced simulation capabilities, NLM provides the tools you need.

The combination of low-level control and high-level convenience methods makes NLM suitable for a wide range of applications, from educational demonstrations to cutting-edge research.
