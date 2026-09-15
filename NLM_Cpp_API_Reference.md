# NLM C++ API Reference

## Overview

This document provides comprehensive API documentation for the NLM (Neural Learning Machine) C++ neural simulation framework. The NLM is a brain-inspired spiking neural network simulator implementing Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, multiple plasticity mechanisms, and integrated memory, prediction, and neuromodulation systems.

## Core Types and Enumerations

### NeuronType
```cpp
enum class NeuronType : uint8_t {
    Excitatory,     // Excitatory neuron (positive effect on postsynaptic neurons)
    Inhibitory,     // Inhibitory neuron (negative effect on postsynaptic neurons)
    Modulatory,     // Modulatory neuron (affects plasticity and other neurons)
    Sensory,        // Sensory neuron (receives external input)
    Motor,          // Motor neuron (produces action output)
    Internal        // Internal neuron (for internal processing)
};
```

### SynapseType
```cpp
enum class SynapseType : uint8_t {
    Excitatory,    // Excitatory synapse (depolarizes postsynaptic neuron)
    Inhibitory,    // Inhibitory synapse (hyperpolarizes postsynaptic neuron)
    Modulatory,    // Modulatory synapse (affects postsynaptic plasticity)
    Electrical,    // Electrical synapse (gap junction)
    GapJunction   // Electrical coupling between neurons
};
```

### DevelopmentalStage
```cpp
enum class DevelopmentalStage : uint8_t {
    Initial,        // Initial development stage (high plasticity)
    CriticalPeriod, // Critical period (sensitive learning phase)
    Maturation,     // Maturation phase (development toward adult)
    Adult,          // Adult stage (stable function)
    Aging           // Aging phase (declining plasticity)
};
```

### FiringState
```cpp
enum class FiringState : uint8_t {
    Resting,    // Not currently firing
    Active,     // Currently firing
    Refractory, // In refractory period after spike
    Inhibited   // Inhibited from firing
};
```

### ActionType
```cpp
enum class ActionType {
    MoveForward,   // Move forward
    MoveBackward,  // Move backward
    TurnLeft,      // Turn left
    TurnRight,     // Turn right
    Look,          // Look forward
    LookUp,        // Look up
    LookDown,      // Look down
    Interact,      // Interact with object
    Eat,           // Eat from resource
    Drink,         // Drink from water source
    Rest,          // Rest/idle
    Wait,          // Wait
    Custom        // Custom action with parameters
};
```

### WorldObjectType
```cpp
enum class WorldObjectType {
    Empty,     // Empty cell
    Resource,  // Resource (food, water, etc.)
    Hazard,    // Hazardous object
    Wall,      // Wall
    Marker     // Marker point
};
```

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
    
    constexpr uint64_t index() const { return value; }
};
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
    
    constexpr uint64_t index() const { return value; }
};
```

### PlasticityFlags
```cpp
struct PlasticityFlags {
    bool hebbian;          // Hebbian plasticity enabled
    bool stdp;             // STDP plasticity enabled
    bool reward_modulated; // Reward-modulated plasticity enabled
    bool structural;       // Structural plasticity enabled
    bool eligible;         // Currently eligible for plasticity
};
```

### Simulation Types
```cpp
using SimulationStep = uint64_t;          // Simulation step counter
using Timestamp = double;                  // Simulation time in seconds
using TimestepDuration = double;           // Time step duration in seconds
using NeuronIndex = uint64_t;              // Neuron index in array
using SynapseIndex = uint64_t;             // Synapse index in array
using PopulationIndex = uint64_t;          // Population index
using RegionIndex = uint64_t;              // Region index
using SynapticWeight = float;              // Synaptic weight (-1.0 to 1.0)
using MembranePotential = float;           // Membrane voltage (mV)
using FiringRate = float;                  // Firing rate (Hz)
using Delay = uint32_t;                    // Connection delay in steps
```

## Core Classes

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
    
    // Load from file (with auto-detect of format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON file
    bool loadFromJSON(const std::string& filepath);
    
    // Load from YAML file
    bool loadFromYAML(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

#### Config Public Members
```cpp
enum class ConfigSource {
    Default,      // Default configuration
    File,         // Loaded from file
    CommandLine,  // Loaded from command line
    Runtime       // Set at runtime
};

using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;
```

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
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset brain state
    void reset();
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target,
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};
```

#### Brain Main Methods

**Initialization and Simulation**
```cpp
// Create brain with configuration
Brain brain(config);
brain.initialize();

// Simulation loop
for (size_t step = 0; step < max_steps; ++step) {
    brain.step(step, current_time);
}
```

**Sensory and Motor Interfaces**
```cpp
// Receive sensory input
brain.receiveSensoryInput(sensory_input);

// Produce action
auto action = brain.produceAction();
```

**State Management**
```cpp
// Reset
brain.reset();

// Save/Load
brain.save("checkpoint.bin");
brain.load("checkpoint.bin");
```

**Statistics and Monitoring**
```cpp
// Get statistics
float ei_ratio = brain.getExcitationInhibitionRatio();
size_t total_spikes = brain.getTotalSpikeCount();
size_t firing_neurons = brain.getFiringNeuronCount();
float avg_rate = brain.getAverageFiringRate();
```

**Memory Systems Access**
```cpp
// Access integrated memory systems
auto* working_memory = brain.getWorkingMemory();
auto* episodic_memory = brain.getEpisodicMemory();
auto* associative_memory = brain.getAssociativeMemory();
```

**Prediction and Cognition**
```cpp
// Access prediction system
auto* prediction = brain.getPredictionSystem();

// Access cognition systems
auto* planner = brain.getPlanner();
auto* concepts = brain.getConceptFormation();
auto* attention = brain.getAttention();
```

**Development and Neuromodulation**
```cpp
// Development
auto* dev_system = brain.getDevelopmentSystem();
brain.setDevelopmentalStage(DevelopmentalStage::Adult);

// Neuromodulators
auto* dopamine = brain.getDopamine();
auto* curiosity = brain.getCuriosity();
auto* novelty = brain.getNovelty();
```

### NeuralRegion
```cpp
class NeuralRegion {
public:
    explicit NeuralRegion(RegionId id);
    NeuralRegion(RegionId id, const std::string& name);
    ~NeuralRegion();
    
    // Disable copying, enable moving
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    NeuralRegion(NeuralRegion&&) noexcept;
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    // Identity
    RegionId getId() const;
    const std::string& getName() const;
    void setName(const std::string& name);
    
    // Population management
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    NeuralPopulation* getPopulation(PopulationId id);
    const NeuralPopulation* getPopulation(PopulationId id) const;
    size_t getPopulationCount() const;
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    std::vector<NeuralPopulation*> getAllPopulations();
    
    // Synapse management (local connections within this region)
    SynapseId addSynapse(NeuronId source, NeuronId destination,
                         SynapticWeight weight = 0.0f, Delay delay = 1);
    Synapse* getSynapse(SynapseId id);
    const Synapse* getSynapse(SynapseId id) const;
    size_t getSynapseCount() const;
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
    // Connectivity queries
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Local connectivity statistics
    float getAverageSynapticWeight() const;
    float getSynapticDensity() const;  // fraction of possible connections
    
    // Step all populations and synapses
    void step(Timestamp currentTime);
    
    // Reset all neurons and synapses
    void reset();
    
    // Initialize connectivity
    void initializeRandomConnectivity(RandomGenerator& rng,
                                      float connectionProbability,
                                      float meanWeight,
                                      float weightVariance);
    
    // Get all neurons across all populations
    std::vector<Neuron*> getAllNeurons();
    std::vector<const Neuron*> getAllNeurons() const;
    
private:
    struct Impl;
    Impl* pImpl;
};
```

