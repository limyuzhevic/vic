# NLM Python API Reference

This document provides detailed reference information for the NLM Python bindings.

## Module Structure

The main module is `pynlm`. All classes and functions are accessible directly from this module.

```python
import pynlm

# Create objects
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
config = pynlm.createDefaultConfig()
```

## Core Classes

### Config

The `Config` class manages neural simulation parameters.

#### Methods

- `loadFromFile(filepath: str) -> bool`
  - Load configuration from file
  - Returns `True` if successful, `False` otherwise

- `loadFromArgs(args: list[str]) -> bool`
  - Load configuration from command line arguments
  - Python equivalent of C++ `loadFromArgs(int argc, char** argv)`
  - Returns `True` if successful, `False` otherwise

- `saveToFile(filepath: str) -> bool`
  - Save configuration to file
  - Returns `True` if successful, `False` otherwise

- `has(key: str) -> bool`
  - Check if configuration has a key

- `getKeys() -> list[str]`
  - Get all configuration keys

- `clear() -> None`
  - Clear all configuration entries

- `summary() -> str`
  - Get configuration summary as a formatted string

#### Property Access Methods

- `getInt(key: str, default: int) -> int`
  - Get integer configuration value

- `getInt64(key: str, default: int) -> int`
  - Get 64-bit integer configuration value

- `getDouble(key: str, default: float) -> float`
  - Get floating-point configuration value

- `getBool(key: str, default: bool) -> bool`
  - Get boolean configuration value

- `getString(key: str, default: str) -> str`
  - Get string configuration value

#### Setting Methods

- `set(key: str, value: int) -> None`
  - Set integer configuration value

- `set(key: str, value: float) -> None`
  - Set floating-point configuration value

- `set(key: str, value: bool) -> None`
  - Set boolean configuration value

- `set(key: str, value: str) -> None`
  - Set string configuration value

### Brain

The `Brain` class represents an artificial neural system.

#### Constructor

- `createBrain(config: Config) -> Brain`
  - Create a new brain with configuration

#### Core Methods

- `initialize() -> None`
  - Initialize the brain with configuration

- `step(currentStep: int) -> None`
  - Perform a simulation step with step number

- `step(currentStep: int, currentTime: float) -> None`
  - Perform a simulation step with step number and timestamp

- `receiveSensoryInput(input: SensoryInput) -> None`
  - Inject sensory input into the brain

- `injectCurrent(neuron: NeuronId, current: float) -> None`
  - Inject current into a specific neuron

- `injectCurrentToNeurons(neuronType: NeuronType, current: float) -> None`
  - Inject current into all neurons of a specific type

- `produceAction() -> Action`
  - Produce motor action based on neural activity

- `reset() -> None`
  - Reset brain state

- `save(filepath: str) -> bool`
  - Save brain state to file

- `load(filepath: str) -> bool`
  - Load brain state from file

- `addRegion(name: str = "") -> RegionId`
  - Add a new neural region

- `getRegion(id: RegionId) -> NeuralRegion`
  - Get a region by ID

- `getRegionCount() -> int`
  - Get the number of regions

- `getRegionIds() -> list[RegionId]`
  - Get all region IDs

- `getRegions() -> list[NeuralRegion]`
  - Get all regions

- `getTotalNeuronCount() -> int`
  - Get total neuron count across all regions

- `getTotalSynapseCount() -> int`
  - Get total synapse count across all regions

- `getActiveNeuronCount() -> int`
  - Get count of active neurons

- `getFiringNeuronCount() -> int`
  - Get count of currently firing neurons

- `getAverageFiringRate() -> float`
  - Get average firing rate across all neurons

- `getExcitationInhibitionRatio() -> float`
  - Get excitation/inhibition balance ratio

- `getTotalSpikeCount() -> int`
  - Get total spike count

- `getDevelopmentalStage() -> DevelopmentalStage`
  - Get current developmental stage

- `setDevelopmentalStage(stage: DevelopmentalStage) -> None`
  - Set developmental stage

- `getConfig() -> Config`
  - Get the configuration

- `logStatus() -> None`
  - Log brain status

### AgentBrain

The `AgentBrain` class bridges the connection between the brain and the world.

#### Constructor

- `createAgentBrain(brain: Brain) -> AgentBrain`
  - Create a new agent brain interface

#### Core Methods

- `initialize(world: SimpleWorld) -> None`
  - Initialize with world

- `getSensoryInputSize() -> int`
  - Get expected sensory input size

- `getMotorOutputSize() -> int`
  - Get expected motor output size

- `processSensoryInput(percept: SensoryPercept) -> None`
  - Process sensory percept and inject into brain

- `decodeMotorCommand() -> MotorCommand`
  - Decode brain motor activity into motor command

- `applyRewardModulation(reward: float, predictedReward: float) -> None`
  - Apply reward-based neuromodulation

- `updateDevelopment(timestep: float) -> None`
  - Update development system

#### Query Methods

- `getDevelopmentalStage() -> DevelopmentalStage`
  - Get current developmental stage

- `getNeuromodulationLevel() -> float`
  - Get current neuromodulation level

- `getCuriosityLevel() -> float`
  - Get curiosity level

- `getNoveltyLevel() -> float`
  - Get novelty level

- `getPredictionError() -> float`
  - Get prediction error

#### Control Methods

- `reset() -> None`
  - Reset agent for new episode

- `getBrain() -> Brain`
  - Get the underlying brain

- `enableRewardModulation(enable: bool) -> None`
  - Enable/disable reward modulation

- `enableStructuralPlasticity(enable: bool) -> None`
  - Enable/disable structural plasticity

- `enableDevelopment(enable: bool) -> None`
  - Enable/disable development

- `enableCuriosity(enable: bool) -> None`
  - Enable/disable curiosity

#### Status Methods

- `isRewardModulationEnabled() -> bool`
  - Check if reward modulation is enabled

- `isStructuralPlasticityEnabled() -> bool`
  - Check if structural plasticity is enabled

- `isDevelopmentEnabled() -> bool`
  - Check if development is enabled

- `isCuriosityEnabled() -> bool`
  - Check if curiosity is enabled

### SimpleWorld

The `SimpleWorld` class represents the simulated environment.

#### Constructor

- `createSimpleWorld() -> SimpleWorld`
  - Create a new simple world

#### Configuration Methods

- `configure(width: int, height: int, visionWidth: int, visionHeight: int) -> None`
  - Configure world dimensions and sensory fields

- `setAgentStart(x: float, y: float) -> None`
  - Set agent's starting position

#### Simulation Methods

- `reset() -> None`
  - Reset world to initial state

- `update(timestep: float) -> None`
  - Update world by given timestep

- `applyMotorCommand(cmd: MotorCommand, currentTime: float) -> ActionResult`
  - Apply motor command and return action result

- `getSensoryPercept() -> SensoryPercept`
  - Get sensory percept from current world state

- `getAgentBody() -> AgentBody`
  - Get current agent body state

#### Object Management Methods

- `addObject(obj: WorldObject) -> None`
  - Add world object

- `removeObject(x: float, y: float) -> None`
  - Remove world object at given position

- `isValidPosition(x: float, y: float) -> bool`
  - Check if position is valid

#### Query Methods

- `getWidth() -> int`
  - Get world width

- `getHeight() -> int`
  - Get world height

- `getMaxEnergy() -> float`
  - Get maximum energy

- `setMaxEnergy(e: float) -> None`
  - Set maximum energy

- `getEnergyDecayRate() -> float`
  - Get energy decay rate

- `setEnergyDecayRate(r: float) -> None`
  - Set energy decay rate

- `getSimulationTime() -> float`
  - Get simulation time

- `setRandomSeed(seed: int) -> None`
  - Set random seed

- `getRandomSeed() -> int`
  - Get random seed

### SensoryInput and Subclasses

#### SensoryInput (Base Class)

- `getType() -> NeuronType`
  - Get the type of sensory input

- `getData() -> list[float]`
  - Get the raw data as a list

- `getDimensions() -> int`
  - Get the dimensionality

- `getTimestamp() -> float`
  - Get the timestamp

- `setTimestamp(timestamp: float) -> None`
  - Set the timestamp

#### Vision (SensoryInput subclass)

- `Vision(width: int, height: int, channels: int = 3)`
  - Create vision sensory input

- `setData(data: list[float]) -> None`
  - Set vision data

- `getWidth() -> int`
  - Get vision width

- `getHeight() -> int`
  - Get vision height

- `getChannels() -> int`
  - Get number of channels

#### Audio (SensoryInput subclass)

- `Audio(sampleRate: int, numSamples: int)`
  - Create audio sensory input

- `setData(data: list[float]) -> None`
  - Set audio data

- `setSampleRate(sampleRate: int) -> None`
  - Set sample rate

- `getSampleRate() -> int`
  - Get sample rate

- `getNumSamples() -> int`
  - Get number of samples

#### InternalSignals (SensoryInput subclass)

- `InternalSignals()`
  - Create internal signals sensory input

- `addSignal(value: float) -> None`
  - Add a signal value

- `clearSignals() -> None`
  - Clear all signals

### Action

The `Action` class represents motor commands.

#### Constructors

- `Action()`
  - Create empty action

- `Action(type: ActionType)`
  - Create action with type

- `Action(type: ActionType, parameters: list[float])`
  - Create action with type and parameters

#### Properties

- `type: ActionType`
  - Action type

- `parameters: list[float]`
  - Action parameters

#### Methods

- `getType() -> ActionType`
  - Get action type

- `setType(type: ActionType) -> None`
  - Set action type

- `getParameters() -> list[float]`
  - Get action parameters

- `setParameters(parameters: list[float]) -> None`
  - Set action parameters

- `getName() -> str`
  - Get action name

- `clone() -> Action`
  - Create a copy of the action

### AgentBody

The `AgentBody` class represents the agent's physical state.

#### Properties

- `x: float`
  - X position

- `y: float`
  - Y position

- `orientation: float`
  - Orientation (radians)

- `velocityX: float`
  - X velocity

- `velocityY: float`
  - Y velocity

- `angularVelocity: float`
  - Angular velocity

- `energy: float`
  - Current energy

- `health: float`
  - Current health

- `age: float`
  - Age

- `isMoving: bool`
  - Movement state

- `isTurning: bool`
  - Turning state

- `lastActionTime: float`
  - Time since last action

#### Methods

- `reset() -> None`
  - Reset to initial state

### WorldObject

The `WorldObject` class represents objects in the world.

#### Constructor

- `WorldObject(x: float, y: float, type: WorldObjectType, value: float = 0.0, radius: float = 0.5)`
  - Create world object

#### Properties

- `x: float`
  - X position

- `y: float`
  - Y position

- `radius: float`
  - Radius

- `type: WorldObjectType`
  - Object type

- `value: float`
  - Object value

- `active: bool`
  - Active state

### ActionResult

The `ActionResult` class represents the result of an action.

#### Constructor

- `ActionResult()`
  - Create empty action result

- `ActionResult(reward: float, success: bool, message: str = "")`
  - Create action result with parameters

#### Properties

- `reward: float`
  - Reward value

- `success: bool`
  - Success status

- `message: str`
  - Result message

### SensoryPercept

The `SensoryPercept` class represents processed sensory information.

#### Constructor

- `SensoryPercept()`
  - Create empty sensory percept

#### Methods

- `getVision() -> Vision`
  - Get vision data

- `setVision(vision: Vision) -> None`
  - Set vision data

- `getVisionWidth() -> int`
  - Get vision width

- `getVisionHeight() -> int`
  - Get vision height

- `getTouch() -> list[float]`
  - Get touch data

- `setTouch(touch: list[float]) -> None`
  - Set touch data

- `getInternal() -> list[float]`
  - Get internal signals

- `setInternal(internal: list[float]) -> None`
  - Set internal signals

- `getProprioception() -> list[float]`
  - Get proprioception data

- `setProprioception(proprioception: list[float]) -> None`
  - Set proprioception data

- `getAudio() -> list[float]`
  - Get audio data

- `setAudio(audio: list[float]) -> None`
  - Set audio data

- `getAllSignals() -> list[list[float]]`
  - Get all sensory signals

- `getTimestamp() -> float`
  - Get timestamp

- `setTimestamp(timestamp: float) -> None`
  - Set timestamp

## Constants

### NeuronId Constants

- `pynlm.INVALID_NEURON_ID`
  - Invalid neuron ID

### SynapseId Constants

- `pynlm.INVALID_SYNAPSE_ID`
  - Invalid synapse ID

### RegionId Constants

- `pynlm.INVALID_REGION_ID`
  - Invalid region ID

### PopulationId Constants

- `pynlm.INVALID_POPULATION_ID`
  - Invalid population ID

## Enumeration Types

### NeuronType

