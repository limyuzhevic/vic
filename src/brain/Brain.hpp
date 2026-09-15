    // ========== ADDITIONAL INTEGRATION ==========
    
    // Connect working memory to sensory input
    void connectWorkingMemoryToSensory();
    
    // Connect prediction system to agent brain
    void connectPredictionToAgent(class AgentBrain* agent);
    
    // Store prediction error for neuromodulation
    void setPredictionError(float error) { pImpl->predictionError = error; }
    
    // Store prediction error confidence
    void setPredictionErrorConfidence(float confidence) { pImpl->predictionErrorConfidence = confidence; }
    
private:
    struct Impl {
        std::shared_ptr<Config> config;
        std::unique_ptr<RandomGenerator> rng;
        std::vector<std::unique_ptr<NeuralRegion>> regions;
        std::vector<InterRegionConnection> interRegionConnections;
        
        // ========== INTEGRATED MEMORY SYSTEMS ==========
        std::unique_ptr<NeuralWorkingMemory> workingMemory;
        std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
        std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
        
        // ========== INTEGRATED PREDICTION SYSTEM ==========
        std::unique_ptr<PredictionSystem> predictionSystem;
        
        // ========== INTEGRATED COGNITION SYSTEMS ==========
        std::unique_ptr<NeuralPlanner> planner;
        std::unique_ptr<ConceptFormation> conceptFormation;
        std::unique_ptr<AttentionalSelection> attention;
        
        // ========== DEVELOPMENT SYSTEM ==========
        std::unique_ptr<DevelopmentSystem> developmentSystem;
        
        // ========== NEUROMODULATION SYSTEMS ==========
        std::unique_ptr<Dopamine> dopamine;
        std::unique_ptr<Curiosity> curiosity;
        std::unique_ptr<PredictionError> predictionError;
        std::unique_ptr<Novelty> novelty;
        
        // Phase 2: Real neural computation components
        std::unique_ptr<SpikeSystem> spikeSystem;
        std::unique_ptr<STDP> stdp;
        std::unique_ptr<Hebbian> hebbian;
        std::unique_ptr<StructuralPlasticity> structuralPlasticity;
        
        // Simulation parameters
        TimestepDuration timestep;
        SimulationStep currentStep;
        Timestamp currentTime;
        
        // Statistics
        size_t totalSpikesThisStep;
        size_t totalSpikesTotal;
        
        // Sensory neurons for input injection
        std::vector<Neuron*> sensoryNeurons;
        std::vector<Neuron*> motorNeurons;
        
        // Integration state
        bool isResting;  // For sleep/rest cycle
        size_t stepsSinceLastEpisode;
        size_t replayInterval;
        size_t consolidationInterval;
        
        // Checkpoint system
        std::unique_ptr<CheckpointManager> checkpointManager;
        
        // Additional integration state
        float predictionError;
        float predictionErrorConfidence;
        std::vector<float> previousVision_;
        float sensoryNoveltyDecay_;
        
        Impl(std::shared_ptr<Config> cfg)
            : config(cfg)
            , rng(nullptr)
            , developmentalStage(DevelopmentalStage::Initial)
            , nextRegionId(1)
            , timestep(0.001)
            , currentStep(0)
            , currentTime(0.0)
            , totalSpikesThisStep(0)
            , totalSpikesTotal(0)
            , isResting(false)
            , stepsSinceLastEpisode(0)
            , replayInterval(100)      // Replay every 100 steps
            , consolidationInterval(1000)  // Consolidate every 1000 steps
            , predictionError(0.0f)
            , predictionErrorConfidence(0.0f)
            , sensoryNoveltyDecay_(0.99f)
        {
            // Initialize random generator with seed from config
            uint64_t seed = 42;  // Default seed
            if (auto seedOpt = config->get<uint64_t>("random_seed")) {
                seed = *seedOpt;
            }
            rng = std::make_unique<RandomGenerator>(seed);
            
            // Initialize plasticity systems
            spikeSystem = std::make_unique<SpikeSystem>();
            stdp = std::make_unique<STDP>();
            hebbian = std::make_unique<Hebbian>();
            structuralPlasticity = std::make_unique<StructuralPlasticity>();
            
            // ========== INITIALIZE INTEGRATED SYSTEMS ==========
            
            // Initialize memory systems
            workingMemory = std::make_unique<NeuralWorkingMemory>();
            episodicMemory = std::make_unique<NeuralEpisodicMemory>();
            associativeMemory = std::make_unique<NeuralAssociativeMemory>();
            
            // Initialize prediction system
            predictionSystem = std::make_unique<PredictionSystem>();
            
            // Initialize cognition systems
            planner = std::make_unique<NeuralPlanner>();
            conceptFormation = std::make_unique<ConceptFormation>();
            attention = std::make_unique<AttentionalSelection>();
            
            // Initialize development system
            developmentSystem = std::make_unique<DevelopmentSystem>();
            
            // Initialize neuromodulation systems
            dopamine = std::make_unique<Dopamine>();
            curiosity = std::make_unique<Curiosity>();
            predictionError = std::make_unique<PredictionError>();
            novelty = std::make_unique<Novelty>();
            
            // Configure STDP parameters
            float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
            float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
            float tau = config->getOr<float>("stdp_tau", 20.0f);
            stdp->configure(ltpWeight, ltdWeight, tau);
            
            // Configure structural plasticity
            float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
            float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
            structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
            structuralPlasticity->setPruningRate(pruningRate);
            
            // Get timestep
            timestep = config->getOr<double>("simulation_timestep", 0.001);
            
            // Get integration intervals from config
            replayInterval = config->getOr<size_t>("replay_interval", 100);
            consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
            
            // Initialize checkpoint manager
            checkpointManager = std::make_unique<CheckpointManager>();
        }
        
        DevelopmentalStage developmentalStage;
        RegionId nextRegionId;
        
        // Forward declarations
        class DevelopmentalStage;
        class RegionId;
        class NeuronId;
        class SynapseId;
        class PopulationId;
        class NeuronType;
        class SynapseType;
        class ActionType;
        class MotorCommand;
        class WorldObjectType;
        class SimulationStep;
        class Timestamp;
        class TimestepDuration;
        class MembranePotential;
        class Delay;
        class PlasticityFlags;
        class FiringState;
        class DevelopmentalStage;
        class Config;
        class RandomGenerator;
        class SimulationClock;
        class Logger;
        class NeuralRegion;
        class NeuralWorkingMemory;
        class NeuralEpisodicMemory;
        class NeuralAssociativeMemory;
        class PredictionSystem;
        class NeuralPlanner;
        class ConceptFormation;
        class AttentionalSelection;
        class DevelopmentSystem;
        class Dopamine;
        class Curiosity;
        class Novelty;
        class PredictionError;
        class SpikeSystem;
        class STDP;
        class Hebbian;
        class StructuralPlasticity;
        class CheckpointManager;
        class CheckpointWriter;
        class CheckpointReader;
        class NeuronCheckpointData;
        class SynapseCheckpointData;
        class Neuron;
        class Synapse;
        class Population;
        class SpikeEvent;
        class DelayedSpikeEvent;
        class DetailedSpikeEvent;
        class Action;
        class Neuromodulator;
        class SensoryInput;
        class Vision;
        class Audio;
        class InternalSignals;
        class WorldObject;
        class AgentBody;
        class ActionResult;
        class SensoryPercept;
        class SimpleWorld;
        class AgentBrain;
        
        // Initialize all integrated systems
        void initializeIntegratedSystems() {
            if (!workingMemory) return;
            
            // Initialize working memory
            workingMemory->initialize(this);
            workingMemory->setCapacity(100);
            
            // Initialize episodic memory
            episodicMemory->initialize(this);
            episodicMemory->setMaxEpisodes(1000);
            
            // Initialize associative memory
            associativeMemory->initialize(this);
            
            // Initialize prediction system
            predictionSystem->initialize(this);
            
            // Initialize cognition systems
            planner->initialize(this);
            planner->setPlanningDepth(5);
            
            conceptFormation->initialize(this);
            
            attention->initialize(this);
            attention->setInhibitionStrength(0.5f);
            attention->setExcitationStrength(1.5f);
            
            // Initialize neuromodulation
            novelty->initialize(this);
            curiosity->initialize(this);
            
            // Register spike handlers for event-driven processing
            spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
                // Count spikes
                ++totalSpikesThisStep;
                ++totalSpikesTotal;
            });
            
            // Register delayed spike handler to deliver synaptic input
            spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
                // Find destination neuron and deliver synaptic input
                for (auto& region : regions) {
                    auto neurons = region->getAllNeurons();
                    for (auto* neuron : neurons) {
                        if (neuron->getId() == event.destination_neuron) {
                            // Apply synaptic weight as current
                            MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                            if (event.is_excitatory) {
                                neuron->receiveExcitatoryInput(synapticCurrent);
                            } else {
                                neuron->receiveInhibitoryInput(-synapticCurrent);
                            }
                            return;
                        }
                    }
                }
            });
        }
    };
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
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
    
    // ========== ADDITIONAL INTEGRATION ==========
    
    // Connect working memory to sensory input
    void connectWorkingMemoryToSensory();
    
    // Connect prediction system to agent brain
    void connectPredictionToAgent(class AgentBrain* agent);
    
    // Store prediction error for neuromodulation
    void setPredictionError(float error) { pImpl->predictionError = error; }
    
    // Store prediction error confidence
    void setPredictionErrorConfidence(float confidence) { pImpl->predictionErrorConfidence = confidence; }
    
private:
    struct Impl;
    Impl* pImpl;
};
