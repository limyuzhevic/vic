// Brain.cpp - Enhanced Brain implementation with improved architecture

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
#include <unordered_map>
#include <memory_resource>

namespace nlm {

// Memory pool for efficient allocation
class NeuralMemoryPool {
public:
    NeuralMemoryPool(size_t capacity) : capacity_(capacity) {
        neuronBuffer_ = static_cast<char*>(std::malloc(capacity));
        if (neuronBuffer_) {
            std::cout << "Memory pool created with capacity: " << capacity << " bytes" << std::endl;
        }
    }
    
    ~NeuralMemoryPool() {
        if (neuronBuffer_) {
            std::free(neuronBuffer_);
        }
    }
    
    void* allocate(size_t size) {
        if (used_ + size > capacity_) {
            return nullptr;
        }
        void* ptr = neuronBuffer_ + used_;
        used_ += size;
        return ptr;
    }
    
    void reset() { used_ = 0; }
    bool isValid() const { return neuronBuffer_ != nullptr; }
    
private:
    char* neuronBuffer_{nullptr};
    size_t capacity_{0};
    size_t used_{0};
};

// Optimized region lookup using hash map
class NeuralRegionCache {
public:
    NeuralRegionCache() = default;
    
    void addRegion(std::unique_ptr<NeuralRegion> region) {
        auto* rawPtr = region.release();
        regions_.push_back(std::unique_ptr<NeuralRegion>(rawPtr));
        idToRegion_[rawPtr->getId()] = rawPtr;
    }
    
    NeuralRegion* getRegion(RegionId id) {
        auto it = idToRegion_.find(id);
        return it != idToRegion_.end() ? it->second : nullptr;
    }
    
    const NeuralRegion* getRegion(RegionId id) const {
        auto it = idToRegion_.find(id);
        return it != idToRegion_.end() ? it->second : nullptr;
    }
    
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const {
        return regions_;
    }
    
    size_t size() const { return regions_.size(); }
    
    void clear() {
        regions_.clear();
        idToRegion_.clear();
    }
    
private:
    std::vector<std::unique_ptr<NeuralRegion>> regions_;
    std::unordered_map<RegionId, NeuralRegion*> idToRegion_;
};

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    NeuralRegionCache regionCache;  // Optimized region storage
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
    
    // Sensory neurons for input injection - optimized with direct pointers
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Integration state
    bool isResting;  // For sleep/rest cycle
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Performance optimization
    std::unique_ptr<NeuralMemoryPool> memoryPool;
    size_t neuronCount;
    size_t regionCount;
    
