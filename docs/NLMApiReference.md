# NLM API Documentation
# Neural Learning Machine C++ API Reference

**Version**: 0.1.0
**Date**: October 10, 2026

## Overview

This document provides comprehensive API documentation for the NLM (Neural Learning Machine) C++ library. NLM is an experimental artificial developmental brain system that implements real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

## Namespace Hierarchy

The NLM API is organized into the following namespaces:

- `nlm` - Main NLM namespace
  - `nlm::brain` - Brain components
  - `nlm::memory` - Memory systems
  - `nlm::plasticity` - Plasticity rules
  - `nlm::development` - Developmental systems
  - `nlm::neuromodulation` - Neuromodulators
  - `nlm::prediction` - Prediction systems
  - `nlm::cognition` - Cognitive systems
  - `nlm::sensory` - Sensory processing
  - `nlm::motor` - Motor systems
  - `nlm::environment` - Environment interfaces
  - `nlm::world` - World simulation
  - `nlm::experiments` - Experiment framework
  - `nlm::performance` - Performance infrastructure
  - `nlm::visualization` - Visualization
  - `nlm::cli` - Command-line interface

## Core Classes

### nlm::Config
Configuration management system for NLM parameters.

```cpp
class Config {
public:
    Config();
    ~Config();
    
    // Loading and saving
    bool loadFromFile(const std::string& filepath);
    void saveToFile(const std::string& filepath) const;
    void loadFromArgs(int argc, char** argv);
    
    // Value setting and getting
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Default);
    
    template<typename T>
    bool get(const std::string& key, T& value) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Configuration keys
    static const std::string RANDOM_SEED;
    static const std::string SIMULATION_TIMESTEP;
    static const std::string NEURON_COUNT;
    static const std::string REGION_COUNT;
    static const std::string CONNECTION_PROBABILITY;
    
    // ... other static keys
};
```

### nlm::Logger
Logging system for NLM.

```cpp
class Logger {
public:
    enum class Level {
        Off = 0,
        Error = 1,
        Warn = 2,
        Info = 3,
        Debug = 4,
        Trace = 5
    };
    
    static Logger& getInstance();
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    void setLevel(Level level);
    Level getLevel() const;
    
    void addLogger(std::shared_ptr<Logger> logger);
    void log(Level level, const std::string& message);
    
    static std::string levelToString(Level level);
    static Level stringToLevel(const std::string& levelStr);
};
```

### nlm::RandomGenerator
Random number generation for simulations.

```cpp
class RandomGenerator {
public:
    RandomGenerator();
    RandomGenerator(uint64_t seed);
    
    uint64_t getSeed() const;
    void setSeed(uint64_t seed);
    
    // Random number generation
    float uniformReal(float min = 0.0f, float max = 1.0f);
    double uniformReal(double min = 0.0, double max = 1.0);
    int64_t uniformInt(int64_t min = 0, int64_t max = 100);
    
    float gaussian(float mean = 0.0f, float stddev = 1.0f);
    double gaussian(double mean = 0.0, double stddev = 1.0);
    
    // Distribution sampling
    float exponential(float lambda = 1.0f);
    float gamma(float shape, float scale);
    
    // Random state management
    void randomize();
    void seed(uint64_t seed);
    
private:
    std::mt19937 generator_;
    std::uniform_real_distribution<float> uniformDist_;
    std::normal_distribution<float> normalDist_;
};
```

### nlm::SimulationClock
Manages simulation time.

```cpp
class SimulationClock {
public:
    SimulationClock(double timestep = 0.001);
    
    void step();
    void setTime(double time);
    double getTime() const;
    SimulationStep getStep() const;
    double getTimestep() const;
    
    Timestamp currentTime() const;
    SimulationStep currentStep() const;
    
private:
    double timestep_;
    double currentTime_;
    SimulationStep currentStep_;
};
```

## Brain Components

### nlm::Brain
Main brain class coordinating all neural systems.

```cpp
class Brain {
public:
    // Constructors and destructors
    explicit Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialization and control
    bool initialize();
    void step(SimulationStep currentStep, Timestamp currentTime);
    void step(SimulationStep currentStep);  // Convenience overload
    void reset();
    
    // Input and output
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    std::unique_ptr<Action> produceAction();
    
    // System access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction and cognition
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Development
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    void develop();
    
    // Neuromodulation
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Statistics and information
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Persistence
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Inter-region connections
    void addInterRegionConnection(RegionId source, RegionId target,
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Random number generator access
    RandomGenerator* getRandomGenerator();
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const;
    
    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};
```

### nlm::Neuron
Leaky Integrate-and-Fire (LIF) neuron model.

```cpp
class Neuron {
public:
    // Neuron properties
    NeuronId getId() const;
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // State management
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // Membrane dynamics
    void stepLIF(Timestamp currentTime, double timestep);
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    
    // Threshold and reset
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    MembranePotential getResetPotential() const;
    void setResetPotential(MembranePotential reset);
    
    // Leak and time constants
    float getLeakConductance() const;
    void setLeakConductance(float conductance);
    double getMembraneTimeConstant() const;
    void setMembraneTimeConstant(double tau);
    
    // Refractory period
    bool isRefractory() const;
    float getRefractoryRemaining() const;
    void setRefractoryPeriod(float period);
    void startRefractory();
    
    // Input and output
    void injectCurrent(MembranePotential current);
    void receiveExcitatoryInput(MembranePotential input);
    void receiveInhibitoryInput(MembranePotential input);
    
    // Spike management
    bool isFiring() const;
    Timestamp getLastSpikeTime() const;
    void setLastSpikeTime(Timestamp time);
    
    // Connectivity
    void addSynapse(std::unique_ptr<Synapse> synapse);
    const std::vector<Synapse*>& getSynapses() const;
    void addInputConnection(Neuron* presynaptic);
    void addOutputConnection(Neuron* postsynaptic);
    
    // Activity measures
    float getActivity() const;
    
private:
    NeuronId id_;
    NeuronType type_;
    NeuronState state_;
    std::vector<Synapse*> inputConnections_;
    std::vector<Synapse*> outputConnections_;
};
```

### nlm::Synapse
Neural connection with plasticity properties.

```cpp
class Synapse {
public:
    // Properties
    SynapseId getId() const;
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    float getWeight() const;
    void setWeight(float weight);
    
    // Timing
    Delay getDelay() const;
    void setDelay(Delay delay);
    
    // Type and polarity
    SynapseType getType() const;
    void setType(SynapseType type);
    bool isExcitatory() const;
    bool isInhibitory() const;
    
    // Plasticity
    PlasticityFlags getPlasticityFlags() const;
    void enablePlasticity(bool stdp, bool hebbian, bool structural);
    void addToWeight(float delta);
    
    // STDP
    void recordPreSpike(Timestamp time);
    void recordPostSpike(Timestamp time);
    std::vector<Timestamp> getPreSpikeHistory() const;
    std::vector<Timestamp> getPostSpikeHistory() const;
    
    // Eligibility traces for neuromodulation
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float factor);
    
    // State management
    void step(Timestamp currentTime);
    
private:
    SynapseId id_;
    NeuronId sourceNeuron_;
    NeuronId destinationNeuron_;
    float weight_;
    Delay delay_;
    SynapseType type_;
    PlasticityFlags plasticityFlags_;
    std::vector<Timestamp> preSpikeHistory_;
    std::vector<Timestamp> postSpikeHistory_;
    float eligibilityTrace_;
};
```

### nlm::NeuralRegion
Collection of neurons with shared properties.

