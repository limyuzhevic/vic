# NLM Python API Documentation

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. This document provides comprehensive documentation for the Python bindings (`pynlm`) that interface with the C++ NLM neural simulation framework.

## Core Classes

### Brain Class

#### Purpose
The central neural simulation brain class that implements spiking neural computation with event-driven dynamics and integrated memory, prediction, cognition, and neuromodulation systems.

#### Methods

**Constructor**
```python
Brain(config: Config) -> Brain
```
Create a new brain with configuration.

**Initialization**
```python
brain.initialize() -> bool
```
Initialize the brain with configuration. Always call this before using the brain.

**Simulation**
```python
brain.step(currentStep: SimulationStep) -> None
```
Perform a simulation step (with timestamp inferred from step number).

```python
brain.step(currentStep: SimulationStep, currentTime: Timestamp) -> None
```
Perform a simulation step with explicit timestamp.

**Input Processing**
```python
brain.receiveSensoryInput(input: SensoryInput) -> None
```
Inject sensory input into the brain. Injects current into sensory neurons based on input pattern.

```python
brain.injectCurrent(neuron: NeuronId, current: MembranePotential) -> None
```
Inject current directly into a specific neuron.

```python
brain.injectCurrentToNeurons(neuronType: NeuronType, current: MembranePotential) -> None
```
Inject current into all neurons of a specific type.

**Output Processing**
```python
action = brain.produceAction() -> Action
```
Produce motor/action output based on motor neuron activity.

**State Management**
```python
brain.reset() -> None
```
Reset brain state.

```python
brain.save(filepath: str) -> bool
```\nSave brain state to file (checkpointing).

```python
brain.load(filepath: str) -> bool
```
Load brain state from file.

**Region Management**
```python
regionId = brain.addRegion(name: str = "") -> RegionId
```
Add a new neural region.

```python
region = brain.getRegion(regionId: RegionId) -> NeuralRegion
```
Get a region by ID.

**Statistics**
```python
neuronCount = brain.getTotalNeuronCount() -> int
```
Get total neuron count across all regions.

```python
synapseCount = brain.getTotalSynapseCount() -> int
```
Get total synapse count across all regions.

```python
activeCount = brain.getActiveNeuronCount() -> int
```
Get count of active neurons.

```python
firingCount = brain.getFiringNeuronCount() -> int
```
Get count of currently firing neurons.

```python
averageRate = brain.getAverageFiringRate() -> float
```
Get average firing rate across all neurons.

```python
eiRatio = brain.getExcitationInhibitionRatio() -> float
```
Get excitation/inhibition balance ratio.

```python
totalSpikes = brain.getTotalSpikeCount() -> int
```
Get total spike count.

**System Access**
```python
spikeSystem = brain.getSpikeSystem() -> SpikeSystem
```
Get spike system reference.

```python
stdp = brain.getSTDP() -> STDP
```
Get STDP plasticity rule reference.

```python
hebbian = brain.getHebbian() -> Hebbian
```
Get Hebbian plasticity rule reference.

```python
structural = brain.getStructuralPlasticity() -> StructuralPlasticity
```
Get structural plasticity rule reference.

**Development**
```python
stage = brain.getDevelopmentalStage() -> DevelopmentalStage
```
Get current developmental stage.

```python
brain.setDevelopmentalStage(stage: DevelopmentalStage) -> None
```
Set developmental stage.

**Memory Systems**
```python
workingMemory = brain.getWorkingMemory() -> NeuralWorkingMemory
```
Get working memory system reference.

```python
episodicMemory = brain.getEpisodicMemory() -> NeuralEpisodicMemory
```
Get episodic memory system reference.

```python
associativeMemory = brain.getAssociativeMemory() -> NeuralAssociativeMemory
```
Get associative memory system reference.

**Prediction System**
```python
predictionSystem = brain.getPredictionSystem() -> PredictionSystem
```
Get prediction system reference.

**Cognition Systems**
```python
planner = brain.getPlanner() -> NeuralPlanner
```
Get neural planner reference.

```python
conceptFormation = brain.getConceptFormation() -> ConceptFormation
```
Get concept formation reference.

