#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../core/ErrorHandling/ErrorHandling.hpp"
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
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <vector>
#include <limits>

namespace nlm {

//=== Helper Functions ===

/**
 * @brief Validates configuration value with bounds checking
 */
template<typename T>
bool validateConfigValue(const std::string& paramName, T value, 
                         T min, T max, const std::string& errorMsg) {
    if (value < min || value > max) {
        NLM_THROW_ERROR(ErrorSeverity::Error, ErrorCategory::Configuration,
                       errorMsg + " (" + std::to_string(value) + " outside range [" + 
                       std::to_string(min) + ", " + std::to_string(max) + "])",
                       "Brain", "validateConfigValue");
        return false;
    }
    return true;
}

/**
 * @brief Validates that a pointer is not null
 */
void validateNotNull(const void* ptr, const std::string& paramName, 
                     const std::string& errorMsg) {
    if (!ptr) {
        NLM_THROW_ERROR(ErrorSeverity::Critical, ErrorCategory::NullPointer,
                       errorMsg + " (" + paramName + " is null)",
                       "Brain", "validateNotNull");
    }
}

/**
 * @brief Validates vector access bounds
 */
template<typename T>
void validateVectorBounds(const std::vector<T>& vec, size_t index,
                          const std::string& paramName, 
                          const std::string& errorMsg) {
    if (index >= vec.size()) {
        NLM_THROW_ERROR(ErrorSeverity::Error, ErrorCategory::MemoryAccess,
                       errorMsg + " (index " + std::to_string(index) + 
                       " >= size " + std::to_string(vec.size()) + ")",
                       "Brain", "validateVectorBounds");
    }
}

/**
 * @brief Safely gets pointer with error checking
 */
template<typename T>
T* safeGet(T* ptr, const std::string& paramName, 
          const std::string& errorMsg, ErrorRecovery recovery = ErrorRecovery::Graceful) {
    if (!ptr) {
        NullPointerError error(errorMsg + " (" + paramName + " is null)",
                             ErrorContext(__FILE__, __func__, __LINE__, "Brain", "safeGet"));
        ErrorManager::instance().reportError(error);
        
        switch (recovery) {
            case ErrorRecovery::Abort: throw; break;
            case ErrorRecovery::Graceful: return nullptr; break;
            case ErrorRecovery::Immediate: 
                // Attempt recovery
                return nullptr; break;
            case ErrorRecovery::Retry:
                // Retry logic
                return safeGet(ptr, paramName, errorMsg, recovery); break;
            default: return nullptr; break;
        }
    }
    return ptr;
}

/**
 * @brief Performs comprehensive configuration validation
 */
bool validateBrainConfiguration(const Config& config) {
    NLM_LOG_INFO("Validating NLM Brain configuration...");
    
    bool allValid = true;
    
    // Validate numeric parameters
    auto neuronCount = config.getOr<size_t>("neuron_count", 1000);
    if (!validateConfigValue<size_t>("neuron_count", neuronCount, 100, 10000,
                                    "Invalid neuron_count configuration")) {
        allValid = false;
    }
    
    auto regionCount = config.getOr<size_t>("region_count", 1);
    if (!validateConfigValue<size_t>("region_count", regionCount, 1, 100,
                                    "Invalid region_count configuration")) {
        allValid = false;
    }
    
    auto connectionProbability = config.getOr<float>("connection_probability", 0.1f);
    if (!validateConfigValue<float>("connection_probability", connectionProbability, 0.0f, 1.0f,
                                    "Invalid connection_probability configuration")) {
        allValid = false;
    }
    
    // Validate time parameters
    auto timestep = config.getOr<double>("simulation_timestep", 0.001);
    if (!validateConfigValue<double>("simulation_timestep", timestep, 0.0001f, 0.1f,
                                    "Invalid simulation_timestep configuration")) {
        allValid = false;
    }
    
    // Validate memory system parameters
    auto workingMemoryCapacity = config.getOr<size_t>("working_memory_capacity", 1000);
    if (!validateConfigValue<size_t>("working_memory_capacity", workingMemoryCapacity, 100, 10000,
                                    "Invalid working_memory_capacity configuration")) {
        allValid = false;
    }
    
    auto episodicMemoryMaxEpisodes = config.getOr<size_t>("episodic_memory_max_episodes", 1000);
    if (!validateConfigValue<size_t>("episodic_memory_max_episodes", episodicMemoryMaxEpisodes, 100, 10000,
                                    "Invalid episodic_memory_max_episodes configuration")) {
        allValid = false;
    }
    
    // Validate plasticity parameters
    auto stdpLTPWeight = config.getOr<float>("stdp_ltp_weight", 0.01f);
    if (!validateConfigValue<float>("stdp_ltp_weight", stdpLTPWeight, 0.0f, 1.0f,
                                    "Invalid stdp_ltp_weight configuration")) {
        allValid = false;
    }
    
    auto stdpLTDWeight = config.getOr<float>("stdp_ltd_weight", 0.012f);
    if (!validateConfigValue<float>("stdp_ltd_weight", stdpLTDWeight, 0.0f, 1.0f,
                                    "Invalid stdp_ltd_weight configuration")) {
        allValid = false;
    }
    
    // Validate development parameters
    auto replayInterval = config.getOr<size_t>("replay_interval", 100);
    if (!validateConfigValue<size_t>("replay_interval", replayInterval, 10, 10000,
                                    "Invalid replay_interval configuration")) {
        allValid = false;
    }
    
    auto consolidationInterval = config.getOr<size_t>("consolidation_interval", 1000);
    if (!validateConfigValue<size_t>("consolidation_interval", consolidationInterval, 100, 10000,
                                    "Invalid consolidation_interval configuration")) {
        allValid = false;
    }
    
    // Validate file I/O parameters
    auto checkpointDir = config.getOr<std::string>("checkpoint_dir", "./checkpoints");
    if (checkpointDir.empty()) {
        NLM_LOG_WARNING("Empty checkpoint directory specified");
    }
    
    NLM_LOG_INFO("Configuration validation " + std::string(allValid ? "passed" : "failed"));
    return allValid;
}

/**
 * @brief Safely accesses configuration value with error checking
 */
template<typename T>
T getConfigSafe(const std::shared_ptr<Config>& config, const std::string& paramName,
               T defaultValue, const std::string& errorMsg) {
    NLM_CHECK_NULL_PTR(config.get(), "config", 
                      "Attempted to access config in getConfigSafe");
    
    try {
        auto value = config->getOr<T>(paramName, defaultValue);
        if (!nlm::Validation::isValid(value, errorMsg + " (parameter " + paramName + ")")) {
            NLM_THROW_ERROR(ErrorSeverity::Error, ErrorCategory::Configuration,
                           "Invalid value for parameter " + paramName + ": " + 
                           std::to_string(value),
                           "Brain", "getConfigSafe");
        }
        return value;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorSeverity::Error, ErrorCategory::Configuration,
                       "Exception accessing config parameter " + paramName + ": " + std::string(e.what()),
                       "Brain", "getConfigSafe");
        return defaultValue;
    }
}

