# NLM (熙然) - Comprehensive Header Files API Documentation

## Overview

This document provides comprehensive API documentation for all header files in the NLM (Neural Learning Machine) project. The documentation follows a consistent format and includes detailed information about classes, functions, enumerations, and usage examples.

## Core Architecture Headers

### 1. core/Config/Config.hpp

#### Configuration Management

**Purpose**: The main configuration system for NLM, providing key-value storage with type flexibility and persistence.

**Key Features**:
- Strong typing with variant-based value storage
- File-based configuration loading (JSON-compatible key=value format)
- Command-line argument integration
- Runtime configuration modification
- Thread-safe operations

**Main Classes**:

##### Config
```cpp
class Config {
public:
    // Construction/Destruction
    Config();
    ~Config();
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // File Operations
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    
    // Command Line Interface
    bool loadFromArgs(int argc, char** argv);
    
    // Type-Safe Value Access
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Value Setting (Multiple Overloads)
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Utility Functions
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;
};
```

**Enumerations**:

##### ConfigSource
```cpp
enum class ConfigSource {
    Default,     // Built-in default values
    File,        // Loaded from configuration file
    CommandLine, // Passed via command line
    Runtime      // Set at runtime during execution
};
```

**Usage Examples**:

```cpp
#include "core/Config/Config.hpp"

// Create and configure a brain
auto config = std::make_shared<Config>();
config->set("neuron_count", 1000);
config->set("region_count", 3);
config->set("connection_probability", 0.05);
config->set("random_seed", 12345LL);

// Load from file
if (!config->loadFromFile("/path/to/config.cfg")) {
    // Handle error
}

// Get configuration values
auto neuronCount = config->get<int64_t>("neuron_count");
if (neuronCount.has_value()) {
    std::cout << "Neuron count: " << neuronCount.value() << std::endl;
}

// Command line interface
// Usage: ./nlm --neuron_count=1000 --region_count=2 --random_seed=42
```

**Configuration Key Types**:
- **Numeric**: int, int64_t, double
- **Boolean**: bool
- **String**: std::string
- **Vector Types**: std::vector<int>, std::vector<double>, std::vector<std::string>

**Error Handling**:
- `get()` returns `std::optional<T>` for type-safe access
- Functions return `bool` indicating success/failure
- Comprehensive error logging through NLMError system

### 2. core/Logger/Logger.hpp

#### Logging Framework

**Purpose**: Comprehensive logging system with multiple log levels, colored output, and error handling integration.

**Key Features**:
- Multiple log levels (Debug, Info, Warning, Error, Critical)
- Colored console output
- File logging support
- Thread-safe operations
- Integration with NLMError system

**Main Classes**:

##### ILogger (Interface)
```cpp
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};
```

##### ConsoleLogger
```cpp
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Configuration Options
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
private:
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};
```

##### Logger (Main Class)
```cpp
class Logger {
public:
    // Global Instance Access
    static Logger& getGlobal();
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Configuration
    void setLevel(LogLevel level);
    bool isEnabled(LogLevel level) const;
    
    // Logging Functions
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    // Convenience Methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
};
```

**Enumerations**:

##### LogLevel
```cpp
enum class LogLevel {
    Debug = 0,    // Fine-grained informational messages
    Info = 1,      // High-level information messages
    Warning = 2,   // Warnings about potential issues
    Error = 3,     // Error conditions
    Critical = 4   // Critical errors, system may be unusable
};
```

**Convenience Macros**:
```cpp
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)
```

**Usage Examples**:

```cpp
#include "core/Logger/Logger.hpp"

// Enable debug logging
nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Debug);

// Basic logging
NLM_LOG_INFO("Starting NLM brain simulation");
NLM_LOG_DEBUG("Neuron count: " + std::to_string(neuronCount));
NLM_LOG_WARNING("High firing rate detected");

// Error logging with context
nlm::NLMError::logError(
    nlm::NLMError::ErrorCode::FILE_NOT_FOUND,
    "Configuration file not found",
    "Please check file path and permissions"
);
```

### 3. brain/Neuron.hpp

#### Single Neuron Model

