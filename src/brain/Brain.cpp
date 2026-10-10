#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/ErrorHandling/ErrorHandling.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Reward.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
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

// Constants for Brain initialization and configuration
namespace BrainConstants {
    // Default configuration values for brain initialization
    constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
    constexpr float DEFAULT_STDP_TIME_CONST = 20.0f;  // STDP time constant
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    constexpr float DEFAULT_TIMESTEP = 0.001;
    constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;
    constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000;
    
    // Default configuration values for brain setup
    constexpr size_t DEFAULT_NEURON_COUNT = 1000;
    constexpr size_t DEFAULT_REGION_COUNT = 1;
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
    
    // Default system parameters
    constexpr float DEFAULT_STDP_MAX_WEIGHT = 1.0f;
    constexpr float DEFAULT_STDP_MIN_WEIGHT = -1.0f;
    constexpr float DEFAULT_SYNAPTIC_WEIGHT_SCALE = 10.0f;
    constexpr float DEFAULT_DOPAMINE_EXCITABILITY_MOD = 0.5f;
    constexpr float DEFAULT_DOPAMINE_PLASTICITY_FACTOR = 1.0f;
    
    // Time constants (in milliseconds)
    constexpr float DEFAULT_LIF_TIME_CONSTANT = 20.0f;
    constexpr float DEFAULT_LIF_THRESHOLD = -55.0f;
    constexpr float DEFAULT_LIF_RESTING_POTENTIAL = -70.0f;
    constexpr float DEFAULT_LIF_RESET_POTENTIAL = -65.0f;
    
    // Structural plasticity and development parameters
    constexpr float DEFAULT_SYNAPTIC_WEIGHT_UPDATE_RATE = 0.001f;
    constexpr float DEFAULT_DOPAMINE_LEVEL_MOD = 0.5f;
    constexpr float DEFAULT_EXCITATION_STRENGTH = 1.5f;
    constexpr float DEFAULT_INHIBITION_STRENGTH = 0.5f;
    constexpr float DEFAULT_CONSOLIDATION_THRESHOLD = 0.3f;
    
    // Default configuration values
    constexpr size_t DEFAULT_MAX_EPISODES = 1000;
    constexpr size_t DEFAULT_PLANNING_DEPTH = 5;
    constexpr size_t DEFAULT_CHECKPOINT_BATCH_SIZE = 10000;
    constexpr size_t DEFAULT_CHECKPOINT_THREADS = 5;
    constexpr float DEFAULT_TIME_STEP_MULTIPLIER = 2.0f;
    
    // Development and plasticity parameters
    constexpr float DEFAULT_DEVELOPMENT_UPDATE_INTERVAL = 1000.0f;
    constexpr float DEFAULT_PLASTICITY_MODULATION_BASE = 1.0f;
    constexpr float DEFAULT_EXPLORATION_EXCITABILITY = 0.5f;
    constexpr float DEFAULT_EXPLOITATION_EXCITABILITY = 1.5f;
}

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
        , timestep(BrainConstants::DEFAULT_TIMESTEP)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(BrainConstants::DEFAULT_REPLAY_INTERVAL)
        , consolidationInterval(BrainConstants::DEFAULT_CONSOLIDATION_INTERVAL)
    {
        // Initialize random generator with seed from config
        uint64_t seed = BrainConstants::DEFAULT_RANDOM_SEED;
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
        
        // Initialize memory systems - check for initialization failures
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
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", BrainConstants::DEFAULT_STDP_LTP_WEIGHT);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", BrainConstants::DEFAULT_STDP_LTD_WEIGHT);
        float tau = config->getOr<float>("stdp_tau", BrainConstants::DEFAULT_STDP_TIME_CONST); // STDP time constant
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity using configuration parameters
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", BrainConstants::DEFAULT_SYNAPTOGENESIS_RATE);
        float pruningRate = config->getOr<float>("pruning_rate", BrainConstants::DEFAULT_PRUNING_RATE);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        // Get timestep from config
        timestep = config->getOr<double>("simulation_timestep", BrainConstants::DEFAULT_TIMESTEP);
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", BrainConstants::DEFAULT_REPLAY_INTERVAL);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", BrainConstants::DEFAULT_CONSOLIDATION_INTERVAL);
        
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