### NeuralPopulation
```cpp
class NeuralPopulation {
public:
    NeuralPopulation(PopulationId id, RegionId regionId, size_t size, NeuronType type);
    ~NeuralPopulation();
    
    // Disable copying, enable moving
    NeuralPopulation(const NeuralPopulation&) = delete;
    NeuralPopulation& operator=(const NeuralPopulation&) = delete;
    NeuralPopulation(NeuralPopulation&&) noexcept;
    NeuralPopulation& operator=(NeuralPopulation&&) noexcept;
    
    // Identity
    PopulationId getId() const;
    RegionId getRegionId() const;
    size_t getSize() const;
    NeuronType getType() const;
    
    // Neuron access
    Neuron* getNeuron(NeuronId id);
    const Neuron* getNeuron(NeuronId id) const;
    std::vector<Neuron*> getAllNeurons();
    std::vector<const Neuron*> getAllNeurons() const;
    
    // Statistics
    size_t getNeuronCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    float getTotalActivity() const;
    
    // Connectivity
    void addSynapse(NeuronId target, SynapseType type, SynapticWeight weight = 1.0f,
                   Delay delay = 1, const PlasticityFlags& flags = PlasticityFlags());
    void addRecurrentSynapse(NeuronId target, SynapseType type, SynapticWeight weight = 0.1f,
                            Delay delay = 1, const PlasticityFlags& flags = PlasticityFlags());
    void removeSynapse(NeuronId target);
    
    // Get synapses
    std::vector<Synapse*> getSynapsesFrom(NeuronId source) const;
    std::vector<Synapse*> getSynapsesTo(NeuronId target) const;
    Synapse* getSynapse(NeuronId source, NeuronId target) const;
    
    // Properties
    float getExcitatoryRatio() const;
    float getInhibitoryRatio() const;
    float getModulatoryRatio() const;
    
    // Step population
    void step(Timestamp currentTime, TimestepDuration dt);
    
    // Reset population
    void reset();
    
    // Statistical analysis
    float getFiringRate(NeuronId neuron) const;
    MembranePotential getMembranePotential(NeuronId neuron) const;
    FiringState getFiringState(NeuronId neuron) const;
    
    // Pattern analysis
    std::vector<float> getActivationPattern() const;
    std::vector<float> getPhasePattern() const;
    
    // Connectivity analysis
    float getAverageSynapticWeight() const;
    float getSynapticDensity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### Neuron
```cpp
class Neuron {
public:
    Neuron(NeuronId id, NeuronType type, RegionId regionId);
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&) = delete;
    Neuron& operator=(Neuron&) = delete;
    
    // Identity
    NeuronId getId() const;
    NeuronType getType() const;
    RegionId getRegionId() const;
    PopulationId getPopulationId() const;
    void setPopulationId(PopulationId id);
    
    // State
    void stepLIF(Timestamp currentTime, TimestepDuration dt);
    void reset();
    void inhibit(float amount, Timestamp until);
    void setRefractoryPeriod(Timestamp duration);
    
    // Membrane properties
    MembranePotential getMembranePotential() const;
    void setMembranePotential(float v);
    MembranePotential getThreshold() const;
    void setThreshold(float v);
    MembranePotential getRestPotential() const;
    void setRestPotential(float v);
    MembranePotential getResetPotential() const;
    void setResetPotential(float v);
    TimestepDuration getMembraneTimeConstant() const;
    void setMembraneTimeConstant(float tau);
    TimestepDuration getRefractoryPeriod() const;
    Timestamp getRefractoryUntil() const;
    
    // Spiking
    bool isFiring() const;
    Timestamp getLastSpikeTime() const;
    float getAverageFiringRate() const;
    
    // Input/output
    void addInput(NeuronId source, Synapse* synapse);
    void removeInput(NeuronId source);
    void addOutput(Synapse* synapse);
    void removeOutput(Synapse* synapse);
    
    // Statistics
    size_t getInputCount() const;
    size_t getOutputCount() const;
    float getTotalInputCurrent() const;
    float getTotalOutputCurrent() const;
    float getExcitatoryInputRatio() const;
    
    // Properties
    float getExcitability() const;
    void setExcitability(float value);
    float getInhibitorySensitivity() const;
    void setInhibitorySensitivity(float value);
    
    // Activity patterns
    bool isActive() const;
    bool isInhibited() const;
    FiringState getFiringState() const;
    
    // Homeostatic properties
    float getHomeostaticSetPoint() const;
    void setHomeostaticSetPoint(float setpoint);
    float getHomeostaticRate() const;
    void setHomeostaticRate(float rate);
    
    // Plasticity
    void applyPlasticity(float deltaWeight, const PlasticityFlags& flags);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### Synapse
```cpp
class Synapse {
public:
    Synapse(SynapseId id, NeuronId source, NeuronId target,
            SynapseType type, SynapticWeight weight = 1.0f,
            Delay delay = 1, const PlasticityFlags& flags = PlasticityFlags());
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity
    SynapseId getId() const;
    NeuronId getSource() const;
    NeuronId getTarget() const;
    SynapseType getType() const;
    void setType(SynapseType type);
    
    // State
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    Delay getDelay() const;
    void setDelay(Delay delay);
    bool isPotentiationEnabled() const;
    bool isDepressionEnabled() const;
    const PlasticityFlags& getPlasticityFlags() const;
    void setPlasticityFlags(const PlasticityFlags& flags);
    
    // Transmission
    bool transmit(Neuron* targetNeuron, Timestamp timestamp, SimulationStep step);
    bool isTransmitted(Timestamp currentTime) const;
    
    // Statistics
    size_t getTransmissionCount() const;
    float getTransmissionEfficiency() const;
    float getEffectiveWeight() const;
    float getLastTransmissionTime() const;
    
    // Plasticity eligibility
    void setEligibilityTrace(float value);
    float getEligibilityTrace() const;
    void decayEligibilityTrace(float rate);
    
    // NMDA receptor dynamics
    float getNMDAConductance() const;
    void setNMDAConductance(float g);
    float getNMDAMagnesiumBlock() const;
    
    // Short-term plasticity
    float getUtilization() const;
    void setUtilization(float u);
    float getRecoveryRate() const;
    void setRecoveryRate(float rate);
    float getTimeConstant() const;
    void setTimeConstant(float tau);
    
    // Stress and fatigue
    float getStressLevel() const;
    void setStressLevel(float stress);
    float getFatigueLevel() const;
    void setFatigueLevel(float fatigue);
    
    // History tracking
    void recordTransmission(Timestamp timestamp);
    std::vector<Timestamp> getTransmissionHistory() const;
    
    // Analysis
    float getWeightChangeRate() const;
    float getTransmissionPatternComplexity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### STDP
```cpp
class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // STDP parameters - biologically realistic
    void setLTPWeight(float weight);    // A+ for potentiation (~10-20ms window)
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // A- for depression (~30-50ms window)
    float getLTDWeight() const;
    void setTimeConstantPlus(float tau);    // τ+ for potentiation time constant
    float getTimeConstantPlus() const;
    void setTimeConstantMinus(float tau);   // τ- for depression time constant
    float getTimeConstantMinus() const;
    void setTauPlus(float tauPlus);         // Alternative parameter name
    float getTauPlus() const;
    void setTauMinus(float tauMinus);        // Alternative parameter name
    float getTauMinus() const;
    void setHomeostaticFactor(float factor); // Homeostatic scaling factor
    float getHomeostaticFactor() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tauPlus, float tauMinus,
                   float homeostaticFactor = 1.0f);
    
    // Eligibility trace management
    void updateEligibilityTrace(Synapse* synapse, float delta, float decayRate = 0.1f);
    float getTraceValue(const Synapse* synapse) const;
    
    // Triplet STDP parameters
    void setTripletA1(float a1);      // A1 parameter for triplet STDP
    void setTripletA2(float a2);      // A2 parameter for triplet STDP
    void setTripletTau1(float tau1);  // τ1 for triplet STDP
    void setTripletTau2(float tau2);  // τ2 for triplet STDP
    
