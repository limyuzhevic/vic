# NLM (熙然) - API Reference Documentation

## Overview

This document provides comprehensive API documentation for the NLM (Neural Learning Machine) project. It documents all public classes, functions, and interfaces provided by the NLM system.

## Core Types

### NeuronId
```cpp
struct NeuronId {
    uint64_t value;
    
    constexpr NeuronId() : value(0) {}
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const NeuronId& other) const = default;
    constexpr bool operator!=(const NeuronId& other) const = default;
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    constexpr NeuronId& operator++();
    constexpr NeuronId operator++(int);
    
    constexpr uint64_t index() const { return value; }
};
```

**Description**: Strong identifier for neurons with comparison and arithmetic operators.

**Usage**:
```cpp
NeuronId id1 = NeuronId(1);
NeuronId id2 = NeuronId(42);
bool equal = (id1 == id2); // false
id1++; // post-increment
```

### SynapseId
```cpp
struct SynapseId {
    uint64_t value;
    
    constexpr SynapseId() : value(0) {}
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const SynapseId& other) const = default;
    constexpr bool operator!=(const SynapseId& other) const = default;
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    constexpr SynapseId& operator++();
    constexpr SynapseId operator++(int);
    
    constexpr uint64_t index() const { return value; }
};
```

### RegionId
```cpp
struct RegionId {
    uint64_t value;
    
    constexpr RegionId() : value(0) {}
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const RegionId& other) const = default;
    constexpr bool operator!=(const RegionId& other) const = default;
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    constexpr RegionId& operator++();
    constexpr RegionId operator++(int);
    
    constexpr uint64_t index() const { return value; }
};
```

### PopulationId
```cpp
struct PopulationId {
    uint64_t value;
    
    constexpr PopulationId() : value(0) {}
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const PopulationId& other) const = default;
    constexpr bool operator!=(const PopulationId& other) const = default;
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    constexpr PopulationId& operator++();
    constexpr PopulationId operator++(int);
    
    constexpr uint64_t index() const { return value; }
};
```

### Enumerations

#### NeuronType
```cpp
enum class NeuronType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Sensory,
    Motor,
    Internal
};
```

**Values**:
- `NeuronType::Excitatory` - Standard excitatory neuron
- `NeuronType::Inhibitory` - Standard inhibitory neuron  
- `NeuronType::Modulatory` - Neuromodulatory neuron
- `NeuronType::Sensory` - Sensory processing neuron
- `NeuronType::Motor` - Motor output neuron
- `NeuronType::Internal` - Internal computation neuron

**Usage**:
```cpp
NeuronType neuronType = NeuronType::Sensory;
if (neuronType == NeuronType::Excitatory) {
    // Handle excitatory neurons
}
```

#### SynapseType
```cpp
enum class SynapseType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Electrical,
    GapJunction
};
```

**Values**:
- `SynapseType::Excitatory` - Standard chemical excitatory synapse
- `SynapseType::Inhibitory` - Standard chemical inhibitory synapse
- `SynapseType::Modulatory` - Neuromodulatory synapse
- `SynapseType::Electrical` - Electrical coupling synapse
- `SynapseType::GapJunction` - Direct electrical synapse

#### FiringState
```cpp
enum class FiringState : uint8_t {
    Resting,
    Active,
    Refractory,
    Inhibited
};
```

**Values**:
- `FiringState::Resting` - Neuron at resting potential
- `FiringState::Active` - Neuron currently firing action potential
- `FiringState::Refractory` - Neuron in refractory period after spike
- `FiringState::Inhibited` - Neuron actively inhibited

#### DevelopmentalStage
```cpp
enum class DevelopmentalStage : uint8_t {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging
};
```

**Values**:
- `DevelopmentalStage::Initial` - Initial developmental stage with high plasticity
- `DevelopmentalStage::CriticalPeriod` - Sensitive period for learning
- `DevelopmentalStage::Maturation` - Ongoing development
- `DevelopmentalStage::Adult` - Fully developed with stable circuits
- `DevelopmentalStage::Aging` - Age-related changes in neural dynamics

#### PlasticityFlags
```cpp
struct PlasticityFlags {
    bool hebbian : 1;
    bool stdp : 1;
    bool reward_modulated : 1;
    bool structural : 1;
    bool eligible : 1;
    
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
};
```