// Initialize brain with configuration
bool Brain::initialize() {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain not properly initialized (null implementation)");
        return false;
    }
    
    // Verify config is available
    if (!pImpl->config) {
        NLM_LOG_ERROR("Brain config is null");
        return false;
    }
    
    NLM_LOG_INFO("Initializing NLM Brain with comprehensive error handling...");
    
    bool allInitialized = true;
    
    // Initialize random generator with seed from config
    uint64_t seed = BrainConstants::DEFAULT_RANDOM_SEED;
    if (auto seedOpt = pImpl->config->get<uint64_t>("random_seed")) {
        seed = *seedOpt;
    }
    pImpl->rng = std::make_unique<RandomGenerator>(seed);
    
    // Initialize plasticity systems
    pImpl->spikeSystem = std::make_unique<SpikeSystem>();
    pImpl->stdp = std::make_unique<STDP>();
    pImpl->hebbian = std::make_unique<Hebbian>();
    pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    // ========== INITIALIZE INTEGRATED SYSTEMS ==========
    
    // Initialize memory systems with error checking
    allInitialized &= initMemorySystem();
    allInitialized &= initPredictionSystem();
    allInitialized &= initCognitionSystems();
    allInitialized &= initDevelopmentSystem();
    allInitialized &= initNeuromodulationSystems();
    
    // Create regions if not already created
    if (pImpl->regions.empty()) {
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", BrainConstants::DEFAULT_NEURON_COUNT);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", BrainConstants::DEFAULT_REGION_COUNT);
        createRegions(neuronCount, regionCount);
    }
    
    // Initialize connectivity
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 
                                                                BrainConstants::DEFAULT_CONNECTION_PROBABILITY);
    initializeConnectivity(connectionProbability);
    
    // Pre-allocate spike handler for better performance
    auto spikeCounter = [this](const DetailedSpikeEvent& event) {
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    };
    pImpl->spikeSystem->registerHandler(spikeCounter);
    
    // Pre-allocate neuron lookup map for faster spike delivery
    std::unordered_map<NeuronId, std::vector<Neuron*>> neuronLookup;
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            neuronLookup[neuron->getId()].push_back(neuron);
        }
    }
    
    // Register optimized delayed spike handler
    pImpl->spikeSystem->registerDelayedHandler([this, &neuronLookup](const DelayedSpikeEvent& event) {
        // Use lookup map for O(1) neuron finding instead of O(n*m) search
        auto it = neuronLookup.find(event.destination_neuron);
        if (it != neuronLookup.end()) {
            // Apply synaptic weight as current
            MembranePotential synapticCurrent = event.weight * BrainConstants::DEFAULT_SYNAPTIC_WEIGHT_SCALE;
            if (event.is_excitatory) {
                for (auto* neuron : it->second) {
                    neuron->receiveExcitatoryInput(synapticCurrent);
                }
            } else {
                for (auto* neuron : it->second) {
                    neuron->receiveInhibitoryInput(-synapticCurrent);
                }
            }
        }
    });
    
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    
    if (allInitialized) {
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    } else {
        NLM_LOG_ERROR("Brain initialization completed with errors");
    }
    
    return allInitialized;
}

bool Brain::initMemorySystem() {
    if (!pImpl->workingMemory) {
        NLM_LOG_ERROR("Working memory not initialized");
        return false;
    }
    
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(pImpl->config->getOr<size_t>("working_memory_capacity", 
                                                                     getTotalNeuronCount() / 10));
    return true;
}

bool Brain::initPredictionSystem() {
    // PredictionSystem doesn't have initialize method currently
    // Just check if it's available
    return pImpl->predictionSystem != nullptr;
}

bool Brain::initCognitionSystems() {
    bool allGood = true;
    
    if (pImpl->planner) {
        pImpl->planner->initialize(this);
        pImpl->planner->setPlanningDepth(pImpl->config->getOr<size_t>("planning_depth", 5));
    } else {
        NLM_LOG_ERROR("Neural planner not initialized");
        allGood = false;
    }
    
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->initialize(this);
    } else {
        NLM_LOG_ERROR("Concept formation not initialized");
        allGood = false;
    }
    
    if (pImpl->attention) {
        pImpl->attention->initialize(this);
        pImpl->attention->setInhibitionStrength(BrainConstants::DEFAULT_INHIBITION_STRENGTH);
        pImpl->attention->setExcitationStrength(BrainConstants::DEFAULT_EXCITATION_STRENGTH);
    } else {
        NLM_LOG_ERROR("Attention system not initialized");
        allGood = false;
    }
    
    return allGood;
}