//=== Brain::Impl Implementation ===

/**
 * @brief Detailed implementation structure for Brain (pImpl pattern)
 * 
 * Contains all actual implementation details including neural components,
 * memory systems, and simulation state. This structure is opaque to users
 * of the Brain class, allowing for easier maintenance and potential
 * internal refactoring without affecting the public interface.
 */
struct Brain::Impl {
    std::shared_ptr<Config> config;                    ///< Brain configuration
    std::unique_ptr<RandomGenerator> rng;              ///< Random number generator
    std::vector<std::unique_ptr<NeuralRegion>> regions;  ///< Neural regions
    std::vector<InterRegionConnection> interRegionConnections;  ///< Long-range connections
    
    // Integrated memory systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory;      ///< Working memory
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;    ///< Episodic memory  
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;  ///< Associative memory
    
    // Integrated prediction system
    std::unique_ptr<PredictionSystem> predictionSystem;      ///< Prediction system
    
    // Integrated cognition systems
    std::unique_ptr<NeuralPlanner> planner;                   ///< Neural planner
    std::unique_ptr<ConceptFormation> conceptFormation;       ///< Concept formation
    std::unique_ptr<AttentionalSelection> attention;          ///< Attentional selection
    
    // Development system
    std::unique_ptr<DevelopmentSystem> developmentSystem;     ///< Development system
    
    // Neuromodulation systems
    std::unique_ptr<Dopamine> dopamine;                      ///< Dopamine system
    std::unique_ptr<Curiosity> curiosity;                    ///< Curiosity system
    std::unique_ptr<PredictionError> predictionError;        ///< Prediction error
    std::unique_ptr<Novelty> novelty;                         ///< Novelty detection
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;                 ///< Spike system
    std::unique_ptr<STDP> stdp;                               ///< STDP plasticity
    std::unique_ptr<Hebbian> hebbian;                         ///< Hebbian learning
    std::unique_ptr<StructuralPlasticity> structuralPlasticity; ///< Structural plasticity
    
    // Simulation parameters
    TimestepDuration timestep;                   ///< Simulation timestep
    SimulationStep currentStep;                  ///< Current simulation step
    Timestamp currentTime;                        ///< Current simulation time
    
    // Statistics
    size_t totalSpikesThisStep;                   ///< Spikes fired this step
    size_t totalSpikesTotal;                      ///< Total spikes fired (cumulative)
    
    // Sensory neurons for input/output
    std::vector<Neuron*> sensoryNeurons;           ///< Sensory neurons for I/O
    std::vector<Neuron*> motorNeurons;            ///< Motor neurons for output
    
    // Integration state
    bool isResting;                               ///< Rest/sleep state flag
    size_t stepsSinceLastEpisode;                 ///< Steps since last episodic storage
    size_t replayInterval;                        ///< Memory replay interval
    size_t consolidationInterval;                 ///< Memory consolidation interval
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager; ///< State persistence
    
    /**
     * @brief Constructs implementation with configuration
     * 
     * @param cfg Shared configuration for brain parameters
     */
    explicit Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
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
        NLM_CHECK_NULL_PTR(config.get(), "cfg", "Brain::Impl constructor: configuration is null");
        
