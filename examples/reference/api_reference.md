# API Reference

This document provides a complete reference of the NLM Python API, including all available classes, methods, and their parameters.

## Overview

The NLM Python API provides bindings to the NLM (Neural Learning Machine) C++ neural simulation framework. It exposes all the advanced systems from Phase 6 of the NLM project, including memory systems, neuromodulation, prediction and planning, development, and structural plasticity.

## Core Classes

### Config

**File:** `/workspace/app/python/bindings.cpp:156`

The Config class provides configuration management for the NLM system.

#### Constructor
```python
Config()
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `loadFromFile(filepath)` | `filepath: str` | `bool` | Load configuration from a JSON file |
| `loadFromArgs(argc, argv)` | `argc: int, argv: list[str]` | `bool` | Load configuration from command line arguments |
| `saveToFile(filepath)` | `filepath: str` | `bool` | Save configuration to a JSON file |
| `has(key)` | `key: str` | `bool` | Check if a configuration key exists |
| `getKeys()` | | `list[str]` | Get all configuration keys |
| `clear()` | | | Clear all configuration entries |
| `summary()` | | `str` | Get a summary string of the configuration |

#### Advanced Configuration Management Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `hasValidSchema()` | | `bool` | Check if the configuration has a valid schema |
| `getSchema()` | | `dict` | Get the configuration schema |
| `validateConfiguration()` | | `list[str]` | Validate the configuration against its schema |
| `setValidationEnabled(enabled)` | `enabled: bool` | | Enable or disable schema validation |
| `isValidationEnabled()` | | `bool` | Check if schema validation is enabled |
| `getValidationErrors()` | | `list[str]` | Get validation errors |
| `clearValidationErrors()` | | | Clear validation errors |

#### Configuration Templates Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `getTemplateCount()` | | `int` | Get the number of saved templates |
| `getTemplateNames()` | | `list[str]` | Get names of all saved templates |
| `getTemplate(templateName)` | `templateName: str` | `Config` | Get a configuration template by name |
| `addTemplate(templateName, templateConfig)` | `templateName: str, templateConfig: Config` | | Add a configuration template |
| `removeTemplate(templateName)` | `templateName: str` | | Remove a configuration template |
| `loadTemplate(templateName, filepath)` | `templateName: str, filepath: str` | | Load a configuration template from file |

#### Configuration Monitoring Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `hasMonitor()` | | `bool` | Check if configuration monitoring is enabled |
| `getMonitor()` | | `ConfigMonitor` | Get the configuration monitor |
| `setMonitor(monitor)` | `monitor: ConfigMonitor` | | Set the configuration monitor |
| `onConfigurationChanged(key, oldValue, newValue, source)` | `key: str, oldValue: any, newValue: any, source: str` | | Handle configuration change event |
| `getConfigurationHistory()` | | `list[dict]` | Get configuration change history |
| `getRecentChanges(maxChanges)` | `maxChanges: int` | `list[dict]` | Get recent configuration changes |
| `rollback(changeIndex)` | `changeIndex: int` | | Rollback to a specific configuration change |
| `getDifferences(otherConfig)` | `otherConfig: Config` | `list[dict]` | Get differences between this and another configuration |

#### Configuration Comparison Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `compareConfigurations(config1, config2)` | `config1: Config, config2: Config` | `list[dict]` | Compare two configurations |
| `getConfigurationDifferences(config1, config2)` | `config1: Config, config2: Config` | `list[dict]` | Get differences between two configurations |
| `mergeConfigurations(config1, config2)` | `config1: Config, config2: Config` | `Config` | Merge two configurations into a new one |

#### Configuration Security Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `encryptConfiguration(key)` | `key: str` | `str` | Encrypt the configuration |
| `decryptConfiguration(encryptedData, key)` | `encryptedData: str, key: str` | `Config` | Decrypt configuration data |
| `validateSignature(signature, data)` | `signature: str, data: str` | `bool` | Validate configuration signature |
| `setEncryptionKey(key)` | `key: str` | | Set encryption key for configuration |
| `getEncryptionStatus()` | | `dict` | Get encryption status |

#### Configuration Export/Import Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `exportConfiguration(filepath, format)` | `filepath: str, format: str` | `bool` | Export configuration to file |
| `importConfiguration(filepath, format)` | `filepath: str, format: str` | `Config` | Import configuration from file |
| `getExportFormats()` | | `list[str]` | Get available export formats |
| `setExportPath(path)` | `path: str` | | Set export directory path |
| `getExportPath()` | | `str` | Get export directory path |

### SensoryInput

**File:** `/workspace/app/python/bindings.cpp:268`

Base class for sensory input.

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `getType()` | | `int` | Get the type of sensory input |
| `getData()` | | `list[float]` | Get the raw data as a vector |
| `getDimensions()` | | `int` | Get the dimensionality |
| `getTimestamp()` | | `float` | Get the timestamp |
| `setTimestamp(timestamp)` | `timestamp: float` | | Set the timestamp |

### Vision

**File:** `/workspace/app/python/bindings.cpp:276`

Vision sensory input.

#### Constructor
```python
Vision()
Vision(width, height, channels)
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `setData(data)` | `data: list[float]` | | Set the vision data |
| `getWidth()` | | `int` | Get the width |
| `getHeight()` | | `int` | Get the height |
| `getChannels()` | | `int` | Get the number of channels |

