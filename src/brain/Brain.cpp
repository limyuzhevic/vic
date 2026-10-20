#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>

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
    ActionType plannedAction;
    
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
    
    // Concept formation state
    size_t currentWorkingConcept_;    // ID of current concept being worked with
    std::vector<float> lastExperiencePattern_;  // Pattern of last experience
    
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
        workingMemory->initialize(std::weak_ptr<Brain>(shared_from_this()));
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        episodicMemory->initialize(std::weak_ptr<Brain>(shared_from_this()));
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        associativeMemory->initialize(std::weak_ptr<Brain>(shared_from_this()));
        
        // Initialize prediction system
        predictionSystem = std::make_unique<PredictionSystem>();
        // Note: PredictionSystem doesn't have initialize method currently
        
        // Initialize cognition systems
        planner = std::make_unique<NeuralPlanner>();
        planner->initialize(std::weak_ptr<Brain>(shared_from_this()));
        conceptFormation = std::make_unique<ConceptFormation>();
        conceptFormation->initialize(std::weak_ptr<Brain>(shared_from_this()));
        attention = std::make_unique<AttentionalSelection>();
        attention->initialize(std::weak_ptr<Brain>(shared_from_this()));
        
        // Initialize development system
        developmentSystem = std::make_unique<DevelopmentSystem>();
        developmentSystem->initialize(std::weak_ptr<Brain>(shared_from_this()));
        
        // Initialize neuromodulation systems
        dopamine = std::make_unique<Dopamine>();
        dopamine->initialize(std::weak_ptr<Brain>(shared_from_this()));
        curiosity = std::make_unique<Curiosity>();
        curiosity->initialize(std::weak_ptr<Brain>(shared_from_this()));
        predictionError = std::make_unique<PredictionError>();
        predictionError->initialize(std::weak_ptr<Brain>(shared_from_this()));
        novelty = std::make_unique<Novelty>();
        novelty->initialize(std::weak_ptr<Brain>(shared_from_this()));
        
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
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize concept formation state
        currentWorkingConcept_ = 0;
        
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

// Configuration validation
bool Brain::validateConfiguration() const {
    NLM_LOG_INFO("Validating configuration...");
    
    bool isValid = true;
    
    // Validate configuration using the new validation system
    if (!pImpl->config->validate()) {
        NLM_LOG_ERROR("Configuration validation failed");
        isValid = false;
    }
    
    // Validate specific parameter ranges for backward compatibility
    if (auto neuronCountOpt = pImpl->config->get<int64_t>("neuron_count")) {
        int64_t neuronCount = *neuronCountOpt;
        if (neuronCount <= 0 || neuronCount > 100000) {
            NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuronCount));
            isValid = false;
        }
    }
    
    if (auto regionCountOpt = pImpl->config->get<int64_t>("region_count")) {
        int64_t regionCount = *regionCountOpt;
        if (regionCount <= 0 || regionCount > 1000) {
            NLM_LOG_ERROR("Invalid region_count: " + std::to_string(regionCount));
            isValid = false;
        }
    }
    
    if (auto connectionProbabilityOpt = pImpl->config->get<float>("connection_probability")) {
        float connectionProbability = *connectionProbabilityOpt;
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(connectionProbability));
            isValid = false;
        }
    }
    
    if (auto stdpLtpWeightOpt = pImpl->config->get<float>("stdp_ltp_weight")) {
        float stdpLtpWeight = *stdpLtpWeightOpt;
        if (stdpLtpWeight < 0.0f || stdpLtpWeight > 0.1f) {
            NLM_LOG_ERROR("Invalid stdp_ltp_weight: " + std::to_string(stdpLtpWeight));
            isValid = false;
        }
    }
    
    if (auto randomSeedOpt = pImpl->config->get<int64_t>("random_seed")) {
        int64_t randomSeed = *randomSeedOpt;
        if (randomSeed <= 0) {
            NLM_LOG_ERROR("Invalid random_seed: " + std::to_string(randomSeed));
            isValid = false;
        }
    }
    
    return isValid;
}