**Purpose**: Implements a single Leaky Integrate-and-Fire (LIF) neuron with realistic biophysical properties.

**Key Features**:
- LIF dynamics with adaptive spike-frequency adaptation
- Synaptic input integration (excitatory, inhibitory, modulatory)
- Refractory period handling
- Spike history recording
- Random parameter initialization

**Main Classes**:

##### NeuronState (Struct)
```cpp
struct NeuronState {
    // Membrane Potential Dynamics
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    
    // Firing Dynamics
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    
    // Adaptation Variables
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
    // Default constructor with biological parameters
    NeuronState()
        : membranePotential(-70.0f)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , leakConductance(10.0f)
        , synapseConductance(0.0f)
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        , refractoryPeriod(9)  // Phase 6: 9 steps (not 5)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};
```

##### Neuron
```cpp
class Neuron {
public:
    // Construction/Destruction
    explicit Neuron(NeuronId id);
    ~Neuron();
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity
    NeuronId getId() const;
    
    // Type Management
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // State Access
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // Membrane Potential Control
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold Control
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    // Firing State Management
    bool isFiring() const;
    bool isRefractory() const;
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void decrementRefractory();
    
    // Firing Rate Control
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF Neuron Parameters
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    MembranePotential getResetPotential() const;
    
    // Spike Detection
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    // LIF Dynamics
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    void step(Timestamp currentTime);
    
    // Synaptic Input
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // Spike History
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Synapse Management
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity State
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/Population Membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // State Management
    void reset();
    void initializeRandom(RandomGenerator& rng);
};
```

**Enumerations**:

##### NeuronType
```cpp
enum class NeuronType {
    Excitatory,   // Standard excitatory neurons
    Inhibitory,   // Inhibitory interneurons
    Modulatory,   // Neuromodulatory neurons
    Sensory,      // Sensory input neurons
    Motor,        // Motor output neurons
    Internal,     // Internal processing neurons
};
```

##### FiringState
```cpp
enum class FiringState {
    Resting,      // Not firing, below threshold
    Active,       // Currently firing
    Refractory,   // Cannot fire due to refractory period
    Inhibited,    // Actively inhibited
};
```

**Usage Examples**:

```cpp
#include "brain/Neuron.hpp"

// Create a neuron
NeuronId neuronId(42);
Neuron neuron(neuronId);
neuron.setType(NeuronType::Excitatory);

// Configure neuron parameters
neuron.setThreshold(-50.0f);
neuron.setRestingPotential(-70.0f);
neuron.setResetPotential(-70.0f);
neuron.setRefractoryPeriod(5);

// Initialize with random parameters
RandomGenerator rng(12345);
neuron.initializeRandom(rng);

// Process inputs
neuron.injectCurrent(15.0f);        // External input
neuron.receiveExcitatoryInput(10.0f); // Synaptic input
neuron.receiveInhibitoryInput(5.0f);  // Inhibition

// Step simulation
bool fired = neuron.stepLIF(currentTime, 0.001f);
if (fired) {
    std::cout << "Neuron " << neuronId.value << " fired at time " << currentTime << std::endl;
}
```

### 4. brain/Synapse.hpp

#### Synaptic Connection Model

**Purpose**: Represents synaptic connections between neurons with different types of plasticity.

**Key Features**:
- Multiple synapse types (excitatory, inhibitory, modulatory, electrical, gap junction)
- Delay-based transmission
- Plasticity flags for different plasticity mechanisms
- Connection strength management
- Spike timing-dependent plasticity (STDP) support

**Main Classes**:

##### SynapseId (Struct)
```cpp
struct SynapseId {
    uint64_t value;
    
    SynapseId() : value(0) {}
    SynapseId(uint64_t v) : value(v) {}
    
    uint64_t index() const { return value; }
    bool operator==(const SynapseId& other) const { return value == other.value; }
    bool operator!=(const SynapseId& other) const { return value != other.value; }
};
```

