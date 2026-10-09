# API Reference Documentation - NLM Python Bindings

## Overview

This document provides comprehensive API reference documentation for the **NLM Python Bindings** (`pynlm` module). This reference serves as a complete guide to all classes, methods, enumerations, and constants available in the NLM Python API.

## Table of Contents

1. [Brain Class](#brain-class)
2. [Config Class](#config-class)
3. [AgentBrain Class](#agentbrain-class)
4. [SimpleWorld Class](#simpleworld-class)
5. [SensoryInput Classes](#sensoryinput-classes)
6. [Action Classes](#action-classes)
7. [World Classes](#world-classes)
8. [Enumeration Types](#enumeration-types)
9. [Constants](#constants)
10. [Type Aliases](#type-aliases)

## Brain Class

The **Brain** class is the central neural simulation engine. It manages neurons, synapses, regions, and the simulation loop.

### Constructor

```python
brain = pynlm.createBrain(config)
```

**Parameters:**
- `config`: `Config` object containing brain configuration

**Returns:** `Brain` instance

**Example:**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuronCount", 1000)
brain = pynlm.createBrain(config)
```

### Initialization Methods

#### `initialize(enable_development, enable_plasticity)`

Initialize the brain with configuration.

**Parameters:**
- `enable_development`: `bool` - Whether to enable developmental plasticity (default: `True`)
- `enable_plasticity`: `bool` - Whether to enable synaptic plasticity (default: `True`)

**Returns:** `None`

**Example:**
```python
brain.initialize(enable_development=True, enable_plasticity=True)
```

### Simulation Methods

#### `step(currentStep, timestamp=None)`

Perform a simulation step.

**Parameters:**
- `currentStep`: `int` - The current simulation step number
- `timestamp`: `float` - Optional simulation time (defaults to step number)

**Returns:** `None`

**Overloads:**
```python
brain.step(42)           # Step with default timestamp
brain.step(42, 4.2)      # Step with explicit timestamp
```

**Example:**
```python
brain.step(100)        # Step 100 at time 100
brain.step(100, 10.5)   # Step 100 at time 10.5
```

#### `reset()`

Reset brain state to initial conditions.

**Parameters:** `None`

**Returns:** `None`

**Example:**
```python
brain.reset()  # Clear all neural activity
```

### Input/Output Methods

#### `receiveSensoryInput(sensory_input)`

Inject sensory input into the brain.

**Parameters:**
- `sensory_input`: `SensoryInput` - Sensory input data (Vision, Audio, etc.)

**Returns:** `None`

**Example:**
```python
percept = world.getSensoryPercept()
brain.processSensoryInput(percept)
```

#### `produceAction()`

Produce motor action based on neural activity.

**Parameters:** `None`

**Returns:** `ActionResult` - Action result with reward and action details

**Example:**
```python
result = brain.produceAction()
print(f"Action: {result.message}, Reward: {result.reward}")
```

#### `injectCurrent(neuron, current)`

Inject current into a specific neuron.

**Parameters:**
- `neuron`: `NeuronId` - Target neuron ID
- `current`: `float` - Current value to inject (microsiemens)

**Returns:** `None`

**Example:**
```python
brain.injectCurrent(neuron_id, 0.5)
```

#### `injectCurrentToNeurons(neuron_type, current)`

Inject current into all neurons of a specific type.

**Parameters:**
- `neuron_type`: `str` or `NeuronType` - Neuron type (Excitatory, Inhibitory, etc.)
- `current`: `float` - Current value to inject (microsiemens)

**Returns:** `None`

**Example:**
```python
brain.injectCurrentToNeurons("Excitatory", 0.5)
```

### State Management Methods

#### `save(filepath)`

Save brain state to file.

**Parameters:**
- `filepath`: `str` - Path to save the brain state

**Returns:** `None`

**Example:**
```python
brain.save("brain_state.pkl")
```

#### `load(filepath)`

Load brain state from file.

**Parameters:**
- `filepath`: `str` - Path to load the brain state from

**Returns:** `None`

**Example:**
```python
brain.load("brain_state.pkl")
```

### Region Management Methods

#### `addRegion(name)`

Add a new neural region.

**Parameters:**
- `name`: `str` - Optional name for the new region (default: "")

**Returns:** `RegionId` - ID of the newly created region

**Example:**
```python
region_id = brain.addRegion("cortex")
```

#### `getRegion(id)`

Get a region by ID.

**Parameters:**
- `id`: `RegionId` - Region identifier

**Returns:** `NeuralRegion` or `None` - NeuralRegion object or None if not found

**Example:**
```python
region = brain.getRegion(region_id)
if region:
    print(f"Region has {region.getNeuronCount()} neurons")
```

#### `getRegionCount()`

Get the number of regions in the brain.

**Parameters:** `None`

**Returns:** `int` - Number of regions

**Example:**
```python
count = brain.getRegionCount()
print(f"Brain has {count} regions")
```

#### `getRegionIds()`

Get all region IDs.

**Parameters:** `None`

**Returns:** `list[RegionId]` - List of all region IDs

**Example:**
```python
region_ids = brain.getRegionIds()
for rid in region_ids:
    print(f"Region ID: {rid}")
```

### Statistics Methods

#### `getTotalNeuronCount()`

Get total number of neurons.

**Parameters:** `None`

**Returns:** `int` - Total neurons

**Example:**
```python
neurons = brain.getTotalNeuronCount()
print(f"Total neurons: {neurons}")
```

#### `getTotalSynapseCount()`

Get total number of synapses.

**Parameters:** `None`

**Returns:** `int` - Total synapses

**Example:**
```python
synapses = brain.getTotalSynapseCount()
print(f"Total synapses: {synapses}")
```

#### `getFiringNeuronCount()`

Get number of currently firing neurons.

**Parameters:** `None`

**Returns:** `int` - Currently firing neurons

**Example:**
```python
firing = brain.getFiringNeuronCount()
print(f"Firing neurons: {firing}")
```

#### `getAverageFiringRate()`

Get average firing rate across all neurons.

**Parameters:** `None`

**Returns:** `float` - Average firing rate (Hz)

**Example:**
```python
rate = brain.getAverageFiringRate()
print(f"Average firing rate: {rate:.2f} Hz")
```

#### `getExcitationInhibitionRatio()`

Get excitation-inhibition balance ratio.

**Parameters:** `None`

**Returns:** `float` - E/I ratio (excitatory/inhibitory)

**Example:**
```python
ei_ratio = brain.getExcitationInhibitionRatio()
print(f"E/I ratio: {ei_ratio:.2f}")
```

### Context Manager Support

The Brain class can be used as a context manager for automatic resource cleanup:

```python
with pynlm.createBrain(config) as brain:
    brain.initialize()
    for step in range(50):
        brain.step(step)
    # Brain automatically cleaned up when exiting context
```

## Config Class

The **Config** class provides configuration management for all NLM components.

### Constructor

```python
config = pynlm.createDefaultConfig()
```

**Parameters:** `None`

**Returns:** `Config` instance

### Query Methods

#### `has(key)`

Check if a configuration key exists.

**Parameters:**
- `key`: `str` - Configuration key to check

**Returns:** `bool` - True if key exists, False otherwise

**Example:**
```python
if config.has("brain.neuronCount"):
    print(config["brain.neuronCount"])
```

#### `getKeys()`

Get all configuration keys.

**Parameters:** `None`

**Returns:** `list[str]` - List of all configuration keys

**Example:**
```python
keys = config.getKeys()
print(f"Available keys: {keys}")
```

### Modification Methods

#### `__setitem__(key, value)`

Set configuration value by key (Pythonic access).

**Parameters:**
- `key`: `str` - Configuration key
- `value`: `str` or `any` - Configuration value

**Returns:** `None`

**Example:**
```python
config["brain.neuronCount"] = "1000"
config.setValue("world.width", "20.0")
```

#### `__getitem__(key)`

Get configuration value by key (Pythonic access).

**Parameters:**
- `key`: `str` - Configuration key

**Returns:** `any` - Configuration value

**Raises:** `KeyError` - If key doesn't exist

**Example:**
```python
neurons = config["brain.neuronCount"]
```

#### `getValue(key, defaultValue="")`

Get configuration value with default.

**Parameters:**
- `key`: `str` - Configuration key
- `defaultValue`: `str` - Default value if key doesn't exist (default: "")

**Returns:** `str` - Configuration value or default

**Example:**
```python
neurons = config.getValue("brain.neuronCount", "1000")
```

#### `setValue(key, value)`

Set configuration value.

**Parameters:**
- `key`: `str` - Configuration key
- `value`: `str` - Configuration value

**Returns:** `None`

**Example:**
```python
config.setValue("brain.neuronCount", "1000")
```

#### `loadFromFile(filepath)`

Load configuration from JSON file.

**Parameters:**
- `filepath`: `str` - Path to the JSON configuration file

**Returns:** `None`

**Example:**
```python
config.loadFromFile("config.json")
```

#### `loadFromArgs(argc, argv)`

Load configuration from command line arguments.

**Parameters:**
- `argc`: `int` - Number of command line arguments
- `argv`: `list[str]` - Array of command line argument strings

**Returns:** `None`

**Example:**
```python
import sys
config.loadFromArgs(len(sys.argv), sys.argv)
```

#### `saveToFile(filepath)`

Save configuration to JSON file.

**Parameters:**
- `filepath`: `str` - Path to save the configuration

**Returns:** `None`

**Example:**
```python
config.saveToFile("config.json")
```

### Configuration Convenience Methods

#### `createDefault()`

Create default configuration with sensible values.

**Parameters:** `None`

**Returns:** `None`

**Example:**
```python
config.createDefault()
```

#### `createMinimal()`

Create minimal configuration for basic operation.

**Parameters:** `None`

**Returns:** `None`

**Example:**
```python
config.createMinimal()
```

#### `createHighPerformance()`

Create high-performance configuration.

**Parameters:** `None`

**Returns:** `None`

**Example:**
```python
config.createHighPerformance()
```

#### `createExperimental()`

Create experimental configuration.

**Parameters:** `None`

**Returns:** `None`

**Example:**
```python
config.createExperimental()
```

#### `getInt(key, defaultValue=0)`

Get configuration value as integer.

**Parameters:**
- `key`: `str` - Configuration key
- `defaultValue`: `int` - Default value if key doesn't exist (default: 0)

**Returns:** `int` - Integer value or default

**Example:**
```python
neurons = config.getInt("brain.neuronCount", 1000)
```

#### `getFloat(key, defaultValue=0.0)`

Get configuration value as float.

**Parameters:**
- `key`: `str` - Configuration key
- `defaultValue`: `float` - Default value if key doesn't exist (default: 0.0)

**Returns:** `float` - Float value or default

**Example:**
```python
timestep = config.getFloat("simulation.timeStep", 0.1)
```

#### `getBool(key, defaultValue=False)`

Get configuration value as boolean.

**Parameters:**
- `key`: `str` - Configuration key
- `defaultValue`: `bool` - Default value if key doesn't exist (default: False)

**Returns:** `bool` - Boolean value or default

**Example:**
```python
plasticity = config.getBool("brain.plasticity.enabled", True)
```

#### `validate()`

Validate configuration.

**Parameters:** `None`

**Returns:** `bool` - True if configuration is valid

**Example:**
```python
if config.validate():
    print("Configuration is valid")
```

#### `copy()`

Create a copy of configuration.

**Parameters:** `None`

**Returns:** `Config` - New Config object with same values

**Example:**
```python
config_copy = config.copy()
```

### Context Manager Support

The Config class can be used as a context manager:

```python
with pynlm.createDefaultConfig() as config:
    config.setValue("brain.neuronCount", 500)
    # Config automatically cleaned up when exiting context
```

## AgentBrain Class

The **AgentBrain** class connects NLM brains to the world environment.

### Constructor

```python
agent = pynlm.createAgentBrain(brain)
```

**Parameters:**
- `brain`: `Brain` - NLM brain to control

**Returns:** `AgentBrain` instance

**Example:**
```python
agent = pynlm.createAgentBrain(brain)
```

### Initialization Method

#### `initialize(world)`

Initialize agent brain with world.

**Parameters:**
- `world`: `SimpleWorld` - World to interact with

**Returns:** `None`

**Example:**
```python
agent.initialize(world)
```

### Sensory Processing Methods

#### `processSensoryInput(percept)`

Process sensory input from world.

**Parameters:**
- `percept`: `SensoryPercept` - Sensory percept data

**Returns:** `None`

**Example:**
```python
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
```

#### `getCuriosityLevel()`

Get current curiosity level.

**Parameters:** `None`

**Returns:** `float` - Curiosity level (0.0 to 1.0)

**Example:**
```python
curiosity = agent.getCuriosityLevel()
print(f"Curiosity level: {curiosity:.3f}")
```

#### `getNoveltyLevel()`

Get current novelty level.

**Parameters:** `None`

**Returns:** `float` - Novelty level (0.0 to 1.0)

**Example:**
```python
novelty = agent.getNoveltyLevel()
print(f"Novelty level: {novelty:.3f}")
```

#### `getPredictionError()`

Get current prediction error.

**Parameters:** `None`

**Returns:** `float` - Prediction error value

**Example:**
```python
error = agent.getPredictionError()
print(f"Prediction error: {error:.3f}")
```

### Motor Decoding Method

#### `decodeMotorCommand()`

Decode motor command from brain activity.

**Parameters:** `None`

**Returns:** `MotorCommand` - Selected motor command

**Example:**
```python
motor_cmd = agent.decodeMotorCommand()
```

### Neuromodulation Methods

#### `applyRewardModulation(reward, predicted_reward)`

Apply reward modulation to brain.

**Parameters:**
- `reward`: `float` - Actual reward received
- `predicted_reward`: `float` - Predicted reward

**Returns:** `None`

**Example:**
```python
agent.applyRewardModulation(reward, predicted_reward)
```

#### `enableRewardModulation(enabled)`

Enable or disable reward modulation.

**Parameters:**
- `enabled`: `bool` - Whether to enable reward modulation

**Returns:** `None`

**Example:**
```python
agent.enableRewardModulation(True)
```

#### `enableStructuralPlasticity(enabled)`

Enable or disable structural plasticity.

**Parameters:**
- `enabled`: `bool` - Whether to enable structural plasticity

**Returns:** `None`

**Example:**
```python
agent.enableStructuralPlasticity(True)
```

#### `enableDevelopment(enabled)`

Enable or disable development.

**Parameters:**
- `enabled`: `bool` - Whether to enable development

**Returns:** `None`

**Example:**
```python
agent.enableDevelopment(True)
```

#### `enableCuriosity(enabled)`

Enable or disable curiosity.

**Parameters:**
- `enabled`: `bool` - Whether to enable curiosity

**Returns:** `None`

**Example:**
```python
agent.enableCuriosity(True)
```

### Development Methods

#### `updateDevelopment(timestep)`

Update developmental stage.

**Parameters:**
- `timestep`: `float` - Time step for development update

**Returns:** `None`

**Example:**
```python
agent.updateDevelopment(0.1)
```

#### `getDevelopmentalStage()`

Get current developmental stage.

**Parameters:** `None`

**Returns:** `str` - Developmental stage name

**Example:**
```python
stage = agent.getDevelopmentalStage()
print(f"Developmental stage: {stage}")
```

### Context Manager Support

The AgentBrain class can be used as a context manager:

```python
with pynlm.createAgentBrain(brain) as agent:
    agent.initialize(world)
    agent.enableRewardModulation(True)
    # Agent automatically cleaned up when exiting context
```

## SimpleWorld Class

The **SimpleWorld** class represents a 2D simulated environment for agent interaction.

### Constructor

```python
world = pynlm.createSimpleWorld()
```

**Parameters:** `None`

**Returns:** `SimpleWorld` instance

**Example:**
```python
world = pynlm.createSimpleWorld()
```

### Configuration Method

#### `configure(width, height, visionWidth, visionHeight)`

Configure world dimensions and agent sensing.

**Parameters:**
- `width`: `float` - World width in units
- `height`: `float` - World height in units
- `visionWidth`: `int` - Agent's vision width
- `visionHeight`: `int` - Agent's vision height

**Returns:** `None`

**Example:**
```python
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
```

### Control Methods

#### `reset()`

Reset world to initial state.

**Parameters:** `None`

**Returns:** `None`

**Example:**
```python
world.reset()
```

#### `update(timestep)`

Update world physics.

**Parameters:**
- `timestep`: `float` - Time step for physics update

**Returns:** `None`

**Example:**
```python
world.update(timestep=0.1)
```

#### `setRandomSeed(seed)`

Set random seed for reproducible simulations.

**Parameters:**
- `seed`: `int` - Random seed value

**Returns:** `None`

**Example:**
```python
world.setRandomSeed(42)
```

### Agent Interaction Methods

#### `setAgentStart(x, y)`

Set agent starting position.

**Parameters:**
- `x`: `float` - X coordinate
- `y`: `float` - Y coordinate

**Returns:** `None`

**Example:**
```python
world.setAgentStart(10.0, 10.0)
```

#### `applyMotorCommand(cmd, currentTime)`

Apply motor command to the world.

**Parameters:**
- `cmd`: `MotorCommand` - Motor command to execute
- `currentTime`: `float` - Current simulation time

**Returns:** `None`

**Example:**
```python
world.applyMotorCommand(action, world.getSimulationTime())
```

### State Access Methods

#### `getSensoryPercept()`

Get sensory percept for the agent.

**Parameters:** `None`

**Returns:** `SensoryPercept` - SensoryPercept object

**Example:**
```python
percept = world.getSensoryPercept()
vision = percept.getVision()
```

#### `getAgentBody()`

Get agent body state.

**Parameters:** `None`

**Returns:** `AgentBody` - AgentBody object

**Example:**
```python
body = world.getAgentBody()
print(f"Position: ({body.x}, {body.y})")
```

#### `getWidth()`

Get world width.

**Parameters:** `None`

**Returns:** `float` - World width

**Example:**
```python
width = world.getWidth()
```

#### `getHeight()`

Get world height.

**Parameters:** `None`

**Returns:** `float` - World height

**Example:**
```python
height = world.getHeight()
```

#### `getMaxEnergy()`

Get maximum agent energy.

**Parameters:** `None`

**Returns:** `float` - Maximum energy value

**Example:**
```python
max_energy = world.getMaxEnergy()
```

#### `getEnergyDecayRate()`

Get energy decay rate.

**Parameters:** `None`

**Returns:** `float` - Energy decay rate per timestep

**Example:**
```python
decay_rate = world.getEnergyDecayRate()
```

#### `getSimulationTime()`

Get current simulation time.

**Parameters:** `None`

**Returns:** `float` - Current simulation time

**Example:**
```python
time = world.getSimulationTime()
```

### World Object Management

#### `addObject(obj)`

Add an object to the world.

**Parameters:**
- `obj`: `WorldObject` - WorldObject to add

**Returns:** `None`

**Example:**
```python
obj = pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Resource, value=10.0)
world.addObject(obj)
```

#### `removeObject(x, y)`

Remove object at position.

**Parameters:**
- `x`: `float` - X coordinate
- `y`: `float` - Y coordinate

**Returns:** `None`

**Example:**
```python
world.removeObject(x=5, y=5)
```

#### `isValidPosition(x, y)`

Check if position is valid (within bounds and not occupied).

**Parameters:**
- `x`: `float` - X coordinate
- `y`: `float` - Y coordinate

**Returns:** `bool` - True if position is valid, False otherwise

**Example:**
```python
if world.isValidPosition(x=3, y=4):
    world.addObject(obj)
```

#### Convenience Methods

##### `createResource(x, y, value=10.0)`

Create and add a resource object to the world.

**Parameters:**
- `x`: `float` - X coordinate
- `y`: `float` - Y coordinate
- `value`: `float` - Resource value (default: 10.0)

**Returns:** `WorldObject` - Created WorldObject

**Example:**
```python
resource = world.createResource(5, 5, value=20.0)
```

##### `createHazard(x, y, value=5.0)`

Create and add a hazard object to the world.

**Parameters:**
- `x`: `float` - X coordinate
- `y`: `float` - Y coordinate
- `value`: `float` - Hazard severity (default: 5.0)

**Returns:** `WorldObject` - Created WorldObject

**Example:**
```python
hazard = world.createHazard(3, 3, value=2.0)
```

##### `createWall(x, y, radius=0.5)`

Create and add a wall object to the world.

**Parameters:**
- `x`: `float` - X coordinate
- `y`: `float` - Y coordinate
- `radius`: `float` - Wall radius (default: 0.5)

**Returns:** `WorldObject` - Created WorldObject

**Example:**
```python
wall = world.createWall(2, 2, radius=1.0)
```

### Context Manager Support

The SimpleWorld class can be used as a context manager for automatic resource cleanup:

```python
with pynlm.createSimpleWorld() as world:
    world.configure(15, 15, 6, 6)
    world.reset()
    # World is automatically cleaned up when exiting context
```

## SensoryInput Classes

### SensoryInput (Base Class)

Base class for all sensory input.

#### `getType()`

Get the type of sensory input.

**Parameters:** `None`

**Returns:** `str` - Sensory input type

#### `getData()`

Get the raw data as a vector.

**Parameters:** `None`

**Returns:** `list[float]` - Raw sensor data

#### `getDimensions()`

Get the dimensionality.

**Parameters:** `None`

**Returns:** `int` - Data dimensionality

#### `getTimestamp()`

Get the timestamp.

**Parameters:** `None`

**Returns:** `float` - Timestamp

#### `setTimestamp(timestamp)`

Set the timestamp.

**Parameters:**
- `timestamp`: `float` - Timestamp value

**Returns:** `None`

### Vision Class

Vision sensory input for visual data.

#### Constructor

```python
vision = pynlm.Vision(width, height, channels=3)
```

**Parameters:**
- `width`: `int` - Vision width
- `height`: `int` - Vision height
- `channels`: `int` - Number of color channels (default: 3)

**Returns:** `Vision` instance

#### `setData(data)`

Set vision data.

**Parameters:**
- `data`: `list[float]` - Vision data

**Returns:** `None`

#### `getWidth()`

Get vision width.

**Parameters:** `None`

**Returns:** `int` - Vision width

#### `getHeight()`

Get vision height.

**Parameters:** `None`

**Returns:** `int` - Vision height

#### `getChannels()`

Get number of channels.

**Parameters:** `None`

**Returns:** `int` - Number of channels

### Audio Class

Audio sensory input for sound data.

#### Constructor

```python
audio = pynlm.Audio(sampleRate, numSamples)
```

**Parameters:**
- `sampleRate`: `int` - Audio sample rate
- `numSamples`: `int` - Number of audio samples

**Returns:** `Audio` instance

#### `setData(data)`

Set audio data.

**Parameters:**
- `data`: `list[float]` - Audio data

**Returns:** `None`

#### `setSampleRate(sampleRate)`

Set sample rate.

**Parameters:**
- `sampleRate`: `int` - Sample rate

**Returns:** `None`

#### `getSampleRate()`

Get sample rate.

**Parameters:** `None`

**Returns:** `int` - Sample rate

#### `getNumSamples()`

Get number of samples.

**Parameters:** `None`

**Returns:** `int` - Number of samples

### InternalSignals Class

Internal signals sensory input for internal state data.

#### Constructor

```python
internal = pynlm.InternalSignals()
```

**Parameters:** `None`

**Returns:** `InternalSignals` instance

#### `addSignal(value)`

Add internal signal.

**Parameters:**
- `value`: `float` - Signal value

**Returns:** `None`

#### `clearSignals()`

Clear all signals.

**Parameters:** `None`

**Returns:** `None`

## Action Classes

### Action Class

Action representation for motor output.

#### Constructor

```python
action = pynlm.Action(actionType, parameters=[])
```

**Parameters:**
- `actionType`: `ActionType` or `str` - Action type
- `parameters`: `list[float]` - Action parameters (default: empty list)

**Returns:** `Action` instance

**Alternative constructor:**
```python
action = pynlm.Action(actionType)
```

#### `getType()`

Get action type.

**Parameters:** `None`

**Returns:** `ActionType` - Action type

#### `setType(type)`

Set action type.

**Parameters:**
- `type`: `ActionType` or `str` - Action type

**Returns:** `None`

#### `getParameters()`

Get action parameters.

**Parameters:** `None`

**Returns:** `list[float]` - Action parameters

#### `setParameters(params)`

Set action parameters.

**Parameters:**
- `params`: `list[float]` - Action parameters

**Returns:** `None`

#### `getName()`

Get action name.

**Parameters:** `None`

**Returns:** `str` - Action name

#### `clone()`

Create a copy of the action.

**Parameters:** `None`

**Returns:** `Action` - Copy of action

## World Classes

### WorldObject Class

World object representation.

#### Constructor

```python
obj = pynlm.WorldObject(x, y, obj_type, value=0.0, radius=0.5)
```

**Parameters:**
- `x`: `float` - X coordinate
- `y`: `float` - Y coordinate
- `obj_type`: `WorldObjectType` - Object type
- `value`: `float` - Object value (default: 0.0)
- `radius`: `float` - Object radius (default: 0.5)

**Returns:** `WorldObject` instance

#### Properties

- `x`: `float` - X coordinate
- `y`: `float` - Y coordinate
- `radius`: `float` - Object radius
- `type`: `WorldObjectType` - Object type
- `value`: `float` - Object value
- `active`: `bool` - Object active state

### AgentBody Class

Agent body state representing position, velocity, and physiological state.

#### Constructor

```python
body = pynlm.AgentBody()
```

**Parameters:** `None`

**Returns:** `AgentBody` instance

#### Properties

- `x`: `float` - X position
- `y`: `float` - Y position
- `orientation`: `float` - Orientation angle (radians)
- `velocityX`: `float` - X velocity
- `velocityY`: `float` - Y velocity
- `angularVelocity`: `float` - Angular velocity
- `energy`: `float` - Current energy
- `health`: `float` - Current health
- `age`: `float` - Current age
- `isMoving`: `bool` - Whether agent is moving
- `isTurning`: `bool` - Whether agent is turning
- `lastActionTime`: `float` - Time since last action

#### Methods

##### `reset()`

Reset agent body to initial state.

**Parameters:** `None`

**Returns:** `None`

### ActionResult Class

Action result from world interaction.

#### Constructor

```python
result = pynlm.ActionResult(reward, success, message="")
```

**Parameters:**
- `reward`: `float` - Reward value
- `success`: `bool` - Whether action was successful
- `message`: `str` - Descriptive message (default: "")

**Returns:** `ActionResult` instance

#### Properties

- `reward`: `float` - Reward value
- `success`: `bool` - Success status
- `message`: `str` - Descriptive message

### SensoryPercept Class

Sensory percept data containing all sensory modalities.

#### Constructor

```python
percept = pynlm.SensoryPercept()
```

**Parameters:** `None`

**Returns:** `SensoryPercept` instance

#### Methods

##### `getVision()`

Get vision data.

**Parameters:** `None`

**Returns:** `Vision` - Vision object

##### `setVision(vision)`

Set vision data.

**Parameters:**
- `vision`: `Vision` - Vision object

**Returns:** `None`

##### `getVisionWidth()`

Get vision width.

**Parameters:** `None`

**Returns:** `int` - Vision width

##### `getVisionHeight()`

Get vision height.

**Parameters:** `None`

**Returns:** `int` - Vision height

##### `getTouch()`

Get touch data.

**Parameters:** `None`

**Returns:** `list[float]` - Touch data

##### `setTouch(touch)`

Set touch data.

**Parameters:**
- `touch`: `list[float]` - Touch data

**Returns:** `None`

##### `getInternal()`

Get internal signals.

**Parameters:** `None`

**Returns:** `list[float]` - Internal signal data

##### `setInternal(internal)`

Set internal signals.

**Parameters:**
- `internal`: `list[float]` - Internal signal data

**Returns:** `None`

##### `getProprioception()`

Get proprioception data.

**Parameters:** `None`

**Returns:** `list[float]` - Proprioception data

##### `setProprioception(proprioception)`

Set proprioception data.

**Parameters:**
- `proprioception`: `list[float]` - Proprioception data

**Returns:** `None`

##### `getAudio()`

Get audio data.

**Parameters:** `None`

**Returns:** `Audio` - Audio object

##### `setAudio(audio)`

Set audio data.

**Parameters:**
- `audio`: `Audio` - Audio object

**Returns:** `None`

##### `getAllSignals()`

Get all sensory signals.

**Parameters:** `None`

**Returns:** `dict` - Dictionary of all signals

##### `getTimestamp()`

Get percept timestamp.

**Parameters:** `None`

**Returns:** `float` - Timestamp

##### `setTimestamp(timestamp)`

Set percept timestamp.

**Parameters:**
- `timestamp`: `float` - Timestamp value

**Returns:** `None`

## Enumeration Types

### NeuronType

```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal
```

### SynapseType

```python
pynlm.SynapseType.Excitatory
pynlm.SynapseType.Inhibitory
pynlm.SynapseType.Modulatory
pynlm.SynapseType.Electrical
pynlm.SynapseType.GapJunction
```

### DevelopmentalStage

```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

### FiringState

```python
pynlm.FiringState.Resting
pynlm.FiringState.Active
pynlm.FiringState.Refractory
pynlm.FiringState.Inhibited
```

### ActionType

```python
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.MoveLeft
pynlm.ActionType.MoveRight
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.LookUp
pynlm.ActionType.LookDown
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait
pynlm.ActionType.Custom
```

### MotorCommand

```python
pynlm.MotorCommand.MoveForward
pynlm.MotorCommand.MoveBackward
pynlm.MotorCommand.TurnLeft
pynlm.MotorCommand.TurnRight
pynlm.MotorCommand.LookLeft
pynlm.MotorCommand.LookRight
pynlm.MotorCommand.Interact
pynlm.MotorCommand.Wait
```

### WorldObjectType

```python
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

## Constants

### Mathematical Constants

```python
pynlm.PI                    # Mathematical constant π
pynlm.E                     # Euler's number
pynlm.EPSILON               # Small epsilon value for numerical comparisons
```

### Simulation Constants

```python
pynlm.DEFAULT_TIME_STEP     # Default simulation time step (0.1 seconds)
pynlm.DEFAULT_MAX_STEPS     # Default maximum simulation steps (1000)
pynlm.DEFAULT_NEURON_COUNT   # Default number of neurons (1000)
pynlm.DEFAULT_REGION_COUNT   # Default number of regions (5)
```

## Type Aliases

### NeuronId

Alias for neuron identifier (uint64_t).

```python
neuron_id = pynlm.NeuronId(42)
```

### SynapseId

Alias for synapse identifier (uint64_t).

```python
synapse_id = pynlm.SynapseId(100)
```

### RegionId

Alias for region identifier (uint64_t).

```python
region_id = pynlm.RegionId(1)
```

### PopulationId

Alias for population identifier (uint64_t).

```python
pop_id = pynlm.PopulationId(2)
```

### ActionResultType

Union type for action results (tuple of reward, success, message).

```python
result = brain.produceAction()  # Returns (reward, success, message)
```

## Error Classes

### RuntimeError

Base exception class for runtime errors.

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
```

### KeyError

Exception for missing configuration keys.

```python
if not config.has("brain.neuronCount"):
    raise pynlm.KeyError("brain.neuronCount not found in configuration")
```

### ValueError

Exception for invalid parameter values.

```python
def validate_neuron_count(count):
    if count < 1 or count > 100000:
        raise pynlm.ValueError(f"Invalid neuron count: {count}. Must be between 1 and 100000.")
```

## Utility Functions

### `createResearchSetup()`

Create a complete research setup with all components.

**Parameters:** `None`

**Returns:** `tuple[Brain, SimpleWorld, AgentBrain]` - Brain, world, and agent

**Example:**
```python
brain, world, agent = pynlm.createResearchSetup()

# Run simulation
rewards, firing_counts = pynlm.runStandardSimulation(
    brain, world, agent, 200, enableLearning=True, verbose=True
)
```

### `runStandardSimulation(brain, world, agent, numSteps, enableLearning=False, verbose=False)`

Run a standard simulation with agent learning.

**Parameters:**
- `brain`: `Brain` - Brain instance
- `world`: `SimpleWorld` - World instance
- `agent`: `AgentBrain` - Agent instance
- `numSteps`: `int` - Number of simulation steps
- `enableLearning`: `bool` - Whether to enable learning (default: False)
- `verbose`: `bool` - Whether to print verbose output (default: False)

**Returns:** `tuple[list[float], list[int]]` - (rewards, firing_counts)

**Example:**
```python
rewards, firing_counts = pynlm.runStandardSimulation(
    brain, world, agent, 200, enableLearning=True, verbose=True
)

avg_reward = sum(rewards) / len(rewards) if rewards else 0
print(f"Average reward: {avg_reward:.3f}")
```

### `createBrainConfig(config_type)`

Create brain configuration with predefined settings.

**Parameters:**
- `config_type`: `str` - Configuration type ("minimal", "standard", "research", "high-performance", "experimental")

**Returns:** `Config` - Configured brain

**Example:**
```python
config = pynlm.createBrainConfig("research")
config["brain.plasticity.enabled"] = "true"
config["brain.development.enabled"] = "true"
```

### `createSimpleWorld()`

Create a simple world instance.

**Parameters:** `None`

**Returns:** `SimpleWorld` - World instance

**Example:**
```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
```

### `createAgentBrain(brain)`

Create an agent brain interface.

**Parameters:**
- `brain`: `Brain` - Brain instance to control

**Returns:** `AgentBrain` - Agent brain instance

**Example:**
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

### `createDefaultConfig()`

Create a default configuration.

**Parameters:** `None`

**Returns:** `Config` - Default configuration

**Example:**
```python
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
```

## Function Summary

### High-Level Functions

| Function | Category | Description |
|----------|----------|-------------|
| `createResearchSetup()` | Factory | Create complete research setup |
| `runStandardSimulation()` | Execution | Run standard simulation with learning |
| `createBrainConfig()` | Factory | Create predefined brain configuration |
| `createSimpleWorld()` | Factory | Create simple world instance |
| `createAgentBrain()` | Factory | Create agent brain interface |
| `createDefaultConfig()` | Factory | Create default configuration |

### Configuration Functions

| Function | Category | Description |
|----------|----------|-------------|
| `createDefaultConfig()` | Configuration | Create default configuration |
| `createMinimalConfig()` | Configuration | Create minimal configuration |
| `createHighPerformanceConfig()` | Configuration | Create high-performance configuration |
| `createExperimentalConfig()` | Configuration | Create experimental configuration |

### Context Manager Functions

The following functions return objects that can be used as context managers:

- `createBrain(config)`
- `createAgentBrain(brain)`
- `createSimpleWorld()`
- `createDefaultConfig()`

## Version Information

```python
import pynlm
print(f"NLM Python Bindings version: {pynlm.__version__}")
```

## Copyright and License

Copyright © 2026 NLM Research Project

MIT License - See LICENSE file for details

## Author Information

For questions, issues, or feature requests:

- **Documentation Issues**: Report documentation bugs at https://github.com/Kilo-Org/kilocode/issues
- **Feature Requests**: Request new features at https://github.com/Kilo-Org/kilocode/issues
- **General Support**: Get help at https://kilo.ai/docs

## Compatibility

- **Python**: >= 3.8
- **Compiler**: C++20 compiler with pybind11 support
- **Platform**: Cross-platform (Linux, macOS, Windows)

## Building from Source

### Prerequisites

```bash
pip install scikit-build-core pybind11 numpy pytest
```

### Build Instructions

```bash
cd /path/to/nlm
pip install -e .  # Install with scikit-build-core
```

Or use traditional CMake build:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Testing

### Running Tests

```bash
# Run Python unit tests
pytest

# Run integration tests
python -m pytest tests/ -v

# Run performance benchmarks
python -m pytest benchmarks/ -v
```

### Test Coverage

See `TEST_COVERAGE_REPORT.md` for detailed test coverage information.

## Acknowledgements

This documentation was generated from the NLM Python bindings source code (pybind11 module).

Special thanks to:
- The NLM development team for their hard work
- Python community for pybind11 and related tools
- Documentation contributors for their valuable input

## Changes History

### Version 1.0.0
- Initial release of NLM Python bindings
- Comprehensive API documentation
- Full integration with NLM Phase 6 systems

### Version 0.9.0
- Early beta release
- Basic configuration and simulation capabilities
- Foundation for advanced research features

## Disclaimer

This is a research project. NLM is an experimental brain simulation framework for scientific research purposes. It is not intended for commercial or safety-critical applications.

The NLM Python bindings are part of the NLM research project and are provided as-is. While every effort has been made to ensure correctness and reliability, bugs and limitations may exist. Use at your own risk.

For research purposes and collaboration, please reference:

NLM Research Project. (2026). NLM/熙然 - Neural Learning Machine. GitHub: https://github.com/Kilo-Org/kilocode

## End of API Reference

This document serves as the definitive reference for the NLM Python bindings API. For the most up-to-date information, please check the official documentation repository.

For additional examples, tutorials, and getting started guides, see:
- `README.md` - Basic setup and installation
- `HOW_TO_USE.md` - Comprehensive usage guide  
- `easy_usage.md` - Quick start guide for beginners
- `docs/` - Technical documentation directory

---

*This documentation was automatically generated from the NLM Python bindings source code using the pybind11 documentation system.*