### Audio

**File:** `/workspace/app/python/bindings.cpp:287`

Audio sensory input.

#### Constructor
```python
Audio()
Audio(sampleRate, numSamples)
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `setData(data)` | `data: list[float]` | | Set the audio data |
| `setSampleRate(sampleRate)` | `sampleRate: int` | | Set the sample rate |
| `getSampleRate()` | | `int` | Get the sample rate |
| `getNumSamples()` | | `int` | Get the number of samples |

### InternalSignals

**File:** `/workspace/app/python/bindings.cpp:297`

Internal signals sensory input.

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `addSignal(value)` | `value: float` | | Add a signal value |
| `clearSignals()` | | | Clear all signals |

### Action

**File:** `/workspace/app/python/bindings.cpp:302`

Action representation for motor output.

#### Constructor
```python
Action()
Action(type)
Action(type, parameters)
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `getType()` | | `ActionType` | Get the action type |
| `setType(type)` | `type: ActionType` | | Set the action type |
| `getParameters()` | | `list[float]` | Get the parameters |
| `setParameters(params)` | `params: list[float]` | | Set the parameters |
| `getName()` | | `str` | Get the action name |
| `clone()` | | `Action` | Clone the action |

### WorldObject

**File:** `/workspace/app/python/bindings.cpp:313`

World object representation.

#### Constructor
```python
WorldObject()
WorldObject(x, y, type, value, radius)
```

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `x` | `float` | X position |
| `y` | `float` | Y position |
| `radius` | `float` | Radius |
| `type` | `WorldObjectType` | Object type |
| `value` | `float` | Value |
| `active` | `bool` | Whether the object is active |

### AgentBody

**File:** `/workspace/app/python/bindings.cpp:325`

Agent body state.

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `x` | `float` | X position |
| `y` | `float` | Y position |
| `orientation` | `float` | Orientation |
| `velocityX` | `float` | X velocity |
| `velocityY` | `float` | Y velocity |
| `angularVelocity` | `float` | Angular velocity |
| `energy` | `float` | Energy |
| `health` | `float` | Health |
| `age` | `float` | Age |
| `isMoving` | `bool` | Whether moving |
| `isTurning` | `bool` | Whether turning |
| `lastActionTime` | `float` | Last action time |

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `reset()` | | | Reset the agent body |

### ActionResult

**File:** `/workspace/app/python/bindings.cpp:341`

Action result from world.

#### Constructor
```python
ActionResult()
ActionResult(reward, success, message)
```

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `reward` | `float` | Reward received |
| `success` | `bool` | Whether action was successful |
| `message` | `str` | Message about the action |

### SensoryPercept
n
**File:** `/workspace/app/python/bindings.cpp:349`

Sensory percept data.

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `getVision()` | | `Vision` | Get the vision data |
| `setVision(vision)` | `vision: Vision` | | Set the vision data |
| `getVisionWidth()` | | `int` | Get vision width |
| `getVisionHeight()` | | `int` | Get vision height |
| `getTouch()` | | `InternalSignals` | Get touch data |
| `setTouch(touch)` | `touch: InternalSignals` | | Set touch data |
| `getInternal()` | | `InternalSignals` | Get internal signals |
| `setInternal(internal)` | `internal: InternalSignals` | | Set internal signals |
| `getProprioception()` | | `InternalSignals` | Get proprioception |
| `setProprioception(proprioception)` | `proprioception: InternalSignals` | | Set proprioception |
| `getAudio()` | | `Audio` | Get audio data |
| `setAudio(audio)` | `audio: Audio` | | Set audio data |
| `getAllSignals()` | | `dict` | Get all signals |
| `getTimestamp()` | | `float` | Get timestamp |
| `setTimestamp(timestamp)` | `timestamp: float` | | Set timestamp |

### SimpleWorld

**File:** `/workspace/app/python/bindings.cpp:367`

Simple 2D world for NLM simulation.

