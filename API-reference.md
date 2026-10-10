# NLM API Reference

## Core Namespace: nlm

This document provides detailed API documentation for the NLM (Neural Learning Machine) framework.

## 1. Core Types

### 1.1 ID Types

```cpp
using NeuronId = uint32_t;
using SynapseId = uint32_t;
using RegionId = uint32_t;
using PopulationId = uint32_t;
using SynapseHandle = uint32_t;
using Timestamp = double;  // Simulation time in milliseconds
using MembranePotential = float;  // Membrane voltage in mV
using SynapticWeight = float;     // Synaptic weight (-1.0 to 1.0)
using FiringRate = float;         // Firing rate in Hz
using TimestepDuration = double;  // Time step duration in seconds
using PlasticityFlags = struct {
    bool hebbian;          // Hebbian plasticity enabled
    bool stdp;             // STDP plasticity enabled  
    bool reward_modulated; // Reward-modulated plasticity enabled
};
```

### 1.2 Neuron State Structure

```cpp
struct NeuronState {
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
};
```

### 1.3 Enumerations

#### NeuronType
```cpp
enum class NeuronType {
    Sensory,        // Receives external input
    Interneuron,    // Processes and relays information
    Motor,          // Generates motor commands
    Modulatory,     // Regulates plasticity and learning
    Internal        // Internally generated activity
};
```

#### FiringState
```cpp
enum class FiringState {
    Resting,      // Not firing, below threshold
    Active,       // Firing or just crossed threshold
    Refractory,   // Recovering from spike, cannot fire
    Adapting      // Spike-frequency adaptation active
};
```

#### NeuronType
```cpp
enum class NeuronType {
    Sensory,
    Interneuron,
    Motor,
    Modulatory,
    Internal
};
```

#### LogLevel
```cpp
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};
```

#### ConfigSource
```cpp
enum class ConfigSource {
    Default,      // Hardcoded defaults
    File,         // Load from configuration file
    CommandLine,  // Command line arguments
    Runtime       // Programmatically set during execution
};
```

#### DevelopmentalStage
```cpp
enum class DevelopmentalStage {
    Initial,      // Newborn, high plasticity
    CriticalPeriod, // Sensitive learning period
    Maturation,   // Development of adult properties
    Adult         // Mature, stable network
};
```

## 2. Core Classes

### 2.1 Brain Class

**Header**: `src/brain/Brain.hpp`

The main neural computation engine coordinating the entire NLM system.

#### Constructor
```cpp
explicit Brain(std::shared_ptr<Config> config);
```

#### Public Methods

**Initialization and Configuration:**
```cpp
bool initialize();                    // Initialize brain with current config
void reset();                         // Reset all neurons to initial state
bool save(const std::string& filepath) const;  // Save state to file
bool load(const std::string& filepath);       // Load state from file
```

**Main Simulation Loop:**
```cpp
void step(SimulationStep currentStep);           // Step with default timestep
void step(SimulationStep currentStep, Timestamp currentTime);  // Step with explicit time
```

**Input/Output Processing:**
```cpp
void receiveSensoryInput(const SensoryInput& input);  // Process external input
void injectCurrent(NeuronId neuron, MembranePotential current);  // Inject current
void injectCurrentToNeurons(NeuronType type, MembranePotential current);  // Inject to all neurons of type
std::unique_ptr<Action> produceAction();  // Generate motor output
```

**System Access:**
```cpp
SpikeSystem* getSpikeSystem();                    // Access spike processing
STDP* getSTDP();                                 // Access STDP plasticity
Hebbian* getHebbian();                           // Access Hebbian plasticity
StructuralPlasticity* getStructuralPlasticity(); // Access structural plasticity
```

**Memory System Access:**
```cpp
NeuralWorkingMemory* getWorkingMemory();         // Transient memory
NeuralEpisodicMemory* getEpisodicMemory();       // Experience storage
NeuralAssociativeMemory* getAssociativeMemory(); // Pattern associations
```

**Prediction System Access:**
```cpp
PredictionSystem* getPredictionSystem();        // Temporal sequence learning
```

**Cognition System Access:**
```cpp
NeuralPlanner* getPlanner();                     // Action planning
ConceptFormation* getConceptFormation();         // Pattern discovery
AttentionalSelection* getAttention();           // Competitive attention
```