private:
    struct Impl;
    Impl* pImpl;
};
```

### RewardModulatedSTDP
```cpp
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Dopamine modulation parameters
    void setDopamineLevel(float level);
    float getDopamineLevel() const;
    void setLearningRateModulation(float mod);
    float getLearningRateModulation() const;
    void setRewardPredictionError(float rpe);
    float getRewardPredictionError() const;
    
    // Credit assignment window
    void setCreditAssignmentWindow(float window);
    float getCreditAssignmentWindow() const;
    
    // Eligibility trace dynamics
    void updateEligibilityTrace(Synapse* synapse, float delta, float dt);
    void decayTraces(float decayRate);
    float getTraceValue(const Synapse* synapse) const;
    
    // Configure from dopamine system
    void configure(float ltpWeight, float ltdWeight, float tauPlus, float tauMinus,
                   float dopamineLevel = 0.0f, float rpe = 0.0f);
    
private:
    struct Impl;
    Impl* pImpl;
};
```

### Hebbian
```cpp
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Hebbian learning parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setCoactivityThreshold(float threshold);
    float getCoactivityThreshold() const;
    void setWeightBounds(float min, float max);
    void getWeightBounds(float& min, float& max) const;
    
    // Coactivity detection
    void recordActivity(NeuronId preNeuron, NeuronId postNeuron, Timestamp time);
    float getCoactivityScore(NeuronId preNeuron, NeuronId postNeuron) const;
    
    // Competition mechanisms
    void enableCompetition(bool enable);
    bool isCompetitionEnabled() const;
    void setCompetitionStrength(float strength);
    float getCompetitionStrength() const;
    
    // Short-term modulation
    void setStdpModulation(float mod);
    float getStdpModulation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};
```

### StructuralPlasticity
```cpp
class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Disable copying, enable moving
    StructuralPlasticity(const StructuralPlasticity&) = delete;
    StructuralPlasticity& operator=(const StructuralPlasticity&) = delete;
    StructuralPlasticity(StructuralPlasticity&&) noexcept;
    StructuralPlasticity& operator=(StructuralPlasticity&&) noexcept;
    
    // Synaptogenesis (new synapse formation)
    bool addSynapse(RegionId region, NeuronId source, NeuronId target,
                   SynapseType type = SynapseType::Excitatory,
                   float weight = 1.0f, Delay delay = 1,
                   const PlasticityFlags& flags = PlasticityFlags());
    
    // Pruning (synapse removal)
    bool removeSynapse(SynapseId id);
    bool pruneWeakSynapses(float threshold, RegionId region = INVALID_REGION_ID);
    size_t getSynapseCount(RegionId region = INVALID_REGION_ID) const;
    
    // Statistics
    float getSynaptogenesisRate() const;
    void setSynaptogenesisRate(float rate);
    float getPruningRate() const;
    void setPruningRate(float rate);
    float getCurrentSynaptogenesisProbability() const;
    float getCurrentPruningProbability() const;
    
    // Developmental control
    void setDevelopmentStage(DevelopmentalStage stage);
    DevelopmentalStage getDevelopmentStage() const;
    void updateDevelopmentModifiers(float age);
    
    // Homeostatic scaling
    void enableHomeostaticScaling(bool enable);
    bool isHomeostaticScalingEnabled() const;
    void setScalingTarget(float target);
    float getScalingTarget() const;
    void applyHomeostaticScaling(Brain* brain);
    
    // Stochastic process parameters
    void setRandomSeed(uint64_t seed);
    uint64_t getRandomSeed() const;
    void setSynaptogenesisStochasticity(float sigma);
    float getSynaptogenesisStochasticity() const;
    void setPruningStochasticity(float sigma);
    float getPruningStochasticity() const;
    
    // Configuration
    void configure(float synaptogenesisRate = 0.0001f,
                   float pruningRate = 0.00001f,
                   DevelopmentalStage initialStage = DevelopmentalStage::Initial);
    
    // Statistics
    size_t getTotalSynaptogenesisEvents() const;
    size_t getTotalPruningEvents() const;
    float getSynapseDensity(RegionId region = INVALID_REGION_ID) const;
    float getAverageSynapticWeight(RegionId region = INVALID_REGION_ID) const;
    
    // Update structural plasticity
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### Dopamine
```cpp
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    bool isActive() const override;
    
    // Real dopamine dynamics
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void signalPunishment(float magnitude);
    
    // Temporal difference learning
    void updatePrediction(float value);
    float computeTD(float reward, float currentValue, float nextValue);
    
    // Eligibility traces for reward-modulated learning
    void updateEligibilityTrace(float traceDecay = 0.9f);
    float getEligibilityTrace() const;
    void addEligibilityContribution(float weight, float preSpikeTime, float postSpikeTime);
    
    // Burst and pause dynamics
    void signalBurst(float magnitude, float duration);
    void signalPause(float duration);
    
    // Learning rate modulation
    float getLearningRateModulation() const;
    void setLearningRateModulation(float modulation);
    
    // Baseline adaptation
    void setBaseline(float baseline);
    float getBaseline() const;
    
    // Burst detection
    bool isBursting() const;
    float getBurstIntensity() const;
    
    // Getters and setters for fine-tuning
    void setDecayRate(float rate);
    void setReleaseRate(float rate);
    void setAdaptationRate(float rate);
    void setTDLearningRate(float rate);
    
    // Get prediction signals
    float getPredictedValue() const;
    float getValuePredictionError() const;
    
    // Get reward statistics
    float getAverageReward() const;
    float getTotalReward() const;
    size_t getRewardCount() const;
    
private:
    struct Impl;
    Impl* pImpl;
};
```

### Neuromodulator (Abstract Base)
```cpp
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (0.0 to 1.0 normalized)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns plasticity modulation factor (typically 0.0 to 1.0)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Reset neuromodulator to baseline
    virtual void reset() = 0;
    
    // Check if neuromodulator has a significant effect
    virtual bool isActive() const = 0;
    
protected:
    Neuromodulator() = default;
};
```

