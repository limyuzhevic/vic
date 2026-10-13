# Complete Python Bindings for Neural Learning Machine (NLM)

## Overview

This document provides a comprehensive guide to the complete Python bindings implementation for the Neural Learning Machine (NLM) framework, covering all neural architecture, neuromodulation, memory, cognition, prediction, plasticity, development, and performance systems.

## Implementation Summary

The NLM Python bindings provide complete access to the entire C++ neural simulation framework, exposing:

### 1. Neural Architecture

**Core Components Exposed:**

- **NeuralRegion**: Represents brain regions containing multiple populations and local connectivity
  - Methods: `getId()`, `getName()`, `setName()`, `addPopulation()`, `getPopulation()`, `getPopulationCount()`, `getPopulations()`, `addSynapse()`, `getSynapse()`, `getSynapseCount()`, `getSynapses()`, `getSynapsesFrom()`, `getSynapsesTo()`, `getTotalNeuronCount()`, `getActiveNeuronCount()`, `getFiringNeuronCount()`, `getAverageFiringRate()`, `getAverageSynapticWeight()`, `getSynapticDensity()`, `getAllNeurons()`, `step()`, `reset()`, `initializeRandomConnectivity()`

- **NeuralPopulation**: Group of neurons with shared properties
  - Methods: `getId()`, `getSize()`, `isEmpty()`, `getNeuron()`, `addNeuron()`, `getNeurons()`, `getNeuronType()`, `setNeuronType()`, `getActiveCount()`, `getFiringCount()`, `getAverageFiringRate()`, `getActivityLevel()`, `initializeRandom()`, `step()`, `reset()`, `getNeuronIds()`, `recordActivity()`, `getMeanMembranePotential()`, `getVarianceMembranePotential()`

- **Neuron**: Individual neuron with spiking dynamics
  - Methods: `getId()`, `setId()`, `getType()`, `setType()`, `getMembranePotential()`, `setMembranePotential()`, `getRestPotential()`, `setRestPotential()`, `getThreshold()`, `setThreshold()`, `getRefractoryPeriod()`, `setRefractoryPeriod()`, `getActive()`, `setActive()`, `getFiringCount()`, `incrementFiringCount()`, `getFiringRate()`, `getLastSpikingTime()`, `setLastSpikingTime()`, `getRegionId()`, `setRegionId()`, `getPopulationId()`, `setPopulationId()`, `getPosition()`, `setPosition()`, `getDynamics()`, `setDynamics()`, `spike()`, `reset()`, `update()`, `getStateString()`

- **Synapse**: Connection between neurons with weight and delay
  - Methods: `getId()`, `setId()`, `getSource()`, `setSource()`, `getTarget()`, `setTarget()`, `getWeight()`, `setWeight()`, `getDelay()`, `setDelay()`, `getType()`, `setType()`, `getLastUpdateTime()`, `setLastUpdateTime()`, `getPlasticityRule()`, `setPlasticityRule()`, `getStateString()`

### 2. Neuromodulation System

**Neuromodulator Classes:**

- **Neuromodulator** (Base class)
  - Methods: `getName()`, `getLevel()`, `setLevel()`, `getPlasticityFactor()`, `update()`

- **Dopamine**: Reward and reinforcement learning signal
  - Methods: `signalReward()`, `signalRewardPredictionError()`

- **Curiosity**: Exploration motivation based on novelty and prediction error
  - Methods: `initialize()`, `getLevel()`, `update()`, `getExplorationDrive()`, `setNoveltyWeight()`, `setPredictionErrorWeight()`, `reset()`

- **Novelty**: Detects novelty from comparison with previous observations
  - Methods: `initialize()`, `getLevel()`, `setLevel()`, `detectNovelty()`, `update()`, `getHistory()`, `clearHistory()`

- **PredictionError**: Prediction error signal for learning and curiosity
  - Methods: `initialize()`, `getError()`, `computeError()`, `updatePrediction()`, `getHistory()`, `clearHistory()`, `getMagnitude()`

- **Reward**: Placeholder neuromodulator (Phase 2)
  - Methods: Implementation from Neuromodulator.hpp

### 3. Memory Systems

**Memory Classes:**

- **NeuralWorkingMemory**: Transient working memory using neural dynamics
  - Methods: `initialize()`, `store()`, `storeToNeuron()`, `retrieve()`, `contains()`, `getNeuronActivation()`, `update()`, `clear()`, `getCapacity()`, `setCapacity()`, `getDecayRate()`, `setDecayRate()`, `getMemoryNeurons()`, `strengthenMemory()`, `runCompetition()`, `isWinning()`, `getMemoryActivity()`