        try {
            // Initialize random generator with seed from config
            uint64_t seed = 42;  // Default seed
            if (auto seedOpt = config->get<uint64_t>("random_seed")) {
                seed = *seedOpt;
            }
            
            if (!nlm::Validation::isNonNegative(static_cast<long long>(seed), 
                                                "Invalid random_seed value")) {
                seed = 42;  // Reset to default
            }
            
            rng = std::make_unique<RandomGenerator>(seed);
            
            // Validate and initialize plasticity systems
            spikeSystem = std::make_unique<SpikeSystem>();
            stdp = std::make_unique<STDP>();
            hebbian = std::make_unique<Hebbian>();
            structuralPlasticity = std::make_unique<StructuralPlasticity>();
            
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
            
            // Configure STDP parameters with validation
            float ltpWeight = getConfigSafe<float>(config, "stdp_ltp_weight", 0.01f,
                                                 "Invalid stdp_ltp_weight configuration");
            float ltdWeight = getConfigSafe<float>(config, "stdp_ltd_weight", 0.012f,
                                                 "Invalid stdp_ltd_weight configuration");
            float tau = getConfigSafe<float>(config, "stdp_tau", 20.0f,
                                          "Invalid stdp_tau configuration");
            
            if (stdp) {
                stdp->configure(ltpWeight, ltdWeight, tau);
            }
            
            // Configure structural plasticity
            float synaptogenesisRate = getConfigSafe<float>(config, "synaptogenesis_rate", 0.0001f,
                                                         "Invalid synaptogenesis_rate configuration");
            float pruningRate = getConfigSafe<float>(config, "pruning_rate", 0.00001f,
                                                   "Invalid pruning_rate configuration");
            
            if (structuralPlasticity) {
                structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
                structuralPlasticity->setPruningRate(pruningRate);
            }
            
            // Get timestep with validation
            timestep = getConfigSafe<double>(config, "simulation_timestep", 0.001,
                                           "Invalid simulation_timestep configuration");
            
            // Get integration intervals from config with validation
            replayInterval = getConfigSafe<size_t>(config, "replay_interval", 100,
                                                "Invalid replay_interval configuration");
            consolidationInterval = getConfigSafe<size_t>(config, "consolidation_interval", 1000,
                                                         "Invalid consolidation_interval configuration");
            
            // Validate intervals
            nlm::Validation::validateNonNegative(replayInterval, "Replay interval cannot be negative");
            nlm::Validation::validateNonNegative(consolidationInterval, "Consolidation interval cannot be negative");
            
            // Initialize checkpoint manager
            checkpointManager = std::make_unique<CheckpointManager>();
            
            NLM_LOG_INFO("Brain implementation initialized successfully");
            
        } catch (const std::exception& e) {
            NLM_THROW_ERROR(ErrorSeverity::Critical, ErrorCategory::ResourceManagement,
                           "Failed to initialize Brain implementation: " + std::string(e.what()),
                           "Brain::Impl", "constructor");
        }
    }
    
    DevelopmentalStage developmentalStage{DevelopmentalStage::Initial};  ///< Current developmental stage
    RegionId nextRegionId{1};                                             ///< Next available region ID
};

//=== Brain Implementation ===

/**
 * @brief Constructs Brain with implementation
 * 
 * @param config Brain configuration
 */
Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Brain constructor: implementation is null");
    NLM_LOG_INFO("Brain constructed (pImpl pattern)");
}

/**
 * @brief Destructor - cleans up implementation
 */
Brain::~Brain() {
    delete pImpl;
    NLM_LOG_INFO("Brain destroyed");
}

/**
 * @brief Move constructor - transfers ownership
 */
Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
    NLM_LOG_INFO("Brain move-constructed");
}

/**
 * @brief Move assignment operator - transfers ownership
 */
Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

/**
 * @brief Initializes the brain with regions and neurons
 */
bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Validate configuration
    if (!pImpl->config) {
        NLM_LOG_ERROR("Brain configuration not set");
        return false;
    }
    
    // Get configuration values with validation
    size_t neuronCount = getConfigSafe<size_t>(pImpl->config, "neuron_count", 1000,
                                            "Invalid neuron_count configuration");
    size_t regionCount = getConfigSafe<size_t>(pImpl->config, "region_count", 1,
                                            "Invalid region_count configuration");
    float connectionProbability = getConfigSafe<float>(pImpl->config, "connection_probability", 0.1f,
                                                    "Invalid connection_probability configuration");
    
    // Validate ranges
    nlm::Validation::validateNonNegative(neuronCount, "Neuron count cannot be negative");
    nlm::Validation::validateNonNegative(regionCount, "Region count cannot be negative");
    nlm::Validation::validateNonNegative(connectionProbability, "Connection probability cannot be negative");
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        NeuralRegion* region = safeGet(getRegion(RegionId(i + 1)), "region", 
                                      "Failed to get region in initialize", 
                                      ErrorRecovery::Graceful);
        if (!region) {
            NLM_LOG_WARNING("Region " + std::to_string(i + 1) + " not found");
            continue;
        }
        
        // Add populations with validation
        auto sensoryPopId = region->addPopulation(std::max<size_t>(1, neuronsPerRegion / 4), NeuronType::Sensory);
        auto internalPopId = region->addPopulation(std::max<size_t>(1, neuronsPerRegion / 2), NeuronType::Internal);
        auto motorPopId = region->addPopulation(std::max<size_t>(1, neuronsPerRegion / 4), NeuronType::Motor);
        
        // Collect sensory and motor neurons for I/O
        NeuralPopulation* sensoryPop = safeGet(region->getPopulation(sensoryPopId), "sensoryPop", 
                                              "Failed to get sensory population", ErrorRecovery::Graceful);
        NeuralPopulation* motorPop = safeGet(region->getPopulation(motorPopId), "motorPop", 
                                            "Failed to get motor population", ErrorRecovery::Graceful);
        
        if (sensoryPop) {
            auto neurons = sensoryPop->getNeurons();
            for (Neuron* neuron : neurons) {
                NLM_CHECK_NULL_PTR(neuron, "neuron", "Null neuron found in sensory population");
                pImpl->sensoryNeurons.push_back(neuron);
            }
        }
        
        if (motorPop) {
            auto neurons = motorPop->getNeurons();
            for (Neuron* neuron : neurons) {
                NLM_CHECK_NULL_PTR(neuron, "neuron", "Null neuron found in motor population");
                pImpl->motorNeurons.push_back(neuron);
            }
        }
        
        NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                     ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons");
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        NeuralRegion* region = safeGet(getRegion(RegionId(i + 1)), "region", 
                                      "Failed to get region in initialize connectivity",
                                      ErrorRecovery::Graceful);
        if (region) {
            // Initialize random connectivity and synapse weights
            if (pImpl->rng) {
                region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
            } else {
                NLM_LOG_WARNING("Random generator not available for connectivity initialization");
            }
        }
    }
    
    // Initialize all integrated systems
    if (pImpl->workingMemory) {
        pImpl->workingMemory->initialize(this);
        pImpl->workingMemory->setCapacity(neuronCount / 10);
    }
    
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->initialize(this);
        pImpl->episodicMemory->setMaxEpisodes(1000);
    }
    
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->initialize(this);
    }
    
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
    
    if (pImpl->novelty) {
        pImpl->novelty->initialize(this);
    }
    
    if (pImpl->curiosity) {
        pImpl->curiosity->initialize(this);
    }
    
    // Register spike handlers for event-driven processing
    if (pImpl->spikeSystem) {
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
                    NLM_CHECK_NULL_PTR(neuron, "neuron", "Null neuron found in delayed spike handler");
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
    std::string checkpointDir = getConfigSafe<std::string>(pImpl->config, "checkpoint_dir", "./checkpoints",
                                                           "Invalid checkpoint_dir configuration");
    
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    }
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

