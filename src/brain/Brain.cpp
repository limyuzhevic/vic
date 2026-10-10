#include <cmath>
#include <algorithm>
#include <sstream>

// Define M_PI if not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
    std::unique_ptr<SelfModel> selfModel;
    std::unique_ptr<SocialLearning> socialLearning;
    
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
        selfModel = std::make_unique<SelfModel>();
        socialLearning = std::make_unique<SocialLearning>();
        
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
    
    // ===== CHECKPOINT DATA STRUCTURES =====
    
    // Memory system checkpoint data
    struct WorkingMemoryCheckpointData {
        std::vector<NeuronId> memoryNeurons;
        std::vector<float> memoryActivations;
        std::vector<SimulationStep> memoryTimestamps;
        std::vector<size_t> activeTraces;
        size_t capacity;
        float decayRate;
    };
    
    struct EpisodicMemoryCheckpointData {
        std::vector<EpisodicMemoryItem> episodes;
        size_t maxEpisodes;
        bool replayEnabled;
    };
    
    struct AssociativeMemoryCheckpointData {
        std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons;
        std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    };
    
    // Neuromodulation system checkpoint data
    struct NeuromodulationCheckpointData {
        // Dopamine
        float dopamineLevel;
        float dopamineBaseline;
        float dopaminePeak;
        float dopamineDecayRate;
        float dopamineReleaseRate;
        
        // Curiosity
        float curiosityLevel;
        float curiosityNoveltyWeight;
        float curiosityPredictionErrorWeight;
        float curiosityDecayRate;
        
        // Novelty
        float noveltyLevel;
        float noveltyDecayRate;
        float noveltyNoveltyThreshold;
        std::vector<float> noveltyHistory;
        std::vector<float> noveltyLastPattern;
        
        // Prediction Error
        float predictionErrorValue;
        float predictionErrorPredictedValue;
        float predictionErrorActualValue;
        std::vector<float> predictionErrorHistory;
    };
    
    // Development system checkpoint data
    struct DevelopmentCheckpointData {
        DevelopmentalStage developmentalStage;
        double developmentalAge;
        float plasticityModifier;
        bool isCriticalPeriod;
        float criticalPeriodProgress;
    };
    
    // Cognition system checkpoint data
    struct CognitionCheckpointData {
        // Neural Planner
        size_t planningDepth;
        float planningConfidence;
        std::vector<float> currentGoal;
        std::vector<float> actionQuality;
        std::deque<bool> recentPlanSuccess;
        
        // Concept Formation
        std::vector<DiscoveredConcept> concepts;
        size_t nextConceptId;
        float formationThreshold;
        float stabilityThreshold;
        size_t stabilityWindow;
        
        // Attentional Selection
        float inhibitionStrength;
        float excitationStrength;
        float competitionThreshold;
        std::vector<RegionId> attendedRegions;
        std::vector<NeuronId> winners;
        std::vector<float> neuronSalience;
        std::unordered_map<uint64_t, float> topDownBias;
        std::unordered_map<uint64_t, float> bottomUpSalience;
    };
    
    // Spike history checkpoint data
    struct SpikeHistoryCheckpointData {
        std::vector<DetailedSpikeEvent> spikeHistory;
        std::vector<std::pair<uint64_t, size_t>> spikeCountPerNeuron;
        std::unordered_map<SimulationStep, std::vector<DelayedSpikeEvent>> delayedSpikes;
    };
    
    // Random generator checkpoint data
    struct RandomGeneratorCheckpointData {
        uint64_t seed;
    };
    
    // Simulation state checkpoint data
    struct SimulationCheckpointData {
        SimulationStep currentStep;
        Timestamp currentTime;
        TimestepDuration timestep;
        size_t totalSpikesThisStep;
        size_t totalSpikesTotal;
        bool isResting;
        size_t stepsSinceLastEpisode;
        size_t replayInterval;
        size_t consolidationInterval;
    };
    
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
    
    pImpl->selfModel->initialize(this);
    pImpl->socialLearning->initialize(this);
    
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
    
    // ========== STEP 4.5: Enhanced attention integration with working memory ==========
    // Connect working memory patterns to attention system for selection
    if (pImpl->attention && pImpl->workingMemory) {
        // Get current working memory patterns for attention competition
        std::vector<NeuronId> workingMemoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        
        if (!workingMemoryNeurons.empty()) {
            // Apply attention to working memory content
            std::vector<NeuronId> attendedNeurons = pImpl->attention->processCompetition(workingMemoryNeurons);
            
            // Convert attention winners to pattern representation for attention system
            for (size_t i = 0; i < workingMemoryNeurons.size(); ++i) {
                NeuronId neuron = workingMemoryNeurons[i];
                float activation = pImpl->workingMemory->getNeuronActivation(neuron);
                
                // Apply attention-based gain modulation
                if (attendedNeurons.empty() || 
                    std::find(attendedNeurons.begin(), attendedNeurons.end(), neuron) != attendedNeurons.end()) {
                    // This neuron is attended - increase its activation through recurrent connections
                    if (pImpl->attention->getExcitationFor(neuron) > 0.0f) {
                        float excitation = pImpl->attention->getExcitationFor(neuron);
                        // Use Brain's injectCurrent method
                        this->injectCurrent(neuron, activation * excitation * 2.0f);
                        
                        // Apply top-down bias to attended neurons
                        pImpl->attention->applyTopDownBias(neuron, 0.5f);
                    }
                } else {
                    // This neuron is not attended - apply inhibition
                    float inhibition = pImpl->attention->getInhibitionFor(neuron);
                    if (inhibition > 0.0f) {
                        this->injectCurrent(neuron, -activation * inhibition * 2.0f);
                    }
                }
            }
            
            // Apply bottom-up salience based on working memory activity
            for (size_t i = 0; i < workingMemoryNeurons.size(); ++i) {
                NeuronId neuron = workingMemoryNeurons[i];
                float activation = pImpl->workingMemory->getNeuronActivation(neuron);
                
                // Salience based on activation level
                float salience = activation * 10.0f;
                if (salience > 0.1f) {
                    pImpl->attention->applyBottomUpSalience(neuron, salience);
                }
            }
        }
    }
    
    // ========== STEP 5: Enhanced neuromodulation effects ==========
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity with enhanced integration
    if (pImpl->curiosity) {
        float novelty = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
        float predictionError = pImpl->predictionError ? pImpl->predictionError->getError() : 0.0f;
        pImpl->curiosity->update(novelty, predictionError, pImpl->timestep);
        
        // Apply curiosity effects on exploration and learning
        if (pImpl->curiosity->getExplorationDrive() > 0.5f) {
            // High curiosity enhances exploration
            // Modify action selection to be more exploratory
            for (auto* neuron : pImpl->motorNeurons) {
                // Add exploration bias
                neuron->injectCurrent(0.1f);
            }
        }
    }
    
    // Update dopamine with enhanced plasticity effects
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // Apply enhanced dopamine effects on neural excitability
        // Enhanced neuromodulation rules for different plasticity types
        float dopamineLevel = pImpl->dopamine->getLevel();
        
        // Calculate enhanced neuromodulation factors
        float stdpFactor = pImpl->dopamine->getEnhancedSTDPFactor();
        float hebbianFactor = pImpl->dopamine->getEnhancedHebbianFactor();
        float structuralFactor = pImpl->dopamine->getEnhancedStructuralFactor();
        
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Enhanced dopaminergic excitability modulation
                    // Different regions respond differently to dopamine
                    float regionMod = 1.0f;
                    if (pop->getNeuronType() == NeuronType::Internal) {
                        regionMod = 1.2f;  // Internal neurons more responsive
                    } else if (pop->getNeuronType() == NeuronType::Sensory) {
                        regionMod = 0.8f;  // Sensory neurons less responsive
                    }
                    
                    // Apply neuromodulation based on dopamine level and region type
                    float excitabilityMod = dopamineLevel * regionMod * stdpFactor * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
        
        // Enhanced prediction error integration
        if (pImpl->predictionError) {
            float error = pImpl->predictionError->getError();
            float reward = dopamineLevel;  // Use dopamine level as proxy for reward
            pImpl->dopamine->updatePredictionIntegration(error, reward);
        }
    }
    
    // ========== STEP 6: Apply enhanced plasticity rules (STDP and Hebbian) ==========
    // Calculate enhanced neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
        
        // Apply enhanced dopamine effects on different plasticity types
        // STDP, Hebbian, and structural plasticity have different modulation rules
        float stdpFactor = pImpl->dopamine->getEnhancedSTDPFactor();
        float hebbianFactor = pImpl->dopamine->getEnhancedHebbianFactor();
        float structuralFactor = pImpl->dopamine->getEnhancedStructuralFactor();
        
        for (auto& region : pImpl->regions) {
            for (auto& syn : region->getSynapses()) {
                // Apply enhanced STDP with dopamine modulation
                if (syn->getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        // Apply enhanced STDP with different modulation rules
                        pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                        
                        // Enhanced weight change based on dopamine level
                        float weight = syn->getWeight();
                        float enhancedDelta = (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f * stdpFactor;
                        syn->setWeight(weight + enhancedDelta);
                    }
                }
                
                // Apply enhanced Hebbian learning with dopamine modulation
                if (syn->getPlasticityFlags().hebbian) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        // Apply enhanced Hebbian learning
                        pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                        
                        // Additional Hebbian modulation from dopamine
                        float correlationFactor = 1.0f + pImpl->dopamine->getEnhancedHebbianFactor() * 0.5f;
                        float enhancedDelta = 0.01f * correlationFactor * (plasticityMod - 1.0f);
                        syn->addToWeight(enhancedDelta);
                    }
                }
                
                // Apply enhanced reward-modulated plasticity
                if (syn->getPlasticityFlags().reward_modulated) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        // Apply enhanced reward-modulated STDP
                        float predictionError = pImpl->predictionError ? pImpl->predictionError->getError() : 0.0f;
                        float reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                        
                        // Calculate reward prediction error modulation
                        float rpeMod = 1.0f;
                        if (predictionError > 0.0f) {
                            rpeMod = 1.0f + predictionError * pImpl->dopamine->getPredictionErrorWeight();
                        } else if (reward > 0.0f) {
                            rpeMod = 1.0f + reward * pImpl->dopamine->getRewardPredictionWeight();
                        }
                        
                        // Apply RPE-modulated weight changes
                        pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                        float weight = syn->getWeight();
                        float rpeDelta = (weight > 0 ? 1.0f : -1.0f) * (rpeMod - 1.0f) * 0.001f;
                        syn->setWeight(weight + rpeDelta);
                    }
                }
                
                // Update synapse state
                syn->step(currentTime);
            }
        }
    } else {
        // Apply standard plasticity rules if no dopamine
        for (auto& region : pImpl->regions) {
            for (auto& syn : region->getSynapses()) {
                // Apply STDP
                if (syn->getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
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
    
    // ========== STEP 9.5: Update self-model ==========
    if (pImpl->selfModel) {
        // Update self-model based on current state and actions
        // This helps the agent predict consequences of its own actions
        std::vector<float> currentSensoryPattern;
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Convert working memory activations to sensory pattern representation
            currentSensoryPattern = pImpl->workingMemory->getNeuronActivations(pImpl->workingMemory->getMemoryNeurons());
        }
        
        if (!currentSensoryPattern.empty()) {
            pImpl->selfModel->updateSelfModel(
                currentSensoryPattern,
                // Would need current neural state
                currentSensoryPattern,  // placeholder
                ActionType::Wait      // Would get from current action
            );
        }
    }
    
    // ========== STEP 9.6: Update social learning ==========
    if (pImpl->socialLearning) {
        // Update social learning with current experiences
        // This helps the agent learn from others' actions and effects
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Store current experience for social learning
            std::vector<float> currentPattern = pImpl->workingMemory->getNeuronActivations(pImpl->workingMemory->getMemoryNeurons());
            pImpl->socialLearning->observeAgentAction(
                ActionType::Wait,      // Would get from current action
                currentPattern,
                currentPattern        // Would need resulting state
            );
        }
    }
    
    // ========== STEP 9.7: Update neural planner ==========
    if (pImpl->planner) {
        // Neural planner uses learned predictions to plan multi-step actions
        // This helps the agent select actions leading to high-value outcomes
        std::vector<float> currentState;
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Get current state representation from working memory
            currentState = pImpl->workingMemory->getNeuronActivations(pImpl->workingMemory->getMemoryNeurons());
        }
        
        if (!currentState.empty()) {
            // Plan next action based on current state
            ActionType plannedAction = pImpl->planner->planAction(currentState, 0.5f);
            
            // Apply planned action to motor neurons (placeholder)
            // In a full implementation, this would convert planned action to motor command
            // and apply it to the motor system
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Concept formation processes current neural activity patterns to form concepts
        // This helps discover recurring patterns from experience
        std::vector<float> currentPattern;
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Get current neural activity pattern from working memory
            currentPattern = pImpl->workingMemory->getNeuronActivations(pImpl->workingMemory->getMemoryNeurons());
        }
        
        if (!currentPattern.empty()) {
            // Present experience to concept formation system
            std::vector<float> features;  // In a real implementation, extract features from pattern
            float reward = 0.0f;          // Would get from neuromodulation
            SimulationStep currentStep = pImpl->currentStep;
            
            // Let concept formation process this experience
            size_t conceptId = pImpl->conceptFormation->presentExperience(
                currentPattern,
                features,
                reward,
                currentStep
            );
            
            // Optionally, we could use the concept to influence attention or behavior
            if (conceptId > 0) {
                // Concept formed or matched - could influence attention or planning
                // For now, just log that concept formation processed the experience
            }
        }
    }
    
    // ========== STEP 11: Apply enhanced structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        // Apply enhanced structural plasticity with neuromodulation
        if (pImpl->dopamine && pImpl->getStructuralPlasticity()) {
            // Apply dopamine-modulated structural plasticity
            float structuralFactor = pImpl->dopamine->getEnhancedStructuralFactor();
            float activityThreshold = 0.001f;  // Base activity threshold
            
            // Adjust structural plasticity parameters based on dopamine
            pImpl->structuralPlasticity->setSynaptogenesisRate(
                pImpl->structuralPlasticity->getSynaptogenesisRate() * structuralFactor);
            pImpl->structuralPlasticity->setPruningRate(
                pImpl->structuralPlasticity->getPruningRate() * (1.0f + pImpl->dopamine->getLevel()));
            
            pImpl->structuralPlasticity->update(this, *pImpl->rng);
        } else {
            // Apply standard structural plasticity
            pImpl->structuralPlasticity->update(this, *pImpl->rng);
        }
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
        
        // Development affects plasticity rates with neuromodulation integration
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
            
            // Integrate dopamine effects on developmental plasticity
            if (pImpl->dopamine) {
                float dopamineLevel = pImpl->dopamine->getLevel();
                float dopaminePlasticityMod = 1.0f - dopamineLevel * 0.3f;  // Dopamine stabilizes plasticity
                plasticityMod *= dopaminePlasticityMod;
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
    // Sophisticated sensory encoding and working memory integration
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // 1. Create sophisticated sensory patterns from input
    // Extract spatial and feature dimensions
    std::vector<float> sensoryPattern;
    sensoryPattern.reserve(values.size());
    
    // Apply feature extraction (edge detection, orientation, etc.)
    for (size_t i = 0; i < values.size(); ++i) {
        // Normalize input to range [0, 1]
        float normalizedValue = static_cast<float>(values[i]);
        
        // Apply feature enhancement (sigmoid for non-linearity)
        normalizedValue = 1.0f / (1.0f + std::exp(-normalizedValue * 5.0f));
        
        // Add spatial encoding based on neuron index
        float spatialWeight = 1.0f;
        if (numSensory > 1) {
            // Create spatial gradient across sensory neurons
            spatialWeight = static_cast<float>(i) / static_cast<float>(numSensory - 1);
            // Apply spatial modulation (sinusoidal for orientation columns)
            spatialWeight = 0.5f * (1.0f + std::sin(spatialWeight * M_PI));
        }
        
        // Combine feature and spatial information
        float encodedValue = normalizedValue * spatialWeight;
        
        // Inject current into sensory neuron for immediate processing
        float currentToInject = encodedValue * 10.0f;
        pImpl->sensoryNeurons[i]->injectCurrent(currentToInject);
        
        // Store in sensory pattern for working memory
        sensoryPattern.push_back(encodedValue);
    }
    
    // 2. Store sophisticated pattern in working memory
    if (pImpl->workingMemory && !sensoryPattern.empty()) {
        // Apply attention-based gain modulation based on input salience
        float patternStrength = 0.0f;
        for (float val : sensoryPattern) {
            patternStrength += std::abs(val);
        }
        patternStrength /= sensoryPattern.size();
        
        // Store the pattern using working memory's sophisticated store method
        pImpl->workingMemory->store(sensoryPattern, patternStrength);
        
        // Create episodic memory representation for experience storage
        if (pImpl->episodicMemory) {
            // Create episodic memory item capturing the sensory experience
            EpisodicMemoryItem episode;
            episode.timestamp = pImpl->currentStep;
            episode.sensoryState = sensoryPattern;
            episode.reward = 0.0f; // Will be updated by neuromodulators
            episode.energy = 1.0f; // Normalized energy level
            episode.novelty = patternStrength * 0.5f; // Novelty based on pattern strength
            
            // Add position information if available (for spatial experiences)
            if (input.getType() && std::string(input.getType()) == "Vision") {
                // For vision input, simulate spatial position
                // In a real implementation, this would come from the environment
                episode.positionX = 0.5f;
                episode.positionY = 0.5f;
                episode.orientation = 0.0f;
            }
            
            // Store the episode
            pImpl->episodicMemory->storeEpisode(episode);
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

// ===== CHECKPOINT IMPLEMENTATION =====

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving integrated brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata with extended information
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
                synapseData.plasticityFlags.push_back(syn->getPlasticityFlags());
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                synapseData.efficacy.push_back(syn->getEfficacy());
                synapseData.shortTermDepression.push_back(syn->getShortTermDepression());
                synapseData.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // ===== SAVE INTEGRATED SYSTEM STATES =====
        
        // 1. Save Memory System States
        {
            WorkingMemoryCheckpointData wmData;
            if (pImpl->workingMemory) {
                wmData.memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
                wmData.capacity = pImpl->workingMemory->getCapacity();
                wmData.decayRate = pImpl->workingMemory->getDecayRate();
            }
            
            // Serialize working memory
            std::vector<uint8_t> serializedWM;
            if (serializeWorkingMemory(wmData, serializedWM)) {
                if (!writer.writeSection(CheckpointSection::Memory, serializedWM.data(), serializedWM.size())) {
                    NLM_LOG_ERROR("Failed to write working memory to checkpoint");
                    return false;
                }
            }
        }
        
        // 2. Save Episodic Memory
        {
            EpisodicMemoryCheckpointData emData;
            if (pImpl->episodicMemory) {
                emData.episodes = convertToEpisodicItems(*pImpl->episodicMemory);
                emData.maxEpisodes = 1000;
                emData.replayEnabled = pImpl->episodicMemory->isReplayEnabled();
            }
            
            std::vector<uint8_t> serializedEM;
            if (serializeEpisodicMemory(emData, serializedEM)) {
                if (!writer.writeSection(CheckpointSection::Memory, serializedEM.data(), serializedEM.size())) {
                    NLM_LOG_ERROR("Failed to write episodic memory to checkpoint");
                    return false;
                }
            }
        }
        
        // 3. Save Associative Memory
        {
            AssociativeMemoryCheckpointData amData;
            if (pImpl->associativeMemory) {
                amData.patternNeurons = convertToPatternNeurons(*pImpl->associativeMemory);
                amData.associations = convertToAssociations(*pImpl->associativeMemory);
            }
            
            std::vector<uint8_t> serializedAM;
            if (serializeAssociativeMemory(amData, serializedAM)) {
                if (!writer.writeSection(CheckpointSection::Memory, serializedAM.data(), serializedAM.size())) {
                    NLM_LOG_ERROR("Failed to write associative memory to checkpoint");
                    return false;
                }
            }
        }
        
        // 4. Save Prediction System
        {
            PredictionSystemCheckpointData psData;
            if (pImpl->predictionSystem) {
                psData = convertToPredictionSystemData(*pImpl->predictionSystem);
            }
            
            std::vector<uint8_t> serializedPS;
            if (serializePredictionSystem(psData, serializedPS)) {
                if (!writer.writeSection(CheckpointSection::Memory, serializedPS.data(), serializedPS.size())) {
                    NLM_LOG_ERROR("Failed to write prediction system to checkpoint");
                    return false;
                }
            }
        }
        
        // 5. Save Neuromodulation Systems
        {
            NeuromodulationCheckpointData nmData;
            if (pImpl->dopamine) {
                nmData.dopamineLevel = pImpl->dopamine->getLevel();
            }
            if (pImpl->curiosity) {
                nmData.curiosityLevel = pImpl->curiosity->getLevel();
            }
            if (pImpl->novelty) {
                nmData.noveltyLevel = pImpl->novelty->getLevel();
            }
            if (pImpl->predictionError) {
                nmData.predictionErrorValue = pImpl->predictionError->getError();
            }
            
            std::vector<uint8_t> serializedNM;
            if (serializeNeuromodulation(nmData, serializedNM)) {
                if (!writer.writeSection(CheckpointSection::Neuromodulation, serializedNM.data(), serializedNM.size())) {
                    NLM_LOG_ERROR("Failed to write neuromodulation to checkpoint");
                    return false;
                }
            }
        }
        
        // 6. Save Development System
        {
            DevelopmentCheckpointData devData;
            if (pImpl->developmentSystem) {
                devData.developmentalStage = pImpl->developmentSystem->getStage();
                devData.developmentalAge = pImpl->developmentSystem->getDevelopmentalAge();
                devData.plasticityModifier = pImpl->developmentSystem->getPlasticityModifier();
                devData.isCriticalPeriod = pImpl->developmentSystem->isCriticalPeriod();
                devData.criticalPeriodProgress = pImpl->developmentSystem->getCriticalPeriodProgress();
            }
            
            std::vector<uint8_t> serializedDev;
            if (serializeDevelopment(devData, serializedDev)) {
                if (!writer.writeSection(CheckpointSection::Development, serializedDev.data(), serializedDev.size())) {
                    NLM_LOG_ERROR("Failed to write development system to checkpoint");
                    return false;
                }
            }
        }
        
        // 7. Save Cognition Systems
        {
            CognitionCheckpointData cogData;
            
            if (pImpl->planner) {
                cogData.planningDepth = pImpl->planner->getPlanningDepth();
                cogData.planningConfidence = pImpl->planner->getPlanningConfidence();
            }
            if (pImpl->conceptFormation) {
                cogData.concepts = pImpl->conceptFormation->getConcepts();
            }
            if (pImpl->attention) {
                cogData.inhibitionStrength = pImpl->attention->getInhibitionStrength();
            }
            
            std::vector<uint8_t> serializedCog;
            if (serializeCognition(cogData, serializedCog)) {
                if (!writer.writeSection(CheckpointSection::Neuromodulation, serializedCog.data(), serializedCog.size())) {
                    NLM_LOG_ERROR("Failed to write cognition systems to checkpoint");
                    return false;
                }
            }
        }
        
        // 8. Save Spike History
        {
            SpikeHistoryCheckpointData spikeData;
            if (pImpl->spikeSystem) {
                spikeData.spikeHistory = pImpl->spikeSystem->getSpikeHistory();
            }
            
            std::vector<uint8_t> serializedSpike;
            if (serializeSpikeHistory(spikeData, serializedSpike)) {
                if (!writer.writeSection(CheckpointSection::SpikeHistory, serializedSpike.data(), serializedSpike.size())) {
                    NLM_LOG_ERROR("Failed to write spike history to checkpoint");
                    return false;
                }
            }
        }
        
        // 9. Save Random Generator State
        {
            RandomGeneratorCheckpointData rngData;
            if (pImpl->rng) {
                rngData.seed = pImpl->rng->getSeed();
            }
            
            std::vector<uint8_t> serializedRNG;
            if (serializeRandomGenerator(rngData, serializedRNG)) {
                if (!writer.writeSection(CheckpointSection::RandomState, serializedRNG.data(), serializedRNG.size())) {
                    NLM_LOG_ERROR("Failed to write random generator to checkpoint");
                    return false;
                }
            }
        }
        
        // 10. Save Simulation State
        {
            SimulationCheckpointData simData;
            simData.currentStep = pImpl->currentStep;
            simData.currentTime = pImpl->currentTime;
            simData.timestep = pImpl->timestep;
            simData.totalSpikesThisStep = pImpl->totalSpikesThisStep;
            simData.totalSpikesTotal = pImpl->totalSpikesTotal;
            simData.isResting = pImpl->isResting;
            simData.stepsSinceLastEpisode = pImpl->stepsSinceLastEpisode;
            simData.replayInterval = pImpl->replayInterval;
            simData.consolidationInterval = pImpl->consolidationInterval;
            
            std::vector<uint8_t> serializedSim;
            if (serializeSimulation(simData, serializedSim)) {
                if (!writer.writeSection(CheckpointSection::SimulationState, serializedSim.data(), serializedSim.size())) {
                    NLM_LOG_ERROR("Failed to write simulation state to checkpoint");
                    return false;
                }
            }
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Integrated brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving integrated brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading integrated brain state from " + filepath);
    
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
        
        // Apply synapse states
        if (!applySynapseStates(synapseData)) {
            NLM_LOG_ERROR("Failed to apply synapse states from checkpoint");
            return false;
        }
        
        // ===== LOAD INTEGRATED SYSTEM STATES =====
        
        // 1. Load Memory System States
        {
            std::vector<uint8_t> memorySection = reader.readSection(CheckpointSection::Memory);
            if (!memorySection.empty()) {
                WorkingMemoryCheckpointData wmData;
                if (deserializeWorkingMemory(memorySection, wmData)) {
                    applyWorkingMemoryState(wmData);
                    NLM_LOG_INFO("Loaded working memory from checkpoint");
                }
            }
        }
        
        // 2. Load Episodic Memory
        {
            std::vector<uint8_t> episodicSection = reader.readSection(CheckpointSection::Memory);
            if (!episodicSection.empty()) {
                EpisodicMemoryCheckpointData emData;
                if (deserializeEpisodicMemory(episodicSection, emData)) {
                    applyEpisodicMemoryState(emData);
                    NLM_LOG_INFO("Loaded episodic memory from checkpoint");
                }
            }
        }
        
        // 3. Load Associative Memory
        {
            std::vector<uint8_t> associativeSection = reader.readSection(CheckpointSection::Memory);
            if (!associativeSection.empty()) {
                AssociativeMemoryCheckpointData amData;
                if (deserializeAssociativeMemory(associativeSection, amData)) {
                    applyAssociativeMemoryState(amData);
                    NLM_LOG_INFO("Loaded associative memory from checkpoint");
                }
            }
        }
        
        // 4. Load Prediction System
        {
            std::vector<uint8_t> predictionSection = reader.readSection(CheckpointSection::Memory);
            if (!predictionSection.empty()) {
                PredictionSystemCheckpointData psData;
                if (deserializePredictionSystem(predictionSection, psData)) {
                    applyPredictionSystemState(psData);
                    NLM_LOG_INFO("Loaded prediction system from checkpoint");
                }
            }
        }
        
        // 5. Load Neuromodulation Systems
        {
            std::vector<uint8_t> neuromodulationSection = reader.readSection(CheckpointSection::Neuromodulation);
            if (!neuromodulationSection.empty()) {
                NeuromodulationCheckpointData nmData;
                if (deserializeNeuromodulation(neuromodulationSection, nmData)) {
                    applyNeuromodulationState(nmData);
                    NLM_LOG_INFO("Loaded neuromodulation from checkpoint");
                }
            }
        }
        
        // 6. Load Development System
        {
            std::vector<uint8_t> developmentSection = reader.readSection(CheckpointSection::Development);
            if (!developmentSection.empty()) {
                DevelopmentCheckpointData devData;
                if (deserializeDevelopment(developmentSection, devData)) {
                    applyDevelopmentState(devData);
                    NLM_LOG_INFO("Loaded development system from checkpoint");
                }
            }
        }
        
        // 7. Load Cognition Systems
        {
            std::vector<uint8_t> cognitionSection = reader.readSection(CheckpointSection::Neuromodulation);
            if (!cognitionSection.empty()) {
                CognitionCheckpointData cogData;
                if (deserializeCognition(cognitionSection, cogData)) {
                    applyCognitionState(cogData);
                    NLM_LOG_INFO("Loaded cognition systems from checkpoint");
                }
            }
        }
        
        // 8. Load Spike History
        {
            std::vector<uint8_t> spikeHistorySection = reader.readSection(CheckpointSection::SpikeHistory);
            if (!spikeHistorySection.empty()) {
                SpikeHistoryCheckpointData spikeData;
                if (deserializeSpikeHistory(spikeHistorySection, spikeData)) {
                    applySpikeHistoryState(spikeData);
                    NLM_LOG_INFO("Loaded spike history from checkpoint");
                }
            }
        }
        
        // 9. Load Random Generator State
        {
            std::vector<uint8_t> randomSection = reader.readSection(CheckpointSection::RandomState);
            if (!randomSection.empty()) {
                RandomGeneratorCheckpointData rngData;
                if (deserializeRandomGenerator(randomSection, rngData)) {
                    applyRandomGeneratorState(rngData);
                    NLM_LOG_INFO("Loaded random generator from checkpoint");
                }
            }
        }
        
        // 10. Load Simulation State
        {
            std::vector<uint8_t> simulationSection = reader.readSection(CheckpointSection::SimulationState);
            if (!simulationSection.empty()) {
                SimulationCheckpointData simData;
                if (deserializeSimulation(simulationSection, simData)) {
                    applySimulationState(simData);
                    NLM_LOG_INFO("Loaded simulation state from checkpoint");
                }
            }
        }
        
        NLM_LOG_INFO("Integrated brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading integrated brain: ") + e.what());
        return false;
    }
}

// ===== CHECKPOINT IMPLEMENTATION =====

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving integrated brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata with extended information
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
                synapseData.plasticityFlags.push_back(syn->getPlasticityFlags());
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                synapseData.efficacy.push_back(syn->getEfficacy());
                synapseData.shortTermDepression.push_back(syn->getShortTermDepression());
                synapseData.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // ===== SAVE INTEGRATED SYSTEM STATES =====
        
        // 1. Save Memory System States
        {
            WorkingMemoryCheckpointData wmData;
            if (pImpl->workingMemory) {
                wmData.memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
                wmData.capacity = pImpl->workingMemory->getCapacity();
                wmData.decayRate = pImpl->workingMemory->getDecayRate();
            }
            
            // Serialize working memory
            std::vector<uint8_t> serializedWM;
            if (serializeWorkingMemory(wmData, serializedWM)) {
                if (!writer.writeSection(CheckpointSection::Memory, serializedWM.data(), serializedWM.size())) {
                    NLM_LOG_ERROR("Failed to write working memory to checkpoint");
                    return false;
                }
            }
        }
        
        // 2. Save Episodic Memory
        {
            EpisodicMemoryCheckpointData emData;
            if (pImpl->episodicMemory) {
                emData.episodes = convertToEpisodicItems(*pImpl->episodicMemory);
                emData.maxEpisodes = 1000;
                emData.replayEnabled = pImpl->episodicMemory->isReplayEnabled();
            }
            
            std::vector<uint8_t> serializedEM;
            if (serializeEpisodicMemory(emData, serializedEM)) {
                if (!writer.writeSection(CheckpointSection::Memory, serializedEM.data(), serializedEM.size())) {
                    NLM_LOG_ERROR("Failed to write episodic memory to checkpoint");
                    return false;
                }
            }
        }
        
        // 3. Save Associative Memory
        {
            AssociativeMemoryCheckpointData amData;
            if (pImpl->associativeMemory) {
                amData.patternNeurons = convertToPatternNeurons(*pImpl->associativeMemory);
                amData.associations = convertToAssociations(*pImpl->associativeMemory);
            }
            
            std::vector<uint8_t> serializedAM;
            if (serializeAssociativeMemory(amData, serializedAM)) {
                if (!writer.writeSection(CheckpointSection::Memory, serializedAM.data(), serializedAM.size())) {
                    NLM_LOG_ERROR("Failed to write associative memory to checkpoint");
                    return false;
                }
            }
        }
        
        // 4. Save Prediction System
        {
            PredictionSystemCheckpointData psData;
            if (pImpl->predictionSystem) {
                psData = convertToPredictionSystemData(*pImpl->predictionSystem);
            }
            
            std::vector<uint8_t> serializedPS;
            if (serializePredictionSystem(psData, serializedPS)) {
                if (!writer.writeSection(CheckpointSection::Memory, serializedPS.data(), serializedPS.size())) {
                    NLM_LOG_ERROR("Failed to write prediction system to checkpoint");
                    return false;
                }
            }
        }
        
        // 5. Save Neuromodulation Systems
        {
            NeuromodulationCheckpointData nmData;
            if (pImpl->dopamine) {
                nmData.dopamineLevel = pImpl->dopamine->getLevel();
            }
            if (pImpl->curiosity) {
                nmData.curiosityLevel = pImpl->curiosity->getLevel();
            }
            if (pImpl->novelty) {
                nmData.noveltyLevel = pImpl->novelty->getLevel();
            }
            if (pImpl->predictionError) {
                nmData.predictionErrorValue = pImpl->predictionError->getError();
            }
            
            std::vector<uint8_t> serializedNM;
            if (serializeNeuromodulation(nmData, serializedNM)) {
                if (!writer.writeSection(CheckpointSection::Neuromodulation, serializedNM.data(), serializedNM.size())) {
                    NLM_LOG_ERROR("Failed to write neuromodulation to checkpoint");
                    return false;
                }
            }
        }
        
        // 6. Save Development System
        {
            DevelopmentCheckpointData devData;
            if (pImpl->developmentSystem) {
                devData.developmentalStage = pImpl->developmentSystem->getStage();
                devData.developmentalAge = pImpl->developmentSystem->getDevelopmentalAge();
                devData.plasticityModifier = pImpl->developmentSystem->getPlasticityModifier();
                devData.isCriticalPeriod = pImpl->developmentSystem->isCriticalPeriod();
                devData.criticalPeriodProgress = pImpl->developmentSystem->getCriticalPeriodProgress();
            }
            
            std::vector<uint8_t> serializedDev;
            if (serializeDevelopment(devData, serializedDev)) {
                if (!writer.writeSection(CheckpointSection::Development, serializedDev.data(), serializedDev.size())) {
                    NLM_LOG_ERROR("Failed to write development system to checkpoint");
                    return false;
                }
            }
        }
        
        // 7. Save Cognition Systems
        {
            CognitionCheckpointData cogData;
            
            if (pImpl->planner) {
                cogData.planningDepth = pImpl->planner->getPlanningDepth();
                cogData.planningConfidence = pImpl->planner->getPlanningConfidence();
            }
            if (pImpl->conceptFormation) {
                cogData.concepts = pImpl->conceptFormation->getConcepts();
            }
            if (pImpl->attention) {
                cogData.inhibitionStrength = pImpl->attention->getInhibitionStrength();
            }
            
            std::vector<uint8_t> serializedCog;
            if (serializeCognition(cogData, serializedCog)) {
                if (!writer.writeSection(CheckpointSection::Neuromodulation, serializedCog.data(), serializedCog.size())) {
                    NLM_LOG_ERROR("Failed to write cognition systems to checkpoint");
                    return false;
                }
            }
        }
        
        // 8. Save Spike History
        {
            SpikeHistoryCheckpointData spikeData;
            if (pImpl->spikeSystem) {
                spikeData.spikeHistory = pImpl->spikeSystem->getSpikeHistory();
            }
            
            std::vector<uint8_t> serializedSpike;
            if (serializeSpikeHistory(spikeData, serializedSpike)) {
                if (!writer.writeSection(CheckpointSection::SpikeHistory, serializedSpike.data(), serializedSpike.size())) {
                    NLM_LOG_ERROR("Failed to write spike history to checkpoint");
                    return false;
                }
            }
        }
        
        // 9. Save Random Generator State
        {
            RandomGeneratorCheckpointData rngData;
            if (pImpl->rng) {
                rngData.seed = pImpl->rng->getSeed();
            }
            
            std::vector<uint8_t> serializedRNG;
            if (serializeRandomGenerator(rngData, serializedRNG)) {
                if (!writer.writeSection(CheckpointSection::RandomState, serializedRNG.data(), serializedRNG.size())) {
                    NLM_LOG_ERROR("Failed to write random generator to checkpoint");
                    return false;
                }
            }
        }
        
        // 10. Save Simulation State
        {
            SimulationCheckpointData simData;
            simData.currentStep = pImpl->currentStep;
            simData.currentTime = pImpl->currentTime;
            simData.timestep = pImpl->timestep;
            simData.totalSpikesThisStep = pImpl->totalSpikesThisStep;
            simData.totalSpikesTotal = pImpl->totalSpikesTotal;
            simData.isResting = pImpl->isResting;
            simData.stepsSinceLastEpisode = pImpl->stepsSinceLastEpisode;
            simData.replayInterval = pImpl->replayInterval;
            simData.consolidationInterval = pImpl->consolidationInterval;
            
            std::vector<uint8_t> serializedSim;
            if (serializeSimulation(simData, serializedSim)) {
                if (!writer.writeSection(CheckpointSection::SimulationState, serializedSim.data(), serializedSim.size())) {
                    NLM_LOG_ERROR("Failed to write simulation state to checkpoint");
                    return false;
                }
            }
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Integrated brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving integrated brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading integrated brain state from " + filepath);
    
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
        
        // Apply synapse states
        if (!applySynapseStates(synapseData)) {
            NLM_LOG_ERROR("Failed to apply synapse states from checkpoint");
            return false;
        }
        
        // ===== LOAD INTEGRATED SYSTEM STATES =====
        
        // 1. Load Memory System States
        {
            std::vector<uint8_t> memorySection = reader.readSection(CheckpointSection::Memory);
            if (!memorySection.empty()) {
                WorkingMemoryCheckpointData wmData;
                if (deserializeWorkingMemory(memorySection, wmData)) {
                    applyWorkingMemoryState(wmData);
                    NLM_LOG_INFO("Loaded working memory from checkpoint");
                }
            }
        }
        
        // 2. Load Episodic Memory
        {
            std::vector<uint8_t> episodicSection = reader.readSection(CheckpointSection::Memory);
            if (!episodicSection.empty()) {
                EpisodicMemoryCheckpointData emData;
                if (deserializeEpisodicMemory(episodicSection, emData)) {
                    applyEpisodicMemoryState(emData);
                    NLM_LOG_INFO("Loaded episodic memory from checkpoint");
                }
            }
        }
        
        // 3. Load Associative Memory
        {
            std::vector<uint8_t> associativeSection = reader.readSection(CheckpointSection::Memory);
            if (!associativeSection.empty()) {
                AssociativeMemoryCheckpointData amData;
                if (deserializeAssociativeMemory(associativeSection, amData)) {
                    applyAssociativeMemoryState(amData);
                    NLM_LOG_INFO("Loaded associative memory from checkpoint");
                }
            }
        }
        
        // 4. Load Prediction System
        {
            std::vector<uint8_t> predictionSection = reader.readSection(CheckpointSection::Memory);
            if (!predictionSection.empty()) {
                PredictionSystemCheckpointData psData;
                if (deserializePredictionSystem(predictionSection, psData)) {
                    applyPredictionSystemState(psData);
                    NLM_LOG_INFO("Loaded prediction system from checkpoint");
                }
            }
        }
        
        // 5. Load Neuromodulation Systems
        {
            std::vector<uint8_t> neuromodulationSection = reader.readSection(CheckpointSection::Neuromodulation);
            if (!neuromodulationSection.empty()) {
                NeuromodulationCheckpointData nmData;
                if (deserializeNeuromodulation(neuromodulationSection, nmData)) {
                    applyNeuromodulationState(nmData);
                    NLM_LOG_INFO("Loaded neuromodulation from checkpoint");
                }
            }
        }
        
        // 6. Load Development System
        {
            std::vector<uint8_t> developmentSection = reader.readSection(CheckpointSection::Development);
            if (!developmentSection.empty()) {
                DevelopmentCheckpointData devData;
                if (deserializeDevelopment(developmentSection, devData)) {
                    applyDevelopmentState(devData);
                    NLM_LOG_INFO("Loaded development system from checkpoint");
                }
            }
        }
        
        // 7. Load Cognition Systems
        {
            std::vector<uint8_t> cognitionSection = reader.readSection(CheckpointSection::Neuromodulation);
            if (!cognitionSection.empty()) {
                CognitionCheckpointData cogData;
                if (deserializeCognition(cognitionSection, cogData)) {
                    applyCognitionState(cogData);
                    NLM_LOG_INFO("Loaded cognition systems from checkpoint");
                }
            }
        }
        
        // 8. Load Spike History
        {
            std::vector<uint8_t> spikeHistorySection = reader.readSection(CheckpointSection::SpikeHistory);
            if (!spikeHistorySection.empty()) {
                SpikeHistoryCheckpointData spikeData;
                if (deserializeSpikeHistory(spikeHistorySection, spikeData)) {
                    applySpikeHistoryState(spikeData);
                    NLM_LOG_INFO("Loaded spike history from checkpoint");
                }
            }
        }
        
        // 9. Load Random Generator State
        {
            std::vector<uint8_t> randomSection = reader.readSection(CheckpointSection::RandomState);
            if (!randomSection.empty()) {
                RandomGeneratorCheckpointData rngData;
                if (deserializeRandomGenerator(randomSection, rngData)) {
                    applyRandomGeneratorState(rngData);
                    NLM_LOG_INFO("Loaded random generator from checkpoint");
                }
            }
        }
        
        // 10. Load Simulation State
        {
            std::vector<uint8_t> simulationSection = reader.readSection(CheckpointSection::SimulationState);
            if (!simulationSection.empty()) {
                SimulationCheckpointData simData;
                if (deserializeSimulation(simulationSection, simData)) {
                    applySimulationState(simData);
                    NLM_LOG_INFO("Loaded simulation state from checkpoint");
                }
            }
        }
        
        NLM_LOG_INFO("Integrated brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading integrated brain: ") + e.what());
        return false;
    }
}