**Description**: Flags indicating which plasticity rules apply to a synapse.

**Usage**:
```cpp
PlasticityFlags flags;
flags.hebbian = true;           // Enable Hebbian learning
flags.stdp = true;              // Enable spike-timing-dependent plasticity
flags.reward_modulated = false; // Disable reward-modulated plasticity
```

### Type Aliases

#### Core Types
```cpp
using SimulationStep = uint64_t;      // Discrete simulation steps
using Timestamp = double;             // Continuous time in seconds
using TimestepDuration = double;      // Duration of each timestep
using NeuronIndex = uint64_t;         // Index into neuron array
using SynapseIndex = uint64_t;        // Index into synapse array
using PopulationIndex = uint64_t;     // Index into population array
using RegionIndex = uint64_t;         // Index into region array
```

#### Value Types
```cpp
using SynapticWeight = float;         // Synaptic connection strength (-1.0 to 1.0)
using MembranePotential = float;     // Membrane voltage (millivolts)
using FiringRate = float;             // Neuronal firing rate (Hertz)
using Delay = uint32_t;               // Synaptic transmission delay (steps)
```

### Invalid ID Sentinels
```cpp
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);
```

## Classes

### Brain
```cpp
class Brain {
public:
    explicit Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Core functionality
    bool initialize();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // I/O operations
    void receiveSensoryInput(const class SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Access methods
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Output and neuromodulation
    std::unique_ptr<class Action> produceAction();
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Plasticity and development
    void updatePlasticity();
    void develop();
    
    // Persistence
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connections
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Memory systems
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction system
    PredictionSystem* getPredictionSystem();
    
    // Cognition systems
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Development system
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Neuromodulation systems
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Configuration and state
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    
    // Learned preference (Phase 5)
    float getLearnedPreference() const;
    void setLearnedPreference(float preference);
    
    // Development and lifecycle
    void logStatus() const;
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};
```

**Description**: Central coordinator of the neural system implementing real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

**Constructors**:
```cpp
// Create brain with configuration
explicit Brain(std::shared_ptr<Config> config);
```

**Key Methods**:
- `initialize()` - Initialize brain with configuration
- `step()` - Main simulation step processing all systems
- `produceAction()` - Generate motor output based on neural activity
- `save()`/`load()` - Checkpoint/restore brain state
- `addRegion()` - Create a new neural region

**Notes**:
- Uses PImpl pattern for implementation hiding
- Thread-safe for concurrent access
- Supports moving semantics for efficient resource management
- Manages complete brain lifecycle from initialization to shutdown

### Neuron
```cpp
class Neuron {
public:
    explicit Neuron(NeuronId id);
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity
    NeuronId getId() const;
    
    // Type
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // State access
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // Membrane potential
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    // Firing state
    bool isFiring() const;
    bool isRefractory() const;
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void decrementRefractory();
    
    // Firing rate
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF parameters
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // Spike detection
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    // LIF step function
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // Spike history
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Synapse management
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // Simulation
    void step(Timestamp currentTime);
    
    // State management
    void reset();
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};
```

**Description**: Single neuron implementation using Leaky Integrate-and-Fire (LIF) dynamics with synaptic inputs, spike generation, and plasticity mechanisms.

**Constructors**:
```cpp
explicit Neuron(NeuronId id);  // Create neuron with unique ID
```

**Key Methods**:
- `stepLIF()` - Implements LIF dynamics, returns true if neuron fired
- `recordSpike()` - Records spike timestamp for plasticity
- `initializeRandom()` - Sets biological parameters with random variation
- `checkThreshold()` - Determines if neuron should fire

**Notes**:
- Thread-safe implementation
- Supports both event-driven and step-based computation
- Memory-efficient with PImpl pattern
- Biological parameter ranges based on empirical data

### Synapse
```cpp
class Synapse {
public:
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity
    SynapseId getId() const;
    
    // Connection
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    
    // Weight
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    
    // Delay
    Delay getDelay() const;
    void setDelay(Delay delay);
    
    // Type
    SynapseType getType() const;
    void setType(SynapseType type);
    
    // Excitatory/Inhibitory
    bool isExcitatory() const;
    bool isInhibitory() const;
    
    // Activity history for plasticity
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    void clearHistory();
    
    // Phase 5 checkpoint support
    void setPreSpikeHistory(const std::vector<Timestamp>& history);
    void setPostSpikeHistory(const std::vector<Timestamp>& history);
    
    // Plasticity flags
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    
    // Enable/disable plasticity rules
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Eligibility trace (for reward-modulated learning)
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    float getEfficacy() const;
    void setEfficacy(float efficacy);
    
    // Update synapse for one simulation step
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};
```

**Description**: Synapse representing a connection between neurons implementing real synaptic transmission with delays and short-term plasticity.

**Constructors**:
```cpp
Synapse(SynapseId id, NeuronId source, NeuronId destination);
```

**Key Methods**:
- `step()` - Updates synaptic dynamics including short-term plasticity
- `recordPreSpike()/recordPostSpike()` - Records spike timing for STDP
- `initializeRandom()` - Sets biological parameters based on synapse type
- `setWeight()/addToWeight()` - Modify connection strength

**Notes**:
- Implements Tsodyks-Markram short-term plasticity model
- Supports both chemical and electrical synapses
- Integrates with reward-modulated learning via eligibility traces
- Maintains spike history for temporal plasticity

### NeuralWorkingMemory
```cpp
class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store information in working memory
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    void storeToNeuron(NeuronId neuron, float activation);
    
    // Retrieve current working memory content
    std::vector<float> retrieve() const;
    
    // Check if specific neuron is part of working memory
    bool contains(NeuronId neuron) const;
    
    // Get activation level of a specific neuron
    float getNeuronActivation(NeuronId neuron) const;
    
    // Update working memory (maintenance and decay)
    void update(TimestepDuration dt);
    
    // Clear working memory
    void clear();
    
    // Get statistics
    size_t getActiveTraces() const;
    size_t getMemoryNeuronCount() const;
    const std::vector<float>& getMemoryActivations() const;
    const std::vector<SimulationStep>& getMemoryTimestamps() const;
    
    // Checkpoint support
    void restoreSnapshot(const std::vector<NeuronId>& neurons,
                         const std::vector<float>& activations,
                         const std::vector<SimulationStep>& timestamps);
    
    // Capacity management
    size_t getCapacity() const;
    void setCapacity(size_t cap);
    
    // Decay rate
    float getDecayRate() const;
    void setDecayRate(float rate);
    
    // Get neurons currently in working memory
    const std::vector<NeuronId>& getMemoryNeurons() const;
    
    // Strengthen memory representations
    void strengthenMemory(float factor);
    
    // Competition mechanisms
    void runCompetition();
    bool isWinning(NeuronId neuron) const;
    
    // Activity metrics
    float getMemoryActivity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    
    // Memory content
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    
    // Active memory traces
    std::vector<size_t> activeTraces_;
    
    // Recurrent connections for maintenance
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    
    // Competition winners
    std::vector<NeuronId> winners_;
};
```

**Description**: Implements working memory using persistent neural activity and recurrent connections, not a simple storage vector.

**Key Features**:
- **Persistent Activity**: Maintains information through reverberating neural activity
- **Recurrent Connections**: Creates feedback loops for information maintenance
- **Short-Term Plasticity**: Activity-dependent modulation of synaptic strength
- **Neural Competition**: Selective retention through winner-take-all dynamics

**Key Methods**:
- `storeToNeuron()` - Store information as neural activation
- `update()` - Maintain and decay memory traces
- `runCompetition()` - Select winning memory traces
- `restoreSnapshot()` - Restore memory from checkpoint