#### Constructor
```python
SimpleWorld()
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `configure(width, height, visionWidth, visionHeight)` | `width: int, height: int, visionWidth: int, visionHeight: int` | | Configure the world |
| `reset()` | | | Reset the world to initial state |
| `setAgentStart(x, y)` | `x: float, y: float` | | Set agent start position |
| `update(timestep)` | `timestep: float` | | Update world physics |
| `applyMotorCommand(cmd, currentTime)` | `cmd: MotorCommand, currentTime: float` | `ActionResult` | Apply motor command |
| `getSensoryPercept()` | | `SensoryPercept` | Get sensory percept |
| `getAgentBody()` | | `AgentBody` | Get agent body |
| `addObject(obj)` | `obj: WorldObject` | | Add world object |
| `removeObject(x, y)` | `x: float, y: float` | | Remove object at position |
| `isValidPosition(x, y)` | `x: float, y: float` | `bool` | Check if position is valid |
| `getWidth()` | | `int` | Get world width |
| `getHeight()` | | `int` | Get world height |
| `getMaxEnergy()` | | `float` | Get maximum energy |
| `setMaxEnergy(e)` | `e: float` | | Set maximum energy |
| `getEnergyDecayRate()` | | `float` | Get energy decay rate |
| `setEnergyDecayRate(r)` | `r: float` | | Set energy decay rate |
| `getSimulationTime()` | | `float` | Get simulation time |
| `setRandomSeed(seed)` | `seed: int` | | Set random seed |
| `getRandomSeed()` | | `int` | Get random seed |

### Brain

**File:** `/workspace/app/python/bindings.cpp:393`

Central neural simulation brain class.

#### Constructor
```python
Brain(config)
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `initialize()` | | | Initialize the brain with configuration |
| `step(currentStep)` | `currentStep: int` | | Perform a simulation step |
| `step(currentStep, currentTime)` | `currentStep: int, currentTime: float` | | Perform a simulation step with timestamp |
| `receiveSensoryInput(input)` | `input: SensoryInput` | | Inject sensory input into the brain |
| `injectCurrent(neuron, current)` | `neuron: NeuronId, current: float` | | Inject current into a specific neuron |
| `injectCurrentToNeurons(type, current)` | `type: NeuronType, current: float` | | Inject current into all neurons of a specific type |
| `produceAction()` | | `Action` | Produce motor action based on neural activity |
| `reset()` | | | Reset brain state |
| `save(filepath)` | `filepath: str` | | Save brain state to file |
| `load(filepath)` | `filepath: str` | | Load brain state from file |
| `addRegion(name)` | `name: str` | `RegionId` | Add a new neural region |
| `getRegion(id)` | `id: RegionId` | `Region` | Get a region by ID |
| `getRegionCount()` | | `int` | Get the number of regions |
| `getRegionIds()` | | `list[RegionId]` | Get all region IDs |
| `getRegions()` | | `list[Region]` | Get all regions |
| `getTotalNeuronCount()` | | `int` | Get total neuron count across all regions |
| `getTotalSynapseCount()` | | `int` | Get total synapse count across all regions |
| `getActiveNeuronCount()` | | `int` | Get count of active neurons |
| `getFiringNeuronCount()` | | `int` | Get count of currently firing neurons |
| `getAverageFiringRate()` | | `float` | Get average firing rate across all neurons |
| `getExcitationInhibitionRatio()` | | `float` | Get excitation/inhibition balance ratio |
| `getTotalSpikeCount()` | | `int` | Get total spike count |
| `getDevelopmentalStage()` | | `DevelopmentalStage` | Get current developmental stage |
| `setDevelopmentalStage(stage)` | `stage: DevelopmentalStage` | | Set developmental stage |
| `getConfig()` | | `Config` | Get the configuration |
| `logStatus()` | | | Log brain status |

#### Advanced Systems Access Methods

| Method | Returns | Description |
|--------|---------|-------------|
| `getDevelopmentSystem()` | `DevelopmentSystem` | Get development system |
| `getSTDP()` | `STDP` | Get STDP system |
| `getHebbian()` | `Hebbian` | Get Hebbian system |
| `getStructuralPlasticity()` | `StructuralPlasticity` | Get structural plasticity system |
| `getWorkingMemory()` | `WorkingMemory` | Get working memory system |
| `getEpisodicMemory()` | `EpisodicMemory` | Get episodic memory system |
| `getAssociativeMemory()` | `AssociativeMemory` | Get associative memory system |

### WorkingMemory

**File:** `/workspace/app/python/bindings.cpp:497`

Working memory system for temporary active storage of information.

#### Constructor
```python
WorkingMemory()
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `initialize()` | | | Initialize working memory |
| `setCapacity(capacity)` | `capacity: int` | | Set working memory capacity |
| `store(neuron, value)` | `neuron: NeuronId, value: float` | | Store information in working memory |
| `recall(neuron)` | `neuron: NeuronId` | `float` | Retrieve information from working memory |
| `update(decayRate)` | `decayRate: float` | | Update working memory with decay |
| `getActiveTraces()` | | `int` | Get number of active memory traces |
| `getMemoryNeurons()` | | `int` | Get memory neurons count |
| `clear()` | | | Clear working memory |

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `capacity` | `int` | Working memory capacity |
| `activeTraces` | `int` | Number of active memory traces |
| `memoryNeurons` | `int` | Memory neurons count |

### EpisodicMemory

**File:** `/workspace/app/python/bindings.cpp:521`

Episodic memory system for storing experiences and events.

#### Constructor
```python
EpisodicMemory()
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `initialize()` | | | Initialize episodic memory |
| `setMaxEpisodes(maxEpisodes)` | `maxEpisodes: int` | | Set maximum number of episodes |
| `storeEpisode(episode)` | `episode: EpisodicMemoryItem` | | Store an episode in episodic memory |
| `getEpisodes()` | | `int` | Get number of stored episodes |
| `replayEpisode(episode)` | `episode: EpisodicMemoryItem` | | Replay a stored episode |
| `getEpisodesForReplay(count)` | `count: int` | `list[EpisodicMemoryItem]` | Get episodes suitable for replay |
| `consolidate(relevanceThreshold)` | `relevanceThreshold: float` | | Consolidate memory by removing irrelevant episodes |
| `clear()` | | | Clear episodic memory |

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `maxEpisodes` | `int` | Maximum number of episodes |
| `episodeCount` | `int` | Current number of episodes |

