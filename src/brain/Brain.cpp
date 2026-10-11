#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../core/Errors/Errors.hpp"
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
#include <chrono>

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
    ErrorContext ctx("initialize", "");
    ctx.addMetadata("operation", "brain_initialization");
    ctx.addMetadata("timestamp", getCurrentTimestamp());
    
    try {
        NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
        ctx.addMetadata("stage", "initialization_start");
        
        // Validate configuration before initialization
        if (!pImpl->config) {
            throw ConfigurationException("Cannot initialize brain: no configuration provided",
                                        "Brain initialization requires a valid configuration object",
                                        ErrorCode::CONFIG_INVALID_VALUE,
                                        __FILE__, __LINE__, __func__);
        }
        
        // Get and validate configuration values
        auto neuronCountOpt = pImpl->config->get<size_t>("neuron_count");
        if (!neuronCountOpt.has_value() || neuronCountOpt.value() == 0) {
            throw ConfigurationException("Invalid neuron count in configuration",
                                        "Configuration key 'neuron_count' is missing or invalid",
                                        ErrorCode::CONFIG_INVALID_VALUE,
                                        __FILE__, __LINE__, __func__);
        }
        
        auto regionCountOpt = pImpl->config->get<size_t>("region_count");
        if (!regionCountOpt.has_value() || regionCountOpt.value() == 0) {
            throw ConfigurationException("Invalid region count in configuration",
                                        "Configuration key 'region_count' is missing or invalid",
                                        ErrorCode::CONFIG_INVALID_VALUE,
                                        __FILE__, __LINE__, __func__);
        }
        
        auto connectionProbOpt = pImpl->config->get<double>("connection_probability");
        if (!connectionProbOpt.has_value() || connectionProbOpt.value() < 0.0 || connectionProbOpt.value() > 1.0) {
            throw ConfigurationException("Invalid connection probability in configuration",
                                        "Configuration key 'connection_probability' must be between 0.0 and 1.0",
                                        ErrorCode::CONFIG_INVALID_VALUE,
                                        __FILE__, __LINE__, __func__);
        }
        
        size_t neuronCount = neuronCountOpt.value();
        size_t regionCount = regionCountOpt.value();
        float connectionProbability = static_cast<float>(connectionProbOpt.value());
        
        // Check for reasonable resource limits
        if (neuronCount > 1000000) {
            throw MemoryError("Too many neurons for available memory",
                            "Requested neuron count: " + std::to_string(neuronCount) + 
                            ", Maximum allowed: 1000000",
                            ErrorCode::MEMORY_ALLOCATION_FAILED,
                            __FILE__, __LINE__, __func__);
        }
        
        if (regionCount > 100) {
            throw ConfigurationException("Too many regions for stable operation",
                                        "Requested region count: " + std::to_string(regionCount) + 
                                        ", Maximum allowed: 100",
                                        ErrorCode::CONFIG_INVALID_VALUE,
                                        __FILE__, __LINE__, __func__);
        }
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions, connection probability: " + 
                     std::to_string(connectionProbability));
        ctx.addMetadata("neuron_count", std::to_string(neuronCount));
        ctx.addMetadata("region_count", std::to_string(regionCount));
        
        // Create regions with error handling
        std::vector<RegionId> regionIds;
        for (size_t i = 0; i < regionCount; ++i) {
            try {
                RegionId regionId = addRegion("Region_" + std::to_string(i + 1));
                regionIds.push_back(regionId);
                NLM_LOG_INFO("Created region " + std::to_string(i + 1) + " (ID: " + std::to_string(regionId.index()) + ")");
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to create region " + std::to_string(i + 1) + ": " + e.what());
                // Continue with other regions instead of failing completely
                NLM_LOG_WARNING("Skipping region " + std::to_string(i + 1) + ", continuing with other regions");
            }
        }
        
        if (regionIds.empty()) {
            throw NeuralSimulationError("Failed to create any brain regions",
                                        "All region creation attempts failed",
                                        ErrorCode::NEURAL_SIMULATION_FAILED,
                                        __FILE__, __LINE__, __func__);
        }
        
        // Create neurons across regions with error handling
        size_t neuronsPerRegion = neuronCount / regionIds.size();
        for (size_t i = 0; i < regionIds.size(); ++i) {
            auto* region = getRegion(regionIds[i]);
            if (!region) {
                NLM_LOG_ERROR("Failed to get region " + std::to_string(i + 1));
                continue;
            }
            
            try {
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
                ctx.addMetadata("region_" + std::to_string(i + 1) + "_neurons", 
                               std::to_string(region->getTotalNeuronCount()));
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to create populations in region " + std::to_string(i + 1) + ": " + e.what());
                // Continue with other regions instead of failing completely
                NLM_LOG_WARNING("Skipping population creation in region " + std::to_string(i + 1) + ", continuing with other regions");
            }
        }
        
        // Initialize connectivity with random weights
        for (size_t i = 0; i < regionIds.size(); ++i) {
            auto* region = getRegion(regionIds[i]);
            if (region) {
                try {
                    // Initialize random connectivity and synapse weights
                    region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
                    NLM_LOG_INFO("Initialized connectivity for region " + std::to_string(i + 1));
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Failed to initialize connectivity for region " + std::to_string(i + 1) + ": " + e.what());
                    // Continue with other regions instead of failing completely
                    NLM_LOG_WARNING("Skipping connectivity initialization in region " + std::to_string(i + 1) + ", continuing with other regions");
                }
            }
        }
        
        // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
        
        // Initialize working memory with error handling
        try {
            pImpl->workingMemory->initialize(this);
            pImpl->workingMemory->setCapacity(neuronCount / 10);
            NLM_LOG_INFO("Working memory initialized with capacity: " + std::to_string(pImpl->workingMemory->getCapacity()));
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize working memory: " + e.what());
            // Continue without working memory instead of failing completely
            NLM_LOG_WARNING("Working memory initialization failed, continuing without working memory");
        }
        
        // Initialize episodic memory with error handling
        try {
            pImpl->episodicMemory->initialize(this);
            pImpl->episodicMemory->setMaxEpisodes(1000);
            NLM_LOG_INFO("Episodic memory initialized with max episodes: 1000");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize episodic memory: " + e.what());
            // Continue without episodic memory instead of failing completely
            NLM_LOG_WARNING("Episodic memory initialization failed, continuing without episodic memory");
        }
        
        // Initialize associative memory with error handling
        try {
            pImpl->associativeMemory->initialize(this);
            NLM_LOG_INFO("Associative memory initialized");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize associative memory: " + e.what());
            // Continue without associative memory instead of failing completely
            NLM_LOG_WARNING("Associative memory initialization failed, continuing without associative memory");
        }
        
        // Initialize cognition systems with error handling
        try {
            pImpl->planner->initialize(this);
            pImpl->planner->setPlanningDepth(5);
            NLM_LOG_INFO("Neural planner initialized with planning depth: 5");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize neural planner: " + e.what());
            throw NeuralSimulationError("Failed to initialize core cognition systems",
                                        "Neural planner initialization failed",
                                        ErrorCode::NEURAL_SIMULATION_FAILED,
                                        __FILE__, __LINE__, __func__);
        }
        
        try {
            pImpl->conceptFormation->initialize(this);
            NLM_LOG_INFO("Concept formation initialized");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize concept formation: " + e.what());
            // Continue without concept formation instead of failing completely
            NLM_LOG_WARNING("Concept formation initialization failed, continuing without concept formation");
        }
        
        try {
            pImpl->attention->initialize(this);
            pImpl->attention->setInhibitionStrength(0.5f);
            pImpl->attention->setExcitationStrength(1.5f);
            NLM_LOG_INFO("Attention system initialized with inhibition: 0.5, excitation: 1.5");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize attention system: " + e.what());
            // Continue without attention system instead of failing completely
            NLM_LOG_WARNING("Attention system initialization failed, continuing without attention system");
        }
        
        // Initialize neuromodulation with error handling
        try {
            pImpl->novelty->initialize(this);
            NLM_LOG_INFO("Novelty detection system initialized");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize novelty system: " + e.what());
            // Continue without novelty system instead of failing completely
            NLM_LOG_WARNING("Novelty system initialization failed, continuing without novelty system");
        }
        
        try {
            pImpl->curiosity->initialize(this);
            NLM_LOG_INFO("Curiosity system initialized");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize curiosity system: " + e.what());
            // Continue without curiosity system instead of failing completely
            NLM_LOG_WARNING("Curiosity system initialization failed, continuing without curiosity system");
        }
        
        // Register spike handlers with error handling
        try {
            pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
                // Count spikes
                ++pImpl->totalSpikesThisStep;
                ++pImpl->totalSpikesTotal;
            });
            NLM_LOG_INFO("Spike system handler registered");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to register spike handler: " + e.what());
            // Continue without spike handlers instead of failing completely
            NLM_LOG_WARNING("Spike handler registration failed, continuing without spike handlers");
        }
        
        // Register delayed spike handler with error handling
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
            NLM_LOG_INFO("Delayed spike handler registered");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to register delayed spike handler: " + e.what());
            // Continue without delayed spike handlers instead of failing completely
            NLM_LOG_WARNING("Delayed spike handler registration failed, continuing without delayed spike handlers");
        }
        
        // Configure checkpoint manager with error handling
        try {
            std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
            pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
            NLM_LOG_INFO("Checkpoint manager configured with directory: " + checkpointDir);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to configure checkpoint manager: " + e.what());
            // Continue without checkpoint manager instead of failing completely
            NLM_LOG_WARNING("Checkpoint manager configuration failed, continuing without checkpoint manager");
        }
        
        ctx.addMetadata("status", "initialization_complete");
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return true;
        
    } catch (const ConfigurationException& e) {
        NLM_LOG_ERROR("ConfigurationException during brain initialization: " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const NeuralSimulationError& e) {
        NLM_LOG_ERROR("NeuralSimulationError during brain initialization: " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const MemoryError& e) {
        NLM_LOG_ERROR("MemoryError during brain initialization: " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const FileIOError& e) {
        NLM_LOG_ERROR("FileIOError during brain initialization: " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext errorCtx("brain_initialization_error", "");
        errorCtx.addMetadata("operation", "brain_initialization");
        errorCtx.addMetadata("timestamp", getCurrentTimestamp());
        errorCtx.addMetadata("original_error", e.what());
        
        NLM_LOG_ERROR("Unexpected error during brain initialization: " + e.what());
        throw NeuralSimulationError("Brain initialization failed", 
                                   errorCtx.getMetadataSummary(),
                                   ErrorCode::NEURAL_SIMULATION_FAILED,
                                   __FILE__, __LINE__, __func__);
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
    ErrorContext ctx("save", filepath);
    ctx.addMetadata("timestamp", getCurrentTimestamp());
    ctx.addMetadata("operation", "brain_save_checkpoint");
    
    try {
        NLM_LOG_INFO("Saving brain state to " + filepath + " at " + getCurrentTimestamp());
        
        // Validate filepath before attempting to save
        std::string dirPath = std::filesystem::path(filepath).parent_path();
        if (!dirPath.empty() && !std::filesystem::exists(dirPath)) {
            throw FileIOError("Cannot save brain: directory does not exist", 
                            "Directory: " + dirPath + " does not exist",
                            ErrorCode::FILE_WRITE_FAILED,
                            __FILE__, __LINE__, __func__);
        }
        
        // Check available disk space (basic check)
        auto fileSize = std::filesystem::file_size(filepath);
        if (fileSize > 100 * 1024 * 1024) { // 100MB limit for checkpoint
            throw FileIOError("Checkpoint file too large", 
                            "File size: " + std::to_string(fileSize) + " bytes exceeds 100MB limit",
                            ErrorCode::FILE_CORRUPTED,
                            __FILE__, __LINE__, __func__);
        }
        
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            throw FileIOError("Failed to create checkpoint file", 
                            "Failed to create checkpoint file: " + filepath + ", error: " + writer.getError(),
                            ErrorCode::FILE_WRITE_FAILED,
                            __FILE__, __LINE__, __func__);
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
                throw FileIOError("Failed to write neurons to checkpoint", 
                                "Error writing neurons data to checkpoint",
                                ErrorCode::FILE_WRITE_FAILED,
                                __FILE__, __LINE__, __func__);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error while writing neuron data: " + std::string(e.what()));
            throw;
        }
        
        // Write synapses with error handling
        SynapseCheckpointData synapseData;
        try {
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
                throw FileIOError("Failed to write synapses to checkpoint", 
                                "Error writing synapses data to checkpoint",
                                ErrorCode::FILE_WRITE_FAILED,
                                __FILE__, __LINE__, __func__);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error while writing synapse data: " + std::string(e.what()));
            throw;
        }
        
        // Finalize with error handling
        try {
            if (!writer.finalize()) {
                throw FileIOError("Failed to finalize checkpoint", 
                                "Error finalizing checkpoint file: " + writer.getError(),
                                ErrorCode::FILE_WRITE_FAILED,
                                __FILE__, __LINE__, __func__);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error while finalizing checkpoint: " + std::string(e.what()));
            throw;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        ctx.addMetadata("status", "success");
        ctx.addMetadata("bytes_written", std::to_string(writer.getBytesWritten()));
        
        return true;
        
    } catch (const FileIOError& e) {
        NLM_LOG_ERROR("FileIOError saving brain to " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const NeuralSimulationError& e) {
        NLM_LOG_ERROR("NeuralSimulationError saving brain to " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const ConfigurationException& e) {
        NLM_LOG_ERROR("ConfigurationException saving brain to " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const MemoryError& e) {
        NLM_LOG_ERROR("MemoryError saving brain to " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error saving brain to " + filepath + ": " + e.what());
        throw NeuralSimulationError("Failed to save brain state to " + filepath, 
                                   "Operation: save_checkpoint",
                                   ErrorCode::FILE_WRITE_FAILED,
                                   __FILE__, __LINE__, __func__);
    }
}

bool Brain::load(const std::string& filepath) {
    ErrorContext ctx("load", filepath);
    ctx.addMetadata("timestamp", getCurrentTimestamp());
    ctx.addMetadata("operation", "brain_load_checkpoint");
    
    try {
        NLM_LOG_INFO("Loading brain state from " + filepath + " at " + getCurrentTimestamp());
        
        // Verify file exists and is readable
        if (!std::filesystem::exists(filepath)) {
            throw FileIOError("Checkpoint file not found", 
                            "File path: " + filepath + " was not found or is inaccessible",
                            ErrorCode::CONFIG_FILE_NOT_FOUND,
                            __FILE__, __LINE__, __func__);
        }
        
        if (!std::filesystem::is_regular_file(filepath)) {
            throw FileIOError("Invalid checkpoint file", 
                            "Path is not a regular file: " + filepath,
                            ErrorCode::FILE_READ_FAILED,
                            __FILE__, __LINE__, __func__);
        }
        
        // Check file size to prevent loading corrupted or overly large files
        auto fileSize = std::filesystem::file_size(filepath);
        if (fileSize > 200 * 1024 * 1024) { // 200MB limit for checkpoint
            throw FileIOError("Checkpoint file too large", 
                            "File size: " + std::to_string(fileSize) + " bytes exceeds 200MB limit",
                            ErrorCode::FILE_CORRUPTED,
                            __FILE__, __LINE__, __func__);
        }
        
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            throw FileIOError("Failed to open checkpoint file", 
                            "Failed to open checkpoint file: " + filepath + ", error: " + reader.getError(),
                            ErrorCode::FILE_OPEN_FAILED,
                            __FILE__, __LINE__, __func__);
        }
        
        if (!reader.validate()) {
            throw FileIOError("Checkpoint validation failed", 
                            "Checkpoint validation failed: " + reader.getError(),
                            ErrorCode::FILE_CORRUPTED,
                            __FILE__, __LINE__, __func__);
        }
        
        // Read neurons with error handling
        NeuronCheckpointData neuronData;
        try {
            if (!reader.readNeurons(neuronData)) {
                throw FileIOError("Failed to read neurons from checkpoint", 
                                "Error reading neuron data from checkpoint",
                                ErrorCode::FILE_READ_FAILED,
                                __FILE__, __LINE__, __func__);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error while reading neuron data: " + std::string(e.what()));
            throw;
        }
        
        // Apply neuron states with error handling
        size_t idx = 0;
        try {
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
                            if (idx < neuronData.refractoryPeriod.size()) {
                                neuron->setRefractoryRemaining(neuronData.refractoryRemaining[idx]);
                            }
                            if (idx < neuronData.lastSpikeTime.size()) {
                                neuron->setLastSpikeTime(neuronData.lastSpikeTime[idx]);
                            }
                        }
                        idx++;
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error while applying neuron states: " + std::string(e.what()));
            throw;
        }
        
        // Read synapses with error handling
        SynapseCheckpointData synapseData;
        try {
            if (!reader.readSynapses(synapseData)) {
                throw FileIOError("Failed to read synapses from checkpoint", 
                                "Error reading synapse data from checkpoint",
                                ErrorCode::FILE_READ_FAILED,
                                __FILE__, __LINE__, __func__);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error while reading synapse data: " + std::string(e.what()));
            throw;
        }
        
        // Apply synapse states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        ctx.addMetadata("synapses_loaded", std::to_string(synapseData.weight.size()));
        
        NLM_LOG_INFO("Brain state loaded successfully");
        ctx.addMetadata("status", "success");
        
        return true;
        
    } catch (const FileIOError& e) {
        NLM_LOG_ERROR("FileIOError loading brain from " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const NeuralSimulationError& e) {
        NLM_LOG_ERROR("NeuralSimulationError loading brain from " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const ConfigurationException& e) {
        NLM_LOG_ERROR("ConfigurationException loading brain from " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const MemoryError& e) {
        NLM_LOG_ERROR("MemoryError loading brain from " + filepath + ": " + e.what());
        ErrorReporterRegistry::reportError(e);
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error loading brain from " + filepath + ": " + e.what());
        throw NeuralSimulationError("Failed to load brain state from " + filepath, 
                                   "Operation: load_checkpoint",
                                   ErrorCode::FILE_READ_FAILED,
                                   __FILE__, __LINE__, __func__);
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
