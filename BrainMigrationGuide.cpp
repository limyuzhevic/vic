// Brain.cpp Migration to MemoryPool Integration
// This file shows the key changes needed to migrate from std::make_unique to MemoryPool

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
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../performance/MemoryPool.hpp"
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
    
    // NEW: MemoryPool integration
    std::unique_ptr<nlm::NeuronPool> neuronPool;
    std::unique_ptr<nlm::SynapsePool> synapsePool;
    bool useMemoryPool;
    
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
        , useMemoryPool(true)  // Enable MemoryPool integration
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
        
        // Initialize MemoryPool systems
        neuronPool = std::make_unique<nlm::NeuronPool>(config->getOr<size_t>("neuron_count", 1000));
        synapsePool = std::make_unique<nlm::SynapsePool>(config->getOr<size_t>("synapse_count", 10000));
        
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
    
    // Helper methods for MemoryPool integration
    Neuron* createNeuronWithMemoryPool(NeuronId id, NeuronType type, RegionId region, PopulationId population);
    Synapse* createSynapseWithMemoryPool(SynapseId id, NeuronId source, NeuronId destination);
    
    // Statistics for MemoryPool usage
    void updateMemoryPoolStats() const;
    
    // Migration helpers
    bool isMigrationEnabled() const { return useMemoryPool; }
    void enableMemoryPool(bool enable) { useMemoryPool = enable; }
    
    // Fallback methods for backward compatibility
    Neuron* legacyCreateNeuron(NeuronId id);
    Synapse* legacyCreateSynapse(SynapseId id, NeuronId source, NeuronId destination);
};

// Implementation of MemoryPool integration helper methods

Neuron* Brain::Impl::createNeuronWithMemoryPool(NeuronId id, NeuronType type, RegionId region, PopulationId population) {
    if (!neuronPool || !useMemoryPool) {
        return legacyCreateNeuron(id);
    }
    
    // Allocate neuron from MemoryPool
    NeuronId poolId = neuronPool->allocate();
    NeuronIndex index = neuronPool->getIndex(poolId);
    
    // Set neuron properties in SoA arrays
    NeuronState& state = neuronPool->getNeuronState(index);
    state.id = id;
    state.type = type;
    state.regionId = region;
    state.populationId = population;
    
    // Initialize neuron state
    state.membranePotential = -70.0f;
    state.restingPotential = -70.0f;
    state.threshold = -55.0f;
    state.resetPotential = -70.0f;
    state.leakConductance = 10.0f;
    state.synapticConductance = 0.0f;
    state.firingRate = 0.0f;
    state.firingState = FiringState::Resting;
    state.refractoryRemaining = 0;
    state.refractoryPeriod = 5;
    state.adaptationVariable = 0.0f;
    state.lastSpikeTime = -1.0f;
    
    // Create wrapper neuron for backward compatibility
    return new Neuron(id, index, neuronPool.get());
}

Synapse* Brain::Impl::createSynapseWithMemoryPool(SynapseId id, NeuronId source, NeuronId destination) {
    if (!synapsePool || !useMemoryPool) {
        return legacyCreateSynapse(id, source, destination);
    }
    
    // Allocate synapse from MemoryPool
    SynapseId poolId = synapsePool->allocate(source, destination);
    SynapseIndex index = synapsePool->getIndex(poolId);
    
    // Set synapse properties in SoA arrays
    SynapseState& state = synapsePool->getSynapseState(index);
    state.source = source;
    state.destination = destination;
    state.weight = 0.0f;
    state.delay = 1;
    state.type = SynapseType::Excitatory;
    state.plasticityFlags = PlasticityFlags();
    state.shortTermDepression = 1.0f;
    state.shortTermFacilitation = 0.0f;
    state.lastPreSpikeTime = -1.0f;
    state.lastPostSpikeTime = -1.0f;
    state.eligibilityTrace = 0.0f;
    state.efficacy = 1.0f;
    
    // Create wrapper synapse for backward compatibility
    return new Synapse(id, index, synapsePool.get());
}

void Brain::Impl::updateMemoryPoolStats() const {
    if (neuronPool) {
        NLM_LOG_INFO("NeuronPool: " + std::to_string(neuronPool->size()) + 
                    " neurons, " + std::to_string(neuronPool->allocatedCount()) + 
                    " allocated, " + std::to_string(neuronPool->memoryUsage()) + " bytes");
    }
    if (synapsePool) {
        NLM_LOG_INFO("SynapsePool: " + std::to_string(synapsePool->size()) + 
                    " synapses, " + std::to_string(synapsePool->allocatedCount()) + 
                    " allocated, " + std::to_string(synapsePool->memoryUsage()) + " bytes");
    }
}

Neuron* Brain::Impl::legacyCreateNeuron(NeuronId id) {
    auto neuron = std::make_unique<Neuron>(id);
    Neuron* ptr = neuron.get();
    neurons_.push_back(std::move(neuron));
    return ptr;
}

Synapse* Brain::Impl::legacyCreateSynapse(SynapseId id, NeuronId source, NeuronId destination) {
    auto synapse = std::make_unique<Synapse>(id, source, destination);
    Synapse* ptr = synapse.get();
    synapses_.push_back(std::move(synapse));
    return ptr;
}

// Brain implementation continues with existing methods...
// (The rest of Brain.cpp remains unchanged)

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
    NLM_LOG_INFO("MemoryPool integration: " + std::string(pImpl->useMemoryPool ? "ENABLED" : "DISABLED"));
    
    // Create regions using standard allocation (regions are lightweight)
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions - neurons will use MemoryPool if enabled
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
    
    // Log MemoryPool statistics
    pImpl->updateMemoryPoolStats();
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated with MemoryPool)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

// Additional Brain methods continue...
// (The rest of Brain.cpp implementation remains unchanged)

} // namespace nlm