- **NeuralEpisodicMemory**: Experience storage with temporal context
  - Methods: `store()`, `retrieve()`, `retrieveByPattern()`, `getAllEpisodes()`, `getEpisodeCount()`, `deleteEpisode()`, `updateEpisodeFeatures()`, `getTemporalSequence()`, `getAssociatedEpisodes()`, `addAssociation()`, `getTemporalProximity()`

- **NeuralAssociativeMemory**: Creates and retrieves relationships between experiences
  - Methods: `initialize()`, `associate()`, `associateFromExperience()`, `retrieve()`, `getAssociationStrength()`, `updateAssociation()`, `spreadActivation()`, `clear()`, `getAssociationCount()`

### 4. Cognition System

**Cognitive Classes:**

- **NeuralPlanner**: Uses learned predictions to plan multi-step actions
  - Methods: `initialize()`, `planAction()`, `evaluateSequence()`, `getPlanningDepth()`, `setPlanningDepth()`, `getPlanningConfidence()`, `updatePlanQuality()`, `clearCache()`, `setActionQuality()`, `getCurrentGoal()`, `setCurrentGoal()`, `wasRecentPlanSuccessful()`

- **ConceptFormation**: Discovers recurring patterns from experience
  - Methods: `initialize()`, `presentExperience()`, `getMatchingConcept()`, `getConcepts()`, `getConcept()`, `getConceptPrototype()`, `getConceptInstances()`, `getConceptStability()`, `getConceptCount()`, `mergeConcepts()`, `updateConcept()`, `computeSimilarity()`, `isNovel()`, `findConceptForPattern()`, `clear()`, `getGeneralizationAbility()`, `setFormationThreshold()`, `setStabilityWindow()`

- **AttentionalSelection**: Neural attention through competition
  - Methods: `initialize()`, `processCompetition()`, `focusOnRegion()`, `releaseAttention()`, `getAttendedRegions()`, `setInhibitionStrength()`, `setExcitationStrength()`, `setCompetitionThreshold()`, `getInhibitionFor()`, `getExcitationFor()`, `update()`, `isAttended()`, `getWinners()`, `applyTopDownBias()`, `applyBottomUpSalience()`, `reset()`

### 5. Development System

**Development Classes:**

- **DevelopmentSystem**: Orchestrates neural development and maturation
  - Methods: `getStage()`, `setStage()`, `advanceStage()`, `getStageName()`, `update()`, `getPlasticityModifier()`, `isCriticalPeriod()`, `getCriticalPeriodProgress()`, `getDevelopmentalAge()`, `setDevelopmentalAge()`

- **Maturation**: Neural maturation processes
  - Methods: Implementation from DevelopmentSystem.hpp

- **Pruning**: Neural pruning and cleanup
  - Methods: Implementation from DevelopmentSystem.hpp

- **Synaptogenesis**: Synapse formation
  - Methods: Implementation from DevelopmentSystem.hpp

### 6. Plasticity System

**Plasticity Classes:**

- **PlasticityRule** (Base class)
  - Methods: `isEnabled()`, `setEnabled()`, `getName()`

- **STDP**: Spike-timing dependent plasticity
  - Methods: `setLTPWeight()`, `getLTPWeight()`, `setLTDWeight()`, `getLTDWeight()`, `setTimeConstant()`, `getTimeConstant()`, `configure()`

- **Hebbian**: Hebbian learning rule
  - Methods: `setLearningRate()`, `getLearningRate()`, `setMaxWeight()`, `getMaxWeight()`

- **StructuralPlasticity**: Structural synaptic plasticity
  - Methods: `addNewConnection()`, `removeConnection()`, `getNewConnectionProbability()`, `setNewConnectionProbability()`, `getPruningProbability()`, `setPruningProbability()`, `getConnectionLimit()`, `setConnectionLimit()`

### 7. Prediction System

**Prediction Classes:**

- **PredictionSystem**: Sensory prediction and error computation
  - Methods: `getPredictionError()`, `updatePrediction()`, `resetPrediction()`, `setPredictionPrecision()`, `getPredictionPrecision()`, `getPredictions()`, `addPredictionSource()`, `removePredictionSource()`

- **NeuralPrediction**: Neural prediction mechanism
  - Methods: Implementation from PredictionSystem.hpp

### 8. Performance System

**Performance Classes:**

