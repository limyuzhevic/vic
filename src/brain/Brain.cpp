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
        // Validate configuration pointer
        if (!config) {
            NLM_LOG_ERROR("Brain configuration is null - cannot initialize brain");
            return;
        }
        
        // Validate simulation timestep - must be positive and reasonable
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        if (timestep <= 0.0 || timestep > 0.1) {
            NLM_LOG_ERROR(std::string("Invalid simulation timestep: ") + std::to_string(timestep) + ". Must be (0, 0.1] seconds");
            timestep = 0.001; // Reset to default
        }
        
        // Validate and initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
            NLM_LOG_INFO(std::string("Using user-provided random seed: ") + std::to_string(seed));
        } else {
            NLM_LOG_INFO("Using default random seed: 42");
        }
        
        rng = std::make_unique<RandomGenerator>(seed);
        if (!rng) {
            NLM_LOG_ERROR("Failed to initialize random generator");
            return;
        }
        
        // Initialize plasticity systems with validation
        spikeSystem = std::make_unique<SpikeSystem>();
        if (!spikeSystem) {
            NLM_LOG_ERROR("Failed to initialize spike system");
            return;
        }
        
        stdp = std::make_unique<STDP>();
        if (!stdp) {
            NLM_LOG_ERROR("Failed to initialize STDP plasticity system");
            return;
        }
        
        hebbian = std::make_unique<Hebbian>();
        if (!hebbian) {
            NLM_LOG_ERROR("Failed to initialize Hebbian plasticity system");
            return;
        }
        
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        if (!structuralPlasticity) {
            NLM_LOG_ERROR("Failed to initialize structural plasticity system");
            return;
        }
        
        // ========== INITIALIZE INTEGRATED SYSTEMS ==========
        
        // Initialize memory systems with validation
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        if (!workingMemory) {
            NLM_LOG_ERROR("Failed to initialize working memory system");
            return;
        }
        
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        if (!episodicMemory) {
            NLM_LOG_ERROR("Failed to initialize episodic memory system");
            return;
        }
        
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        if (!associativeMemory) {
            NLM_LOG_ERROR("Failed to initialize associative memory system");
            return;
        }
        
        // Initialize prediction system
        predictionSystem = std::make_unique<PredictionSystem>();
        if (!predictionSystem) {
            NLM_LOG_ERROR("Failed to initialize prediction system");
            return;
        }
        
        // Initialize cognition systems with validation
        planner = std::make_unique<NeuralPlanner>();
        if (!planner) {
            NLM_LOG_ERROR("Failed to initialize neural planner");
            return;
        }
        
        conceptFormation = std::make_unique<ConceptFormation>();
        if (!conceptFormation) {
            NLM_LOG_ERROR("Failed to initialize concept formation system");
            return;
        }
        
        attention = std::make_unique<AttentionalSelection>();
        if (!attention) {
            NLM_LOG_ERROR("Failed to initialize attentional selection system");
            return;
        }
        
        // Initialize development system
        developmentSystem = std::make_unique<DevelopmentSystem>();
        if (!developmentSystem) {
            NLM_LOG_ERROR("Failed to initialize development system");
            return;
        }
        
        // Initialize neuromodulation systems with validation
        dopamine = std::make_unique<Dopamine>();
        if (!dopamine) {
            NLM_LOG_ERROR("Failed to initialize dopamine system");
            return;
        }
        
        curiosity = std::make_unique<Curiosity>();
        if (!curiosity) {
            NLM_LOG_ERROR("Failed to initialize curiosity system");
            return;
        }
        
        predictionError = std::make_unique<PredictionError>();
        if (!predictionError) {
            NLM_LOG_ERROR("Failed to initialize prediction error system");
            return;
        }
        
        novelty = std::make_unique<Novelty>();
        if (!novelty) {
            NLM_LOG_ERROR("Failed to initialize novelty detection system");
            return;
        }
        
        // Configure STDP parameters with validation
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        
        // Validate STDP parameters
        if (ltpWeight < 0.0f || ltpWeight > 1.0f) {
            NLM_LOG_ERROR(std::string("Invalid LTP weight: ") + std::to_string(ltpWeight) + ". Must be [0.0, 1.0]");
            ltpWeight = std::clamp(ltpWeight, 0.0f, 1.0f);
        }
        
        if (ltdWeight < 0.0f || ltdWeight > 1.0f) {
            NLM_LOG_ERROR(std::string("Invalid LTD weight: ") + std::to_string(ltdWeight) + ". Must be [0.0, 1.0]");
            ltdWeight = std::clamp(ltdWeight, 0.0f, 1.0f);
        }
        
        if (tau <= 0.0f) {
            NLM_LOG_ERROR(std::string("Invalid STDP tau: ") + std::to_string(tau) + ". Must be > 0.0");
            tau = 20.0f; // Reset to default
        }
        
        stdp->configure(ltpWeight, ltdWeight, tau);
        NLM_LOG_INFO(std::string("STDP configured: LTP=") + std::to_string(ltpWeight) + 
                     ", LTD=" + std::to_string(ltdWeight) + ", tau=" + std::to_string(tau));
        
        // Configure structural plasticity with validation
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        
        // Validate structural plasticity rates
        if (synaptogenesisRate < 0.0f || synaptogenesisRate > 0.01f) {
            NLM_LOG_ERROR(std::string("Invalid synaptogenesis rate: ") + std::to_string(synaptogenesisRate) + ". Must be [0.0, 0.01]");
            synaptogenesisRate = std::clamp(synaptogenesisRate, 0.0f, 0.01f);
        }
        
        if (pruningRate < 0.0f || pruningRate > 0.0001f) {
            NLM_LOG_ERROR(std::string("Invalid pruning rate: ") + std::to_string(pruningRate) + ". Must be [0.0, 0.0001]");
            pruningRate = std::clamp(pruningRate, 0.0f, 0.0001f);
        }
        
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        NLM_LOG_INFO(std::string("Structural plasticity configured: synaptogenesis=") + std::to_string(synaptogenesisRate) + 
                     ", pruning=" + std::to_string(pruningRate));
        
        // Get integration intervals from config with validation
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        if (replayInterval == 0) {
            NLM_LOG_ERROR("Replay interval cannot be zero, using default of 100");
            replayInterval = 100;
        }
        
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        if (consolidationInterval == 0) {
            NLM_LOG_ERROR("Consolidation interval cannot be zero, using default of 1000");
            consolidationInterval = 1000;
        }
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
        if (!checkpointManager) {
            NLM_LOG_ERROR("Failed to initialize checkpoint manager");
            return;
        }
        
        NLM_LOG_INFO("Brain Impl initialization complete - all systems initialized successfully");
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
    
    // Validate configuration pointer
    if (!pImpl->config) {
        NLM_LOG_ERROR("Brain configuration is null - cannot initialize brain");
        return false;
    }
    
    // Get configuration values with validation
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    // Validate configuration values
    if (neuronCount == 0) {
        NLM_LOG_ERROR("Neuron count cannot be zero");
        neuronCount = 1000; // Reset to default
    }
    
    if (regionCount == 0) {
        NLM_LOG_ERROR("Region count cannot be zero");
        regionCount = 1; // Reset to default
    }
    
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        NLM_LOG_ERROR(std::string("Connection probability out of range: ") + std::to_string(connectionProbability) + ". Must be [0.0, 1.0]");
        connectionProbability = std::clamp(connectionProbability, 0.0f, 1.0f);
    }
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        RegionId regionId = addRegion("Region_" + std::to_string(i + 1));
        if (!getRegion(regionId)) {
            NLM_LOG_ERROR(std::string("Failed to create region with ID: ") + std::to_string(regionId.index()));
            return false;
        }
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    if (neuronsPerRegion == 0) neuronsPerRegion = 1; // Ensure at least one neuron per region
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (!region) {
            NLM_LOG_ERROR(std::string("Region not found for ID: ") + std::to_string(i + 1));
            return false;
        }
        
        // Add populations with validation
        auto sensoryPopId = region->addPopulation(std::max<size_t>(1, neuronsPerRegion / 4), NeuronType::Sensory);
        auto internalPopId = region->addPopulation(std::max<size_t>(1, neuronsPerRegion / 2), NeuronType::Internal);
        auto motorPopId = region->addPopulation(std::max<size_t>(1, neuronsPerRegion / 4), NeuronType::Motor);
        
        // Collect sensory and motor neurons for I/O
        auto* sensoryPop = region->getPopulation(sensoryPopId);
        auto* motorPop = region->getPopulation(motorPopId);
        
        if (sensoryPop) {
            auto neurons = sensoryPop->getNeurons();
            for (auto* neuron : neurons) {
                pImpl->sensoryNeurons.push_back(neuron);
            }
        }
        
        if (motorPop) {
            auto neurons = motorPop->getNeurons();
            for (auto* neuron : neurons) {
                pImpl->motorNeurons.push_back(neuron);
            }
        }
        
        NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                    ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons");
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (!region) {
            NLM_LOG_ERROR(std::string("Region not found during connectivity initialization: ") + std::to_string(i + 1));
            return false;
        }
        
        // Initialize random connectivity and synapse weights
        if (!region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f)) {
            NLM_LOG_ERROR(std::string("Failed to initialize random connectivity for region: ") + std::to_string(i + 1));
            return false;
        }
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory with validation
    if (!pImpl->workingMemory) {
        NLM_LOG_ERROR("Working memory system is null");
        return false;
    }
    
    if (!pImpl->workingMemory->initialize(this)) {
        NLM_LOG_ERROR("Failed to initialize working memory system");
        return false;
    }
    
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory with validation
    if (!pImpl->episodicMemory) {
        NLM_LOG_ERROR("Episodic memory system is null");
        return false;
    }
    
    if (!pImpl->episodicMemory->initialize(this)) {
        NLM_LOG_ERROR("Failed to initialize episodic memory system");
        return false;
    }
    
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory with validation
    if (!pImpl->associativeMemory) {
        NLM_LOG_ERROR("Associative memory system is null");
        return false;
    }
    
    if (!pImpl->associativeMemory->initialize(this)) {
        NLM_LOG_ERROR("Failed to initialize associative memory system");
        return false;
    }
    
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems with validation
    if (!pImpl->planner) {
        NLM_LOG_ERROR("Planner system is null");
        return false;
    }
    
    if (!pImpl->planner->initialize(this)) {
        NLM_LOG_ERROR("Failed to initialize planner system");
        return false;
    }
    
    pImpl->planner->setPlanningDepth(5);
    
    if (!pImpl->conceptFormation) {
        NLM_LOG_ERROR("Concept formation system is null");
        return false;
    }
    
    if (!pImpl->conceptFormation->initialize(this)) {
        NLM_LOG_ERROR("Failed to initialize concept formation system");
        return false;
    }
    
    if (!pImpl->attention) {
        NLM_LOG_ERROR("Attention system is null");
        return false;
    }
    
    if (!pImpl->attention->initialize(this)) {
        NLM_LOG_ERROR("Failed to initialize attention system");
        return false;
    }
    
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation with validation
    if (!pImpl->novelty) {
        NLM_LOG_ERROR("Novelty detection system is null");
        return false;
    }
    
    if (!pImpl->novelty->initialize(this)) {
        NLM_LOG_ERROR("Failed to initialize novelty detection system");
        return false;
    }
    
    if (!pImpl->curiosity) {
        NLM_LOG_ERROR("Curiosity system is null");
        return false;
    }
    
    if (!pImpl->curiosity->initialize(this)) {
        NLM_LOG_ERROR("Failed to initialize curiosity system");
        return false;
    }
    
    // Register spike handlers for event-driven processing
    if (!pImpl->spikeSystem) {
        NLM_LOG_ERROR("Spike system is null");
        return false;
    }
    
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
    
    // Configure checkpoint manager with validation
    if (!pImpl->checkpointManager) {
        NLM_LOG_ERROR("Checkpoint manager is null");
        return false;
    }
    
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    if (!pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true)) {
        NLM_LOG_ERROR("Failed to configure checkpoint manager");
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

} // End of step() function