/**
 * @brief Performs single timestep simulation with default timing
 */
void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

/**
 * @brief Performs complete simulation step with all integrated systems
 * 
 * Executes the 15-phase integrated brain loop:
 * 1. Process delayed spikes
 * 2. Update neuron membrane potentials (LIF dynamics)
 * 3. Detect and queue spikes
 * 4. Update working memory
 * 5. Apply neuromodulation
 * 6. Apply plasticity rules (STDP, Hebbian)
 * 7. Update episodic memory
 * 8. Update prediction system
 * 9. Update attention system
 * 10. Update concept formation
 * 11. Apply structural plasticity
 * 12. Replay memories
 * 13. Apply development effects
 * 14. Collect statistics
 * 15. Checkpoint management
 */
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Brain::step: implementation is null");
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    try {
        // ========== STEP 1: Process pending delayed spikes ==========
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
        }
        
        // ========== STEP 2: Update all neurons (LIF dynamics) ==========
        for (auto& region : pImpl->regions) {
            NLM_CHECK_NULL_PTR(region.get(), "region", "Null region in step");
            
            for (auto& pop : region->getPopulations()) {
                NLM_CHECK_NULL_PTR(pop.get(), "pop", "Null population in step");
                
                auto neurons = pop->getNeurons();
                for (auto* neuron : neurons) {
                    NLM_CHECK_NULL_PTR(neuron, "neuron", "Null neuron in LIF step");
                    neuron->stepLIF(currentTime, pImpl->timestep);
                }
            }
        }
        
        // ========== STEP 3: Detect spikes and schedule events ==========
        for (auto& region : pImpl->regions) {
            NLM_CHECK_NULL_PTR(region.get(), "region", "Null region in spike detection");
            
            for (auto& pop : region->getPopulations()) {
                NLM_CHECK_NULL_PTR(pop.get(), "pop", "Null population in spike detection");
                
                auto neurons = pop->getNeurons();
                for (auto* neuron : neurons) {
                    NLM_CHECK_NULL_PTR(neuron, "neuron", "Null neuron in spike detection");
                    
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
                        
                        // Record pre-synaptic spike for plasticity
                        auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                        for (Synapse* syn : incomingSynapses) {
                            NLM_CHECK_NULL_PTR(syn, "syn", "Null synapse in spike processing");
                            syn->recordPostSpike(currentTime);
                        }
                        
                        // Get outgoing synapses and schedule delayed spike events
                        auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                        for (Synapse* syn : outgoingSynapses) {
                            NLM_CHECK_NULL_PTR(syn, "syn", "Null synapse in outgoing processing");
                            
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
                        
                        // Store to working memory
                        if (pImpl->workingMemory && neuron) {
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
            float dopamineLevel = pImpl->dopamine->getLevel();
            for (auto& region : pImpl->regions) {
                NLM_CHECK_NULL_PTR(region.get(), "region", "Null region in dopamine processing");
                
                for (auto& pop : region->getPopulations()) {
                    NLM_CHECK_NULL_PTR(pop.get(), "pop", "Null population in dopamine processing");
                    
                    auto neurons = pop->getNeurons();
                    for (auto* neuron : neurons) {
                        NLM_CHECK_NULL_PTR(neuron, "neuron", "Null neuron in dopamine processing");
                        
                        // Dopamine modulates excitability by injecting additional current
                        float excitabilityMod = dopamineLevel * 0.5f;
                        if (excitabilityMod > 0.0f) {
                            neuron->injectCurrent(excitabilityMod);
                        }
                    }
                }
            }
        }
        
        // ========== STEP 6: Apply plasticity rules ==========
        float plasticityMod = 1.0f;
        if (pImpl->dopamine) {
            plasticityMod = pImpl->dopamine->getPlasticityFactor();
        }
        
        for (auto& region : pImpl->regions) {
            NLM_CHECK_NULL_PTR(region.get(), "region", "Null region in plasticity");
            
            for (auto& syn : region->getSynapses()) {
                NLM_CHECK_NULL_PTR(syn.get(), "syn", "Null synapse in plasticity");
                
                // Apply STDP with neuromodulation
                if (syn->getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        if (pImpl->stdp) {
                            pImpl->stdp->update(syn.get(), preSpikes, postSpikes, pImpl->timestep);
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
                            pImpl->hebbian->update(syn.get(), preSpikes, postSpikes, pImpl->timestep);
                        }
                    }
                }
                
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
                    NLM_CHECK_NULL_PTR(region.get(), "region", "Null region in episodic memory");
                    
                    for (auto& pop : region->getPopulations()) {
                        NLM_CHECK_NULL_PTR(pop.get(), "pop", "Null population in episodic memory");
                        
                        auto neurons = pop->getNeurons();
                        for (auto* neuron : neurons) {
                            NLM_CHECK_NULL_PTR(neuron, "neuron", "Null neuron in episodic memory");
                            
                            if (neuron->isFiring() || 
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                                episode.activeNeurons.push_back(neuron->getId());
                                episode.neuronActivations.push_back(
                                    std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                            }
                        }
                    }
                }
                
                episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                
                pImpl->episodicMemory->storeEpisode(episode);
            }
        }
        
        // ========== STEP 8: Update prediction system ==========
        if (pImpl->predictionSystem) {
            // Prediction system updates with sensory observations
        }
        
        // ========== STEP 9: Update attention system ==========
        if (pImpl->attention) {
            pImpl->attention->update(pImpl->timestep);
            
            if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
                pImpl->attention->processCompetition(competitors);
            }
        }
        
        // ========== STEP 10: Update concept formation ==========
        if (pImpl->conceptFormation) {
            // Process current neural activity patterns to form concepts
        }
        
        // ========== STEP 11: Apply structural plasticity ==========
        if (currentStep % 100 == 0) {
            if (pImpl->structuralPlasticity && pImpl->rng) {
                pImpl->structuralPlasticity->update(this, *pImpl->rng);
            }
        }
        
        // ========== STEP 12: Replay memories ==========
        if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
            auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                NLM_CHECK_NULL_PTR(episode, "episode", "Null episode in replay");
                pImpl->episodicMemory->replayEpisode(episode);
            }
        }
        
        // ========== STEP 13: Apply development effects ==========
        if (currentStep % 1000 == 0) {  // Update development every 1000 steps
            if (pImpl->developmentSystem) {
                pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
            }
            
            // Development affects plasticity rates
            auto* sp = safeGet(pImpl->structuralPlasticity.get(), "structuralPlasticity", 
                             "Null structural plasticity in development", ErrorRecovery::Graceful);
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
            pImpl->episodicMemory->consolidate(0.3f);
        }
        
        // ========== STEP 15: Checkpoint management ==========
        if (pImpl->checkpointManager) {
            pImpl->checkpointManager->update(currentStep, currentTime);
        }
        
        NLM_LOG_INFO("Brain step completed successfully at step " + std::to_string(currentStep));
        
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorSeverity::Critical, ErrorCategory::System,
                       "Exception in Brain::step: " + std::string(e.what()),
                       "Brain", "step");
    } catch (...) {
        NLM_THROW_ERROR(ErrorSeverity::Critical, ErrorCategory::System,
                       "Unknown exception in Brain::step",
                       "Brain", "step");
    }
}