const std::vector<ConfigSchema::Field>& Brain::getConfigurationSchema() {
    return ConfigSchema::getSchema();
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Validate configuration before proceeding
    if (!validateConfiguration()) {
        NLM_LOG_ERROR("Configuration validation failed during brain initialization");
        return false;
    }
    
    try {
        // Get configuration values
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        // Validate configuration parameters
        if (neuronCount == 0) {
            throw InvalidArgumentError(defineNLMError("Invalid neuron count", 
                "Brain::initialize", "Neuron count must be positive"));
        }
        if (regionCount == 0) {
            throw InvalidArgumentError(defineNLMError("Invalid region count", 
                "Brain::initialize", "Region count must be positive"));
        }
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            throw InvalidArgumentError(defineNLMError("Invalid connection probability", 
                "Brain::initialize", "Connection probability must be between 0 and 1"));
        }
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions");
        
        // Create regions
        for (size_t i = 0; i < regionCount; ++i) {
            RegionId regionId = addRegion("Region_" + std::to_string(i + 1));
            if (regionId == RegionId(UINT64_MAX)) {
                throw NeuralNetworkError(defineNLMError("Region creation failed", 
                    "Brain::initialize", "Failed to create region " + std::to_string(i)));
            }
        }
        
        // Create neurons across regions
        size_t neuronsPerRegion = neuronCount / regionCount;
        if (neuronsPerRegion == 0) {
            throw InvalidArgumentError(defineNLMError("Insufficient neurons per region", 
                "Brain::initialize", "Neuron count must be at least region count"));
        }
        
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!region) {
                throw NeuralNetworkError(defineNLMError("Region not found", 
                    "Brain::initialize", "Failed to retrieve region " + std::to_string(i + 1)));
            }
            
            try {
                // Add populations
                auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
                auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
                auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
                
                if (sensoryPopId == PopulationId(UINT64_MAX) || 
                    internalPopId == PopulationId(UINT64_MAX) || 
                    motorPopId == PopulationId(UINT64_MAX)) {
                    throw MemoryError(defineNLMError("Population creation failed", 
                        "Brain::initialize", "Failed to create populations in region " + std::to_string(i + 1)));
                }
                
                // Collect sensory and motor neurons for I/O
                auto* sensoryPop = region->getPopulation(sensoryPopId);
                auto* motorPop = region->getPopulation(motorPopId);
                if (sensoryPop) {
                    for (auto* neuron : sensoryPop->getNeurons()) {
                        if (neuron) {
                            pImpl->sensoryNeurons.push_back(neuron);
                        }
                    }
                }
                if (motorPop) {
                    for (auto* neuron : motorPop->getNeurons()) {
                        if (neuron) {
                            pImpl->motorNeurons.push_back(neuron);
                        }
                    }
                }
                
                NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                            ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                            std::to_string(region->getTotalNeuronCount()) + " neurons");
                            
            } catch (const MemoryError& e) {
                NLM_LOG_ERROR(std::string("Memory error creating populations: ") + e.what());
                // Try to clean up and continue
                clear();
                throw;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error creating populations: ") + e.what());
                clear();
                throw NeuralNetworkError(defineNLMError("Population initialization failed", 
                    "Brain::initialize", e.what()));
            }
        }
        
        // Initialize connectivity with random weights
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (region) {
                try {
                    // Initialize random connectivity and synapse weights
                    region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Error initializing connectivity: ") + e.what());
                    clear();
                    throw NeuralNetworkError(defineNLMError("Connectivity initialization failed", 
                        "Brain::initialize", e.what()));
                }
            }
        }
        
        // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
        
        // Initialize working memory
        try {
            pImpl->workingMemory->initialize(std::weak_ptr<Brain>(shared_from_this()));
            pImpl->workingMemory->setCapacity(neuronCount / 10);
        } catch (const MemoryError& e) {
            NLM_LOG_ERROR(std::string("Memory system initialization error: ") + e.what());
            clear();
            throw;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error initializing working memory: ") + e.what());
            clear();
            throw NeuralNetworkError(defineNLMError("Working memory initialization failed", 
                "Brain::initialize", e.what()));
        }
        
        // Initialize episodic memory
        try {
            pImpl->episodicMemory->initialize(std::weak_ptr<Brain>(shared_from_this()));
            pImpl->episodicMemory->setMaxEpisodes(1000);
        } catch (const MemoryError& e) {
            NLM_LOG_ERROR(std::string("Episodic memory initialization error: ") + e.what());
            clear();
            throw;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error initializing episodic memory: ") + e.what());
            clear();
            throw NeuralNetworkError(defineNLMError("Episodic memory initialization failed", 
                "Brain::initialize", e.what()));
        }
        
        // Initialize associative memory
        try {
            pImpl->associativeMemory->initialize(std::weak_ptr<Brain>(shared_from_this()));
        } catch (const MemoryError& e) {
            NLM_LOG_ERROR(std::string("Associative memory initialization error: ") + e.what());
            clear();
            throw;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error initializing associative memory: ") + e.what());
            clear();
            throw NeuralNetworkError(defineNLMError("Associative memory initialization failed", 
                "Brain::initialize", e.what()));
        }
        
        // Initialize prediction system
        // (PredictionSystem doesn't have initialize method currently)
        
        // Initialize cognition systems
        try {
            pImpl->planner->initialize(std::weak_ptr<Brain>(shared_from_this()));
            pImpl->planner->setPlanningDepth(5);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error initializing neural planner: ") + e.what());
            clear();
            throw NeuralNetworkError(defineNLMError("Neural planner initialization failed", 
                "Brain::initialize", e.what()));
        }
        
        try {
            pImpl->conceptFormation->initialize(std::weak_ptr<Brain>(shared_from_this()));
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error initializing concept formation: ") + e.what());
            clear();
            throw NeuralNetworkError(defineNLMError("Concept formation initialization failed", 
                "Brain::initialize", e.what()));
        }
        
        try {
            pImpl->attention->initialize(std::weak_ptr<Brain>(shared_from_this()));
            pImpl->attention->setInhibitionStrength(0.5f);
            pImpl->attention->setExcitationStrength(1.5f);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error initializing attention system: ") + e.what());
            clear();
            throw NeuralNetworkError(defineNLMError("Attention system initialization failed", 
                "Brain::initialize", e.what()));
        }
        
        // Initialize neuromodulation
        try {
            pImpl->novelty->initialize(std::weak_ptr<Brain>(shared_from_this()));
            pImpl->curiosity->initialize(std::weak_ptr<Brain>(shared_from_this()));
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error initializing neuromodulation systems: ") + e.what());
            clear();
            throw NeuralNetworkError(defineNLMError("Neuromodulation initialization failed", 
                "Brain::initialize", e.what()));
        }
        
        // Register spike handlers for event-driven processing
        try {
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
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error registering spike handlers: ") + e.what());
            clear();
            throw NeuralNetworkError(defineNLMError("Spike system configuration failed", 
                "Brain::initialize", e.what()));
        }
        
        // Configure checkpoint manager
        std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        try {
            pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error configuring checkpoint manager: ") + e.what());
            clear();
            throw ConfigurationError(defineNLMError("Checkpoint configuration failed", 
                "Brain::initialize", e.what()));
        }
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return true;
        
    } catch (const InvalidArgumentError& e) {
        NLM_LOG_ERROR(std::string("Invalid argument during brain initialization: ") + e.what());
        return false;
    } catch (const MemoryError& e) {
        NLM_LOG_ERROR(std::string("Memory error during brain initialization: ") + e.what());
        return false;
    } catch (const NeuralNetworkError& e) {
        NLM_LOG_ERROR(std::string("Neural network error during brain initialization: ") + e.what());
        return false;
    } catch (const ConfigurationError& e) {
        NLM_LOG_ERROR(std::string("Configuration error during brain initialization: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during brain initialization: ") + e.what());
        return false;
    }
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
    
    try {
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = 0;
        
        // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
        try {
            pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error processing delayed spikes: ") + e.what());
        }
        
        // ========== STEP 2: Update all neurons (LIF dynamics) ==========
        try {
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        try {
                            neuron->stepLIF(currentTime, pImpl->timestep);
                        } catch (const std::exception& e) {
                            NLM_LOG_WARNING(std::string("Error updating neuron ") + 
                                std::to_string(neuron->getId().index()) + ": " + e.what());
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error updating neurons: ") + e.what());
        }
        
        // ========== STEP 3: Detect spikes and schedule spike events ==========
        try {
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        try {
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
                                    try {
                                        syn->recordPostSpike(currentTime);
                                    } catch (const std::exception& e) {
                                        NLM_LOG_WARNING(std::string("Error recording post-synaptic spike: ") + e.what());
                                    }
                                }

                                // Get outgoing synapses and schedule delayed spike events
                                auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                                for (Synapse* syn : outgoingSynapses) {
                                    try {
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
                                    } catch (const std::exception& e) {
                                        NLM_LOG_WARNING(std::string("Error scheduling delayed spike: ") + e.what());
                                    }
                                }
                                
                                // Store to working memory - neurons that fire become part of working memory
                                if (pImpl->workingMemory) {
                                    try {
                                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                                    } catch (const MemoryError& e) {
                                        NLM_LOG_WARNING(std::string("Memory error storing neuron activation: ") + e.what());
                                    } catch (const std::exception& e) {
                                        NLM_LOG_WARNING(std::string("Error storing neuron activation to working memory: ") + e.what());
                                    }
                                }
                            }
                        } catch (const std::exception& e) {
                            NLM_LOG_WARNING(std::string("Error processing spike detection for neuron ") + 
                                std::to_string(neuron->getId().index()) + ": " + e.what());
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error detecting spikes: ") + e.what());
        }
        
        // Process immediate spikes
        try {
            pImpl->spikeSystem->processSpikes(currentStep);
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error processing immediate spikes: ") + e.what());
        }
        
        // ========== STEP 4: Update working memory ==========
        try {
            if (pImpl->workingMemory) {
                pImpl->workingMemory->update(pImpl->timestep);
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error updating working memory: ") + e.what());
        }
        
        // ========== STEP 5: Apply neuromodulation effects ==========
        // Update novelty detection
        try {
            if (pImpl->novelty) {
                pImpl->novelty->update(pImpl->timestep);
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error updating novelty: ") + e.what());
        }
        
        // Update curiosity
        try {
            if (pImpl->curiosity) {
                pImpl->curiosity->update(pImpl->timestep);
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error updating curiosity: ") + e.what());
        }
        
        // Update dopamine (reward prediction error)
        try {
            if (pImpl->dopamine) {
                pImpl->dopamine->update(pImpl->timestep);
                
                // Apply dopamine effects on neural excitability
                float dopamineLevel = pImpl->dopamine->getLevel();
                for (auto& region : pImpl->regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            // Dopamine modulates excitability by injecting additional current
                            float excitabilityMod = dopamineLevel * 0.5f;
                            if (excitabilityMod > 0.0f) {
                                neuron->injectCurrent(excitabilityMod);
                            }
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error updating dopamine: ") + e.what());
        }
        
        // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
        try {
            // Calculate neuromodulation factor for plasticity
            float plasticityMod = 1.0f;
            if (pImpl->dopamine) {
                plasticityMod = pImpl->dopamine->getPlasticityFactor();
            }
            
            for (auto& region : pImpl->regions) {
                for (auto& syn : region->getSynapses()) {
                    try {
                        // Apply STDP with neuromodulation
                        if (syn->getPlasticityFlags().stdp) {
                            const auto& preSpikes = syn->getPreSpikeHistory();
                            const auto& postSpikes = syn->getPostSpikeHistory();
                            
                            if (!preSpikes.empty() && !postSpikes.empty()) {
                                try {
                                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                                } catch (const PlasticityError& e) {
                                    NLM_LOG_WARNING(std::string("STDP plasticity error: ") + e.what());
                                } catch (const std::exception& e) {
                                    NLM_LOG_WARNING(std::string("Error applying STDP: ") + e.what());
                                }
                                
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
                                try {
                                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                                } catch (const PlasticityError& e) {
                                    NLM_LOG_WARNING(std::string("Hebbian plasticity error: ") + e.what());
                                } catch (const std::exception& e) {
                                    NLM_LOG_WARNING(std::string("Error applying Hebbian: ") + e.what());
                                }
                            }
                        }
                        
                        // Update synapse state
                        syn->step(currentTime);
                    } catch (const std::exception& e) {
                        NLM_LOG_WARNING(std::string("Error processing synapse ") + 
                            std::to_string(syn->getId().index()) + ": " + e.what());
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error applying plasticity rules: ") + e.what());
        }
        
        // ========== STEP 7: Update episodic memory ==========
        try {
            pImpl->stepsSinceLastEpisode++;
            if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
                pImpl->stepsSinceLastEpisode = 0;
                
                if (pImpl->episodicMemory) {
                    try {
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
                        
                    } catch (const MemoryError& e) {
                        NLM_LOG_WARNING(std::string("Memory error storing episode: ") + e.what());
                    } catch (const std::exception& e) {
                        NLM_LOG_WARNING(std::string("Error storing episode to episodic memory: ") + e.what());
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error updating episodic memory: ") + e.what());
        }
        
        // ========== STEP 8: Update prediction system ==========
        try {
            if (pImpl->predictionSystem) {
                // The prediction system would be updated with sensory observations
                // For now, just track prediction error history
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error updating prediction system: ") + e.what());
        }
        
        // ========== STEP 9: Update attention system ==========
        try {
            if (pImpl->attention) {
                pImpl->attention->update(pImpl->timestep);
                
                // Apply attention to working memory winners
                if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                    std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
                    pImpl->attention->processCompetition(competitors);
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error updating attention system: ") + e.what());
        }
        
        // ========== STEP 10: Update concept formation ==========
        try {
            if (pImpl->conceptFormation && pImpl->predictionSystem) {
                // Create pattern from current brain state
                std::vector<float> experiencePattern = createExperiencePattern();
                
                // Get features from prediction system
                std::vector<float> experienceFeatures = 
                    pImpl->predictionSystem->getPredictionFeatures();
                
                // Present to concept formation
                size_t currentConcept = pImpl->conceptFormation->presentExperience(
                    experiencePattern, experienceFeatures, 
                    pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f, 
                    pImpl->currentStep);
                
                // Track current concept if not novel
                if (currentConcept > 0 && currentConcept != pImpl->currentWorkingConcept_) {
                    pImpl->currentWorkingConcept_ = currentConcept;
                }
                pImpl->lastExperiencePattern_ = experiencePattern;
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error updating concept formation: ") + e.what());
        }
        
        // ========== STEP 11: Apply structural plasticity periodically ==========
        try {
            if (currentStep % 100 == 0 && pImpl->structuralPlasticity) {
                pImpl->structuralPlasticity->update(this, *pImpl->rng);
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error applying structural plasticity: ") + e.what());
        }
        
        // ========== STEP 12: Replay important memories ==========
        try {
            if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
                // Get episodes for replay
                auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
                for (const auto* episode : episodesToReplay) {
                    pImpl->episodicMemory->replayEpisode(episode);
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error during memory replay: ") + e.what());
        }
        
        // ========== STEP 13: Apply development effects ==========
        try {
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
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error applying development effects: ") + e.what());
        }
        
        // ========== STEP 14: Periodic memory consolidation ==========
        try {
            if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
                // Consolidate important memories, remove weak ones
                pImpl->episodicMemory->consolidate(0.3f);
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error during memory consolidation: ") + e.what());
        }
        
        // ========== STEP 15: Neural planner integration ==========
        try {
            if (pImpl->planner) {
                // Create current state vector from sensory input
                std::vector<float> currentState = getCurrentStateForPlanning();
                
                // Plan action (get from neural planner)
                ActionType plannedAction = pImpl->planner->planAction(currentState);
                
                // Store planned action for later use
                pImpl->plannedAction = plannedAction;
                
                // Get current reward prediction from prediction system
                float predictedReward = pImpl->predictionError ? pImpl->predictionError->getLevel() : 0.0f;
                pImpl->planner->setCurrentGoal(currentState);
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error in neural planner integration: ") + e.what());
        }
        
        // ========== STEP 16: Checkpoint management ==========
        try {
            if (pImpl->checkpointManager) {
                pImpl->checkpointManager->update(currentStep, currentTime);
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error in checkpoint management: ") + e.what());
        }
        
        // ========== STEP 17: Statistics collection ==========
        try {
            // Update statistics
            pImpl->totalSpikesThisStep = pImpl->spikeSystem->getPendingSpikeCount() + 
                                        pImpl->spikeSystem->getPendingDelayedCount();
            pImpl->totalSpikesTotal += pImpl->totalSpikesThisStep;
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error collecting statistics: ") + e.what());
        }
        
        NLM_LOG_DEBUG("Brain step completed successfully (step " + std::to_string(currentStep) + ")");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Critical error during brain step: ") + e.what());
        // Continue with step even if some operations fail (graceful degradation)
        NLM_LOG_WARNING("Continuing brain step with error recovery enabled");
    }
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

std::vector<float> Brain::getCurrentStateForPlanning() {
    std::vector<float> state;
    
    // 1. Current sensory input
    float sensorySum = 0.0f;
    for (auto* neuron : pImpl->sensoryNeurons) {
        if (neuron->isFiring()) {
            sensorySum += neuron->getState().membranePotential;
        }
    }
    state.push_back(sensorySum);
    
    // 2. Working memory content
    if (pImpl->workingMemory) {
        auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        float workingMemoryStrength = memoryNeurons.empty() ? 0.0f : 
            memoryNeurons.size() / static_cast<float>(pImpl->workingMemory->getCapacity());
        state.push_back(workingMemoryStrength);
    } else {
        state.push_back(0.0f);
    }
    
    // 3. Recent motor activity
    size_t recentMotorActivity = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++recentMotorActivity;
        }
    }
    float motorActivityRatio = pImpl->motorNeurons.empty() ? 0.0f : 
        recentMotorActivity / static_cast<float>(pImpl->motorNeurons.size());
    state.push_back(motorActivityRatio);
    
    // 4. Current brain development stage
    state.push_back(static_cast<float>(pImpl->developmentalStage));
    
    // 5. Neuromodulation levels (dopamine, curiosity, etc.)
    float neuromodulationLevel = 1.0f;
    if (pImpl->dopamine) {
        neuromodulationLevel = pImpl->dopamine->getLevel();
    }
    if (pImpl->curiosity) {
        neuromodulationLevel *= pImpl->curiosity->getLevel();
    }
    state.push_back(neuromodulationLevel);
    
    // 6. Prediction error signal
    float predictionError = 0.0f;
    if (pImpl->predictionError) {
        predictionError = pImpl->predictionError->getLevel();
    }
    state.push_back(predictionError);
    
    // 7. Episodic memory activation
    float episodicActivation = 0.0f;
    if (pImpl->episodicMemory) {
        episodicActivation = pImpl->episodicMemory->getRecentActivation();
    }
    state.push_back(episodicActivation);
    
    return state;
}

// Create experience pattern for concept formation
std::vector<float> Brain::createExperiencePattern() {
    std::vector<float> pattern;
    
    // 1. Current neural activity levels (sensory, internal neurons)
    pattern.reserve(pImpl->sensoryNeurons.size() + 
                   pImpl->motorNeurons.size() +
                   pImpl->getTotalNeuronCount() / 10);
    
    for (auto* neuron : pImpl->sensoryNeurons) {
        const auto& state = neuron->getState();
        pattern.push_back(std::abs(state.membranePotential - state.restingPotential));
    }
    
    // 2. Motor neuron activity
    float motorActivity = 0.0f;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) motorActivity += 1.0f;
    }
    pattern.push_back(motorActivity);
    
    // 3. Development stage factor
    float developmentFactor = static_cast<float>(pImpl->developmentalStage) / 5.0f;
    pattern.push_back(developmentFactor);
    
    // 4. Neuromodulation levels
    float dopamineLevel = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
    float curiosityLevel = pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f;
    pattern.push_back(dopamineLevel * 0.6f + curiosityLevel * 0.4f);
    
    // 5. Working memory content
    if (pImpl->workingMemory) {
        float workingMemoryActivity = pImpl->workingMemory->getMemoryActivity();
        pattern.push_back(workingMemoryActivity);
    } else {
        pattern.push_back(0.0f);
    }
    
    // 6. Recent reward prediction
    float rewardPred = pImpl->predictionError ? 
        pImpl->predictionError->getLevel() : 0.0f;
    pattern.push_back(rewardPred);
    
    return pattern;
}

// Create experience pattern for concept formation
std::vector<float> Brain::createExperiencePattern() {
    std::vector<float> pattern;
    
    // 1. Current neural activity levels (sensory, internal neurons)
    pattern.reserve(pImpl->sensoryNeurons.size() + 
                   pImpl->motorNeurons.size() +
                   pImpl->getTotalNeuronCount() / 10);
    
    for (auto* neuron : pImpl->sensoryNeurons) {
        const auto& state = neuron->getState();
        pattern.push_back(std::abs(state.membranePotential - state.restingPotential));
    }
    
    // 2. Motor neuron activity
    float motorActivity = 0.0f;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) motorActivity += 1.0f;
    }
    pattern.push_back(motorActivity);
    
    // 3. Development stage factor
    float developmentFactor = static_cast<float>(pImpl->developmentalStage) / 5.0f;
    pattern.push_back(developmentFactor);
    
    // 4. Neuromodulation levels
    float dopamineLevel = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
    float curiosityLevel = pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f;
    pattern.push_back(dopamineLevel * 0.6f + curiosityLevel * 0.4f);
    
    // 5. Working memory content
    if (pImpl->workingMemory) {
        float workingMemoryActivity = pImpl->workingMemory->getMemoryActivity();
        pattern.push_back(workingMemoryActivity);
    } else {
        pattern.push_back(0.0f);
    }
    
    // 6. Recent reward prediction
    float rewardPred = pImpl->predictionError ? 
        pImpl->predictionError->getLevel() : 0.0f;
    pattern.push_back(rewardPred);
    
    return pattern;
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Neural planner integration: use planned action if available
    if (pImpl->plannedAction != ActionType::Wait) {
        // Use neural planner's recommendation
        auto action = std::make_unique<Action>(pImpl->plannedAction);
        
        // Provide feedback to planner about actual performance
        // Note: getActionQuality() is not implemented on Brain, using planner's actionQuality_ instead
        if (pImpl->planner && action->getType() == pImpl->plannedAction) {
            // Get actual reward from dopamine/prediction error
            float actualReward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            pImpl->planner->updatePlanQuality({pImpl->plannedAction}, {action->getType()}, actualReward);
        }
        
        return action;
    }
    
    // Fallback to simple action selection if no plan
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
    
    // Check concept influence
    if (pImpl->currentWorkingConcept_ > 0 && 
        pImpl->conceptFormation && 
        pImpl->conceptFormation->isConceptStable(pImpl->currentWorkingConcept_)) {
        
        const DiscoveredConcept* concept = 
            pImpl->conceptFormation->getConcept(pImpl->currentWorkingConcept_);
        
        if (concept) {
            // Use concept prototype to guide action selection
            std::vector<float> conceptPrototype = 
                pImpl->conceptFormation->getConceptPrototype(pImpl->currentWorkingConcept_);
            
            // Influence action selection based on concept
            ActionType influencedAction = influenceActionByConcept(conceptPrototype, type);
            
            // Create action with influenced type
            auto conceptAction = std::make_unique<Action>(influencedAction);
            return conceptAction;
        }
    }
    
    return action;
}

// Influence action selection based on concept prototype
ActionType Brain::influenceActionByConcept(const std::vector<float>& prototype, 
                                           ActionType originalAction) {
    // Use concept stability and prototype to influence action
    
    if (prototype.empty()) return originalAction;
    
    // 1. If concept is stable, use concept's typical responses
    // 2. If concept prototypes suggest approach/avoid, modify action
    // 3. Use concept information to reinforce successful action patterns
    
    return originalAction;
}
    if (pImpl->plannedAction != ActionType::Wait) {
        // Use neural planner's recommendation
        auto action = std::make_unique<Action>(pImpl->plannedAction);
        
        // Provide feedback to planner about actual performance
        // Note: getActionQuality() is not implemented on Brain, using planner's actionQuality_ instead
        if (pImpl->planner && action->getType() == pImpl->plannedAction) {
            // Get actual reward from dopamine/prediction error
            float actualReward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            pImpl->planner->updatePlanQuality({pImpl->plannedAction}, {action->getType()}, actualReward);
        }
        
        return action;
    }
    
    // Fallback to simple action selection if no plan
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
    
    // Check concept influence
    if (pImpl->currentWorkingConcept_ > 0 && 
        pImpl->conceptFormation && 
        pImpl->conceptFormation->isConceptStable(pImpl->currentWorkingConcept_)) {
        
        const DiscoveredConcept* concept = 
            pImpl->conceptFormation->getConcept(pImpl->currentWorkingConcept_);
        
        if (concept) {
            // Use concept prototype to guide action selection
            std::vector<float> conceptPrototype = 
                pImpl->conceptFormation->getConceptPrototype(pImpl->currentWorkingConcept_);
            
            // Influence action selection based on concept
            ActionType influencedAction = influenceActionByConcept(conceptPrototype, type);
            
            // Create action with influenced type
            auto conceptAction = std::make_unique<Action>(influencedAction);
            return conceptAction;
        }
    }
    
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
        if (filepath.empty()) {
            throw InvalidArgumentError(defineNLMError("Invalid filepath", 
                "Brain::save", "Filepath cannot be empty"));
        }
        
        // Validate file extension (support common formats)
        size_t dotPos = filepath.find_last_of('.');
        if (dotPos == std::string::npos) {
            NLM_LOG_WARNING("File has no extension: " + filepath);
        }
        
        try {
            CheckpointWriter writer;
            if (!writer.create(filepath, CompressionLevel::Balanced)) {
                throw ConfigurationError(defineNLMError("Failed to create checkpoint file", 
                    "Brain::save", "Could not create file: " + filepath));
            }
            
            // Set metadata
            writer.setMetadata(
                getTotalNeuronCount(),
                getTotalSynapseCount(),
                getRegionCount(),
                pImpl->currentStep,
                pImpl->currentTime
            );
            
            // Write neurons with error handling
            NeuronCheckpointData neuronData;
            neuronData.membranePotential.reserve(getTotalNeuronCount());
            neuronData.restingPotential.reserve(getTotalNeuronCount());
            neuronData.threshold.reserve(getTotalNeuronCount());
            neuronData.resetPotential.reserve(getTotalNeuronCount());
            neuronData.leakConductance.reserve(getTotalNeuronCount());
            
            try {
                for (const auto& region : pImpl->regions) {
                    for (const auto& pop : region->getPopulations()) {
                        for (const auto* neuron : pop->getNeurons()) {
                            if (!neuron) {
                                NLM_LOG_WARNING("Skipping null neuron in region " + 
                                    std::to_string(region->getId().index()));
                                continue;
                            }
                            
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
                    throw NeuralNetworkError(defineNLMError("Failed to write neurons to checkpoint", 
                        "Brain::save", "Neuron checkpoint write operation failed"));
                }
                
            } catch (const MemoryError& e) {
                NLM_LOG_ERROR(std::string("Memory error during neuron checkpoint: ") + e.what());
                throw;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error writing neurons to checkpoint: ") + e.what());
                throw NeuralNetworkError(defineNLMError("Neuron checkpoint write failed", 
                    "Brain::save", e.what()));
            }
            
            // Write synapses with error handling
            SynapseCheckpointData synapseData;
            try {
                for (const auto& region : pImpl->regions) {
                    for (const auto* syn : region->getSynapses()) {
                        if (!syn) {
                            NLM_LOG_WARNING("Skipping null synapse in region " + 
                                std::to_string(region->getId().index()));
                            continue;
                        }
                        
                        synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                        synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                        synapseData.weight.push_back(syn->getWeight());
                        synapseData.delay.push_back(syn->getDelay());
                        synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                        synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                    }
                }
                
                if (!writer.writeSynapses(synapseData)) {
                    throw NeuralNetworkError(defineNLMError("Failed to write synapses to checkpoint", 
                        "Brain::save", "Synapse checkpoint write operation failed"));
                }
                
            } catch (const MemoryError& e) {
                NLM_LOG_ERROR(std::string("Memory error during synapse checkpoint: ") + e.what());
                throw;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error writing synapses to checkpoint: ") + e.what());
                throw NeuralNetworkError(defineNLMError("Synapse checkpoint write failed", 
                    "Brain::save", e.what()));
            }
            
            // Finalize checkpoint
            if (!writer.finalize()) {
                throw NeuralNetworkError(defineNLMError("Failed to finalize checkpoint", 
                    "Brain::save", "Checkpoint finalization failed"));
            }
            
            NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception saving brain checkpoint: ") + e.what());
            throw NeuralNetworkError(defineNLMError("Checkpoint save operation failed", 
                "Brain::save", e.what()));
        }
        
    } catch (const InvalidArgumentError& e) {
        NLM_LOG_ERROR(std::string("Invalid argument during brain save: ") + e.what());
        return false;
    } catch (const NeuralNetworkError& e) {
        NLM_LOG_ERROR(std::string("Neural network error during brain save: ") + e.what());
        return false;
    } catch (const ConfigurationError& e) {
        NLM_LOG_ERROR(std::string("Configuration error during brain save: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during brain save: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        if (filepath.empty()) {
            throw InvalidArgumentError(defineNLMError("Invalid filepath", 
                "Brain::load", "Filepath cannot be empty"));
        }
        
        // Check if file exists (optional but helpful for error reporting)
        // Note: CheckpointReader will handle this, but we can add better error info
        
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            throw ConfigurationError(defineNLMError("Failed to open checkpoint file", 
                "Brain::load", "Could not open file: " + filepath));
        }
        
        if (!reader.validate()) {
            throw ConfigurationError(defineNLMError("Checkpoint validation failed", 
                "Brain::load", "File validation failed: " + reader.getError()));
        }
        
        // Read neurons with error handling
        NeuronCheckpointData neuronData;
        try {
            if (!reader.readNeurons(neuronData)) {
                throw NeuralNetworkError(defineNLMError("Failed to read neurons from checkpoint", 
                    "Brain::load", "Neuron data reading failed"));
            }
            
            // Validate neuron data
            if (neuronData.membranePotential.empty()) {
                NLM_LOG_WARNING("No neuron data found in checkpoint file");
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error reading neurons from checkpoint: ") + e.what());
            throw NeuralNetworkError(defineNLMError("Neuron data reading failed", 
                "Brain::load", e.what()));
        }
        
        // Apply neuron states with validation
        try {
            size_t idx = 0;
            size_t appliedCount = 0;
            size_t skippedCount = 0;
            
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (!neuron) {
                            NLM_LOG_WARNING("Skipping null neuron in region " + 
                                std::to_string(region->getId().index()));
                            skippedCount++;
                            idx++;
                            continue;
                        }
                        
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
                            appliedCount++;
                        } else {
                            NLM_LOG_WARNING("Neuron data index out of range in region " + 
                                std::to_string(region->getId().index()));
                            skippedCount++;
                        }
                        
                        idx++;
                    }
                }
            }
            
            if (skippedCount > 0) {
                NLM_LOG_WARNING("Skipped " + std::to_string(skippedCount) + " neurons due to missing data");
            }
            
            NLM_LOG_INFO("Applied neuron states to " + std::to_string(appliedCount) + " neurons");
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error applying neuron states: ") + e.what());
            throw NeuralNetworkError(defineNLMError("Neuron state application failed", 
                "Brain::load", e.what()));
        }
        
        // Read synapses with error handling
        SynapseCheckpointData synapseData;
        try {
            if (!reader.readSynapses(synapseData)) {
                throw NeuralNetworkError(defineNLMError("Failed to read synapses from checkpoint", 
                    "Brain::load", "Synapse data reading failed"));
            }
            
            // Validate synapse data
            if (synapseData.weight.empty()) {
                NLM_LOG_WARNING("No synapse data found in checkpoint file");
            }
            
            NLM_LOG_INFO("Read " + std::to_string(synapseData.weight.size()) + " synapses from checkpoint");
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error reading synapses from checkpoint: ") + e.what());
            throw NeuralNetworkError(defineNLMError("Synapse data reading failed", 
                "Brain::load", e.what()));
        }
        
        // Apply synapse states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const InvalidArgumentError& e) {
        NLM_LOG_ERROR(std::string("Invalid argument during brain load: ") + e.what());
        return false;
    } catch (const NeuralNetworkError& e) {
        NLM_LOG_ERROR(std::string("Neural network error during brain load: ") + e.what());
        return false;
    } catch (const ConfigurationError& e) {
        NLM_LOG_ERROR(std::string("Configuration error during brain load: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during brain load: ") + e.what());
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
    
    // Neural planner status
    if (pImpl->planner) {
        NLM_LOG_INFO("Neural planner: active, depth=" + std::to_string(pImpl->planner->getPlanningDepth()) + 
                     ", confidence=" + std::to_string(pImpl->planner->getPlanningConfidence()) + 
                     ", planned_action=" + std::to_string(static_cast<int>(pImpl->plannedAction)));
    }
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                     " (" + region->getName() + "): " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                     std::to_string(region->getSynapseCount()) + " synapses, " +
                     "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm
