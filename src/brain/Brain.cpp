#include "../performance/MemoryPool.hpp"
#include "../performance/EventQueue.hpp"
#include "../performance/SIMDVectorization.hpp"
#include "../performance/CheckpointSystem.hpp"

namespace nlm {

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
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Integration state
    bool isResting;  // For sleep/rest cycle
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Performance systems
    std::unique_ptr<NeuronPool> neuronPool;
    std::unique_ptr<SynapsePool> synapsePool;
    
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
    {
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // PERFORMANCE OPTIMIZATIONS
        // Initialize memory pools for neurons and synapses (if enabled)
        if (NLM_USE_MEMORY_POOL) {
            neuronPool = std::make_unique<NeuronPool>();
            synapsePool = std::make_unique<SynapsePool>();
            
            // Pre-allocate neurons based on configuration
            size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
            neuronPool->reserve(neuronCount);
            NLM_LOG_INFO("Neuron pool reserved for " + std::to_string(neuronCount) + " neurons");
        }
        
        // Initialize event queue for spike processing (if enabled)
        if (NLM_USE_EVENT_QUEUE) {
            spikeSystem->setEventQueueEnabled(true);
        }
        
        // Initialize SIMD support (if enabled)
        if (NLM_USE_SIMD) {
            SIMDVectorization::initializeSIMDSupport();
        }
        
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
        
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
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
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() {
    delete pImpl;
}

Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

// Properly implement Impl destructor to clean up all resources
Brain::Impl::~Impl() {
    // Clean up checkpoint manager
    if (checkpointManager) {
        checkpointManager.reset();
    }
    
    // Clean up neuromodulation systems
    dopamine.reset();
    curiosity.reset();
    novelty.reset();
    predictionError.reset();
    
    // Clean up cognition systems
    planner.reset();
    conceptFormation.reset();
    attention.reset();
    
    // Clean up memory systems
    workingMemory.reset();
    episodicMemory.reset();
    associativeMemory.reset();
    
    // Clean up regions (and their contained populations and neurons)
    regions.clear();
    
    // Clean up spike system
    spikeSystem.reset();
    
    // Clean up other systems
    developmentSystem.reset();
    
    // Clean up performance systems
    if (neuronPool) {
        neuronPool.reset();
    }
    if (synapsePool) {
        synapsePool.reset();
    }
    
    // Clear neuron and motor neuron lists
    sensoryNeurons.clear();
    motorNeurons.clear();
    
    // Reset random generator
    rng.reset();
    
    // Config is shared_ptr, so we don't need to delete it
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // === CONFIGURATION VALIDATION ===
    
    // Validate required configuration keys
    std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count", 
        "simulation_timestep",
        "random_seed"
    };
    
    for (const auto& key : requiredKeys) {
        if (!pImpl->config->has(key)) {
            NLM_LOG_ERROR("Missing required configuration key: " + key);
            return false;
        }
    }
    
    // Get and validate configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    double simulationTimestep = pImpl->config->getOr<double>("simulation_timestep", 0.001);
    
    // Validate configuration values
    if (neuronCount == 0) {
        NLM_LOG_ERROR("Invalid neuron_count: 0. Must be at least 1.");
        return false;
    }
    
    if (neuronCount > 1000000) {
        NLM_LOG_WARNING("Large neuron_count: " + std::to_string(neuronCount) +
                       ". This may impact performance.");
    }
    
    if (regionCount == 0) {
        NLM_LOG_ERROR("Invalid region_count: 0. Must be at least 1.");
        return false;
    }
    
    if (regionCount > 100) {
        NLM_LOG_WARNING("Large region_count: " + std::to_string(regionCount) +
                       ". This may impact performance.");
    }
    
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(connectionProbability) +
                     ". Must be between 0.0 and 1.0.");
        return false;
    }
    
    if (simulationTimestep <= 0.0) {
        NLM_LOG_ERROR("Invalid simulation_timestep: " + std::to_string(simulationTimestep) +
                     ". Must be positive.");
        return false;
    }
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    NLM_LOG_INFO("Connection probability: " + std::to_string(connectionProbability) +
                 ", Timestep: " + std::to_string(simulationTimestep) + "s");
    
    // Check memory requirements
    size_t estimatedMemoryMB = (neuronCount * 4 + regionCount * 100) / (1024 * 1024);
    if (estimatedMemoryMB > 512) {
        NLM_LOG_WARNING("Estimated memory usage: " + std::to_string(estimatedMemoryMB) +
                       " MB. Consider reducing neuron_count or region_count.");
    }
    
    // === PRE-INITIALIZATION VALIDATION ===
    
    // Validate configuration is properly initialized
    if (!pImpl->config) {
        NLM_LOG_ERROR("Configuration is null!");
        return false;
    }
    
    if (!pImpl->rng) {
        NLM_LOG_ERROR("Random generator is not properly initialized!");
        return false;
    }
    
    // Check if random generator has proper seed
    uint64_t seed = pImpl->config->getOr<uint64_t>("random_seed", 42);
    pImpl->rng->setSeed(seed);
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        if (!addRegion("Region_" + std::to_string(i + 1))) {
            NLM_LOG_ERROR("Failed to create region " + std::to_string(i + 1));
            return false;
        }
    }
    
    // Verify all regions were created
    if (getRegionCount() != regionCount) {
        NLM_LOG_ERROR("Region count mismatch: expected " + std::to_string(regionCount) +
                     ", got " + std::to_string(getRegionCount()));
        return false;
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    size_t neuronsCreated = 0;
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (!region) {
            NLM_LOG_ERROR("Failed to get region " + std::to_string(i + 1));
            return false;
        }
        
        // Add populations with error handling
        auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
        auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
        auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
        
        if (!sensoryPopId || !internalPopId || !motorPopId) {
            NLM_LOG_ERROR("Failed to create populations in region " + std::to_string(i + 1));
            return false;
        }
        
        // Collect sensory and motor neurons for I/O
        auto* sensoryPop = region->getPopulation(sensoryPopId);
        auto* motorPop = region->getPopulation(motorPopId);
        
        if (sensoryPop) {
            size_t before = pImpl->sensoryNeurons.size();
            for (auto* neuron : sensoryPop->getNeurons()) {
                pImpl->sensoryNeurons.push_back(neuron);
            }
            neuronsCreated += sensoryPop->getNeuronCount();
            
            if (pImpl->sensoryNeurons.size() == before) {
                NLM_LOG_WARNING("No sensory neurons collected in region " + std::to_string(i + 1));
            }
        }
        
        if (motorPop) {
            size_t before = pImpl->motorNeurons.size();
            for (auto* neuron : motorPop->getNeurons()) {
                pImpl->motorNeurons.push_back(neuron);
            }
            neuronsCreated += motorPop->getNeuronCount();
            
            if (pImpl->motorNeurons.size() == before) {
                NLM_LOG_WARNING("No motor neurons collected in region " + std::to_string(i + 1));
            }
        }
        
        NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                     ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons");
    }
    
    // Verify total neurons match expected
    if (neuronsCreated != neuronCount) {
        NLM_LOG_WARNING("Neuron count mismatch: expected " + std::to_string(neuronCount) +
                       ", created approximately " + std::to_string(neuronsCreated));
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (!region) {
            NLM_LOG_ERROR("Failed to get region " + std::to_string(i + 1) + " for connectivity");
            return false;
        }
        
        // Initialize random connectivity and synapse weights
        try {
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize connectivity for region " + std::to_string(i + 1) +
                         ": " + std::string(e.what()));
            return false;
        }
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    try {
        pImpl->workingMemory->initialize(this);
        pImpl->workingMemory->setCapacity(neuronCount / 10);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize working memory: " + std::string(e.what()));
        return false;
    }
    
    // Initialize episodic memory
    try {
        pImpl->episodicMemory->initialize(this);
        pImpl->episodicMemory->setMaxEpisodes(1000);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize episodic memory: " + std::string(e.what()));
        return false;
    }
    
    // Initialize associative memory
    try {
        pImpl->associativeMemory->initialize(this);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize associative memory: " + std::string(e.what()));
        return false;
    }
    
    // Initialize cognition systems
    try {
        pImpl->planner->initialize(this);
        pImpl->planner->setPlanningDepth(5);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize planner: " + std::string(e.what()));
        return false;
    }
    
    try {
        pImpl->conceptFormation->initialize(this);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize concept formation: " + std::string(e.what()));
        return false;
    }
    
    try {
        pImpl->attention->initialize(this);
        pImpl->attention->setInhibitionStrength(0.5f);
        pImpl->attention->setExcitationStrength(1.5f);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize attention system: " + std::string(e.what()));
        return false;
    }
    
    // Initialize neuromodulation
    try {
        pImpl->novelty->initialize(this);
        pImpl->curiosity->initialize(this);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize neuromodulation: " + std::string(e.what()));
        return false;
    }
    
    // Register spike handlers for event-driven processing
    try {
        pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            // Count spikes
            ++pImpl->totalSpikesThisStep;
            ++pImpl->totalSpikesTotal;
        });
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to register spike handler: " + std::string(e.what()));
        return false;
    }
    
    try {
        pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            // Find destination neuron and deliver synaptic input
            for (auto& region : pImpl->regions) {
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to register delayed spike handler: " + std::string(e.what()));
        return false;
    }
    
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    try {
        pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to configure checkpoint manager: " + std::string(e.what()));
        return false;
    }
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
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
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
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
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
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
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            pImpl->currentStep,
            pImpl->currentTime
        );
        
        // Write neurons
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    neuronData.membranePotential.push_back(state.membranePotential);
                    neuronData.restingPotential.push_back(state.restingPotential);
                    neuronData.threshold.push_back(state.threshold);
                    neuronData.resetPotential.push_back(state.resetPotential);
                    neuronData.leakConductance.push_back(state.leakConductance);
                    neuronData.firingState.push_back(static_cast<uint8_t>(state.firingState));
                    neuronData.refractoryRemaining.push_back(state.refractoryRemaining);
                    neuronData.refractoryPeriod.push_back(state.refractoryPeriod);
                    neuronData.lastSpikeTime.push_back(state.lastSpikeTime);
                }
            }
        }
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        for (const auto& region : pImpl->regions) {
            for (const auto* syn : region->getSynapses()) {
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states
        size_t idx = 0;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (idx < neuronData.membranePotential.size()) {
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
                        }
                    }
                    idx++;
                }
            }
        }
        
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        // Apply synapse states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
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
    
    // Advanced batch operations for power users
    void Brain::injectCurrentToRegion(RegionId regionId, MembranePotential current) {
        if (auto* region = getRegion(regionId)) {
            for (auto& neuron : region->getAllNeurons()) {
                neuron->injectCurrent(current);
            }
        }
    }
    
    void Brain::injectCurrentToNeuronsByType(const std::vector<NeuronType>& types, MembranePotential current) {
        for (const auto& type : types) {
            injectCurrentToNeurons(type, current);
        }
    }
    
    std::unique_ptr<Action> Brain::produceActionFromPattern(ActionType pattern) {
        auto action = std::make_unique<Action>();
        action->setType(pattern);
        // Add pattern-specific parameters based on action type
        if (pattern == ActionType::Look || pattern == ActionType::LookUp || pattern == ActionType::LookDown) {
            action->setParameters(std::vector<float>{0.5f, 0.5f});  // direction, angle
        } else if (pattern == ActionType::MoveForward || pattern == ActionType::MoveBackward) {
            action->setParameters(std::vector<float>{1.0f});  // speed
        }
        return action;
    }
    
    // Memory and state management
    void Brain::saveCheckpoint(const std::string& baseFilename, int maxCheckpoints) {
        // Simple checkpoint implementation
        std::string filename = baseFilename + "_checkpoint_" + 
                               std::to_string(getTotalSpikeCount()) + ".bin";
        save(filename);
        
        // Clean up old checkpoints
        if (maxCheckpoints > 0) {
            for (int i = 0; i < maxCheckpoints; ++i) {
                std::string oldFile = baseFilename + "_checkpoint_" + 
                                     std::to_string(i) + ".bin";
                std::remove(oldFile.c_str());
            }
        }
    }
    
    bool Brain::loadLatestCheckpoint(const std::string& baseFilename) {
        // Find the latest checkpoint file
        std::string latestFile = baseFilename;
        bool found = false;
        
        // This would be a more sophisticated implementation to find the latest checkpoint
        // For now, just try to load the base filename
        return load(latestFile);
    }
    
    // Advanced plasticity control
    void Brain::enableAllPlasticity(bool enable) {
        // Enable plasticity on all regions and synapses
        for (auto& region : pImpl->regions) {
            for (auto& synapse : region->getSynapses()) {
                // Enable all plasticity flags
                PlasticityFlags flags = PlasticityFlags::STDP | PlasticityFlags::Hebbian | 
                                      PlasticityFlags::Structural;
                synapse->setPlasticityFlags(flags);
            }
        }
    }
    
    void Brain::setPlasticityFlags(RegionId regionId, PlasticityFlags flags) {
        if (auto* region = getRegion(regionId)) {
            for (auto& synapse : region->getSynapses()) {
                synapse->setPlasticityFlags(flags);
            }
        }
    }
    
    PlasticityFlags Brain::getPlasticityFlags(RegionId regionId) const {
        if (auto* region = getRegion(regionId)) {
            if (!region->getSynapses().empty()) {
                return region->getSynapses().front()->getPlasticityFlags();
            }
        }
        return PlasticityFlags();
    }
    
    // Statistical analysis methods
    std::map<std::string, float> Brain::computeNetworkStatistics() const {
        std::map<std::string, float> stats;
        
        stats["total_neurons"] = static_cast<float>(getTotalNeuronCount());
        stats["total_synapses"] = static_cast<float>(getTotalSynapseCount());
        stats["active_neurons"] = static_cast<float>(getActiveNeuronCount());
        stats["firing_neurons"] = static_cast<float>(getFiringNeuronCount());
        stats["total_spikes"] = static_cast<float>(getTotalSpikeCount());
        stats["avg_firing_rate"] = getAverageFiringRate();
        stats["e_i_ratio"] = getExcitationInhibitionRatio();
        stats["connection_density"] = getConnectionDensity();
        
        return stats;
    }
    
    std::vector<NeuronId> Brain::getTopActiveNeurons(size_t count) const {
        // This would be implemented using the spike system
        // For now, return empty vector
        return std::vector<NeuronId>();
    }
    
    float Brain::getConnectionDensity() const {
        size_t totalNeurons = getTotalNeuronCount();
        if (totalNeurons < 2) return 0.0f;
        
        size_t connections = 0;
        for (const auto& region : pImpl->regions) {
            connections += region->getSynapseCount();
        }
        
        return static_cast<float>(connections) / (totalNeurons * (totalNeurons - 1) / 2.0f);
    }
    
    // Development and maturation
    void Brain::accelerateDevelopment() {
        if (auto* devSystem = getDevelopmentSystem()) {
            devSystem->setMaturationSpeed(2.0f);  // Double maturation speed
            devSystem->setSynaptogenesisRate(devSystem->getSynaptogenesisRate() * 1.5f);
            devSystem->setPruningRate(devSystem->getPruningRate() * 0.5f);
        }
    }
    
    void Brain::simulateAging() {
        if (auto* devSystem = getDevelopmentSystem()) {
            devSystem->setMaturationSpeed(0.5f);  // Slow down maturation
            devSystem->setSynaptogenesisRate(0.0f);  // Stop synaptogenesis
            devSystem->setPruningRate(devSystem->getPruningRate() * 2.0f);  // Increase pruning
        }
        
        // Decrease plasticity with age
        auto plasticitySystem = getStructuralPlasticity();
        if (plasticitySystem) {
            plasticitySystem->setLearningRate(plasticitySystem->getLearningRate() * 0.8f);
        }
    }
    
    DevelopmentalStage Brain::estimateDevelopmentalStage() const {
        // Simple estimation based on neuron count and synapse count
        size_t totalNeurons = getTotalNeuronCount();
        size_t totalSynapses = getTotalSynapseCount();
        
        if (totalNeurons > 1000 && totalSynapses > 10000) {
            return DevelopmentalStage::Adult;
        } else if (totalNeurons > 500 && totalSynapses > 5000) {
            return DevelopmentalStage::CriticalPeriod;
        } else if (totalNeurons > 100 && totalSynapses > 1000) {
            return DevelopmentalStage::Maturation;
        } else {
            return DevelopmentalStage::Initial;
        }
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