### EpisodicMemoryItem

**File:** `/workspace/app/python/bindings.cpp:96`

Container for episodic memory episode data.

#### Constructor
```python
EpisodicMemoryItem(timestamp, neurons, values, metadata)
```

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `timestamp` | `int` | Simulation step when episode occurred |
| `neurons` | `list[NeuronId]` | List of NeuronId objects |
| `values` | `list[float]` | List of corresponding float values |
| `metadata` | `str` | Optional metadata string |

### AssociativeMemory

**File:** `/workspace/app/python/bindings.cpp:543`

Associative memory system for creating relationships between neural representations.

#### Constructor
```python
AssociativeMemory()
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `initialize()` | | | Initialize associative memory |
| `addAssociation(a, b, strength)` | `a: NeuronId, b: NeuronId, strength: float` | | Create association between two neurons |
| `recall(pattern)` | `pattern: list[float]` | `list[float]` | Recall patterns associated with a query pattern |
| `getAssociations(neuron)` | `neuron: NeuronId` | `list[NeuronId]` | Get neurons associated with a specific neuron |
| `clear()` | | | Clear associative memory |

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `associationCount` | `int` | Number of associations |

### Neuromodulator

**File:** `/workspace/app/python/bindings.cpp:559`

Abstract base class for neuromodulatory signals.

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `getName()` | | `str` | Get neuromodulator name |
| `getLevel()` | | `float` | Get current neuromodulator level |
| `setLevel(level)` | `level: float` | | Set neuromodulator level |
| `getPlasticityFactor()` | | `float` | Get plasticity factor for this neuromodulator |
| `update(dt)` | `dt: float` | | Update neuromodulator state |

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `level` | `float` | Current neuromodulator level |
| `enabled` | `bool` | Whether neuromodulator is enabled |
| `type` | `str` | Neuromodulator type |

### Dopamine

**File:** `/workspace/app/python/bindings.cpp:570`

Dopamine neuromodulator for reward and reinforcement.

#### Constructor
```python
Dopamine()
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `getLevel()` | | `float` | Get current dopamine level |
| `setLevel(level)` | `level: float` | | Set dopamine level |
| `getPlasticityFactor()` | | `float` | Get plasticity factor modulated by dopamine |
| `update(dt)` | `dt: float` | | Update dopamine level with decay |
| `signalReward(reward)` | `reward: float` | | Signal reward to dopamine system |
| `signalRewardPredictionError(error)` | `error: float` | | Signal reward prediction error to dopamine system |

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `plasticityFactor` | `float` | Plasticity factor modulated by dopamine |
| `rewardPredictionError` | `float` | Reward prediction error |

### Novelty

**File:** `/workspace/app/python/bindings.cpp:582`

Novelty detection system.

#### Constructor
```python
Novelty()
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `getLevel()` | | `float` | Get current novelty level |
| `setLevel(level)` | `level: float` | | Set novelty level |
| `update(dt)` | `dt: float` | | Update novelty detection |
| `getHistory()` | | `list[float]` | Get novelty detection history |
| `clearHistory()` | | | Clear novelty history |
| `detectNovelty(currentPattern, previousPattern)` | `currentPattern: list[float], previousPattern: list[float]` | `float` | Detect novelty from sensory patterns |

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `detectionStrength` | `float` | Detection strength |
| `recentActivity` | `list[float]` | Recent activity |

### Curiosity

**File:** `/workspace/app/python/bindings.cpp:595`

Curiosity drive system for exploration.

#### Constructor
```python
Curiosity()
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `getLevel()` | | `float` | Get current curiosity level |
| `update(novelty, predictionError, dt)` | `novelty: float, predictionError: float, dt: float` | | Update curiosity based on novelty and prediction error |
| `getExplorationDrive()` | | `float` | Get exploration bias |
| `setNoveltyWeight(weight)` | `weight: float` | | Set weight of novelty in curiosity calculation |
| `setPredictionErrorWeight(weight)` | `weight: float` | | Set weight of prediction error in curiosity calculation |
| `reset()` | | | Reset curiosity to zero |

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `learningRate` | `float` | Learning rate |

### PredictionError

**File:** `/workspace/app/python/bindings.cpp:606`

Prediction error signal for learning.