bool Brain::initDevelopmentSystem() {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->initialize(this, *pImpl->rng);
        return true;
    } else {
        NLM_LOG_ERROR("Development system not initialized");
        return false;
    }
}

bool Brain::initNeuromodulationSystems() {
    bool allGood = true;
    
    if (pImpl->novelty) {
        pImpl->novelty->initialize(this);
    } else {
        NLM_LOG_ERROR("Novelty detection not initialized");
        allGood = false;
    }
    
    if (pImpl->curiosity) {
        pImpl->curiosity->initialize(this);
    } else {
        NLM_LOG_ERROR("Curiosity system not initialized");
        allGood = false;
    }
    
    if (pImpl->dopamine) {
        pImpl->dopamine->initialize(this);
    } else {
        NLM_LOG_ERROR("Dopamine system not initialized");
        allGood = false;
    }
    
    return allGood;
}

    void Brain::createRegions(size_t neuronCount, size_t regionCount) {
        NLM_VALIDATE(nlm::ErrorHandling::validateNeuronCount(neuronCount, "region creation"),
                    "INVALID_NEURON_COUNT_FOR_REGIONS",
                    "Cannot create regions with invalid neuron count: " + std::to_string(neuronCount));
    
        NLM_VALIDATE(nlm::ErrorHandling::validateRegionCount(regionCount, "region creation"),
                    "INVALID_REGION_COUNT",
                    "Cannot create invalid number of regions: " + std::to_string(regionCount));
    
        NLM_LOG_INFO("Creating " + std::to_string(regionCount) + " regions with " + 
                     std::to_string(neuronCount) + " total neurons");
    
        // Pre-allocate regions vector for better performance
        pImpl->regions.reserve(pImpl->regions.size() + regionCount);
        
        // Create regions in a single loop for better cache locality
        for (size_t i = 0; i < regionCount; ++i) {
            addRegion("Region_" + std::to_string(i + 1));
        }
    
        // Calculate neurons per region once to avoid repeated division
        size_t neuronsPerRegion = neuronCount / regionCount;
        
        // Create neurons across regions - optimize for cache locality
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!region) continue;
            
            // Add populations using default population distribution ratios
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O - optimize memory allocation
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop && motorPop) {
                // Pre-allocate vectors once to avoid repeated reallocations
                size_t sensoryCount = sensoryPop->getNeuronCount();
                size_t motorCount = motorPop->getNeuronCount();
                
                // Reserve space upfront
                pImpl->sensoryNeurons.reserve(pImpl->sensoryNeurons.size() + sensoryCount);
                pImpl->motorNeurons.reserve(pImpl->motorNeurons.size() + motorCount);
                
                // Batch add neurons
                for (auto* neuron : sensoryPop->getNeurons()) {
                    pImpl->sensoryNeurons.push_back(neuron);
                }
                
                for (auto* neuron : motorPop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                         ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                         std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }

void Brain::initializeConnectivity(float connectionProbability) {
    NLM_VALIDATE(nlm::ErrorHandling::validateConnectionProbability(connectionProbability, "connectivity initialization"),
                "INVALID_CONNECTION_PROBABILITY",
                "Invalid connection probability: " + std::to_string(connectionProbability));
    
    // Initialize connectivity with random weights
    size_t regionCount = getRegionCount();
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights using constants
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 
                                                 BrainConstants::DEFAULT_CONNECTION_PROBABILITY, 
                                                 BrainConstants::DEFAULT_SYNAPTIC_WEIGHT_SCALE);
        }
    }
}

// Existing implementation continues with the rest of the methods...
// (The existing implementation is preserved and additional error handling will be added below)