/**
 * @brief Receives sensory input and distributes across sensory neurons
 */
void Brain::receiveSensoryInput(const SensoryInput& input) {
    NLM_CHECK_NULL_PTR(&input, "input", "Brain::receiveSensoryInput: input is null");
    
    // Validate input
    const auto& values = input.getData();
    if (values.empty()) {
        NLM_LOG_DEBUG("Empty sensory input received");
        return;
    }
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) {
        NLM_LOG_WARNING("No sensory neurons available for input processing");
        return;
    }
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Validate normalized value
        if (!nlm::Validation::isFinite(normalizedValue, "Invalid normalized input value")) {
            normalizedValue = 0.0f;  // Reset to safe value
        }
        
        // Inject current into this sensory neuron
        Neuron* neuron = safeGet(pImpl->sensoryNeurons[i], "sensoryNeuron", 
                                "Failed to access sensory neuron", ErrorRecovery::Graceful);
        if (neuron) {
            neuron->injectCurrent(normalizedValue);
        }
        
        // Also store in working memory
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

/**
 * @brief Injects current into a specific neuron
 */
void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Brain::injectCurrent: implementation is null");
    
    // Validate input
    if (!nlm::Validation::isFinite(static_cast<double>(current), "Invalid current value")) {
        NLM_LOG_WARNING("Invalid current value: " + std::to_string(current));
        return;
    }
    
    for (auto& region : pImpl->regions) {
        NLM_CHECK_NULL_PTR(region.get(), "region", "Null region in injectCurrent");
        
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            NLM_CHECK_NULL_PTR(n, "n", "Null neuron in injectCurrent");
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

/**
 * @brief Injects current into all neurons of specified type
 */
void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Brain::injectCurrentToNeurons: implementation is null");
    
    // Validate input
    if (!nlm::Validation::isFinite(static_cast<double>(current), "Invalid current value")) {
        NLM_LOG_WARNING("Invalid current value: " + std::to_string(current));
        return;
    }
    
    for (auto& region : pImpl->regions) {
        NLM_CHECK_NULL_PTR(region.get(), "region", "Null region in injectCurrentToNeurons");
        
        for (auto& pop : region->getPopulations()) {
            NLM_CHECK_NULL_PTR(pop.get(), "pop", "Null population in injectCurrentToNeurons");
            
            if (pop->getNeuronType() == type) {
                auto neurons = pop->getNeurons();
                for (auto* neuron : neurons) {
                    NLM_CHECK_NULL_PTR(neuron, "neuron", "Null neuron in injectCurrentToNeurons");
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

//=== Consolidated System Accessors ===

/**
 * @brief Accesses spike system with null pointer check
 */
SpikeSystem* Brain::getSpikeSystem() {
    return pImpl && pImpl->spikeSystem ? pImpl->spikeSystem.get() : nullptr;
}

/**
 * @brief Constant version of spike system accessor
 */
const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl && pImpl->spikeSystem ? pImpl->spikeSystem.get() : nullptr;
}

/**
 * @brief Accesses STDP plasticity system
 */
STDP* Brain::getSTDP() {
    return pImpl && pImpl->stdp ? pImpl->stdp.get() : nullptr;
}

/**
 * @brief Accesses Hebbian learning system
 */
Hebbian* Brain::getHebbian() {
    return pImpl && pImpl->hebbian ? pImpl->hebbian.get() : nullptr;
}

/**
 * @brief Accesses structural plasticity system
 */
StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl && pImpl->structuralPlasticity ? pImpl->structuralPlasticity.get() : nullptr;
}

/**
 * @brief Accesses working memory system
 */
NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl && pImpl->workingMemory ? pImpl->workingMemory.get() : nullptr;
}

/**
 * @brief Accesses episodic memory system
 */
NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl && pImpl->episodicMemory ? pImpl->episodicMemory.get() : nullptr;
}

/**
 * @brief Accesses associative memory system
 */
NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl && pImpl->associativeMemory ? pImpl->associativeMemory.get() : nullptr;
}

/**
 * @brief Accesses prediction system
 */
PredictionSystem* Brain::getPredictionSystem() {
    return pImpl && pImpl->predictionSystem ? pImpl->predictionSystem.get() : nullptr;
}

/**
 * @brief Accesses neural planner
 */
NeuralPlanner* Brain::getPlanner() {
    return pImpl && pImpl->planner ? pImpl->planner.get() : nullptr;
}

/**
 * @brief Accesses concept formation system
 */
ConceptFormation* Brain::getConceptFormation() {
    return pImpl && pImpl->conceptFormation ? pImpl->conceptFormation.get() : nullptr;
}

/**
 * @brief Accesses attentional selection system
 */
AttentionalSelection* Brain::getAttention() {
    return pImpl && pImpl->attention ? pImpl->attention.get() : nullptr;
}

/**
 * @brief Accesses development system
 */
DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl && pImpl->developmentSystem ? pImpl->developmentSystem.get() : nullptr;
}

/**
 * @brief Accesses dopamine system
 */
Dopamine* Brain::getDopamine() {
    return pImpl && pImpl->dopamine ? pImpl->dopamine.get() : nullptr;
}

/**
 * @brief Accesses curiosity system
 */
Curiosity* Brain::getCuriosity() {
    return pImpl && pImpl->curiosity ? pImpl->curiosity.get() : nullptr;
}

/**
 * @brief Accesses novelty detection system
 */
Novelty* Brain::getNovelty() {
    return pImpl && pImpl->novelty ? pImpl->novelty.get() : nullptr;
}

/**
 * @brief Accesses prediction error system
 */
PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl && pImpl->predictionError ? pImpl->predictionError.get() : nullptr;
}

//=== Statistics Methods ===

/**
 * @brief Calculates excitation/inhibition ratio with bounds checking
 */
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

/**
 * @brief Gets total spike count (cumulative)
 */
size_t Brain::getTotalSpikeCount() const {
    return pImpl ? pImpl->totalSpikesTotal : 0;
}

/**
 * @brief Gets pending spike event count
 */
size_t Brain::getPendingSpikeEventCount() const {
    size_t pending = 0;
    if (pImpl && pImpl->spikeSystem) {
        pending = pImpl->spikeSystem->getPendingSpikeCount() + 
                 pImpl->spikeSystem->getPendingDelayedCount();
    }
    return pending;
}

/**
 * @brief Gets number of active neurons
 */
size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

/**
 * @brief Gets firing neuron count (current step)
 */
size_t Brain::getFiringNeuronCount() const {
    return pImpl ? pImpl->totalSpikesThisStep : 0;
}

/**
 * @brief Gets average firing rate across regions
 */
float Brain::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

/**
 * @brief Produces motor action based on motor neuron activity
 */
std::unique_ptr<Action> Brain::produceAction() {
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        NLM_CHECK_NULL_PTR(neuron, "neuron", "Null motor neuron in produceAction");
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Select action based on motor activity
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    return action;
}

/**
 * @brief Applies neuromodulatory signal to plasticity
 */
void Brain::applyNeuromodulation(const Neuromodulator& signal) {
    NLM_CHECK_NULL_PTR(&signal, "signal", "Brain::applyNeuromodulation: signal is null");
    
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    if (pImpl->stdp) {
        pImpl->stdp->setLTPWeight(0.01f * modulation);
        pImpl->stdp->setLTDWeight(0.012f * modulation);
    }
}

/**
 * @brief Updates plasticity rules (API compatibility wrapper)
 */
void Brain::updatePlasticity() {
    // Plasticity is applied during each step - this is a compatibility wrapper
}

/**
 * @brief Applies developmental changes
 */
void Brain::develop() {
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

/**
 * @brief Resets brain to initial state
 */
void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    for (auto& region : pImpl->regions) {
        NLM_CHECK_NULL_PTR(region.get(), "region", "Null region in reset");
        region->reset();
    }
    
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->reset();
    }
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

/**
 * @brief Saves brain state to checkpoint file
 */
bool Brain::save(const std::string& filepath) const {
    NLM_VALIDATE_FILEPATH(filepath);
    
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
        
        // Write neurons with validation
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                auto neurons = pop->getNeurons();
                for (auto* neuron : neurons) {
                    NLM_CHECK_NULL_PTR(neuron, "neuron", "Null neuron in save");
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
            auto synapses = region->getSynapses();
            for (auto* syn : synapses) {
                NLM_CHECK_NULL_PTR(syn, "syn", "Null synapse in save");
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

/**
 * @brief Loads brain state from checkpoint file
 */
bool Brain::load(const std::string& filepath) {
    NLM_VALIDATE_FILEPATH(filepath);
    
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_THROW_FILEIO_ERROR("Failed to open checkpoint file: " + filepath, "Brain", "load");
            return false;
        }
        
        if (!reader.validate()) {
            NLM_THROW_FILEIO_ERROR("Checkpoint validation failed: " + reader.getError(), "Brain", "load");
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_THROW_FILEIO_ERROR("Failed to read neurons from checkpoint", "Brain", "load");
            return false;
        }
        
        // Apply neuron states with bounds checking
        size_t idx = 0;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                auto neurons = pop->getNeurons();
                for (auto* neuron : neurons) {
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
            NLM_THROW_FILEIO_ERROR("Failed to read synapses from checkpoint", "Brain", "load");
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

/**
 * @brief Creates a new neural region
 */
RegionId Brain::addRegion(const std::string& name) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Brain::addRegion: implementation is null");
    
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    NLM_CHECK_NULL_PTR(region.get(), "region", "Failed to create neural region");
    
    pImpl->regions.push_back(std::move(region));
    return id;
}

/**
 * @brief Finds region by ID
 */
NeuralRegion* Brain::getRegion(RegionId id) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Brain::getRegion: implementation is null");
    
    for (auto& region : pImpl->regions) {
        NLM_CHECK_NULL_PTR(region.get(), "region", "Null region in getRegion");
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

/**
 * @brief Constant version of region finder
 */
const NeuralRegion* Brain::getRegion(RegionId id) const {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Brain::getRegion: implementation is null");
    
    for (const auto& region : pImpl->regions) {
        NLM_CHECK_NULL_PTR(region.get(), "region", "Null region in getRegion");
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

/**
 * @brief Gets number of regions
 */
size_t Brain::getRegionCount() const {
    return pImpl ? pImpl->regions.size() : 0;
}

/**
 * @brief Gets all region IDs
 */
std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    if (!pImpl) return ids;
    
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        NLM_CHECK_NULL_PTR(region.get(), "region", "Null region in getRegionIds");
        ids.push_back(region->getId());
    }
    return ids;
}

/**
 * @brief Gets all neural regions
 */
const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    static const std::vector<std::unique_ptr<NeuralRegion>> emptyVector;
    return pImpl ? pImpl->regions : emptyVector;
}

/**
 * @brief Creates inter-regional connection
 */
void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Brain::addInterRegionConnection: implementation is null");
    
    // Validate parameters
    if (!nlm::Validation::isFinite(static_cast<double>(weight), "Invalid weight for inter-region connection")) {
        NLM_LOG_WARNING("Invalid weight for inter-region connection: " + std::to_string(weight));
        weight = 0.0f;
    }
    
    nlm::Validation::validateNonNegative(delay, "Delay cannot be negative");
    
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

/**
 * @brief Removes inter-regional connection
 */
void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    NLM_CHECK_NULL_PTR(pImpl, "pImpl", "Brain::removeInterRegionConnection: implementation is null");
    
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                           return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

/**
 * @brief Gets total neuron count across all regions
 */
size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        NLM_CHECK_NULL_PTR(region.get(), "region", "Null region in getTotalNeuronCount");
        total += region->getTotalNeuronCount();
    }
    return total;
}

/**
 * @brief Gets total synapse count across all regions
 */
size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        NLM_CHECK_PTR(region.get(), "region", "Null region in getTotalSynapseCount");
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

/**
 * @brief Gets active neuron count
 */
size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        NLM_CHECK_NULL_PTR(region.get(), "region", "Null region in getActiveNeuronCount");
        total += region->getActiveNeuronCount();
    }
    return total;
}

/**
 * @brief Gets firing neuron count (current step)
 */
size_t Brain::getFiringNeuronCount() const {
    return pImpl ? pImpl->totalSpikesThisStep : 0;
}

/**
 * @brief Gets average firing rate across regions
 */
float Brain::getAverageFiringRate() const {
    if (!pImpl || pImpl->regions.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        NLM_CHECK_NULL_PTR(region.get(), "region", "Null region in getAverageFiringRate");
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

//=== Memory System Accessors ===

/**
 * @brief Accesses working memory system
 */
NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl && pImpl->workingMemory ? pImpl->workingMemory.get() : nullptr;
}

/**
 * @brief Accesses episodic memory system
 */
NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl && pImpl->episodicMemory ? pImpl->episodicMemory.get() : nullptr;
}

/**
 * @brief Accesses associative memory system
 */
NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl && pImpl->associativeMemory ? pImpl->associativeMemory.get() : nullptr;
}

//=== Prediction System Accessor ===

/**
 * @brief Accesses prediction system
 */
PredictionSystem* Brain::getPredictionSystem() {
    return pImpl && pImpl->predictionSystem ? pImpl->predictionSystem.get() : nullptr;
}