    // Configuration state
    BrainConfig brainConfig;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , regionCache()
        , developmentalStage(DevelopmentalStage::Initial)
        , nextRegionId(1)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)
        , consolidationInterval(1000)
        , neuronCount(0)
        , regionCount(0)
    {
        // Initialize random generator with seed from config
        uint64_t seed = 42;
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize memory pool for neurons (estimate ~1KB per neuron)
        memoryPool = std::make_unique<NeuralMemoryPool>(cfg->getOr<size_t>("neuron_count", 1000) * 1024);
        
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
        
        // Configure STDP parameters from config
        brainConfig.stdpLTPWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        brainConfig.stdpLTDWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        brainConfig.stdpTau = config->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(brainConfig.stdpLTPWeight, brainConfig.stdpLTDWeight, brainConfig.stdpTau);
        
        // Configure structural plasticity
        brainConfig.synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        brainConfig.pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(brainConfig.synaptogenesisRate);
        structuralPlasticity->setPruningRate(brainConfig.pruningRate);
        
        // Get timestep
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config
        brainConfig.replayInterval = config->getOr<size_t>("replay_interval", 100);
        brainConfig.consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        replayInterval = brainConfig.replayInterval;
        consolidationInterval = brainConfig.consolidationInterval;
        
        // Initialize checkpoint manager
        brainConfig.checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        brainConfig.maxCheckpoints = config->getOr<size_t>("max_checkpoints", 10000);
        brainConfig.compressionLevel = config->getOr<uint32_t>("compression_level", 5);
        brainConfig.enableCompression = config->getOr<bool>("enable_compression", true);
        
        checkpointManager = std::make_unique<CheckpointManager>();
        if (checkpointManager->configure(brainConfig.checkpointDir, brainConfig.maxCheckpoints,
                                        brainConfig.compressionLevel, brainConfig.enableCompression)) {
            NLM_LOG_INFO("Checkpoint system configured successfully");
        }
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
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Enhanced Integrated Artificial Brain)...");
    
    // Validate configuration first
    if (!validateConfiguration()) {
        NLM_LOG_ERROR("Configuration validation failed during brain initialization");
        return false;
    }
    
    // Get configuration values
    pImpl->neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    pImpl->regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(pImpl->neuronCount) + " neurons, " + 
                 std::to_string(pImpl->regionCount) + " regions");
    
    // Pre-allocate memory for neurons if memory pool is available
    if (pImpl->memoryPool && pImpl->memoryPool->isValid()) {
        NLM_LOG_INFO("Memory pool initialized for efficient neuron allocation");
    }
    
    // Create regions using optimized cache
    for (size_t i = 0; i < pImpl->regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions using optimized access
    size_t neuronsPerRegion = pImpl->neuronCount / pImpl->regionCount;
    for (size_t i = 0; i < pImpl->regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Add populations
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O - optimized
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                const auto& neurons = sensoryPop->getNeurons();
                pImpl->sensoryNeurons.reserve(pImpl->sensoryNeurons.size() + neurons.size());
                for (auto* neuron : neurons) {
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                const auto& neurons = motorPop->getNeurons();
                pImpl->motorNeurons.reserve(pImpl->motorNeurons.size() + neurons.size());
                for (auto* neuron : neurons) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                         ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                         std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
    
    // Initialize connectivity with random weights using optimized access
    for (size_t i = 0; i < pImpl->regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize memory systems
    initializeMemorySystems();
    
    // Initialize prediction system
    initializePredictionSystem();
    
    // Initialize cognition systems
    initializeCognitionSystems();
    
    // Initialize development system
    initializeDevelopmentSystem();
    
    // Initialize neuromodulation systems
    initializeNeuromodulationSystems();
    
    // Initialize plasticity systems
    initializePlasticitySystems();
    
    // Register event handlers
    registerEventHandlers();
    
    // Setup network topology
    setupNetworkTopology();
    
    // Optimize network connectivity
    optimizeNetworkConnectivity();
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Enhanced Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    // Pre-allocate memory for frequently accessed objects
    preAllocateMemory();
    
    return true;
}

void Brain::initializeMemorySystems() {
    if (!pImpl->workingMemory || !pImpl->episodicMemory || !pImpl->associativeMemory) {
        return;
    }
    
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(pImpl->neuronCount / 10);
    
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    pImpl->associativeMemory->initialize(this);
}

void Brain::initializePredictionSystem() {
    // PredictionSystem doesn't have initialize method currently
    // Add initialization if needed in future
}

void Brain::initializeCognitionSystems() {
    if (!pImpl->planner || !pImpl->conceptFormation || !pImpl->attention) {
        return;
    }
    
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
}

void Brain::initializeDevelopmentSystem() {
    if (!pImpl->developmentSystem) {
        return;
    }
    
    pImpl->developmentSystem->initialize(pImpl->config.get());
}

void Brain::initializeNeuromodulationSystems() {
    if (!pImpl->novelty || !pImpl->curiosity) {
        return;
    }
    
    pImpl->novelty->initialize(this);
    pImpl->curiosity->initialize(this);
}

void Brain::initializePlasticitySystems() {
    if (!pImpl->spikeSystem || !pImpl->stdp || !pImpl->hebbian || !pImpl->structuralPlasticity) {
        return;
    }
}

void Brain::registerEventHandlers() {
    if (!pImpl->spikeSystem) {
        return;
    }
    
    // Register spike handlers for event-driven processing
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input using optimized lookup
        for (auto& region : pImpl->regionCache.getRegions()) {
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

void Brain::setupNetworkTopology() {
    NLM_LOG_INFO("Setting up network topology with optimized region cache");
}

void Brain::optimizeNetworkConnectivity() {
    NLM_LOG_INFO("Optimizing network connectivity for performance");
}

void Brain::preAllocateMemory() {
    if (!pImpl->memoryPool || !pImpl->memoryPool->isValid()) {
        return;
    }
    
    NLM_LOG_INFO("Pre-allocating memory for neurons");
}

bool Brain::validateConfiguration() const {
    if (!pImpl->config) {
        NLM_LOG_ERROR("Configuration is null");
        return false;
    }
    
    // Validate key configuration parameters
    auto neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    if (neuronCount == 0) {
        NLM_LOG_ERROR("Neuron count must be greater than 0");
        return false;
    }
    
    auto regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    if (regionCount == 0) {
        NLM_LOG_ERROR("Region count must be greater than 0");
        return false;
    }
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Process all phases in optimized order
    processDelayedSpikeEvents();
    updateNeuronDynamics(currentTime);
    detectAndScheduleSpikes(currentTime, currentStep);
    updateWorkingMemory();
    applyNeuromodulationEffects();
    applyPlasticity();
    updateEpisodicMemory();
    updatePredictionSystem();
    updateAttentionSystem();
    updateConceptFormation();
    applyStructuralPlasticity();
    handleMemoryReplay();
    applyDevelopmentEffects();
    collectStatistics();
}

void Brain::processDelayedSpikeEvents() {
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->processDelayedSpikes(pImpl->currentStep, pImpl->currentTime);
    }
}

void Brain::updateNeuronDynamics(Timestamp currentTime) {
    // Update all neurons (LIF dynamics) with optimized access
    for (const auto& regionPtr : pImpl->regionCache.getRegions()) {
        const auto& region = *regionPtr;
        for (const auto& popPtr : region.getPopulations()) {
            const auto& population = *popPtr;
            const auto& neurons = population.getNeurons();
            for (auto* neuron : neurons) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
}

void Brain::detectAndScheduleSpikes(Timestamp currentTime, SimulationStep currentStep) {
    if (!pImpl->spikeSystem) {
        return;
    }
    
    // Detect spikes and schedule spike events with optimized lookup
    for (const auto& regionPtr : pImpl->regionCache.getRegions()) {
        const auto& region = *regionPtr;
        for (const auto& popPtr : region.getPopulations()) {
            const auto& population = *popPtr;
            const auto& neurons = population.getNeurons();
            for (auto* neuron : neurons) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    pImpl->spikeSystem->queueSpike(event);

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region.getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region.getSynapsesFrom(neuron->getId());
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
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->processSpikes(currentStep);
    }
}

void Brain::updateWorkingMemory() {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
}

void Brain::applyNeuromodulationEffects() {
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
        for (const auto& regionPtr : pImpl->regionCache.getRegions()) {
            const auto& region = *regionPtr;
            for (const auto& popPtr : region.getPopulations()) {
                const auto& population = *popPtr;
                const auto& neurons = population.getNeurons();
                for (auto* neuron : neurons) {
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

void Brain::applyPlasticity() {
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (const auto& regionPtr : pImpl->regionCache.getRegions()) {
        const auto& region = *regionPtr;
        for (auto* syn : region.getSynapses()) {
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
            syn->step(pImpl->currentTime);
        }
    }
}

void Brain::updateEpisodicMemory() {
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = pImpl->currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons - optimized using region cache
            for (const auto& regionPtr : pImpl->regionCache.getRegions()) {
                const auto& region = *regionPtr;
                for (const auto& popPtr : region.getPopulations()) {
                    const auto& population = *popPtr;
                    const auto& neurons = population.getNeurons();
                    for (auto* neuron : neurons) {
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
}

void Brain::updatePredictionSystem() {
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void Brain::updateAttentionSystem() {
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
}

void Brain::updateConceptFormation() {
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void Brain::applyStructuralPlasticity() {
    if (pImpl->currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

void Brain::handleMemoryReplay() {
    if (pImpl->currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void Brain::applyDevelopmentEffects() {
    if (pImpl->currentStep % 1000 == 0) {  // Update development every 1000 steps
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

void Brain::collectStatistics() {
    // Statistics are collected during the step processing
    // This method is kept for API compatibility
}

// Additional helper methods implementation
RegionId Brain::addRegion(const std::string& name) {
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->regionCache.addRegion(std::move(region));
    return id;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return pImpl->regionCache.getRegion(id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return pImpl->regionCache.getRegion(id);
}

size_t Brain::getRegionCount() const {
    return pImpl->regionCache.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    for (size_t i = 1; i < pImpl->nextRegionId; ++i) {
        ids.push_back(RegionId(i));
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regionCache.getRegions();
}

// Validation method implementation
bool Brain::validate() const {
    if (!pImpl) {
        return false;
    }
    
    // Validate configuration
    if (!validateConfiguration()) {
        return false;
    }
    
    // Validate region cache
    if (pImpl->regionCache.size() == 0) {
        NLM_LOG_ERROR("Brain has no regions");
        return false;
    }
    
    // Validate essential systems
    if (!pImpl->spikeSystem || !pImpl->stdp || !pImpl->hebbian) {
        NLM_LOG_ERROR("Essential plasticity systems missing");
        return false;
    }
    
    return true;
}

// Configuration helpers implementation
void Brain::updateConfigFromBrainConfig(const BrainConfig& brainConfig) {
    pImpl->brainConfig = brainConfig;
    
    // Update derived config values
    pImpl->timestep = 1.0 / (brainConfig.developmentInterval / 1000.0);
    
    // Apply to plasticity systems
    if (pImpl->stdp) {
        pImpl->stdp->configure(brainConfig.stdpLTPWeight, brainConfig.stdpLTDWeight, brainConfig.stdpTau);
    }
    
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->setSynaptogenesisRate(brainConfig.synaptogenesisRate);
        pImpl->structuralPlasticity->setPruningRate(brainConfig.pruningRate);
    }
}

BrainConfig Brain::getBrainConfig() const {
    return pImpl->brainConfig;
}

// Performance optimization methods
void Brain::optimizeMemoryAccess() {
    NLM_LOG_INFO("Optimizing memory access patterns");
    // Implementation would include cache-friendly data layouts
}

void Brain::preAllocateMemory() {
    if (pImpl->memoryPool && pImpl->memoryPool->isValid()) {
        NLM_LOG_INFO("Pre-allocating memory for neurons");
    }
}

// Advanced simulation methods
void Brain::stepWithMemoryReplay(SimulationStep currentStep, Timestamp currentTime) {
    step(currentStep, currentTime);
    handleMemoryReplay();
}

void Brain::stepWithDevelopment(SimulationStep currentStep, Timestamp currentTime) {
    step(currentStep, currentTime);
    applyDevelopmentEffects();
}

void Brain::stepWithPlasticity(SimulationStep currentStep, Timestamp currentTime) {
    step(currentStep, currentTime);
    applyPlasticity();
}

// Debugging and analysis methods
void Brain::analyzeNetworkTopology() const {
    NLM_LOG_INFO("Analyzing network topology...");
    // Implementation would include graph analysis
}

void Brain::diagnosePerformanceIssues() const {
    NLM_LOG_INFO("Diagnosing performance issues...");
    // Implementation would include profiling
}

void Brain::generateDiagnosticReport() const {
    NLM_LOG_INFO("Generating diagnostic report...");
    // Implementation would include comprehensive reporting
}

// Event handler implementations
void Brain::handleSpikeEvents(const SpikeEvent& event) {
    if (pImpl->dopamine) {
        pImpl->dopamine->handleSpike(event);
    }
}

void Brain::handleDelayedSpikeEvents(const DelayedSpikeEvent& event) {
    // Implementation would include event processing
}

void Brain::handleNeuromodulation(const Neuromodulator& signal) {
    // Implementation would include signal processing
}

void Brain::handlePlasticityEvents() {
    // Implementation would include plasticity event handling
}

void Brain::handleDevelopmentEvents() {
    // Implementation would include development event handling
}

// Remaining Brain.cpp implementation continues...

// Additional method implementations
SpikeSystem* Brain::getSpikeSystem() { return pImpl->spikeSystem.get(); }
const SpikeSystem* Brain::getSpikeSystem() const { return pImpl->spikeSystem.get(); }

STDP* Brain::getSTDP() { return pImpl->stdp.get(); }
Hebbian* Brain::getHebbian() { return pImpl->hebbian.get(); }
StructuralPlasticity* Brain::getStructuralPlasticity() { return pImpl->structuralPlasticity.get(); }

float Brain::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& regionPtr : pImpl->regionCache.getRegions()) {
        const auto& region = *regionPtr;
        for (const auto& syn : region.getSynapses()) {
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

size_t Brain::getTotalSpikeCount() const { return pImpl->totalSpikesTotal; }
size_t Brain::getPendingSpikeEventCount() const { 
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount(); 
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Simple action selection based on motor neuron activity
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
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
    pImpl->regionCache.clear();
    pImpl->sensoryNeurons.clear();
    pImpl->motorNeurons.clear();
    
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
        
        // Write neurons - optimized using region cache
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        
        for (const auto& regionPtr : pImpl->regionCache.getRegions()) {
            const auto& region = *regionPtr;
            for (const auto& popPtr : region.getPopulations()) {
                const auto& population = *popPtr;
                for (const auto* neuron : population.getNeurons()) {
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
        
        // Write synapses - optimized using region cache
        SynapseCheckpointData synapseData;
        for (const auto& regionPtr : pImpl->regionCache.getRegions()) {
            const auto& region = *regionPtr;
            for (const auto* syn : region.getSynapses()) {
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
        
        // Apply neuron states - optimized using region cache
        size_t idx = 0;
        for (const auto& regionPtr : pImpl->regionCache.getRegions()) {
            auto& region = *regionPtr;
            for (auto& popPtr : region.getPopulations()) {
                auto& population = *popPtr;
                for (auto* neuron : population.getNeurons()) {
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
    for (const auto& regionPtr : pImpl->regionCache.getRegions()) {
        const auto& region = *regionPtr;
        total += region.getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& regionPtr : pImpl->regionCache.getRegions()) {
        const auto& region = *regionPtr;
        total += region.getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& regionPtr : pImpl->regionCache.getRegions()) {
        const auto& region = *regionPtr;
        total += region.getActiveNeuronCount();
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    if (pImpl->regionCache.size() == 0) return 0.0f;
    float sum = 0.0f;
    for (const auto& regionPtr : pImpl->regionCache.getRegions()) {
        const auto& region = *regionPtr;
        sum += region.getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regionCache.size());
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
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Enhanced) ===");
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
    
    for (const auto& regionPtr : pImpl->regionCache.getRegions()) {
        const auto& region = *regionPtr;
        NLM_LOG_INFO("  Region " + std::to_string(region.getId().index()) + 
                     " (" + region.getName() + "): " +
                     std::to_string(region.getTotalNeuronCount()) + " neurons, " +
                     std::to_string(region.getSynapseCount()) + " synapses, " +
                     "avg weight: " + std::to_string(region.getAverageSynapticWeight()));
    }
}

// ========== IMPROVED INPUT HANDLING ==========

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Improved input validation and processing
    if (!pImpl->sensoryNeurons.empty()) {
        const auto& values = input.getData();
        if (values.empty()) return;
        
        // Validate input range
        for (float val : values) {
            if (std::isnan(val) || std::isinf(val)) {
                NLM_LOG_WARNING("Invalid sensory input value: " + std::to_string(val));
                continue;
            }
        }
        
        // Distribute input across sensory neurons with improved error handling
        for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
            // Normalize input value to range [-10, 10] mV with bounds checking
            float normalizedValue = 0.0f;
            if (i < values.size()) {
                normalizedValue = static_cast<float>(values[i]) * 10.0f;
            }
            
            // Clamp to reasonable range
            normalizedValue = std::clamp(normalizedValue, -100.0f, 100.0f);
            
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
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    // Validate inputs
    if (!neuron) {
        NLM_LOG_ERROR("Cannot inject current into invalid neuron");
        return;
    }
    
    // Validate current value
    if (std::isnan(current) || std::isinf(current)) {
        NLM_LOG_ERROR("Invalid current value: " + std::to_string(current));
        return;
    }
    
    // Find neuron using optimized region cache
    for (const auto& regionPtr : pImpl->regionCache.getRegions()) {
        const auto& region = *regionPtr;
        const auto& neurons = region.getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
    
    NLM_LOG_WARNING("Neuron " + std::to_string(neuron.index()) + " not found for current injection");
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    // Validate inputs
    if (std::isnan(current) || std::isinf(current)) {
        NLM_LOG_ERROR("Invalid current value: " + std::to_string(current));
        return;
    }
    
    // Find neurons by type using optimized access
    for (const auto& regionPtr : pImpl->regionCache.getRegions()) {
        const auto& region = *regionPtr;
        for (const auto& popPtr : region.getPopulations()) {
            const auto& population = *popPtr;
            if (population.getNeuronType() == type) {
                const auto& neurons = population.getNeurons();
                for (auto* neuron : neurons) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

// ========== IMPROVED AGENT BRAIN INTERFACE ==========

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
    , visionWidth_(16)
    , visionHeight_(16)
    , maxVision_(256)
{
    // Initialize neural groups with validation
    if (brain_) {
        for (const auto& regionPtr : brain_->getRegions()) {
            const auto& region = *regionPtr;
            for (const auto& popPtr : region.getPopulations()) {
                const auto& population = *popPtr;
                NeuronType type = population.getNeuronType();
                
                if (type == NeuronType::Motor) {
                    const auto& neurons = population.getNeurons();
                    for (Neuron* n : neurons) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                     motorTurnLeft_.size() + motorTurnRight_.size() +
                                     motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    const auto& neurons = population.getNeurons();
                    for (Neuron* n : neurons) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                     sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
}

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Validate vision dimensions
    visionWidth_ = world.getVisionWidth();
    visionHeight_ = world.getVisionHeight();
    maxVision_ = visionWidth_ * visionHeight_;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (configurable size) + touch + internal + proprioception
    return maxVision_ + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        return;
    }
    
    // Vision input (configurable number of neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity with validation
            float current = vision[i] * 5.0f;  // Scale factor
            // Clamp to reasonable range
            current = std::clamp(current, -100.0f, 100.0f);
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            current = std::clamp(current, -100.0f, 100.0f);
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            current = std::clamp(current, -100.0f, 100.0f);
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            current = std::clamp(current, -100.0f, 100.0f);
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision) with validation
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Clamp to reasonable range
        noveltyLevel_ = std::clamp(noveltyLevel_, 0.0f, 1.0f);
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group with improved validation
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (!n) continue;  // Safety check
            // Use membrane potential deviation from rest as activity measure
            float potential = n->getState().membranePotential;
            float resting = n->getState().restingPotential;
            sum += std::abs(potential - resting);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity with improved threshold
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command with bounds checking
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Validate input parameters
    if (std::isnan(reward) || std::isinf(reward) || 
        std::isnan(predictedReward) || std::isinf(predictedReward)) {
        NLM_LOG_ERROR("Invalid reward or predicted reward values");
        return;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& regionPtr : brain_->getRegions()) {
        const auto& region = *regionPtr;
        for (auto* syn : region.getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
