// Memory system - handles all memory-related functionality
class MemorySystem {
public:
    MemorySystem() {
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    }
    
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory.get(); }
    
private:
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
};

// Prediction system - handles predictive processing
class PredictionSystem {
public:
    PredictionSystem() {
        predictionSystem = std::make_unique<PredictionSystem>();
    }
    
    PredictionSystem* getPredictionSystem() { return predictionSystem.get(); }
    
private:
    std::unique_ptr<PredictionSystem> predictionSystem;
};

// Cognition system - handles higher-order cognitive functions
class CognitionSystem {
public:
    CognitionSystem() {
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
    }
    
    NeuralPlanner* getPlanner() { return planner.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation.get(); }
    AttentionalSelection* getAttention() { return attention.get(); }
    
private:
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
};

// Neuromodulation system - handles neuromodulators and motivation
class NeuromodulationSystem {
public:
    NeuromodulationSystem() {
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
    }
    
    Dopamine* getDopamine() { return dopamine.get(); }
    Curiosity* getCuriosity() { return curiosity.get(); }
    PredictionError* getPredictionError() { return predictionError.get(); }
    Novelty* getNovelty() { return novelty.get(); }
    
private:
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
};

// Plasticity system - handles synaptic plasticity rules
class PlasticitySystem {
public:
    PlasticitySystem() {
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
    }
    
    SpikeSystem* getSpikeSystem() { return spikeSystem.get(); }
    STDP* getSTDP() { return stdp.get(); }
    Hebbian* getHebbian() { return hebbian.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity.get(); }
    
    // Configure plasticity parameters
    void configurePlasticity(const Config& config) {
        float ltpWeight = config.getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config.getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config.getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        float synaptogenesisRate = config.getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config.getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
    }
    
private:
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
};

// Development system - handles developmental processes
class DevelopmentSystem {
public:
    DevelopmentSystem() {
        developmentSystem = std::make_unique<DevelopmentSystem>();
    }
    
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem.get(); }
    
private:
    std::unique_ptr<DevelopmentSystem> developmentSystem;
};

// Core simulation - handles simulation state and basic functionality
class SimulationCore {
public:
    SimulationCore(std::shared_ptr<Config> cfg, const TimestepDuration& timestep = 0.001)
        : config(cfg), timestep(timestep), currentStep(0), currentTime(0.0),
          totalSpikesThisStep(0), totalSpikesTotal(0), isResting(false),
          stepsSinceLastEpisode(0), replayInterval(100), consolidationInterval(1000) {
        
        uint64_t seed = 42;
        if (auto seedOpt = config->get<int64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
    }
    
    std::shared_ptr<Config> getConfig() const { return config; }
    RandomGenerator* getRandomGenerator() { return rng.get(); }
    
    void updateStep(SimulationStep step, Timestamp time) {
        currentStep = step;
        currentTime = time;
    }
    
    void incrementTotalSpikes(size_t count) {
        totalSpikesThisStep += count;
        totalSpikesTotal += count;
    }
    
    void resetStepStats() {
        totalSpikesThisStep = 0;
    }
    
    size_t getTotalSpikesThisStep() const { return totalSpikesThisStep; }
    size_t getTotalSpikesTotal() const { return totalSpikesTotal; }
    
    SimulationStep getCurrentStep() const { return currentStep; }
    Timestamp getCurrentTime() const { return currentTime; }
    TimestepDuration getTimestep() const { return timestep; }
    
    bool isRestingState() const { return isResting; }
    void setRestingState(bool resting) { isResting = resting; }
    
    size_t getStepsSinceLastEpisode() const { return stepsSinceLastEpisode; }
    void incrementStepsSinceLastEpisode() { stepsSinceLastEpisode++; }
    void resetStepsSinceLastEpisode() { stepsSinceLastEpisode = 0; }
    
    size_t getReplayInterval() const { return replayInterval; }
    size_t getConsolidationInterval() const { return consolidationInterval; }
    
    void configureFromConfig(const Config& config) {
        timestep = config.getOr<double>("simulation_timestep", 0.001);
        replayInterval = static_cast<size_t>(config.getOr<int64_t>("replay_interval", 100));
        consolidationInterval = static_cast<size_t>(config.getOr<int64_t>("consolidation_interval", 1000));
    }
    
private:
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
};

struct Brain::Impl {
    // Core simulation
    std::unique_ptr<SimulationCore> simulationCore;
    
    // System components
    std::unique_ptr<MemorySystem> memorySystem;
    std::unique_ptr<PredictionSystem> predictionSystem;
    std::unique_ptr<CognitionSystem> cognitionSystem;
    std::unique_ptr<NeuromodulationSystem> neuromodulationSystem;
    std::unique_ptr<PlasticitySystem> plasticitySystem;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // Neural structures
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    std::unordered_map<uint64_t, Neuron*> neuronById;
    bool spikeHandlersRegistered;
    
    // Phase 5 learned preference trait
    float learnedPreference = 0.0f;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    Impl(std::shared_ptr<Config> cfg)
        : simulationCore(std::make_unique<SimulationCore>(cfg))
        , memorySystem(std::make_unique<MemorySystem>())
        , predictionSystem(std::make_unique<PredictionSystem>())
        , cognitionSystem(std::make_unique<CognitionSystem>())
        , neuromodulationSystem(std::make_unique<NeuromodulationSystem>())
        , plasticitySystem(std::make_unique<PlasticitySystem>())
        , developmentSystem(std::make_unique<DevelopmentSystem>())
        , sensoryNeurons(), motorNeurons(), neuronById(), spikeHandlersRegistered(false)
        , learnedPreference(0.0f)
    {
        try {
            plasticitySystem->configurePlasticity(*cfg);
            simulationCore->configureFromConfig(*cfg);
            checkpointManager = std::make_unique<CheckpointManager>();
        } catch (const std::exception&) {
            throw;
        }
    }
};