**Development System Access:**
```cpp
DevelopmentSystem* getDevelopmentSystem();      // Stage progression
DevelopmentalStage getDevelopmentalStage() const;
void setDevelopmentalStage(DevelopmentalStage stage);
```

**Neuromodulation System Access:**
```cpp
Dopamine* getDopamine();                        // Reward signals
Curiosity* getCuriosity();                       // Exploration motivation
Novelty* getNovelty();                            // Novelty detection
PredictionError* getPredictionErrorSignal();    // Error signals
```

**Statistics and Monitoring:**
```cpp
float getExcitationInhibitionRatio() const;     // E/I balance
size_t getTotalSpikeCount() const;              // Total spikes fired
float getAverageFiringRate() const;             // Network firing rate
```

#### Private Methods

```cpp
static bool initDevelopmentSystem(Impl* impl);
static bool initNeuromodulationSystems(Impl* impl);
static void createRegions(Impl* impl, size_t neuronCount, size_t regionCount);
static void initializeConnectivity(Impl* impl, float connectionProbability);
```

### 2.2 Config Class

**Header**: `src/core/Config/Config.hpp`

Hierarchical configuration management system.

#### Constructor and Destructor
```cpp
Config();
~Config();
```

#### Configuration Loading
```cpp
bool loadFromFile(const std::string& filepath);     // Load from JSON/YAML/INI
bool loadFromArgs(int argc, char** argv);           // Load from command line
```

#### Type-Safe Parameter Access
```cpp
template<typename T>
std::optional<T> get(const std::string& key) const;  // Get with optional result

template<typename T>
T getOr(const std::string& key, const T& defaultValue) const;  // Get with default
```

#### Parameter Setting
```cpp
void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
```

#### Configuration Management
```cpp
bool has(const std::string& key) const;             // Check if key exists
void remove(const std::string& key);               // Remove key
void clear();                                      // Clear all configuration
std::vector<std::string> getKeys() const;          // Get all keys
std::string summary() const;                       // Get configuration summary
```

### 2.3 Logger Class

**Header**: `src/core/Logger/Logger.hpp`

Flexible logging system with multiple output formats.

#### Constructor and Destructor
```cpp
Logger();
~Logger();
```

#### Logger Management
```cpp
void addLogger(std::shared_ptr<ILogger> logger);   // Add logging sink
void clearLoggers();                              // Remove all loggers
void setLevel(LogLevel level);                    // Set global log level
```

#### Logging Functions
```cpp
void log(LogLevel level, const std::string& message, 
         const std::string& file = "", int line = 0, 
         const std::string& function = "");

void debug(const std::string& message, const std::string& file = "", 
           int line = 0, const std::string& function = "");
void info(const std::string& message, const std::string& file = "", 
          int line = 0, const std::string& function = "");
void warning(const std::string& message, const std::string& file = "", 
             int line = 0, const std::string& function = "");
void error(const std::string& message, const std::string& file = "", 
           int line = 0, const std::string& function = "");
void critical(const std::string& message, const std::string& file = "", 
              int line = 0, const std::string& function = "");
```

#### Global Logger Access
```cpp
static Logger& getGlobal();                        // Get global logger instance
static void setGlobal(std::shared_ptr<Logger> logger);  // Set global logger
```

#### Convenience Macros
```cpp
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)
```

### 2.4 Neuron Class

**Header**: `src/brain/Neuron.hpp`

Individual neuron implementing Leaky Integrate-and-Fire dynamics.

#### Constructor and Destructor
```cpp
explicit Neuron(NeuronId id);
~Neuron();
```

#### Identity and Classification
```cpp
NeuronId getId() const;                          // Get neuron ID
NeuronType getType() const;                      // Get neuron type
void setType(NeuronType type);                    // Set neuron type
```

#### State Access
```cpp
const NeuronState& getState() const;             // Get read-only state
NeuronState& getState();                         // Get mutable state
```

#### Membrane Potential Management
```cpp
MembranePotential getMembranePotential() const;
void setMembranePotential(MembranePotential potential);
void addToMembranePotential(MembranePotential delta);
```

#### Firing State Management
```cpp
bool isFiring() const;                           // Check if neuron is firing
bool isRefractory() const;                        // Check if in refractory period
void setFiringState(FiringState state);
void setRefractoryPeriod(uint32_t steps);
void decrementRefractory();                       // Called each simulation step
```

#### Rate-Based Properties
```cpp
FiringRate getFiringRate() const;
void setFiringRate(FiringRate rate);
```