### NeuralEpisodicMemory
```cpp
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store a new experience
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve episodes with various criteria
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(
        const std::vector<float>& sensoryPattern,
        size_t maxResults = 5) const;
    
    std::vector<const EpisodicMemoryItem*> retrieveTemporal(
        SimulationStep startTime,
        SimulationStep endTime,
        size_t maxResults = 10) const;
    
    std::vector<const EpisodicMemoryItem*> retrieveByLocation(
        float x, float y, float radius,
        size_t maxResults = 5) const;
    
    std::vector<const EpisodicMemoryItem*> retrieveAfterAction(
        ActionType action,
        size_t maxResults = 5) const;
    
    // Replay and learning
    void replayEpisode(const EpisodicMemoryItem* episode);
    void updateRelevance(SimulationStep episodeId, float relevanceDelta);
    
    // Access methods
    const EpisodicMemoryItem* getEpisode(size_t index) const;
    size_t getEpisodeCount() const;
    const std::deque<EpisodicMemoryItem>& getEpisodes() const;
    
    // Recent episodes
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const;
    
    // Statistics and metrics
    float getAverageReward() const;
    
    // Memory management
    void consolidate(float relevanceThreshold);
    void clear();
    
    // Replay mechanisms
    void enableReplay(bool enable);
    bool isReplayEnabled() const;
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    void replaySequence(const std::vector<size_t>& episodeIds);
    
    // Checkpoint support
    void restoreEpisodes(const std::vector<EpisodicMemoryItem>& episodes);
    void setMaxEpisodes(size_t n);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::deque<EpisodicMemoryItem> episodes_;
    size_t maxEpisodes_;
    bool replayEnabled_;
    
    // Helper methods
    float computeSimilarity(const std::vector<float>& query,
                          const EpisodicMemoryItem& episode) const;
    
    bool matchesCriteria(const EpisodicMemoryItem& episode,
                        const std::vector<float>* sensoryQuery,
                        SimulationStep* startTime,
                        SimulationStep* endTime,
                        ActionType* actionQuery) const;
};
```

**Description**: Stores experiences as neural activity patterns with retrieval via pattern completion, temporal ordering, and relevance-weighted recall.

**Key Features**:
- **Pattern Storage**: Episodes encoded as neural activity patterns
- **Pattern Completion**: Hopfield-like retrieval from partial cues
- **Temporal Ordering**: Episodes ordered by occurrence time
- **Multi-criteria Retrieval**: Find episodes by similarity, time, location, action
- **Replay Mechanisms**: Reactivate neural patterns for consolidation

**Key Methods**:
- `storeEpisode()` - Encode experience as neural pattern
- `retrieveSimilar()` - Find similar experiences using pattern completion
- `retrieveTemporal()` - Get episodes from specific time windows
- `replayEpisode()` - Reactivate neural patterns to strengthen connections
- `consolidate()` - Remove irrelevant episodes based on relevance threshold

### NeuralAssociativeMemory
```cpp
class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Create associations between patterns
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);
    
    // Learn associations from experiences
    void associateFromExperience(const EpisodicMemoryItem& episode);
    
    // Retrieve patterns associated with a query
    std::vector<std::vector<float>> retrieve(
        const std::vector<float>& queryPattern,
        size_t maxResults = 5) const;
    
    // Get association strength between patterns
    float getAssociationStrength(
        const std::vector<float>& patternA,
        const std::vector<float>& patternB) const;
    
    // Update associations based on outcomes
    void updateAssociation(const std::vector<float>& patternA,
                         const std::vector<float>& patternB,
                         float delta);
    
    // Spread activation through association network
    std::vector<std::vector<float>> spreadActivation(
        const std::vector<float>& cuePattern,
        size_t steps = 2) const;
    
    // Memory management
    void clear();
    
    // Statistics
    size_t getAssociationCount() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
    
    // Helper methods
    NeuronId findPatternNeuron(const std::vector<float>& pattern);
    float computeSimilarity(const std::vector<float>& a,
                          const std::vector<float>& b) const;
};
```

**Description**: Creates and retrieves relationships between neural patterns through Hebbian plasticity and spreading activation.

**Key Features**:
- **Hebbian Learning**: Co-activating neurons form associations
- **Pattern Completion**: Retrieve related items given partial cues
- **Spreading Activation**: Cascade activation through association chains
- **Pattern Neurons**: Each pattern represented by a dedicated neuron

**Key Methods**:
- `associate()` - Create bidirectional association between patterns
- `associateFromExperience()` - Learn associations from episodic experiences
- `retrieve()` - Find patterns associated with a query pattern
- `spreadActivation()` - Cascade activation through multiple steps
- `findPatternNeuron()` - Find or create pattern representation neuron