```cpp
class NeuralRegion {
public:
    // Properties
    RegionId getId() const;
    std::string getName() const;
    void setName(const std::string& name);
    
    // Neuron management
    PopulationId addPopulation(size_t neuronCount, NeuronType type);
    void removePopulation(PopulationId id);
    NeuralPopulation* getPopulation(PopulationId id);
    const NeuralPopulation* getPopulation(PopulationId id) const;
    
    // Neuron access
    const std::vector<Neuron*>& getAllNeurons() const;
    Neuron* getNeuron(NeuronId id);
    const Neuron* getNeuron(NeuronId id) const;
    
    // Statistics
    size_t getPopulationCount() const;
    size_t getTotalNeuronCount() const;
    size_t getSynapseCount() const;
    float getAverageSynapticWeight() const;
    float getAverageFiringRate() const;
    size_t getActiveNeuronCount() const;
    
    // Connectivity
    void connectNeurons(NeuronId source, NeuronId destination, float weight = 1.0f, Delay delay = 1);
    void removeConnection(NeuronId source, NeuronId destination);
    std::vector<Synapse*> getSynapsesFrom(NeuronId source) const;
    std::vector<Synapse*> getSynapsesTo(NeuronId destination) const;
    
    // Random connectivity
    void initializeRandomConnectivity(RandomGenerator& rng, float connectionProb, float weightMean = 1.0f, float weightStddev = 0.2f);
    
    // State management
    void reset();
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
private:
    RegionId id_;
    std::string name_;
    std::vector<std::unique_ptr<NeuralPopulation>> populations_;
    std::vector<Neuron*> allNeurons_;
    std::vector<std::unique_ptr<Synapse>> synapses_;
};
```

## Memory Systems

### nlm::NeuralWorkingMemory
Transient working memory for active information.

```cpp
class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();
    
    // Initialization
    void initialize(Brain* brain);
    void setCapacity(size_t capacity);
    
    // Memory operations
    void store(const std::vector<float>& pattern, float strength);
    void storeToNeuron(NeuronId neuron, float activation);
    void update(double timestep);
    
    // Retrieval
    std::vector<float> retrieve() const;
    bool contains(NeuronId neuron) const;
    float getNeuronActivation(NeuronId neuron) const;
    std::vector<NeuronId> getMemoryNeurons() const;
    size_t getActiveTraces() const;
    
    // Maintenance
    void createRecurrentConnection(NeuronId source, NeuronId target, float weight);
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};
```

### nlm::NeuralEpisodicMemory
Long-term episodic memory storage.

```cpp
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    // Initialization
    void initialize(Brain* brain);
    void setMaxEpisodes(size_t maxEpisodes);
    
    // Storage
    void storeEpisode(const EpisodicMemoryItem& episode);
    void update();
    
    // Retrieval
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(
        const std::vector<float>& sensoryPattern, size_t maxResults) const;
    std::vector<const EpisodicMemoryItem*> retrieveTemporal(
        SimulationStep startTime, SimulationStep endTime, size_t maxResults) const;
    
    // Replay and consolidation
    void replayEpisode(const EpisodicMemoryItem* episode);
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    void consolidate(float threshold);
    
    // Statistics
    size_t getEpisodeCount() const;
    size_t getReplayCount() const;
    size_t getConsolidationCount() const;
    
private:
    struct Impl;
    Impl* pImpl;
};
```

## Plasticity Systems

### nlm::STDP
Spike-Timing-Dependent Plasticity.

```cpp
class STDP {
public:
    STDP();
    ~STDP();
    
    // Configuration
    void configure(float ltpWeight, float ltdWeight, float tau);
    void setLTPWeight(float weight);
    void setLTDWeight(float weight);
    void setTau(float tau);
    
    // Plasticity update
    void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes, Timestamp timestep);
    
    // Statistics
    float getTotalLTP() const;
    float getTotalLTD() const;
    
private:
    float ltpWeight_;
    float ltdWeight_;
    float tau_;
    float totalLTP_;
    float totalLTD_;
};
```

### nlm::Hebbian
Hebbian learning (fire together, wire together).

```cpp
class Hebbian {
public:
    Hebbian();
    ~Hebbian();
    
    // Configuration
    void configure(float learningRate, float maxWeightChange);
    
    // Plasticity update
    void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes, Timestamp timestep);
    
private:
    float learningRate_;
    float maxWeightChange_;
};
```

### nlm::StructuralPlasticity
Structural plasticity for synapse formation and elimination.

```cpp
class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Configuration
    void setSynaptogenesisRate(float rate);
    void setPruningRate(float rate);
    
    // Update
    void update(Brain* brain, RandomGenerator& rng);
    
    // Statistics
    size_t getSynaptogenesisCount() const;
    size_t getPruningCount() const;
    
private:
    float synaptogenesisRate_;
    float pruningRate_;
    size_t synaptogenesisCount_;
    size_t pruningCount_;
};
```