##### Synapse
```cpp
class Synapse {
public:
    // Construction/Destruction
    Synapse(SynapseId id, NeuronId source, NeuronId target);
    ~Synapse();
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity
    SynapseId getId() const;
    NeuronId getSourceId() const;
    NeuronId getTargetId() const;
    
    // Connection Properties
    void setWeight(SynapticWeight weight);
    SynapticWeight getWeight() const;
    void setDelay(Delay delay);
    Delay getDelay() const;
    
    // Plasticity Control
    void enablePlasticity(PlasticityFlags flags);
    PlasticityFlags getPlasticityFlags() const;
    bool isPlastic() const;
    
    // Transmission
    void transmit(float weightFactor = 1.0f);
    bool canTransmit(Timestamp currentTime) const;
    
    // State Management
    void reset();
    
    // Statistics
    float getTransmissionCount() const;
    Timestamp getLastTransmissionTime() const;
};
```

**Enumerations**:

##### SynapseType
```cpp
enum class SynapseType {
    Excitatory,    // Glutamatergic excitation
    Inhibitory,    // GABAergic inhibition
    Modulatory,    // Neuromodulatory signaling
    Electrical,    // Electrical coupling
    GapJunction,   // Direct ion flow
};
```

**Usage Examples**:

```cpp
#include "brain/Synapse.hpp"

// Create a synaptic connection
SynapseId synapseId(123);
NeuronId sourceId(42);
NeuronId targetId(100);

Synapse synapse(synapseId, sourceId, targetId);
synapse.setType(SynapseType::Excitatory);
synapse.setWeight(0.5f);
synapse.setDelay(5);

// Configure plasticity
synapse.enablePlasticity({
    .stdp = true,
    .hebbian = true,
    .reward_modulated = true
});

// Transmission (during simulation)
if (synapse.canTransmit(currentTime)) {
    synapse.transmit();
}
```

### 5. brain/Brain.hpp

#### Central Neural System

**Purpose**: Main brain class integrating all neural subsystems (memory, neuromodulation, cognition, development, prediction).

**Key Features**:
- Closed-loop brain-world interaction
- Real-time spiking neural computation
- Integrated memory systems
- Neuromodulation with reward and curiosity
- Predictive processing
- Developmental stages
- Action selection and motor output
- Checkpointing and persistence

**Main Classes**:

##### Brain
```cpp
class Brain {
public:
    // Construction/Destruction
    explicit Brain(std::shared_ptr<Config> config);
    ~Brain();
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Core Lifecycle
    bool initialize();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    void reset();
    
    // Sensory/Motor Interface
    void receiveSensoryInput(const SensoryInput& input);
    std::unique_ptr<Action> produceAction();
    
    // Neural Control
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // System Access (Integrated Subsystems)
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics and Monitoring
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Memory Systems
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Neuromodulation Systems
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Cognition Systems
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Development System
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Persistence
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Other Methods (Abbreviated)
    void applyNeuromodulation(const Neuromodulator& signal);
    void updatePlasticity();
    void develop();
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    float getLearnedPreference() const;
    void setLearnedPreference(float preference);
    void logStatus() const;
};
```

**Usage Examples**:

```cpp
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"

// Create and initialize brain
auto config = std::make_shared<Config>();
config->set("neuron_count", 1000);
config->set("region_count", 3);

Brain brain(config);
if (!brain.initialize()) {
    std::cerr << "Failed to initialize brain" << std::endl;
    return 1;
}

// Create world and agent
SimpleWorld world;
world.configure(16, 16, 16, 16);
AgentBrain agent(brain);
agent.initialize(world);

// Run simulation loop
for (SimulationStep step = 0; step < 1000; ++step) {
    // World update
    world.update(0.001);
    
    // Sensory processing
    auto percept = world.getSensoryPercept();
    agent.processSensoryInput(percept);
    
    // Brain computation
    brain.step(step, step * 0.001);
    
    // Action production
    auto action = brain.produceAction();
    if (action) {
        world.applyMotorCommand(*action, step * 0.001);
    }
}

// Save brain state
brain.save("/tmp/brain_checkpoint.bin");
```

## Memory System Headers

### 6. memory/NeuralWorkingMemory.hpp

#### Working Memory

**Purpose**: Temporary storage of active information with competition and decay.

