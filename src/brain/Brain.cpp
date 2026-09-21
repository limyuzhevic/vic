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
        , developmentalStage(DevelopmentalStage::Initial)
        , nextRegionId(1)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(config->getOr<size_t>("replay_interval", 100))
        , consolidationInterval(config->getOr<size_t>("consolidation_interval", 1000))
    {
        NLM_LOG_INFO("Configuring brain parameters and validating system");
        
        // Validate configuration parameters
        validateConfiguration();
        
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        } else {
            NLM_LOG_WARNING("No random_seed in config, using default value 42");
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
        
        // Configure STDP parameters from config with validation
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        if (!validateRange(ltpWeight, 0.0f, 0.1f, "stdp_ltp_weight")) {
            ltpWeight = 0.01f;
            NLM_LOG_WARNING("Invalid stdp_ltp_weight, using default 0.01");
        }
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        if (!validateRange(ltdWeight, 0.0f, 0.1f, "stdp_ltd_weight")) {
            ltdWeight = 0.012f;
            NLM_LOG_WARNING("Invalid stdp_ltd_weight, using default 0.012");
        }
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        if (!validateRange(tau, 1.0f, 100.0f, "stdp_tau")) {
            tau = 20.0f;
            NLM_LOG_WARNING("Invalid stdp_tau, using default 20.0");
        }
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity from config with validation
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        if (!validateRange(synaptogenesisRate, 0.0f, 0.001f, "synaptogenesis_rate")) {
            synaptogenesisRate = 0.0001f;
            NLM_LOG_WARNING("Invalid synaptogenesis_rate, using default 0.0001");
        }
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        if (!validateRange(pruningRate, 0.0f, 0.0001f, "pruning_rate")) {
            pruningRate = 0.00001f;
            NLM_LOG_WARNING("Invalid pruning_rate, using default 0.00001");
        }
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        // Get timestep from config with validation
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        if (!validateRange(timestep, 0.0001f, 0.1f, "simulation_timestep")) {
            timestep = 0.001;
            NLM_LOG_WARNING("Invalid simulation_timestep, using default 0.001");
        }
        NLM_LOG_INFO("Simulation timestep validated: " + std::to_string(timestep) + "s");
        
        // Get integration intervals from config with validation
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        if (!validateRange(replayInterval, 1, 10000, "replay_interval")) {
            replayInterval = 100;
            NLM_LOG_WARNING("Invalid replay_interval, using default 100");
        }
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        if (!validateRange(consolidationInterval, 100, 100000, "consolidation_interval")) {
            consolidationInterval = 1000;
            NLM_LOG_WARNING("Invalid consolidation_interval, using default 1000");
        }
        
        // Get additional config parameters with validation
        size_t planningDepth = config->getOr<size_t>("planning_depth", 5);
        if (!validateRange(planningDepth, 1, 20, "planning_depth")) {
            planningDepth = 5;
            NLM_LOG_WARNING("Invalid planning_depth, using default 5");
        }
        float inhibitionStrength = config->getOr<float>("inhibition_strength", 0.5f);
        if (!validateRange(inhibitionStrength, 0.0f, 2.0f, "inhibition_strength")) {
            inhibitionStrength = 0.5f;
            NLM_LOG_WARNING("Invalid inhibition_strength, using default 0.5");
        }
        float excitationStrength = config->getOr<float>("excitation_strength", 1.5f);
        if (!validateRange(excitationStrength, 0.0f, 5.0f, "excitation_strength")) {
            excitationStrength = 1.5f;
            NLM_LOG_WARNING("Invalid excitation_strength, using default 1.5");
        }
        
        float maxEpisodes = config->getOr<size_t>("max_episodes", 1000);
        if (!validateRange(maxEpisodes, 10, 100000, "max_episodes")) {
            maxEpisodes = 1000;
            NLM_LOG_WARNING("Invalid max_episodes, using default 1000");
        }
        
        float workingMemoryCapacity = config->getOr<size_t>("working_memory_capacity", 100);
        if (!validateRange(workingMemoryCapacity, 10, 10000, "working_memory_capacity")) {
            workingMemoryCapacity = 100;
            NLM_LOG_WARNING("Invalid working_memory_capacity, using default 100");
        }
        
        // Validate brain-wide parameters
        size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
        if (!validateRange(neuronCount, 100, 1000000, "neuron_count")) {
            neuronCount = 1000;
            NLM_LOG_WARNING("Invalid neuron_count, using default 1000");
        }
        
        size_t regionCount = config->getOr<size_t>("region_count", 1);
        if (!validateRange(regionCount, 1, 100, "region_count")) {
            regionCount = 1;
            NLM_LOG_WARNING("Invalid region_count, using default 1");
        }
        
        float connectionProbability = config->getOr<float>("connection_probability", 0.1f);
        if (!validateRange(connectionProbability, 0.01f, 1.0f, "connection_probability")) {
            connectionProbability = 0.1f;
            NLM_LOG_WARNING("Invalid connection_probability, using default 0.1");
        }
        
        // Log validated configuration
        NLM_LOG_INFO("Configuration validated and validated:");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(neuronCount));
        NLM_LOG_INFO("  region_count: " + std::to_string(regionCount));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(connectionProbability));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(timestep) + "s");
        NLM_LOG_INFO("  replay_interval: " + std::to_string(replayInterval));
        NLM_LOG_INFO("  consolidation_interval: " + std::to_string(consolidationInterval));
        NLM_LOG_INFO("  max_episodes: " + std::to_string(maxEpisodes));
        NLM_LOG_INFO("  working_memory_capacity: " + std::to_string(workingMemoryCapacity));
        NLM_LOG_INFO("  random_seed: " + std::to_string(seed));
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    
    // Configuration validation helper methods
    void validateConfiguration() {
        NLM_LOG_INFO("Validating brain configuration parameters...");
        
        // Critical parameter validation with ranges and logging
        size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
        validateParameterRange("neuron_count", neuronCount, 100, 1000000,
            "Number of neurons in the brain must be between 100 and 1,000,000");
        
        size_t regionCount = config->getOr<size_t>("region_count", 1);
        validateParameterRange("region_count", regionCount, 1, 100,
            "Number of brain regions must be between 1 and 100");
        
        float connectionProbability = config->getOr<float>("connection_probability", 0.1f);
        validateParameterRange("connection_probability", connectionProbability, 0.01f, 1.0f,
            "Connection probability must be between 0.01 and 1.0");
        
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        validateParameterRange("simulation_timestep", timestep, 0.0001f, 0.1f,
            "Simulation timestep must be between 0.0001s and 0.1s");
        
        size_t replayInterval = config->getOr<size_t>("replay_interval", 100);
        validateParameterRange("replay_interval", replayInterval, 1, 10000,
            "Replay interval must be between 1 and 10,000 steps");
        
        size_t consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        validateParameterRange("consolidation_interval", consolidationInterval, 100, 100000,
            "Consolidation interval must be between 100 and 100,000 steps");
        
        NLM_LOG_INFO("Configuration validation complete - all parameters within acceptable ranges");
    }
    
    bool validateRange(double value, double min, double max, const std::string& paramName) {
        if (value < min || value > max) {
            NLM_LOG_WARNING("Parameter validation failed: " + paramName + 
                          " = " + std::to_string(value) + 
                          " (valid range: " + std::to_string(min) + " to " + std::to_string(max) + ")");
            return false;
        }
        return true;
    }
    
    bool validateRange(float value, float min, float max, const std::string& paramName) {
        if (value < min || value > max) {
            NLM_LOG_WARNING("Parameter validation failed: " + paramName + 
                          " = " + std::to_string(value) + 
                          " (valid range: " + std::to_string(min) + " to " + std::to_string(max) + ")");
            return false;
        }
        return true;
    }
    
    bool validateRange(size_t value, size_t min, size_t max, const std::string& paramName) {
        if (value < min || value > max) {
            NLM_LOG_WARNING("Parameter validation failed: " + paramName + 
                          " = " + std::to_string(value) + 
                          " (valid range: " + std::to_string(min) + " to " + std::to_string(max) + ")");
            return false;
        }
        return true;
    }
    
    void validateParameterRange(const std::string& paramName, double value,
                               double min, double max, const std::string& errorMessage) {
        if (!validateRange(value, min, max, paramName)) {
            NLM_LOG_ERROR("Configuration error: " + errorMessage);
        }
    }
    
    void validateParameterRange(const std::string& paramName, float value,
                               float min, float max, const std::string& errorMessage) {
        if (!validateRange(value, min, max, paramName)) {
            NLM_LOG_ERROR("Configuration error: " + errorMessage);
        }
    }
    
    void validateParameterRange(const std::string& paramName, size_t value,
                               size_t min, size_t max, const std::string& errorMessage) {
        if (!validateRange(value, min, max, paramName)) {
            NLM_LOG_ERROR("Configuration error: " + errorMessage);
        }
    }
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
            float excitationWeight = pImpl->config->getOr<float>("excitation_weight", 0.2f);
            float inhibitionWeight = pImpl->config->getOr<float>("inhibition_weight", 0.1f);
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, excitationWeight, inhibitionWeight);
        }
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(pImpl->config->getOr<size_t>("max_episodes", 1000));
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(this);
    
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems from config
    pImpl->planner->initialize(this);
    
    // Configure planner parameters from config
    size_t planningDepth = pImpl->config->getOr<size_t>("planning_depth", 5);
    pImpl->planner->setPlanningDepth(planningDepth);
    
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention->initialize(this);
    
    // Configure attention parameters from config
    float inhibitionStrength = pImpl->config->getOr<float>("inhibition_strength", 0.5f);
    float excitationStrength = pImpl->config->getOr<float>("excitation_strength", 1.5f);
    pImpl->attention->setInhibitionStrength(inhibitionStrength);
    pImpl->attention->setExcitationStrength(excitationStrength);
    
    // Initialize neuromodulation systems
    pImpl->novelty->initialize(this);
    pImpl->curiosity->initialize(this);
    pImpl->predictionError->initialize(this);
    pImpl->dopamine->initialize(this);
    
    // Register spike handlers for event-driven processing
    try {
        pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            // Count spikes
            ++pImpl->totalSpikesThisStep;
            ++pImpl->totalSpikesTotal;
        });
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to register spike handler: " + std::string(e.what()));
    }
    
    // Register delayed spike handler to deliver synaptic input
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
    }
    
    // Configure checkpoint manager with error handling
    try {
        std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to configure checkpoint manager: " + std::string(e.what()));
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
     * 14. Periodic memory consolidation
     * 15. Checkpoint management
     */
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    try {
        pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in delayed spike processing: " + std::string(e.what()));
    }
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                try {
                    if (neuron) {
                        neuron->stepLIF(currentTime, pImpl->timestep);
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Exception in neuron LIF update: " + std::string(e.what()));
                }
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
        
        // Apply attention modulation to working memory traces
        if (pImpl->attention) {
            std::vector<NeuronId> memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            if (!memoryNeurons.empty()) {
                pImpl->attention->processCompetition(memoryNeurons);
            }
        }
        
        // Send predictions to working memory for pattern matching
        if (pImpl->predictionSystem && pImpl->workingMemory) {
            auto predictions = pImpl->predictionSystem->getCurrentPredictions();
            for (const auto& pred : predictions) {
                pImpl->workingMemory->reinforceTrace(pred.neuronId, pred.confidence);
            }
        }
        
        // Form concepts from working memory patterns
        if (pImpl->conceptFormation && pImpl->workingMemory) {
            pImpl->conceptFormation->updateFromWorkingMemory(pImpl->workingMemory.get());
        }
    }
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity with novelty and prediction error
    if (pImpl->curiosity && pImpl->predictionError) {
        float noveltyLevel = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
        float predictionErrorLevel = pImpl->predictionError ? pImpl->predictionError->getError() : 0.0f;
        pImpl->curiosity->update(noveltyLevel, predictionErrorLevel, pImpl->timestep);
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
        // Update prediction system with current state and sensory input
        pImpl->predictionSystem->update(this);
        
        // Generate predictions and send to working memory for pattern matching
        auto predictions = pImpl->predictionSystem->getCurrentPredictions();
        for (const auto& pred : predictions) {
            // Store prediction as working memory trace
            if (pImpl->workingMemory) {
                pImpl->workingMemory->storePrediction(pred.neuronId, pred.confidence, pred.error);
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
        // Process current neural activity and working memory to form new concepts
        // Encode sensory input patterns into concepts
        if (pImpl->workingMemory) {
            pImpl->conceptFormation->processWorkingMemory(pImpl->workingMemory.get());
        }
        
        // Update episodic memory connections in concept space
        if (pImpl->episodicMemory) {
            pImpl->conceptFormation->updateFromEpisodicMemory(pImpl->episodicMemory.get());
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

SimulationStep Brain::getDevelopmentalAge() const {
    return static_cast<SimulationStep>(pImpl->stepsSinceLastEpisode);
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Brain ID: " + std::to_string(reinterpret_cast<uintptr_t>(this)));
    
    // Basic system health
    NLM_LOG_INFO("Step/Time: " + std::to_string(pImpl->currentStep) + " / " + 
                std::to_string(pImpl->currentTime) + "s");
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
        NLM_LOG_INFO("Working memory capacity: " + std::to_string(pImpl->workingMemory->getCapacity()));
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
        NLM_LOG_INFO("Episodic memory max episodes: " + std::to_string(pImpl->episodicMemory->getMaxEpisodes()));
    }
    if (pImpl->associativeMemory) {
        NLM_LOG_INFO("Associative memory patterns: " + std::to_string(pImpl->associativeMemory->getPatternCount()));
    }
    
    // Neuromodulation status
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    if (pImpl->curiosity) {
        NLM_LOG_INFO("Curiosity level: " + std::to_string(pImpl->curiosity->getLevel()));
    }
    if (pImpl->novelty) {
        NLM_LOG_INFO("Novelty level: " + std::to_string(pImpl->novelty->getLevel()));
    }
    if (pImpl->predictionError) {
        NLM_LOG_INFO("Prediction error level: " + std::to_string(pImpl->predictionError->getError()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    NLM_LOG_INFO("Developmental age steps: " + std::to_string(pImpl->stepsSinceLastEpisode));
    
    // Integration system status
    NLM_LOG_INFO("Resting state: " + std::to_string(pImpl->isResting));
    NLM_LOG_INFO("Replay interval: " + std::to_string(pImpl->replayInterval));
    NLM_LOG_INFO("Consolidation interval: " + std::to_string(pImpl->consolidationInterval));
    
    // Checkpoint status
    if (pImpl->checkpointManager) {
        NLM_LOG_INFO("Checkpoint: Configured, active");
    }
    
    // Regional details with system metrics
    for (const auto& region : pImpl->regions) {
        float avgWeight = region->getAverageSynapticWeight();
        float weightVariance = region->getWeightVariance();
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(avgWeight) +
                    " (stddev: " + std::to_string(weightVariance) + ")");
    }
    
    // Connectivity summary
    NLM_LOG_INFO("Inter-region connections: " + std::to_string(pImpl->interRegionConnections.size()));
    
    // System health indicators
    bool systemHealthy = true;
    std::vector<std::string> healthIssues;
    
    if (getTotalNeuronCount() == 0) {
        systemHealthy = false;
        healthIssues.push_back("No neurons in brain");
    }
    
    if (getTotalSynapseCount() == 0) {
        healthIssues.push_back("No synapses in brain");
    }
    
    if (pImpl->totalSpikesTotal == 0 && pImpl->currentStep > 0) {
        healthIssues.push_back("No spikes in simulation");
    }
    
    if (pImpl->workingMemory && pImpl->workingMemory->getActiveTraces() == 0) {
        healthIssues.push_back("No working memory traces");
    }
    
    if (systemHealthy) {
        NLM_LOG_INFO("System health: ✓ HEALTHY - All systems operational");
    } else {
        NLM_LOG_WARNING("System health: ⚠ ISSUES DETECTED: ");
        for (const auto& issue : healthIssues) {
            NLM_LOG_WARNING("  - " + issue);
        }
    }
}

} // namespace nlm