## Neuromodulation Systems

### nlm::Dopamine
Dopamine neuromodulator for reward and reinforcement.

```cpp
class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    // Initialization
    void initialize(Brain* brain);
    
    // Level management
    void setLevel(float level);
    float getLevel() const;
    float getPlasticityFactor() const;
    
    // Update
    void update(double timestep);
    
private:
    float level_;
    float plasticityFactor_;
};
```

### nlm::Curiosity
Curiosity neuromodulator for exploration motivation.

```cpp
class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Initialization
    void initialize(Brain* brain);
    
    // Level management
    void setLevel(float level);
    float getLevel() const;
    
    // Update
    void update(double timestep);
    
private:
    float level_;
};
```

### nlm::Novelty
Novelty detection neuromodulator.

```cpp
class Novelty {
public:
    Novelty();
    ~Novelty();
    
    // Initialization
    void initialize(Brain* brain);
    
    // Detection
    void update(double timestep);
    float getLevel() const;
    
private:
    float level_;
    std::vector<float> previousPattern_;
};
```

## Prediction System

### nlm::PredictionSystem
Sensory prediction and error computation.

```cpp
class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialization
    void initialize(Brain* brain);
    
    // Prediction
    std::vector<float> predict(const std::vector<float>& sensoryInput);
    float getPredictionError() const;
    float getConfidence() const;
    
    // Training
    void train(const std::vector<float>& sensoryInput, float reward);
    
    // Statistics
    size_t getTrainingSamples() const;
    float getAccuracy() const;
    
private:
    std::vector<float> predictionModel_;
    float predictionError_;
    float confidence_;
    size_t trainingSamples_;
    float accuracy_;
};
```

## Cognitive Systems

### nlm::NeuralPlanner
Action planning and sequencing.

```cpp
class NeuralPlanner {
public:
    NeuralPlanner();
    ~NeuralPlanner();
    
    // Initialization
    void initialize(Brain* brain);
    void setPlanningDepth(int depth);
    
    // Planning
    std::vector<Action> planActions(const std::vector<float>& context);
    float evaluateActionSequence(const std::vector<Action>& sequence);
    
    // Statistics
    size_t getPlanCount() const;
    float getPlanningTime() const;
    
private:
    int planningDepth_;
    size_t planCount_;
    float planningTime_;
};
```

### nlm::ConceptFormation
Concept discovery from experience patterns.

```cpp
class ConceptFormation {
public:
    ConceptFormation();
    ~ConceptFormation();
    
    // Initialization
    void initialize(Brain* brain);
    
    // Concept processing
    std::vector<Concept> extractConcepts(const std::vector<float>& neuralActivity);
    float evaluateConceptSimilarity(const Concept& concept1, const Concept& concept2);
    
    // Learning
    void learnFromExperience(const std::vector<float>& experience);
    
    // Statistics
    size_t getConceptCount() const;
    size_t getLearningEvents() const;
    
private:
    std::vector<Concept> concepts_;
    size_t learningEvents_;
};
```

### nlm::AttentionalSelection
Selective attention and competitive selection.

```cpp
class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();
    
    // Initialization
    void initialize(Brain* brain);
    void setInhibitionStrength(float strength);
    void setExcitationStrength(float strength);
    
    // Attention
    void update(double timestep);
    std::vector<NeuronId> selectAttendedNeurons(const std::vector<NeuronId>& competitors);
    float getAttentionLevel(NeuronId neuron) const;
    
    // Competition
    void processCompetition(const std::vector<NeuronId>& neurons);
    float computeCompetitiveness(NeuronId neuron) const;
    
private:
    float inhibitionStrength_;
    float excitationStrength_;
    std::vector<float> attentionLevels_;
};
```

## Development System

### nlm::DevelopmentSystem
Developmental progression and maturation.

```cpp
class DevelopmentSystem {
public:
    DevelopmentSystem();
    ~DevelopmentSystem();
    
    // Initialization
    void initialize(Brain* brain);
    
    // Development
    void update(Brain* brain, RandomGenerator& rng, double timestep);
    void setStage(DevelopmentalStage stage);
    DevelopmentalStage getStage() const;
    
    // Effects
    float getPlasticityModifier() const;
    float getMaturationLevel() const;
    
private:
    DevelopmentalStage currentStage_;
    float maturationLevel_;
    double developmentalTime_;
};
```