### PlasticityRule (Abstract Base)
```cpp
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    virtual void update(Synapse* synapse,
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) = 0;
    
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    virtual const char* getName() const = 0;
};
```

### NeuralWorkingMemory
```cpp
class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store information in working memory
    // The information is encoded as neural activity pattern
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    
    // Store specific neuron activation
    void storeToNeuron(NeuronId neuron, float activation);
    
    // Retrieve current working memory content as activity levels
    std::vector<float> retrieve() const;
    
    // Check if specific neuron is part of working memory
    bool contains(NeuronId neuron) const;
    
    // Get activation level of a specific neuron in working memory
    float getNeuronActivation(NeuronId neuron) const;
    
    // Update working memory (maintenance and decay)
    void update(TimestepDuration dt);
    
    // Clear working memory
    void clear();
    
    // Get number of active memory traces
    size_t getActiveTraces() const { return activeTraces_.size(); }
    
    // Get capacity
    size_t getCapacity() const { return capacity_; }
    void setCapacity(size_t cap) { capacity_ = cap; }
    
    // Decay rate for memory traces
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = rate; }
    
    // Get neurons currently in working memory
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }
    
    // Strengthen working memory representation (for rehearsal)
    void strengthenMemory(float factor);
    
    // Competition between memory traces
    void runCompetition();
    
    // Is this neuron part of the winning population?
    bool isWinning(NeuronId neuron) const;
    
    // Get overall memory activity level
    float getMemoryActivity() const;
    
private:
    // Create recurrent connection for maintenance
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);
    
    // Update recurrent connections for maintenance
    void updateRecurrentConnections();
    
    // Decay weak memory traces
    void decayWeakTraces();
    
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
    
    // Winner neurons (for competition)
    std::vector<NeuronId> winners_;
};
```

### NeuralEpisodicMemory
```cpp
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store episode
    void storeEpisode(const class SensoryInput& sensoryInput,
                     const class Action& action,
                     float reward,
                     Timestamp timestamp);
    
    // Retrieve episode
    std::unique_ptr<EpisodicMemoryEntry> retrieveEpisode(size_t episodeId) const;
    std::vector<size_t> retrieveSimilarEpisodes(const std::vector<float>& pattern,
                                                float similarityThreshold = 0.7f) const;
    
    // Replay episodes
    void replayEpisode(size_t episodeId, float strength = 1.0f);
    void replayAllEpisodes(float strength = 1.0f);
    
    // Consolidate episodes
    void consolidateOldEpisodes(Timestamp cutoffTime);
    void consolidateImportantEpisodes(const std::vector<size_t>& episodeIds);
    
    // Update memory (consolidation and replay)
    void update(TimestepDuration dt);
    
    // Clear memory
    void clear();
    
    // Statistics
    size_t getEpisodeCount() const;
    size_t getActiveReplayCount() const;
    float getMemoryUtilization() const;
    Timestamp getOldestEpisodeTime() const;
    Timestamp getNewestEpisodeTime() const;
    
    // Configuration
    void setReplayInterval(size_t steps);
    void setConsolidationInterval(size_t steps);
    void setReplayStrength(float strength);
    void setConsolidationThreshold(float threshold);
    
    // Get episodes
    const std::vector<std::unique_ptr<EpisodicMemoryEntry>>& getEpisodes() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### PredictionSystem
```cpp
class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update prediction system
    void update(TimestepDuration dt);
    
    // Train prediction on sensory input
    void train(const class SensoryInput& sensoryInput, Timestamp timestamp);
    
    // Get prediction
    float getPrediction(const std::vector<float>& inputPattern) const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getPredictionHistory() const;
    const std::vector<float>& getErrorHistory() const;
    
    // Reset system
    void reset();
    
    // Configuration
    void setPredictionWindow(size_t window);
    void setLearningRate(float rate);
    void setPredictionThreshold(float threshold);
    
    // Statistics
    size_t getTrainingSampleCount() const;
    float getPredictionAccuracy() const;
    float getAveragePredictionError() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### AgentBrain
```cpp
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};
```

### SimpleWorld
```cpp
class SimpleWorld {
public:
    SimpleWorld();
    ~SimpleWorld();
    
    // Configuration
    void configure(int width, int height, int visionWidth, int visionHeight);
    
    // Control
    void reset();
    void setRandomSeed(uint64_t seed);
    void update(TimestepDuration timestep);
    
    // Agent interaction
    void setAgentStart(float x, float y);
    void applyMotorCommand(const MotorCommand& cmd, Timestamp currentTime);
    
    // State access
    SensoryPercept getSensoryPercept() const;
    AgentBody getAgentBody() const;
    
    // World objects
    void addObject(const WorldObject& obj);
    void removeObject(float x, float y);
    bool isValidPosition(float x, float y) const;
    
    // Properties
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    float getMaxEnergy() const { return maxEnergy_; }
    void setMaxEnergy(float e) { maxEnergy_ = e; }
    float getEnergyDecayRate() const { return energyDecayRate_; }
    void setEnergyDecayRate(float r) { energyDecayRate_ = r; }
    float getSimulationTime() const { return simulationTime_; }
    
    // Get world objects at position
    const WorldObject* getObject(float x, float y) const;
    std::vector<WorldObject> getObjectsInRadius(float x, float y, float radius) const;
    
    // Agent movement and physics
    void moveAgent(float dx, float dy);
    void turnAgent(float angle);
    float getAgentDistance(float x, float y) const;
    float getAgentAngle(float x, float y) const;
    
    // Reward system
    void addResource(float x, float y, float value);
    void addHazard(float x, float y, float danger);
    void addWall(float x, float y);
    void addMarker(float x, float y, float value = 1.0f);
    
    // Get rewards and hazards
    float getResourceReward(float x, float y) const;
    float getHazardPenalty(float x, float y) const;
    bool isInHazardZone(float x, float y) const;
    
    // Visibility and perception
    std::vector<float> getVision(float agentX, float agentY, float visionAngle = 0.0f) const;
    std::vector<float> getTouchSensors(float agentX, float y) const;
    std::vector<float> getInternalSensors() const;
    std::vector<float> getProprioception() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### SensoryInput (Base Class)
```cpp
class SensoryInput {
public:
    virtual ~SensoryInput() = default;
    
    // Get type
    virtual std::string getType() const = 0;
    
    // Get data
    virtual const std::vector<float>& getData() const = 0;
    virtual int getDimensions() const = 0;
    
    // Get timestamp
    virtual double getTimestamp() const = 0;
    virtual void setTimestamp(double timestamp) = 0;
};
```

### Vision
```cpp
class Vision : public SensoryInput {
public:
    Vision();
    Vision(size_t width, size_t height, size_t channels = 3);
    
    // Set data
    void setData(const std::vector<float>& data);
    
    // Get dimensions
    size_t getWidth() const { return width_; }
    size_t getHeight() const { return height_; }
    size_t getChannels() const { return channels_; }
    
    // Get type
    std::string getType() const override { return "vision"; }
    