#### Constructor
```python
PredictionError()
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `getLevel()` | | `float` | Get current prediction error level |
| `computeError(predicted, actual)` | `predicted: float, actual: float` | `float` | Compute prediction error from predicted and actual values |
| `updatePrediction(newPrediction)` | `newPrediction: float` | | Update prediction value |
| `getHistory()` | | `list[float]` | Get prediction error history |
| `clearHistory()` | | | Clear prediction error history |
| `getMagnitude()` | | `float` | Get absolute magnitude of prediction error |

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `predictionError` | `float` | Prediction error |
| `confidence` | `float` | Confidence |

### PredictionSystem

**File:** `/workspace/app/python/bindings.cpp:649`

Prediction system for sensory prediction and error computation.

#### Constructor
```python
PredictionSystem()
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `predict(currentState)` | `currentState: list[float]` | `SensoryInput` | Make prediction for next timestep based on current sensory state |
| `getPrediction()` | | `SensoryInput` | Get prediction (placeholder implementation) |
| `getPredictionError()` | | `float` | Get prediction error |
| `getPredictionConfidence()` | | `float` | Get prediction confidence |
| `getPredictionAccuracy()` | | `float` | Get prediction accuracy (alias for confidence) |
| `update(predicted, actual)` | `predicted: SensoryInput, actual: SensoryInput` | | Update predictions based on actual observation |
| `getErrorHistory()` | | `list[float]` | Get prediction error history |
| `clearHistory()` | | | Clear prediction error history |
| `train(observation)` | `observation: SensoryInput` | | Train prediction model with new observation |

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `enabled` | `bool` | Whether the prediction system is enabled |
| `predictionAccuracy` | `float` | Current prediction accuracy |
| `predictionConfidence` | `float` | Current prediction confidence |

### NeuralPlanner

**File:** `/workspace/app/python/bindings.cpp:685`

Neural planner for action planning using predictive neural dynamics.

#### Constructor
```python
NeuralPlanner()
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `initialize(brain)` | `brain: Brain` | | Initialize planner with brain reference |
| `plan(currentState, targetReward)` | `currentState: list[float], targetReward: float` | `ActionType` | Plan next action given current state and target reward |
| `evaluateAction(action, state)` | `action: ActionType, state: list[float]` | `float` | Evaluate a single action from a given state |
| `evaluateSequence(actions, startState)` | `actions: list[ActionType], startState: list[float]` | `PlanningCandidate` | Evaluate a sequence of actions |
| `getBestAction(currentState)` | `currentState: list[float]` | `ActionType` | Get best action from current state (simplified) |
| `setPlanningDepth(depth)` | `depth: int` | | Set planning horizon |
| `getPlanningDepth()` | | `int` | Get current planning depth |
| `getPlanningHorizon()` | | `int` | Alias for getPlanningDepth |
| `getActionQuality()` | | `float` | Get current planning confidence (action quality) |
| `update(plannedActions, actualActions, actualReward)` | `plannedActions: list[ActionType], actualActions: list[ActionType], actualReward: float` | | Update plans based on actual outcomes |
| `clearCache()` | | | Clear planning cache |
| `setActionQuality(action, quality)` | `action: ActionType, quality: float` | | Set quality of an action from experience |
| `getCurrentGoal()` | | `list[float]` | Get current planning goal |
| `setCurrentGoal(goal)` | `goal: list[float]` | | Set current planning goal |
| `wasRecentPlanSuccessful()` | | `bool` | Check if recent planning was successful |

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `planningDepth` | `int` | Planning depth (look-ahead horizon) |
| `planningHorizon` | `int` | Planning horizon (alias for planningDepth) |
| `actionQuality` | `float` | Current action quality/confidence |

### ConceptFormation

**File:** `/workspace/app/python/bindings.cpp:725`

Concept formation system that discovers recurring patterns from experience.

#### Constructor
```python
ConceptFormation()
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `initialize(brain)` | `brain: Brain` | | Initialize concept formation with brain reference |
| `learnPattern(pattern, features, reward, currentTime)` | `pattern: list[float], features: list[float], reward: float, currentTime: float` | | Learn a pattern and form/update concepts |
| `getConcept(pattern, similarityThreshold)` | `pattern: list[float], similarityThreshold: float` | `int` | Get the concept ID that best matches a pattern |
| `getConceptStability(conceptId)` | `conceptId: int` | `float` | Get stability of a specific concept |
| `getConceptCount()` | | `int` | Get number of discovered concepts |
| `updateConcept(conceptId, newPattern, features, reward)` | `conceptId: int, newPattern: list[float], features: list[float], reward: float` | | Update a concept with new instance |
| `computeSimilarity(patternA, patternB)` | `patternA: list[float], patternB: list[float]` | `float` | Compute similarity between two patterns |
| `isNovel(pattern, similarityThreshold)` | `pattern: list[float], similarityThreshold: float` | `bool` | Check if a pattern is novel |
| `getAllConcepts()` | | `list[Concept]` | Get all discovered concepts |
| `getConceptPrototype(conceptId)` | `conceptId: int` | `list[float]` | Get the prototype pattern for a concept |
| `getConceptInstances(conceptId)` | `conceptId: int` | `list[list[float]]` | Get all instances of a concept |
| `mergeConcepts(conceptA, conceptB)` | `conceptA: int, conceptB: int` | | Merge two concepts when discovered they're actually the same |
| `clear()` | | | Clear all concepts |
| `getGeneralizationAbility(conceptId)` | `conceptId: int` | `float` | Get generalization ability of a concept |
| `setFormationThreshold(threshold)` | `threshold: float` | | Set similarity threshold for concept formation |
| `setStabilityWindow(window)` | `window: int` | | Set stability window (number of observations to consider) |

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `stability` | `float` | Concept stability (generalization ability) |
| `learningRate` | `float` | Learning rate |
| `conceptCount` | `int` | Number of discovered concepts |

