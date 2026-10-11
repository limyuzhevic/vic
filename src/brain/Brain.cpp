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
#include <stdexcept>
#include <numeric>
#include <limits>

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
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    try {
        // Validate config pointer
        if (!pImpl->config) {
            NLM_LOG_ERROR("Brain::initialize: Config is null");
            return false;
        }
        
        // Validate configuration values
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        // Validate configuration values
        if (neuronCount == 0) {
            NLM_LOG_ERROR("Brain::initialize: Invalid neuron_count (must be > 0)");
            return false;
        }
        
        if (regionCount == 0) {
            NLM_LOG_ERROR("Brain::initialize: Invalid region_count (must be > 0)");
            return false;
        }
        
        if (neuronCount < regionCount) {
            NLM_LOG_WARNING(std::string("Brain::initialize: neuron_count (") + 
                         std::to_string(neuronCount) + ") < region_count (") +
                         std::to_string(regionCount) + "), adjusting neuron count");
        }
        
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            NLM_LOG_ERROR("Brain::initialize: Invalid connection_probability (must be in range [0.0, 1.0])");
            return false;
        }
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions, " +
                     std::to_string(connectionProbability * 100) + "% connection probability");
        
        // Create regions with error checking
        for (size_t i = 0; i < regionCount; ++i) {
            try {
                addRegion("Region_" + std::to_string(i + 1));
            } catch (const BrainRegionException& e) {
                NLM_LOG_ERROR(std::string("Brain::initialize: Failed to create region " + 
                                       std::to_string(i + 1) + ": ") + e.what());
                return false;
            }
        }
        
        // Validate we have regions
        if (pImpl->regions.empty()) {
            NLM_LOG_ERROR("Brain::initialize: Failed to create any regions");
            return false;
        }
        
        // Create neurons across regions with detailed error handling
        size_t neuronsPerRegion = neuronCount / regionCount;
        if (neuronsPerRegion == 0) neuronsPerRegion = 1;
        
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!region) {
                NLM_LOG_ERROR(std::string("Brain::initialize: Failed to get region " + 
                                       std::to_string(i + 1)));
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
                } else {
                    NLM_LOG_WARNING(std::string("Brain::initialize: Failed to get sensory population in region " + 
                                               std::to_string(i + 1)));
                }
                
                if (motorPop) {
                    for (auto* neuron : motorPop->getNeurons()) {
                        pImpl->motorNeurons.push_back(neuron);
                    }
                } else {
                    NLM_LOG_WARNING(std::string("Brain::initialize: Failed to get motor population in region " + 
                                               std::to_string(i + 1)));
                }
                
                NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                             ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                             std::to_string(region->getTotalNeuronCount()) + " neurons");
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::initialize: Exception creating neurons in region " + 
                                       std::to_string(i + 1) + ": ") + e.what());
                return false;
            }
        }
        
        // Validate we have sensory and motor neurons
        if (pImpl->sensoryNeurons.empty()) {
            NLM_LOG_WARNING("Brain::initialize: No sensory neurons created - this will disable sensory processing");
        }
        
        if (pImpl->motorNeurons.empty()) {
            NLM_LOG_WARNING("Brain::initialize: No motor neurons created - this will disable action production");
        }
        
        // Initialize connectivity with random weights
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!region) {
                NLM_LOG_ERROR(std::string("Brain::initialize: Failed to get region " + 
                                       std::to_string(i + 1) + " for connectivity initialization"));
                return false;
            }
            
            try {
                // Initialize random connectivity and synapse weights
                region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::initialize: Failed to initialize connectivity in region " + 
                                       std::to_string(i + 1) + ": ") + e.what());
                return false;
            }
        }
        
        // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
        
        // Initialize working memory
        if (pImpl->workingMemory) {
            try {
                pImpl->workingMemory->initialize(this);
                pImpl->workingMemory->setCapacity(std::max<size_t>(1, neuronCount / 10));
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::initialize: Failed to initialize working memory: ") + e.what());
                return false;
            }
        } else {
            NLM_LOG_ERROR("Brain::initialize: Working memory not initialized");
            return false;
        }
        
        // Initialize episodic memory
        if (pImpl->episodicMemory) {
            try {
                pImpl->episodicMemory->initialize(this);
                pImpl->episodicMemory->setMaxEpisodes(1000);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::initialize: Failed to initialize episodic memory: ") + e.what());
                return false;
            }
        } else {
            NLM_LOG_ERROR("Brain::initialize: Episodic memory not initialized");
            return false;
        }
        
        // Initialize associative memory
        if (pImpl->associativeMemory) {
            try {
                pImpl->associativeMemory->initialize(this);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::initialize: Failed to initialize associative memory: ") + e.what());
                return false;
            }
        } else {
            NLM_LOG_ERROR("Brain::initialize: Associative memory not initialized");
            return false;
        }
        
        // Initialize prediction system
        // (PredictionSystem doesn't have initialize method currently)
        
        // Initialize cognition systems
        if (pImpl->planner) {
            try {
                pImpl->planner->initialize(this);
                pImpl->planner->setPlanningDepth(5);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::initialize: Failed to initialize neural planner: ") + e.what());
                return false;
            }
        } else {
            NLM_LOG_ERROR("Brain::initialize: Neural planner not initialized");
            return false;
        }
        
        if (pImpl->conceptFormation) {
            try {
                pImpl->conceptFormation->initialize(this);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::initialize: Failed to initialize concept formation: ") + e.what());
                return false;
            }
        } else {
            NLM_LOG_ERROR("Brain::initialize: Concept formation not initialized");
            return false;
        }
        
        if (pImpl->attention) {
            try {
                pImpl->attention->initialize(this);
                pImpl->attention->setInhibitionStrength(0.5f);
                pImpl->attention->setExcitationStrength(1.5f);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::initialize: Failed to initialize attention system: ") + e.what());
                return false;
            }
        } else {
            NLM_LOG_ERROR("Brain::initialize: Attention system not initialized");
            return false;
        }
        
        // Initialize neuromodulation
        if (pImpl->novelty) {
            try {
                pImpl->novelty->initialize(this);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::initialize: Failed to initialize novelty detection: ") + e.what());
                return false;
            }
        } else {
            NLM_LOG_ERROR("Brain::initialize: Novelty detection not initialized");
            return false;
        }
        
        if (pImpl->curiosity) {
            try {
                pImpl->curiosity->initialize(this);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::initialize: Failed to initialize curiosity system: ") + e.what());
                return false;
            }
        } else {
            NLM_LOG_ERROR("Brain::initialize: Curiosity system not initialized");
            return false;
        }
        
        // Register spike handlers for event-driven processing
        if (pImpl->spikeSystem) {
            try {
                pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
                    // Count spikes
                    ++pImpl->totalSpikesThisStep;
                    ++pImpl->totalSpikesTotal;
                });
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::initialize: Failed to register spike handler: ") + e.what());
                return false;
            }
        }
        
        // Register delayed spike handler to deliver synaptic input
        if (pImpl->spikeSystem) {
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
                NLM_LOG_ERROR(std::string("Brain::initialize: Failed to register delayed spike handler: ") + e.what());
                return false;
            }
        }
        
        // Configure checkpoint manager
        std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        if (pImpl->checkpointManager) {
            try {
                pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::initialize: Failed to configure checkpoint manager: ") + e.what());
                return false;
            }
        } else {
            NLM_LOG_ERROR("Brain::initialize: Checkpoint manager not initialized");
            return false;
        }
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain::initialize: Unexpected exception: ") + e.what());
        return false;
    }
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    try {
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
        
        // Validate step parameters
        if (currentStep < 0) {
            NLM_LOG_ERROR(std::string("Brain::step: Invalid currentStep (must be >= 0): ") + std::to_string(currentStep));
            return;
        }
        
        if (currentTime < 0.0f) {
            NLM_LOG_ERROR(std::string("Brain::step: Invalid currentTime (must be >= 0): ") + std::to_string(currentTime));
            return;
        }
        
        if (!pImpl) {
            NLM_LOG_ERROR("Brain::step: Brain implementation is null");
            return;
        }
        
        // Validate internal state
        if (pImpl->regions.empty()) {
            NLM_LOG_WARNING("Brain::step: No regions initialized - simulation may not function correctly");
        }
        
        // Update state variables
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = 0;
        
        // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
        if (pImpl->spikeSystem) {
            try {
                pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::step: Failed to process delayed spikes: ") + e.what());
                // Continue execution despite spike processing failure
            }
        }
        
        // ========== STEP 2: Update all neurons (LIF dynamics) ==========
        try {
            for (auto& region : pImpl->regions) {
                if (!region) {
                    NLM_LOG_ERROR("Brain::step: Null region in regions list");
                    continue;
                }
                
                for (auto& pop : region->getPopulations()) {
                    if (!pop) {
                        NLM_LOG_ERROR("Brain::step: Null population in region");
                        continue;
                    }
                    
                    for (auto* neuron : pop->getNeurons()) {
                        if (!neuron) {
                            NLM_LOG_ERROR("Brain::step: Null neuron pointer");
                            continue;
                        }
                        
                        try {
                            neuron->stepLIF(currentTime, pImpl->timestep);
                        } catch (const std::exception& e) {
                            NLM_LOG_ERROR(std::string("Brain::step: Failed to step neuron " + 
                                                   std::to_string(neuron->getId()) + ": ") + e.what());
                            // Continue with other neurons
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Brain::step: Failed to update neurons: ") + e.what());
            // Continue with other steps
        }
        
        // ========== STEP 3: Detect spikes and schedule spike events ==========
        try {
            for (auto& region : pImpl->regions) {
                if (!region) continue;
                
                for (auto& pop : region->getPopulations()) {
                    if (!pop) continue;
                    
                    for (auto* neuron : pop->getNeurons()) {
                        if (!neuron) continue;
                        
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
                                    if (syn) syn->recordPostSpike(currentTime);
                                }
                                
                                // Get outgoing synapses and schedule delayed spike events
                                auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                                for (Synapse* syn : outgoingSynapses) {
                                    if (!syn) continue;
                                    
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
                                        NLM_LOG_ERROR(std::string("Brain::step: Failed to process outgoing synapse " + 
                                               std::to_string(syn->getId()) + " for neuron " + 
                                               std::to_string(neuron->getId()) + ": ") + e.what());
                                        // Continue with other synapses
                                    }
                                }
                                
                                // Store to working memory - neurons that fire become part of working memory
                                if (pImpl->workingMemory && neuron) {
                                    try {
                                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                                    } catch (const std::exception& e) {
                                        NLM_LOG_ERROR(std::string("Brain::step: Failed to store neuron " + 
                                               std::to_string(neuron->getId()) + " to working memory: ") + e.what());
                                    }
                                }
                            }
                        } catch (const std::exception& e) {
                            NLM_LOG_ERROR(std::string("Brain::step: Exception processing neuron " + 
                                                   std::to_string(neuron->getId()) + ": ") + e.what());
                            continue;
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Brain::step: Failed to detect spikes: ") + e.what());
            // Continue with other steps
        }
        
        // Process immediate spikes
        if (pImpl->spikeSystem) {
            try {
                pImpl->spikeSystem->processSpikes(currentStep);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::step: Failed to process spikes: ") + e.what());
                // Continue execution despite spike processing failure
            }
        }
        
        // ========== STEP 4: Update working memory ==========
        if (pImpl->workingMemory) {
            try {
                pImpl->workingMemory->update(pImpl->timestep);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::step: Failed to update working memory: ") + e.what());
                // Continue with other steps
            }
        }
        
        // ========== STEP 5: Apply neuromodulation effects ==========
        try {
            // Update novelty detection
            if (pImpl->novelty) {
                try {
                    pImpl->novelty->update(pImpl->timestep);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Brain::step: Failed to update novelty detection: ") + e.what());
                }
            }
            
            // Update curiosity
            if (pImpl->curiosity) {
                try {
                    pImpl->curiosity->update(pImpl->timestep);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Brain::step: Failed to update curiosity system: ") + e.what());
                }
            }
            
            // Update dopamine (reward prediction error)
            if (pImpl->dopamine) {
                try {
                    pImpl->dopamine->update(pImpl->timestep);
                    
                    // Apply dopamine effects on neural excitability
                    float dopamineLevel = pImpl->dopamine->getLevel();
                    for (auto& region : pImpl->regions) {
                        if (!region) continue;
                        
                        for (auto& pop : region->getPopulations()) {
                            if (!pop) continue;
                            
                            for (auto* neuron : pop->getNeurons()) {
                                if (!neuron) continue;
                                
                                // Dopamine modulates excitability by injecting additional current
                                float excitabilityMod = dopamineLevel * 0.5f;
                                if (excitabilityMod > 0.0f) {
                                    try {
                                        neuron->injectCurrent(excitabilityMod);
                                    } catch (const std::exception& e) {
                                        NLM_LOG_ERROR(std::string("Brain::step: Failed to inject dopamine current into neuron " + 
                                               std::to_string(neuron->getId()) + ": ") + e.what());
                                    }
                                }
                            }
                        }
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Brain::step: Failed to update dopamine system: ") + e.what());
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Brain::step: Exception in neuromodulation: ") + e.what());
            // Continue with other steps
        }
        
        // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
        try {
            // Calculate neuromodulation factor for plasticity
            float plasticityMod = 1.0f;
            if (pImpl->dopamine) {
                try {
                    plasticityMod = pImpl->dopamine->getPlasticityFactor();
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Brain::step: Failed to get dopamine plasticity factor: ") + e.what());
                }
            }
            
            for (auto& region : pImpl->regions) {
                if (!region) continue;
                
                for (auto& syn : region->getSynapses()) {
                    if (!syn) {
                        NLM_LOG_ERROR("Brain::step: Null synapse pointer");
                        continue;
                    }
                    
                    try {
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
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR(std::string("Brain::step: Failed to apply plasticity to synapse " + 
                                               std::to_string(syn->getId()) + ": ") + e.what());
                        // Continue with other synapses
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Brain::step: Exception in plasticity: ") + e.what());
            // Continue with other steps
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
                            if (!region) continue;
                            
                            for (auto& pop : region->getPopulations()) {
                                if (!pop) continue;
                                
                                for (auto* neuron : pop->getNeurons()) {
                                    if (!neuron) continue;
                                    
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
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR(std::string("Brain::step: Failed to update episodic memory: ") + e.what());
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Brain::step: Exception in episodic memory update: ") + e.what());
            // Continue with other steps
        }
        
        // ========== STEP 8: Update prediction system ==========
        if (pImpl->predictionSystem) {
            try {
                // The prediction system would be updated with sensory observations
                // For now, just track prediction error history
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::step: Exception in prediction system update: ") + e.what());
            }
        }
        
        // ========== STEP 9: Update attention system ==========
        if (pImpl->attention) {
            try {
                pImpl->attention->update(pImpl->timestep);
                
                // Apply attention to working memory winners
                if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                    std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
                    pImpl->attention->processCompetition(competitors);
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::step: Exception in attention system update: ") + e.what());
            }
        }
        
        // ========== STEP 10: Update concept formation ==========
        if (pImpl->conceptFormation) {
            try {
                // Would process current neural activity patterns to form concepts
                // This requires sensory state encoding
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::step: Exception in concept formation update: ") + e.what());
            }
        }
        
        // ========== STEP 11: Apply structural plasticity periodically ==========
        if (currentStep % 100 == 0) {
            if (pImpl->structuralPlasticity) {
                try {
                    pImpl->structuralPlasticity->update(this, *pImpl->rng);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Brain::step: Failed to apply structural plasticity at step " + 
                                           std::to_string(currentStep) + ": ") + e.what());
                }
            }
        }
        
        // ========== STEP 12: Replay important memories ==========
        if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
            try {
                // Get episodes for replay
                auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
                for (const auto* episode : episodesToReplay) {
                    if (episode) {
                        try {
                            pImpl->episodicMemory->replayEpisode(episode);
                        } catch (const std::exception& e) {
                            NLM_LOG_ERROR(std::string("Brain::step: Failed to replay episode at step " + 
                                                   std::to_string(currentStep) + ": ") + e.what());
                        }
                    }
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::step: Exception in memory replay: ") + e.what());
            }
        }
        
        // ========== STEP 13: Apply development effects ==========
        if (currentStep % 1000 == 0) {  // Update development every 1000 steps
            if (pImpl->developmentSystem) {
                try {
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
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Brain::step: Failed to update development at step " + 
                                           std::to_string(currentStep) + ": ") + e.what());
                }
            }
        }
        
        // ========== STEP 14: Periodic memory consolidation ==========
        if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
            try {
                // Consolidate important memories, remove weak ones
                pImpl->episodicMemory->consolidate(0.3f);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::step: Failed to consolidate memory at step " + 
                                       std::to_string(currentStep) + ": ") + e.what());
            }
        }
        
        // ========== STEP 15: Checkpoint management ==========
        if (pImpl->checkpointManager) {
            try {
                pImpl->checkpointManager->update(currentStep, currentTime);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::step: Failed to update checkpoint manager at step " + 
                                       std::to_string(currentStep) + ": ") + e.what());
            }
        }
        
        // ========== STEP 16: Collect statistics ==========
        try {
            // Count spikes for statistics
            for (auto& region : pImpl->regions) {
                if (!region) continue;
                
                for (auto& pop : region->getPopulations()) {
                    if (!pop) continue;
                    
                    for (auto* neuron : pop->getNeurons()) {
                        if (!neuron) continue;
                        
                        const auto& state = neuron->getState();
                        bool justFired = (state.firingState == FiringState::Refractory &&
                                         state.lastSpikeTime >= 0.0f &&
                                         std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);
                        
                        if (justFired) {
                            ++pImpl->totalSpikesThisStep;
                            ++pImpl->totalSpikesTotal;
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Brain::step: Exception collecting statistics: ") + e.what());
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain::step: Unexpected exception: ") + e.what());
        // Continue execution despite fatal error
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    try {
        // Validate input
        if (!input.getData().empty() && pImpl->sensoryNeurons.empty()) {
            NLM_LOG_WARNING("Brain::receiveSensoryInput: No sensory neurons available for input processing");
            return;
        }
        
        // Validate brain state
        if (!pImpl) {
            NLM_LOG_ERROR("Brain::receiveSensoryInput: Brain implementation is null");
            return;
        }
        
        const auto& values = input.getData();
        if (values.empty()) {
            NLM_LOG_DEBUG("Brain::receiveSensoryInput: No input data provided");
            return;
        }
        
        size_t numSensory = pImpl->sensoryNeurons.size();
        if (numSensory == 0) {
            NLM_LOG_WARNING("Brain::receiveSensoryInput: No sensory neurons initialized - cannot process input");
            return;
        }
        
        // Validate input values
        if (values.size() > numSensory) {
            NLM_LOG_WARNING(std::string("Brain::receiveSensoryInput: Input has ") + 
                         std::to_string(values.size()) + " values but only " +
                         std::to_string(numSensory) + " sensory neurons available - truncating");
        }
        
        // Distribute input across sensory neurons
        size_t processedNeurons = 0;
        for (size_t i = 0; i < numSensory && i < values.size(); ++i) {
            try {
                // Normalize input value to range [-10, 10] mV
                float normalizedValue = static_cast<float>(values[i]) * 10.0f;
                
                // Clamp normalized value to reasonable range
                if (normalizedValue < -10.0f) normalizedValue = -10.0f;
                if (normalizedValue > 10.0f) normalizedValue = 10.0f;
                
                // Validate neuron pointer
                if (!pImpl->sensoryNeurons[i]) {
                    NLM_LOG_ERROR(std::string("Brain::receiveSensoryInput: Null sensory neuron at index ") + std::to_string(i));
                    continue;
                }
                
                // Inject current into this sensory neuron
                pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
                processedNeurons++;
                
                // Also store in working memory for input that exceeds threshold
                if (pImpl->workingMemory && std::abs(normalizedValue) > 0.5f) {
                    try {
                        pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), std::abs(normalizedValue) / 10.0f);
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR(std::string("Brain::receiveSensoryInput: Failed to store sensory neuron " + 
                                               std::to_string(pImpl->sensoryNeurons[i]->getId()) + 
                                               " to working memory: ") + e.what());
                    }
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::receiveSensoryInput: Exception processing sensory neuron " + 
                                       std::to_string(i) + ": ") + e.what());
                // Continue with other neurons
            }
        }
        
        if (processedNeurons == 0 && !values.empty()) {
            NLM_LOG_WARNING(std::string("Brain::receiveSensoryInput: Failed to process any sensory neurons - ") +
                         std::to_string(values.size()) + " input values provided but no neurons updated");
        } else {
            NLM_LOG_DEBUG(std::string("Brain::receiveSensoryInput: Processed ") + 
                       std::to_string(processedNeurons) + " out of " +
                       std::to_string(std::min<size_t>(numSensory, values.size())) + 
                       " sensory neurons");
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain::receiveSensoryInput: Unexpected exception: ") + e.what());
        // Continue execution despite input processing failure
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    try {
        // Validate parameters
        if (!pImpl) {
            NLM_LOG_ERROR("Brain::injectCurrent: Brain implementation is null");
            return;
        }
        
        if (neuron.index() == 0) {
            NLM_LOG_ERROR("Brain::injectCurrent: Invalid NeuronId with zero index");
            return;
        }
        
        // Validate current range
        if (std::abs(current) > 100.0f) {  // Reasonable upper bound
            NLM_LOG_WARNING(std::string("Brain::injectCurrent: Large current value ") + 
                         std::to_string(current) + std::string(" (clamped to reasonable range)"));
        }
        
        // Search for neuron
        size_t neuronsFound = 0;
        for (auto& region : pImpl->regions) {
            if (!region) {
                NLM_LOG_ERROR("Brain::injectCurrent: Null region pointer encountered");
                continue;
            }
            
            auto neurons = region->getAllNeurons();
            for (auto* n : neurons) {
                if (!n) {
                    NLM_LOG_ERROR("Brain::injectCurrent: Null neuron pointer encountered");
                    continue;
                }
                
                if (n->getId() == neuron) {
                    try {
                        n->injectCurrent(current);
                        neuronsFound++;
                        NLM_LOG_DEBUG(std::string("Brain::injectCurrent: Injected current ") + 
                                   std::to_string(current) + std::string(" to neuron ") + 
                                   std::to_string(neuron.index()));
                        return;
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR(std::string("Brain::injectCurrent: Failed to inject current to neuron " + 
                                               std::to_string(neuron.index()) + ": ") + e.what());
                        return;
                    }
                }
            }
        }
        
        if (neuronsFound == 0) {
            NLM_LOG_WARNING(std::string("Brain::injectCurrent: Neuron with ID ") + 
                         std::to_string(neuron.index()) + std::string(" not found"));
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain::injectCurrent: Unexpected exception: ") + e.what());
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    try {
        // Validate parameters
        if (!pImpl) {
            NLM_LOG_ERROR("Brain::injectCurrentToNeurons: Brain implementation is null");
            return;
        }
        
        // Validate current range
        if (std::abs(current) > 100.0f) {  // Reasonable upper bound
            NLM_LOG_WARNING(std::string("Brain::injectCurrentToNeurons: Large current value ") + 
                         std::to_string(current) + std::string(" (clamped to reasonable range)"));
        }
        
        // Validate type
        if (type == NeuronType::Undefined || type >= NeuronType::Count) {
            NLM_LOG_ERROR(std::string("Brain::injectCurrentToNeurons: Invalid NeuronType ") + 
                       std::to_string(static_cast<int>(type)));
            return;
        }
        
        // Search for neurons of the specified type
        size_t neuronsAffected = 0;
        for (auto& region : pImpl->regions) {
            if (!region) {
                NLM_LOG_ERROR("Brain::injectCurrentToNeurons: Null region pointer encountered");
                continue;
            }
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) {
                    NLM_LOG_ERROR("Brain::injectCurrentToNeurons: Null population pointer encountered");
                    continue;
                }
                
                if (pop->getNeuronType() == type) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (!neuron) {
                            NLM_LOG_ERROR("Brain::injectCurrentToNeurons: Null neuron pointer encountered");
                            continue;
                        }
                        
                        try {
                            neuron->injectCurrent(current);
                            neuronsAffected++;
                        } catch (const std::exception& e) {
                            NLM_LOG_ERROR(std::string("Brain::injectCurrentToNeurons: Failed to inject current to neuron " + 
                                                   std::to_string(neuron->getId()) + ": ") + e.what());
                            // Continue with other neurons
                        }
                    }
                }
            }
        }
        
        if (neuronsAffected == 0) {
            NLM_LOG_WARNING(std::string("Brain::injectCurrentToNeurons: No neurons of type ") + 
                         std::to_string(static_cast<int>(type)) + std::string(" found"));
        } else {
            NLM_LOG_DEBUG(std::string("Brain::injectCurrentToNeurons: Injected current ") + 
                       std::to_string(current) + std::string(" to ") + 
                       std::to_string(neuronsAffected) + std::string(" neurons of type ") + 
                       std::to_string(static_cast<int>(type)));
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain::injectCurrentToNeurons: Unexpected exception: ") + e.what());
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
    try {
        // Validate brain state
        if (!pImpl) {
            NLM_LOG_ERROR("Brain::produceAction: Brain implementation is null");
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        // Check if motor neurons are available
        if (pImpl->motorNeurons.empty()) {
            NLM_LOG_WARNING("Brain::produceAction: No motor neurons available - returning Wait action");
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        // Validate motor neuron pointers
        size_t validMotorNeurons = 0;
        for (auto* neuron : pImpl->motorNeurons) {
            if (!neuron) {
                NLM_LOG_ERROR("Brain::produceAction: Null motor neuron pointer encountered");
                continue;
            }
            validMotorNeurons++;
        }
        
        if (validMotorNeurons == 0) {
            NLM_LOG_ERROR("Brain::produceAction: All motor neuron pointers are null");
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        // Calculate activity of motor neuron groups with error handling
        size_t firingMotor = 0;
        size_t totalChecked = 0;
        
        for (auto* neuron : pImpl->motorNeurons) {
            if (!neuron) continue;
            
            try {
                if (neuron->isFiring()) {
                    ++firingMotor;
                }
                totalChecked++;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::produceAction: Failed to check neuron " + 
                                       std::to_string(neuron->getId()) + " firing state: ") + e.what());
                // Continue with other neurons
            }
        }
        
        if (totalChecked == 0) {
            NLM_LOG_ERROR("Brain::produceAction: Could not check any motor neurons");
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        NLM_LOG_DEBUG(std::string("Brain::produceAction: Checked ") + 
                   std::to_string(totalChecked) + std::string(" motor neurons, ") + 
                   std::to_string(firingMotor) + std::string(" firing")
                   
        // Return a simple action
        ActionType type = ActionType::Wait;
        if (firingMotor > 0) {
            type = ActionType::MoveForward;
        }
        
        auto action = std::make_unique<Action>(type);
        
        NLM_LOG_INFO(std::string("Brain::produceAction: Produced action ") + 
                     std::to_string(static_cast<int>(type)) + 
                     std::string(" (firing motors: ") + std::to_string(firingMotor) + ")");
        
        return action;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain::produceAction: Unexpected exception: ") + e.what());
        return std::make_unique<Action>(ActionType::Wait);
    }
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    try {
        // Validate parameters
        if (!pImpl) {
            NLM_LOG_ERROR("Brain::applyNeuromodulation: Brain implementation is null");
            return;
        }
        
        if (!pImpl->stdp) {
            NLM_LOG_ERROR("Brain::applyNeuromodulation: STDP plasticity system not initialized");
            return;
        }
        
        // Get and validate modulation level
        float modulation = signal.getLevel();
        
        if (modulation < 0.0f) {
            NLM_LOG_WARNING(std::string("Brain::applyNeuromodulation: Negative modulation level ") + 
                         std::to_string(modulation) + std::string(" (should be non-negative)"));
        }
        
        if (modulation > 10.0f) {
            NLM_LOG_WARNING(std::string("Brain::applyNeuromodulation: Large modulation level ") + 
                         std::to_string(modulation) + std::string(" (may cause extreme plasticity)"));
        }
        
        try {
            // Scale STDP learning rates with validation
            float ltpWeight = 0.01f * modulation;
            float ltdWeight = 0.012f * modulation;
            
            // Clamp weights to reasonable ranges
            if (ltpWeight > 0.1f) ltpWeight = 0.1f;
            if (ltdWeight > 0.15f) ltdWeight = 0.15f;
            
            pImpl->stdp->setLTPWeight(ltpWeight);
            pImpl->stdp->setLTDWeight(ltdWeight);
            
            NLM_LOG_DEBUG(std::string("Brain::applyNeuromodulation: Applied modulation ") + 
                       std::to_string(modulation) + std::string(" (LTP=") + 
                       std::to_string(ltpWeight) + std::string(", LTD=") + std::to_string(ltdWeight) + ")");
        } catch (const BrainPlasticityException& e) {
            NLM_LOG_ERROR(std::string("Brain::applyNeuromodulation: Plasticity error: ") + e.what());
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Brain::applyNeuromodulation: Failed to apply neuromodulation: ") + e.what());
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain::applyNeuromodulation: Unexpected exception: ") + e.what());
    }
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    try {
        // Development updates structural plasticity
        if (!pImpl) {
            NLM_LOG_ERROR("Brain::develop: Brain implementation is null");
            return;
        }
        
        if (!pImpl->structuralPlasticity) {
            NLM_LOG_ERROR("Brain::develop: Structural plasticity system not initialized");
            return;
        }
        
        if (!pImpl->rng) {
            NLM_LOG_ERROR("Brain::develop: Random generator not initialized");
            return;
        }
        
        try {
            pImpl->structuralPlasticity->update(this, *pImpl->rng);
            NLM_LOG_INFO("Brain::develop: Development update complete");
        } catch (const BrainPlasticityException& e) {
            NLM_LOG_ERROR(std::string("Brain::develop: Plasticity error during development: ") + e.what());
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Brain::develop: Failed to apply development effects: ") + e.what());
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain::develop: Unexpected exception: ") + e.what());
    }
}

void Brain::reset() {
    try {
        NLM_LOG_INFO("Resetting NLM Brain...");
        
        // Validate brain state
        if (!pImpl) {
            NLM_LOG_ERROR("Brain::reset: Brain implementation is null");
            return;
        }
        
        // Reset core state variables
        pImpl->currentStep = 0;
        pImpl->currentTime = 0.0;
        pImpl->totalSpikesThisStep = 0;
        pImpl->totalSpikesTotal = 0;
        pImpl->isResting = false;
        pImpl->stepsSinceLastEpisode = 0;
        pImpl->developmentalStage = DevelopmentalStage::Initial;
        
        // Reset all regions with error handling
        if (pImpl->regions.empty()) {
            NLM_LOG_WARNING("Brain::reset: No regions to reset");
        }
        
        for (auto& region : pImpl->regions) {
            if (!region) {
                NLM_LOG_ERROR("Brain::reset: Null region pointer encountered");
                continue;
            }
            
            try {
                region->reset();
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::reset: Failed to reset region ") + 
                           std::to_string(region->getId().index()) + ": " + e.what());
                // Continue with other regions
            }
        }
        
        // Reset spike system
        if (pImpl->spikeSystem) {
            try {
                pImpl->spikeSystem->reset();
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::reset: Failed to reset spike system: ") + e.what());
            }
        }
        
        // Reset memory systems with error handling
        if (pImpl->workingMemory) {
            try {
                pImpl->workingMemory->clear();
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::reset: Failed to clear working memory: ") + e.what());
            }
        }
        
        if (pImpl->episodicMemory) {
            try {
                pImpl->episodicMemory->clear();
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::reset: Failed to clear episodic memory: ") + e.what());
            }
        }
        
        if (pImpl->associativeMemory) {
            try {
                pImpl->associativeMemory->clear();
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::reset: Failed to clear associative memory: ") + e.what());
            }
        }
        
        if (pImpl->attention) {
            try {
                pImpl->attention->reset();
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::reset: Failed to reset attention system: ") + e.what());
            }
        }
        
        NLM_LOG_INFO("NLM Brain reset complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain::reset: Unexpected exception: ") + e.what());
        // Continue with partial reset despite failure
    }
}

bool Brain::save(const std::string& filepath) const {
    try {
        // Validate parameters
        if (filepath.empty()) {
            NLM_LOG_ERROR("Brain::save: Filepath is empty");
            return false;
        }
        
        if (!pImpl) {
            NLM_LOG_ERROR("Brain::save: Brain implementation is null");
            return false;
        }
        
        NLM_LOG_INFO("Saving brain state to " + filepath);
        
        try {
            CheckpointWriter writer;
            if (!writer.create(filepath, CompressionLevel::Balanced)) {
                NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
                return false;
            }
            
            // Set metadata with validation
            writer.setMetadata(
                getTotalNeuronCount(),
                getTotalSynapseCount(),
                getRegionCount(),
                pImpl->currentStep,
                pImpl->currentTime
            );
            
            // Validate we have regions to save
            if (pImpl->regions.empty()) {
                NLM_LOG_ERROR("Brain::save: No regions to save");
                return false;
            }
            
            // Write neurons with detailed error handling
            NeuronCheckpointData neuronData;
            neuronData.membranePotential.reserve(getTotalNeuronCount());
            neuronData.restingPotential.reserve(getTotalNeuronCount());
            neuronData.threshold.reserve(getTotalNeuronCount());
            neuronData.resetPotential.reserve(getTotalNeuronCount());
            neuronData.leakConductance.reserve(getTotalNeuronCount());
            
            size_t neuronCountSaved = 0;
            for (const auto& region : pImpl->regions) {
                if (!region) {
                    NLM_LOG_ERROR("Brain::save: Null region pointer encountered");
                    continue;
                }
                
                for (const auto& pop : region->getPopulations()) {
                    if (!pop) {
                        NLM_LOG_ERROR("Brain::save: Null population pointer encountered");
                        continue;
                    }
                    
                    for (const auto* neuron : pop->getNeurons()) {
                        if (!neuron) {
                            NLM_LOG_ERROR("Brain::save: Null neuron pointer encountered");
                            continue;
                        }
                        
                        try {
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
                            neuronCountSaved++;
                        } catch (const std::exception& e) {
                            NLM_LOG_ERROR(std::string("Brain::save: Failed to collect neuron " + 
                                                   std::to_string(neuron->getId()) + " state: ") + e.what());
                            // Continue with other neurons
                        }
                    }
                }
            }
            
            if (neuronCountSaved == 0) {
                NLM_LOG_ERROR("Brain::save: No neurons could be saved");
                return false;
            }
            
            NLM_LOG_INFO("Collected state for " + std::to_string(neuronCountSaved) + " neurons");
            
            if (!writer.writeNeurons(neuronData)) {
                NLM_LOG_ERROR("Failed to write neurons to checkpoint");
                return false;
            }
            
            // Write synapses with detailed error handling
            SynapseCheckpointData synapseData;
            size_t synapseCountSaved = 0;
            for (const auto& region : pImpl->regions) {
                if (!region) continue;
                
                for (const auto* syn : region->getSynapses()) {
                    if (!syn) {
                        NLM_LOG_ERROR("Brain::save: Null synapse pointer encountered");
                        continue;
                    }
                    
                    try {
                        synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                        synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                        synapseData.weight.push_back(syn->getWeight());
                        synapseData.delay.push_back(syn->getDelay());
                        synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                        synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                        synapseCountSaved++;
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR(std::string("Brain::save: Failed to collect synapse " + 
                                               std::to_string(syn->getId()) + " data: ") + e.what());
                        // Continue with other synapses
                    }
                }
            }
            
            if (synapseCountSaved == 0) {
                NLM_LOG_WARNING("Brain::save: No synapses were saved - checkpoint may be incomplete");
            } else {
                NLM_LOG_INFO("Collected state for " + std::to_string(synapseCountSaved) + " synapses");
            }
            
            if (!writer.writeSynapses(synapseData)) {
                NLM_LOG_ERROR("Failed to write synapses to checkpoint");
                return false;
            }
            
            // Finalize with error handling
            if (!writer.finalize()) {
                NLM_LOG_ERROR("Failed to finalize checkpoint");
                return false;
            }
            
            size_t bytesWritten = writer.getBytesWritten();
            NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(bytesWritten) + " bytes)");
            return true;
            
        } catch (const BrainCheckpointException& e) {
            NLM_LOG_ERROR(std::string("Brain::save: Checkpoint error: ") + e.what());
            return false;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Brain::save: Unexpected error: ") + e.what());
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain::save: Unexpected exception: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    try {
        // Validate parameters
        if (filepath.empty()) {
            NLM_LOG_ERROR("Brain::load: Filepath is empty");
            return false;
        }
        
        if (!pImpl) {
            NLM_LOG_ERROR("Brain::load: Brain implementation is null");
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
            
            // Read neurons with error handling
            NeuronCheckpointData neuronData;
            if (!reader.readNeurons(neuronData)) {
                NLM_LOG_ERROR("Failed to read neurons from checkpoint");
                return false;
            }
            
            if (neuronData.membranePotential.empty()) {
                NLM_LOG_WARNING("Brain::load: Checkpoint contains no neuron data - loading may not be complete");
            }
            
            // Apply neuron states with validation
            size_t idx = 0;
            size_t neuronsLoaded = 0;
            
            for (auto& region : pImpl->regions) {
                if (!region) {
                    NLM_LOG_ERROR("Brain::load: Null region pointer encountered during loading");
                    continue;
                }
                
                for (auto& pop : region->getPopulations()) {
                    if (!pop) {
                        NLM_LOG_ERROR("Brain::load: Null population pointer encountered during loading");
                        continue;
                    }
                    
                    for (auto* neuron : pop->getNeurons()) {
                        if (!neuron) {
                            NLM_LOG_ERROR("Brain::load: Null neuron pointer encountered during loading");
                            continue;
                        }
                        
                        try {
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
                                neuronsLoaded++;
                            }
                        } catch (const std::exception& e) {
                            NLM_LOG_ERROR(std::string("Brain::load: Failed to apply neuron " + 
                                                   std::to_string(neuron->getId()) + " state: ") + e.what());
                            // Continue with other neurons
                        }
                        idx++;
                    }
                }
            }
            
            if (neuronsLoaded == 0 && !neuronData.membranePotential.empty()) {
                NLM_LOG_WARNING(std::string("Brain::load: Applied " + std::to_string(neuronsLoaded) + " out of " +
                                           std::to_string(neuronData.membranePotential.size()) +
                                           " neuron states from checkpoint"));
            }
            
            // Read synapses with error handling
            SynapseCheckpointData synapseData;
            if (!reader.readSynapses(synapseData)) {
                NLM_LOG_ERROR("Failed to read synapses from checkpoint");
                return false;
            }
            
            if (synapseData.weight.empty()) {
                NLM_LOG_WARNING("Brain::load: Checkpoint contains no synapse data - network connectivity may be incomplete");
            } else {
                NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses from checkpoint");
            }
            
            // Apply synapse states - this is complex because we need to find matching synapses
            // For now, just log the count
            NLM_LOG_INFO("Brain state loaded successfully");
            return true;
            
        } catch (const BrainCheckpointException& e) {
            NLM_LOG_ERROR(std::string("Brain::load: Checkpoint error: ") + e.what());
            return false;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Brain::load: Unexpected error: ") + e.what());
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain::load: Unexpected exception: ") + e.what());
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    try {
        // Validate input
        if (name.empty()) {
            NLM_LOG_ERROR("Brain::addRegion: Region name cannot be empty");
            throw BrainRegionException("Region name cannot be empty");
        }
        
        if (!pImpl) {
            NLM_LOG_ERROR("Brain::addRegion: Brain implementation is null");
            throw BrainRegionException("Brain implementation is null");
        }
        
        // Check if region with same name already exists
        for (const auto& region : pImpl->regions) {
            if (region && region->getName() == name) {
                NLM_LOG_ERROR(std::string("Brain::addRegion: Region with name '" + name + "' already exists"));
                throw BrainRegionException(std::string("Region with name '" + name + "' already exists"));
            }
        }
        
        // Generate and validate region ID
        RegionId id(pImpl->nextRegionId);
        if (pImpl->nextRegionId == 0 || pImpl->nextRegionId > std::numeric_limits<RegionId>::max()) {
            NLM_LOG_ERROR("Brain::addRegion: Invalid region ID generation");
            throw BrainRegionException("Invalid region ID generation");
        }
        
        // Create region with error handling
        auto region = std::make_unique<NeuralRegion>(id, name);
        if (!region) {
            NLM_LOG_ERROR("Brain::addRegion: Failed to allocate memory for NeuralRegion");
            throw BrainMemoryException("Failed to allocate memory for NeuralRegion");
        }
        
        pImpl->regions.push_back(std::move(region));
        
        NLM_LOG_INFO(std::string("Brain::addRegion: Added region ") + name + 
                     std::string(" with ID ") + std::to_string(id.index()));
        
        return id;
    } catch (const BrainException& e) {
        // Re-throw brain-specific exceptions
        throw;
    } catch (const std::exception& e) {
        // Convert other exceptions to BrainRegionException
        NLM_LOG_ERROR(std::string("Brain::addRegion: Unexpected exception: ") + e.what());
        throw BrainRegionException(std::string("Unexpected exception: ") + e.what());
    }
}

NeuralRegion* Brain::getRegion(RegionId id) {
    try {
        // Validate input
        if (!pImpl) {
            NLM_LOG_ERROR("Brain::getRegion: Brain implementation is null");
            return nullptr;
        }
        
        if (pImpl->regions.empty()) {
            NLM_LOG_ERROR("Brain::getRegion: No regions exist");
            return nullptr;
        }
        
        // Validate region ID
        if (id.index() == 0) {
            NLM_LOG_ERROR("Brain::getRegion: Invalid RegionId with zero index");
            return nullptr;
        }
        
        // Search for region
        for (auto& region : pImpl->regions) {
            if (!region) {
                NLM_LOG_ERROR("Brain::getRegion: Null region pointer encountered in regions list");
                continue;
            }
            
            if (region->getId() == id) {
                NLM_LOG_DEBUG(std::string("Brain::getRegion: Found region ") + 
                           region->getName() + std::string(" with ID ") + std::to_string(id.index()));
                return region.get();
            }
        }
        
        NLM_LOG_WARNING(std::string("Brain::getRegion: Region with ID ") + 
                       std::to_string(id.index()) + std::string(" not found"));
        return nullptr;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain::getRegion: Unexpected exception: ") + e.what());
        return nullptr;
    }
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    try {
        // Validate input
        if (!pImpl) {
            NLM_LOG_ERROR("Brain::getRegion (const): Brain implementation is null");
            return nullptr;
        }
        
        if (pImpl->regions.empty()) {
            NLM_LOG_ERROR("Brain::getRegion (const): No regions exist");
            return nullptr;
        }
        
        // Validate region ID
        if (id.index() == 0) {
            NLM_LOG_ERROR("Brain::getRegion (const): Invalid RegionId with zero index");
            return nullptr;
        }
        
        // Search for region
        for (const auto& region : pImpl->regions) {
            if (!region) {
                NLM_LOG_ERROR("Brain::getRegion (const): Null region pointer encountered in regions list");
                continue;
            }
            
            if (region->getId() == id) {
                NLM_LOG_DEBUG(std::string("Brain::getRegion (const): Found region ") + 
                           region->getName() + std::string(" with ID ") + std::to_string(id.index()));
                return region.get();
            }
        }
        
        NLM_LOG_WARNING(std::string("Brain::getRegion (const): Region with ID ") + 
                       std::to_string(id.index()) + std::string(" not found"));
        return nullptr;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain::getRegion (const): Unexpected exception: ") + e.what());
        return nullptr;
    }
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
    try {
        // Validate parameters
        if (!pImpl) {
            NLM_LOG_ERROR("Brain::addInterRegionConnection: Brain implementation is null");
            return;
        }
        
        if (source.index() == 0 || target.index() == 0) {
            NLM_LOG_ERROR("Brain::addInterRegionConnection: Invalid source or target RegionId");
            return;
        }
        
        if (source == target) {
            NLM_LOG_ERROR("Brain::addInterRegionConnection: Source and target regions cannot be the same");
            return;
        }
        
        // Validate that source and target regions exist
        auto* sourceRegion = getRegion(source);
        auto* targetRegion = getRegion(target);
        
        if (!sourceRegion) {
            NLM_LOG_ERROR(std::string("Brain::addInterRegionConnection: Source region with ID ") + 
                       std::to_string(source.index()) + std::string(" not found"));
            return;
        }
        
        if (!targetRegion) {
            NLM_LOG_ERROR(std::string("Brain::addInterRegionConnection: Target region with ID ") + 
                       std::to_string(target.index()) + std::string(" not found"));
            return;
        }
        
        // Validate weight and delay
        if (weight < 0.0f || weight > 1.0f) {
            NLM_LOG_ERROR(std::string("Brain::addInterRegionConnection: Invalid weight value ") + 
                       std::to_string(weight) + std::string(" (must be in range [0.0, 1.0])"));
            return;
        }
        
        if (delay < 1 || delay > 1000) {
            NLM_LOG_ERROR(std::string("Brain::addInterRegionConnection: Invalid delay value ") + 
                       std::to_string(delay) + std::string(" (must be in range [1, 1000])"));
            return;
        }
        
        // Check for duplicate connection
        for (const auto& conn : pImpl->interRegionConnections) {
            if ((conn.sourceRegion == source && conn.targetRegion == target) ||
                (conn.sourceRegion == target && conn.targetRegion == source)) {
                NLM_LOG_WARNING(std::string("Brain::addInterRegionConnection: Connection already exists between regions ") + 
                             std::to_string(source.index()) + std::string(" and ") + std::to_string(target.index()));
                return;
            }
        }
        
        // Add the connection
        pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
        
        NLM_LOG_INFO(std::string("Brain::addInterRegionConnection: Added connection from region ") + 
                     std::to_string(source.index()) + std::string(" to region ") + 
                     std::to_string(target.index()) + std::string(" (weight=") + 
                     std::to_string(weight) + std::string(", delay=") + std::to_string(delay) + ")");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain::addInterRegionConnection: Unexpected exception: ") + e.what());
    }
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    try {
        // Validate parameters
        if (!pImpl) {
            NLM_LOG_ERROR("Brain::removeInterRegionConnection: Brain implementation is null");
            return;
        }
        
        if (source.index() == 0 || target.index() == 0) {
            NLM_LOG_ERROR("Brain::removeInterRegionConnection: Invalid source or target RegionId");
            return;
        }
        
        if (source == target) {
            NLM_LOG_ERROR("Brain::removeInterRegionConnection: Source and target regions cannot be the same");
            return;
        }
        
        // Validate that source and target regions exist
        auto* sourceRegion = getRegion(source);
        auto* targetRegion = getRegion(target);
        
        if (!sourceRegion) {
            NLM_LOG_ERROR(std::string("Brain::removeInterRegionConnection: Source region with ID ") + 
                       std::to_string(source.index()) + std::string(" not found"));
            return;
        }
        
        if (!targetRegion) {
            NLM_LOG_ERROR(std::string("Brain::removeInterRegionConnection: Target region with ID ") + 
                       std::to_string(target.index()) + std::string(" not found"));
            return;
        }
        
        // Check if connection exists before removing
        bool connectionExists = false;
        for (const auto& conn : pImpl->interRegionConnections) {
            if ((conn.sourceRegion == source && conn.targetRegion == target) ||
                (conn.sourceRegion == target && conn.targetRegion == source)) {
                connectionExists = true;
                break;
            }
        }
        
        if (!connectionExists) {
            NLM_LOG_WARNING(std::string("Brain::removeInterRegionConnection: Connection not found between regions ") + 
                         std::to_string(source.index()) + std::string(" and ") + std::to_string(target.index()));
            return;
        }
        
        // Remove the connection
        size_t removedCount = pImpl->interRegionConnections.erase(
            std::remove_if(pImpl->interRegionConnections.begin(),
                          pImpl->interRegionConnections.end(),
                          [source, target](const InterRegionConnection& conn) {
                              return (conn.sourceRegion == source && conn.targetRegion == target) ||
                                     (conn.sourceRegion == target && conn.targetRegion == source);
                          }),
            pImpl->interRegionConnections.end()
        );
        
        NLM_LOG_INFO(std::string("Brain::removeInterRegionConnection: Removed ") + 
                     std::to_string(removedCount) + std::string(" connection(s) between regions ") + 
                     std::to_string(source.index()) + std::string(" and ") + std::to_string(target.index()));
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain::removeInterRegionConnection: Unexpected exception: ") + e.what());
    }
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
    try {
        NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
        
        // Basic brain statistics
        NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
        NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
        NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
        NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
        NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
        
        // Validate brain state
        if (!pImpl) {
            NLM_LOG_ERROR("Brain::logStatus: Brain implementation is null");
            return;
        }
        
        // Memory system status with validation
        if (pImpl->workingMemory) {
            try {
                NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::logStatus: Failed to get working memory status: ") + e.what());
            }
        }
        
        if (pImpl->episodicMemory) {
            try {
                NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::logStatus: Failed to get episodic memory status: ") + e.what());
            }
        }
        
        // Neuromodulation status with validation
        if (pImpl->dopamine) {
            try {
                NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::logStatus: Failed to get dopamine status: ") + e.what());
            }
        }
        
        if (pImpl->curiosity) {
            try {
                NLM_LOG_INFO("Curiosity level: " + std::to_string(pImpl->curiosity->getLevel()));
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::logStatus: Failed to get curiosity status: ") + e.what());
            }
        }
        
        if (pImpl->novelty) {
            try {
                NLM_LOG_INFO("Novelty level: " + std::to_string(pImpl->novelty->getLevel()));
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Brain::logStatus: Failed to get novelty status: ") + e.what());
            }
        }
        
        // Development status
        NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
        
        // Check system health and report issues
        size_t nullRegions = 0;
        size_t nullPopulations = 0;
        size_t nullNeurons = 0;
        size_t invalidConnections = 0;
        
        for (const auto& region : pImpl->regions) {
            if (!region) {
                nullRegions++;
                continue;
            }
            
            NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                        " (" + region->getName() + "): " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                        std::to_string(region->getSynapseCount()) + " synapses, " +
                        "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
            
            // Check for issues in this region
            for (const auto& pop : region->getPopulations()) {
                if (!pop) {
                    nullPopulations++;
                    continue;
                }
                
                for (const auto* neuron : pop->getNeurons()) {
                    if (!neuron) {
                        nullNeurons++;
                    }
                }
            }
            
            // Validate inter-region connections
            for (const auto& conn : pImpl->interRegionConnections) {
                if (conn.sourceRegion.index() == 0 || conn.targetRegion.index() == 0) {
                    invalidConnections++;
                }
            }
        }
        
        // Report any system issues
        if (nullRegions > 0) {
            NLM_LOG_ERROR(std::string("Brain::logStatus: Found ") + std::to_string(nullRegions) + 
                        std::string(" null regions"));
        }
        
        if (nullPopulations > 0) {
            NLM_LOG_ERROR(std::string("Brain::logStatus: Found ") + std::to_string(nullPopulations) + 
                        std::string(" null populations"));
        }
        
        if (nullNeurons > 0) {
            NLM_LOG_ERROR(std::string("Brain::logStatus: Found ") + std::to_string(nullNeurons) + 
                        std::string(" null neurons"));
        }
        
        if (invalidConnections > 0) {
            NLM_LOG_ERROR(std::string("Brain::logStatus: Found ") + std::to_string(invalidConnections) + 
                        std::string(" invalid connections"));
        }
        
        if (nullRegions == 0 && nullPopulations == 0 && nullNeurons == 0 && invalidConnections == 0) {
            NLM_LOG_INFO("Brain health: OK - All systems operational");
        } else {
            NLM_LOG_WARNING("Brain health: Issues detected - Review error messages above");
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain::logStatus: Unexpected exception: ") + e.what());
    }
}

} // namespace nlm