### AttentionalSelection
```cpp
class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Process competing sensory inputs and select winners
    std::vector<NeuronId> processCompetition(
        const std::vector<NeuronId>& competitors,
        float globalInhibition = 0.5f);
    
    // Apply attention to specific regions
    void focusOnRegion(RegionId region);
    void releaseAttention();
    
    // Attention state
    std::vector<RegionId> getAttendedRegions() const;
    
    // Set attention parameters
    void setInhibitionStrength(float strength);
    void setExcitationStrength(float strength);
    void setCompetitionThreshold(float threshold);
    
    // Get inhibition/excitation levels
    float getInhibitionFor(NeuronId neuron) const;
    float getExcitationFor(NeuronId neuron) const;
    
    // Update attention dynamics
    void update(TimestepDuration dt);
    
    // Query attention state
    bool isAttended(NeuronId neuron) const;
    const std::vector<NeuronId>& getWinners() const;
    
    // Bias mechanisms
    void applyTopDownBias(NeuronId neuron, float biasStrength);
    void applyBottomUpSalience(NeuronId neuron, float salienceStrength);
    
    // Reset attention
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    float inhibitionStrength_;
    float excitationStrength_;
    float competitionThreshold_;
    
    std::vector<RegionId> attendedRegions_;
    std::vector<NeuronId> winners_;
    std::vector<float> neuronSalience_;
    
    std::unordered_map<uint64_t, float> topDownBias_;
    std::unordered_map<uint64_t, float> bottomUpSalience_;
};
```

**Description**: Neural attention through competition using lateral inhibition and gain modulation, not Transformer-style attention.

**Key Features**:
- **Neural Competition**: Lateral inhibition between competing neural populations
- **Salience-based Selection**: Bottom-up stimulus-driven attention
- **Top-down Bias**: Goal-directed attentional modulation
- **Regional Focus**: Selective attention to specific brain regions

**Key Methods**:
- `processCompetition()` - Select winning neurons through competition
- `focusOnRegion()` - Direct attention to specific brain regions
- `applyTopDownBias()/applyBottomUpSalience()` - Modulate attention with biases
- `update()` - Decay salience and bias over time

### Config
```cpp
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Loading
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;
    
    // Value access (defined in Config.cpp)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Value setting
    void set(const std::string& key, const ConfigValue& value,
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value,
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value,
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value,
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value,
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value,
             ConfigSource source = ConfigSource::Runtime);
    
    // Query
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};
```

**Description**: Configuration management system with validation and error handling supporting multiple sources (file, command line, runtime).

**Key Features**:
- **Multiple Sources**: File, command line, runtime configuration
- **Type Safety**: Strong typing with automatic conversions
- **Validation**: Configuration validation with error reporting
- **Persistence**: Save/load configuration to/from files

**Key Methods**:
- `get<T>()` - Get typed value with optional result
- `getOr()` - Get value with default if not present
- `set()` - Set value with source tracking
- `summary()` - Generate human-readable configuration summary

### Logger
```cpp
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Logger management
    void addLogger(std::shared_ptr<ILogger> logger);
    void clearLoggers();
    void setLevel(LogLevel level);
    
    // Global logging functions
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message,
               const std::string& file = "", int line = 0,
               const std::string& function = "");
    void info(const std::string& message,
              const std::string& file = "", int line = 0,
              const std::string& function = "");
    void warning(const std::string& message,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "");
    void error(const std::string& message,
               const std::string& file = "", int line = 0,
               const std::string& function = "");
    void critical(const std::string& message,
                  const std::string& file = "", int line = 0,
                  const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Global logger instance
    static Logger& getGlobal();
    static void setGlobal(std::shared_ptr<Logger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

**Description**: Comprehensive logging system with multiple sinks, levels, and thread safety.

**Key Features**:
- **Multiple Sinks**: Console, file, and custom logger implementations
- **Log Levels**: Debug, Info, Warning, Error, Critical
- **Thread Safety**: Proper synchronization for concurrent access
- **Structured Logging**: File, line, and function information
- **Global Logger**: Default logger instance for simple usage

**Key Methods**:
- `addLogger()` - Add logging sink
- `log()` - Low-level logging with metadata
- Convenience methods - Debug, info, warning, error, critical
- `getGlobal()` - Access to default logger instance

### LogStream
```cpp
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};
```

**Description**: Stream-style logging helper for easy logging of complex data.

**Usage**:
```cpp
NLM_LOG_STREAM(LogLevel::Info) << "Message: " << value << " more text";
```

### EpisodicMemoryItem
```cpp
struct EpisodicMemoryItem {
    SimulationStep timestamp;
    
    // Sensory state at time of experience
    std::vector<float> sensoryState;
    
    // Location/position information
    float positionX;
    float positionY;
    float orientation;
    
    // Action taken (if any)
    ActionType action;
    
