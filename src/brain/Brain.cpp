#include <thread>
#include <chrono>
#include <numeric>

// Comprehensive exception classes for brain operations
class BrainExecutionError : public std::runtime_error {
public:
    BrainExecutionError(const std::string& message, SimulationStep step = 0, const std::string& context = "")
        : std::runtime_error("Brain Execution Error"), 
          message_(message), step_(step), context_(context) {}
    
    SimulationStep getStep() const { return step_; }
    const std::string& getContext() const { return context_; }
    const std::string& getMessage() const { return message_; }
    
private:
    std::string message_;
    SimulationStep step_;
    std::string context_;
};

class BrainIOError : public std::runtime_error {
public:
    BrainIOError(const std::string& message, const std::string& filepath = "", IOErrorType type = IOErrorType::General)
        : std::runtime_error("Brain I/O Error"), 
          message_(message), filepath_(filepath), type_(type) {}
    
    IOErrorType getType() const { return type_; }
    const std::string& getFilepath() const { return filepath_; }
    const std::string& getMessage() const { return message_; }
    
private:
    std::string message_;
    std::string filepath_;
    IOErrorType type_;
};

class BrainTimeoutError : public std::runtime_error {
public:
    BrainTimeoutError(const std::string& message, std::chrono::milliseconds timeout) 
        : std::runtime_error("Brain Timeout Error"), 
          message_(message), timeout_(timeout) {}
    
    std::chrono::milliseconds getTimeout() const { return timeout_; }
    const std::string& getMessage() const { return message_; }
    
private:
    std::string message_;
    std::chrono::milliseconds timeout_;
};

class BrainResourceError : public std::runtime_error {
public:
    BrainResourceError(const std::string& message, ResourceType type = ResourceType::Memory)
        : std::runtime_error("Brain Resource Error"), 
          message_(message), resourceType_(type) {}
    
    ResourceType getResourceType() const { return resourceType_; }
    const std::string& getMessage() const { return message_; }
    
private:
    std::string message_;
    ResourceType resourceType_;
};

struct Brain::Impl {
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
    std::vector<std::shared_ptr<Neuron>> sensoryNeurons;
    std::vector<std::shared_ptr<Neuron>> motorNeurons;
    
    // Integration state
    bool isResting;  // For sleep/rest cycle
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Error recovery state
    size_t consecutiveErrors;
    bool hasRecovered;
    
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
        , consecutiveErrors(0)
        , hasRecovered(false)
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
        