## Environment and World Systems

### nlm::SimpleWorld
Simple simulation environment.

```cpp
class SimpleWorld {
public:
    SimpleWorld();
    ~SimpleWorld();
    
    // Environment properties
    void setSize(size_t width, size_t height);
    void setVisionSize(size_t width, size_t height);
    
    // Agent management
    void addAgent(AgentBrain* agent);
    void removeAgent(AgentBrain* agent);
    
    // Observations
    SensoryPercept observe(const AgentBrain& agent) const;
    float computeReward(const AgentBrain& agent) const;
    
    // Actions
    void applyAction(AgentBrain* agent, const MotorCommand& action);
    
    // State
    void update(double timestep);
    void reset();
    
    // Statistics
    size_t getAgentCount() const;
    size_t getObjectCount() const;
    
private:
    size_t width_;
    size_t height_;
    size_t visionWidth_;
    size_t visionHeight_;
    std::vector<AgentBrain*> agents_;
    // ... other world state
};
```

## Experiment Framework

### nlm::Experiment
Base class for NLM experiments.

```cpp
class Experiment {
public:
    Experiment();
    virtual ~Experiment();
    
    // Experiment lifecycle
    virtual void initialize();
    virtual void run();
    virtual void cleanup();
    
    // Configuration
    void setConfig(const ExperimentConfig& config);
    const ExperimentConfig& getConfig() const;
    
    // Metrics
    void recordMetric(const std::string& name, float value);
    float getMetric(const std::string& name) const;
    std::unordered_map<std::string, float> getAllMetrics() const;
    
protected:
    ExperimentConfig config_;
    std::unordered_map<std::string, float> metrics_;
};
```

### nlm::Phase6IntegratedExperiment
Phase 6 integration experiment.

```cpp
class Phase6IntegratedExperiment : public Experiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    // Integration testing
    bool verifyIntegration() const;
    bool testMemoryIntegration() const;
    bool testNeuromodulationIntegration() const;
    bool testCheckpointing() const;
    bool testReplay() const;
    
    // Full experiment
    Phase6IntegrationResult run(const Phase6Config& config);
    
    // Single system tests
    bool testPredictionIntegration() const;
    bool testDevelopmentIntegration() const;
    
private:
    bool initializeBrainForIntegration(std::shared_ptr<Brain>& brain) const;
    bool verifyBrainHasIntegratedSystems(const Brain& brain) const;
    // ... implementation details
};
```

## Performance Infrastructure

### nlm::MemoryPool
Efficient memory allocation for high-performance scenarios.

```cpp
class MemoryPool {
public:
    MemoryPool(size_t blockSize = 1024, size_t initialBlocks = 100);
    ~MemoryPool();
    
    // Allocation
    void* allocate();
    void deallocate(void* ptr);
    
    // Statistics
    size_t getAllocatedBlocks() const;
    size_t getTotalAllocations() const;
    size_t getDeallocations() const;
    
private:
    struct BlockHeader {
        void* next;
        size_t size;
    };
    
    BlockHeader* freeList_;
    size_t blockSize_;
    size_t allocatedBlocks_;
    size_t totalAllocations_;
    size_t totalDeallocations_;
};
```

### nlm::EventQueue
High-performance event queue for simulation.

```cpp
class EventQueue {
public:
    EventQueue(size_t capacity = 100000);
    ~EventQueue();
    
    // Event management
    bool enqueue(const SpikeEvent& event);
    bool dequeue(SpikeEvent& event);
    size_t getSize() const;
    
    // Statistics
    size_t getEnqueueCount() const;
    size_t getDequeueCount() const;
    
private:
    SpikeEvent* events_;
    size_t capacity_;
    size_t head_;
    size_t tail_;
    size_t size_;
    // ... other fields
};
```

## Visualization Interface

### nlm::VisualizationInterface
Base class for visualization systems.

```cpp
class VisualizationInterface {
public:
    VisualizationInterface();
    virtual ~VisualizationInterface();
    
    // Initialization
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    
    // Rendering
    virtual void update(const Brain& brain, double timestep) = 0;
    virtual void render() = 0;
    
    // Configuration
    virtual void setParameter(const std::string& name, float value) = 0;
    virtual float getParameter(const std::string& name) const = 0;
    
    // State
    virtual void saveState(const std::string& filename) = 0;
    virtual void loadState(const std::string& filename) = 0;
    
protected:
    bool initialized_;
    double lastUpdateTime_;
};
```

## CLI API

### nlm::cli::NLMCLI
Command-line interface class.

```cpp
class NLMCLI {
public:
    NLMCLI();
    ~NLMCLI();
    
    // Main entry point
    int run(int argc, char* argv[]);
    
    // Configuration
    struct CLIConfig {
        std::string mode;
        std::string configFile;
        std::string checkpointFile;
        bool verbose;
        bool quiet;
        int steps;
        int neurons;
        std::string outputFile;
        bool enableCheckpointing;
        bool enableReplay;
        bool enableDevelopment;
        std::string logLevel;
        std::string experimentMode;
    };
    
    // Command execution
    int executeDemoMode(const CLIConfig& config);
    int executeExperimentMode(const CLIConfig& config);
    int executeExploreMode(const CLIConfig& config);
    int executeBenchmarkMode(const CLIConfig& config);
    
    // Utility functions
    static void printHelp(const std::string& programName);
    static void printVersion();
    static void printAdvancedHelp();
    
private:
    CLIConfig parseArguments(int argc, char* argv[]);
    // ... other private members
};
```

## Type Definitions and Constants

### Core Types

```cpp
namespace nlm {
    using NeuronId = uint32_t;
    using PopulationId = uint32_t;
    using RegionId = uint32_t;
    using SynapseId = uint32_t;
    using SimulationStep = uint64_t;
    using Timestamp = double;
    using MembranePotential = float;
    using Delay = uint8_t;
    using Weight = float;
    
    enum class NeuronType {
        Sensory,
        Internal,
        Motor,
        Unknown
    };
    
    enum class SynapseType {
        Excitatory,
        Inhibitory,
        Modulatory,
        Unknown
    };
    
    enum class FiringState {
        Resting,
        Active,
        Refractory,
        Adapting
    };
    
    enum class DevelopmentalStage {
        Initial,
        CriticalPeriod,
        Maturation,
        Adult
    };
    
    enum class ActionType {
        MoveForward,
        MoveBackward,
        TurnLeft,
        TurnRight,
        LookLeft,
        LookRight,
        Interact,
        Wait
    };
    
    enum class ConfigSource {
        Default,
        File,
        CommandLine,
        Override
    };
}
```

### Memory Types

```cpp
namespace nlm {
    struct NeuronState {
        MembranePotential membranePotential;
        MembranePotential restingPotential;
        MembranePotential threshold;
        MembranePotential resetPotential;
        float leakConductance;
        FiringState firingState;
        float refractoryRemaining;
        float refractoryPeriod;
        Timestamp lastSpikeTime;
    };
    
    struct SpikeEvent {
        NeuronId neuronId;
        Timestamp time;
        SimulationStep step;
    };
    
    struct DelayedSpikeEvent {
        NeuronId sourceNeuron;
        NeuronId destinationNeuron;
        SynapseId synapseId;
        Weight weight;
        SynapseType type;
        Timestamp scheduledTime;
        Timestamp deliveryTime;
        SimulationStep scheduledStep;
        SimulationStep deliveryStep;
    };
    
    struct InterRegionConnection {
        RegionId sourceRegion;
        RegionId targetRegion;
        Weight weight;
        Delay delay;
        PlasticityFlags plasticityFlags;
    };
    
    struct EpisodicMemoryItem {
        SimulationStep timestamp;
        std::vector<NeuronId> activeNeurons;
        std::vector<float> neuronActivations;
        float reward;
        size_t age;
        std::vector<float> sensoryState;
    };
    
    struct Action {
        ActionType type;
        std::vector<float> parameters;
    };
    
    struct SensoryPercept {
        std::vector<float> vision;
        std::vector<float> touch;
        std::vector<float> internal;
        std::vector<float> proprioception;
    };
    
    struct Concept {
        std::vector<float> pattern;
        float strength;
        std::vector<float> associations;
    };
}
```

## Error Handling

### Error Types

```cpp
namespace nlm {
    enum class ErrorType {
        None,
        InitializationFailed,
        InvalidConfiguration,
        MemoryError,
        FileError,
        SimulationError,
        PlasticityError,
        MemoryError,
        PredictionError,
        CognitiveError,
        DevelopmentError,
        NeuromodulationError,
        VisualizationError,
        Unknown
    };
    
    class NLMException : public std::runtime_error {
    public:
        NLMException(ErrorType type, const std::string& message);
        ErrorType getType() const;
        
    private:
        ErrorType type_;
    };
}
```

### Error Handling Functions

```cpp
// Global error handling
void setErrorHandler(std::function<void(ErrorType, const std::string&)> handler);
ErrorType getLastError();
std::string getErrorMessage();

// Try-catch for NLM operations
try {
    auto brain = std::make_shared<Brain>(config);
    bool success = brain->initialize();
    if (!success) {
        throw NLMException(ErrorType::InitializationFailed, "Failed to initialize brain");
    }
    // ... use brain
} catch (const NLMException& e) {
    // Handle NLM-specific errors
    std::cerr << "NLM Error: " << e.what() << std::endl;
} catch (const std::exception& e) {
    // Handle other exceptions
    std::cerr << "Error: " << e.what() << std::endl;
}
```

## Thread Safety

### Thread Safety Guidelines

- **Brain objects**: Not thread-safe. Use separate Brain instances for different threads.
- **Shared resources**: Use mutexes when accessing shared resources.
- **Configuration**: Config objects are thread-safe for read operations.
- **Logging**: Logger is thread-safe.

### Thread-Safe Functions

```cpp
// These functions can be safely called from multiple threads
const std::shared_ptr<const Config>& Config::getSharedConfig() const;
const std::shared_ptr<const Logger>& Logger::getGlobalLogger();

// These functions require external synchronization
void Brain::step(SimulationStep currentStep, Timestamp currentTime);
void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength);
```

## Memory Management

### Smart Pointer Usage

```cpp
// Recommended usage patterns

// Preferred: Use smart pointers
auto brain = std::make_shared<Brain>(config);
auto memory = std::make_unique<NeuralWorkingMemory>();

// Shared ownership
std::shared_ptr<Brain> brainPtr = brain;
std::unique_ptr<NeuralWorkingMemory> memoryPtr = std::move(memory);

// Raw pointers (use with caution)
Brain* brainPtr = brain.get();
Neuron* neuronPtr = neuron.get();
```

### Resource Management

```cpp
// RAII pattern for resource management
class BrainManager {
public:
    BrainManager(std::shared_ptr<Config> config) {
        brain_ = std::make_shared<Brain>(config);
        if (!brain_->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
    }
    
    ~BrainManager() {
        if (brain_) {
            brain_->save("/tmp/brain_state.bin"); // Save on destruction
        }
    }
    
    Brain& get() { return *brain_; }
    const Brain& get() const { return *brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
};
```

## Performance Considerations

### Optimization Guidelines

1. **Use const references** where possible to avoid copying
2. **Reserve container sizes** when known in advance
3. **Use move semantics** for temporary objects
4. **Minimize dynamic memory allocation** in hot paths
5. **Use caching** for frequently accessed data
6. **Profile before optimizing**

### Profiling

```cpp
// Use the built-in profiler
nlm::Profiler profiler;
profiler.start("brain_step");
brain->step(step, time);
profiler.stop("brain_step");

// Print results
profiler.printResults();
```

## Build Instructions

### CMake Configuration

```bash
# Basic build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# With custom options
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DNLM_ENABLE_CLI=ON \
    -DNLM_ENABLE_VISUALIZATION=ON \
    -DNLM_ENABLE_PROFILING=ON

# Install
make install
```

### Building with Different Compilers

```bash
# GCC/G++
cmake .. -DCMAKE_CXX_COMPILER=g++

# Clang/Clang++
cmake .. -DCMAKE_CXX_COMPILER=clang++

# MSVC
cmake .. -G "Visual Studio 16 2019"
```

## Testing

### Running Tests

```bash
# Run unit tests
./test_runner

# Run integration tests
./integration_tests

# Run performance benchmarks
./benchmarks

# Run all tests
./test_all
```

### Test Framework

```cpp
// Example unit test
TEST(NeuronTest, LIFDynamics) {
    Neuron neuron(NeuronType::Internal);
    neuron.setMembranePotential(0.0f);
    neuron.setThreshold(1.0f);
    
    // Apply input
    neuron.injectCurrent(5.0f);
    
    // Step simulation
    neuron.stepLIF(0.001, 0.001);
    
    // Check results
    EXPECT_GT(neuron.getMembranePotential(), 0.0f);
}

// Example integration test
TEST(IntegrationTest, BrainWithMemory) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(config);
    ASSERT_TRUE(brain->initialize());
    
    // Check memory integration
    ASSERT_NE(brain->getWorkingMemory(), nullptr);
    ASSERT_NE(brain->getEpisodicMemory(), nullptr);
}
```

## Example Programs

### Basic Brain Example

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Config/Config.hpp>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("simulation_timestep", 0.001);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (size_t step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Print results
    brain->logStatus();
    
    return 0;
}
```

### Memory Integration Example

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/memory/NeuralWorkingMemory.hpp>

int main() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        return 1;
    }
    
    // Store some patterns in working memory
    std::vector<float> pattern1 = {0.1f, 0.5f, 0.8f, 0.3f, 0.7f};
    brain->getWorkingMemory()->store(pattern1, 1.0f);
    
    std::vector<float> pattern2 = {0.9f, 0.2f, 0.4f, 0.6f, 0.1f};
    brain->getWorkingMemory()->store(pattern2, 0.8f);
    
    // Retrieve patterns
    std::vector<float> retrieved = brain->getWorkingMemory()->retrieve();
    
    std::cout << "Retrieved " << retrieved.size() << " memory traces" << std::endl;
    
    return 0;
}
```

### CLI Usage Example

```cpp
#include <nlm/cli/NLMCLI.hpp>

int main(int argc, char* argv[]) {
    nlm::cli::NLMCLI cli;
    return cli.run(argc, argv);
}
```

## Migration Guide

### From Phase 2 to Phase 6

1. **Update includes**: Replace `#include "main.cpp"` with `#include "brain/Brain.hpp"`
2. **Change initialization**: Use `Brain::initialize()` instead of `brain->initialize()`
3. **Update step calls**: Use `brain->step(step, time)` with time parameter
4. **Add memory systems**: Access through `brain->getWorkingMemory()` etc.
5. **Enable plasticity**: Call `syn->enablePlasticity(true, true, false)`

### Version Compatibility

- **Version 0.1.0**: Phase 6 integration (current)
- **Version 0.2.0**: Planned features (to be implemented)
- **Version 1.0.0**: Stable API (planned)

## Contributing

### Code Style

- Use C++20 features where appropriate
- Follow existing code style and conventions
- Use descriptive variable and function names
- Add comprehensive comments for complex code
- Include unit tests for new functionality

### Pull Requests

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add tests for new functionality
5. Update documentation
6. Submit a pull request

### Issues and Feature Requests

Report issues and request features through the project issue tracker.

## Acknowledgements

This project builds on decades of research in computational neuroscience and artificial intelligence. Special thanks to:

- Researchers in neural dynamics and plasticity
- Developers of related brain simulation frameworks
- Contributors to open-source neural network libraries

## References

1. Dayan, P., & Abbott, L. F. (2001). Theoretical Neuroscience. MIT Press.
2. Koch, C. (2004). Biophysical Modeling of Subcellular Systems. Harvard University Press.
3. Gerstner, W., Kistler, W., Naud, R., & Paninski, L. (2014). Neuronal Dynamics. Cambridge University Press.
4. Maass, W., Bishop, R., & Platio, R. (2002). Computability of Spiking Neuron Models. Neural Computation.

---

*This API documentation is a work in progress. Contributions and feedback are welcome!*
