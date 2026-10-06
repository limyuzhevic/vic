# NLM Python Bindings - Complete API Reference

# This file contains the complete Python bindings for the NLM library.
# It provides comprehensive coverage of all NLM functionality with Pythonic interfaces.

# The bindings are organized into logical groups:
# 1. Core Classes - Fundamental neural components
# 2. Plasticity Rules - Learning mechanisms
# 3. Neuromodulation - Reward, curiosity, novelty
# 4. Memory Systems - Working, episodic, semantic memory
# 5. Development - Brain development and maturation
# 6. Cognition - Abstract thinking and planning
# 7. Sensory Systems - Vision, audio, internal signals
# 8. Motor Systems - Movement and actions
# 9. Prediction - Forward models and prediction
# 10. Performance - Optimizations and profiling
# 11. Experiments - Experiment framework
# 12. Convenience Functions - High-level APIs
# 13. Error Handling - Comprehensive exception hierarchy
# 14. Utility Functions - Helper functions

# This file serves as documentation and reference for the complete Python API.
# The actual implementation is in python/bindings.cpp.

## Complete API Reference

### Core Classes (Phase 2-3 Foundations)

#### 1. Neuron - Single neuron with LIF dynamics
- **Location**: brain/Neuron.hpp
- **Key Methods**:
  - `__init__(id)` - Create neuron with ID
  - `getId()`, `setType(type)` - Identity and type
  - `getMembranePotential()`, `setMembranePotential()` - State control
  - `stepLIF(currentTime, dt)` - LIF dynamics simulation
  - `receiveExcitatoryInput()`, `receiveInhibitoryInput()` - Synaptic inputs
  - `injectCurrent()` - External current injection
  - `recordSpike()` - Spike recording
  - `getSpikeHistory()`, `clearSpikeHistory()` - Spike history management
  - `addIncomingSynapse()`, `addOutgoingSynapse()` - Synapse management
  - `enablePlasticity(hebbian, stdp, rewardModulated)` - Plasticity control
  - `reset()`, `initializeRandom(rng)` - State management

#### 2. Synapse - Neural connections with plasticity
- **Location**: brain/Synapse.hpp
- **Key Methods**:
  - `__init__(id, source, destination)` - Create synapse
  - `getId()`, `getSourceNeuron()`, `getDestinationNeuron()` - Identity
  - `getWeight()`, `setWeight()`, `addToWeight()` - Weight control
  - `getDelay()`, `setDelay()` - Transmission delay
  - `getType()`, `setType()` - Synapse type
  - `recordPreSpike()`, `recordPostSpike()` - STDP timing
  - `getPreSpikeHistory()`, `getPostSpikeHistory()` - Spike history
  - `enablePlasticity()` - Plasticity rules
  - `step()` - Dynamics simulation
  - `reset()`, `initializeRandom()` - State management

#### 3. BrainRegion - Brain organization structure
- **Location**: brain/NeuralRegion.hpp
- **Key Methods**:
  - `getId()`, `setId()`, `getName()`, `setName()` - Identity
  - `getRegionType()`, `setRegionType()` - Regional classification
  - `addNeuron()`, `addSynapse()` - Component management
  - `getNeuron()`, `getSynapse()` - Individual component access
  - `getNeuronCount()`, `getSynapseCount()` - Statistics
  - `getAllNeurons()`, `getAllSynapses()` - Bulk operations
  - `getNeuronsByType()`, `getNeuronsInPopulation()` - Filtering
  - `getSynapsesToNeuron()`, `getSynapsesFromNeuron()` - Connectivity

#### 4. NeuralPopulation - Neuron groups with shared properties
- **Location**: brain/NeuralPopulation.hpp
- **Key Methods**:
  - `getId()`, `setId()`, `getName()`, `setName()` - Identity
  - `getRegionId()`, `setRegionId()` - Regional affiliation
  - `getPopulationType()`, `setPopulationType()` - Population classification
  - `addNeuron()`, `removeNeuron()` - Membership management
  - `getNeuron()`, `getNeuronCount()` - Access and statistics
  - `getAllNeurons()`, `getNeuronsByType()` - Bulk operations
  - `getActiveNeuronCount()`, `getAverageFiringRate()` - Dynamics