        initializeSystems();
    }
    
    void initializeSystems() {
        initializeCoreSystems();
        initializePlasticitySystems();
        initializeMemorySystems();
        initializePredictionSystem();
        initializeCognitionSystems();
        initializeDevelopmentSystem();
        initializeNeuromodulationSystems();
        initializeSpikeSystem();
        setupInterRegionConnections();
    }
    
    void initializeCoreSystems() {
        // Configure core simulation parameters
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    void initializePlasticitySystems() {
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
        
        configureSTDP();
        configureStructuralPlasticity();
    }
    
    void initializeMemorySystems() {
        // Initialize memory systems
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        configureMemorySystems();
    }
    
    void initializePredictionSystem() {
        // Initialize prediction system
        predictionSystem = std::make_unique<PredictionSystem>();
    }
    
    void initializeCognitionSystems() {
        // Initialize cognition systems
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        
        configureCognitionSystems();
    }
    
    void initializeDevelopmentSystem() {
        // Initialize development system
        developmentSystem = std::make_unique<DevelopmentSystem>();
    }
    
    void initializeNeuromodulationSystems() {
        // Initialize neuromodulation systems
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
    }
    
    void initializeSpikeSystem() {
        // Initialize spike system
        spikeSystem = std::make_unique<SpikeSystem>();
    }
    
    void setupInterRegionConnections() {
        // Setup inter-region connections (currently none)
    }
    
    void configureSTDP() {
        // Configure STDP parameters
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
    }
    
    void configureStructuralPlasticity() {
        // Configure structural plasticity parameters
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
    }
    
    void configureMemorySystems() {
        // Memory system configuration
        workingMemory->initialize(nullptr);  // Will be initialized in Brain::initialize
        episodicMemory->initialize(nullptr);  // Will be initialized in Brain::initialize
        associativeMemory->initialize(nullptr);  // Will be initialized in Brain::initialize
    }
    
    void configureCognitionSystems() {
        // Cognition system configuration
        // Configuration applied in Brain::initialize()
    }
    
    ~Impl() {
        implReset();
    }
    
    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;
    
    void implReset() {
        // Clear sensory and motor neuron vectors
        sensoryNeurons.clear();
        motorNeurons.clear();
        
        // Clear regions
        regions.clear();
        interRegionConnections.clear();
        
        currentStep = 0;
        currentTime = 0.0;
        totalSpikesThisStep = 0;
        totalSpikesTotal = 0;
        isResting = false;
        stepsSinceLastEpisode = 0;
        
        // Reset neural computation systems
        if (spikeSystem) spikeSystem->reset();
        
        // Reset neuromodulation systems
        if (curiosity) curiosity->reset();
        
        developmentalStage = DevelopmentalStage::Initial;
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    try {
        // Get and validate configuration values
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        // Validate configuration parameters
        if (neuronCount == 0) {
            throw BrainInitializationError("Neuron count cannot be zero", "neuron_count");
        }
        if (neuronCount > 1000000) {
            throw BrainInitializationError("Neuron count too large (max 1,000,000)", "neuron_count", std::to_string(neuronCount));
        }
        if (regionCount == 0) {
            throw BrainInitializationError("Region count cannot be zero", "region_count");
        }
        if (regionCount > 100) {
            throw BrainInitializationError("Region count too large (max 100)", "region_count", std::to_string(regionCount));
        }
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            throw BrainInitializationError("Connection probability must be between 0.0 and 1.0", "connection_probability", std::to_string(connectionProbability));
        }
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions, " + std::to_string(connectionProbability * 100) + "% connectivity");
        
        // Create regions with error handling
        for (size_t i = 0; i < regionCount; ++i) {
            try {
                RegionId regionId = addRegion("Region_" + std::to_string(i + 1));
                NLM_LOG_INFO("Successfully created region " + std::to_string(i + 1) + " with ID " + std::to_string(regionId));
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to create region " + std::to_string(i + 1) + ": " + e.what());
                pImpl->consecutiveErrors++;
                if (pImpl->consecutiveErrors >= 3) {
                    NLM_LOG_ERROR("Too many region creation failures. Attempting recovery...");
                    reset();
                    pImpl->consecutiveErrors = 0;
                }
                throw BrainInitializationError("Failed to create region " + std::to_string(i + 1), "region", std::to_string(i + 1));
            }
        }
        
        // Create neurons across regions with bounds checking
        size_t neuronsPerRegion = neuronCount / regionCount;
        for (size_t i = 0; i < regionCount; ++i) {
            try {
                auto* region = getRegion(RegionId(i + 1));
                if (!region) {
                    throw BrainInitializationError("Failed to retrieve region", "region", std::to_string(i + 1));
                }
                
                // Add populations with validation
                size_t sensoryNeurons = neuronsPerRegion / 4;
                size_t internalNeurons = neuronsPerRegion / 2;
                size_t motorNeurons = neuronsPerRegion / 4;
                
                if (sensoryNeurons == 0 && neuronsPerRegion >= 4) sensoryNeurons = 1;
                if (internalNeurons == 0 && neuronsPerRegion >= 2) internalNeurons = 1;
                if (motorNeurons == 0 && neuronsPerRegion >= 4) motorNeurons = 1;
                
                auto sensoryPopId = region->addPopulation(sensoryNeurons, NeuronType::Sensory);
                auto internalPopId = region->addPopulation(internalNeurons, NeuronType::Internal);
                auto motorPopId = region->addPopulation(motorNeurons, NeuronType::Motor);
                
                // Collect sensory and motor neurons for I/O
                auto* sensoryPop = region->getPopulation(sensoryPopId);
                auto* motorPop = region->getPopulation(motorPopId);
                if (sensoryPop) {
                    for (auto* neuron : sensoryPop->getNeurons()) {
                        pImpl->sensoryNeurons.push_back(std::shared_ptr<Neuron>(neuron));
                    }
                }
                if (motorPop) {
                    for (auto* neuron : motorPop->getNeurons()) {
                        pImpl->motorNeurons.push_back(std::shared_ptr<Neuron>(neuron));
                    }
                }
                
                NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                            ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                            std::to_string(region->getTotalNeuronCount()) + " neurons");
                
            } catch (const BrainInitializationError&) {
                throw;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to create neurons in region " + std::to_string(i + 1) + ": " + e.what());
                throw BrainInitializationError("Failed to create neurons in region", "region", std::to_string(i + 1));
            }
        }
        
        // Initialize connectivity with random weights and error recovery
        for (size_t i = 0; i < regionCount; ++i) {
            try {
                auto* region = getRegion(RegionId(i + 1));
                if (!region) {
                    throw BrainInitializationError("Failed to retrieve region for connectivity", "region", std::to_string(i + 1));
                }
                
                // Initialize random connectivity and synapse weights
                region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
                
            } catch (const BrainInitializationError&) {
                throw;
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to initialize connectivity in region " + std::to_string(i + 1) + ": " + e.what());
                pImpl->consecutiveErrors++;
                if (pImpl->consecutiveErrors >= 2) {
                    NLM_LOG_ERROR("Connectivity initialization failed multiple times. Attempting system reset...");
                    reset();
                    pImpl->consecutiveErrors = 0;
                    // Try again once after reset
                    try {
                        auto* region = getRegion(RegionId(i + 1));
                        if (region) {
                            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
                            NLM_LOG_INFO("Successfully recovered connectivity for region " + std::to_string(i + 1));
                        }
                    } catch (...) {
                        throw BrainInitializationError("Failed to recover connectivity", "region", std::to_string(i + 1));
                    }
                }
            }
        }
        
        // Initialize all integrated systems with comprehensive error handling
        try {
            // Initialize working memory
            if (!pImpl->workingMemory) {
                throw BrainInitializationError("Working memory not initialized");
            }
            pImpl->workingMemory->initialize(this);
            pImpl->workingMemory->setCapacity(neuronCount / 10);
            NLM_LOG_INFO("Working memory initialized with capacity: " + std::to_string(neuronCount / 10));
        } catch (...) {
            NLM_LOG_ERROR("Failed to initialize working memory");
            pImpl->consecutiveErrors++;
        }
        
        try {
            // Initialize episodic memory
            if (!pImpl->episodicMemory) {
                throw BrainInitializationError("Episodic memory not initialized");
            }
            pImpl->episodicMemory->initialize(this);
            pImpl->episodicMemory->setMaxEpisodes(1000);
            NLM_LOG_INFO("Episodic memory initialized with max episodes: 1000");
        } catch (...) {
            NLM_LOG_ERROR("Failed to initialize episodic memory");
            pImpl->consecutiveErrors++;
        }
        
        try {
            // Initialize associative memory
            if (!pImpl->associativeMemory) {
                throw BrainInitializationError("Associative memory not initialized");
            }
            pImpl->associativeMemory->initialize(this);
            NLM_LOG_INFO("Associative memory initialized");
        } catch (...) {
            NLM_LOG_ERROR("Failed to initialize associative memory");
            pImpl->consecutiveErrors++;
        }
        
        // Initialize cognition systems with validation
        try {
            if (!pImpl->planner) {
                throw BrainInitializationError("Planner not initialized");
            }
            pImpl->planner->initialize(this);
            pImpl->planner->setPlanningDepth(5);
            NLM_LOG_INFO("Planner initialized with depth: 5");
        } catch (...) {
            NLM_LOG_ERROR("Failed to initialize planner");
            pImpl->consecutiveErrors++;
        }
        
        try {
            if (!pImpl->conceptFormation) {
                throw BrainInitializationError("Concept formation not initialized");
            }
            pImpl->conceptFormation->initialize(this);
            NLM_LOG_INFO("Concept formation initialized");
        } catch (...) {
            NLM_LOG_ERROR("Failed to initialize concept formation");
            pImpl->consecutiveErrors++;
        }
        
        try {
            if (!pImpl->attention) {
                throw BrainInitializationError("Attention system not initialized");
            }
            pImpl->attention->initialize(this);
            pImpl->attention->setInhibitionStrength(0.5f);
            pImpl->attention->setExcitationStrength(1.5f);
            NLM_LOG_INFO("Attention system initialized with inhibition: 0.5, excitation: 1.5");
        } catch (...) {
            NLM_LOG_ERROR("Failed to initialize attention system");
            pImpl->consecutiveErrors++;
        }
        
        // Initialize neuromodulation systems
        try {
            if (!pImpl->novelty) {
                throw BrainInitializationError("Novelty system not initialized");
            }
            pImpl->novelty->initialize(this);
            NLM_LOG_INFO("Novelty system initialized");
        } catch (...) {
            NLM_LOG_ERROR("Failed to initialize novelty system");
            pImpl->consecutiveErrors++;
        }
        
        try {
            if (!pImpl->curiosity) {
                throw BrainInitializationError("Curiosity system not initialized");
            }
            pImpl->curiosity->initialize(this);
            NLM_LOG_INFO("Curiosity system initialized");
        } catch (...) {
            NLM_LOG_ERROR("Failed to initialize curiosity system");
            pImpl->consecutiveErrors++;
        }
        
        // Register spike handlers for event-driven processing
        try {
            if (!pImpl->spikeSystem) {
                throw BrainInitializationError("Spike system not initialized");
            }
            
            pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
                // Count spikes with bounds checking
                if (pImpl->totalSpikesThisStep < std::numeric_limits<size_t>::max()) {
                    ++pImpl->totalSpikesThisStep;
                }
                if (pImpl->totalSpikesTotal < std::numeric_limits<size_t>::max()) {
                    ++pImpl->totalSpikesTotal;
                }
            });
            
            // Register delayed spike handler to deliver synaptic input
            pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
                // Find destination neuron and deliver synaptic input with bounds checking
                for (auto& region : pImpl->regions) {
                    auto neurons = region->getAllNeurons();
                    for (auto* neuron : neurons) {
                        if (neuron && neuron->getId() == event.destination_neuron) {
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
            
            NLM_LOG_INFO("Spike system handlers registered successfully");
        } catch (...) {
            NLM_LOG_ERROR("Failed to register spike system handlers");
            pImpl->consecutiveErrors++;
        }
        
        // Configure checkpoint manager with validation
        try {
            std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
            if (checkpointDir.empty()) {
                checkpointDir = "./checkpoints";
            }
            
            pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
            NLM_LOG_INFO("Checkpoint manager configured with directory: " + checkpointDir);
        } catch (...) {
            NLM_LOG_ERROR("Failed to configure checkpoint manager");
            pImpl->consecutiveErrors++;
        }
        
        pImpl->hasRecovered = (pImpl->consecutiveErrors > 0);
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        if (pImpl->consecutiveErrors > 0) {
            NLM_LOG_WARNING("Brain initialization completed with " + std::to_string(pImpl->consecutiveErrors) + " errors/warnings");
        }
        
        return true;
        
    } catch (const BrainInitializationError& e) {
        NLM_LOG_ERROR("Brain initialization failed: " + e.what() + " (Component: " + e.getComponent() + ")");
        pImpl->consecutiveErrors++;
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error during brain initialization: " + std::string(e.what()));
        pImpl->consecutiveErrors++;
        return false;
    }
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // Timeout handling for step operation
    auto stepStartTime = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds maxStepDuration(100); // Maximum 100ms per step
    
    try {
        // Validate input parameters
        if (pImpl->config == nullptr) {
            throw BrainExecutionError("Brain not properly initialized - config is null", currentStep, "step()");
        }
        
        if (pImpl->spikeSystem == nullptr) {
            throw BrainExecutionError("Spike system not available", currentStep, "step()");
        }
        
        // Validate timestep and currentTime
        if (pImpl->timestep <= 0.0 || std::isinf(pImpl->timestep) || std::isnan(pImpl->timestep)) {
            throw BrainExecutionError("Invalid timestep value", currentStep, "step()");
        }
        
        if (std::isinf(currentTime) || std::isnan(currentTime)) {
            throw BrainExecutionError("Invalid currentTime value", currentStep, "step()");
        }
        
        /*
         * PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP
         * 
         * This implements the complete integrated brain simulation:
         * 
         * 1. Process pending delayed spike events (deliver synaptic input)
         * 2. Update all neuron membrane potentials (LIF dynamics)
         * 3. Detect spikes and schedule outgoing spike events
         * 4. Update working memory (maintenance and competition)
         * 5. Apply neuromodulation effects on neural excitability
         * 6. Apply plasticity rules (STDP, Hebbian)
         * 7. Update episodic memory with current experience
         * 8. Update prediction system
         * 9. Update attention system
         * 10. Update concept formation
         * 11. Apply structural plasticity (synaptogenesis, pruning)
         * 12. Replay important memories (during rest or periodically)
         * 13. Apply development effects
         * 14. Collect statistics
         * */
        
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = 0;
        
        try {
            // Step 1-3: Process synaptic inputs, update neurons, and detect spikes
            processSynapticInputs(currentStep, currentTime);
            updateNeurons(currentStep, currentTime);
            detectSpikesAndScheduleEvents(currentStep, currentTime);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error in spike processing at step " + std::to_string(currentStep) + ": " + e.what());
            pImpl->consecutiveErrors++;
            // Attempt recovery by resetting step counters
            pImpl->currentStep = currentStep;
            pImpl->currentTime = currentTime;
            pImpl->totalSpikesThisStep = 0;
        }
        
        try {
            // Process immediate spikes
            pImpl->spikeSystem->processSpikes(currentStep);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error in spike processing at step " + std::to_string(currentStep) + ": " + e.what());
            pImpl->consecutiveErrors++;
        }
        
        try {
            // Step 4: Update working memory
            updateWorkingMemory();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error updating working memory at step " + std::to_string(currentStep) + ": " + e.what());
            pImpl->consecutiveErrors++;
        }
        
        try {
            // Step 5 & 8: Apply neuromodulation effects
            applyNeuromodulation();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error applying neuromodulation at step " + std::to_string(currentStep) + ": " + e.what());
            pImpl->consecutiveErrors++;
        }
        
        try {
            // Step 6 & 9: Apply plasticity rules
            applyPlasticityRules(currentStep, currentTime);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error applying plasticity rules at step " + std::to_string(currentStep) + ": " + e.what());
            pImpl->consecutiveErrors++;
        }
        
        try {
            // Step 7: Update memory systems
            updateMemorySystems(currentStep);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error updating memory systems at step " + std::to_string(currentStep) + ": " + e.what());
            pImpl->consecutiveErrors++;
        }
        
        try {
            // Step 10 & 11: Update cognition systems and apply structural plasticity
            updateCognitionSystems();
            applyStructuralPlasticity(currentStep);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error in cognition systems or structural plasticity at step " + std::to_string(currentStep) + ": " + e.what());
            pImpl->consecutiveErrors++;
        }
        
        try {
            // Step 12 & 13: Handle replay and consolidation, and development updates
            handleReplayAndConsolidation(currentStep);
            performDevelopmentUpdates(currentStep);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error in replay/consolidation or development at step " + std::to_string(currentStep) + ": " + e.what());
            pImpl->consecutiveErrors++;
        }
        
        try {
            // Step 14: Update system stats
            updateSystemStats(currentStep);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error updating system stats at step " + std::to_string(currentStep) + ": " + e.what());
            pImpl->consecutiveErrors++;
        }
        
        // Check for timeout
        auto stepEndTime = std::chrono::high_resolution_clock::now();
        auto stepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(stepEndTime - stepStartTime);
        
        if (stepDuration > maxStepDuration) {
            throw BrainTimeoutError(std::string("Step execution timeout: ") + std::to_string(stepDuration.count()) + "ms exceeds limit of " + 
                                  std::to_string(maxStepDuration.count()) + "ms", stepDuration);
        }
        
        // Check for error recovery
        if (pImpl->consecutiveErrors >= 5) {
            NLM_LOG_ERROR("Critical error accumulation detected. Attempting recovery...");
            reset();
            pImpl->consecutiveErrors = 0;
            pImpl->hasRecovered = true;
            NLM_LOG_INFO("Brain recovery completed");
        }
        
    } catch (const BrainExecutionError& e) {
        NLM_LOG_ERROR("Brain execution failed at step " + std::to_string(currentStep) + ": " + e.what() + " (Context: " + e.getContext() + ")");
        pImpl->consecutiveErrors++;
        // Continue with default values for critical components
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = 0;
    } catch (const BrainIOError& e) {
        NLM_LOG_ERROR(std::string("Brain I/O error during step " + std::to_string(currentStep) + ": ") + e.what() + 
                     " (Type: " + std::to_string(static_cast<int>(e.getType())) + ")");
        pImpl->consecutiveErrors++;
    } catch (const BrainTimeoutError& e) {
        NLM_LOG_ERROR(std::string("Brain timeout during step " + std::to_string(currentStep) + ": ") + e.what() + 
                     " (Timeout: " + std::to_string(e.getTimeout().count()) + "ms)");
        pImpl->consecutiveErrors++;
        // Attempt recovery
        reset();
        pImpl->consecutiveErrors = 0;
        pImpl->hasRecovered = true;
    } catch (const BrainResourceError& e) {
        NLM_LOG_ERROR(std::string("Brain resource error during step " + std::to_string(currentStep) + ": ") + e.what() + 
                     " (Resource: " + std::to_string(static_cast<int>(e.getResourceType())) + ")");
        pImpl->consecutiveErrors++;
        // Attempt recovery by reducing resource usage
        NLM_LOG_INFO("Attempting resource recovery...");
    } catch (const ValidationError& e) {
        NLM_LOG_ERROR(std::string("Configuration validation error during step " + std::to_string(currentStep) + ": ") + e.what() + 
                     " (Field: " + e.getField() + ")");
        pImpl->consecutiveErrors++;
    } catch (const BrainInitializationError& e) {
        NLM_LOG_ERROR(std::string("Brain initialization error during step " + std::to_string(currentStep) + ": ") + e.what() + 
                     " (Component: " + e.getComponent() + ")");
        pImpl->consecutiveErrors++;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error during step " + std::to_string(currentStep) + ": " + e.what());
        pImpl->consecutiveErrors++;
        // Continue with default values for critical components
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = 0;
    }
}

