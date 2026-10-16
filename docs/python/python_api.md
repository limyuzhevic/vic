# NLM Python API Documentation

**Complete API reference for the NLM (Neural Learning Machine) Python bindings.**

This documentation provides comprehensive coverage of the `pynlm` Python module, including all classes, methods, parameters, and usage examples.

## Table of Contents

1. [Module Overview](#module-overview)
2. [Core Classes](#core-classes)
   - [Brain](#brain)
   - [Config](#config)
   - [AgentBrain](#agentbrain)
   - [SimpleWorld](#simpleworld)
   - [SensoryInput Types](#sensoryinput-types)
   - [Action and Motor System](#action-and-motor-system)
   - [World System](#world-system)
   - [Agent System](#agent-system)
   - [Plasticity System](#plasticity-system)
   - [Neuromodulation System](#neuromodulation-system)
   - [Development System](#development-system)
   - [Memory System](#memory-system)
3. [Enumerations](#enumerations)
4. [Factory Functions](#factory-functions)
5. [Error Handling](#error-handling)
6. [Performance Considerations](#performance-considerations)
7. [Thread Safety](#thread-safety)
8. [Migration Guide](#migration-guide)

## Module Overview

The `pynlm` module provides Python bindings to the NLM C++ neural simulation framework. It enables users to create, simulate, and analyze neural networks that exhibit learning, development, and adaptive behavior.

### Installation

```bash
pip install pynlm
# Or install from source
pip install -e .
```

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
```

## Core Classes

### Brain
Central neural simulation brain with neurons, synapses, and learning capabilities.

#### Constructor
```python
brain = pynlm.createBrain(config)
```
**Parameters:**
- `config`: `Config` object containing brain parameters

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize()` | Initialize brain with configuration | None | None |
| `step(step)` | Perform simulation step | `step: int` | None |
| `step(step, time)` | Perform step with timestamp | `step: int, time: float` | None |
| `reset()` | Reset brain state | None | None |
| `receiveSensoryInput(input)` | Inject sensory data | `input: SensoryInput` | None |
| `produceAction()` | Generate motor action | None | `Action` |
| `save(filepath)` | Save brain state | `filepath: str` | None |
| `load(filepath)` | Load brain state | `filepath: str` | None |
| `getTotalNeuronCount()` | Get total neurons | None | `int` |
| `getTotalSynapseCount()` | Get total synapses | None | `int` |
| `getFiringNeuronCount()` | Get currently firing neurons | None | `int` |
| `getAverageFiringRate()` | Get average firing rate (Hz) | None | `float` |
| `getTotalSpikeCount()` | Get total spikes generated | None | `int` |
| `getExcitationInhibitionRatio()` | Get E/I balance | None | `float` |
| `addRegion(name)` | Add neural region | `name: str` | `RegionId` |
| `getRegion(region_id)` | Get region by ID | `region_id: RegionId` | `Region` |
| `getSTDP()` | Get STDP plasticity rule | None | `STDP` |

#### Error Conditions

- `NLMNotInitializedError`: Operations before initialization
- `NLMInvalidArgumentError`: Invalid parameters or files
- `NLMBrainError`: Brain-specific errors
- `NLMError`: General simulation errors

### Config
Configuration management system for NLM parameters.

#### Constructor
```python
config = pynlm.createDefaultConfig()
```

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `loadFromFile(filepath)` | Load from JSON file | `filepath: str` | None |
| `saveToFile(filepath)` | Save to JSON file | `filepath: str` | None |
| `getKeys()` | Get all configuration keys | None | `list[str]` |
| `has(key)` | Check if key exists | `key: str` | `bool` |
| `get(key)` | Get string value | `key: str` | `str` |
| `getInt(key)` | Get integer value | `key: str` | `int` |
| `getDouble(key)` | Get double value | `key: str` | `float` |
| `getBool(key)` | Get boolean value | `key: str` | `bool` |
| `set(key, value)` | Set string value | `key: str, value: str` | None |
| `setInt(key, value)` | Set integer value | `key: str, value: int` | None |
| `setDouble(key, value)` | Set double value | `key: str, value: float` | None |
| `setBool(key, value)` | Set boolean value | `key: str, value: bool` | None |
| `clear()` | Clear all configuration | None | None |
| `summary()` | Get configuration summary | None | `str` |

### AgentBrain
Interface layer connecting brain to world simulation.

#### Constructor
```python
agent = pynlm.createAgentBrain(brain)
```

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize(world)` | Initialize agent with world | `world: SimpleWorld` | None |
| `processSensoryInput(percept)` | Process sensory information | `percept: SensoryPercept` | None |
| `decodeMotorCommand()` | Convert neural activity to action | None | `Action` |
| `applyRewardModulation(reward, predicted)` | Apply reward learning | `reward: float, predicted: float` | None |
| `updateDevelopment(timestep)` | Advance development | `timestep: float` | None |
| `enableRewardModulation(enable)` | Enable/disable reward learning | `enable: bool` | None |
| `enableCuriosity(enable)` | Enable/disable curiosity | `enable: bool` | None |
| `enableDevelopment(enable)` | Enable/disable development | `enable: bool` | None |
| `enableStructuralPlasticity(enable)` | Enable/disable structural plasticity | `enable: bool` | None |
| `getCuriosityLevel()` | Get curiosity level | None | `float` |
| `getNoveltyLevel()` | Get novelty level | None | `float` |
| `getPredictionError()` | Get prediction error | None | `float` |
| `getNeuromodulationLevel()` | Get overall neuromodulation | None | `float` |

### SimpleWorld
2D simulation environment with physics and object interaction.

#### Constructor
```python
world = pynlm.createSimpleWorld()
```

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `configure(width, height, visionWidth, visionHeight)` | Set world dimensions | `width: size_t, height: size_t, visionWidth: size_t, visionHeight: size_t` | None |
| `reset()` | Reset world state | None | None |
| `update(timestep)` | Advance simulation | `timestep: float` | None |
| `applyMotorCommand(action, time)` | Execute action | `action: Action, time: float` | None |
| `getSensoryPercept()` | Get current sensory data | None | `SensoryPercept` |
| `getAgentBody()` | Get agent state | None | `AgentBody` |
| `setAgentStart(x, y)` | Set agent starting position | `x: float, y: float` | None |
| `isValidPosition(x, y)` | Check if position valid | `x: float, y: float` | `bool` |
| `addObject(object)` | Add world object | `object: WorldObject` | None |
| `removeObject(x, y)` | Remove object at position | `x: float, y: float` | None |
| `setRandomSeed(seed)` | Set random seed | `seed: uint64_t` | None |
| `getWidth()`, `getHeight()` | Get world dimensions | None | `size_t` |
| `getSimulationTime()` | Get simulation time | None | `float` |

### SensoryInput Types

#### Vision
```python
vision = pynlm.Vision(width, height, channels)
vision.setData(pixel_data_vector)
```

**Methods:**
- `getType()`: Get input type
- `getData()`: Get raw data vector
- `getDimensions()`: Get dimensionality
- `getTimestamp()`: Get timestamp
- `getWidth()`, `getHeight()`, `getChannels()`: Get vision properties

#### Audio
```python
audio = pynlm.Audio(sampleRate, numSamples)
audio.setData(sample_vector)
```

**Methods:**
- `setSampleRate(rate)`: Set sample rate
- `getSampleRate()`: Get sample rate
- `getNumSamples()`: Get number of samples

#### InternalSignals
```python
internal = pynlm.InternalSignals()
internal.addSignal(value)
```

**Methods:**
- `addSignal(value)`: Add internal signal
- `clearSignals()`: Clear all signals

### Action and Motor System

#### Action Class
```python
action = pynlm.Action(pynlm.ActionType.MoveForward)
action = pynlm.Action(pynlm.ActionType.MoveForward, [2.0, 3.0])
```

**Methods:**
- `getType()`: Get action type
- `setType(type)`: Set action type
- `getParameters()`: Get action parameters
- `setParameters(params)`: Set action parameters
- `getName()`: Get action name
- `clone()`: Clone action

#### WorldObject Class
```python
obj = pynlm.WorldObject(x, y, type, value, radius)
```

**Properties:**
- `x`, `y`: Position coordinates
- `type`: `WorldObjectType` enum
- `value`: Object value
- `radius`: Object radius
- `active`: Object active state

### Agent System

#### AgentBody Class
Agent state with physiological and positional properties.

**Properties:**
- `x`, `y`: Position
- `orientation`: Facing angle (0-360)
- `velocityX`, `velocityY`: Linear velocity
- `angularVelocity`: Rotational velocity
- `energy`: Energy level (0-1000)
- `health`: Health level (0-100)
- `age`: Age
- `isMoving`, `isTurning`: State flags

#### ActionResult Class
Motor command results with reward and success information.

**Properties:**
- `reward`: Reward value
- `success`: Success flag
- `message`: Descriptive message

### Plasticity System

#### STDP Class
Spike-Timing-Dependent Plasticity rule.

**Methods:**
- `setLTPWeight(weight)`: Set LTP weight (0-1)
- `getLTPWeight()`: Get LTP weight
- `setLTDWeight(weight)`: Set LTD weight (0-1)
- `getLTDWeight()`: Get LTD weight
- `setTimeConstant(tau)`: Set time constant (ms)
- `getTimeConstant()`: Get time constant
- `configure(ltp, ltd, tau)`: Configure all parameters

### Neuromodulation System

#### Neuromodulators
Various neuromodulators affecting brain function:
- **Dopamine**: Reward prediction and learning
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **PredictionError**: Learning from mistakes

**Agent Methods:**
- `getNeuromodulationLevel()`: Overall neuromodulation
- `getCuriosityLevel()`: Curiosity level
- `getNoveltyLevel()`: Novelty level
- `getPredictionError()`: Prediction error

### Development System

#### DevelopmentalStage Enum
- `Initial`: Newborn state
- `CriticalPeriod`: Rapid learning phase
- `Maturation`: Refinement phase
- `Adult`: Stable performance
- `Aging`: Decline phase

**Brain Methods:**
- `getDevelopmentalStage()`: Get current stage
- `setDevelopmentalStage(stage)`: Set developmental stage
- `updateDevelopment(timestep)`: Advance development

### Memory System

#### Working Memory
Persistent activity for temporary information storage.

#### Episodic Memory
Experience encoding and replay.

#### Associative Memory
Experience-based associations.

## Enumerations

### NeuronType
- `Excitatory`: Positive feedback neurons
- `Inhibitory`: Negative feedback neurons
- `Modulatory`: Neuromodulation neurons
- `Sensory`: Input neurons
- `Motor`: Output neurons
- `Internal`: Internal state neurons

### ActionType
- `MoveForward`, `MoveBackward`: Linear movement
- `TurnLeft`, `TurnRight`: Rotation
- `Look`, `LookUp`, `LookDown`: Looking actions
- `Interact`, `Eat`, `Drink`: Interaction actions
- `Rest`, `Wait`: Stationary actions
- `Custom`: Custom action type

### WorldObjectType
- `Empty`: Empty space
- `Resource`: Valuable resource
- `Hazard`: Dangerous object
- `Wall`: Impassable barrier
- `Marker`: Target location

### DevelopmentalStage
- `Initial`: Newborn, high plasticity
- `CriticalPeriod`: Rapid sensory learning
- `Maturation`: Refinement and pruning
- `Adult`: Stable, efficient processing
- `Aging`: Declining function

### Other Enumerations
- `SynapseType`: Excitatory, Inhibitory, Modulatory
- `FiringState`: Resting, Active, Refractory, Inhibited

## Factory Functions

### Object Creation

```python
# Configuration
config = pynlm.createDefaultConfig()

# Brain
brain = pynlm.createBrain(config)

# World
world = pynlm.createSimpleWorld()

# Agent
agent = pynlm.createAgentBrain(brain)

# Sensory Inputs
vision = pynlm.Vision(640, 480, 3)
audio = pynlm.Audio(44100, 1024)
internal = pynlm.InternalSignals()

# Actions
action = pynlm.Action(pynlm.ActionType.MoveForward)
action = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])

# World Objects
obj = pynlm.WorldObject(10.0, 20.0, pynlm.WorldObjectType.Resource, 5.0, 1.0)

# Agent Body
body = pynlm.AgentBody()
```

### Plasticity Rules

```python
# Create STDP rule
stdp = pynlm.STDP()
stdp.configure(0.05, 0.06, 25.0)

# Factory function
stdp = pynlm.createSTDP()
```

## Error Handling

### Exception Hierarchy

```python
try:
    brain.step(step)
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
except pynlm.NLMInvalidArgumentError as e:
    print(f"Invalid argument: {e}")
except pynlm.NLMNotInitializedError as e:
    print(f"Not initialized: {e}")
except pynlm.NLMBrainError as e:
    print(f"Brain error: {e}")
except pynlm.NLMInvalidStateError as e:
    print(f"Invalid state: {e}")
except pynlm.NLMBoundaryError as e:
    print(f"Boundary error: {e}")
```

### Common Error Scenarios

1. **Not Initialized**: Always call `brain.initialize()` before `brain.step()`
2. **Invalid Parameters**: Check input values before operations
3. **File Errors**: Use try-except for file I/O operations
4. **State Transitions**: Ensure valid state transitions

## Performance Considerations

### Memory Management
- Objects are automatically garbage collected
- Reuse existing objects in performance-critical loops
- Use `clone()` for action copies

### Simulation Performance
- Simulation steps are event-driven
- Large networks (1000+ neurons) require optimization
- World simulation uses discrete timestep updates

### Configuration Optimization
```python
# For large simulations
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05)  # Reduce connections

# For real-time applications
config.set("simulation_timestep", 0.001)
```

## Thread Safety

The Python bindings are **not thread-safe** for:
- Modifying brain state
- Concurrent world updates
- Shared configuration access

Use external synchronization for multi-threaded applications:
```python
import threading

def run_simulation(brain, steps):
    for i in range(steps):
        brain.step(i)

# Thread-safe execution
threads = []
for i in range(4):
    t = threading.Thread(target=run_simulation, args=(brain, 100))
    threads.append(t)
    t.start()

for t in threads:
    t.join()
```

## Migration Guide

### From C++ to Python

**C++:** `brain.step(0, 0.0);`
**Python:** `brain.step(0)`

**C++:** `config.setString("key", "value");`
**Python:** `config.set("key", "value")`

**C++:** `if (config.has("key")) { value = config.getString("key"); }`
**Python:** `if config.has("key"): value = config.get("key")`

### From Old API to New API

**Old:** `createBrain(config)`
**New:** `createBrain(config)` (unchanged)

**Old:** `brain.getFiringNeurons()`
**New:** `brain.getFiringNeuronCount()`

**Old:** `brain.getSTDPRule()`
**New:** `brain.getSTDP()`

## Version Notes

- **Version 0.1.0**: Initial Python bindings release
- APIs are stable but may be extended
- New features added in patch releases
- Report issues on GitHub

## Additional Resources

### Example Scripts

See the `examples/` directory for:
- `basic_usage.py`: Basic brain operations
- `plasticity_demo.py`: STDP learning demonstrations
- `sensory_demo.py`: Sensory input processing
- `simulation_demo.py`: Complete agent simulations
- `model_persistence.py`: Save/load brain states
- `neuromodulation_demo.py`: Neuromodulator demonstrations
- `development_demo.py`: Developmental stage examples

### Documentation

- `docs/python/getting_started.md`: Tutorial for new users
- `HOW_TO_USE.md`: Detailed usage guide
- `easy_usage.md`: Quick start guide

### Community

- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Documentation](https://nlm.readthedocs.io)
- [Discussions](https://github.com/nlm-project/nlm/discussions)

## Index

- Classes: Brain, Config, AgentBrain, SimpleWorld, Vision, Audio, InternalSignals, Action, WorldObject, AgentBody, ActionResult, STDP
- Enumerations: NeuronType, ActionType, WorldObjectType, DevelopmentalStage
- Methods: All class methods with parameters and return types
- Examples: Complete working examples in examples/ directory

---

*This documentation covers NLM Version 0.1.0. APIs are stable but may be extended in future releases.*