```python
attention = brain.getAttention() -> AttentionalSelection
```
Get attentional selection reference.

**Development System**
```python
developmentSystem = brain.getDevelopmentSystem() -> DevelopmentSystem
```
Get development system reference.

**Neuromodulation Systems**
```python
dopamine = brain.getDopamine() -> Dopamine
```
Get dopamine neuromodulator reference.

```python
curiosity = brain.getCuriosity() -> Curiosity
```
Get curiosity neuromodulator reference.

```python
novelty = brain.getNovelty() -> Novelty
```
Get novelty neuromodulator reference.

```python
predictionError = brain.getPredictionErrorSignal() -> PredictionError
```
Get prediction error neuromodulator reference.

**Configuration**
```python
config = brain.getConfig() -> Config
```
Get the configuration.

**Logging**
```python
brain.logStatus() -> None
```
Log brain status.

### Config Class

#### Purpose
Configuration management system for NLM. Supports multiple file formats (JSON, YAML, simple key=value).

#### Methods

**Constructor**
```python
config = Config() -> Config
```
Create a default configuration.

**File I/O**
```python
config.loadFromFile(filepath: str) -> bool
```
Load configuration from a file. Automatically detects JSON, YAML, or simple format.

```python
config.loadFromArgs(argc: int, argv: List[str]) -> bool
```
Load configuration from command line arguments.

```python
config.saveToFile(filepath: str) -> bool
```
Save configuration to a file.

**Key-Value Operations**
```python
config.has(key: str) -> bool
```
Check if a configuration key exists.

```python
config.getKeys() -> List[str]
```
Get all configuration keys.

```python
config.clear() -> None
```
Clear all configuration entries.

**Value Access**
```python
config.set(key: str, value: Any) -> None
```
Set a configuration value. Supports all primitive types and vectors.

```python
config.get(key: str) -> Any
```
Get a configuration value.

```python
config.getOr(key: str, default: Any) -> Any
```
Get configuration value with default fallback.

**Summary**
```python
summary = config.summary() -> str
```
Get a summary string of the configuration.

### AgentBrain Class

#### Purpose
Agent brain interface connecting NLM brain to the world. Handles sensory transduction and motor decoding.

#### Methods

**Constructor**
```python
agent = AgentBrain(brain: Brain) -> AgentBrain
```
Create agent brain interface.

**Initialization**
```python
agent.initialize(world: SimpleWorld) -> None
```
Initialize with world.

**Sensory Processing**
```python
agent.processSensoryInput(percept: SensoryPercept) -> None
```
Process sensory percept and inject into brain.

**Motor Processing**
```python
motorCmd = agent.decodeMotorCommand() -> MotorCommand
```
Decode brain motor activity into motor command.

**Neuromodulation**
```python
agent.applyRewardModulation(reward: float, predictedReward: float) -> None
```
Apply reward-based neuromodulation.

**Development**
```python
agent.updateDevelopment(timestep: double) -> None
```
Update development system.

**Statistics**
```python
stage = agent.getDevelopmentalStage() -> DevelopmentalStage
```
Get current developmental stage.

```python
neuromodLevel = agent.getNeuromodulationLevel() -> float
```
Get current neuromodulation level.

```python
curiosityLevel = agent.getCuriosityLevel() -> float
```
Get curiosity level.

```python
noveltyLevel = agent.getNoveltyLevel() -> float
```
Get novelty level.

```python
predError = agent.getPredictionError() -> float
```
Get prediction error.

**Configuration**
```python
agent.enableRewardModulation(enable: bool) -> None
```
Enable/disable reward modulation.

```python
agent.enableStructuralPlasticity(enable: bool) -> None
```
Enable/disable structural plasticity.

```python
agent.enableDevelopment(enable: bool) -> None
```
Enable/disable development.

```python
agent.enableCuriosity(enable: bool) -> None
```
Enable/disable curiosity.

**State**
```python
agent.reset() -> None
```
Reset agent for new episode.

```python
agent.getBrain() -> Brain
```
Get the underlying brain.

### SimpleWorld Class

#### Purpose
Simple 2D world for NLM simulation. Provides environment interaction capabilities.