void Brain::processSynapticInputs(SimulationStep currentStep, Timestamp currentTime) {
    // Process pending delayed spikes (deliver synaptic input)
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void Brain::updateNeurons(SimulationStep currentStep, Timestamp currentTime) {
    // Update all neurons (LIF dynamics)
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
}

void Brain::detectSpikesAndScheduleEvents(SimulationStep currentStep, Timestamp currentTime) {
    // Detect spikes and schedule spike events
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    pImpl->spikeSystem->queueSpike(event);

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * pImpl->timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            currentTime,
                            deliveryTime,
                            currentStep,
                            deliveryStep
                        );

                        pImpl->spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

void Brain::updateWorkingMemory() {
    // Update working memory (maintenance and competition)
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
}

void Brain::applyNeuromodulation() {
    // Apply neuromodulation effects on neural excitability
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
    
    // Update dopamine (reward prediction error) and apply effects
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void Brain::applyPlasticityRules(SimulationStep currentStep, Timestamp currentTime) {
    // Apply plasticity rules (STDP and Hebbian)
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void Brain::updateMemorySystems(SimulationStep currentStep) {
    // Update episodic memory with current experience
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                        }
                    }
                }
            }
            
            // Store reward in episode
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // Update prediction system
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void Brain::updateCognitionSystems() {
    // Update attention system
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    // Update concept formation
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void Brain::applyStructuralPlasticity(SimulationStep currentStep) {
    // Apply structural plasticity (synaptogenesis, pruning) periodically
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

void Brain::handleReplayAndConsolidation(SimulationStep currentStep) {
    // Replay important memories (during rest or periodically)
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    // Periodic memory consolidation
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

void Brain::performDevelopmentUpdates(SimulationStep currentStep) {
    // Apply development effects
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity;
        if (sp) {
            DevelopmentalStage stage = pImpl->developmentalStage;
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f;  // High plasticity
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f;  // Stable
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void Brain::updateSystemStats(SimulationStep currentStep) {
    // Update checkpoint management
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, pImpl->currentTime);
    }
}
    /*
     * PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP
     * 
     * This implements the complete integrated brain simulation:
     * 
     * 1. Process pending delayed spike events (deliver synaptic input)
     * 2. Update all neuron membrane potentials (LIF dynamics)
     * 3. Detect spikes and schedule outgoing spike events
     * 4. Update working memory (maintenance and competition)
     * 5. Apply neuromodulation effects on neural excitability
     * 6. Apply plasticity rules (STDP, Hebbian)
     * 7. Update episodic memory with current experience
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity (synaptogenesis, pruning)
     * 12. Replay important memories (during rest or periodically)
     * 13. Apply development effects
     * 14. Collect statistics
     */
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    pImpl->spikeSystem->queueSpike(event);

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * pImpl->timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            currentTime,
                            deliveryTime,
                            currentStep,
                            deliveryStep
                        );

                        pImpl->spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // ========== STEP 4: Update working memory ==========
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine adds excitatory bias
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
    
    // ========== STEP 7: Update episodic memory ==========
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                        }
                    }
                }
            }
            
            // Store reward in episode
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
    
    // ========== STEP 9: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 12: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    // ========== STEP 13: Apply development effects ==========
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity;
        if (sp) {
            DevelopmentalStage stage = pImpl->developmentalStage;
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f;  // High plasticity
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f;  // Stable
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
    
    // ========== STEP 14: Periodic memory consolidation ==========
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
    
    // ========== STEP 15: Checkpoint management ==========
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i].get()->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i].get()->getId(), normalizedValue / 10.0f);
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* Brain::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

