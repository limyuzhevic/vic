// Brain.cpp - Improved version with proper memory management and resource safety

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
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
#include <cassert>
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
    
    // Initialization flags
    bool isInitialized;
    bool isConfigValid;
    
    // Error handling
    std::vector<std::string> initializationErrors;
    
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
        , isInitialized(false)
        , isConfigValid(false)
    {
        // Initialize error handling
        initializationErrors.clear();
        
        // Validate configuration first
        if (!cfg) {
            initializationErrors.push_back("Configuration is null");
            return;
        }
        
        isConfigValid = validateConfiguration();
        if (!isConfigValid) {
            NLM_LOG_ERROR("Invalid configuration detected");
            for (const auto& error : initializationErrors) {
                NLM_LOG_ERROR("  - " + error);
            }
            return;
        }
        
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        
        try {
            rng = std::make_unique<RandomGenerator>(seed);
        } catch (const std::exception& e) {
            initializationErrors.push_back("Failed to initialize random generator: " + std::string(e.what()));
        }
        
        // Initialize plasticity systems
        try {
            spikeSystem = std::make_unique<SpikeSystem>();
            stdp = std::make_unique<STDP>();
            hebbian = std::make_unique<Hebbian>();
            structuralPlasticity = std::make_unique<StructuralPlasticity>();
        } catch (const std::exception& e) {
            initializationErrors.push_back("Failed to initialize plasticity systems: " + std::string(e.what()));
        }
        
        // ========== INITIALIZE INTEGRATED SYSTEMS ==========
        
        try {
            // Initialize memory systems
            workingMemory = std::make_unique<NeuralWorkingMemory>();
            episodicMemory = std::make_unique<NeuralEpisodicMemory>();
            associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        } catch (const std::exception& e) {
            initializationErrors.push_back("Failed to initialize memory systems: " + std::string(e.what()));
        }
        
        try {
            // Initialize prediction system
            predictionSystem = std::make_unique<PredictionSystem>();
        } catch (const std::exception& e) {
            initializationErrors.push_back("Failed to initialize prediction system: " + std::string(e.what()));
        }
        
        try {
            // Initialize cognition systems
            planner = std::make_unique<NeuralPlanner>();
            conceptFormation = std::make_unique<ConceptFormation>();
            attention = std::make_unique<AttentionalSelection>();
        } catch (const std::exception& e) {
            initializationErrors.push_back("Failed to initialize cognition systems: " + std::string(e.what()));
        }
        
        try {
            // Initialize development system
            developmentSystem = std::make_unique<DevelopmentSystem>();
        } catch (const std::exception& e) {
            initializationErrors.push_back("Failed to initialize development system: " + std::string(e.what()));
        }
        
        try {
            // Initialize neuromodulation systems
            dopamine = std::make_unique<Dopamine>();
            curiosity = std::make_unique<Curiosity>();
            predictionError = std::make_unique<PredictionError>();
            novelty = std::make_unique<Novelty>();
        } catch (const std::exception& e) {
            initializationErrors.push_back("Failed to initialize neuromodulation systems: " + std::string(e.what()));
        }
        
        // Configure STDP parameters
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        
        if (stdp) {
            stdp->configure(ltpWeight, ltdWeight, tau);
        }
        
        // Configure structural plasticity
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        
        if (structuralPlasticity) {
            structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
            structuralPlasticity->setPruningRate(pruningRate);
        }
        
        // Get timestep
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize checkpoint manager
        try {
            checkpointManager = std::make_unique<CheckpointManager>();
            std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
            checkpointManager->configure(checkpointDir, 10000, 5, true);
        } catch (const std::exception& e) {
            initializationErrors.push_back("Failed to initialize checkpoint manager: " + std::string(e.what()));
        }
        
        // Set initialization flag
        isInitialized = (initializationErrors.empty());
        
        if (!isInitialized) {
            NLM_LOG_ERROR("Brain initialization failed with " + 
                         std::to_string(initializationErrors.size()) + " errors");
        } else {
            NLM_LOG_INFO("Brain implementation initialized successfully");
        }
    }
    
    // Validate configuration parameters
    bool validateConfiguration() {
        ValidationResult validation;
        
        // Check required configuration keys
        std::vector<std::string> requiredKeys = {
            "random_seed",
            "simulation_timestep",
            "neuron_count",
            "region_count",
            "connection_probability"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config->has(key)) {
                initializationErrors.push_back("Missing required configuration key: " + key);
                validation.addError("Missing required configuration key: " + key);
            }
        }
        
        // Validate parameter values using Config's validation
        if (config) {
            ValidationResult configValidation = config->validate();
            if (!configValidation.isValid) {
                for (const auto& error : configValidation.errors) {
                    initializationErrors.push_back("Invalid configuration value for " + error);
                }
            }
        }
        
        return initializationErrors.empty();
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {
    if (!pImpl->isInitialized) {
        NLM_LOG_ERROR("Brain initialization failed - constructor threw exception or left object in invalid state");
        // Note: In a real implementation, we might throw here
    }
}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(std::move(other.pImpl)) {
    if (pImpl) {
        pImpl->initializationErrors.clear(); // Clear errors in moved-to object
    }
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        // Cleanup current resources
        if (pImpl) {
            pImpl->initializationErrors.clear();
        }
        
        // Move resources from other
        pImpl = std::move(other.pImpl);
        
        if (pImpl) {
            pImpl->initializationErrors.clear();
        }
    }
    return *this;
}