#### LIF Parameters
```cpp
void setLeakConductance(MembranePotential conductance);
MembranePotential getLeakConductance() const;
uint32_t getRefractoryPeriod() const;
void setRestingPotential(MembranePotential potential);
MembranePotential getRestingPotential() const;
void setResetPotential(MembranePotential potential);
```

#### Spike Detection
```cpp
bool checkThreshold() const;                      // Check if threshold reached
float getLastSpikeTime() const;                   // Get last spike time
```

#### LIF Integration
```cpp
bool stepLIF(Timestamp currentTime, TimestepDuration dt);  // Core LIF computation
```

#### Input Processing
```cpp
void receiveExcitatoryInput(MembranePotential amplitude);
void receiveInhibitoryInput(MembranePotential amplitude);
void receiveModulatoryInput(MembranePotential amplitude);
void injectCurrent(MembranePotential current);
MembranePotential getTotalCurrent() const;
void clearTotalCurrent();
```

#### Spike History and STDP
```cpp
void recordSpike(Timestamp timestamp);           // Record spike for STDP
const std::vector<Timestamp>& getSpikeHistory() const;
void clearSpikeHistory();
```

#### Synaptic Connectivity
```cpp
void addIncomingSynapse(SynapseHandle handle);
void addOutgoingSynapse(SynapseHandle handle);
const std::vector<SynapseHandle>& getIncomingSynapses() const;
const std::vector<SynapseHandle>& getOutgoingSynapses() const;
```

#### Plasticity Management
```cpp
const PlasticityFlags& getPlasticityFlags() const;
PlasticityFlags& getPlasticityFlags();
void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
```

#### Regional Organization
```cpp
void setRegionId(RegionId region);
RegionId getRegionId() const;
void setPopulationId(PopulationId population);
PopulationId getPopulationId() const;
```

#### Simulation Control
```cpp
void step(Timestamp currentTime);                // Default timestep (1ms)
void reset();                                    // Reset to initial state
void initializeRandom(RandomGenerator& rng);     // Initialize with random parameters
```

### 2.5 NeuralAssociativeMemory Class

**Header**: `src/memory/NeuralAssociativeMemory.hpp`

Pattern association and retrieval system.

#### Constructor and Destructor
```cpp
NeuralAssociativeMemory();
~NeuralAssociativeMemory();
```

#### Initialization
```cpp
void initialize(Brain* brain);  // Initialize with brain reference
```

#### Association Formation
```cpp
void associate(const std::vector<float>& patternA,
               const std::vector<float>& patternB,
               float strength = 1.0f);
void associateFromExperience(const EpisodicMemoryItem& episode);
```

#### Pattern Retrieval
```cpp
std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern,
                                         size_t maxResults = 5) const;
```

#### Association Management
```cpp
float getAssociationStrength(const std::vector<float>& patternA,
                             const std::vector<float>& patternB) const;
void updateAssociation(const std::vector<float>& patternA,
                       const std::vector<float>& patternB,
                       float delta);
```

#### Spreading Activation
```cpp
std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern,
                                                  size_t steps = 2) const;
```

#### Utility Functions
```cpp
void clear();                           // Clear all associations
size_t getAssociationCount() const;     // Get number of associations
```

### 2.6 STDP Class

**Header**: `src/plasticity/STDP.hpp`

Spike-Timing-Dependent Plasticity implementation.

#### Constructor and Destructor
```cpp
STDP();
~STDP() override;
```

#### Plasticity Update
```cpp
void update(Synapse* synapse,
            const std::vector<Timestamp>& preSpikes,
            const std::vector<Timestamp>& postSpikes,
            TimestepDuration dt) override;
void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
```

#### Parameter Configuration
```cpp
void setLTPWeight(float weight);      // Set long-term potentiation weight
float getLTPWeight() const;
void setLTDWeight(float weight);      // Set long-term depression weight  
float getLTDWeight() const;
void setTimeConstant(float tau);      // Set STDP time constant (ms)
float getTimeConstant() const;
void configure(float ltpWeight, float ltdWeight, float tau);  // Configure from config
```

#### Base Class Interface
```cpp
const char* getName() const override;
```

## 3. Other Important Classes

### 3.1 PlasticityRule (Abstract Base Class)

**Header**: `src/plasticity/PlasticityRule.hpp`

Abstract base class for all plasticity rules.

#### Pure Virtual Methods
```cpp
virtual void update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) = 0;

virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;

virtual const char* getName() const = 0;
```