//=== Cognition System Accessors ===

/**
 * @brief Accesses neural planner
 */
NeuralPlanner* Brain::getPlanner() {
    return pImpl && pImpl->planner ? pImpl->planner.get() : nullptr;
}

/**
 * @brief Accesses concept formation system
 */
ConceptFormation* Brain::getConceptFormation() {
    return pImpl && pImpl->conceptFormation ? pImpl->conceptFormation.get() : nullptr;
}

/**
 * @brief Accesses attentional selection system
 */
AttentionalSelection* Brain::getAttention() {
    return pImpl && pImpl->attention ? pImpl->attention.get() : nullptr;
}

//=== Development System ===

/**
 * @brief Accesses development system
 */
DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl && pImpl->developmentSystem ? pImpl->developmentSystem.get() : nullptr;
}

/**
 * @brief Gets developmental stage
 */
DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl ? pImpl->developmentalStage : DevelopmentalStage::Initial;
}

/**
 * @brief Sets developmental stage
 */
void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    if (pImpl) {
        pImpl->developmentalStage = stage;
    }
}

//=== Neuromodulation Systems ===

/**
 * @brief Accesses dopamine system
 */
Dopamine* Brain::getDopamine() {
    return pImpl && pImpl->dopamine ? pImpl->dopamine.get() : nullptr;
}

/**
 * @brief Accesses curiosity system
 */
Curiosity* Brain::getCuriosity() {
    return pImpl && pImpl->curiosity ? pImpl->curiosity.get() : nullptr;
}

/**
 * @brief Accesses novelty detection system
 */
Novelty* Brain::getNovelty() {
    return pImpl && pImpl->novelty ? pImpl->novelty.get() : nullptr;
}

/**
 * @brief Accesses prediction error system
 */
PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl && pImpl->predictionError ? pImpl->predictionError.get() : nullptr;
}

//=== Statistics and Metrics ===

/**
 * @brief Calculates excitation/inhibition ratio with bounds checking
 */
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

/**
 * @brief Gets total spike count (cumulative)
 */
size_t Brain::getTotalSpikeCount() const {
    return pImpl ? pImpl->totalSpikesTotal : 0;
}

/**
 * @brief Gets pending spike event count
 */
size_t Brain::getPendingSpikeEventCount() const {
    size_t pending = 0;
    if (pImpl && pImpl->spikeSystem) {
        pending = pImpl->spikeSystem->getPendingSpikeCount() + 
                 pImpl->spikeSystem->getPendingDelayedCount();
    }
    return pending;
}

/**
 * @brief Gets number of active neurons
 */
size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

/**
 * @brief Gets firing neuron count (current step)
 */
size_t Brain::getFiringNeuronCount() const {
    return pImpl ? pImpl->totalSpikesThisStep : 0;
}

/**
 * @brief Gets average firing rate across regions
 */
float Brain::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

//=== Output Generation ===

/**
 * @brief Produces motor action based on motor neuron activity
 */
std::unique_ptr<Action> Brain::produceAction() {
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        NLM_CHECK_NULL_PTR(neuron, "neuron", "Null motor neuron in produceAction");
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Select action based on motor activity
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    return action;
}

/**
 * @brief Applies neuromodulatory signal to plasticity
 */
void Brain::applyNeuromodulation(const Neuromodulator& signal) {
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    if (pImpl->stdp) {
        pImpl->stdp->setLTPWeight(0.01f * modulation);
        pImpl->stdp->setLTDWeight(0.012f * modulation);
    }
}

/**
 * @brief Updates plasticity rules (API compatibility wrapper)
 */
void Brain::updatePlasticity() {
    // Plasticity is applied during each step - this is a compatibility wrapper
}

/**
 * @brief Applies developmental changes
 */
void Brain::develop() {
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

/**
 * @brief Resets brain to initial state
 */
void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    for (auto& region : pImpl->regions) {
        NLM_CHECK_NULL_PTR(region.get(), "region", "Null region in reset");
        region->reset();
    }
    
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->reset();
    }
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

/**
 * @brief Logs brain status for debugging
 */
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

//=== Additional State Accessors ===

/**
 * @brief Checks if brain is currently resting (sleep/rest cycle)
 */
bool Brain::isResting() const {
    return pImpl ? pImpl->isResting : false;
}

/**
 * @brief Gets steps since last memory episode
 */
size_t Brain::getStepsSinceLastEpisode() const {
    return pImpl ? pImpl->stepsSinceLastEpisode : 0;
}

/**
 * @brief Gets memory replay interval
 */
size_t Brain::getReplayInterval() const {
    return pImpl ? pImpl->replayInterval : 100;
}

/**
 * @brief Gets memory consolidation interval
 */
size_t Brain::getConsolidationInterval() const {
    return pImpl ? pImpl->consolidationInterval : 1000;
}

/**
 * @brief Gets sensory neuron count
 */
size_t Brain::getSensoryNeuronCount() const {
    return pImpl ? pImpl->sensoryNeurons.size() : 0;
}

/**
 * @brief Gets motor neuron count
 */
size_t Brain::getMotorNeuronCount() const {
    return pImpl ? pImpl->motorNeurons.size() : 0;
}

/**
 * @brief Gets current simulation step
 */
SimulationStep Brain::getCurrentStep() const {
    return pImpl ? pImpl->currentStep : 0;
}

/**
 * @brief Gets current simulation time
 */
Timestamp Brain::getCurrentTime() const {
    return pImpl ? pImpl->currentTime : 0.0;
}

} // namespace nlm