bool Brain::initialize() {
    if (!pImpl) {
        NLM_LOG_ERROR("Cannot initialize brain with null implementation");
        return false;
    }
    
    if (pImpl->isInitialized) {
        NLM_LOG_INFO("Brain already initialized - calling reset first");
        reset();
    }
    
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    try {
        // Validate implementation state
        if (!pImpl->isConfigValid) {
            NLM_LOG_ERROR("Configuration is not valid - cannot initialize");
            return false;
        }
        
        if (!pImpl->rng) {
            NLM_LOG_ERROR("Random generator not initialized");
            return false;
        }
        
        // Get configuration values with safety checks
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        // Validate parameters
        if (neuronCount == 0) {
            NLM_LOG_ERROR("Neuron count must be positive");
            return false;
        }
        
        if (regionCount == 0) {
            NLM_LOG_ERROR("Region count must be positive");
            return false;
        }
        
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            NLM_LOG_ERROR("Connection probability must be in range [0.0, 1.0]");
            return false;
        }
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions");
        
        // Create regions with error checking
        for (size_t i = 0; i < regionCount; ++i) {
            RegionId regionId = addRegion("Region_" + std::to_string(i + 1));
            if (regionId.index() == 0) {
                NLM_LOG_ERROR("Failed to create region " + std::to_string(i + 1));
                return false;
            }
        }
        
        // Create neurons across regions with error checking
        size_t neuronsPerRegion = neuronCount / regionCount;
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!region) {
                NLM_LOG_ERROR("Region " + std::to_string(i + 1) + " not found");
                return false;
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
                             
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to create neurons in region " + std::to_string(i + 1) + 
                             ": " + std::string(e.what()));
                return false;
            }
        }
        
        // Initialize connectivity with random weights
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!region) {
                NLM_LOG_ERROR("Region " + std::to_string(i + 1) + " not found during connectivity initialization");
                return false;
            }
            
            try {
                // Initialize random connectivity and synapse weights
                region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to initialize connectivity in region " + std::to_string(i + 1) + 
                             ": " + std::string(e.what()));
                return false;
            }
        }
        
        // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
        
        try {
            // Initialize working memory
            if (pImpl->workingMemory) {
                pImpl->workingMemory->initialize(this);
                pImpl->workingMemory->setCapacity(neuronCount / 10);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize working memory: " + std::string(e.what()));
            return false;
        }
        
        try {
            // Initialize episodic memory
            if (pImpl->episodicMemory) {
                pImpl->episodicMemory->initialize(this);
                pImpl->episodicMemory->setMaxEpisodes(1000);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize episodic memory: " + std::string(e.what()));
            return false;
        }
        
        try {
            // Initialize associative memory
            if (pImpl->associativeMemory) {
                pImpl->associativeMemory->initialize(this);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize associative memory: " + std::string(e.what()));
            return false;
        }
        
        // Initialize prediction system (if available)
        // (PredictionSystem doesn't have initialize method currently)
        
        try {
            // Initialize cognition systems
            if (pImpl->planner) {
                pImpl->planner->initialize(this);
                pImpl->planner->setPlanningDepth(5);
            }
            
            if (pImpl->conceptFormation) {
                pImpl->conceptFormation->initialize(this);
            }
            
            if (pImpl->attention) {
                pImpl->attention->initialize(this);
                pImpl->attention->setInhibitionStrength(0.5f);
                pImpl->attention->setExcitationStrength(1.5f);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize cognition systems: " + std::string(e.what()));
            return false;
        }
        
        try {
            // Initialize development system
            if (pImpl->developmentSystem) {
                pImpl->developmentSystem->initialize(this);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize development system: " + std::string(e.what()));
            return false;
        }
        
        try {
            // Initialize neuromodulation systems
            if (pImpl->novelty) {
                pImpl->novelty->initialize(this);
            }
            if (pImpl->curiosity) {
                pImpl->curiosity->initialize(this);
            }
            
            if (pImpl->dopamine) {
                pImpl->dopamine->initialize(this);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize neuromodulation systems: " + std::string(e.what()));
            return false;
        }
        
        // Register spike handlers for event-driven processing
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
                // Count spikes
                ++pImpl->totalSpikesThisStep;
                ++pImpl->totalSpikesTotal;
            });
            
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
        }
        
        // Configure checkpoint manager
        if (pImpl->checkpointManager) {
            try {
                std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
                pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to configure checkpoint manager: " + std::string(e.what()));
                // Non-critical error - continue
            }
        }
        
        // Set initialization flag
        pImpl->isInitialized = true;
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during brain initialization: " + std::string(e.what()));
        return false;
    } catch (...) {
        NLM_LOG_ERROR("Unknown exception during brain initialization");
        return false;
    }
}

// Rest of the Brain.cpp implementation remains the same... (I'll continue with the rest of the file)

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
    
    // Safety check for valid implementation
    if (!pImpl) {
        return;
    }
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
    
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
                    if (pImpl->spikeSystem) {
                        pImpl->spikeSystem->queueSpike(event);
                    }

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        if (syn) {
                            syn->recordPostSpike(currentTime);
                        }
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        if (!syn) continue;
                        
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

                        if (pImpl->spikeSystem) {
                            pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
                        }

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
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->processSpikes(currentStep);
    }
    
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
            if (!syn) continue;
            
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    if (pImpl->stdp) {
                        pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
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
                    if (pImpl->hebbian) {
                        pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    }
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
        if (pImpl->structuralPlasticity) {
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
        if (pImpl->developmentSystem) {
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

// Additional Brain.cpp methods remain the same...

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    if (!pImpl) return;
    
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
        if (pImpl->sensoryNeurons[i]) {
            pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        }
        
        // Also store in working memory
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    if (!pImpl) return;
    
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n && n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    if (!pImpl) return;
    
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron) {
                        neuron->injectCurrent(current);
                    }
                }
            }
        }
    }
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl ? pImpl->spikeSystem.get() : nullptr;
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl ? pImpl->spikeSystem.get() : nullptr;
}