**Key Features**:
- Pattern storage and retrieval
- Competitive dynamics between memories
- Adaptive decay based on age and activation
- Integration with neural activity
- Capacity limits and management

**Main Classes**:

##### NeuralWorkingMemory
```cpp
class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();
    
    // Core Functionality
    void initialize(Brain* brain);
    void store(const std::vector<float>& pattern, float strength);
    void storeToNeuron(NeuronId neuron, float activation);
    std::vector<float> retrieve() const;
    bool contains(NeuronId neuron) const;
    float getNeuronActivation(NeuronId neuron) const;
    
    // Management
    void clear();
    void update(TimestepDuration dt);
    void decay(float decayRate);
    void strengthenMemory(float factor);
    
    // Access and Statistics
    size_t getCapacity() const;
    size_t getCurrentSize() const;
    float getMemoryActivity() const;
    std::vector<NeuronId> getActiveTraces() const;
    size_t getActiveTracesCount() const;
    
    // Internal State
    bool isWinning(NeuronId neuron) const;
    std::vector<NeuronId> getWinners() const;
};
```

**Usage Example**:

```cpp
#include "memory/NeuralWorkingMemory.hpp"

auto* wm = brain->getWorkingMemory();
if (wm) {
    // Store pattern
    std::vector<float> pattern = {0.1f, 0.5f, 0.9f, 0.3f};
    wm->store(pattern, 1.0f);
    
    // Store to specific neuron
    NeuronId neuronId(42);
    wm->storeToNeuron(neuronId, 0.8f);
    
    // Retrieve memories
    std::vector<float> retrieved = wm->retrieve();
    for (float activation : retrieved) {
        std::cout << "Memory activation: " << activation << std::endl;
    }
    
    // Get statistics
    std::cout << "Active traces: " << wm->getActiveTracesCount() << std::endl;
    std::cout << "Memory activity: " << wm->getMemoryActivity() << std::endl;
}
```

### 7. memory/NeuralEpisodicMemory.hpp

#### Episodic Memory

**Purpose**: Storage of events and experiences for later recall and replay.

**Key Features**:
- Event-based memory storage
- Time-stamped episodes
- Replay mechanisms
- Memory consolidation
- Integration with working memory

**Main Classes**:

##### EpisodicMemoryItem (Struct)
```cpp
struct EpisodicMemoryItem {
    SimulationStep timestamp;
    std::vector<NeuronId> neurons;
    std::vector<float> values;
    std::string metadata;
    
    EpisodicMemoryItem() : timestamp(0) {}
};
```

##### NeuralEpisodicMemory
```cpp
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    // Core Operations
    void storeEpisode(const EpisodicMemoryItem& episode);
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    size_t getEpisodeCount() const;
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    // Management
    void clear();
    void consolidate(float relevanceThreshold);
    
    // Replay
    std::vector<EpisodicMemoryItem> getEpisodesForReplay(size_t count) const;
    
    // Statistics
    size_t getTotalNeuronsStored() const;
    float getAverageActivation() const;
};
```

**Usage Example**:

```cpp
#include "memory/NeuralEpisodicMemory.hpp"

auto* em = brain->getEpisodicMemory();
if (em) {
    // Create an episode
    EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.neurons = {NeuronId(1), NeuronId(42), NeuronId(100)};
    episode.values = {0.8f, 0.3f, 0.6f};
    episode.metadata = "Feeding event";
    
    // Store episode
    em->storeEpisode(episode);
    
    // Retrieve recent episodes
    auto recent = em->getRecentEpisodes(5);
    for (const auto& ep : recent) {
        std::cout << "Episode at step " << ep.timestamp << " with " 
                 << ep.neurons.size() << " neurons" << std::endl;
    }
    
    // Get episodes for replay
    auto replayEpisodes = em->getEpisodesForReplay(3);
}
```

### 8. memory/NeuralAssociativeMemory.hpp

#### Associative Memory

**Purpose**: Formation and retrieval of associations between neural patterns.

**Key Features**:
- Pattern association learning
- Strength-based recall
- Competition and interference management
- Integration with Hebbian learning
- Associative inference

**Main Classes**:

##### NeuralAssociativeMemory
```cpp
class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Association Operations
    void associate(NeuronId a, NeuronId b, float strength);
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Management
    void clear();
    
    // Statistics and Analysis
    size_t getAssociationCount() const;
    float getAverageStrength() const;
    float getAssociationCoherence() const;
};
```

**Usage Example**:

```cpp
#include "memory/NeuralAssociativeMemory.hpp"

auto* am = brain->getAssociativeMemory();
if (am) {
    // Create associations between neurons
    am->associate(NeuronId(1), NeuronId(42), 0.8f);
    am->associate(NeuronId(1), NeuronId(100), 0.6f);
    am->associate(NeuronId(42), NeuronId(100), 0.4f);
    
    // Get associations for a neuron
    auto associations = am->getAssociations(NeuronId(1));
    for (const auto& neuron : associations) {
        float strength = am->getAssociationStrength(NeuronId(1), neuron);
        std::cout << "Neuron " << neuron.value << ": strength = " << strength << std::endl;
    }
    
    // Update association strength
    am->updateAssociation(NeuronId(1), NeuronId(42), 0.1f);
}
```

## Neuromodulation System Headers

### 9. neuromodulation/Dopamine.hpp

#### Dopamine System

**Purpose**: Reward prediction and reinforcement signaling.

**Key Features**:
- Reward prediction error computation
- Dopamine level regulation
- Reinforcement learning modulation
- Interaction with plasticity rules
- Exploration-exploitation balance

**Main Classes**:

##### Dopamine
```cpp
class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    // Core Operations
    void update(float reward, float predictionError, TimestepDuration dt);
    float getLevel() const;
    void setLevel(float level);
    
    // Configuration
    void setBaseline(float baseline);
    float getBaseline() const;
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // State Management
    void reset();
    bool isActive() const;
    
    // Statistics
    float getTotalRewardsReceived() const;
    size_t getUpdateCount() const;
    float getAveragePredictionError() const;
};
```

**Usage Example**:

```cpp
#include "neuromodulation/Dopamine.hpp"

auto* dopamine = brain->getDopamine();
if (dopamine) {
    // Update with reward prediction
    dopamine->update(1.0f, 0.5f, 0.001f);  // reward, prediction error, dt
    
    // Check dopamine level
    float level = dopamine->getLevel();
    std::cout << "Dopamine level: " << level << std::endl;
    
    // Configure system
    dopamine->setBaseline(0.1f);
    dopamine->setLearningRate(0.01f);
}
```

### 10. neuromodulation/Curiosity.hpp

#### Curiosity System

**Purpose**: Exploration motivation and intrinsic reward generation.

**Key Features**:
- Intrinsic reward based on novelty and uncertainty
- Exploration-exploitation balance
- Action selection motivation
- Integration with working memory
- Adaptation to environment complexity

**Main Classes**:

##### Curiosity
```cpp
class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Core Operations
    void update(float novelty, float predictionError, TimestepDuration dt);
    float getLevel() const;
    void setLevel(float level);
    
    // Configuration
    void setTargetLevel(float level);
    float getTargetLevel() const;
    void setAdaptationRate(float rate);
    float getAdaptationRate() const;
    
    // State Management
    void reset();
    bool isActive() const;
    
    // Statistics
    float getTotalNoveltyExperienced() const;
    size_t getUpdateCount() const;
    float getAverageNovelty() const;
};
```

**Usage Example**:

```cpp
#include "neuromodulation/Curiosity.hpp"

auto* curiosity = brain->getCuriosity();
if (curiosity) {
    // Update with novelty signals
    curiosity->update(0.7f, 0.3f, 0.001f);  // novelty, prediction error, dt
    
    // Check curiosity level
    float level = curiosity->getLevel();
    std::cout << "Curiosity level: " << level << std::endl;
    
    // Configure system
    curiosity->setTargetLevel(0.5f);
    curiosity->setAdaptationRate(0.05f);
}
```

### 11. neuromodulation/Neuromodulator.hpp

#### Neuromodulator Base Class

**Purpose**: Base class for all neuromodulators providing common functionality.

**Key Features**:
- Common neuromodulator interface
- Level management
- State tracking
- Configuration management
- Integration utilities

