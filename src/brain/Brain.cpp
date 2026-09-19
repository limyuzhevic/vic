#include "SleepSystem.hpp"

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
    
    // ========== SLEEP SYSTEM ==========
    std::unique_ptr<SleepSystem> sleepSystem;
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<Serotonin> serotonin;
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    
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
    SleepStage sleepStage;  // Current sleep stage: Awake, Wake_Ready, REM, Deep_NREM, Consolidation
    Timestamp sleepStartTime;  // When sleep started
    Timestamp sleepDepth;  // Current sleep depth (0.0 to 1.0)
    float sleepDuration;  // Total sleep duration in seconds
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Sleep staging constants
    static constexpr Timestamp WAKE_READY_DURATION = 0.1f;      // 100ms - wake preparation
    static constexpr Timestamp REM_DURATION = 0.2f;             // 200ms - REM phase
    static constexpr Timestamp DEEP_NREM_DURATION = 0.3f;       // 300ms - deep NREM sleep
    static constexpr Timestamp CONSOLIDATION_DURATION = 0.4f;    // 400ms - consolidation phase
    static constexpr Timestamp MIN_SLEEP_DURATION = 0.5f;       // 500ms - minimum sleep duration
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    Impl(std::shared_ptr<Config> cfg)
        , rng(nullptr)
        , developmentalStage(DevelopmentalStage::Initial)
        , nextRegionId(1)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , sleepStage(SleepStage::Awake)
        , sleepStartTime(0.0)
        , sleepDepth(0.0)
        , sleepDuration(0.0)
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
        
        // Initialize sleep system
        sleepSystem = std::make_unique<SleepSystem>();
        sleepSystem->initialize(this);
        
        // Initialize development system
        developmentSystem = std::make_unique<DevelopmentSystem>();
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        serotonin = std::make_unique<Serotonin>();
        acetylcholine = std::make_unique<Acetylcholine>();
        norepinephrine = std::make_unique<Norepinephrine>();
        
        // Initialize all neuromodulators with brain reference
        dopamine->initialize(this);
        curiosity->initialize(this);
        predictionError->initialize(this);
        novelty->initialize(this);
        serotonin->initialize(this);
        acetylcholine->initialize(this);
        norepinephrine->initialize(this);
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