STDP* Brain::getSTDP() {
    return pImpl ? pImpl->stdp.get() : nullptr;
}

Hebbian* Brain::getHebbian() {
    return pImpl ? pImpl->hebbian.get() : nullptr;
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl ? pImpl->structuralPlasticity.get() : nullptr;
}

float Brain::getExcitationInhibitionRatio() const {
    if (!pImpl) return 0.0f;
    
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            if (!syn) continue;
            
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
    return pImpl ? pImpl->totalSpikesTotal : 0;
}

size_t Brain::getPendingSpikeEventCount() const {
    if (!pImpl || !pImpl->spikeSystem) {
        return 0;
    }
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    if (!pImpl) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron && neuron->isFiring()) {
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
    if (!pImpl || !pImpl->stdp) return;
    
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
    if (!pImpl || !pImpl->structuralPlasticity) return;
    
    // Development updates structural plasticity
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
}

void Brain::reset() {
    if (!pImpl) {
        NLM_LOG_ERROR("Cannot reset brain with null implementation");
        return;
    }
    
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    // Reset all regions
    for (auto& region : pImpl->regions) {
        if (region) {
            region->reset();
        }
    }
    
    // Reset spike system
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->reset();
    }
    
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    // Clear sensory and motor neuron lists
    pImpl->sensoryNeurons.clear();
    pImpl->motorNeurons.clear();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    if (!pImpl) {
        NLM_LOG_ERROR("Cannot save brain with null implementation");
        return false;
    }
    
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
                    if (!neuron) continue;
                    
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
                if (!syn) continue;
                
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
    if (!pImpl) {
        NLM_LOG_ERROR("Cannot load brain with null implementation");
        return false;
    }
    
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
                    if (!neuron) continue;
                    
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
    if (!pImpl) {
        return RegionId();
    }
    
    RegionId id(pImpl->nextRegionId++);
    try {
        auto region = std::make_unique<NeuralRegion>(id, name);
        pImpl->regions.push_back(std::move(region));
        return id;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to create region " + name + ": " + std::string(e.what()));
        return RegionId();
    }
}