- **SpikeSystem**: Event-driven spike processing system
  - Methods: `queueSpike()`, `queueDelayedSpike()`, `processSpikes()`, `processDelayedSpikes()`, `registerHandler()`, `registerDelayedHandler()`, `getSpikeHistory()`, `clearHistory()`, `getSpikeCount()`, `getPendingSpikeCount()`, `getPendingDelayedCount()`, `getAverageSpikeRate()`, `getMostActiveNeurons()`, `reset()`

- **CheckpointSystem**: Brain state checkpointing and recovery
  - Methods: `createCheckpoint()`, `restoreCheckpoint()`, `deleteCheckpoint()`, `getCheckpointList()`, `getCheckpointInfo()`, `setAutoCheckpoint()`, `isAutoCheckpointEnabled()`, `getLastCheckpointTime()`, `getCheckpointCount()`, `clearOldCheckpoints()`

- **SynapseLoader**: Load and save synaptic configurations
  - Methods: `loadSynapseConfig()`, `saveSynapseConfig()`, `validateSynapseConfig()`, `getSupportedFormats()`, `setFileFormat()`, `getFileFormat()`, `setCompressionEnabled()`, `isCompressionEnabled()`

### 9. Brain Components

**Brain Integration:**

- **Brain**: Central neural simulation brain class
  - Methods: `initialize()`, `step()`, `receiveSensoryInput()`, `injectCurrent()`, `injectCurrentToNeurons()`, `produceAction()`, `reset()`, `save()`, `load()`, `addRegion()`, `getRegion()`, `getRegionCount()`, `getRegionIds()`, `getRegions()`, `getTotalNeuronCount()`, `getTotalSynapseCount()`, `getActiveNeuronCount()`, `getFiringNeuronCount()`, `getAverageFiringRate()`, `getExcitationInhibitionRatio()`, `getTotalSpikeCount()`, `getDevelopmentalStage()`, `setDevelopmentalStage()`, `getConfig()`, `logStatus()`, `getSpikeSystem()`, `getSTDP()`, `getHebbian()`, `getStructuralPlasticity()`, `getWorkingMemory()`, `getEpisodicMemory()`, `getAssociativeMemory()`, `getPredictionSystem()`, `getPlanner()`, `getConceptFormation()`, `getAttention()`, `getDevelopmentSystem()`, `getDopamine()`, `getCuriosity()`, `getNovelty()`, `getPredictionErrorSignal()`

- **AgentBrain**: Agent brain interface connecting NLM brain to world
  - Methods: `initialize()`, `getSensoryInputSize()`, `getMotorOutputSize()`, `processSensoryInput()`, `decodeMotorCommand()`, `applyRewardModulation()`, `updateDevelopment()`, `getDevelopmentalStage()`, `getNeuromodulationLevel()`, `getCuriosityLevel()`, `getNoveltyLevel()`, `getPredictionError()`, `reset()`, `getBrain()`, `enableRewardModulation()`, `enableStructuralPlasticity()`, `enableDevelopment()`, `enableCuriosity()`, `isRewardModulationEnabled()`, `isStructuralPlasticityEnabled()`, `isDevelopmentEnabled()`, `isCuriosityEnabled()`

### 10. World Components

**World Integration:**

- **SimpleWorld**: Simple 2D world for NLM simulation
  - Methods: `configure()`, `reset()`, `setAgentStart()`, `update()`, `applyMotorCommand()`, `getSensoryPercept()`, `getAgentBody()`, `addObject()`, `removeObject()`, `isValidPosition()`, `getWidth()`, `getHeight()`, `getMaxEnergy()`, `setMaxEnergy()`, `getEnergyDecayRate()`, `setEnergyDecayRate()`, `getSimulationTime()`, `setRandomSeed()`, `getRandomSeed()`

- **WorldObject**: World object representation
  - Methods: Constructor, `x`, `y`, `radius`, `type`, `value`, `active` attributes

- **AgentBody**: Agent body state
  - Methods: Constructor, `x`, `y`, `orientation`, `velocityX`, `velocityY`, `angularVelocity`, `energy`, `health`, `age`, `isMoving`, `isTurning`, `lastActionTime`, `reset()`

### 11. Sensory Components

**Sensory Integration:**

- **SensoryInput** (Base class)
  - Methods: `getType()`, `getData()`, `getDimensions()`, `getTimestamp()`, `setTimestamp()`

- **Vision**: Vision sensory input
  - Methods: Constructor, `setData()`, `getWidth()`, `getHeight()`, `getChannels()`

- **Audio**: Audio sensory input
  - Methods: Constructor, `setData()`, `setSampleRate()`, `getSampleRate()`, `getNumSamples()`