#### Methods

**Constructor**
```python
world = SimpleWorld() -> SimpleWorld
```
Create a new simple world.

**Configuration**
```python
world.configure(width: int, height: int, visionWidth: int, visionHeight: int) -> None
```
Configure world dimensions.

**Control**
```python
world.reset() -> None
```
Reset world to initial state.

```python
world.update(timestep: float) -> None
```
Update world state.

```python
world.setRandomSeed(seed: int) -> None
```
Set random seed for reproducible simulations.

**Agent Interaction**
```python
world.setAgentStart(x: float, y: float) -> None
```
Set agent starting position.

```python
world.applyMotorCommand(cmd: MotorCommand, currentTime: float) -> None
```
Apply motor command to world.

**State Access**
```python
percept = world.getSensoryPercept() -> SensoryPercept
```
Get sensory percept from world.

```python
body = world.getAgentBody() -> AgentBody
```
Get agent body state.

**World Objects**
```python
world.addObject(obj: WorldObject) -> None
```
Add object to world.

```python
world.removeObject(x: float, y: float) -> None
```
Remove object at position.

```python
world.isValidPosition(x: float, y: float) -> bool
```
Check if position is valid.

**Properties**
```python
width = world.getWidth() -> int
```
Get world width.

```python
height = world.getHeight() -> int
```
Get world height.

```python
time = world.getSimulationTime() -> float
```
Get simulation time.

```python
energy = world.getMaxEnergy() -> float
```
Get maximum energy.

```python
world.setMaxEnergy(energy: float) -> None
```
Set maximum energy.

### SensoryInput Classes

#### Vision
```python
vision = Vision(width: int, height: int, channels: int = 3) -> Vision
```
Create vision sensory input.

```python
vision.setData(data: List[float]) -> None
```
Set vision data.

```python
vision.getWidth() -> int
```
Get vision width.

```python
vision.getHeight() -> int
```
Get vision height.

```python
vision.getChannels() -> int
```
Get number of channels.

#### Audio
```python
audio = Audio(sampleRate: int, numSamples: int) -> Audio
```
Create audio sensory input.

```python
audio.setData(data: List[float]) -> None
```
Set audio data.

```python
audio.setSampleRate(sampleRate: int) -> None
```
Set sample rate.

#### InternalSignals
```python
internal = InternalSignals() -> InternalSignals
```
Create internal signals sensory input.

```python
internal.addSignal(value: float) -> None
```
Add internal signal.

```python
internal.clearSignals() -> None
```
Clear all signals.

### Action Classes

#### Constructor
```python
action = Action() -> Action
```
Create empty action.

```python
action = Action(actionType: ActionType) -> Action
```
Create action with type.

```python
action = Action(actionType: ActionType, parameters: List[float]) -> Action
```
Create action with type and parameters.

#### Properties
```python
action.getType() -> ActionType
```
Get action type.

```python
action.setType(actionType: ActionType) -> None
```
Set action type.

```python
action.getParameters() -> List[float]
```
Get action parameters.

```python
action.setParameters(params: List[float]) -> None
```
Set action parameters.

```python
action.getName() -> str
```
Get action name.

```python
action.clone() -> Action
```
Create a copy of the action.

### WorldObject Class

#### Constructor
```python
obj = WorldObject(x: float, y: float, objType: WorldObjectType, value: float = 0.0, radius: float = 0.5) -> WorldObject
```
Create world object.

#### Properties
```python
obj.x -> float
```
X coordinate.

```python
obj.y -> float
```
Y coordinate.

```python
obj.radius -> float
```
Radius.

```python
obj.type -> WorldObjectType
```
Object type.

```python
obj.value -> float
```
Object value.

```python
obj.active -> bool
```
Active state.

### AgentBody Class

#### Properties
```python
body.x -> float
```
X coordinate.

```python
body.y -> float
```
Y coordinate.

```python
body.orientation -> float
```
Orientation in radians.

```python
body.velocityX -> float
```
X velocity.

```python
body.velocityY -> float
```
Y velocity.

```python
body.angularVelocity -> float
```
Angular velocity.

