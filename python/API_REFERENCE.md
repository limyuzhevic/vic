# NLM Python API Reference

This document provides a complete reference for the enhanced NLM Python API.

## Overview

NLM (Neural Learning Machine) is a brain-inspired spiking neural network simulator with enhanced Python bindings that provide comprehensive access to all neural systems.

## Core Classes

### Config

The `Config` class manages neural network configuration parameters.

#### Methods

**Creation**
- `createDefaultConfig()` - Create a default configuration object
- `config.loadFromFile(filepath)` - Load configuration from JSON file
- `config.loadFromArgs(argc, argv)` - Load configuration from command line arguments

**Configuration Access**
- `config.has(key)` - Check if key exists
- `config.getKeys()` - Get all configuration keys
- `config.getInt(key, default)` - Get integer value
- `config.getFloat(key, default)` - Get float value
- `config.getString(key, default)` - Get string value

**Configuration Modification**
- `config.setInt(key, value)` - Set integer value
- `config.setFloat(key, value)` - Set float value
- `config.setString(key, value)` - Set string value
- `config.set(key, value)` - Set value (overloaded)

**Convenience Methods**
- `config.validate()` - Validate configuration and return errors/warnings
- `config.createDerived(suffix)` - Create derived configuration
- `config.exportToDict()` - Export configuration to dictionary
- `config.importFromDict(data)` - Import configuration from dictionary

**Configuration Analysis**
- `config.summary()` - Get configuration summary string
- `config.mergeFrom(other)` - Merge with another configuration

### Brain

The `Brain` class represents the central neural processing unit.

#### Methods

**Creation and Initialization**
- `createBrain(config)` - Create brain with configuration
- `brain.initialize()` - Initialize brain

**Simulation**
- `brain.step(step)` - Perform simulation step
- `brain.step(step, time)` - Perform simulation step with timestamp
- `brain.reset()` - Reset brain state

**Sensory Processing**
- `brain.receiveSensoryInput(input)` - Inject sensory input
- `brain.injectCurrent(neuron, current)` - Inject current into neuron
- `brain.injectCurrentToNeurons(type, current)` - Inject current to neurons of type

**Motor Output**
- `brain.produceAction()` - Produce motor action

**Region Management**
- `brain.addRegion(name)` - Add new neural region
- `brain.getRegion(id)` - Get region by ID
- `brain.getRegionCount()` - Get number of regions
- `brain.getRegionIds()` - Get all region IDs
- `brain.getRegions()` - Get all regions

**Statistics**
- `brain.getTotalNeuronCount()` - Total neuron count
- `brain.getTotalSynapseCount()` - Total synapse count
- `brain.getActiveNeuronCount()` - Active neuron count
- `brain.getFiringNeuronCount()` - Firing neuron count
- `brain.getAverageFiringRate()` - Average firing rate
- `brain.getExcitationInhibitionRatio()` - E/I balance ratio
- `brain.getTotalSpikeCount()` - Total spike count
- `brain.getPendingSpikeEventCount()` - Pending spike events
- `brain.getDevelopmentalStage()` - Current developmental stage

**State Management**
- `brain.save(filepath)` - Save brain state
- `brain.load(filepath)` - Load brain state

**Advanced Methods**
- `brain.getBrainStats()` - Get comprehensive statistics
- `brain.getConnectivityStats()` - Get connectivity statistics
- `brain.getNeuronDistribution()` - Get neuron distribution
- `brain.getNetworkActivity()` - Get network activity
- `brain.getLearningProgress()` - Get learning progress
- `brain.createNetworkSnapshot()` - Create network snapshot
- `brain.compareSnapshots(other_brain)` - Compare with another brain
- `brain.enablePlasticity(neuron, hebbian, stdp, structural)` - Enable plasticity
- `brain.disablePlasticity(neuron)` - Disable plasticity
- `brain.setPlasticity(hebbian, stdp, structural)` - Set plasticity globally
- `brain.benchmarkStep(steps)` - Benchmark brain performance

### AgentBrain

The `AgentBrain` class connects NLM brain to the world.

#### Methods

**Initialization**
- `createAgentBrain(brain)` - Create agent brain interface
- `agent.initialize(world)` - Initialize with world

**Sensory Processing**
- `agent.processSensoryInput(percept)` - Process sensory percept

**Motor Output**
- `agent.decodeMotorCommand()` - Decode brain activity to motor command

**Neuromodulation**
- `agent.applyRewardModulation(reward, predicted_reward)` - Apply reward modulation

**Development**
- `agent.updateDevelopment(timestep)` - Update development system
- `agent.getDevelopmentalStage()` - Get current developmental stage

**System Status**
- `agent.getNeuromodulationLevel()` - Get neuromodulation level
- `agent.getCuriosityLevel()` - Get curiosity level
- `agent.getNoveltyLevel()` - Get novelty level
- `agent.getPredictionError()` - Get prediction error

**Configuration**
- `agent.enableRewardModulation(enable)` - Enable/disable reward modulation
- `agent.enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `agent.enableDevelopment(enable)` - Enable/disable development
- `agent.enableCuriosity(enable)` - Enable/disable curiosity

**Advanced Methods**
- `agent.getAgentStats()` - Get comprehensive agent statistics
- `agent.getSensoryStats()` - Get sensory processing statistics
- `agent.getMotorStats()` - Get motor output statistics
- `agent.getLearningState()` - Get current learning state
- `agent.enableAllSystems()` - Enable all systems
- `agent.disableAllSystems()` - Disable all systems
- `agent.applyLearningStimulus(reward, curiosity)` - Apply learning stimulus
- `agent.getBehaviorProfile()` - Get agent behavior profile
- `agent.createAgentSnapshot()` - Create agent snapshot
- `agent.restoreAgentSnapshot(snapshot)` - Restore agent from snapshot

### SimpleWorld

The `SimpleWorld` class represents the simulation environment.

#### Methods

**Configuration**
- `createSimpleWorld()` - Create simple world
- `world.configure(width, height, vision_width, vision_height)` - Configure world

**Control**
- `world.reset()` - Reset world
- `world.setAgentStart(x, y)` - Set agent starting position
- `world.update(timestep)` - Update world
- `world.applyMotorCommand(cmd, current_time)` - Apply motor command

**Sensory**
- `world.getSensoryPercept()` - Get current sensory percept
- `world.getAgentBody()` - Get agent body state

**Objects**
- `world.addObject(obj)` - Add world object
- `world.removeObject(x, y)` - Remove object at position
- `world.isValidPosition(x, y)` - Check if position is valid
- `world.getObjectAt(x, y)` - Get object at position

**Statistics**
- `world.getWidth()`, `world.getHeight()` - Get world dimensions
- `world.getMaxEnergy()`, `world.setMaxEnergy(e)` - Get/set max energy
- `world.getEnergyDecayRate()`, `world.setEnergyDecayRate(r)` - Get/set energy decay
- `world.getSimulationTime()` - Get simulation time
- `world.getRandomSeed()`, `world.setRandomSeed(seed)` - Get/set random seed

**Convenience Methods**
- `world.getObjectCounts()` - Get counts of different object types
- `world.getEnvironmentStats()` - Get environmental statistics
- `world.getDistanceStats()` - Get distance statistics
- `world.getCollisionStats()` - Get collision statistics
- `world.getResourceStats()` - Get resource statistics
- `world.getHazardStats()` - Get hazard statistics
- `world.findNearestObject(type)` - Find nearest object of specified type
- `world.getObjectsOfType(type)` - Get all objects of specified type
- `world.createRandomObject(type, x, y)` - Create random object
- `world.spawnResources(count)` - Spawn random resources
- `world.spawnHazards(count)` - Spawn random hazards
- `world.spawnWalls(count)` - Spawn random walls
- `world.runSimulationStep(timestep)` - Run one simulation step
- `world.saveState(filename)` - Save world state
- `world.loadState(filename)` - Load world state

### SensoryInput and Subclasses

#### SensoryInput (Base Class)

- `getType()` - Get sensory input type
- `getData()` - Get raw data
- `getDimensions()` - Get dimensionality
- `getTimestamp()` - Get timestamp
- `setTimestamp(timestamp)` - Set timestamp
- `getStatistics()` - Get statistical summary
- `normalize()` - Normalize data
- `clone()` - Clone input

#### Vision

- `Vision(width, height, channels)` - Create vision object
- `getWidth()`, `getHeight()`, `getChannels()` - Get dimensions
- `setData(data)` - Set data
- `normalizeIntensity(target_mean, target_std)` - Normalize intensity
- `computeHistogram(bins)` - Compute histogram
- `getAverageColor()` - Get average color (R, G, B)
- `detectEdges(threshold)` - Detect edges
- `resize(new_width, new_height)` - Resize image
- `rotate(angle)` - Rotate image (placeholder)
- `flip(horizontal, vertical)` - Flip image
- `getSubImage(x, y, width, height)` - Extract sub-image

#### Audio

- `Audio(sample_rate, num_samples)` - Create audio object
- `setData(data)` - Set audio data
- `setSampleRate(rate)` - Set sample rate
- `getSampleRate()`, `getNumSamples()` - Get audio properties
- `normalizeAmplitude(target_amplitude)` - Normalize amplitude
- `computeSpectrogram(window_size, hop_size)` - Compute spectrogram
- `getFrequencySpectrum(bins)` - Get frequency spectrum (in dB)
- `getAmplitudeStats()` - Get amplitude statistics

#### InternalSignals

- `InternalSignals()` - Create internal signals
- `addSignal(value)` - Add signal value
- `clearSignals()` - Clear signals
- `getSignalCount()` - Get number of signals
- `getSignalStats()` - Get signal statistics
- `detectAnomalies(threshold)` - Detect anomalies
- `smooth(alpha)` - Apply exponential smoothing

### Action

The `Action` class represents motor output.

#### Methods

**Creation**
- `Action()` - Create empty action
- `Action(type)` - Create action with type
- `Action(type, parameters)` - Create action with type and parameters

**Properties**
- `getType()`, `setType(type)` - Get/set action type
- `getParameters()`, `setParameters(params)` - Get/set parameters
- `getName()` - Get action name

**Modification**
- `addParameter(value)` - Add parameter
- `getParameterCount()` - Get number of parameters
- `normalizeParameters(min, max)` - Normalize parameters
- `scaleParameters(factor)` - Scale parameters
- `addNoise(noise_level)` - Add random noise
- `createComposite(base_type, base_params, modifier_type, modifier_params)` - Create composite action

**Validation**
- `validate()` - Validate parameters and return errors
- `serialize()` - Serialize to string
- `deserialize(data)` - Deserialize from string

### WorldObject

The `WorldObject` class represents objects in the world.

#### Properties
- `x`, `y` - Position coordinates
- `radius` - Object radius
- `type` - Object type
- `value` - Object value (reward/damage)
- `active` - Active status

#### Methods
- `distanceTo(x, y)` - Calculate distance to point
- `containsPoint(x, y)` - Check if point is inside object
- `getPosition()` - Get position as (x, y) tuple
- `setPosition(x, y)` - Set position
- `isResource()`, `isHazard()`, `isWall()` - Check object type

### AgentBody

The `AgentBody` class represents agent physical state.

#### Properties
- `x`, `y` - Position coordinates
- `orientation` - Orientation angle
- `velocityX`, `velocityY` - Linear velocity
- `angularVelocity` - Angular velocity
- `energy` - Energy level
- `health` - Health level
- `age` - Age
- `isMoving`, `isTurning` - State flags
- `lastActionTime` - Last action time

#### Methods
- `reset()` - Reset agent body
- `distanceTo(x, y)` - Calculate distance to point
- `getVelocity()` - Get velocity as (vx, vy) tuple
- `getPosition()` - Get position as (x, y) tuple
- `setPosition(x, y)` - Set position
- `setVelocity(vx, vy)` - Set velocity
- `updatePosition(dt)` - Update position based on velocity
- `isHealthy()` - Check if healthy
- `getEnergyRatio()` - Get energy-health ratio
- `applyForce(fx, fy, dt)` - Apply force
- `applyFriction(friction, dt)` - Apply friction

### ActionResult

The `ActionResult` class represents result of agent action.

#### Properties
- `reward` - Reward value
- `success` - Success status
- `message` - Result message

#### Methods
- `isPositive()` - Check if result is positive
- `isNegative()` - Check if result is negative
- `getConfidence()` - Get confidence level
- `toDict()` - Convert to dictionary
- `fromDict(data)` - Create from dictionary

### Convenience Functions

**Core Functions**
- `createDefaultConfig()` - Create default configuration
- `createBrain(config)` - Create brain
- `createSimpleWorld()` - Create simple world
- `createAgentBrain(brain)` - Create agent brain

**Simulation Functions**
- `runSimpleSimulation(brain, world, agent, steps)` - Run simple simulation
- `createCompleteSystem(config_file)` - Create complete system
- `benchmarkBrain(brain, neuron_count, steps)` - Benchmark brain performance

**Analysis Functions**
- `analyzeBrainNetwork(brain)` - Analyze brain network
- `exportNetworkGraph(brain, filename)` - Export network graph
- `createTestEnvironment(width, height)` - Create test environment
- `validateEnvironment(world)` - Validate environment

**Experimental Functions**
- `createPhase6Experiment()` - Create Phase 6 integration experiment
- `runPhase6Experiment(experiment, config)` - Run Phase 6 experiment
- `createPhase6Demo()` - Create Phase 6 demo
- `runPhase6Demo(demo)` - Run Phase 6 demo

**Utility Functions**
- `getVersion()` - Get version information
- `getAvailableFunctions()` - Get list of available functions
- `validateConfig(config)` - Validate configuration
- `initLogging()` - Initialize logging system
- `setLogLevel(level)` - Set logging level
- `shutdown()` - Shutdown NLM system

### Enumerations

**NeuronType**
- Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal

**SynapseType**
- Excitatory, Inhibitory, Modulatory, Electrical, GapJunction

**ActionType**
- MoveForward, MoveBackward, MoveLeft, MoveRight, TurnLeft, TurnRight,
  Look, LookUp, LookDown, Interact, Eat, Drink, Rest, Wait, Custom

**MotorCommand**
- MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, Wait

**WorldObjectType**
- Empty, Resource, Hazard, Wall, Marker

**DevelopmentalStage**
- Initial, CriticalPeriod, Maturation, Adult, Aging

**FiringState**
- Resting, Active, Refractory, Inhibited

## Error Handling

The Python bindings use standard Python exception handling. Common exceptions include:

- `RuntimeError` - General runtime errors
- `ValueError` - Invalid parameter values
- `TypeError` - Type mismatches

## Installation

### Using pip

```bash
pip install pynlm
```

### From source

```bash
pip install -e .
```

### Building from source

```bash
mkdir build
cmpy ...
```

## Examples

See the `examples/` directory for comprehensive examples.

## Testing

Run tests with:

```bash
pytest
```

Or run the example scripts:

```bash
python python/examples.py --run-all
python python/beginner_examples.py
python python/advanced_examples.py
```

## Performance Tips

1. **Reuse Brain objects**: Create and reuse brain objects instead of creating new ones
2. **Batch operations**: Use batch operations when possible
3. **Configuration caching**: Cache configurations for repeated use
4. **Memory management**: Use `with` statements for resource management
5. **Profiling**: Use `benchmarkBrain()` for performance profiling

## License

MIT License - See LICENSE file for details.

## Version

Enhanced NLM Python Bindings 1.0.0-enhanced

## Documentation

For comprehensive documentation, visit:
- https://nlm.readthedocs.io
- https://github.com/nlm-project/nlm