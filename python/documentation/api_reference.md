#!/usr/bin/env python3
"""
API Reference - NLM Python Bindings

This is the comprehensive API reference for the enhanced NLM Python bindings.
It documents all public classes, methods, enumerations, and functions available.

## Overview

The NLM Python bindings provide complete access to the NLM (熙然) neural simulation framework,
including:
- Neural brain core with spiking neurons
- Agent brain interface for world interaction
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Developmental stages and plasticity
- Prediction and learning systems
- Episode management
- Serialization and checkpointing
- Performance profiling
- Error handling and validation

## Module Structure

### Main Classes
- `Brain` - Central neural simulation brain class
- `AgentBrain` - Agent brain interface connecting NLM to world
- `Config` - Configuration management
- `SensoryPercept` - Sensory percept data container
- `SimpleWorld` - Simple 2D world for NLM simulation
- `NeuronId`, `SynapseId`, `RegionId`, `PopulationId` - Unique identifiers

### Enumerations
- `NeuronType` - Types of neurons
- `SynapseType` - Types of synapses
- `DevelopmentalStage` - Developmental stages
- `FiringState` - Neuron firing states
- `ActionType` - Action types
- `MotorCommand` - Low-level motor commands
- `WorldObjectType` - Types of world objects

### Utility Classes and Functions
- `NLMError`, `ValidationError`, `SerializationError`, `CallbackError` - Exception classes
- `createDefaultConfig()` - Create default configuration
- `createBrain(config)` - Create new brain with configuration
- `createSimpleWorld()` - Create new simple world
- `createAgentBrain(brain)` - Create new agent brain interface

## Detailed API Documentation

### Brain Class

The `Brain` class is the central neural simulation component, containing neurons, synapses,
regions, and managing the complete neural network.

#### Initialization
```python
brain = nlm.createBrain(config)
```

#### Core Methods

##### Initialization and Configuration

**`initialize()`** - Initialize the brain with configuration
```python
brain.initialize()
```

**`step(currentStep)`** - Perform a simulation step
```python
brain.step(currentStep)
```

**`step(currentStep, currentTime)`** - Perform a simulation step with timestamp
```python
brain.step(currentStep, currentTime)
```

**`logStatus()`** - Log brain status
```python
brain.logStatus()
```

##### Neural Processing

**`receiveSensoryInput(input)`** - Inject sensory input into the brain
```python
brain.receiveSensoryInput(percept)
```

**`injectCurrent(neuron, current)`** - Inject current into a specific neuron
```python
brain.injectCurrent(neuron_id, 0.5)
```

**`injectCurrentToNeurons(type, current)`** - Inject current into all neurons of a specific type
```python
brain.injectCurrentToNeurons(nlm.NeuronType.Excitatory, 1.0)
```

**`produceAction()`** - Produce motor action based on neural activity
```python
action = brain.produceAction()
```

##### State Management

**`reset()`** - Reset brain state
```python
brain.reset()
```

**`save(filepath)`** - Save brain state to file
```python
brain.save("brain_state.json")
```

**`load(filepath)`** - Load brain state from file
```python
brain.load("brain_state.json")
```

**`addRegion(name="")`** - Add a new neural region
```python
region_id = brain.addRegion("visual_cortex")
```

**`getRegion(id)`** - Get a region by ID
```python
region = brain.getRegion(region_id)
```

**`getRegionCount()`** - Get the number of regions
```python
count = brain.getRegionCount()
```

**`getRegionIds()`** - Get all region IDs
```python
region_ids = brain.getRegionIds()
```

**`getRegions()`** - Get all regions
```python
regions = brain.getRegions()
```

##### Statistics and Metrics

**`getTotalNeuronCount()`** - Get total neuron count across all regions
```python
neuron_count = brain.getTotalNeuronCount()
```

**`getTotalSynapseCount()`** - Get total synapse count across all regions
```python
synapse_count = brain.getTotalSynapseCount()
```

**`getActiveNeuronCount()`** - Get count of active neurons
```python
active_count = brain.getActiveNeuronCount()
```

**`getFiringNeuronCount()`** - Get count of currently firing neurons
```python
firing_count = brain.getFiringNeuronCount()
```

**`getAverageFiringRate()`** - Get average firing rate across all neurons
```python
avg_firing_rate = brain.getAverageFiringRate()
```

**`getExcitationInhibitionRatio()`** - Get excitation/inhibition balance ratio
```python
ei_ratio = brain.getExcitationInhibitionRatio()
```

**`getTotalSpikeCount()`** - Get total spike count
```python
total_spikes = brain.getTotalSpikeCount()
```

**`getDevelopmentalStage()`** - Get current developmental stage
```python
stage = brain.getDevelopmentalStage()
```

**`setDevelopmentalStage(stage)`** - Set developmental stage
```python
brain.setDevelopmentalStage(nlm.DevelopmentalStage.Adult)
```

##### Configuration Access

**`getConfig()`** - Get the configuration
```python
config = brain.getConfig()
```

##### Advanced Neural Operations

**`getRegion(neuron_id)`** - Get region containing a specific neuron
```python
region = brain.getRegion(neuron_id)
```

**`getNeuronsByType(neurons, type_filter)`** - Get neurons by type
```python
neurons = brain.getNeuronsByType(neuron_ids, nlm.NeuronType.Excitatory)
```

**`getSynapsesByType(synapses, type_filter)`** - Get synapses by type
```python
synapses = brain.getSynapsesByType(synapse_ids, nlm.SynapseType.Excitatory)
```

**`getActivityByRegion(region_id)`** - Get neural activity by region
```python
activity = brain.getActivityByRegion(region_id)
```

### AgentBrain Class

The `AgentBrain` class provides the interface between the neural brain and the external world,
handling sensory input processing, motor command decoding, and episode management.

#### Initialization

**`initialize(world)`** - Initialize with world
```python
agent.initialize(world)
```

#### Core Methods

##### Sensory and Motor Processing

**`getSensoryInputSize()`** - Get expected sensory input size
```python
size = agent.getSensoryInputSize()
```

**`getMotorOutputSize()`** - Get expected motor output size
```python
size = agent.getMotorOutputSize()
```

**`processSensoryInput(percept, computeNovelty)`** - Process sensory input with novelty computation
```python
agent.processSensoryInput(percept, True)
```

**`decodeMotorCommand()`** - Decode brain motor activity into motor command
```python
command = agent.decodeMotorCommand()
```

##### Neuromodulation

**`applyRewardModulation(reward, predictedReward)`** - Apply reward-based neuromodulation
```python
agent.applyRewardModulation(1.0, 0.5)
```

**`getNeuromodulationLevel()`** - Get current neuromodulation level
```python
level = agent.getNeuromodulationLevel()
```

**`getCuriosityLevel()`** - Get curiosity level
```python
curiosity = agent.getCuriosityLevel()
```

**`getNoveltyLevel()`** - Get novelty level
```python
novelty = agent.getNoveltyLevel()
```

**`getPredictionError()`** - Get prediction error
```python
error = agent.getPredictionError()
```

##### Development

**`updateDevelopment(timestep)`** - Update development system
```python
agent.updateDevelopment(1.0)
```

**`getDevelopmentalStage()`** - Get current developmental stage
```python
stage = agent.getDevelopmentStage()
```

##### Episode Management

**`reset()`** - Reset agent for new episode
```python
agent.reset()
```

**`isEpisodeActive()`** - Check if episode is active
```python
active = agent.isEpisodeActive()
```

**`startEpisode(episodeId=0, episodeType="default")`** - Start a new episode
```python
agent.startEpisode(2, "learning")
```

**`endEpisode(finalReward)`** - End current episode with final reward
```python
agent.endEpisode(5.0)
```

**`forceReset()`** - Force reset (equivalent to reset but for episodes)
```python
agent.forceReset()
```

**`getCurrentEpisode()`** - Get current episode ID
```python
episode_id = agent.getCurrentEpisode()
```

**`getEpisodeType()`** - Get episode type
```python
episode_type = agent.getEpisodeType()
```

**`getEpisodeScore()`** - Get current episode score
```python
score = agent.getEpisodeScore()
```

**`recordEpisodeAction(action, reward)`** - Record action and reward for episode
```python
agent.recordEpisodeAction(nlm.MotorCommand.MoveForward, 2.0)
```

##### Learning Control

**`enableLearning(enable)`** - Enable or disable learning
```python
agent.enableLearning(True)
```

**`setLearningRate(rate)`** - Set learning rate
```python
agent.setLearningRate(0.01)
```

**`setExplorationRate(rate)`** - Set exploration rate
```python
agent.setExplorationRate(0.1)
```

**`setRewardShaping(shapingFactor)`** - Set reward shaping factor
```python
agent.setRewardShaping(0.8)
```

**`setCuriosityExploration(exploration)`** - Set curiosity-based exploration level
```python
agent.setCuriosityExploration(0.3)
```

**`triggerLearningEpoch()`** - Trigger learning epoch
```python
agent.triggerLearningEpoch()
```

**`triggerCriticalPeriod()`** - Trigger critical period development
```python
agent.triggerCriticalPeriod()
```

**`triggerAdultStage()`** - Trigger adult development stage
```python
agent.triggerAdultStage()
```

##### Batch Processing

**`processPercepts(percepts)`** - Process multiple sensory percepts
```python
agent.processPercepts([percept1, percept2, percept3])
```

**`decodeCommands(steps)`** - Decode motor commands for multiple steps
```python
commands = agent.decodeCommands(10)
```

**`stepMultiple(percepts, rewards, predictedRewards, maxSteps=100)`** - Process multiple steps with batch processing
```python
agent.stepMultiple(percepts, rewards, predictedRewards, maxSteps=50)
```

##### State Checkpoint/Save/Load Utilities

**`saveState(filepath)`** - Save brain state to file
```python
agent.saveState("agent_state.json")
```

**`loadState(filepath)`** - Load brain state from file
```python
agent.loadState("agent_state.json")
```

**`setCheckpoint(threshold=0.5)`** - Set a checkpoint with given threshold
```python
agent.setCheckpoint(0.7)
```

**`loadCheckpoint(name)`** - Load checkpoint by name
```python
agent.loadCheckpoint("episode_10")
```

**`getAvailableCheckpoints()`** - Get list of available checkpoints
```python
checkpoints = agent.getAvailableCheckpoints()
```

##### Simulation Statistics and Metrics Collection

**`collectMetrics()`** - Collect simulation metrics
```python
agent.collectMetrics()
```

**`resetMetrics()`** - Reset metrics collection
```python
agent.resetMetrics()
```

**`getMetricsSummary()`** - Get metrics summary as string
```python
summary = agent.getMetricsSummary()
print(summary)
```

##### Reward Shaping and Penalty Systems

**`applyRewardShaping(baseReward, shapingFactor=1.0)`** - Apply reward shaping
```python
agent.applyRewardShaping(1.0, 0.8)
```

**`addPenalty(penalty, reason="")`** - Add a penalty with optional reason
```python
agent.addPenalty(-0.5, "illegal_action")
```

**`clearPenalties()`** - Clear all penalties
```python
agent.clearPenalties()
```

**`getPenalties()`** - Get list of penalties
```python
penalties = agent.getPenalties()
```

**`getTotalPenalty()`** - Get total penalty value
```python
total_penalty = agent.getTotalPenalty()
```

##### Memory Introspection and Debugging Tools

**`introspectMemory()`** - Get memory statistics and introspection data
```python
memory_data = agent.introspectMemory()
```

**`getNeuronActivityProfile()`** - Get neuron activity profile
```python
activity_profile = agent.getNeuronActivityProfile()
```

**`getActiveNeurons()`** - Get list of active neurons with activity levels
```python
active_neurons = agent.getActiveNeurons()
```

**`getSynapticStrengths()`** - Get synaptic strength measurements
```python
synaptic_strengths = agent.getSynapticStrengths()
```

##### Performance Profiling Utilities

**`profilePerformance()`** - Get performance statistics
```python
metrics = agent.profilePerformance()
```

**`getProcessingEfficiency()`** - Get processing efficiency ratio
```python
efficiency = agent.getProcessingEfficiency()
```

**`getFPS()`** - Get frames per second (steps per second)
```python
fps = agent.getFPS()
```

**`getMemoryUsage()`** - Get current memory usage in bytes
```python
memory_usage = agent.getMemoryUsage()
```

**`getSessionUptime()`** - Get session uptime in seconds
```python
uptime = agent.getSessionUptime()
```

##### Error Handling and Validation

**`validateSensoryPercept(percept)`** - Validate sensory percept input
```python
is_valid = agent.validateSensoryPercept(percept)
```

**`validateRewardValue(reward)`** - Validate reward value range
```python
is_valid = agent.validateRewardValue(0.5)
```

**`validateMotorCommand(cmd)`** - Validate motor command
```python
is_valid = agent.validateMotorCommand(nlm.MotorCommand.MoveForward)
```

**`validateWorldState(world)`** - Validate world state
```python
is_valid = agent.validateWorldState(world)
```

**`validateBrainState()`** - Validate brain state consistency
```python
is_valid = agent.validateBrainState()
```

**`validateNeuromodulationState()`** - Validate neuromodulation state
```python
is_valid = agent.validateNeuromodulationState()
```

**`validateDevelopmentState()`** - Validate development state
```python
is_valid = agent.validateDevelopmentState()
```

**`validateMemoryState()`** - Validate memory state
```python
is_valid = agent.validateMemoryState()
```

##### Python-Specific Features

**`perceptToPythonVector(percept)`** - Convert percept to Python vector for interop
```python
vector = agent.perceptToPythonVector(percept)
```

**`pythonVectorToPercept(data)`** - Convert Python vector to percept
```python
percept = agent.pythonVectorToPercept([0.1, 0.2, 0.3])
```

**`getMotorCommandFromIndex(index)`** - Get motor command from index
```python
command = agent.getMotorCommandFromIndex(0)
```

**`getMotorCommandIndex(cmd)`** - Get motor command index
```python
index = agent.getMotorCommandIndex(nlm.MotorCommand.MoveForward)
```

**`pythonListToMotorCommands(indices)`** - Convert Python list of indices to motor commands
```python
commands = agent.pythonListToMotorCommands([0, 1, 2])
```

**`motorCommandsToPythonList(commands)`** - Convert motor commands to Python list of indices
```python
indices = agent.motorCommandsToPythonList([nlm.MotorCommand.MoveForward, nlm.MotorCommand.TurnLeft])
```

##### Serialization Methods for Python Objects

**`serializeToJSON()`** - Serialize agent brain state to JSON
```python
json_str = agent.serializeToJSON()
```

**`serializeToMsgPack()`** - Serialize agent brain state to MessagePack
```python
msgpack_str = agent.serializeToMsgPack()
```

**`deserializeFromJSON(jsonStr)`** - Deserialize agent brain state from JSON
```python
agent.deserializeFromJSON(json_str)
```

**`deserializeFromMsgPack(msgpackStr)`** - Deserialize agent brain state from MessagePack
```python
agent.deserializeFromMsgPack(msgpack_str)
```

##### Callback Mechanism Support

**`addRewardCallback(callback)`** - Add reward callback function
```python
agent.addRewardCallback(lambda reward: print(f"Reward: {reward}"))
```

**`addActionCallback(callback)`** - Add action callback function
```python
agent.addActionCallback(lambda action: print(f"Action: {action}"))
```

**`addLearningCallback(callback)`** - Add learning callback function
```python
agent.addLearningCallback(lambda epoch: print(f"Learning epoch: {epoch}"))
```

**`removeRewardCallback(id)`** - Remove reward callback by ID
```python
agent.removeRewardCallback(callback_id)
```

**`removeActionCallback(id)`** - Remove action callback by ID
```python
agent.removeActionCallback(callback_id)
```

**`removeLearningCallback(id)`** - Remove learning callback by ID
```python
agent.removeLearningCallback(callback_id)
```

**`clearAllCallbacks()`** - Clear all callbacks
```python
agent.clearAllCallbacks()
```

##### Debug and Introspection Utilities

**`getStateSummary()`** - Get state summary as string
```python
summary = agent.getStateSummary()
```

**`debugInfo()`** - Get debug information
```python
debug_info = agent.debugInfo()
```

**`enableDebugMode(enable)`** - Enable or disable debug mode
```python
agent.enableDebugMode(True)
```

**`isDebugModeEnabled()`** - Check if debug mode is enabled
```python
debug_enabled = agent.isDebugModeEnabled()
```

**`logMetrics()`** - Log metrics to output
```python
agent.logMetrics()
```

##### Utility Methods for Python

**`normalizeReward(reward, min=-1.0, max=1.0)`** - Normalize reward to range [min, max]
```python
normalized = agent.normalizeReward(0.5, -1.0, 1.0)
```

**`denormalizeReward(normalizedReward, min=-1.0, max=1.0)`** - Denormalize reward from range [min, max]
```python
denormalized = agent.denormalizeReward(0.5, -1.0, 1.0)
```

**`selectActionWithProbability(actions, probabilities)`** - Select action based on probabilities
```python
action = agent.selectActionWithProbability(
    ['move_forward', 'turn_left', 'turn_right'],
    [0.7, 0.2, 0.1]
)
```

**`computeDistance(vec1, vec2)`** - Compute distance between two vectors
```python
distance = agent.computeDistance([0.1, 0.2, 0.3], [0.4, 0.5, 0.6])
```

##### Existing Methods (for compatibility)

**`applyAdvancedModulation(reward, predictedReward, shapingFactor=1.0)`** - Apply advanced reward modulation
```python
agent.applyAdvancedModulation(1.0, 0.5, 0.8)
```

**`updateDevelopmentalStage(timestep)`** - Update developmental stage
```python
agent.updateDevelopmentalStage(1.0)
```

### Config Class

The `Config` class manages NLM configuration settings.

#### Methods

**`loadFromFile(filepath)`** - Load configuration from a JSON file
```python
config.loadFromFile("config.json")
```

**`loadFromArgs(argc, argv)`** - Load configuration from command line arguments
```python
config.loadFromArgs(len(sys.argv), sys.argv)
```

**`saveToFile(filepath)`** - Save configuration to a JSON file
```python
config.saveToFile("config.json")
```

**`has(key)`** - Check if a configuration key exists
```python
has_key = config.has("neural_learning_rate")
```

**`getKeys()`** - Get all configuration keys
```python
keys = config.getKeys()
```

**`clear()`** - Clear all configuration entries
```python
config.clear()
```

**`summary()`** - Get a summary string of the configuration
```python
summary = config.summary()
```

### SensoryInput and Subclasses

#### SensoryInput (Base Class)

**`getType()`** - Get the type of sensory input
```python
input_type = sensory_input.getType()
```

**`getData()`** - Get the raw data as a vector
```python
data = sensory_input.getData()
```

**`getDimensions()`** - Get the dimensionality
```python
dimensions = sensory_input.getDimensions()
```

**`getTimestamp()`** - Get the timestamp
```python
timestamp = sensory_input.getTimestamp()
```

**`setTimestamp(timestamp)`** - Set the timestamp
```python
sensory_input.setTimestamp(time.time())
```

#### Vision

**`Vision(width, height, channels=3)`** - Create vision sensory input
```python
vision = nlm.Vision(64, 64, 3)
```

**`setData(data)`** - Set vision data
```python
vision.setData([0.1] * 64 * 64 * 3)
```

**`getWidth()`** - Get vision width
```python
width = vision.getWidth()
```

**`getHeight()`** - Get vision height
```python
height = vision.getHeight()
```

**`getChannels()`** - Get number of channels
```python
channels = vision.getChannels()
```

#### Audio

**`Audio(sampleRate, numSamples)`** - Create audio sensory input
```python
audio = nlm.Audio(44100, 1024)
```

**`setSampleRate(sampleRate)`** - Set sample rate
```python
audio.setSampleRate(48000)
```

**`getSampleRate()`** - Get sample rate
```python
sample_rate = audio.getSampleRate()
```

**`getNumSamples()`** - Get number of samples
```python
num_samples = audio.getNumSamples()
```

#### InternalSignals

**`InternalSignals()`** - Create internal signals sensory input
```python
internal = nlm.InternalSignals()
```

**`addSignal(value)`** - Add internal signal
```python
internal.addSignal(0.5)
```

**`clearSignals()`** - Clear all signals
```python
internal.clearSignals()
```

### Action and Subclasses

#### Action

**`Action(type, parameters=None)`** - Create action
```python
action = nlm.Action(nlm.ActionType.MoveForward, [0.1, 0.2])
```

**`getType()`** - Get action type
```python
action_type = action.getType()
```

**`setType(type)`** - Set action type
```python
action.setType(nlm.ActionType.MoveBackward)
```

**`getParameters()`** - Get action parameters
```python
parameters = action.getParameters()
```

**`setParameters(params)`** - Set action parameters
```python
action.setParameters([0.1, 0.2])
```

**`getName()`** - Get action name
```python
name = action.getName()
```

**`clone()`** - Clone action
```python
cloned_action = action.clone()
```

### WorldObject

**`WorldObject(x, y, type, value=0.0, radius=0.5)`** - Create world object
```python
obj = nlm.WorldObject(10.0, 20.0, nlm.WorldObjectType.Resource, 1.0, 1.0)
```

**`x`** - X coordinate (read/write)
```python
obj.x = 15.0
```

**`y`** - Y coordinate (read/write)
```python
obj.y = 25.0
```

**`type`** - World object type (read/write)
```python
obj.type = nlm.WorldObjectType.Wall
```

**`radius`** - Radius (read/write)
```python
obj.radius = 2.0
```

**`value`** - Value (read/write)
```python
obj.value = 5.0
```

**`active`** - Active status (read/write)
```python
obj.active = True
```

### AgentBody

**`AgentBody()`** - Create agent body state
```python
body = nlm.AgentBody()
```

**`x`** - X position (read/write)
**`y`** - Y position (read/write)
**`orientation`** - Orientation (read/write)
**`velocityX`** - X velocity (read/write)
**`velocityY`** - Y velocity (read/write)
**`angularVelocity`** - Angular velocity (read/write)
**`energy`** - Energy (read/write)
**`health`** - Health (read/write)
**`age`** - Age (read/write)
**`isMoving`** - Moving flag (read/write)
**`isTurning`** - Turning flag (read/write)
**`lastActionTime`** - Last action time (read/write)

**`reset()`** - Reset body
```python
body.reset()
```

### ActionResult

**`ActionResult(reward, success, message="")`** - Create action result
```python
result = nlm.ActionResult(1.0, True, "Moved forward successfully")
```

**`reward`** - Reward value (read/write)
**`success`** - Success flag (read/write)
**`message`** - Message (read/write)

### SensoryPercept

**`SensoryPercept()`** - Create sensory percept
```python
percept = nlm.SensoryPercept()
```

**`setVision(data)`** - Set vision data
**`getVision()`** - Get vision data
**`getVisionWidth()`** - Get vision width
**`getVisionHeight()`** - Get vision height

**`setTouch(data)`** - Set touch data
**`getTouch()`** - Get touch data

**`setInternal(data)`** - Set internal data
**`getInternal()`** - Get internal data

**`setProprioception(data)`** - Set proprioception data
**`getProprioception()`** - Get proprioception data

**`setAudio(data)`** - Set audio data
**`getAudio()`** - Get audio data

**`getAllSignals()`** - Get all signals as vector
**`getTimestamp()`** - Get timestamp
**`setTimestamp(timestamp)`** - Set timestamp

### SimpleWorld

**`SimpleWorld()`** - Create simple world
```python
world = nlm.createSimpleWorld()
```

**`configure(width, height, visionWidth, visionHeight)`** - Configure world
```python
world.configure(100, 100, 32, 32)
```

**`reset()`** - Reset world
**`setAgentStart(x, y)`** - Set agent start position
**`update(timestep)`** - Update world
**`applyMotorCommand(cmd, currentTime)`** - Apply motor command
**`getSensoryPercept()`** - Get sensory percept
**`getAgentBody()`** - Get agent body
**`addObject(obj)`** - Add world object
**`removeObject(x, y)`** - Remove world object
**`isValidPosition(x, y)`** - Check if position is valid
**`getWidth()`** - Get world width
**`getHeight()`** - Get world height
**`getMaxEnergy()`** - Get maximum energy
**`setMaxEnergy(e)`** - Set maximum energy
**`getEnergyDecayRate()`** - Get energy decay rate
**`setEnergyDecayRate(r)`** - Set energy decay rate
**`getSimulationTime()`** - Get simulation time
**`setRandomSeed(seed)`** - Set random seed
**`getRandomSeed()`** - Get random seed

### Utility Functions

**`createDefaultConfig()`** - Create a default configuration
```python
config = nlm.createDefaultConfig()
```

**`createBrain(config)`** - Create a new brain with configuration
```python
brain = nlm.createBrain(config)
```

**`createSimpleWorld()`** - Create a new simple world
```python
world = nlm.createSimpleWorld()
```

**`createAgentBrain(brain)`** - Create a new agent brain interface
```python
agent = nlm.createAgentBrain(brain)
```

### Enumeration Values

#### NeuronType
- `NeuronType.Excitatory` - Excitatory neurons
- `NeuronType.Inhibitory` - Inhibitory neurons
- `NeuronType.Modulatory` - Modulatory neurons
- `NeuronType.Sensory` - Sensory neurons
- `NeuronType.Motor` - Motor neurons
- `NeuronType.Internal` - Internal neurons

#### SynapseType
- `SynapseType.Excitatory` - Excitatory synapses
- `SynapseType.Inhibitory` - Inhibitory synapses
- `SynapseType.Modulatory` - Modulatory synapses
- `SynapseType.Electrical` - Electrical synapses
- `SynapseType.GapJunction` - Gap junctions

#### DevelopmentalStage
- `DevelopmentalStage.Initial` - Initial stage
- `DevelopmentalStage.CriticalPeriod` - Critical period
- `DevelopmentalStage.Maturation` - Maturation stage
- `DevelopmentalStage.Adult` - Adult stage
- `DevelopmentalStage.Aging` - Aging stage

#### FiringState
- `FiringState.Resting` - Resting state
- `FiringState.Active` - Active state
- `FiringState.Refractory` - Refractory state
- `FiringState.Inhibited` - Inhibited state

#### ActionType
- `ActionType.MoveForward` - Move forward
- `ActionType.MoveBackward` - Move backward
- `ActionType.MoveLeft` - Move left
- `ActionType.MoveRight` - Move right
- `ActionType.TurnLeft` - Turn left
- `ActionType.TurnRight` - Turn right
- `ActionType.Look` - Look
- `ActionType.LookUp` - Look up
- `ActionType.LookDown` - Look down
- `ActionType.Interact` - Interact
- `ActionType.Eat` - Eat
- `ActionType.Drink` - Drink
- `ActionType.Rest` - Rest
- `ActionType.Wait` - Wait
- `ActionType.Custom` - Custom action

#### MotorCommand
- `MotorCommand.MoveForward` - Move forward
- `MotorCommand.MoveBackward` - Move backward
- `MotorCommand.TurnLeft` - Turn left
- `MotorCommand.TurnRight` - Turn right
- `MotorCommand.LookLeft` - Look left
- `MotorCommand.LookRight` - Look right
- `MotorCommand.Interact` - Interact
- `MotorCommand.Wait` - Wait

#### WorldObjectType
- `WorldObjectType.Empty` - Empty space
- `WorldObjectType.Resource` - Resource
- `WorldObjectType.Hazard` - Hazard
- `WorldObjectType.Wall` - Wall
- `WorldObjectType.Marker` - Marker

### Exception Classes

#### NLMError
Base exception for all NLM errors.

#### ValidationError
Exception for input validation failures.

#### SerializationError
Exception for file I/O and serialization issues.

#### CallbackError
Exception for callback mechanism errors.

### Type Aliases

#### NeuronId
Unique identifier for a neuron.

#### SynapseId
Unique identifier for a synapse.

#### RegionId
Unique identifier for a brain region.

#### PopulationId
Unique identifier for a neuron population.

## Usage Examples

### Basic Neural Network
```python
import nlm

# Create brain and agent
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
agent = nlm.createAgentBrain(brain)

# Initialize
world = nlm.createSimpleWorld()
world.configure(100, 100, 32, 32)
agent.initialize(world)

# Create percept
percept = nlm.SensoryPercept()
percept.setVision([0.1] * 1024)
percept.setTouch([0.1, 0.1])

# Process and act
agent.processSensoryInput(percept, True)
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, time.time())
```

### Episode Learning
```python
# Run learning episode
agent.startEpisode(0, "learning")

for step in range(100):
    # Create percept
    percept = create_dynamic_percept(step)
    
    # Process with novelty computation
    agent.processSensoryInput(percept, True)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply to world
    result = world.applyMotorCommand(action, time.time())
    
    # Record with reward
    agent.recordEpisodeAction(action, result.reward)
    
    # Update development
    agent.updateDevelopment(1.0)
    
    # End if conditions met
    if agent.getEpisodeScore() >= 100.0:
        agent.endEpisode(result.reward)
        break
```

### Error Handling
```python
import nlm

try:
    # Create and initialize agent
    agent.initialize(world)
    
    # Process percept with validation
    if not agent.validateSensoryPercept(percept):
        raise nlm.ValidationError("Invalid sensory percept")
    
    agent.processSensoryInput(percept, True)
    
except nlm.ValidationError as e:
    print(f"Validation error: {e}")
    # Recover or handle error
except nlm.NLMError as e:
    print(f"NLM error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Performance Monitoring
```python
# Get performance metrics
metrics = agent.profilePerformance()

print(f"Processing efficiency: {metrics.processingEfficiency:.3f}")
print(f"Steps per second: {metrics.stepsPerSecond:.1f}")
print(f"Memory usage: {metrics.memoryUsageBytes / (1024*1024):.1f} MB")
print(f"Session uptime: {metrics.sessionDurationSeconds:.1f}s")
```

### Serialization and Checkpointing
```python
# Save checkpoint
agent.setCheckpoint(0.7)
agent.saveState("checkpoint.json")

# Load checkpoint
agent.loadState("checkpoint.json")

# Serialize to JSON
json_data = agent.serializeToJSON()

# Deserialize from MessagePack
agent.deserializeFromMsgPack(msgpack_data)
```

### Batch Processing
```python
# Process batch of percepts
percepts = [create_percept(i) for i in range(100)]
agent.processPercepts(percepts)

# Decode multiple commands
commands = agent.decodeCommands(10)

# Step multiple with batch rewards
rewards = [1.0] * 50
predicted_rewards = [0.8] * 50
agent.stepMultiple(percepts, rewards, predicted_rewards, maxSteps=100)
```

## Configuration

### Default Configuration
The default configuration includes:
- Neural learning rates
- Development parameters
- Neuromodulation settings
- Memory system parameters
- World simulation parameters

### Configuration File Format
Configuration files are JSON format with the following structure:
```json
{
    "neural": {
        "learning_rate": 0.01,
        "plasticity_rate": 0.1,
        "excitation_inhibition_ratio": 1.0
    },
    "development": {
        "critical_period_end": 50,
        "maturation_threshold": 100
    },
    "neuromodulation": {
        "dopamine_influence": 0.8,
        "curiosity_strength": 0.5
    },
    "memory": {
        "working_memory_size": 256,
        "episodic_retention": 100
    },
    "world": {
        "width": 100,
        "height": 100,
        "vision_width": 32,
        "vision_height": 32
    }
}
```

## Performance Considerations

### Memory Optimization
- Use batch processing for multiple steps
- Enable checkpointing for long-running simulations
- Monitor memory usage during execution
- Use vectorized operations where possible

### Computational Efficiency
- Process multiple percepts in batches
- Use numpy arrays for vector operations
- Configure appropriate learning rates
- Enable development system only when needed

### Scaling
- Use parallel processing for large simulations
- Distribute workloads across multiple processes
- Implement efficient caching strategies
- Use appropriate data structures for frequent access

## Integration Examples

### Integration with Machine Learning Frameworks
The NLM Python bindings integrate with:
- **scikit-learn**: Feature extraction, behavior prediction, development tracking
- **TensorFlow**: Neural network integration, advanced modeling
- **PyTorch**: Deep learning, custom architectures

### Integration with Neuroscience Toolkits
- **NetPyNE**: Network modeling and simulation
- **NEURON**: Detailed neuronal modeling
- **Brian2**: Spiking neural network simulation

### Integration with Simulation Environments
- **OpenAI Gym**: Reinforcement learning environments
- **PyBullet**: Physics-based simulation
- **Webots**: Robotics simulation

## Troubleshooting

### Common Issues

**1. Memory errors during simulation**
```python
# Solution: Enable memory optimization and checkpointing
agent.setCheckpoint(0.5)
agent.saveState("checkpoint.json")
```

**2. Performance degradation**
```python
# Solution: Profile and optimize
metrics = agent.profilePerformance()
agent.setLearningRate(0.005)
agent.setExplorationRate(0.05)
```

**3. Serialization errors**
```python
# Solution: Try alternative formats and error handling
try:
    agent.saveState("checkpoint.json")
except nlm.SerializationError as e:
    print(f"Serialization failed: {e}")
    # Fallback to simpler format
    agent.saveState("checkpoint.txt", format="txt")
```

**4. Validation errors**
```python
# Solution: Validate inputs and provide defaults
if not agent.validateSensoryPercept(percept):
    print("Percept invalid, using defaults")
    # Create valid percept
    valid_percept = agent.pythonVectorToPercept([0.5] * 256)
    agent.processSensoryInput(valid_percept, True)
```

### Error Codes and Messages

- **NLMError**: General NLM runtime errors
- **ValidationError**: Input validation failures
- **SerializationError**: File I/O and serialization issues
- **CallbackError**: Callback mechanism errors

## License

MIT License

Copyright (c) 2026 NLM Authors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""