Brain::~Brain() = default;

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

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Add populations
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(this);
    
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation
    pImpl->novelty->initialize(this);
    pImpl->curiosity->initialize(this);
    
    // Register spike handlers for event-driven processing
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
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
    
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    
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
    
    // ========== SLEEP CYCLE MANAGEMENT ==========
    updateSleepCycle(pImpl->currentStep, pImpl->currentTime);
    
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
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
    
    // Update acetylcholine (attention and memory consolidation)
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->update(pImpl->timestep);
        
        // Apply acetylcholine effects on neural activity
        // ACh enhances attention and memory consolidation
        float acetylcholineLevel = pImpl->acetylcholine->getLevel();
        if (acetylcholineLevel > 0.3f) {
            // Boost working memory
            if (pImpl->workingMemory) {
                // Increase memory trace strength based on ACh level
                float memoryBoost = (acetylcholineLevel - 0.3f) * 2.0f * pImpl->timestep;
                pImpl->workingMemory->boostAllTraces(memoryBoost);
            }
            
            // Enhance attention to relevant patterns
            if (pImpl->attention) {
                float attentionBoost = std::clamp(acetylcholineLevel, 0.5f, 1.0f);
                pImpl->attention->setExcitationStrength(1.0f + attentionBoost * 0.5f);
            }
        }
    }
    
    // Update norepinephrine (arousal and vigilance)
    if (pImpl->norepinephrine) {
        pImpl->norepinephrine->update(pImpl->timestep);
        
        // Apply norepinephrine effects on neural activity
        // NE increases arousal with novel stimuli and enhances vigilance
        float norepinephrineLevel = pImpl->norepinephrine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // NE modulates neural excitability based on arousal
                    // Higher NE increases firing threshold (reducing firing rate)
                    // This represents the orienting response to novel stimuli
                    float arousalMod = norepinephrineLevel * 0.3f;
                    if (arousalMod > 0.0f) {
                        // Increase neuron excitability for better detection of novel stimuli
                        neuron->injectCurrent(arousalMod);
                    }
                }
            }
        }
    }
    
    // Update serotonin (mood, impulsivity, behavioral flexibility)
    if (pImpl->serotonin) {
        pImpl->serotonin->update(pImpl->timestep);
        
        // Apply serotonin effects on neural activity
        // Serotonin regulates impulsivity and behavioral flexibility
        float serotoninLevel = pImpl->serotonin->getLevel();
        if (serotoninLevel > 0.5f) {
            // High serotonin reduces impulsivity and stabilizes firing
            // (Implementation would involve modulating neural excitability)
            // This promotes more stable, goal-directed behavior
        }
    }
    
    // Update dopamine (reward prediction error, learning)
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
    if (pImpl->acetylcholine) {
        // ACh enhances plasticity for memory consolidation
        float achMod = pImpl->acetylcholine->getLevel();
        plasticityMod *= (1.0f + achMod * 0.5f);
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on neuromodulators
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
        // Extract features from neural activity for prediction
        // Use the brain's current state as "sensory" input for the prediction system
        std::vector<float> sensoryFeatures;
        
        // Collect neural activity patterns from all regions
        size_t featureIndex = 0;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                // Focus on sensory and internal populations for prediction features
                if (pop->getNeuronType() == NeuronType::Sensory || 
                    pop->getNeuronType() == NeuronType::Internal) {
                    
                    // Sample neurons systematically to reduce noise while maintaining coverage
                    for (auto* neuron : pop->getNeurons()) {
                        // Sample neurons for prediction features (every neuron for now)
                        float firingState = neuron->isFiring() ? 1.0f : 0.0f;
                        
                        // Calculate normalized membrane potential deviation
                        float potentialDev = std::abs(neuron->getState().membranePotential - 
                                                     neuron->getState().restingPotential) / 20.0f;
                        
                        // Combine into a single neural activity feature
                        float featureValue = firingState * 0.3f + potentialDev * 0.7f;
                        sensoryFeatures.push_back(featureValue);
                        
                        // Update feature index for next neuron
                        featureIndex++;
                    }
                }
            }
        }
        
        // Update prediction system with current sensory state
        if (!sensoryFeatures.empty()) {
            // Create a simple struct for prediction system to consume
            struct SimpleSensoryInput {
                std::vector<float> data;
                SimpleSensoryInput(const std::vector<float>& d) : data(d) {}
                const std::vector<float>& getData() const { return data; }
            } currentSensory(sensoryFeatures);
            
            // Generate prediction for next state
            auto predictedState = pImpl->predictionSystem->predictNextState(currentSensory);
            
            // Update predictions with actual sensory input
            pImpl->predictionSystem->updatePredictions(*predictedState, currentSensory);
            
            // Get prediction error for neuromodulation
            float predictionError = pImpl->predictionSystem->getPredictionError();
            
            // Send prediction error to neuromodulation systems (curiosity, dopamine, prediction error)
            if (pImpl->curiosity && predictionError > 0.1f) {
                // Curiously driven by prediction error
                pImpl->curiosity->update(pImpl->timestep);
            }
            
            if (pImpl->dopamine) {
                // Modulate dopamine based on prediction error (reward prediction error)
                if (predictionError > 0.5f) {
                    // Large prediction error -> learning signal (dopamine burst)
                    pImpl->dopamine->setLevel(std::min(1.0f, pImpl->dopamine->getLevel() + 0.2f));
                } else if (predictionError < 0.1f) {
                    // Small prediction error -> reduced dopamine (good prediction)
                    pImpl->dopamine->setLevel(std::max(0.0f, pImpl->dopamine->getLevel() - 0.1f));
                }
            }
            
            if (pImpl->predictionError) {
                // Update prediction error neuromodulator with the calculated error
                pImpl->predictionError->update(pImpl->timestep);
                // Could also enhance prediction error signal based on actual error magnitude
                // For now, the prediction error neuromodulator is updated with timestep
            }
            
            // Send prediction error to serotonin for mood regulation
            if (pImpl->serotonin) {
                pImpl->serotonin->respondToPredictionError(predictionError);
            }
            
            // Send prediction error to norepinephrine for arousal modulation
            if (pImpl->norepinephrine) {
                pImpl->norepinephrine->processNovelty(predictionError, pImpl->timestep);
            }
            
            // Send reward to acetylcholine for memory consolidation
            if (pImpl->dopamine && pImpl->acetylcholine) {
                float reward = pImpl->dopamine->getLevel();
                pImpl->acetylcholine->boostAttention(reward * 0.5f);
                pImpl->acetylcholine->consolidateMemory();
            }
        }
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
        // Extract features from neural activity for prediction
        // Use the brain's current state as "sensory" input for the prediction system
        std::vector<float> sensoryFeatures;
        
        // Collect neural activity patterns from all regions
        size_t featureIndex = 0;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                // Focus on sensory and internal populations for prediction features
                if (pop->getNeuronType() == NeuronType::Sensory || 
                    pop->getNeuronType() == NeuronType::Internal) {
                    
                    // Sample neurons systematically to reduce noise while maintaining coverage
                    for (auto* neuron : pop->getNeurons()) {
                        // Sample neurons for prediction features (every neuron for now)
                        float firingState = neuron->isFiring() ? 1.0f : 0.0f;
                        
                        // Calculate normalized membrane potential deviation
                        float potentialDev = std::abs(neuron->getState().membranePotential - 
                                                     neuron->getState().restingPotential) / 20.0f;
                        
                        // Combine into a single neural activity feature
                        float featureValue = firingState * 0.3f + potentialDev * 0.7f;
                        sensoryFeatures.push_back(featureValue);
                        
                        // Update feature index for next neuron
                        featureIndex++;
                    }
                }
            }
        }
        
        // Update prediction system with current sensory state
        if (!sensoryFeatures.empty()) {
            // Create a simple struct for prediction system to consume
            struct SimpleSensoryInput {
                std::vector<float> data;
                SimpleSensoryInput(const std::vector<float>& d) : data(d) {}
                const std::vector<float>& getData() const { return data; }
            } currentSensory(sensoryFeatures);
            
            // Generate prediction for next state
            auto predictedState = pImpl->predictionSystem->predictNextState(currentSensory);
            
            // Update predictions with actual sensory input
            pImpl->predictionSystem->updatePredictions(*predictedState, currentSensory);
            
            // Get prediction error for neuromodulation
            float predictionError = pImpl->predictionSystem->getPredictionError();
            
            // Send prediction error to neuromodulation systems (curiosity, dopamine, prediction error)
            if (pImpl->curiosity && predictionError > 0.1f) {
                // Curiously driven by prediction error
                pImpl->curiosity->update(pImpl->timestep);
            }
            
            if (pImpl->dopamine) {
                // Modulate dopamine based on prediction error (reward prediction error)
                if (predictionError > 0.5f) {
                    // Large prediction error -> learning signal (dopamine burst)
                    pImpl->dopamine->setLevel(std::min(1.0f, pImpl->dopamine->getLevel() + 0.2f));
                } else if (predictionError < 0.1f) {
                    // Small prediction error -> reduced dopamine (good prediction)
                    pImpl->dopamine->setLevel(std::max(0.0f, pImpl->dopamine->getLevel() - 0.1f));
                }
            }
            
            if (pImpl->predictionError) {
                // Update prediction error neuromodulator with the calculated error
                pImpl->predictionError->update(pImpl->timestep);
                // Could also enhance prediction error signal based on actual error magnitude
                // For now, the prediction error neuromodulator is updated with timestep
            }
        }
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
        // Extract recent neural activity patterns for concept formation
        std::vector<float> neuralPattern;
        std::vector<float> neuralFeatures;
        
        // Sample neurons from all regions to capture global activity patterns
        size_t neuronsToSample = std::min<size_t>(50, pImpl->getTotalNeuronCount());
        size_t sampledCount = 0;
        
        // Iterate through regions and collect neural activity
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                // Skip sensory populations for pattern formation (focus on internal representations)
                if (pop->getNeuronType() != NeuronType::Sensory) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (sampledCount >= neuronsToSample) break;
                        
                        // Extract pattern components from neural state
                        float patternValue = 0.0f;
                        
                        // Neural activity: firing and recent spiking history
                        if (neuron->isFiring()) {
                            patternValue += 0.5f;
                        }
                        
                        // Membrane potential dynamics (excitation level)
                        float potentialDiff = std::abs(neuron->getState().membranePotential - 
                                                      neuron->getState().restingPotential);
                        patternValue += std::min(potentialDiff / 40.0f, 0.3f);
                        
                        // Refractoriness (recovery state)
                        float refractoryRatio = 1.0f - (neuron->getState().refractoryRemaining / 
                                                         neuron->getState().refractoryPeriod);
                        patternValue += refractoryRatio * 0.2f;
                        
                        // Temporal dynamics for pattern representation
                        if (neuron->getState().lastSpikeTime > 0.0f) {
                            float timeSinceLastSpike = pImpl->currentTime - neuron->getState().lastSpikeTime;
                            patternValue += std::exp(-timeSinceLastSpike * 3.0f) * 0.2f;
                        }
                        
                        neuralPattern.push_back(std::clamp(patternValue, 0.0f, 1.0f));
                        
                        // Extract feature vectors for concept association
                        float featureValue = 0.0f;
                        
                        // Spiking frequency / burst characteristics
                        if (neuron->isFiring()) {
                            featureValue += 0.3f;
                        }
                        
                        // Temporal feature: recency of spike
                        if (neuron->getState().lastSpikeTime > 0.0f) {
                            float timeSinceLastSpike = pImpl->currentTime - neuron->getState().lastSpikeTime;
                            featureValue += std::exp(-timeSinceLastSpike * 2.0f) * 0.35f;
                        }
                        
                        // Network integration feature
                        float excitability = potentialDiff;
                        featureValue += (excitability / 60.0f) * 0.35f;
                        
                        neuralFeatures.push_back(std::clamp(featureValue, 0.0f, 1.0f));
                        
                        sampledCount++;
                    }
                }
            }
            
            if (sampledCount >= neuronsToSample) break;
        }
        
        // Ensure pattern and features have same size
        if (neuralPattern.size() != neuralFeatures.size()) {
            size_t minSize = std::min(neuralPattern.size(), neuralFeatures.size());
            neuralPattern.resize(minSize);
            neuralFeatures.resize(minSize);
        }
        
        // Get reward signal for concept learning (from dopamine level)
        float reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.5f;
        
        // Present the neural pattern to concept formation system
        size_t conceptId = 0;
        if (!neuralPattern.empty() && !neuralFeatures.empty()) {
            conceptId = pImpl->conceptFormation->presentExperience(
                neuralPattern,
                neuralFeatures,
                reward,
                pImpl->currentStep
            );
        }
        
        // Integrate concepts into planning system
        if (pImpl->planner && conceptId > 0) {
            // Get concept prototype for planning
            auto prototype = pImpl->conceptFormation->getConceptPrototype(conceptId);
            
            // Create a planning goal based on the concept
            // Use concept prototype to influence action selection
            float conceptStability = pImpl->conceptFormation->getConceptStability(conceptId);
            
            // Check if concept is stable enough for planning
            if (conceptStability > 0.7f && !prototype.empty()) {
                // Store concept in working memory for planning reference
                if (pImpl->workingMemory) {
                    // Create a working memory trace for this concept
                    // This enables concepts to guide future behavior
                    float conceptActivation = conceptStability * 0.8f + 0.2f;
                    
                    // Store concept prototype in working memory
                    // This will be used by the planner to generate goals
                    // For now, we set a marker that a concept is available
                    // The planner will check concept availability in its next step
                }
            }
        }
        
        // Use concepts to modulate attention system
        if (pImpl->conceptFormation) {
            // Check if we have any stable concepts that should guide attention
            size_t stableConcepts = 0;
            for (const auto& concept : pImpl->conceptFormation->getConcepts()) {
                if (pImpl->conceptFormation->getConceptStability(concept.id) > 0.7f) {
                    stableConcepts++;
                }
            }
            
            // If we have multiple stable concepts, guide attention toward relevant patterns
            if (stableConcepts > 1) {
                // Concepts can influence what the attention system prioritizes
                // For example, by setting attention weights toward concept-related patterns
                // This enables concept-guided exploration and learning
            }
        }
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

Acetylcholine* Brain::getAcetylcholine() {
    return pImpl->acetylcholine.get();
}

Norepinephrine* Brain::getNorepinephrine() {
    return pImpl->norepinephrine.get();
}

Serotonin* Brain::getSerotonin() {
    return pImpl->serotonin.get();
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