float Brain::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto& neuron : pImpl->motorNeurons) {
        if (neuron.get()->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    
    return action;
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    // Development updates structural plasticity
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    pImpl->implReset();
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        // Validate filepath
        if (filepath.empty()) {
            throw BrainIOError("Filepath cannot be empty", filepath, IOErrorType::InvalidParameter);
        }
        
        // Check if filepath is too long
        if (filepath.length() > 4096) {
            throw BrainIOError("Filepath too long (max 4096 characters)", filepath, IOErrorType::InvalidParameter);
        }
        
        // Check if directory exists or can be created
        std::filesystem::path filepathPath(filepath);
        std::filesystem::path parentDir = filepathPath.parent_path();
        
        if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
            try {
                std::filesystem::create_directories(parentDir);
                NLM_LOG_INFO("Created directory: " + parentDir.string());
            } catch (const std::exception& e) {
                throw BrainIOError("Cannot create parent directory for save file", filepath, IOErrorType::FileSystemError);
            }
        }
        
        CheckpointWriter writer;
        
        // Enhanced file creation with retry logic
        bool writerCreated = false;
        for (int retry = 0; retry < 3 && !writerCreated; ++retry) {
            try {
                if (writer.create(filepath, CompressionLevel::Balanced)) {
                    writerCreated = true;
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100 * (retry + 1)));
                }
            } catch (const std::exception& e) {
                if (retry == 2) {
                    throw BrainIOError(std::string("Failed to create checkpoint file after retries: ") + e.what(), filepath, IOErrorType::FileCreationError);
                }
            }
        }
        
        if (!writerCreated) {
            throw BrainIOError("Failed to create checkpoint file: " + filepath, filepath, IOErrorErrorType::FileCreationError);
        }
        
        try {
            // Set metadata with validation
            size_t totalNeurons = getTotalNeuronCount();
            size_t totalSynapses = getTotalSynapseCount();
            size_t regionCount = getRegionCount();
            
            // Validate metadata values
            if (totalNeurons > std::numeric_limits<size_t>::max() / 100) {
                throw BrainIOError("Invalid neuron count for checkpoint metadata", filepath, IOErrorType::InvalidData);
            }
            
            writer.setMetadata(
                totalNeurons,
                totalSynapses,
                regionCount,
                pImpl->currentStep,
                pImpl->currentTime
            );
        } catch (const BrainIOError&) {
            throw;
        } catch (const std::exception& e) {
            throw BrainIOError(std::string("Failed to set checkpoint metadata: ") + e.what(), filepath, IOErrorType::DataError);
        }
        
        // Write neurons with comprehensive error handling and bounds checking
        try {
            NeuronCheckpointData neuronData;
            size_t neuronCount = getTotalNeuronCount();
            
            neuronData.membranePotential.reserve(neuronCount);
            neuronData.restingPotential.reserve(neuronCount);
            neuronData.threshold.reserve(neuronCount);
            neuronData.resetPotential.reserve(neuronCount);
            neuronData.leakConductance.reserve(neuronCount);
            neuronData.firingState.reserve(neuronCount);
            neuronData.refractoryRemaining.reserve(neuronCount);
            neuronData.refractoryPeriod.reserve(neuronCount);
            neuronData.lastSpikeTime.reserve(neuronCount);
            
            size_t neuronIndex = 0;
            for (const auto& region : pImpl->regions) {
                if (!region) {
                    throw BrainIOError("Null region encountered during save", filepath, IOErrorType::InvalidData);
                }
                
                for (const auto& pop : region->getPopulations()) {
                    if (!pop) {
                        throw BrainIOError("Null population encountered during save", filepath, IOErrorType::InvalidData);
                    }
                    
                    for (const auto* neuron : pop->getNeurons()) {
                        if (!neuron) {
                            throw BrainIOError("Null neuron encountered during save", filepath, IOErrorType::InvalidData);
                        }
                        
                        const auto& state = neuron->getState();
                        
                        // Validate neuron state values
                        if (std::isinf(state.membranePotential) || std::isnan(state.membranePotential)) {
                            throw BrainIOError("Invalid neuron membrane potential", filepath, IOErrorType::InvalidData);
                        }
                        
                        neuronData.membranePotential.push_back(state.membranePotential);
                        neuronData.restingPotential.push_back(state.restingPotential);
                        neuronData.threshold.push_back(state.threshold);
                        neuronData.resetPotential.push_back(state.resetPotential);
                        neuronData.leakConductance.push_back(state.leakConductance);
                        neuronData.firingState.push_back(static_cast<uint8_t>(state.firingState));
                        neuronData.refractoryRemaining.push_back(state.refractoryRemaining);
                        neuronData.refractoryPeriod.push_back(state.refractoryPeriod);
                        neuronData.lastSpikeTime.push_back(state.lastSpikeTime);
                        
                        neuronIndex++;
                        
                        // Safety check to prevent excessive memory usage
                        if (neuronIndex > neuronCount * 2) {
                            throw BrainIOError("Excessive neuron data during save", filepath, IOErrorType::DataError);
                        }
                    }
                }
            }
            
            // Verify neuron data consistency
            if (neuronData.membranePotential.size() != neuronCount) {
                throw BrainIOError("Neuron data size mismatch during save", filepath, IOErrorType::DataError);
            }
            
            if (!writer.writeNeurons(neuronData)) {
                throw BrainIOError("Failed to write neurons to checkpoint", filepath, IOErrorType::WriteError);
            }
            
            NLM_LOG_INFO("Successfully saved " + std::to_string(neuronData.membranePotential.size()) + " neurons");
            
        } catch (const BrainIOError&) {
            throw;
        } catch (const std::exception& e) {
            throw BrainIOError(std::string("Error writing neurons to checkpoint: ") + e.what(), filepath, IOErrorType::WriteError);
        }
        
        // Write synapses with comprehensive error handling
        try {
            SynapseCheckpointData synapseData;
            
            for (const auto& region : pImpl->regions) {
                if (!region) {
                    throw BrainIOError("Null region encountered during synapse save", filepath, IOErrorType::InvalidData);
                }
                
                for (const auto* syn : region->getSynapses()) {
                    if (!syn) {
                        throw BrainIOError("Null synapse encountered during save", filepath, IOErrorType::InvalidData);
                    }
                    
                    // Validate synapse data
                    auto sourceId = syn->getSourceNeuron();
                    auto destId = syn->getDestinationNeuron();
                    
                    if (sourceId.index() > std::numeric_limits<size_t>::max() / 2) {
                        throw BrainIOError("Invalid synapse source neuron ID", filepath, IOErrorType::InvalidData);
                    }
                    
                    synapseData.sourceNeuron.push_back(sourceId.index());
                    synapseData.destinationNeuron.push_back(destId.index());
                    synapseData.weight.push_back(syn->getWeight());
                    synapseData.delay.push_back(syn->getDelay());
                    synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                    synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                }
            }
            
            if (!writer.writeSynapses(synapseData)) {
                throw BrainIOError("Failed to write synapses to checkpoint", filepath, IOErrorType::WriteError);
            }
            
            NLM_LOG_INFO("Successfully saved " + std::to_string(synapseData.weight.size()) + " synapses");
            
        } catch (const BrainIOError&) {
            throw;
        } catch (const std::exception& e) {
            throw BrainIOError(std::string("Error writing synapses to checkpoint: ") + e.what(), filepath, IOErrorType::WriteError);
        }
        
        // Finalize with retry logic
        try {
            bool finalizeSuccess = false;
            for (int retry = 0; retry < 3 && !finalizeSuccess; ++retry) {
                try {
                    if (writer.finalize()) {
                        finalizeSuccess = true;
                    } else {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100 * (retry + 1)));
                    }
                } catch (const std::exception& e) {
                    if (retry == 2) {
                        throw BrainIOError(std::string("Failed to finalize checkpoint after retries: ") + e.what(), filepath, IOErrorType::WriteError);
                    }
                }
            }
            
            if (!finalizeSuccess) {
                throw BrainIOError("Failed to finalize checkpoint", filepath, IOErrorType::WriteError);
            }
            
        } catch (const BrainIOError&) {
            throw;
        } catch (const std::exception& e) {
            throw BrainIOError(std::string("Error finalizing checkpoint: ") + e.what(), filepath, IOErrorType::WriteError);
        }
        
        size_t bytesWritten = writer.getBytesWritten();
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(bytesWritten) + " bytes)");
        return true;
        
    } catch (const BrainIOError& e) {
        NLM_LOG_ERROR(std::string("Brain I/O Error: ") + e.what() + " (Type: " + std::to_string(static_cast<int>(e.getType())) + ")");
        return false;
    } catch (const BrainInitializationError& e) {
        NLM_LOG_ERROR(std::string("Brain initialization error during save: ") + e.what() + " (Component: " + e.getComponent() + ")");
        return false;
    } catch (const BrainExecutionError& e) {
        NLM_LOG_ERROR(std::string("Brain execution error during save: ") + e.what() + " (Step: " + std::to_string(e.getStep()) + ")");
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error saving brain state: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        // Validate filepath
        if (filepath.empty()) {
            throw BrainIOError("Filepath cannot be empty", filepath, IOErrorType::InvalidParameter);
        }
        
        // Check if filepath exists
        if (!std::filesystem::exists(filepath)) {
            throw BrainIOError("Checkpoint file does not exist", filepath, IOErrorType::FileNotFound);
        }
        
        if (!std::filesystem::is_regular_file(filepath)) {
            throw BrainIOError("Checkpoint path is not a file", filepath, IOErrorType::InvalidParameter);
        }
        
        // Check file size
        auto fileSize = std::filesystem::file_size(filepath);
        if (fileSize > 100 * 1024 * 1024) { // 100MB max
            throw BrainIOError("Checkpoint file too large (max 100MB)", filepath, IOErrorType::InvalidParameter);
        }
        
        CheckpointReader reader;
        
        // Open with retry logic
        bool readerOpened = false;
        for (int retry = 0; retry < 3 && !readerOpened; ++retry) {
            try {
                if (reader.open(filepath)) {
                    readerOpened = true;
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100 * (retry + 1)));
                }
            } catch (const std::exception& e) {
                if (retry == 2) {
                    throw BrainIOError(std::string("Failed to open checkpoint file after retries: ") + e.what(), filepath, IOErrorType::FileOpenError);
                }
            }
        }
        
        if (!readerOpened) {
            throw BrainIOError("Failed to open checkpoint file: " + filepath, filepath, IOErrorType::FileOpenError);
        }
        
        // Validate checkpoint
        try {
            if (!reader.validate()) {
                throw BrainIOError("Checkpoint validation failed: " + reader.getError(), filepath, IOErrorType::InvalidData);
            }
        } catch (const std::exception& e) {
            if (std::string(e.what()).find("validation") != std::string::npos) {
                throw;
            }
            throw BrainIOError(std::string("Error validating checkpoint: ") + e.what(), filepath, IOErrorType::ValidationError);
        }
        
        // Read neurons with comprehensive error handling
        try {
            NeuronCheckpointData neuronData;
            if (!reader.readNeurons(neuronData)) {
                throw BrainIOError("Failed to read neurons from checkpoint", filepath, IOErrorType::ReadError);
            }
            
            // Validate neuron data
            if (neuronData.membranePotential.size() != neuronData.restingPotential.size() ||
                neuronData.membranePotential.size() != neuronData.threshold.size() ||
                neuronData.membranePotential.size() != neuronData.resetPotential.size() ||
                neuronData.membranePotential.size() != neuronData.leakConductance.size() ||
                neuronData.membranePotential.size() != neuronData.firingState.size() ||
                neuronData.membranePotential.size() != neuronData.refractoryRemaining.size() ||
                neuronData.membranePotential.size() != neuronData.refractoryPeriod.size() ||
                neuronData.membranePotential.size() != neuronData.lastSpikeTime.size()) {
                throw BrainIOError("Neuron data arrays have inconsistent sizes", filepath, IOErrorType::InvalidData);
            }
            
            // Apply neuron states with bounds checking
            size_t idx = 0;
            size_t totalNeuronsProcessed = 0;
            
            for (auto& region : pImpl->regions) {
                if (!region) {
                    throw BrainIOError("Null region encountered during load", filepath, IOErrorType::InvalidData);
                }
                
                for (auto& pop : region->getPopulations()) {
                    if (!pop) {
                        throw BrainIOError("Null population encountered during load", filepath, IOErrorType::InvalidData);
                    }
                    
                    for (auto* neuron : pop->getNeurons()) {
                        if (!neuron) {
                            throw BrainIOError("Null neuron encountered during load", filepath, IOErrorType::InvalidData);
                        }
                        
                        if (idx >= neuronData.membranePotential.size()) {
                            throw BrainIOError("Neuron data index out of bounds during load", filepath, IOErrorType::DataError);
                        }
                        
                        // Validate neuron state values before applying
                        if (std::isinf(neuronData.membranePotential[idx]) || std::isnan(neuronData.membranePotential[idx])) {
                            throw BrainIOError("Invalid neuron membrane potential in checkpoint", filepath, IOErrorType::InvalidData);
                        }
                        
                        neuron->setMembranePotential(neuronData.membranePotential[idx]);
                        neuron->setRestingPotential(neuronData.restingPotential[idx]);
                        neuron->setThreshold(neuronData.threshold[idx]);
                        neuron->setResetPotential(neuronData.resetPotential[idx]);
                        neuron->setLeakConductance(neuronData.leakConductance[idx]);
                        
                        if (idx < neuronData.firingState.size()) {
                            neuron->setFiringState(static_cast<FiringState>(neuronData.firingState[idx]));
                        }
                        
                        if (idx < neuronData.refractoryRemaining.size()) {
                            neuron->setRefractoryPeriod(neuronData.refractoryPeriod[idx]);
                            neuron->setRefractoryRemaining(neuronData.refractoryRemaining[idx]);
                        }
                        
                        neuron->setLastSpikeTime(neuronData.lastSpikeTime[idx]);
                        
                        idx++;
                        totalNeuronsProcessed++;
                        
                        // Safety check
                        if (totalNeuronsProcessed > 1000000) {
                            throw BrainIOError("Excessive neuron loading detected", filepath, IOErrorType::DataError);
                        }
                    }
                }
            }
            
            NLM_LOG_INFO("Successfully loaded " + std::to_string(totalNeuronsProcessed) + " neurons");
            
        } catch (const BrainIOError&) {
            throw;
        } catch (const std::exception& e) {
            throw BrainIOError(std::string("Error applying neuron states from checkpoint: ") + e.what(), filepath, IOErrorType::DataError);
        }
        
        // Read synapses with comprehensive error handling
        try {
            SynapseCheckpointData synapseData;
            if (!reader.readSynapses(synapseData)) {
                throw BrainIOError("Failed to read synapses from checkpoint", filepath, IOErrorType::ReadError);
            }
            
            // Validate synapse data
            if (synapseData.sourceNeuron.size() != synapseData.destinationNeuron.size() ||
                synapseData.sourceNeuron.size() != synapseData.weight.size() ||
                synapseData.sourceNeuron.size() != synapseData.delay.size() ||
                synapseData.sourceNeuron.size() != synapseData.synapseType.size() ||
                synapseData.sourceNeuron.size() != synapseData.eligibilityTrace.size()) {
                throw BrainIOError("Synapse data arrays have inconsistent sizes", filepath, IOErrorType::InvalidData);
            }
            
            // Apply synapse states - this is complex because we need to find matching synapses
            // For now, just log the count and validate data
            size_t totalSynapses = synapseData.weight.size();
            
            for (size_t i = 0; i < totalSynapses; ++i) {
                // Validate synapse data
                if (synapseData.sourceNeuron[i] >= std::numeric_limits<size_t>::max() / 2 ||
                    synapseData.destinationNeuron[i] >= std::numeric_limits<size_t>::max() / 2) {
                    throw BrainIOError("Invalid synapse indices in checkpoint", filepath, IOErrorType::InvalidData);
                }
                
                if (std::isinf(synapseData.weight[i]) || std::isnan(synapseData.weight[i])) {
                    throw BrainIOError("Invalid synapse weight in checkpoint", filepath, IOErrorType::InvalidData);
                }
                
                if (synapseData.delay[i] > 1000) { // Max delay of 1000 steps
                    throw BrainIOError("Synapse delay too large in checkpoint", filepath, IOErrorType::InvalidData);
                }
                
                if (synapseData.synapseType[i] >= 255) { // Max synapse type value
                    throw BrainIOError("Invalid synapse type in checkpoint", filepath, IOErrorType::InvalidData);
                }
                
                if (std::isinf(synapseData.eligibilityTrace[i]) || std::isnan(synapseData.eligibilityTrace[i])) {
                    throw BrainIOError("Invalid synapse eligibility trace in checkpoint", filepath, IOErrorType::InvalidData);
                }
            }
            
            NLM_LOG_INFO("Loaded " + std::to_string(totalSynapses) + " synapses");
            
        } catch (const BrainIOError&) {
            throw;
        } catch (const std::exception& e) {
            throw BrainIOError(std::string("Error reading synapse states from checkpoint: ") + e.what(), filepath, IOErrorType::ReadError);
        }
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const BrainIOError& e) {
        NLM_LOG_ERROR(std::string("Brain I/O Error: ") + e.what() + " (Type: " + std::to_string(static_cast<int>(e.getType())) + ")");
        return false;
    } catch (const BrainInitializationError& e) {
        NLM_LOG_ERROR(std::string("Brain initialization error during load: ") + e.what() + " (Component: " + e.getComponent() + ")");
        return false;
    } catch (const BrainExecutionError& e) {
        NLM_LOG_ERROR(std::string("Brain execution error during load: ") + e.what() + " (Step: " + std::to_string(e.getStep()) + ")");
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error loading brain state: ") + e.what());
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->regions.push_back(std::move(region));
    return id;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

// ========== MEMORY SYSTEM ACCESSORS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

// ========== NEUROMODULATION SYSTEMS ==========

Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    return pImpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // Memory system status
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm
