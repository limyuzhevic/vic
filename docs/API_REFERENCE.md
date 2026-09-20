# NLM Advanced API Reference

This document provides comprehensive API documentation for the NLM (Neural Learning Machine) system, including both basic and advanced features.

## Table of Contents

1. [Core Brain API](#core-brain-api)
2. [Memory Systems API](#memory-systems-api)
3. [Prediction System API](#prediction-system-api)
4. [Neuromodulation API](#neuromodulation-api)
5. [Cognition Systems API](#cognition-systems-api)
6. [Development API](#development-api)
7. [Advanced Features](#advanced-features)
8. [Performance Optimization](#performance-optimization)

## Core Brain API

### Brain Class

The `Brain` class is the central coordinator of the NLM system. It manages neural computation, plasticity, memory, and all other integrated systems.

#### Key Methods

```cpp
// Creation and Initialization
Brain(std::shared_ptr<Config> config);
bool initialize();

// Main Simulation Loop
void step(SimulationStep step);
void step(SimulationStep step, Timestamp time);
void reset();

// Neural Input/Output
void receiveSensoryInput(const SensoryInput& input);
void injectCurrent(NeuronId neuron, MembranePotential current);
std::unique_ptr<Action> produceAction();

// Memory System Access
NeuralWorkingMemory* getWorkingMemory();
NeuralEpisodicMemory* getEpisodicMemory();
NeuralAssociativeMemory* getAssociativeMemory();

// Plasticity Systems Access
STDP* getSTDP();
Hebbian* getHebbian();
StructuralPlasticity* getStructuralPlasticity();

// Statistics and Monitoring
float getExcitationInhibitionRatio() const;
size_t getTotalSpikeCount() const;
size_t getPendingSpikeEventCount() const;
float getAverageFiringRate() const;

// State Management
bool save(const std::string& filepath) const;
bool load(const std::string& filepath);

// Development
DevelopmentalStage getDevelopmentalStage() const;
void setDevelopmentalStage(DevelopmentalStage stage);
void develop();

// Neuromodulation
void applyNeuromodulation(const Neuromodulator& signal);
```

### Config Class

Configuration management for NLM parameters.

```cpp
// Creation and Access
Config();
std::optional<T> get(const std::string& key) const;
template<typename T> T getOr(const std::string& key, const T& defaultValue) const;

// Modification
set(const std::string& key, const std::string& value);
set(const std::string& key, int value);
set(const std::string& key, double value);
set(const std::string& key, bool value);

// File I/O
bool loadFromFile(const std::string& filepath);
bool loadFromArgs(int argc, char** argv);
bool saveToFile(const std::string& filepath) const;
```

## Memory Systems API

### NeuralWorkingMemory

Transient memory for active information with competitive dynamics.

```cpp
// Core Operations
void initialize(Brain* brain);
void setCapacity(size_t capacity);
void storeToNeuron(NeuronId neuronId, float activation);
void update(TimestepDuration dt);

// Access and Queries
size_t getActiveTraces() const;
const std::vector<NeuronId>& getMemoryNeurons() const;
float getTraceActivation(NeuronId neuronId) const;
```

### NeuralEpisodicMemory

Experience storage with encoding, storage, and replay capabilities.

```cpp
// Core Operations
void initialize(Brain* brain);
void setMaxEpisodes(size_t maxEpisodes);
void storeEpisode(const EpisodicMemoryItem& episode);

// Memory Management
void storeEpisode(const EpisodicMemoryItem& episode);
const EpisodicMemoryItem* getEpisode(size_t index) const;
size_t getEpisodeCount() const;

// Replay and Consolidation
void replayEpisode(const EpisodicMemoryItem* episode);
void consolidate(float threshold);
void clear();
```

### NeuralAssociativeMemory

Pattern association through Hebbian learning.

```cpp
// Core Operations
void initialize(Brain* brain);

// Association Management
void associate(const Pattern& pattern1, const Pattern& pattern2, float strength);
float getAssociation(const Pattern& pattern1, const Pattern& pattern2) const;
void clearAssociations();
```

## Prediction System API

### PredictionSystem

Temporal sequence prediction and prediction error computation.

```cpp
// Core Operations
void updateWithSensoryInput(SimulationStep step, Timestamp time, const std::vector<float>& input);
void updateWithAction(SimulationStep step, Timestamp time, const Action& action);

// Prediction Access
float getPredictionError() const;
float getPredictionConfidence() const;
const std::vector<float>& getPredictedSequence() const;

// Training
void train(const std::vector<float>& input, const std::vector<float>& target);
void reset();
```

## Neuromodulation API

### Dopamine

Reward prediction error and reinforcement signaling.

```cpp
// Core Operations
void initialize(Brain* brain);
void update(TimestepDuration dt);

// Access
float getLevel() const;
float getPlasticityFactor() const;

// Configuration
setLevel(float level);
```

### Curiosity

Exploration motivation based on novelty and prediction error.

```cpp
// Core Operations
void initialize(Brain* brain);
void update(TimestepDuration dt);

// Access
float getLevel() const;
float getExplorationBonus() const;

// Configuration
setNoveltyWeight(float weight);
setErrorWeight(float weight);
```

### Novelty Detection

Sensory change detection for surprise signals.

```cpp
// Core Operations
void initialize(Brain* brain);
void update(TimestepDuration dt);

// Core Methods
void updateWithInput(const std::vector<float>& input);

// Access
float getLevel() const;
float getAdaptiveWeight() const;
```

## Cognition Systems API

### NeuralPlanner

Action planning with sequence evaluation and hierarchical decision making.

```cpp
// Core Operations
void initialize(Brain* brain);
void setPlanningDepth(size_t depth);

// Planning Methods
action_sequence planAction(const Percept& context);
float evaluateActionSequence(const action_sequence& sequence);

// Access
const action_sequence& getCurrentPlan() const;
```

### ConceptFormation

Pattern discovery without predefined labels.

```cpp
// Core Operations
void initialize(Brain* brain);

// Concept Methods
void processInput(const std::vector<float>& input);
ConceptId discoverConcept(const std::vector<float>& pattern);
float getConceptStrength(ConceptId conceptId) const;

// Access
const Concept& getConcept(ConceptId conceptId) const;
```

## Development API

### DevelopmentSystem

Orchestrates developmental stages and plasticity modulation.

```cpp
// Core Operations
developmentSystem();

// Stage Management
DevelopmentalStage getStage() const;
void setStage(DevelopmentalStage stage);
void advanceStage();

// Development Updates
void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);

// Stage-Specific Properties
float getPlasticityModifier() const;
bool isCriticalPeriod() const;
float getCriticalPeriodProgress() const;
```

## Advanced Features

### 1. Custom Neural Dynamics

Implement custom neuron dynamics by extending `NeuralDynamics`.

```cpp
class CustomDynamics : public NeuralDynamics {
public:
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override {
        // Custom dynamics implementation
    }
    
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override {
        // Custom synaptic dynamics
    }
};
```

### 2. Custom Plasticity Rules

Create custom learning rules by extending `PlasticityRule`.

```cpp
class CustomPlasticity : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        // Custom plasticity implementation
    }
    
    const char* getName() const override {
        return "CustomPlasticity";
    }
};
```

### 3. Memory Integration

Integrate custom memory systems with the brain.

```cpp
class CustomMemorySystem {
public:
    virtual void initialize(Brain* brain) = 0;
    virtual void update(TimestepDuration dt) = 0;
    virtual void storeExperience(const Experience& exp) = 0;
    virtual float retrieve(const Query& query) const = 0;
};
```

## Performance Optimization

### 1. Memory Pool Usage

Reduce memory allocation overhead:

```cpp
// Enable memory pools
MemoryPoolManager::enable(true);
MemoryPoolManager::setPoolSize("neuron_state", 1024);
MemoryPoolManager::setPoolSize("synapse", 4096);
```

### 2. Event-Driven Processing

Use event-driven spike processing:

```cpp
// Configure spike system
brain->getSpikeSystem()->enableEventDriven(true);
brain->getSpikeSystem()->setMaxEvents(100000);
```

### 3. SIMD Vectorization

Enable SIMD optimizations:

```cpp
// Enable SIMD optimizations
VectorOptimization::enableSIMD(true);
VectorOptimization::preferAVX2(true);
```

## Error Handling

NLM uses a combination of return codes and exceptions for error handling:

### Return Codes

- `true`/`false` for success/failure (initialization, file operations)
- `0` for success, non-zero for errors (main function)

### Exceptions

- `std::invalid_argument` for invalid parameters
- `std::runtime_error` for runtime errors
- `std::bad_alloc` for memory allocation failures

## Thread Safety

NLM is not thread-safe for concurrent access to the same brain instance. For multi-threaded applications:

1. Use separate brain instances per thread
2. Implement proper synchronization when sharing brain data
3. Use the thread-local storage features for per-thread optimization

## Configuration Examples

### Minimal Configuration

```cpp
Config config;
config.set("brain.neuron_count", 1000);
config.set("brain.connection_probability", 0.1f);
config.set("plasticity.stdp.enable", true);
config.set("neuromod.dopamine.scale", 1.0f);
```

### Advanced Configuration

```cpp
Config config;
config.set("brain.neuron_count", 10000);
config.set("brain.synapse_density", 0.2f);
config.set("plasticity.stdp.learning_rate", 0.001f);
config.set("plasticity.hebbian.enable", true);
config.set("plasticity.structural.enable", true);
config.set("neuromod.dopamine.scale", 1.5f);
config.set("neuromod.curiosity.enable", true);
config.set("neuromod.novelty.enable", true);
config.set("development.enable", true);
config.set("checkpoint.dir", "./checkpoints");
```

## Migration Guide

### From Phase 5 to Phase 6

1. **Memory Integration**: Replace `nullptr` returns with actual memory system instances
2. **Prediction System**: Connect prediction system to sensory input processing
3. **Cognition Integration**: Instantiate and use neural planner, concept formation, and attention systems
4. **Development Effects**: Implement full developmental stage effects beyond plasticity rates
5. **Checkpointing**: Replace stub implementations with full CheckpointSystem integration

### Upgrading to Phase 7

1. **Enhanced Neuromodulation**: Implement full ACh, NE, 5-HT neuromodulators
2. **Sleep/Consolidation**: Implement memory consolidation during rest periods
3. **Replay Mechanisms**: Implement systematic episodic memory replay
4. **Advanced Learning**: Implement meta-learning and continual learning capabilities
5. **Self-Awareness**: Implement self-model development and self-monitoring

## API Stability

This API is stable and follows semantic versioning:

- **Major version changes**: Breaking changes in API design
- **Minor version changes**: New features, bug fixes
- **Patch version changes**: Bug fixes, performance improvements

## Future API Plans

### Phase 7: Enhanced Integration

- Full neuromodulation system with cholinergic, noradrenergic, and serotonergic effects
- Sleep/rest cycles with memory consolidation
- Advanced learning mechanisms including meta-learning
- Self-awareness and metacognition capabilities

### Phase 8: Scalability

- Distributed brain computing
- GPU-accelerated neural dynamics
- Cloud-based brain deployment
- Real-time learning with streaming data

## Conclusion

The NLM API provides a comprehensive framework for building artificial brain systems with biological inspiration. The Phase 6 implementation represents a significant step forward in integrating memory, prediction, cognition, and neuromodulation into a coherent artificial brain system.

The API is designed to be both accessible for beginners and extensible for advanced users, with clear separation of concerns and well-defined interfaces for all major components.