**Main Classes**:

##### Neuromodulator
```cpp
class Neuromodulator {
public:
    Neuromodulator();
    virtual ~Neuromodulator();
    
    // Core Operations
    virtual void update(float signal, TimestepDuration dt) = 0;
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Configuration
    void setBaseline(float baseline);
    float getBaseline() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
    // State Management
    virtual void reset();
    bool isActive() const;
    bool wasRecentlyActive(float timeWindow) const;
    
    // Statistics
    float getTotalInput() const;
    size_t getUpdateCount() const;
    float getAverageLevel() const;
    
    // Utility
    float getInfluenceFactor() const;
    void setInfluenceFactor(float factor);
};
```

## World and Environment Headers

### 12. world/SimpleWorld.hpp

#### Simple World Environment

**Purpose**: Basic 2D world environment for agent interaction and simulation.

**Key Features**:
- Grid-based world representation
- Agent body state management
- Object interaction (resources, hazards, etc.)
- Sensory input generation
- Motor command processing
- Physics simulation

**Main Classes**:

##### SimpleWorld
```cpp
class SimpleWorld {
public:
    SimpleWorld();
    ~SimpleWorld();
    
    // Configuration
    void configure(size_t width, size_t height, 
                   size_t visionWidth, size_t visionHeight);
    
    // State Management
    void reset();
    void setAgentStart(float x, float y);
    void setAgentStart(float x, float y, float orientation);
    
    // Time and State Updates
    void update(TimestepDuration dt);
    
    // Object Management
    void addObject(const WorldObject& object);
    void removeObject(float x, float y);
    bool isValidPosition(float x, float y) const;
    
    // Sensory Input
    SensoryPercept getSensoryPercept() const;
    
    // Agent State
    AgentBody getAgentBody() const;
    
    // Query Functions
    size_t getWidth() const;
    size_t getHeight() const;
    float getMaxEnergy() const;
    void setMaxEnergy(float e);
    float getEnergyDecayRate() const;
    void setEnergyDecayRate(float r);
    Timestamp getSimulationTime() const;
    void setRandomSeed(uint64_t seed);
    uint64_t getRandomSeed() const;
    
    // Motor Command Processing
    ActionResult applyMotorCommand(const MotorCommand& cmd, Timestamp time);
};
```

**Usage Example**:

```cpp
#include "world/SimpleWorld.hpp"

SimpleWorld world;
world.configure(16, 16, 16, 16);  // 16x16 world with 16x16 vision
world.reset();
world.setAgentStart(8, 8);       // Start agent in center

// Update world
world.update(0.001);               // 1ms timestep

// Get agent state
AgentBody body = world.getAgentBody();
std::cout << "Agent position: (" << body.x << ", " << body.y << ")" << std::endl;

// Apply motor command
MotorCommand cmd;
cmd.type = MotorCommand::MoveForward;
ActionResult result = world.applyMotorCommand(cmd, 0.001f);
std::cout << "Action result: reward = " << result.reward << ", success = " << result.success << std::endl;
```

### 13. environment/Environment.hpp

#### Environment Base Class

**Purpose**: Abstract base class for environment simulations.

**Key Features**:
- Abstract environment interface
- Template for custom environments
- State management
- Agent interaction
- Reward system
- Episode management

**Main Classes**:

##### Environment (Abstract)
```cpp
class Environment {
public:
    virtual ~Environment() = default;
    
    // Configuration
    virtual void configure(const Config& config) = 0;
    
    // Episode Management
    virtual void reset() = 0;
    virtual bool isDone() const = 0;
    virtual float getReward() const = 0;
    virtual Timestamp getTime() const = 0;
    
    // Agent Control
    virtual std::unique_ptr<Action> act() = 0;
    virtual void observe() = 0;
    
    // State Access
    virtual std::string getState() const = 0;
    virtual void setState(const std::string& state) = 0;
    
    // Environment Query
    virtual size_t getStateSize() const = 0;
    virtual size_t getActionSize() const = 0;
    virtual float getMaxReward() const = 0;
    
    // Episode Statistics
    virtual float getEpisodeReturn() const = 0;
    virtual size_t getEpisodeLength() const = 0;
};
```