    // Get data
    const std::vector<float>& getData() const override { return data_; }
    int getDimensions() const override { return static_cast<int>(width_ * height_ * channels_); }
    
    // Get timestamp
    double getTimestamp() const override { return timestamp_; }
    void setTimestamp(double timestamp) override { timestamp_ = timestamp; }
    
    // Static factory methods
    static Vision createGrayscale(size_t width, size_t height);
    static Vision createColor(size_t width, size_t height);
    
private:
    size_t width_;
    size_t height_;
    size_t channels_;
    std::vector<float> data_;
    double timestamp_;
};
```

### Action
```cpp
class Action {
public:
    Action();
    Action(ActionType type);
    Action(ActionType type, const std::vector<float>& parameters);
    
    // Get type
    ActionType getType() const { return type_; }
    void setType(ActionType type) { type_ = type; }
    
    // Get parameters
    const std::vector<float>& getParameters() const { return parameters_; }
    void setParameters(const std::vector<float>& params) { parameters_ = params; }
    
    // Get parameter by index
    float getParameter(size_t index) const;
    void setParameter(size_t index, float value);
    
    // Get human-readable name
    std::string getName() const;
    
    // Clone
    std::unique_ptr<Action> clone() const;
    
    // Comparison
    bool operator==(const Action& other) const;
    bool operator!=(const Action& other) const { return !(*this == other); }
    
private:
    ActionType type_;
    std::vector<float> parameters_;
};
```

### SensoryPercept
```cpp
class SensoryPercept {
public:
    SensoryPercept();
    ~SensoryPercept() = default;
    
    // Vision (read-write access)
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v) { vision_ = v; }
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    void setVisionDimensions(size_t width, size_t height) {
        visionWidth_ = width;
        visionHeight_ = height;
        vision_.resize(width * height);
    }
    
    // Touch/proximity sensors (read-write access)
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t) { touch_ = t; }
    
    // Internal state signals (read-write access)
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i) { internal_ = i; }
    
    // Proprioception: body position/velocity signals (read-write access)
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; }
    
    // Audio-like signals (if enabled) (read-write access)
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a) { audio_ = a; }
    
    // Get all signals concatenated (for brain input)
    std::vector<float> getAllSignals() const;
    
    // Timestamp (read-write access)
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; }
    
    // Signal processing
    void addNoise(float intensity);
    void addDelay(double delay);
    void addFiltering(const std::string& type, float strength);
    
    // Comparison
    bool equals(const SensoryPercept& other, float threshold = 0.01f) const;
    
private:
    std::vector<float> vision_;
    size_t visionWidth_;
    size_t visionHeight_;
    std::vector<float> touch_;
    std::vector<float> internal_;
    std::vector<float> proprioception_;
    std::vector<float> audio_;
    double timestamp_;
};
```

### ActionResult
```cpp
class ActionResult {
public:
    ActionResult();
    ActionResult(float reward, bool success, const std::string& message = "");
    
    // Get results
    float getReward() const { return reward_; }
    bool isSuccess() const { return success_; }
    const std::string& getMessage() const { return message_; }
    
    // Set results
    void setReward(float reward) { reward_ = reward; }
    void setSuccess(bool success) { success_ = success; }
    void setMessage(const std::string& message) { message_ = message; }
    
    // Validation
    bool isValid() const { return success_ || reward_ > 0; }
    float getValue() const { return success_ ? reward_ : -reward_ * 0.5f; }
    
    // Comparison
    bool operator==(const ActionResult& other) const;
    bool operator!=(const ActionResult& other) const { return !(*this == other); }
    
private:
    float reward_;
    bool success_;
    std::string message_;
};
```

### DevelopmentSystem
```cpp
class DevelopmentSystem {
public:
    DevelopmentSystem();
    ~DevelopmentSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update development
    void update(TimestepDuration dt);
    
    // Get current stage
    DevelopmentalStage getStage() const { return currentStage_; }
    float getAge() const { return age_; }
    float getMaturationLevel() const { return maturationLevel_; }
    
    // Stage transitions
    void setStage(DevelopmentalStage stage);
    bool canTransitionTo(DevelopmentalStage targetStage) const;
    void triggerDevelopmentEvent(DevelopmentEvent event);
    
    // Plasticity modifiers
    float getPlasticityModifier() const { return plasticityModifier_; }
    void setPlasticityModifier(float modifier) { plasticityModifier_ = modifier; }
    
    // Age-based effects
    float getAgeBasedPlasticityRate() const;
    float getAgeBasedLearningRate() const;
    float getAgeBasedMemoryRetention() const;
    
    // Critical period
    bool isInCriticalPeriod() const;
    float getCriticalPeriodStrength() const;
    void setCriticalPeriodStrength(float strength);
    
    // Maturation
    void advanceMaturation(float amount);
    bool isMature() const;
    
    // Stress and resilience
    void recordStress(float amount);
    float getStressLevel() const { return stressLevel_; }
    float getResilience() const { return resilience_; }
    void improveResilience(float amount);
    
    // Configuration
    void setMaturationRate(float rate);
    void setCriticalPeriodDuration(float duration);
    void setHomeostaticDrive(float drive);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    DevelopmentalStage currentStage_;
    float age_;
    float maturationLevel_;
    float plasticityModifier_;
    float stressLevel_;
    float resilience_;
    Timestamp lastStageTransition_;
    bool inCriticalPeriod_;
    float criticalPeriodStrength_;
};
```

### NeuralPlanner
```cpp
class NeuralPlanner {
public:
    NeuralPlanner();
    ~NeuralPlanner();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Plan actions
    MotorCommand planAction(const std::vector<float>& state);
    std::vector<MotorCommand> planSequence(const std::vector<float>& state,
                                          size_t sequenceLength);
    
    // Evaluate actions
    float evaluateAction(const MotorCommand& action,
                        const std::vector<float>& state) const;
    float evaluateSequence(const std::vector<MotorCommand>& sequence,
                          const std::vector<float>& state) const;
    
    // Get predictions
    float getActionValue(const MotorCommand& action) const;
    const std::vector<float>& getActionValues() const;
    
    // Update planner
    void update(TimestepDuration dt);
    
    // Reset
    void reset();
    
    // Configuration
    void setPlanningHorizon(float horizon);
    void setExplorationRate(float rate);
    void setPlanningDepth(size_t depth);
    void enableMonteCarlo(bool enable);
    
    // Statistics
    size_t getPlanCount() const;
    float getAveragePlanQuality() const;
    float getPlanningTime() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### ConceptFormation
```cpp
class ConceptFormation {
public:
    ConceptFormation();
    ~ConceptFormation();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Process experience
    void processExperience(const class SensoryInput& sensoryInput,
                           const class Action& action,
                           float reward,
                           Timestamp timestamp);
    
    // Get concepts
    const Concept* getConcept(size_t index) const;
    size_t getConceptCount() const;
    std::vector<size_t> findSimilarConcepts(const std::vector<float>& pattern,
                                          float similarityThreshold = 0.7f) const;
    
    // Get predictions
    float predictConcept(const std::vector<float>& pattern) const;
    const std::vector<float>& getConceptVector(size_t conceptId) const;
    
    // Update concept formation
    void update(TimestepDuration dt);
    
    // Reset
    void reset();
    
    // Configuration
    void setConceptLearningRate(float rate);
    void setConceptStabilityThreshold(float threshold);
    void setMaxConcepts(size_t max);
    void enableIncrementalLearning(bool enable);
    
    // Statistics
    float getConceptCohesion(size_t conceptId) const;
    float getConceptSeparation(size_t conceptId) const;
    float getConceptStability(size_t conceptId) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### AttentionalSelection
```cpp
class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Process competing inputs
    std::vector<NeuronId> processCompetition(const std::vector<NeuronId>& competitors,
                                           float globalInhibition = 0.5f);
    
    // Apply attention to specific regions
    void focusOnRegion(RegionId region);
    void releaseAttention();
    
    // Get current attention focus
    std::vector<RegionId> getAttendedRegions() const;
    
    // Set attention parameters
    void setInhibitionStrength(float strength);
    void setExcitationStrength(float strength);
    void setCompetitionThreshold(float threshold);
    
    // Get inhibition level for a neuron
    float getInhibitionFor(NeuronId neuron) const;
    
    // Get excitation level for a neuron
    float getExcitationFor(NeuronId neuron) const;
    
    // Update attention dynamics
    void update(TimestepDuration dt);
    
    // Is this neuron currently attended?
    bool isAttended(NeuronId neuron) const;
    
    // Get attention winners
    const std::vector<NeuronId>& getWinners() const { return winners_; }
    
    // Top-down attention (goal-directed)
    void applyTopDownBias(NeuronId neuron, float biasStrength);
    
    // Bottom-up attention (stimulus-driven)
    void applyBottomUpSalience(NeuronId neuron, float salienceStrength);
    
    // Reset attention
    void reset();
    
    // Configuration
    void setSelectionStrategy(SelectionStrategy strategy);
    void setInhibitionCurve(const std::string& curve);
    void setRecurrentExcitation(float strength);
    
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
    
    // Biases/salience stored by NeuronId for correct lookup in processCompetition
    std::unordered_map<uint64_t, float> topDownBias_;
    std::unordered_map<uint64_t, float> bottomUpSalience_;
};
```

### SelfModel
```cpp
class SelfModel {
public:
    SelfModel();
    ~SelfModel();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get body model
    const BodyModel& getBodyModel() const { return bodyModel_; }
    void updateBodyModel(const AgentBody& body);
    
    // Get action effects model
    const ActionEffectsModel& getActionEffectsModel() const { return actionEffectsModel_; }
    void recordActionEffect(const Action& action, float reward, float predictionError);
    
    // Get capability model
    const CapabilityModel& getCapabilityModel() const { return capabilityModel_; }
    void recordCapability(NeuronId neuron, float capability);
    
    // Get agency model
    const AgencyModel& getAgencyModel() const { return agencyModel_; }
    void updateAgency(const std::vector<MotorCommand>& actions);
    
    // Update self model
    void update(TimestepDuration dt);
    
    // Reset
    void reset();
    
    // Configuration
    void setModelUpdateRate(float rate);
    void setPredictionErrorThreshold(float threshold);
    void enableMetaLearning(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### SocialLearning
```cpp
class SocialLearning {
public:
    SocialLearning();
    ~SocialLearning();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Observe other agents
    void observe(const AgentState& otherAgent, Timestamp timestamp);
    void recordObservation(const Observation& observation);
    
    // Learn from observation
    void learnFromObservation(size_t observationId);
    void imitateAction(const Action& action, const AgentState& model);
    
    // Get models
    const AgentModel* getModel(NeuronId neuron) const;
    const std::vector<AgentModel*>& getAllModels() const;
    
    // Update social learning
    void update(TimestepDuration dt);
    
    // Reset
    void reset();
    
    // Configuration
    void setImitationRate(float rate);
    void setObservationWindow(float window);
    void setSocialInfluence(float influence);
    void enableTheoryOfMind(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### WorldObject
```cpp
class WorldObject {
public:
    WorldObject();
    WorldObject(float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f);
    
    // Get position
    float getX() const { return x_; }
    float getY() const { return y_; }
    void setPosition(float x, float y) { x_ = x; y_ = y; }
    
    // Get type
    WorldObjectType getType() const { return type_; }
    void setType(WorldObjectType type) { type_ = type; }
    
    // Get properties
    float getValue() const { return value_; }
    void setValue(float value) { value_ = value; }
    float getRadius() const { return radius_; }
    void setRadius(float radius) { radius_ = radius; }
    bool isActive() const { return active_; }
    void setActive(bool active) { active_ = active; }
    
    // Get bounding box
    float getLeft() const { return x_ - radius_; }
    float getRight() const { return x_ + radius_; }
    float getTop() const { return y_ + radius_; }
    float getBottom() const { return y_ - radius_; }
    
    // Distance to point
    float distanceTo(float x, float y) const;
    bool contains(float x, float y) const;
    
    // Comparison
    bool operator==(const WorldObject& other) const;
    bool operator!=(const WorldObject& other) const { return !(*this == other); }
    
    // Serialization
    void serialize(std::ostream& stream) const;
    void deserialize(std::istream& stream);
    
private:
    float x_;
    float y_;
    WorldObjectType type_;
    float value_;
    float radius_;
    bool active_;
};
```

### AgentBody
```cpp
class AgentBody {
public:
    AgentBody();
    
    // Get position
    float getX() const { return x_; }
    float getY() const { return y_; }
    void setPosition(float x, float y) { x_ = x; y_ = y; }
    
    // Get orientation
    float getOrientation() const { return orientation_; }
    void setOrientation(float angle) { orientation_ = angle; }
    
    // Get velocity
    float getVelocityX() const { return velocityX_; }
    float getVelocityY() const { return velocityY_; }
    void setVelocity(float vx, float vy) { velocityX_ = vx; velocityY_ = vy; }
    
    // Get angular velocity
    float getAngularVelocity() const { return angularVelocity_; }
    void setAngularVelocity(float av) { angularVelocity_ = av; }
    
    // Get energy and health
    float getEnergy() const { return energy_; }
    void setEnergy(float e) { energy_ = e; }
    float getHealth() const { return health_; }
    void setHealth(float h) { health_ = h; }
    
    // Get age
    float getAge() const { return age_; }
    void setAge(float a) { age_ = a; }
    
    // Get state
    bool isMoving() const { return isMoving_; }
    void setIsMoving(bool moving) { isMoving_ = moving; }
    bool isTurning() const { return isTurning_; }
    void setIsTurning(bool turning) { isTurning_ = turning; }
    
    // Get timing
    float getLastActionTime() const { return lastActionTime_; }
    void setLastActionTime(float time) { lastActionTime_ = time; }
    
    // Physics update
    void update(float dt);
    void applyForce(float fx, float fy);
    void applyTorque(float torque);
    
    // Collision detection
    bool collidesWith(const WorldObject& other) const;
    bool collidesWithWall(float worldWidth, float worldHeight) const;
    
    // Reset
    void reset();
    
    // Serialization
    void serialize(std::ostream& stream) const;
    void deserialize(std::istream& stream);
    
private:
    float x_;
    float y_;
    float orientation_;
    float velocityX_;
    float velocityY_;
    float angularVelocity_;
    float energy_;
    float health_;
    float age_;
    bool isMoving_;
    bool isTurning_;
    float lastActionTime_;
    
    // Physics
    float mass_;
    float friction_;
    float maxSpeed_;
    float maxAngularSpeed_;
    
    // Forces
    float forceX_;
    float forceY_;
    float torque_;
};
```

### NeuralDynamics
```cpp
class NeuralDynamics {
public:
    NeuralDynamics();
    ~NeuralDynamics();
    
    // Step LIF neuron
    static MembranePotential stepLIF(Neuron& neuron, Timestamp currentTime, TimestepDuration dt);
    
    // Get firing state
    static FiringState getFiringState(const Neuron& neuron, Timestamp currentTime);
    
    // Get input current
    static MembranePotential getInputCurrent(const Neuron& neuron);
    
    // Get output current
    static MembranePotential getOutputCurrent(const Neuron& neuron, Timestamp currentTime);
    
    // Get membrane potential dynamics
    static MembranePotential getMembranePotentialDynamics(const Neuron& neuron,
                                                         Timestamp currentTime,
                                                         TimestepDuration dt);
    
    // Get synaptic input
    static MembranePotential getSynapticInput(const Neuron& neuron, Timestamp currentTime);
    
    // Get intrinsic properties
    static float getExcitability(const Neuron& neuron);
    static float getInhibitorySensitivity(const Neuron& neuron);
    static float getRestPotential(const Neuron& neuron);
    static float getThreshold(const Neuron& neuron);
    static float getResetPotential(const Neuron& neuron);
    
    // Set properties
    static void setExcitability(Neuron& neuron, float value);
    static void setInhibitorySensitivity(Neuron& neuron, float value);
    static void setRestPotential(Neuron& neuron, float value);
    static void setThreshold(Neuron& neuron, float value);
    static void setResetPotential(Neuron& neuron, float value);
    
    // Get statistics
    static float getAverageFiringRate(const Neuron& neuron);
    static float getFiringRate(const Neuron& neuron, Timestamp startTime, Timestamp endTime);
    static float getTotalInput(const Neuron& neuron);
    static float getTotalOutput(const Neuron& neuron);
    
private:
    NeuralDynamics() = delete;
    ~NeuralDynamics() = delete;
};
```

### SpikeSystem
```cpp
class SpikeSystem {
public:
    SpikeSystem();
    ~SpikeSystem();
    
    // Process delayed spikes
    void processDelayedSpikes(const std::vector<DelayedSpikeEvent>& events,
                             Timestamp currentTime);
    
    // Schedule spike
    void scheduleSpike(NeuronId source, NeuronId target,
                      SynapticWeight weight, SynapseType type,
                      Timestamp currentTime, SimulationStep currentStep,
                      Delay delay);
    
    // Get pending spikes
    const std::vector<DelayedSpikeEvent>& getPendingSpikes() const;
    size_t getPendingSpikeCount() const;
    
    // Clear spikes
    void clearSpikes();
    
    // Configuration
    void setMaxPendingSpikes(size_t max);
    void setMaxSpikeHistory(size_t max);
    
    // Statistics
    float getSpikeTransmissionEfficiency() const;
    float getAverageSpikeDelay() const;
    
    // Update spike system
    void update(TimestepDuration dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### SimpleWorld
```cpp
class SimpleWorld {
public:
    SimpleWorld();
    ~SimpleWorld();
    
    // Configuration
    void configure(int width, int height, int visionWidth, int visionHeight);
    
    // Control
    void reset();
    void setRandomSeed(uint64_t seed);
    void update(TimestepDuration timestep);
    
    // Agent interaction
    void setAgentStart(float x, float y);
    void applyMotorCommand(const MotorCommand& cmd, Timestamp currentTime);
    
    // State access
    SensoryPercept getSensoryPercept() const;
    AgentBody getAgentBody() const;
    
    // World objects
    void addObject(const WorldObject& obj);
    void removeObject(float x, float y);
    bool isValidPosition(float x, float y) const;
    
    // Properties
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    float getMaxEnergy() const { return maxEnergy_; }
    void setMaxEnergy(float e) { maxEnergy_ = e; }
    float getEnergyDecayRate() const { return energyDecayRate_; }
    void setEnergyDecayRate(float r) { energyDecayRate_ = r; }
    float getSimulationTime() const { return simulationTime_; }
    
    // Get world objects at position
    const WorldObject* getObject(float x, float y) const;
    std::vector<WorldObject> getObjectsInRadius(float x, float y, float radius) const;
    
    // Agent movement and physics
    void moveAgent(float dx, float dy);
    void turnAgent(float angle);
    float getAgentDistance(float x, float y) const;
    float getAgentAngle(float x, float y) const;
    
    // Reward system
    void addResource(float x, float y, float value);
    void addHazard(float x, float y, float danger);
    void addWall(float x, float y);
    void addMarker(float x, float y, float value = 1.0f);
    
    // Get rewards and hazards
    float getResourceReward(float x, float y) const;
    float getHazardPenalty(float x, float y) const;
    bool isInHazardZone(float x, float y) const;
    
    // Visibility and perception
    std::vector<float> getVision(float agentX, float agentY, float visionAngle = 0.0f) const;
    std::vector<float> getTouchSensors(float agentX, float y) const;
    std::vector<float> getInternalSensors() const;
    std::vector<float> getProprioception() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    int width_;
    int height_;
    float maxEnergy_;
    float energyDecayRate_;
    float simulationTime_;
    uint64_t randomSeed_;
};
```

### RewardModulation
```cpp
class RewardModulation {
public:
    RewardModulation();
    ~RewardModulation();
    
    // Apply reward to synapses
    void applyReward(Synapse* synapse, float reward, float predictionError,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt);
    
    // Get reward statistics
    float getAverageReward() const { return averageReward_; }
    float getTotalReward() const { return totalReward_; }
    size_t getRewardCount() const { return rewardCount_; }
    
    // Reset
    void reset();
    
    // Configuration
    void setRewardDecayRate(float rate);
    void setLearningRate(float rate);
    void setPredictionErrorWeight(float weight);
    
private:
    float averageReward_;
    float totalReward_;
    size_t rewardCount_;
    float rewardDecayRate_;
    float learningRate_;
    float predictionErrorWeight_;
};
```

## Performance Infrastructure Classes

### MemoryPool
```cpp
class MemoryPool {
public:
    MemoryPool(size_t blockSize = 1024, size_t initialPoolSize = 1000);
    ~MemoryPool();
    
    // Allocate block
    void* allocate(size_t size);
    
    // Deallocate block
    void deallocate(void* ptr, size_t size);
    
    // Get statistics
    size_t getBlockCount() const { return blockCount_; }
    size_t getAllocatedCount() const { return allocatedCount_; }
    size_t getFreeCount() const { return freeCount_; }
    size_t getTotalMemoryUsage() const { return totalMemoryUsage_; }
    
    // Configuration
    void setBlockSize(size_t size);
    void setInitialPoolSize(size_t size);
    void setMaxPoolSize(size_t size);
    
    // Cleanup
    void cleanup();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### EventQueue
```cpp
class EventQueue {
public:
    EventQueue();
    ~EventQueue();
    
    // Add event
    void enqueue(Event event);
    
    // Get next event
    Event dequeue();
    Event peek() const;
    
    // Check if empty
    bool isEmpty() const { return events_.empty(); }
    
    // Get statistics
    size_t getEventCount() const { return events_.size(); }
    
    // Clear queue
    void clear();
    
    // Configuration
    void setMaxQueueSize(size_t max);
    
    // Get events by type
    std::vector<Event> getEvents(EventType type) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### SparseConnectivity
```cpp
class SparseConnectivity {
public:
    SparseConnectivity();
    ~SparseConnectivity();
    
    // Add connection
    bool addConnection(NeuronId source, NeuronId target,
                       SynapseType type = SynapseType::Excitatory,
                       SynapticWeight weight = 1.0f, Delay delay = 1);
    
    // Remove connection
    bool removeConnection(NeuronId source, NeuronId target);
    
    // Get connections
    const Synapse* getConnection(NeuronId source, NeuronId target) const;
    std::vector<const Synapse*> getConnectionsFrom(NeuronId source) const;
    std::vector<const Synapse*> getConnectionsTo(NeuronId target) const;
    
    // Check if connection exists
    bool hasConnection(NeuronId source, NeuronId target) const;
    
    // Statistics
    size_t getConnectionCount() const { return connections_.size(); }
    float getConnectionDensity() const;
    
    // Clear all connections
    void clear();
    
    // Configuration
    void setConnectionLimit(size_t limit);
    void setRecurrentConnectionProbability(float prob);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

## Configuration Parameter Reference

### Brain Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Example C++ Code

### Basic Brain Usage
```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 1000);
    config->set("brain.synapse_density", 0.1);
    
    // Create brain
    Brain brain(config);
    
    // Initialize brain
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (size_t step = 0; step < 1000; ++step) {
        brain.step(step);
        
        // Print statistics every 100 steps
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                      << brain.getFiringNeuronCount() << " firing neurons, "
                      << brain.getTotalSpikeCount() << " total spikes" << std::endl;
        }
    }
    
    std::cout << "Simulation complete!" << std::endl;
    return 0;
}
```

### Complete Agent Example
```cpp
#include "Brain.hpp"
#include "AgentBrain.hpp"
#include "SimpleWorld.hpp"
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Create brain and initialize
    Brain brain(config);
    brain.initialize();
    
    // Create agent brain interface
    AgentBrain agent(std::make_shared<Brain>(brain));
    
    // Create and configure world
    SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    world.setAgentStart(10.0, 10.0);
    
    // Initialize agent with world
    agent.initialize(world);
    
    // Enable subsystems
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    agent.enableDevelopment(true);
    
    // Run simulation loop
    for (int step = 0; step < 500; ++step) {
        // Update world
        world.update(0.1);
        
        // Get sensory input from world
        SensoryPercept percept = world.getSensoryPercept();
        
        // Process sensory input in brain
        agent.processSensoryInput(percept);
        
        // Run brain step
        brain.step(step);
        
        // Decode motor command from brain activity
        MotorCommand cmd = agent.decodeMotorCommand();
        
        // Apply motor command to world
        world.applyMotorCommand(cmd, world.getSimulationTime());
        
        // Apply reward modulation
        float reward = percept.getInternal()[0] if percept.getInternal() else 0.0f;
        agent.applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent.updateDevelopment(0.1);
        
        // Print progress
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                      << brain.getFiringNeuronCount() << " firing neurons, "
                      << "curiosity: " << agent.getCuriosityLevel() << ", "
                      << "novelty: " << agent.getNoveltyLevel() << ", "
                      << "dev stage: " << static_cast<int>(agent.getDevelopmentalStage()) << std::endl;
        }
    }
    
    std::cout << "Agent simulation complete!" << std::endl;
    return 0;
}
```

### Memory System Usage
```cpp
#include "Brain.hpp"

int main() {
    // Create brain
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Access memory systems
    auto* working_memory = brain.getWorkingMemory();
    auto* episodic_memory = brain.getEpisodicMemory();
    auto* associative_memory = brain.getAssociativeMemory();
    
    if (working_memory) {
        // Store pattern in working memory
        std::vector<float> pattern = {0.1f, 0.5f, 0.9f, 0.3f, 0.7f};
        working_memory->store(pattern);
        
        // Retrieve pattern
        std::vector<float> retrieved = working_memory->retrieve();
        
        // Update working memory
        working_memory->update(0.1f);
    }
    
    if (episodic_memory) {
        // Store episode
        // ... create sensory input, action, reward, timestamp ...
        
        // Update memory (consolidation and replay)
        episodic_memory->update(0.1f);
    }
    
    return 0;
}
```

### Advanced Plasticity
```cpp
#include "Brain.hpp"
#include "STDP.hpp"
#include "Hebbian.hpp"

int main() {
    // Create brain
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Access plasticity systems
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    
    if (stdp) {
        // Configure STDP parameters
        stdp->configure(0.01f, 0.012f, 20.0f, 20.0f);
        
        // Access STDP-specific parameters
        stdp->setLTPWeight(0.015f);
        stdp->setLTDWeight(0.015f);
        stdp->setTimeConstantPlus(15.0f);
        stdp->setTimeConstantMinus(25.0f);
    }
    
    if (hebbian) {
        // Configure Hebbian learning
        hebbian->setLearningRate(0.001f);
        hebbian->setCoactivityThreshold(0.5f);
        
        // Enable competition
        hebbian->enableCompetition(true);
    }
    
    return 0;
}
```

### Visualization and Debugging
```cpp
#include "Brain.hpp"
#include <fstream>

void saveBrainState(const Brain& brain, const std::string& filename) {
    brain.save(filename);
}

void loadBrainState(Brain& brain, const std::string& filename) {
    brain.load(filename);
}

void logBrainStatus(const Brain& brain) {
    brain.logStatus();
}

int main() {
    // Create and initialize brain
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain.step(step);
        
        // Log status periodically
        if (step % 100 == 0) {
            std::cout << "=== Step " << step << " ===" << std::endl;
            logBrainStatus(brain);
            
            // Save checkpoint
            saveBrainState(brain, "checkpoint_" + std::to_string(step) + ".bin");
        }
    }
    
    return 0;
}
```

## Configuration Examples

### JSON Configuration
```json
{
    "brain": {
        "neuron_count": 5000,
        "synapse_density": 0.15,
        "connection_probability": 0.08,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.01,
            "coactivity_threshold": 0.5
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true
        },
        "novelty": {
            "enable": true
        }
    },
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "replay_interval": 100,
    "consolidation_interval": 1000
}
```

### Command Line Arguments
```bash
./nlm_simulation \
    --config brain.json \
    --neuron_count 2000 \
    --synapse_density 0.2 \
    --stdp_learning_rate 0.002 \
    --reward_modulation true \
    --curiosity_enabled true
```

## Building and Running

### Building with CMake
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=20
make -j$(nproc)
```

### Running Tests
```bash
cd build
ctest --output-on-failure
```

### Running Examples
```bash
cd build
./nlm_phase6_demo
```

## License

MIT License - See LICENSE file for details.

---

*This documentation covers NLM version 6.0. For specific API details of individual classes and methods,
refer to the source code headers in the C++ source tree.*