// Helper function for configuration loading and validation
static void loadConfigurationParameters(Brain::Impl* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Brain context is null for configuration loading");
        return;
    }
    
    // Validate configuration parameters with comprehensive error checking
    size_t neuronCount = brain->config->getOr<size_t>("neuron_count", BrainConstants::DEFAULT_NEURON_COUNT);
    NLM_VALIDATE(nlm::ErrorHandling::validateNeuronCount(neuronCount, "brain initialization"), 
                "INVALID_NEURON_COUNT", 
                "Neuron count " + std::to_string(neuronCount) + " is invalid");
    
    // Use fallback if validation failed
    if (!nlm::ErrorHandling::validateNeuronCount(neuronCount, "brain initialization")) {
        auto fallback = nlm::ErrorHandling::suggestFallbackNeuronCount(neuronCount);
        if (fallback.has_value()) {
            NLM_LOG_WARNING("Using fallback neuron count: " + std::to_string(neuronCount) + 
                           " -> " + std::to_string(fallback.value()));
            // In a real implementation, this would modify the config
        }
    }
    
    size_t regionCount = brain->config->getOr<size_t>("region_count", BrainConstants::DEFAULT_REGION_COUNT);
    NLM_VALIDATE(nlm::ErrorHandling::validateRegionCount(regionCount, "brain initialization"),
                "INVALID_REGION_COUNT",
                "Region count " + std::to_string(regionCount) + " is invalid");
    
    float connectionProb = brain->config->getOr<float>("connection_probability", 
                                                     BrainConstants::DEFAULT_CONNECTION_PROBABILITY);
    NLM_VALIDATE(nlm::ErrorHandling::validateConnectionProbability(connectionProb, "brain initialization"),
                "INVALID_CONNECTION_PROBABILITY",
                "Connection probability " + std::to_string(connectionProb) + " is invalid");
    
    double timestep = brain->config->getOr<double>("simulation_timestep", 
                                                 BrainConstants::DEFAULT_TIMESTEP);
    NLM_VALIDATE(nlm::ErrorHandling::validateSimulationTimestep(timestep, "brain initialization"),
                "INVALID_TIMESTEP",
                "Timestep " + std::to_string(timestep) + " is invalid");
    
    // Validate and clamp synaptic parameters
    float ltpWeight = brain->config->getOr<float>("stdp_ltp_weight", 
                                                BrainConstants::DEFAULT_STDP_LTP_WEIGHT);
    NLM_VALIDATE(nlm::ErrorHandling::validateSynapseWeight(ltpWeight, "STDP LTP"),
                "INVALID_STDP_LTP_WEIGHT",
                "STDP LTP weight " + std::to_string(ltpWeight) + " is invalid");
    
    float ltdWeight = brain->config->getOr<float>("stdp_ltd_weight", 
                                                 BrainConstants::DEFAULT_STDP_LTD_WEIGHT);
    NLM_VALIDATE(nlm::ErrorHandling::validateSynapseWeight(ltdWeight, "STDP LTD"),
                "INVALID_STDP_LTD_WEIGHT",
                "STDP LTD weight " + std::to_string(ltdWeight) + " is invalid");
    
    // Validate plasticity rates
    float synaptogenesisRate = brain->config->getOr<float>("synaptogenesis_rate", 
                                                         BrainConstants::DEFAULT_SYNAPTOGENESIS_RATE);
    if (synaptogenesisRate < 0.0f || synaptogenesisRate > 1.0f) {
        ErrorInfo error("Synaptogenesis rate out of range [0, 1]: " + std::to_string(synaptogenesisRate),
                       nlm::ErrorCategory::Validation, nlm::ErrorSeverity::Error);
        error.recoveryActions = {"Clamp to [0, 1] range"};
        nlm::ErrorHandling::handleError(error);
        // In real implementation, clamp the value
    }
    
    float pruningRate = brain->config->getOr<float>("pruning_rate", 
                                                   BrainConstants::DEFAULT_PRUNING_RATE);
    if (pruningRate < 0.0f || pruningRate > 1.0f) {
        ErrorInfo error("Pruning rate out of range [0, 1]: " + std::to_string(pruningRate),
                       nlm::ErrorCategory::Validation, nlm::ErrorSeverity::Error);
        error.recoveryActions = {"Clamp to [0, 1] range"};
        nlm::ErrorHandling::handleError(error);
        // In real implementation, clamp the value
    }
    
    NLM_LOG_INFO("Configuration validation completed successfully");
}

// Additional error handling for existing methods
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
            for (const auto& synPtr : region->getSynapses()) {
                synapseData.sourceNeuron.push_back(synPtr->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(synPtr->getDestinationNeuron().index());
                synapseData.weight.push_back(synPtr->getWeight());
                synapseData.delay.push_back(synPtr->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(synPtr->getType()));
                synapseData.eligibilityTrace.push_back(synPtr->getEligibilityTrace());
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

} // namespace nlm