### Concept

**File:** `/workspace/app/python/bindings.cpp:541`

Represents a discovered pattern or concept.

#### Constructor
```python
Concept(name, pattern, frequency)
```

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `name` | `str` | Concept name |
| `pattern` | `list[float]` | Pattern vector |
| `frequency` | `float` | Occurrence frequency |

### PlanningCandidate

**File:** `/workspace/app/python/bindings.cpp:428`

A possible action sequence considered during planning.

#### Constructor
```python
PlanningCandidate(actions, predicted_states, expected_reward, confidence, depth)
```

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `actions` | `list[ActionType]` | List of ActionType objects |
| `predicted_states` | `list[list[float]]` | List of predicted state vectors |
| `expected_reward` | `float` | Expected total reward |
| `confidence` | `float` | Planning confidence (0.0 to 1.0) |
| `depth` | `int` | Planning depth (number of steps) |

### DevelopmentSystem

**File:** `/workspace/app/python/bindings.cpp:825`

Neural development system orchestrating developmental stages, plasticity modulation, and structural changes.

#### Constructor
```python
DevelopmentSystem()
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `getCurrentStage()` | | `DevelopmentalStage` | Get current developmental stage |
| `setCurrentStage(stage)` | `stage: DevelopmentalStage` | | Set current developmental stage |
| `update(brain, currentStep)` | `brain: Brain, currentStep: int` | | Update development system with simulation step |
| `update(brain, rng, dt)` | `brain: Brain, rng: RandomGenerator, dt: float` | | Update development system with time duration |
| `getPlasticityModifier()` | | `float` | Get plasticity modifier for current stage |
| `isCriticalPeriod()` | | `bool` | Check if currently in critical period |
| `getCriticalPeriodProgress()` | | `float` | Get progress through critical period |
| `getDevelopmentalAge()` | | `float` | Get developmental age in simulation time |
| `setDevelopmentalAge(age)` | `age: float` | | Set developmental age |
| `getStageName()` | | `str` | Get name of current developmental stage |
| `enable()` | | | Enable development system (set to critical period) |
| `disable()` | | | Disable development system (set to adult stage) |
| `isEnabled()` | | `bool` | Check if development system is enabled (in critical period) |

### AttentionalSelection

**File:** `/workspace/app/python/bindings.cpp:767`

Attentional selection system for focusing on relevant neural populations.

#### Constructor
```python
AttentionalSelection()
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `initialize(brain)` | `brain: Brain` | | Initialize attentional selection with brain reference |
| `focus(region)` | `region: RegionId` | | Focus attention on a specific brain region |
| `getFocusedNeurons(competitors, globalInhibition)` | `competitors: list[NeuronId], globalInhibition: float` | `list[NeuronId]` | Process competing neurons and return winners |
| `getFocusedNeurons()` | | `list[NeuronId]` | Get currently focused neurons |
| `setInhibitionStrength(strength)` | `strength: float` | | Set inhibition strength for attention competition |
| `getInhibitionStrength()` | | `float` | Get current inhibition strength |
| `setExcitationStrength(strength)` | `strength: float` | | Set excitation strength for attended neurons |
| `getExcitationStrength()` | | `float` | Get current excitation strength |
| `update(dt)` | `dt: float` | | Update attentional selection dynamics |
| `isAttended(neuron)` | `neuron: NeuronId` | `bool` | Check if a neuron is currently attended |
| `releaseAttention()` | | | Release attention (allow all regions equal processing) |
| `reset()` | | | Reset attentional selection state |

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `inhibitionStrength` | `float` | Current inhibition strength |
| `excitationStrength` | `float` | Current excitation strength |
| `attentionFocus` | `list[RegionId]` | Current attention focus regions |

### STDP

**File:** `/workspace/app/python/bindings.cpp:859`

Spike-timing dependent plasticity for synaptic learning.

