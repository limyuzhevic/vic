# NLM Python API Reference

## Overview

This document provides comprehensive API documentation for the NLM (Neural Learning Machine) Python bindings. The NLM framework is a modular neural simulation platform that supports real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

## Key Components

The main Python API is exposed through the `pynlm` module, which provides factory functions to create NLM components. The core classes include:

- **Brain** - Central neural simulation coordinator
- **AgentBrain** - Bridge connecting brain to world
- **SimpleWorld** - 2D simulation environment
- **Config** - Configuration management system
- **Action** - Motor command representation
- **SensoryInput** - Sensory input base class (Vision, Audio, InternalSignals)
- **SensoryPercept** - Processed sensory data

## Configuration API

### `createDefaultConfig()`

**Returns**: `Config` object

**Description**: Creates a new configuration object with default settings.

**Example**:
```python
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
```

### `Config.loadFromFile(filepath)`

**Parameters**:
- `filepath` (str): Path to JSON configuration file

**Description**: Loads configuration from a JSON file. Returns `True` on success, `False` on failure.

**Example**:
```python
config = pynlm.createDefaultConfig()
if config.loadFromFile("config.json"):
    print("Configuration loaded successfully")
else:
    print("Failed to load configuration")
```

### `Config.loadFromArgs(argc, argv)`

**Parameters**:
- `argc` (int): Number of arguments
- `argv` (list): Command line arguments

**Description**: Loads configuration from command line arguments.

### `Config.set(key, value, source=ConfigSource.Runtime)`

**Parameters**:
- `key` (str): Configuration key
- `value`: Configuration value (int, float, bool, str, list)
- `source` (ConfigSource): Configuration source (Default, File, CommandLine, Runtime)

**Description**: Sets a configuration value.

**Example**:
```python
config = pynlm.createDefaultConfig()
config.set("world.width", 100.0)
config.set("world.height", 100.0)
config.set("brain.regionCount", 3)
```

### `Config.get(key)`

**Parameters**:
- `key` (str): Configuration key

**Returns**: `Optional[T]` - Value if key exists, None otherwise

**Example**:
```python
config = pynlm.createDefaultConfig()
width = config.get("world.width")
if width is not None:
    print(f"World width: {width}")
```

### `Config.getOr(key, defaultValue)`

**Parameters**:
- `key` (str): Configuration key
- `defaultValue`: Default value if key doesn't exist

**Returns**: `T` - Value or default value

### `Config.has(key)`

**Parameters**:
- `key` (str): Configuration key

**Returns**: `bool` - True if key exists

### `Config.clear()`

**Description**: Removes all configuration entries.

### `Config.summary()`

**Returns**: `str` - Human-readable summary of configuration

## Brain API

### `createBrain(config)`

**Parameters**:
- `config` (Config): Brain configuration

**Returns**: `Brain` object

**Description**: Creates a new neural brain simulation.

**Example**:
```python
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
```

### `Brain.initialize()`

**Description**: Initializes the brain with configuration. Must be called before any other operations.

### `Brain.step(step_number)`

**Parameters**:
- `step_number` (int): Current simulation step

**Description**: Performs a simulation step. Updates neural dynamics, processes spikes, and applies learning.

### `Brain.step(step_number, current_time)`

**Parameters**:
- `step_number` (int): Current simulation step
- `current_time` (float): Current simulation time

**Description**: Performs a simulation step with timestamp.

### `Brain.receiveSensoryInput(input)`

**Parameters**:
- `input` (SensoryInput): Sensory input to inject

**Description**: Injects sensory current into sensory neurons based on input pattern.

### `Brain.injectCurrent(neuron, current)`

**Parameters**:
- `neuron` (NeuronId): Target neuron ID
- `current` (float): Current to inject

**Description**: Injects current directly into a specific neuron.

### `Brain.injectCurrentToNeurons(type, current)`

**Parameters**:
- `type` (NeuronType): Type of neurons to inject into
- `current` (float): Current to inject

**Description**: Injects current into all neurons of a specific type.

### `Brain.produceAction()`

**Returns**: `Action` object

**Description**: Produces motor action based on neural activity.

### `Brain.applyNeuromodulation(signal)`

**Parameters**:
- `signal` (Neuromodulator): Neuromodulatory signal to apply

**Description**: Applies neuromodulatory signals to plasticity and neural activity.

### `Brain.updatePlasticity()`

**Description**: Updates plasticity rules (STDP, Hebbian learning). Called automatically in step.

### `Brain.develop()`

**Description**: Applies developmental changes (synaptogenesis, pruning). Called automatically in step.

### `Brain.reset()`

**Description**: Resets brain state to initial conditions.

### `Brain.save(filepath)`

**Parameters**:
- `filepath` (str): File path to save state

**Returns**: `bool` - True on success

**Description**: Saves brain state to file (checkpointing).

### `Brain.load(filepath)`

**Parameters**:
- `filepath` (str): File path to load state

**Returns**: `bool` - True on success

**Description**: Loads brain state from file.

### `Brain.addRegion(name="")`

**Parameters**:
- `name` (str): Optional region name

**Returns**: `RegionId` - ID of newly created region

**Description**: Adds a new neural region to the brain.

### `Brain.getRegion(id)`

**Parameters**:
- `id` (RegionId): Region identifier

**Returns**: `NeuralRegion` object

**Description**: Gets a region by ID.

### `Brain.getRegionCount()`

**Returns**: `int` - Number of regions

### `Brain.getTotalNeuronCount()`

**Returns**: `int` - Total neuron count across all regions

### `Brain.getTotalSynapseCount()`

**Returns**: `int` - Total synapse count across all regions

### `Brain.getActiveNeuronCount()`

**Returns**: `int` - Count of active neurons

### `Brain.getFiringNeuronCount()`

**Returns**: `int` - Count of currently firing neurons

### `Brain.getAverageFiringRate()`

**Returns**: `float` - Average firing rate across all neurons

### `Brain.getExcitationInhibitionRatio()`

**Returns**: `float` - Excitation/inhibition balance ratio

### `Brain.getTotalSpikeCount()`

**Returns**: `int` - Total spike count

### `Brain.getPendingSpikeEventCount()`

**Returns**: `int` - Count of pending spike events

### `Brain.getDevelopmentalStage()`

**Returns**: `DevelopmentalStage` - Current developmental stage

### `Brain.setDevelopmentalStage(stage)`

**Parameters**:
- `stage` (DevelopmentalStage): Target developmental stage

### `Brain.logStatus()`

**Description**: Logs brain status to console.

## AgentBrain API

### `createAgentBrain(brain)`

**Parameters**:
- `brain` (Brain): Underlying brain

**Returns**: `AgentBrain` object

**Description**: Creates an agent brain interface.

### `AgentBrain.initialize(world)`

**Parameters**:
- `world` (SimpleWorld): World to initialize with

**Description**: Initializes agent with world and connects sensory/motor systems.

### `AgentBrain.processSensoryInput(percept)`

**Parameters**:
- `percept` (SensoryPercept): Sensory perception to process

**Description**: Processes sensory percept and injects into brain.

### `AgentBrain.decodeMotorCommand()`

**Returns**: `MotorCommand` - Decoded motor command

**Description**: Decodes brain motor activity into motor command.

### `AgentBrain.applyRewardModulation(reward, predicted_reward)`

**Parameters**:
- `reward` (float): Actual reward received
- `predicted_reward` (float): Predicted reward

**Description**: Applies reward-based neuromodulation.

### `AgentBrain.updateDevelopment(timestep)`

**Parameters**:
- `timestep` (float): Simulation timestep

**Description**: Updates development system.

### `AgentBrain.enableRewardModulation(enable)`

**Parameters**:
- `enable` (bool): Enable/disable reward modulation

### `AgentBrain.enableStructuralPlasticity(enable)`

**Parameters**:
- `enable` (bool): Enable/disable structural plasticity

### `AgentBrain.enableDevelopment(enable)`

**Parameters**:
- `enable` (bool): Enable/disable development

### `AgentBrain.enableCuriosity(enable)`

**Parameters**:
- `enable` (bool): Enable/disable curiosity

## SimpleWorld API

### `createSimpleWorld()`

**Returns**: `SimpleWorld` object

**Description**: Creates a new simple 2D world.

### `SimpleWorld.configure(width, height, visionWidth, visionHeight)`

**Parameters**:
- `width` (float): World width
- `height` (float): World height
- `visionWidth` (int): Agent vision width
- `visionHeight` (int): Agent vision height

**Description**: Configures world dimensions and agent sensory capabilities.

### `SimpleWorld.reset()`

**Description**: Resets world to initial state.

### `SimpleWorld.setAgentStart(x, y)`

**Parameters**:
- `x` (float): Starting X position
- `y` (float): Starting Y position

**Description**: Sets agent starting position.

### `SimpleWorld.update(timestep)`

**Parameters**:
- `timestep` (float): Simulation timestep

**Description**: Updates world state (called each simulation step).

### `SimpleWorld.applyMotorCommand(cmd, currentTime)`

**Parameters**:
- `cmd` (MotorCommand): Motor command to apply
- `currentTime` (float): Current simulation time

**Returns**: `ActionResult` - Result of action

**Description**: Applies motor command to agent.

### `SimpleWorld.getSensoryPercept()`

**Returns**: `SensoryPercept` - Current sensory perception

**Description**: Gets current sensory percept for the agent.

### `SimpleWorld.getAgentBody()`

**Returns**: `AgentBody` - Current agent state

**Description**: Gets agent body state.

### `SimpleWorld.addObject(obj)`

**Parameters**:
- `obj` (WorldObject): World object to add

**Description**: Adds object to world.

### `SimpleWorld.removeObject(x, y)`

**Parameters**:
- `x` (float): X position
- `y` (float): Y position

**Description**: Removes object at position.

### `SimpleWorld.isValidPosition(x, y)`

**Parameters**:
- `x` (float): X position
- `y` (float): Y position

**Returns**: `bool` - True if position is valid (not wall)

## Sensory API

### Vision

**Constructor**: `Vision(width, height, channels=3)`

**Methods**:
- `getWidth()` - Vision width
- `getHeight()` - Vision height
- `getChannels()` - Number of color channels
- `setData(data)` - Set vision data (list of floats)

### Audio

**Constructor**: `Audio(sampleRate, numSamples)`

**Methods**:
- `getSampleRate()` - Sample rate
- `getNumSamples()` - Number of samples
- `setData(data)` - Set audio data (list of floats)
- `setSampleRate(sampleRate)` - Set sample rate

### InternalSignals

**Constructor**: `InternalSignals()`

**Methods**:
- `addSignal(value)` - Add a signal value
- `clearSignals()` - Clear all signals

## Action API

**Constructor**: `Action(type, parameters=[])`

**Properties**:
- `type` (ActionType) - Action type
- `parameters` (list) - Action parameters

**Methods**:
- `getType()` - Get action type
- `setType(type)` - Set action type
- `getParameters()` - Get parameters
- `setParameters(params)` - Set parameters
- `getName()` - Get action name
- `clone()` - Create copy

## Enums and Types

### NeuronType

- `Excitatory` - Excitatory neuron
- `Inhibitory` - Inhibitory neuron
- `Modulatory` - Modulatory neuron
- `Sensory` - Sensory neuron
- `Motor` - Motor neuron
- `Internal` - Internal neuron

### SynapseType

- `Excitatory` - Excitatory synapse
- `Inhibitory` - Inhibitory synapse
- `Modulatory` - Modulatory synapse
- `Electrical` - Electrical synapse
- `GapJunction` - Gap junction synapse

### ActionType

- `MoveForward`, `MoveBackward`
- `MoveLeft`, `MoveRight`
- `TurnLeft`, `TurnRight`
- `Look`, `LookUp`, `LookDown`
- `Interact`, `Eat`, `Drink`
- `Rest`, `Wait`, `Custom`

### MotorCommand

- `MoveForward`, `MoveBackward`
- `TurnLeft`, `TurnRight`
- `LookLeft`, `LookRight`
- `Interact`, `Wait`

### WorldObjectType

- `Empty` - Empty space
- `Resource` - Resource object
- `Hazard` - Hazardous object
- `Wall` - Wall object
- `Marker` - Marker object

### DevelopmentalStage

- `Initial` - Initial stage
- `CriticalPeriod` - Critical period
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

### FiringState

- `Resting` - Resting state
- `Active` - Active state
- `Refractory` - Refractory state
- `Inhibited` - Inhibited state

## Common Pitfalls

1. **Initialization**: Always call `brain.initialize()` before any operations
2. **Simulation Loop**: Include `world.update(timestep)` in your simulation loop
3. **Sensory Input**: Connect sensory neurons properly to receive input
4. **Memory Systems**: Use `getWorkingMemory()`, `getEpisodicMemory()`, etc. to access memory
5. **Neuromodulation**: Enable neuromodulators before using reward learning
6. **Development**: Call `updateDevelopment(timestep)` for developmental changes
7. **Error Handling**: Use try-catch for file operations and invalid parameters

## Example Advanced Usage

See `advanced_examples.md` for comprehensive advanced usage examples.
