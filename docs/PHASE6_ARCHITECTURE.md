# NLM/熙然 Phase 6 Architecture Documentation

## Overview

Phase 6 integrates all previously separate systems into a coherent artificial brain. The architecture follows a closed-loop design where neural activity, plasticity, memory, neuromodulation, and behavior continuously influence each other.

## Brain Loop Architecture

The main simulation loop (`Brain::step()`) implements the complete brain simulation:

```cpp
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // 1. Process pending delayed spikes (deliver synaptic input)
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // 2. Update all neurons (LIF dynamics)
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // 3. Detect spikes and schedule spike events
    // ... spike detection and propagation ...
    
    // 4. Update working memory
    pImpl->workingMemory->update(pImpl->timestep);
    
    // 5. Apply neuromodulation effects
    // - Dopamine affects neural excitability
    // - Novelty detection updates
    // - Curiosity computation
    
    // 6. Apply plasticity rules (STDP and Hebbian)
    // with neuromodulation modulation
    
    // 7. Update episodic memory with current experience
    
    // 8. Update prediction system
    
    // 9. Update attention system
    
    // 10. Update concept formation
    
    // 11. Apply structural plasticity (synaptogenesis/pruning)
    
    // 12. Replay important memories
    
    // 13. Apply development effects
    
    // 14. Memory consolidation
    
    // 15. Checkpoint management
}
```

## Advanced Python Bindings for Phase 6

### 1. Enhanced Brain Statistics

#### Synaptic Statistics
- **getSynapticDensity()**: Returns average synaptic density across all regions
- **getAverageSynapticWeight()**: Returns average synaptic weight
- **getConnectionProbability()**: Returns connection probability

#### Regional Statistics
- **getRegionalStats()**: Returns detailed statistics for all regions including:
  - Region ID and name
  - Neuron and synapse counts
  - Active and firing neuron counts
  - Average firing rate
  - Excitation/inhibition ratio
  - Synaptic density and weight

#### Firing Statistics
- **getFiringStatistics()**: Returns detailed firing statistics including:
  - Total spike count
  - Firing neuron count
  - Average, maximum, and minimum firing rates
  - Standard deviation, skew, and kurtosis of firing rates

#### Neural Population Statistics
- **getNeuralPopulationStats()**: Returns statistics for neural populations including:
  - Population ID and region
  - Neuron count
  - Neuron type
  - Active and firing neuron counts
  - Average firing rate
  - Excitation/inhibition ratio

#### Network Metrics
- **getCriticalityIndex()**: Returns criticality index (measure of being at edge of chaos)
- **getNetworkEfficiency()**: Returns global network efficiency
- **getSynchronizationMetrics()**: Returns neural synchronization metrics
- **getControlFlowMetrics()**: Returns control flow and information routing metrics
- **getInformationProcessingRate()**: Returns information processing rate (bits per second)
- **getEnergyConsumption()**: Returns total energy consumption

### 2. Memory System Access

#### Working Memory (NeuralWorkingMemory)

**Python API Methods:**
- **initialize(brain)**: Initialize with brain reference
- **store(pattern, strength=1.0)**: Store information in working memory
- **storeToNeuron(neuron, activation)**: Store specific neuron activation
- **retrieve()**: Retrieve current working memory content as activity levels
- **contains(neuron)**: Check if specific neuron is part of working memory
- **getNeuronActivation(neuron)**: Get activation level of a specific neuron
- **update(dt)**: Update working memory (maintenance and decay)
- **clear()**: Clear working memory
- **getActiveTraces()**: Get number of active memory traces
- **getCapacity()**: Get capacity
- **setCapacity(cap)**: Set capacity
- **getDecayRate()**: Get decay rate
- **setDecayRate(rate)**: Set decay rate
- **getMemoryNeurons()**: Get neurons currently in working memory
- **strengthenMemory(factor)**: Strengthen working memory representation
- **runCompetition()**: Competition between memory traces
- **isWinning(neuron)**: Is this neuron part of the winning population?
- **getMemoryActivity()**: Get overall memory activity level