### 3.2 NeuralDynamics (Abstract Base Class)

**Header**: `src/dynamics/NeuralDynamics.hpp`

Abstract base class for neuron dynamics models.

### 3.3 SpikeSystem

**Header**: `src/dynamics/SpikeSystem.hpp`

Event-driven spike processing system.

### 3.4 RandomGenerator

**Header**: `src/core/Random/Random.hpp`

Deterministic random number generator.

### 3.5 SimulationClock

**Header**: `src/core/SimulationClock/SimulationClock.hpp`

High-resolution timing system.

## 4. Memory System Classes

### 4.1 Working Memory System

#### NeuralWorkingMemory
Pattern completion system for temporary information storage.

### 4.2 Episodic Memory System

#### NeuralEpisodicMemory  
Experience encoding and retrieval system.

### 4.3 Associative Memory System

#### NeuralAssociativeMemory
Pattern association and recall system (see detailed documentation above)

## 5. Cognitive System Classes

### 5.1 Prediction System

#### NeuralPrediction
Temporal sequence learning and prediction engine.

#### ActionConsequencePredictor
Action-effect prediction system.

#### PredictionErrorSignal
Error computation for learning.

### 5.2 Cognitive Processing

#### AttentionalSelection
Competitive attention mechanism (NOT Transformer QKV).

#### NeuralPlanner
Predictive action selection system.

#### SelfModel
Sensorimotor self-awareness system.

#### SocialLearning
Observation and imitation learning system.

## 6. Neuromodulation System Classes

### 6.1 Basic Neuromodulators

#### Dopamine
Reward and reinforcement signaling.

#### Novelty
Novelty detection and signaling.

#### Curiosity
Exploration motivation system.

#### PredictionError
Prediction error signal for learning.

## 7. Development System Classes

### 7.1 Developmental Plasticity

#### DevelopmentSystem
Orchestration of developmental stages and processes.

#### Synaptogenesis
New synapse formation during development.

#### Pruning
Weak synapse removal for efficiency.

#### Maturation
Development of adult-level properties.

## 8. Performance Infrastructure Classes

### 8.1 Event-Driven Processing

#### EventQueue
Lock-free ring buffer for spike events.

#### TimeBucketQueue
Efficient delayed spike processing.

### 8.2 Memory Management

#### MemoryPool
Pre-allocated neuron and synapse storage.

#### SparseConnectivity
Efficient connectivity representation for large networks.

### 8.3 Parallel Processing

#### ThreadPool
Lock-free task queue for parallel processing.

#### ParallelFor
Parallel loop execution with chunking.

### 8.4 Vectorization

#### SIMDVectorization
Hardware-accelerated neural computations.

## 9. Helper Classes and Structures

### 9.1 Synapse

Individual synaptic connection with plasticity support.

### 9.2 NeuralPopulation

Group of neurons with shared properties.

### 9.3 NeuralRegion

Logical grouping of neural populations.

### 9.4 Environment

Abstract environment interface for world interaction.

### 9.5 Observation

Wrapper for sensory observations.

### 9.6 Action

Wrapper for motor commands.

## 10. Factory and Utility Classes

### 10.1 BrainFactory

Factory for creating configured brain instances.

### 10.2 ConfigFactory

Factory for creating configuration from various sources.

### 10.3 LoggerFactory

Factory for creating logging configurations.

## 11. Error Handling

### 11.1 Error Types

#### ErrorHandling
Centralized error reporting and recovery.

### 11.2 Exception Classes

#### NLMException
Base exception class for all NLM errors.

#### ConfigurationException
Configuration-related errors.

#### NeuralException
Neural computation errors.

#### PerformanceException
Performance-related errors.

## 12. Template and Utility Functions

### 12.1 Mathematical Utilities

#### VectorMath
Vector operations optimized for neural computation.

#### MathUtils
Common mathematical functions with numerical stability.

### 12.2 Type Conversions

#### TypeTraits
Compile-time type information and conversions.

#### NumericLimits
Extended numeric limits with neural-specific values.

## 13. Implementation Notes

### 13.1 Memory Layout

The NLM framework uses Structure-of-Arrays (SoA) for memory efficiency:

- **NeuronState**: Each field in separate contiguous array
- **Synaptic weights**: Dense or sparse depending on network size
- **Spike history**: Circular buffer for bounded memory usage

### 13.2 Thread Safety

Key thread safety considerations:

- **Read operations**: Generally thread-safe
- **Write operations**: Require synchronization
- **Global state**: Protected by mutexes where appropriate
- **Lock-free structures**: Used for high-performance paths

### 13.3 Numerical Stability

Important numerical considerations:

- **Float precision**: Use `MembranePotential` and `SynapticWeight` types
- **Clamp operations**: Prevent runaway values
- **Time integration**: Exponential Euler for stability
- **Delta calculations**: Absolute threshold for updates

## 14. Performance Characteristics

### 14.1 Computational Complexity

- **Neuron update**: O(1) per neuron
- **Synaptic update**: O(k) where k = connections per neuron
- **Memory access**: O(1) for direct, O(k²) for spreading activation

### 14.2 Memory Usage

| Component | Memory per Instance | Typical Scaling |
|-----------|---------------------|-----------------|
| Neuron | ~64 bytes (SoA layout) | 64 × neuron_count |
| Synapse | ~32 bytes (dense) / ~24 bytes (sparse) | Varies by connectivity |
| SpikeHistory | ~8 × history_length bytes | Bounded by MAX_SPIKE_HISTORY |

### 14.3 Bandwidth Requirements

- **Memory bandwidth**: Critical for large networks
- **Cache locality**: SoA layout optimizes for cache
- **NUMA awareness**: Thread-local storage for NUMA systems

## 15. Extension Points

### 15.1 Adding New Dynamics Models

```cpp
class CustomDynamics : public NeuralDynamics {
public:
    bool step(NeuronState& state, const std::vector<float>& inputs, 
              TimestepDuration dt, Timestamp currentTime) override {
        // Custom dynamics implementation
        return true; // return true if neuron fired
    }
    
    const char* getName() const override { return "CustomDynamics"; }
};
```

### 15.2 Adding New Plasticity Rules

```cpp
class CustomPlasticity : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        // Custom plasticity implementation
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        // Custom weight application
    }
    
    const char* getName() const override { return "CustomPlasticity"; }
};
```

### 15.3 Adding New Memory Systems

```cpp
class CustomMemory : public MemorySystem {
public:
    void store(const std::vector<float>& pattern, MemoryId id) override {
        // Custom storage implementation
    }
    
    std::vector<float> retrieve(MemoryId id) const override {
        // Custom retrieval implementation
        return {};
    }
    
    void clear() override {
        // Custom clear implementation
    }
    
    const char* getName() const override { return "CustomMemory"; }
};
```

## 16. Migration Guide

### 16.1 From Version 5 to 6

Key changes in version 6:

1. **Configuration**: Changed from JSON to YAML support
2. **Memory**: New sparse connectivity implementation
3. **Threading**: Lock-free queues for better scalability
4. **SIMD**: AVX-512 optimizations added

### 16.2 Migration Examples

See migration examples in `examples/migration/` directory.

## 17. Frequently Asked Questions

### 17.1 Performance Questions

**Q**: Why is my simulation slow?
**A**: Check memory bandwidth, cache misses, and thread contention.

**Q**: How many neurons can I simulate?
**A**: Depends on hardware and connectivity pattern. See performance documentation.

### 17.2 Configuration Questions

**Q**: How do I add a new configuration parameter?
**A**: Add to config file and update configuration parser.

**Q**: How do I validate configuration?
**A**: Add validation functions and call them after loading.

### 17.3 Implementation Questions

**Q**: How do I add a new neuron type?
**A**: Implement NeuronType enum and update factory methods.

**Q**: How do I modify plasticity?
**A**: Create new PlasticityRule subclass and register with Brain.

## 18. References and Bibliography

1. Dayan, P., & Abbott, L. F. (2001). Theoretical Neuroscience. MIT Press.
2. Gerstner, W., Kistler, W. M., Naud, R., & Paninski, L. (2014). Neuronal Dynamics.
3. Abbott, L. F., & Regehr, W. G. (2004). Synaptic computation. Nature.
4. Song, S., Miller, K. D., & Abbott, L. F. (2000). Competitive learning in neural networks.

## Index

- A: Associative Memory
- C: Configuration
- D: Development
- E: Error Handling
- L: Logging
- M: Memory Systems
- N: Neural Dynamics
- P: Prediction System
- R: Random Number Generation
- S: Spiking Neural Networks
- T: Thread Safety
- V: Vectorization

This API reference is continuously updated with new features and improvements. For the latest documentation, visit the NLM documentation website.