#### Constructor
```python
STDP()
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `update(synapse, preSpikes, postSpikes, dt)` | `synapse: SynapseId, preSpikes: list[float], postSpikes: list[float], dt: float` | | Update STDP synaptic weights based on spike timing |
| `applyWeightChange(synapse, delta)` | `synapse: SynapseId, delta: float` | | Apply weight change to synapse |
| `getName()` | | `str` | Get STDP system name |
| `configure(ltpWeight, ltdWeight, tau)` | `ltpWeight: float, ltdWeight: float, tau: float` | | Configure STDP parameters |

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `ltpWeight` | `float` | LTP weight |
| `ltdWeight` | `float` | LTD weight |
| `timeConstant` | `float` | Time constant |

### Hebbian

**File:** `/workspace/app/python/bindings.cpp:821`

Hebbian learning rule (fire together, wire together).

#### Constructor
```python
Hebbian()
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `getLearningRate()` | | `float` | Get learning rate |
| `setLearningRate(rate)` | `rate: float` | | Set learning rate |
| `getCoincidenceThreshold()` | | `float` | Get coincidence detection threshold |

### StructuralPlasticity

**File:** `/workspace/app/python/bindings.cpp:844`

Creates new neural connections (structural changes).

#### Constructor
```python
StructuralPlasticity()
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `enableGrowth(enable)` | `enable: bool` | | Enable or disable structural growth |
| `isEnabled()` | | `bool` | Check if structural growth is enabled |
| `growConnection(sourceRegion, targetRegion)` | `sourceRegion: RegionId, targetRegion: RegionId` | `SynapseId` | Grow new connection between regions |
| `getConnectionCount()` | | `int` | Get number of structural connections |
| `pruneConnection(synapseId)` | `synapseId: SynapseId` | | Prune structural connection |

### PlasticityRule

**File:** `/workspace/app/python/bindings.cpp:746`

Abstract base for plasticity rules.

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `getName()` | | `str` | Get rule name |
| `shouldFire(preSpike, postSpike, synapse)` | `preSpike: float, postSpike: float, synapse: SynapseId` | `bool` | Check if rule should fire |
| `computeChange(preSpike, postSpike, synapse)` | `preSpike: float, postSpike: float, synapse: SynapseId` | `float` | Compute synaptic weight change |

### AgentBrain

**File:** `/workspace/app/python/bindings.cpp:623`

Agent brain interface connecting NLM brain to world.

#### Constructor
```python
AgentBrain(brain)
```

#### Methods

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `initialize(world)` | `world: SimpleWorld` | | Initialize with world |
| `getSensoryInputSize()` | | `int` | Get expected sensory input size |
| `getMotorOutputSize()` | | `int` | Get expected motor output size |
| `processSensoryInput(percept)` | `percept: SensoryPercept` | | Process sensory percept and inject into brain |
| `decodeMotorCommand()` | | `MotorCommand` | Decode brain motor activity into motor command |
| `applyRewardModulation(reward, predictedReward)` | `reward: float, predictedReward: float` | | Apply reward-based neuromodulation |
| `updateDevelopment(timestep)` | `timestep: int` | | Update development system |
| `getDevelopmentalStage()` | | `DevelopmentalStage` | Get current developmental stage |
| `getNeuromodulationLevel()` | | `float` | Get current neuromodulation level |

## Factory Functions

### Brain Factory

| Function | Parameters | Returns | Description |
|----------|------------|---------|-------------|
| `createBrain(config)` | `config: Config` | `Brain` | Create a new brain with configuration |

### World Factory

| Function | Parameters | Returns | Description |
|----------|------------|---------|-------------|
| `createSimpleWorld()` | | `SimpleWorld` | Create a new simple world |

### Agent Factory

| Function | Parameters | Returns | Description |
|----------|------------|---------|-------------|
| `createAgentBrain(brain)` | `brain: Brain` | `AgentBrain` | Create a new agent brain interface |

### Neuromodulator Factories

| Function | Returns | Description |
|----------|---------|-------------|
| `createDopamine()` | `Dopamine` | Create a dopamine neuromodulator |
| `createNovelty()` | `Novelty` | Create a novelty detection system |
| `createCuriosity()` | `Curiosity` | Create a curiosity system |
| `createPredictionError()` | `PredictionError` | Create a prediction error system |

### Cognition System Factories

| Function | Returns | Description |
|----------|---------|-------------|
| `createPredictionSystem()` | `PredictionSystem` | Create a prediction system |
| `createNeuralPlanner()` | `NeuralPlanner` | Create a neural planner |
| `createConceptFormation()` | `ConceptFormation` | Create a concept formation system |
| `createAttentionalSelection()` | `AttentionalSelection` | Create an attentional selection system |

### Config Factory

| Function | Returns | Description |
|----------|---------|-------------|
| `createDefaultConfig()` | `Config` | Create default configuration |

## Enums

### NeuronType

```python
Excitatory
Inhibitory
Modulatory
Sensory
Motor
Internal
```

### SynapseType

```python
Excitatory
Inhibitory
Modulatory
Electrical
GapJunction
```

### DevelopmentalStage

```python
Initial
CriticalPeriod
Maturation
Adult
Aging
```

### FiringState

```python
Resting
Active
Refractory
Inhibited
```

### ActionType

```python
MoveForward
MoveBackward
MoveLeft
MoveRight
TurnLeft
TurnRight
Look
LookUp
LookDown
Interact
Eat
Drink
Rest
Wait
Custom
```

### MotorCommand

```python
MoveForward
MoveBackward
TurnLeft
TurnRight
LookLeft
LookRight
Interact
Wait
```

### WorldObjectType

```python
Empty
Resource
Hazard
Wall
Marker
```

## Type Definitions

| Type | Description |
|------|-------------|
| `NeuronId` | Unique identifier for a neuron |
| `SynapseId` | Unique identifier for a synapse |
| `RegionId` | Unique identifier for a brain region |
| `PopulationId` | Unique identifier for a neuron population |
| `Action` | Action representation for motor output |
| `Brain` | Central neural simulation brain class |
| `Config` | Configuration class for NLM system |
| `SensoryInput` | Base class for sensory input |
| `Vision` | Vision sensory input |
| `Audio` | Audio sensory input |
| `InternalSignals` | Internal signals sensory input |
| `WorldObject` | World object representation |
| `AgentBody` | Agent body state |
| `ActionResult` | Action result from world |
| `SensoryPercept` | Sensory percept data |
| `SimpleWorld` | Simple 2D world for NLM simulation |
| `WorkingMemory` | Working memory system |
| `EpisodicMemory` | Episodic memory system |
| `AssociativeMemory` | Associative memory system |
| `Neuromodulator` | Abstract base class for neuromodulators |
| `Dopamine` | Dopamine neuromodulator |
| `Novelty` | Novelty detection system |
| `Curiosity` | Curiosity drive system |
| `PredictionError` | Prediction error signal |
| `PredictionSystem` | Prediction system |
| `NeuralPlanner` | Neural planner |
| `ConceptFormation` | Concept formation system |
| `AttentionalSelection` | Attentional selection system |
| `Concept` | Represents a discovered pattern |
| `PlanningCandidate` | A possible action sequence |
| `DevelopmentSystem` | Neural development system |
| `STDP` | Spike-timing dependent plasticity |
| `Hebbian` | Hebbian learning rule |
| `StructuralPlasticity` | Structural plasticity system |
| `PlasticityRule` | Abstract base for plasticity rules |
| `AgentBrain` | Agent brain interface |

## Usage Examples

### Creating a Complete NLM System

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(20, 20, 12, 12)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create neuromodulation systems
dopamine = pynlm.createDopamine()
curiosity = pynlm.createCuriosity()
novelty = pynlm.createNovelty()
prediction_error = pynlm.createPredictionError()

# Create cognition systems
prediction_system = pynlm.createPredictionSystem()
planner = pynlm.createNeuralPlanner()
concept_formation = pynlm.createConceptFormation()
attentional_selection = pynlm.createAttentionalSelection()

# Create memory systems
working_memory = pynlm.WorkingMemory()
working_memory.setCapacity(20)

episodic_memory = pynlm.EpisodicMemory()
episodic_memory.setMaxEpisodes(1000)

associative_memory = pynlm.AssociativeMemory()

# Create development system
development_system = brain.getDevelopmentSystem()

# Create structural plasticity system
structural_plasticity = brain.getStructuralPlasticity()

# Run simulation
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with agent
    agent.processSensoryInput(percept)
    
    # Brain step
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Using Working Memory

```python
# Create working memory
working_memory = pynlm.WorkingMemory()
working_memory.setCapacity(20)