#### Episodic Memory (NeuralEpisodicMemory)

**Python API Methods:**
- **initialize(brain)**: Initialize with brain reference
- **storeEpisode(episode)**: Store a new experience
- **retrieveSimilar(sensoryPattern, maxResults=5)**: Retrieve episodes similar to a query pattern
- **retrieveTemporal(startTime, endTime, maxResults=10)**: Retrieve episodes from a specific time window
- **retrieveByLocation(x, y, radius, maxResults=5)**: Retrieve episodes by location
- **retrieveAfterAction(action, maxResults=5)**: Retrieve episodes following a specific action
- **replayEpisode(episode)**: Replay an episode - reactivates the neural patterns
- **updateRelevance(episodeId, relevanceDelta)**: Update episode relevance based on recent use
- **getEpisode(index)**: Get episode by index
- **getEpisodeCount()**: Get total number of episodes
- **getRecentEpisodes(count)**: Get recent episodes
- **getAverageReward()**: Get average reward from episodes
- **consolidate(relevanceThreshold)**: Clear old episodes based on relevance threshold
- **clear()**: Clear all episodes
- **enableReplay(enable)**: Enable/disable replay
- **isReplayEnabled()**: Check if replay is enabled
- **getEpisodesForReplay(count)**: Get episodes for replay
- **replaySequence(episodeIds)**: Replay multiple episodes

#### Associative Memory (NeuralAssociativeMemory)

**Python API Methods:**
- **initialize(brain)**: Initialize with brain reference
- **associate(patternA, patternB, strength=1.0)**: Create association between two neural patterns
- **associateFromExperience(episode)**: Create association based on experience
- **retrieve(queryPattern, maxResults=5)**: Retrieve patterns associated with a query pattern
- **getAssociationStrength(patternA, patternB)**: Get association strength between two patterns
- **updateAssociation(patternA, patternB, delta)**: Update association based on outcome
- **spreadActivation(cuePattern, steps=2)**: Spread activation from cue pattern
- **clear()**: Clear associations
- **getAssociationCount()**: Get number of associations

### 3. Prediction System Access

#### PredictionSystem

**Python API Methods:**
- **predictNextState(currentState)**: Predict next state from current state
- **updatePredictions(predicted, actual)**: Update predictions with actual observed state
- **getPredictionError()**: Get prediction error
- **getConfidence()**: Get prediction confidence
- **getErrorHistory()**: Get prediction error history
- **clearHistory()**: Clear prediction error history
- **train(observation)**: Train prediction model with observation

#### PredictionError

**Python API Methods:**
- **initialize(brain)**: Initialize with brain reference
- **getError()**: Get error value
- **computeError(predicted, actual)**: Compute prediction error
- **updatePrediction(newPrediction)**: Update prediction
- **getHistory()**: Get error history
- **clearHistory()**: Clear error history
- **getMagnitude()**: Get error magnitude for neuromodulation

### 4. Development System Access

#### DevelopmentSystem

**Python API Methods:**
- **getStage()**: Get current developmental stage
- **setStage(stage)**: Set developmental stage
- **advanceStage()**: Advance developmental stage
- **getStageName()**: Get current stage name
- **update(brain, currentStep)**: Update development system
- **update(brain, rng, dt)**: Update development system with full parameters
- **getPlasticityModifier()**: Get plasticity modifier for current stage
- **isCriticalPeriod()**: Check if currently in critical period
- **getCriticalPeriodProgress()**: Get critical period progress (0 to 1)
- **getDevelopmentalAge()**: Get developmental age in simulation time
- **setDevelopmentalAge(age)**: Set developmental age

### 5. Neuromodulation System

#### Neuromodulator (Base Class)

**Python API Methods:**
- **getName()**: Get neuromodulator name
- **getLevel()**: Get current neuromodulator level
- **setLevel(level)**: Set neuromodulator level
- **getPlasticityFactor()**: Get plasticity factor
- **update(dt)**: Update neuromodulator state

#### Dopamine

**Python API Methods:**
- **signalReward(reward)**: Signal reward (dopamine response)
- **signalRewardPredictionError(error)**: Signal reward prediction error

#### Curiosity

**Python API Methods:**
- **initialize(brain)**: Initialize with brain reference
- **getLevel()**: Get curiosity level
- **getExplorationDrive()**: Get exploration drive
- **update(novelty, predictionError, dt)**: Update curiosity level
- **setNoveltyWeight(weight)**: Set novelty weight
- **setPredictionErrorWeight(weight)**: Set prediction error weight
- **reset()**: Reset curiosity level

#### Novelty

**Python API Methods:**
- **initialization methods**: Basic initialization for novelty detection

#### PredictionError

**Python API Methods:**
- **initialize(brain)**: Initialize with brain reference
- **getError()**: Get error value
- **computeError(predicted, actual)**: Compute prediction error
- **updatePrediction(newPrediction)**: Update prediction
- **getHistory()**: Get error history
- **clearHistory()**: Clear error history
- **getMagnitude()**: Get error magnitude

### 6. Advanced Configuration

#### Configuration Validation and Schema Support

**Python API Methods:**
- **validateConfig(config, schemaJson)**: Validate configuration against JSON schema
- **validateConfigWithRules(config, rulesJson)**: Validate configuration against validation rules
- **getConfigSchema(config)**: Generate JSON schema from current configuration
- **mergeConfig(config, jsonConfig)**: Merge configuration from JSON string
- **diffConfig(config1, config2)**: Compare two configurations and return differences
- **exportConfig(config, format)**: Export configuration to different formats (json, yaml, ini)
- **importConfig(config, data, format)**: Import configuration from different formats
- **getConfigTypes(config)**: Get type information for all configuration keys
- **getConfigStatistics(config)**: Get configuration statistics (size, complexity, etc.)
- **getConfigUsagePatterns(config)**: Get usage patterns and statistics for configuration keys
- **isConfigComplete(config)**: Check if configuration has all required values
- **getMissingRequiredKeys(config)**: Get list of missing required configuration keys
- **getInvalidKeys(config)**: Get list of configuration keys with invalid values
- **validateDependencyChain(config)**: Validate configuration dependencies

### 7. Performance Monitoring

#### Brain Performance Metrics

**Python API Methods:**
- **getSynapticDensity()**: Get average synaptic density across all regions
- **getRegionalStats()**: Get statistics for all regions
- **getFiringStatistics()**: Get detailed firing statistics
- **getNeuralPopulationStats()**: Get statistics for neural populations
- **getCriticalityIndex()**: Get criticality index (measure of being at edge of chaos)
- **getNetworkEfficiency()**: Get global network efficiency
- **getSynchronizationMetrics()**: Get neural synchronization metrics
- **getControlFlowMetrics()**: Get control flow and information routing metrics
- **getInformationProcessingRate()**: Get information processing rate (bits per second)
- **getEnergyConsumption()**: Get total energy consumption
- **getSystemUptime()**: Get system uptime (simulation time since initialization)
- **getMemoryUsage()**: Get approximate memory usage in bytes
- **getMemoryUtilization()**: Get memory usage as percentage of capacity
- **getDevelopmentalPlasticity()**: Get developmental plasticity
- **getCriticalPeriodProgress()**: Get critical period progress
- **getInterRegionalConnectionCount()**: Get inter-regional connection count
- **getInterRegionalConnectivity()**: Get inter-regional connectivity
- **getClusteringCoefficient()**: Get clustering coefficient
- **getTemporalCoherence()**: Get temporal coherence
- **getBurstingRate()**: Get bursting rate
- **getSpikeTemporalPrecision()**: Get spike temporal precision

### 8. Region and Population Access

#### Neural Region and Population Access