```python
body.energy -> float
```
Energy level.

```python
body.health -> float
```
Health level.

```python
body.age -> float
```
Age in simulation time.

```python
body.isMoving -> bool
```
Moving state.

```python
body.isTurning -> bool
```
Turning state.

```python
body.lastActionTime -> float
```
Time of last action.

#### Methods
```python
body.reset() -> None
```
Reset body state.

### SensoryPercept Class

#### Vision Access
```python
vision = percept.getVision() -> Vision
```
Get vision data.

```python
percept.setVision(vision: Vision) -> None
```
Set vision data.

#### Other Sensory Data
```python
touch = percept.getTouch() -> List[float]
```
Get touch data.

```python
internal = percept.getInternal() -> List[float]
```
Get internal signals.

```python
proprioception = percept.getProprioception() -> List[float]
```
Get proprioception data.

```python
audio = percept.getAudio() -> Audio
```
Get audio data.

#### Utility
```python
signals = percept.getAllSignals() -> List[float]
```
Get all sensory signals.

```python
timestamp = percept.getTimestamp() -> float
```
Get percept timestamp.

```python
percept.setTimestamp(timestamp: float) -> None
```
Set percept timestamp.

### Enumeration Types

#### NeuronType
- `Excitatory`
- `Inhibitory`
- `Modulatory`
- `Sensory`
- `Motor`
- `Internal`

#### SynapseType
- `Excitatory`
- `Inhibitory`
- `Modulatory`
- `Electrical`
- `GapJunction`

#### DevelopmentalStage
- `Initial`
- `CriticalPeriod`
- `Maturation`
- `Adult`
- `Aging`

#### FiringState
- `Resting`
- `Active`
- `Refractory`
- `Inhibited`

#### ActionType
- `MoveForward`
- `MoveBackward`
- `MoveLeft`
- `MoveRight`
- `TurnLeft`
- `TurnRight`
- `Look`
- `LookUp`
- `LookDown`
- `Interact`
- `Eat`
- `Drink`
- `Rest`
- `Wait`
- `Custom`

#### MotorCommand
- `MoveForward`
- `MoveBackward`
- `TurnLeft`
- `TurnRight`
- `LookLeft`
- `LookRight`
- `Interact`
- `Wait`

#### WorldObjectType
- `Empty`
- `Resource`
- `Hazard`
- `Wall`
- `Marker`

### Global Constants

```python
INVALID_NEURON_ID = NeuronId()
```
Invalid neuron ID.

```python
INVALID_SYNAPSE_ID = SynapseId()
```
Invalid synapse ID.

```python
INVALID_REGION_ID = RegionId()
```
Invalid region ID.

```python
INVALID_POPULATION_ID = PopulationId()
```
Invalid population ID.

### Factory Functions

```python
config = createDefaultConfig() -> Config
```
Create a default configuration.

```python
brain = createBrain(config: Config) -> Brain
```
Create a new brain with configuration.

```python
world = createSimpleWorld() -> SimpleWorld
```
Create a new simple world.

```python
agent = createAgentBrain(brain: Brain) -> AgentBrain
```
Create a new agent brain interface.

## Type Definitions

### Core Types
- `NeuronId`: Unique identifier for a neuron
- `SynapseId`: Unique identifier for a synapse
- `RegionId`: Unique identifier for a brain region
- `PopulationId`: Unique identifier for a neuron population

### Value Types
- `MembranePotential`: Neuron membrane potential (float)
- `SimulationStep`: Simulation step count (size_t)
- `Timestamp`: Simulation time (float)
- `Delay`: Synaptic delay (int)

### Configuration Value Types
Configuration values can be:
- Primitive: `int`, `int64_t`, `double`, `bool`, `std::string`
- Arrays: `std::vector<int>`, `std::vector<double>`, `std::vector<std::string>`

## Example Usage

See `EXAMPLES.md` for comprehensive examples of using the NLM Python API.

## Related Documentation

- `HOW_TO_USE.md`: Basic usage guide
- `easy_usage.md`: Beginner-friendly guide
- `docs/ARCHITECTURE.md`: System architecture
- `docs/SCIENCE.md`: Scientific background