// ===== CHECKPOINT IMPLEMENTATION =====

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving integrated brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata with extended information
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
                synapseData.plasticityFlags.push_back(syn->getPlasticityFlags());
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                synapseData.efficacy.push_back(syn->getEfficacy());
                synapseData.shortTermDepression.push_back(syn->getShortTermDepression());
                synapseData.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // ===== SAVE INTEGRATED SYSTEM STATES =====
        
        // 1. Save Memory System States
        {
            WorkingMemoryCheckpointData wmData;
            if (pImpl->workingMemory) {
                wmData.memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
                wmData.capacity = pImpl->workingMemory->getCapacity();
                wmData.decayRate = pImpl->workingMemory->getDecayRate();
            }
            
            // Serialize working memory
            std::vector<uint8_t> serializedWM;
            if (serializeWorkingMemory(wmData, serializedWM)) {
                if (!writer.writeSection(CheckpointSection::Memory, serializedWM.data(), serializedWM.size())) {
                    NLM_LOG_ERROR("Failed to write working memory to checkpoint");
                    return false;
                }
            }
        }
        
        // 2. Save Episodic Memory
        {
            EpisodicMemoryCheckpointData emData;
            if (pImpl->episodicMemory) {
                emData.episodes = convertToEpisodicItems(*pImpl->episodicMemory);
                emData.maxEpisodes = 1000;
                emData.replayEnabled = pImpl->episodicMemory->isReplayEnabled();
            }
            
            std::vector<uint8_t> serializedEM;
            if (serializeEpisodicMemory(emData, serializedEM)) {
                if (!writer.writeSection(CheckpointSection::Memory, serializedEM.data(), serializedEM.size())) {
                    NLM_LOG_ERROR("Failed to write episodic memory to checkpoint");
                    return false;
                }
            }
        }
        
        // 3. Save Associative Memory
        {
            AssociativeMemoryCheckpointData amData;
            if (pImpl->associativeMemory) {
                amData.patternNeurons = convertToPatternNeurons(*pImpl->associativeMemory);
                amData.associations = convertToAssociations(*pImpl->associativeMemory);
            }
            
            std::vector<uint8_t> serializedAM;
            if (serializeAssociativeMemory(amData, serializedAM)) {
                if (!writer.writeSection(CheckpointSection::Memory, serializedAM.data(), serializedAM.size())) {
                    NLM_LOG_ERROR("Failed to write associative memory to checkpoint");
                    return false;
                }
            }
        }
        
        // 4. Save Prediction System
        {
            PredictionSystemCheckpointData psData;
            if (pImpl->predictionSystem) {
                psData = convertToPredictionSystemData(*pImpl->predictionSystem);
            }
            
            std::vector<uint8_t> serializedPS;
            if (serializePredictionSystem(psData, serializedPS)) {
                if (!writer.writeSection(CheckpointSection::Memory, serializedPS.data(), serializedPS.size())) {
                    NLM_LOG_ERROR("Failed to write prediction system to checkpoint");
                    return false;
                }
            }
        }
        
        // 5. Save Neuromodulation Systems
        {
            NeuromodulationCheckpointData nmData;
            if (pImpl->dopamine) {
                nmData.dopamineLevel = pImpl->dopamine->getLevel();
            }
            if (pImpl->curiosity) {
                nmData.curiosityLevel = pImpl->curiosity->getLevel();
            }
            if (pImpl->novelty) {
                nmData.noveltyLevel = pImpl->novelty->getLevel();
            }
            if (pImpl->predictionError) {
                nmData.predictionErrorValue = pImpl->predictionError->getError();
            }
            
            std::vector<uint8_t> serializedNM;
            if (serializeNeuromodulation(nmData, serializedNM)) {
                if (!writer.writeSection(CheckpointSection::Neuromodulation, serializedNM.data(), serializedNM.size())) {
                    NLM_LOG_ERROR("Failed to write neuromodulation to checkpoint");
                    return false;
                }
            }
        }
        
        // 6. Save Development System
        {
            DevelopmentCheckpointData devData;
            if (pImpl->developmentSystem) {
                devData.developmentalStage = pImpl->developmentSystem->getStage();
                devData.developmentalAge = pImpl->developmentSystem->getDevelopmentalAge();
                devData.plasticityModifier = pImpl->developmentSystem->getPlasticityModifier();
                devData.isCriticalPeriod = pImpl->developmentSystem->isCriticalPeriod();
                devData.criticalPeriodProgress = pImpl->developmentSystem->getCriticalPeriodProgress();
            }
            
            std::vector<uint8_t> serializedDev;
            if (serializeDevelopment(devData, serializedDev)) {
                if (!writer.writeSection(CheckpointSection::Development, serializedDev.data(), serializedDev.size())) {
                    NLM_LOG_ERROR("Failed to write development system to checkpoint");
                    return false;
                }
            }
        }
        
        // 7. Save Cognition Systems
        {
            CognitionCheckpointData cogData;
            
            if (pImpl->planner) {
                cogData.planningDepth = pImpl->planner->getPlanningDepth();
                cogData.planningConfidence = pImpl->planner->getPlanningConfidence();
            }
            if (pImpl->conceptFormation) {
                cogData.concepts = pImpl->conceptFormation->getConcepts();
            }
            if (pImpl->attention) {
                cogData.inhibitionStrength = pImpl->attention->getInhibitionStrength();
            }
            
            std::vector<uint8_t> serializedCog;
            if (serializeCognition(cogData, serializedCog)) {
                if (!writer.writeSection(CheckpointSection::Neuromodulation, serializedCog.data(), serializedCog.size())) {
                    NLM_LOG_ERROR("Failed to write cognition systems to checkpoint");
                    return false;
                }
            }
        }
        
        // 8. Save Spike History
        {
            SpikeHistoryCheckpointData spikeData;
            if (pImpl->spikeSystem) {
                spikeData.spikeHistory = pImpl->spikeSystem->getSpikeHistory();
            }
            
            std::vector<uint8_t> serializedSpike;
            if (serializeSpikeHistory(spikeData, serializedSpike)) {
                if (!writer.writeSection(CheckpointSection::SpikeHistory, serializedSpike.data(), serializedSpike.size())) {
                    NLM_LOG_ERROR("Failed to write spike history to checkpoint");
                    return false;
                }
            }
        }
        
        // 9. Save Random Generator State
        {
            RandomGeneratorCheckpointData rngData;
            if (pImpl->rng) {
                rngData.seed = pImpl->rng->getSeed();
            }
            
            std::vector<uint8_t> serializedRNG;
            if (serializeRandomGenerator(rngData, serializedRNG)) {
                if (!writer.writeSection(CheckpointSection::RandomState, serializedRNG.data(), serializedRNG.size())) {
                    NLM_LOG_ERROR("Failed to write random generator to checkpoint");
                    return false;
                }
            }
        }
        
        // 10. Save Simulation State
        {
            SimulationCheckpointData simData;
            simData.currentStep = pImpl->currentStep;
            simData.currentTime = pImpl->currentTime;
            simData.timestep = pImpl->timestep;
            simData.totalSpikesThisStep = pImpl->totalSpikesThisStep;
            simData.totalSpikesTotal = pImpl->totalSpikesTotal;
            simData.isResting = pImpl->isResting;
            simData.stepsSinceLastEpisode = pImpl->stepsSinceLastEpisode;
            simData.replayInterval = pImpl->replayInterval;
            simData.consolidationInterval = pImpl->consolidationInterval;
            
            std::vector<uint8_t> serializedSim;
            if (serializeSimulation(simData, serializedSim)) {
                if (!writer.writeSection(CheckpointSection::SimulationState, serializedSim.data(), serializedSim.size())) {
                    NLM_LOG_ERROR("Failed to write simulation state to checkpoint");
                    return false;
                }
            }
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Integrated brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving integrated brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading integrated brain state from " + filepath);
    
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
        
        // Apply synapse states
        if (!applySynapseStates(synapseData)) {
            NLM_LOG_ERROR("Failed to apply synapse states from checkpoint");
            return false;
        }
        
        // ===== LOAD INTEGRATED SYSTEM STATES =====
        
        // 1. Load Memory System States
        {
            std::vector<uint8_t> memorySection = reader.readSection(CheckpointSection::Memory);
            if (!memorySection.empty()) {
                WorkingMemoryCheckpointData wmData;
                if (deserializeWorkingMemory(memorySection, wmData)) {
                    applyWorkingMemoryState(wmData);
                    NLM_LOG_INFO("Loaded working memory from checkpoint");
                }
            }
        }
        
        // 2. Load Episodic Memory
        {
            std::vector<uint8_t> episodicSection = reader.readSection(CheckpointSection::Memory);
            if (!episodicSection.empty()) {
                EpisodicMemoryCheckpointData emData;
                if (deserializeEpisodicMemory(episodicSection, emData)) {
                    applyEpisodicMemoryState(emData);
                    NLM_LOG_INFO("Loaded episodic memory from checkpoint");
                }
            }
        }
        
        // 3. Load Associative Memory
        {
            std::vector<uint8_t> associativeSection = reader.readSection(CheckpointSection::Memory);
            if (!associativeSection.empty()) {
                AssociativeMemoryCheckpointData amData;
                if (deserializeAssociativeMemory(associativeSection, amData)) {
                    applyAssociativeMemoryState(amData);
                    NLM_LOG_INFO("Loaded associative memory from checkpoint");
                }
            }
        }
        
        // 4. Load Prediction System
        {
            std::vector<uint8_t> predictionSection = reader.readSection(CheckpointSection::Memory);
            if (!predictionSection.empty()) {
                PredictionSystemCheckpointData psData;
                if (deserializePredictionSystem(predictionSection, psData)) {
                    applyPredictionSystemState(psData);
                    NLM_LOG_INFO("Loaded prediction system from checkpoint");
                }
            }
        }
        
        // 5. Load Neuromodulation Systems
        {
            std::vector<uint8_t> neuromodulationSection = reader.readSection(CheckpointSection::Neuromodulation);
            if (!neuromodulationSection.empty()) {
                NeuromodulationCheckpointData nmData;
                if (deserializeNeuromodulation(neuromodulationSection, nmData)) {
                    applyNeuromodulationState(nmData);
                    NLM_LOG_INFO("Loaded neuromodulation from checkpoint");
                }
            }
        }
        
        // 6. Load Development System
        {
            std::vector<uint8_t> developmentSection = reader.readSection(CheckpointSection::Development);
            if (!developmentSection.empty()) {
                DevelopmentCheckpointData devData;
                if (deserializeDevelopment(developmentSection, devData)) {
                    applyDevelopmentState(devData);
                    NLM_LOG_INFO("Loaded development system from checkpoint");
                }
            }
        }
        
        // 7. Load Cognition Systems
        {
            std::vector<uint8_t> cognitionSection = reader.readSection(CheckpointSection::Neuromodulation);
            if (!cognitionSection.empty()) {
                CognitionCheckpointData cogData;
                if (deserializeCognition(cognitionSection, cogData)) {
                    applyCognitionState(cogData);
                    NLM_LOG_INFO("Loaded cognition systems from checkpoint");
                }
            }
        }
        
        // 8. Load Spike History
        {
            std::vector<uint8_t> spikeHistorySection = reader.readSection(CheckpointSection::SpikeHistory);
            if (!spikeHistorySection.empty()) {
                SpikeHistoryCheckpointData spikeData;
                if (deserializeSpikeHistory(spikeHistorySection, spikeData)) {
                    applySpikeHistoryState(spikeData);
                    NLM_LOG_INFO("Loaded spike history from checkpoint");
                }
            }
        }
        
        // 9. Load Random Generator State
        {
            std::vector<uint8_t> randomSection = reader.readSection(CheckpointSection::RandomState);
            if (!randomSection.empty()) {
                RandomGeneratorCheckpointData rngData;
                if (deserializeRandomGenerator(randomSection, rngData)) {
                    applyRandomGeneratorState(rngData);
                    NLM_LOG_INFO("Loaded random generator from checkpoint");
                }
            }
        }
        
        // 10. Load Simulation State
        {
            std::vector<uint8_t> simulationSection = reader.readSection(CheckpointSection::SimulationState);
            if (!simulationSection.empty()) {
                SimulationCheckpointData simData;
                if (deserializeSimulation(simulationSection, simData)) {
                    applySimulationState(simData);
                    NLM_LOG_INFO("Loaded simulation state from checkpoint");
                }
            }
        }
        
        NLM_LOG_INFO("Integrated brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading integrated brain: ") + e.what());
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

SelfModel* Brain::getSelfModel() {
    return pImpl->selfModel.get();
}

SocialLearning* Brain::getSocialLearning() {
    return pImpl->socialLearning.get();
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