- `pynlm.NeuronType.Excitatory`
- `pynlm.NeuronType.Inhibitory`
- `pynlm.NeuronType.Modulatory`
- `pynlm.NeuronType.Sensory`
- `pynlm.NeuronType.Motor`
- `pynlm.NeuronType.Internal`

### SynapseType

- `pynlm.SynapseType.Excitatory`
- `pynlm.SynapseType.Inhibitory`
- `pynlm.SynapseType.Modulatory`
- `pynlm.SynapseType.Electrical`
- `pynlm.SynapseType.GapJunction`

### DevelopmentalStage

- `pynlm.DevelopmentalStage.Initial`
- `pynlm.DevelopmentalStage.CriticalPeriod`
- `pynlm.DevelopmentalStage.Maturation`
- `pynlm.DevelopmentalStage.Adult`
- `pynlm.DevelopmentalStage.Aging`

### FiringState

- `pynlm.FiringState.Resting`
- `pynlm.FiringState.Active`
- `pynlm.FiringState.Refractory`
- `pynlm.FiringState.Inhibited`

### ActionType

- `pynlm.ActionType.MoveForward`
- `pynlm.ActionType.MoveBackward`
- `pynlm.ActionType.MoveLeft`
- `pynlm.ActionType.MoveRight`
- `pynlm.ActionType.TurnLeft`
- `pynlm.ActionType.TurnRight`
- `pynlm.ActionType.Look`
- `pynlm.ActionType.LookUp`
- `pynlm.ActionType.LookDown`
- `pynlm.ActionType.Interact`
- `pynlm.ActionType.Eat`
- `pynlm.ActionType.Drink`
- `pynlm.ActionType.Rest`
- `pynlm.ActionType.Wait`
- `pynlm.ActionType.Custom`

### MotorCommand

- `pynlm.MotorCommand.MoveForward`
- `pynlm.MotorCommand.MoveBackward`
- `pynlm.MotorCommand.TurnLeft`
- `pynlm.MotorCommand.TurnRight`
- `pynlm.MotorCommand.LookLeft`
- `pynlm.MotorCommand.LookRight`
- `pynlm.MotorCommand.Interact`
- `pynlm.MotorCommand.Wait`

### WorldObjectType

- `pynlm.WorldObjectType.Empty`
- `pynlm.WorldObjectType.Resource`
- `pynlm.WorldObjectType.Hazard`
- `pynlm.WorldObjectType.Wall`
- `pynlm.WorldObjectType.Marker`

## Error Handling

The Python bindings use standard Python exceptions. Common exceptions include:

- `RuntimeError`: For runtime errors
- `ValueError`: For invalid parameter values
- `AttributeError`: For missing attributes
- `TypeError`: For type mismatches

## Installation Verification

To verify that the Python bindings are properly installed:

```python
import pynlm

# Test basic functionality
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

print("✓ NLM Python bindings installed correctly")
```

## Advanced Topics

### Configuration Files

Configuration can be loaded from various file formats. See `Config.loadFromFile()` for details.

### Simulation Control

Advanced control of simulation parameters is available through the configuration system.

### Custom Sensory Inputs

Custom sensory input types can be created by subclassing `SensoryInput`.

### Agent Behavior Control

Agent behavior can be controlled through various neuromodulation mechanisms.

## Troubleshooting

### Common Issues

1. **ImportError: No module named 'pynlm'**
   - Ensure the Python bindings are built and installed
   - Check `sys.path` includes the Python module directory

2. **RuntimeError: Unable to initialize brain**
   - Check that configuration is valid
   - Ensure all required files are present

3. **AttributeError: 'NoneType' object has no attribute 'someMethod'**
   - Check that objects are properly initialized before use
   - Verify that required dependencies are available

### Getting Help

For additional help, see:

- Python documentation: `python/README.md`
- C++ documentation: `docs/ARCHITECTURE.md`
- Examples: `examples/` directory (if available)

## Further Reading

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [NLM Python Examples](examples/) (if available)
- [NLM Development Guide](docs/ROADMAP.md)
- [NLM Phase 6 Audit](docs/PHASE6_FINAL_AUDIT.md)

## License

The NLM Python bindings are distributed under the MIT license. See `LICENSE.md` for details.

---

**Last updated: 2026-11-15**

**Phase: 6 (Final Integration)**

**Status: Production Ready**

This Python API reference is automatically generated from the C++ bindings and is subject to change as the project evolves.