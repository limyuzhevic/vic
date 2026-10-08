# NLM API Reference

**Comprehensive API Documentation for NLM (熙然)**

## Table of Contents

1. [Core Layer](#core-layer)
   - [Types](#types)
   - [Config](#config)
   - [Random Generator](#random-generator)
   - [Logger](#logger)
   - [SimulationClock](#simulationclock)

2. [Brain Layer](#brain-layer)
   - [Brain](#brain)
   - [Neuron](#neuron)
   - [Synapse](#synapse)
   - [NeuralPopulation](#neuralpopulation)
   - [NeuralRegion](#neuralregion)

3. [Dynamics Layer](#dynamics-layer)
   - [NeuralDynamics](#neuraldynamics)
   - [SpikeSystem](#spikesystem)

4. [Plasticity Layer](#plasticity-layer)
   - [PlasticityRule](#plasticityrule)
   - [STDP](#stdp)
   - [Hebbian](#hebbian)
   - [StructuralPlasticity](#structuralplasticity)

5. [Development Layer](#development-layer)
   - [DevelopmentSystem](#developmentsystem)
   - [Synaptogenesis](#synaptogenesis)
   - [Pruning](#pruning)
   - [Maturation](#maturation)

6. [Neuromodulation Layer](#neuromodulation-layer)
   - [Neuromodulator](#neuromodulator)
   - [Dopamine](#dopamine)
   - [Novelty](#novelty)
   - [Curiosity](#curiosity)
   - [PredictionError](#predictionerror)

7. [Memory Layer](#memory-layer)
   - [NeuralWorkingMemory](#neuralworkingmemory)
   - [NeuralEpisodicMemory](#neuralepisodicmemory)
   - [NeuralAssociativeMemory](#neuralassociativememory)

8. [Prediction Layer](#prediction-layer)
   - [NeuralPrediction](#neuralprediction)
   - [PredictionSystem](#predictionsystem)

9. [Cognitive Layer](#cognitive-layer)
   - [AttentionalSelection](#attentionalselection)
   - [NeuralPlanner](#neuralplanner)
   - [ConceptFormation](#conceptformation)
   - [SelfModel](#selfmodel)
   - [SocialLearning](#sociallearning)

10. [Sensory Layer](#sensory-layer)
    - [SensoryInput](#sensoryinput)
    - [Vision](#vision)
    - [Audio](#audio)
    - [InternalSignals](#internalsignals)

11. [Motor Layer](#motor-layer)
    - [Action](#action)
    - [MotorSystem](#motorsystem)

12. [Environment Layer](#environment-layer)
    - [Environment](#environment)
    - [Observation](#observation)

13. [Experiments Layer](#experiments-layer)
    - [Experiment](#experiment)
    - [ExperimentRunner](#experimentrunner)
    - [Metrics](#metrics)

## Core Layer

### Types

The Types layer provides strongly-typed identifiers for all neural components, ensuring type safety and clear semantics throughout the system.

#### Key Type Definitions

```cpp
// Neuron types
using NeuronId = uint32_t;
using RegionId = uint32_t;
using PopulationId = uint32_t;
using SynapseId = uint32_t;

// Numerical types
using MembranePotential = float;      // -100 to +50 mV
using Conductance = float;            // nS
using Current = float;                // nA
using Time = float;                   // ms
using SimulationStep = uint64_t;     // Step count
using Delay = uint32_t;               // Spike delay in steps

// Boolean types for plasticity
struct PlasticityFlags {
    bool stdpEnabled : 1;
    bool hebbianEnabled : 1;
    bool structuralEnabled : 1;
    bool neuromodulated : 1;
    // ... more flags
};
```

#### NeuronType Enumeration
```cpp
enum class NeuronType {
    EXCITATORY,    // Positive effect on postsynaptic neuron
    INHIBITORY,     // Negative effect on postsynaptic neuron
    SENSORY,       // Processes external input
    MOTOR,         // Generates action output
    INTERNALLY_CONNECTED, // Maintains working memory
    PROPRIOCEPTIVE // Provides body state information
};
```

### Config

Configuration management system for all NLM parameters.

#### Configuration Structure
```cpp
struct Config {
    // Neural core configuration
    uint32_t neuronCount = 7500;
    float synapseDensity = 0.1f;
    float plasticityEnabled = true;
    float developmentEnabled = true;
    
    // Memory system configuration
    uint32_t workingMemoryCapacity = 384;
    uint32_t episodicMemoryCapacity = 10000;
    float replayEnabled = true;
    
    // Neuromodulation configuration
    bool dopamineEnabled = true;
    bool curiosityEnabled = true;
    bool noveltyEnabled = true;
    
    // Simulation configuration
    Time timestep = 0.001f;           // 1ms simulation step
    SimulationStep maxSteps = 100000;
    float realTimeScale = 10.0f;      // Real-time speed multiplier
    
    // Development configuration
    float developmentEnabled = true;
    SimulationStep criticalPeriodStart = 1000;
    SimulationStep criticalPeriodEnd = 10000;
    
    // Random configuration
    uint64_t randomSeed = 0;          // 0 for random seed
    
    // Performance configuration
    bool enableCheckpointing = true;
    bool enableReplay = true;
    
    // Logging configuration
    std::string logLevel = "Info";
    std::string logFile = "nlm.log";
};
```

#### Configuration File Format (JSON)
```json
{
    "brain": {
        "neuron_count": 7500,
        "synapse_density": 0.1,
        "plasticity_enabled": true,
        "development_enabled": true
    },
    "memory": {
        "working_memory_capacity": 384,
        "episodic_memory_capacity": 10000,
        "replay_enabled": true
    },
    "neuromodulation": {
        "dopamine_enabled": true,
        "curiosity_enabled": true,
        "novelty_enabled": true
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 100000,
        "real_time_scale": 10.0
    },
    "development": {
        "critical_period_start": 1000,
        "critical_period_end": 10000
    }
}
```

### Random Generator

Deterministic random number generator for reproducible experiments.

#### Class Interface
```cpp
class RandomGenerator {
public:
    // Constructor
    RandomGenerator(uint64_t seed = 0);
    
    // Get random number in range [0, max)
    uint32_t nextUint32(uint32_t max);
    
    // Get random float in range [0, 1)
    float nextFloat();
    
    // Get random float in range [min, max)
    float nextFloat(float min, float max);
    
    // Get random boolean
    bool nextBool();
    
    // Get random index from distribution
    size_t nextChoice(const std::vector<float>& weights);
    
    // Reset generator with new seed
    void reset(uint64_t seed);
    
    // Get current seed
    uint64_t getSeed() const;
};
```

### Logger

Structured logging system for debugging and monitoring.

#### Logger Interface
```cpp
class Logger {
public:
    enum class Level {
        Debug,   // Detailed debug information
        Info,    // General information
        Warning, // Warnings and potential issues
        Error    // Errors and failures
    };
    
    // Singleton access
    static Logger& getInstance();
    
    // Set log level
    void setLevel(Level level);
    
    // Log message at different levels
    void logDebug(const std::string& message);
    void logInfo(const std::string& message);
    void logWarning(const std::string& message);
    void logError(const std::string& message);
    
    // Log with context
    template<typename... Args>
    void logInfo(const std::string& format, Args&&... args);
    
    // Set log file
    void setLogFile(const std::string& filepath);
    
    // Flush log buffer
    void flush();
};
```

### SimulationClock

Time management for neural simulation.

#### Class Interface
```cpp
class SimulationClock {
public:
    // Constructor
    SimulationClock(Time timestep = 0.001f);
    
    // Get current simulation time
    Time getCurrentTime() const;
    
    // Get current simulation step
    SimulationStep getCurrentStep() const;
    
    // Advance simulation by one timestep
    void advanceStep();
    
    // Set time to specific value
    void setTime(Time time);
    
    // Set step to specific value
    void setStep(SimulationStep step);
    
    // Get simulation time in real seconds
    float getRealTime() const;
    
    // Reset clock
    void reset();
    
    // Get time since last spike (for refractory calculations)
    Time getTimeSinceLastSpike() const;
    
    // Record spike time
    void recordSpike();
};
```

## Brain Layer

### Brain

The central coordinator of the neural system, implementing real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

#### Class Interface
```cpp
class Brain {
public:
    // Constructor
    explicit Brain(std::shared_ptr<Config> config);
    
    // Destructor
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialization
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Sensory input processing
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics and monitoring
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    std::size_t getTotalNeuronCount() const;
    std::size_t getTotalSynapseCount() const;
    std::size_t getActiveNeuronCount() const;
    std::size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Action production
    std::unique_ptr<Action> produceAction();
    
    // Neuromodulation
    void applyNeuromodulation(const Neuromodulator& signal);
    
    // Plasticity updates
    void updatePlasticity();
    
    // Development
    void develop();
    
    // State management
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction system access
    PredictionSystem* getPredictionSystem();
    
    // Cognitive system access
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Development system access
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Neuromodulation system access
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    
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
    
    // Logging
    void logStatus() const;
};
```

#### Brain::step() Method Details
```cpp
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // Phase 1: Process pending delayed spikes
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // Phase 2: Update all neurons (LIF dynamics)
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // Phase 3: Detect spikes and schedule events
    // (Implementation details for spike detection and scheduling)
    
    // Phase 4: Apply plasticity rules
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP and Hebbian plasticity
            // STDP based on spike timing
            // Hebbian based on co-activity
        }
    }
    
    // Phase 5: Apply structural plasticity periodically
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // Phase 6: Update connected systems (if integrated)
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(this, currentStep, currentTime);
    }
    
    // Additional integrated system updates...
}
```

### Neuron

Individual neuron representation implementing Leaky Integrate-and-Fire (LIF) dynamics.

#### Class Interface
```cpp
class Neuron {
public:
    // Constructor
    Neuron(NeuronId id, NeuronType type);
    
    // LIF dynamics update
    void stepLIF(Timestamp currentTime, Time timestep);
    
    // Process synaptic input
    void addSynapticInput(Current current, Timestamp spikeTime);
    
    // Check and handle spiking
    bool checkThreshold(Timestamp currentTime);
    void handleSpike(Timestamp currentTime);
    
    // State management
    void reset();
    void copyState(const Neuron& other);
    
    // Properties
    NeuronId getId() const;
    NeuronType getType() const;
    MembranePotential getPotential() const;
    MembranePotential getThreshold() const;
    MembranePotential getRestPotential() const;
    bool isSpiking() const;
    Timestamp getLastSpikeTime() const;
    float getRefractoryRemaining() const;
    
    // Plasticity
    void recordSpike(Timestamp spikeTime);
    float getExcitatoryInput() const;
    float getInhibitoryInput() const;
    
    // Development
    void applyDevelopmentChanges(const DevelopmentSystem& dev);
    
    // Serialization
    void save(std::ostream& out) const;
    void load(std::istream& in);
};
```

#### Neuron::stepLIF() Method
```cpp
void Neuron::stepLIF(Timestamp currentTime, Time timestep) {
    // Update membrane potential with decay
    potential *= std::exp(-timestep / tau_m); // Membrane time constant
    
    // Add leak current
    potential += (e_l - potential) * timestep / tau_m;
    
    // Check refractory period
    if (refractoryRemaining > 0) {
        refractoryRemaining -= timestep;
        // Hold potential at threshold during refractory period
        potential = threshold;
        return;
    }
    
    // Add accumulated synaptic inputs
    potential += totalInput;
    totalInput = 0.0f;
    
    // Check threshold and spike
    if (potential >= threshold) {
        handleSpike(currentTime);
    }
}
```

### Synapse

Synaptic connection between neurons with delay and plasticity.

#### Class Interface
```cpp
class Synapse {
public:
    // Constructor
    Synapse(SynapseId id, NeuronId pre, NeuronId post, 
            NeuronType preType, NeuronType postType);
    
    // Update function
    void update(Timestamp currentTime, Time timestep);
    
    // Apply plasticity
    void applyPlasticity(const STDP& stdp, const Hebbian& hebbian,
                        const Neuromodulator& neuromodulator,
                        RandomGenerator& rng);
    
    // Get synaptic weight
    float getWeight() const;
    void setWeight(float weight);
    
    // Properties
    SynapseId getId() const;
    NeuronId getPreNeuron() const;
    NeuronId getPostNeuron() const;
    NeuronType getPreType() const;
    NeuronType getPostType() const;
    Delay getDelay() const;
    void setDelay(Delay delay);
    
    // State
    bool isActive() const;
    bool hasFired() const;
    Timestamp getLastFireTime() const;
    
    // Plasticity flags
    const PlasticityFlags& getPlasticityFlags() const;
    void setPlasticityFlags(const PlasticityFlags& flags);
    
    // Serialization
    void save(std::ostream& out) const;
    void load(std::istream& in);
};
```

### NeuralPopulation

Group of neurons with shared properties and management.

#### Class Interface
```cpp
class NeuralPopulation {
public:
    // Constructor
    NeuralPopulation(PopulationId id, NeuronType type, size_t size);
    
    // Neuron management
    NeuronId addNeuron();
    Neuron* getNeuron(NeuronId id);
    const Neuron* getNeuron(NeuronId id) const;
    size_t getNeuronCount() const;
    
    // Population properties
    PopulationId getId() const;
    NeuronType getType() const;
    size_t getSize() const;
    
    // Statistics
    float getAverageFiringRate() const;
    size_t getActiveNeuronCount() const;
    size_t getSpikingNeuronCount() const;
    
    // State management
    void reset();
    void copyState(const NeuralPopulation& other);
    
    // Input/output management
    void addExternalInput(MembranePotential current);
    void clearExternalInput();
    
    // Plasticity
    void updatePlasticity(const STDP& stdp, const Hebbian& hebbian,
                         const Neuromodulator& neuromodulator,
                         RandomGenerator& rng);
    
    // Development
    void applyDevelopmentChanges(const DevelopmentSystem& dev);
    
    // Serialization
    void save(std::ostream& out) const;
    void load(std::istream& in);
};
```

### NeuralRegion

Brain region containing multiple neural populations.

#### Class Interface
```cpp
class NeuralRegion {
public:
    // Constructor
    NeuralRegion(RegionId id, const std::string& name);
    
    // Population management
    PopulationId addPopulation(NeuronType type, size_t size);
    NeuralPopulation* getPopulation(PopulationId id);
    const NeuralPopulation* getPopulation(PopulationId id) const;
    size_t getPopulationCount() const;
    
    // Region properties
    RegionId getId() const;
    const std::string& getName() const;
    void setName(const std::string& name);
    
    // Neuron/synapse management
    NeuronId addNeuronToPopulation(NeuronType type, size_t size);
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    
    // Statistics
    float getRegionFiringRate() const;
    size_t getActiveNeuronCount() const;
    
    // State management
    void reset();
    void copyState(const NeuralRegion& other);
    
    // Input/output management
    void addExternalInput(NeuronType type, MembranePotential current);
    void clearExternalInput(NeuronType type);
    
    // Plasticity
    void updatePlasticity(const STDP& stdp, const Hebbian& hebbian,
                         const Neuromodulator& neuromodulator,
                         RandomGenerator& rng);
    
    // Development
    void applyDevelopmentChanges(const DevelopmentSystem& dev);
    
    // Serialization
    void save(std::ostream& out) const;
    void load(std::istream& in);
};
```

## Dynamics Layer

### NeuralDynamics

Abstract base class for neuron dynamics models.

#### Class Interface
```cpp
class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    
    // Update dynamics
    virtual void update(Timestamp currentTime, Time timestep) = 0;
    
    // Check for threshold crossing
    virtual bool checkThreshold(Timestamp currentTime) = 0;
    
    // Handle spike event
    virtual void handleSpike(Timestamp currentTime) = 0;
    
    // Reset neuron state
    virtual void reset() = 0;
    
    // Copy state from another neuron
    virtual void copyState(const NeuralDynamics& other) = 0;
    
    // Get current membrane potential
    virtual float getPotential() const = 0;
    
    // Check if neuron is in refractory period
    virtual bool isRefractory() const = 0;
    
    // Get remaining refractory time
    virtual float getRefractoryRemaining() const = 0;
    
    // Get neuron type
    virtual NeuronType getType() const = 0;
};
```

### SpikeSystem

Event-driven spike processing system with efficient spike queue management.

#### Class Interface
```cpp
class SpikeSystem {
public:
    // Constructor
    SpikeSystem(size_t capacity = 100000);
    
    // Schedule spike event
    void scheduleSpike(NeuronId neuron, Timestamp spikeTime, Delay delay = 0);
    
    // Process scheduled spikes
    void processDelayedSpikes(SimulationStep currentStep, 
                             Timestamp currentTime);
    
    // Get next spike time
    std::optional<std::pair<NeuronId, Timestamp>> getNextSpike() const;
    
    // Check if system has pending spikes
    bool hasPendingSpikes() const;
    
    // Get spike count
    size_t getSpikeCount() const;
    
    // Clear all pending spikes
    void clear();
    
    // Get total spikes processed
    uint64_t getTotalSpikesProcessed() const;
    
    // Set spike processing function (callback)
    void setSpikeHandler(std::function<void(NeuronId, Timestamp)> handler);
};
```

## Plasticity Layer

### PlasticityRule

Abstract base class for synaptic plasticity rules.

#### Class Interface
```cpp
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Apply plasticity to synapse based on pre- and post-synaptic activity
    virtual void apply(Synapse& synapse, 
                      const Neuron& preNeuron,
                      const Neuron& postNeuron,
                      RandomGenerator& rng) = 0;
    
    // Get learning rate for this rule
    virtual float getLearningRate() const = 0;
    
    // Set learning rate
    virtual void setLearningRate(float rate) = 0;
    
    // Get maximum synaptic weight
    virtual float getMaxWeight() const = 0;
    
    // Set maximum synaptic weight
    virtual void setMaxWeight(float maxWeight) = 0;
    
    // Get minimum synaptic weight
    virtual float getMinWeight() const = 0;
    
    // Set minimum synaptic weight
    virtual void setMinWeight(float minWeight) = 0;
};
```

### STDP

Spike-Timing-Dependent Plasticity implementation.

#### Class Interface
```cpp
class STDP : public PlasticityRule {
public:
    // Constructor
    STDP(float learningRate = 0.01f, float timeConstant = 20.0f);
    
    // Apply STDP to synapse
    void apply(Synapse& synapse,
              const Neuron& preNeuron,
              const Neuron& postNeuron,
              RandomGenerator& rng) override;
    
    // Get learning rate
    float getLearningRate() const override;
    void setLearningRate(float rate) override;
    
    // Get time constants
    float getPrePostTimeConstant() const;
    float getPostPreTimeConstant() const;
    
    // Set time constants
    void setTimeConstants(float prePost, float postPre);
    
    // Get maximum weight change
    float getMaxWeightChange() const;
    void setMaxWeightChange(float maxChange);
    
    // Get depot window size
    float getDepotentiationWindow() const;
    void setDepotentiationWindow(float window);
    
    // Get neuromodulation factor
    float getNeuromodulationFactor() const;
    void setNeuromodulationFactor(float factor);
    
    // Apply neuromodulation scaling
    void applyNeuromodulation(const Neuromodulator& modulator);
    
private:
    float learningRate_;
    float prePostTimeConstant_;
    float postPreTimeConstant_;
    float maxWeightChange_;
    float depotentiationWindow_;
    float neuromodulationFactor_;
    
    // Helper functions for STDP calculation
    float calculatePrePostChange(const Synapse& synapse, 
                                const Neuron& preNeuron,
                                const Neuron& postNeuron) const;
    float calculatePostPreChange(const Synapse& synapse,
                                const Neuron& preNeuron,
                                const Neuron& postNeuron) const;
};
```

### Hebbian

Hebbian learning rule implementation.

#### Class Interface
```cpp
class Hebbian : public PlasticityRule {
public:
    // Constructor
    Hebbian(float learningRate = 0.001f, float cooperationThreshold = 0.5f);
    
    // Apply Hebbian plasticity
    void apply(Synapse& synapse,
              const Neuron& preNeuron,
              const Neuron& postNeuron,
              RandomGenerator& rng) override;
    
    // Get learning rate
    float getLearningRate() const override;
    void setLearningRate(float rate) override;
    
    // Get cooperation threshold
    float getCooperationThreshold() const;
    void setCooperationThreshold(float threshold);
    
    // Get competition coefficient
    float getCompetitionCoefficient() const;
    void setCompetitionCoefficient(float coeff);
    
    // Get homeostatic target
    float getHomeostaticTarget() const;
    void setHomeostaticTarget(float target);
    
    // Get weight regularization
    float getWeightRegularization() const;
    void setWeightRegularization(float reg);
    
    // Apply homeostatic regulation
    void applyHomeostasis(const std::vector<Synapse*>& synapses);
    
    // Calculate co-activity measure
    float calculateCoActivity(const Neuron& preNeuron,
                             const Neuron& postNeuron) const;
    
private:
    float learningRate_;
    float cooperationThreshold_;
    float competitionCoefficient_;
    float homeostaticTarget_;
    float weightRegularization_;
};
```

### StructuralPlasticity

Synaptogenesis and pruning system.

#### Class Interface
```cpp
class StructuralPlasticity {
public:
    // Constructor
    StructuralPlasticity(float formationRate = 0.01f,
                        float pruningRate = 0.005f,
                        float competitionThreshold = 0.1f);
    
    // Update structural plasticity (called periodically)
    void update(Brain* brain, RandomGenerator& rng);
    
    // Schedule synaptogenesis for new connections
    void scheduleSynaptogenesis(Brain* brain, RandomGenerator& rng);
    
    // Schedule synaptic pruning
    void schedulePruning(Brain* brain, RandomGenerator& rng);
    
    // Get formation rate
    float getFormationRate() const;
    void setFormationRate(float rate);
    
    // Get pruning rate
    float getPruningRate() const;
    void setPruningRate(float rate);
    
    // Get competition threshold
    float getCompetitionThreshold() const;
    void setCompetitionThreshold(float threshold);
    
    // Get minimum connection strength
    float getMinConnectionStrength() const;
    void setMinConnectionStrength(float strength);
    
    // Get maximum connections per neuron
    size_t getMaxConnectionsPerNeuron() const;
    void setMaxConnectionsPerNeuron(size_t max);
    
    // Get development modulation
    float getDevelopmentFormationFactor() const;
    float getDevelopmentPruningFactor() const;
    
    // Set development modulation
    void setDevelopmentFactors(float formation, float pruning);
    
    // Get connectivity statistics
    float getCurrentAverageConnections() const;
    float getTargetAverageConnections() const;
    
private:
    float formationRate_;
    float pruningRate_;
    float competitionThreshold_;
    float minConnectionStrength_;
    size_t maxConnectionsPerNeuron_;
    float developmentFormationFactor_;
    float developmentPruningFactor_;
    
    // Helper functions for synaptogenesis
    bool shouldFormConnection(const Neuron& preNeuron,
                             const Neuron& postNeuron,
                             RandomGenerator& rng) const;
    
    // Helper functions for pruning
    bool shouldPruneConnection(const Synapse& synapse,
                              const Neuron& preNeuron,
                              const Neuron& postNeuron,
                              RandomGenerator& rng) const;
}; 
```

## Development Layer

### DevelopmentSystem

Orchestrates developmental stages and age-dependent changes.

#### Class Interface
```cpp
class DevelopmentSystem {
public:
    // Constructor
    DevelopmentSystem(SimulationStep criticalPeriodStart = 1000,
                     SimulationStep criticalPeriodEnd = 10000);
    
    // Update development system
    void update(Brain* brain, SimulationStep currentStep);
    
    // Get current developmental stage
    DevelopmentalStage getCurrentStage() const;
    
    // Get stage progress (0.0 to 1.0)
    float getStageProgress() const;
    
    // Check if in critical period
    bool isInCriticalPeriod() const;
    
    // Get critical period start
    SimulationStep getCriticalPeriodStart() const;
    
    // Get critical period end
    SimulationStep getCriticalPeriodEnd() const;
    
    // Set critical period
    void setCriticalPeriod(SimulationStep start, SimulationStep end);
    
    // Get developmental rates for different processes
    float getPlasticityRate() const;
    float getSynaptogenesisRate() const;
    float getPruningRate() const;
    float getMaturationRate() const;
    
    // Set developmental rates
    void setPlasticityRate(float rate);
    void setSynaptogenesisRate(float rate);
    void setPruningRate(float rate);
    void setMaturationRate(float rate);
    
    // Get development state
    bool isDeveloping() const;
    SimulationStep getDevelopmentStep() const;
    
    // Reset development
    void reset();
    
    // Get development statistics
    SimulationStep getTotalDevelopmentTime() const;
    float getAveragePlasticity() const;
    
private:
    DevelopmentalStage currentStage_;
    float stageProgress_;
    SimulationStep criticalPeriodStart_;
    SimulationStep criticalPeriodEnd_;
    SimulationStep developmentStep_;
    
    SimulationStep totalDevelopmentTime_;
    float averagePlasticity_;
    
    // Update current stage based on progress
    void updateStage(SimulationStep currentStep);
    
    // Calculate plasticity rate for current stage
    float getStagePlasticityRate(DevelopmentalStage stage) const;
    
    // Calculate other developmental rates
    float getStageSynaptogenesisRate(DevelopmentalStage stage) const;
    float getStagePruningRate(DevelopmentalStage stage) const;
    float getStageMaturationRate(DevelopmentalStage stage) const;
}; 
```

### Synaptogenesis

New synapse formation system.

#### Class Interface
```cpp
class Synaptogenesis {
public:
    // Constructor
    Synaptogenesis(float formationRate = 0.01f,
                   float competitionThreshold = 0.1f,
                   size_t maxConnectionsPerNeuron = 50);
    
    // Schedule new synapse formation
    void scheduleFormation(Brain* brain, RandomGenerator& rng);
    
    // Check if connection should form
    bool shouldFormConnection(const Neuron& preNeuron,
                             const Neuron& postNeuron,
                             RandomGenerator& rng) const;
    
    // Get formation rate
    float getFormationRate() const;
    void setFormationRate(float rate);
    
    // Get competition threshold
    float getCompetitionThreshold() const;
    void setCompetitionThreshold(float threshold);
    
    // Get maximum connections per neuron
    size_t getMaxConnectionsPerNeuron() const;
    void setMaxConnectionsPerNeuron(size_t max);
    
    // Get development factor
    float getDevelopmentFactor() const;
    void setDevelopmentFactor(float factor);
    
    // Apply development modulation
    void applyDevelopment(float stageFactor, Brain* brain);
    
    // Get connectivity statistics
    float getCurrentAverageConnections() const;
    
private:
    float formationRate_;
    float competitionThreshold_;
    size_t maxConnectionsPerNeuron_;
    float developmentFactor_;
    
    // Helper functions for connection evaluation
    float calculateConnectionLikelihood(const Neuron& preNeuron,
                                        const Neuron& postNeuron) const;
    
    // Check developmental constraints
    bool checkDevelopmentalConstraints(const Neuron& preNeuron,
                                        const Neuron& postNeuron,
                                        SimulationStep currentStep) const;
}; 
```

### Pruning

Weak synapse elimination system.

#### Class Interface
```cpp
class Pruning {
public:
    // Constructor
    Pruning(float pruningRate = 0.005f,
            float competitionThreshold = 0.1f,
            float minConnectionStrength = 0.01f);
    
    // Schedule synaptic pruning
    void schedulePruning(Brain* brain, RandomGenerator& rng);
    
    // Check if synapse should be pruned
    bool shouldPruneConnection(const Synapse& synapse,
                              const Neuron& preNeuron,
                              const Neuron& postNeuron,
                              RandomGenerator& rng) const;
    
    // Get pruning rate
    float getPruningRate() const;
    void setPruningRate(float rate);
    
    // Get competition threshold
    float getCompetitionThreshold() const;
    void setCompetitionThreshold(float threshold);
    
    // Get minimum connection strength
    float getMinConnectionStrength() const;
    void setMinConnectionStrength(float strength);
    
    // Get development factor
    float getDevelopmentFactor() const;
    void setDevelopmentFactor(float factor);
    
    // Apply development modulation
    void applyDevelopment(float stageFactor, Brain* brain);
    
    // Get pruning statistics
    size_t getTotalSynapsesPruned() const;
    float getPruningEfficiency() const;
    
private:
    float pruningRate_;
    float competitionThreshold_;
    float minConnectionStrength_;
    float developmentFactor_;
    
    size_t totalSynapsesPruned_;
    
    // Helper functions for pruning evaluation
    float calculateConnectionStrength(const Synapse& synapse,
                                      const Neuron& preNeuron,
                                      const Neuron& postNeuron) const;
    
    // Check developmental constraints
    bool checkDevelopmentalConstraints(const Synapse& synapse,
                                        const Neuron& preNeuron,
                                        const Neuron& postNeuron,
                                        SimulationStep currentStep) const;
}; 
```

### Maturation

Adult property development and refinement.

#### Class Interface
```cpp
class Maturation {
public:
    // Constructor
    Maturation(float maturationRate = 0.01f,
              float stabilityTarget = 0.9f,
              float refinementRate = 0.005f);
    
    // Update maturation system
    void update(Brain* brain, SimulationStep currentStep);
    
    // Get maturation rate
    float getMaturationRate() const;
    void setMaturationRate(float rate);
    
    // Get stability target
    float getStabilityTarget() const;
    void setStabilityTarget(float target);
    
    // Get refinement rate
    float getRefinementRate() const;
    void setRefinementRate(float rate);
    
    // Get development factor
    float getDevelopmentFactor() const;
    void setDevelopmentFactor(float factor);
    
    // Apply development modulation
    void applyDevelopment(float stageFactor, Brain* brain);
    
    // Get maturation statistics
    float getCurrentStability() const;
    size_t getTotalRefinements() const;
    float getMaturationProgress() const;
    
    // Check if maturation is complete
    bool isMature() const;
    
private:
    float maturationRate_;
    float stabilityTarget_;
    float refinementRate_;
    float developmentFactor_;
    
    float currentStability_;
    size_t totalRefinements_;
    float maturationProgress_;
    bool mature_;
    
    // Calculate current stability
    float calculateStability(const Brain& brain) const;
    
    // Determine if refinement should occur
    bool shouldRefine(const Brain& brain, RandomGenerator& rng) const;
    
    // Apply refinement to specific neurons
    void refineNeuron(Neuron& neuron, RandomGenerator& rng);
}; 
```

## Neuromodulation Layer

### Neuromodulator

Abstract base class for neuromodulatory signals.

#### Class Interface
```cpp
class Neuromodulator {
public:
    // Constructor
    Neuromodulator(float baseline = 0.0f);
    
    // Get current neuromodulator level
    float getLevel() const;
    
    // Set neuromodulator level
    void setLevel(float level);
    
    // Get baseline level
    float getBaseline() const;
    
    // Set baseline level
    void setBaseline(float baseline);
    
    // Get modulation strength
    float getModulationStrength() const;
    
    // Set modulation strength
    void setModulationStrength(float strength);
    
    // Update neuromodulator (typically called each simulation step)
    virtual void update(Time timestep) = 0;
    
    // Reset neuromodulator
    virtual void reset() = 0;
    
    // Get neuromodulator type
    virtual NeuromodulatorType getType() const = 0;
    
    // Get neuromodulator name
    virtual std::string getName() const = 0;
    
    // Apply neuromodulation to plasticity
    virtual void applyToPlasticity(PlasticityRule& rule) = 0;
    
    // Apply neuromodulation to neuron excitability
    virtual void applyToNeuron(Neuron& neuron) = 0;
    
    // Check if neuromodulator is active
    virtual bool isActive() const = 0;
    
    virtual ~Neuromodulator() = default;
};
```

### Dopamine

Reward prediction error signaling system.

#### Class Interface
```cpp
class Dopamine : public Neuromodulator {
public:
    // Constructor
    Dopamine(float baseline = 0.01f, float phasicGain = 1.0f,
            float tonicGain = 0.1f, float eligibilityTraceDecay = 0.9f);
    
    // Update dopamine system
    void update(Time timestep) override;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Set prediction error
    void setPredictionError(float error);
    
    // Get phasic component
    float getPhasic() const;
    
    // Get tonic component
    float getTonic() const;
    
    // Set gains
    void setPhasicGain(float gain);
    void setTonicGain(float gain);
    
    // Get eligibility trace decay
    float getEligibilityTraceDecay() const;
    void setEligibilityTraceDecay(float decay);
    
    // Get synaptic scaling factor
    float getSynapticScalingFactor() const;
    void setSynapticScalingFactor(float factor);
    
    // Get dopamine type
    NeuromodulatorType getType() const override;
    
    // Get name
    std::string getName() const override;
    
    // Apply to STDP
    void applyToPlasticity(PlasticityRule& rule) override;
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron) override;
    
    // Check if active
    bool isActive() const override;
    
    // Reset to baseline
    void reset() override;
    
    // Update with reward
    void updateWithReward(float reward, float predictedReward);
    
    // Get reward prediction error
    float calculatePredictionError(float reward, float predictedReward);
    
    // Apply eligibility trace
    void applyEligibilityTrace(Synapse& synapse);
    
    // Update synaptic scaling
    void updateSynapticScaling(const std::vector<Synapse*>& synapses);
    
private:
    float baseline_;
    float phasicGain_;
    float tonicGain_;
    float eligibilityTraceDecay_;
    float synapticScalingFactor_;
    
    float predictionError_;
    float phasic_;
    float tonic_;
    float eligibilityTrace_;
    
    // Update phasic component
    void updatePhasic(float error);
    
    // Update tonic component
    void updateTonic(float error);
}; 
```

### Novelty

Change detection system.

#### Class Interface
```cpp
class Novelty : public Neuromodulator {
public:
    // Constructor
    Novelty(float baseline = 0.01f, float sensitivity = 0.5f,
            float decayRate = 0.95f, float threshold = 0.1f);
    
    // Update novelty system
    void update(Time timestep) override;
    
    // Get novelty detector
    NoveltyDetector* getDetector() const;
    
    // Set novelty detector
    void setDetector(std::unique_ptr<NoveltyDetector> detector);
    
    // Get sensitivity
    float getSensitivity() const;
    void setSensitivity(float sensitivity);
    
    // Get decay rate
    float getDecayRate() const;
    void setDecayRate(float decayRate);
    
    // Get threshold
    float getThreshold() const;
    void setThreshold(float threshold);
    
    // Get memory window size
    size_t getMemoryWindowSize() const;
    void setMemoryWindowSize(size_t size);
    
    // Add sensory pattern to novelty detector
    void addPattern(const std::vector<float>& pattern);
    
    // Get novelty score for pattern
    float getNoveltyScore(const std::vector<float>& pattern) const;
    
    // Get novelty type
    NeuromodulatorType getType() const override;
    
    // Get name
    std::string getName() const override;
    
    // Apply to plasticity
    void applyToPlasticity(PlasticityRule& rule) override;
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron) override;
    
    // Check if active
    bool isActive() const override;
    
    // Reset to baseline
    void reset() override;
    
    // Update with new sensory input
    void updateWithSensoryInput(const std::vector<float>& input);
    
    // Calculate novelty score
    float calculateNovelty(const std::vector<float>& currentPattern,
                         const std::vector<std::vector<float>>& memory) const;
    
private:
    float baseline_;
    float sensitivity_;
    float decayRate_;
    float threshold_;
    
    std::unique_ptr<NoveltyDetector> detector_;
    std::vector<std::vector<float>> noveltyMemory_;
}; 
```

### Curiosity

Exploration motivation system.

#### Class Interface
```cpp
class Curiosity : public Neuromodulator {
public:
    // Constructor
    Curiosity(float baseline = 0.01f, float noveltyWeight = 0.5f,
              float predictionErrorWeight = 0.5f,
              float explorationBonus = 0.1f);
    
    // Update curiosity system
    void update(Time timestep) override;
    
    // Get novelty system
    Novelty* getNovelty() const;
    
    // Set novelty system
    void setNovelty(Novelty* novelty);
    
    // Get prediction error system
    PredictionError* getPredictionError() const;
    
    // Set prediction error system
    void setPredictionError(PredictionError* predictionError);
    
    // Get novelty weight
    float getNoveltyWeight() const;
    void setNoveltyWeight(float weight);
    
    // Get prediction error weight
    float getPredictionErrorWeight() const;
    void setPredictionErrorWeight(float weight);
    
    // Get exploration bonus
    float getExplorationBonus() const;
    void setExplorationBonus(float bonus);
    
    // Get curiosity threshold
    float getCuriosityThreshold() const;
    void setCuriosityThreshold(float threshold);
    
    // Get exploration rate
    float getExplorationRate() const;
    void setExplorationRate(float rate);
    
    // Get exploitation rate
    float getExploitationRate() const;
    void setExploitationRate(float rate);
    
    // Calculate exploration-exploitation balance
    float calculateExplorationExploitationBalance() const;
    
    // Determine if should explore
    bool shouldExplore() const;
    
    // Get curiosity type
    NeuromodulatorType getType() const override;
    
    // Get name
    std::string getName() const override;
    
    // Apply to plasticity
    void applyToPlasticity(PlasticityRule& rule) override;
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron) override;
    
    // Check if active
    bool isActive() const override;
    
    // Reset to baseline
    void reset() override;
    
    // Update with environmental changes
    void updateWithEnvironmentChange(const std::vector<float>& change);
    
    // Calculate curiosity motivation
    float calculateCuriosityMotivation() const;
    
    // Adjust exploration-exploitation balance
    void adjustExplorationExploitation(float reward, bool success);
    
private:
    float baseline_;
    float noveltyWeight_;
    float predictionErrorWeight_;
    float explorationBonus_;
    float curiosityThreshold_;
    float explorationRate_;
    float exploitationRate_;
    
    Novelty* novelty_;
    PredictionError* predictionError_;
    
    mutable float accumulatedNovelty_;
    mutable float accumulatedPredictionError_;
}; 
```

### PredictionError

Error signal computation for learning.

#### Class Interface
```cpp
class PredictionError : public Neuromodulator {
public:
    // Constructor
    PredictionError(float baseline = 0.01f, float predictionGain = 1.0f,
                    float errorDecay = 0.9f, float confidenceWeight = 0.5f);
    
    // Update prediction error system
    void update(Time timestep) override;
    
    // Get predictor system
    PredictionSystem* getPredictor() const;
    
    // Set predictor system
    void setPredictor(PredictionSystem* predictor);
    
    // Get prediction gain
    float getPredictionGain() const;
    void setPredictionGain(float gain);
    
    // Get error decay rate
    float getErrorDecayRate() const;
    void setErrorDecayRate(float decay);
    
    // Get confidence weight
    float getConfidenceWeight() const;
    void setConfidenceWeight(float weight);
    
    // Get prediction error window size
    size_t getPredictionWindowSize() const;
    void setPredictionWindowSize(size_t size);
    
    // Update with predicted and actual values
    void update(float predicted, float actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getPredictionConfidence() const;
    
    // Get prediction accuracy
    float getPredictionAccuracy() const;
    
    // Get prediction type
    NeuromodulatorType getType() const override;
    
    // Get name
    std::string getName() const override;
    
    // Apply to plasticity
    void applyToPlasticity(PlastictyRule& rule) override;
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron) override;
    
    // Check if active
    bool isActive() const override;
    
    // Reset to baseline
    void reset() override;
    
    // Update prediction
    void updatePrediction(const std::vector<float>& sensoryInput);
    
    // Calculate prediction error
    float calculatePredictionError(float predicted, float actual) const;
    
    // Calculate prediction confidence
    float calculatePredictionConfidence(float error) const;
    
    // Update prediction accuracy
    void updatePredictionAccuracy(float accuracy);
    
    // Get prediction error statistics
    float getMeanSquaredError() const;
    float getRootMeanSquaredError() const;
    
private:
    float baseline_;
    float predictionGain_;
    float errorDecayRate_;
    float confidenceWeight_;
    
    PredictionSystem* predictor_;
    
    float predictionError_;
    float predictionConfidence_;
    float predictionAccuracy_;
    
    std::vector<float> predictionHistory_;
    float meanSquaredError_;
    
    // Update prediction error
    void updateError(float error);
    
    // Calculate mean squared error
    void calculateMeanSquaredError(float error);
}; 
```

## Memory Layer

### NeuralWorkingMemory

Persistent activity for temporary information storage.

#### Class Interface
```cpp
class NeuralWorkingMemory {
public:
    // Constructor
    NeuralWorkingMemory(uint32_t capacity = 384,
                       float persistenceTime = 1000.0f,
                       float competitionStrength = 1.0f);
    
    // Update working memory
    void update(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Store information in working memory
    void store(const std::vector<float>& pattern, 
               const std::string& label = "");
    
    // Retrieve information from working memory
    std::optional<std::vector<float>> retrieve(const std::string& label) const;
    std::vector<std::pair<std::string, std::vector<float>>> 
        retrieveSimilar(const std::vector<float>& pattern,
                       float similarityThreshold = 0.7f) const;
    
    // Get working memory capacity
    uint32_t getCapacity() const;
    
    // Get persistence time
    float getPersistenceTime() const;
    void setPersistenceTime(float time);
    
    // Get competition strength
    float getCompetitionStrength() const;
    void setCompetitionStrength(float strength);
    
    // Get current stored patterns
    std::vector<std::pair<std::string, std::vector<float>>> 
        getStoredPatterns() const;
    
    // Get pattern by label
    std::optional<std::vector<float>> getPattern(const std::string& label) const;
    
    // Update pattern activity
    void updatePatternActivity(const std::string& label, 
                              float activityChange);
    
    // Remove pattern
    void removePattern(const std::string& label);
    
    // Clear all patterns
    void clear();
    
    // Get working memory statistics
    size_t getActivePatternCount() const;
    float getAveragePatternActivity() const;
    float getMemoryUtilization() const;
    
    // Reset working memory
    void reset();
    
    // Save working memory
    void save(std::ostream& out) const;
    
    // Load working memory
    void load(std::istream& in);
    
    // Apply development changes
    void applyDevelopmentChanges(const DevelopmentSystem& dev);
    
    // Synchronize with brain
    void synchronizeWithBrain(const Brain& brain);
    
private:
    struct PatternInfo {
        std::vector<float> pattern;
        std::string label;
        float activity;
        Timestamp lastUpdated;
        Timestamp lastAccessed;
        size_t accessCount;
        
        PatternInfo(const std::vector<float>& p, const std::string& l)
            : pattern(p), label(l), activity(0.0f), 
              lastUpdated(0), lastAccessed(0), accessCount(0) {}
    };
    
    uint32_t capacity_;
    float persistenceTime_;
    float competitionStrength_;
    
    std::vector<PatternInfo> patterns_;
    
    // Find neuron group for pattern storage
    PopulationId findNeuronGroupForPattern(const std::vector<float>& pattern) const;
    
    // Get neuron group for working memory
    PopulationId getWorkingMemoryNeuronGroup(NeuronType type) const;
    
    // Update pattern competition
    void updatePatternCompetition();
    
    // Apply decay to patterns
    void applyDecay(Timestamp currentTime);
}; 
```

### NeuralEpisodicMemory

Experience storage with replay consolidation.

#### Class Interface
```cpp
class NeuralEpisodicMemory {
public:
    // Constructor
    NeuralEpisodicMemory(uint32_t capacity = 10000,
                         float consolidationInterval = 100.0f,
                         float replayStrength = 0.8f);
    
    // Update episodic memory
    void update(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Store experience
    void storeExperience(const Experience& experience);
    
    // Retrieve experience
    std::optional<Experience> retrieveExperience(size_t index) const;
    
    // Retrieve similar experiences
    std::vector<Experience> retrieveSimilar(const std::vector<float>& pattern,
                                             float similarityThreshold = 0.7f) const;
    
    // Get memory capacity
    uint32_t getCapacity() const;
    
    // Get consolidation interval
    float getConsolidationInterval() const;
    void setConsolidationInterval(float interval);
    
    // Get replay strength
    float getReplayStrength() const;
    void setReplayStrength(float strength);
    
    // Get current experiences
    std::vector<Experience> getExperiences() const;
    
    // Get experience count
    size_t getExperienceCount() const;
    
    // Update experience relevance
    void updateExperienceRelevance(size_t index, float relevanceChange);
    
    // Remove experience
    void removeExperience(size_t index);
    
    // Clear all experiences
    void clear();
    
    // Get memory statistics
    size_t getActiveExperienceCount() const;
    float getMemoryUtilization() const;
    float getAverageExperienceRelevance() const;
    
    // Reset memory
    void reset();
    
    // Save memory
    void save(std::ostream& out) const;
    
    // Load memory
    void load(std::istream& in);
    
    // Apply development changes
    void applyDevelopmentChanges(const DevelopmentSystem& dev);
    
    // Synchronize with brain
    void synchronizeWithBrain(const Brain& brain);
    
    // Trigger replay consolidation
    void triggerReplayConsolidation(SimulationStep currentStep);
    
    // Get replay events
    std::vector<std::pair<Timestamp, std::vector<float>>> 
        getRecentReplayEvents() const;
    
private:
    uint32_t capacity_;
    float consolidationInterval_;
    float replayStrength_;
    
    std::vector<Experience> experiences_;
    
    // Find similar experiences
    std::vector<size_t> findSimilarExperiences(
        const std::vector<float>& pattern,
        float similarityThreshold) const;
    
    // Calculate experience similarity
    float calculateExperienceSimilarity(
        const Experience& exp1,
        const Experience& exp2) const;
    
    // Trigger replay for experience
    void triggerReplayForExperience(const Experience& experience,
                                    Timestamp currentTime);
    
    // Consolidate experiences
    void consolidateExperiences();
    
    // Update experience relevance
    void updateExperienceRelevance();
}; 
```

### NeuralAssociativeMemory

Pattern association and completion system.

#### Class Interface
```cpp
class NeuralAssociativeMemory {
public:
    // Constructor
    NeuralAssociativeMemory(float connectionThreshold = 0.1f,
                            float spreadActivationStrength = 1.0f,
                            float completionThreshold = 0.8f);
    
    // Update associative memory
    void update(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Learn association
    void learnAssociation(const std::vector<float>& pattern1,
                          const std::vector<float>& pattern2,
                          float strength = 1.0f);
    
    // Retrieve pattern completion
    std::optional<std::vector<float>> completePattern(
        const std::vector<float>& partialPattern) const;
    
    // Get association strength
    std::optional<float> getAssociationStrength(
        const std::vector<float>& pattern1,
        const std::vector<float>& pattern2) const;
    
    // Get similar patterns
    std::vector<std::pair<std::vector<float>, float>> 
        getSimilarPatterns(const std::vector<float>& pattern,
                          float similarityThreshold = 0.7f) const;
    
    // Get association statistics
    float getAverageAssociationStrength() const;
    float getAssociationDensity() const;
    
    // Update association strength
    void updateAssociationStrength(const std::vector<float>& pattern1,
                                   const std::vector<float>& pattern2,
                                   float strengthChange);
    
    // Remove association
    void removeAssociation(const std::vector<float>& pattern1,
                           const std::vector<float>& pattern2);
    
    // Clear all associations
    void clear();
    
    // Get memory statistics
    size_t getActiveAssociationCount() const;
    float getMemoryUtilization() const;
    
    // Reset memory
    void reset();
    
    // Save memory
    void save(std::ostream& out) const;
    
    // Load memory
    void load(std::istream& in);
    
    // Apply development changes
    void applyDevelopmentChanges(const DevelopmentSystem& dev);
    
    // Synchronize with brain
    void synchronizeWithBrain(const Brain& brain);
    
private:
    float connectionThreshold_;
    float spreadActivationStrength_;
    float completionThreshold_;
    
    // Association storage
    std::unordered_map<std::vector<float>, 
                      std::unordered_map<std::vector<float>, float>> associations_;
    
    // Pattern storage
    std::vector<std::vector<float>> patterns_;
    
    // Calculate pattern similarity
    float calculatePatternSimilarity(const std::vector<float>& pattern1,
                                     const std::vector<float>& pattern2) const;
    
    // Get associated patterns
    std::vector<std::pair<std::vector<float>, float>> 
        getAssociatedPatterns(const std::vector<float>& pattern) const;
    
    // Complete pattern using spread activation
    std::vector<float> completePatternUsingSpreadActivation(
        const std::vector<float>& partialPattern) const;
}; 
```

## Prediction Layer

### NeuralPrediction

Temporal sequence learning and prediction system.

#### Class Interface
```cpp
class NeuralPrediction {
public:
    // Constructor
    NeuralPrediction(size_t predictionWindow = 10,
                     float predictionConfidenceThreshold = 0.5f,
                     float learningRate = 0.01f);
    
    // Update prediction system
    void update(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Train prediction model
    void train(const std::vector<std::vector<float>>& sequence);
    
    // Make prediction
    std::optional<std::vector<float>> predict(const std::vector<float>& currentState) const;
    
    // Get prediction confidence
    float getPredictionConfidence(const std::vector<float>& prediction) const;
    
    // Get prediction accuracy
    float getPredictionAccuracy() const;
    
    // Update prediction accuracy
    void updatePredictionAccuracy(float accuracy);
    
    // Get prediction statistics
    size_t getPredictionCount() const;
    float getAverageConfidence() const;
    
    // Reset prediction model
    void reset();
    
    // Get prediction history
    std::vector<std::vector<float>> getPredictionHistory() const;
    
    // Get sequence learning statistics
    float getSequenceLearningProgress() const;
    
    // Get prediction type
    NeuromodulatorType getType() const;
    
    // Get name
    std::string getName() const;
    
    // Apply to plasticity
    void applyToPlasticity(PlasticityRule& rule);
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron);
    
    // Check if active
    bool isActive() const;
    
    // Save prediction model
    void save(std::ostream& out) const;
    
    // Load prediction model
    void load(std::istream& in);
    
    // Apply development changes
    void applyDevelopmentChanges(const DevelopmentSystem& dev);
    
private:
    size_t predictionWindow_;
    float predictionConfidenceThreshold_;
    float learningRate_;
    
    std::vector<std::vector<float>> sequenceModel_;
    std::vector<std::vector<float>> predictionHistory_;
    
    float predictionAccuracy_;
    size_t predictionCount_;
    
    // Calculate prediction
    std::vector<float> calculatePrediction(
        const std::vector<float>& currentState) const;
    
    // Calculate prediction confidence
    float calculatePredictionConfidence(
        const std::vector<float>& prediction,
        const std::vector<float>& actual) const;
    
    // Learn from prediction error
    void learnFromPredictionError(const std::vector<float>& prediction,
                                  const std::vector<float>& actual);
}; 
```

### PredictionSystem

Hierarchical temporal processing and action consequence prediction.

#### Class Interface
```cpp
class PredictionSystem {
public:
    // Constructor
    PredictionSystem(size_t hierarchyLevels = 3,
                     float predictionConfidenceThreshold = 0.5f,
                     float predictionErrorThreshold = 0.1f);
    
    // Update prediction system
    void update(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Add prediction target
    void addPredictionTarget(const std::vector<float>& target,
                            const std::string& label = "");
    
    // Make prediction for action
    std::optional<std::vector<float>> predictActionConsequences(
        const Action& action,
        const std::vector<float>& currentState) const;
    
    // Get prediction confidence
    float getPredictionConfidence(const std::vector<float>& prediction) const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Update prediction error
    void updatePredictionError(float error);
    
    // Get prediction statistics
    size_t getPredictionCount() const;
    float getAverageConfidence() const;
    
    // Get system statistics
    float getPredictionAccuracy() const;
    float getSequenceLearningProgress() const;
    
    // Reset prediction system
    void reset();
    
    // Get prediction targets
    std::vector<std::pair<std::string, std::vector<float>>> 
        getPredictionTargets() const;
    
    // Get prediction history
    std::vector<std::vector<float>> getPredictionHistory() const;
    
    // Get prediction type
    NeuromodulatorType getType() const;
    
    // Get name
    std::string getName() const;
    
    // Apply to plasticity
    void applyToPlasticity(PlasticityRule& rule);
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron);
    
    // Check if active
    bool isActive() const;
    
    // Save prediction system
    void save(std::ostream& out) const;
    
    // Load prediction system
    void load(std::istream& in);
    
    // Apply development changes
    void applyDevelopmentChanges(const DevelopmentSystem& dev);
    
private:
    size_t hierarchyLevels_;
    float predictionConfidenceThreshold_;
    float predictionErrorThreshold_;
    
    std::vector<std::vector<float>> predictionModel_;
    std::vector<std::vector<float>> predictionHistory_;
    
    float predictionError_;
    size_t predictionCount_;
    
    // Calculate hierarchical prediction
    std::vector<float> calculateHierarchicalPrediction(
        const std::vector<float>& currentState,
        size_t level) const;
    
    // Make action consequence prediction
    std::vector<float> predictActionConsequences(
        const Action& action,
        const std::vector<float>& currentState,
        size_t level) const;
    
    // Update prediction model
    void updatePredictionModel(const std::vector<std::vector<float>>& sequences);
    
    // Calculate prediction error
    float calculatePredictionError(
        const std::vector<float>& predicted,
        const std::vector<float>& actual) const;
}; 
```

## Cognitive Layer

### AttentionalSelection

Competitive selection system for attention allocation (NOT Transformer QKV).

#### Class Interface
```cpp
class AttentionalSelection {
public:
    // Constructor
    AttentionalSelection(size_t maxAttendedItems = 5,
                         float competitionStrength = 1.0f,
                         float attentionDecayRate = 0.95f);
    
    // Update attentional selection
    void update(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Add item to attention competition
    void addItemToAttention(const std::vector<float>& item,
                           float priority = 1.0f,
                           const std::string& label = "");
    
    // Get attended items
    std::vector<std::pair<std::string, std::vector<float>>> 
        getAttendedItems() const;
    
    // Get attended item by label
    std::optional<std::vector<float>> getAttendedItem(const std::string& label) const;
    
    // Get attention allocation
    float getAttentionAllocation(const std::string& label) const;
    
    // Get attended item count
    size_t getAttendedItemCount() const;
    
    // Remove item from attention
    void removeItemFromAttention(const std::string& label);
    
    // Clear all attended items
    void clearAttendedItems();
    
    // Get attention statistics
    size_t getTotalItemsProcessed() const;
    float getAverageAttentionAllocation() const;
    
    // Reset attentional selection
    void reset();
    
    // Get attentional selection type
    NeuromodulatorType getType() const;
    
    // Get name
    std::string getName() const;
    
    // Apply to plasticity
    void applyToPlasticity(PlasticityRule& rule);
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron);
    
    // Check if active
    bool isActive() const;
    
    // Save attentional selection
    void save(std::ostream& out) const;
    
    // Load attentional selection
    void load(std::istream& in);
    
    // Apply development changes
    void applyDevelopmentChanges(const DevelopmentSystem& dev);
    
private:
    size_t maxAttendedItems_;
    float competitionStrength_;
    float attentionDecayRate_;
    
    std::vector<std::pair<std::string, std::vector<float>>> attendedItems_;
    std::vector<float> attentionAllocations_;
    
    size_t totalItemsProcessed_;
    
    // Calculate attention allocation
    std::vector<float> calculateAttentionAllocations() const;
    
    // Update attention dynamics
    void updateAttentionDynamics(Timestamp currentTime);
    
    // Determine winner-take-all
    void determineWinnerTakeAll();
    
    // Apply lateral inhibition
    void applyLateralInhibition();
}; 
```

### NeuralPlanner

Predictive action selection through Monte Carlo tree search in neural space.

#### Class Interface
```cpp
class NeuralPlanner {
public:
    // Constructor
    NeuralPlanner(size_t planningHorizon = 10,
                 float explorationRate = 0.1f,
                 float exploitationRate = 0.9f,
                 size_t monteCarloSamples = 1000);
    
    // Update planner
    void update(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Add action to planning consideration
    void addActionToPlan(const Action& action,
                        const std::vector<float>& currentState,
                        float immediateReward);
    
    // Get planned action
    std::optional<Action> getPlannedAction() const;
    
    // Get action evaluation score
    float getActionScore(const Action& action) const;
    
    // Get planning statistics
    size_t getActionCount() const;
    float getAverageActionScore() const;
    
    // Get planner statistics
    size_t getTotalPlansGenerated() const;
    float getPlanningAccuracy() const;
    
    // Reset planner
    void reset();
    
    // Get planner type
    NeuromodulatorType getType() const;
    
    // Get name
    std::string getName() const;
    
    // Apply to plasticity
    void applyToPlasticity(PlasticityRule& rule);
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron);
    
    // Check if active
    bool isActive() const;
    
    // Save planner
    void save(std::ostream& out) const;
    
    // Load planner
    void load(std::istream& in);
    
    // Apply development changes
    void applyDevelopmentChanges(const DevelopmentSystem& dev);
    
private:
    size_t planningHorizon_;
    float explorationRate_;
    float exploitationRate_;
    size_t monteCarloSamples_;
    
    std::vector<std::pair<Action, float>> actionPlans_;
    std::vector<float> actionScores_;
    
    size_t totalPlansGenerated_;
    float planningAccuracy_;
    
    // Generate plan using Monte Carlo search
    void generatePlan(const std::vector<float>& currentState,
                     const std::vector<Action>& availableActions);
    
    // Evaluate action sequence
    float evaluateActionSequence(
        const std::vector<Action>& sequence,
        const std::vector<float>& currentState) const;
    
    // Update planning based on outcome
    void updatePlanningBasedOnOutcome(const Action& chosenAction,
                                     float actualOutcome);
}; 
```

### ConceptFormation

Pattern discovery and abstraction system.

#### Class Interface
```cpp
class ConceptFormation {
public:
    // Constructor
    ConceptFormation(size_t conceptCapacity = 100,
                     float abstractionLevel = 0.5f,
                     float stabilityThreshold = 0.8f);
    
    // Update concept formation
    void update(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Learn concept from pattern
    void learnConcept(const std::vector<float>& pattern);
    
    // Get concept for pattern
    std::optional<std::vector<float>> getConcept(const std::vector<float>& pattern) const;
    
    // Get concept prototypes
    std::vector<std::vector<float>> getConceptPrototypes() const;
    
    // Get concept statistics
    size_t getConceptCount() const;
    float getConceptAbstractionLevel() const;
    
    // Get concept stability
    float getConceptStability() const;
    
    // Get concept formation statistics
    size_t getTotalPatternsProcessed() const;
    float getLearningProgress() const;
    
    // Reset concept formation
    void reset();
    
    // Get concept formation type
    NeuromodulatorType getType() const;
    
    // Get name
    std::string getName() const;
    
    // Apply to plasticity
    void applyToPlasticity(PlasticityRule& rule);
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron);
    
    // Check if active
    bool isActive() const;
    
    // Save concept formation
    void save(std::ostream& out) const;
    
    // Load concept formation
    void load(std::istream& in);
    
    // Apply development changes
    void applyDevelopmentChanges(const DevelopmentSystem& dev);
    
private:
    size_t conceptCapacity_;
    float abstractionLevel_;
    float stabilityThreshold_;
    
    std::vector<std::vector<float>> conceptPrototypes_;
    
    size_t totalPatternsProcessed_;
    float learningProgress_;
    
    // Calculate concept abstraction
    float calculateConceptAbstraction(const std::vector<float>& prototype) const;
    
    // Learn concept from pattern
    void learnConceptFromPattern(const std::vector<float>& pattern);
    
    // Find similar concept
    size_t findSimilarConcept(const std::vector<float>& pattern) const;
    
    // Update concept stability
    void updateConceptStability();
}; 
```

### SelfModel

Sensorimotor self-awareness and capability representation.

#### Class Interface
```cpp
class SelfModel {
public:
    // Constructor
    SelfModel(size_t capabilityCapacity = 50,
              float selfModelConfidenceThreshold = 0.5f,
              float selfModelStabilityRate = 0.95f);
    
    // Update self model
    void update(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Learn self capability
    void learnSelfCapability(const Action& action, float successRate);
    
    // Get self capability
    std::optional<float> getSelfCapability(const Action& action) const;
    
    // Get self capabilities
    std::vector<std::pair<Action, float>> getSelfCapabilities() const;
    
    // Get self model statistics
    size_t getCapabilityCount() const;
    float getSelfModelConfidence() const;
    
    // Update self model
    void updateSelfModel();
    
    // Reset self model
    void reset();
    
    // Get self model type
    NeuromodulatorType getType() const;
    
    // Get name
    std::string getName() const;
    
    // Apply to plasticity
    void applyToPlasticity(PlasticityRule& rule);
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron);
    
    // Check if active
    bool isActive() const;
    
    // Save self model
    void save(std::ostream& out) const;
    
    // Load self model
    void load(std::istream& in);
    
    // Apply development changes
    void applyDevelopmentChanges(const DevelopmentSystem& dev);
    
private:
    size_t capabilityCapacity_;
    float selfModelConfidenceThreshold_;
    float selfModelStabilityRate_;
    
    std::vector<std::pair<Action, float>> selfCapabilities_;
    
    float selfModelConfidence_;
    
    // Calculate self model confidence
    float calculateSelfModelConfidence() const;
    
    // Update self model based on experience
    void updateSelfModelBasedOnExperience(
        const Action& action, float actualSuccessRate);
    
    // Determine action capability
    float determineActionCapability(const Action& action) const;
}; 
```

### SocialLearning

Observation and imitation capability system.

#### Class Interface
```cpp
class SocialLearning {
public:
    // Constructor
    SocialLearning(size_t observationCapacity = 100,
                   float imitationConfidenceThreshold = 0.5f,
                   float observationDecayRate = 0.9f);
    
    // Update social learning
    void update(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Observe other agent
    void observeOtherAgent(const Agent& agent);
    
    // Imitate observed action
    std::optional<Action> imitateObservedAction(
        const Action& targetAction) const;
    
    // Get observation count
    size_t getObservationCount() const;
    
    // Get observation confidence
    float getObservationConfidence(const Action& action) const;
    
    // Get social learning statistics
    size_t getTotalObservations() const;
    float getImitationSuccessRate() const;
    
    // Reset social learning
    void reset();
    
    // Get social learning type
    NeuromodulatorType getType() const;
    
    // Get name
    std::string getName() const;
    
    // Apply to plasticity
    void applyToPlasticity(PlasticityRule& rule);
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron);
    
    // Check if active
    bool isActive() const;
    
    // Save social learning
    void save(std::ostream& out) const;
    
    // Load social learning
    void load(std::istream& in);
    
    // Apply development changes
    void applyDevelopmentChanges(const DevelopmentSystem& dev);
    
private:
    size_t observationCapacity_;
    float imitationConfidenceThreshold_;
    float observationDecayRate_;
    
    std::vector<std::pair<Agent, Action>> observations_;
    
    size_t totalObservations_;
    float imitationSuccessRate_;
    
    // Analyze observation
    void analyzeObservation(const Agent& agent, const Action& action);
    
    // Determine imitation likelihood
    float determineImitationLikelihood(
        const Action& targetAction,
        const Agent& observer) const;
    
    // Update imitation success rate
    void updateImitationSuccessRate(bool success);
}; 
```

## Sensory Layer

### SensoryInput

Base class for all sensory input modalities.

#### Class Interface
```cpp
class SensoryInput {
public:
    // Constructor
    SensoryInput(const std::string& modality);
    
    // Get sensory modality
    const std::string& getModality() const;
    
    // Get sensory values
    virtual std::vector<float> getValues() const = 0;
    
    // Get sensory value by index
    virtual float getValue(size_t index) const = 0;
    
    // Get sensory value by name
    virtual float getValue(const std::string& name) const = 0;
    
    // Get sensory timestamp
    Timestamp getTimestamp() const;
    
    // Set sensory timestamp
    void setTimestamp(Timestamp timestamp);
    
    // Get sensory quality
    virtual float getQuality() const;
    
    // Set sensory quality
    virtual void setQuality(float quality);
    
    // Check if sensory input is valid
    virtual bool isValid() const;
    
    // Get sensory input type
    virtual SensoryInputType getInputType() const = 0;
    
    // Get sensory input name
    virtual std::string getName() const = 0;
    
    // Apply to plasticity
    virtual void applyToPlasticity(PlasticityRule& rule);
    
    // Apply to neuron
    virtual void applyToNeuron(Neuron& neuron);
    
    // Check if active
    virtual bool isActive() const;
    
    virtual ~SensoryInput() = default;
    
    // Save sensory input
    virtual void save(std::ostream& out) const;
    
    // Load sensory input
    virtual void load(std::istream& in);
    
protected:
    std::string modality_;
    Timestamp timestamp_;
};
```

### Vision

Visual input processing system.

#### Class Interface
```cpp
class Vision : public SensoryInput {
public:
    // Constructor
    Vision(size_t width = 64, size_t height = 64,
           float resolution = 0.5f);
    
    // Get visual values
    std::vector<float> getValues() const override;
    
    // Get visual value by index
    float getValue(size_t index) const override;
    
    // Get visual value by name
    float getValue(const std::string& name) const override;
    
    // Get visual dimensions
    size_t getWidth() const;
    size_t getHeight() const;
    
    // Get visual resolution
    float getResolution() const;
    void setResolution(float resolution);
    
    // Get visual features
    std::vector<float> getFeatures() const;
    
    // Get visual pattern
    std::vector<std::vector<float>> getPattern() const;
    
    // Get visual quality
    float getQuality() const override;
    void setQuality(float quality) override;
    
    // Check if valid
    bool isValid() const override;
    
    // Get input type
    SensoryInputType getInputType() const override;
    
    // Get name
    std::string getName() const override;
    
    // Apply to plasticity
    void applyToPlasticity(PlasticityRule& rule) override;
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron) override;
    
    // Check if active
    bool isActive() const override;
    
    // Save visual input
    void save(std::ostream& out) const override;
    
    // Load visual input
    void load(std::istream& in) override;
    
    // Update visual input
    void update(const std::vector<std::vector<float>>& newFrame);
    
    // Detect visual features
    std::vector<float> detectFeatures() const;
    
    // Detect visual changes
    std::vector<float> detectChanges(const Vision& previous) const;
    
private:
    size_t width_;
    size_t height_;
    float resolution_;
    
    std::vector<std::vector<float>> visualPattern_;
    std::vector<float> visualFeatures_;
    
    // Calculate visual complexity
    float calculateVisualComplexity() const;
    
    // Extract visual features
    std::vector<float> extractFeatures() const;
}; 
```

### Audio

Audio input processing system.

#### Class Interface
```cpp
class Audio : public SensoryInput {
public:
    // Constructor
    Audio(size_t channels = 8, float sampleRate = 44100.0f,
          float frequencyRange = 2000.0f);
    
    // Get audio values
    std::vector<float> getValues() const override;
    
    // Get audio value by index
    float getValue(size_t index) const override;
    
    // Get audio value by name
    float getValue(const std::string& name) const override;
    
    // Get audio channels
    size_t getChannels() const;
    
    // Get audio sample rate
    float getSampleRate() const;
    void setSampleRate(float sampleRate);
    
    // Get audio frequency range
    float getFrequencyRange() const;
    void setFrequencyRange(float range);
    
    // Get audio features
    std::vector<float> getFeatures() const;
    
    // Get audio spectrum
    std::vector<std::pair<float, float>> getSpectrum() const;
    
    // Get audio quality
    float getQuality() const override;
    void setQuality(float quality) override;
    
    // Check if valid
    bool isValid() const override;
    
    // Get input type
    SensoryInputType getInputType() const override;
    
    // Get name
    std::string getName() const override;
    
    // Apply to plasticity
    void applyToPlasticity(PlasticityRule& rule) override;
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron) override;
    
    // Check if active
    bool isActive() const override;
    
    // Save audio input
    void save(std::ostream& out) const override;
    
    // Load audio input
    void load(std::istream& in) override;
    
    // Update audio input
    void update(const std::vector<std::vector<float>>& newAudio);
    
    // Detect audio features
    std::vector<float> detectFeatures() const;
    
    // Detect audio changes
    std::vector<float> detectChanges(const Audio& previous) const;
    
private:
    size_t channels_;
    float sampleRate_;
    float frequencyRange_;
    
    std::vector<std::vector<float>> audioData_;
    std::vector<float> audioFeatures_;
    
    // Calculate audio complexity
    float calculateAudioComplexity() const;
    
    // Extract audio features
    std::vector<float> extractFeatures() const;
}; 
```

### InternalSignals

Internal state signals system.

#### Class Interface
```cpp
class InternalSignals : public SensoryInput {
public:
    // Constructor
    InternalSignals(size_t signalCount = 16);
    
    // Get internal signal values
    std::vector<float> getValues() const override;
    
    // Get internal signal value by index
    float getValue(size_t index) const override;
    
    // Get internal signal value by name
    float getValue(const std::string& name) const override;
    
    // Get signal count
    size_t getSignalCount() const;
    
    // Get signal names
    std::vector<std::string> getSignalNames() const;
    
    // Get signal values
    std::vector<float> getSignalValues() const;
    
    // Set signal values
    void setSignalValues(const std::vector<float>& values);
    
    // Get internal signal features
    std::vector<float> getFeatures() const;
    
    // Get internal signal quality
    float getQuality() const override;
    void setQuality(float quality) override;
    
    // Check if valid
    bool isValid() const override;
    
    // Get input type
    SensoryInputType getInputType() const override;
    
    // Get name
    std::string getName() const override;
    
    // Apply to plasticity
    void applyToPlasticity(PlasticityRule& rule) override;
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron) override;
    
    // Check if active
    bool isActive() const override;
    
    // Save internal signals
    void save(std::ostream& out) const override;
    
    // Load internal signals
    void load(std::istream& in) override;
    
    // Update internal signals
    void update(const std::vector<float>& newSignals);
    
    // Detect internal signal changes
    std::vector<float> detectChanges(const InternalSignals& previous) const;
    
    // Get internal signal health
    float getHealth() const;
    
    // Set internal signal health
    void setHealth(float health);
    
private:
    size_t signalCount_;
    
    std::vector<float> signalValues_;
    std::vector<float> signalFeatures_;
    
    float health_;
    
    // Calculate signal complexity
    float calculateSignalComplexity() const;
    
    // Extract signal features
    std::vector<float> extractFeatures() const;
}; 
```

## Motor Layer

### Action

Behavioral output representation system.

#### Class Interface
```cpp
class Action {
public:
    // Constructor
    Action(const std::string& type = "default",
           std::vector<float> parameters = {});
    
    // Get action type
    const std::string& getType() const;
    void setType(const std::string& type);
    
    // Get action parameters
    const std::vector<float>& getParameters() const;
    void setParameters(const std::vector<float>& parameters);
    
    // Get action parameter by index
    float getParameter(size_t index) const;
    void setParameter(size_t index, float value);
    
    // Get action parameter by name
    float getParameter(const std::string& name) const;
    void setParameter(const std::string& name, float value);
    
    // Get action magnitude
    float getMagnitude() const;
    
    // Get action direction
    std::vector<float> getDirection() const;
    
    // Get action strength
    float getStrength() const;
    
    // Get action timestamp
    Timestamp getTimestamp() const;
    void setTimestamp(Timestamp timestamp);
    
    // Get action duration
    float getDuration() const;
    void setDuration(float duration);
    
    // Get action success probability
    float getSuccessProbability() const;
    void setSuccessProbability(float probability);
    
    // Get action cost
    float getCost() const;
    void setCost(float cost);
    
    // Get action value
    float getValue() const;
    
    // Get action utility
    float getUtility() const;
    
    // Calculate action expected value
    float calculateExpectedValue(const std::vector<float>& rewardDistribution) const;
    
    // Calculate action entropy
    float calculateEntropy() const;
    
    // Get action statistics
    size_t getExecutionCount() const;
    float getSuccessRate() const;
    float getAverageValue() const;
    
    // Increment execution count
    void incrementExecutionCount();
    
    // Update success rate
    void updateSuccessRate(bool success);
    
    // Update action value
    void updateValue(float newValue);
    
    // Reset action
    void reset();
    
    // Save action
    void save(std::ostream& out) const;
    
    // Load action
    void load(std::istream& in);
    
    // Get action type
    ActionType getActionType() const;
    
    // Get action name
    std::string getName() const;
    
    // Apply to plasticity
    void applyToPlasticity(PlasticityRule& rule);
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron);
    
    // Check if active
    bool isActive() const;
    
private:
    std::string type_;
    std::vector<float> parameters_;
    Timestamp timestamp_;
    float duration_;
    float successProbability_;
    float cost_;
    
    size_t executionCount_;
    float successCount_;
    float accumulatedValue_;
    
    // Calculate action magnitude
    float calculateMagnitude() const;
    
    // Calculate action direction
    std::vector<float> calculateDirection() const;
    
    // Calculate action strength
    float calculateStrength() const;
    
    // Calculate action utility
    float calculateUtility() const;
}; 
```

### MotorSystem

Neural-to-action conversion system.

#### Class Interface
```cpp
class MotorSystem {
public:
    // Constructor
    MotorSystem(size_t actionCapacity = 100,
                float actionSelectionTemperature = 1.0f,
                float actionExplorationRate = 0.1f);
    
    // Update motor system
    void update(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Add action to consideration
    void addAction(const Action& action,
                   const std::vector<float>& neuralActivation,
                   float estimatedValue);
    
    // Get selected action
    std::optional<Action> getSelectedAction() const;
    
    // Get action by index
    std::optional<Action> getAction(size_t index) const;
    
    // Get action count
    size_t getActionCount() const;
    
    // Get action selection statistics
    size_t getTotalActionsSelected() const;
    float getActionSelectionEntropy() const;
    
    // Get motor system statistics
    float getAverageActionValue() const;
    float getActionSelectionSpeed() const;
    
    // Remove action from consideration
    void removeAction(size_t index);
    
    // Clear all actions
    void clearActions();
    
    // Get motor system type
    NeuromodulatorType getType() const;
    
    // Get name
    std::string getName() const;
    
    // Apply to plasticity
    void applyToPlasticity(PlasticityRule& rule);
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron);
    
    // Check if active
    bool isActive() const;
    
    // Save motor system
    void save(std::ostream& out) const;
    
    // Load motor system
    void load(std::istream& in);
    
    // Apply development changes
    void applyDevelopmentChanges(const DevelopmentSystem& dev);
    
private:
    size_t actionCapacity_;
    float actionSelectionTemperature_;
    float actionExplorationRate_;
    
    std::vector<Action> actionCandidates_;
    std::vector<float> neuralActivations_;
    std::vector<float> estimatedValues_;
    
    size_t totalActionsSelected_;
    float actionSelectionEntropy_;
    
    // Calculate action probability
    float calculateActionProbability(size_t index) const;
    
    // Select action using softmax
    size_t selectActionUsingSoftmax() const;
    
    // Select action using epsilon-greedy
    size_t selectActionUsingEpsilonGreedy() const;
    
    // Update action values
    void updateActionValues(const std::vector<float>& rewards);
    
    // Calculate action selection entropy
    float calculateActionSelectionEntropy() const;
}; 
```

## Environment Layer

### Environment

Abstract environment interface.

#### Class Interface
```cpp
class Environment {
public:
    // Constructor
    Environment(const std::string& name = "default");
    
    // Get environment name
    const std::string& getName() const;
    
    // Get environment type
    EnvironmentType getType() const;
    
    // Reset environment
    virtual void reset() = 0;
    
    // Step environment
    virtual void step() = 0;
    
    // Get observation
    virtual Observation getObservation() const = 0;
    
    // Apply action
    virtual void applyAction(const Action& action) = 0;
    
    // Get reward
    virtual float getReward() const = 0;
    
    // Check if episode is done
    virtual bool isEpisodeDone() const = 0;
    
    // Get episode length
    virtual size_t getEpisodeLength() const;
    
    // Set episode length
    virtual void setEpisodeLength(size_t length);
    
    // Get environment statistics
    virtual float getEfficiency() const;
    virtual float getDiversity() const;
    virtual float getComplexity() const;
    
    // Get environment parameters
    virtual std::vector<float> getParameters() const;
    virtual void setParameters(const std::vector<float>& parameters);
    
    // Check if environment is terminated
    bool isTerminated() const;
    
    // Terminate environment
    void terminate();
    
    // Resume environment
    void resume();
    
    // Get environment status
    std::string getStatus() const;
    
    // Save environment
    virtual void save(std::ostream& out) const;
    
    // Load environment
    virtual void load(std::istream& in);
    
    // Get environment type
    virtual EnvironmentType getEnvironmentType() const = 0;
    
    // Get environment name
    virtual std::string getEnvironmentName() const = 0;
    
    virtual ~Environment() = default;
};
```

### Observation

Sensory observation wrapper.

#### Class Interface
```cpp
class Observation {
public:
    // Constructor
    Observation(const std::string& source = "sensor");
    
    // Add sensory input
    void addSensoryInput(std::unique_ptr<SensoryInput> input);
    
    // Get sensory input by type
    template<typename T>
    std::unique_ptr<T> getSensoryInput() const;
    
    // Get sensory input by index
    std::unique_ptr<SensoryInput> getSensoryInput(size_t index) const;
    
    // Get sensory input count
    size_t getSensoryInputCount() const;
    
    // Get observation source
    const std::string& getSource() const;
    
    // Get observation timestamp
    Timestamp getTimestamp() const;
    
    // Set observation timestamp
    void setTimestamp(Timestamp timestamp);
    
    // Get observation features
    std::vector<float> getFeatures() const;
    
    // Get observation quality
    float getQuality() const;
    
    // Set observation quality
    void setQuality(float quality);
    
    // Check if observation is complete
    bool isComplete() const;
    
    // Get observation statistics
    size_t getTotalSensoryInputs() const;
    float getAverageQuality() const;
    
    // Reset observation
    void reset();
    
    // Save observation
    void save(std::ostream& out) const;
    
    // Load observation
    void load(std::istream& in);
    
    // Apply to plasticity
    void applyToPlasticity(PlasticityRule& rule);
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron);
    
    // Check if active
    bool isActive() const;
    
private:
    std::string source_;
    Timestamp timestamp_;
    float quality_;
    
    std::vector<std::unique_ptr<SensoryInput>> sensoryInputs_;
}; 
```

## Experiments Layer

### Experiment

Single experiment recording system.

#### Class Interface
```cpp
class Experiment {
public:
    // Constructor
    Experiment(const std::string& name = "experiment");
    
    // Get experiment name
    const std::string& getName() const;
    
    // Get experiment type
    ExperimentType getType() const;
    
    // Get experiment description
    const std::string& getDescription() const;
    
    // Add observation
    void addObservation(const Observation& observation);
    
    // Add action
    void addAction(const Action& action);
    
    // Add reward
    void addReward(float reward);
    
    // Get observations
    const std::vector<Observation>& getObservations() const;
    
    // Get actions
    const std::vector<Action>& getActions() const;
    
    // Get rewards
    const std::vector<float>& getRewards() const;
    
    // Get experiment duration
    float getDuration() const;
    
    // Get experiment statistics
    size_t getObservationCount() const;
    size_t getActionCount() const;
    size_t getRewardCount() const;
    
    // Get experiment summary
    std::string getSummary() const;
    
    // Reset experiment
    void reset();
    
    // Save experiment
    void save(std::ostream& out) const;
    
    // Load experiment
    void load(std::istream& in);
    
    // Get experiment type
    virtual ExperimentType getExperimentType() const = 0;
    
    // Get experiment name
    virtual std::string getExperimentName() const = 0;
    
    virtual ~Experiment() = default;
};
```

### ExperimentRunner

Experiment execution system.

#### Class Interface
```cpp
class ExperimentRunner {
public:
    // Constructor
    ExperimentRunner(std::unique_ptr<Environment> environment,
                     std::unique_ptr<AgentBrain> agentBrain,
                     std::unique_ptr<Brain> brain);
    
    // Run experiment
    Experiment run(size_t steps, const Config& config);
    
    // Run experiment with specific configuration
    Experiment run(const Config& config, size_t steps);
    
    // Run experiment and collect metrics
    ExperimentResult runWithMetrics(const Config& config, size_t steps);
    
    // Get environment
    Environment* getEnvironment() const;
    
    // Get agent brain
    AgentBrain* getAgentBrain() const;
    
    // Get brain
    Brain* getBrain() const;
    
    // Get experiment runner type
    ExperimentRunnerType getType() const;
    
    // Get name
    std::string getName() const;
    
    // Apply to plasticity
    void applyToPlasticity(PlasticityRule& rule);
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron);
    
    // Check if active
    bool isActive() const;
    
    // Save experiment runner
    void save(std::ostream& out) const;
    
    // Load experiment runner
    void load(std::istream& in);
    
    // Apply development changes
    void applyDevelopmentChanges(const DevelopmentSystem& dev);
    
private:
    std::unique_ptr<Environment> environment_;
    std::unique_ptr<AgentBrain> agentBrain_;
    std::unique_ptr<Brain> brain_;
    
    mutable Experiment currentExperiment_;
    ExperimentResult currentResult_;
    
    // Run experiment loop
    void runExperimentLoop(size_t steps, const Config& config);
    
    // Process observation
    void processObservation(const Observation& observation);
    
    // Process action
    void processAction(const Action& action);
    
    // Process reward
    void processReward(float reward);
    
    // Calculate experiment metrics
    ExperimentResult calculateMetrics() const;
}; 
```

### Metrics

Data analysis and metrics collection system.

#### Class Interface
```cpp
class Metrics {
public:
    // Constructor
    Metrics();
    
    // Add metric
    void addMetric(const std::string& name, float value);
    
    // Get metric value
    float getMetric(const std::string& name) const;
    
    // Get all metric names
    std::vector<std::string> getMetricNames() const;
    
    // Get all metric values
    std::vector<float> getMetricValues() const;
    
    // Get metric statistics
    float getMetricMean(const std::string& name) const;
    float getMetricStdDev(const std::string& name) const;
    float getMetricMin(const std::string& name) const;
    float getMetricMax(const std::string& name) const;
    
    // Get average metric value
    float getAverageMetric() const;
    
    // Get metric count
    size_t getMetricCount() const;
    
    // Reset metrics
    void reset();
    
    // Save metrics
    void save(std::ostream& out) const;
    
    // Load metrics
    void load(std::istream& in);
    
    // Get metrics type
    MetricsType getType() const;
    
    // Get name
    std::string getName() const;
    
    // Apply to plasticity
    void applyToPlasticity(PlasticityRule& rule);
    
    // Apply to neuron
    void applyToNeuron(Neuron& neuron);
    
    // Check if active
    bool isActive() const;
    
    // Generate metrics report
    std::string generateReport() const;
    
    // Calculate metric correlation
    std::unordered_map<std::string, float> calculateCorrelations() const;
    
    // Get metrics with thresholds
    std::vector<std::string> getMetricsAboveThreshold(float threshold) const;
    
private:
    std::unordered_map<std::string, std::vector<float>> metricData_;
    
    // Calculate metric statistics
    float calculateMean(const std::vector<float>& values) const;
    float calculateStdDev(const std::vector<float>& values, float mean) const;
    
    // Calculate metric correlation
    float calculateCorrelation(const std::vector<float>& values1,
                               const std::vector<float>& values2) const;
}; 
```

This comprehensive API reference provides detailed documentation for all major NLM components, including class interfaces, method signatures, parameter descriptions, and implementation details. The documentation follows standard C++ conventions and includes examples where helpful for understanding usage patterns.