    // Internal state (energy, reward, etc.)
    float reward;
    float energy;
    float novelty;
    
    // Consequence of action
    std::vector<float> resultingSensoryState;
    float resultingReward;
    
    // Associated neural activity pattern
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    
    // Time since this episode
    SimulationStep age;
    
    EpisodicMemoryItem()
        : timestamp(0), positionX(0), positionY(0), orientation(0)
        , action(ActionType::Wait)
        , reward(0), energy(0), novelty(0)
        , resultingReward(0), age(0) {}
};
```

**Description**: Represents a stored experience with sensory state, actions, outcomes, and associated neural activity.

## Function Reference

### Brain Constructors

#### Brain(std::shared_ptr<Config> config)
**Parameters**:
- `config` - Configuration object for brain parameters

**Description**: Create a new brain instance with the specified configuration.

**Example**:
```cpp
auto config = std::make_shared<Config>("configs/default.cfg");
Brain brain(config);
```

### Neuron Constructors

#### Neuron(NeuronId id)
**Parameters**:
- `id` - Unique identifier for the neuron

**Description**: Create a new neuron with the specified ID.

**Example**:
```cpp
Neuron neuron(NeuronId(1));
neuron.setType(NeuronType::Excitatory);
```

### Synapse Constructors

#### Synapse(SynapseId id, NeuronId source, NeuronId destination)
**Parameters**:
- `id` - Unique identifier for the synapse
- `source` - Source neuron ID
- `destination` - Destination neuron ID

**Description**: Create a new synapse connecting two neurons.

**Example**:
```cpp
Synapse synapse(SynapseId(1), NeuronId(2), NeuronId(3));
synapse.setWeight(0.5f);
```

## Configuration Keys

### Core Simulation Parameters
- `neuron_count` - Total number of neurons (default: 1000)
- `region_count` - Number of brain regions (default: 1)
- `connection_probability` - Probability of synapse formation (default: 0.1)
- `simulation_timestep` - Duration of each simulation step (default: 0.001)
- `random_seed` - Random number generator seed (default: 42)

### Neural Dynamics
- `neuron_count` - Number of neurons per region
- `connection_probability` - Synapse formation probability

### Plasticity Parameters
- `stdp_ltp_weight` - STDP long-term potentiation weight (default: 0.01)
- `stdp_ltd_weight` - STDP long-term depression weight (default: 0.012)\n- `stdp_tau` - STDP time constant (default: 20.0)
- `synaptogenesis_rate` - Rate of new synapse formation (default: 0.0001)
- `pruning_rate` - Rate of synapse elimination (default: 0.00001)

### Memory Systems
- `working_memory_capacity` - Working memory capacity (default: 100)
- `episodic_memory_max_episodes` - Maximum episodic memories (default: 1000)
- `associative_memory_strength_threshold` - Association strength threshold (default: 0.3)

### Neuromodulation
- `dopamine_strength` - Dopamine modulation strength (default: 1.0)
- `curiosity_threshold` - Curiosity activation threshold (default: 0.3)
- `novelty_decay_rate` - Novelty decay rate (default: 0.99)

### Development
- `enable_development` - Enable developmental processes (default: true)
- `developmental_stage` - Initial developmental stage
- `plasticity_modifier_rate` - Rate of plasticity changes (default: 0.1)

### Logging
- `level` - Logging level (debug, info, warning, error, critical) (default: info)
- `file_path` - Log file path (default: ./logs/nlm.log)
- `console_output` - Enable console output (default: true)
- `max_file_size` - Maximum log file size (default: 10485760)

## Template Instantiations

The following template functions are explicitly instantiated in Config.cpp:

### Integer Types
```cpp
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
```

### Default Value Types
```cpp
template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
```

## See Also

For related functionality and additional information:

- **Brain.hpp**: Main brain interface
- **Neuron.hpp**: Detailed neuron documentation
- **Synapse.hpp**: Detailed synapse documentation
- **README.md**: Project overview and usage instructions
- **INSTALL.md**: Installation and setup guide
- **DEVELOPING.md**: Development guidelines
- **API_REFERENCE.md**: This document
- **EXAMPLES.md**: Usage examples and tutorials

## Version Information

This API reference corresponds to NLM Phase 6 (Final Integration).

**Last Updated**: 2026-11-12
**Version**: 6.1.0
**Status**: Production Ready