# Store information
neuron_id = pynlm.NeuronId(1)
working_memory.store(neuron_id, 0.8)

# Retrieve information
value = working_memory.recall(neuron_id)
```

### Using Neuromodulation

```python
# Create neuromodulation systems
dopamine = pynlm.createDopamine()
curiosity = pynlm.createCuriosity()
novelty = pynlm.createNovelty()
prediction_error = pynlm.createPredictionError()

# Use neuromodulation in agent
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

# Signal reward
dopamine.signalReward(0.8)

# Update curiosity
curiosity.update(novelty=0.5, prediction_error=0.2, dt=0.1)
```

### Using Prediction and Planning

```python
# Create prediction and planning systems
prediction_system = pynlm.createPredictionSystem()
planner = pynlm.createNeuralPlanner()

# Initialize planner
planner.initialize(brain)
planner.setPlanningDepth(3)

# Make prediction
current_state = percept.getVision()
prediction = prediction_system.predict(current_state)

# Plan action
action = planner.plan(current_state)
```

## Files and Locations

- **Main bindings file:** `/workspace/app/python/bindings.cpp`
- **Default configuration:** `/workspace/app/configs/default.cfg`
- **CMake configuration:** `/workspace/app/python/CMakeLists.txt`

## Building

To build the NLM Python bindings:

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

## Installation

After building, the `pynlm` module will be available in the build directory for Python import.

## Licensing

This software is licensed under the terms of the MIT License.