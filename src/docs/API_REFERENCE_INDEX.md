# NLM API Reference Documentation

## Overview

The NLM (Neural Learning Machine) API provides a comprehensive interface for simulating and studying artificial brain development. This documentation covers all public classes, methods, and interfaces provided by the NLM framework.

## API Structure

The NLM API is organized into logical modules:

### Core Module
- [Types](./types) - Strong typing system for neurons, synapses, regions, populations
- [Config](./config) - Configuration management
- [Random](./random) - Deterministic random number generation

### Neural Computation Module
- [Brain](./brain) - Central coordinator and main interface
- [Neuron](./neuron) - Individual neuron implementation (LIF dynamics)
- [Synapse](./synapse) - Synaptic connections and plasticity
- [SpikeSystem](./spike-system) - Event-driven spike processing
- [NeuralPopulation](./population) - Groups of neurons with shared properties
- [NeuralRegion](./region) - Brain regions with multiple populations

### Plasticity Module
- [PlasticityRule](./plasticity/rule) - Abstract base for plasticity rules
- [STDP](./plasticity/stdp) - Spike-Timing-Dependent Plasticity
- [Hebbian](./plasticity/hebbian) - Hebbian learning rule
- [StructuralPlasticity](./plasticity/structural) - Synapse formation and removal

### Development Module
- [DevelopmentSystem](./development) - Orchestrates developmental processes
- [Synaptogenesis](./development/synaptogenesis) - New synapse formation
- [Pruning](./development/pruning) - Synapse elimination
- [Maturation](./development/maturation) - Adult property development

### Neuromodulation Module
- [NeuromodulationManager](./neuromodulation) - Central coordinator for neuromodulation
- [Dopamine](./neuromodulation/dopamine) - Reward and reinforcement signals
- [Novelty](./neuromodulation/novelty) - Novelty detection
- [Curiosity](./neuromodulation/curiosity) - Exploration motivation
- [PredictionError](./neuromodulation/prediction-error) - Prediction error signals

### Memory Module
- [NeuralWorkingMemory](./memory/working) - Transient active information
- [NeuralEpisodicMemory](./memory/episodic) - Experience storage
- [NeuralAssociativeMemory](./memory/associative) - Pattern associations

### Prediction Module
- [PredictionSystem](./prediction) - Temporal sequence learning
- [NeuralPrediction](./prediction/neural) - Prediction engine

### Cognitive Module
- [NeuralPlanner](./cognition/planner) - Action planning
- [ConceptFormation](./cognition/concept) - Pattern discovery
- [AttentionalSelection](./cognition/attention) - Competitive selection
- [SelfModel](./cognition/self-model) - Body schema and agency
- [SocialLearning](./cognition/social) - Observation and imitation

### Sensory/Motor Module
- [SensoryInput](./sensory) - Base class for sensory data
- [Vision](./sensory/vision) - Visual input processing
- [MotorSystem](./motor) - Neural-to-action conversion
- [Action](./motor/action) - Action representation

### Environment Module
- [Environment](./environment) - Abstract environment interface
- [Observation](./environment/observation) - Sensory observation wrapper
- [SimpleWorld](./environment/simple-world) - Simple 2D world implementation

### Agent Module
- [AgentBrain](./agent) - Interface between brain and environment
- [AgentBody](./agent/body) - Physical body representation
- [SensoryPercept](./agent/percept) - Sensory perception data

### Experiment Module
- [Experiment](./experiments) - Single experiment recording
- [ExperimentRunner](./experiments/runner) - Experiment execution

### Performance Module
- [CheckpointManager](./performance/checkpoint) - State persistence
- [RealTimeDebugSystem](./performance/debug) - Debugging and monitoring

## API Documentation by Module

### Core Module

#### Types

##### NeuronId
**Description**: Unique identifier for a neuron with index access

**Methods**:
- `NeuronId()` - Default constructor (creates ID 0)
- `NeuronId(uint64_t value)` - Constructor with explicit value
- `index()` - Returns the neuron index
- `operator==()` - Equality comparison
- `operator!=()` - Inequality comparison
- `operator<()` - Less-than comparison for sorting

**Usage Example**:
```cpp
NeuronId neuronId(42);
uint64_t index = neuronId.index(); // Returns 42
```

##### SynapseId
**Description**: Unique identifier for a synapse with index access