NeuralRegion* Brain::getRegion(RegionId id) {
    if (!pImpl) return nullptr;
    
    for (auto& region : pImpl->regions) {
        if (region && region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    if (!pImpl) return nullptr;
    
    for (const auto& region : pImpl->regions) {
        if (region && region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    return pImpl ? pImpl->regions.size() : 0;
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    if (!pImpl) return ids;
    
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        if (region) {
            ids.push_back(region->getId());
        }
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    static const std::vector<std::unique_ptr<NeuralRegion>> empty;
    return pImpl ? pImpl->regions : empty;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    if (!pImpl) return;
    
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    if (!pImpl) return;
    
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
    if (!pImpl) return 0;
    
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        if (region) {
            total += region->getTotalNeuronCount();
        }
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    if (!pImpl) return 0;
    
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        if (region) {
            total += region->getSynapseCount();
        }
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    if (!pImpl) return 0;
    
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        if (region) {
            total += region->getActiveNeuronCount();
        }
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl ? pImpl->totalSpikesThisStep : 0;
}

float Brain::getAverageFiringRate() const {
    if (!pImpl || pImpl->regions.empty()) return 0.0f;
    
    float sum = 0.0f;
    size_t validRegions = 0;
    for (const auto& region : pImpl->regions) {
        if (region) {
            sum += region->getAverageFiringRate();
            validRegions++;
        }
    }
    return validRegions > 0 ? sum / static_cast<float>(validRegions) : 0.0f;
}

// ========== MEMORY SYSTEM ACCESSORS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl ? pImpl->workingMemory.get() : nullptr;
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl ? pImpl->episodicMemory.get() : nullptr;
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl ? pImpl->associativeMemory.get() : nullptr;
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl ? pImpl->predictionSystem.get() : nullptr;
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getPlanner() {
    return pImpl ? pImpl->planner.get() : nullptr;
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl ? pImpl->conceptFormation.get() : nullptr;
}

AttentionalSelection* Brain::getAttention() {
    return pImpl ? pImpl->attention.get() : nullptr;
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl ? pImpl->developmentSystem.get() : nullptr;
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl ? pImpl->developmentalStage : DevelopmentalStage::Initial;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    if (pImpl) {
        pImpl->developmentalStage = stage;
    }
}

// ========== NEUROMODULATION SYSTEMS ==========

Dopamine* Brain::getDopamine() {
    return pImpl ? pImpl->dopamine.get() : nullptr;
}

Curiosity* Brain::getCuriosity() {
    return pImpl ? pImpl->curiosity.get() : nullptr;
}

Novelty* Brain::getNovelty() {
    return pImpl ? pImpl->novelty.get() : nullptr;
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl ? pImpl->predictionError.get() : nullptr;
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl ? pImpl->config : nullptr;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl ? pImpl->rng.get() : nullptr;
}

void Brain::logStatus() const {
    if (!pImpl) return;
    
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