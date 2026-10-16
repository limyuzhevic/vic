# NLM Python API Reference

**The NLM (Neural Learning Machine) Python API provides comprehensive bindings to the C++ neural simulation framework through the `pynlm` module.**

## Module Overview

The `pynlm` module enables Python users to create and simulate neural networks that exhibit learning, development, and adaptive behavior. All classes are accessible directly from the module:

```python
import pynlm

# Access classes
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Access enumerations
neuron_type = pynlm.NeuronType.Excitatory
stage = pynlm.DevelopmentalStage.CriticalPeriod
```

## Core Classes

### Config
Configuration manager for NLM system parameters. Handles loading, saving, and modifying settings.

**Key Methods:**
- `loadFromFile(filepath)` - Load from JSON file
- `saveToFile(filepath)` - Save to JSON file
- `getKeys()` - Get all configuration keys
- `set(key, value)` - Set configuration value
- `get(key)` - Get configuration value

**Error Conditions:**
- `NLMInvalidArgumentError` - For invalid keys or values
- `NLMError` - For file access errors

### Brain
Central neural simulation brain with neurons, synapses, and learning capabilities.

**Initialization:**
```python
brain = pynlm.createBrain(config)
brain.initialize()
```

**Key Methods:**
- `step(currentStep)` or `step(currentStep, currentTime)` - Perform simulation step
- `receiveSensoryInput(sensory_input)` - Inject sensory data
- `produceAction()` - Generate motor action
- `reset()` - Reset brain state
- `save(filepath)` and `load(filepath)` - Persist state

**Error Conditions:**
- `NLMNotInitializedError` - Operations before initialization
- `NLMInvalidArgumentError` - Invalid parameters
- `NLMError` - Simulation or file errors

### AgentBrain
Interface layer connecting brain to world simulation. Handles sensory processing and motor decoding.

**Key Methods:**
- `processSensoryInput(percept)` - Process sensory information
- `decodeMotorCommand()` - Convert neural activity to action
- `applyRewardModulation(reward, predicted)` - Apply reward-based learning
- `updateDevelopment(timestep)` - Advance developmental stage

**Key Attributes:**
- `enableRewardModulation(True/False)` - Enable/disable reward learning
- `enableCuriosity(True/False)` - Enable curiosity-driven exploration

### SimpleWorld
2D simulation environment with physics and object interaction.

**Key Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Set world dimensions
- `update(timestep)` - Advance simulation
- `applyMotorCommand(action, time)` - Execute action
- `getSensoryPercept()` - Get current sensory data
- `getAgentBody()` - Get agent state

**Error Conditions:**
- `NLMInvalidArgumentError` - Invalid dimensions or coordinates
- `NLMError` - Simulation or world errors

### Vision, Audio, InternalSignals
Sensory input classes. All inherit from SensoryInput base class.

**Vision:**
```python
vision = pynlm.Vision(width, height, channels)
vision.setData(pixel_data_vector)
```

**Audio:**
```python
audio = pynlm.Audio(sampleRate, numSamples)
audio.setData(sample_vector)
audio.setSampleRate(rate)
```

**InternalSignals:**
```python
internal = pynlm.InternalSignals()
internal.addSignal(value)
```

### Action
Motor output representation with type and parameters.

**Creation:**
```python
action = pynlm.Action(pynlm.ActionType.MoveForward)
action = pynlm.Action(pynlm.ActionType.MoveForward, [2.0, 3.0])
```

### WorldObject
Objects in the simulation world (resources, hazards, walls, markers).

### AgentBody
Agent state with position, orientation, and physiological parameters.

### STDP
Spike-Timing-Dependent Plasticity learning rule.

**Configuration:**
```python
stdp = pynlm.STDP()
stdp.configure(ltpWeight, ltdWeight, tau)
stdp.setLTPWeight(weight)
stdp.setLTDWeight(weight)
stdp.setTimeConstant(tau)
```

### Other Key Classes
- `NeuronId`, `SynapseId`, `RegionId`, `PopulationId` - Unique identifiers
- `ActionResult` - Motor command results with reward
- `SensoryPercept` - Processed sensory information
- `WorldObjectType` - Enum for object types
- `ActionType` - Enum for action types

## Enumerations

### NeuronType
- `Excitatory` - Positive feedback neurons
- `Inhibitory` - Negative feedback neurons
- `Modulatory` - Neuromodulation neurons
- `Sensory` - Input neurons
- `Motor` - Output neurons
- `Internal` - Internal state neurons

### DevelopmentalStage
- `Initial` - Newborn state
- `CriticalPeriod` - Rapid learning phase
- `Maturation` - Refinement phase
- `Adult` - Stable performance
- `Aging` - Decline phase

### ActionType
Movement, sensing, and interaction actions including:
`MoveForward`, `MoveBackward`, `TurnLeft`, `TurnRight`, `Look`, `Interact`, `Eat`, `Drink`, `Rest`

### WorldObjectType
`Empty`, `Resource`, `Hazard`, `Wall`, `Marker`

## Factory Functions

### Configuration
- `createDefaultConfig()` - Create default configuration

### Simulation Objects
- `createBrain(config)` - Create neural brain
- `createSimpleWorld()` - Create 2D world
- `createAgentBrain(brain)` - Create agent interface
- `createSTDP()` - Create STDP plasticity rule
- `createDefaultConfig()` - Create default configuration

### Sensory Inputs
- `Vision(width, height, channels)` - Visual sensor
- `Audio(sampleRate, numSamples)` - Auditory sensor
- `InternalSignals()` - Internal state sensor

### Actions
- `Action(type)` or `Action(type, parameters)` - Create motor action
- `WorldObject(x, y, type, value, radius)` - Create world object
- `AgentBody()` - Create agent body

## Performance Considerations

- Simulation steps are event-driven; time scales with neural activity
- Large networks (1000+ neurons) may require optimization
- State saving/loading provides checkpoint capability
- World simulation uses discrete timestep updates

## Error Handling

All NLM exceptions inherit from `NLMError`:

- `NLMError` - Base exception
- `NLMInvalidArgumentError` - Invalid parameters
- `NLMNotInitializedError` - Operations before initialization
- `NLMBrainError` - Brain-specific errors
- `NLMInvalidStateError` - Invalid state transitions
- `NLMBoundaryError` - Boundary violations

Use try-except blocks for robust error handling in production code.

## Memory Management

The `pynlm` module uses pybind11's reference counting. Objects are automatically garbage collected when no longer referenced. For performance-critical loops, reuse existing objects rather than creating new ones.

## Thread Safety

The Python bindings are not thread-safe for modifying brain state. Use external synchronization when running multiple threads or processes.

## Version Notes

This documentation covers NLM Version 0.1.0. APIs are stable but may be extended in future releases.