##### RegionId
**Description**: Unique identifier for a brain region

##### PopulationId
**Description**: Unique identifier for a neural population

##### NeuronType
**Description**: Enumeration of neuron types

**Values**:
- `Excitatory` - Excitatory neurons
- `Inhibitory` - Inhibitory neurons
- `Modulatory` - Neuromodulatory neurons
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons
- `Internal` - Internal neurons

##### SynapseType
**Description**: Enumeration of synapse types

**Values**:
- `Excitatory` - Excitatory synapses
- `Inhibitory` - Inhibitory synapses
- `Modulatory` - Neuromodulatory synapses
- `Electrical` - Electrical synapses
- `GapJunction` - Gap junctions

#### Config

##### Config Class
**Description**: Configuration management system for NLM

**Public Methods**:

- `Config()` - Default constructor
- `~Config()` - Destructor
- `loadFromFile(const std::string& filepath)` - Load configuration from JSON file
- `loadFromArgs(int argc, char** argv)` - Load configuration from command line
- `saveToFile(const std::string& filepath)` - Save configuration to file

**Template Methods**:
- `get<T>(const std::string& key)` - Get value of type T (returns optional)
- `getOr<T>(const std::string& key, const T& defaultValue)` - Get value or return default

**Set Methods**:
- `set(const std::string& key, const ConfigValue& value, ConfigSource source = Runtime)` - Set any type
- `set(const std::string& key, const std::string& value, ConfigSource source = Runtime)` - Set string
- `set(const std::string& key, int value, ConfigSource source = Runtime)` - Set integer
- `set(const std::string& key, double value, ConfigSource source = Runtime)` - Set floating point
- `set(const std::string& key, bool value, ConfigSource source = Runtime)` - Set boolean

**Query Methods**:
- `has(const std::string& key)` - Check if key exists
- `remove(const std::string& key)` - Remove key
- `getKeys()` - Get all keys
- `clear()` - Clear all configuration
- `summary()` - Get configuration summary string

**Usage Example**:
```cpp
Config config;
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp.enable", true);

if (config.has("brain.neuron_count")) {
    int neurons = config.getOr<int>("brain.neuron_count", 1000);
}
```

### Neural Computation Module

#### Brain
**Description**: The central coordinator of the neural system

**Factory Methods**:
- `createBasicBrain()` - Create a basic brain with default configuration
- `createLearningBrain()` - Create a brain optimized for learning
- `createDevelopmentBrain()` - Create a brain in development mode
- `createOptimizedBrain(size_t neuronCount)` - Create an optimized brain
- `createCustomBrain(const BrainConfig& config)` - Create with custom configuration

**Initialization Methods**:
- `initialize()` - Initialize brain with default config
- `initialize(const BrainConfig& config)` - Initialize with specific config
- `initializeWithValidation()` - Initialize with validation result

**High-Level Control**:
- `runSimulation()` - Run simulation with inputs and options
- `runTraining()` - Run training with data and options
- `runEvaluation()` - Run evaluation with inputs and options
- `runBatch()` - Run batch processing

**Lifecycle Management**:
- `resetBrain()` - Reset brain state
- `saveBrain()` - Save brain state to file
- `loadBrain()` - Load brain state from file

**Configuration Management**:
- `createDefaultConfig()` - Get default BrainConfig
- `createLearningConfig()` - Get learning-specific config
- `createDevelopmentConfig()` - Get development config
- `createOptimizedConfig()` - Get optimized config

**Runtime Configuration**:
- `setLearningRate()` - Set learning rate
- `setNeuronCount()` - Set neuron count
- `enableLearning()` - Enable/disable learning
- `enableDevelopment()` - Enable/disable development
- `enableCuriosity()` - Enable/disable curiosity
- `enableRewardModulation()` - Enable/disable reward modulation
- `setVerboseLogging()` - Enable/disable verbose logging

**Core Methods**:
- `step()` - Single simulation step
- `step()` - Simulation step with timestamp
- `receiveSensoryInput()` - Receive sensory input
- `injectCurrent()` - Inject current into neuron
- `injectCurrentToNeurons()` - Inject current to neurons by type
- `produceAction()` - Produce motor output

**Access Methods**:
- `getSpikeSystem()` - Get spike system pointer
- `getSTDP()` - Get STDP plasticity system
- `getHebbian()` - Get Hebbian plasticity system
- `getStructuralPlasticity()` - Get structural plasticity system