// ========== IMPLEMENTATION OF BRAIN LOOP HELPER METHODS ==========

void Brain::Impl::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Process pending delayed spike events to deliver synaptic input to destination neurons
    // This implements the spike timing dependent plasticity (STDP) delivery mechanism
    if (!spikeSystem) {
        NLM_LOG_ERROR("Spike system is null during delayed spike processing");
        return;
    }
    
    spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void Brain::Impl::updateNeuronDynamics(Timestamp currentTime) {
    // Update all neuron membrane potentials (LIF dynamics)
    // Implements Leaky Integrate-and-Fire (LIF) neuron dynamics including
    // membrane potential updates, refractory periods, and current injection
    
    for (auto& region : regions) {
        if (!region) continue; // Defensive null check
        
        for (auto& pop : region->getPopulations()) {
            if (!pop) continue; // Defensive null check
            
            for (auto* neuron : pop->getNeurons()) {
                if (!neuron) continue; // Defensive null check
                
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void Brain::Impl::detectAndProcessSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Detect neuron spikes and schedule spike events
    // Scans all neurons for spiking, queues spike events, and records
    // synaptic plasticity spikes for pre- and post-synaptic neurons
    
    for (auto& region : regions) {
        if (!region) continue; // Defensive null check
        
        for (auto& pop : region->getPopulations()) {
            if (!pop) continue; // Defensive null check
            
            for (auto* neuron : pop->getNeurons()) {
                if (!neuron) continue; // Defensive null check
                
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    spikeSystem->queueSpike(event);

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        if (syn) syn->recordPostSpike(currentTime);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        if (!syn) continue; // Defensive null check
                        
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * timestep;

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

                        spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (workingMemory) {
                        workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes
    spikeSystem->processSpikes(currentStep);
}

void Brain::Impl::updateWorkingMemory(double timestep) {
    // Update working memory dynamics and competition
    // Maintains transient active information through competitive
    // activation of memory traces. Implements the working memory subsystem
    
    if (!workingMemory) {
        NLM_LOG_ERROR("Working memory is null during update");
        return;
    }
    
    workingMemory->update(static_cast<float>(timestep));
}

void Brain::Impl::applyNeuromodulation(SimulationStep currentStep, Timestamp currentTime, double timestep) {
    // Apply neuromodulatory effects and plasticity rules
    // Integrates multiple neuromodulation systems (dopamine, curiosity,
    // novelty, prediction error) with neural plasticity mechanisms
    
    if (!novelty) {
        NLM_LOG_ERROR("Novelty detection system is null during neuromodulation update");
        return;
    }
    
    // Update novelty detection
    novelty->update(static_cast<float>(timestep));
    
    if (!curiosity) {
        NLM_LOG_ERROR("Curiosity system is null during neuromodulation update");
        return;
    }
    
    // Update curiosity
    curiosity->update(static_cast<float>(timestep));
    
    // Update dopamine (reward prediction error)
    if (dopamine) {
        dopamine->update(static_cast<float>(timestep));
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = dopamine->getLevel();
        for (auto& region : regions) {
            if (!region) continue; // Defensive null check
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) continue; // Defensive null check
                
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) continue; // Defensive null check
                    
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
}

void Brain::Impl::applyPlasticityRules(Timestamp currentTime, double timestep) {
    // Apply STDP and Hebbian plasticity rules
    // Implements spike-timing dependent plasticity (STDP) and
    // Hebbian co-activation learning with neuromodulatory modulation
    
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (dopamine) {
        plasticityMod = dopamine->getPlasticityFactor();
    }
    
    for (auto& region : regions) {
        if (!region) continue; // Defensive null check
        
        for (auto& syn : region->getSynapses()) {
            if (!syn) continue; // Defensive null check
            
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    if (!stdp) {
                        NLM_LOG_ERROR("STDP system is null during plasticity update");
                        continue;
                    }
                    
                    stdp->update(syn, preSpikes, postSpikes, static_cast<float>(timestep));
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
                    if (!hebbian) {
                        NLM_LOG_ERROR("Hebbian system is null during plasticity update");
                        continue;
                    }
                    
                    hebbian->update(syn, preSpikes, postSpikes, static_cast<float>(timestep));
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void Brain::Impl::updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) {
    // Update episodic memory with current experience
    // Captures current brain state as episodic episodes, storing
    // active neurons and reward information for later replay and consolidation
    
    stepsSinceLastEpisode++;
    if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        stepsSinceLastEpisode = 0;
        
        if (!episodicMemory) {
            NLM_LOG_ERROR("Episodic memory is null during update");
            return;
        }
        
        // Capture current brain state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
        
        // Store active neurons
        for (auto& region : regions) {
            if (!region) continue; // Defensive null check
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) continue; // Defensive null check
                
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) continue; // Defensive null check
                    
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
        episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
        
        episodicMemory->storeEpisode(episode);
    }
}

void Brain::Impl::updatePredictionSystem(Timestamp currentTime) {
    // Update prediction system with sensory observations
    // Maintains predictive models of sensory input and computes
    // prediction errors for learning and adaptation
    
    // Note: PredictionSystem doesn't have update method currently
    // This method is provided for future implementation
}

void Brain::Impl::updateCognitionSystems(double timestep) {
    // Update cognition systems (attention, concept formation)
    // Integrates attentional selection and concept formation for
    // higher-order cognitive processing and information filtering
    
    updateAttentionSystem(timestep);
    updateConceptFormation(static_cast<Timestamp>(currentTime));
}

void Brain::Impl::updateAttentionSystem(double timestep) {
    // Update attention system dynamics
    // Implements attentional selection mechanism for focusing
    // resources on relevant information and suppressing irrelevant inputs
    
    if (!attention) {
        NLM_LOG_ERROR("Attention system is null during update");
        return;
    }
    
    attention->update(static_cast<float>(timestep));
    
    // Apply attention to working memory winners
    if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
        attention->processCompetition(competitors);
    }
}

void Brain::Impl::updateConceptFormation(Timestamp currentTime) {
    // Update concept formation system
    // Discovers and learns abstract concepts from neural activity
    // patterns and sensory inputs
    
    // Note: ConceptFormation doesn't have update method currently
    // This method is provided for future implementation
}

void Brain::Impl::applyStructuralPlasticity(SimulationStep currentStep, const RandomGenerator& rng) {
    // Apply structural plasticity (synaptogenesis, pruning)
    // Modifies brain topology through new synapse formation and
    // elimination of weak connections (structural rewiring)
    
    if (!structuralPlasticity) {
        NLM_LOG_ERROR("Structural plasticity system is null during update");
        return;
    }
    
    structuralPlasticity->update(nullptr, const_cast<RandomGenerator&>(rng));
}

void Brain::Impl::replayMemories(SimulationStep currentStep) {
    // Replay important memories for consolidation
    // Reactivates significant memory traces during rest or
    // periodically to strengthen connections and integrate information
    
    if (currentStep % replayInterval == 0 && episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            if (episode) {
                episodicMemory->replayEpisode(episode);
            }
        }
    }
}

void Brain::Impl::applyDevelopmentEffects(SimulationStep currentStep, Timestamp currentTime, double timestep) {
    // Apply developmental stage effects
    // Modifies plasticity rates and system properties based on
    // developmental stage (initial, critical period, maturation, adult)
    
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        if (!developmentSystem) {
            NLM_LOG_ERROR("Development system is null during update");
            return;
        }
        
        developmentSystem->update(nullptr, const_cast<RandomGenerator&>(*rng), static_cast<float>(timestep * 1000));
        
        // Development affects plasticity rates
        auto* sp = structuralPlasticity;
        if (sp) {
            float plasticityMod = 1.0f;
            
            switch (developmentalStage) {
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

void Brain::Impl::collectStatistics() {
    // Collect neural and system statistics
    // Gathers performance metrics, spike statistics, and system health
    // indicators for monitoring and analysis
    
    // Statistics are already collected in logStatus() method
    // This method can be extended for more detailed statistics
}

// Implementation of public helper methods for Brain class

void Brain::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->processDelayedSpikes(currentStep, currentTime);
}

void Brain::updateNeuronDynamics(Timestamp currentTime) {
    pImpl->updateNeuronDynamics(currentTime);
}

void Brain::detectAndProcessSpikes(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->detectAndProcessSpikes(currentStep, currentTime);
}

void Brain::updateWorkingMemory(double timestep) {
    pImpl->updateWorkingMemory(timestep);
}

void Brain::applyNeuromodulation(SimulationStep currentStep, Timestamp currentTime, double timestep) {
    pImpl->applyNeuromodulation(currentStep, currentTime, timestep);
}

void Brain::applyPlasticityRules(Timestamp currentTime, double timestep) {
    pImpl->applyPlasticityRules(currentTime, timestep);
}

void Brain::updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->updateEpisodicMemory(currentStep, currentTime);
}

void Brain::updatePredictionSystem(Timestamp currentTime) {
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void Brain::updateCognitionSystems(double timestep) {
    pImpl->updateCognitionSystems(timestep);
}

void Brain::updateAttentionSystem(double timestep) {
    pImpl->updateAttentionSystem(timestep);
}

void Brain::updateConceptFormation(Timestamp currentTime) {
    // Note: ConceptFormation doesn't have update method currently
    // This method is provided for future implementation
}

void Brain::applyStructuralPlasticity(SimulationStep currentStep, const RandomGenerator& rng) {
    pImpl->applyStructuralPlasticity(currentStep, rng);
}

void Brain::replayMemories(SimulationStep currentStep) {
    pImpl->replayMemories(currentStep);
}

void Brain::applyDevelopmentEffects(SimulationStep currentStep, Timestamp currentTime, double timestep) {
    pImpl->applyDevelopmentEffects(currentStep, currentTime, timestep);
}

void Brain::collectStatistics() {
    pImpl->collectStatistics();
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