- **InternalSignals**: Internal signals sensory input
  - Methods: Constructor, `addSignal()`, `clearSignals()`

### 12. Motor Components

**Motor Integration:**

- **Action**: Action representation for motor output
  - Methods: Constructor, `getType()`, `setType()`, `getParameters()`, `setParameters()`, `getName()`, `clone()`

- **ActionResult**: Action result from world
  - Methods: Constructor, `reward`, `success`, `message` attributes

### 13. Environment Components

**Environment Integration:**

- **Observation**: Sensory input observation
  - Methods: Constructor, `getSensoryInput()`, `isValid()`, `clone()`

### 14. Spatial and Temporal Processing

**Cognitive Processing:**

- **SpatialRepresentation**: Learns spatial relationships from experience
  - Methods: `initialize()`, `recordPosition()`, `getPredictedPosition()`, `integrateMovement()`, `getPositionActivation()`, `getPlaceNeuronsNear()`, `getGridSpacing()`, `clear()`

- **TemporalRelation**: Learns temporal relationships (before, after, sequence)
  - Methods: `initialize()`, `recordSequence()`, `predictNext()`, `predictPrevious()`, `getPredictionConfidence()`, `getSequenceLength()`, `clear()`

- **SelfModel**: Represents the agent's internal model of itself
  - Methods: `initialize()`, `recordSelfAction()`, `predictActionConsequence()`, `getSelfModelConfidence()`, `computeSelfGeneratedLikeness()`, `getPreferredAction()`, `updateSelfModel()`, `getCapabilityLevel()`, `getBodyAwareness()`, `clear()`, `hasSelfModel()`

- **SocialLearning**: Enables learning from observing other agents
  - Methods: `initialize()`, `observeAgentAction()`, `canImitate()`, `getImitationAction()`, `learnCommunicationSignal()`, `detectSignal()`, `getSignalPattern()`, `getSignalMeaning()`, `updateSocialKnowledge()`, `clear()`, `hasSocialKnowledge()`, `getObservationCount()`

### 15. Visualization

**Visualization Interface:**

- **Visualizer**: Visualization interface for debugging and analysis
  - Methods: `initialize()`, `update()`, `render()`, `close()`, `isActive()`, `setUpdateRate()`, `getUpdateRate()`, `visualizeNetwork()`, `visualizeActivity()`, `visualizeSpikes()`, `visualizeWeights()`

## Factory Functions and Utilities

### Factory Functions

```python
# Core factory functions
createDefaultConfig() -> Config
createBrain(config: Config) -> Brain
createSimpleWorld() -> SimpleWorld
createAgentBrain(brain: Brain) -> AgentBrain
createDefaultSensoryInput(type: str, width: int, height: int, channels: int = 3) -> SensoryInput
```

### Network Analysis

```python
# Network analysis utilities
analyzeNetworkTopology(brain: Brain) -> dict
computeNetworkMetrics(brain: Brain) -> dict
findCriticalNeurons(brain: Brain, threshold: float) -> list
calculateClusteringCoefficient(brain: Brain) -> float
computeSmallWorldness(brain: Brain) -> float
```

### Simulation Control

```python
# Simulation control utilities
runSimulation(brain: Brain, world: SimpleWorld, steps: int) -> int
runEpisode(agent: AgentBrain, world: SimpleWorld, maxSteps: int) -> Action
```

### Debugging Tools

```python
# Debugging utilities
brainProfiler(brain: Brain) -> dict
debugSpikeTiming(brain: Brain) -> dict
validateBrainState(brain: Brain) -> bool
analyzeMemoryUsage(brain: Brain) -> dict
```

### Advanced Utilities

```python
# Advanced utilities
showExampleUsage() -> str
float_array_to_list(array: list) -> list
list_to_float_array(list: list) -> list
setMemoryLimit(limitMB: int) -> bool
getMemoryUsage() -> float
```

## Constants

```python
# ID constants
INVALID_NEURON_ID
INVALID_SYNAPSE_ID
INVALID_REGION_ID
INVALID_POPULATION_ID

# Version and metadata
VERSION
BUILD_DATE
API_LEVEL
```

## Error Classes

```python
# Custom error classes
RuntimeError
InvalidArgument
```

## Installation and Setup

### Prerequisites

- Python 3.8+
- C++ compiler with C++20 support
- CMake 3.16+
- pybind11

### Building

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

### Python Usage