**Memory System Access**:
- `getWorkingMemory()` - Get working memory system
- `getEpisodicMemory()` - Get episodic memory system
- `getAssociativeMemory()` - Get associative memory system

**Prediction System Access**:
- `getPredictionSystem()` - Get prediction system

**Cognitive System Access**:
- `getPlanner()` - Get neural planner
- `getConceptFormation()` - Get concept formation system
- `getAttention()` - Get attentional selection

**Neuromodulation System Access**:
- `getNeuromodulationManager()` - Get neuromodulation manager

**Statistics**:
- `getExcitationInhibitionRatio()` - Get E/I balance
- `getTotalSpikeCount()` - Get total spike count
- `getPendingSpikeEventCount()` - Get pending spike events
- `produceAction()` - Produce motor output

**Memory Statistics**:
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getActiveNeuronCount()` - Get active neuron count
- `getFiringNeuronCount()` - Get firing neuron count
- `getAverageFiringRate()` - Get average firing rate

**Reward System**:
- `getTotalReward()` - Get current total reward

**Random Access**:
- `getRandomGenerator()` - Get random number generator

**Utility**:
- `logStatus()` - Log brain status
- `validateConfig()` - Validate configuration
- `validateRuntimeState()` - Validate runtime state

**Example Usage**:
```cpp
// Create and configure brain
auto brain = Brain::createLearningBrain();
brain->setLearningRate(0.1f);
brain->enableDevelopment(true);

// Initialize
brain->initialize();

// Run simulation
SimulationResult result = brain->runSimulation(inputs);
if (result.success) {
    std::cout << "Total reward: " << result.totalReward << std::endl;
}
```

### Neuromodulation Module

#### NeuromodulationManager
**Description**: Central coordinator for all neuromodulation systems

**Public Methods**:

- `initialize(Brain* brain)` - Initialize with brain reference
- `updateNeuromodulators(TimestepDuration dt)` - Update neuromodulators
- `applyNeuromodulationEffects()` - Apply effects to neural systems
- `applyPlasticityFactors()` - Apply to synaptic plasticity
- `processReward(float reward)` - Process reward signal
- `processPredictionError(float error)` - Process prediction error
- `reset()` - Reset systems
- `enableNeuromodulation(bool enable)` - Enable/disable neuromodulation
- `isNeuromodulationEnabled()` - Check if enabled

**Statistics**:
- `getStats()` - Get comprehensive neuromodulation statistics

### Memory Module

#### NeuralWorkingMemory
**Description**: Transient active information storage system

**Key Methods**:
- `update(const std::vector<float>& input)` - Update working memory
- `getActivePattern()` - Get current active pattern
- `getCapacity()` - Get memory capacity
- `reset()` - Reset memory

#### NeuralEpisodicMemory
**Description**: Experience storage for long-term memory

**Key Methods**:
- `storeExperience(const ExperienceRecord& experience)` - Store experience
- `retrieveExperience(const ExperienceId& id)` - Retrieve experience
- `replayMemory(const ReplayConfig& config)` - Replay stored memories
- `getMemoryStats()` - Get memory statistics

#### NeuralAssociativeMemory
**Description**: Pattern association system

**Key Methods**:
- `learnAssociation(const PatternPair& patternPair)` - Learn pattern association
- `recallPattern(const Pattern& cue)` - Recall pattern from cue
- `getAssociationStrength(const PatternPair& pair)` - Get association strength
- `getAssociatedPatterns(const Pattern& pattern)` - Get associated patterns

## Configuration Options

### Brain Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.region_count` | int | 1 | Number of brain regions |
| `brain.connection_probability` | float | 0.1 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |

### Plasticity Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.prediction_error.weight` | float | 1.0 | Prediction error weighting |

### Development Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.enable` | bool | true | Enable developmental stages |
| `development.critical_period` | bool | true | Enable critical period plasticity |
| `development.synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `development.pruning_rate` | float | 0.00001 | Rate of synapse removal |

## Configuration Templates

### Default Configuration
```cpp
BrainConfig config = BrainConfig::createDefault();
config.neuronCount = 1000;
config.regionCount = 1;
config.connectionProbability = 0.1f;
config.enableDevelopment = true;
config.enableLearning = true;
config.enableCuriosity = true;
```

### Learning Configuration
```cpp
BrainConfig config = BrainConfig::createLearning(0.01f);
config.neuronCount = 500;
config.stdpLtpWeight = 0.005f;
config.stdpLtdWeight = 0.006f;
config.curiosityStrength = 0.8f;
config.rewardInfluence = 1.2f;
```

### Development Configuration
```cpp
BrainConfig config = BrainConfig::createDevelopment();
config.neuronCount = 2000;
config.enableDevelopment = true;
config.developmentalAge = 0.0;
config.synaptogenesisRate = 0.0002;
config.pruningRate = 0.00002;
```

### Optimized Configuration
```cpp
BrainConfig config = BrainConfig::createOptimized(1000);
config.neuronCount = 1000;
config.connectionProbability = 0.05f;
config.maxSteps = 10000;
config.verboseLogging = false;
```

## Error Handling

The NLM API provides comprehensive error handling:

### BrainErrorCode Enumeration
- `Success` - Operation completed successfully
- `InvalidConfig` - Invalid configuration provided
- `InitializationFailed` - Brain initialization failed
- `SimulationError` - Simulation encountered an error
- `SaveLoadError` - File save/load operation failed
- `MemoryError` - Memory allocation failed
- `Timeout` - Operation timed out
- `PluginError` - Plugin-related error

### Error Handling Example
```cpp
Brain brain = Brain::createLearningBrain();
BrainError error = brain.initializeWithError(config);

if (error.code != BrainErrorCode::Success) {
    std::cout << "Initialization failed: " << error.message << std::endl;
    for (const auto& detail : error.details) {
        std::cout << "  - " << detail << std::endl;
    }
}
```

## Performance Considerations

### Memory Management
- Use smart pointers for automatic memory management
- Reuse buffers when possible
- Monitor memory usage during long simulations

### Simulation Performance
- Use larger timesteps for batch processing
- Consider SIMD optimizations for large networks
- Use checkpointing for long-running simulations

### Configuration Optimization
- Cache frequently accessed configuration values
- Use structured configuration objects for complex settings
- Validate configuration before use

## Thread Safety

The NLM API provides the following thread safety guarantees:

- `const` methods are thread-safe
- `Brain` object is not thread-safe for concurrent modification
- Use external synchronization for multi-threaded access

## Memory Model

NLM uses the following memory management approach:

- RAII for automatic resource cleanup
- Smart pointers for dynamic memory
- Move semantics for performance
- No global static state (except configuration)

## Platform Support

NLM is designed to be cross-platform:

- Supports Linux, macOS, Windows
- Uses standard C++20 features
- Compatible with major compilers (GCC, Clang, MSVC)
- Python bindings available via pybind11

## Licensing

NLM is licensed under the MIT License. See LICENSE.md for full details.

## Versioning

NLM follows semantic versioning:
- Major version: Breaking changes
- Minor version: New features, backward compatible
- Patch version: Bug fixes, backward compatible

## API Stability

The NLM API is considered stable for Phase 6:
- Core interfaces are final
- Extension points are well-defined
- Backward compatibility is maintained
- Deprecation warnings are provided for removed features

## See Also

- [Architecture Guide](ARCHITECTURE.md) - System architecture overview
- [Science Guide](SCIENCE.md) - Scientific background
- [Tutorials](tutorials/) - Step-by-step tutorials
- [Examples](examples/) - Complete working examples

## Index

This API reference includes the following modules:

- Core: Types, Config, Random
- Neural: Brain, Neuron, Synapse, SpikeSystem
- Plasticity: PlasticityRule, STDP, Hebbian, StructuralPlasticity
- Development: DevelopmentSystem, Synaptogenesis, Pruning, Maturation
- Neuromodulation: NeuromodulationManager, Dopamine, Novelty, Curiosity, PredictionError
- Memory: NeuralWorkingMemory, NeuralEpisodicMemory, NeuralAssociativeMemory
- Prediction: PredictionSystem, NeuralPrediction
- Cognition: NeuralPlanner, ConceptFormation, AttentionalSelection, SelfModel, SocialLearning
- Sensory: SensoryInput, Vision
- Motor: MotorSystem, Action
- Environment: Environment, Observation, SimpleWorld
- Agent: AgentBrain, AgentBody, SensoryPercept
- Experiment: Experiment, ExperimentRunner
- Performance: CheckpointManager, RealTimeDebugSystem