### Plasticity Rules

#### 5. PlasticityRule - Base class for plasticity
- **Location**: plasticity/PlasticityRule.hpp
- **Key Methods**:
  - `getType()`, `setType()` - Rule type identification
  - `computeChange()` - Compute weight changes
  - `apply(preNeuron, postNeuron, preSpikeTime, postSpikeTime)` - Apply learning

#### 6. Hebbian - Hebbian learning rule
- **Location**: plasticity/Hebbian.hpp
- **Key Methods**:
  - `__init__()` - Create Hebbian rule
  - `getLearningRate()`, `setLearningRate()` - Learning control
  - `computeChange(weight, preActive, postActive)` - Weight change computation

#### 7. STDP - Spike-Timing Dependent Plasticity
- **Location**: plasticity/STDP.hpp
- **Key Methods**:
  - `__init__()` - Create STDP rule
  - `getLTPWeight()`, `setLTPWeight()` - LTP parameter
  - `getLTDWeight()`, `setLTDWeight()` - LTD parameter
  - `getTau()`, `setTau()` - Time constant
  - `computeChange(preSpikeTime, postSpikeTime, currentWeight)` - STDP calculation

#### 8. StructuralPlasticity - Growth and pruning
- **Location**: plasticity/StructuralPlasticity.hpp
- **Key Methods**:
  - `__init__()` - Create structural plasticity rule
  - `getGrowthRate()`, `setGrowthRate()` - Synaptogenesis control
  - `getPruningRate()`, `setPruningRate()` - Pruning control
  - `shouldGrow()`, `shouldPrune()` - Connection modification decisions

### Neuromodulation

#### 9. Neuromodulator - Neuromodulatory signals
- **Location**: neuromodulation/Neuromodulator.hpp
- **Key Methods**:
  - `__init__()` - Create neuromodulator
  - `getType()`, `setType()` - Signal type
  - `getLevel()`, `setLevel()` - Signal strength
  - `getEffect()`, `setEffect()` - Biological effect

#### 10. Reward - Reward signals
- **Location**: neuromodulation/Reward.hpp
- **Key Methods**:
  - `__init__()` - Create reward system
  - `getPredictionError()` - Prediction error computation
  - `applyRewardModulation()` - Reward-based learning

#### 11. PredictionError - Prediction error signals
- **Location**: neuromodulation/PredictionError.hpp
- **Key Methods**:
  - `__init__()` - Create prediction error system
  - `getError()`, `setError()` - Error value
  - `getConfidence()`, `setConfidence()` - Confidence level

#### 12. Novelty - Novelty detection
- **Location**: neuromodulation/Novelty.hpp
- **Key Methods**:
  - `__init__()` - Create novelty detector
  - `getNovelty()`, `setNovelty()` - Novelty level
  - `getThreshold()`, `setThreshold()` - Detection threshold
  - `shouldDriveLearning()` - Learning decision

#### 13. Curiosity - Curiosity signals
- **Location**: neuromodulation/Curiosity.hpp
- **Key Methods**:
  - `__init__()` - Create curiosity system
  - `getCuriosityLevel()` - Current curiosity level
  - `getExplorationBias()` - Exploration tendency

### Memory Systems

#### 14. Memory - Base memory class
- **Location**: memory/Memory.hpp
- **Key Methods**:
  - `getCapacity()`, `setCapacity()` - Storage capacity
  - `getConsolidationRate()` - Memory consolidation
  - `store()` - Store information
  - `retrieve()` - Retrieve information
  - `update()` - Update stored information

#### 15. WorkingMemory - Short-term working memory
- **Location**: memory/WorkingMemory.hpp
- **Key Methods**:
  - `__init__()` - Create working memory
  - `getActiveItems()` - Get currently active items
  - `getItem(itemId)` - Retrieve specific item
  - `addItem()`, `removeItem()` - Item management
  - `updateItems()` - Update all items

#### 16. EpisodicMemory - Event-based episodic memory
- **Location**: memory/EpisodicMemory.hpp
- **Key Methods**:
  - `__init__()` - Create episodic memory
  - `recordEvent()` - Record events with context
  - `getEvents()` - Retrieve events by time
  - `getEventsByContext()` - Retrieve by contextual tags
  - `replayEvents()` - Replay memory sequences

#### 17. SemanticMemory - Knowledge-based semantic memory
- **Location**: memory/SemanticMemory.hpp
- **Key Methods**:
  - `__init__()` - Create semantic memory
  - `storeConcept()` - Store abstract concepts
  - `retrieveConcept()` - Retrieve concepts
  - `getRelatedConcepts()` - Find related knowledge
  - `getConceptNetwork()` - Get concept relationships

### Development

#### 18. DevelopmentSystem - Brain development system
- **Location**: development/DevelopmentSystem.hpp
- **Key Methods**:
  - `__init__()` - Create development system
  - `getDevelopmentalStage()` - Current stage
  - `setDevelopmentalStage()` - Stage transition
  - `getPlasticityRate()` - Learning rate control
  - `update()` - Development progression

#### 19. Synaptogenesis - Formation of synapses
- **Location**: development/Synaptogenesis.hpp
- **Key Methods**:
  - `__init__()` - Create synaptogenesis system
  - `getGrowthRate()` - Connection formation rate
  - `shouldCreateConnection()` - New connection decision
  - `createConnection()` - Establish new synapse

#### 20. Pruning - Elimination of synapses
- **Location**: development/Pruning.hpp
- **Key Methods**:
  - `__init__()` - Create pruning system
  - `getPruningRate()` - Synapse elimination rate
  - `shouldPruneConnection()` - Pruning decision
  - `pruneConnection()` - Remove synapse

#### 21. Maturation - Neural maturation
- **Location**: development/Maturation.hpp
- **Key Methods**:
  - `__init__()` - Create maturation system
  - `getMaturationRate()` - Maturation progression
  - `getCurrentCapacity()` - Current processing capacity
  - `updateMaturation()` - Update maturation state

### Cognition

#### 22. ConceptFormation - Abstract concept creation
- **Location**: cognition/ConceptFormation.hpp
- **Key Methods**:
  - `__init__()` - Create concept formation system
  - `processInput()` - Process sensory input
  - `getConcepts()` - Retrieve formed concepts
  - `getConceptAssociations()` - Get concept relationships

#### 23. NeuralPlanner - Neural-based planning
- **Location**: cognition/NeuralPlanner.hpp
- **Key Methods**:
  - `__init__()` - Create neural planner
  - `planAction()` - Generate action sequences
  - `evaluateActionSequence()` - Assess action quality
  - `getActionPreferences()` - Get action tendencies

### Sensory Systems

#### 24. Vision - Vision sensory input
- **Location**: sensory/Vision.hpp
- **Key Methods**:
  - `__init__(width, height, channels)` - Create vision system
  - `setData()` - Set visual input data
  - `getWidth()`, `getHeight()`, `getChannels()` - Vision dimensions
  - `getPixel()` - Individual pixel access
  - `getIntensityMap()` - Get intensity representation

#### 25. Audio - Audio sensory input
- **Location**: sensory/Audio.hpp
- **Key Methods**:
  - `__init__(sampleRate, numSamples)` - Create audio system
  - `setData()` - Set audio input data
  - `getSampleRate()`, `getNumSamples()` - Audio specifications
  - `getAmplitude()` - Get amplitude information

#### 26. InternalSignals - Internal state signals
- **Location**: sensory/InternalSignals.hpp
- **Key Methods**:
  - `__init__()` - Create internal signals system
  - `addSignal()` - Add internal signal
  - `getSignal()` - Retrieve specific signal
  - `getAllSignals()` - Get all internal signals

### Motor Systems

#### 27. MotorSystem - Motor system control
- **Location**: motor/MotorSystem.hpp
- **Key Methods**:
  - `__init__()` - Create motor system
  - `getMotorCommand()` - Generate motor commands
  - `setMotorCommand()` - Set motor commands
  - `getMotorOutput()` - Get motor output

#### 28. Action - Motor actions
- **Location**: motor/Action.hpp
- **Key Methods**:
  - `__init__(type)` - Create action
  - `__init__(type, parameters)` - Create action with parameters
  - `getType()`, `setType()` - Action type
  - `getParameters()`, `setParameters()` - Action parameters
  - `getName()` - Get action name
  - `clone()` - Create action copy

### Prediction

#### 29. PredictionSystem - Prediction system
- **Location**: prediction/PredictionSystem.hpp
- **Key Methods**:
  - `__init__()` - Create prediction system
  - `predict()` - Generate predictions
  - `getPredictionError()` - Compute prediction errors
  - `update()` - Update prediction model

#### 30. NeuralPrediction - Neural prediction
- **Location**: prediction/NeuralPrediction.hpp
- **Key Methods**:
  - `__init__()` - Create neural prediction
  - `getPrediction()` - Get prediction value
  - `setPrediction()` - Set prediction
  - `getConfidence()` - Get prediction confidence

### Performance

#### 31. OptimizedBrain - Optimized brain implementation
- **Location**: performance/OptimizedBrain.hpp
- **Key Methods**:
  - `__init__()` - Create optimized brain
  - `optimize()` - Optimize brain performance
  - `getPerformanceMetrics()` - Get performance statistics

#### 32. SIMDVectorization - SIMD optimizations
- **Location**: performance/SIMDVectorization.hpp
- **Key Methods**:
  - `__init__()` - Create SIMD vectorization
  - `processVector()` - Process data with SIMD
  - `getSIMDOptimization()` - Get optimization statistics

#### 33. MemoryPool - Memory management optimizations
- **Location**: performance/MemoryPool.hpp
- **Key Methods**:
  - `__init__()` - Create memory pool
  - `allocate()` - Allocate memory
  - `deallocate()` - Deallocate memory
  - `getUsageStats()` - Get memory usage statistics

#### 34. EventQueue - Event processing optimizations
- **Location**: performance/EventQueue.hpp
- **Key Methods**:
  - `__init__()` - Create event queue
  - `pushEvent()` - Add event to queue
  - `processEvents()` - Process queued events
  - `getEventStats()` - Get event statistics

#### 35. SparseConnectivity - Sparse connectivity optimizations
- **Location**: performance/SparseConnectivity.hpp
- **Key Methods**:
  - `__init__()` - Create sparse connectivity
  - `addConnection()` - Add sparse connection
  - `removeConnection()` - Remove sparse connection
  - `getConnectionStats()` - Get connection statistics

#### 36. ParallelProcessing - Parallel processing optimizations
- **Location**: performance/ParallelProcessing.hpp
- **Key Methods**:
  - `__init__()` - Create parallel processor
  - `processInParallel()` - Process data in parallel
  - `getParallelStats()` - Get parallel processing statistics

### Experiments

#### 37. Experiment - Base experiment class
- **Location**: experiments/Experiment.hpp
- **Key Methods**:
  - `__init__()` - Create experiment
  - `run()` - Run experiment
  - `getResults()` - Get experiment results
  - `getMetrics()` - Get performance metrics

#### 38. Phase3Experiment - Phase 3 experiments
- **Location**: experiments/Phase3Experiment.hpp
- **Key Methods**:
  - `__init__()` - Create Phase 3 experiment
  - `runBasicConnectivityTest()` - Test neural connectivity
  - `runPlasticityExperiment()` - Test plasticity learning
  - `runStdpVerification()` - Verify STDP mechanism

#### 39. Phase4Experiment - Phase 4 experiments
- **Location**: experiments/Phase4Experiment.hpp
- **Key Methods**:
  - `__init__()` - Create Phase 4 experiment
  - `runPredictionTest()` - Test prediction capabilities
  - `runCognitionTest()` - Test cognitive functions
  - `runMemoryTest()` - Test memory systems

#### 40. Phase5Experiment - Phase 5 experiments
- **Location**: experiments/Phase5Experiment.hpp
- **Key Methods**:
  - `__init__()` - Create Phase 5 experiment
  - `runPerformanceTest()` - Test performance optimizations
  - `runScalabilityTest()` - Test scalability
  - `runEfficiencyTest()` - Test efficiency

#### 41. Phase6IntegratedExperiment - Phase 6 integrated experiment
- **Location**: experiments/Phase6IntegratedExperiment.hpp
- **Key Methods**:
  - `__init__()` - Create Phase 6 experiment
  - `runIntegrationTest()` - Test system integration
  - `runAllPhasesTest()` - Run all phase tests
  - `getIntegrationResults()` - Get integration results

#### 42. AblationSystem - Ablation studies
- **Location**: experiments/AblationSystem.hpp
- **Key Methods**:
  - `__init__()` - Create ablation system
  - `ablateComponent()` - Remove specific component
  - `restoreComponent()` - Restore ablated component
  - `getAblationResults()` - Get ablation study results

#### 43. ScalingBenchmark - Scaling benchmarks
- **Location**: experiments/ScalingBenchmark.hpp
- **Key Methods**:
  - `__init__()` - Create scaling benchmark
  - `runScalingTest()` - Test with different network sizes
  - `getScalingResults()` - Get scaling performance

#### 44. Metrics - Performance metrics
- **Location**: experiments/Metrics.hpp
- **Key Methods**:
  - `__init__()` - Create metrics system
  - `recordMetric()` - Record performance metric
  - `getMetric()` - Retrieve specific metric
  - `getAllMetrics()` - Get all metrics

#### 45. ExperimentRunner - Experiment runner
- **Location**: experiments/ExperimentRunner.hpp
- **Key Methods**:
  - `__init__()` - Create experiment runner
  - `runExperiment()` - Run specific experiment
  - `runAllExperiments()` - Run all experiments
  - `getExperimentResults()` - Get experiment results

### Convenience Functions

#### 46. setupDefaultNetwork - Setup function for default network
```python
def setupDefaultNetwork(neuronCount=1000, connectionProbability=0.05):
    """Setup a default neural network configuration."""
```

#### 47. setupDevelopmentNetwork - Setup function for development network
```python
def setupDevelopmentNetwork(neuronCount=2000, developmentEnabled=True):
    """Setup a development-focused neural network."""
```

#### 48. setupLearningNetwork - Setup function for learning network
```python
def setupLearningNetwork(neuronCount=500, learningEnabled=True):
    """Setup a learning-focused neural network."""
```

#### 49. getVersion - Get library version
```python
def get_version():
    """Get the version of NLM Python bindings."""
```

#### 50. getBuildInfo - Get build information
```python
def get_build_info():
    """Get build information."""
```

#### 51. getSupportedFeatures - Get supported features
```python
def get_supported_features():
    """Get list of supported features."""
```

### Error Handling

#### 52. NLMException - Base exception class
```python
class NLMException(Exception):
    """Base exception for NLM errors."""
```

#### 53-64. Specific exception classes
- `ConfigException`, `BrainException`, `WorldException`, etc.
- Each inherits from `NLMException`
- Provide specific error information

### Utility Functions

#### 65. randomFloat - Generate random float
```python
def random_float(min_val=0.0, max_val=1.0, seed=None):
    """Generate random float in range [min_val, max_val]."""
```

#### 66. randomInt - Generate random integer
```python
def random_int(min_val=0, max_val=100, seed=None):
    """Generate random integer in range [min_val, max_val]."""
```

#### 67. clamp - Clamp value
```python
def clamp(value, min_val, max_val):
    """Clamp value to [min_val, max_val] range."""
```

#### 68. normalize - Normalize value
```python
def normalize(value, min_val, max_val):
    """Normalize value to [0, 1] range."""
```

#### 69. currentTime - Get current simulation time
```python
def current_time():
    """Get current simulation time in milliseconds."""
```

#### 70. getSystemInfo - Get system information
```python
def get_system_info():
    """Get system information including version, available memory, etc."""
```

#### 71. getMemoryUsage - Get memory usage
```python
def get_memory_usage():
    """Get current memory usage in MB."""
```

#### 72. getPerformanceMetrics - Get performance metrics
```python
def get_performance_metrics():
    """Get performance metrics including processing speed, memory usage, etc."""
```

## API Usage Examples

### Basic Usage
```python
import pynlm

# Create a basic brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create a world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Advanced Usage with New Classes
```python
import pynlm
import numpy as np

# Create a development network
config = pynlm.createDevelopmentConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create a neuron for direct manipulation
neuron = pynlm.Neuron(pynlm.NeuronId(1))
neuron.setType(pynlm.NeuronType.Excitatory)
neuron.setMembranePotential(-65.0f)
neuron.setThreshold(-50.0f)

# Inject current and run LIF dynamics
neuron.injectCurrent(10.0f)
fired = neuron.stepLIF(0.0, 0.001)
if fired:
    print("Neuron fired!")

# Create a synapse
synapse = pynlm.Synapse(pynlm.SynapseId(1), neuron.getId(), pynlm.NeuronId(2))
synapse.setWeight(0.5f)
synapse.setType(pynlm.SynapseType.Excitatory)

# Enable plasticity
synapse.enablePlasticity(True, True, False)

# Create a brain region
region = pynlm.BrainRegion()
region.setName("Cortex")
region.setRegionType("Association")
region.addNeuron(neuron)
region.addSynapse(synapse)

print(f"Region: {region}")
print(f"Neurons: {region.getNeuronCount()}")
print(f"Synapses: {region.getSynapseCount()}")
```

### Learning with Neuromodulation
```python
import pynlm

# Create a learning network
config = pynlm.createLearningConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run learning simulation
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward and apply learning
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
    
    if step % 100 == 0:
        print(f"Step {step}: Curiosity={agent.getCuriosityLevel():.3f}, Novelty={agent.getNoveltyLevel():.3f}")
```

## API Design Principles

1. **Pythonic Interface**: Where possible, provide Pythonic interfaces (property access, method chaining)
2. **Comprehensive Documentation**: Extensive docstrings with examples
3. **Error Handling**: Comprehensive exception hierarchy with specific error types
4. **Performance**: Optimized bindings that maintain C++ performance
5. **Backward Compatibility**: Maintain compatibility with existing API
6. **Extensibility**: Framework for future enhancements
7. **Testing**: Comprehensive testing suite for all bindings
8. **Documentation**: Complete documentation with examples

## Files to Modify

1. **python/bindings.cpp** - Add all new bindings
2. **python/CMakeLists.txt** - Update if necessary
3. **tests/python/** - Create Python test files
4. **docs/** - Update documentation
5. **examples/** - Add example scripts

## Expected Benefits

1. **Completeness**: All C++ functionality exposed in Python
2. **Usability**: Pythonic API that's easy to use and understand
3. **Performance**: Optimized bindings that maintain performance
4. **Documentation**: Comprehensive documentation and examples
5. **Testing**: Comprehensive testing suite for Python bindings
6. **Extensibility**: Framework for future enhancements
7. **Research**: Advanced features for research applications
8. **Education**: Educational examples and tutorials

## Timeline

The implementation will be done in phases:
1. Phase 1: Add core missing classes (Neuron, Synapse, BrainRegion, NeuralPopulation)
2. Phase 2: Add additional types and utility functions
3. Phase 3: Add error handling and exception classes
4. Phase 4: Add convenience functions and setup helpers
5. Phase 5: Add advanced features and research capabilities
6. Phase 6: Implement testing and validation
7. Phase 7: Add documentation and examples

This complete API reference provides a comprehensive overview of the NLM Python bindings.
It shows the full scope of functionality that will be available once all bindings are implemented.