```python
import pynlm

# Create configuration and brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Add neural regions
region_id = brain.addRegion("sensory")
region = brain.getRegion(region_id)

# Create populations
population_id = region.addPopulation(100, pynlm.NeuronType.Excitatory)
population = region.getPopulation(population_id)

# Initialize and step simulation
brain.step(0)
action = agent.produceAction()

# Neuromodulation example
dopamine = brain.getDopamine()
dopamine.signalReward(1.0)

print(f"Action: {action.getType()}, Reward: {action.getParameters()[0]}")
```

## Advanced Usage Examples

### Network Analysis

```python
def analyze_brain_network(brain):
    # Analyze network topology
    topology = pynlm.analyzeNetworkTopology(brain)
    
    # Compute network metrics
    metrics = pynlm.computeNetworkMetrics(brain)
    
    # Find critical neurons
    critical = pynlm.findCriticalNeurons(brain, 0.5)
    
    # Calculate clustering coefficient
    clustering = pynlm.calculateClusteringCoefficient(brain)
    
    # Compute small-worldness
    small_world = pynlm.computeSmallWorldness(brain)
    
    return topology, metrics, critical, clustering, small_world
```

### Simulation Control

```python
def run_experiment(config, world, agent, steps=1000):
    # Run simulation
    results = pynlm.runSimulation(agent.getBrain(), world, steps)
    
    # Run episode
    action = pynlm.runEpisode(agent, world, steps)
    
    return results, action
```

### Neuromodulation

```python
def modulate_brain(brain, reward=1.0):
    # Apply reward modulation
    dopamine = brain.getDopamine()
    dopamine.signalReward(reward)
    
    # Get curiosity level
    curiosity = brain.getCuriosity()
    curiosity_level = curiosity.getLevel()
    
    # Get novelty level
    novelty = brain.getNovelty()
    novelty_level = novelty.getLevel()
    
    # Get prediction error
    prediction_error = brain.getPredictionErrorSignal()
    error = prediction_error.getError()
    
    return curiosity_level, novelty_level, error
```

## Performance Considerations

### Memory Management

```python
# Set memory limit
pynlm.setMemoryLimit(512)  # 512MB

# Monitor memory usage
usage = pynlm.getMemoryUsage()
print(f"Memory usage: {usage} MB")
```

### Network Analysis Optimization

```python
# Network analysis with custom parameters
topology = pynlm.analyzeNetworkTopology(brain)
metrics = pynlm.computeNetworkMetrics(brain)
critical = pynlm.findCriticalNeurons(brain, 0.7)  # Higher threshold
```

## Testing and Validation

### Basic Functionality Test

```python
def test_basic_functionality():
    import pynlm
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Add region and population
    region_id = brain.addRegion("test")
    region = brain.getRegion(region_id)
    pop_id = region.addPopulation(10)
    population = region.getPopulation(pop_id)
    
    # Step simulation
    brain.step(0)
    
    # Test action production
    action = brain.produceAction()
    
    # Test neuromodulation
    dopamine = brain.getDopamine()
    dopamine.signalReward(1.0)
    
    print("Basic functionality test passed!")
    return True
```

### Network Analysis Test

```python
def test_network_analysis():
    import pynlm
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Add region
    region_id = brain.addRegion("test")
    brain.addRegion(region_id)
    
    # Run network analysis
    topology = pynlm.analyzeNetworkTopology(brain)
    metrics = pynlm.computeNetworkMetrics(brain)
    critical = pynlm.findCriticalNeurons(brain, 0.5)
    
    # Validate results
    assert isinstance(topology, dict)
    assert isinstance(metrics, dict)
    assert isinstance(critical, list)
    
    print("Network analysis test passed!")
    return True
```

## Known Limitations

1. **Phase 2 Implementation**: Some classes are marked as placeholder implementations (Phase 2 TODO)
2. **Memory Management**: Memory pools and optimization features may need further tuning
3. **Visualization**: Real-time visualization requires additional libraries
4. **Performance**: Large-scale simulations may require additional optimization

## Future Enhancements

1. **Advanced Neuromodulation**: Implement realistic neuromodulation dynamics
2. **Synaptic Dynamics**: Real synaptic dynamics with short-term plasticity
3. **Learning Mechanisms**: More sophisticated learning algorithms
4. **Cognitive Architectures**: Integration with cognitive architectures
5. **Multi-Agent Systems**: Support for multiple interacting agents
6. **Hardware Acceleration**: GPU acceleration for large-scale simulations

## License

Apache-2.0

## Acknowledgments

This implementation represents a significant milestone in the NLM project, providing comprehensive Python bindings for the entire neural simulation framework. Special thanks to all contributors and the open-source community.