## Python Bindings Headers

### 14. python/bindings.cpp

#### Pybind11 Python Module

**Purpose**: Python bindings for the NLM C++ library using pybind11.

**Key Features**:
- Complete Python API for all NLM classes
- Type conversions between C++ and Python
- Error handling with Python exceptions
- Documentation generation
- Module initialization and export

**Main Components**:

##### Pybind11 Module Definition
```cpp
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, 
        SensoryInput, and Action.
    )pbdoc";
    
    // Core Classes
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", ...);
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", ...);
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", ...);
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", ...);
    
    // Sensorimotor Classes
    py::class_<SensoryInput>(m, "SensoryInput", ...);
    py::class_<Vision, SensoryInput>(m, "Vision", ...);
    py::class_<Audio, SensoryInput>(m, "Audio", ...);
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", ...);
    py::class_<SensoryPercept>(m, "SensoryPercept", ...);
    py::class_<Action>(m, "Action", ...);
    py::class_<WorldObject>(m, "WorldObject", ...);
    
    // Motor System
    py::class_<AgentBody>(m, "AgentBody", ...);
    
    // Enumerations
    py::enum_<NeuronType>(m, "NeuronType", ...);
    py::enum_<SynapseType>(m, "SynapseType", ...);
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", ...);
    py::enum_<FiringState>(m, "FiringState", ...);
    py::enum_<ActionType>(m, "ActionType", ...);
    py::enum_<MotorCommand>(m, "MotorCommand", ...);
    py::enum_<WorldObjectType>(m, "WorldObjectType", ...);
    
    // Factory Functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}
```

**Python API Features**:
- **Brain Class**: Complete neural simulation with all subsystems
- **Config Class**: Configuration management with type safety
- **AgentBrain Class**: Bridge between neural system and environment
- **SimpleWorld Class**: Basic environment for testing and demonstration
- **Sensory Classes**: Vision, audio, and internal signal processing
- **Action Classes**: Motor control and behavior representation
- **Enumeration Types**: All important NLM types as Python enums
- **Helper Functions**: Factory functions and utility functions

## Error Handling Headers

### 15. error_handling_improvements.hpp

#### Enhanced Error Handling System

**Purpose**: Comprehensive error handling framework with standardized error codes and logging.

**Key Features**:
- Standardized error codes
- Context-aware error messages
- Integration with existing logging system
- Safe operation wrappers
- Error checking macros

**Main Classes**:

##### NLMError
```cpp
class NLMError {
public:
    enum class ErrorCode {
        SUCCESS = 0,
        NULL_POINTER,
        INVALID_PARAMETER,
        FILE_NOT_FOUND,
        MEMORY_ALLOCATION_FAILED,
        INITIALIZATION_FAILED,
        CONFIGURATION_ERROR,
        NETWORK_ERROR,
        TIMEOUT,
        UNKNOWN_ERROR
    };
    
    static std::string getErrorCodeString(ErrorCode code);
    static void logError(ErrorCode code, const std::string& context, 
                        const std::string& details = "");
};
```

##### SafePointer
```cpp
template<typename T>
class SafePointer {
private:
    std::shared_ptr<T> ptr;
    
public:
    SafePointer() : ptr(nullptr) {}
    SafePointer(std::shared_ptr<T> p) : ptr(p) {}
    SafePointer(T* p) : ptr(p) {}
    
    bool valid() const { return ptr != nullptr; }
    operator bool() const { return valid(); }
    T* operator->() const { return ptr.get(); }
    T& operator*() const { return *ptr; }
    
    template<typename U>
    bool operator==(const SafePointer<U>& other) const {
        return ptr.get() == other.operator->();
    }
    
    template<typename U>
    bool operator!=(const SafePointer<U>& other) const {
        return !(*this == other);
    }
};
```

##### SafeOperation
```cpp
class SafeOperation {
private:
    bool success_;
    std::string errorMessage_;
    
public:
    SafeOperation() : success_(true), errorMessage_("") {}
    
    bool success() const { return success_; }
    const std::string& errorMessage() const { return errorMessage_; }
    
    void setSuccess() { success_ = true; }
    void setFailure(const std::string& msg) { 
        success_ = false; 
        errorMessage_ = msg; 
        NLMError::logError(NLMError::ErrorCode::UNKNOWN_ERROR, 
                         "Operation failed", msg);
    }
    
    template<typename Func, typename... Args>
    auto execute(Func&& func, Args&&... args) -> decltype(func(args...)) {
        try {
            auto result = func(std::forward<Args>(args)...);
            setSuccess();
            return result;
        } catch (const std::exception& e) {
            setFailure(e.what());
            throw; // Re-throw after logging
        }
    }
};
```

**Error Handling Macros**:
```cpp
#define CHECK_NULL(ptr, error_msg) \
    if (!(ptr)) { \
        NLMError::logError(NLMError::ErrorCode::NULL_POINTER, error_msg, "NULL pointer detected"); \
        return false; \
    }

#define CHECK_CONDITION(condition, error_code, error_msg) \
    if (!(condition)) { \
        NLMError::logError(error_code, error_msg, "Validation failed"); \
        return false; \
    }
```

## Testing and Examples

### Test Files

#### test_phase6_integration.cpp
**Purpose**: Comprehensive Phase 6 integration test suite.
**Features**:
- Tests all Phase 6 systems integration
- Validates memory, neuromodulation, cognition, prediction
- Tests world interaction and complete brain loop
- Verifies checkpoint system functionality
- Provides detailed test output and statistics

#### example/error_handling_example.cpp
**Purpose**: Demonstration of error handling features.
**Features**:
- Configuration loading with error handling
- Command line argument parsing
- Configuration saving and reloading
- Error code demonstration
- Complete error handling workflow

## Build and Installation

### Building the Library
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Building Python Bindings
```bash
# For C++ build with Python bindings
mkdir build_python
cd build_python
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON -DCMAKE_BUILD_TYPE=Release
make -j4

# For pip install
pip install .
```

### Running Tests
```bash
# Run Phase 6 integration tests
./nlm_phase6_demo  # Built-in integration test

# Or run custom test executables
./test_phase6_integration
```

## Python Module Usage

### Installation
```bash
# From source (development)
pip install pybind11 scikit-build-core pytest numpy
pip install .  # or pip install -e .

# From PyPI (when available)
pip install pynlm
```

### Basic Usage
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 3)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create environment and agent
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Run simulation
for step in range(1000):
    world.update(0.001)
    brain.step(step)
    action = brain.produceAction()
    world.applyMotorCommand(action)

# Save brain state
brain.save("/tmp/brain_state.bin")

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing rate: {brain.getAverageFiringRate()} Hz")
```

### Advanced Usage
```python
import pynlm

# Batch simulation
configs = [
    {"neuron_count": 500, "region_count": 2},
    {"neuron_count": 1000, "region_count": 3},
    {"neuron_count": 1500, "region_count": 4},
]

def run_simulation(config_dict):
    config = pynlm.createDefaultConfig()
    for key, value in config_dict.items():
        setattr(config, key, value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
    
    return brain.getAverageFiringRate()

# Run batch simulation
results = pynlm.batch_simulate(configs, steps=100)
for result in results:
    print(f"Config {result['config_index']}: {result['final_firing_rate']} Hz")
```

## License and Attribution

This documentation is part of the NLM (熙然) project, an experimental artificial developmental brain simulator. The project is licensed under the MIT License.

All improvements described in this document are part of the ongoing Phase 6 (FINAL INTEGRATION) development effort, aimed at creating a complete, coherent artificial brain system.

## Contact and Support

For questions, issues, or contributions:
- Project Documentation: https://nlm.readthedocs.io
- Repository: https://github.com/nlm-project/nlm
- Issues: https://github.com/nlm-project/nlm/issues

## Acknowledgments

Special thanks to all contributors and researchers who have advanced the NLM project through its various phases. This comprehensive improvement suite represents years of development and research in brain-inspired artificial intelligence.

---

*This documentation was generated as part of the NLM project's comprehensive improvement effort, focusing on enhanced error handling, Python bindings, and overall code quality improvements.*