**Python API Methods:**
- **getRegionPopulationCount(regionId)**: Get population count for a region
- **getTotalConnectionsFromNeuron(neuronId)**: Get total connections from a neuron
- **getTotalConnectionsToNeuron(neuronId)**: Get total connections to a neuron
- **getCircuitExcitability(regionId)**: Get circuit excitability for a region
- **getCircuitInhibition(regionId)**: Get circuit inhibition for a region
- **getDominantNeurons(regionId, count)**: Get dominant neurons in a region
- **getFeedForwardLoopCount()**: Get feed-forward loop count
- **getFeedbackLoopCount()**: Get feedback loop count
- **getRecurrentLoopCount()**: Get recurrent loop count
- **getLearningProgress()**: Get learning progress
- **getMemoryRetention()**: Get memory retention
- **getPredictionAccuracy()**: Get prediction accuracy
- **getSystemStability()**: Get system stability
- **getOscillationFrequency()**: Get oscillation frequency
- **getAlphaPower()**: Get alpha power
- **getTaskPerformanceScore()**: Get task performance score
- **getCognitiveLoad()**: Get cognitive load
- **getDecisionConfidence()**: Get decision confidence

## Data Flow

### Sensory Processing
```
World → SensoryPercept → AgentBrain::processSensoryInput()
                               ↓
                         Brain::receiveSensoryInput()
                               ↓
                         Sensory neurons receive current
                               ↓
                         LIF dynamics integrate input
```

### Action Selection
```
Motor neuron activity → AgentBrain::decodeMotorCommand()
                               ↓
                         Curiosity exploration check
                               ↓
                         MotorCommand selected
                               ↓
                         World::applyAction()
```

### Memory Formation
```
Brain step (spikes) → Working memory stores firing neurons
                               ↓
                         Every 10 steps: Episodic memory captures state
                               ↓
                         Every 100 steps: Replay of important episodes
                               ↓
                         Every 1000 steps: Consolidation
```

### Learning
```
Reward from environment → AgentBrain::applyRewardModulation()
                               ↓
                         Dopamine computed (prediction error)
                               ↓
                         Synapses with eligibility traces updated
                               ↓
                         STDP weights modulated by dopamine
```

## System Independence and Coupling

### Loose Coupling (can function somewhat independently):
- Working memory (stores current activity)
- Episodic memory (stores experiences)
- Prediction system (computes errors)
- Concept formation (organizes patterns)

### Tight Coupling (strongly interconnected):
- Neurons ↔ Synapses ↔ Plasticity
- Dopamine ↔ Eligibility traces ↔ Synaptic weights
- Novelty ↔ Curiosity ↔ Exploration
- Development ↔ Plasticity rates ↔ Structural plasticity

## Extension Points

### Where to add new mechanisms:

1. **New plasticity rules**: Add to `Brain::step()` after STDP/Hebbian
2. **New neuromodulators**: Add to `Brain::Impl` and update `Brain::step()`
3. **New memory types**: Add to `Brain::Impl` and update `Brain::step()`
4. **New cognition**: Add to `Brain::Impl` and connect to perception/action

### Key integration hooks:

1. `receiveSensoryInput()` - Entry point for perception
2. `produceAction()` - Exit point for action
3. `step()` - Main loop where all systems update
4. `applyRewardModulation()` - Where learning signals apply
5. `save()/load()` - State persistence

## Python Integration Notes

### Type Safety
All Python bindings are type-safe with proper conversion between C++ and Python types. The bindings use pybind11's type conversion mechanisms to ensure data integrity.

### Error Handling
The bindings include comprehensive error handling with:
- C++ exceptions translated to Python exceptions
- Validation checks for invalid inputs
- Graceful degradation when components are not available

### Performance Considerations
- Reference policies are used where appropriate to avoid unnecessary copying
- Pybind11's `return_value_policy::reference_internal` is used for internal objects
- Vector and map types are automatically converted with efficient copying

### Memory Management
- Shared pointers are used for objects that need to be shared between Python and C++
- Python object ownership is carefully managed to prevent double-free issues
- All temporary objects are properly cleaned up

### Documentation
Each Python binding includes comprehensive docstrings following the pybind11 documentation style, providing clear usage examples and parameter descriptions.
